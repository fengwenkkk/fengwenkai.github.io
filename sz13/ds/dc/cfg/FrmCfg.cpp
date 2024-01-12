//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"

#include "FrmCfg.h"
#include "FrmDCUserCfg.h"
#include "FrmDCOperatorCfg.h"
#include "FrmDepotCfg.h"
#include "FrmStationCfg.h"
#include "FrmStationUECfg.h"
#include "FrmPortalUECfg.h"
#include "FrmBrdZoneCfg.h"
#include "FrmTrainCfg.h"
#include "FrmTrainUECfg.h"

#include "FrmPredefinedSMSCfg.h"
#include "FrmRecBrdCfg.h"
#include "FrmGroupCfg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxControls"
#pragma link "cxCustomData"
#pragma link "cxDropDownEdit"
#pragma link "cxGraphics"
#pragma link "cxInplaceContainer"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
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
#pragma link "cxContainer"
#pragma link "cxEdit"
#pragma link "cxGroupBox"
#pragma link "cxButtons"
#pragma link "cxPC"
#pragma link "dxSkinscxPCPainter"
#pragma link "cxRadioGroup"
#pragma link "cxCheckBox"
#pragma link "cxImageComboBox"
#pragma link "cxMemo"
#pragma link "cxLabel"
#pragma link "cxTrackBar"
#pragma link "cxMaskEdit"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "dxBarBuiltInMenu"
#pragma link "cxClasses"
#pragma link "dxBar"
#pragma resource "*.dfm"

TFormCfg *FormCfg;

void __fastcall TFormCfg::InitSkin()
{
}
//---------------------------------------------------------------------------
__fastcall TFormCfg::TFormCfg(TComponent* Owner)
    : TSubForm(Owner)
{
    TcxButton *btn[] = {btnDCUser,  btnDCOperator, btnDepot, btnStation, btnStationUE,
                        btnPortalUE, btnBrdZone, btnTrain,   btnTrainUE, btnPredefinedSMS,
                        btnRecBrdCfg, btnGroupCfg};
    int         iTop = 30;

    for (UINT32 i=0; i<ARRAY_SIZE(btn); i++)
    {
        if (btn[i]->Visible)
        {
            btn[i]->Top = iTop;
            iTop += 40;
        }

        btn[i]->Width = 120;
        btn[i]->Left = (btn[i]->Parent->Width - btn[i]->Width)/2;
        btn[i]->TabOrder = i;
        btn[i]->GroupIndex = 1;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::ShowForm(TForm *Form)
{
    if (Form->Parent != gbMain)
    {
        Form->Parent = gbMain;
        Form->BorderStyle = bsNone;
        Form->Align = alClient;
        Form->Visible = true;
    }

    Form->BringToFront();
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::TimerInitTimer(TObject *Sender)
{
    TimerInit->Enabled = false;
    btnDCUserClick(this);
    btnDCUser->SpeedButtonOptions->Down = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::btnDCUserClick(TObject *Sender)
{
    ShowForm(FormDCUser);
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::btnDepotClick(TObject *Sender)
{
    ShowForm(FormDepotCfg);
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::btnStationClick(TObject *Sender)
{
    ShowForm(FormStationCfg);
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::btnStationUEClick(TObject *Sender)
{
    ShowForm(FormStationUECfg);
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::btnPortalUEClick(TObject *Sender)
{
    ShowForm(FormPortalUECfg);
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::btnBrdZoneClick(TObject *Sender)
{
    ShowForm(FormBrdZoneCfg);
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::btnTrainClick(TObject *Sender)
{
    ShowForm(FormTrainCfg);
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::btnTrainUEClick(TObject *Sender)
{
    ShowForm(FormTrainUECfg);
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::btnPredefinedSMSClick(TObject *Sender)
{
    ShowForm(FormPredefinedSMSCfg);
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::btnDCOperatorClick(TObject *Sender)
{
    ShowForm(FormDCOperatorCfg);
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::btnRecBrdCfgClick(TObject *Sender)
{
    ShowForm(FormRecBrdCfg);
}
//---------------------------------------------------------------------------
void __fastcall TFormCfg::btnGroupCfgClick(TObject *Sender)
{
    ShowForm(FormGroupCfg);
}
//---------------------------------------------------------------------------

