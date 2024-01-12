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

#include "FrmAlarmTrend.h"
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
#pragma link "cxClasses"
#pragma link "cxHint"
#pragma link "dxCustomHint"
#pragma link "dxScreenTip"
#pragma resource "*.dfm"
TFormAlarmTrend *FormAlarmTrend;

extern TColor GetAlarmColor(UINT32 ulAlarmLevel);
extern AnsiString GetAlarmLevelText(UINT32 ulAlarmLevel);
extern void InitChart(TChart *cht);

// 3->10, 30->30, 34->40, 123 -> 200, 1234 -> 2000
UINT32 GetCeilNum(UINT32 d)
{
    int     iNum = 0;
    char    acNum[32];
    UINT32  ulLen = sprintf(acNum, "%u", d);

    if (ulLen <= 1)
    {
        return 10;
    }

    UINT32      ulFirst = acNum[0] - '0';
    UINT32      ulNew;

    ulNew = ulFirst * powf(10, ulLen-1);
    if (ulNew == d)
    {
        return d;
    }

    ulNew = (ulFirst+1) * powf(10, ulLen-1);

    return ulNew;
}

//---------------------------------------------------------------------------
__fastcall TFormAlarmTrend::TFormAlarmTrend(TComponent* Owner)
    : TSubForm(Owner)
{
    InitChart(chtMain);
    chtMain->Title->Visible = false;

    SeriesCriticalAlarm->Color = GetAlarmColor(ALARM_LEVEL_CRITICAL);
    SeriesMainAlarm->Color = GetAlarmColor(ALARM_LEVEL_MAIN);
    SeriesMinorAlarm->Color = GetAlarmColor(ALARM_LEVEL_MINOR);
    SeriesNormalAlarm->Color = GetAlarmColor(ALARM_LEVEL_NORMAL);
}
//---------------------------------------------------------------------------
void __fastcall TFormAlarmTrend::Init(OMC_ALARM_TREND_T &stTrend, bool bHour, AnsiString strTitle)
{
    m_bHour = bHour;
    gbMain->Caption = strTitle;
    memcpy(&m_stTrend, &stTrend, sizeof(m_stTrend));

    for (int i=0; i<chtMain->SeriesCount(); i++)
    {
        chtMain->Series[i]->BeginUpdate();
        chtMain->Series[i]->Clear();
    }

    CHAR        acTime[32];
    CHAR        *szTime = acTime+8;

    for (UINT32 i=0; i<stTrend.ulNum; i++)
    {
        OMC_ALARM_STAT_T    *pstStat = &stTrend.astAlarmTrend[i];

        if (m_bHour)
        {
            gos_get_text_time_ex(&pstStat->ulTime, "%04u%02u%02u%u", acTime);
            szTime = acTime+8;
        }
        else
        {
            gos_get_text_time_ex(&pstStat->ulTime, "%04u%u-%u", acTime);
            szTime = acTime+4;
        }

        SeriesCriticalAlarm->AddXY(i, pstStat->ulCriticalAlarmNum, szTime);
        SeriesMainAlarm->AddXY(i, pstStat->ulMainAlarmNum, szTime);
        SeriesMinorAlarm->AddXY(i, pstStat->ulMinorAlarmNum, szTime);
        SeriesNormalAlarm->AddXY(i, pstStat->ulNormalAlarmNum, szTime);
    }

    double dMax = 0;

    for (int i=0; i<chtMain->SeriesCount(); i++)
    {
        if (dMax < chtMain->Series[i]->MaxYValue())
        {
            dMax = chtMain->Series[i]->MaxYValue();
        }

        chtMain->Series[i]->EndUpdate();
    }

    if (dMax < 10)
    {
        dMax = 10;
    }
    else
    {
        dMax = GetCeilNum(dMax);
    }

    UINT32  ulIncNum = 10;
    double  dStep = dMax/ulIncNum;

    dMax = ceil(dStep)*ulIncNum;

    SetChartAxisRange(chtMain->LeftAxis, 0, dMax);
    SetChartAxisIncrement(chtMain->LeftAxis, ulIncNum);
}
//---------------------------------------------------------------------------
void __fastcall TFormAlarmTrend::chtMainClickSeries(TCustomChart *Sender, TChartSeries *Series,
          int ValueIndex, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (ValueIndex < 0 || ValueIndex >= (int)m_stTrend.ulNum)
    {
        return;
    }

    OMC_ALARM_STAT_T    *pstStat = &m_stTrend.astAlarmTrend[ValueIndex];
    CHAR                acTime[128];
    AnsiString          strHint;

    if (m_bHour)
    {
        gos_get_text_time_ex(&pstStat->ulTime, "%04u%02u%02u%u点", acTime);
        strHint = "告警时间：" + AnsiString(acTime+8);
    }
    else
    {
        gos_get_text_time_ex(&pstStat->ulTime, "%04u%u月%u日", acTime);
        strHint = "告警日期：" + AnsiString(acTime+4);
    }

    strHint += "\n" + GetAlarmLevelText(ALARM_LEVEL_CRITICAL) + "：" + pstStat->ulCriticalAlarmNum;
    strHint += "\n" + GetAlarmLevelText(ALARM_LEVEL_MAIN) + "：" + pstStat->ulMainAlarmNum;
    strHint += "\n" + GetAlarmLevelText(ALARM_LEVEL_MINOR) + "：" + pstStat->ulMinorAlarmNum;
    strHint += "\n" + GetAlarmLevelText(ALARM_LEVEL_NORMAL) + "：" + pstStat->ulNormalAlarmNum;

    chtMain->Hint = strHint;
    chtMain->ShowHint = true;
}
//---------------------------------------------------------------------------


