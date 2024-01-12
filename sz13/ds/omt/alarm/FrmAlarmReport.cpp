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

#include "FrmAlarmReport.h"
#include "FrmAlarmStat.h"
#include "FrmAlarmTrend.h"
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
#pragma link "cxClasses"
#pragma resource "*.dfm"
TFormAlarmReport *FormAlarmReport;

extern TColor GetSkinColor(AnsiString strSkinName);

TFormAlarmStat      *g_FormActiveAlarmStat = NULL;
TFormAlarmTrend     *g_FormActiveAlarmTrend = NULL;

TFormAlarmStat      *g_FormHistoryAlarmStat = NULL;
TFormAlarmTrend     *g_FormHistoryAlarmTrend = NULL;

extern void InitChart(TChart *cht);
//---------------------------------------------------------------------------
__fastcall TFormAlarmReport::TFormAlarmReport(TComponent* Owner)
    : TSubForm(Owner)
{
	Application->CreateForm(__classid(TFormAlarmStat), &g_FormActiveAlarmStat);
    Application->CreateForm(__classid(TFormAlarmTrend), &g_FormActiveAlarmTrend);
	Application->CreateForm(__classid(TFormAlarmStat), &g_FormHistoryAlarmStat);
    Application->CreateForm(__classid(TFormAlarmTrend), &g_FormHistoryAlarmTrend);

	InitMsgHandler(EV_OMT_OMC_RSP, MSG_OMT_GetAlarmStat, OnGetAlarmStatRsp);
}
//---------------------------------------------------------------------------
void __fastcall TFormAlarmReport::TimerInitTimer(TObject *Sender)
{
    if (!TimerInit->Enabled)
    {
        return;
	}

	if (g_FormActiveAlarmStat->MainPanel->Parent != gbMain)
    {
		g_FormActiveAlarmStat->MainPanel->Parent = gbMain;
        g_FormActiveAlarmStat->MainPanel->Align = alNone;

		g_FormActiveAlarmTrend->MainPanel->Parent = gbMain;
        g_FormActiveAlarmTrend->MainPanel->Align = alNone;

        g_FormHistoryAlarmStat->MainPanel->Parent = gbMain;
		g_FormHistoryAlarmStat->MainPanel->Align = alNone;

		g_FormHistoryAlarmTrend->MainPanel->Parent = gbMain;
        g_FormHistoryAlarmTrend->MainPanel->Align = alNone;

		gbMainResize(this);
	}
	else
	{
		RefreshData();
		TimerInit->Enabled = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormAlarmReport::gbMainResize(TObject *Sender)
{
    int     iLeftWidth = gbMain->Width*0.4;
    int     iTopHeight = gbMain->Height/2;
    int     iSkip = 10;

	g_FormActiveAlarmStat->MainPanel->Left = iSkip;
	g_FormActiveAlarmStat->MainPanel->Top = iSkip;
	g_FormActiveAlarmStat->MainPanel->Width = iLeftWidth;
	g_FormActiveAlarmStat->MainPanel->Height = iTopHeight;

    g_FormActiveAlarmTrend->MainPanel->Left = iLeftWidth+2*iSkip;
    g_FormActiveAlarmTrend->MainPanel->Top = iSkip;
	g_FormActiveAlarmTrend->MainPanel->Width = gbMain->Width - (iLeftWidth+3*iSkip+1);
	g_FormActiveAlarmTrend->MainPanel->Height = iTopHeight;

	g_FormHistoryAlarmStat->MainPanel->Left = iSkip;
    g_FormHistoryAlarmStat->MainPanel->Top = 2*iSkip + iTopHeight;
    g_FormHistoryAlarmStat->MainPanel->Width = iLeftWidth;
    g_FormHistoryAlarmStat->MainPanel->Height = gbMain->Height - (iTopHeight+3*iSkip+1);

	g_FormHistoryAlarmTrend->MainPanel->Left = g_FormActiveAlarmTrend->MainPanel->Left;
    g_FormHistoryAlarmTrend->MainPanel->Top = g_FormHistoryAlarmStat->MainPanel->Top;
    g_FormHistoryAlarmTrend->MainPanel->Width = g_FormActiveAlarmTrend->MainPanel->Width;
    g_FormHistoryAlarmTrend->MainPanel->Height = g_FormHistoryAlarmStat->MainPanel->Height;
}
//---------------------------------------------------------------------------
void __fastcall TFormAlarmReport::TimerCheckTimer(TObject *Sender)
{
	RefreshData();
	// 三分钟获取一次告警统计信息
	TimerCheck->Interval = 3*60*1000;
}
//---------------------------------------------------------------------------
void __fastcall TFormAlarmReport::RefreshData()
{
	SendServer(EV_OMT_OMC_REQ, MSG_OMT_GetAlarmStat);

    TimerCheck->Enabled = false;
	TimerCheck->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormAlarmReport::OnGetAlarmStatRsp(UINT32 ulSeqID, GJson &Json)
{
	CHAR            *szValue;
	GJson           *pJson;
	GJson           SubJson;
    VECTOR<GJson*>  vJson;
    OMC_ALARM_STAT_T    stActiveAlarmStat   = {0};
    OMC_ALARM_TREND_T   stActiveAlarmTrend  = {0};
    OMC_ALARM_STAT_T    stHistoryAlarmStat  = {0};
    OMC_ALARM_TREND_T   stHistoryAlarmTrend = {0};

    szValue = Json.GetValue("ActiveAlarmStat");
    SubJson.Parse(szValue);

    if (!SubJson.GetValue("CriticalAlarm", &stActiveAlarmStat.ulCriticalAlarmNum) ||
        !SubJson.GetValue("MainAlarm",     &stActiveAlarmStat.ulMainAlarmNum)     ||
        !SubJson.GetValue("MinorAlarm",    &stActiveAlarmStat.ulMinorAlarmNum)    ||
        !SubJson.GetValue("NormalAlarm",   &stActiveAlarmStat.ulNormalAlarmNum)  )
    {
		GosLog(LOG_ERROR, "parse active alarm stat failed on %s", Json.GetText());
        return;
    }

    szValue = Json.GetValue("ActiveAlarmTrend");
    SubJson.Parse(szValue);
    SubJson.GetSubJson(vJson);
    stActiveAlarmTrend.ulNum = vJson.size();
    for (UINT32 i=0; i<vJson.size(); i++)
    {
        pJson = vJson[i];

        if (!pJson->GetValue("Time",          &stActiveAlarmTrend.astAlarmTrend[i].ulTime) ||
            !pJson->GetValue("CriticalAlarm", &stActiveAlarmTrend.astAlarmTrend[i].ulCriticalAlarmNum) ||
            !pJson->GetValue("MainAlarm",     &stActiveAlarmTrend.astAlarmTrend[i].ulMainAlarmNum)  ||
            !pJson->GetValue("MinorAlarm",    &stActiveAlarmTrend.astAlarmTrend[i].ulMinorAlarmNum) ||
            !pJson->GetValue("NormalAlarm",   &stActiveAlarmTrend.astAlarmTrend[i].ulNormalAlarmNum)  )
        {
            GosLog(LOG_ERROR, "parse active alarm trend failed on %s", Json.GetText());
            return;
        }
    }

	g_FormActiveAlarmStat->Init(stActiveAlarmStat, "近1天之内活跃告警统计");
	g_FormActiveAlarmTrend->Init(stActiveAlarmTrend, true, "近1天之内活跃告警走势");

	// history alarm
    szValue = Json.GetValue("HistoryAlarmStat");
	SubJson.Parse(szValue);
	if (!SubJson.GetValue("CriticalAlarm", &stHistoryAlarmStat.ulCriticalAlarmNum) ||
        !SubJson.GetValue("MainAlarm",     &stHistoryAlarmStat.ulMainAlarmNum)  ||
        !SubJson.GetValue("MinorAlarm",    &stHistoryAlarmStat.ulMinorAlarmNum) ||
        !SubJson.GetValue("NormalAlarm",   &stHistoryAlarmStat.ulNormalAlarmNum)  )
    {
        GosLog(LOG_ERROR, "parse history alarm stat failed on %s", Json.GetText());
        return;
    }

	szValue = Json.GetValue("HistoryAlarmTrend");
    SubJson.Parse(szValue);
    SubJson.GetSubJson(vJson);

    stHistoryAlarmTrend.ulNum = vJson.size();
	for (UINT32 i=0; i<vJson.size(); i++)
    {
		pJson = vJson[i];

		if (!pJson->GetValue("Time", 		  &stHistoryAlarmTrend.astAlarmTrend[i].ulTime) ||
            !pJson->GetValue("CriticalAlarm", &stHistoryAlarmTrend.astAlarmTrend[i].ulCriticalAlarmNum) ||
			!pJson->GetValue("MainAlarm",     &stHistoryAlarmTrend.astAlarmTrend[i].ulMainAlarmNum)  ||
            !pJson->GetValue("MinorAlarm",    &stHistoryAlarmTrend.astAlarmTrend[i].ulMinorAlarmNum) ||
            !pJson->GetValue("NormalAlarm",   &stHistoryAlarmTrend.astAlarmTrend[i].ulNormalAlarmNum)  )
        {
            GosLog(LOG_ERROR, "parse history alarm trend failed on %s", Json.GetText());
            return;
		}
    }

	g_FormHistoryAlarmStat->Init(stHistoryAlarmStat, "近90天内历史告警统计");
	g_FormHistoryAlarmTrend->Init(stHistoryAlarmTrend, false, "近90天内历史告警走势");
}
//---------------------------------------------------------------------------
void __fastcall TFormAlarmReport::gbMainMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    if (!(Button & mbRight))
    {
        return;
    }

	pmMain->PopupFromCursorPos();
}
//---------------------------------------------------------------------------


