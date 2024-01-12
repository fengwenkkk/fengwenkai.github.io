//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "DlgStaticGroupMember.h"

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
TDialogStaticGroupMember *DialogStaticGroupMember;

//---------------------------------------------------------------------------
__fastcall TDialogStaticGroupMember::TDialogStaticGroupMember(TComponent* Owner)
    : TDialogForm(Owner)
{
}

void __fastcall TDialogStaticGroupMember::InitSkin()
{
    SetSkin(tlMain, true);
}
//---------------------------------------------------------------------------
void __fastcall TDialogStaticGroupMember::btnCloseClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogStaticGroupMember::Init(GROUP_INFO_T *pstGroup, VECTOR<UINT32> &vUser, VECTOR<INT64> &vGroup)
{
    TcxTreeListNode     *ParentNode;
    TcxTreeListNode     *Node;
    BOOL                bOnline;
    UINT32              ulUserID;
    UINT32              ulUserType;
    UINT32              ulFuncType;
    UINT32              ulStatus;
    UINT32              ulCellID;
    UINT32              ulCallStatus;
    CHAR                acUserName[64];

    Caption = Group::GetGroupTypeName(pstGroup->ulGroupType) + " - " + Group::GetGroupDisplayName(pstGroup->i64GroupID);

    tlMain->BeginUpdate();
    tlMain->Clear();

    ParentNode = tlMain->Add();
    ParentNode->ImageIndex = -1;
    ParentNode->Values[0] = "ÖÕ¶Ë";
    for (UINT32 i=0; i<vUser.size(); i++)
    {
        ulUserID = vUser[i];

        if (!User::GetUserInfo(ulUserID, acUserName, &ulUserType, &ulFuncType, &ulStatus, &ulCellID, &ulCallStatus))
        {
            continue;
        }

        Node = ParentNode->AddChild();

        bOnline = User::IsUserOnline(ulCallStatus);
        if (ulUserType == SMALL_PORTAL_UE ||
            ulUserType == LARGE_PORTAL_UE )
        {
            Node->ImageIndex = bOnline?1:2;
        }
        else if (ulUserType == STATION_UE)
        {
            Node->ImageIndex = bOnline?3:4;
        }
        else if (ulUserType == TRAIN_UE)
        {
            Node->ImageIndex = bOnline?5:6;
        }
        else if (ulUserType == DC_USER)
        {
            Node->ImageIndex = bOnline?7:8;
        }
        else
        {
            Node->ImageIndex = -1;
        }

        Node->Values[0] = User::GetUserDisplayName(ulUserID, acUserName);
    }

    if (vGroup.size() > 0)
    {
        ParentNode = tlMain->Add();
        ParentNode->ImageIndex = -1;
        ParentNode->Values[0] = "Èº×é";
        for (UINT32 i=0; i<vGroup.size(); i++)
        {
            Node = ParentNode->AddChild();
            Node->ImageIndex = 0;

            Node->Values[0] = Group::GetGroupDisplayName(vGroup[i]);
        }
    }

    tlMain->Root->Expand(true);
    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------

void __fastcall TDialogStaticGroupMember::FormCreate(TObject *Sender)
{
    SetImage(btnClose, IMG_CLOSE);
}
//---------------------------------------------------------------------------

