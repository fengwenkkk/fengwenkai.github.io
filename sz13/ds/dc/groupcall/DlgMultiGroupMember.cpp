//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "DlgMultiGroupMember.h"

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
TDialogMultiGroupMember *DialogMultiGroupMember;

//---------------------------------------------------------------------------
__fastcall TDialogMultiGroupMember::TDialogMultiGroupMember(TComponent* Owner)
    : TDialogForm(Owner)
{

}

void __fastcall TDialogMultiGroupMember::InitSkin()
{
    SetSkin(tlMain);
}
//---------------------------------------------------------------------------
void __fastcall TDialogMultiGroupMember::btnCloseClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogMultiGroupMember::Init(INT64 i64GroupID, VECTOR<INT64> &vMember)
{
    m_vAddMember.clear();
    m_vDelMember.clear();
    m_vMember.assign(vMember.begin(), vMember.end());

    Caption = "设置多选组成员：" + Group::GetGroupDisplayName(i64GroupID);

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

            Node->CheckGroupType = ncgCheckGroup;

            for (UINT32 k=0; k<vMember.size(); k++)
            {
                if (vMember[k] == vGroup[j]->i64GroupID)
                {
                    Node->Checked = true;
                    Node->Visible = true;
                    vMember.erase(vMember.begin()+k);

                    TypeNode->Expand(true);
                    break;
                }
            }

            Node->CheckGroupType = ncgCheckGroup;

            if (MultiGroup::GetBelongingMultiGroupInfo(vGroup[j]->i64GroupID, i64MultiGroupID, ulDCUserID) )
            {
                if (ulDCUserID != APP::GetCurrDCUserID() ||
                    i64MultiGroupID != i64GroupID)
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
            }
        }
    }

    tlMain->Root->Expand(true);
    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TDialogMultiGroupMember::btnOKClick(TObject *Sender)
{
    INT64               i64GroupID;
    bool                bExist;
    TcxTreeListNode     *Node = tlMain->Root->getFirstChild();

    m_vAddMember.clear();
    m_vDelMember.clear();
    m_vDelMember.assign(m_vMember.begin(), m_vMember.end());

    for (;Node; Node=Node->GetNext())
    {
        if (!Node->Checked ||
            !Node->Enabled ||
            Node->Values[1].IsNull())
        {
            continue;
        }

        i64GroupID = (INT64)Node->Values[1];
        bExist = false;
        for (UINT32 i=0; i<m_vDelMember.size(); i++)
        {
            if (m_vDelMember[i] == i64GroupID)
            {
                bExist = true;
                m_vDelMember.erase(m_vDelMember.begin()+i);
                break;
            }
        }

        if (!bExist)
        {
            m_vAddMember.push_back(i64GroupID);
        }
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogMultiGroupMember::FormDeactivate(TObject *Sender)
{
    m_vMember.clear();
}
//---------------------------------------------------------------------------
void __fastcall TDialogMultiGroupMember::GetMember(VECTOR<INT64> &vAddMember, VECTOR<INT64> &vDelMember)
{
    vAddMember.clear();
    vDelMember.clear();

    vAddMember.assign(m_vAddMember.begin(), m_vAddMember.end());
    vDelMember.assign(m_vDelMember.begin(), m_vDelMember.end());
}
void __fastcall TDialogMultiGroupMember::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnClose, IMG_CLOSE);
}
//---------------------------------------------------------------------------
void __fastcall TDialogMultiGroupMember::cbShowAllPropertiesChange(TObject *Sender)
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

