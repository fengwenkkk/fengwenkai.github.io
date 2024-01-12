#ifndef ClientCommonH
#define ClientCommonH

void InitTreeListPos(TcxTreeList *tl);

BOOL HasDCType(UINT32 ulDCType);

UINT32 GetMsgSeqID();

void AppRelogin();

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

#endif
