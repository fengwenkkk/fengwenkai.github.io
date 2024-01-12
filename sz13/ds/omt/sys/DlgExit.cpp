//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "DlgExit.h"
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

TDialogExit *DialogExit;

extern BOOL g_bSilentExit;
extern void OMTRelogin();

void __fastcall TDialogExit::InitSkin()
{
    this->Font->Name = "微软雅黑";
    this->Font->Size = 12;
}
//---------------------------------------------------------------------------
__fastcall TDialogExit::TDialogExit(TComponent* Owner)
    : TDialogForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TDialogExit::Init()
{
}
//---------------------------------------------------------------------------
void __fastcall TDialogExit::btnOKClick(TObject *Sender)
{
    ModalResult = mrOk;

    if (rbExit->Checked)  //退出客服端
    {
        this->Visible = false;
        g_bSilentExit = true;
        FormMain->Exit();
    }
    else if (rbLogout->Checked)  //注销操作，回到登录界面
    {
        g_bSilentExit = true;
        this->Visible = false;
        FormMain->Visible = false;

        OMTRelogin();
        FormMain->Exit();
    }
    else   //锁屏
    {
        this->Visible = false;

        FormMain->LockWindow();
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogExit::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogExit::FormClose(TObject *Sender, TCloseAction &Action)
{
    btnCancelClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TDialogExit::FormActivate(TObject *Sender)
{
    Init();
}
//---------------------------------------------------------------------------
void __fastcall TDialogExit::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------

