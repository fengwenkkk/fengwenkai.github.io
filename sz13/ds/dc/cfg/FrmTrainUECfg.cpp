//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"

#include "FrmTrainUECfg.h"
#include "DlgSetTrainUE.h"
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
TFormTrainUECfg *FormTrainUECfg;

//---------------------------------------------------------------------------
__fastcall TFormTrainUECfg::TFormTrainUECfg(TComponent* Owner)
    : TSubForm(Owner)
{
    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainUECfg::btnAddClick(TObject *Sender)
{
    DialogSetTrainUE->Init();
    if (DialogSetTrainUE->ShowModal() != mrOk)
    {
        return;
    }

    TRAIN_UE_INFO_T     stInfo = {0};

    DialogSetTrainUE->Get(stInfo);

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("UserID", stInfo.ulUserID);
    ReqJsonParam.Add("Name", stInfo.acName);
    ReqJsonParam.Add("LanAddr", stInfo.acLanAddr);
    ReqJsonParam.Add("TrainUnitID", stInfo.ulTrainUnitID);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCAddTrainUE, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainUECfg::btnDelClick(TObject *Sender)
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

    if (!AskMsg((CHAR*)"删除车载台%s吗？", strUserID.c_str()))
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("UserID", strUserID.c_str());

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCDelTrainUE, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainUECfg::btnSetClick(TObject *Sender)
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
    TRAIN_UE_INFO_T *pstInfo = TrainUE::GetTrainUE(ulUserID);

    if (!pstInfo)
    {
        InfoMsg("终端不存在，确认是否已删除");
        return;
    }

    DialogSetTrainUE->Init(*pstInfo);
    if (DialogSetTrainUE->ShowModal() != mrOk)
    {
        return;
    }

    TRAIN_UE_INFO_T     stInfo = {0};

    DialogSetTrainUE->Get(stInfo);

    if (strcmp(stInfo.acName, pstInfo->acName) == 0 &&
        strcmp(stInfo.acLanAddr, pstInfo->acLanAddr) == 0 &&
        stInfo.ulTrainUnitID == pstInfo->ulTrainUnitID)
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("UserID", ulUserID);
    ReqJsonParam.Add("Name", stInfo.acName);
    ReqJsonParam.Add("LanAddr", stInfo.acLanAddr);
    ReqJsonParam.Add("TrainUnitID", stInfo.ulTrainUnitID);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCSetTrainUE, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainUECfg::btnRefreshClick(TObject *Sender)
{
    if (!LoadTrainUEInfo())
    {
        return;
    }

    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainUECfg::InitView()
{
    TcxTreeListNode             *Node;
    VECTOR<TRAIN_UE_INFO_T*>    &vInfo = TrainUE::GetTrainUE();

    tlMain->BeginUpdate();
    tlMain->Clear();
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        Node = tlMain->Add();
        Node->Values[0] = vInfo[i]->ulUserID;
        Node->Values[1] = vInfo[i]->acName;
        Node->Values[2] = vInfo[i]->acLanAddr;
        Node->Values[3] = vInfo[i]->ulTrainUnitID;
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainUECfg::FormCreate(TObject *Sender)
{
    SetImage(btnAdd, IMG_ADD);
    SetImage(btnDel, IMG_DEL);
    SetImage(btnSet, IMG_SET);
    SetImage(btnRefresh, IMG_REFRESH);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainUECfg::Column4GetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[3].IsNull())
    {
        return;
    }

    UINT32  ulTrainUnitID = ANode->Values[3];

    Value = Train::GetTrainUnitIDText(ulTrainUnitID);
}
//---------------------------------------------------------------------------

