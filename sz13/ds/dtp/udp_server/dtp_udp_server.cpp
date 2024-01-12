#include "g_include.h"

#include "dtp_public.h"
#include "dtp_common.h"
#include "dtp_func.h"
#include "dtp_udp_server.h"

extern GOS_THREAD_RET DTPUDPServerSendThreadEntry(VOID *pvData);

VOID _CloseSocket(DTP_SERVER_INFO_T *pstServer, SOCKET *pstSocket);

// 接收消息后对消息的核查结果
#define DTP_MSG_CHECK_SUCCESS               0
#define DTP_MSG_CHECK_HDR_INCOMPLETE        1
#define DTP_MSG_CHECK_MSG_INCOMPLETE        2
#define DTP_MSG_CHECK_LEN_ERROR             3

#define DTPServerImp    DTPUDPServer

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
    }
}

/*******************************************************************************
* 函数名称: CheckRecvMsg
* 功    能: 处理客户端的请求消息
* 参    数:
* 参数名称              输入/输出   描述
* stSocket              IN          客户端socket
* pucMsg                IN          客户端请求消息
* ulTotalMsgLen         IN          接收到的消息长度
* pulMsgSize            OUT         本次处理的消息长度
* 函数返回:
* 说    明:
*******************************************************************************/
UINT32 DTPServerImp::CheckRecvMsg(DTP_CONN_INFO_T *pstConnInfo, UINT8 *pucMsg, UINT32 ulTotalMsgLen)
{
    DTP_MSG_HDR_T   *pstMsgHdr;
    UINT32          ulMsgSize;

    if (ulTotalMsgLen < sizeof(DTP_MSG_HDR_T))
    {
        DTPLog(LOG_ERROR, "server_check_recv_msg: recv msg length(%u) is too short!", ulTotalMsgLen);

        return DTP_MSG_CHECK_HDR_INCOMPLETE;
    }

    /* 字节顺序转换 */
    pstMsgHdr = (DTP_MSG_HDR_T *)pucMsg;
    ulMsgSize = NTOHL(pstMsgHdr->ulMsgLen);

    if (ulMsgSize < sizeof(DTP_MSG_HDR_T))
    {
        DTPLog(LOG_ERROR, "server_check_recv_msg: msg length(%u) is too short!", ulMsgSize);

        return DTP_MSG_CHECK_LEN_ERROR;
    }

    if (ulMsgSize > sizeof(DTP_MSG_T) )
    {
        DTPLog(LOG_ERROR, "server_check_recv_msg: msg length(%u) is too long!", ulMsgSize);

        return DTP_MSG_CHECK_LEN_ERROR;
    }

    if (ulTotalMsgLen != ulMsgSize)
    {
        DTPLog(LOG_ERROR, "server_check_recv_msg: msg length(%u) shoule be %u, msg ID is %u", ulTotalMsgLen, ulMsgSize, NTOHS(pstMsgHdr->usMsgType));
        return DTP_MSG_CHECK_MSG_INCOMPLETE;
    }

    dtp_msg_ntoh(pstMsgHdr);
    if (pstMsgHdr->ulMsgMagic != DTP_MSG_MAGIC ||
        pstMsgHdr->usMsgVer != DTP_MSG_VER)
    {
        DTPLog(LOG_ERROR, "server_check_recv_msg: invalid msg magic or msg ver!");

        return DTP_MSG_CHECK_HDR_INCOMPLETE;
    }

    /* 消息处理 */
    HandleClientMsg(pstConnInfo, (DTP_MSG_T *)pucMsg);

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
static DTP_CONN_INFO_T* _GetConnectInfo(DTP_SERVER_INFO_T *pstServer, SOCKADDR_IN *pstSocketAddr)
{
    UINT32          i;

    for (i=0; i<ARRAY_SIZE(pstServer->astConnInfo); i++)
    {
        if (CmpSockAddr(&pstServer->astConnInfo[i].stClientSendAddr, pstSocketAddr))
        {
            DTP_CONN_INFO_T *pstConnInfo = &pstServer->astConnInfo[i];

            return pstConnInfo;
        }
    }

    return NULL;
}

VOID DTPServerImp::_InitConnectInfo(DTP_CONN_INFO_T *pstConn, BOOL bNeedMutex)
{
    if (bNeedMutex && !gos_mutex(m_pstServer->hConnInfo))
    {
        DTPLog(LOG_ERROR, "mutex failed");
        return;
    }

    memset(pstConn, 0, sizeof(DTP_CONN_INFO_T));
    pstConn->enClientStatus = DTP_CLIENT_IDLE;

    if (bNeedMutex)
    {
        gos_unmutex(m_pstServer->hConnInfo);
    }
}

/*******************************************************************************
* 函数名称: _SetConnectInfo
* 功    能: 设置客户端连接信息
* 参    数:
* 参数名称              输入/输出   描述
* pstAddr               IN          客户端地址
* 函数返回:
* 说    明:
*******************************************************************************/
DTP_CONN_INFO_T* DTPServerImp::_SetConnectInfo(SOCKADDR_IN *pstAddr)
{
    UINT32              i;
    UINT32              ulCurrTime = gos_get_uptime();
    DTP_CONN_INFO_T    *pstTmpInfo;
    DTP_CONN_INFO_T    *pstFreeConnInfo = NULL;

    if (!gos_mutex(m_pstServer->hConnInfo))
    {
        DTPLog(LOG_ERROR, "mutex failed");
        return NULL;
    }

    for (i=0; i<ARRAY_SIZE(m_pstServer->astConnInfo); i++)
    {
        pstTmpInfo = &m_pstServer->astConnInfo[i];
        if (pstTmpInfo->stClientSendAddr.sin_port != 0)
        {
            if (CmpSockAddr(&pstTmpInfo->stClientSendAddr, pstAddr) )
            {
                _InitConnectInfo(pstTmpInfo, FALSE);

                pstFreeConnInfo = pstTmpInfo;

                break;
            }
            else if ((ulCurrTime - pstTmpInfo->ulLastRecvTime) > m_pstServer->ulRecvCheckTime)
            {
                DTPLog(LOG_INFO, "close connect with " IP_FMT ":%u by link check failed",
                    IP_ARG(&pstTmpInfo->stClientSendAddr.sin_addr.s_addr), NTOHS(pstTmpInfo->stClientSendAddr.sin_port));
                _InitConnectInfo(pstTmpInfo, FALSE);
            }
        }

        if (NULL == pstFreeConnInfo &&
            pstTmpInfo->stClientSendAddr.sin_port == 0)
        {
            pstFreeConnInfo = pstTmpInfo;
        }
    }

    if (!pstFreeConnInfo)
    {
        DTPLog(LOG_ERROR, "no free connection to be alloced");

        gos_unmutex(m_pstServer->hConnInfo);
        return NULL;
    }

    pstFreeConnInfo->usClientID = DTP_INVALID_INST_ID;
    if (!AllocClientID(pstFreeConnInfo))
    {
        DTPLog(LOG_INFO, "alloc client id(%u) failed", pstFreeConnInfo->stSocket);

        _InitConnectInfo(pstFreeConnInfo, FALSE);
        gos_unmutex(m_pstServer->hConnInfo);
        return FALSE;
    }

    pstFreeConnInfo->usClientRecvPort = NTOHS(pstAddr->sin_port);
    pstFreeConnInfo->stClientSendAddr = *pstAddr;
    pstFreeConnInfo->ulConnectSetupTime = ulCurrTime;
    pstFreeConnInfo->enClientStatus = DTP_CLIENT_CONNECTED;
    pstFreeConnInfo->ulLastRecvTime = gos_get_uptime();

    gos_unmutex(m_pstServer->hConnInfo);

    DTPLog(LOG_INFO, "add new connect with " IP_FMT ":%u",
                        IP_ARG(&pstAddr->sin_addr.s_addr), NTOHS(pstAddr->sin_port));

    return pstFreeConnInfo;
}

VOID DTPServerImp::PrintConnect() {}

/*******************************************************************************
* 函数名称: RecvMsg
* 功    能: 接收客户端消息
* 参    数:
* 参数名称              输入/输出   描述
* stSocket              IN          客户端socket
* piErrCode             OUT         错误码
* 函数返回:
* 说    明:
*******************************************************************************/
INT32 DTPServerImp::RecvMsg(DTP_CONN_INFO_T *pstConnInfo, INT32 *piErrCode)
{
    return -1;
}

/*******************************************************************************
* 函数名称: ListenMsg
* 功    能: 侦听UDP消息
* 参    数:
* 参数名称              输入/输出   描述
* piErrCode             OUT         错误码
* 函数返回:
* 说    明:
*******************************************************************************/
INT32 DTPServerImp::ListenMsg(UINT32 ulTimeout)
{
    DTP_CONN_INFO_T *pstConnInfo;
    CHAR        *szRecvBuf = (CHAR*)&m_pstServer->stRecvMsgBuf;
    UINT32      ulRecvBufSize = sizeof(m_pstServer->stRecvMsgBuf);
    SOCKADDR_IN stClientAddr = {0};


    INT32       iRecvSize = gos_recv_udp_msg(m_pstServer->stSocket, szRecvBuf, ulRecvBufSize, ulTimeout*1000, &stClientAddr);

    if (iRecvSize < 0)
    {
        return -1;
    }
    else if (iRecvSize == 0)
    {
        return 0;
    }

    pstConnInfo = _GetConnectInfo(m_pstServer, &stClientAddr);
    if (!pstConnInfo)
    {
        pstConnInfo = _SetConnectInfo(&stClientAddr);
        if (!pstConnInfo)
        {
            DTPLog(LOG_ERROR, "server_recv_msg: set client(" IP_FMT ":%u) conn info failed",
                IP_ARG(&stClientAddr.sin_addr.s_addr), NTOHS(stClientAddr.sin_port));
            return -1;
        }
    }

    UINT32 ulRet = CheckRecvMsg(pstConnInfo, (UINT8*)szRecvBuf, iRecvSize);

    if (ulRet == DTP_MSG_CHECK_SUCCESS) /* 提取消息成功 */
    {
        return iRecvSize;
    }
    else
    {
        return -1;
    }
}

/*******************************************************************************
* 函数名称: CreateSocket
* 功    能: 创建UDP socket
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

    stSocket = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);
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
        DTPLog(LOG_ERROR, "udp server_create_socket: bind socket failed, %s", gos_get_socket_err());
        CLOSE_SOCKET (stSocket);

        return INVALID_SOCKET;
    }

    return stSocket;
}

BOOL DTPServerImp::Init(UINT16 usInstID, UINT8 *pucAddr, UINT16 usPort)
{
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

    DTPLog(LOG_FATAL, "dtp udp server init successful");

    m_pstServer = pstServer;

    return TRUE;

fail:
    Close();

    return FALSE;
}

DTPServerImp::DTPServerImp(DTPServerMsgHandler *pMsgHandle, GMemAlloc *pMemAlloc)
{
    m_bTCPMode = FALSE;
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
                DTPLog(LOG_INFO, "server_start: create socket(%d) successful", m_pstServer->stSocket);
            }
            else
            {
                if (ulSleepTime < ulMaxSleepTime)
                {
                    ulSleepTime *= 2;
                }

                gos_sleep(ulSleepTime);

                DTPLog(LOG_ERROR, "udp server_start: create socket failed");

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

GOS_THREAD_RET DTPUDPServerThreadEntry(VOID *pvData)
{
    DTPServerImp    *pServer = (DTPServerImp *)pvData;

    pServer->Start();

    pServer->Close();

    return 0;
}

VOID DTPServerImp::Run()
{
    gos_create_thread(DTPUDPServerThreadEntry, 128*1024, this);
    gos_create_thread(DTPUDPServerSendThreadEntry, 128*1024, this);
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
