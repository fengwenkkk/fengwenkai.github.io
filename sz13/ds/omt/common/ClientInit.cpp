#include <vcl.h>
#include <Registry.hpp>

#include "ClientInclude.h"
#include "ds_public.h"
#include "pis.h"
#include "msg.h"
#include "main.h"
#include "GTransFile.h"

#define LOAD_DATA_CHECK(szLoadDisplayName, szLoadName, LoadFunc)\
{\
    SetInitInfo(szLoadDisplayName);\
    if (!LoadFunc())               \
    {                              \
        GosLog(LOG_FATAL, "Load %s failed!", szLoadName);\
        return FALSE;                                    \
    }\
    GosLog(LOG_FATAL, "Load %s succ", szLoadName);\
}

LOCAL_CFG_T     g_stLocalCfg = {0};
LOG_CFG_T       g_stLogCfg = {0};
RemoteMsg       *g_pRemoteMsg = NULL;
CHAR            g_acLocalCfgFile[1024] = {0};
NTP_CFG_T       g_stNTPCfg = {{0,0,0,0}, 123, 60};

VECTOR<OMC_SW_INFO_T>           g_vSwInfo;
VECTOR<OMC_SW_UPDATE_INFO_T>    g_vSwUpdateInfo;
//VECTOR<OMT_USER_INFO_T>           g_vOmtOperatorInfo; //ZWJ
VECTOR<OMT_OPERATOR_INFO_T>         g_vOmtOperatorInfo;

UINT32      g_usLocalTransFilePort = DEFAULT_CLIENTT_TRANS_FILE_PORT; // 本地transfile缺省端口
UINT32      g_usTransFilePort;  // 服务器transfile_port，启动时从服务器获取
UINT16      g_usFtpPort;
CHAR        g_acFtpVerDir[256];
CHAR        g_acFtpLogFileDir[256];
CHAR        g_acFtpUser[32];
CHAR        g_acFtpPwd[32];
CHAR        g_acOMCVersion[32];
CHAR        *g_szConfigFile = "omt.ini";
CHAR        *g_szNeBasicInfoFile = "NeBasicInfo.ini";

AnsiString  g_strVolumeSkinName = "Office2010Silver";
AnsiString  g_strStatusbarButtonSkinName = "DevExpressDarkStyle";
AnsiString  g_strToolbarButtonSkinName = "GlassOceans";
AnsiString  g_strLeftToolbarButtonSkinName = "GlassOceans";
AnsiString  g_strRightToolbarButtonSkinName = "Office2013DarkGray";
AnsiString  g_strActionButtonSkinName = "MoneyTwins";
AnsiString  g_strDownButtonSkinName = "Office2013DarkGray";

VectorMap<AnsiString, NE_BASIC_INFO_T>        g_mNEBasicInfo;
VectorMap<AnsiString,    ALARM_INFO_T>        g_mActiveAlarmInfo;
VectorMap<UINT32,   ALARM_LEVEL_CFG_T>        g_mAlarmLevelCfg;
VectorMap<UINT32,   STATION_INFO_T>           g_mStationInfo;
int         g_iMidToolbarButtonWidth = 108;
int         g_iMidToolbarButtonHeight = 72;

TColor      g_TreeNodeCheckedColor = clSkyBlue;//clCream;//clWebDeepPink;

UINT8       g_aucOmcAddr[4] = {0};

UINT32     g_ulAutoLockPeriod = 36000;

extern UINT32 GetMaxInitNum();
extern void SetInitInfo(AnsiString strInitInfo);
extern void LoadSkinCfg();
extern BOOL g_bShowTitle;
extern void ClientConnectionChange(bool bConnected);

UINT32 GetAutoLockPeriod()
{
    return g_ulAutoLockPeriod;
}

VOID SetAutoLockPeriod(UINT32 ulValue)
{
    g_ulAutoLockPeriod = ulValue;
}

VOID LoadLocalConfInfo()
{
/*
    if (!LoadConfInfo("Param", "IPHRecMergeMode", &g_bIPHRecMergeMode))
    {
        g_bIPHRecMergeMode = TRUE;
    }*/
}

VOID SaveLocalConfInfo()
{
//  SaveConfInfo("Param", "IPHRecMergeMode", g_bIPHRecMergeMode);
}

BOOL GetLocalCfg()
{
    CHAR    acFile[256] = {0};
    CHAR    *szValue = NULL;
    INT32   iValue = 0;

    LoadLocalConfInfo();

    sprintf(acFile, "%s/%s", gos_get_root_path(), g_szConfigFile);

    if (!gos_file_exist(acFile))
    {
        ErrMsg("本地配置文件[%s]不存在!", acFile);
        return FALSE;
    }

    strcpy(g_acLocalCfgFile, acFile);

    GConfFile   Conf(acFile);

    szValue = Conf.Get("Sys", "omt_type");
    if (szValue)
    {
         gos_atou(szValue, &g_ulOperType);
    }

    if (Conf.Get("Sys", "user_name", g_stLocalCfg.acUserName, sizeof(g_stLocalCfg.acUserName)))
    {
        szValue = Conf.Get("Sys", "user_pwd");
        if (szValue)
        {
            DecryptString(szValue, g_stLocalCfg.acUserPwd);;
        }
    }

    szValue = Conf.Get("Sys", "omc_ip");
    if (!gos_get_ip_port(szValue, g_stLocalCfg.aucOMCAddr, &g_stLocalCfg.usOMCPort))
    {
        GosLog(LOG_ERROR, "get omc_ip of Sys failed");
        return FALSE;
    }

    if (g_stLocalCfg.usOMCPort == 0)
    {
        g_stLocalCfg.usOMCPort = DEFAULT_OMC_UDP_PORT;
    }

    g_stLocalCfg.bTCPMode = TRUE;
    szValue = Conf.Get("Sys", "tcp_mode");
    if (szValue && strcasecmp(szValue, "false") == 0)
    {
        g_stLocalCfg.bTCPMode = FALSE;
    }

    // TransFile port
    szValue = Conf.Get("Sys", "transfile_port");
    if (szValue && gos_atoi(szValue, &iValue))
    {
        if (iValue<=0 || iValue >= 0xffff)
        {
            GosLog(LOG_ERROR, "invalid transfile_port in %s", g_szConfigFile);
            return FALSE;
        }
        g_usLocalTransFilePort = iValue;
    }

    szValue = Conf.Get("Sys", "lock_window_time");
    if (szValue &&
        gos_atoi(szValue, &iValue))
    {
        if (iValue > 0 && iValue < 60)
        {
            iValue = 60;
        }
        else if (iValue > 86400*30 || iValue < 0)
        {
            iValue = 0;
        }
    }
    else
    {
        iValue = 0;
    }

    SetAutoLockPeriod(iValue);

    if (!Conf.Get("log", "log_level", LOG_INFO, &iValue))
    {
        GosLog(LOG_ERROR, "get log_level of Log failed");
        return FALSE;
    }

    if (iValue > LOG_FATAL)
    {
        iValue = LOG_FATAL;
    }
    else if (iValue < LOG_NULL)
    {
        iValue = LOG_NULL;
    }

    g_stLogCfg.bLogToStdout = FALSE;
    g_stLogCfg.bLogToFile = TRUE;
    g_stLogCfg.ulLogLevel = iValue;

    // log_file_num
    if (!Conf.Get("log", "log_file_num", 90, &iValue))
    {
        GosLog(LOG_ERROR, "get log_file_num of Log failed");
        return FALSE;
    }

    if (iValue < 2)
    {
        iValue = 2;
    }
    else if (iValue > 100)
    {
        iValue = 100;
    }

    g_stLogCfg.ulLogFileNum = iValue;

    // log_file_size
    if (!Conf.Get("log", "log_file_size", 4096, &iValue))
    {
        GosLog(LOG_ERROR, "get log_file_size of Log failed");
        return FALSE;
    }

    if (iValue < 1024)
    {
        iValue = 1024;
    }
    else if (iValue > 102400)
    {
        iValue = 102400;
    }

    g_stLogCfg.ulLogFileSize = 1024*iValue;

    return TRUE;
}

BOOL InitRemoteMsg()
{
    static BOOL bConnect  = FALSE;

    if (bConnect)
    {
        return TRUE;
    }

    /* 初始化远程消息模块 */
    g_pRemoteMsg = RemoteMsg::New(g_stLocalCfg.aucOMCAddr, g_stLocalCfg.usOMCPort, g_stLocalCfg.bTCPMode);
    if (!g_pRemoteMsg)
    {
        GosLog(LOG_FATAL, "Get Remote Msg failed!");

        return FALSE;
    }

//  g_pRemoteMsg->SetClientType(DIS_CLIENT_DC);
    g_pRemoteMsg->SetClientConnectionChange(ClientConnectionChange);

    for (int i=0; i<100; i++)
    {
        if (g_pRemoteMsg->IsConnected())
        {
            break;
        }

        gos_sleep_1ms(100);
    }

    if (!g_pRemoteMsg->IsConnected())
    {
        GosLog(LOG_FATAL, "Connect to server failed!");

        return FALSE;
    }

    bConnect = TRUE;

    return TRUE;
}

BOOL InitClientEnv()
{
    /* dev express时间格式设置 */
    TcxFormatController     *FormatController = cxFormatController();

    FormatController->UseDelphiDateTimeFormats = true;

    /* 全局设置 */
    if (dxFormatSettings)
    {
        dxFormatSettings->DateSeparator = '-';
        dxFormatSettings->ShortDateFormat = "yyyy-MM-dd";
        //dxFormatSettings->ShortTimeFormat = "hh:mm:ss";
        //dxFormatSettings->LongTimeFormat = "HH:mm:ss";
    }

    gos_init_socket();

    GosInitLog();

    if (!GetLocalCfg())
    {
        GosLog(LOG_FATAL, "Get local Cfg failed!");

        return FALSE;
    }

    GosInitLog(&g_stLogCfg);

    LoadSkinCfg();
    InitSystemFont();
    gos_init_svcenv();

    /* 日志 */
    GLogFactory     *pLogFactory = GLogFactory::GetInstance();

    if (!pLogFactory)
    {
        GosLog(LOG_FATAL, "Get LogFactory failed!");

        return FALSE;
    }

    Logger *pLogger = pLogFactory->GetDefaultLogger();
    if (!pLogger)
    {
        GosLog(LOG_FATAL, "Get Logger failed!");

        return FALSE;
    }

    pLogger->SetLogToStdout(FALSE);
    pLogger->SetLogToFile(g_stLogCfg.ulLogFileNum>0, g_stLogCfg.ulLogFileNum, g_stLogCfg.ulLogFileSize, 0);
    pLogger->SetLogLevel(g_stLogCfg.ulLogLevel);
    pLogger->SetLogModule(MODULE_DTP, MODULE_DTP_NAME);

    return TRUE;
}

// 加载数据
CHAR* LoadData(CHAR *szDataType, GJsonParam *pJsonParam, GJson *pRspJson, UINT32 ulTimeout=10)
{
    UINT32                  ulRspLen = 0;
    CHAR                    *szReq;
    UINT32                  ulBufLen = 64*1024;
    CHAR                    *szRsp = NULL;
    GJsonParam              ReqParam;

    if (pRspJson)
    {
        szRsp = (CHAR*)calloc(1, ulBufLen);
        if (!szRsp)
        {
            return NULL;
        }
    }

    ReqParam.Add("MsgName", szDataType);
    ReqParam.Add("UserID", g_stLocalCfg.acUserName);
    ReqParam.Add("MsgSeqID", GetMsgSeqID());
    if (!pJsonParam)
    {
        GJsonParam  EmptyJsonParam;

        ReqParam.Add("MsgInfo", EmptyJsonParam);
    }
    else
    {
        ReqParam.Add("MsgInfo", pJsonParam);
    }

    szReq = ReqParam.GetString();
    if (!pRspJson)
    {
        g_pRemoteMsg->SendReq(EV_OMT_OMC_REQ, szReq, strlen(szReq)+1);

        return NULL;
    }

    if(!g_pRemoteMsg->SyncSend(EV_OMT_OMC_REQ, szReq, strlen(szReq)+1,
                               szRsp, ulBufLen-1, &ulRspLen, ulTimeout))
    {
        GOS_FREE(szRsp);
        return NULL;
    }

    if (!pRspJson->Parse(szRsp))
    {
        GOS_FREE(szRsp);
        return NULL;
    }

	GOS_FREE(szRsp);

    CHAR    *szRetMsgType = pRspJson->GetValue("MsgName");

    if (strcmp(szDataType, szRetMsgType) != 0)
    {
		GosLog(LOG_ERROR, "invalid msg type(%s)", szRetMsgType);
        return FALSE;
    }

    CHAR *szMsgInfo = pRspJson->GetValue("MsgInfo");

    if (!szMsgInfo)
    {
        GosLog(LOG_ERROR, "no MsgInfo in msg %s", pRspJson->GetText());
        return NULL;
    }

    return szMsgInfo;
}

CHAR* LoadData(CHAR *szDataType, GJsonParam *pJsonParam, GJson &RspJson)
{
    return LoadData(szDataType, pJsonParam, &RspJson);
}

CHAR* LoadData(CHAR *szDataType, GJson &RspJson)
{
    return LoadData(szDataType, NULL, RspJson);
}

CHAR* LoadCfgData(CHAR *szDataType, GJsonParam &ReqParam, GJson *pRspJson, UINT32 ulTimeout=10)
{
    ReqParam.Add("UserID", g_stLocalCfg.acUserName);

    return LoadData(szDataType, &ReqParam, pRspJson, ulTimeout);
}

CHAR* LoadCfgData(CHAR *szDataType, GJsonParam &ReqParam, GJson &RspJson, UINT32 ulTimeout=10)
{
    return LoadCfgData(szDataType, ReqParam, &RspJson, ulTimeout);
}

CHAR* LoadCfgData(CHAR *szDataType, GJson &RspJson)
{
    GJsonParam  ReqParam;

    return LoadCfgData(szDataType, ReqParam, RspJson);
}

CHAR* LoadCfgData(CHAR *szDataType, GJson *pRspJson)
{
    GJsonParam  ReqParam;

    return LoadCfgData(szDataType, ReqParam, pRspJson);
}

VOID RefreshNEBasicInfo()
{
    GJsonParam  ReqParam;

    LoadCfgData(MSG_OMT_GetNeBasicInfo, ReqParam, NULL);
}

VOID RefreshAlarmLevelCfg()
{
    GJsonParam  ReqParam;
    GJson       RspJson;

    //LoadCfgData(MSG_OMT_GetAlarmLevelCfg, ReqParam, RspJson);

	LoadCfgData(MSG_OMT_GetAlarmLevelCfg, ReqParam, NULL);
}

VOID RefreshActiveAlarmInfo()
{
    GJsonParam  ReqParam;

    LoadCfgData(MSG_OMT_GetActiveAlarm , ReqParam, NULL);
}

VOID RefreshStationInfo()
{
    GJsonParam  ReqParam;

    LoadCfgData(MSG_OMT_GetStationInfo , ReqParam, NULL);

    return;
}

VOID RefreshServerStat()
{
    GJsonParam  ReqParam;

    LoadCfgData(MSG_OMT_GetClusterStatus , ReqParam, NULL);

    return;
}

//BOOL LoadNEBasicInfo()
//{
//  GJsonParam  ReqParam;
//  GJson       RspJson;
//  CHAR        *szMsgInfo;
//
//  szMsgInfo = LoadCfgData(MSG_OMT_GetNeBasicInfo, ReqParam, RspJson);
//  if (!szMsgInfo)
//  {
//      return FALSE;
//  }
//
//  GJson           MsgInfoJson;
//
//  if (!MsgInfoJson.Parse(szMsgInfo))
//  {
//      return FALSE;
//  }
//
//  GJson           *pSubJson;
//  VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
//  NE_BASIC_INFO_T stInfo = {0};
//  VectorMap<AnsiString, NE_BASIC_INFO_T>   mInfo;
//
//  for (UINT32 i=0; i<vSubJson.size(); i++)
//  {
//      pSubJson = vSubJson[i];
//
//      if (!pSubJson->GetValue("ne_id", stInfo.acNeID, sizeof(stInfo.acNeID)) ||
//          !pSubJson->GetValue("mac", stInfo.acDevMac, sizeof(stInfo.acDevMac)) ||
//          !pSubJson->GetValue("ip", stInfo.acDevIp, sizeof(stInfo.acDevIp)) ||
//          !pSubJson->GetValue("type", &stInfo.ulDevType) ||
//          !pSubJson->GetValue("name", stInfo.acDevName, sizeof(stInfo.acDevName)) ||
//          !pSubJson->GetValue("sw_ver", stInfo.acSoftwareVer, sizeof(stInfo.acSoftwareVer)) ||
//          !pSubJson->GetValue("hw_ver", stInfo.acHardwareVer, sizeof(stInfo.acHardwareVer)) ||
//          !pSubJson->GetValue("model", stInfo.acModelName, sizeof(stInfo.acModelName)) ||
////            !pSubJson->GetValue("online_time", &stInfo.ulOnlineTime) ||
//          !pSubJson->GetValue("online_sec", &stInfo.iOnlineSec)) /* ||
//          !pSubJson->GetValue("android_ver", stInfo.acAndroidVer,sizeof(stInfo.acAndroidVer)) ||
//          !pSubJson->GetValue("radio_ver"), stInfo.acRadioVer, sizeof(stInfo.acRadioVer))  */
//      {
//          GosLog(LOG_ERROR, "invalid msg format: %s", szMsgInfo);
//          return FALSE;
//      }
//
//      mInfo.Add(stInfo.acNeID, stInfo);
//  }
//
//  g_mNEBasicInfo.Clear();
//  g_mNEBasicInfo.AddAll(mInfo);
//
//  return TRUE;
//}

//BOOL LoadActiveAlarmInfo()
//{
//  GJsonParam  ReqParam;
//  GJson       RspJson;
//  CHAR        *szMsgInfo;
//
//  szMsgInfo = LoadCfgData(MSG_OMT_GetActiveAlarm, ReqParam, RspJson);
//  if (!szMsgInfo)
//  {
//      return FALSE;
//  }
//
//  GJson           MsgInfoJson;
//
//  if (!MsgInfoJson.Parse(szMsgInfo))
//  {
//      return FALSE;
//  }
//
//  GJson           *pSubJson;
//  VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
//  ALARM_INFO_T    stInfo = {0};
//  VectorMap<AnsiString, ALARM_INFO_T>    mInfo;
//
//  for (UINT32 i=0; i<vSubJson.size(); i++)
//  {
//      pSubJson = vSubJson[i];
//
//      if (!pSubJson->GetValue("ne_id", stInfo.acNeID, sizeof(stInfo.acNeID)) ||
//          !pSubJson->GetValue("alarm_id", &stInfo.ulAlarmID)      ||
//          !pSubJson->GetValue("level", &stInfo.ulAlarmLevel)      ||
//          !pSubJson->GetValue("type", &stInfo.ulAlarmType)        ||
//          !pSubJson->GetValue("reason", stInfo.acAlarmReason, sizeof(stInfo.acAlarmReason)) ||
//          !pSubJson->GetValue("time", &stInfo.ulAlarmRaiseTime)   ||
//          !pSubJson->GetValue("info", stInfo.acAlarmInfo, sizeof(stInfo.acAlarmInfo))
//         )
//      {
//          GosLog(LOG_ERROR, "parse alarm info failed : %s", pSubJson->GetText());
//          return FALSE;
//      }
//      mInfo.Add(stInfo.acNeID, stInfo);
//  }
//
//  g_mActiveAlarmInfo.Clear();
//  g_mActiveAlarmInfo.AddAll(mInfo);
//  return TRUE;
//
//}

BOOL LoadNEBasicInfo()
{
    GJsonParam  ReqParam;
    GJson       RspJson;
    CHAR        *szMsgInfo;
	CHAR        acFile[256] = {0};
	CHAR        acLocalFile[256] = {0};
    NE_BASIC_INFO_T stInfo = {0};
    VectorMap<AnsiString, NE_BASIC_INFO_T>  mNEBasicInfo;

    szMsgInfo = LoadCfgData(MSG_OMT_GetNeBasicInfo, ReqParam, RspJson);
    if (!szMsgInfo)
    {
        return FALSE;
    }

    GJson           MsgInfoJson;
    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

	if (!MsgInfoJson.GetValue("FileName", acFile, sizeof(acFile)))
    {
        GosLog(LOG_ERROR, "invalid msg format: %s", szMsgInfo);
        return FALSE;
    }

	sprintf(acLocalFile, "%s/%s", gos_get_root_path(), acFile);

	if (!TransFile(g_stLocalCfg.aucOMCAddr, g_usTransFilePort, acFile, acLocalFile))
    {
		GosLog(LOG_ERROR, "DownLoad file %s fail", acFile);
        return FALSE;
    }

    GJson           JsonResult;
    GJson           *pSubJson;

	if (!JsonResult.ParseFile(acLocalFile))
	{
		gos_delete_file(acLocalFile);
		GosLog(LOG_ERROR, "ParseFile %s fail", acLocalFile);
		return FALSE;
	}
	gos_delete_file(acLocalFile);

    VECTOR<GJson*>  &vSubJson = JsonResult.GetSubJson();

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];

        if (!pSubJson->GetValue("ne_id", stInfo.acNeID, sizeof(stInfo.acNeID)) ||
            !pSubJson->GetValue("mac", stInfo.acDevMac, sizeof(stInfo.acDevMac)) ||
            !pSubJson->GetValue("ip", stInfo.acDevIp, sizeof(stInfo.acDevIp)) ||
            !pSubJson->GetValue("type", &stInfo.ulDevType) ||
            !pSubJson->GetValue("name", stInfo.acDevName, sizeof(stInfo.acDevName)) ||
            !pSubJson->GetValue("sw_ver", stInfo.acSoftwareVer, sizeof(stInfo.acSoftwareVer)) ||
            !pSubJson->GetValue("hw_ver", stInfo.acHardwareVer, sizeof(stInfo.acHardwareVer)) ||
            !pSubJson->GetValue("model", stInfo.acModelName, sizeof(stInfo.acModelName)) ||
            !pSubJson->GetValue("online_sec", &stInfo.iOnlineSec))
        {
            GosLog(LOG_ERROR, "invalid msg format: %s", szMsgInfo);
            return FALSE;
        }

        if (stInfo.ulDevType == OMC_DEV_TYPE_TX ||
            stInfo.ulDevType == OMC_DEV_TYPE_FX  )
        {
             if (!pSubJson->GetValue("android_ver", stInfo.acAndroidVer,sizeof(stInfo.acAndroidVer)) ||
                 !pSubJson->GetValue("radio_ver", stInfo.acRadioVer, sizeof(stInfo.acRadioVer)))
             {
                GosLog(LOG_ERROR, "invalid msg format: %s", szMsgInfo);
                return FALSE;
             }
        }

        mNEBasicInfo.Add(stInfo.acNeID, stInfo);
    }

    NEBasic::InitNEBasicInfo(mNEBasicInfo);
    return TRUE;
}

BOOL LoadActiveAlarmInfo()
{
    GJsonParam      ReqParam;
    GJson           JsonResult;
    VECTOR<GJson*>  vSubJson;
    GJson           *pSubJson;
    GJson           RspJson;
    CHAR            *szMsgInfo;
    CHAR            acKey[128];
	CHAR            acFile[256] = {0};
    CHAR            acLocalFile[256] = {0};

    szMsgInfo = LoadCfgData(MSG_OMT_GetActiveAlarm, ReqParam, RspJson);
    if (!szMsgInfo)
    {
        return FALSE;
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    vSubJson.clear();
    if (MsgInfoJson.GetValue("File", acFile, sizeof(acFile)))
    {
        // 设置客户端本地保存文件目录
        sprintf(acLocalFile, "%s/%s", gos_get_root_path(), acFile);

		if (!TransFile(g_stLocalCfg.aucOMCAddr, g_usTransFilePort, acFile, acLocalFile))
        {
			GosLog(LOG_ERROR, "DownLoad file %s fail", acFile);
            return FALSE;
		}

        // 解析Json文件
		if (!JsonResult.ParseFile(acFile))
		{
			gos_delete_file(acFile);
            GosLog(LOG_ERROR, "ParseFile %s fail", acFile);
            return FALSE;
		}
		gos_delete_file(acFile);
        
		vSubJson = JsonResult.GetSubJson();
    }
    else
    {
        vSubJson = MsgInfoJson.GetSubJson();
    }
/*
    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
*/
    ALARM_INFO_T    stInfo = {0};
    VectorMap<AnsiString, ALARM_INFO_T> mActiveAlarm;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];

        if (!pSubJson->GetValue("ne_id", stInfo.acNeID, sizeof(stInfo.acNeID))||
            !pSubJson->GetValue("alarm_id", &stInfo.ulAlarmID) ||
            !pSubJson->GetValue("level", &stInfo.ulAlarmLevel)      ||
            !pSubJson->GetValue("type", &stInfo.ulAlarmType)        ||
            !pSubJson->GetValue("reason", stInfo.acAlarmReason, sizeof(stInfo.acAlarmReason)) ||
            !pSubJson->GetValue("time", &stInfo.ulAlarmRaiseTime)   ||
            !pSubJson->GetValue("info", stInfo.acAlarmInfo, sizeof(stInfo.acAlarmInfo))
           )
        {
            GosLog(LOG_ERROR, "parse active alarm info failed : %s", pSubJson->GetText());
            return FALSE;
        }

        sprintf(acKey, "%u_%s", stInfo.ulAlarmID, stInfo.acNeID);
        mActiveAlarm.Add(acKey, stInfo);
    }

    Alarm::InitActiveAlarm(mActiveAlarm);

    return TRUE;
}

BOOL LoadAlarmLevelCfg()
{
    GJsonParam  ReqParam;
    GJson       RspJson;
    CHAR        *szMsgInfo;

    szMsgInfo = LoadCfgData(MSG_OMT_GetAlarmLevelCfg, ReqParam, RspJson);
    if (!szMsgInfo)
    {
        return FALSE;
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
	VECTOR<ALARM_LEVEL_CFG_T>    vCfg;

	for (UINT32 i=0; i<vSubJson.size(); i++)
	{
        ALARM_LEVEL_CFG_T   stCfg = {0};
        pSubJson = vSubJson[i];

		if (!pSubJson->GetValue("dev_type", &stCfg.ulDevType) ||
			!pSubJson->GetValue("alarm_id", &stCfg.ulAlarmID) ||
            !pSubJson->GetValue("alarm_level", &stCfg.ulAlarmLevel) ||
            !pSubJson->GetValue("alarm_status", &stCfg.bIsIgnore))
        {
            GosLog(LOG_ERROR, "parse alarm level failed: %s", pSubJson->GetText());
            return FALSE;
        }

        vCfg.push_back(stCfg);
    }

//  g_mAlarmLevelCfg.Clear();
//  g_mAlarmLevelCfg.AddAll(mCfg);
    Alarm::InitAlarmLevelCfg(vCfg);
    return TRUE;

}

BOOL LoadStationInfo()
{
    GJsonParam  ReqParam;
    GJson       RspJson;
    CHAR        *szMsgInfo;

    szMsgInfo = LoadCfgData(MSG_OMT_GetStationInfo, ReqParam, RspJson);
    if (!szMsgInfo)
    {
        return FALSE;
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }


    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    STATION_INFO_T  stInfo = {0};
    VectorMap<UINT32, STATION_INFO_T>    mInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("StationID",&stInfo.ulStationID) ||
            !pSubJson->GetValue("StationName", stInfo.acStationName,sizeof(stInfo.acStationName)) ||
            !pSubJson->GetValue("StationType", &stInfo.ulStationType))
        {
            GosLog(LOG_ERROR, "invalid msg format: %s", szMsgInfo);
            return FALSE;
        }

        mInfo.Add(stInfo.ulStationID, stInfo);
    }

//  g_mStationInfo.Clear();
//  g_mStationInfo.AddAll(mInfo);
    Station::InitStationInfo(mInfo);
    return TRUE;
}

BOOL LoadNeInfo(GJsonParam &ReqJsonParam,GJson &RspJson)
{
    CHAR        *szMsgInfo;

    szMsgInfo = LoadCfgData(MSG_OMT_GetNeInfo, ReqJsonParam, RspJson);
    if (!szMsgInfo)
    {
        return FALSE;
    }

    return true;
}

AnsiString GetNeIDByDevName(AnsiString strDevName)
{
    VectorMap<AnsiString, NE_BASIC_INFO_T>  mNEBasicInfo;
    NEBasic::GetNEBasicInfo(mNEBasicInfo);

    if (!strDevName.IsEmpty())
    {
        for (UINT32 i=0; i<mNEBasicInfo.Size(); i++)
        {
            if (strDevName == mNEBasicInfo[i]->acDevName)
            {
                return mNEBasicInfo[i]->acNeID;
            }
        }
    }

    return "";
}

AnsiString GetDevNameByNeID(AnsiString strNeID)
{
    VectorMap<AnsiString, NE_BASIC_INFO_T>  mNEBasicInfo;
    NEBasic::GetNEBasicInfo(mNEBasicInfo);

    NE_BASIC_INFO_T     *pstInfo = mNEBasicInfo.GetValue(strNeID);

    if (pstInfo)
    {
        return pstInfo->acDevName;
    }

    return "";
}

INT32 GetOnlineStatusByNeID(AnsiString strNeID)
{
    VectorMap<AnsiString, NE_BASIC_INFO_T>  mNEBasicInfo;
    NEBasic::GetNEBasicInfo(mNEBasicInfo);

    NE_BASIC_INFO_T     *pstInfo = mNEBasicInfo.GetValue(strNeID);

    if (pstInfo)
    {
		return pstInfo->iOnlineSec;
	}

	return -1;
}

BOOL GetDevOperLog(AnsiString strNeID, UINT32 ulStartTime, UINT32 ulEndTime, VECTOR<OMA_OPERLOG_T> &vLog)
{
    GJsonParam  ReqParam;
    GJson       RspJson;
    CHAR        *szMsgInfo;

    vLog.clear();

    ReqParam.Add("NeID", strNeID);
    ReqParam.Add("StartTime", ulStartTime);
    ReqParam.Add("EndTime", ulEndTime);
    szMsgInfo = LoadCfgData(MSG_OMT_GetDevOperLog, ReqParam, RspJson, 3);
    if (!szMsgInfo)
    {
        return FALSE;
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    OMA_OPERLOG_T   stInfo = {0};

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];

        if (!pSubJson->GetValue("log", stInfo.acLogInfo, sizeof(stInfo.acLogInfo)) ||
            !pSubJson->GetValue("time", &stInfo.ulTime) )
        {
            GosLog(LOG_ERROR, "invalid msg format: %s", szMsgInfo);
            return FALSE;
        }

        vLog.push_back(stInfo);
    }

    return TRUE;
}

BOOL GetDevFileList(UINT32 ulDevFileType, AnsiString strNeID, UINT32 ulStartTime, UINT32 ulEndTime, VECTOR<OMA_FILE_INFO_T> &vInfo)
{
    GJsonParam  ReqParam;
    GJson       RspJson;
    CHAR        *szMsgInfo;

    vInfo.clear();

    ReqParam.Add("DevFileType", ulDevFileType);
    ReqParam.Add("NeID", strNeID);
    ReqParam.Add("StartTime", ulStartTime);
    ReqParam.Add("EndTime", ulEndTime);
    szMsgInfo = LoadCfgData(MSG_OMT_GetDevFileList, ReqParam, RspJson, 3);
    if (!szMsgInfo)
    {
        return FALSE;
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    OMA_FILE_INFO_T stInfo = {0};

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];

        if (!pSubJson->GetValue("name", stInfo.acFileName, sizeof(stInfo.acFileName)) ||
        //  !pSubJson->GetValue("type", &stInfo.ulFileType) ||
            !pSubJson->GetValue("time", &stInfo.ulFileTime) ||
            !pSubJson->GetValue("size", stInfo.acFileSize, sizeof(stInfo.acFileSize)) ||
            !pSubJson->GetValue("exist", &stInfo.bFileExist) )
        {
            GosLog(LOG_ERROR, "invalid msg format: %s", szMsgInfo);
            return FALSE;
        }

        vInfo.push_back(stInfo);
    }

    return TRUE;
}

BOOL GetSwUpdateInfo()
{
    GJsonParam  ReqParam;
    GJson       RspJson;
    CHAR        *szMsgInfo;
    VECTOR<OMC_SW_UPDATE_INFO_T>   vInfo;

    szMsgInfo = LoadCfgData(MSG_OMT_GetSwUpdateInfo, ReqParam, RspJson, 3);
    if (!szMsgInfo)
    {
        return FALSE;
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    OMC_SW_UPDATE_INFO_T    stInfo = {0};

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];

        if (!pSubJson->GetValue("ne_id", stInfo.acNeID, sizeof(stInfo.acNeID)) ||
        //  !pSubJson->GetValue("curr_ver", stInfo.acCurrSwVer, sizeof(stInfo.acCurrSwVer)) ||
            !pSubJson->GetValue("new_ver", stInfo.acNewSwVer, sizeof(stInfo.acNewSwVer)) ||
            !pSubJson->GetValue("set_flag", &stInfo.bSetFlag)  )
        {
            GosLog(LOG_ERROR, "invalid msg format: %s", szMsgInfo);
            return FALSE;
        }

        vInfo.push_back(stInfo);
    }

    g_vSwUpdateInfo.swap(vInfo);

    return TRUE;
}

BOOL LoadSwInfo()
{
    GJsonParam  ReqParam;
    GJson       RspJson;
    CHAR        *szMsgInfo;
    VECTOR<OMC_SW_INFO_T>   vInfo;

    szMsgInfo = LoadCfgData(MSG_OMT_GetSwInfo, ReqParam, RspJson, 3);
    if (!szMsgInfo)
    {
        return FALSE;
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    OMC_SW_INFO_T   stInfo = {0};

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];

        if (!pSubJson->GetValue("ver", stInfo.acSwVer, sizeof(stInfo.acSwVer)) ||
            !pSubJson->GetValue("file", stInfo.acSwFile, sizeof(stInfo.acSwFile)) ||
            !pSubJson->GetValue("type", &stInfo.ulSwType) ||
            !pSubJson->GetValue("time", &stInfo.ulSwTime)  )
        {
            GosLog(LOG_ERROR, "invalid msg format: %s", szMsgInfo);
            return FALSE;
        }

        vInfo.push_back(stInfo);
    }

    g_vSwInfo.swap(vInfo);

    return TRUE;
}

BOOL GetOmcCfg()
{
    GJsonParam  ReqParam;
    GJson       RspJson;
    CHAR        *szMsgInfo;

    szMsgInfo = LoadCfgData(MSG_OMT_GetCfg, ReqParam, RspJson, 3);
    if (!szMsgInfo)
    {
        return FALSE;
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    if (!MsgInfoJson.GetValue("ftp_port", &g_usFtpPort) ||
        !MsgInfoJson.GetValue("ftp_ver_dir", g_acFtpVerDir, sizeof(g_acFtpVerDir)) ||
        !MsgInfoJson.GetValue("ftp_log_dir", g_acFtpLogFileDir, sizeof(g_acFtpLogFileDir)) ||
        !MsgInfoJson.GetValue("ftp_user", g_acFtpUser, sizeof(g_acFtpUser)) ||
        !MsgInfoJson.GetValue("ftp_pwd", g_acFtpPwd, sizeof(g_acFtpPwd)) ||
        !MsgInfoJson.GetValue("transfile_port", &g_usTransFilePort) ||
        !MsgInfoJson.GetIP("NTPAddr", g_stNTPCfg.aucNTPAddr) ||
        !MsgInfoJson.GetValue("NTPPort", &g_stNTPCfg.usNTPPort) ||
        !MsgInfoJson.GetValue("NTPSyncPeriod", &g_stNTPCfg.ulNTPSyncPeriod) ||
        !MsgInfoJson.GetValue("OMCVersion", g_acOMCVersion, sizeof(g_acOMCVersion)))
    {
        GosLog(LOG_ERROR, "invalid msg format: %s", szMsgInfo);
        return FALSE;
    }

    return TRUE;
}

BOOL LoadCfgData()
{
    GetMaxInitNum();
    LOAD_DATA_CHECK("配置数据", "omc cfg", GetOmcCfg);
    LOAD_DATA_CHECK("设备基本信息", "ne basic info", LoadNEBasicInfo);
    LOAD_DATA_CHECK("设备版本库", "sw ver info", LoadSwInfo);
    LOAD_DATA_CHECK("设备版本状态", "sw update info", GetSwUpdateInfo);
    LOAD_DATA_CHECK("活跃告警信息", "active alarm info", LoadActiveAlarmInfo);
    LOAD_DATA_CHECK("告警等级配置", "alarm level cfg", LoadAlarmLevelCfg);
    LOAD_DATA_CHECK("车站信息", "station info", LoadStationInfo);

    GosLog(LOG_FATAL, "load cfg data successful!");

    return TRUE;
}


BOOL LoadOmtOperatorInfo()
{
   GJsonParam   ReqParam;
    GJson       RspJson;
    CHAR        *szMsgInfo;

    ReqParam.Add("UserType", "OmtOperator");

    szMsgInfo = LoadCfgData(MSG_OMT_GetUserInfo, ReqParam, RspJson);
    if (!szMsgInfo)
    {
        return FALSE;
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    OMT_OPERATOR_INFO_T     stInfo = {0};
    VECTOR<OMT_OPERATOR_INFO_T>     vUser;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("User", stInfo.acName, sizeof(stInfo.acName)) ||
            !pSubJson->GetValue("Pwd", stInfo.acPassword, sizeof(stInfo.acPassword)) ||
            !pSubJson->GetValue("Type", &stInfo.ulOmtType) )
        {
            GosLog(LOG_ERROR, "parse dc operator info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        vUser.push_back(stInfo);
    }

    g_vOmtOperatorInfo.swap(vUser);

    return TRUE;
}

BOOL LoadHistoryAlarm()
{
    GJsonParam  ReqParam;
    GJson       RspJson;
    CHAR        *szMsgInfo;

	szMsgInfo = LoadCfgData(MSG_OMT_GetHistoryAlarm, ReqParam, RspJson);
    if (!szMsgInfo)
    {
        return FALSE;
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    ALARM_INFO_T    stInfo = {0};
    VectorMap<AnsiString, ALARM_INFO_T> mHistoryAlarm;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];

        if (!pSubJson->GetValue("ne_id", stInfo.acNeID, sizeof(stInfo.acNeID)) ||
            !pSubJson->GetValue("alarm_id", &stInfo.ulAlarmID) ||
            !pSubJson->GetValue("level", &stInfo.ulAlarmLevel) ||
            !pSubJson->GetValue("type", &stInfo.ulAlarmType) ||
            !pSubJson->GetValue("reason", stInfo.acAlarmReason, sizeof(stInfo.acAlarmReason)) |
            !pSubJson->GetValue("time", &stInfo.ulAlarmRaiseTime) ||
            !pSubJson->GetValue("info", stInfo.acAlarmInfo, sizeof(stInfo.acAlarmInfo)) ||
            !pSubJson->GetValue("clear_time", &stInfo.ulAlarmClearTime) ||
            !pSubJson->GetValue("clear_info", stInfo.acAlarmClearInfo, sizeof(stInfo.acAlarmClearInfo)))
        {
            GosLog(LOG_ERROR, "parse alarm info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        mHistoryAlarm.Add(stInfo.acNeID,stInfo);
    }

    Alarm::InitHistoryAlarm(mHistoryAlarm);

    return TRUE;
}

BOOL LoadOperLogInfo(UINT32 ulStartTime, UINT32 ulEndTime, CHAR *szOperatorID, VECTOR<OMT_OPER_LOG_INFO_T> &vInfo)
{
    GJsonParam  Param;
    GJson       RspJson;

    Param.Add("StartTime", ulStartTime);
    Param.Add("EndTime", ulEndTime);
	Param.Add("OperatorID", szOperatorID);

	CHAR        *szMsgInfo = LoadCfgData(MSG_OMT_LoadOperLogInfo, Param, RspJson);

	vInfo.clear();
	if (!szMsgInfo)
    {
        return FALSE;
	}

    GJson           MsgInfoJson;

	if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
	OMT_OPER_LOG_INFO_T stInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("Time",     &stInfo.ulTime)   ||
			//!pSubJson->GetValue("DCUserID", &stInfo.ulUserID) ||
			!pSubJson->GetValue("OperatorID", stInfo.acUserID, sizeof(stInfo.acUserID)) ||
            !pSubJson->GetValue("LogInfo", stInfo.acLogInfo, sizeof(stInfo.acLogInfo)) )
        {
            GosLog(LOG_ERROR, "parse oper log info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        vInfo.push_back(stInfo);
    }

	return TRUE;
}
