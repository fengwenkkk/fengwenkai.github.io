//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"

#include "MsgDialog.h"
#include "msg.h"
#include "ds_public.h"
#include "ClientUtil.h"
#include "Resource.h"
#include "ClientCommon.h"
#include "RemoteMsg.h"
#include "ClientInit.h"

#include "FrmPredefinedSMSCfg.h"
#include "DlgSetPredefinedSMS.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxCustomData"
#pragma link "cxDropDownEdit"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxGroupBox"
#pragma link "cxInplaceContainer"
#pragma link "cxLabel"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxMemo"
#pragma link "cxStyles"
#pragma link "cxTextEdit"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
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
#pragma link "MDIForm"
#pragma link "dxBar"
#pragma link "cxProgressBar"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma resource "*.dfm"
TFormPredefinedSMSCfg *FormPredefinedSMSCfg;

extern VECTOR<PREDEFINED_SMS_INFO_T>    g_vPredefinedSMS;

//---------------------------------------------------------------------------
__fastcall TFormPredefinedSMSCfg::TFormPredefinedSMSCfg(TComponent* Owner)
    : TSubForm(Owner)
{
    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormPredefinedSMSCfg::btnAddClick(TObject *Sender)
{
    DialogSetPredefinedSMS->Init();
    if (DialogSetPredefinedSMS->ShowModal() != mrOk)
    {
        return;
    }

    PREDEFINED_SMS_INFO_T   stInfo;

    DialogSetPredefinedSMS->Get(stInfo);

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

  //    ReqJsonParam.Add("SMSID", ulSMSID);
    ReqJsonParam.Add("SMSInfo", stInfo.acSMSInfo);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCAddPredefinedSMS, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormPredefinedSMSCfg::btnDelClick(TObject *Sender)
{
    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlMain->Selections[0];

    if (Node->Values[2].IsNull())
    {
        return;
    }

    UINT32      ulSMSID = Node->Values[0];
    AnsiString  strInfo = Node->Texts[2];

    if (!AskMsg((CHAR*)"删除该预定义短消息吗？\n%s", strInfo.c_str()))
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("SMSID", ulSMSID);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCDelPredefinedSMS, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormPredefinedSMSCfg::btnSetClick(TObject *Sender)
{
    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlMain->Selections[0];

    if (Node->Values[2].IsNull())
    {
        return;
    }

    PREDEFINED_SMS_INFO_T   stInfo;

    stInfo.ulSMSID = (UINT32)Node->Values[0];
    stInfo.ulSMSTemplateType = (UINT32)Node->Values[1];
    StrCpy(stInfo.acSMSInfo, Node->Texts[2]);

    DialogSetPredefinedSMS->Init(stInfo);
    if (DialogSetPredefinedSMS->ShowModal() != mrOk)
    {
        return;
    }

    DialogSetPredefinedSMS->Get(stInfo);

    if (Node->Texts[1] == stInfo.acSMSInfo)
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("SMSID", stInfo.ulSMSID);
    ReqJsonParam.Add("SMSTemplateType", stInfo.ulSMSTemplateType);
    ReqJsonParam.Add("SMSInfo", stInfo.acSMSInfo);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCSetPredefinedSMS, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormPredefinedSMSCfg::btnRefreshClick(TObject *Sender)
{
    if (!LoadPredefinedSMS())
    {
        return;
    }

    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormPredefinedSMSCfg::InitView()
{
    TcxTreeListNode         *Node;

    tlMain->BeginUpdate();
    tlMain->Clear();
    for (UINT32 i=0; i<g_vPredefinedSMS.size(); i++)
    {
        Node = tlMain->Add();
        Node->Values[0] = g_vPredefinedSMS[i].ulSMSID;
        Node->Values[1] = g_vPredefinedSMS[i].ulSMSTemplateType;
        Node->Values[2] = g_vPredefinedSMS[i].acSMSInfo;
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormPredefinedSMSCfg::FormCreate(TObject *Sender)
{
    SetImage(btnAdd, IMG_ADD);
    SetImage(btnDel, IMG_DEL);
    SetImage(btnSet, IMG_SET);
    SetImage(btnRefresh, IMG_REFRESH);
}
//---------------------------------------------------------------------------

void __fastcall TFormPredefinedSMSCfg::tlMainColumn3GetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[1].IsNull())
    {
        return;
    }

    UINT32  ulSDSTemplateType = ANode->Values[1];

    if (ulSDSTemplateType == SDS_TEMPLATE_TX)
    {
        Value = "车载台";
    }
    else if (ulSDSTemplateType == SDS_TEMPLATE_FX)
    {
        Value = "固定台";
    }
    else if (ulSDSTemplateType == SDS_TEMPLATE_ALL)
    {
        Value = "车载台/固定台";
    }
}
//---------------------------------------------------------------------------

