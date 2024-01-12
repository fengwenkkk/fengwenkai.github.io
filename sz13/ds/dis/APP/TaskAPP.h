#ifndef TASK_APP_H
#define TASK_APP_H

#include "DISPublic.h"

typedef struct
{
    UINT32          ulSDSIndex;
    UINT32          ulUserID;
    UINT32          ulSendTime;
    INT32           iCount;
}AUTO_SDS_REPLY_INFO_T;

class TaskAPP: public GBaseTask
{
public:
    TaskAPP(UINT16 usDispatchID);
    virtual ~TaskAPP();

    BOOL Init();
    virtual VOID TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);

private:
    CHAR        m_acMsg[64*1024];
    UINT32      m_ulAPPSeqID;
//    BOOL        m_bInitAppCfg;
    BOOL        m_bInitRegisterInfo;
    UINT32      m_ulDCUserID;
    CHAR        m_acDCOperatorID[32];

    CfgDao      *m_pDao;

    SOCKET      m_stAppSock;

    VOID AddOperLog(const CHAR *szLog, ...);

    VOID LogAPPMsg(const CHAR *szMsgName, const CHAR *szMsg);

    UINT32 GetReqCallDCType(UINT32 ulDCType, UINT32 ulTrainUnitID);

    BOOL SendDC(UINT32 ulDCUserID, UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &Param);
    BOOL SendAPPRsp(const CHAR *szMsgName, CHAR *szMsg);
    BOOL SendAPPReq(const CHAR *szMsgName, UINT32 ulUserID, GJsonParam &Param);
    BOOL SendAPPReq(const CHAR *szMsgName, UINT32 ulUserID, UINT32 ulSeqID, CHAR *szMsg);

    virtual VOID OnServerDisconnectClient(UINT16 usClientID);

    BOOL InitAPPCfg();
    BOOL DirectSendToApp(UINT16 usMsgType, CHAR *szMsg, UINT32 ulMsgLen);

    VOID CheckRegister();
    VOID OnCheckTimer();

    VOID SaveCellInfo(CELL_INFO_T &stInfo);
    VOID SaveCellInfoToDB();

    VOID OnAppDisReq(CHAR *szMsg, UINT32 ulMsgLen);
    VOID OnAppGetDSCfgInfoReq(CHAR *szMsg, GJson &Json);
    VOID OnAppGetNTPCfgReq(CHAR *szMsgInfo, GJson &Json);
    VOID OnAppSetTrainGroupReq(CHAR *szMsg, GJson &Json);
    VOID OnAppRegisterReq(CHAR *szMsg, GJson &Json);
    VOID OnAppUnregisterReq(CHAR *szMsg, GJson &Json);
    VOID OnAppRequestCallReq(CHAR *szMsg, GJson &Json);
    VOID OnAppGetATSInfoReq(CHAR *szMsg, GJson &Json);
    VOID OnAppGetStationATSInfoReq(CHAR *szMsg, GJson &Json);
    VOID OnAppReportTrainInfo(CHAR *szMsg, GJson &Json);
    VOID OnAppSetTrainPosReq(CHAR *szMsg, GJson &Json);
    VOID OnAppBroadcastStartRsp(CHAR *szMsg, GJson &Json);
    VOID OnAppSDSReplyReq(CHAR *szMsg, GJson &Json);
    VOID OnAppAutoSDSReplyRsp(CHAR *szMsg, GJson &Json);
    VOID OnAppEmergencyCallReq(CHAR *szMsg, GJson &Json);

    VOID OnAppDisDcRsp(CHAR *szMsg, UINT32 ulMsgLen);
    VOID OnAppDisDcRequestCallRsp(GJson &Json);
    VOID OnAppDisDcRegisterRsp(GJson &Json);
    VOID OnAppDisDcUnregisterRsp(GJson &Json);

    // Çëºô
    VOID OnDCGetReqCallInfo(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCApplyReqCall(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCCloseReqCall(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);
    VOID OnDCEmergencyCallRsp(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);

    VOID OnDcDisAppReq(CHAR *szMsg, UINT32 ulMsgLen);
    VOID OnDCSetTrainGroupReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSetTrainPosReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCBroadcastStartReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCBroadcastEndReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDCSetDynamicGroupReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json);

};

#endif
