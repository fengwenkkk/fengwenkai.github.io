#ifndef TASK_SA_H
#define TASK_SA_H

#include "DISPublic.h"

typedef struct
{
    UINT32      ulTrainUnitID;
    CHAR        acSAID[64];
    PID_T       stSAPID;
    UINT32      ulUpdateTime;
}SA_PID_T;

typedef struct
{
    UINT32      ulRecID;
    UINT32      ulBrdSeqID;
    UINT32      ulBrdLoopTime;
    UINT32      ulTrainUnitID;
    UINT32      ulSendUptime;
}REC_BRD_SEND_INFO_T;

typedef struct
{
    UINT32      ulBrdSeqID;
    UINT32      ulDCUserID;
    UINT32      ulTrainUnitID;
    UINT32      ulSendUptime;
}REAL_BRD_SEND_INFO_T;

//管理PAIndex与BrdSeqID对应关系，管理本地BrdSeqID和DCUserID的对应关系
typedef struct
{
    BOOL        bIsRealBrd;     // 是否是实时广播
    UINT32      ulNOCCPAIndex;  // NOCC广播序号
    UINT32      ulBrdSeqID;     // 本地广播序号
    UINT32      ulMsgSeqID;
    UINT32      ulDCUserID;     // 本地广播调度台号码,0表示NOCC广播
    UINT32      ulStartTime;    // 开始时间（uptime）
}BRD_MAP_INFO_T;

class TaskSA: public GBaseTask
{
public :
    TaskSA(UINT16 usDispatchID);

    BOOL Init();
    virtual VOID TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);

private:
    CHAR        m_acMsg[64*1024];
    CfgDao      *m_pDao;
    UINT32      m_ulDCUserID;

    virtual VOID OnServerDisconnectClient(UINT16 usClientID);
    BOOL SendTrainSA(const CHAR *szMsgName, UINT32 ulTrainUnitID, GJsonParam &Param);
    BOOL SendTrainSA(UINT32 ulTrainUnitID, CHAR *szReq);

    VOID NotifyIPHTalkInfo();

    VOID OnCheckTimer();
    VOID OnSAReq(CHAR *szMsg, UINT32 ulMsgLen);
    VOID OnDCDISSAReq(CHAR *szMsg, UINT32 ulMsgLen);
    VOID OnDCAcceptIPHTalkReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCPauseIPHTalkReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json); // DC挂起IPH业务
    VOID OnDCStopIPHTalkReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCStartRecBrdReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCGetActiveIPHTalkReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCGetRecBrdCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnSAIPHApplyACKMsg(CHAR *szMsgInfo, GJson &Json);
    VOID OnSAPauseIPHTalkRsp(CHAR *szMsgInfo, GJson &Json);
    VOID OnSAIPHDevResetReq(CHAR *szMsgInfo, GJson &Json);

    BOOL SendDC(UINT32 ulDCUserID, UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &Param);

    VOID OnSAHeartBeatReq(CHAR *szMsgInfo, GJson &Json);
    VOID OnSAGetNTPCfgReq(CHAR *szMsgInfo, GJson &Json);
    VOID OnSAGetCfgReq(CHAR *szMsgInfo, GJson &Json);
    VOID OnSAApplyIPHTalkReq(CHAR *szMsgInfo, GJson &Json);
    VOID OnSAStopIPHTalkReq(CHAR *szMsgInfo, GJson &Json);
    VOID OnSAStartRecBrdRsp(CHAR *szMsgInfo, GJson &Json);
    VOID OnSAGetRecBrdCfgRsp(CHAR *szMsgInfo, GJson &Json);
    VOID OnSAStartRealBrdRsp(CHAR *szMsgInfo, GJson &Json);
    VOID OnSAStopRealBrdRsp(CHAR *szMsgInfo, GJson &Json);

    VOID OnDCGetMaxRealBrdSeqIDReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCStartRealBrdReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCStopRealBrdReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDCSendRealBrdAudioDataReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json); /// 处理调度台人工广播语音业务
    VOID OnDCSendIPHAudioDataReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json); ///处理调度台IPH语音业务
    VOID OnSAUpLoadIPHAudioReq(CHAR *szMsgInfo, GJson &Json);

    VOID CheckSAPID();
    VOID SaveSAPID(UINT32 ulTrainUnitID, CHAR *szSAID, PID_T *pstPID);
    VOID GetSAPID(UINT32 ulTrainUnitID, VECTOR<PID_T> &vPID);

    VOID NewRecBrdInfo(REC_BRD_SEND_INFO_T &stInfo);
    VOID DelRecBrdInfo(UINT32 ulTrainUnitID, UINT32 ulBrdSeqID);
    VOID CheckRecBrdInfo();

    VOID NewRealBrdInfo(REAL_BRD_SEND_INFO_T &stInfo);
    VOID DelRealBrdInfo(/*UINT32 ulTrainUnitID, */UINT32 ulBrdSeqID);
    VOID GetRealBrdTrains(UINT32 ulBrdSeqID, VECTOR<UINT32> &vTrainUnitID);
    VOID CheckRealBrdInfo();

    BOOL CheckIPHTalkInfo();
    IPH_TALK_INFO_T* AcceptIPHTalk(UINT32 ulTrainUnitID, UINT32 ulCarriageID, UINT32 ulIPHDevID, UINT32 ulDCUserID);
    UINT32 StopIPHTalk(UINT32 ulTrainUnitID, UINT32 ulCarriageID, UINT32 ulIPHDevID, IPH_TALK_INFO_T *pstInfo);
    BOOL   StopIPHTalk(UINT32 ulTrainUnitID, UINT32 ulCarriageID, UINT32 ulIPHDevID);
    BOOL   CleanIPHTalk(UINT32 ulTrainUnitID, VECTOR<IPH_TALK_INFO_T> &vIPHInfo);
    BOOL NewIPHTalk(IPH_TALK_INFO_T &stApplyInfo);
    VOID PrintIPHInfo();

    UINT32 GetRecBrdSeqID(BOOL bIsNOCCBrd);
    UINT32 GetRealBrdSeqID(BOOL bIsNOCCBrd);

    VOID OnNOCCReq(CHAR *szMsg, UINT32 ulMsgLen);
   // VOID OnNOCCGetPARecCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnNOCCStartTrainRecPAReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnNOCCStartTrainRealPAReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnNOCCStopTrainRealPAReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnNOCCPushTrainRealPAAudioReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
};

#endif
