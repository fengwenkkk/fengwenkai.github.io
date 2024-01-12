//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"

#include "MsgDialog.h"
#include "msg.h"
#include "ds_public.h"
#include "ClientUtil.h"
#include "Resource.h"
#include "ClientCommon.h"
#include "RemoteMsg.h"
#include "ClientInit.h"

#include "FrmAlarmStat.h"
#include "FrmNeInfo.h"
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
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxImageComboBox"
#pragma link "cxImageList"
#pragma link "cxCheckBox"
#pragma link "cxCalendar"
#pragma link "cxDateUtils"
#pragma link "cxMaskEdit"
#pragma link "dxCore"
#pragma link "cxSplitter"
#pragma resource "*.dfm"
TFormAlarmStat *FormAlarmStat;

extern TColor GetAlarmColor(UINT32 ulAlarmLevel);
extern AnsiString GetAlarmLevelText(UINT32 ulAlarmLevel);
extern void InitChart(TChart *cht);

//---------------------------------------------------------------------------
__fastcall TFormAlarmStat::TFormAlarmStat(TComponent* Owner)
    : TSubForm(Owner)
{
    InitChart(chtMain);

    chtMain->Title->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormAlarmStat::Init(OMC_ALARM_STAT_T stStat, AnsiString strTitle)
{
	gbMain->Caption = strTitle;
	AnsiString  strHint = "";

	SeriesStat->BeginUpdate();

	SeriesStat->Clear();

	SeriesStat->AddPie(stStat.ulCriticalAlarmNum, GetAlarmLevelText(ALARM_LEVEL_CRITICAL), GetAlarmColor(ALARM_LEVEL_CRITICAL));
	SeriesStat->AddPie(stStat.ulMainAlarmNum, GetAlarmLevelText(ALARM_LEVEL_MAIN), GetAlarmColor(ALARM_LEVEL_MAIN));
	SeriesStat->AddPie(stStat.ulMinorAlarmNum, GetAlarmLevelText(ALARM_LEVEL_MINOR), GetAlarmColor(ALARM_LEVEL_MINOR));
	SeriesStat->AddPie(stStat.ulNormalAlarmNum, GetAlarmLevelText(ALARM_LEVEL_NORMAL), GetAlarmColor(ALARM_LEVEL_NORMAL));

	SeriesStat->EndUpdate();

	//SeriesStat->Marks->Visible = false;
	//SeriesStat->Marks->Visible = true;
}
//---------------------------------------------------------------------------

