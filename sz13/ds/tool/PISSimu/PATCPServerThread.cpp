//---------------------------------------------------------------------------

#pragma hdrstop

#include "g_include.h"
#include "ds_public.h"
#include "pis.h"
//#include "msg.h"
#include "InnerMsg.h"
#include "PATCPServerThread.h"

UINT16  g_usPATCPPort = PA_TCP_PORT;

extern UINT32 dtp_get_fd_num(FDSET* fds);
extern SOCKET dtp_get_socket_by_fds(FDSET *fds, UINT32 ulIndex);

SOCKET      g_stPATCPSocket = INVALID_SOCKET;
SOCKET      g_stSARealBrdSocket = INVALID_SOCKET;
//SOCKET        g_stSASocket = INVALID_SOCKET;

static SOCKET CreateSocket()
{
    SOCKET              stSocket = INVALID_SOCKET;
    SOCKADDR_IN         stLocalAddr = {0};
    UINT32              ulFlag = 1;
//    UINT8               aucLocalAddr[4] = {192,0,1,223};
    UINT8               aucLocalAddr[] = {0,0,0,0};

    gos_init_socket();

    memset(&stLocalAddr, 0, sizeof(stLocalAddr));
    stLocalAddr.sin_family = PF_INET;
    stLocalAddr.sin_port   = HTONS(g_usPATCPPort);
    memcpy(&stLocalAddr.sin_addr.s_addr, aucLocalAddr, 4);

    stSocket = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
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

    // mpk
    if (listen (stSocket, 16) == SOCKET_ERROR)
    {
        GosLog(LOG_ERROR, "server_create_socket: listen socket failed, %s", gos_get_socket_err());
        CLOSE_SOCKET (stSocket);

        return INVALID_SOCKET;
    }

    return stSocket;
}

VOID PATCPThread::HandleRecvMsg()
{
    PA_MSG_HDR_T    *pstMsgHdr = (PA_MSG_HDR_T*)m_aucRecvBuf;
    UINT32          ulMsgLen = sizeof(PA_MSG_HDR_T)+pstMsgHdr->usMsgLen;
    INNER_MSG_T     *pstMsg = (INNER_MSG_T*)calloc(1, STRUCT_OFFSET(INNER_MSG_T, aucMsg)+ulMsgLen);

    if (!pstMsg)
    {
        GosLog(LOG_ERROR,"PATcpThread Alloc InnerMsg Failed");
        return;
    }

    pstMsg->ulMsgType = MSG_PA_TCP;
    pstMsg->ulMsgLen = ulMsgLen;
    memcpy(pstMsg->aucMsg, m_aucRecvBuf, pstMsg->ulMsgLen);

    if (!PostMessage(m_hWnd, WM_INNER_MSG, 0, (LPARAM)pstMsg))
    {
        GosLog(LOG_ERROR, "post PA tcp msg failed!");
        free(pstMsg);
    }
}

INT32 PATCPThread::RecvMsg(SOCKET stSocket)
{
    INT32       iRecvSize;
    UINT32      ulByteToRecved = 0;
    PA_MSG_HDR_T    *pstMsgHdr = (PA_MSG_HDR_T*)m_aucRecvBuf;

    /* 看看有多少字节可读 */
    if (ioctlsocket(stSocket, FIONREAD, (unsigned long*)&ulByteToRecved) == SOCKET_ERROR)
    {
        return -1;
    }

    /* 如果没有字节可读, 则对端已经关闭连接, 返回错误 */
    if (ulByteToRecved == 0)
    {
        return 0;
    }

    /* 处理上次RecvMsg的剩余数据 */
    iRecvSize = recv(stSocket, (CHAR*)pstMsgHdr, sizeof(PA_MSG_HDR_T), 0);
    if (iRecvSize != sizeof(PA_MSG_HDR_T))
    {
        return -1;
    }

    NTOHS_IT(pstMsgHdr->usMsgMagic);

    if (pstMsgHdr->usMsgMagic != PIS_MSG_MAGIC)
    {
        return -1;
    }

    NTOHS_IT(pstMsgHdr->usMsgType);
    NTOHS_IT(pstMsgHdr->usMsgLen);

    UINT32  usMsgLen = pstMsgHdr->usMsgLen;

    iRecvSize = recv(stSocket, (CHAR*)(pstMsgHdr+1), usMsgLen, 0);
    if (iRecvSize != usMsgLen)
    {
        return -1;
    }

    return sizeof(PA_MSG_HDR_T)+iRecvSize;
}

INT32 PATCPThread::ListenMsg(UINT32 ulTimeout)
{
    FDSET       fds;
    FDSET       fdsAll;
    TIMEVAL     stTimeout = {0};
    SOCKET      stClientSocket = INVALID_SOCKET;
    SOCKADDR    stAddr;
    INT32       iAddrLen = sizeof(SOCKADDR);
    INT32       iRet;
    INT32       iRecvLen;
    INT32       iErrCode = 0;
    UINT32      ulFdCount;
    UINT32      ulTotalFdCount;
    UINT32      i;
    INT32       iNoDelay = 1;
    BOOL        bNeedClose;
    SOCKET      stSocket = g_stPATCPSocket;

    FD_ZERO(&fdsAll);
    FD_SET(stSocket, &fdsAll);

again:
    fds = fdsAll;

    stTimeout.tv_sec = ulTimeout;
    iRet = select(g_stPATCPSocket+1, &fds, NULL, NULL, &stTimeout);
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
    }

    if (FD_ISSET(stSocket, &fds))
    {
       // FD_CLR(stSocket, &fds);
accept_again:
        stClientSocket = accept (stSocket, &stAddr, (socklen_t*)&iAddrLen);
        if (stClientSocket == SOCKET_ERROR)
        {
            if (/*(errno == ECONNABORTED) || (*/ errno == EINTR)
            {
                goto accept_again;
            }

            gos_sleep_1ms(10);
            goto again;
            //return -1;
        }

        /* 设置NoDelay方式 */
        setsockopt(stClientSocket, IPPROTO_TCP, TCP_NODELAY, (CHAR*)&iNoDelay, sizeof(iNoDelay));

        if (g_stSARealBrdSocket != stClientSocket)
        {
            if (g_stSARealBrdSocket != INVALID_SOCKET)
            {
                FD_CLR(g_stSARealBrdSocket, &fdsAll);
                CLOSE_SOCKET(g_stSARealBrdSocket);

                GosLog(LOG_INFO, "close old socket with SA");
            }

            FD_SET(stClientSocket, &fdsAll);
            g_stSARealBrdSocket = stClientSocket;
        }

        goto again;
    }

    /* 处理 tcp 端口上的消息 */
    ulFdCount = 0;
    ulTotalFdCount = dtp_get_fd_num(&fds);
    for (i = 0; i < FD_SETSIZE; i++)
    {
        bNeedClose = FALSE;

        if (ulFdCount >= ulTotalFdCount)
        {
            break;
        }

        stClientSocket = dtp_get_socket_by_fds(&fds, i);
        if (stClientSocket == INVALID_SOCKET)
        {
            continue;
        }

        if (stClientSocket == stSocket)
        {
            continue;
        }

        ulFdCount++;

        if (!FD_ISSET(stClientSocket, &fds))
        {
            continue;
        }

        /* 有消息到, 进行处理 */
        iErrCode = 0;
        iRecvLen = RecvMsg(stClientSocket);
        if (iRecvLen <= 0)
        {
            FD_CLR(stClientSocket, &fdsAll);
            CLOSE_SOCKET(stClientSocket);
            g_stSARealBrdSocket = INVALID_SOCKET;
        }
        else
        {
            HandleRecvMsg();
        }
    }

    goto again;
}

BOOL PATCPThread::Init()
{
    g_stPATCPSocket = CreateSocket();
    if (g_stPATCPSocket == INVALID_SOCKET)
    {
        return FALSE;
    }

    return TRUE;
}

PATCPThread::PATCPThread(HWND hWnd):GThread(NULL)
{
    m_hWnd = hWnd;
}

GOS_THREAD_RET PATCPThread::ThreadEntry(void* pPara)
{
    UINT32      ulTimeout = 2;

    while(1)
    {
        ListenMsg(ulTimeout);
    }
}

//---------------------------------------------------------------------------
#pragma package(smart_init)
