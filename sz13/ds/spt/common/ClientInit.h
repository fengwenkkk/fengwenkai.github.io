#ifndef ClientInitH
#define ClientInitH

#define DEFAULT_MDC_PORT    5060
#define DEFAULT_DIS_PORT    GOS_DEFAULT_SERVER_PORT

typedef struct
{
 // UINT8       aucMDCAddr[4];
 // UINT16      usMDCPort;
 // UINT8       aucMRSAddr[4];
 // CHAR        acUserName[32];
 // CHAR        acUserPwd[32];

    UINT8       aucServerAddr[4];
    UINT16      usServerPort;
    BOOL        bTCPMode;
}LOCAL_CFG_T;

extern LOG_CFG_T        g_stLogCfg;
extern LOCAL_CFG_T      g_stLocalCfg;
extern RemoteMsg        *g_pRemoteMsg;

//extern VECTOR<DC_USER_INFO_T>             g_vDCUserInfo;
//extern VECTOR<DC_OPERATOR_INFO_T>         g_vDCOperatorInfo;

extern AnsiString   g_strActionButtonSkinName;
extern AnsiString   g_strStatusbarButtonSkinName;
extern AnsiString   g_strToolbarButtonSkinName;
extern AnsiString   g_strLeftToolbarButtonSkinName;
extern AnsiString   g_strRightToolbarButtonSkinName;
extern AnsiString   g_strDownButtonSkinName;

extern TColor       g_TreeNodeCheckedColor;

//extern DC_USER_INFO_T *g_pstCurrDCUser;

extern BOOL         g_bShowCallWin;
extern BOOL         g_bDCLoginMode;

extern int          g_iMidToolbarButtonWidth;
extern int          g_iMidToolbarButtonHeight;

extern AnsiString   g_strDCOperatorID;
extern AnsiString   g_strDCOperatorName;


bool IsDepotMode();
bool IsSameDepot(UINT32 ulDepotID);

VOID SaveLocalConfInfo();

BOOL InitClientEnv();
BOOL InitRemoteMsg();
BOOL LoadCfgData();
BOOL LoadDCUserInfo(UINT32 ulTimeout);
BOOL LoadStationInfo();
BOOL LoadBrdZoneInfo();
BOOL LoadStationUEInfo();
BOOL LoadTrainUEInfo();
BOOL LoadPortalUEInfo();
BOOL LoadTrainInfo();
BOOL LoadPredefinedSMS();
BOOL LoadSDSTemplate();
BOOL LoadSDSInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulSDSType,
                 CHAR *szSendUserName, CHAR *szKeyword, VECTOR<SDS_INFO_T> &vInfo);

BOOL ParseTrainInfo(GJson *pSubJson, TRAIN_INFO_T &stInfo);

BOOL LoadP2PCallInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulCallType, UINT32 ulUserID, VECTOR<P2P_CALL_INFO_T> &vInfo);
BOOL LoadGroupCallInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulCallType, UINT32 ulDCUserID, INT64 i64GroupID, VECTOR<GROUP_CALL_INFO_T> &vInfo);
BOOL LoadReqCallInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulReqCallType, VECTOR<REQ_CALL_INFO_T> &vInfo);
BOOL LoadRecBrdInfo(UINT32 ulStartTime, UINT32 ulEndTime, VECTOR<REC_BRD_INFO_T> &vInfo);
BOOL LoadIPHTalkInfo(UINT32 ulStartTime, UINT32 ulEndTime, VECTOR<IPH_TALK_INFO_T> &vInfo);
BOOL LoadOperLogInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulDCUserID, VECTOR<OPER_LOG_INFO_T> &vInfo);

#endif
