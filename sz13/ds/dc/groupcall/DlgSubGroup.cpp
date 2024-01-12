//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "DlgSubGroup.h"
//#include "DlgGroupMember.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxLabel"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxTrackBar"
#pragma link "dxSkinBlue"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "cxTextEdit"
#pragma link "cxCheckBox"
#pragma link "cxDropDownEdit"
#pragma link "cxMaskEdit"
#pragma link "cxButtonEdit"
#pragma link "cxCustomData"
#pragma link "cxInplaceContainer"
#pragma link "cxStyles"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "cxMemo"
#pragma link "cxImageList"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxGroupBox"
#pragma resource "*.dfm"
TDialogSubGroup *DialogSubGroup;

//---------------------------------------------------------------------------
__fastcall TDialogSubGroup::TDialogSubGroup(TComponent* Owner)
    : TDialogForm(Owner)
{
//  memset(&m_stGroupInfo, 0, sizeof(m_stGroupInfo));
}

void __fastcall TDialogSubGroup::InitSkin()
{
    SetSkin(tlMain, true);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSubGroup::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSubGroup::Init(Int64Set &sGroup)
{
    tlMain->BeginUpdate();
    tlMain->Clear();

    tlMain->Root->CheckGroupType = ncgCheckGroup;
    tlMain->OptionsView->CheckGroups = true;

    TcxTreeListNode     *Node;
    TcxTreeListNode     *SubNode;
    UINT32              i;
    VECTOR<GROUP_INFO_T*> vGroup;

    Group::GetGroup(STATIC_GROUP, vGroup);

    // 列车组、车站组、维护组、多职能组、其他
    Node = tlMain->Add();
    Node->Values[0] = "静态组";
    for (i=0; i<vGroup.size(); i++)
    {
        SubNode = Node->AddChild();
        SubNode->Values[0] = Group::GetGroupDisplayName(vGroup[i]->i64GroupID);
        SubNode->Values[1] = vGroup[i]->i64GroupID;
    }

    for (int i=0; i<tlMain->AbsoluteCount; i++)
    {
        Node = tlMain->AbsoluteItems[i];
        Node->CheckGroupType = ncgCheckGroup;
    }

    for (int i=0; i<tlMain->AbsoluteCount; i++)
    {
        Node = tlMain->AbsoluteItems[i];
        if (!Node->Values[1].IsNull())
        {
            INT64   i64GroupID = (INT64)Node->Values[1];

            if (sGroup.Has(i64GroupID))
            {
                Node->Checked = true;
            }
        }
    }

    tlMain->Root->Expand(true);
    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TDialogSubGroup::Get(Int64Set &sGroup)
{
    INT64               i64GroupID;
    TcxTreeListNode     *Node;

    sGroup.Clear();
    for (int i=0; i<tlMain->AbsoluteCount; i++)
    {
        Node = tlMain->AbsoluteItems[i];
        if (Node->Values[1].IsNull() ||
            !Node->Checked )
        {
            continue;
        }

        i64GroupID = Node->Values[1];
        sGroup.Add(i64GroupID);
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogSubGroup::btnOKClick(TObject *Sender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSubGroup::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------

