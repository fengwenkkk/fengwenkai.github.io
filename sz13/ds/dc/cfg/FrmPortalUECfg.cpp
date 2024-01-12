//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "ClientData.h"
#include "User.h"
#include "Station.h"

#include "FrmPortalUECfg.h"
#include "DlgSetPortalUE.h"
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
TFormPortalUECfg *FormPortalUECfg;

extern AnsiString GetPortalUETypeText(UINT32 ulType);

//---------------------------------------------------------------------------
__fastcall TFormPortalUECfg::TFormPortalUECfg(TComponent* Owner)
    : TSubForm(Owner)
{
    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormPortalUECfg::btnAddClick(TObject *Sender)
{
    DialogSetPortalUE->Init();
    if (DialogSetPortalUE->ShowModal() != mrOk)
    {
        return;
    }

    PORTAL_UE_INFO_T    stInfo = {0};

    DialogSetPortalUE->Get(stInfo);

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("UserID", stInfo.ulUserID);
    ReqJsonParam.Add("Name", stInfo.acName);
    ReqJsonParam.Add("Type", stInfo.ulType);
    ReqJsonParam.Add("FuncType", stInfo.ulFuncType);
    ReqJsonParam.Add("StationID", stInfo.ulStationID);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCAddPortalUE, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormPortalUECfg::btnDelClick(TObject *Sender)
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

    AnsiString  strUserID = Node->Texts[0];

    if (!AskMsg((CHAR*)"删除手持台%s吗？", strUserID.c_str()))
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("UserID", strUserID.c_str());

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCDelPortalUE, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormPortalUECfg::btnSetClick(TObject *Sender)
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

    UINT32          ulUserID = (UINT32)Node->Values[0];
    PORTAL_UE_INFO_T    *pstInfo = PortalUE::GetPortalUE(ulUserID);

    if (!pstInfo)
    {
        InfoMsg("终端不存在，确认是否已删除");
        return;
    }

    DialogSetPortalUE->Init(*pstInfo);
    if (DialogSetPortalUE->ShowModal() != mrOk)
    {
        return;
    }

    PORTAL_UE_INFO_T        stInfo = {0};

    DialogSetPortalUE->Get(stInfo);

    if (strcmp(stInfo.acName, pstInfo->acName) == 0 &&
        stInfo.ulType == pstInfo->ulType &&
        stInfo.ulStationID == pstInfo->ulStationID )
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("UserID", ulUserID);
    ReqJsonParam.Add("Name", stInfo.acName);
    ReqJsonParam.Add("Type", stInfo.ulType);
    ReqJsonParam.Add("FuncType", stInfo.ulFuncType);
    ReqJsonParam.Add("StationID", stInfo.ulStationID);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCSetPortalUE, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormPortalUECfg::btnRefreshClick(TObject *Sender)
{
    if (!LoadPortalUEInfo())
    {
        return;
    }

    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormPortalUECfg::InitView()
{
    TcxTreeListNode             *Node;
    VECTOR<PORTAL_UE_INFO_T*>   vInfo;

    PortalUE::GetPortalUE(vInfo);

    tlMain->BeginUpdate();
    tlMain->Clear();
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        Node = tlMain->Add();
        Node->Values[0] = vInfo[i]->ulUserID;
        Node->Values[1] = vInfo[i]->acName;
        Node->Values[2] = vInfo[i]->ulType;
        Node->Values[3] = vInfo[i]->ulFuncType;
        Node->Values[4] = vInfo[i]->ulStationID;
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormPortalUECfg::ColumnTypeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulType = ANode->Values[2];

    Value = GetPortalUETypeText(ulType);
}
//---------------------------------------------------------------------------
void __fastcall TFormPortalUECfg::ColumnFuncTypeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[3].IsNull())
    {
        return;
    }

    UINT32  ulType = ANode->Values[3];

    Value = GetUserFuncTypeText(ulType);
}
//---------------------------------------------------------------------------
void __fastcall TFormPortalUECfg::ColumnStationIDGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[4].IsNull())
    {
        return;
    }

    UINT32  ulStationID = ANode->Values[4];

    Value = Station::GetStationName(ulStationID);
}
//---------------------------------------------------------------------------
void __fastcall TFormPortalUECfg::FormCreate(TObject *Sender)
{
    SetImage(btnAdd, IMG_ADD);
    SetImage(btnDel, IMG_DEL);
    SetImage(btnSet, IMG_SET);
    SetImage(btnRefresh, IMG_REFRESH);
}
//---------------------------------------------------------------------------

