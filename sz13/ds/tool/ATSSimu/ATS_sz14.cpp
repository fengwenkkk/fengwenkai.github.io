//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include "g_include.h"

#include "RemoteMsg.h"
#include "msg.h"
#include "ds_public.h"
#include "FrmATS.h"
#include "ClientInit.h"
#include "ClientUtil.h"
#include "LocalCfg.h"
#include "station.h"
//---------------------------------------------------------------------------
USEFORM("FrmATS.cpp", FormATS);
//---------------------------------------------------------------------------
BOOL    g_bTestMode = FALSE;
BOOL    g_bNeedLogin = TRUE;
BOOL    g_bShowTitle = TRUE;
UINT32  g_ulCurrDCUserID = INVALID_USER_ID;

typedef struct
{
    CHAR        acDCUserName[32];
    UINT8       aucDISAddr[4];
    UINT16      usDISPort;
    BOOL        bTCPMode;

}ATSLocalCfg;

RemoteMsg       *g_pRemoteMsg = NULL;

ATSLocalCfg     g_stATSLocalCfg = {0};

extern AnsiString g_strSkinName;

void ClientConnectionChange(bool bConnected)
{
}

void LoadSkinCfg()
{
}


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
    ReqParam.Add("DCUserID", g_ulCurrDCUserID);
    ReqParam.Add("MsgSeqID", 0);
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

CHAR* LoadDCSpecData(CHAR *szDataType, GJsonParam &ReqParam, GJson *pRspJson, UINT32 ulTimeout=10)
{
    ReqParam.Add("DCUserID", g_ulCurrDCUserID);

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

BOOL LoadStationInfo()
{
    GJson       RspJson;
    CHAR        *szMsgInfo = LoadDCSpecData(MSG_DCLoadStationInfo, RspJson);

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


BOOL ParseTrainInfo(GJson *pSubJson, TRAIN_INFO_T &stInfo)
{
    CHAR        *szValue;
    GJson       Json;
    APP_TRAIN_INFO_T        *pstTrain;

    if (!pSubJson->GetValue("UpdateTime", &stInfo.ulUpdateTime) ||
        !pSubJson->GetValue("UnitID", &stInfo.ulTrainUnitID) ||
        !pSubJson->GetValue("Type", &stInfo.ulTrainType) ||
        !pSubJson->GetValue("TrainServiceNumber", &stInfo.ulTrainServiceNumber) ||
        !pSubJson->GetValue("TrainSerialNumber", &stInfo.usTrainSerialNumber) ||
        !pSubJson->GetValue("TrainID", stInfo.acTrainID, sizeof(stInfo.acTrainID)) ||
        !pSubJson->GetValue("DriverID", &stInfo.ulDriverID) ||
        !pSubJson->GetValue("ComeTime", &stInfo.ulComeTime) ||
        !pSubJson->GetValue("ParkTime", &stInfo.ulParkTime) ||
        !pSubJson->GetValue("LeaveTime", &stInfo.ulLeaveTime) ||
        !pSubJson->GetValue("DestID", stInfo.acDestID, sizeof(stInfo.acDestID)) ||
        !pSubJson->GetValue("RollingStockNum", &stInfo.ulRollingStockNum) ||
        !pSubJson->GetValue("Direction", &stInfo.ulDirection) ||
        !pSubJson->GetValue("StationID", &stInfo.ulStationID) ||
        !pSubJson->GetValue("PrevStationID", &stInfo.ulUpsideStationID) ||
        !pSubJson->GetValue("NextStationID", &stInfo.ulDownsideStationID) ||
        !pSubJson->GetValue("TurnbackTrackFlag", &stInfo.ulTurnbackTrackFlag) ||
        !pSubJson->GetValue("PlatformID", &stInfo.ulPlatformID) ||
        !pSubJson->GetValue("TransferTrackFlag", &stInfo.ulTransferTrackFlag) ||
        !pSubJson->GetValue("InOutDepot", &stInfo.ulInOutDepot) ||
        !pSubJson->GetValue("LostRealLocation", &stInfo.ulLostRealLocation) ||
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
        !Json.GetValue("CurrentGroupID", &pstTrain->i64CurrentGroupID) ||
        !Json.GetValue("StationID", &pstTrain->ulStationID) ||
        !Json.GetValue("CellID", &pstTrain->ulCellID) ||
        !Json.GetValue("CallType", &pstTrain->ulCallType) ||
        !Json.GetValue("CallID", &pstTrain->i64CallID) ||
        !Json.GetValue("CurrentTime", &pstTrain->ulCurrentTime) ||
        !Json.GetValue("Uptime", &pstTrain->ulUptime) ||
        !Json.GetValue("SerialNumber", pstTrain->acSerialNumber, sizeof(pstTrain->acSerialNumber)) ||
        !Json.GetValue("APPVer", pstTrain->acAPPVer, sizeof(pstTrain->acAPPVer)) )
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
        !Json.GetValue("CurrentGroupID", &pstTrain->i64CurrentGroupID) ||
        !Json.GetValue("StationID", &pstTrain->ulStationID) ||
        !Json.GetValue("CellID", &pstTrain->ulCellID) ||
        !Json.GetValue("CallType", &pstTrain->ulCallType) ||
        !Json.GetValue("CallID", &pstTrain->i64CallID) ||
        !Json.GetValue("CurrentTime", &pstTrain->ulCurrentTime) ||
        !Json.GetValue("Uptime", &pstTrain->ulUptime) ||
        !Json.GetValue("SerialNumber", pstTrain->acSerialNumber, sizeof(pstTrain->acSerialNumber)) ||
        !Json.GetValue("APPVer", pstTrain->acAPPVer, sizeof(pstTrain->acAPPVer)) )
    {
        return FALSE;
    }

    return TRUE;
}

BOOL LoadTrainInfo()
{
    GJson       RspJson;
    CHAR        *szMsgInfo = LoadDCSpecData(MSG_DCLoadTrainInfo, RspJson);

    if (!szMsgInfo)
    {
        return FALSE;
    }

    GJson           CfgDataJson;
    GJson           MsgInfoJson;

    if (!CfgDataJson.Parse(szMsgInfo))
    {
        return FALSE;
    }

    char    *szCfgData = CfgDataJson.GetValue("CfgData");

    if (!MsgInfoJson.Parse(szCfgData) )
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

    Train::InitTrainInfo(vInfo);

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
    g_pRemoteMsg = RemoteMsg::New(g_stATSLocalCfg.aucDISAddr,
                                  g_stATSLocalCfg.usDISPort,
                                  true);
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

BOOL GetClientCfg()
{
    CHAR    acFile[256];
    CHAR    *szValue;
    INT32   iValue;

    sprintf(acFile, "%s/%s", gos_get_root_path(), "dc.ini");

    if (!gos_file_exist(acFile))
    {
        GosLog(LOG_ERROR, "open file %s failed!", acFile);
        return FALSE;
    }

    GConfFile   Conf(acFile);

    /*
    if (!Conf.Get("Sys", "user_name", g_stATSLocalCfg.acDCUserName, sizeof(g_stATSLocalCfg.acDCUserName)))
    {
        GosLog(LOG_ERROR, "get user_name of Login failed");
        return FALSE;
    }  */

    if (!Conf.Get("Sys", "dc_number", &g_ulCurrDCUserID))
    {
        GosLog(LOG_ERROR, "get dc_number of Sys failed");
        return FALSE;
    }

    szValue = Conf.Get("Sys", "dis_ip");
    if (!gos_get_ip_port(szValue, g_stATSLocalCfg.aucDISAddr, &g_stATSLocalCfg.usDISPort))
    {
        GosLog(LOG_ERROR, "get dis_ip of Sys failed");
        return FALSE;
    }

    if (g_stATSLocalCfg.usDISPort == 0)
    {
        g_stATSLocalCfg.usDISPort = GOS_DEFAULT_SERVER_PORT;
    }

    g_stATSLocalCfg.bTCPMode = FALSE;
    szValue = Conf.Get("Sys", "tcp_mode");
    if (!szValue || strcasecmp(szValue, "true") == 0)
    {
        g_stATSLocalCfg.bTCPMode = TRUE;
    }

    return TRUE;
}
//-------------------------------------------------------------------
BOOL InitClientEnvATS()
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

    LoadSkinCfg();
    InitSystemFont();
    gos_init_svcenv();

    return TRUE;
}

//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    g_strSkinName = "Lilian";

    // 提示本进程的权限
    EnablePriv();

    if (!GetClientCfg() ||
        !InitClientEnvATS() ||
        !InitRemoteMsg()
        )
    {
        return -1;
    }

    gos::PrintCurrentVersion("ATS-sz14", "V1.0");

    try
    {
        Application->Initialize();
        Application->MainFormOnTaskBar = false;
        Application->CreateForm(__classid(TFormATS), &FormATS);
         Application->Run();
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    catch (...)
    {
        try
        {
            throw Exception("");
        }
        catch (Exception &exception)
        {
            Application->ShowException(&exception);
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
