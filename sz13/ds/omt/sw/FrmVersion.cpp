//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MDIForm.h"
#include "ClientInclude.h"
#include "MsgDialog.h"
#include "msg.h"
#include "ds_public.h"
#include "ClientUtil.h"
#include "Resource.h"
#include "ClientCommon.h"
#include "RemoteMsg.h"
#include "ClientInit.h"

#include "DlgSetSw.h"
#include "FrmSwUpdate.h"
#include "FrmVersion.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxCustomData"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxGroupBox"
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
#pragma link "AdvSmoothButton"
#pragma link "MDIForm"
#pragma resource "*.dfm"
TFormVersion *FormVersion;

extern VECTOR<OMC_SW_INFO_T>    g_vSwInfo;
extern AnsiString GetSwTypeText(UINT32 ulSwType);
extern BOOL LoadSwInfo();
//---------------------------------------------------------------------------
__fastcall TFormVersion::TFormVersion(TComponent* Owner)
    : TSubForm(Owner)
{
    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormVersion::FormCreate(TObject *Sender)
{
    SetImage(btnAdd, IMG_ADD);
    SetImage(btnDel, IMG_DEL);
    SetImage(btnRefresh, IMG_REFRESH);
}
//---------------------------------------------------------------------------
void __fastcall TFormVersion::InitView()
{
    TcxTreeListNode         *Node;

    tlMain->BeginUpdate();
    tlMain->Clear();
    for (UINT32 i=0; i<g_vSwInfo.size(); i++)
    {
        Node = tlMain->Add();
        Node->Values[0] = g_vSwInfo[i].acSwVer;
        Node->Values[1] = g_vSwInfo[i].acSwFile;
        Node->Values[2] = g_vSwInfo[i].ulSwType;
        Node->Values[3] = GetTextTime(g_vSwInfo[i].ulSwTime);
    }

    tlMain->EndUpdate();
}

void __fastcall TFormVersion::btnAddClick(TObject *Sender)
{
    DialogSetSw->Init();
    if (DialogSetSw->ShowModal() != mrOk)
    {
        return;
    }

    OMC_SW_INFO_T   stInfo = {0};

    DialogSetSw->Get(stInfo);

    GJsonParam  ReqJsonParam;
 // GJson       RspJson;

    ReqJsonParam.Add("sw_ver", stInfo.acSwVer);
    ReqJsonParam.Add("sw_file", stInfo.acSwFile);
    ReqJsonParam.Add("sw_type", stInfo.ulSwType);
    ReqJsonParam.Add("sw_time", stInfo.ulSwTime);

    if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_AddSwInfo, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormVersion::btnDelClick(TObject *Sender)
{
    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlMain->Selections[0];

    if (Node->Values[0].IsNull())
    {
        return;
    }

    AnsiString  strVer = Node->Texts[0];
    AnsiString  strFile = Node->Texts[1];

    if (!AskMsg((CHAR*)"删除版本%s吗？", strVer.c_str()))
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("sw_ver", strVer);
    ReqJsonParam.Add("sw_file", strFile);

    if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_DelSwInfo, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormVersion::btnRefreshClick(TObject *Sender)
{
    if (!LoadSwInfo())
    {
        return;
    }

    FormSwUpdate->UpdateSwInfo();

    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormVersion::ColumnTypeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
     if (ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulSwType = ANode->Values[2];

    Value = GetSwTypeText(ulSwType);
}
//---------------------------------------------------------------------------

