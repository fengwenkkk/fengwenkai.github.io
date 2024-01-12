//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "DlgNewMultiGroup.h"
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
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "dxBevel"
#pragma link "dxBevel"
#pragma resource "*.dfm"
TDialogNewMultiGroup *DialogNewMultiGroup;
//---------------------------------------------------------------------------
__fastcall TDialogNewMultiGroup::TDialogNewMultiGroup(TComponent* Owner)
    : TDialogForm(Owner)
{
    Init();
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewMultiGroup::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

void __fastcall TDialogNewMultiGroup::InitSkin()
{
    SetSkin(tlMain);
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewMultiGroup::InitTree()
{
    TcxTreeListNode     *TypeNode;
    TcxTreeListNode     *Node;
    std::vector<UINT32> vGroupType;
    INT64               i64MultiGroupID;
    UINT32              ulDCUserID;

    Group::GetGroupType(vGroupType);

    tlMain->Root->CheckGroupType = ncgCheckGroup;
    tlMain->OptionsView->CheckGroups = true;

    tlMain->BeginUpdate();
    tlMain->Clear();

    for (UINT32 i=0; i<vGroupType.size(); i++)
    {
        UINT32      ulGroupType = vGroupType[i];
        AnsiString  strGroupType = Group::GetGroupTypeName(ulGroupType);
        VECTOR<GROUP_INFO_T*>   vGroup;

        Group::GetGroup(ulGroupType, vGroup);

        TypeNode = tlMain->Add();
        TypeNode->ImageIndex = -1;

        TypeNode->CheckGroupType = ncgCheckGroup;
        TypeNode->Values[0] = strGroupType;

        for (UINT32 j=0; j<vGroup.size(); j++)
        {
            Node = TypeNode->AddChild();
            Node->ImageIndex = 0;

            Node->Values[0] = Group::GetGroupDisplayName(vGroup[j]->i64GroupID);
            Node->Values[1] = vGroup[j]->i64GroupID;

            if (MultiGroup::GetBelongingMultiGroupInfo(vGroup[j]->i64GroupID, i64MultiGroupID, ulDCUserID) )
            {
                Node->Values[2] = Group::GetGroupDisplayName(i64MultiGroupID);
                Node->Values[3] = ulDCUserID;

                Node->CheckGroupType = ncgNone;
                Node->Enabled = false;

                if (!cbShowAll->Checked)
                {
                    Node->Visible = false;
                }
            }
            else
            {
                Node->CheckGroupType = ncgCheckGroup;
            }
        }
    }

    tlMain->Root->Expand(true);
    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewMultiGroup::Init()
{
    edtGroupID->Text = "";
    edtName->Text = "";

    InitTree();
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewMultiGroup::btnOKClick(TObject *Sender)
{
    AnsiString  strName = edtName->Text.Trim();

    if (!ToInt64(edtGroupID->Text, &m_i64GroupID) ||
        m_i64GroupID <= 0)
    {
        ErrMsg("组编号非法");
        edtGroupID->Focused();
        ModalResult = mrNone;
        return;
    }

    if (strName.Length() == 0)
    {
        ErrMsg("组名称不能为空");
        edtName->Focused();
        ModalResult = mrNone;
        return;
    }

    if (strName.Length() >= (int)sizeof(m_acGroupName))
    {
        ErrMsg("组名称长度不能大于%u", sizeof(m_acGroupName)-1);
        edtName->Focused();
        ModalResult = mrNone;
        return;
    }

    StrCpy(m_acGroupName, strName);

    INT64               i64GroupID;
    TcxTreeListNode     *Node = tlMain->Root->getFirstChild();

    m_vGroup.clear();
    for (;Node; Node=Node->GetNext())
    {
        if (!Node->Checked ||
            !Node->Enabled ||
            Node->Values[1].IsNull())
        {
            continue;
        }

        i64GroupID = (INT64)Node->Values[1];
        m_vGroup.push_back(i64GroupID);
    }

    if (m_vGroup.size() <= 1)
    {
        ErrMsg("请选择至少2个群组");
        tlMain->Focused();
        ModalResult = mrNone;
        return;
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewMultiGroup::GetGroupInfo(INT64 *pi64GroupID, CHAR *szGroupName, VECTOR<INT64> &vGroup)
{
    *pi64GroupID = m_i64GroupID;
    strcpy(szGroupName, m_acGroupName);
    vGroup.swap(m_vGroup);
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewMultiGroup::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewMultiGroup::cbShowAllPropertiesChange(TObject *Sender)
{
    tlMain->BeginUpdate();

    for (int i=0; i<tlMain->AbsoluteCount; i++)
    {
        if (cbShowAll->Checked)
        {
            tlMain->AbsoluteItems[i]->Visible = true;
        }
        else
        {
            if (tlMain->AbsoluteItems[i]->Values[3].IsNull())
            {
                tlMain->AbsoluteItems[i]->Visible = true;
            }
            else
            {
                tlMain->AbsoluteItems[i]->Visible = false;
            }
        }
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------

