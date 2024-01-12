#ifndef ClientInitH
#define ClientInitH

#define DEFAULT_MDC_PORT    5060
#define DEFAULT_DIS_PORT    GOS_DEFAULT_SERVER_PORT

extern RemoteMsg        *g_pRemoteMsg;
extern RemoteMsg        *g_pMRSRemoteMsg;

VOID SaveLocalConfInfo();

BOOL InitClientEnv();
BOOL InitRemoteMsg();
BOOL InitMRSRemoteMsg();
BOOL LoadCfgData();

VOID ReloadCfgData(AnsiString strCfgType);

typedef BOOL (*LOAD_CFG_FUNC)(CHAR *szMsgInfo);

BOOL ReloadCfgData(GJson &Json);
VOID OnLoadCfgDataRsp(AnsiString strMsgType, UINT32 ulCfgVer, CHAR *szCfgData);
VOID GetLoadCfgDataType(VECTOR<CHAR*> &vCfgType);

BOOL LoadAPPCfg(CHAR *szMsgInfo=NULL);
BOOL LoadGroupCfg(CHAR *szMsgInfo=NULL);
BOOL LoadDCUserInfo(CHAR *szMsgInfo=NULL);
BOOL LoadDCOperatorInfo(CHAR *szMsgInfo=NULL);
BOOL LoadStationUEInfo(CHAR *szMsgInfo=NULL);
BOOL LoadTrainUEInfo(CHAR *szMsgInfo=NULL);
BOOL LoadPortalUEInfo(CHAR *szMsgInfo=NULL);
BOOL LoadGroupInfo(CHAR *szMsgInfo=NULL);
BOOL LoadAutoJoinGroupInfo(CHAR *szMsgInfo=NULL);
BOOL LoadSubGroupInfo(CHAR *szMsgInfo=NULL);
BOOL LoadSubVideoGroupInfo(CHAR *szMsgInfo=NULL);
BOOL LoadSubUserInfo(CHAR *szMsgInfo=NULL);
BOOL LoadSubVideoUserInfo(CHAR *szMsgInfo=NULL);
BOOL LoadMultiGroupInfo(CHAR *szMsgInfo=NULL);
BOOL LoadTrainInfo(CHAR *szMsgInfo=NULL);
BOOL LoadDepotInfo(CHAR *szMsgInfo=NULL);
BOOL LoadStationInfo(CHAR *szMsgInfo=NULL);
BOOL LoadBrdZoneInfo(CHAR *szMsgInfo=NULL);
BOOL LoadSectionInfo(CHAR *szMsgInfo=NULL);
BOOL LoadDestInfo(CHAR *szMsgInfo=NULL);
BOOL LoadPredefinedSMS(CHAR *szMsgInfo=NULL);
BOOL LoadSDSTemplate(CHAR *szMsgInfo=NULL);
BOOL LoadRecBrdCfg(CHAR *szMsgInfo=NULL);

BOOL LoadSDSInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulSDSType,
                 CHAR *szSendUserName, CHAR *szKeyword, VECTOR<SDS_INFO_T> &vInfo);

BOOL ParseTrainInfo(GJson *pSubJson, TRAIN_INFO_T &stInfo);

BOOL LoadP2PCallInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulCallType, UINT32 ulUserID, VECTOR<P2P_CALL_INFO_T> &vInfo);
BOOL LoadGroupCallInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulCallType, UINT32 ulDCUserID, INT64 i64GroupID, VECTOR<GROUP_CALL_INFO_T> &vInfo);
BOOL LoadReqCallInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulReqCallType, VECTOR<REQ_CALL_INFO_T> &vInfo);
BOOL LoadRecBrdInfo(UINT32 ulStartTime, UINT32 ulEndTime, VECTOR<REC_BRD_INFO_T> &vInfo);
BOOL LoadRealBrdInfo(UINT32 ulStartTime, UINT32 ulEndTime, VECTOR<REAL_BRD_INFO_T> &vInfo);
BOOL LoadIPHTalkInfo(UINT32 ulStartTime, UINT32 ulEndTime, VECTOR<IPH_TALK_INFO_T> &vInfo);
BOOL LoadOperLogInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulDCUserID, VECTOR<OPER_LOG_INFO_T> &vInfo);

#endif
