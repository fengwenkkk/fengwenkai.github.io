#include <vcl.h>
#include <Registry.hpp>

#include "ClientInclude.h"
#include "pis.h"
#include "main.h"
#include "ThreadOMA.h"
#include "SDSTemplate.h"

#define LOAD_DATA_CHECK(szLoadDisplayName, szLoadName, LoadFunc)\
{\
    SetInitInfo(szLoadDisplayName);\
    if (!LoadFunc())               \
    {                              \
        GosLog(LOG_FATAL, "Load %s failed!", szLoadName);\
        return FALSE;                                    \
    }\
    GosLog(LOG_INFO, "Load %s succ", szLoadName);\
}

RemoteMsg   *g_pRemoteMsg = NULL;
RemoteMsg   *g_pMRSRemoteMsg = NULL;

ThreadOma   *g_ThreadOma = NULL;
static BOOL g_bAllowReloadCfg = TRUE;

const char  *g_szNativeServiceName = "eLTE_NativeService.exe";

typedef struct
{
    const char      *szLoadType;
    LOAD_CFG_FUNC   pfLoadFunc;
    UINT32          ulCfgVer;
}LOAD_CFG_INFO_T;

LOAD_CFG_INFO_T     g_astLoadCfgInfo[] = {
    {MSG_DCLoadAPPCfg,             LoadAPPCfg           , 0},
    {MSG_DCLoadGroupCfg,           LoadGroupCfg         , 0},
    {MSG_DCLoadDCUserInfo,         LoadDCUserInfo       , 0},
    {MSG_DCLoadDCOperatorInfo,     LoadDCOperatorInfo   , 0},
    {MSG_DCLoadStationUEInfo,      LoadStationUEInfo    , 0},
    {MSG_DCLoadTrainUEInfo,        LoadTrainUEInfo      , 0},
    {MSG_DCLoadPortalUEInfo,       LoadPortalUEInfo     , 0},
    {MSG_DCLoadGroupInfo,          LoadGroupInfo        , 0},
    {MSG_DCLoadAutoJoinGroupInfo,  LoadAutoJoinGroupInfo, 0},
    {MSG_DCLoadSubGroupInfo,       LoadSubGroupInfo     , 0},
    {MSG_DCLoadSubVideoGroupInfo,  LoadSubVideoGroupInfo, 0},
    {MSG_DCLoadSubUserInfo,        LoadSubUserInfo      , 0},
    {MSG_DCLoadSubVideoUserInfo,   LoadSubVideoUserInfo , 0},
    {MSG_DCLoadMultiGroupInfo,     LoadMultiGroupInfo   , 0},
    {MSG_DCLoadTrainInfo,          LoadTrainInfo        , 0},
    {MSG_DCLoadDepotInfo,          LoadDepotInfo        , 0},
    {MSG_DCLoadStationInfo,        LoadStationInfo      , 0},
    {MSG_DCLoadBrdZoneInfo,        LoadBrdZoneInfo      , 0},
    {MSG_DCLoadSectionInfo,        LoadSectionInfo      , 0},
    {MSG_DCLoadDestInfo,           LoadDestInfo         , 0},
    {MSG_DCLoadPredefinedSMS,      LoadPredefinedSMS    , 0},
    {MSG_DCLoadSDSTemplate,        LoadSDSTemplate      , 0},
    {MSG_DCLoadRecBrdCfg,          LoadRecBrdCfg        , 0},
};

extern UINT32 GetMaxInitNum();
extern void SetInitInfo(AnsiString strInitInfo);
extern void LoadSkinCfg();

extern VOID InitRecBrdCfg(VECTOR<REC_BRD_CFG_T> &vInfo);

extern void ClientConnectionChange(bool bConnected);

extern void LoadAudioDevCfg();

BOOL IsLocalLoadDataType(CHAR *szDataType)
{
    BOOL    bLocalLoad = FALSE;

    for (UINT32 i=0; i<ARRAY_SIZE(g_astLoadCfgInfo); i++)
    {
        if (strcmp(szDataType, g_astLoadCfgInfo[i].szLoadType) == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}

CHAR* LoadLocalData(CHAR *szDataType, GJson *pJson)
{
    if (!IsLocalLoadDataType(szDataType))
    {
        return "[]";
    }

    CHAR    acFile[256];

    sprintf(acFile, "%s/cfg/%s.json", gos_get_root_path(), szDataType);

    if (!pJson->ParseFile(acFile))
    {
        return NULL;
    }

    return pJson->GetText();
}

BOOL SaveLocalLoadData(CHAR *szDataType, CHAR *szMsgInfo)
{
    CHAR        acPath[256];
    CHAR        acFile[256];
  //  FILE        *fp;

    sprintf(acPath, "%s/cfg", gos_get_root_path());
    if (!gos_file_exist(acPath))
    {
        gos_create_dir(acPath);
    }

    sprintf(acFile, "%s/%s.json", acPath, szDataType);

    if (!gos_save_string_to_file(acFile, szMsgInfo))
    {
        GosLog(LOG_ERROR, "save cfg data of %s failed", szDataType);
        return FALSE;
    }

    return TRUE;

    /*
    fp = fopen(acFile, "w+b");
    if (!fp)
    {
        GosLog(LOG_ERROR, "save %s to local faild", szDataType);
        return FALSE;
    }

    UINT32  ulLen = strlen(szMsgInfo)+1;

    if (ulLen != fwrite(szMsgInfo, 1, ulLen, fp))
    {
        fclose(fp);
        return FALSE;
    }

    fclose(fp);
    return TRUE;   */
}

BOOL InitNativeServicePath()
{
    CHAR        acNativeServicePath[512];
    TRegistry   *Reg = new TRegistry;
    AnsiString  strValue = "";
    AnsiString  strName = "ImagePath";
    AnsiString  strKey = "SYSTEM\\CurrentControlSet\\Services\\HUAWEI eSDK eLTE Service";

    try
    {
        Reg->RootKey = HKEY_LOCAL_MACHINE;
        if (Reg->OpenKey(strKey, false))
        {
            if (Reg->ValueExists(strName))
            {
                strValue = Reg->ReadString(strName);
            }
            Reg->CloseKey();
        }
    }
    __finally
    {
        delete Reg;
    }

    if (strValue.IsEmpty())
    {
        GosLog(LOG_ERROR, "param %s of reg entry %s doesn't exist", strName.c_str(), strKey.c_str());
        return FALSE;
    }

    // C:\app\eLTE_NativeService_C10SPC200\eLTE_NativeService.exe
    if (!FileExists(strValue) )
    {
        GosLog(LOG_ERROR, "File %s doesn't exists!", strValue.c_str());
        return FALSE;
    }

    if (strValue.Length() <= (int)strlen(g_szNativeServiceName))
    {
        GosLog(LOG_ERROR, "The length of file %s is too short", strValue.c_str());
        return FALSE;
    }

    CHAR    *szTmp;

    StrCpy(acNativeServicePath, strValue.c_str());
    gos_format_path(acNativeServicePath);
    szTmp = acNativeServicePath + strlen(acNativeServicePath) - strlen(g_szNativeServiceName);
    if (strcasecmp(szTmp, g_szNativeServiceName) != 0)
    {
        GosLog(LOG_ERROR, "invalid eLTE native service file %s", acNativeServicePath);
        return FALSE;
    }

    *szTmp = '\0';
    if (strlen(acNativeServicePath) < 2)
    {
        return FALSE;
    }

    szTmp--;
    if (*szTmp != '\\')
    {
        return FALSE;
    }

    *szTmp = '\0';
    SetNativeServicePath(acNativeServicePath);

    return TRUE;
}

VOID LoadLocalConfInfo()
{
    BOOL    bValue;

    if (!LoadConfInfo("Param", "ShowCallWin", &bValue))
    {
        bValue = TRUE;
    }

    SetShowCallWin(bValue);

    if (!LoadConfInfo("Param", "OriginalVideoFormat", &bValue))
    {
        bValue = TRUE;
    }

    SetOriginalVideoFormat(bValue);

    if (!LoadConfInfo("Param", "IPHAECMode", &bValue))
    {
        bValue = FALSE;
    }

    SetLocalIPHAECMode(bValue);
}

VOID SaveLocalConfInfo()
{
    SaveConfInfo("Param", "ShowCallWin", IsShowCallWin());
    SaveConfInfo("Param", "OriginalVideoFormat", IsOriginalVideoFormat());
    SaveConfInfo("Param", "IPHAECMode", IsLocalIPHAECMode());
}

/**
 * @brief           读取调度台系统相关的配置项，并存入 LocalCfg 这个类中
 * @param           Conf    [in]
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-08-27 10:43:09
 * @note
 */
BOOL GetLocalCfg()
{
    AnsiString strCfgFileName;

    strCfgFileName.sprintf("%s/%s", gos_get_root_path(), "dc.ini");

    if (!gos_file_exist(strCfgFileName.c_str()))
    {
        ErrMsg("本地配置文件[%s]不存在!", strCfgFileName.c_str());
        return FALSE;
    }

    /// 获取配置文件的全路径
    if (!LocalCfg::GetInstance().Load(strCfgFileName))
    {
        GosLog(LOG_ERROR, "load local cfg failed!");
        return FALSE;
    }

    LoadLocalConfInfo();
    LoadAudioDevCfg();

    return TRUE;
}

BOOL InitRemoteMsg()
{
    static BOOL bConnect = FALSE;

    if (bConnect)
    {
        return TRUE;
    }

    /* 初始化远程消息模块 */
    g_pRemoteMsg = RemoteMsg::New(LocalCfg::GetInstance().GetDisIP().aucIP,
                                  LocalCfg::GetInstance().GetDisIP().usPort,
                                  LocalCfg::GetInstance().GetTcpMode());
    if (!g_pRemoteMsg)
    {
        GosLog(LOG_FATAL, "Get Remote Msg failed!");

        return FALSE;
    }

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

BOOL InitMRSRemoteMsg()
{
    static BOOL bConnect  = FALSE;

    if (bConnect)
    {
        return TRUE;
    }

    if (GET_INT(LocalCfg::GetInstance().GetMrsIP().aucIP) == 0)
    {
        return TRUE;
    }

    /* 初始化远程消息模块 */
    g_pMRSRemoteMsg = RemoteMsg::New(LocalCfg::GetInstance().GetMrsIP().aucIP,
                                     LocalCfg::GetInstance().GetMrsIP().usPort,
                                     LocalCfg::GetInstance().GetMrsTcpMode());
    if (!g_pMRSRemoteMsg)
    {
        GosLog(LOG_FATAL, "Get MRS Remote Msg failed!");

        return FALSE;
    }

    g_pMRSRemoteMsg->SetClientType(DIS_CLIENT_DC);
//    g_pMRSRemoteMsg->SetClientConnectionChange(ClientConnectionChange);

    for (int i=0; i<100; i++)
    {
        if (g_pMRSRemoteMsg->IsConnected())
        {
            break;
        }

        gos_sleep_1ms(100);
    }

    if (!g_pMRSRemoteMsg->IsConnected())
    {
        GosLog(LOG_FATAL, "Connect to MRS server failed!");

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
    }

    gos_init_socket();

    /// 使用默认参数初始化日志模块
    GosInitLog();

    /// 获取本地配置
    if (!GetLocalCfg())
    {
        GosLog(LOG_FATAL, "Get local Cfg failed!");

        return FALSE;
    }

    LOG_CFG_T stLog = LocalCfg::GetInstance().GetLogCfg();
    /// 使用配置项来初始化一次 log 模块
    GosInitLog(&stLog);

    LoadSkinCfg();
    InitSystemFont();
    gos_init_svcenv();

    GLogFactory::GetInstance()->GetDefaultLogger()->SetLogModule(MODULE_DTP, MODULE_DTP_NAME);

    /// 如果 omc 配置不为空则开启 oma 线程
    if (!LocalCfg::GetInstance().IsOmcIPEmpty())
    {
        g_ThreadOma = new ThreadOma(LocalCfg::GetInstance().GetLocalConfFileName(),
                                    LocalCfg::GetInstance().GetDCVersion().c_str(),
                                    LocalCfg::GetInstance().GetMdcIP().aucIP,
                                    LocalCfg::GetInstance().GetOmcIP().aucIP,
                                    (UINT8 *)NULL);
    }

    return TRUE;
}

// 加载数据
static CHAR* LoadCfgData(CHAR *szDataType, GJsonParam *pJsonParam, GJson *pRspJson, UINT32 ulTimeout=10)
{
    UINT32                  ulRspLen = 0;
    CHAR                    *szReq;
    UINT32                  ulBufLen = 64*1024;
    CHAR                    *szRsp = NULL;
    GJsonParam              ReqParam;

    if (APP::AllowDISOffline())
    {
        return LoadLocalData(szDataType, pRspJson);
    }

    if (pRspJson)
    {
        szRsp = (CHAR*)calloc(1, ulBufLen);
        if (!szRsp)
        {
            return NULL;
        }
    }

    ReqParam.Add("MsgName", szDataType);
    ReqParam.Add("DCUserID", APP::GetCurrDCUserID());
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
        g_pRemoteMsg->SendReq(EV_DC_DIS_REQ, szReq, strlen(szReq)+1);

        return NULL;
    }

    if(!g_pRemoteMsg->SyncSend(EV_DC_DIS_REQ, szReq, strlen(szReq)+1,
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

    if (!szRetMsgType)
    {
        GosLog(LOG_ERROR, "no msg type on %s", pRspJson->GetText());
        return NULL;
    }

    if (strcmp(szDataType, szRetMsgType) != 0)
    {
        GosLog(LOG_ERROR, "invalid msg type(%s)", szRetMsgType);
        return NULL;
    }

    CHAR *szMsgInfo = pRspJson->GetValue("MsgInfo");

    if (!szMsgInfo)
    {
        GosLog(LOG_ERROR, "no MsgInfo in msg %s", pRspJson->GetText());
        return NULL;
    }

    CHAR *szZipped = pRspJson->GetValue("Zipped");
    std::string strOriMsg;

    if (szZipped && strcasecmp(szZipped, "true") == 0)
    {
        strOriMsg = UnzipString(szMsgInfo);
        szMsgInfo = (CHAR*)strOriMsg.c_str();
    }

    GJson       CfgDataJson;
    UINT32      ulCfgVer;

    if (!CfgDataJson.Parse(szMsgInfo))
    {
        GosLog(LOG_ERROR, "invalid MsgInfo in msg %s", pRspJson->GetText());
        return NULL;
    }

    if (!CfgDataJson.GetValue("CfgVer", &ulCfgVer))
    {
        GosLog(LOG_ERROR, "invalid CfgVer in msg %s", pRspJson->GetText());
        return NULL;
    }

    CHAR        *szCfgData = CfgDataJson.GetValue("CfgData");

    if (!szCfgData)
    {
        GosLog(LOG_ERROR, "invalid CfgData in msg %s", pRspJson->GetText());
        return NULL;
    }

    if (IsLocalLoadDataType(szDataType))
    {
        SaveLocalLoadData(szDataType, szCfgData);
    }

    for (UINT32 i=0; i<ARRAY_SIZE(g_astLoadCfgInfo); i++)
    {
        if (strcmp(szDataType, g_astLoadCfgInfo[i].szLoadType) == 0)
        {
            g_astLoadCfgInfo[i].ulCfgVer = ulCfgVer;
            break;
        }
    }

    static AnsiString   strCfgData = "";

    strCfgData = szCfgData;

    return strCfgData.c_str();
}

static CHAR* LoadCfgData(CHAR *szDataType, GJsonParam &ReqParam, GJson &RspJson, UINT32 ulTimeout=10)
{
    return LoadCfgData(szDataType, &ReqParam, &RspJson, ulTimeout);
}

static CHAR* LoadCfgData(CHAR *szDataType, GJson &RspJson)
{
    GJsonParam  ReqParam;

    return LoadCfgData(szDataType, ReqParam, RspJson);
}

static CHAR* LoadCfgData(CHAR *szDataType)
{
    GJsonParam  ReqParam;

    return LoadCfgData(szDataType, NULL, NULL, 0);
}

VOID GetLoadCfgDataType(VECTOR<CHAR*> &vCfgType)
{
    vCfgType.clear();

    for (UINT32 i=0; i<ARRAY_SIZE(g_astLoadCfgInfo); i++)
    {
        if (strcmp(MSG_DCLoadTrainInfo, g_astLoadCfgInfo[i].szLoadType) == 0)
        {
            continue;
        }

        vCfgType.push_back((CHAR*)g_astLoadCfgInfo[i].szLoadType);
    }
}

VOID OnLoadCfgDataRsp(AnsiString strMsgType, UINT32 ulCfgVer, CHAR *szCfgData)
{
    LOAD_CFG_FUNC   pfLoadFunc = NULL;
    GJson           Json;
    UINT32          *pulCfgVer;

    for (UINT32 i=0; i<ARRAY_SIZE(g_astLoadCfgInfo); i++)
    {
        if (strMsgType == g_astLoadCfgInfo[i].szLoadType)
        {
            pfLoadFunc = g_astLoadCfgInfo[i].pfLoadFunc;
            pulCfgVer = &g_astLoadCfgInfo[i].ulCfgVer;
            break;
        }
    }

    if (!pfLoadFunc)
    {
        GosLog(LOG_ERROR, "unknown cfg data: %s", strMsgType.c_str());
        return;
    }

    GosLog(LOG_INFO, "auto load cfg data: %s, curr ver is %u, new ver is %u",
           strMsgType.c_str(), *pulCfgVer, ulCfgVer);

    if (!pfLoadFunc(szCfgData))
    {
        return;
    }

    // 加载成功才更新配置数据版本号
    *pulCfgVer = ulCfgVer;
}

BOOL ReloadCfgData(GJson &Json)
{
    UINT32      ulCfgVer;

    for (UINT32 i=0; i<ARRAY_SIZE(g_astLoadCfgInfo); i++)
    {
        if (!Json.GetValue(g_astLoadCfgInfo[i].szLoadType, &ulCfgVer))
        {
            GosLog(LOG_ERROR, "parse %s on %s failed", g_astLoadCfgInfo[i].szLoadType, Json.GetText());

            return FALSE;
        }

        if (g_astLoadCfgInfo[i].ulCfgVer != ulCfgVer)
        {
            //g_astLoadCfgInfo[i].pfLoadFunc(NULL);
            GosLog(LOG_INFO, "reload cfg data %s, curr ver is %u, new ver is %u",
                   g_astLoadCfgInfo[i].szLoadType, g_astLoadCfgInfo[i].ulCfgVer, ulCfgVer);

            LoadCfgData((CHAR*)g_astLoadCfgInfo[i].szLoadType);
        }
    }

    return TRUE;
}

/**
 * @brief           加载数据库中 groupcfg 表内容，静态组内容，并加载dis服务端存储的 ATS 地址和NTP地址与NTP更新周期
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-15 17:46:05
 * @note
 */
BOOL LoadAPPCfg(CHAR *szMsgInfo)
{
    GJsonParam  ReqParam;
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadAPPCfg, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
    }

    GJson           MsgInfoJson;
    NTP_CFG_T       stNTPCfg = {0};
    CHAR            acATSAddr1[16];
    CHAR            acATSAddr2[16];

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    BOOL    bAllowHalfDuplexP2PCall = FALSE;
    BOOL    bAllowReloadCfg = FALSE;
    BOOL    bIPHVideoOnTcpMode = FALSE;
    UINT32  ulIPHVideoOpenTimeout;
    BOOL    bIPHRecMergeMode = FALSE;
    BOOL    bDirectSendIPHAudio = FALSE;
    UINT32  ulPAAudioSampleRate;
    UINT32  ulPAAudioBitsPerSample;
    BOOL    bDirectSendPAAudio  = FALSE;
    UINT32  ulIPHAudioSampleRate;
    UINT32  ulIPHAudioBitsPerSample;
    UINT32  ulMinTmpPatchGroupNum;
    BOOL    bPAAECMode;
    UINT32  ulPAMaxPkgSize;
    BOOL    bIPHAECMode;
    UINT32  ulIPHMaxPkgSize;
    UINT32  ulMaxIPHTalkTime;
    BOOL    bManageOneDepot;
    BOOL    bAllowVideoWall;
    BOOL    bAllowGISSub;
    BOOL    bAllowP2PVideoCall;
    BOOL    bAutoCloseMulticastCall;
    BOOL    bSendSMSByRealMsg;
    BOOL    bSimpleSMSMode;
    UINT32  ulATSValidPeriod;

    /// 加载dis端 appcfg 表中的 ATS 地址、NTP 地址与端口还有 NTP 更新周期
    if (!MsgInfoJson.GetValue("ATS1Addr", acATSAddr1, sizeof(acATSAddr1)) ||
        !MsgInfoJson.GetValue("ATS2Addr", acATSAddr2, sizeof(acATSAddr2)) ||
        !MsgInfoJson.GetValue("ATSValidPeriod", &ulATSValidPeriod) ||
        !MsgInfoJson.GetIP("NTPAddr", stNTPCfg.aucNTPAddr) ||
        !MsgInfoJson.GetValue("NTPPort", &stNTPCfg.usNTPPort) ||
        !MsgInfoJson.GetValue("NTPSyncPeriod", &stNTPCfg.ulNTPSyncPeriod) ||
        !MsgInfoJson.GetValue("MinTmpPatchGroupNum", &ulMinTmpPatchGroupNum) ||
        !MsgInfoJson.GetValue("AllowReloadCfg", &bAllowReloadCfg) ||
        !MsgInfoJson.GetValue("AllowHalfDuplexP2PCall", &bAllowHalfDuplexP2PCall)||
        !MsgInfoJson.GetValue("IPHVideoOnTcpMode", &bIPHVideoOnTcpMode)||
        !MsgInfoJson.GetValue("IPHVideoOpenTimeout", &ulIPHVideoOpenTimeout) ||
        !MsgInfoJson.GetValue("IPHRecMergeMode", &bIPHRecMergeMode) ||
        !MsgInfoJson.GetValue("DirectSendIPHAudio", &bDirectSendIPHAudio) ||
        !MsgInfoJson.GetValue("IPHAudioSampleRate", &ulIPHAudioSampleRate) ||
        !MsgInfoJson.GetValue("IPHAudioBitsPerSample", &ulIPHAudioBitsPerSample) ||
        !MsgInfoJson.GetValue("DirectSendPAAudio", &bDirectSendPAAudio) ||
        !MsgInfoJson.GetValue("PAAudioSampleRate", &ulPAAudioSampleRate) ||
        !MsgInfoJson.GetValue("PAAudioBitsPerSample", &ulPAAudioBitsPerSample) ||
        !MsgInfoJson.GetValue("PAAECMode", &bPAAECMode) ||
        !MsgInfoJson.GetValue("PAMaxPkgSize", &ulPAMaxPkgSize) ||
        !MsgInfoJson.GetValue("IPHAECMode", &bIPHAECMode) ||
        !MsgInfoJson.GetValue("IPHMaxPkgSize", &ulIPHMaxPkgSize) ||
        !MsgInfoJson.GetValue("MaxIPHTalkTime", &ulMaxIPHTalkTime) ||
        !MsgInfoJson.GetValue("ManageOneDepot", &bManageOneDepot) ||
        !MsgInfoJson.GetValue("AllowVideoWall", &bAllowVideoWall) ||
        !MsgInfoJson.GetValue("AllowGISSub", &bAllowGISSub) ||
        !MsgInfoJson.GetValue("AllowP2PVideoCall", &bAllowP2PVideoCall) ||
        !MsgInfoJson.GetValue("AutoCloseMulticastCall", &bAutoCloseMulticastCall) ||
        !MsgInfoJson.GetValue("SendSMSByRealMsg", &bSendSMSByRealMsg) ||
        !MsgInfoJson.GetValue("SimpleSMSMode", &bSimpleSMSMode) )
    {
        GosLog(LOG_ERROR, "parse app cfg %s failed on %s",
                MsgInfoJson.GetCurrKey(), szMsgInfo);
        return FALSE;
    }

    /// 保存至内存中的变量
    SetNTPCfg(stNTPCfg);
    SetATSAddr1(acATSAddr1);
    SetATSAddr2(acATSAddr2);
    SetATSValidPeriod(ulATSValidPeriod);

    g_bAllowReloadCfg = bAllowReloadCfg;
    SetAllowHalfDuplexP2PCall(bAllowHalfDuplexP2PCall);

    SetIPHVideoOnTcpMode(bIPHVideoOnTcpMode);
    SetIPHVideoOpenTimeout(ulIPHVideoOpenTimeout);
    SetIPHRecMergeMode(bIPHRecMergeMode);
    SetDirectSendIPHAudio(bDirectSendIPHAudio);
    SetIPHAudioSampleRate(ulIPHAudioSampleRate);
    SetIPHAudioBitsPerSample(ulIPHAudioBitsPerSample);
    SetDirectSendPAAudio(bDirectSendPAAudio);
    SetPAAudioSampleRate(ulPAAudioSampleRate);
    SetPAAudioBitsPerSample(ulPAAudioBitsPerSample);

    SetAllowPAAECMode(bPAAECMode);
    SetPAMaxPkgSize(ulPAMaxPkgSize);
    SetAllowIPHAECMode(bIPHAECMode);
    SetIPHMaxPkgSize(ulIPHMaxPkgSize);
 //   SetPADelaySendTime(ulPADelaySendTime);
    SetMaxIPHTalkTime(ulMaxIPHTalkTime);

    ManageOneDepot(bManageOneDepot);
    SetAllowVideoWall(bAllowVideoWall);
    SetAllowGISSub(bAllowGISSub);
    SetAllowP2PVideoCall(bAllowP2PVideoCall);
    SetAutoCloseMulticastCall(bAutoCloseMulticastCall);
    SetSendSMSByRealMsg(bSendSMSByRealMsg);
    SetSimpleSMSMode(bSimpleSMSMode);

    PatchGroup::SetMinTmpPatchGroupNum(ulMinTmpPatchGroupNum);

    return TRUE;
}

BOOL LoadGroupCfg(CHAR *szMsgInfo)
{
    GJsonParam  ReqParam;
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadGroupCfg, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
    }

    GJson           MsgInfoJson;
    GROUP_CFG_T     stGroupCfg = {0};

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    if (!MsgInfoJson.GetValue("UpLineTrainGroupID", &stGroupCfg.i64UpLineTrainGroupID) ||
        !MsgInfoJson.GetValue("DownLineTrainGroupID", &stGroupCfg.i64DownLineTrainGroupID) ||
        !MsgInfoJson.GetValue("MainLineTrainGroupID", &stGroupCfg.i64MainLineTrainGroupID) ||
        !MsgInfoJson.GetValue("UpLineTrainBrdGroupID", &stGroupCfg.i64UpLineTrainBrdGroupID) ||
        !MsgInfoJson.GetValue("DownLineTrainBrdGroupID", &stGroupCfg.i64DownLineTrainBrdGroupID) ||
        !MsgInfoJson.GetValue("MainLineTrainBrdGroupID", &stGroupCfg.i64MainLineTrainBrdGroupID) ||
        !MsgInfoJson.GetValue("AllStationGroupID", &stGroupCfg.i64AllStationGroupID)  ||
        !MsgInfoJson.GetValue("StartTempDynamicGroupID", &stGroupCfg.i64StartTempDynamicGroupID) ||
        !MsgInfoJson.GetValue("EndTempDynamicGroupID", &stGroupCfg.i64EndTempDynamicGroupID) ||
        !MsgInfoJson.GetValue("DriverGroupID", &stGroupCfg.i64DriverGroupID) ||
        !MsgInfoJson.GetValue("DepotDriverGroupID", &stGroupCfg.i64DepotDriverGroupID)  )
    {
        GosLog(LOG_ERROR, "parse group cfg failed on %s", szMsgInfo);
        return FALSE;
    }

    GroupCfg::InitGroupCfg(stGroupCfg);

    return TRUE;
}

BOOL ConvertStationList(CHAR *szStationList, UINT32 *pulStationList, UINT32 ulMaxStationNum, UINT32 &ulStationNum)
{
    CHAR    *szTmp;

    ulStationNum = 0;
    if (*szStationList == '\0')
    {
        return TRUE;
    }

    strcat(szStationList, ",");
    while(*szStationList)
    {
        szTmp = strchr(szStationList, ',');
        if (!szTmp)
        {
            break;
        }

        *szTmp++ = '\0';
        if (ulStationNum >= ulMaxStationNum)
        {
            return FALSE;
        }

        if (!gos_atou(szStationList, pulStationList))
        {
            return FALSE;
        }

        pulStationList++;
        ulStationNum++;
        szStationList = szTmp;
    }

    return TRUE;
}

/**
 * @brief           加载 dis 端数据库中 SDK 账号信息， 表(dcuser)
 * @param           ulTimeout    [in]   阻塞等待的事件，单位为 秒
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-15 11:24:27
 * @note
 */
BOOL LoadDCUserInfo(CHAR *szMsgInfo)
{
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadDCUserInfo, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    CHAR            acStationList[256];
    VECTOR<DC_USER_INFO_T>  vUser;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        GJson* pSubJson = vSubJson[i];
        DC_USER_INFO_T  stInfo = {0};

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

        DC_USER_INFO_T    *pstUser = DCUser::GetDCUser(stInfo.ulUserID);

        if (pstUser)
        {
            stInfo.ulCallStatus = pstUser->ulCallStatus;
        }

        vUser.push_back(stInfo);
    }

    DCUser::InitDCUser(vUser);

    return TRUE;
}

/**
 * @brief           从数据库中加载表 dcoperator 中的信息
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-15 11:45:59
 * @note
 */
BOOL LoadDCOperatorInfo(CHAR *szMsgInfo)
{
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadDCOperatorInfo, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
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
            !pSubJson->GetValue("Password", stInfo.acPassword, sizeof(stInfo.acPassword)) ||
            !pSubJson->GetValue("DCType", &stInfo.ulDCType) )
        {
            GosLog(LOG_ERROR, "parse dc operator info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        vUser.push_back(stInfo);
    }

    DCOperator::InitDCOperator(vUser);

    return TRUE;
}

/**
 * @brief           加载固定台信息， 数据库表 stationue
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-15 19:15:23
 * @note
 */
BOOL LoadStationUEInfo(CHAR *szMsgInfo)
{
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadStationUEInfo, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    STATION_UE_INFO_T   stInfo = {0};
    VectorMap<UINT32, STATION_UE_INFO_T>     mInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("UserID", &stInfo.ulUserID) ||
            !pSubJson->GetValue("Name", stInfo.acName, sizeof(stInfo.acName)) ||
            !pSubJson->GetValue("LanAddr", stInfo.acLanAddr, sizeof(stInfo.acLanAddr))   ||
            !pSubJson->GetValue("StationID", &stInfo.ulStationID)   ||
            !pSubJson->GetValue("DefaultGroupID", &stInfo.i64DefaultGroupID)  )
        {
            GosLog(LOG_ERROR, "parse fixed ue failed on %s", pSubJson->GetText());
            return FALSE;
        }

        mInfo.Add(stInfo.ulUserID, stInfo);
    }

    StationUE::InitStationUE(mInfo);

    return TRUE;
}

/**
 * @brief           加载车载台信息， 数据库表 TrainUEInfo
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-15 19:15:48
 * @note
 */
BOOL LoadTrainUEInfo(CHAR *szMsgInfo)
{
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadTrainUEInfo, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    TRAIN_UE_INFO_T stInfo = {0};
    VectorMap<UINT32, TRAIN_UE_INFO_T>   mInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("UserID", &stInfo.ulUserID) ||
            !pSubJson->GetValue("Name", stInfo.acName, sizeof(stInfo.acName)) ||
            !pSubJson->GetValue("LanAddr", stInfo.acLanAddr, sizeof(stInfo.acLanAddr))   ||
            !pSubJson->GetValue("TrainUnitID", &stInfo.ulTrainUnitID)  /* ||
            !pSubJson->GetValue("DefaultGroupID", &stInfo.i64DefaultGroupID) */ )
        {
            GosLog(LOG_ERROR, "parse train ue failed on %s", pSubJson->GetText());
            return FALSE;
        }

        mInfo.Add(stInfo.ulUserID, stInfo);
    }

    TrainUE::InitTrainUE(mInfo);

    return TRUE;
}

/**
 * @brief           加载手持台信息， portalue 表中信息
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-15 19:35:33
 * @note
 */
BOOL LoadPortalUEInfo(CHAR *szMsgInfo)
{
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadPortalUEInfo, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    PORTAL_UE_INFO_T    stInfo = {0};
    VectorMap<UINT32, PORTAL_UE_INFO_T>  mInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("UserID", &stInfo.ulUserID) ||
            !pSubJson->GetValue("Name", stInfo.acName, sizeof(stInfo.acName)) ||
            !pSubJson->GetValue("Type", &stInfo.ulType)   ||
            !pSubJson->GetValue("FuncType", &stInfo.ulFuncType)   ||
            !pSubJson->GetValue("StationID", &stInfo.ulStationID) ||
            !pSubJson->GetValue("Lon", &stInfo.dLon) ||
            !pSubJson->GetValue("Lat", &stInfo.dLat) )
        {
            GosLog(LOG_ERROR, "parse train ue failed on %s", pSubJson->GetText());
            return FALSE;
        }

        mInfo.Add(stInfo.ulUserID, stInfo);

        GIS::InitGISInfo(stInfo.ulUserID, stInfo.dLon, stInfo.dLat);
    }

    PortalUE::InitPortalUE(mInfo);

    return TRUE;
}

BOOL LoadGroupInfo(CHAR *szMsgInfo)
{
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadGroupInfo, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    GROUP_INFO_T    stInfo;
    VECTOR<GROUP_INFO_T>  vInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("GroupID", &stInfo.i64GroupID) ||
            !pSubJson->GetValue("Name", stInfo.acGroupName, sizeof(stInfo.acGroupName)) ||
            !pSubJson->GetValue("Type", &stInfo.ulGroupType) ||
            !pSubJson->GetValue("FuncType", &stInfo.ulFuncType))
        {
            GosLog(LOG_ERROR, "parse group info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        vInfo.push_back(stInfo);
    }

    Group::InitGroupMarkInfo(vInfo);

    return TRUE;
}

/**
 * @brief           本调度员自动加入的群组的信息，数据来源 dis(db: autojoingroup)
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-16 09:23:32
 * @note
 */
BOOL LoadAutoJoinGroupInfo(CHAR *szMsgInfo)
{
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadAutoJoinGroupInfo, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    GROUP_STATUS_T  stStatus;
    VECTOR<GROUP_STATUS_T>   vStatus;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("GroupID", &stStatus.i64GroupID)   )
        {
            GosLog(LOG_ERROR, "parse auto join group info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        stStatus.ulGroupStatus = GROUP_STATUS_TOJOIN;
        stStatus.bNeedSave = FALSE;

        vStatus.push_back(stStatus);
    }

    Group::InitAutoJoinGroup(vStatus);

    return TRUE;
}

/**
 * @brief           加载本调度员自动订阅的组信息， 数据来源 dis(db: subgroup)
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-16 09:30:16
 * @note
 */
BOOL LoadSubGroupInfo(CHAR *szMsgInfo)
{
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadSubGroupInfo, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    INT64           i64GroupID;
    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();

    g_sSubGroup.Clear();

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("GroupID", &i64GroupID)   )
        {
            GosLog(LOG_ERROR, "parse sub group info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        g_sSubGroup.Add(i64GroupID);
    }

    return TRUE;
}

/**
 * @brief           加载自动订阅视频组， 数据来源 dis(db: subvideogroup)
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-16 09:34:25
 * @note
 */
BOOL LoadSubVideoGroupInfo(CHAR *szMsgInfo)
{
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadSubVideoGroupInfo, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    INT64           i64GroupID;
    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();

    g_sSubVideoGroup.Clear();

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("GroupID", &i64GroupID)   )
        {
            GosLog(LOG_ERROR, "parse sub group info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        g_sSubVideoGroup.Add(i64GroupID);
        GosLog(LOG_INFO, "add subvideogroup " FORMAT_I64 , i64GroupID);
    }

    return TRUE;
}

/**
 * @brief           加载本调度台应该自动订阅的调度员用户, 数据来源 dis(db: subuser)
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-16 09:35:41
 * @note
 */
BOOL LoadSubUserInfo(CHAR *szMsgInfo)
{
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadSubUserInfo, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    UINT32          ulUserID;

    g_sSubUser.Clear();

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("UserID", &ulUserID)   )
        {
            GosLog(LOG_ERROR, "parse sub user info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        g_sSubUser.Add(ulUserID);
    }

    return TRUE;
}

BOOL LoadSubVideoUserInfo(CHAR *szMsgInfo)
{
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadSubVideoUserInfo, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    UINT32          ulUserID;

    g_sSubVideoUser.Clear();

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("UserID", &ulUserID)   )
        {
            GosLog(LOG_ERROR, "parse sub video user info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        g_sSubVideoUser.Add(ulUserID);
    }

    return TRUE;
}

BOOL LoadMultiGroupInfo(CHAR *szMsgInfo)
{
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadMultiGroupInfo, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    MULTI_GROUP_INFO_T  stInfo;
    VECTOR<MULTI_GROUP_INFO_T>  vInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("DCUserID", &stInfo.ulDCUserID) ||
            !pSubJson->GetValue("GroupID", &stInfo.i64GroupID) ||
            !pSubJson->GetValue("GroupName", stInfo.acGroupName, sizeof(stInfo.acGroupName)) ||
            !pSubJson->GetValue("GroupMember", stInfo.acGroupMember, sizeof(stInfo.acGroupMember)) )
        {
            GosLog(LOG_ERROR, "parse multi group info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        vInfo.push_back(stInfo);
    }

    MultiGroup::InitAllMultiGroup(vInfo);

    return TRUE;
}

BOOL ParseTrainInfo(GJson *pSubJson, TRAIN_INFO_T &stInfo)
{
    CHAR        *szValue;
    GJson       Json;
    APP_TRAIN_INFO_T        *pstTrain;

    memset(&stInfo, 0, sizeof(stInfo));

    if (!pSubJson->GetValue("TimeDiff", &stInfo.ulTimeDiff) ||
//        !pSubJson->GetValue("UpdateTime", &stInfo.ulUpdateTime) ||
        !pSubJson->GetValue("UnitID", &stInfo.ulTrainUnitID) ||
        !pSubJson->GetValue("Type", &stInfo.ulTrainType) ||
        !pSubJson->GetValue("Name", stInfo.acTrainName, sizeof(stInfo.acTrainName)) ||
//        !pSubJson->GetValue("TrainServiceNumber", &stInfo.ulTrainServiceNumber) ||
//        !pSubJson->GetValue("TrainSerialNumber", &stInfo.usTrainSerialNumber) ||
        !pSubJson->GetValue("TrainID", stInfo.acTrainID, sizeof(stInfo.acTrainID)) ||
        !pSubJson->GetValue("DriverID", &stInfo.ulDriverID) ||
        !pSubJson->GetValue("ComeTime", &stInfo.ulComeTime) ||
        !pSubJson->GetValue("ParkTime", &stInfo.ulParkTime) ||
        !pSubJson->GetValue("LeaveTime", &stInfo.ulLeaveTime) ||
        !pSubJson->GetValue("DestID", stInfo.acDestID, sizeof(stInfo.acDestID)) ||
        !pSubJson->GetValue("RollingStockNum", &stInfo.ulRollingStockNum) ||
        !pSubJson->GetValue("Direction", &stInfo.ulDirection) ||
        !pSubJson->GetValue("DriveMode", &stInfo.ulDriveMode) ||
        !pSubJson->GetValue("StationID", &stInfo.ulStationID) ||
        !pSubJson->GetValue("PrevStationID", &stInfo.ulUpsideStationID) ||
        !pSubJson->GetValue("NextStationID", &stInfo.ulDownsideStationID) ||
        !pSubJson->GetValue("TurnbackTrackFlag", &stInfo.ulTurnbackTrackFlag) ||
        !pSubJson->GetValue("DepotTrackFlag", &stInfo.ulDepotTrackFlag) ||
        !pSubJson->GetValue("SectionID", &stInfo.usSectionID) ||
        !pSubJson->GetValue("PlatformID", &stInfo.ulPlatformID) ||
        !pSubJson->GetValue("TransferTrackFlag", &stInfo.ulTransferTrackFlag) ||
        !pSubJson->GetValue("InOutDepot", &stInfo.ulInOutDepot) ||
        !pSubJson->GetValue("LostRealLocation", &stInfo.ulLostRealLocation) ||
        !pSubJson->GetValue("TrainOrder", &stInfo.iTrainOrder) ||
        !pSubJson->GetValue("TrackName", stInfo.acTrackName, sizeof(stInfo.acTrackName)) ||
        !pSubJson->GetValue("TrainStationPos", &stInfo.iTrainStationPos) ||
        !pSubJson->GetValue("DefaultGroupID", &stInfo.i64DefaultGroupID) ||
        !pSubJson->GetValue("Register", &stInfo.bRegister) ||
        !pSubJson->GetValue("RegisterDepotID", &stInfo.ulRegisterDepotID) ||
        !pSubJson->GetValue("TrainStatus", &stInfo.ulTrainStatus) ||
        !pSubJson->GetValue("MasterStatus", &stInfo.stIPHAudioNetInfo.ulMasterStatus) ||
        !pSubJson->GetValue("IPHStatus", &stInfo.stIPHAudioNetInfo.ulIPHStatus) ||
        !pSubJson->GetIP("IPHAddr", stInfo.stIPHAudioNetInfo.aucIPHAddr)||
        !pSubJson->GetValue("Lon", &stInfo.dLon) ||
        !pSubJson->GetValue("Lat", &stInfo.dLat) )
    {
        GosLog(LOG_ERROR, "ParseTrainInfo fail in msg : %s", pSubJson->GetText());
        return FALSE;
    }

    szValue = pSubJson->GetValue("AppTrainInfo1");
    if (!szValue)
    {
        GosLog(LOG_ERROR, "no AppTrainInfo1 in msg : %s", pSubJson->GetText());
        return FALSE;
    }

    Json.Parse(szValue);
    pstTrain = &stInfo.astAppTrainInfo[0];
    if (!Json.GetValue("TrainID", pstTrain->acTrainID, sizeof(pstTrain->acTrainID)) ||
        !Json.GetValue("TC", &pstTrain->ulTC) ||
        !Json.GetValue("CurrentGroupID", &pstTrain->i64CurrentGroupID) ||
        !Json.GetValue("StationID", &pstTrain->ulStationID) ||
        !Json.GetValue("CellID", &pstTrain->ulCellID) ||
        !Json.GetValue("CallType", &pstTrain->ulCallType) ||
        !Json.GetValue("CallID", &pstTrain->i64CallID) ||
        !Json.GetValue("CurrentTime", &pstTrain->ulCurrentTime) ||
        !Json.GetValue("Uptime", &pstTrain->ulUptime) ||
        !Json.GetValue("SerialNumber", pstTrain->acSerialNumber, sizeof(pstTrain->acSerialNumber)) ||
        !Json.GetValue("APPVer", pstTrain->acAPPVer, sizeof(pstTrain->acAPPVer)) ||
        !Json.GetValue("IsIPHOnline", &pstTrain->bIsIPHOnline) ||
        !Json.GetValue("IPHStatus", &pstTrain->ulIPHStatus) ||
        !Json.GetValue("IsPAOnline", &pstTrain->bIsPAOnline) ||
        !Json.GetValue("PAStatus", &pstTrain->ulPAStatus) ||
        !Json.GetValue("IsMasterPA", &pstTrain->bIsMasterPA) )
    {
        return FALSE;
    }

    szValue = pSubJson->GetValue("AppTrainInfo2");
    if (!szValue)
    {
        GosLog(LOG_ERROR, "no AppTrainInfo2 in msg : %s", pSubJson->GetText());
        return FALSE;
    }

    Json.Parse(szValue);
    pstTrain = &stInfo.astAppTrainInfo[1];
    if (!Json.GetValue("TrainID", pstTrain->acTrainID, sizeof(pstTrain->acTrainID)) ||
        !Json.GetValue("TC", &pstTrain->ulTC) ||
        !Json.GetValue("CurrentGroupID", &pstTrain->i64CurrentGroupID) ||
        !Json.GetValue("StationID", &pstTrain->ulStationID) ||
        !Json.GetValue("CellID", &pstTrain->ulCellID) ||
        !Json.GetValue("CallType", &pstTrain->ulCallType) ||
        !Json.GetValue("CallID", &pstTrain->i64CallID) ||
        !Json.GetValue("CurrentTime", &pstTrain->ulCurrentTime) ||
        !Json.GetValue("Uptime", &pstTrain->ulUptime) ||
        !Json.GetValue("SerialNumber", pstTrain->acSerialNumber, sizeof(pstTrain->acSerialNumber)) ||
        !Json.GetValue("APPVer", pstTrain->acAPPVer, sizeof(pstTrain->acAPPVer)) ||
        !Json.GetValue("IsIPHOnline", &pstTrain->bIsIPHOnline) ||
        !Json.GetValue("IPHStatus", &pstTrain->ulIPHStatus) ||
        !Json.GetValue("IsPAOnline", &pstTrain->bIsPAOnline) ||
        !Json.GetValue("PAStatus", &pstTrain->ulPAStatus) ||
        !Json.GetValue("IsMasterPA", &pstTrain->bIsMasterPA) )
    {
        return FALSE;
    }

    return TRUE;
}

BOOL LoadTrainInfo(CHAR *szMsgInfo)
{
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadTrainInfo, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    TRAIN_INFO_T    stInfo = {0};
    UINT32          ulATSDataTimeDiff = 0;
    VECTOR<TRAIN_INFO_T>    vInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!ParseTrainInfo(pSubJson, stInfo))
        {
            GosLog(LOG_ERROR, "parse train info failed on %s of %s", pSubJson->GetCurrKey(), pSubJson->GetText());
            return FALSE;
        }

        if (i == 0)
        {
            if (!pSubJson->GetValue("ATSDataTimeDiff", &ulATSDataTimeDiff) )
            {
                GosLog(LOG_ERROR, "invalid ATSDataTimeDiff of %s", pSubJson->GetText());
                ulATSDataTimeDiff = 0;

//                return FALSE;
            }

        }

        stInfo.bRegister = FALSE;
        stInfo.ulRegisterDepotID = DEPOT_ID_UNKNOWN;
        vInfo.push_back(stInfo);
    }

    Train::InitTrainInfo(vInfo, ulATSDataTimeDiff);

    return TRUE;
}

/**
 * @brief           从 dis 获取数据库表 depot 表中的所有信息
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-15 14:10:51
 * @note
 */
BOOL LoadDepotInfo(CHAR *szMsgInfo)
{
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadDepotInfo, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
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

    Depot::InitDepot(vInfo);

    return TRUE;
}

/**
 * @brief           加载车站信息，数据来源 dis(db: station)
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-16 10:16:41
 * @note
 */
BOOL LoadStationInfo(CHAR *szMsgInfo)
{
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadStationInfo, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    STATION_INFO_T  stInfo;
    VECTOR<STATION_INFO_T>  vInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("StationIndex", &stInfo.ulStationIndex) ||
            !pSubJson->GetValue("StationID", &stInfo.ulStationID) ||
            !pSubJson->GetValue("StationName", stInfo.acStationName, sizeof(stInfo.acStationName)) ||
            !pSubJson->GetValue("StationType", &stInfo.ulStationType) ||
            !pSubJson->GetValue("DepotID", &stInfo.ulDepotID) ||
            !pSubJson->GetValue("InterLockStationID", &stInfo.ulInterLockStationID) ||
            !pSubJson->GetValue("StationGroupID", &stInfo.i64StationGroupID) )
        {
            GosLog(LOG_ERROR, "parse station info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        vInfo.push_back(stInfo);
    }

    Station::InitStation(vInfo);

    return TRUE;
}

/**
 * @brief           获取广播区域信息， 数据来源 dis(db: brdzoneinfo)
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-16 10:29:47
 * @note
 */
BOOL LoadBrdZoneInfo(CHAR *szMsgInfo)
{
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadBrdZoneInfo, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    BRD_ZONE_INFO_T stInfo;
    VECTOR<BRD_ZONE_INFO_T>     vInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("GroupID", &stInfo.i64GroupID) ||
            !pSubJson->GetValue("StationID", &stInfo.ulStationID) ||
            !pSubJson->GetValue("ZoneID", &stInfo.ulZoneID) ||
            !pSubJson->GetValue("ZoneName", stInfo.acZoneName, sizeof(stInfo.acZoneName)) )
        {
            GosLog(LOG_ERROR, "parse zone brd info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        vInfo.push_back(stInfo);
    }

    Group::InitBrdZoneInfo(vInfo);

    return TRUE;
}

BOOL LoadSectionInfo(CHAR *szMsgInfo)
{
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadSectionInfo, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    SECTION_INFO_T  stInfo;
    VECTOR<SECTION_INFO_T>     vInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("ID", &stInfo.ulSectionID) ||
            !pSubJson->GetValue("Name", stInfo.acSectionName, sizeof(stInfo.acSectionName)) ||
            !pSubJson->GetValue("StationID", &stInfo.ulStationID) ||
            !pSubJson->GetValue("x1", &stInfo.x1)  )
        {
            GosLog(LOG_ERROR, "parse section info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        if (stInfo.x1 >= 0)
        {
            if (!pSubJson->GetValue("y1", &stInfo.y1) ||
                !pSubJson->GetValue("x2", &stInfo.x2) ||
                !pSubJson->GetValue("y2", &stInfo.y2) )
            {
                GosLog(LOG_ERROR, "parse section info failed on %s", pSubJson->GetText());
                return FALSE;
            }
        }
        else
        {
            stInfo.y1 = -1;
            stInfo.x2 = -1;
            stInfo.y2 = -1;
        }

        vInfo.push_back(stInfo);
    }

    Section::InitSection(vInfo);

    return TRUE;
}

BOOL LoadDestInfo(CHAR *szMsgInfo)
{
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadDestInfo, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    DEST_INFO_T     stInfo;
    VECTOR<DEST_INFO_T>     vInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("DestID", &stInfo.ulDestID) ||
            !pSubJson->GetValue("DestName", stInfo.acDestName, sizeof(stInfo.acDestName)) ||
            !pSubJson->GetValue("StationID", &stInfo.ulStationID) )
        {
            GosLog(LOG_ERROR, "parse dest info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        vInfo.push_back(stInfo);
    }

    DestInfo::InitDestInfo(vInfo);

    return TRUE;
}

/**
 * @brief           从数据库中加载操作日志的信息
 * @param           ulStartTime     [in]    查询时间区间的开始
 * @param           ulEndTime       [in]    查询时间区间的结束
 * @param           ulDCUserID      [in]    查询的调度台号码
 * @param           vInfo           [out]   返回查询到的信息
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-16 10:40:44
 * @note
 */
BOOL LoadOperLogInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulDCUserID, VECTOR<OPER_LOG_INFO_T> &vInfo)
{
    GJsonParam  Param;
    GJson       RspJson;

    Param.Add("StartTime", ulStartTime);
    Param.Add("EndTime", ulEndTime);
    Param.Add("DCUserID", ulDCUserID);

    CHAR        *szMsgInfo = LoadCfgData(MSG_DCLoadOperLogInfo, Param, RspJson);

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

BOOL LoadP2PCallInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulCallType, UINT32 ulUserID, VECTOR<P2P_CALL_INFO_T> &vInfo)
{
    GJsonParam  Param;
    GJson       RspJson;

    Param.Add("StartTime", ulStartTime);
    Param.Add("EndTime", ulEndTime);
    Param.Add("CallType", ulCallType);
    Param.Add("UserID", ulUserID);

    CHAR        *szMsgInfo = LoadCfgData(MSG_DCLoadP2PCallInfo, Param, RspJson);

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
    P2P_CALL_INFO_T stInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("Caller", &stInfo.ulCaller) ||
            !pSubJson->GetValue("Callee", &stInfo.ulCallee) ||
            !pSubJson->GetValue("Time",   &stInfo.ulTime)   ||
            !pSubJson->GetValue("CallType", &stInfo.ulCallType)   ||
            !pSubJson->GetValue("Status", &stInfo.ulStatus) )
        {
            GosLog(LOG_ERROR, "parse P2P call info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        vInfo.push_back(stInfo);
    }

    return TRUE;
}

BOOL LoadGroupCallInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulCallType, UINT32 ulDCUserID, INT64 i64GroupID, VECTOR<GROUP_CALL_INFO_T> &vInfo)
{
    GJsonParam  Param;
    GJson       RspJson;

    Param.Add("StartTime", ulStartTime);
    Param.Add("EndTime", ulEndTime);
    Param.Add("CallType", ulCallType);
    Param.Add("DCUserID", ulDCUserID);
    Param.Add("GroupID", i64GroupID);

    CHAR        *szMsgInfo = LoadCfgData(MSG_DCLoadGroupCallInfo, Param, RspJson);

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
    GROUP_CALL_INFO_T   stInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("DCUserID", &stInfo.ulDCUserID) ||
            !pSubJson->GetValue("GroupID", &stInfo.i64GroupID) ||
            !pSubJson->GetValue("Time",    &stInfo.ulTime)     ||
            !pSubJson->GetValue("CallType",&stInfo.ulCallType) ||
            !pSubJson->GetValue("Speaker", &stInfo.ulSpeaker)  )
        {
            GosLog(LOG_ERROR, "parse group call info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        vInfo.push_back(stInfo);
    }

    return TRUE;
}

BOOL LoadReqCallInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulReqCallType, VECTOR<REQ_CALL_INFO_T> &vInfo)
{
    GJsonParam  Param;
    GJson       RspJson;

    Param.Add("StartTime", ulStartTime);
    Param.Add("EndTime", ulEndTime);
    Param.Add("ReqCallType", ulReqCallType);

    CHAR        *szMsgInfo = LoadCfgData(MSG_DCLoadReqCallInfo, Param, RspJson);

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
    REQ_CALL_INFO_T stInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("ReqTime", &stInfo.ulReqTime) ||
            !pSubJson->GetValue("ReqCallType", &stInfo.ulReqCallType) ||
            !pSubJson->GetValue("ReqUserID", &stInfo.ulReqUserID)     ||
            !pSubJson->GetValue("Caller", &stInfo.ulCaller) ||
            !pSubJson->GetValue("DCUserID", &stInfo.ulDCUserID) ||
            !pSubJson->GetValue("AcceptTime", &stInfo.ulAcceptTime)  )
        {
            GosLog(LOG_ERROR, "parse req call info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        vInfo.push_back(stInfo);
    }

    return TRUE;
}

BOOL LoadRecBrdInfo(UINT32 ulStartTime, UINT32 ulEndTime, VECTOR<REC_BRD_INFO_T> &vInfo)
{
    GJsonParam  Param;
    GJson       RspJson;

    Param.Add("StartTime", ulStartTime);
    Param.Add("EndTime", ulEndTime);

    CHAR        *szMsgInfo = LoadCfgData(MSG_DCLoadRecBrdInfo, Param, RspJson);

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
    REC_BRD_INFO_T  stInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("Time", &stInfo.ulTime) ||
            !pSubJson->GetValue("RecID", &stInfo.ulRecID) ||
            !pSubJson->GetValue("RecInfo", stInfo.acRecInfo, sizeof(stInfo.acRecInfo)) ||
            !pSubJson->GetValue("BrdLoopTime", &stInfo.ulBrdLoopTime) ||
            !pSubJson->GetValue("DCUserID", &stInfo.ulDCUserID) ||
            !pSubJson->GetValue("TrainList", stInfo.acTrainList, sizeof(stInfo.acTrainList))  )
        {
            GosLog(LOG_ERROR, "parse rec brd info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        vInfo.push_back(stInfo);
    }

    return TRUE;
}

BOOL LoadRealBrdInfo(UINT32 ulStartTime, UINT32 ulEndTime, VECTOR<REAL_BRD_INFO_T> &vInfo)
{
    GJsonParam  Param;
    GJson       RspJson;

    Param.Add("StartTime", ulStartTime);
    Param.Add("EndTime", ulEndTime);

    CHAR        *szMsgInfo = LoadCfgData(MSG_DCLoadRealBrdInfo, Param, RspJson);

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
    REAL_BRD_INFO_T stInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("StartTime", &stInfo.ulStartTime) ||
            !pSubJson->GetValue("EndTime", &stInfo.ulEndTime) ||
            !pSubJson->GetValue("DCUserID", &stInfo.ulDCUserID) ||
            !pSubJson->GetValue("TrainList", stInfo.acTrainList, sizeof(stInfo.acTrainList))  )
        {
            GosLog(LOG_ERROR, "parse real brd info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        vInfo.push_back(stInfo);
    }

    return TRUE;
}

BOOL LoadIPHTalkInfo(UINT32 ulStartTime, UINT32 ulEndTime, VECTOR<IPH_TALK_INFO_T> &vInfo)
{
    GJsonParam  Param;
    GJson       RspJson;

    Param.Add("StartTime", ulStartTime);
    Param.Add("EndTime", ulEndTime);
//  Param.Add("ReqCallType", ulReqCallType);

    CHAR        *szMsgInfo = LoadCfgData(MSG_DCLoadIPHTalkInfo, Param, RspJson);

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
    IPH_TALK_INFO_T stInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("ApplyTime", &stInfo.ulApplyTime) ||
            !pSubJson->GetValue("TrainUnitID", &stInfo.ulTrainUnitID) ||
            !pSubJson->GetValue("TrainID", stInfo.acTrainID, sizeof(stInfo.acTrainID)) ||
            !pSubJson->GetValue("CarriageID", &stInfo.ulCarriageID) ||
            !pSubJson->GetValue("IPHDevID", &stInfo.ulIPHDevID) ||
            !pSubJson->GetValue("AcceptDCUserID", &stInfo.ulAcceptDCUserID) ||
            !pSubJson->GetValue("AcceptTime", &stInfo.ulAcceptTime) ||
            !pSubJson->GetValue("StopTime", &stInfo.ulStopTime)  )
        {
            GosLog(LOG_ERROR, "parse IPH talk info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        vInfo.push_back(stInfo);
    }

    return TRUE;
}

/**
 * @brief           加载预定义短消息, 数据来源 dis db: predefinedsms
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-16 10:44:55
 * @note
 */
BOOL LoadPredefinedSMS(CHAR *szMsgInfo)
{
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadPredefinedSMS, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    PREDEFINED_SMS_INFO_T           stInfo = {0};
    VECTOR<PREDEFINED_SMS_INFO_T>   vInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("SMSID", &stInfo.ulSMSID) ||
            !pSubJson->GetValue("SDSType", &stInfo.ulSDSType) ||
            !pSubJson->GetValue("SMSTemplateType", &stInfo.ulSMSTemplateType) ||
            !pSubJson->GetValue("SMSInfo", stInfo.acSMSInfo, sizeof(stInfo.acSMSInfo))  )
        {
            GosLog(LOG_ERROR, "parse predefined SMS failed on %s", pSubJson->GetText());
            return FALSE;
        }

        DecodeJsonMsg(stInfo.acSMSInfo, sizeof(stInfo.acSMSInfo)-1);
        vInfo.push_back(stInfo);
    }

    InitPredefinedSMS(vInfo);

    return TRUE;
}

/**
 * @brief           加载电子工单模板， 数据来源 dis db: SDSTemplate
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-16 11:22:33
 * @note
 */
BOOL LoadSDSTemplate(CHAR *szMsgInfo)
{
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadSDSTemplate, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    SDS_TEMPLATE_T          stInfo;
    VECTOR<SDSTemplate*>    vTemplate;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("TemplateID", &stInfo.ulTemplateID) ||
            !pSubJson->GetValue("SDSType", &stInfo.ulSDSType) ||
            !pSubJson->GetValue("Title", stInfo.acTitle, sizeof(stInfo.acTitle)) ||
            !pSubJson->GetValue("SDSInfo", stInfo.acSDSInfo, sizeof(stInfo.acSDSInfo)))
        {
            GosLog(LOG_ERROR, "parse sds template failed on %s", pSubJson->GetText());
            return FALSE;
        }

        DecodeJsonMsg(stInfo.acSDSInfo, sizeof(stInfo.acSDSInfo)-1);

        SDSTemplate     *pTemplate = new SDSTemplate();

        if (!pTemplate->Init(stInfo))
        {
            delete pTemplate;
            for (UINT32 j=0; j<vTemplate.size(); j++)
            {
                pTemplate = vTemplate[j];
                delete pTemplate;
            }

            return FALSE;
        }

        vTemplate.push_back(pTemplate);
    }

    InitSDSTemplate(vTemplate);

    return TRUE;
}

BOOL LoadSDSInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulSDSType,
                 CHAR *szSendUserName, CHAR *szKeyword, VECTOR<SDS_INFO_T> &vInfo)
{
    GJsonParam  Param;
    GJson       RspJson;

    Param.Add("StartTime", ulStartTime);
    Param.Add("EndTime", ulEndTime);
    Param.Add("SDSType", ulSDSType);
    Param.Add("SendUserName", szSendUserName);
    Param.Add("Keyword", szKeyword);

    CHAR        *szMsgInfo = LoadCfgData(MSG_DCGetSDSInfo, Param, RspJson);

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
    SDS_INFO_T      stInfo;

    vInfo.clear();
    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];

        if (!pSubJson->GetValue("SDSID", &stInfo.ulSDSID) ||
            !pSubJson->GetValue("SendUserID", &stInfo.ulSendUserID) ||
            !pSubJson->GetValue("SendUserName", stInfo.acSendUserName, sizeof(stInfo.acSendUserName)) ||
            !pSubJson->GetValue("SendTime", &stInfo.ulSendTime) ||
            !pSubJson->GetValue("SDSIndex", &stInfo.ulSDSIndex) ||
            !pSubJson->GetValue("SDSType", &stInfo.ulSDSType) ||
            !pSubJson->GetValue("SDSInfo", stInfo.acSDSInfo, sizeof(stInfo.acSDSInfo)) ||
            !pSubJson->GetValue("NeedReply", &stInfo.bNeedReply) ||
            !pSubJson->GetValue("RecvUserNum", &stInfo.ulRecvUserNum) ||
            !pSubJson->GetValue("ReplyUserNum", &stInfo.ulReplyUserNum) ||
            !pSubJson->GetValue("File", stInfo.acFile, sizeof(stInfo.acFile) ))
        {
            GosLog(LOG_ERROR, "parse sds info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        DecodeJsonMsg(stInfo.acSDSInfo, sizeof(stInfo.acSDSInfo)-1);

        vInfo.push_back(stInfo);
    }

    return TRUE;
}

/**
 * @brief           加载预录音广播信息， 数据来源 recbrdinfo
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-16 11:38:28
 * @note
 */
BOOL LoadRecBrdCfg(CHAR *szMsgInfo)
{
    GJson       RspJson;

    if (!szMsgInfo)
    {
        szMsgInfo = LoadCfgData(MSG_DCLoadRecBrdCfg, RspJson);
        if (!szMsgInfo)
        {
            return FALSE;
        }
    }

    GJson           MsgInfoJson;

    if (!MsgInfoJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = MsgInfoJson.GetSubJson();
    REC_BRD_CFG_T   stInfo;
    VECTOR<REC_BRD_CFG_T>   vInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("RecID", &stInfo.ulRecID) ||
            !pSubJson->GetValue("RecInfo", stInfo.acRecInfo, sizeof(stInfo.acRecInfo)))
        {
            GosLog(LOG_ERROR, "parse rec broadcast info on %s", pSubJson->GetText());
            return FALSE;
        }

        vInfo.push_back(stInfo);
    }

    InitRecBrdCfg(vInfo);

    return TRUE;
}

BOOL LoadCfgData()
{
    GetMaxInitNum();

 //   LOAD_DATA_CHECK("配置数据", "app cfg", LoadAPPCfg);
    LOAD_DATA_CHECK("配置数据", "group cfg", LoadGroupCfg);
    LOAD_DATA_CHECK("固定台数据", "station user data", LoadStationUEInfo);
    LOAD_DATA_CHECK("车载台数据", "train user data", LoadTrainUEInfo);
    LOAD_DATA_CHECK("手持台数据", "portal user data", LoadPortalUEInfo);
    LOAD_DATA_CHECK("群组数据", "auto join group data", LoadAutoJoinGroupInfo);
    LOAD_DATA_CHECK("群组数据", "subscribe group data", LoadSubGroupInfo);
    LOAD_DATA_CHECK("群组数据", "subscribe video group data", LoadSubVideoGroupInfo);
    LOAD_DATA_CHECK("群组数据", "group info", LoadGroupInfo);
    LOAD_DATA_CHECK("群组数据", "multi group info", LoadMultiGroupInfo);
    LOAD_DATA_CHECK("用户数据", "subscribe user data", LoadSubUserInfo);
    LOAD_DATA_CHECK("用户数据", "subscribe video user data", LoadSubVideoUserInfo);
    LOAD_DATA_CHECK("列车数据", "train data", LoadTrainInfo);
    LOAD_DATA_CHECK("车站数据", "depot data", LoadDepotInfo);
    LOAD_DATA_CHECK("车站数据", "station data", LoadStationInfo);
    LOAD_DATA_CHECK("车站数据", "station broadcast group data", LoadBrdZoneInfo);
    LOAD_DATA_CHECK("车站数据", "section", LoadSectionInfo);
    LOAD_DATA_CHECK("车站数据", "dest data", LoadDestInfo);

    LOAD_DATA_CHECK("预定义短消息", "predefined sms", LoadPredefinedSMS);
    LOAD_DATA_CHECK("电子工单模板", "dispatch command template", LoadSDSTemplate);
    LOAD_DATA_CHECK("预录音广播数据", "rec broadcast cfg", LoadRecBrdCfg);

    GosLog(LOG_FATAL, "load cfg data successful!");

    return TRUE;
}
