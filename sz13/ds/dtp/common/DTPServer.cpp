#include "g_include.h"

#include "dtp_public.h"
#include "dtp_common.h"
#include "dtp_func.h"
#include "dtp_tcp_server.h"
#include "dtp_udp_server.h"

DTPServer* DTPServer::New(UINT16 usInstID, UINT8 *pucAddr, UINT16 usPort,
                          DTPServerMsgHandler *pMsgHandle,
                          GMemAlloc *pMemAlloc,
                          BOOL bTCPMode)
{
    if (bTCPMode)
    {
        return DTPTCPServer::New(usInstID, pucAddr, usPort,pMsgHandle, pMemAlloc);
    }
    else
    {
        return DTPUDPServer::New(usInstID, pucAddr, usPort,pMsgHandle, pMemAlloc);
    }
}

DTPServer::~DTPServer()
{
    if (m_bTCPMode)
    {
        DTPTCPServer    *pInstance = (DTPTCPServer*)this;

        pInstance->Close();
    }
    else
    {
        DTPUDPServer    *pInstance = (DTPUDPServer*)this;

        pInstance->Close();
    }
}

#if 0
DTPServer *g_pServer;

#define BLOCK_NUM       1024//*5

class MyDTPServerMsgHandle : public DTPServerMsgHandler
{
public:
    virtual VOID MsgHandle(UINT16 usClientID, DTP_MSG_T *pstMsg);
    virtual VOID OnAcceptClient(UINT16 usClientID)
    {
        printf("accept client %d\n", usClientID);
    };
    virtual VOID OnDisconnectClient(UINT16 usClientID)
    {
        printf("close client %d\n", usClientID);
    };
};

VOID MyDTPServerMsgHandle::MsgHandle(UINT16 usClientID, DTP_MSG_T *pstMsg)
{
    printf("recv client %d = %u\n", usClientID, pstMsg->stMsgHdr.ulSeqID);
    g_pServer->Send(0, usClientID, 0, 0, pstMsg->stMsgHdr.ulSeqID, pstMsg->aucMsgBody, pstMsg->stMsgHdr.ulMsgLen);
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
    };
};

void dtp_server_test_main()
{
    UINT8   aucServerAddr[4] = {0,0,0,0};
    UINT16  usServerPort = 20000;
    BOOL    bTCPMode = TRUE;

    MyMemAlloc *pMemAlloc = new MyMemAlloc();
    MyDTPServerMsgHandle *pMsgHandle = new MyDTPServerMsgHandle();
    Logger *pLogger = new GLogger();
    pLogger->SetLogLevel(LOG_INFO);
    pLogger->SetLogToStdout(TRUE);
    g_pServer = DTPServer::New(100, aucServerAddr, usServerPort, pMsgHandle, pMemAlloc, pLogger, bTCPMode);

    g_pServer->Run();

    while(1)
    {
       gos_sleep(5);
    }

}
#endif
