//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmLocalCfg.h"
#include "main.h"

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
#pragma link "dxSkinDarkRoom"
#pragma link "dxSkinDarkSide"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinLilian"
#pragma link "dxSkinMcSkin"
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
#pragma link "cxScrollBox"
#pragma link "cxCheckBox"
#pragma link "cxRadioGroup"
#pragma resource "*.dfm"
TFormLocalCfg *FormLocalCfg;


extern VOID SaveLocalConfInfo();

void __fastcall TFormLocalCfg::Init(bool bInit)
{
    UINT32              i;

}
//---------------------------------------------------------------------------
__fastcall TFormLocalCfg::TFormLocalCfg(TComponent* Owner)
    : TSubForm(Owner)
{
    Init(true);

  //    InitWorkPanel();

    pcMain->ActivePageIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::FormCreate(TObject *Sender)
{
//  SetImage(btnSetBasic,    IMG_SET);
//  SetImage(btnCancelBasic, IMG_CANCEL);
//  SetImage(btnRefresh,     IMG_REFRESH);
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::pcMainPageChanging(TObject *Sender, TcxTabSheet *NewPage,
          bool &AllowChange)
{
/*  if (NewPage == tsSMS)
    {
        FormPredefinedSMSCfg->Parent = NewPage;
        FormPredefinedSMSCfg->BorderStyle = bsNone;
        FormPredefinedSMSCfg->Align = alClient;

        FormPredefinedSMSCfg->gbMain->PanelStyle->Active = true;

        FormPredefinedSMSCfg->Visible = true;
    }   */
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::gbBasicCfgMainResize(TObject *Sender)
{
 /* int  iLeft = (gbBasicCfg->Parent->Width - gbBasicCfg->Width)/2;
    int  iTop  = (gbBasicCfg->Parent->Height - gbBasicCfg->Height)/2;

    gbBasicCfg->Left = iLeft<0?0:iLeft;
    gbBasicCfg->Top  = iTop<0?0:iTop;    */
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::btnDefaultParamClick(TObject *Sender)
{
    btnSetParamClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::btnSetParamClick(TObject *Sender)
{
//  g_bIPHRecMergeMode = cbIPHRecMergeMode->Checked;

    SaveLocalConfInfo();
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::btnCancelParamClick(TObject *Sender)
{
    Init();
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::gbServiceCfgMainResize(TObject *Sender)
{
    int  iLeft = (gbServiceCfg->Parent->Width - gbServiceCfg->Width)/2;
    int  iTop  = (gbServiceCfg->Parent->Height - gbServiceCfg->Height)/2;

    gbServiceCfg->Left = iLeft<0?0:iLeft;
    gbServiceCfg->Top  = iTop<0?0:iTop;
}
//---------------------------------------------------------------------------

