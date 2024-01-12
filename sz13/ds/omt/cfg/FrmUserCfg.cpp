//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"
#include "MsgDialog.h"
#include "msg.h"
#include "ds_public.h"
#include "omc.h"
#include "ClientUtil.h"
#include "Resource.h"
#include "ClientCommon.h"
#include "RemoteMsg.h"
#include "ClientInit.h"

#include "FrmUserCfg.h"
#include "DlgSetOmtOperator.h"

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
#pragma link "cxClasses"
#pragma link "dxBar"
#pragma resource "*.dfm"
TFormUserCfg *FormUserCfg;
extern VECTOR<OMT_OPERATOR_INFO_T>              g_vOmtOperatorInfo;


AnsiString GetUserTypeText(UINT32 ulUserType)
{
	if (ulUserType == OPER_TYPE_ADMIN)  return "超级用户";
	if (ulUserType == OPER_TYPE_MANAGE) return "管理用户";
	if (ulUserType == OPER_TYPE_QUERY)  return "查询用户";

    return "";
}

__fastcall TFormUserCfg::TFormUserCfg(TComponent* Owner)
    : TSubForm(Owner)
{
    //InitView();

    btnRefreshClick(this);

//  if (g_stLocalCfg.ulOmtUserType == OMT_USER_TYPE_ROUSER)
//  {
//      SetBtnDisable();
//  }
}
//---------------------------------------------------------------------------

void __fastcall TFormUserCfg::SetBtnDisable()
{
    btnAdd->Enabled = false;
    btnDel->Enabled = false;
    btnSet->Enabled = false;

    pmiAdd->Enabled = false;
    pmiDel->Enabled = false;
    pmiSet->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TFormUserCfg::InitView()
{
    TcxTreeListNode    *Node;

    tlMain->BeginUpdate();
    tlMain->Clear();
    for (UINT32 i=0; i<g_vOmtOperatorInfo.size(); i++)
    {
        Node = tlMain->Add();
        Node->Values[0] = i+1;
        Node->Values[1] = g_vOmtOperatorInfo[i].acName;
        Node->Values[2] = g_vOmtOperatorInfo[i].acPassword;
        Node->Values[3] = g_vOmtOperatorInfo[i].ulOmtType;
    }

    tlMain->EndUpdate();
}
//-------------------------------------------------------------------------

void __fastcall TFormUserCfg::btnAddClick(TObject *Sender)
{
    DialogSetOmtOperator->Init();
    if (DialogSetOmtOperator->ShowModal() != mrOk)
    {
        return;
    }

    OMT_OPERATOR_INFO_T      stInfo;

    DialogSetOmtOperator->Get(stInfo);

    GJsonParam  ReqJsonParam;

    //ReqJsonParam.Add("OperatorID", stInfo.acOperatorID);
    ReqJsonParam.Add("User", stInfo.acName);
    ReqJsonParam.Add("Pwd", stInfo.acPassword);
    ReqJsonParam.Add("Type", stInfo.ulOmtType);

    if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_AddNewUser, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------

void __fastcall TFormUserCfg::btnRefreshClick(TObject *Sender)
{
    if (!LoadOmtOperatorInfo())
    {
        return;
    }

    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormUserCfg::btnDelClick(TObject *Sender)
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

    AnsiString  strUserName = Node->Texts[1];

    //if (strUserName == "super")
    if (strUserName == "super")
    {
        WarnMsg("[ super ]账号不能被删除");
        return;
    }

    if (!AskMsg((CHAR*)"确认删除用户 %s 吗？", strUserName.c_str()))
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("User", strUserName.c_str());

    if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_DeleteUser, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormUserCfg::btnSetClick(TObject *Sender)
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

    OMT_OPERATOR_INFO_T      stInfo;
    AnsiString               strOperatorUser = Node->Values[1];
    AnsiString               strOperatorPwd = Node->Values[2];

    StrCpy(stInfo.acName, strOperatorUser);
    StrCpy(stInfo.acPassword, strOperatorPwd);
    stInfo.ulOmtType = (UINT32)Node->Values[3];

    if (strOperatorUser == "super" &&
        strcmp(g_stLocalCfg.acUserName, "super") != 0)
    {
        WarnMsg("本账号无权修改[ super ]账号");
        return;
    }

    DialogSetOmtOperator->Init(stInfo);
    if (DialogSetOmtOperator->ShowModal() != mrOk)
    {
        return;
    }

    OMT_OPERATOR_INFO_T      stNewInfo;

    DialogSetOmtOperator->Get(stNewInfo);

    if (strOperatorUser == stNewInfo.acName &&
        strOperatorPwd == stNewInfo.acPassword &&
        stInfo.ulOmtType == stNewInfo.ulOmtType )
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("User", stNewInfo.acName);
    ReqJsonParam.Add("Pwd", stNewInfo.acPassword);
    ReqJsonParam.Add("UserType", stNewInfo.ulOmtType);

    if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_AlterUserInfo, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormUserCfg::ColumnUserTypeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[3].IsNull())
    {
        return ;
    }

    UINT32 ulUserType = ANode->Values[3];
    Value = GetUserTypeText(ulUserType);
}
//---------------------------------------------------------------------------


void __fastcall TFormUserCfg::FormCreate(TObject *Sender)
{
    SetImage(btnAdd, IMG_ADD);
    SetImage(btnDel, IMG_DEL);
    SetImage(btnSet, IMG_SET);
    SetImage(btnRefresh, IMG_REFRESH);

    SetImage(pmiAdd, IMG_ADD);
    SetImage(pmiDel, IMG_DEL);
    SetImage(pmiSet, IMG_SET);
    SetImage(pmiRefresh, IMG_REFRESH);
}
//---------------------------------------------------------------------------



