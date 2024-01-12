//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "omc.h"
#include "ClientInit.h"
#include "main.h"
#include "FrmActiveAlarm.h"
#include "FrmAlarmCfg.h"
#include "GTransFile.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxGroupBox"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
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
#pragma resource "*.dfm"
TFormActiveAlarm *FormActiveAlarm;
//---------------------------------------------------------------------------


extern Dict g_DictDevType;
extern UINT32   g_usTransFilePort;
extern AnsiString GetDevTypeText(UINT32 ulDevType);
extern BOOL GetActiveAlarm();
static Dict    s_DictAlarmID;

ID_VALUE_T  g_astAlarmLevel[] = {
									{ALARM_LEVEL_ALL,           "ȫ��"},
									{ALARM_LEVEL_CRITICAL,  "�����澯"},
					              	{ALARM_LEVEL_MAIN,      "��Ҫ�澯"},
					              	{ALARM_LEVEL_MINOR,     "��Ҫ�澯"},
					              	{ALARM_LEVEL_NORMAL,    "��ͨ�澯"},
								};
Dict    g_DictAlarmLevel(g_astAlarmLevel, ARRAY_SIZE(g_astAlarmLevel));

static ID_VALUE_T s_astAlarmID[] = {
						{SNMP_ALARM_CODE_CpuUsageTooHigh,       "CPUʹ���ʹ��߸澯"},
						{SNMP_ALARM_CODE_MemUsageTooHigh,      "�ڴ�ʹ���ʹ��߸澯"},
						{SNMP_ALARM_CODE_DiskUsageTooHigh,     "Ӳ��ռ���ʹ��߸澯"},
						{SNMP_ALARM_CODE_HardDiskMountFail,      "Ӳ�̹���ʧ�ܸ澯"},
						{SNMP_ALARM_CODE_DevOffLine,                 "�豸���߸澯"},
						{SNMP_ALARM_CODE_EmergencyCall,              "�������и澯"},
						{SNMP_ALARM_CODE_EmergencyBrace,             "����ͣ���澯"},
						{SNMP_ALARM_CODE_SelfCheckFailed,            "�Լ�ʧ�ܸ澯"},
						{SNMP_ALARM_CODE_CoverOpened,              "�豸�Ǵ򿪸澯"},
						{SNMP_ALARM_CODE_SwitchToMaster,       "�л�������״̬�澯"},
						{SNMP_ALARM_CODE_SwitchToSlave,        "�л�������״̬�澯"},
						{SNMP_ALARM_CODE_GetATSDataFail,      "��ȡATS����ʧ�ܸ澯"},
						{SNMP_ALARM_CODE_SyncTimeFail,           "ʱ��ͬ��ʧ�ܸ澯"},
						{SNMP_ALARM_CODE_LinkToLteFail,        "���Ӻ�����ʧ�ܸ澯"},
						{SNMP_ALARM_CODE_LanStatus,                 "LAN�ڲ�θ澯"},
						{SNMP_ALARM_CODE_Reboot,                     "�豸�����澯"},
						{SNMP_ALARM_CODE_DevTempTooHigh,         "�豸�¶ȹ��߸澯"},
						{SNMP_ALARM_CODE_PISLinkFail,             "PIS����ʧ�ܸ澯"},
						{SNMP_ALARM_CODE_PALinkFail,               "PA����ʧ�ܸ澯"},
						{SNMP_ALARM_CODE_PowerDown,                  "�豸����澯"},
						{SNMP_ALARM_CODE_FileStorageTooHigh, "�洢�ļ�ռ���ʹ��߸澯"},
						{SNMP_ALARM_CODE_All,                            "ȫ���澯"},
                        };

Dict    g_DictAlarmID(s_astAlarmID,ARRAY_SIZE(s_astAlarmID));

TColor          g_AlarmColorCritical = (TColor)RGB(0xED, 0x55, 0x65);
TColor          g_AlarmColorMain = (TColor)RGB(0xF3, 0x9C, 0x35);//0x80D0;
TColor          g_AlarmColorMinor = clGreen; //(TColor)RGB(225, 128, 64);
TColor          g_AlarmColorNormal = (TColor)RGB(0x4E, 0x97, 0xA8);

TColor GetAlarmColor(UINT32 ulAlarmLevel)
{
    if (ulAlarmLevel == ALARM_LEVEL_CRITICAL)   return g_AlarmColorCritical;
    if (ulAlarmLevel == ALARM_LEVEL_MAIN    )   return g_AlarmColorMain;
    if (ulAlarmLevel == ALARM_LEVEL_MINOR   )   return g_AlarmColorMinor;
    if (ulAlarmLevel == ALARM_LEVEL_NORMAL  )   return g_AlarmColorNormal;

    return (TColor)0;
}
//---------------------------------------------------------------------------
AnsiString GetAlarmIDText(UINT32 ulAlarmID)
{
	if (ulAlarmID == SNMP_ALARM_CODE_CpuUsageTooHigh    )   return "CPUʹ���ʹ��߸澯";
	if (ulAlarmID == SNMP_ALARM_CODE_MemUsageTooHigh    )   return "�ڴ�ʹ���ʹ��߸澯";
	if (ulAlarmID == SNMP_ALARM_CODE_DiskUsageTooHigh   )   return "Ӳ��ռ���ʹ��߸澯";
	if (ulAlarmID == SNMP_ALARM_CODE_HardDiskMountFail  )   return "Ӳ�̹���ʧ�ܸ澯";
	if (ulAlarmID == SNMP_ALARM_CODE_DevOffLine         )   return "�豸���߸澯";
	if (ulAlarmID == SNMP_ALARM_CODE_Reboot             )   return "�豸�����澯";
	if (ulAlarmID == SNMP_ALARM_CODE_SwitchToMaster     )   return "�л�������״̬�澯";
	if (ulAlarmID == SNMP_ALARM_CODE_SwitchToSlave      )   return "�л�������״̬�澯";
	if (ulAlarmID == SNMP_ALARM_CODE_GetATSDataFail     )   return "��ȡATS����ʧ�ܸ澯";
	if (ulAlarmID == SNMP_ALARM_CODE_SyncTimeFail       )   return "ʱ��ͬ��ʧ�ܸ澯";
	if (ulAlarmID == SNMP_ALARM_CODE_LinkToLteFail      )   return "���Ӻ�����ʧ�ܸ澯";
	if (ulAlarmID == SNMP_ALARM_CODE_EmergencyCall      )   return "�������и澯";
	if (ulAlarmID == SNMP_ALARM_CODE_EmergencyBrace     )   return "����ͣ���澯";
	if (ulAlarmID == SNMP_ALARM_CODE_SelfCheckFailed    )   return "�Լ�ʧ�ܸ澯";
	if (ulAlarmID == SNMP_ALARM_CODE_LanStatus          )   return "LAN�ڲ�θ澯";
	if (ulAlarmID == SNMP_ALARM_CODE_DevTempTooHigh     )   return "�豸�¶ȹ��߸澯";
	if (ulAlarmID == SNMP_ALARM_CODE_PISLinkFail        )   return "PIS����ʧ�ܸ澯";
	if (ulAlarmID == SNMP_ALARM_CODE_PALinkFail         )   return "PA����ʧ�ܸ澯";
	if (ulAlarmID == SNMP_ALARM_CODE_PowerDown          )   return "�豸����澯";
	if (ulAlarmID == SNMP_ALARM_CODE_FileStorageTooHigh )   return "�洢�ļ�ռ���ʹ��߸澯";
	if (ulAlarmID == SNMP_ALARM_CODE_All                )   return "ȫ���澯";
    return ulAlarmID;
}
//---------------------------------------------------------------------------
AnsiString GetAlarmLevelText(UINT32 ulAlarmLevel)
{
    for (UINT32 i=0; i<ARRAY_SIZE(g_astAlarmLevel); i++)
    {
        if (g_astAlarmLevel[i].ulID == ulAlarmLevel)
        {
            return g_astAlarmLevel[i].szValue;
        }
    }

    return "";
}
//---------------------------------------------------------------------------
void InitDevAlarmInfo(TcxComboBox *cb, UINT32 ulDevType)
{
	cb->Properties->Items->Clear();
	// �����豸���еĻ����澯
	cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_CpuUsageTooHigh));
	cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_MemUsageTooHigh));
	cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_DiskUsageTooHigh));
	cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_DevOffLine));

	if (ulDevType == OMC_DEV_TYPE_TAU)
	{
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_PowerDown));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_LanStatus));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_PowerDown));
	}
	else if (ulDevType == OMC_DEV_TYPE_TX)
	{
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_PowerDown));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_DevOffLine));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_EmergencyCall));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_EmergencyBrace));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_SelfCheckFailed));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_DevTempTooHigh));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_PISLinkFail));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_PALinkFail));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_HardDiskMountFail));
	}
	else if (ulDevType == OMC_DEV_TYPE_FX)
	{
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_PowerDown));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_DevOffLine));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_EmergencyCall));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_SelfCheckFailed));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_DevTempTooHigh));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_PALinkFail));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_HardDiskMountFail));
	}
	else if (ulDevType == OMC_DEV_TYPE_DIS)
	{
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_SwitchToMaster));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_SwitchToSlave));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_GetATSDataFail));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_SyncTimeFail));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_LinkToLteFail));
	}
	else if (ulDevType == OMC_DEV_TYPE_DC)
	{
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_GetATSDataFail));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_SyncTimeFail));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_LinkToLteFail));
	}
	else if (ulDevType == OMC_DEV_TYPE_REC)
	{
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_SyncTimeFail));
		cb->Properties->Items->Add(g_DictAlarmID.GetValue(SNMP_ALARM_CODE_FileStorageTooHigh));
	}
	else if (ulDevType == OMC_DEV_TYPE_ALL)
	{
		g_DictAlarmID.Init(cb);
	}
	else
	{
		GosLog(LOG_ERROR, "invalid DevType: %u", ulDevType);
	}

	return;
}

BOOL HasAlarm(CHAR *szNeID)
{
    VectorMap<AnsiString, ALARM_INFO_T> mAlarmInfo;
    Alarm::GetActiveAlarm(mAlarmInfo);

    for (UINT32 i=0; i<mAlarmInfo.Size(); i++)
    {
        if (strcmp(mAlarmInfo[i]->acNeID, szNeID) == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TFormActiveAlarm::tlActiveAlarmCustomDrawDataCell(TcxCustomTreeList *Sender, TcxCanvas *ACanvas,
          TcxTreeListEditCellViewInfo *AViewInfo, bool &ADone)
{
    if (AViewInfo->Node && !AViewInfo->Node->Values[4].IsNull() &&
        AViewInfo->Column->ItemIndex == 4 )
    {
        UINT32  ulAlarmLevel = AViewInfo->Node->Values[4];
        TColor  AlarmColor = GetAlarmColor(ulAlarmLevel);

        if (AlarmColor != 0)
        {
            ACanvas->Canvas->Brush->Color = AlarmColor;
        }
    }
}
//---------------------------------------------------------------------------
bool HaveStr(AnsiString str, AnsiString strFilter)
{
    return str.UpperCase().Pos(strFilter) > 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormActiveAlarm::ColumnAlarmRaiseTimeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[6].IsNull())
    {
        return;
    }

    UINT32  ulValue = (UINT32)ANode->Values[6];

    if (ulValue == 0)
    {
        Value = "";
    }
    else
    {
        Value = GetTextTime(ulValue);
    }
}
//---------------------------------------------------------------------------
__fastcall TFormActiveAlarm::TFormActiveAlarm(TComponent* Owner)
    : TSubForm(Owner)
{
    g_DictDevType.Init(cbDevType);
    g_DictAlarmLevel.Init(cbAlarmLevel);

    gbWorkPanel->Parent = MainPanel;
    tlActiveAlarm->Parent = MainPanel;

    InitMsgHandler(EV_OMT_OMC_RSP, MSG_OMT_GetActiveAlarm, OnGetActiveAlarmRsp);
    tlActiveAlarm->OnCustomDrawDataCell =   tlActiveAlarmCustomDrawDataCell;
    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormActiveAlarm::btnRefreshClick(TObject *Sender)
{
    static UINT32 ulLastTime = 0;
    UINT32  ulCurrTime = gos_get_uptime_1ms();

    if (ulLastTime == 0 ||
        (ulLastTime+1000) < ulCurrTime)
    {
        SendServer(EV_OMT_OMC_REQ, MSG_OMT_GetActiveAlarm);
        ulLastTime = gos_get_uptime_1ms();
    }

}
//---------------------------------------------------------------------------

void __fastcall TFormActiveAlarm::TimerRefreshTimer(TObject *Sender)
{
    RefreshActiveAlarmInfo();
}
//---------------------------------------------------------------------------

void __fastcall TFormActiveAlarm::cbAlarmLevelPropertiesChange(TObject *Sender)
{
	// ���� cbDevType �� cbAlarmLevl ѡ���ѡ�������� cbAlarmID ��ֵ
	VECTOR<ALARM_LEVEL_CFG_T>   vAlarmLevelCfg;
	UINT32 ulDevType = OMC_DEV_TYPE_ALL;
	UINT32 ulAlarmLevel =  ALARM_LEVEL_ALL;

	Alarm::GetAlarmLevelCfg(vAlarmLevelCfg);
	g_DictDevType.GetID(cbDevType, &ulDevType);
	g_DictAlarmLevel.GetID(cbAlarmLevel, &ulAlarmLevel);

	cbAlarmID->Properties->Items->Clear();
	cbAlarmID->Properties->Items->Add("ȫ��");
	for (UINT32 i=0; i<vAlarmLevelCfg.size(); i++)
	{
		// ������豸���͵ĸ澯���ã������豸���͵ĸ澯����Ϊ׼
		// ���û�У���ʹ�ø澯���͵�����
	   if ( vAlarmLevelCfg[i].ulDevType == ulDevType &&
			vAlarmLevelCfg[i].ulAlarmLevel == ulAlarmLevel )
	   {
			cbAlarmID->Properties->Items->Add(g_DictAlarmID.GetValue(vAlarmLevelCfg[i].ulAlarmID));
	   }
	}


    InitView();
}
//---------------------------------------------------------------------------
 void __fastcall TFormActiveAlarm::cbAlarmIDPropertiesChange(TObject *Sender)
{
    InitView();
}
//---------------------------------------------------------------------------

void __fastcall TFormActiveAlarm::edtSearchPropertiesChange(TObject *Sender)
{
	InitView();
}
//---------------------------------------------------------------------------

void __fastcall TFormActiveAlarm::deStartTimePropertiesChange(TObject *Sender)
{
    InitView();
}
//---------------------------------------------------------------------------

void __fastcall TFormActiveAlarm::deEndTimePropertiesChange(TObject *Sender)
{
    InitView();
}
//---------------------------------------------------------------------------

void __fastcall TFormActiveAlarm::ColumnDevNameGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulDevType = ANode->Values[2];

    Value = GetDevTypeText(ulDevType);
}
//---------------------------------------------------------------------------
void __fastcall TFormActiveAlarm::ColumnAlarmIDGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[3].IsNull())
    {
        return;
    }

    UINT32  ulAlarmID = ANode->Values[3];

    Value = GetAlarmIDText(ulAlarmID);
}
//---------------------------------------------------------------------------
void __fastcall TFormActiveAlarm::ColumnAlarmLevelGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[4].IsNull())
    {
        return;
    }

    UINT32  ulAlarmLevel = ANode->Values[4];

    Value = GetAlarmLevelText(ulAlarmLevel);
}
//---------------------------------------------------------------------------
void __fastcall TFormActiveAlarm::cbDevTypePropertiesChange(TObject *Sender)
{
	UINT32 ulDevType = OMC_DEV_TYPE_ALL;

	g_DictDevType.GetID(cbDevType, &ulDevType);
	InitDevAlarmInfo(cbAlarmID, ulDevType);
	InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormActiveAlarm::UpdateAlarmLevel()
{
    UINT32      ulAlarmLevel = 0;

    if (cbAlarmLevel->ItemIndex < 0)
    {
        ulAlarmLevel = ALARM_LEVEL_ALL;
    }
	else if (!g_DictAlarmLevel.GetID(cbAlarmLevel, &ulAlarmLevel))
	{
		return;
    }

    // ���� cbAlarmLevl ѡ���ѡ�������� cbAlarmID ��ֵ
	VECTOR<ALARM_LEVEL_CFG_T>   vAlarmLevel;
    AnsiString                  strAlarmName;

	Alarm::GetAlarmLevelCfg(vAlarmLevel);
	s_DictAlarmID.Clear();

	s_DictAlarmID.Add(SNMP_ALARM_CODE_All,"ȫ��");
    for (UINT32 i=0; i<vAlarmLevel.size(); i++)
    {
        if (ulAlarmLevel == ALARM_LEVEL_ALL ||
			vAlarmLevel[i].ulAlarmLevel == ulAlarmLevel)
        {
			strAlarmName = GetAlarmIDText(vAlarmLevel[i].ulAlarmID);
			s_DictAlarmID.Add(vAlarmLevel[i].ulAlarmID, strAlarmName.c_str());
        }
    }

    s_DictAlarmID.Init(cbAlarmID);
}
//---------------------------------------------------------------------------

void __fastcall TFormActiveAlarm::pmiAlarmClearClick(TObject *Sender)
{
    if (tlActiveAlarm->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tlActiveAlarm->Selections[0];

    if (Node->Values[0].IsNull() ||
        Node->Values[1].IsNull() )
    {
        return;
    }

    AnsiString  strNeID = Node->Values[0];
    UINT32      ulAlarmID = Node->Values[3];
    UINT32      ulAlarmRaiseTime = Node->Values[6];
    UINT32      ulAlarmLevel = Node->Values[4];
    GJsonParam      Param;

    Param.Add("ne_id", strNeID);
    Param.Add("alarm_id", ulAlarmID);
    Param.Add("alarm_raise_time", ulAlarmRaiseTime);
    Param.Add("alarm_level", ulAlarmLevel);

    if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_ClearActiveAlarm, Param))
    {
        WarnMsg("ɾ��ʧ��");
        return;
    }

    Node->Delete();
    btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormActiveAlarm::InitNode(TcxTreeListNode *Node, ALARM_INFO_T *pstAlarm, NE_BASIC_INFO_T *pstInfo)
{
    Node->Values[0] = pstAlarm->acNeID;
    Node->Values[1] = pstInfo->acDevName;
    Node->Values[2] = pstInfo->ulDevType;
    Node->Values[3] = pstAlarm->ulAlarmID;
    Node->Values[4] = pstAlarm->ulAlarmLevel;
    Node->Values[5] = pstAlarm->ulAlarmType;
    Node->Values[6] = pstAlarm->ulAlarmRaiseTime;
    Node->Values[7] = pstAlarm->acAlarmInfo;
    Node->Values[8] = pstAlarm->acAlarmReason;
}


void __fastcall TFormActiveAlarm::InitView()
{
    UINT32          ulDevType;
    UINT32          ulAlarmLevel;
    AnsiString      strNeID = "";
    UINT32          ulAlarmID = 0xffffffff;
    AnsiString      strSelectedNeID = "";
    UINT32          ulStartTime = GetTime(deStartTime->Date);
    UINT32          ulEndTime = GetTime(deEndTime->Date);
    AnsiString      strKey = edtSearch->Text.Trim().UpperCase();
    AnsiString      strAlarmName = cbAlarmID->Text;
    VectorMap<AnsiString, NE_BASIC_INFO_T>  mNEBasicInfo;

    Alarm::GetActiveAlarm(m_mAlarmInfo);
    NEBasic::GetNEBasicInfo(mNEBasicInfo);


    if (ulEndTime == 0)
    {
        ulEndTime = 0xffffffff;
    }

	if (ulStartTime > ulEndTime)
    {
        deEndTime->Clear();
        return;
    }

    if (!g_DictDevType.GetID(cbDevType, &ulDevType))
    {
        ulDevType = OMC_DEV_TYPE_ALL;
    }

    if (!g_DictAlarmLevel.GetID(cbAlarmLevel, &ulAlarmLevel))
    {
        ulAlarmLevel = ALARM_LEVEL_ALL ;
    }

    if (!g_DictAlarmID.GetID(cbAlarmID,&ulAlarmID))
    {
        ulAlarmID = SNMP_ALARM_CODE_All;
    }

    tlActiveAlarm->BeginUpdate();
    tlActiveAlarm->Clear();
    TcxTreeListNode *SelectedNode = NULL;

    //��ȫ�ּ��صĻ�Ծ�澯�н��й���
    for (UINT32 i=0; i<m_mAlarmInfo.Size(); i++)
    {
        ALARM_INFO_T    *pstAlarm = m_mAlarmInfo.GetValueByIndex(i);
        NE_BASIC_INFO_T *pstInfo = mNEBasicInfo.GetValue(pstAlarm->acNeID);

        if (!pstInfo)
        {
            continue;
        }

        if (ulDevType != OMC_DEV_TYPE_ALL &&
            ulDevType != pstInfo->ulDevType)
        {
            continue;
        }

        if (!strNeID.IsEmpty() &&
            strNeID != pstAlarm->acNeID)
        {
            continue;
        }

        if (ulAlarmLevel != ALARM_LEVEL_ALL &&
            ulAlarmLevel != pstAlarm->ulAlarmLevel)
        {
            continue;
        }

        if (ulAlarmID != SNMP_ALARM_CODE_All &&
            ulAlarmID != pstAlarm->ulAlarmID)
        {
           continue;
        }

        if (pstAlarm->ulAlarmRaiseTime < ulStartTime ||
            pstAlarm->ulAlarmRaiseTime > ulEndTime )
        {
            continue;
        }

        if (!strKey.IsEmpty() &&
            !HaveStr(pstAlarm->acAlarmInfo, strKey.c_str()) &&
            !HaveStr(pstInfo->acDevName,strKey.c_str()))
        {
            continue;
        }

        TcxTreeListNode *Node = tlActiveAlarm->Add();

        InitNode(Node, pstAlarm,pstInfo);
//      Node->Values[0] = pstAlarm->acNeID;
//      Node->Values[1] = pstInfo->ulDevType;
//      Node->Values[2] = pstInfo->acDevName;
//      Node->Values[3] = pstAlarm->ulAlarmID;
//      Node->Values[4] = pstAlarm->ulAlarmLevel;
//      Node->Values[5] = pstAlarm->ulAlarmType;
//      Node->Values[6] = pstAlarm->ulAlarmRaiseTime;
//      Node->Values[7] = pstAlarm->acAlarmInfo;
//      Node->Values[8] = pstAlarm->acAlarmReason;
        if (!SelectedNode &&
            !strSelectedNeID.IsEmpty() &&
            strSelectedNeID == pstAlarm->acNeID &&
            ulAlarmID == pstAlarm->ulAlarmID)
        {
            SelectedNode = Node;
        }
    }

    if (SelectedNode)
    {
        SelectedNode->MakeVisible();
    }

    tlActiveAlarm->EndUpdate();
}
//---------------------------------------------------------------------------

void __fastcall TFormActiveAlarm::OnGetActiveAlarmRsp(UINT32 ulSeqID, GJson &Json)
{
    GJson           *pSubJson;
    GJson           JsonResult;
    VECTOR<GJson*>  vSubJson;
    ALARM_INFO_T    stInfo = {0};
    CHAR            acKey[128] = {0};
	CHAR            acFile[256] = {0};
	CHAR            acLocalFile[256] = {0};
    VectorMap<AnsiString, ALARM_INFO_T> mActiveAlarm;
    VectorMap<AnsiString, NE_BASIC_INFO_T>     mNEBasicInfo;
	NEBasic::GetNEBasicInfo(mNEBasicInfo);
    std::map<UINT32, UINT32> mDevAlarmCount;

    /*
    ��Ծ�澯�������п��ܻᳬ��һ����64K�Ĵ�С
    ��������������������Json�ļ��������ļ������ͻ��ˡ�
    ����ȡ���ļ���������ý���Json�ļ��ķ�ʽ����
    */

    vSubJson.clear();
	if (Json.GetValue("File", acFile, sizeof(acFile)))
    {
        // ���ÿͻ��˱��ر����ļ�Ŀ¼
        sprintf(acLocalFile, "%s/%s", gos_get_root_path(), acFile);

		if (!TransFile(g_stLocalCfg.aucOMCAddr, g_usTransFilePort, acFile, acLocalFile))
        {
            GosLog(LOG_ERROR, "DownLoad file %s fail", acFile);
            return;
        }

        // ����Json�ļ�
		if (!JsonResult.ParseFile(acLocalFile))
		{
			gos_delete_file(acLocalFile);
			GosLog(LOG_ERROR, "ParseFile %s fail", acLocalFile);
            return;
		}
		gos_delete_file(acLocalFile);

		vSubJson = JsonResult.GetSubJson();
    }
    else
    {
		vSubJson = Json.GetSubJson();
    }
    /*
    ToDo: ��ͬ�����豸�澯ͳ�Ʒ��࣬�������������ж��豸�Ƿ��и澯���ж�
    */
    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];

        if (!pSubJson->GetValue("ne_id", stInfo.acNeID, sizeof(stInfo.acNeID)) ||
            !pSubJson->GetValue("alarm_id", &stInfo.ulAlarmID)   ||
            !pSubJson->GetValue("level", &stInfo.ulAlarmLevel)      ||
            !pSubJson->GetValue("type", &stInfo.ulAlarmType)        ||
            !pSubJson->GetValue("reason", stInfo.acAlarmReason, sizeof(stInfo.acAlarmReason)) ||
            !pSubJson->GetValue("time", &stInfo.ulAlarmRaiseTime)   ||
            !pSubJson->GetValue("info", stInfo.acAlarmInfo, sizeof(stInfo.acAlarmInfo))
           )
        {
            GosLog(LOG_ERROR, "TFormActiveAlarm::OnGetActiveAlarmRsp: parse alarm info failed : %s", pSubJson->GetText());
            return ;
        }

        // NeID �� AlarmID �����ֶβ�������ĳ���豸��ĳ��澯
        sprintf(acKey, "%u_%s", stInfo.ulAlarmID, stInfo.acNeID);
        mActiveAlarm.Add(acKey, stInfo);

        // ͳ��ÿ���豸�Ļ�Ծ�澯����
        AnsiString  strNeID = stInfo.acNeID;
        NE_BASIC_INFO_T *pstInfo = mNEBasicInfo.GetValue(stInfo.acNeID);
        if (NULL != pstInfo)
        {
            ++mDevAlarmCount[pstInfo->ulDevType];
        }
    }

    Alarm::InitActiveAlarm(mActiveAlarm);
    Alarm::InitDevAlarmCount(mDevAlarmCount);
    InitView();
}
