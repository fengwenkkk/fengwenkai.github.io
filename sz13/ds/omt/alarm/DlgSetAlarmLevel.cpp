//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "DlgSetAlarmLevel.h"
#include "ClientUtil.h"
#include "ClientCommon.h"
#include "FrmAlarm.h"
#include "MsgDialog.h"
#include "Resource.h"
#include "omc.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxDropDownEdit"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxLabel"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxMaskEdit"
#pragma link "cxTextEdit"
#pragma link "dxBevel"
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
#pragma link "cxCheckBox"
#pragma resource "*.dfm"
TDialogSetAlarmLevel *DialogSetAlarmLevel;

extern AnsiString GetAlarmIDText(UINT32 ulAlarmID);
extern AnsiString GetAlarmLevelText(UINT32 ulAlarmLevel);
extern AnsiString GetDevTypeText(UINT32 ulDevType);
extern INT32 GetAlarmLevelDigit(UnicodeString &Value);
extern Dict    g_DictAlarmID;
extern void InitDevAlarmInfo(TcxComboBox *cb, UINT32 ulDevType);
extern Dict    g_DictAlarmLevel;

//---------------------------------------------------------------------------
__fastcall TDialogSetAlarmLevel::TDialogSetAlarmLevel(TComponent* Owner)
	: TDialogForm(Owner)
{
	g_DictAlarmLevel.Init(cbAlarmLevel, TRUE, TRUE);
	g_DictAlarmID.Init(cbAlarmID, TRUE, TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetAlarmLevel::Init(UINT32 ulDevType, UINT32 ulAlarmID, UINT32 ulAlarmLevel)
{
	m_ulDevType = ulDevType;
	m_ulAlarmID  = ulAlarmID;
	m_ulAlarmLevel = ulAlarmLevel;

	edtDevType->Enabled = false;
	edtDevType->Text = GetDevTypeText(m_ulDevType);

	g_DictAlarmID.SetID(cbAlarmID, m_ulAlarmID);
	cbAlarmID->Enabled = false;

	g_DictAlarmLevel.SetID(cbAlarmLevel, m_ulAlarmLevel);


	Caption = "修改告警等级配置";
}

void __fastcall TDialogSetAlarmLevel::Init(UINT32 ulDevType)
{
	m_ulDevType = ulDevType;

	cbAlarmID->Properties->Items->Clear();
	InitDevAlarmInfo(cbAlarmID, ulDevType);

	cbAlarmLevel->Properties->Items->Clear();
	g_DictAlarmLevel.Init(cbAlarmLevel, TRUE, TRUE);

	lbDevType->Visible = true;
	edtDevType->Visible = true;
	edtDevType->Enabled = false;
	edtDevType->Text = GetDevTypeText(m_ulDevType);
	cbAlarmID->Enabled = true;

	Caption = "增加告警等级配置";
}

void __fastcall TDialogSetAlarmLevel::GetSetAlarmCfg(UINT32 &ulSetAlarmLevel, BOOL &bSetAlarmStatus)
{
	ulSetAlarmLevel = m_ulSetAlarmLevel;
	bSetAlarmStatus = cbIgnore->Checked;
	return;
}

void __fastcall TDialogSetAlarmLevel::GetSetAlarmCfg(UINT32 &ulSetAlarmID, UINT32 &ulSetAlarmLevel, BOOL &bSetAlarmStatus)
{
	ulSetAlarmID = m_ulSetAlarmID;
	ulSetAlarmLevel = m_ulSetAlarmLevel;
	bSetAlarmStatus = cbIgnore->Checked;
	return;
}

void __fastcall TDialogSetAlarmLevel::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetAlarmLevel::btnOKClick(TObject *Sender)
{
	if (!g_DictAlarmLevel.GetID(cbAlarmLevel, &m_ulSetAlarmLevel))
	{
		InfoMsg("告警等级非法");
		cbAlarmLevel->Focused();
		return;
	}

	if (!g_DictAlarmID.GetID(cbAlarmID, &m_ulSetAlarmID))
	{
		InfoMsg("告警类型非法");
		cbAlarmLevel->Focused();
		return;
	}

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetAlarmLevel::FormCreate(TObject *Sender)
{
	SetImage(btnOK, IMG_OK);
	SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------


