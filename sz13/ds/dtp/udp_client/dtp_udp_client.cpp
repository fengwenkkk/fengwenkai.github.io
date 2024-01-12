#include "g_include.h"

#include "dtp_public.h"
#include "dtp_common.h"
#include "dtp_func.h"
#include "dtp_udp_client.h"

extern GOS_THREAD_RET DTPUDPClientSendThreadEntry(VOID *pvData);

#define DTPClientImp    DTPUDPClient

VOID DTPClientImp::SetClientType(UINT8 ucClientType)
{
    if (!m_pstClient)
    {
        return;
    }

    m_pstClient->ucSenderType = ucClientType;
}

/*******************************************************************************
* 函数名称: dtp_client_init
* 功    能: 客户端初始化
* 参    数:
* 参数名称              输入/输出   描述
* paucServerIP          IN          服务器IP
* 函数返回:
* 说    明:
*******************************************************************************/
BOOL DTPClientImp::Init(DTP_CLIENT_CFG_T *pstClientCfg)
{
    DTP_CLIENT_INFO_T       *pstClient;
    UINT32                  ulMaxCtrlMsgBufSize = 64*1024;

    if (!pstClientCfg)
    {
        if (!pstClientCfg)
        {
            DTPLog(LOG_FATAL, "dtp_client_init pstClientCfg failed");
        }

        return FALSE;
    }

    pstClient = (DTP_CLIENT_INFO_T*)GOS_ZALLOC(sizeof(DTP_CLIENT_INFO_T));
    if (!pstClient)
    {
        DTPLog(LOG_FATAL, "dtp_client_init malloc pstClient failed");
        return FALSE;
    }

    /* 参数赋值 */
    pstClient->usServerID = pstClientCfg->usServerID;
    memcpy(pstClient->aucServerAddr, pstClientCfg->aucServerAddr, sizeof(pstClient->aucServerAddr));
    pstClient->usServerPort = pstClientCfg->usServerPort;
    if (0 == pstClient->usServerPort)
    {
        pstClient->usServerPort = DTP_DEFAULT_SERVER_PORT;
    }

    pstClient->ulLinkHelloInterval = pstClientCfg->ulLinkHelloInterval;
    if (0 == pstClient->ulLinkHelloInterval)
    {
        pstClient->ulLinkHelloInterval = 10;
    }

    pstClient->ulLinkHelloFailNum = pstClientCfg->ulLinkHelloFailNum;
    if (0 == pstClient->ulLinkHelloFailNum)
    {
        pstClient->ulLinkHelloFailNum = 6;
    }

    pstClient->ulMaxSocketErrNum = pstClientCfg->ulMaxSocketErrNum;
    if (0 == pstClient->ulMaxSocketErrNum)
    {
        pstClient->ulMaxSocketErrNum = 1;//3;
    }

    /* 其它初始化 */
    gos_init_socket();
    pstClient->stSocket = INVALID_SOCKET;

    pstClient->hLinkStatMutex = gos_mutex_init();
    if (!pstClient->hLinkStatMutex)
    {
        DTPLog(LOG_FATAL, "dtp_client_init LinkStatMutex failed");
        goto fail;
    }

    pstClient->hStatusMutex = gos_mutex_init();
    if (!pstClient->hStatusMutex)
    {
        DTPLog(LOG_FATAL, "dtp_client_init StatusMutex failed");
        goto fail;
    }
    pstClient->enClientStatus = DTP_CLIENT_IDLE;

    pstClient->hMsgQueueMutex = gos_mutex_init();
    if (!pstClient->hMsgQueueMutex)
    {
        DTPLog(LOG_FATAL, "dtp_client_init MsgQueueMutex failed");
        goto fail;
    }

    pstClient->hMsgQueue = gos_list_init(ulMaxCtrlMsgBufSize, ulMaxCtrlMsgBufSize, sizeof(DTP_CLIENT_MSG_NODE_T),
                                         NULL, NULL, NULL, NULL);
    if (!pstClient->hMsgQueue)
    {
        DTPLog(LOG_FATAL, "dtp_client_init MsgQueue failed");
        goto fail;
    }

    pstClient->hMsgQueueSem = gos_sem_init();
    if (pstClient->hMsgQueueSem == INVALID_SEM)
    {
        DTPLog(LOG_FATAL, "dtp_client_init MsgQueueSem failed");
        goto fail;
    }

    if (pstClientCfg->bAllowSyncMsg)
    {
        pstClient->hSyncMsgMutex = gos_mutex_init();
        if (!pstClient->hSyncMsgMutex)
        {
            DTPLog(LOG_FATAL, "dtp_client_init SyncMsgMutex failed");
            goto fail;
        }
    }

    pstClient->ucMsgPri = pstClientCfg->ucMsgPri;

    pstClient->hSeqIDMutex = gos_mutex_init();
    if (!pstClient->hSeqIDMutex)
    {
        DTPLog(LOG_FATAL, "dtp_client_init SeqIDMutex failed");
        goto fail;
    }

    pstClient->enRunStatus = THREAD_STATE_WORK;

    m_pstClient = pstClient;

    return TRUE;

fail:
    pstClient->enRunStatus = THREAD_STATE_STOP;
    Close();

    return FALSE;
}

VOID DTPClientImp::Close()
{
    if (!m_pstClient)
    {
        return;
    }

    Stop();

    gos_mutex_free(m_pstClient->hLinkStatMutex);
    gos_mutex_free(m_pstClient->hMsgQueueMutex);
    gos_mutex_free(m_pstClient->hSyncMsgMutex);
    gos_mutex_free(m_pstClient->hStatusMutex);
    gos_mutex_free(m_pstClient->hSeqIDMutex);

    gos_sem_free(m_pstClient->hMsgQueueSem);

    gos_list_free((GLIST*)m_pstClient->hMsgQueue);

    CLOSE_SOCKET(m_pstClient->stSocket);

    free(m_pstClient);
    m_pstClient = NULL;
}

BOOL DTPClientImp::Stop()
{
    if (!m_pstClient)
    {
        return FALSE;
    }

    if (m_pstClient->enRunStatus == THREAD_STATE_STOP)
    {
        return TRUE;
    }

    DTPLog(LOG_FATAL, "stop client");

    ReleaseLink();

    m_pstClient->enRunStatus = THREAD_STATE_STOPPING;

    CLOSE_SOCKET(m_pstClient->stSocket);

    while(m_pstClient->bSendThreadRunning ||
          m_pstClient->enRunStatus != THREAD_STATE_STOP )
    {
        gos_sleep_1ms(100);
    }

    m_pstClient->enRunStatus = THREAD_STATE_NULL;

    return TRUE;
}

BOOL DTPClientImp::IsConnected()
{
    BOOL                bConnected = FALSE;

    if (!m_pstClient)
    {
        return FALSE;
    }

    if (m_pstClient->enClientStatus == DTP_CLIENT_LINKSETUP )
    {
        bConnected = TRUE;
    }

    return bConnected;
}

BOOL AutoBindPort(SOCKET stSock, UINT8 *pucLocalAddr, UINT16 *pusLocalPort)
{
    SOCKADDR_IN     stLocalAddr = {0};
    UINT8           aucLocalAddr[4] = {0,0,0,0};
    UINT16          usLocalPort = 0;

    stLocalAddr.sin_family = AF_INET;
    memcpy(&stLocalAddr.sin_addr, aucLocalAddr, 4);

    while(1)
    {
        usLocalPort = gos_get_range_rand(53000, 63000);
        stLocalAddr.sin_port = HTONS(usLocalPort);

        if (bind (stSock, (SOCKADDR *) &stLocalAddr, sizeof(SOCKADDR)) != SOCKET_ERROR)
        {
            memcpy(pucLocalAddr, aucLocalAddr, 4);
            *pusLocalPort = usLocalPort;

            return TRUE;
        }
    }

//    return FALSE;
}

/*******************************************************************************
* 函数名称: Connect
* 功    能: 客户端与服务器建立连接
* 参    数:
* 参数名称              输入/输出   描述
* 函数返回:
* 说    明:
*******************************************************************************/
BOOL DTPClientImp::Connect()
{
    SOCKADDR_IN         stServerAddr = {0};
    UINT32              ulFlag = 1;
    SOCKET              stSocket;

    gos_init_socket();

    stServerAddr.sin_family = AF_INET;
    stServerAddr.sin_port   = HTONS(m_pstClient->usServerPort);
    memcpy(&stServerAddr.sin_addr.s_addr, m_pstClient->aucServerAddr, 4);

    CLOSE_SOCKET(m_pstClient->stSocket);
    m_pstClient->enClientStatus = DTP_CLIENT_IDLE;

    stSocket = socket (AF_INET, SOCK_DGRAM, 0);
    if (stSocket == INVALID_SOCKET)
    {
        DTPLog(LOG_ERROR, "client_connect: Create socket failed: %s", gos_get_socket_err());
        return FALSE;
    }

    if (setsockopt(stSocket, SOL_SOCKET, SO_REUSEADDR, (CHAR*)&ulFlag, sizeof(ulFlag)) == SOCKET_ERROR)
    {
        DTPLog(LOG_ERROR, "setsockopt failed, %s", gos_get_socket_err());
        CLOSE_SOCKET (stSocket);

        return FALSE;
    }

    if (!AutoBindPort(stSocket, m_pstClient->aucClientAddr, &m_pstClient->usClientPort))
    {
        DTPLog(LOG_ERROR, "bind socket failed!");
        CLOSE_SOCKET(stSocket);

        return FALSE;
    }

    DTPLog(LOG_INFO, "client bind socket at " IP_FMT ":%u",
            IP_ARG(m_pstClient->aucClientAddr), m_pstClient->usClientPort);

    m_pstClient->enClientStatus = DTP_CLIENT_CONNECTED;

    m_pstClient->ulLinkHelloReqNum = 0;

    m_pstClient->stSocket = stSocket;

    return TRUE;
}

/*******************************************************************************
* 函数名称: CloseConnect
* 功    能: 客户端断开与服务器的连接
* 参    数:
* 参数名称              输入/输出   描述
* 函数返回:
* 说    明:
*******************************************************************************/
VOID DTPClientImp::CloseConnect()
{
    if (!m_pstClient)
    {
        return;
    }

    ReleaseLink();
    CLOSE_SOCKET(m_pstClient->stSocket);

    m_pstClient->ulSocketErrNum = 0;
}

/*******************************************************************************
* 函数名称: RecvMsg
* 功    能: 客户端接收消息
* 参    数:
* 参数名称              输入/输出   描述
* 函数返回:
* 说    明:
*******************************************************************************/
BOOL DTPClientImp::RecvMsg()
{
    INT32               iTimeout = 1000;
    INT32               iRecvMsgHdrLen;
    UINT32              ulCurrTime;
    UINT32              ulRecvFailNum = 0;
    UINT32              ulMsgLen;
    UINT32              ulMsgBodyLen;
    INT32               iIdleTime;
    UINT32              ulMsgHdrLen = sizeof(DTP_MSG_HDR_T);
    DTP_MSG_HDR_T      *pstMsgHdr = &m_pstClient->stRecvMsgBuf.stMsgHdr;

    while (m_pstClient->enRunStatus != THREAD_STATE_STOPPING &&
           m_pstClient->enRunStatus != THREAD_STATE_STOP)
    {
        if (m_pstClient->enClientStatus == DTP_CLIENT_SUSPEND)
        {
            DTPLog(LOG_FATAL, "close connect due to suspend to server");
            break;
        }

        gos_mutex(m_pstClient->hLinkStatMutex);
        if (m_pstClient->ulLinkHelloReqNum > (UINT32)m_pstClient->ulLinkHelloFailNum)
        {
            gos_unmutex(m_pstClient->hLinkStatMutex);
            m_pstClient->enClientStatus = DTP_CLIENT_IDLE;
            DTPLog(LOG_FATAL, "close connect due to link hello failed");
            break;
        }

        ulCurrTime = gos_get_uptime();
        if (m_pstClient->ulLastSendTime > m_pstClient->ulLastRecvTime)
        {
            iIdleTime = ulCurrTime - m_pstClient->ulLastSendTime;
        }
        else
        {
            iIdleTime = ulCurrTime - m_pstClient->ulLastRecvTime;
        }

        if (iIdleTime >= (INT32)m_pstClient->ulLinkHelloInterval)
        {
            m_pstClient->ulLinkHelloReqNum++;
            gos_unmutex(m_pstClient->hLinkStatMutex);

            CheckLink();
        }
        else
        {
            gos_unmutex(m_pstClient->hLinkStatMutex);
        }

    //    iRecvMsgLen = 0;
        iRecvMsgHdrLen = dtp_udp_recv(m_pstClient->stSocket, pstMsgHdr, sizeof(m_pstClient->stRecvMsgBuf), iTimeout);

        if (iRecvMsgHdrLen > 0)
        {
            m_pstClient->ulLastRecvTime = gos_get_uptime();
        }

        // 超时
        if (0 == iRecvMsgHdrLen)
        {
            continue;
        }
        else if (iRecvMsgHdrLen < 0)
        {
            goto fail;
        }
        else if ((UINT32)iRecvMsgHdrLen < ulMsgHdrLen)
        {
            goto fail;
        }

        ulMsgLen = NTOHL(pstMsgHdr->ulMsgLen);
        if ((INT32)ulMsgLen != iRecvMsgHdrLen)
        {
            goto fail;
        }

        ulMsgBodyLen = ulMsgLen - ulMsgHdrLen;
        if (ulMsgBodyLen > DTP_MAX_MSG_LEN)
        {
            break;
        }

        goto ok;

fail:
        ulRecvFailNum ++;
        if (ulRecvFailNum < m_pstClient->ulMaxSocketErrNum)
        {
            continue;
        }
        else
        {
            DTPLog(LOG_ERROR, "client_recv_msg: too many recv error %u", ulRecvFailNum);
            break;
        }

ok:
        ulRecvFailNum = 0;

        HandleRecvMsg(&m_pstClient->stRecvMsgBuf);
    }

    gos_sleep_1ms(300);
    CLOSE_SOCKET(m_pstClient->stSocket);

    m_pstClient->ulSocketErrNum = 0;
    if (m_pstClient->enClientStatus != DTP_CLIENT_SUSPEND)
    {
        m_pstClient->enClientStatus = DTP_CLIENT_IDLE;
    }

    return TRUE;
}

VOID DTPClientImp::SetServer(UINT8 *pucServerAddr, UINT16 usServerPort)
{
    if (memcmp(m_pstClient->aucServerAddr, pucServerAddr, sizeof(m_pstClient->aucServerAddr)) == 0 &&
        m_pstClient->usServerPort == usServerPort)
    {
        return;
    }

    memcpy(m_pstClient->aucServerAddr, pucServerAddr, sizeof(m_pstClient->aucServerAddr));
    m_pstClient->usServerPort = usServerPort;

    CloseConnect();
    m_pstClient->enClientStatus = DTP_CLIENT_IDLE;
}

VOID DTPClientImp::GetServerAddr(UINT8 *pucAddr, UINT16 *pusPort)
{
    memcpy(pucAddr, m_pstClient->aucServerAddr, sizeof(m_pstClient->aucServerAddr));
    *pusPort = m_pstClient->usServerPort;
}

VOID DTPClientImp::GetClientAddr(UINT8 *pucAddr, UINT16 *pusPort)
{
    if (pucAddr)
    {
        memcpy(pucAddr, m_pstClient->aucClientAddr, sizeof(m_pstClient->aucClientAddr));
    }

    if (pusPort)
    {
        *pusPort = m_pstClient->usClientPort;
    }
}

BOOL DTPClientImp::MainLoop()
{
    switch(m_pstClient->enClientStatus)
    {
        case DTP_CLIENT_IDLE:
            if (Connect())
            {
                m_pstClient->enClientStatus = DTP_CLIENT_CONNECTED;

                return TRUE;
            }
            else
            {
                return FALSE;
            }

        case DTP_CLIENT_CONNECTED:
            if (AuthLink() )
            {
                m_pstClient->enClientStatus = DTP_CLIENT_LINKAUTHED;

                m_pMsgHandle->OnConnectServer(m_pstClient->usServerID);
                return TRUE;
            }
            else
            {
                CloseConnect();
                m_pstClient->enClientStatus = DTP_CLIENT_IDLE;
                return FALSE;
            }

        case DTP_CLIENT_LINKAUTHED:
            RecvMsg();
            m_pMsgHandle->OnDisconnectServer(m_pstClient->usServerID);
            return TRUE;

        case DTP_CLIENT_SUSPEND:
            return TRUE;

        default:
            m_pstClient->enClientStatus = DTP_CLIENT_IDLE;
            return FALSE;
    }
}

/*******************************************************************************
* 函数名称: Suspend
* 功    能:
* 参    数:
* 参数名称              输入/输出   描述
* 函数返回:
* 说    明:
*******************************************************************************/
VOID DTPClientImp::Suspend()
{
    m_pstClient->enClientStatus = DTP_CLIENT_SUSPEND;
}

VOID DTPClientImp::Resume()
{
    m_pstClient->enClientStatus = DTP_CLIENT_IDLE;
}

VOID DTPClientImp::Start()
{
    while(1)
    {
        while (m_pstClient->enRunStatus != THREAD_STATE_STOP &&
               m_pstClient->enRunStatus != THREAD_STATE_STOPPING )
        {
            if (!MainLoop())
            {
                gos_sleep_1ms(1000);
            }
        }

        break;
#if 0
        m_pstClient->enRunStatus = THREAD_STATE_STOP;

        while (m_pstClient->enRunStatus != THREAD_STATE_WORK)
        {
            gos_sleep_1ms(100);
        }
#endif
    }

    while (m_pstClient->bSendThreadRunning)
    {
        gos_sleep_1ms(10);
    }

    m_pstClient->enRunStatus = THREAD_STATE_STOP;
}

/*******************************************************************************
* 函数名称: DTPUDPClientThreadEntry
* 功    能: 客户端线程入口
* 参    数:
* 参数名称              输入/输出   描述
* 函数返回:
* 说    明:
*******************************************************************************/
GOS_THREAD_RET DTPUDPClientThreadEntry(VOID *pvData)
{
    DTPClientImp        *pDTPClient = (DTPClientImp*)pvData;

    pDTPClient->Start();

    return 0;
}

#if 0
/*******************************************************************************
* 函数名称: SetMsgDesc
* 功    能: 设置消息描述信息
* 参    数:
* 参数名称          输入/输出       描述
* 函数返回:
* 说    明:
*******************************************************************************/
BOOL DTPClientImp::SetMsgDesc(UINT16 usMsgType, UINT32 ulMsgLen, BOOL bNeedAck)
{
    UINT32              i;

    for (i=0; i<m_pstClient->ulMsgDescNum; i++)
    {
        if (m_pstClient->astMsgDesc[i].usMsgType == usMsgType)
        {
            m_pstClient->astMsgDesc[i].ulMsgLen = ulMsgLen;
            m_pstClient->astMsgDesc[i].bNeedAck = bNeedAck;
            return TRUE;
        }
    }

    if (m_pstClient->ulMsgDescNum >= ARRAY_SIZE(m_pstClient->astMsgDesc))
    {
        return FALSE;
    }

    m_pstClient->astMsgDesc[m_pstClient->ulMsgDescNum].usMsgType = usMsgType;
    m_pstClient->astMsgDesc[m_pstClient->ulMsgDescNum].ulMsgLen  = ulMsgLen;
    m_pstClient->astMsgDesc[m_pstClient->ulMsgDescNum].bNeedAck  = bNeedAck;
    m_pstClient->ulMsgDescNum++;

    return TRUE;
}

/*******************************************************************************
* 函数名称: GetMsgDesc
* 功    能: 获取消息描述信息
* 参    数:
* 参数名称          输入/输出       描述
* 函数返回:
* 说    明:
*******************************************************************************/
DTP_CLIENT_MSG_DESC_T * DTPClientImp::GetMsgDesc(UINT16 usMsgType)
{
    UINT32              i;

    if (!m_pstClient)
    {
        return NULL;
    }

    for (i=0; i<m_pstClient->ulMsgDescNum; i++)
    {
        if (m_pstClient->astMsgDesc[i].usMsgType == usMsgType)
        {
            return &m_pstClient->astMsgDesc[i];
        }
    }

    return NULL;
}
#endif

VOID DTPClientImp::Run()
{
    gos_create_thread(DTPUDPClientThreadEntry, 128*1024, this);
    gos_create_thread(DTPUDPClientSendThreadEntry, 128*1024, this);
}

DTPClientImp::DTPClientImp(DTPClientMsgHandler *pMsgHandle,  GMemAlloc *pMemAlloc)
{
    m_bTCPMode = FALSE;

    m_pMsgHandle = pMsgHandle;
    m_pMemAlloc = pMemAlloc;

    m_pMsgHandle->SetDTPClient(this);
}

DTPClientImp* DTPClientImp::New(DTP_CLIENT_CFG_T *pstClientCfg, DTPClientMsgHandler *pMsgHandle, GMemAlloc *pMemAlloc)
{
    DTPClientImp    *pInstance = new DTPClientImp(pMsgHandle, pMemAlloc);

    if (!pInstance)
    {
        return NULL;
    }

    if (!pInstance->Init(pstClientCfg))
    {
        delete pInstance;
        return NULL;
    }

    return pInstance;
}

DTPClientImp::~DTPClientImp()
{
    Close();
}

#undef DTPClientImp
