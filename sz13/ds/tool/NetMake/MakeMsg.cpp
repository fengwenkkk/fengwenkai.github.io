#include "g_include.h"
#include "MsgDialog.h"
#include "GCommon.h"
#include "MDIForm.h"
#include "MakeUtil.h"
#include "InnerMsg.h"
#include "MakeMsg.h"
#include "main.h"

static SOCKET   g_stUDPSocket = INVALID_SOCKET;
static UINT8    g_aucMsgBuf[64*1024];

bool g_bUDPServerRunning = true;

extern TFormMain *FormMain;

SOCKET GetUDPSocket()
{
    return g_stUDPSocket;
}

bool SendPeer(IP_ADDR_T *pstPeerAddr, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    if (g_stUDPSocket == INVALID_SOCKET)
    {
        return false;
    }

    SOCKADDR_IN     stSockAddr;
    UINT8           aucPeerIP[4];
    UINT32          ulTotalLen = ulMsgLen+sizeof(MAKE_MSG_HDR_T);
    MAKE_MSG_HDR_T  *pstMsgHdr = (MAKE_MSG_HDR_T*)malloc(ulTotalLen);

    pstMsgHdr->usMsgID = usMsgID;
    pstMsgHdr->usMsgLen = ulMsgLen;
    if (ulMsgLen)
    {
        memcpy(pstMsgHdr+1, pvMsg, ulMsgLen);
    }

    gos_init_socket_addr(&stSockAddr, pstPeerAddr->aucIP, pstPeerAddr->usPort);

    bool bRet = gos_udp_send(g_stUDPSocket, pstMsgHdr, ulTotalLen, &stSockAddr);

    free(pstMsgHdr);

    return bRet;
}

SOCKET CreateUDPServerSocket(UINT16 usPort)
{
    SOCKET              stSocket = INVALID_SOCKET;
    SOCKADDR_IN         stLocalAddr = {0};
    UINT32              ulFlag = 1;

    memset(&stLocalAddr, 0, sizeof(stLocalAddr));
    stLocalAddr.sin_family = PF_INET;
    stLocalAddr.sin_port   = htons(usPort);

    GosLog(LOG_INFO, "CreateUDPSocket: port is %d", usPort);

    gos_init_socket();
    stSocket = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (stSocket == INVALID_SOCKET)
    {
        GosLog(LOG_ERROR, "server_create_socket: create socket failed: %s", gos_get_socket_err());
        return INVALID_SOCKET;
    }

    if (setsockopt(stSocket, SOL_SOCKET, SO_REUSEADDR, (CHAR*)&ulFlag, sizeof(ulFlag)) == SOCKET_ERROR)
    {
        GosLog(LOG_ERROR, "server_create_socket: setsockopt failed, %s", gos_get_socket_err());
        CLOSE_SOCKET (stSocket);

        return INVALID_SOCKET;
    }

    if (bind (stSocket, (SOCKADDR *) &stLocalAddr, sizeof (SOCKADDR)) == SOCKET_ERROR)
    {
        GosLog(LOG_ERROR, "server_create_socket: bind socket failed, %s", gos_get_socket_err());
        CLOSE_SOCKET (stSocket);

        return INVALID_SOCKET;
    }

    return stSocket;
}

static INT32 RecvSocketMsg(UINT32 ulTimeout, sockaddr_in &stClientAddr)
{
    FD_SET  fds;
    FD_SET  fdsAll;
    TIMEVAL stTimeout = {0};
    INT32   iRet;
    INT32   iTimeout;
    UINT32  ulStartTime = gos_get_uptime_1ms();
    UINT32  ulTimePassed;
    INT32   iAdddLen = sizeof(sockaddr_in);

    if (g_stUDPSocket == INVALID_SOCKET)
    {
        return -1;
    }

    FD_ZERO(&fdsAll);
    FD_SET(g_stUDPSocket, &fdsAll);

again:
    ulTimePassed = gos_get_uptime_1ms() - ulStartTime;
    iTimeout = ulTimeout - ulTimePassed;

    if (iTimeout <= 0)
    {
        return -1;
    }

    stTimeout.tv_sec = iTimeout/1000;
    stTimeout.tv_usec = 1000*MOD(iTimeout,1000);
    fds = fdsAll;

    iRet = select(g_stUDPSocket + 1, &fds, NULL, NULL, &stTimeout);
    if (iRet == 0)
    {
        goto again;
    }

    if (iRet < 0)
    {
        if (errno == EINTR)
        {
            goto again;
        }

        GosLog(LOG_ERROR, "RecvSocketMsg: recv failed, %s", gos_get_socket_err());
        CLOSE_SOCKET(g_stUDPSocket);

        return -1;
    }

    if (FD_ISSET(g_stUDPSocket, &fds))
    {
        iRet = recvfrom(g_stUDPSocket, (CHAR*)g_aucMsgBuf, sizeof(g_aucMsgBuf)-1, 0,(sockaddr*)&stClientAddr, &iAdddLen);

        return iRet;
    }

    goto again;
}

static GOS_THREAD_RET UDPServerThreadEntry(VOID *pvData)
{
    int         iSize;
    UINT32      ulTotalLen;
    UINT32      ulTimeout = 1000;
    sockaddr_in stClientAddr;
//  MAKE_MSG_HDR_T  *pstMsg = (MAKE_MSG_HDR_T*)g_aucMsgBuf;
    INNER_MAKE_MSG_T    *pstInnerMsg;

//  InnerMsgReady();

    while(g_bUDPServerRunning)
    {
        iSize = RecvSocketMsg(ulTimeout, stClientAddr);
        if (iSize < (int)sizeof(MAKE_MSG_HDR_T))
        {
            continue;
        }

        ulTotalLen = STRUCT_OFFSET(INNER_MAKE_MSG_T, aucMsg) + iSize;
        pstInnerMsg = (INNER_MAKE_MSG_T*)calloc(1, ulTotalLen);
        if (!pstInnerMsg)
        {
            break;
        }

        memcpy(pstInnerMsg->stPeerAddr.aucIP, &stClientAddr.sin_addr, 4);
        pstInnerMsg->stPeerAddr.usPort = ntohs(stClientAddr.sin_port);
        pstInnerMsg->usLen = iSize;
        if (iSize > 0)
        {
            memcpy(pstInnerMsg->aucMsg, g_aucMsgBuf, iSize);
        }

        if (!PostMessage(FormMain->Handle, EV_INNER_MAKE_MSG, 0, (LPARAM)pstInnerMsg))
        {
            free(pstInnerMsg);
        }

//      SendInnerMsg(pstMsg->usMsgID, (UINT8*)(pstMsg+1), pstMsg->ulMsgLen);
    }

    CLOSE_SOCKET(g_stUDPSocket);

    return 0;
}

bool InitUDPServer(UINT16 usPort)
{
    CLOSE_SOCKET(g_stUDPSocket);

    g_stUDPSocket = CreateUDPServerSocket(usPort);
    if (g_stUDPSocket == INVALID_SOCKET)
    {
        return false;
    }

    gos_create_thread(UDPServerThreadEntry, 32*1024, NULL);
    return true;
}
