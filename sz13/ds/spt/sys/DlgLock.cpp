//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "DlgLock.h"
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
#pragma link "dxSkinDarkRoom"
#pragma link "dxSkinDarkSide"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinLilian"
#pragma link "dxSkinMcSkin"
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

TDialogLock *DialogLock;

extern BOOL g_bSilentExit;
extern AnsiString ClientLogin(CHAR *szUser, CHAR *szPwd);

void __fastcall TDialogLock::InitSkin()
{
//  lbUser->LookAndFeel->SkinName = "Blue";
//  lbPwd->LookAndFeel->SkinName = "Blue";
    lbUser->Font->Size = 12;
    lbUser->Font->Name = "Î¢ÈíÑÅºÚ";
    lbUser->Font->Color = (TColor)RGB(0x61, 0x40, 0x30);
    lbPwd->Font->Size = 12;
    lbPwd->Font->Name = "Î¢ÈíÑÅºÚ";
    lbPwd->Font->Color = (TColor)RGB(0x61, 0x40, 0x30);

    edtUser->Style->LookAndFeel->SkinName = "Blue";
    edtPwd->Style->LookAndFeel->SkinName = "Blue";
}
//---------------------------------------------------------------------------
__fastcall TDialogLock::TDialogLock(TComponent* Owner)
    : TForm(Owner)
{
    Caption = "½âËøÂ¼ÒôÂ¼ÏñÖÕ¶Ë";
}
//---------------------------------------------------------------------------
void __fastcall TDialogLock::Init()
{
    edtUser->Text = g_strDCOperatorID;
    edtPwd->Text = "";
}
//---------------------------------------------------------------------------
void __fastcall TDialogLock::btnOKClick(TObject *Sender)
{
    AnsiString  strUser = edtUser->Text.Trim();
    AnsiString  strPwd = edtPwd->Text.Trim();
    AnsiString  strErrInfo = ClientLogin(strUser.c_str(), strPwd.c_str());

    if (!strErrInfo.IsEmpty())
    {
        InfoMsg("½âËøÊ§°Ü");
        ModalResult = mrNone;
        return;
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogLock::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogLock::FormClose(TObject *Sender, TCloseAction &Action)
{
    btnCancelClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TDialogLock::FormActivate(TObject *Sender)
{
    Init();
}
//---------------------------------------------------------------------------
void __fastcall TDialogLock::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------
void __fastcall TDialogLock::btnLogoutClick(TObject *Sender)
{
    g_bSilentExit = true;
    this->Visible = false;
    FormMain->Visible = false;

    AppRelogin();

    FormMain->Exit();
}
//---------------------------------------------------------------------------
void __fastcall TDialogLock::TimerCheckTimer(TObject *Sender)
{
    BringToFront();

//    ::SetWindowPos(Handle,HWND_TOPMOST,Left,Top,Width,Height,SWP_NOMOVE);
}
//---------------------------------------------------------------------------

