//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"

#include "SDSTemplate.h"

#include "FrmRecBrdCfg.h"
#include "DlgSetRecBrd.h"
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
TFormRecBrdCfg *FormRecBrdCfg;

extern VECTOR<REC_BRD_CFG_T>& GetRecBrdCfg();

//---------------------------------------------------------------------------
__fastcall TFormRecBrdCfg::TFormRecBrdCfg(TComponent* Owner)
    : TSubForm(Owner)
{
    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormRecBrdCfg::btnAddClick(TObject *Sender)
{
    DialogSetRecBrd->Init();
    if (DialogSetRecBrd->ShowModal() != mrOk)
    {
        return;
    }

    REC_BRD_CFG_T   stInfo;

    DialogSetRecBrd->Get(stInfo);

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("RecID", stInfo.ulRecID);
    ReqJsonParam.Add("RecInfo", stInfo.acRecInfo);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCAddRecBrdCfg, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormRecBrdCfg::btnDelClick(TObject *Sender)
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

    UINT32      ulRecID = Node->Values[0];
    AnsiString  strInfo = Node->Texts[1];

    if (!AskMsg((CHAR*)"删除该预录音广播吗？\n%s", strInfo.c_str()))
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("RecID", ulRecID);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCDelRecBrdCfg, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormRecBrdCfg::btnSetClick(TObject *Sender)
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

    REC_BRD_CFG_T   stInfo;

    stInfo.ulRecID = (UINT32)Node->Values[0];
    StrCpy(stInfo.acRecInfo, Node->Texts[1]);

    DialogSetRecBrd->Init(stInfo);
    if (DialogSetRecBrd->ShowModal() != mrOk)
    {
        return;
    }

    DialogSetRecBrd->Get(stInfo);

    if (Node->Texts[1] == stInfo.acRecInfo)
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("RecID", stInfo.ulRecID);
    ReqJsonParam.Add("RecInfo", stInfo.acRecInfo);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCSetRecBrdCfg, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormRecBrdCfg::btnRefreshClick(TObject *Sender)
{
    if (!LoadRecBrdCfg())
    {
        return;
    }

    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormRecBrdCfg::InitView()
{
    TcxTreeListNode         *Node;
    VECTOR<REC_BRD_CFG_T>&  vInfo = GetRecBrdCfg();

    tlMain->BeginUpdate();
    tlMain->Clear();
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        Node = tlMain->Add();
        Node->Values[0] = vInfo[i].ulRecID;
        Node->Values[1] = vInfo[i].acRecInfo;
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormRecBrdCfg::FormCreate(TObject *Sender)
{
    SetImage(btnAdd, IMG_ADD);
    SetImage(btnDel, IMG_DEL);
    SetImage(btnSet, IMG_SET);
    SetImage(btnRefresh, IMG_REFRESH);
}
//---------------------------------------------------------------------------
