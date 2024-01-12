#include <vcl.h>
#pragma hdrstop

#include "g_include.h"
#include "ds_public.h"
#include "MsgDialog.h"
#include "RemoteMsg.h"

#include "ClientUtil.h"
#include "eSDK.h"
#include "eLTE_SDK.h"
#include "msg.h"
#include "ats.h"
#include "dba_public.h"
#include "main.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxGraphics"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "dxSkinBlue"
#pragma link "dxSkinDarkRoom"
#pragma link "dxSkinDarkSide"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinLilian"
#pragma link "dxSkinMcSkin"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxDropDownEdit"
#pragma link "cxEdit"
#pragma link "cxMaskEdit"
#pragma link "cxTextEdit"
#pragma link "cxCustomData"
#pragma link "cxInplaceContainer"
#pragma link "cxStyles"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "dxSkinsForm"
#pragma link "cxPC"
#pragma link "dxSkinscxPCPainter"
#pragma link "cxGroupBox"
#pragma link "cxMemo"
#pragma link "cxLabel"
#pragma link "cxClasses"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "dxBarBuiltInMenu"
#pragma link "cxRadioGroup"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxCheckBox"
#pragma resource "*.dfm"

#define DEFAULT_DIS_PORT    GOS_DEFAULT_SERVER_PORT

UINT32      g_ulTrainUserID = 0;
UINT32      g_ulTrainUnitID = 0;
INT64       g_i64GroupID = INVALID_GROUP_ID;
UINT32      g_ulDCUserID = INVALID_USER_ID;
UINT32      g_ulMaxPeriod = 0;

TFormMain *FormMain;
RemoteMsg   *g_pRemoteMsg = NULL;

IP_ADDR_T   g_stDISAddr = {0};
BOOL        g_bTCPMode = FALSE;

/*
#define DC_TYPE_ALL             0xff    // 全功能调度台
#define DC_TYPE_DEFAULT         0x00    // 缺省调度台
#define DC_TYPE_TRAIN           0x01    // 行车调度台
#define DC_TYPE_PASSENGER       0x02    // 乘客调度台
#define DC_TYPE_DEPOT           0x04    // 车辆段调度台
#define DC_TYPE_FIX             0x08    // 维修段调度台
#define DC_TYPE_ENV             0x10    // 环控调度台
#define DC_TYPE_POWER           0x20    // 电力调度台
*/

ID_VALUE_T  s_astDCType[] = {   {DC_TYPE_DEFAULT,   "缺省调度台"},
                                {DC_TYPE_TRAIN,     "行车调度台"},
                        //      {DC_TYPE_PASSENGER, "乘客调度台"},
                                {DC_TYPE_DEPOT,     "车辆段调度台"}};

Dict        g_DictDCType(s_astDCType, ARRAY_SIZE(s_astDCType));

ID_VALUE_T  s_astSDSType[] = {
                        {SDS_TYPE_NULL,     ""},
                        {SDS_TYPE_FORMAL_DISPATCH,  "书面命令"},
                        {SDS_TYPE_ORAL_DISPATCH,    "口头命令"},
                        {SDS_TYPE_OPERATE,  "运营信息"},
                        {SDS_TYPE_NORMAL,   "普通信息"},
                                };

Dict    g_DictSDSType(s_astSDSType, ARRAY_SIZE(s_astSDSType));

CHAR* GetSDSTypeText(UINT32 ulType)
{
    return g_DictSDSType.GetValue(ulType);
}

UINT32      g_ulRegisteringDCType = -1;
UINT32      g_ulRegisteredDCType = -2;
AnsiString  g_strCurrGroupID = "0";

Dict    g_DictTrainPos;

UINT32 GetMsgSeqID()
{
    static UINT32   ulSeq = 0;

    return ++ulSeq;
}

BOOL SendMsg(UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &Param)
{
    GJsonParam      ReqParam;

    ReqParam.Add("MsgName", (CHAR*)szMsgName);
    ReqParam.Add("MsgSeqID", GetMsgSeqID());
    ReqParam.Add("MsgInfo", Param);

    CHAR    *szReq = ReqParam.GetString();

    return g_pRemoteMsg->SendReq(usMsgID, szReq, strlen(szReq)+1);
}

BOOL SendMsg(UINT16 usMsgID, const CHAR *szMsgName, GJsonTupleParam &Param)
{
    GJsonParam      ReqParam;

    ReqParam.Add("MsgName", (CHAR*)szMsgName);
    ReqParam.Add("MsgSeqID", GetMsgSeqID());
    ReqParam.Add("MsgInfo", Param);

    CHAR    *szReq = ReqParam.GetString();

    return g_pRemoteMsg->SendReq(usMsgID, szReq, strlen(szReq)+1);
}

BOOL GetLocalCfg()
{
    CHAR    acFile[256];
    CHAR    *szValue;
    INT32   iValue;

    sprintf(acFile, "%s/%s", gos_get_root_path(), "app.ini");

    if (!gos_file_exist(acFile))
    {
        ErrMsg("本地配置文件[%s]不存在!", acFile);
        return FALSE;
    }

    GConfFile   Conf(acFile);

    szValue = Conf.Get(CFG_DIS_IP);
    if (!gos_get_ip_port(szValue, g_stDISAddr.aucIP, &g_stDISAddr.usPort))
    {
        GosLog(LOG_ERROR, "get %s failed", CFG_DIS_IP);
        return FALSE;
    }

    if (g_stDISAddr.usPort == 0)
    {
        g_stDISAddr.usPort = DEFAULT_DIS_PORT;
    }

    szValue = Conf.Get(CFG_TRAIN_UNIT_ID);
    if (!gos_atou(szValue, &g_ulTrainUnitID))
    {
        GosLog(LOG_ERROR, "get %s failed", CFG_TRAIN_UNIT_ID);
        return FALSE;
    }

    szValue = Conf.Get("user_id");
    gos_atou(szValue, &g_ulTrainUserID);

    g_bTCPMode = FALSE;
    szValue = Conf.Get("tcp_mode");
    if (szValue && strcasecmp(szValue, "true") == 0)
    {
        g_bTCPMode = TRUE;
    }

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
    g_pRemoteMsg = RemoteMsg::New(g_stDISAddr.aucIP, g_stDISAddr.usPort, g_bTCPMode);
    if (!g_pRemoteMsg)
    {
        GosLog(LOG_FATAL, "Get Remote Msg failed!");

        return FALSE;
    }

    //g_pRemoteMsg->SetServer(g_stPortCfg.stConnectAddr.aucIP, g_stPortCfg.stConnectAddr.usPort);

    g_pRemoteMsg->SetClientType(DIS_CLIENT_APP);
//  g_pRemoteMsg->SetClientConnectionChange(ClientConnectionChange);

    for (int i=0; i<100; i++)
    {
        if (g_pRemoteMsg->IsConnected())
        {
            break;
        }

        gos_sleep_1ms(300);
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
 /* DateSeparator = '-';
    ShortDateFormat = "yyyy-MM-dd";
    ShortTimeFormat = "hh:mm:ss";
    LongTimeFormat = "HH:mm:ss";
   */

    GosInitLog();
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
    pLogger->SetLogToFile(TRUE, 4, 1024*1024, 0);
    pLogger->SetLogLevel(LOG_INFO);
    pLogger->SetLogModule(MODULE_DTP, MODULE_DTP_NAME);

    return TRUE;
}

BOOL SystemInit()
{
    if (!GetLocalCfg() ||
        !InitClientEnv() ||
        !InitRemoteMsg() )
    {
        return FALSE;
    }

    return TRUE;
}

Dict        g_DictDest;

//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TSubForm(Owner)
{
/*    g_DictDest.Add(1, "航头站");
    g_DictDest.Add(26, "长江南路站");
    g_DictDest.Init(cbDest);
  */

    edtTrainUnitID->Text = g_ulTrainUnitID;
    edtUser->Text = g_ulTrainUserID;

    cbLocalRTSP->Checked = false;
    cbLocalRTSP->Checked = true;

    InnerMsgReady();

    pcMain->ActivePage = tsDISCfg;

    g_DictDCType.Init(cbDCType);
    g_DictDCType.SetID(cbDCType, DC_TYPE_DEFAULT);

    InitMsgHandler(EV_APP_DIS_RSP, MSG_APP_SetTrainGroupReq, OnDCSetTrainGroupReq);
//  InitMsgHandler(EV_APP_DIS_RSP, MSG_APP_SetTrainIDReq, OnDCSetTrainIDReq);

    InitMsgHandler(EV_APP_DIS_RSP, MSG_APP_GetDSCfgInfoRsp, OnGetDSCfgInfoRsp);
    InitMsgHandler(EV_APP_DIS_RSP, MSG_APP_BroadcastStartReq, OnBroadcastStartReq);
    InitMsgHandler(EV_APP_DIS_RSP, MSG_APP_BroadcastEndReq, OnBroadcastEndReq);

    InitMsgHandler(EV_APP_DIS_RSP, MSG_APP_RequestCallRsp, OnRequestCallRsp);
    InitMsgHandler(EV_APP_DIS_RSP, MSG_APP_GroupCallEndReq, OnGroupCallEndReq);

    InitMsgHandler(EV_APP_DIS_RSP, MSG_APP_GetATSInfoRsp, OnGetATSInfoRsp);
    InitMsgHandler(EV_APP_DIS_RSP, MSG_APP_RegisterRsp, OnRegisterRsp);

    InitMsgHandler(EV_DC_DIS_RSP, MSG_DCGetLatestSDSInfo, OnGetLatestSDSInfo);
    InitMsgHandler(EV_APP_DIS_RSP, MSG_APP_SDSReplyRsp, OnSDSReplyRsp);
    InitMsgHandler(EV_APP_DIS_RSP, MSG_APP_AutoSDSReplyReq, OnAutoSDSReplyReq);


}
//---------------------------------------------------------------------------
void __fastcall TFormMain::GetDISCfg()
{
    GJsonParam  Param;

    g_ulTrainUserID = edtUser->Text.ToIntDef(0);

    Param.Add("UserID", g_ulTrainUserID);

    SendMsg(EV_APP_DIS_REQ, MSG_APP_GetDSCfgInfoReq, Param);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerCfgTimer(TObject *Sender)
{
    GetDISCfg();
}
//---------------------------------------------------------------------------
void TFormMain::Log(const char *szFormat, ...)
{
    char        acLog[4096];
    va_list     vaLog;

    va_start(vaLog, szFormat);

    vsnprintf(acLog, sizeof(acLog)-1, szFormat, vaLog);
    acLog[sizeof(acLog)-1] = '\0';

    va_end(vaLog);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::LogMsg(AnsiString strMsgName, AnsiString strMsgInfo)
{
    UINT32      ulTime = gos_get_current_time();
    String      strMsg = Utf8ToAnsi(strMsgInfo);

    mmMsg->Lines->Append("");
    mmMsg->Lines->Append(GetTextTime(ulTime));
    mmMsg->Lines->Append("== DIS Msg:  " + strMsgName);
    mmMsg->Lines->Append("   Msg Info: " + strMsg);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::Log(AnsiString strLog)
{
    mmMsg->Lines->Append(strLog);
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::OnGetDSCfgInfoRsp(UINT32 ulSeqID, GJson &Json)
{
    AnsiString  strText = Json.GetText();
    CHAR        *szText = Json.GetValue("TrainPos");
    UINT32      ulTrainPosID;
    CHAR        *szTrainPosName;
    GJson       SubJson;

    mmCfg->Text = Utf8ToAnsi(strText.c_str());

    strText = Utf8ToAnsi(szText);
    SubJson.Parse(strText.c_str());

    std::vector<GJson*> &JsonTrain = SubJson.GetSubJson();

    g_DictTrainPos.Clear();
    for (UINT32 i=0; i<JsonTrain.size(); i++)
    {
        JsonTrain[i]->GetValue("TrainPosID", &ulTrainPosID);
        szTrainPosName = JsonTrain[i]->GetValue("TrainPosName");

        g_DictTrainPos.Add(ulTrainPosID, szTrainPosName);
    }

    g_DictTrainPos.Init(cbTrainPos);

//  Log(Json.GetText());
    /*
    UINT32  ulGroupStatus;
    UINT32  ulDCUserID;
    UINT32  ulTrainUserID;
    INT64   i64GroupID;
    UINT32  ulResult;

    if (!Json.GetValue("GroupID", &i64GroupID))
    {
        GosLog(LOG_ERROR, "invalid GroupID in msg %s", MSG_DCBroadcastStart);
        return;
    }
    */
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnBroadcastStartReq(UINT32 ulSeqID, GJson &Json)
{
    LogMsg(MSG_APP_BroadcastStartReq, Json.GetText());

    if (!Json.GetValue("GroupID", &g_i64GroupID))
    {
        Log("invalid GroupID in msg %s", MSG_APP_BroadcastStartReq);
        return;
    }

    if (!Json.GetValue("DCUserID", &g_ulDCUserID))
    {
        Log("invalid DCUserID in msg %s", MSG_APP_BroadcastStartReq);
        return;
    }

    if (!Json.GetValue("MaxPeriod", &g_ulMaxPeriod))
    {
        Log("invalid MaxPeriod in msg %s", MSG_APP_BroadcastStartReq);
        return;
    }

/*  GJsonParam  Param;
    UINT32      ulResult = 1;

    Param.Add("GroupID", g_i64GroupID);
    Param.Add("DCUserID", g_ulDCUserID);
    Param.Add("TrainUserID", g_ulTrainUserID);
    Param.Add("Result", ulResult);

    SendMsg(EV_APP_DIS_REQ, MSG_APP_DCBroadcastStartRsp, Param);  */
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::OnBroadcastEndReq(UINT32 ulSeqID, GJson &Json)
{
    LogMsg(MSG_APP_BroadcastEndReq, Json.GetText());
}

void __fastcall TFormMain::btnStartBrdClick(TObject *Sender)
{
    GJsonParam  Param;
    int         iResult = 1;

    g_ulTrainUserID = edtUser->Text.ToIntDef(0);

    Param.Add("GroupID", g_i64GroupID);
    Param.Add("DCUserID", g_ulDCUserID);
    Param.Add("TrainUserID", g_ulTrainUserID);
    Param.Add("Result", iResult);

    SendMsg(EV_APP_DIS_REQ, MSG_APP_BroadcastStartRsp, Param);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnRegisterClick(TObject *Sender)
{
    UINT32      ulDCType = 0;

//  g_DictDCType.GetID(&ulDCType);

    Register(ulDCType);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnUnregisterClick(TObject *Sender)
{
    GJsonParam  Param;
    UINT32      ulTrainUnitID = edtTrainUnitID->Text.ToIntDef(0);

    Param.Add("TrainUnitID", ulTrainUnitID);

    SendMsg(EV_APP_DIS_REQ, MSG_APP_UnregisterReq, Param);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnReqCallClick(TObject *Sender)
{
//
    GJsonParam  Param;
    UINT32      ulCallType = 0;

    if (rbCallStation->Checked)
    {
        ulCallType = 1;
    }

    g_ulTrainUserID = edtUser->Text.ToIntDef(0);

    Param.Add("UserID", g_ulTrainUserID);
    Param.Add("CallType", ulCallType);

    SendMsg(EV_APP_DIS_REQ, MSG_APP_RequestCallReq, Param);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnRequestCallRsp(UINT32 ulSeqID, GJson &Json)
{
    LogMsg(MSG_APP_RequestCallRsp, Json.GetText());
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnGroupCallEndReq(UINT32 ulSeqID, GJson &Json)
{
    LogMsg(MSG_APP_GroupCallEndReq, Json.GetText());
}

static VOID __stdcall ELTE_EventCallBack(INT32 iEventType, VOID* pEventBuf,
                                    UINT32 uiBufSize, VOID* pUserData)
{
}


BOOL _eSDK_Login(CHAR* szUserID, CHAR* szPwd, UINT8* pucServerIP, UINT16 usServerSIPPort, UINT8* pucLocalIP)
{
    CHAR    acServerIP[32];
    CHAR    acLocalIP[32];
    INT32   iCurrDCUserIndex = -1;
    UINT32  ulCurrDCUserID;
    AnsiString  strUserID = szUserID;

    if (!gos_atou(szUserID, &ulCurrDCUserID))
    {
        GosLog(LOG_INFO, "invalid dc user format: %s", szUserID);
        return FALSE;
    }

    sprintf(acServerIP, IP_FMT, IP_ARG(pucServerIP));
    sprintf(acLocalIP, IP_FMT, IP_ARG(pucLocalIP));

    GosLog(LOG_INFO, "login: %s %s %s %s %u", szUserID, szPwd, acServerIP, acLocalIP, usServerSIPPort);

    INT32   iRet = ELTE_SDK_Login(szUserID, szPwd, acServerIP, acLocalIP, usServerSIPPort);

    /*
    if (iRet == 0)
    {
        g_bMDCLogined = TRUE;
        strcpy(g_acCurrDCUserID, szUserID);
        gos_atou(szUserID, &g_ulCurrDCUserID);
    } */

    return (iRet == 0);
}

//---------------------------------------------------------------------------
bool ESDKLogin(CHAR *szUser, CHAR *szPwd)
{
    BOOL    bRet = eSDK_Init(ELTE_EventCallBack);
    UINT8   aucMDCAddr[4] = {172,22,1,21};
    UINT16  usMDCPort = 5060;
    UINT8   aucLocalIP[4];

    if (!bRet)
    {
        return false;
    }

    if (!GetLocalIP(aucMDCAddr, aucLocalIP))
    {
        return false;
    }

    eSDK_TriggerStatusReport(TRUE);

    bRet = eSDK_Login(szUser, szPwd, aucMDCAddr, usMDCPort, aucLocalIP);
    if (!bRet)
    {
        return false;
    }

    return true;
}


void __fastcall TFormMain::btnLoginClick(TObject *Sender)
{
    char    acUser[32];

    StrCpy(acUser, edtDCUser->Text);

    if (!ESDKLogin(acUser, acUser))
    {
        InfoMsg("登录失败");
    }
    else
    {
        InfoMsg("登录成功");
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerATSTimer(TObject *Sender)
{
    mmMsg->Style->Font->Name = "宋体";

    if (edtTrainUnitID->Text.IsEmpty() ||
        edtTrainUnitID->Text.ToIntDef(-1) <= 0  )
    {
        return;
    }

    GJsonParam  Param;

    Param.Add("TrainUnitID", edtTrainUnitID->Text);

    SendMsg(EV_APP_DIS_REQ, MSG_APP_GetATSInfoReq, Param);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ReportTrainInfo()
{
    GJsonParam  Param;

    Param.Add("UserID", edtUser->Text );
    Param.Add("TrainUnitID", edtTrainUnitID->Text);
    Param.Add("TrainID", edtTrainID->Text);
    Param.Add("CurrentGroupID", edtCurrGroupID->Text);

    /*
    if (edtRegisteredDCType->Text == "1")
    {
        Param.Add("PositionID", 1);  //     1：行车段 2：维修段  3：停车场
    }
    else
    {
        Param.Add("PositionID", 3);  //     1：行车段 2：维修段  3：停车场
    } */

    Param.Add("Direction", edtDirection->Text);  //   上下行方向    1：上行 2：下行 0：未知
    Param.Add("StationID", edtStationID->Text);
    Param.Add("CellID", edtCellID->Text);
    Param.Add("CallType", edtCallType->Text);
    Param.Add("CallID", edtCallID->Text);
    Param.Add("CurrentTime", gos_get_current_time());
    Param.Add("Uptime", gos_get_uptime());
    Param.Add("SerialNumber", "12345");
    Param.Add("APPVer", "V2.2.3a");

    SendMsg(EV_APP_DIS_REQ, MSG_APP_ReportTrainInfo, Param);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::Register(UINT32 ulDCType)
{
    GJsonParam  Param;
    UINT32      ulTrainUnitID = edtTrainUnitID->Text.ToIntDef(0);

    Param.Add("TrainUnitID", ulTrainUnitID);
    Param.Add("DCType", ulDCType);

    SendMsg(EV_APP_DIS_REQ, MSG_APP_RegisterReq, Param);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnGetATSInfoRsp(UINT32 ulSeqID, GJson &Json)
{
    LogMsg(MSG_APP_GetATSInfoRsp, Json.GetText());

    return;

    edtTrainID->Text = Json.GetValue("TrainID");
    g_strCurrGroupID = Json.GetValue("DefaultGroupID");
    edtStationID->Text = Json.GetValue("StationID");
    edtDirection->Text = Json.GetValue("Direction");

    AnsiString  strValue = Json.GetValue("PositionID");

    if (strValue == "1")    // 正线
    {
        g_ulRegisteringDCType = DC_TYPE_TRAIN;
    }
    else
    {
        g_ulRegisteringDCType = DC_TYPE_DEPOT;
    }

    if (g_ulRegisteringDCType != g_ulRegisteredDCType)
    {
        Log("Auto registering ...");
   //       Register(0);
    }

//  edtRegisteredDCType->Text = g_ulRegisteringDCType;
    ReportTrainInfo();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnRegisterRsp(UINT32 ulSeqID, GJson &Json)
{
    AnsiString  strValue = Json.GetValue("Result");

    if (strValue != "1")
    {
        return;
    }

    strValue = Json.GetValue("DCType");

    g_ulRegisteredDCType = strValue.ToIntDef(-3);
    edtRegisteredDCType->Text = g_ulRegisteredDCType;

    Log("change group from " + edtCurrGroupID->Text + " to " + g_strCurrGroupID);
    edtCurrGroupID->Text = g_strCurrGroupID;
}
void __fastcall TFormMain::btnGetSDSInfoClick(TObject *Sender)
{
    //
    GJsonParam  Param;

    SendMsg(EV_DC_DIS_REQ, MSG_DCGetLatestSDSInfo, Param);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnGetLatestSDSInfo(UINT32 ulSeqID, GJson &Json)
{
    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = Json.GetSubJson();
    SDS_INFO_T      stInfo;
//  VECTOR<SDS_INFO_T>  vInfo;

    tlSDS->BeginUpdate();
    tlSDS->Clear();
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
            !pSubJson->GetValue("NeedReply", &stInfo.bNeedReply) )
        {
            GosLog(LOG_ERROR, "parse sds info failed on %s", pSubJson->GetText());
            return;
        }

        TcxTreeListNode *Node = tlSDS->Add();

        Node->Values[0] = stInfo.ulSDSID;
        Node->Values[1] = GetTextTime(stInfo.ulSendTime);
        Node->Values[2] = GetSDSTypeText(stInfo.ulSDSType);
        Node->Values[3] = "";
        Node->Values[4] = stInfo.ulSDSIndex;
        if (stInfo.acSendUserName[0])
        {
            Node->Values[5] = stInfo.acSendUserName;
        }
        else
        {
            Node->Values[5] = stInfo.ulSendUserID;
        }
        Node->Values[7] = stInfo.acSDSInfo;
        Node->Values[6] = "";

    }

    tlSDS->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnSDSReplyRsp(UINT32 ulSeqID, GJson &Json)
{
    UINT32              ulSDSID;
    GJsonParam          Param;

    if (!Json.GetValue("SDSID", &ulSDSID))
    {
        GosLog(LOG_ERROR, "invalid SDSID in msg %s", MSG_APP_SDSReplyRsp);
        return;
    }

    for (UINT32 i=0; i<tlSDS->AbsoluteCount; i++)
    {
        TcxTreeListNode     *Node = tlSDS->AbsoluteItems[i];

        if ((UINT32)Node->Values[0] == ulSDSID)
        {
            Node->Values[6] = "收到回执应答";
            break;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnAutoSDSReplyReq(UINT32 ulSeqID, GJson &Json)
{
    UINT32              ulUserID;
    UINT32              ulSDSID;
    GJsonParam          Param;

    if (!Json.GetValue("SDSID", &ulSDSID))
    {
        GosLog(LOG_ERROR, "invalid SDSID in msg %s", MSG_APP_AutoSDSReplyReq);
        return;
    }

    for (UINT32 i=0; i<tlSDS->AbsoluteCount; i++)
    {
        TcxTreeListNode     *Node = tlSDS->AbsoluteItems[i];

        if ((UINT32)Node->Values[0] == ulSDSID)
        {
            Node->Values[6] = "收到自动回执请求";
            break;
        }
    }

}

void __fastcall TFormMain::OnDCSetTrainGroupReq(UINT32 ulSeqID, GJson &Json)
{
    UINT32              ulTrainPos;

    if (!Json.GetValue("TrainPos", &ulTrainPos))
    {
        GosLog(LOG_ERROR, "invalid TrainPos in msg %s", MSG_APP_SetTrainGroupReq);
        return;
    }

    GJsonParam  Param;
    UINT32      ulTrainUnitID = edtTrainUnitID->Text.ToIntDef(0);

    Param.Add("TrainUnitID", ulTrainUnitID);
    Param.Add("TrainPos", ulTrainPos);

    SendMsg(EV_APP_DIS_REQ, MSG_APP_SetTrainGroupReq, Param);
}

void __fastcall TFormMain::OnDCSetTrainIDReq(UINT32 ulSeqID, GJson &Json)
{
    UINT32              ulTrainID;
    UINT32              ulDestID;
    UINT32              ulDirection;

    if (!Json.GetValue("TrainID", &ulTrainID))
    {
        GosLog(LOG_ERROR, "invalid TrainID in msg %s", MSG_APP_SetTrainIDReq);
        return;
    }

    if (!Json.GetValue("DestID", &ulDestID))
    {
        GosLog(LOG_ERROR, "invalid DestID in msg %s", MSG_APP_SetTrainIDReq);
        return;
    }

    if (!Json.GetValue("Direction", &ulDirection))
    {
        GosLog(LOG_ERROR, "invalid Direction in msg %s", MSG_APP_SetTrainIDReq);
        return;
    }

    edtTrainID->Text = ulTrainID;
    edtNewTrainID->Text = ulTrainID;
//  g_DictDest.SetID(ulDestID);
    edtNewDestID->Text = ulDestID;
    if (ulDirection = TRAIN_DIRECTION_UPSIDE)
    {
        rbUp->Checked = true;
    }
    else
    {
        rbDown->Checked = true;
    }

    GJsonParam  Param;
    UINT32      ulTrainUnitID = edtTrainUnitID->Text.ToIntDef(0);

    Param.Add("TrainUnitID", ulTrainUnitID);
    Param.Add("TrainID", ulTrainID);
    Param.Add("DestID", ulDestID);
    Param.Add("Direction", ulDirection);

    SendMsg(EV_APP_DIS_REQ, MSG_APP_SetTrainIDReq, Param);
}

void __fastcall TFormMain::btnSendSDSReplyClick(TObject *Sender)
{
    UINT32          ulSDSID;
    GJsonParam      Param;
    UINT32          ulAutoReply = 0;

    if (tlSDS->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tlSDS->Selections[0];
    String              strTmp;

    ulSDSID = Node->Values[0];
    strTmp = Node->Values[6];
    if (strTmp == "收到自动回执通知")
    {
        ulAutoReply = 1;
    }

    g_ulTrainUserID = edtUser->Text.ToIntDef(0);

    Param.Add("UserID", g_ulTrainUserID);
    Param.Add("SDSID", ulSDSID);
    Param.Add("AutoReply", ulAutoReply);

    if (ulAutoReply == 1)
    {
        SendMsg(EV_APP_DIS_REQ, MSG_APP_AutoSDSReplyRsp, Param);
    }
    else
    {
        SendMsg(EV_APP_DIS_REQ, MSG_APP_SDSReplyReq, Param);
    }

}
//---------------------------------------------------------------------------
void __fastcall TFormMain::cxButton1Click(TObject *Sender)
{
    GetDISCfg();
}

SOCKET g_stUDPSocket = INVALID_SOCKET;

BOOL InitUDPSocket()
{
    if (g_stUDPSocket == INVALID_SOCKET)
    {
        SOCKADDR_IN         stLocalAddr = {0};
        UINT32              ulFlag = 1;

        memset(&stLocalAddr, 0, sizeof(stLocalAddr));
        stLocalAddr.sin_family = PF_INET;
        stLocalAddr.sin_port   = HTONS(DEFAULT_APP_UDP_PORT);

        gos_init_socket();
        g_stUDPSocket = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (g_stUDPSocket == INVALID_SOCKET)
        {
            GosLog(LOG_ERROR, "create udp socket failed");
            return FALSE;
        }

        if (setsockopt(g_stUDPSocket, SOL_SOCKET, SO_REUSEADDR, (CHAR*)&ulFlag, sizeof(ulFlag)) == SOCKET_ERROR)
        {
            GosLog(LOG_ERROR, "server_create_socket: setsockopt failed, %s", gos_get_socket_err());
            CLOSE_SOCKET (g_stUDPSocket);

            return INVALID_SOCKET;
        }

        if (bind (g_stUDPSocket, (SOCKADDR *) &stLocalAddr, sizeof (SOCKADDR)) == SOCKET_ERROR)
        {
            GosLog(LOG_ERROR, "server_create_socket: bind socket failed, %s", gos_get_socket_err());
            CLOSE_SOCKET (g_stUDPSocket);

            return INVALID_SOCKET;
        }

        GosLog(LOG_FATAL, "create udp socket succ");
    }

    return TRUE;
}

IP_ADDR_T   g_stDBAAddr = {0};

BOOL SendDBAMsg(const CHAR *szMsgName, GJsonParam &MsgInfoParam)
{
    GJsonParam      Param;
    CHAR            *szSendMsg = NULL;
    UINT32          ulLen;
    UINT32          ulSeqID = gos_get_uptime();
    SOCKADDR_IN     stDBAAddr = {0};

    if (!InitUDPSocket())
    {
        InfoMsg("创建socket失败");
        return FALSE;
    }

    Param.Add("MsgName", szMsgName);
    Param.Add("MsgSeqID", ulSeqID);

   /*   CHAR *szMsgInfo = MsgInfoParam.GetString();

    if (strcmp(szMsgInfo, "{}") == 0)
    {
        Param.Add("MsgInfo", "");
    }
    else   */
    {
        Param.Add("MsgInfo", MsgInfoParam);
    }

    szSendMsg = Param.GetString();
    ulLen = strlen(szSendMsg);

    stDBAAddr.sin_family = PF_INET;
    stDBAAddr.sin_port   = HTONS(g_stDBAAddr.usPort);
    memcpy(&stDBAAddr.sin_addr.s_addr, g_stDBAAddr.aucIP, 4);

    if (!gos_udp_send(g_stUDPSocket, (void *)szSendMsg, ulLen, &stDBAAddr))
    {
        GosLog(LOG_ERROR, "send msg(%s) to APP failed: %s", szMsgName, gos_get_socket_err());
        return FALSE;
    }

    return TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ShowText(GJson &Json, const CHAR *szKey)
{
    CHAR    *szValue = Json.GetValue(szKey);
    String  strValue = szKey;

    strValue += " = ";
    strValue += szValue?szValue:"";

    mmDBAInfo->Lines->Append(strValue);
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::btnGetMasterStatusClick(TObject *Sender)
{
    btnGetMasterStatus->Enabled = false;

    GetMasterStatus();

    btnGetMasterStatus->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::GetMasterStatus()
{
    GJsonParam      Param;
    UINT32          usPort = DEFAULT_DBA_UDP_PORT;
    CHAR            acText[128];
    static CHAR     acRecvBuf[64*1024];

    StrCpy(acText, edtDBAAddr->Text);
    if (!gos_get_ip(acText, g_stDBAAddr.aucIP))
    {
        InfoMsg("地址错误");
        return;
    }

    g_stDBAAddr.usPort = DEFAULT_DBA_UDP_PORT;

    if (!SendDBAMsg(MSG_GetMasterStatusReq, Param))
    {
        return;
    }

    // select
    FD_SET  fds;
    FD_SET  fdsAll;
    TIMEVAL stTimeout = {0};
    INT32   iRet;
    INT32   iTimeout;
    INT32   iRecvLen;
    UINT32  ulStartTime = gos_get_uptime_1ms();
    UINT32  ulTimePassed;
    GJson   Json;
    GJson   SubJson;
    CHAR    *szValue;
    UINT32  ulTime = 0;
    UINT32  ulTimeout = 5*1000;
    CHAR    *szMsgName;
    CHAR    *szMsgInfo;
    FD_ZERO(&fdsAll);
    FD_SET(g_stUDPSocket, &fdsAll);

again:
    ulTimePassed = gos_get_uptime_1ms() - ulStartTime;
    iTimeout = ulTimeout - ulTimePassed;

    // 超时
    if (iTimeout <= 0)
    {
        return;
    }

    stTimeout.tv_sec = iTimeout/1000;
    stTimeout.tv_usec = 1000*MOD(iTimeout,1000);
    fds = fdsAll;

    iRet = select(g_stUDPSocket + 1, &fds, NULL, NULL, &stTimeout);
    if (iRet == 0)
    {
        goto again;
    }

    if (iRet < 0)
    {
        if (errno == EINTR)
        {
            goto again;
        }

        return;
    }

    if (!FD_ISSET(g_stUDPSocket, &fds))
    {
        goto again;
    }

    // recv
    memset(acRecvBuf, 0, sizeof(acRecvBuf));
    iRecvLen = recv(g_stUDPSocket, (CHAR*)acRecvBuf, sizeof(acRecvBuf)-1, 0);
    if (iRecvLen < 0)
    {
        InfoMsg("接收失败");
        return;
    }
    else if (iRecvLen == 0)
    {
        InfoMsg("接收失败");
        return;
    }

    Json.Parse(acRecvBuf);

    szMsgName = Json.GetValue("MsgName");
    if (!szMsgName)
    {
        goto again;
    }

    szMsgInfo = Json.GetValue("MsgInfo");
    if (!szMsgInfo)
    {
        goto again;
    }

    if (strcmp(szMsgName, MSG_GetMasterStatusRsp) != 0)
    {
        goto again;
    }

    if (!SubJson.Parse(szMsgInfo))
    {
        goto again;
    }

    mmDBAInfo->Text = "";
    ShowText(SubJson, "HwVersion");
    ShowText(SubJson, "FwVersion");
    ShowText(SubJson, "SN");
    ShowText(SubJson, "MAC");
    ShowText(SubJson, "IP");
    ShowText(SubJson, "NetMask");
    ShowText(SubJson, "Gateway");
    ShowText(SubJson, "CoverStatus");
    ShowText(SubJson, "DriveMode");

    if (!SubJson.GetValue("Time", &ulTime))
    {
        mmDBAInfo->Lines->Append("Time = ");
    }
    else
    {
        mmDBAInfo->Lines->Append(String("Time = ") + GetTextTime(ulTime));
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::cxButton3Click(TObject *Sender)
{
    UINT32      ulTrainPos = 0xff;

    if (!g_DictTrainPos.GetID(&ulTrainPos))
    {
        return;
    }


    GJsonParam  Param;
    UINT32      ulTrainUnitID = edtTrainUnitID->Text.ToIntDef(0);

    Param.Add("TrainUnitID", ulTrainUnitID);
    Param.Add("TrainPos", ulTrainPos);

    SendMsg(EV_APP_DIS_REQ, MSG_APP_SetTrainGroupReq, Param);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnSetTrainIDClick(TObject *Sender)
{
    UINT32              ulTrainID = edtNewTrainID->Text.ToIntDef(0);
    UINT32              ulDestID = edtNewDestID->Text.ToIntDef(0);
    UINT32              ulDirection;

    if (rbUp->Checked)
    {
        ulDirection = TRAIN_DIRECTION_UPSIDE;
    }
    else
    {
        ulDirection = TRAIN_DIRECTION_DOWNSIDE;
    }

    GJsonParam  Param;
    UINT32      ulTrainUnitID = edtTrainUnitID->Text.ToIntDef(0);

    Param.Add("TrainUnitID", ulTrainUnitID);
    Param.Add("TrainID", ulTrainID);
    Param.Add("DestID", ulDestID);
    Param.Add("Direction", ulDirection);

    SendMsg(EV_APP_DIS_REQ, MSG_APP_SetTrainIDReq, Param);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnIPHApplyClick(TObject *Sender)
{
    GJsonTupleParam Param;
    GJsonParam      SubParam;
    CHAR            acLocalAddr[32];
    UINT8           aucLocalAddr[4];

    g_pRemoteMsg->GetClientAddr(aucLocalAddr);
    sprintf(acLocalAddr, IP_FMT, IP_ARG(aucLocalAddr));

    SubParam.Add("TrainUnitID", edtTrainUnitID1->Text.ToIntDef(0));
    SubParam.Add("SAID", acLocalAddr);
    SubParam.Add("CarriageID", edtCarriageID1->Text.ToIntDef(0));
    SubParam.Add("IPHDevID", edtIPHDevID1->Text.ToIntDef(0));
    SubParam.Add("IPHDevIP", acLocalAddr);
    SubParam.Add("VideoURL", edtIPHUrl1->Text);

    Param.Add(SubParam);

    SubParam.Clear();
    SubParam.Add("TrainUnitID", edtTrainUnitID2->Text.ToIntDef(0));
    SubParam.Add("SAID", acLocalAddr);
    SubParam.Add("CarriageID", edtCarriageID2->Text.ToIntDef(0));
    SubParam.Add("IPHDevID", edtIPHDevID2->Text.ToIntDef(0));
    SubParam.Add("IPHDevIP", acLocalAddr);
    SubParam.Add("VideoURL", edtIPHUrl2->Text);

    Param.Add(SubParam);

    SendMsg(EV_SA_DIS_REQ, MSG_SAApplyIPHTalk, Param);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::cbLocalRTSPPropertiesChange(TObject *Sender)
{
    if (true)
    {
        edtIPHUrl1->Text = "rtsp://192.168.30.133:554/h264/ch1/main/av_stream";
        edtIPHUrl2->Text = edtIPHUrl1->Text;
    }
    else
    {
        edtIPHUrl1->Text = "http://ivi.bupt.edu.cn/hls/cctv1.m3u8";
        edtIPHUrl2->Text = "http://ivi.bupt.edu.cn/hls/cctv3.m3u8";
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::cbBatchIPHPropertiesChange(TObject *Sender)
{
    TimerIPH->Enabled = cbBatchIPH->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerIPHTimer(TObject *Sender)
{
    GJsonTupleParam Param;
    GJsonParam      SubParam;
    CHAR            acLocalAddr[32];
    UINT8           aucLocalAddr[4];
    static bool     bFirst = true;

    bFirst = !bFirst;

    g_pRemoteMsg->GetClientAddr(aucLocalAddr);
    sprintf(acLocalAddr, IP_FMT, IP_ARG(aucLocalAddr));
    if (bFirst)
    {
        sprintf(acLocalAddr, "127.0.0.1");
    }

    for (UINT32 i=0; i<25; i++)
    {
        SubParam.Clear();

        if (bFirst)
        {
            SubParam.Add("TrainUnitID", edtTrainUnitID1->Text.ToIntDef(0));
        }
        else
        {
            SubParam.Add("TrainUnitID", edtTrainUnitID2->Text.ToIntDef(0));
        }

        SubParam.Add("SAID", acLocalAddr);
        SubParam.Add("CarriageID", 1+i/5);
        SubParam.Add("IPHDevID", 1+MOD(i,5));
        SubParam.Add("IPHDevIP", acLocalAddr);
        SubParam.Add("VideoURL", edtIPHUrl1->Text);

        Param.Add(SubParam);
    }

    SendMsg(EV_SA_DIS_REQ, MSG_SAApplyIPHTalk, Param);

    /*
    Param.Clear();

    for (UINT32 i=0; i<40; i++)
    {
        SubParam.Clear();
        SubParam.Add("TrainUnitID", edtTrainUnitID2->Text.ToIntDef(0));
        SubParam.Add("SAID", acLocalAddr);
        SubParam.Add("CarriageID", 1+i/8);
        SubParam.Add("IPHDevID", 1+MOD(i,8));
        SubParam.Add("IPHDevIP", acLocalAddr);
        SubParam.Add("VideoURL", edtIPHUrl2->Text);

        Param.Add(SubParam);
    }

    SendMsg(EV_SA_DIS_REQ, MSG_SAApplyIPHTalk, Param);   */
}
//---------------------------------------------------------------------------

