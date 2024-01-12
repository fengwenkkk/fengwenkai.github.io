#ifndef ClientCommonH
#define ClientCommonH

void InitTreeListPos(TcxTreeList *tl);

BOOL HasOMTType(UINT32 ulOMTType);
UINT32 GetMsgSeqID();

/*
BOOL SyncSendServer(UINT16 usMsgID, GJsonParam *ReqJsonParam, GJson *RspJson, UINT32 ulTimeout=5);
BOOL SyncSendServer(UINT16 usMsgID, GJsonParam *ReqJsonParam, GJson &RspJson, UINT32 ulTimeout=5);
BOOL SyncSendServer(UINT16 usMsgID, GJsonParam &ReqJsonParam, GJson *RspJson, UINT32 ulTimeout=5);
BOOL SyncSendServer(UINT16 usMsgID, GJsonParam &ReqJsonParam, GJson &RspJson, UINT32 ulTimeout=5);
BOOL SyncSendServer(UINT16 usMsgID, GJsonParam &ReqJsonParam, UINT32 ulTimeout=5);*/
BOOL SyncSendServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &MsgInfoParam, GJson &RspJson, UINT32 ulTimeout=5);
BOOL SyncSendServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonTupleParam &ReqJsonParam, UINT32 ulTimeout=5);
BOOL SyncSendServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &ReqJsonParam, UINT32 ulTimeout=5);

BOOL SendServer(UINT16 usMsgID, const CHAR *szMsgName);
BOOL SendServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &Param);
BOOL SendServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonTupleParam &Param);
BOOL SendServer(UINT16 usMsgID, const CHAR *szMsgName, UINT32 ulSeqID, GJsonParam &Param);
BOOL SendServer(UINT16 usMsgID, const CHAR *szMsgName, UINT32 ulSeqID, GJsonTupleParam &Param);

VOID DeleteConfirmRequest(UINT32 ulSeqID);
VOID SendConfirmRequest();

//VOID SaveNewSDS(CHAR *szUserID, CHAR *szUserName, CHAR *szSDSInfo);

#endif
