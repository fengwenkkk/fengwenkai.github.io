#include <vcl.h>
#include <Registry.hpp>

#include "ClientInclude.h"
#include "pis.h"
#include "main.h"
#include "ThreadOMA.h"
#include "SDSTemplate.h"
#include "TrainPosCall_Public.h"

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

extern UINT32      g_ulCurrDCUserID;

const char  *g_szNativeServiceName = "eLTE_NativeService.exe";

std::vector<DB_UE_TRAIN_POS_CALL_INFO_T> g_vecTrainPosCall;

typedef struct
{
    const char      *szLoadType;
    LOAD_CFG_FUNC   pfLoadFunc;
    UINT32          ulCfgVer;
}LOAD_CFG_INFO_T;

LOAD_CFG_INFO_T     g_astLoadCfgInfo[] = {
    {MSG_DCLoadTrainInfo,          LoadTrainInfo        , 0},
//  {MSG_DCLoadDepotInfo,          LoadDepotInfo        , 0},
    {MSG_DCLoadStationInfo,        LoadStationInfo      , 0},
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
                                    LocalCfg::GetInstance().GetMdcIP().ToIPAddr().aucIP,
                                    LocalCfg::GetInstance().GetOmcIP().ToIPAddr().aucIP,
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

    if (pRspJson)
    {
        szRsp = (CHAR*)calloc(1, ulBufLen);
        if (!szRsp)
        {
            return NULL;
        }
    }

    ReqParam.Add("MsgName", szDataType);
    ReqParam.Add("DCUserID", g_ulCurrDCUserID);
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

BOOL ParseTrainInfo(GJson *pSubJson, TRAIN_INFO_T &stInfo)
{
    CHAR        *szValue;
    GJson       Json;
    APP_TRAIN_INFO_T        *pstTrain;

    if (//!pSubJson->GetValue("TimeDiff", &stInfo.ulUpdateTime) ||
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
        !pSubJson->GetValue("IndexInSection", &stInfo.ucIndexInSection) ||
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
        !pSubJson->GetValue("Lon", &stInfo.dLon) ||
        !pSubJson->GetValue("Lat", &stInfo.dLat) )
    {
        return FALSE;
    }

    szValue = pSubJson->GetValue("AppTrainInfo1");
    if (!szValue)
    {
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
    VECTOR<TRAIN_INFO_T>    vInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!ParseTrainInfo(pSubJson, stInfo))
        {
            GosLog(LOG_ERROR, "parse train info failed on %s", pSubJson->GetText());
            return FALSE;
        }

        stInfo.bRegister = FALSE;
        stInfo.ulRegisterDepotID = DEPOT_ID_UNKNOWN;
        vInfo.push_back(stInfo);
    }

    Train::InitTrainInfo(vInfo, 0);

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
            !pSubJson->GetValue("Direction", &stInfo.ulDirection) ||
            !pSubJson->GetValue("InDepotTrack", &stInfo.bInDepotTrack) ||
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
