#ifndef TASK_OCC_H
#define TASK_OCC_H

class TaskOCC: public GBaseTask
{
public :
    TaskOCC(UINT16 usDispatchID);

    virtual VOID TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);
    BOOL Send(const CHAR *szMsgName, UINT32 ulSeqID, CHAR *szMsgInfo);

private:
    SOCKET      m_stSock;
    SOCKADDR_IN m_stNOCCAddr;

    BOOL        m_bRunning;
    UINT16      m_usLocalPort;

    UINT8       m_aucMsg[64*1024];

    UINT32      m_ulSendHeartBeatMsgPeriod;

    virtual VOID OnServerDisconnectClient(UINT16 usClientID);

    BOOL Init();
    BOOL InitCfg();
    BOOL InitSocket();
    VOID Run();

    INT32 RecvMsg();
    VOID OnRecvMsg(CHAR *szMsg);

    VOID SendHeartBeatMsg();

    VOID OnGetHeartBeatRsp(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnGetLineInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnGetATSInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnGetPARecCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnStartTrainRecPAReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnStartTrainRealPAReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnStopTrainRealPAReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnPushTrainRealPAAudioReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
};

BOOL SendNOCCMsg(const CHAR *szMsgName, UINT32 ulSeqID, CHAR *szMsgInfo);

//---------------------------------------------------------------------------
#endif
