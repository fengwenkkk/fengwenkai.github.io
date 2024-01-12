#ifndef ClientInitH
#define ClientInitH

#define DEFAULT_MDC_PORT    5060
#define DEFAULT_DIS_PORT    GOS_DEFAULT_SERVER_PORT

typedef struct
{
    UINT8       aucOMCAddr[4];
    UINT16      usOMCPort;

    CHAR        acUserName[32];
    CHAR        acUserPwd[32];
    UINT32      ulOmtUserType;
    BOOL        bTCPMode;
}LOCAL_CFG_T;

typedef struct
{
    UINT8   aucNTPAddr[4];
    UINT16  usNTPPort;
    UINT32  ulNTPSyncPeriod;
}NTP_CFG_T;

extern LOG_CFG_T        g_stLogCfg;
extern LOCAL_CFG_T      g_stLocalCfg;
//extern RemoteMsg      *g_pRemoteMsg;
extern UINT32           g_ulOperType;
extern VectorMap<AnsiString, NE_BASIC_INFO_T>   g_mNEBasicInfo;
//extern VectorMap<AnsiString,    ALARM_INFO_T>        g_mActiveAlarmInfo;
extern AnsiString   g_strActionButtonSkinName;
extern AnsiString   g_strStatusbarButtonSkinName;
extern AnsiString   g_strToolbarButtonSkinName;
extern AnsiString   g_strLeftToolbarButtonSkinName;
extern AnsiString   g_strRightToolbarButtonSkinName;
extern AnsiString   g_strDownButtonSkinName;

extern TColor       g_TreeNodeCheckedColor;

extern int          g_iMidToolbarButtonWidth;
extern int          g_iMidToolbarButtonHeight;

VOID SaveLocalConfInfo();

UINT32 GetAutoLockPeriod();
VOID SetAutoLockPeriod(UINT32 ulValue);

BOOL InitClientEnv();
BOOL InitRemoteMsg();

BOOL LoadCfgData();
BOOL LoadHistoryAlarm();

BOOL LoadOmtOperatorInfo();
VOID RefreshNEBasicInfo();
BOOL LoadNEBasicInfo();
BOOL LoadActiveAlarmInfo();
VOID RefreshActiveAlarmInfo();
BOOL LoadAlarmLevelCfg();
AnsiString GetNeIDByDevName(AnsiString strNeName);
AnsiString GetDevNameByNeID(AnsiString strNeID);
VOID RefreshAlarmLevelCfg();
BOOL LoadStationInfo();
VOID RefreshStationInfo();
BOOL LoadNeInfo(GJsonParam &ReqJsonParam,GJson &RspJson);
VOID RefreshServerStat();
BOOL LoadOperLogInfo(UINT32 ulStartTime, UINT32 ulEndTime, CHAR *szOperatorID, VECTOR<OMT_OPER_LOG_INFO_T> &vInfo);
#endif
