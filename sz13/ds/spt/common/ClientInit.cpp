#include <vcl.h>
#include <Registry.hpp>

#include "ClientInclude.h"
#include "ds_public.h"
#include "pis.h"
#include "msg.h"
#include "main.h"
//#include "ThreadOMA.h"

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

BOOL            g_bDCLoginMode = FALSE;

UINT32          g_usTransFilePort = 50221;
LOCAL_CFG_T     g_stLocalCfg = {0};
LOG_CFG_T       g_stLogCfg = {0};
RemoteMsg       *g_pRemoteMsg = NULL;
CHAR            g_acLocalCfgFile[1024] = {0};

AnsiString  g_strDCOperatorID = "";
AnsiString  g_strDCOperatorName = "";


//VECTOR<DC_USER_INFO_T>                g_vDCUserInfo;
VECTOR<DC_OPERATOR_INFO_T>              g_vDCOperatorInfo;

UINT32      g_ulDCType = DC_TYPE_ALL;
//UINT32      g_ulDepotID = DEPOT_ID_UNKNOWN;
BOOL        g_bIPHRecMergeMode = TRUE;

CHAR        *g_szConfigFile = "spt.ini";

AnsiString  g_strStatusbarButtonSkinName = "DevExpressDarkStyle";
AnsiString  g_strToolbarButtonSkinName = "GlassOceans";
AnsiString  g_strLeftToolbarButtonSkinName = "GlassOceans";
AnsiString  g_strRightToolbarButtonSkinName = "Office2013DarkGray";
AnsiString  g_strActionButtonSkinName = "MoneyTwins";
AnsiString  g_strDownButtonSkinName = "Office2013DarkGray";

int         g_iMidToolbarButtonWidth = 108;
int         g_iMidToolbarButtonHeight = 72;

//TColor      g_TreeNodeCheckedColor = (TColor)RGB(0x31, 0x6A, 0xC5); //clSkyBlue;//clCream;//clWebDeepPink;
TColor      g_TreeNodeCheckedColor = clSkyBlue;//clCream;//clWebDeepPink;

UINT32      g_ulMaxListenPeriod = 60;   // 最大侦听时长

//UINT8       g_aucOmcAddr[4] = {0};

UINT8       g_aucNTPAddr[4];
UINT16      g_usNTPPort = 123;
UINT32      g_ulNTPSyncPeriod = 60;
UINT32      g_ulResvDataDays = 366;

//ThreadOma   *g_ThreadOma = NULL;
//BOOL     g_bOmcAddrValid = TRUE;

UINT32      g_ulAutoLockPeriod = 3600;

//DC_USER_INFO_T    *g_pstCurrDCUser = NULL;

BOOL g_bOriginalVideoFormat = FALSE;
BOOL g_bIPHVideoOnTcpMode = TRUE;

//extern void ClientConnectionChange(bool bConnected);
extern UINT32 GetMaxInitNum();
extern void SetInitInfo(AnsiString strInitInfo);
extern void LoadSkinCfg();

extern BOOL g_bShowTitle;
extern BOOL g_bNeedLogin;
extern CHAR *g_szVersion;

extern void ClientConnectionChange(bool bConnected);

/*
bool IsDepotMode()
{
    if (g_ulDCType & DC_TYPE_DEPOT ||
        g_ulDCType & DC_TYPE_FIX )
    {
        return true;
    }

    return false;
}

bool IsSameDepot(UINT32 ulDepotID)
{
    return ulDepotID == g_ulDepotID;
}     */

VOID LoadLocalConfInfo()
{
   /*

    if (!LoadConfInfo("Param", "OriginalVideoFormat", &g_bOriginalVideoFormat))
    {
        g_bOriginalVideoFormat = TRUE;
    }  */
}

VOID SaveLocalConfInfo()
{
  //    SaveConfInfo("Param", "ShowCallWin", g_bShowCallWin);
}

BOOL GetLocalCfg()
{
    CHAR    acFile[256];
    CHAR    *szValue;
    INT32   iValue;

    LoadLocalConfInfo();

    ///g_szConfigFile = strdup("spt.ini");

    sprintf(acFile, "%s/%s", gos_get_root_path(), g_szConfigFile);

    if (!gos_file_exist(acFile))
    {
        ErrMsg("本地配置文件[%s]不存在!", acFile);
        return FALSE;
    }

    strcpy(g_acLocalCfgFile, acFile);

    GConfFile   Conf(acFile);

    szValue = Conf.Get("Sys", "user_name");
    if (szValue)
    {
        g_strDCOperatorID = szValue;
    }

    szValue = Conf.Get("Sys", "server_ip");
    if (!gos_get_ip_port(szValue, g_stLocalCfg.aucServerAddr, &g_stLocalCfg.usServerPort))
    {
        GosLog(LOG_ERROR, "get server_ip of Sys failed");
        return FALSE;
    }

    if (g_stLocalCfg.usServerPort == 0)
    {
        g_stLocalCfg.usServerPort = DEFAULT_REC_TCP_PORT;
    }

    g_stLocalCfg.bTCPMode = TRUE;
    szValue = Conf.Get("Sys", "tcp_mode");
    if (szValue && strcasecmp(szValue, "false") == 0)
    {
        g_stLocalCfg.bTCPMode = FALSE;
    }

   // omc
   /*
    szValue = Conf.Get("Sys", "omc_addr");
    if (!szValue)
    {
        GosLog(LOG_ERROR, "no omc_addr in %s", g_szConfigFile);
        memset(g_aucOmcAddr, 0, sizeof(g_aucOmcAddr));
        g_bOmcAddrValid = FALSE;
        //goto end;
    }
    else if (!gos_get_ip(szValue, g_aucOmcAddr))
    {
        GosLog(LOG_ERROR, "invalid omc_addr in %s", g_szConfigFile);
        g_bOmcAddrValid = FALSE;
    }  */

    szValue = Conf.Get("Sys", "lock_window_time");
    if (szValue &&
        gos_atou(szValue, &g_ulAutoLockPeriod))
    {
        if (g_ulAutoLockPeriod > 0 && g_ulAutoLockPeriod < 60)
        {
            g_ulAutoLockPeriod = 60;
        }
        else if (g_ulAutoLockPeriod > 86400*30)
        {
            g_ulAutoLockPeriod  = 0;
        }
    }
    else
    {
        g_ulAutoLockPeriod = 0;
    }

    szValue = Conf.Get("Sys", "data_resv_day");
    if (szValue)
    {
        gos_atou(szValue, &g_ulResvDataDays);
        if (g_ulResvDataDays < 30)
        {
            g_ulResvDataDays = 30;
        }
        else if (g_ulResvDataDays > 3*366)
        {
            g_ulResvDataDays = 3*366;
        }
    }
    else
    {
        g_ulResvDataDays = 366;
    }

    // log
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
    g_pRemoteMsg = RemoteMsg::New(g_stLocalCfg.aucServerAddr, g_stLocalCfg.usServerPort, g_stLocalCfg.bTCPMode);
    if (!g_pRemoteMsg)
    {
        GosLog(LOG_FATAL, "Get Remote Msg failed!");

        return FALSE;
    }

    //g_pRemoteMsg->SetServer(g_stPortCfg.stConnectAddr.aucIP, g_stPortCfg.stConnectAddr.usPort);

    g_pRemoteMsg->SetClientType(DIS_CLIENT_DC);
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

    // SPT不需要OMA功能 / 如果 omc 配置不为空则开启 oma 线程
/*    if (g_bOmcAddrValid)
    {
        g_ThreadOma = new ThreadOma(g_szConfigFile, g_szVersion, g_aucOmcAddr);
    } */

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
        g_pRemoteMsg->SendReq(EV_CLIENT_REQ, szReq, strlen(szReq)+1);

        return NULL;
    }

    if(!g_pRemoteMsg->SyncSend(EV_CLIENT_REQ, szReq, strlen(szReq)+1,
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

CHAR* LoadDCSpecData(CHAR *szDataType, GJsonParam &ReqParam, GJson *pRspJson, UINT32 ulTimeout=10)
{
    ///ReqParam.Add("User", g_strDCOperatorID);

    return LoadData(szDataType, &ReqParam, pRspJson, ulTimeout);
}

CHAR* LoadDCSpecData(CHAR *szDataType, GJsonParam &ReqParam, GJson &RspJson, UINT32 ulTimeout=10)
{
    return LoadDCSpecData(szDataType, ReqParam, &RspJson, ulTimeout);
}

CHAR* LoadDCSpecData(CHAR *szDataType, GJson &RspJson)
{
    GJsonParam  ReqParam;

    return LoadDCSpecData(szDataType, ReqParam, RspJson);
}

CHAR* LoadDCSpecData(CHAR *szDataType, GJson *pRspJson)
{
    GJsonParam  ReqParam;

    return LoadDCSpecData(szDataType, ReqParam, pRspJson);
}

BOOL LoadDSCfg()
{
    GJsonParam  ReqParam;
    GJson       RspJson;
    CHAR        *szMsgInfo;

    szMsgInfo = LoadDCSpecData(MSG_REC_LoadNtpCfg, RspJson);
    if (!szMsgInfo)
    {
        return FALSE;
    }

    GJson           MsgInfoJson;
    GROUP_CFG_T     stGroupCfg = {0};

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    if (!MsgInfoJson.GetIP("NTPAddr", g_aucNTPAddr) ||
        !MsgInfoJson.GetValue("NTPPort", &g_usNTPPort) ||
        !MsgInfoJson.GetValue("NTPSyncPeriod", &g_ulNTPSyncPeriod) )
    {
        GosLog(LOG_ERROR, "parse ds group cfg failed on %s", szMsgInfo);
        return FALSE;
    }

    return TRUE;
}
/*
BOOL LoadDCUserInfo(UINT32 ulTimeout)//BOOL bSyncMode)
{
    GJsonParam  ReqParam;
    GJson       RspJson;
    CHAR        *szMsgInfo;

    ReqParam.Add("UserType", "DCUser");

    szMsgInfo = LoadDCSpecData(MSG_DCLoadUserInfo, ReqParam, RspJson, ulTimeout);
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
    DC_USER_INFO_T  stInfo = {0};
    CHAR            acStationList[256];
    VECTOR<DC_USER_INFO_T>  vUser;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("UserID", &stInfo.ulUserID) ||
            !pSubJson->GetValue("Name", stInfo.acName, sizeof(stInfo.acName)) ||
            !pSubJson->GetValue("DCType", &stInfo.ulDCType) ||
            !pSubJson->GetValue("DepotID", &stInfo.ulDepotID) ||
            !pSubJson->GetValue("StationList", acStationList, sizeof(acStationList)) )
        {
            GosLog(LOG_ERROR, "parse dc user info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        if (!ConvertStationList(acStationList, stInfo.aulStationList, ARRAY_SIZE(stInfo.aulStationList), stInfo.ulStationNum))
        {
            GosLog(LOG_ERROR, "parse StationList of dc user info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        DC_USER_INFO_T    *pstUser = GetDCUserInfo(stInfo.ulUserID);

        if (pstUser)
        {
            stInfo.ulCallStatus = pstUser->ulCallStatus;
        }

        vUser.push_back(stInfo);
    }

    g_vDCUserInfo.swap(vUser);

    return TRUE;
} */

/*
BOOL LoadOperatorInfo()
{
    GJsonParam  ReqParam;
    GJson       RspJson;
    CHAR        *szMsgInfo;

    szMsgInfo = LoadDCSpecData(MSG_REC_Operator, ReqParam, RspJson);
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
    DC_OPERATOR_INFO_T  stInfo = {0};
    VECTOR<DC_OPERATOR_INFO_T>  vUser;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("OperatorID", stInfo.acOperatorID, sizeof(stInfo.acOperatorID)) ||
            !pSubJson->GetValue("Name", stInfo.acName, sizeof(stInfo.acName)) ||
        //  !pSubJson->GetValue("Password", stInfo.acPassword, sizeof(stInfo.acPassword)) ||
            !pSubJson->GetValue("DCType", &stInfo.ulType) )
        {
            GosLog(LOG_ERROR, "parse operator info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        vUser.push_back(stInfo);
    }

    g_vDCOperatorInfo.swap(vUser);

    return TRUE;
}
   */
/*
BOOL LoadDepotInfo()
{
    GJson       RspJson;
    CHAR        *szMsgInfo = LoadDCSpecData(MSG_DCLoadDepotInfo, RspJson);

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
    DEPOT_INFO_T    stInfo;
    VECTOR<DEPOT_INFO_T>    vInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("DepotID", &stInfo.ulDepotID) ||
            !pSubJson->GetValue("DepotName", stInfo.acDepotName, sizeof(stInfo.acDepotName)) ||
            !pSubJson->GetValue("TrainDirection", &stInfo.ulTrainDirection) )
        {
            GosLog(LOG_ERROR, "parse depot info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        vInfo.push_back(stInfo);
    }

    InitDepot(vInfo);

    return TRUE;
}
*/

/*
BOOL LoadOperLogInfo(UINT32 ulStartTime, UINT32 ulEndTime, CHAR *szUser, VECTOR<OPER_LOG_INFO_T> &vInfo)
{
    GJsonParam  Param;
    GJson       RspJson;

    Param.Add("StartTime", ulStartTime);
    Param.Add("EndTime", ulEndTime);
    Param.Add("User", szUser);

    CHAR        *szMsgInfo = LoadDCSpecData(MSG_DCLoadOperLogInfo, Param, RspJson);

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
    OPER_LOG_INFO_T stInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("Time",     &stInfo.ulTime)   ||
            !pSubJson->GetValue("DCUserID", &stInfo.ulUserID) ||
            !pSubJson->GetValue("DCOperatorID", stInfo.acDCOperatorID, sizeof(stInfo.acDCOperatorID)) ||
            !pSubJson->GetValue("LogInfo", stInfo.acLogInfo, sizeof(stInfo.acLogInfo)) )
        {
            GosLog(LOG_ERROR, "parse oper log info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        vInfo.push_back(stInfo);
    }

    return TRUE;
}
*/

BOOL LoadCfgData()
{
    GetMaxInitNum();

    LOAD_DATA_CHECK("配置数据", "ds cfg", LoadDSCfg);

    GosLog(LOG_FATAL, "load cfg data successful!");

    return TRUE;
}



