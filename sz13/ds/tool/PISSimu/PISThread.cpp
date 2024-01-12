#include "g_include.h"
#include "ds_public.h"
#include "pis.h"
#include "InnerMsg.h"
#include "PISThread.h"


UINT16  g_usPISTCPPort = PIS_TCP_PORT;

//车载台与PIS之间的组播端口为           57000，组播地址224.8.0.0

//extern void GetATSInfo(UINT32 ulMaxNum, GET_ATS_INFO_RSP_T *pstRsp);

extern UINT32 dtp_get_fd_num(FDSET* fds);
extern SOCKET dtp_get_socket_by_fds(FDSET *fds, UINT32 ulIndex);

SOCKET      g_stPISSocket = INVALID_SOCKET;
SOCKET      g_stSASocket = INVALID_SOCKET;      // PIS Client socket

static SOCKET CreateSocket(UINT8 *pucAddr)
{
    SOCKET              stSocket = INVALID_SOCKET;
    SOCKADDR_IN         stLocalAddr = {0};
    UINT32              ulFlag = 1;

    gos_init_socket();

    stLocalAddr.sin_family = PF_INET;
    stLocalAddr.sin_port   = HTONS(g_usPISTCPPort);
    memcpy(&stLocalAddr.sin_addr.s_addr, pucAddr, 4);

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

    GosLog(LOG_INFO, "create PIS tcp server on "IP_FMT ":%u", IP_ARG(pucAddr), g_usPISTCPPort);

    return stSocket;
}

PISThread::PISThread(HWND hWnd, UINT8 *pucAddr):GThread(NULL)
{
    m_hWnd = hWnd;
    memcpy(m_aucAddr, pucAddr, 4);
}

INT32 PISThread::RecvMsg(SOCKET stSocket)
{
    INT32       iRecvSize;
    UINT32      ulByteToRecved = 0;
    PIS_MSG_HDR_T   *pstMsgHdr = (PIS_MSG_HDR_T*)m_aucRecvBuf;

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
    iRecvSize = recv(stSocket, (CHAR*)pstMsgHdr, sizeof(PIS_MSG_HDR_T), 0);
    if (iRecvSize != sizeof(PIS_MSG_HDR_T))
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

#if 0
    if (pstMsgHdr->usMsgType != EV_PIS_HEART_BEAT &&
        pstMsgHdr->usMsgType != EV_IPH_TALK_APPLY &&        // 对讲申请应答
        pstMsgHdr->usMsgType != EV_IPH_AUDIO_DOWNLOAD &&    // 调度台音频下发
        pstMsgHdr->usMsgType != EV_IPH_STOP_TALK )          // 调度台结束对讲
    {
       //   GosLog(LOG_ERROR, "invalid msg type %u", pstMsgHdr->usMsgType);
        return -1;
    }
#endif

    iRecvSize = recv(stSocket, (CHAR*)(pstMsgHdr+1), usMsgLen, 0);
    if (iRecvSize != usMsgLen)
    {
        return -1;
    }

    return sizeof(PIS_MSG_HDR_T)+iRecvSize;
}

INT32 PISThread::ListenMsg(UINT32 ulTimeout)
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
    SOCKET      stSocket = g_stPISSocket;

    FD_ZERO(&fdsAll);
    FD_SET(stSocket, &fdsAll);

again:
    fds = fdsAll;

    stTimeout.tv_sec = ulTimeout;
    iRet = select(g_stPISSocket+1, &fds, NULL, NULL, &stTimeout);
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

        if (g_stSASocket != stClientSocket)
        {
            if (g_stSASocket != INVALID_SOCKET)
            {
                FD_CLR(g_stSASocket, &fdsAll);
                CLOSE_SOCKET(g_stSASocket);

                GosLog(LOG_INFO, "close old socket with SA");
            }

            FD_SET(stClientSocket, &fdsAll);
            g_stSASocket = stClientSocket;
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
            g_stSASocket = INVALID_SOCKET;
        }
        else
        {
            HandleRecvMsg();
        }
    }

    goto again;
}

VOID PISThread::HandleRecvMsg()
{
    PIS_MSG_HDR_T   *pstMsgHdr = (PIS_MSG_HDR_T*)m_aucRecvBuf;
    UINT32          ulMsgLen = sizeof(PIS_MSG_HDR_T)+pstMsgHdr->usMsgLen;
    INNER_MSG_T     *pstMsg = (INNER_MSG_T*)calloc(1, STRUCT_OFFSET(INNER_MSG_T, aucMsg)+ulMsgLen);

    if (!pstMsg)
    {
        return;
    }

    pstMsg->ulMsgType = MSG_PIS;
    pstMsg->ulMsgLen = ulMsgLen;
    memcpy(pstMsg->aucMsg, m_aucRecvBuf, pstMsg->ulMsgLen);

    if (!PostMessage(m_hWnd, WM_INNER_MSG, 0, (LPARAM)pstMsg))
    {
        free(pstMsg);
    }
}

BOOL PISThread::Init()
{
    g_stPISSocket = CreateSocket(m_aucAddr);
    if (g_stPISSocket == INVALID_SOCKET)
    {
        return FALSE;
    }

    return TRUE;
}

GOS_THREAD_RET PISThread::ThreadEntry(void* pPara)
{
    UINT32      ulTimeout = 2;

    while(1)
    {
        ListenMsg(ulTimeout);
    }
}
