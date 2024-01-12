//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "SDSTemplate.h"
#include "FrmSDSTemplate.h"
#include "DlgSetSDSTemplate.h"
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
TFormSDSTemplate *FormSDSTemplate;

void __fastcall TFormSDSTemplate::InitWorkPanel()
{
    TcxButton   *btns[] = { btnRefresh,
                            btnAdd,
                            btnDel,
                            btnSet
                          };

    gbWorkPanel->Width = 120;

    for (UINT32 i=0; i<ARRAY_SIZE(btns); i++)
    {
        btns[i]->TabOrder = i;
        btns[i]->Top = 50+95*i;
        btns[i]->Width = g_iMidToolbarButtonWidth;
        btns[i]->Height = g_iMidToolbarButtonHeight;
        btns[i]->Left = (btns[i]->Parent->Width - btns[i]->Width)/2;
    }
}

//---------------------------------------------------------------------------
__fastcall TFormSDSTemplate::TFormSDSTemplate(TComponent* Owner)
    : TSubForm(Owner)
{
    InitView();
    InitWorkPanel();
}
//---------------------------------------------------------------------------
void __fastcall TFormSDSTemplate::btnAddClick(TObject *Sender)
{
    DialogSetSDSTemplate->Init();
    if (DialogSetSDSTemplate->ShowModal() != mrOk)
    {
        return;
    }

    SDS_TEMPLATE_T      stInfo;

    DialogSetSDSTemplate->Get(stInfo);

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("SDSType", stInfo.ulSDSType);
    ReqJsonParam.Add("Title", stInfo.acTitle);
    ReqJsonParam.Add("SDSInfo", EncodeSMSMsg(stInfo.acSDSInfo));

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCAddSDSTemplate, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormSDSTemplate::btnDelClick(TObject *Sender)
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

    UINT32      ulTemplateID = Node->Values[0];
    AnsiString  strTitle = Node->Texts[2];

    if (!AskMsg((CHAR*)"删除该模板吗？\n%s", strTitle.c_str()))
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("TemplateID", ulTemplateID);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCDelSDSTemplate, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormSDSTemplate::btnSetClick(TObject *Sender)
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

    SDS_TEMPLATE_T    stInfo;

    stInfo.ulTemplateID = (UINT32)Node->Values[0];
    stInfo.ulSDSType = (UINT32)Node->Values[1];
    StrCpy(stInfo.acTitle, Node->Texts[2]);
    StrCpy(stInfo.acSDSInfo, Node->Texts[3]);

    DialogSetSDSTemplate->Init(stInfo);
    if (DialogSetSDSTemplate->ShowModal() != mrOk)
    {
        return;
    }

    DialogSetSDSTemplate->Get(stInfo);

    if (stInfo.ulSDSType == (UINT32)Node->Values[1] &&
        Node->Texts[2] == stInfo.acTitle &&
        Node->Texts[3] == stInfo.acSDSInfo )
    {
        return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("TemplateID", stInfo.ulTemplateID);
    ReqJsonParam.Add("SDSType", stInfo.ulSDSType);
    ReqJsonParam.Add("Title", stInfo.acTitle);
    ReqJsonParam.Add("SDSInfo", EncodeSMSMsg(stInfo.acSDSInfo));

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCSetSDSTemplate, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormSDSTemplate::btnRefreshClick(TObject *Sender)
{
    if (!LoadSDSTemplate())
    {
        return;
    }

    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormSDSTemplate::InitView()
{
    TcxTreeListNode         *Node;
    VECTOR<SDSTemplate*>    &vInfo = GetSDSTemplate();

    tlMain->BeginUpdate();
    tlMain->Clear();
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        SDSTemplate     *pTemplate = vInfo[i];
        SDS_TEMPLATE_T  &stTemplate = pTemplate->GetTemplate();

        Node = tlMain->Add();
        Node->Values[0] = stTemplate.ulTemplateID;
        Node->Values[1] = stTemplate.ulSDSType;
        Node->Values[2] = stTemplate.acTitle;
        Node->Values[3] = stTemplate.acSDSInfo;
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormSDSTemplate::btnImportClick(TObject *Sender)
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
void __fastcall TFormSDSTemplate::Column2GetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[1].IsNull())
    {
        return;
    }

    UINT32  ulSDSType = ANode->Values[1];

    Value = GetSDSTypeText(ulSDSType);
}
//---------------------------------------------------------------------------

void __fastcall TFormSDSTemplate::TimerTimer(TObject *Sender)
{
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------

