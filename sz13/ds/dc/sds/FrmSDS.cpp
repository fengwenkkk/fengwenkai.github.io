//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"

#include "FrmSDS.h"
#include "FrmSDSManage.h"
#include "FrmSDSTemplate.h"
#include "FrmLatestSDS.h"
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

TFormSDS *FormSDS;

void __fastcall TFormSDS::InitSkin()
{
}
//---------------------------------------------------------------------------
__fastcall TFormSDS::TFormSDS(TComponent* Owner)
    : TSubForm(Owner)
{
    pcMain->ActivePageIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormSDS::ShowForm(TForm *Form, TWinControl *Parent)
{
    Form->Parent = Parent;
    Form->BorderStyle = bsNone;
    Form->Align = alClient;
    Form->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormSDS::TimerInitTimer(TObject *Sender)
{
    TimerInit->Enabled = false;

    ShowForm(FormLatestSDS, tsLatestSDS);
    ShowForm(FormSDSTemplate, tsSDSTemplate);

    if (IsSimpleSMSMode())
    {
        tsLatestSDS->Caption = "当日短信";
        tsSDSTemplate->Caption = "短信模板";
    }
}
//---------------------------------------------------------------------------

