//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "ClientData.h"
#include "User.h"

#include "FrmDCOperatorCfg.h"
#include "DlgSetDCOperator.h"
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
#pragma link "dxSkinGlassOceans"
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
TFormDCOperatorCfg *FormDCOperatorCfg;

void StrCpy(CHAR *szDst, Variant &src)
{
    AnsiString      strSrc = src.pcVal;

    StrCpy(szDst, strSrc);
}
//---------------------------------------------------------------------------
__fastcall TFormDCOperatorCfg::TFormDCOperatorCfg(TComponent* Owner)
    : TSubForm(Owner)
{
    if (APP::GetDCLoginAccount() == "admin")
    {
        ColumnPwd->Visible = true;
    }
    else
    {
        ColumnPwd->Visible = false;
    }

    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormDCOperatorCfg::btnAddClick(TObject *Sender)
{
    DialogSetDCOperator->Init();
    if (DialogSetDCOperator->ShowModal() != mrOk)
    {
        return;
    }

    DC_OPERATOR_INFO_T      stInfo;

    DialogSetDCOperator->Get(stInfo);

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("OperatorID", stInfo.acOperatorID);
    ReqJsonParam.Add("Name", stInfo.acName);
    ReqJsonParam.Add("Password", stInfo.acPassword);
    ReqJsonParam.Add("DCType", stInfo.ulDCType);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCAddDCOperator, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormDCOperatorCfg::btnDelClick(TObject *Sender)
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

    AnsiString  strUserID = Node->Texts[1];

    if (strUserID == "admin")
    {
        WarnMsg("admin账号不能被删除");
        return;
    }

    if (!AskMsg((CHAR*)"删除调度员%s吗？", strUserID.c_str()))
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("OperatorID", strUserID.c_str());

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCDelDCOperator, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormDCOperatorCfg::btnSetClick(TObject *Sender)
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

    DC_OPERATOR_INFO_T      stInfo;
    AnsiString              strOperatorID = Node->Values[1];
    AnsiString              strName = Node->Values[2];
    AnsiString              strPwd = Node->Values[3];

    StrCpy(stInfo.acOperatorID, strOperatorID);
    StrCpy(stInfo.acName, strName);
    StrCpy(stInfo.acPassword, strPwd);
    stInfo.ulDCType = (UINT32)Node->Values[4];

    if (strOperatorID == "admin" &&
        APP::GetDCLoginAccount() != "admin")
    {
        WarnMsg("本账号无权修改admin账号");
        return;
    }

    DialogSetDCOperator->Init(stInfo);
    if (DialogSetDCOperator->ShowModal() != mrOk)
    {
        return;
    }

    DC_OPERATOR_INFO_T      stNewInfo;

    DialogSetDCOperator->Get(stNewInfo);

    if (strName == stNewInfo.acName &&
        strPwd == stNewInfo.acPassword &&
        stInfo.ulDCType == stNewInfo.ulDCType )
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("OperatorID", stInfo.acOperatorID);
    ReqJsonParam.Add("Name", stNewInfo.acName);
    ReqJsonParam.Add("Password", stNewInfo.acPassword);
    ReqJsonParam.Add("DCType", stNewInfo.ulDCType);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCSetDCOperator, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormDCOperatorCfg::tlMainColumn5GetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormDCOperatorCfg::btnRefreshClick(TObject *Sender)
{
    if (!LoadDCOperatorInfo())
    {
        return;
    }

    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormDCOperatorCfg::InitView()
{
    TcxTreeListNode             *Node;
    VECTOR<DC_OPERATOR_INFO_T>  vInfo;

    DCOperator::GetDCOperator(vInfo);

    tlMain->BeginUpdate();
    tlMain->Clear();
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        Node = tlMain->Add();
        Node->Values[0] = i+1;
        Node->Values[1] = vInfo[i].acOperatorID;
        Node->Values[2] = vInfo[i].acName;
        Node->Values[3] = vInfo[i].acPassword;
        Node->Values[4] = vInfo[i].ulDCType;
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormDCOperatorCfg::ColumnDCTypeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[4].IsNull())
    {
        return;
    }

    UINT32  ulDCType = ANode->Values[4];
    Value = GetDCTypeText(ulDCType);
}
//---------------------------------------------------------------------------

void __fastcall TFormDCOperatorCfg::FormCreate(TObject *Sender)
{
    SetImage(btnAdd, IMG_ADD);
    SetImage(btnDel, IMG_DEL);
    SetImage(btnSet, IMG_SET);
    SetImage(btnRefresh, IMG_REFRESH);
}
//---------------------------------------------------------------------------

