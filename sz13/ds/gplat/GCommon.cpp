#include "GPublic.h"
#include "DTPServer.h"
#include "DTPClient.h"

extern VOID GosSetExitWhenMsgQueueIsFull(BOOL bExitWhenMsgQueueIsFull);

class GServerMsgHandler:public DTPServerMsgHandler
{
public:
    virtual VOID MsgHandle(UINT16 usClientID, DTP_MSG_T *pstMsg);
    virtual VOID OnAcceptClient(UINT16 usClientID);
    virtual VOID OnDisconnectClient(UINT16 usClientID);
};

class GClientMsgHandler:public DTPClientMsgHandler
{
public:
    virtual VOID MsgHandle(UINT16 usServerID, DTP_MSG_T *pstMsg);
    virtual VOID OnConnectServer(UINT16 usServerID);
    virtual VOID OnDisconnectServer(UINT16 usServerID);
};

typedef struct
{
    DTPClient               *pDTPClient;
    PEER_ADDR_T             stServerAddr;
    UINT16                  usServerID;
}GOS_DTP_CLIENT_INFO_T;

#define MAX_SERVER_ID       255

static UINT8                g_aucServerID[MAX_SERVER_ID];   // 客户端连接的服务器ID
static UINT16               g_usInstID = INVALID_INST_ID;   // 本服务器ID
static DTPServer            *m_pDTPTCPServer = NULL;
static DTPServer            *m_pDTPUDPServer = NULL;
static VECTOR<GOS_DTP_CLIENT_INFO_T>    m_vDTPClient;

static GServerMsgHandler    *m_pServerMsgHandle = NULL;
static GClientMsgHandler    *m_pClientMsgHandle = NULL;

static GMemAlloc            *m_pDTPMemAlloc = NULL;

extern VOID SocketMsgHandle(MSG_T *pstMsgHdr, VOID *pvMsgBody, UINT32 ulMsgBodyLen);

UINT32 GosGetMaxMsgSize()
{
    return DTP_MAX_MSG_LEN;
}

UINT32 GosGetSafeMsgSize()
{
    UINT32  ulSize = GosGetMaxMsgSize();

    ulSize -= 2*1024;

    return ulSize;
}

VOID GServerMsgHandler::MsgHandle(UINT16 usClientID, DTP_MSG_T *pstRecvMsg)
{
    UINT32          ulMsgLen;
    MSG_T           stMsg = {0};

    if (pstRecvMsg->stMsgHdr.ulMsgLen < sizeof(DTP_MSG_HDR_T))
    {
        return;
    }

    ulMsgLen = pstRecvMsg->stMsgHdr.ulMsgLen - sizeof(DTP_MSG_HDR_T);
    stMsg.usMsgID = pstRecvMsg->stMsgHdr.usMsgType;
    stMsg.ulTimeout = pstRecvMsg->stMsgHdr.usTimeout;

    stMsg.stSender.usInstID = usClientID;
    stMsg.stSender.usTaskID = pstRecvMsg->stMsgHdr.usSenderModule; //INVALID_TASK_ID;
    stMsg.stReceiver.usInstID = LOCAL_INST_ID;
    stMsg.stReceiver.usTaskID = pstRecvMsg->stMsgHdr.usReceiverModule;//INVALID_TASK_ID;
    stMsg.ulSeqID = pstRecvMsg->stMsgHdr.ulSeqID;
    stMsg.ulMsgLen = ulMsgLen;
    stMsg.ucMsgPri = pstRecvMsg->stMsgHdr.ucMsgPri;
    stMsg.ucSenderType = pstRecvMsg->stMsgHdr.ucSenderType;

    SocketMsgHandle(&stMsg, pstRecvMsg->aucMsgBody, ulMsgLen);
}

VOID GServerMsgHandler::OnAcceptClient(UINT16 usClientID)
{
    SERVER_ACCEPT_CLIENT_T  stMsg = {0};

    stMsg.stMsgHdr.stSender.usInstID = LOCAL_INST_ID;
    stMsg.stMsgHdr.stSender.usTaskID = INVALID_TASK_ID;

    stMsg.stMsgHdr.stReceiver.usInstID = LOCAL_INST_ID;
    stMsg.stMsgHdr.stReceiver.usTaskID = INVALID_TASK_ID;

    stMsg.stMsgHdr.usMsgID = EV_SERVER_ACCEPT_CLIENT_IND;
    stMsg.stMsgHdr.ulTimeout = 0;
    stMsg.stMsgHdr.ulMsgLen = sizeof(stMsg) - STRUCT_OFFSET(SERVER_ACCEPT_CLIENT_T, usClientID);
    stMsg.stMsgHdr.ucMsgPri = MSG_PRI_IMP;

    stMsg.usClientID = usClientID;

    GosDispatchSystemMsg((MSG_T*)&stMsg.stMsgHdr);
}

VOID GServerMsgHandler::OnDisconnectClient(UINT16 usClientID)
{
    SERVER_DISCONNECT_CLIENT_T  stMsg = {0};

    stMsg.stMsgHdr.stSender.usInstID = LOCAL_INST_ID;
    stMsg.stMsgHdr.stSender.usTaskID = INVALID_TASK_ID;

    stMsg.stMsgHdr.stReceiver.usInstID = LOCAL_INST_ID;
    stMsg.stMsgHdr.stReceiver.usTaskID = INVALID_TASK_ID;

    stMsg.stMsgHdr.usMsgID = EV_SERVER_DISCONNECT_CLIENT_IND;
    stMsg.stMsgHdr.ulTimeout = 0;
    stMsg.stMsgHdr.ulMsgLen = sizeof(stMsg) - STRUCT_OFFSET(SERVER_DISCONNECT_CLIENT_T, usClientID);
    stMsg.stMsgHdr.ucMsgPri = MSG_PRI_IMP;

    stMsg.usClientID = usClientID;

    GosDispatchSystemMsg((MSG_T*)&stMsg.stMsgHdr);
}

BOOL _InitDTPMemAlloc()
{
    GOS_MEM_POOL_T  astMsgPool[] = {{64*1024, 64}};

    if (!m_pDTPMemAlloc)
    {
        m_pDTPMemAlloc = GMemPool::GetInstance(&astMsgPool[0], ARRAY_SIZE(astMsgPool), TRUE, TRUE);
        if (!m_pDTPMemAlloc)
        {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL _InitDTPTCPServer(UINT16 usServerID, UINT8 *pucServerAddr, UINT16 usServerPort)
{
    if (!m_pServerMsgHandle)
    {
        m_pServerMsgHandle = new GServerMsgHandler();
        if (!m_pServerMsgHandle)
        {
            return FALSE;
        }
    }

    m_pDTPTCPServer = DTPServer::New(usServerID, pucServerAddr, usServerPort, m_pServerMsgHandle, m_pDTPMemAlloc, TRUE);
    if (!m_pDTPTCPServer)
    {
        delete m_pServerMsgHandle;
        return FALSE;
    }

    return TRUE;
}

BOOL _InitDTPUDPServer(UINT16 usServerID, UINT8 *pucServerAddr, UINT16 usServerPort)
{
    if (!m_pServerMsgHandle)
    {
        m_pServerMsgHandle = new GServerMsgHandler();
        if (!m_pServerMsgHandle)
        {
            return FALSE;
        }
    }

    m_pDTPUDPServer = DTPServer::New(usServerID, pucServerAddr, usServerPort, m_pServerMsgHandle, m_pDTPMemAlloc, FALSE);
    if (!m_pDTPUDPServer)
    {
        delete m_pServerMsgHandle;
        return FALSE;
    }

    return TRUE;
}

BOOL GosGetUDPPort(UINT16 usTCPPort)
{
    return usTCPPort;
}

BOOL _InitDTPServer(UINT16 usServerID, UINT8 *pucAddr, UINT16 usPort)
{
    UINT16  usUDPPort = GosGetUDPPort(usPort);

    return _InitDTPTCPServer(usServerID, pucAddr, usPort) &&
           _InitDTPUDPServer(usServerID, pucAddr, usUDPPort);
}

VOID _RunDTPServer()
{
    if (m_pDTPTCPServer)
    {
        m_pDTPTCPServer->Run();
    }

    if (m_pDTPUDPServer)
    {
        m_pDTPUDPServer->Run();
    }
}

VOID _FreeDTPServer()
{
    if (m_pDTPTCPServer)
    {
        delete m_pDTPTCPServer;
        m_pDTPTCPServer = NULL;
    }

    if (m_pDTPUDPServer)
    {
        delete m_pDTPUDPServer;
        m_pDTPUDPServer = NULL;
    }
}

VOID GClientMsgHandler::MsgHandle(UINT16 usServerID, DTP_MSG_T *pstMsg)
{
    UINT32          ulMsgLen;
    MSG_T           stMsg = {0};

    if (pstMsg->stMsgHdr.ulMsgLen < sizeof(DTP_MSG_HDR_T))
    {
        return;
    }

    ulMsgLen = pstMsg->stMsgHdr.ulMsgLen - sizeof(DTP_MSG_HDR_T);
    stMsg.usMsgID = pstMsg->stMsgHdr.usMsgType;
    stMsg.ulTimeout = pstMsg->stMsgHdr.usTimeout;

    stMsg.stSender.usInstID = usServerID;
    stMsg.stSender.usTaskID = pstMsg->stMsgHdr.usSenderModule;// INVALID_TASK_ID;
    stMsg.stReceiver.usInstID = LOCAL_INST_ID;
    stMsg.stReceiver.usTaskID = pstMsg->stMsgHdr.usReceiverModule; //INVALID_TASK_ID;
    stMsg.ulSeqID = pstMsg->stMsgHdr.ulSeqID;
    stMsg.ulMsgLen = ulMsgLen;
    stMsg.ucMsgPri = pstMsg->stMsgHdr.ucMsgPri;
    stMsg.ucSenderType = pstMsg->stMsgHdr.ucSenderType;

    SocketMsgHandle(&stMsg, pstMsg->aucMsgBody, ulMsgLen);
}

VOID GClientMsgHandler::OnConnectServer(UINT16 usServerID)
{
    CLIENT_CONNECT_SERVER_T     stMsg = {0};

    stMsg.stMsgHdr.stSender.usInstID = LOCAL_INST_ID;
    stMsg.stMsgHdr.stSender.usTaskID = INVALID_TASK_ID;

    stMsg.stMsgHdr.stReceiver.usInstID = LOCAL_INST_ID;
    stMsg.stMsgHdr.stReceiver.usTaskID = INVALID_TASK_ID;

    stMsg.stMsgHdr.usMsgID = EV_CLIENT_CONNECT_SERVER_IND;
    stMsg.stMsgHdr.ulTimeout = 0;
    stMsg.stMsgHdr.ulMsgLen = sizeof(stMsg) - STRUCT_OFFSET(CLIENT_CONNECT_SERVER_T, usServerID);
    stMsg.stMsgHdr.ucMsgPri = MSG_PRI_IMP;

    stMsg.usServerID = usServerID;

    GosDispatchSystemMsg((MSG_T*)&stMsg.stMsgHdr);
}

VOID GClientMsgHandler::OnDisconnectServer(UINT16 usServerID)
{
    CLIENT_DISCONNECT_SERVER_T     stMsg = {0};

    stMsg.stMsgHdr.stSender.usInstID = LOCAL_INST_ID;
    stMsg.stMsgHdr.stSender.usTaskID = INVALID_TASK_ID;

    stMsg.stMsgHdr.stReceiver.usInstID = LOCAL_INST_ID;
    stMsg.stMsgHdr.stReceiver.usTaskID = INVALID_TASK_ID;

    stMsg.stMsgHdr.usMsgID = EV_CLIENT_DISCONNECT_SERVER_IND;
    stMsg.stMsgHdr.ulTimeout = 0;
    stMsg.stMsgHdr.ulMsgLen = sizeof(stMsg) - STRUCT_OFFSET(CLIENT_DISCONNECT_SERVER_T, usServerID);
    stMsg.stMsgHdr.ucMsgPri = MSG_PRI_IMP;

    stMsg.usServerID = usServerID;

    GosDispatchSystemMsg((MSG_T*)&stMsg.stMsgHdr);
}

UINT16 _AllocServerID()
{
    static BOOL     bInit = FALSE;
    UINT32          i;

    if (!bInit)
    {
        memset(g_aucServerID, 0, sizeof(g_aucServerID));
        bInit = TRUE;
    }

    for (i=1; i<ARRAY_SIZE(g_aucServerID); i++)
    {
        if (g_aucServerID[i] == 0)
        {
            g_aucServerID[i] = 1;
            return i;
        }
    }

    return INVALID_INST_ID;
}

VOID _FreeServerID(UINT16 usServerID)
{
    if (usServerID <= ARRAY_SIZE(g_aucServerID))
    {
        g_aucServerID[usServerID] = 0;
    }
}

DTPClient* _InitDTPClient(UINT8 *pucServerAddr, UINT16 usServerPort, BOOL bTCPMode)
{
    GLogFactory         *pLogFactory = GLogFactory::GetInstance();
    DTP_CLIENT_CFG_T    stCfg = {0};
    UINT16              usServerID = _AllocServerID();

    if (!pLogFactory)
    {
        return NULL;
    }

    if (usServerID == INVALID_INST_ID)
    {
        GosLog(LOG_FATAL, "Alloc server inst failed");
        return NULL;
    }

    if (!bTCPMode)
    {
        usServerPort = GosGetUDPPort(usServerPort);
    }

    memcpy(stCfg.aucServerAddr, pucServerAddr, 4);
    stCfg.usServerPort = usServerPort;
    stCfg.usServerID = usServerID;
    stCfg.bAllowSyncMsg = TRUE;

    if (!m_pClientMsgHandle)
    {
        m_pClientMsgHandle = new GClientMsgHandler();
        if (!m_pClientMsgHandle)
        {
            _FreeServerID(usServerID);
            return NULL;
        }
    }

    DTPClient   *pDTPClient = DTPClient::New(&stCfg, m_pClientMsgHandle, m_pDTPMemAlloc, bTCPMode);

    if (!pDTPClient)
    {
        GosLog(LOG_FATAL, "init dtp client(" IP_FMT "/%u) failed", IP_ARG(pucServerAddr), usServerPort);
        delete m_pClientMsgHandle;
        _FreeServerID(usServerID);

        return NULL;
    }

    GOS_DTP_CLIENT_INFO_T       stDTPClient = {0};

    stDTPClient.pDTPClient = pDTPClient;
    memcpy(stDTPClient.stServerAddr.aucIP, pucServerAddr, sizeof(stDTPClient.stServerAddr.aucIP));
    stDTPClient.stServerAddr.usPort = usServerPort;
    stDTPClient.stServerAddr.ucTCPMode = bTCPMode;
    stDTPClient.usServerID = usServerID;

    m_vDTPClient.push_back(stDTPClient);

    return pDTPClient;
}

VOID _RunDTPClient()
{
    for (UINT32 i=0; i<m_vDTPClient.size(); i++)
    {
        m_vDTPClient[i].pDTPClient->Run();
    }
}

VOID _FreeDTPClient()
{
    for (UINT32 i=0; i<m_vDTPClient.size(); i++)
    {
        DTPClient::Free(m_vDTPClient[i].pDTPClient);
    }

    m_vDTPClient.clear();

    memset(g_aucServerID, 0, sizeof(g_aucServerID));
}

BOOL _IsSameAddr(PEER_ADDR_T *pstInnerAddr, PEER_ADDR_T *pstAddr)
{
    if (pstInnerAddr->ucTCPMode != pstAddr->ucTCPMode ||
        memcmp(pstInnerAddr->aucIP, pstAddr->aucIP, sizeof(pstAddr->aucIP)) != 0)
    {
        return FALSE;
    }

    UINT16  usPort = pstAddr->usPort;

    if (!pstInnerAddr->ucTCPMode)
    {
        usPort = GosGetUDPPort(usPort);
    }

    return usPort == pstInnerAddr->usPort;
}

DTPClient* GosGetDTPClient(PEER_ADDR_T *pstServerAddr)
{
    for (UINT32 i=0; i<m_vDTPClient.size(); i++)
    {
        if (_IsSameAddr(&m_vDTPClient[i].stServerAddr, pstServerAddr))
        {
            return m_vDTPClient[i].pDTPClient;
        }
    }

    return NULL;
}

BOOL GosCreateDTPClient(PEER_ADDR_T *pstServerAddr)//, BOOL bTCPMode)
{
    DTPClient   *pDTPClient;

    pDTPClient = GosGetDTPClient(pstServerAddr);
    if (pDTPClient)
    {
        return TRUE;
    }

    pDTPClient = _InitDTPClient(pstServerAddr->aucIP, pstServerAddr->usPort, pstServerAddr->ucTCPMode);
    if (!pDTPClient)
    {
        return FALSE;
    }

    pDTPClient->Run();

    return TRUE;
}

BOOL GosStartDTPClient(PEER_ADDR_T *pstServerAddr)
{
    VECTOR<GOS_DTP_CLIENT_INFO_T>::iterator     it = m_vDTPClient.begin();

    for (; it!=m_vDTPClient.end(); it++)
    {
        if (_IsSameAddr(&it->stServerAddr, pstServerAddr))
        {
            if (!it->pDTPClient->IsConnected())
            {
                it->pDTPClient->Run();
            }
            return TRUE;
        }
    }

    return FALSE;
}

BOOL GosStartDTPClient(PEER_ADDR_T *pstServerAddr, UINT16 ulServerID)
{
    VECTOR<GOS_DTP_CLIENT_INFO_T>::iterator     it = m_vDTPClient.begin();
    UINT8    aucIP[4];
    UINT16   usPort;

    for (; it!=m_vDTPClient.end(); it++)
    {
        if (it->usServerID == ulServerID)
        {
            it->pDTPClient->GetServerAddr(aucIP, &usPort);
            if (memcmp(aucIP, pstServerAddr->aucIP, sizeof(aucIP)) != 0 ||
                usPort != pstServerAddr->usPort)
            {
                it->pDTPClient->SetServer(pstServerAddr->aucIP, pstServerAddr->usPort);
                memcpy(&it->stServerAddr, pstServerAddr, sizeof(PEER_ADDR_T));
            }

            if (!it->pDTPClient->IsConnected())
            {
                it->pDTPClient->Resume();
            }

            return TRUE;
        }
    }

    return FALSE;
}

BOOL GosStopDTPClient(PEER_ADDR_T *pstServerAddr)
{
    VECTOR<GOS_DTP_CLIENT_INFO_T>::iterator     it = m_vDTPClient.begin();

    for (; it!=m_vDTPClient.end(); it++)
    {
        //if (memcmp(&it->stServerAddr, pstServerAddr, sizeof(PEER_ADDR_T)) == 0)
        if (_IsSameAddr(&it->stServerAddr, pstServerAddr))
        {
            it->pDTPClient->Suspend();
            return TRUE;
        }
    }

    return FALSE;
}

VOID GosFreeDTPClient(PEER_ADDR_T *pstServerAddr)
{
    BOOL    bLocated  = FALSE;

    VECTOR<GOS_DTP_CLIENT_INFO_T>::iterator     it = m_vDTPClient.begin();

    for (; it != m_vDTPClient.end(); it++)
    {
        if (_IsSameAddr(&it->stServerAddr, pstServerAddr))
        {
            bLocated = TRUE;
            break;
        }
    }

    if (!bLocated)
    {
        return;
    }

    DTPClient::Free(it->pDTPClient);

    _FreeServerID(it->usServerID);

    m_vDTPClient.erase(it);
}

UINT16 GosGetInstID()
{
    return g_usInstID;
}

BOOL GosGetPeerAddr(CHAR *szPeer, PEER_ADDR_T *pstPeerAddr)
{
    CHAR    *szTmp = strchr(szPeer, '@');

    pstPeerAddr->ucTCPMode = TRUE;

    if (szTmp)
    {
        CHAR *szMode = szTmp+1;

        gos_trim_string(szMode);

        if (strcasecmp(szMode, "tcp") == 0)
        {
            pstPeerAddr->ucTCPMode = TRUE;
        }
        else if (strcasecmp(szMode, "udp") == 0)
        {
            pstPeerAddr->ucTCPMode = FALSE;
        }
        else
        {
            return FALSE;
        }

        *szTmp = '\0';
    }

    return gos_get_ip_port(szPeer, pstPeerAddr->aucIP, &pstPeerAddr->usPort);
}

BOOL GosLoadPlatConf(PLAT_CFG_T *pstCfg, CHAR *szPlatCfgFile, UINT16 usDefaultServerPort)
{
    CHAR        acFile[512];
    GConfFile   *pConf;
    CHAR        *szValue;
    CHAR        *szTmp;
    CHAR        *szPeer;
    INT32       iValue;
    BOOL        bLocated = FALSE;
    BOOL        bRet = FALSE;
    PEER_ADDR_T     *pstPeerAddr;

    sprintf(acFile, "%s/%s", gos_get_root_path(), szPlatCfgFile);
    gos_format_path(acFile);

    if (!gos_file_exist(acFile))
    {
        GosLog(LOG_ERROR, "file %s not exist!", acFile);
        return FALSE;
    }

    pConf = new GConfFile(acFile);

    if (!pConf->GetBool("multi_inst", FALSE, &pstCfg->bMultiInst))
    {
        goto end;
    }

    // plat_inst_id
    if (!pConf->Get("plat_inst_id", &iValue))
    {
        printf("miss plat_inst_id!\n");
        goto end;
    }

    if (iValue == 0 || iValue > 0xffff)
    {
        GosLog(LOG_ERROR, "invalid plat_inst_id in %s", acFile);
        goto end;
    }

    pstCfg->usInstID = (UINT16)iValue;
    g_usInstID = pstCfg->usInstID;

    /* self */
    szValue = pConf->Get("self_addr");
    if (szValue)
    {
        if (!gos_get_ip_port(szValue, pstCfg->stSelfAddr.aucIP, &pstCfg->stSelfAddr.usPort))
        {
            GosLog(LOG_ERROR, "invalid self_addr in %s", acFile);
            goto end;
        }

        if (pstCfg->stSelfAddr.usPort == 0)
        {
            pstCfg->stSelfAddr.usPort = usDefaultServerPort;
        }

        pstCfg->bServerMode = TRUE;
    }

    /* peer: ip1[:port1[@udp|@tcp]] [ip2[:port2]] */
    szValue = pConf->Get("peer_addr");
    if (szValue)
    {
        pstCfg->ulPeerNum = 0;
        pstPeerAddr = pstCfg->astPeerAddr;

        while(*szValue)
        {
            szPeer = szValue;
            bLocated = FALSE;
            for (szTmp=szPeer; *szTmp!='\0'; szTmp++)
            {
                if (gos_isspace(*szTmp))
                {
                    bLocated = TRUE;
                    *szTmp++ = '\0';
                    szValue = gos_left_trim_string(szTmp);
                    break;
                }
            }

            if (!bLocated)
            {
                szValue = (CHAR*)"";
            }

            if (pstCfg->ulPeerNum >= ARRAY_SIZE(pstCfg->astPeerAddr))
            {
                goto end;
            }

            if (!GosGetPeerAddr(szPeer, pstPeerAddr))
            {
                GosLog(LOG_ERROR, "invalid peer_addr %s in %s", szPeer, acFile);

                goto end;
            }

            if (pstPeerAddr->usPort == 0)
            {
                pstPeerAddr->usPort = usDefaultServerPort;
            }

            pstCfg->ulPeerNum++;
            pstPeerAddr++;
        }
    }

    if (!pstCfg->bServerMode && pstCfg->ulPeerNum == 0)
    {
        GosLog(LOG_ERROR, "no peer_addr in %s", acFile);
        goto end;
    }

    pstCfg->bExitWhenMsgQueueIsFull = FALSE;
    szValue = pConf->Get("exit_when_msg_is_full");
    if (szValue)
    {
        if (strcasecmp(szValue, "true") == 0 ||
            strcmp(szValue, "1") == 0)
        {
            pstCfg->bExitWhenMsgQueueIsFull = TRUE;
        }
    }

    pstCfg->bLogMsgStat = FALSE;
    szValue = pConf->Get("log_msg_stat");
    if (szValue)
    {
        if (strcasecmp(szValue, "true") == 0 ||
            strcmp(szValue, "1") == 0)
        {
            pstCfg->bLogMsgStat = TRUE;
        }
    }

    szValue = pConf->Get("msg_max_wait_time");
    if (szValue)
    {
        if (!gos_atou(szValue, &pstCfg->ulMsgMaxWaitTime))
        {
            pstCfg->ulMsgMaxWaitTime = 0;
        }

        if (pstCfg->ulMsgMaxWaitTime > 0 && pstCfg->ulMsgMaxWaitTime < 60)
        {
            pstCfg->ulMsgMaxWaitTime = 60;
        }
    }

    szValue = pConf->Get("max_msg_node_num");
    if (szValue)
    {
        if (!gos_atou(szValue, &pstCfg->ulMaxMsgNodeNum))
        {
            pstCfg->ulMaxMsgNodeNum = 0;
        }
    }

    /* log */
    if (!pConf->Get("log_level", &iValue))
    {
        iValue = DEFAULT_LOG_LEVEL;
    }
    pstCfg->stLogCfg.ulLogLevel = iValue;

    if (!pConf->Get("imp_log_level", &iValue))
    {
        iValue = LOG_INFO;
    }
    pstCfg->stLogCfg.ulImpLogLevel = iValue;

    if (!pConf->Get("log_to_file", &iValue))
    {
        iValue = DEFAULT_LOG_TO_FILE;
    }
    pstCfg->stLogCfg.bLogToFile = iValue;

    if (!pConf->Get("log_to_stdout", &iValue))
    {
        iValue = DEFAULT_LOG_TO_STDOUT;
    }
    pstCfg->stLogCfg.bLogToStdout = iValue;

    pConf->GetBool("print_color", FALSE, &pstCfg->stLogCfg.bPrintColor);
    pConf->GetBool("log_dev_info", FALSE, &pstCfg->stLogCfg.bLogDevInfo);

    if (!pConf->Get("log_file_num", &iValue))
    {
        iValue = DEFAULT_LOG_FILE_NUM;
    }

    if (iValue > 100)
    {
        iValue = 100;
    }
    else if (iValue <= 2)
    {
        iValue = DEFAULT_LOG_FILE_NUM;
    }

    pstCfg->stLogCfg.ulLogFileNum = iValue;

    if (!pConf->Get("log_file_size", &iValue))
    {
        iValue = DEFAULT_LOG_FILE_SIZE;
    }

    if (iValue < 64)
    {
        iValue = 64;
    }
    else if (iValue > 102400)
    {
        iValue = 102400;
    }
    pstCfg->stLogCfg.ulLogFileSize = 1024*iValue;

    pstCfg->stLogCfg.ulLogBufTime = 0;
    if (pConf->Get("log_buf_time"))
    {
        if (!pConf->Get("log_buf_time", &iValue) || iValue < 0)
        {
            goto end;
        }

        pstCfg->stLogCfg.ulLogBufTime = iValue;
    }

    pstCfg->ulMaxUniqueMsgNum = 0;
    if (!pConf->Get("max_unique_msg_num", &iValue))
    {
        iValue = 0;
    }
    pstCfg->ulMaxUniqueMsgNum = iValue;

    pstCfg->ulMaxSysLogNum = 0;
    if (!pConf->Get("max_syslog_num", &iValue))
    {
        iValue = 0;
    }
    else
    {
        iValue = 100000;
    }
    pstCfg->ulMaxSysLogNum = iValue;

    GosSetMsgMaxWaitTime(pstCfg->ulMsgMaxWaitTime);
    GosSetMaxMsgNodeNum(pstCfg->ulMaxMsgNodeNum);
    GosSetExitWhenMsgQueueIsFull(pstCfg->bExitWhenMsgQueueIsFull);

    GosInitImpLogger(pstCfg->stLogCfg.ulImpLogLevel);

    bRet = TRUE;

end:

    delete pConf;

    return bRet;
}

DTPClient* GosGetDTPClient(UINT16 usServerInstID)
{
    for (UINT32 i=0; i<m_vDTPClient.size(); i++)
    {
        if (m_vDTPClient[i].usServerID == usServerInstID)
        {
            return m_vDTPClient[i].pDTPClient;
        }
    }

    return NULL;
}

VOID GosGetServerList(VECTOR<UINT16> &vServerInstID)
{
    vServerInstID.clear();

    for (UINT32 i=0; i<m_vDTPClient.size(); i++)
    {
        vServerInstID.push_back(m_vDTPClient[i].usServerID);
    }
}

BOOL GosSendServer(UINT16 usSenderTaskID, UINT16 usReceiverTaskID, UINT32 ulSeqID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    if (m_vDTPClient.size() == 0)
    {
        GosLog(MODULE_PLAT, LOG_FAIL, "Send msg(%u) to server failed, invalid server", usMsgID);

        return FALSE;
    }
    else if (m_vDTPClient.size() > 1)
    {
        GosLog(MODULE_PLAT, LOG_FAIL, "Send msg(%u) to server failed, server is unidentified", usMsgID);

        return FALSE;
    }

    return m_vDTPClient[0].pDTPClient &&
           m_vDTPClient[0].pDTPClient->Send(usSenderTaskID, usReceiverTaskID, usMsgID, ulSeqID, pvMsg, ulMsgLen);
}

VOID GosSendAllServer(UINT16 usSenderTaskID, UINT16 usReceiverTaskID, UINT32 ulSeqID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    if (m_vDTPClient.size() == 0)
    {
        GosLog(MODULE_PLAT, LOG_FAIL, "Send msg(%u) to server failed, no valid server", usMsgID);

        return;
    }

    for (UINT32 i=0; i<m_vDTPClient.size(); i++)
    {
        if (m_vDTPClient[i].pDTPClient)
        {
            m_vDTPClient[i].pDTPClient->Send(usSenderTaskID, usReceiverTaskID, usMsgID, ulSeqID, pvMsg, ulMsgLen);
        }
    }
}

BOOL GosSendServer(UINT16 usSenderTaskID, UINT16 usReceiverTaskID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    if (m_vDTPClient.size() == 0)
    {
        GosLog(MODULE_PLAT, LOG_FAIL, "Send msg(%u) to server failed, invalid server", usMsgID);

        return FALSE;
    }
    else if (m_vDTPClient.size() > 1)
    {
        GosLog(MODULE_PLAT, LOG_FAIL, "Send msg(%u) to server failed, server is unidentified", usMsgID);

        return FALSE;
    }

    return m_vDTPClient[0].pDTPClient &&
           m_vDTPClient[0].pDTPClient->SendReq(usSenderTaskID, usReceiverTaskID, usMsgID, pvMsg, ulMsgLen);
}

BOOL GosSendServer(UINT16 usSenderTaskID, PID_T *pstReceiver, UINT32 ulSeqID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    DTPClient   *pDTPClient = GosGetDTPClient(pstReceiver->usInstID);

    if (!pDTPClient)
    {
        GosLog(MODULE_PLAT, LOG_FAIL, "Send msg(%u) to server failed, unknown server(%u)", usMsgID, pstReceiver->usInstID);

        return FALSE;
    }

    return pDTPClient->Send(usSenderTaskID, pstReceiver->usTaskID, usMsgID, ulSeqID, pvMsg, ulMsgLen);
}

BOOL GosSendServer(UINT16 usSenderTaskID, PID_T *pstReceiver, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    DTPClient   *pDTPClient = GosGetDTPClient(pstReceiver->usInstID);

    if (!pDTPClient)
    {
        GosLog(MODULE_PLAT, LOG_FAIL, "Send msg(%u) to server failed, unknown server(%u)", usMsgID, pstReceiver->usInstID);

        return FALSE;
    }

    return pDTPClient->SendReq(usSenderTaskID, pstReceiver->usTaskID, usMsgID, pvMsg, ulMsgLen);
}

DTPServer* GosGetDTPServer(UINT16 usInstID)
{
    if (m_pDTPTCPServer && m_pDTPTCPServer->HasClient(usInstID))
    {
        return m_pDTPTCPServer;
    }

    if (m_pDTPUDPServer && m_pDTPUDPServer->HasClient(usInstID))
    {
        return m_pDTPUDPServer;
    }

    return NULL;
}

BOOL GosSendClient(UINT16 usSenderTaskID, PID_T *pstReceiver, UINT32 ulSeqID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    DTPServer   *pDTPServer = GosGetDTPServer(pstReceiver->usInstID);

    if (pDTPServer)
    {
        return pDTPServer->Send(usSenderTaskID, pstReceiver->usInstID, pstReceiver->usTaskID, usMsgID, ulSeqID, pvMsg, ulMsgLen);
    }

    GosLog(MODULE_PLAT, LOG_FAIL, "Send msg(%u) to client failed, invalid inst id %u!", usMsgID, pstReceiver->usInstID);

    return FALSE;
}

BOOL GosSendClient(UINT16 usSenderTaskID, PID_T *pstReceiver, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    DTPServer   *pDTPServer = GosGetDTPServer(pstReceiver->usInstID);

    if (pDTPServer)
    {
        return pDTPServer->SendReq(usSenderTaskID, pstReceiver->usInstID, pstReceiver->usTaskID, usMsgID, pvMsg, ulMsgLen);
    }

    GosLog(MODULE_PLAT, LOG_FAIL, "Send msg(%u) to client failed, invalid inst id %u!", usMsgID, pstReceiver->usInstID);

    return FALSE;
}

VOID GosSendAllClient(UINT16 usSenderTaskID, UINT16 usReceiverTaskID, UINT8 ucClientType, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    if (m_pDTPTCPServer)
    {
        m_pDTPTCPServer->SendGroupMsg(usSenderTaskID, usReceiverTaskID, ucClientType, usMsgID, pvMsg, ulMsgLen);
    }

    if (m_pDTPUDPServer)
    {
        m_pDTPUDPServer->SendGroupMsg(usSenderTaskID, usReceiverTaskID, ucClientType, usMsgID, pvMsg, ulMsgLen);
    }
}

BOOL GosGetClientList(UINT16 *pusClientID, UINT32 *pulClientNum)
{
    UINT16      ausClientID[DTP_MAX_CONN_NUM];
    UINT32      ulClientNum;

    *pulClientNum = 0;

    if (m_pDTPTCPServer)
    {
        m_pDTPTCPServer->GetClientList(ausClientID, &ulClientNum);
        if (ulClientNum > 0)
        {
            memcpy(pusClientID, ausClientID, ulClientNum*sizeof(UINT16));
            *pulClientNum += ulClientNum;
            pusClientID += ulClientNum;
        }
    }

    if (m_pDTPUDPServer)
    {
        m_pDTPUDPServer->GetClientList(ausClientID, &ulClientNum);
        if (ulClientNum > 0)
        {
            memcpy(pusClientID, ausClientID, ulClientNum*sizeof(UINT16));
            *pulClientNum += ulClientNum;
        }
    }

    return TRUE;
}

BOOL GosSendClient(UINT16 usSenderTaskID, UINT16 usClientID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    DTPServer   *pDTPServer = GosGetDTPServer(usClientID);

    if (pDTPServer)
    {
        return pDTPServer->SendReq(usSenderTaskID, usClientID, INVALID_TASK_ID, usMsgID, pvMsg, ulMsgLen);
    }

    GosLog(MODULE_PLAT, LOG_FAIL, "Send msg(%u) to client failed, invalid inst id %u!", usMsgID, usClientID);

    return FALSE;
}

BOOL GosSyncSendClient(UINT16 usSenderTaskID, PID_T *pstReceiver, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen,VOID *pvRspMsg,UINT32 ulMaxRspLen,UINT32 *ulRspMsgLen, UINT32 ulTimeout)
{
    DTPServer   *pDTPServer = GosGetDTPServer(pstReceiver->usInstID);

    if (pDTPServer)
    {
        return pDTPServer->SyncSend(usSenderTaskID, pstReceiver->usInstID, pstReceiver->usTaskID, usMsgID, pvMsg, ulMsgLen,pvRspMsg,ulMaxRspLen,ulRspMsgLen,ulTimeout);
    }

    GosLog(MODULE_PLAT, LOG_FAIL, "Send msg(%u) to client failed, invalid inst id %u!", usMsgID, pstReceiver->usInstID);

    return FALSE;
}

/*发送广播消息*/
VOID GosSendBroadcast(UINT16 usSenderTaskID, PID_T *pstReceiver, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    if (m_pDTPTCPServer)
    {
        m_pDTPTCPServer->SendBroadcastMsg(usSenderTaskID, pstReceiver->usTaskID, usMsgID, pvMsg, ulMsgLen);
    }

    if (m_pDTPUDPServer)
    {
        m_pDTPUDPServer->SendBroadcastMsg(usSenderTaskID, pstReceiver->usTaskID, usMsgID, pvMsg, ulMsgLen);
    }
}

BOOL GosSend(UINT16 usSenderTaskID, PID_T *pstReceiver, UINT32 ulSeqID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    DTPClient       *pDTPClient = GosGetDTPClient(pstReceiver->usInstID);

    if (pDTPClient)
    {
        return pDTPClient->Send(usSenderTaskID, pstReceiver->usTaskID, usMsgID, ulSeqID, pvMsg, ulMsgLen);
    }

    DTPServer       *pDTPServer = GosGetDTPServer(pstReceiver->usInstID);

    if (pDTPServer)
    {
        return pDTPServer->Send(usSenderTaskID, pstReceiver->usInstID, pstReceiver->usTaskID, usMsgID, ulSeqID, pvMsg, ulMsgLen);
    }

    GosLog(MODULE_PLAT, LOG_FAIL, "Send msg(%u) to remote failed, invalid inst id %u!", usMsgID, pstReceiver->usInstID);

    return FALSE;
}

BOOL GosSendRsp(UINT16 usSenderTaskID, PID_T *pstReceiver, UINT32 ulSeqID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    return GosSend(usSenderTaskID, pstReceiver, ulSeqID, usMsgID, pvMsg, ulMsgLen);
}

BOOL GosSendLocal(PID_T *pstReceiver, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulSeqID)
{
    GMsgPool        *pMsgPool = GMsgPool::GetInstance();
    MSG_T           stMsg = {0};

    if (!pMsgPool)
    {
        GosLog(MODULE_PLAT, LOG_FAIL, "Send msg(%u) to local failed, can't alloc msg pool!", usMsgID);
        return FALSE;
    }

    stMsg.stSender.usInstID = LOCAL_INST_ID;
    stMsg.stSender.usTaskID = GosGetTaskID();
    memcpy(&stMsg.stReceiver, pstReceiver, sizeof(PID_T));
    stMsg.usMsgID = usMsgID;
    stMsg.ulMsgLen = ulMsgLen;
    stMsg.ulSeqID = ulSeqID;
    stMsg.ucMsgPri = MSG_PRI_NOR;

    if (!GosDispatchMsg(&stMsg, pvMsg, ulMsgLen))
    {
        GosLog(MODULE_PLAT, LOG_FAIL, "Send msg(%u) to local failed!", usMsgID);
        return FALSE;
    }

    return TRUE;
}

BOOL GosIsClientConnected()
{
    if (m_vDTPClient.size() == 1 && m_vDTPClient[0].pDTPClient)
    {
        return m_vDTPClient[0].pDTPClient->IsConnected();
    }

    return FALSE;
}

BOOL GosIsClientConnected(UINT16 usServerInstID)
{
    DTPClient   *pDTPClient = GosGetDTPClient(usServerInstID);

    if (pDTPClient)
    {
        return pDTPClient->IsConnected();
    }

    return FALSE;
}

BOOL GosGetClientAddr(UINT16 usClientID, UINT8 *pucClientAddr, UINT16 *pusClientPort)
{
    DTPServer   *pDTPServer = GosGetDTPServer(usClientID);

    if (!pDTPServer)
    {
        GosLog(MODULE_PLAT, LOG_FAIL, "get client failed, invalid client id %u!", usClientID);
        return FALSE;
    }

    return pDTPServer->GetClientAddr(usClientID, pucClientAddr, pusClientPort);
}
