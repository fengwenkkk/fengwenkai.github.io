//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "DlgNewPatchGroup.h"
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
#pragma link "dxBevel"
#pragma resource "*.dfm"
TDialogNewPatchGroup *DialogNewPatchGroup;
//---------------------------------------------------------------------------
__fastcall TDialogNewPatchGroup::TDialogNewPatchGroup(TComponent* Owner)
    : TDialogForm(Owner)
{
    Init();
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewPatchGroup::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewPatchGroup::Init()
{
    edtName->Text = "";
}
//---------------------------------------------------------------------------
void __fastcall TDialogNewPatchGroup::btnOKClick(TObject *Sender)
{
    AnsiString  strName = edtName->Text.Trim();

    if (strName.Length() == 0)
    {
        ErrMsg("名称不能为空");
        edtName->Focused();
        ModalResult = mrNone;
        return;
    }

    if (strName.Length() > 31)
    {
        ErrMsg("名称长度超出范围");
        edtName->Focused();
        ModalResult = mrNone;
        return;
    }

    char    *szGroupName = strName.c_str();

    if (GetPatchGroupID(szGroupName) != INVALID_GROUP_ID)
    {
        ErrMsg("名称已经存在");
        edtName->Focused();
        ModalResult = mrNone;
        return;
    }

    m_strGroupName = szGroupName;

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TDialogNewPatchGroup::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------
