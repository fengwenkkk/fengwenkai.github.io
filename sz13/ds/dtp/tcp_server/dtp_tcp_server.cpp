#include "g_include.h"

#include "dtp_public.h"
#include "dtp_common.h"
#include "dtp_func.h"
#include "dtp_tcp_server.h"

extern GOS_THREAD_RET DTPTCPServerSendThreadEntry(VOID *pvData);

// 接收消息后对消息的核查结果
#define DTP_MSG_CHECK_SUCCESS               0
#define DTP_MSG_CHECK_HDR_INCOMPLETE        1
#define DTP_MSG_CHECK_MSG_INCOMPLETE        2
#define DTP_MSG_CHECK_LEN_ERROR             3
#define DTP_MSG_CHECK_HANDLE_FAIL           4

#define DTPServerImp    DTPTCPServer

/*******************************************************************************
* 函数名称: _CloseSocket
* 功    能: 关闭客户端SOCKET
* 参    数:
* 参数名称              输入/输出   描述
* pstFdSet              IN          fd描述集
* stSocket              IN          客户端socket
* 函数返回:
* 说    明:
*******************************************************************************/
VOID DTPServerImp::_CloseSocket(SOCKET *pstSocket)
{
    if (m_pstServer && pstSocket)
    {
        FD_CLR (*pstSocket, &m_pstServer->stFdSet);
        CLOSE_SOCKET(*pstSocket);
    }

    if (m_pstServer)
    {
        m_pstServer->ulSendErrNum = 0;
        m_pstServer->ulRecvErrNum = 0;
    }
}

/*******************************************************************************
* 函数名称: CheckRecvMsg
* 功    能: 处理客户端的请求消息（TCP方式）
* 参    数:
* 参数名称              输入/输出   描述
* stSocket              IN          accept到底TCP客户端socket
* pucMsg                IN          客户端请求消息
* ulTotalMsgLen         IN          接收到的消息长度
* pulMsgSize            OUT         本次处理的消息长度
* 函数返回:
* 说    明:
*******************************************************************************/
UINT32 DTPServerImp::CheckRecvMsg(SOCKET stSocket, UINT8 *pucMsg, UINT32 ulTotalMsgLen, UINT32 *pulMsgSize)
{
    DTP_MSG_HDR_T   *pstMsgHdr;
    UINT32          ulMsgSize;

    if (ulTotalMsgLen < sizeof(DTP_MSG_HDR_T))
    {
        *pulMsgSize = 0;
        return DTP_MSG_CHECK_HDR_INCOMPLETE;
    }

    /* 字节顺序转换 */
    pstMsgHdr = (DTP_MSG_HDR_T *)pucMsg;
    ulMsgSize = NTOHL(pstMsgHdr->ulMsgLen);

    if (ulMsgSize < sizeof(DTP_MSG_HDR_T))
    {
        DTPLog(LOG_ERROR, "server_check_recv_msg: msg length(%u) is too short!", ulMsgSize);
        *pulMsgSize = 0;

        return DTP_MSG_CHECK_LEN_ERROR;
    }

    if (ulMsgSize > sizeof(DTP_MSG_T) )
    {
        DTPLog(LOG_ERROR, "server_check_recv_msg: msg length(%u) is too long!", ulMsgSize);
        *pulMsgSize = 0;

        return DTP_MSG_CHECK_LEN_ERROR;
    }

    if (ulTotalMsgLen < ulMsgSize)
    {
        *pulMsgSize = 0;

        return DTP_MSG_CHECK_MSG_INCOMPLETE;
    }

    dtp_msg_ntoh(pstMsgHdr);
    if (pstMsgHdr->ulMsgMagic != DTP_MSG_MAGIC ||
        pstMsgHdr->usMsgVer != DTP_MSG_VER)
    {
        return DTP_MSG_CHECK_HDR_INCOMPLETE;
    }

    /* 消息处理 */
    *pulMsgSize = ulMsgSize;

    HandleClientMsg(stSocket, (DTP_MSG_T *)pucMsg);

    return DTP_MSG_CHECK_SUCCESS;
}

/*******************************************************************************
* 函数名称: _GetConnectInfo
* 功    能: 获取socket对应的客户端连接信息
* 参    数:
* 参数名称              输入/输出   描述
* stSocket              IN          客户端socket
* 函数返回:
* 说    明:
*******************************************************************************/
static DTP_CONN_INFO_T* _GetConnectInfo(DTP_SERVER_INFO_T *pstServer, SOCKET stSocket)
{
    UINT32          i;

    for (i=0; i<ARRAY_SIZE(pstServer->astConnInfo); i++)
    {
        if (pstServer->astConnInfo[i].stSocket == stSocket)
        {
            DTP_CONN_INFO_T *pstConnInfo = &pstServer->astConnInfo[i];

            return pstConnInfo;
        }
    }

    return NULL;
}

static BOOL _GetConnectInfo(DTP_SERVER_INFO_T *pstServer, SOCKET stSocket, DTP_CONN_INFO_T *pstConnInfo)
{
    for (UINT32 i=0; i<ARRAY_SIZE(pstServer->astConnInfo); i++)
    {
        if (pstServer->astConnInfo[i].stSocket == stSocket)
        {
            memcpy(pstConnInfo, &pstServer->astConnInfo[i], sizeof(DTP_CONN_INFO_T));

            return TRUE;
        }
    }

    return FALSE;
}

VOID DTPServerImp::_InitConnectInfo(DTP_CONN_INFO_T *pstConn)
{
    UINT8           *pucRecvBuf = pstConn->pucRecvBuf;

    memset(pstConn, 0, sizeof(DTP_CONN_INFO_T));
    pstConn->pucRecvBuf = pucRecvBuf;
    pstConn->stSocket = INVALID_SOCKET;
    pstConn->enClientStatus = DTP_CLIENT_IDLE;
    pstConn->usClientID = DTP_INVALID_INST_ID;
}

VOID DTPServerImp::_InitConnectInfo(SOCKET stSocket, BOOL bNeedMutex)
{
    bNeedMutex = TRUE;

    if (bNeedMutex && !gos_mutex(m_pstServer->hConnInfo))
    {
        DTPLog(LOG_ERROR, "mutex failed");
        return;
    }

    BOOL bExist = FALSE;

    for (UINT32 i=0; i<ARRAY_SIZE(m_pstServer->astConnInfo); i++)
    {
        if (m_pstServer->astConnInfo[i].stSocket == stSocket)
        {
            _InitConnectInfo(&m_pstServer->astConnInfo[i]);
            _CloseSocket(&stSocket);

            bExist = TRUE;
            break;
        }
    }

    if (!bExist)
    {
        DTPLog(LOG_ERROR, "invalid socket(%u)", stSocket);
    }

    if (bNeedMutex)
    {
        gos_unmutex(m_pstServer->hConnInfo);
    }
}

/*******************************************************************************
* 函数名称: _SetConnectInfo
* 功    能: 设置socket对应的客户端连接信息
* 参    数:
* 参数名称              输入/输出   描述
* pstFdSet              IN          fd描述集
* stSocket              IN          客户端socket
* pstAddr               IN          客户端地址
* 函数返回:
* 说    明:
*******************************************************************************/
BOOL DTPServerImp::_SetConnectInfo(SOCKET stSocket, SOCKADDR_IN *pstAddr)
{
    UINT32              i;
    UINT32              ulCurrTime = gos_get_uptime();
    DTP_CONN_INFO_T    *pstTmpInfo;
    DTP_CONN_INFO_T    *pstFreeConnInfo = NULL;

    if (!gos_mutex(m_pstServer->hConnInfo))
    {
        DTPLog(LOG_ERROR, "mutex failed");
        return FALSE;
    }

    for (i=0; i<ARRAY_SIZE(m_pstServer->astConnInfo); i++)
    {
        pstTmpInfo = &m_pstServer->astConnInfo[i];
        if (pstTmpInfo->stSocket != INVALID_SOCKET)
        {
            if (pstTmpInfo->stSocket == stSocket)
            {
                DTPLog(LOG_INFO, "reset connect(%u) by reconnect", pstTmpInfo->stSocket);
                _InitConnectInfo(pstTmpInfo);

                pstFreeConnInfo = pstTmpInfo;

                break;
            }
            else if ((ulCurrTime - pstTmpInfo->ulLastRecvTime) > m_pstServer->ulRecvCheckTime)
            {
                DTPLog(LOG_INFO, "close connect(%u) by link check failed", pstTmpInfo->stSocket);
                _CloseSocket(&pstTmpInfo->stSocket);
            }
        }

        if (NULL == pstFreeConnInfo &&
            pstTmpInfo->stSocket == INVALID_SOCKET)
        {
            pstFreeConnInfo = pstTmpInfo;
        }
    }

    if (!pstFreeConnInfo)
    {
        DTPLog(LOG_ERROR, "no free connection to be alloced");

        gos_unmutex(m_pstServer->hConnInfo);
        return FALSE;
    }

    pstFreeConnInfo->usClientID = DTP_INVALID_INST_ID;
    if (!AllocClientID(pstFreeConnInfo))
    {
        DTPLog(LOG_INFO, "alloc client id(%u) failed", pstFreeConnInfo->stSocket);

        _InitConnectInfo(pstFreeConnInfo->stSocket, FALSE);
        gos_unmutex(m_pstServer->hConnInfo);
        return FALSE;
    }

    pstFreeConnInfo->stSocket = stSocket;
    pstFreeConnInfo->ucNeedClose = FALSE;

    pstFreeConnInfo->stClientSendAddr = *pstAddr;
    pstFreeConnInfo->ulConnectSetupTime = ulCurrTime;
    pstFreeConnInfo->enClientStatus = DTP_CLIENT_CONNECTED;
    pstFreeConnInfo->ulLastRecvTime = gos_get_uptime();
    pstFreeConnInfo->ulRecvOffset = 0;
    pstFreeConnInfo->ulRecvSize = 0;

    gos_unmutex(m_pstServer->hConnInfo);

    return TRUE;
}

BOOL DTPServerImp::SetConnectClose(SOCKET stSocket)
{
    if (!gos_mutex(m_pstServer->hConnInfo))
    {
        return FALSE;
    }

    for (UINT32 i=0; i<ARRAY_SIZE(m_pstServer->astConnInfo); i++)
    {
        if (m_pstServer->astConnInfo[i].stSocket == stSocket)
        {
            m_pstServer->astConnInfo[i].ucNeedClose = TRUE;
            break;
        }
    }

    gos_unmutex(m_pstServer->hConnInfo);

    return TRUE;
}

/*******************************************************************************
* 函数名称: CloseConnnect
* 功    能: 关闭客户端连接
* 参    数:
* 参数名称              输入/输出   描述
* pstFdSet              IN          fd描述集
* stSocket              IN          客户端socket
* 函数返回:
* 说    明:
*******************************************************************************/
VOID DTPServerImp::CloseConnnect(DTP_CONN_INFO_T *pstConnInfo)
{
    if (!pstConnInfo)
    {
        return;
    }

    m_pMsgHandle->OnDisconnectClient(pstConnInfo->usClientID);

    DTPLog(LOG_INFO, "server_close_connect: close the connect(%u) with " IP_FMT " (%d)!",
             pstConnInfo->stSocket, IP_ARG(&pstConnInfo->stClientSendAddr.sin_addr.s_addr), NTOHS(pstConnInfo->stClientSendAddr.sin_port));

    _InitConnectInfo(pstConnInfo->stSocket, FALSE);
}

VOID DTPServerImp::PrintConnect()
{
    for (UINT32 i=0; i<ARRAY_SIZE(m_pstServer->astConnInfo); i++)
    {
        DTP_CONN_INFO_T     *pstConnInfo = &m_pstServer->astConnInfo[i];

        if (pstConnInfo->stSocket != INVALID_SOCKET)
        {
            DTPLog(LOG_INFO, "connect %u: client=%u, socket=%u, addr=" IP_FMT "(%u)",
                i, pstConnInfo->usClientID, pstConnInfo->stSocket,
                IP_ARG(&pstConnInfo->stClientSendAddr.sin_addr.s_addr),
                NTOHS(pstConnInfo->stClientSendAddr.sin_port));
        }
    }
}

/*******************************************************************************
* 函数名称: RecvMsg
* 功    能: 接收客户端TCP消息
* 参    数:
* 参数名称              输入/输出   描述
* stSocket              IN          客户端socket
* piErrCode             OUT         错误码
* 函数返回:
* 说    明:
*******************************************************************************/
INT32 DTPServerImp::RecvMsg(DTP_CONN_INFO_T *pstConnInfo, INT32 *piErrCode)
{
    INT32           iRecvSize;
    UINT32          ulRet;
    UINT32          ulTotalLen;
    UINT32          ulHandledMsgSize;
    UINT8           *pucMsgStart;
    UINT32          ulByteToRecved = 0;
    UINT32          ulMaxBufSize = DTP_MAX_MSGBUF_LEN;
    SOCKET          stSocket = pstConnInfo->stSocket;

#if 1
    /* 看看有多少字节可读 */
    if (ioctlsocket (stSocket, FIONREAD, (unsigned long*)&ulByteToRecved) == SOCKET_ERROR)
    {
        DTPLog(LOG_ERROR, "server_recv_msg: ioctl socket(%u) failed!", stSocket);
        *piErrCode = GetSockErr ();
        return -1;
    }

    /* 如果没有字节可读, 则对端已经关闭连接, 返回错误 */
    if (ulByteToRecved == 0)
    {
        *piErrCode = 0;// SECONNRESET;
        return 0;
    }

#endif

    /* 开始处理该连接上的数据 */
    ulTotalLen = 0;

    if (!pstConnInfo->pucRecvBuf)
    {
        pstConnInfo->pucRecvBuf = (UINT8*)calloc(1, ulMaxBufSize);
        if (!pstConnInfo->pucRecvBuf)
        {
            DTPLog(LOG_ERROR, "server_recv_msg: alloc recv buffer failed!");
            *piErrCode = -1;
            return -1;
        }

        pstConnInfo->ulRecvOffset = 0;
        pstConnInfo->ulRecvSize = 0;
    }

    /* 处理上次RecvMsg的剩余数据 */
    iRecvSize = recv(stSocket, (CHAR*)(pstConnInfo->pucRecvBuf + pstConnInfo->ulRecvOffset + pstConnInfo->ulRecvSize),
                     ulMaxBufSize - (pstConnInfo->ulRecvOffset+pstConnInfo->ulRecvSize), 0);
    if (iRecvSize < 0)
    {
        *piErrCode = -1;
        DTPLog(LOG_ERROR, "server_recv_msg: recv client(%u) failed: %s!", stSocket, gos_get_socket_err());

        return -1;
    }
    else if (iRecvSize == 0)
    {
        DTPLog(LOG_ERROR, "server_recv_msg: recv client(%u) failed by connect closed", stSocket);
        _InitConnectInfo(stSocket, FALSE);

        return 0;
    }

    pstConnInfo->ulRecvSize += iRecvSize;
    ulTotalLen  = pstConnInfo->ulRecvSize;
    pucMsgStart = pstConnInfo->pucRecvBuf + pstConnInfo->ulRecvOffset;

    while (ulTotalLen > 0)
    {
        ulRet = CheckRecvMsg(stSocket, pucMsgStart, ulTotalLen, &ulHandledMsgSize);

        if (ulRet == DTP_MSG_CHECK_SUCCESS) /* 提取消息成功 */
        {
            pucMsgStart += ulHandledMsgSize;
            ulTotalLen  -= ulHandledMsgSize;
        }
        else if (ulRet == DTP_MSG_CHECK_HANDLE_FAIL) /* 该消息解析失败 */
        {
            pucMsgStart += ulHandledMsgSize;
            ulTotalLen  -= ulHandledMsgSize;
        }
        else if (ulRet == DTP_MSG_CHECK_MSG_INCOMPLETE) /* 不能构成完整的消息 */
        {
            break;
        }
        else if (ulRet == DTP_MSG_CHECK_HDR_INCOMPLETE) /* 消息头不完整 */
        {
            break;
        }
        else if (ulRet == DTP_MSG_CHECK_LEN_ERROR) /* 消息大小指示错误 */
        {
            DTPLog(LOG_ERROR, "server_recv_msg: msg length error!");
            *piErrCode = -1;
            return -1;
        }
        else
        {
            DTPLog(LOG_ERROR, "server_recv_msg: unexpected result %u !", ulRet);
            break;
        }
    }

    pstConnInfo->ulRecvOffset += (pstConnInfo->ulRecvSize-ulTotalLen);

    pstConnInfo->ulRecvSize = ulTotalLen;
    if (ulTotalLen > 0 &&
        (pstConnInfo->ulRecvOffset+pstConnInfo->ulRecvSize+sizeof(DTP_MSG_T) > ulMaxBufSize) )
    {
        /* 如果有剩余数据未处理完，则挪到buffer头部 */
        memmove (pstConnInfo->pucRecvBuf, pucMsgStart, ulTotalLen);
        pstConnInfo->ulRecvOffset = 0;
    }

    *piErrCode = 0;
    return iRecvSize;
}

VOID DTPServerImp::ClientLinkCheck(DTP_CONN_INFO_T *pstConnInfo, BOOL *pbNeedClose)
{
    UINT32              ulCurrTime = gos_get_uptime();

    *pbNeedClose = FALSE;
    if (!pstConnInfo)
    {
        return;
    }

    if (pstConnInfo->enClientStatus == DTP_CLIENT_CONNECTED)
    {
        pstConnInfo->enClientStatus = DTP_CLIENT_LINKAUTHED;
        return;
    }

    if (pstConnInfo->enClientStatus != DTP_CLIENT_LINKAUTHED)
    {
        return;
    }

    if ((UINT32)(ulCurrTime - pstConnInfo->ulLastRecvTime) > m_pstServer->ulRecvCheckTime)
    {
        DTPLog(LOG_ERROR, "close connect with client(%u) by link check", pstConnInfo->stSocket);
        *pbNeedClose = TRUE;
        return;
    }

    UINT32                  ulSeqID   = _GetSeqID();
    DTP_TCP_LINK_AUTH_REQ_T stReq = {0};

    stReq.stMsgHdr.ulMsgMagic= DTP_MSG_MAGIC;
    stReq.stMsgHdr.usMsgVer  = DTP_MSG_VER;
    stReq.stMsgHdr.ulMsgLen = sizeof(stReq);
    stReq.stMsgHdr.usMsgType = DTP_EV_LINK_CHECK_REQ;
    stReq.stMsgHdr.ulSeqID = ulSeqID;

    if (!SendBySocket(pstConnInfo->stSocket, &stReq.stMsgHdr))
    {
        *pbNeedClose = TRUE;
    }
}

/*******************************************************************************
* 函数名称: ListenMsg
* 功    能: 侦听TCP消息
* 参    数:
* 参数名称              输入/输出   描述
* piErrCode             OUT         错误码
* 函数返回:
* 说    明:
*******************************************************************************/
INT32 DTPServerImp::ListenMsg(UINT32 ulTimeout)
{
    FDSET       fds;
    TIMEVAL     stTimeout = {0};
    SOCKET      stClientSocket = INVALID_SOCKET;
    SOCKADDR_IN stAddr;
    INT32       iAddrLen = sizeof(SOCKADDR);
    INT32       iRet;
    INT32       iRecvLen;
    INT32       iErrCode = 0;
    UINT32      ulFdCount;
    UINT32      ulTotalFdCount;
    UINT32      i;
    INT32       iNoDelay = 1;
    BOOL        bNeedClose;
    UINT32      ulMaxFd;
    DTP_CONN_INFO_T *pstConnInfo;

    if (m_pstServer->stSocket == INVALID_SOCKET)
    {
        DTPLog(LOG_ERROR, "server_listen_msg: invalid server socket");
        return -1;
    }

again:
    fds = m_pstServer->stFdSet;
    FD_CLR(m_pstServer->stSocket, &fds);
    FD_SET(m_pstServer->stSocket, &fds);

    stTimeout.tv_sec = ulTimeout;
    ulMaxFd = dtp_get_max_fd(&fds);

    iRet = select(ulMaxFd+1, &fds, NULL, NULL, &stTimeout);
    if (iRet == 0)
    {
        return 0;
    }

    if (iRet < 0)
    {
        if (errno == EINTR)
        {
            goto again;
        }

        DTPLog(LOG_ERROR, "server_listen_msg: recv failed, %s", gos_get_socket_err());
    }

    if (m_pstServer->stSocket != INVALID_SOCKET && FD_ISSET(m_pstServer->stSocket, &fds))
    {
        FD_CLR(m_pstServer->stSocket, &fds);
        stClientSocket = accept (m_pstServer->stSocket, (SOCKADDR*)&stAddr, (socklen_t*)&iAddrLen);
        if (stClientSocket == INVALID_SOCKET)
        {
            DTPLog(LOG_ERROR, "server_listen_msg: accept new connect failed, %s", gos_get_socket_err());
            return -1;
        }

        /* 设置NoDelay方式 */
        setsockopt(stClientSocket, IPPROTO_TCP, TCP_NODELAY, (CHAR*)&iNoDelay, sizeof(iNoDelay));

        if (!_SetConnectInfo(stClientSocket, (SOCKADDR_IN*)&stAddr))
        {
            DTPLog(LOG_WARN, "server_listen_msg: set client socket(%d) info failed", stClientSocket);
            _CloseSocket(&stClientSocket);

            goto again;
        }
        else
        {
            DTPLog(LOG_INFO, "server_listen_msg: set client socket(%u) of " IP_FMT ":%u ok", stClientSocket,
                IP_ARG(&stAddr.sin_addr.s_addr), NTOHS(stAddr.sin_port));

            FD_SET(stClientSocket, &m_pstServer->stFdSet);

            fds = m_pstServer->stFdSet;
            FD_CLR(m_pstServer->stSocket, &fds);

            goto again;
        }
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

        if (stClientSocket == m_pstServer->stSocket)
        {
            continue;
        }

        ulFdCount++;

        pstConnInfo = _GetConnectInfo(m_pstServer, stClientSocket);
        if (!pstConnInfo)
        {
            DTPLog(LOG_ERROR, "server_listen_msg: get client socket(%d) info failed!", stClientSocket);
            bNeedClose = TRUE;
            goto end;
        }
        else if (pstConnInfo->ucNeedClose)
        {
            bNeedClose = TRUE;
            goto end;
        }

        /* 有消息到, 进行处理 */
        iErrCode = 0;
        iRecvLen = RecvMsg(pstConnInfo, &iErrCode);
        if (iRecvLen < 0)
        {
            if (iErrCode != 0)
            {
                if (SECONNRESET == iErrCode)
                {
                    DTPLog(LOG_WARN, "server_listen_msg: client socket(%d) closed due to connect reset",
                        stClientSocket);
                }
                else
                {
                    DTPLog(LOG_WARN, "server_listen_msg: client socket(%d) closed due to %s",
                        stClientSocket, gos_get_socket_err());
                }

                bNeedClose = TRUE;
            }
        }
        else if (iRecvLen == 0)
        {
            ClientLinkCheck(pstConnInfo, &bNeedClose);
        }
        else
        {
            m_pstServer->ulRecvErrNum = 0;
        }

end:
        if (bNeedClose)
        {
            CloseConnnect(pstConnInfo);
        }
    }

    return 0;
}

/*******************************************************************************
* 函数名称: CreateSocket
* 功    能: 创建TCP socket
* 参    数:
* 参数名称              输入/输出   描述
* 函数返回:
* 说    明:
*******************************************************************************/
SOCKET DTPServerImp::CreateSocket()
{
    SOCKET              stSocket = INVALID_SOCKET;
    SOCKADDR_IN         stLocalAddr = {0};
    UINT32              ulFlag = 1;

    stLocalAddr.sin_family = PF_INET;
    stLocalAddr.sin_port   = HTONS(m_pstServer->usPort);
    memcpy(&stLocalAddr.sin_addr.s_addr, m_pstServer->aucAddr, 4);

    stSocket = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (stSocket == INVALID_SOCKET)
    {
        DTPLog(LOG_ERROR, "server_create_socket: create socket failed: %s", gos_get_socket_err());
        return INVALID_SOCKET;
    }

    if (setsockopt(stSocket, SOL_SOCKET, SO_REUSEADDR, (CHAR*)&ulFlag, sizeof(ulFlag)) == SOCKET_ERROR)
    {
        DTPLog(LOG_ERROR, "server_create_socket: setsockopt failed, %s", gos_get_socket_err());
        CLOSE_SOCKET (stSocket);

        return INVALID_SOCKET;
    }

    if (bind (stSocket, (SOCKADDR *) &stLocalAddr, sizeof (SOCKADDR)) == SOCKET_ERROR)
    {
        DTPLog(LOG_ERROR, "tcp server_create_socket: bind socket failed on " IP_FMT ":%u, %s", IP_ARG(m_pstServer->aucAddr), m_pstServer->usPort, gos_get_socket_err());
        CLOSE_SOCKET (stSocket);

        return INVALID_SOCKET;
    }

    // mpk
    if (listen (stSocket, 16) == SOCKET_ERROR)
    {
        DTPLog(LOG_ERROR, "tcp server_create_socket: listen socket failed, %s", gos_get_socket_err());
        CLOSE_SOCKET (stSocket);

        return INVALID_SOCKET;
    }

    return stSocket;
}

BOOL DTPServerImp::Init(UINT16 usInstID, UINT8 *pucAddr, UINT16 usPort)
{
    UINT32              i;
    UINT32              ulMaxCtrlMsgBufSize = 64*1024;
    DTP_SERVER_INFO_T   *pstServer = (DTP_SERVER_INFO_T*)GOS_ZALLOC(sizeof(DTP_SERVER_INFO_T));

    if (NULL == pstServer)
    {
        return FALSE;
    }

    pstServer->enStatus = DTP_SERVER_IDLE;
    pstServer->bClose   = FALSE;
    pstServer->stSocket = INVALID_SOCKET;

    if (NULL == pucAddr)
    {
        memset(pstServer->aucAddr, 0, sizeof(pstServer->aucAddr));
    }
    else
    {
        memcpy(pstServer->aucAddr, pucAddr, sizeof(pstServer->aucAddr));
    }

    if (usPort != 0)
    {
        pstServer->usPort = usPort;
    }
    else
    {
        pstServer->usPort = DTP_DEFAULT_SERVER_PORT;
    }

    pstServer->usInstID = usInstID;

    pstServer->ulReconnectTime = 10;
    pstServer->ulMaxSendErrNum = 1;
    pstServer->ulMaxRecvErrNum = 1;
    pstServer->ulRecvTimeout   = 1;
    pstServer->ulClientCheckInterval = 30;
    pstServer->ulRecvCheckTime = pstServer->ulClientCheckInterval*3;

    pstServer->hConnInfo = gos_mutex_init();
    if (!pstServer->hConnInfo)
    {
        DTPLog(LOG_FATAL, "dtp server init failed");
        goto fail;
    }

    for (i=0; i<ARRAY_SIZE(pstServer->astConnInfo); i++)
    {
        pstServer->astConnInfo[i].stSocket = INVALID_SOCKET;
    }

    gos_init_socket();

    FD_ZERO(&pstServer->stFdSet);

    pstServer->hMsgQueueSem = gos_sem_init();
    if (pstServer->hMsgQueueSem == INVALID_SEM)
    {
        goto fail;
    }

    pstServer->hMsgQueueMutex = gos_mutex_init();
    if (!pstServer->hMsgQueueMutex)
    {
        goto fail;
    }

    pstServer->hSyncMsgMutex = gos_mutex_init();
    if (!pstServer->hSyncMsgMutex)
    {
        DTPLog(LOG_FATAL, "dtp server init failed");
        goto fail;
    }

    pstServer->hMsgQueue = gos_list_init(ulMaxCtrlMsgBufSize, ulMaxCtrlMsgBufSize, sizeof(DTP_SERVER_MSG_NODE_T), NULL, NULL, NULL, NULL);
    if (!pstServer->hMsgQueue)
    {
        goto fail;
    }

    m_hMsgStatHash = gos_hashlist_init(64, 0, 200, sizeof(DTP_MSG_STAT_T), 0, gos_hash_int, gos_hash_key_cmp_int, gos_malloc, gos_free);
    if (!m_hMsgStatHash)
    {
        goto fail;
    }

    DTPLog(LOG_FATAL, "dtp tcp server init successful");

    m_pstServer = pstServer;

    return TRUE;

fail:
    Close();

    return FALSE;
}

DTPServerImp::DTPServerImp(DTPServerMsgHandler *pMsgHandle, GMemAlloc *pMemAlloc)
{
    m_bTCPMode = TRUE;
    m_pMsgHandle = pMsgHandle;
    m_pMemAlloc = pMemAlloc;
}

VOID DTPServerImp::Close()
{
    if (!m_pstServer)
    {
        return;
    }

    gos_mutex_free(m_pstServer->hMsgQueueMutex);
    gos_mutex_free(m_pstServer->hSeqIDMutex);

    gos_list_free((GLIST*)m_pstServer->hMsgQueue);

    gos_sem_free(m_pstServer->hMsgQueueSem);

    CLOSE_SOCKET(m_pstServer->stSocket);

    m_pstServer->enStatus = DTP_SERVER_CLOSE;

    gos_mutex_free(m_pstServer->hConnInfo);

    for (UINT32 i=0; i<ARRAY_SIZE(m_pstServer->astConnInfo); i++)
    {
        GOS_FREE(m_pstServer->astConnInfo[i].pucRecvBuf);
    }

    gos_hashlist_free(m_hMsgStatHash);

    free(m_pstServer);
}

/*******************************************************************************
* 函数名称: dtp_server_start
* 功    能: 循环侦听客户端消息
* 参    数:
* 参数名称              输入/输出   描述
* 函数返回:
* 说    明:
*******************************************************************************/
BOOL DTPServerImp::Start()
{
 //   INT32       iRecvMsgLen;
    UINT32      ulSleepTime = 1;
    UINT32      ulMaxSleepTime = 64;

    m_pstServer->enStatus = DTP_SERVER_INIT;
    while(!m_pstServer->bClose)
    {
        if (m_pstServer->stSocket == INVALID_SOCKET)
        {
            m_pstServer->enStatus = DTP_SERVER_INIT;
            m_pstServer->stSocket = CreateSocket();
            if (m_pstServer->stSocket != INVALID_SOCKET)
            {
                DTPLog(LOG_INFO, "tcp server_start: create socket(%d) successful", m_pstServer->stSocket);
            }
            else
            {
                if (ulSleepTime < ulMaxSleepTime)
                {
                    ulSleepTime *= 2;
                }

                gos_sleep(ulSleepTime);

                DTPLog(LOG_ERROR, "tcp server_start: create socket failed");

                continue;
            }
        }

        m_pstServer->enStatus = DTP_SERVER_WORK;
        ulSleepTime = 1;

        ListenMsg(m_pstServer->ulRecvTimeout);
    }

    CLOSE_SOCKET(m_pstServer->stSocket);

    return TRUE;
}

GOS_THREAD_RET DTPTCPServerThreadEntry(VOID *pvData)
{
    DTPServerImp    *pServer = (DTPServerImp *)pvData;

    pServer->Start();

    pServer->Close();

    return 0;
}

VOID DTPServerImp::Run()
{
    gos_create_thread(DTPTCPServerThreadEntry, 128*1024, this);
    gos_create_thread(DTPTCPServerSendThreadEntry, 128*1024, this);
}

BOOL DTPServerImp::IsRunning()
{
    BOOL                bRunning = FALSE;

    if (!m_pstServer)
    {
        return FALSE;
    }

    gos_mutex(m_pstServer->hMsgQueueMutex);

    if (m_pstServer->enStatus == DTP_SERVER_WORK)
    {
        bRunning = TRUE;
    }

    gos_unmutex(m_pstServer->hMsgQueueMutex);

    return bRunning;
}

DTPServerImp* DTPServerImp::New(UINT16 usInstID, UINT8 *pucAddr, UINT16 usPort,
                                 DTPServerMsgHandler *pMsgHandle,
                                 GMemAlloc *pMemAlloc)
{
    DTPServerImp    *pInstance = new DTPServerImp(pMsgHandle, pMemAlloc);

    if (!pInstance)
    {
        return NULL;
    }

    if (!pInstance->Init(usInstID, pucAddr, usPort))
    {
        delete pInstance;
        return NULL;
    }

    return pInstance;
}

DTPServerImp::~DTPServerImp()
{
    Close();
}

#undef DTPServerImp
