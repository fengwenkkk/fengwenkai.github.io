//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "DlgVideoCallCfg.h"
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
#pragma link "cxRadioGroup"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "dxBevel"
#pragma resource "*.dfm"
TDialogVideoCallCfg *DialogVideoCallCfg;


//---------------------------------------------------------------------------
__fastcall TDialogVideoCallCfg::TDialogVideoCallCfg(TComponent* Owner)
    : TDialogForm(Owner)
{
    g_DictVideoFormat.Init(cbVideoFormat);
    g_DictVideoFormat.SetID(cbVideoFormat, VIDEO_FMT_D1);

    Init();
}
//---------------------------------------------------------------------------
void __fastcall TDialogVideoCallCfg::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogVideoCallCfg::Init()
{
    ModalResult = mrCancel;
}
void __fastcall TDialogVideoCallCfg::GetVideoCfg(UINT32 &ulVideoFormat, UINT32 &ulCameraType, BOOL &bNeedConfirm, BOOL &bHasVoice)
{
    g_DictVideoFormat.GetID(cbVideoFormat, &ulVideoFormat);
    ulCameraType = rbFrontCamera->Checked?CAMERA_TYPE_FRONT:CAMERA_TYPE_BACK;
    bNeedConfirm = cbNeedConfirm->Checked;
    bHasVoice = cbHasVoice->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TDialogVideoCallCfg::btnOKClick(TObject *Sender)
{
    if (cbVideoFormat->ItemIndex < 0)
    {
        ErrMsg("视频格式未选择");
        cbVideoFormat->Focused();
        ModalResult = mrNone;
        return;
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TDialogVideoCallCfg::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------

