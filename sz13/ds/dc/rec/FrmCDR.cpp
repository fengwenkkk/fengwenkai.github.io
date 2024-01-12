//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmCDR.h"
#include "main.h"
#include "FrmP2PCallInfo.h"
#include "FrmGroupCallInfo.h"
#include "FrmEmergencyCallInfo.h"
#include "FrmRealBrdInfo.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxCustomData"
#pragma link "cxDropDownEdit"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxGroupBox"
#pragma link "cxInplaceContainer"
#pragma link "cxLabel"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxMemo"
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
#pragma link "MDIForm"
#pragma link "dxBar"
#pragma link "cxProgressBar"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxCalendar"
#pragma link "cxDateUtils"
#pragma link "cxMaskEdit"
#pragma link "dxCore"
#pragma link "cxPC"
#pragma link "cxSpinEdit"
#pragma link "cxTimeEdit"
#pragma link "dxBarBuiltInMenu"
#pragma link "cxClasses"
#pragma link "cxLocalization"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma resource "*.dfm"
TFormCDR *FormCDR;

//---------------------------------------------------------------------------
__fastcall TFormCDR::TFormCDR(TComponent* Owner)
    : TSubForm(Owner)
{
    pcMain->ActivePageIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormCDR::ShowForm(TForm *Form, TcxTabSheet *tsParent)
{
    Form->Parent = tsParent;
    Form->BorderStyle = bsNone;
    Form->Align = alClient;
    Form->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormCDR::TimerInitTimer(TObject *Sender)
{
    TimerInit->Enabled = false;

    ShowForm(FormP2PCallInfo, tsP2PCall);
    ShowForm(FormGroupCallInfo, tsGroupCall);
    ShowForm(FormRealBrdInfo, tsBrd);
}
//---------------------------------------------------------------------------

