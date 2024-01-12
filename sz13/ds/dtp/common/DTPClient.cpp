#include "g_include.h"

#include "dtp_public.h"
#include "dtp_common.h"
#include "dtp_func.h"
#include "dtp_tcp_client.h"
#include "dtp_udp_client.h"

DTPClient* DTPClient::New(DTP_CLIENT_CFG_T *pstClientCfg, DTPClientMsgHandler *pMsgHandle, GMemAlloc *pMemAlloc, BOOL bTCPMode)
{
    if (bTCPMode)
    {
        return DTPTCPClient::New(pstClientCfg, pMsgHandle, pMemAlloc);
    }
    else
    {
        return DTPUDPClient::New(pstClientCfg, pMsgHandle, pMemAlloc);
    }
}

VOID DTPClient::Free(DTPClient *pDTPClient)
{
    if (!pDTPClient)
    {
        return;
    }

    if (pDTPClient->m_bTCPMode)
    {
        DTPTCPClient    *pInstance = (DTPTCPClient*)pDTPClient;
        delete pInstance;
    }
    else
    {
        DTPUDPClient    *pInstance = (DTPUDPClient*)pDTPClient;
        delete pInstance;
    }
}

DTPClient::~DTPClient()
{
}


#if 0
DTPClient   *g_pClient = NULL;
Logger      *g_pLogger = NULL;

VOID send_msg()
{
    static CHAR aucReq[128*1024];
    static UINT32 ulIndex = 0;

    ulIndex ++;
    if (ulIndex > 128*1024)
    {
        return;
    }

    sprintf(aucReq, "%u", ulIndex);

    g_pClient->SendReq(0, 0, 0, aucReq, 32*1024);
}

class MyDTPClientMsgHandler : public DTPClientMsgHandler
{
public:
    virtual VOID MsgHandle(UINT16 usClientModuleID, DTP_MSG_T *pstMsg);
    virtual VOID OnConnectServer(UINT16 usServerID){};
    virtual VOID OnDisconnectServer(UINT16 usServerID){};
};

VOID MyDTPClientMsgHandler::MsgHandle(UINT16 usClientModuleID, DTP_MSG_T *pstMsg)
{
    static UINT32   i = 0;
    static UINT32   ulSize = 0;

    i++;
    ulSize += pstMsg->stMsgHdr.ulMsgLen;

    if (MOD(i, 32) == 0)
    {
        g_pLogger->DTPLog(LOG_INFO, "recv msg num = %u, len = %u", i, ulSize);
    }

    send_msg();
}

class MyMemAlloc:public GMemAlloc
{
public:
    virtual VOID* MemAlloc(UINT32 ulSize){return malloc(ulSize);};
    virtual VOID MemFree(VOID* ptr)
    {
        if (ptr)
        {
            free(ptr);
        }
    }
};

extern BOOL GosGetUDPPort(UINT16 usTCPPort);

VOID dtp_send_test(VOID)
{
    DTP_CLIENT_CFG_T    stClientCfg = {0};

    SET_IP(stClientCfg.aucServerAddr, 127,0,0,1);
    stClientCfg.usServerPort = 63001;
    stClientCfg.usServerID = 100;
    stClientCfg.bAllowSyncMsg = TRUE;
    stClientCfg.ulLinkHelloInterval = 60;
    stClientCfg.ulLinkHelloFailNum = 10;

    DTPClientMsgHandler *pClientMsgHandler = new MyDTPClientMsgHandler();
    GMemAlloc *pMemAlloc = new MyMemAlloc();
    g_pLogger = new GLogger();
    g_pLogger->SetLogLevel(LOG_INFO);
    g_pLogger->SetLogToFile(FALSE);
    g_pLogger->SetLogToStdout(TRUE);

    g_pClient = DTPClient::New(&stClientCfg, pClientMsgHandler, pMemAlloc, g_pLogger, TRUE);

    g_pClient->Run();

    while (!g_pClient->IsConnected())
    {
        gos_sleep_1ms(100);
    }

    send_msg();
}

void dtp_client_test_main(VOID)
{
    dtp_send_test();

    while(1)
    {
        gos_sleep_1ms(2);
    }

    getchar();
}
#endif

