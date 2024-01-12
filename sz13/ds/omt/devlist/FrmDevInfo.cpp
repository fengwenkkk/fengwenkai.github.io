//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "omc.h"
#include "ClientInit.h"
#include "FrmActiveAlarm.h"
#include "FrmAlarmCfg.h"
#include "FrmDevInfo.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxControls"
#pragma link "cxCustomData"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxGraphics"
#pragma link "cxInplaceContainer"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxStyles"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "dxSkinBlack"
#pragma link "dxSkinBlue"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinCaramel"
#pragma link "dxSkinCoffee"
#pragma link "dxSkinDarkRoom"
#pragma link "dxSkinDarkSide"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinDevExpressStyle"
#pragma link "dxSkinFoggy"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinHighContrast"
#pragma link "dxSkiniMaginary"
#pragma link "dxSkinLilian"
#pragma link "dxSkinLiquidSky"
#pragma link "dxSkinLondonLiquidSky"
#pragma link "dxSkinMcSkin"
#pragma link "dxSkinMetropolis"
#pragma link "dxSkinMetropolisDark"
#pragma link "dxSkinMoneyTwins"
#pragma link "dxSkinOffice2007Black"
#pragma link "dxSkinOffice2007Blue"
#pragma link "dxSkinOffice2007Green"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinOffice2007Silver"
#pragma link "dxSkinOffice2010Black"
#pragma link "dxSkinOffice2010Blue"
#pragma link "dxSkinOffice2010Silver"
#pragma link "dxSkinOffice2013DarkGray"
#pragma link "dxSkinOffice2013LightGray"
#pragma link "dxSkinOffice2013White"
#pragma link "dxSkinOffice2016Colorful"
#pragma link "dxSkinOffice2016Dark"
#pragma link "dxSkinPumpkin"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSeven"
#pragma link "dxSkinSevenClassic"
#pragma link "dxSkinSharp"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinSilver"
#pragma link "dxSkinSpringTime"
#pragma link "dxSkinStardust"
#pragma link "dxSkinSummer2008"
#pragma link "dxSkinTheAsphaltWorld"
#pragma link "dxSkinTheBezier"
#pragma link "dxSkinValentine"
#pragma link "dxSkinVisualStudio2013Blue"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "dxSkinVisualStudio2013Light"
#pragma link "dxSkinVS2010"
#pragma link "dxSkinWhiteprint"
#pragma link "dxSkinXmas2008Blue"
#pragma link "cxTextEdit"
#pragma link "cxImageComboBox"
#pragma resource "*.dfm"
TFormDevInfo *FormDevInfo;

extern AnsiString GetDevTypeText(UINT32 ulDevType);
extern AnsiString GetAlarmLevelText(UINT32 ulAlarmLevel);
extern AnsiString GetAlarmIDText(UINT32 ulAlarmID);
//---------------------------------------------------------------------------
__fastcall TFormDevInfo::TFormDevInfo(TComponent* Owner)
    : TSubForm(Owner)
{
}
//---------------------------------------------------------------------------

extern Dict g_DictDevType;

bool HaveStr(AnsiString str, AnsiString strFilter)
{
    return str.UpperCase().Pos(strFilter) > 0;
}

void __fastcall TFormDevInfo::Init(AnsiString &strDevSort)
{
    UINT32 ulDevType;
    g_DictDevType.GetID(strDevSort,&ulDevType);
    VectorMap<AnsiString, ALARM_INFO_T> mActiveAlarm;
    VectorMap<AnsiString, NE_BASIC_INFO_T>  mNEBasic;

    Alarm::GetActiveAlarm(mActiveAlarm);
    NEBasic::GetNEBasicInfo(mNEBasic);

    tlDevAlarm->BeginUpdate();
    tlDevAlarm->Clear();

    for (UINT32 i=0; i<mActiveAlarm.Size(); i++)
    {
        ALARM_INFO_T    *pstAlarm = mActiveAlarm.GetValueByIndex(i);
        NE_BASIC_INFO_T *pstInfo = mNEBasic.GetValue(pstAlarm->acNeID);

        if (!pstInfo)
        {
            continue;
        }

        if (!HaveStr(pstInfo->acDevName,strDevSort) &&
            !HaveStr(pstInfo->ulDevType,ulDevType))
        {
            continue;
        }

        TcxTreeListNode *Node = tlDevAlarm->Add();
        Node->Values[0] = pstInfo->acDevName;
        Node->Values[1] = pstInfo->ulDevType;
        Node->Values[2] = pstAlarm->ulAlarmID;
        Node->Values[3] = pstAlarm->ulAlarmLevel;
        Node->Values[4] = pstAlarm->ulAlarmRaiseTime;
        Node->Values[5] = pstAlarm->acAlarmInfo;
        Node->Values[6] = pstAlarm->acAlarmReason;
    }

    tlDevAlarm->EndUpdate();
}

void __fastcall TFormDevInfo::ColumnDevTypeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[1].IsNull())
    {
        return;
    }

    UINT32  ulDevType = ANode->Values[1];

    Value = GetDevTypeText(ulDevType);
}
//---------------------------------------------------------------------------
void __fastcall TFormDevInfo::ColumnAlarmIDGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulAlarmID = ANode->Values[2];

    Value = GetAlarmIDText(ulAlarmID);
}
//---------------------------------------------------------------------------
void __fastcall TFormDevInfo::ColumnAlarmLevelGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[3].IsNull())
    {
        return;
    }

    UINT32  ulAlarmLevel = ANode->Values[3];

    Value = GetAlarmLevelText(ulAlarmLevel);
}

void __fastcall TFormDevInfo::ColumnAlarmRaiseTimeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[4].IsNull())
    {
        return;
    }

    UINT32  ulValue = (UINT32)ANode->Values[4];

    if (ulValue == 0)
    {
        Value = "";
    }
    else
    {
        Value = GetTextTime(ulValue);
    }
}
//-------------------------------------------------------------------------
