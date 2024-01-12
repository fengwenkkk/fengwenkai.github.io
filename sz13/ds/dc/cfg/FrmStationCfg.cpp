//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmStationCfg.h"
#include "DlgSetStation.h"
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
TFormStationCfg *FormStationCfg;

//---------------------------------------------------------------------------
__fastcall TFormStationCfg::TFormStationCfg(TComponent* Owner)
    : TSubForm(Owner)
{
    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormStationCfg::btnAddClick(TObject *Sender)
{
    DialogSetStation->Init();
    if (DialogSetStation->ShowModal() != mrOk)
    {
        return;
    }

    STATION_INFO_T  stInfo = {0};

    DialogSetStation->Get(stInfo);

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("StationID", stInfo.ulStationID);
    ReqJsonParam.Add("StationName", stInfo.acStationName);
    ReqJsonParam.Add("StationType", stInfo.ulStationType);
    ReqJsonParam.Add("InterLockStationID", stInfo.ulInterLockStationID);
    ReqJsonParam.Add("StationGroupID", stInfo.i64StationGroupID);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCAddStation, ReqJsonParam))
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

    if (!AskMsg((CHAR*)"删除%s吗？", strName.c_str()))
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("StationID", ulID);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCDelStation, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormStationCfg::btnSetClick(TObject *Sender)
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

    UINT32          ulStationID = (UINT32)Node->Values[0];
    STATION_INFO_T  *pstInfo = Station::GetStationInfo(ulStationID);

    if (!pstInfo)
    {
        return;
    }

    DialogSetStation->Init(pstInfo);
    if (DialogSetStation->ShowModal() != mrOk)
    {
        return;
    }

    STATION_INFO_T      stInfo = {0};

    DialogSetStation->Get(stInfo);

    if (strcmp(stInfo.acStationName, pstInfo->acStationName) == 0 &&
        stInfo.ulStationType == pstInfo->ulStationType &&
        stInfo.ulInterLockStationID == pstInfo->ulInterLockStationID &&
        stInfo.i64StationGroupID == pstInfo->i64StationGroupID )
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("StationID", stInfo.ulStationID);
    ReqJsonParam.Add("StationName", stInfo.acStationName);
    ReqJsonParam.Add("StationType", stInfo.ulStationType);
    ReqJsonParam.Add("InterLockStationID", stInfo.ulInterLockStationID);
    ReqJsonParam.Add("StationGroupID", stInfo.i64StationGroupID);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCSetStation, ReqJsonParam))
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
    if (!LoadStationInfo())
    {
        return;
    }

    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormStationCfg::InitView()
{
    TcxTreeListNode         *Node;
    VECTOR<STATION_INFO_T*> &vStationInfo = Station::GetStationInfo();

    tlMain->BeginUpdate();
    tlMain->Clear();
    for (UINT32 i=0; i<vStationInfo.size(); i++)
    {
        Node = tlMain->Add();
        Node->Values[0] = vStationInfo[i]->ulStationID;
        Node->Values[1] = vStationInfo[i]->acStationName;
        Node->Values[2] = vStationInfo[i]->ulStationType;
        Node->Values[3] = vStationInfo[i]->ulInterLockStationID;
        Node->Values[4] = vStationInfo[i]->i64StationGroupID;
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormStationCfg::Column3GetDisplayText(TcxTreeListColumn *Sender,
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

void __fastcall TFormStationCfg::Column4GetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
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
void __fastcall TFormStationCfg::tlMainColumn5GetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (!ANode || ANode->Values[3].IsNull())
    {
        return;
    }

    UINT32  ulStationID = ANode->Values[3];
    Value = Station::GetStationName(ulStationID);
}
//---------------------------------------------------------------------------

