//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "MsgDialog.h"
#include "msg.h"
#include "ClientInit.h"
#include "main.h"
#include "Resource.h"
#include "DlgSetStation.h"
#include "FrmStationCfg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxControls"
#pragma link "cxCustomData"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxGraphics"
#pragma link "cxInplaceContainer"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxStyles"
#pragma link "cxTextEdit"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "dxSkinBlack"
#pragma link "dxSkinBlue"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinCaramel"
#pragma link "dxSkinCoffee"
#pragma link "dxSkinDarkRoom"
#pragma link "dxSkinDarkSide"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinDevExpressStyle"
#pragma link "dxSkinFoggy"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinHighContrast"
#pragma link "dxSkiniMaginary"
#pragma link "dxSkinLilian"
#pragma link "dxSkinLiquidSky"
#pragma link "dxSkinLondonLiquidSky"
#pragma link "dxSkinMcSkin"
#pragma link "dxSkinMetropolis"
#pragma link "dxSkinMetropolisDark"
#pragma link "dxSkinMoneyTwins"
#pragma link "dxSkinOffice2007Black"
#pragma link "dxSkinOffice2007Blue"
#pragma link "dxSkinOffice2007Green"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinOffice2007Silver"
#pragma link "dxSkinOffice2010Black"
#pragma link "dxSkinOffice2010Blue"
#pragma link "dxSkinOffice2010Silver"
#pragma link "dxSkinOffice2013DarkGray"
#pragma link "dxSkinOffice2013LightGray"
#pragma link "dxSkinOffice2013White"
#pragma link "dxSkinOffice2016Colorful"
#pragma link "dxSkinOffice2016Dark"
#pragma link "dxSkinPumpkin"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSeven"
#pragma link "dxSkinSevenClassic"
#pragma link "dxSkinSharp"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinSilver"
#pragma link "dxSkinSpringTime"
#pragma link "dxSkinStardust"
#pragma link "dxSkinSummer2008"
#pragma link "dxSkinTheAsphaltWorld"
#pragma link "dxSkinTheBezier"
#pragma link "dxSkinValentine"
#pragma link "dxSkinVisualStudio2013Blue"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "dxSkinVisualStudio2013Light"
#pragma link "dxSkinVS2010"
#pragma link "dxSkinWhiteprint"
#pragma link "dxSkinXmas2008Blue"
#pragma link "cxContainer"
#pragma link "cxEdit"
#pragma link "cxGroupBox"
#pragma link "cxButtons"
#pragma resource "*.dfm"
TFormStationCfg *FormStationCfg;
//---------------------------------------------------------------------------

__fastcall TFormStationCfg::TFormStationCfg(TComponent* Owner)
    : TSubForm(Owner)
{
    InitView();
}
//---------------------------------------------------------------------------
ID_VALUE_T  s_astStationType[] = {  {STATION_TYPE_NORMAL,   "正线"},
                                    {STATION_TYPE_DEPOT,    "车辆段"},
                                    {STATION_TYPE_FIX,      "维修段"},
                                };
Dict g_DictStationType(s_astStationType, ARRAY_SIZE(s_astStationType));
//---------------------------------------------------------------------------
AnsiString GetStationTypeText(UINT32 ulStationType)
{
    for (UINT32 i=0; i<ARRAY_SIZE(s_astStationType); i++)
    {
        if (s_astStationType[i].ulID == ulStationType)
        {
            return s_astStationType[i].szValue;
        }
    }

    return "";
}
//---------------------------------------------------------------------------
void __fastcall TFormStationCfg::ColumnStationTypeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulStationType = ANode->Values[2];

    Value = GetStationTypeText(ulStationType);
}
//---------------------------------------------------------------------------
void __fastcall TFormStationCfg::btnAddClick(TObject *Sender)
{
    DialogSetStation->Init();
    if (DialogSetStation->ShowModal() != mrOk)
    {
        return;
    }

    STATION_INFO_T stInfo = {0};
    DialogSetStation->Get(stInfo);

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("StationID", stInfo.ulStationID);
    ReqJsonParam.Add("StationName", stInfo.acStationName);
    ReqJsonParam.Add("StationType", stInfo.ulStationType);

    if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_AddStationInfo, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormStationCfg::btnDelClick(TObject *Sender)
{
    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlMain->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }

    UINT32      ulID = Node->Values[0];
    AnsiString  strName = Node->Texts[1];

    if (!AskMsg((CHAR*)"删除车站%s吗？", strName.c_str()))
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("StationID", ulID);

    if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_DelStationInfo, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormStationCfg::btnRefreshClick(TObject *Sender)
{
//    static UINT32 ulLastTime = 0;
//    UINT32  ulCurrTime = gos_get_uptime_1ms();
//
//    if (ulLastTime == 0 ||(ulLastTime+1000) < ulCurrTime)
//    {
//        SendServer(EV_OMT_OMC_REQ, MSG_OMT_GetStationInfo);
//        ulLastTime = gos_get_uptime_1ms();
//    }
    if (!LoadStationInfo())
    {
        return;
    }

    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormStationCfg::FormCreate(TObject *Sender)
{
    SetImage(btnAdd, IMG_ADD);
    SetImage(btnDel, IMG_DEL);
    SetImage(btnSet, IMG_SET);
    SetImage(btnRefresh, IMG_REFRESH);
}
//---------------------------------------------------------------------------
void __fastcall TFormStationCfg::btnSetClick(TObject *Sender)
{
    VectorMap<UINT32, STATION_INFO_T>   mStationInfo;
    Station::GetStationInfo(mStationInfo);

    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlMain->Selections[0];

    if (Node->Values[1].IsNull())
    {
        return;
    }


    UINT32 ulStationID   =  (UINT32)Node->Values[0];

    STATION_INFO_T  *pstInfo = mStationInfo.GetValue(ulStationID);

    if (!pstInfo)
    {
        return;
    }

    DialogSetStation->Init(pstInfo);
    if (DialogSetStation->ShowModal() != mrOk)
    {
        return;
    }

    STATION_INFO_T stInfo = {0};

    DialogSetStation->Get(stInfo);

    if (strcmp(stInfo.acStationName, pstInfo->acStationName) == 0 &&
        stInfo.ulStationType == pstInfo->ulStationType &&
        stInfo.ulInterLockStationID == pstInfo->ulInterLockStationID )
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("StationID", stInfo.ulStationID);
    ReqJsonParam.Add("StationName", stInfo.acStationName);
    ReqJsonParam.Add("StationType", stInfo.ulStationType);

    if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_SetStationInfo, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormStationCfg::InitView()
{
    VectorMap<UINT32, STATION_INFO_T>   mStationInfo;
    Station::GetStationInfo(mStationInfo);
    TcxTreeListNode         *Node;

    tlMain->BeginUpdate();
    tlMain->Clear();
    for (UINT32 i=0; i<mStationInfo.Size(); i++)
    {
        STATION_INFO_T *pstInfo = mStationInfo.GetValueByIndex(i);

        Node = tlMain->Add();
        Node->Values[0] = pstInfo->ulStationID;
        Node->Values[1] = pstInfo->acStationName;
        Node->Values[2] = pstInfo->ulStationType;
    }

    tlMain->EndUpdate();
}
