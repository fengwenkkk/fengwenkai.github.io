//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "DlgSetPwd.h"
#include "Main.h"
#include "DlgLock.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxControls"
#pragma link "cxGraphics"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxSplitter"
#pragma link "dxSkinBlue"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "cxCustomData"
#pragma link "cxInplaceContainer"
#pragma link "cxStyles"
#pragma link "cxTextEdit"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "cxCustomData"
#pragma link "cxInplaceContainer"
#pragma link "cxStyles"
#pragma link "cxTextEdit"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "cxClasses"
#pragma link "cxCheckBox"
#pragma link "cxCheckGroup"
#pragma link "cxDropDownEdit"
#pragma link "cxEdit"
#pragma link "cxVGrid"
#pragma link "dxBar"
#pragma link "cxRadioGroup"
#pragma link "cxMaskEdit"
#pragma link "cxContainer"
#pragma link "cxGroupBox"
#pragma link "cxEditRepositoryItems"
#pragma link "cxExtEditRepositoryItems"
#pragma link "cxCheckComboBox"
#pragma link "cxLabel"
#pragma link "cxSpinEdit"
#pragma link "cxPC"
#pragma link "dxSkinscxPCPainter"
#pragma link "cxButtons"
#pragma link "cxFontNameComboBox"
#pragma link "dxLayoutContainer"
#pragma link "dxLayoutControl"
#pragma link "dxLayoutControlAdapters"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "dxBevel"
#pragma link "cxImage"
#pragma link "dxGDIPlusClasses"
#pragma resource "*.dfm"

TDialogSetPwd *DialogSetPwd;

void __fastcall TDialogSetPwd::InitSkin()
{
}
//---------------------------------------------------------------------------
__fastcall TDialogSetPwd::TDialogSetPwd(TComponent* Owner)
    : TDialogForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetPwd::Init()
{
    edtUser->Text = APP::GetDCLoginAccount();
    edtOldPwd->Text = "";
    edtNewPwd->Text = "";
    edtNewPwd2->Text = "";

    edtOldPwd->Style->Font->Name = "Tahoma";
    edtNewPwd->Style->Font->Name = "Tahoma";
    edtNewPwd2->Style->Font->Name = "Tahoma";
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetPwd::btnOKClick(TObject *Sender)
{
    AnsiString  strUser = edtUser->Text.Trim();
    AnsiString  strOldPwd = edtOldPwd->Text.Trim();
    AnsiString  strNewPwd = edtNewPwd->Text.Trim();
    AnsiString  strNewPwd2 = edtNewPwd2->Text.Trim();

    ModalResult = mrNone;

    if (strOldPwd.IsEmpty())
    {
        InfoMsg("�����������");
        edtOldPwd->SetFocus();
        return;
    }

    if (strNewPwd.IsEmpty())
    {
        InfoMsg("������������");
        edtNewPwd->SetFocus();
        return;
    }

    if (strNewPwd.Length() < 4)
    {
        InfoMsg("���볤������Ϊ4λ");
        edtNewPwd->SetFocus();
        return;
    }

    if (strNewPwd2 != strNewPwd)
    {
        InfoMsg("��������������벻ͬ");
        edtNewPwd2->SetFocus();
        return;
    }

    GJsonParam  ReqJsonParam;

    ReqJsonParam.Add("User", strUser);
    ReqJsonParam.Add("OldPwd", strOldPwd);
    ReqJsonParam.Add("NewPwd", strNewPwd);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCSetPwd, ReqJsonParam))
    {
        ErrMsg("����ʧ��");
        return;
    }

    InfoMsg("�����ɹ�");

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetPwd::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetPwd::FormClose(TObject *Sender, TCloseAction &Action)
{
    btnCancelClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetPwd::FormActivate(TObject *Sender)
{
    Init();
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetPwd::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------

