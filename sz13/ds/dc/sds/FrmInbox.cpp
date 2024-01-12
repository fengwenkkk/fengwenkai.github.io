//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "UICfg.h"
#include "eSDKData.h"

#include "FrmInbox.h"
#include "DlgNewSDS.h"
#include "FrmLatestSDS.h"
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
#pragma link "cxCalendar"
#pragma link "cxDateUtils"
#pragma link "cxMaskEdit"
#pragma link "dxCore"
#pragma link "cxClasses"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma resource "*.dfm"
TFormInbox *FormInbox;

void __fastcall TFormInbox::InitSkin()
{
    tlMain->OptionsSelection->CellSelect = true;
}

//---------------------------------------------------------------------------
__fastcall TFormInbox::TFormInbox(TComponent* Owner)
    : TSubForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormInbox::NewSDS(UINT32 ulUserID)
{
    VECTOR<UINT32> vUserID;

    vUserID.push_back(ulUserID);

    FormLatestSDS->NewSDS(vUserID);
}
//---------------------------------------------------------------------------
void __fastcall TFormInbox::ShowNewSDS(EVENT_SDS_REPORT_T &stSDS)
{
    TcxTreeListNode     *Node = tlMain->Add();

    Node->Values[0] = AnsiString(stSDS.acSendDate) + " " + AnsiString(stSDS.acSendTime);
    Node->Values[1] = stSDS.acFrom;
    Node->Values[2] = stSDS.ulSDSType;
    Node->Values[3] = stSDS.acContent;
}
//---------------------------------------------------------------------------
void __fastcall TFormInbox::gbMainDblClick(TObject *Sender)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormInbox::tlMainFocusedNodeChanged(TcxCustomTreeList *Sender, TcxTreeListNode *APrevFocusedNode,
          TcxTreeListNode *AFocusedNode)
{
    pmiReplySDS->Enabled = false;

    if (!AFocusedNode || AFocusedNode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulSDSType = AFocusedNode->Values[2];

    if (eSDKData::IsRealMsg(ulSDSType))
    {
        pmiReplySDS->Enabled = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormInbox::ColumnSDSTypeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (!ANode || ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulSDSType = ANode->Values[2];

    Value = eSDKData::GetSDSTypeText(ulSDSType);
}
//---------------------------------------------------------------------------
void __fastcall TFormInbox::FormCreate(TObject *Sender)
{
    SetImage(pmiNewSDS, IMG_ADD);
    SetImage(pmiReplySDS, IMG_SDS_REPLY);
}
//---------------------------------------------------------------------------

void __fastcall TFormInbox::pmiReplySDSClick(TObject *Sender)
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

    INT64   i64GroupID;
    AnsiString  strSender = Node->Values[1];

    if (!gos_atoi64(strSender.c_str(), &i64GroupID))
    {
        ErrMsg("未知的发送人");
        return;
    }

    if (!eSDK_ReplyRealMsg(i64GroupID))
    {
        ErrMsg("发送回执失败");
    }
    else
    {
        InfoMsg("发送回执成功");
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormInbox::pmiNewSDSClick(TObject *Sender)
{
    NewSDS(INVALID_USER_ID);
}
//---------------------------------------------------------------------------
void __fastcall TFormInbox::FormShow(TObject *Sender)
{
    gbMain->Style->Font->Size = UICfg::GetGroupBoxFontSize();
}
//---------------------------------------------------------------------------

