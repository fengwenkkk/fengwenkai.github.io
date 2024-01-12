//---------------------------------------------------------------------------

#include "g_include.h"
#include "DTPClient.h"
#include "GCommon.h"

#include "InnerMsg.h"
#include "RemoteMsg.h"

UINT32       g_ulDefaultSyncSendTimeout = 30;

//CLIENT_CONNECTION_CHANGE    *s_pfClientConnectionChange = NULL;

extern BOOL GosGetUDPPort(UINT16 usTCPPort);

class ClientMsgHandler:public DTPClientMsgHandler
{
public:
    ClientMsgHandler(RemoteMsg *pRemoteMsg)
    {
        m_pRemoteMsg = pRemoteMsg;
    }

    virtual VOID MsgHandle(UINT16 usServerID, DTP_MSG_T *pstMsg)
    {
        if (pstMsg->stMsgHdr.ulMsgLen < sizeof(DTP_MSG_HDR_T))
        {
            return;
        }

        UINT32 ulMsgLen = pstMsg->stMsgHdr.ulMsgLen - sizeof(DTP_MSG_HDR_T);

        SendInnerMsg(pstMsg->stMsgHdr.usMsgType, pstMsg->aucMsgBody, ulMsgLen);
    }

    virtual VOID OnConnectServer(UINT16 usServerID)
    {
        CLIENT_CONNECTION_CHANGE    *pfClientConnectionChange = m_pRemoteMsg->GetClientConnectionChangeFunc();

        if (pfClientConnectionChange)
        {
            pfClientConnectionChange(true);
        }
    }

    virtual VOID OnDisconnectServer(UINT16 usServerID)
    {
        CLIENT_CONNECTION_CHANGE    *pfClientConnectionChange = m_pRemoteMsg->GetClientConnectionChangeFunc();

        if (pfClientConnectionChange)
        {
            pfClientConnectionChange(false);
        }
    }

private:
    RemoteMsg   *m_pRemoteMsg;
};

VOID RemoteMsg::SetClientConnectionChange(CLIENT_CONNECTION_CHANGE *pfClientConnectionChange)
{
    m_pfClientConnectionChange = pfClientConnectionChange;
}

BOOL RemoteMsg::IsConnected()
{
    return m_pDTPClient && m_pDTPClient->IsConnected();
}

BOOL RemoteMsg::SendReq(UINT16 usMsgType, VOID *pvMsg, UINT32 ulMsgLen)
{
    if (!IsConnected())
    {
        return FALSE;
    }

    return m_pDTPClient && m_pDTPClient->SendReq(INVALID_TASK_ID, INVALID_TASK_ID, usMsgType, pvMsg, ulMsgLen);
}

BOOL RemoteMsg::SyncSend(UINT16 usMsgType, VOID *pvMsg, UINT32 ulMsgLen, VOID* pvRspMsg,
                         UINT32 ulMaxRspMsgLen, UINT32 *pulRspMsgLen, UINT32 ulTimeout)
{
    if (!IsConnected())
    {
        return FALSE;
    }

    if (0 == ulTimeout)
    {
        ulTimeout = g_ulDefaultSyncSendTimeout;
    }

    UINT32      ulTime = gos_get_uptime_1ms();

    BOOL bRet = m_pDTPClient && m_pDTPClient->SyncSend(INVALID_TASK_ID, INVALID_TASK_ID, usMsgType, pvMsg, ulMsgLen,
                                                  pvRspMsg, ulMaxRspMsgLen, pulRspMsgLen, ulTimeout);

    ulTime = gos_get_uptime_1ms() - ulTime;
    if (ulTime > 200)
    {
        GosLog(LOG_DETAIL, "Sync Msg: msgID = 0x%X, time = %u", usMsgType, ulTime);
    }

    return bRet;
}

VOID RemoteMsg::GetClientAddr(UINT8 *pucAddr, UINT16 *pusPort)
{
    if (m_pDTPClient)
    {
        m_pDTPClient->GetClientAddr(pucAddr, pusPort);
    }
}

BOOL RemoteMsg::Init(UINT8 *pucServerAddr, UINT16 usServerPort, BOOL bTCPMode)
{
    DTP_CLIENT_CFG_T    stClientCfg = {0};
    GOS_MEM_POOL_T      astMsgPool[] = {{64*1024, 32}};

    if (usServerPort == 0)
    {
        return FALSE;
    }

    if (!bTCPMode)
    {
        usServerPort = GosGetUDPPort(usServerPort);
    }

    memcpy(stClientCfg.aucServerAddr, pucServerAddr, 4);
    stClientCfg.usServerPort = usServerPort;
    stClientCfg.usServerID = 0;
    stClientCfg.bAllowSyncMsg = TRUE;
    stClientCfg.ucMsgPri = MSG_PRI_IMP;

    GLogFactory     *pLogFactory = GLogFactory::GetInstance();
    Logger          *pLogger;

    if (!pLogFactory)
    {
        return FALSE;
    }

    pLogger = pLogFactory->GetDefaultLogger();
    if (!pLogger)
    {
        return FALSE;
    }

    GMemPool *pMemPool = GMemPool::GetInstance(&astMsgPool[0], ARRAY_SIZE(astMsgPool), TRUE, TRUE);
    if (!pMemPool)
    {
        return FALSE;
    }

    ClientMsgHandler *pClientMsgHandle = new ClientMsgHandler(this);
    if (!pClientMsgHandle)
    {
        delete pMemPool;
        return FALSE;
    }

    DTPClient   *pDTPClient = DTPClient::New(&stClientCfg, pClientMsgHandle, pMemPool, bTCPMode);

    if (!pDTPClient)
    {
        delete pMemPool;
        delete pClientMsgHandle;

        return FALSE;
    }

    m_pDTPClient = pDTPClient;

    m_pDTPClient->Run();
    return TRUE;
}

RemoteMsg::RemoteMsg()
{
    m_pfClientConnectionChange = NULL;
    m_pDTPClient = NULL;
    m_bReady = FALSE;
}

/*
RemoteMsg* RemoteMsg::GetInstance()
{
    return s_pRemoteMsg;
}  */

RemoteMsg* RemoteMsg::New(UINT8 *pucServerAddr, UINT16 usServerPort, BOOL bTCPMode)
{
    RemoteMsg   *pRemoteMsg = new RemoteMsg();

    if (!pRemoteMsg->Init(pucServerAddr, usServerPort, bTCPMode))
    {
        delete pRemoteMsg;
        pRemoteMsg = NULL;
    }

    return pRemoteMsg;
}

VOID RemoteMsg::SetServer(UINT8 *pucServerAddr, UINT16 usServerPort)
{
    m_pDTPClient->SetServer(pucServerAddr, usServerPort);
}

VOID RemoteMsg::SetClientType(UINT8 ucClientType)
{
    m_pDTPClient->SetClientType(ucClientType);
}

