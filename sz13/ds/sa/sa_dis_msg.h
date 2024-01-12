#ifndef SA_DIS_MSG_H
#define SA_DIS_MSG_H

extern BOOL    g_bGetDISCfg;

VOID OnDCAcceptIPHTalkRsp(CHAR *szMsgInfo, GJson &Json);
VOID OnDCStopIPHTalkRsp(CHAR *szMsgInfo, GJson &Json);
VOID OnDCStartRecBrdReq(CHAR *szMsgInfo, GJson &Json);
VOID OnDISHeartBeatReq(CHAR *szMsgInfo, GJson &Json);
VOID OnDCGetNTPCfgRsp(CHAR *szMsgInfo, GJson &Json);
VOID OnDISGetCfgReq(CHAR *szMsgInfo, GJson &Json);
VOID OnDCGetRecBrdCfgReq();
VOID OnOtherDISMsg(DTP_MSG_HDR_T *pstMsg);
VOID OnDCStartRealBrdReq(CHAR *szMsgInfo, GJson &Json);
VOID OnDCSendRealBrdAudioDataReq(CHAR *szMsgInfo, GJson &Json);
VOID OnDCStopRealBrdReq(CHAR *szMsgInfo, GJson &Json);
VOID OnDISMsg(DTP_MSG_HDR_T *pstMsg);


BOOL SendDISMsg(const CHAR *szMsg);
BOOL SendDISMsg(const CHAR *szMsgName, UINT32 ulSeqID, GJsonParam &Param);
BOOL SendDISMsg(const CHAR *szMsgName, UINT32 ulSeqID, GJsonTupleParam &Param);
BOOL SendSAApplyIPHTalkReqMsg(IPH_TALK_APPLY_REQ_T *pstReq);
BOOL SendSAStopIPHTalkReqMsg(IPH_TALK_STOP_REQ_T *pstReq);
BOOL SendSAStopRealBrdRspMsg(PA_STOP_REAL_BRD_RSP_T *pstRsp);
BOOL SendSAStartRealBrdRspMsg(PA_REAL_BRD_RSP_T *pstRsp);
BOOL SendSAPAHeartBeatRspMsg(PA_GET_SYS_STATUS_RSP_T *pstRsp);
BOOL SendSAIPHHeartBeatRspMsg(IPH_GET_SYS_STATUS_RSP_T *pstRsp);
BOOL SendSAStartRecBrdRspMsg(PA_REC_BRD_RSP_T *pstRsp);
VOID SendDCGetRecBrdCfgRspMsg();

VOID SendDISGetCfgMsg();
VOID SendDISHeartBeatMsg();

VOID CheckResetButton();

VOID GetSACfg();

#endif