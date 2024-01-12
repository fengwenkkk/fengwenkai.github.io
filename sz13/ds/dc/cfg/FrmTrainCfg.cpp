//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "DynamicGroup.h"

#include "FrmTrainCfg.h"
#include "DlgSetTrain.h"
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
TFormTrainCfg *FormTrainCfg;

extern AnsiString GetTrainTypeText(UINT32 ulType);

//---------------------------------------------------------------------------
__fastcall TFormTrainCfg::TFormTrainCfg(TComponent* Owner)
    : TSubForm(Owner)
{
    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::btnAddClick(TObject *Sender)
{
    DialogSetTrain->Init();
    if (DialogSetTrain->ShowModal() != mrOk)
    {
        return;
    }

    TRAIN_INFO_T        stInfo;

    DialogSetTrain->Get(stInfo);

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("DCUserID", APP::GetCurrDCUserID());
    ReqJsonParam.Add("TrainUnitID", stInfo.ulTrainUnitID);
    ReqJsonParam.Add("TrainType", stInfo.ulTrainType);
    ReqJsonParam.Add("TrainName", stInfo.acTrainName);
    ReqJsonParam.Add("DefaultGroupID", stInfo.i64DefaultGroupID);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCAddTrain, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::btnDelClick(TObject *Sender)
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

    if (!AskMsg((CHAR*)"删除列车%u吗？", ulID))
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("TrainUnitID", ulID);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCDelTrain, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::btnSetClick(TObject *Sender)
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

    UINT32          ulTrainUnitID = (UINT32)Node->Values[0];
    TRAIN_INFO_T    *pstOldInfo = Train::GetTrainInfo(ulTrainUnitID);;

    if (!pstOldInfo)
    {
        return;
    }

    DialogSetTrain->Init(*pstOldInfo);
    if (DialogSetTrain->ShowModal() != mrOk)
    {
        return;
    }

    TRAIN_INFO_T    stInfo;

    DialogSetTrain->Get(stInfo);

    if (stInfo.ulTrainUnitID     == pstOldInfo->ulTrainUnitID &&
        stInfo.ulTrainType       == pstOldInfo->ulTrainType &&
        strcmp(stInfo.acTrainName, pstOldInfo->acTrainName) == 0 &&
        stInfo.i64DefaultGroupID == pstOldInfo->i64DefaultGroupID )
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("TrainUnitID", stInfo.ulTrainUnitID);
    ReqJsonParam.Add("TrainType", stInfo.ulTrainType);
    ReqJsonParam.Add("TrainName", stInfo.acTrainName);
    ReqJsonParam.Add("DefaultGroupID", stInfo.i64DefaultGroupID);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCSetTrain, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::btnRefreshClick(TObject *Sender)
{
    if (!LoadTrainInfo())
    {
        return;
    }

    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::InitNode(TcxTreeListNode *Node, TRAIN_INFO_T *pstTrain)
{
    UINT32      ulUserID1;
    UINT32      ulUserID2;

    Train::GetTrainUserID(pstTrain->ulTrainUnitID, &ulUserID1, &ulUserID2);

    Node->Values[0] = pstTrain->ulTrainUnitID;
    Node->Values[1] = pstTrain->ulTrainType;
    Node->Values[2] = ulUserID1;
    Node->Values[3] = ulUserID2;
    Node->Values[4] = pstTrain->i64DefaultGroupID;
    Node->Values[7] = 0;//pstTrain->ulDCUserID;
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::InitView()
{
    TcxTreeListNode         *Node;
    VECTOR<TRAIN_INFO_T*>   vInfo;
    TRAIN_INFO_T            *pstTrain;

    Train::GetTrainInfo(vInfo);

    tlMain->BeginUpdate();

    for (int i=tlMain->AbsoluteCount-1; i>=0; i--)
    {
        Node = tlMain->AbsoluteItems[i];
        pstTrain = NULL;

        for (UINT32 j=0; j<vInfo.size(); j++)
        {
            if ((UINT32)Node->Values[0] == vInfo[j]->ulTrainUnitID)
            {
                pstTrain = vInfo[j];
                InitNode(Node, pstTrain);
                vInfo.erase(vInfo.begin() + j);
                break;
            }
        }

        if (!pstTrain)
        {
            Node->Delete();
        }
    }

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        Node = tlMain->Add();

        InitNode(Node, vInfo[i]);
        Node->Values[8] = "";
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::ColumnTrainTypeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    if (ANode->Values[1].IsNull())
    {
        return;
    }

    UINT32  ulType = ANode->Values[1];

    Value = GetTrainTypeText(ulType);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::btnImportClick(TObject *Sender)
{
    if (!OpenDialog->Execute())
    {
        return;
    }

    AnsiString  strFile = OpenDialog->FileName;

    if (!ImportFromFile(tlMain, strFile.c_str(), false))
    {
        InfoMsg("加载文件失败");
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::FormCreate(TObject *Sender)
{
    SetImage(btnAdd, IMG_ADD);
    SetImage(btnDel, IMG_DEL);
    SetImage(btnSet, IMG_SET);
    SetImage(btnRefresh, IMG_REFRESH);
    SetImage(btnImport, IMG_IMPORT);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrainCfg::ColumnTrainUnitIDGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    if (ANode->Values[0].IsNull())
    {
        return;
    }

    UINT32  ulTrainUnitID = ANode->Values[0];

    Value = Train::GetTrainUnitIDText(ulTrainUnitID);
}
//---------------------------------------------------------------------------

