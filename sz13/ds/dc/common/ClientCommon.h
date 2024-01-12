#ifndef ClientCommonH
#define ClientCommonH

void InitTreeListPos(TcxTreeList *tl);

UINT32 GetMsgSeqID();

BOOL SyncSendServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &MsgInfoParam, GJson &RspJson, UINT32 ulTimeout=5);
BOOL SyncSendServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonTupleParam &ReqJsonParam, UINT32 ulTimeout=5);
BOOL SyncSendServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &ReqJsonParam, UINT32 ulTimeout=5);

BOOL SyncSendMrsServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &MsgInfoParam, GJson &RspSubJson, UINT32 ulTimeout=5);
BOOL SyncSendMrsServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &MsgInfoParam, UINT32 ulTimeout=5);
BOOL SyncSendMrsServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonTupleParam &MsgInfoParam, UINT32 ulTimeout=5);

BOOL SendServer(UINT16 usMsgID, const CHAR *szMsgName);
BOOL SendServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &Param);
BOOL SendServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonTupleParam &Param);
BOOL SendServer(UINT16 usMsgID, const CHAR *szMsgName, UINT32 ulSeqID, GJsonParam &Param);
BOOL SendServer(UINT16 usMsgID, const CHAR *szMsgName, UINT32 ulSeqID, GJsonTupleParam &Param);

BOOL SendMrsServer(UINT16 usMsgID, const CHAR *szMsgName);
BOOL SendMrsServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &Param);
BOOL SendMrsServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonTupleParam &Param);
BOOL SendMrsServer(UINT16 usMsgID, const CHAR *szMsgName, UINT32 ulSeqID, GJsonParam &Param);
BOOL SendMrsServer(UINT16 usMsgID, const CHAR *szMsgName, UINT32 ulSeqID, GJsonTupleParam &Param);

VOID DeleteConfirmRequest(UINT32 ulSeqID);
VOID SendConfirmRequest();

extern AnsiString EncodeSMSMsg(AnsiString strMsgInfo);

VOID SaveP2PCallInfo(UINT32 ulCallType, UINT32 ulCaller, UINT32 ulCallee, UINT32 ulStatus);
VOID SaveP2PVideoCallInfo(UINT32 ulCallType, UINT32 ulCaller, UINT32 ulCallee, UINT32 ulStatus);
VOID SaveGroupCallInfo(UINT32 ulCallType, INT64 i64GroupID, UINT32 ulSpeaker);
VOID SaveRealBrdInfo(UINT32 ulStartTime, UINT32 ulEndTime, CHAR *szTrainList);
VOID SaveGroupInfo(INT64 i64GroupID, CHAR *szGroupName, UINT32 ulFuncType);
VOID SaveUESDSReply(CHAR *szUserID, UINT32 ulSDSID);
VOID SaveUESDSRecv(CHAR *szReceiver, UINT32 ulSDSID, BOOL bSendSucc);
VOID SaveNewSDS(CHAR *szUserID, CHAR *szUserName, CHAR *szSDSInfo);
VOID SaveNewSMS(CHAR *szUserID, CHAR *szUserName, CHAR *szSDSInfo, CHAR *szFile);
VOID SaveDCOperator(CHAR *szOperator, BOOL bOffDuty);
VOID SaveGISInfo(UINT32 ulUserID, DOUBLE dLongtitude, DOUBLE dLatitude);
VOID SaveMultiGroupInfo(VECTOR<SDK_MULTI_GROUP_INFO_T> &vGroup);

VOID SaveOperLog(const char *szOperLog, ...);

void DCRelogin();

#endif
