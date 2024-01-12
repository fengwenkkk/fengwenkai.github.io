#include "g_include.h"

#include "dtp_public.h"
#include "dtp_common.h"
#include "dtp_func.h"
#include "dtp_tcp_client.h"

extern GOS_THREAD_RET DTPTCPClientSendThreadEntry(VOID *pvData);

#define DTPClientImp    DTPTCPClient

VOID DTPClientImp::SetClientType(UINT8 ucClientType)
{
    if (!m_pstClient)
    {
        return;
    }

    m_pstClient->ucSenderType = ucClientType;
}

/*******************************************************************************
* ��������: dtp_client_init
* ��    ��: �ͻ��˳�ʼ��
* ��    ��:
* ��������              ����/���   ����
* paucServerIP          IN          ������IP
* ��������:
* ˵    ��:
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

    /* ������ֵ */
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

    /* ������ʼ�� */
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

/*******************************************************************************
* ��������: Connect
* ��    ��: �ͻ������������������
* ��    ��:
* ��������              ����/���   ����
* ��������:
* ˵    ��:
*******************************************************************************/
BOOL DTPClientImp::Connect()
{
    SOCKADDR_IN         stServerAddr = {0};
    UINT32              ulTimeout = 0;
    UINT32              ulFlag = 1;
    SOCKET              stSocket;
    INT32               iNoDelay = 1;

    gos_init_socket();

    stServerAddr.sin_family = PF_INET;
    stServerAddr.sin_port   = HTONS(m_pstClient->usServerPort);
    memcpy(&stServerAddr.sin_addr.s_addr, m_pstClient->aucServerAddr, 4);

    CLOSE_SOCKET(m_pstClient->stSocket);
    m_pstClient->enClientStatus = DTP_CLIENT_IDLE;

    stSocket = socket (PF_INET, SOCK_STREAM, 0);
    if (stSocket == INVALID_SOCKET)
    {
        DTPLog(LOG_ERROR, "client_connect: Create socket failed: %s", gos_get_socket_err());
        return FALSE;
    }

    if (setsockopt(stSocket, SOL_SOCKET, SO_REUSEADDR, (CHAR*)&ulFlag, sizeof(ulFlag)) == SOCKET_ERROR)
    {
        DTPLog(LOG_ERROR, "setsockopt failed, %s", gos_get_socket_err());
        CLOSE_SOCKET(stSocket);

        return FALSE;
    }

    DTPLog(LOG_INFO, "client_connect: client(%d-" IP_FMT "-%d) is connecting ...",
            m_pstClient->usClientID, IP_ARG(m_pstClient->aucServerAddr), m_pstClient->usServerPort);

    if (gos_connect(stSocket, (SOCKADDR *)&stServerAddr, sizeof (SOCKADDR), ulTimeout) == SOCKET_ERROR)
    {
        DTPLog(LOG_ERROR, "client_connect: connect to " IP_FMT  "(%d) failed, %s",
                 IP_ARG(m_pstClient->aucServerAddr), m_pstClient->usServerPort, gos_get_socket_err());
        CLOSE_SOCKET(stSocket);

        return FALSE;
    }

    setsockopt(stSocket, IPPROTO_TCP, TCP_NODELAY, (CHAR*)&iNoDelay, sizeof(iNoDelay));

    m_pstClient->enClientStatus = DTP_CLIENT_CONNECTED;
    DTPLog(LOG_FATAL, "client_connect: tcp client connected to server, socket is %u", stSocket);

    m_pstClient->ulLinkHelloReqNum = 0;

    m_pstClient->stSocket = stSocket;

    return TRUE;
}

/*******************************************************************************
* ��������: CloseConnect
* ��    ��: �ͻ��˶Ͽ��������������
* ��    ��:
* ��������              ����/���   ����
* ��������:
* ˵    ��:
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
* ��������: RecvMsg
* ��    ��: �ͻ��˽�����Ϣ
* ��    ��:
* ��������              ����/���   ����
* ��������:
* ˵    ��:
*******************************************************************************/
BOOL DTPClientImp::RecvMsg()
{
    INT32               iTimeout = 1000;
    INT32               iRecvMsgHdrLen;
    INT32               iRecvMsgLen;
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

        iRecvMsgLen = 0;
        iRecvMsgHdrLen = dtp_tcp_recv(m_pstClient->stSocket, pstMsgHdr, ulMsgHdrLen, iTimeout);

        if (iRecvMsgHdrLen > 0)
        {
            m_pstClient->ulLastRecvTime = gos_get_uptime();
        }

        // ��ʱ
        if (0 == iRecvMsgHdrLen)
        {
            continue;
        }
        else if (iRecvMsgHdrLen < 0)
        {
            goto fail;
        }
        else if ((UINT32)iRecvMsgHdrLen != ulMsgHdrLen)
        {
            continue;
        }

        ulMsgLen = NTOHL(pstMsgHdr->ulMsgLen);
        if (ulMsgLen < ulMsgHdrLen)
        {
            break;
        }

        ulMsgBodyLen = ulMsgLen - ulMsgHdrLen;
        if (ulMsgBodyLen > DTP_MAX_MSG_LEN)
        {
            break;
        }

        if (ulMsgBodyLen > 0)
        {
            iRecvMsgLen = dtp_tcp_recv(m_pstClient->stSocket, m_pstClient->stRecvMsgBuf.aucMsgBody, ulMsgBodyLen, iTimeout);
            if (iRecvMsgLen > 0)
            {
                m_pstClient->ulLastRecvTime = gos_get_uptime();
            }

            // ��ʱ
            if (0 == iRecvMsgLen)
            {
                continue;
            }
            else if (iRecvMsgLen < 0)
            {
                goto fail;
            }
            else if ((UINT32)iRecvMsgLen != ulMsgBodyLen)
            {
                DTPLog(LOG_ERROR, "client_recv_msg: recv msg(%u) len(%d) should be %d", NTOHS(pstMsgHdr->usMsgType), iRecvMsgLen, ulMsgBodyLen);
                continue;
            }
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
* ��������: Suspend
* ��    ��:
* ��    ��:
* ��������              ����/���   ����
* ��������:
* ˵    ��:
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
    }

    while (m_pstClient->bSendThreadRunning)
    {
        gos_sleep_1ms(10);
    }

    m_pstClient->enRunStatus = THREAD_STATE_STOP;
}

/*******************************************************************************
* ��������: DTPClientThreadEntry
* ��    ��: �ͻ����߳����
* ��    ��:
* ��������              ����/���   ����
* ��������:
* ˵    ��:
*******************************************************************************/
GOS_THREAD_RET DTPTCPClientThreadEntry(VOID *pvData)
{
    DTPClientImp        *pDTPClient = (DTPClientImp*)pvData;

    pDTPClient->Start();

    return 0;
}

VOID DTPClientImp::Run()
{
    gos_create_thread(DTPTCPClientThreadEntry, 128*1024, this);
    gos_create_thread(DTPTCPClientSendThreadEntry, 128*1024, this);
}

DTPClientImp::DTPClientImp(DTPClientMsgHandler *pMsgHandle,  GMemAlloc *pMemAlloc)
{
    m_bTCPMode = TRUE;

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
