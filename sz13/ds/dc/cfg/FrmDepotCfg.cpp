//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmDepotCfg.h"
#include "DlgSetDepot.h"
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
TFormDepotCfg *FormDepotCfg;

//---------------------------------------------------------------------------
__fastcall TFormDepotCfg::TFormDepotCfg(TComponent* Owner)
    : TSubForm(Owner)
{
    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormDepotCfg::btnAddClick(TObject *Sender)
{
    DialogSetDepot->Init();
    if (DialogSetDepot->ShowModal() != mrOk)
    {
        return;
    }

    DEPOT_INFO_T  stInfo = {0};

    DialogSetDepot->Get(stInfo);

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("DepotID", stInfo.ulDepotID);
    ReqJsonParam.Add("DepotName", stInfo.acDepotName);
    ReqJsonParam.Add("TrainDirection", stInfo.ulTrainDirection);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCAddDepotCfg, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormDepotCfg::btnDelClick(TObject *Sender)
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

    ReqJsonParam.Add("DepotID", ulID);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCDelDepotCfg, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormDepotCfg::btnSetClick(TObject *Sender)
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

    UINT32          ulDepotID = (UINT32)Node->Values[0];
    DEPOT_INFO_T    *pstInfo = Depot::GetDepotInfo(ulDepotID);

    if (!pstInfo)
    {
        return;
    }

    DialogSetDepot->Init(pstInfo);
    if (DialogSetDepot->ShowModal() != mrOk)
    {
        return;
    }

    DEPOT_INFO_T      stInfo = {0};

    DialogSetDepot->Get(stInfo);

    if (stInfo.ulDepotID == pstInfo->ulDepotID &&
        strcmp(stInfo.acDepotName, pstInfo->acDepotName) == 0 &&
        stInfo.ulTrainDirection == pstInfo->ulTrainDirection )
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("DepotID", stInfo.ulDepotID);
    ReqJsonParam.Add("DepotName", stInfo.acDepotName);
    ReqJsonParam.Add("TrainDirection", stInfo.ulTrainDirection);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCSetDepotCfg, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormDepotCfg::btnRefreshClick(TObject *Sender)
{
    if (!LoadDepotInfo())
    {
        return;
    }

    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormDepotCfg::InitView()
{
    TcxTreeListNode         *Node;
    VECTOR<DEPOT_INFO_T*> &vDepotInfo = Depot::GetDepotInfo();

    tlMain->BeginUpdate();
    tlMain->Clear();
    for (UINT32 i=0; i<vDepotInfo.size(); i++)
    {
        Node = tlMain->Add();
        Node->Values[0] = vDepotInfo[i]->ulDepotID;
        Node->Values[1] = vDepotInfo[i]->acDepotName;
        Node->Values[2] = vDepotInfo[i]->ulTrainDirection;
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormDepotCfg::FormCreate(TObject *Sender)
{
    SetImage(btnAdd, IMG_ADD);
    SetImage(btnDel, IMG_DEL);
    SetImage(btnSet, IMG_SET);
    SetImage(btnRefresh, IMG_REFRESH);
}
//---------------------------------------------------------------------------

