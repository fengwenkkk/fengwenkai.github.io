#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "omc.h"
#include "ClientInit.h"
#include "main.h"
#include "FrmAlarm.h"
#include "FrmHistoryAlarm.h"
#include "FrmAlarmReport.h"
#include "DlgSetAlarmLevel.h"
#include "FrmActiveAlarm.h"
#include "FrmAlarmStat.h"
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
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
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
#pragma link "cxImageComboBox"
#pragma link "cxPC"
#pragma link "dxSkinscxPCPainter"
#pragma link "cxProgressBar"
#pragma link "cxClasses"
#pragma link "cxImageList"
#pragma link "dxBarBuiltInMenu"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxTrackBar"
#pragma link "dxCameraControl"
#pragma link "AdvSmoothToggleButton"
#pragma link "cxCheckBox"
#pragma link "dxToggleSwitch"
#pragma link "cxLabel"
#pragma link "AdvSmoothPanel"
#pragma link "cxScrollBox"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxMaskEdit"
#pragma link "cxRadioGroup"
#pragma link "cxCalendar"
#pragma link "cxDateUtils"
#pragma link "dxCore"
#pragma link "cxPropertiesStore"
#pragma resource "*.dfm"
TFormAlarm *FormAlarm;


__fastcall TFormAlarm::TFormAlarm(TComponent* Owner)
: TSubForm(Owner)
{
/*
	SetFormParent(FormActiveAlarm, tsActiveAlarm);
	SetFormParent(FormAlarmCfg, tsAlarmCfg);
	SetFormParent(FormHistoryAlarm, tsHistoryAlarm);
	SetFormParent(FormAlarmReport, tsAlarmReport);
*/
    FormActiveAlarm->MainPanel->Parent = tsActiveAlarm;
    FormHistoryAlarm->MainPanel->Parent = tsHistoryAlarm;
    FormAlarmReport->MainPanel->Parent = tsAlarmReport;
    pcMain->ActivePage = tsActiveAlarm;
}

void __fastcall TFormAlarm::TimerInitTimer(TObject *Sender)
{
	if (!TimerInit->Enabled)
    {
        return;
    }

    TimerInit->Enabled = false;

 //     pcMain->ActivePage = tsActiveAlarm;
}
//---------------------------------------------------------------------------
extern TFormAlarmStat      *g_FormHistoryAlarmStat;
void __fastcall TFormAlarm::pcMainPageChanging(TObject *Sender, TcxTabSheet *NewPage,
          bool &AllowChange)
{
	if (NewPage == tsAlarmReport)
	{
		FormAlarmReport->RefreshData();

		if (g_FormHistoryAlarmStat)
		{
			g_FormHistoryAlarmStat->chtMain->Visible = false;
			g_FormHistoryAlarmStat->chtMain->Visible = true;

			g_FormHistoryAlarmStat->SeriesStat->Marks->Visible = false;
			g_FormHistoryAlarmStat->SeriesStat->Marks->Visible = true;


		}
	}

}
//---------------------------------------------------------------------------

