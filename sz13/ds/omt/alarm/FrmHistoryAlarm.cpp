#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "omc.h"
#include "ClientInit.h"
#include "main.h"
#include "FrmHistoryAlarm.h"
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
#pragma resource "*.dfm"
TFormHistoryAlarm *FormHistoryAlarm;

extern Dict     g_DictDevType;
extern Dict     g_DictAlarmLevel;

extern TColor GetAlarmColor(UINT32 ulAlarmLevel);
extern AnsiString GetAlarmLevelText(UINT32 ulAlarmLevel);
extern AnsiString GetAlarmIDText(UINT32 ulAlarmID);


void __fastcall TFormHistoryAlarm::InitSkin()
{
}
//---------------------------------------------------------------------------
__fastcall TFormHistoryAlarm::TFormHistoryAlarm(TComponent* Owner)
    : TSubForm(Owner)
{
    m_ulStartTime = 0;
    m_ulEndTime   = gos_get_current_time();
    g_DictDevType.Init(cbDevType);
    //UpdateDevList();
	InitMsgHandler(EV_OMT_OMC_RSP, MSG_OMT_GetHistoryAlarm, OnGetHistoryAlarmRsp);

	tlAlarm->OnCustomDrawDataCell = tlAlarmCustomDrawDataCell;
}
//---------------------------------------------------------------------------
void __fastcall TFormHistoryAlarm::tlAlarmCustomDrawDataCell(TcxCustomTreeList *Sender, TcxCanvas *ACanvas,
          TcxTreeListEditCellViewInfo *AViewInfo, bool &ADone)
{
    if (AViewInfo->Node && !AViewInfo->Node->Values[2].IsNull() &&
        AViewInfo->Column->ItemIndex == 2 )
    {
        UINT32  ulAlarmLevel = AViewInfo->Node->Values[2];
        TColor  AlarmColor = GetAlarmColor(ulAlarmLevel);

		if (AlarmColor != 0)
        {
            ACanvas->Canvas->Brush->Color = AlarmColor;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormHistoryAlarm::InitView()
{
    AnsiString      strNeID = "";
    UINT32          ulAlarmID;
    BOOL            bDesc = FALSE;
    VectorMap<AnsiString, ALARM_INFO_T> mAlarm;
    Alarm::GetHistoryAlarm(mAlarm);

    if (mAlarm.Size() == 0)
    {
        btnNext->Enabled = false;
        btnPrev->Enabled = false;
    }
    else
    {
        btnNext->Enabled = true;
        btnPrev->Enabled = true;

		if ( mAlarm.GetValueByIndex(0)->ulAlarmRaiseTime <
			 mAlarm.GetValueByIndex(mAlarm.Size()-1)->ulAlarmRaiseTime )
            {
                m_ulStartTime =  mAlarm.GetValueByIndex(0)->ulAlarmRaiseTime;
                m_ulEndTime =  mAlarm.GetValueByIndex(mAlarm.Size()-1)->ulAlarmRaiseTime;
            }
        else
        {
			bDesc = true;
			m_ulEndTime =  mAlarm.GetValueByIndex(0)->ulAlarmRaiseTime;
			m_ulStartTime =  mAlarm.GetValueByIndex(mAlarm.Size()-1)->ulAlarmRaiseTime;
		}
	}

    if (tlAlarm->SelectionCount > 0)
    {
        strNeID = tlAlarm->Selections[0]->Values[0];
        ulAlarmID = tlAlarm->Selections[0]->Values[1];
    }

    tlAlarm->BeginUpdate();
    tlAlarm->Clear();

    TcxTreeListNode *SelectedNode = NULL;

	for (UINT32 i=0; i<mAlarm.Size(); i++)
    {
        ALARM_INFO_T    *pstAlarm;

		if (bDesc)
        {
           pstAlarm = mAlarm.GetValueByIndex(mAlarm.Size()-1-i);
        }
        else
        {
           pstAlarm = mAlarm.GetValueByIndex(i);
        }

        if (GetDevNameByNeID(pstAlarm->acNeID)== "")
        {
            continue;
        }

        TcxTreeListNode *Node = tlAlarm->Add();
        Node->Values[0] = pstAlarm->acNeID;
        Node->Values[1] = pstAlarm->ulAlarmID;
        Node->Values[2] = pstAlarm->ulAlarmLevel;
        Node->Values[3] = pstAlarm->ulAlarmType;
        Node->Values[4] = pstAlarm->ulAlarmRaiseTime;
        Node->Values[5] = pstAlarm->acAlarmInfo;
        Node->Values[6] = pstAlarm->acAlarmReason;
        Node->Values[8] = pstAlarm->ulAlarmClearTime;
        Node->Values[9] = pstAlarm->acAlarmClearInfo;

        if (!SelectedNode &&
            !strNeID.IsEmpty() &&
            strNeID == pstAlarm->acNeID &&
            ulAlarmID == pstAlarm->ulAlarmID)
        {
            SelectedNode = Node;
        }
    }

    if (SelectedNode)
    {
        SelectedNode->MakeVisible();
    }

    tlAlarm->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormHistoryAlarm::ColumnAlarmRaiseTimeGetDisplayText(TcxTreeListColumn *Sender,
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
//---------------------------------------------------------------------------
void __fastcall TFormHistoryAlarm::OnGetHistoryAlarmRsp(UINT32 ulSeqID, GJson &Json)
{
    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = Json.GetSubJson();
    char            acKey[128];
    ALARM_INFO_T    stInfo = {0};
    VectorMap<AnsiString, ALARM_INFO_T> mHistoryAlarm;

	for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];

        if (!pSubJson->GetValue("ne_id", stInfo.acNeID, sizeof(stInfo.acNeID)) ||
            !pSubJson->GetValue("alarm_id", &stInfo.ulAlarmID) ||
            !pSubJson->GetValue("level", &stInfo.ulAlarmLevel) ||
            !pSubJson->GetValue("type", &stInfo.ulAlarmType) ||
            !pSubJson->GetValue("reason", stInfo.acAlarmReason, sizeof(stInfo.acAlarmReason)) |
            !pSubJson->GetValue("time", &stInfo.ulAlarmRaiseTime) ||
            !pSubJson->GetValue("info", stInfo.acAlarmInfo, sizeof(stInfo.acAlarmInfo)) ||
            !pSubJson->GetValue("clear_time", &stInfo.ulAlarmClearTime) ||
            !pSubJson->GetValue("clear_info", stInfo.acAlarmClearInfo, sizeof(stInfo.acAlarmClearInfo)))
        {
            GosLog(LOG_ERROR, "TFormHistoryAlarm::OnGetHistoryAlarmRsp：parse alarm info failed on %s", pSubJson->GetText());
            return;
        }

		sprintf(acKey, "%s_%u_%u", stInfo.acNeID, stInfo.ulAlarmID, stInfo.ulAlarmRaiseTime);
		mHistoryAlarm.Add(acKey,stInfo);
    }

	Alarm::InitHistoryAlarm(mHistoryAlarm);
	InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormHistoryAlarm::FormCreate(TObject *Sender)
{
    SetImage(btnQuery, IMG_QUERY);
    SetImage(btnPrev, IMG_PREV);
    SetImage(btnNext, IMG_NEXT);
}
//---------------------------------------------------------------------------
void __fastcall TFormHistoryAlarm::btnQueryClick(TObject *Sender)
{
    UINT32  ulDevType;
    AnsiString strNeID = "";
    UINT32  ulStartTime = 0;
    UINT32  ulEndTime = gos_get_current_time();
    GJsonParam  Param;

    if (!g_DictDevType.GetID(cbDevType, &ulDevType))
    {
        ulDevType = OMC_DEV_TYPE_ALL;
    }

    if (!cbDev->Text.IsEmpty())
    {
        strNeID = GetNeIDByDevName(cbDev->Text.Trim());
    }

    if (!deStartTime->Text.IsEmpty())
    {
        ulStartTime = GetTime(deStartTime->Date);
    }

    if (!deEndTime->Text.IsEmpty())
    {
        ulEndTime = GetTime(deEndTime->Date);
    }

/*
   ulStartTime = GetTime(deStartTime->Date);
    ulEndTime = GetTime(deEndTime->Date);
*/
    if (ulEndTime == 0)
    {
        ulEndTime = 0xffffffff;
    }

    if (ulStartTime > ulEndTime)
    {
        InfoMsg("开始时间不能大于结束时间");
        return;
    }

    Param.Add("DevType", ulDevType);
    Param.Add("NeID", strNeID);
    Param.Add("StartTime", ulStartTime);
    Param.Add("EndTime", ulEndTime);
    Param.Add("AlarmInfoKey", edtAlarmInfoKey->Text);

    if (!SendServer(EV_OMT_OMC_REQ, MSG_OMT_GetHistoryAlarm, Param))
    {
        InfoMsg("查询失败");
        return;
    }

    InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormHistoryAlarm::ColumnAlarmLevelGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulAlarmLevel = (UINT32)ANode->Values[2];

    Value = GetAlarmLevelText(ulAlarmLevel);
}
//---------------------------------------------------------------------------
void __fastcall TFormHistoryAlarm::ColumnNeIDGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    if (ANode->Values[0].IsNull())
    {
        return;
    }

    AnsiString  strNeID = ANode->Values[0];

    Value = GetDevNameByNeID(strNeID);
}
//---------------------------------------------------------------------------
void __fastcall TFormHistoryAlarm::UpdateDevList()
{
    AnsiString  strDevName = cbDev->Text;

    cbDevTypePropertiesChange(this);

    cbDev->ItemIndex = cbDev->Properties->Items->IndexOf(strDevName);
}
//---------------------------------------------------------------------------
void __fastcall TFormHistoryAlarm::cbDevTypePropertiesChange(TObject *Sender)
{
    UINT32  ulDevType;

    cbDev->Properties->Items->Clear();

    if (!g_DictDevType.GetID(cbDevType, &ulDevType))
    {
        return;
    }

    VectorMap<AnsiString, NE_BASIC_INFO_T>  mNEBasicInfo;
    NEBasic::GetNEBasicInfo(mNEBasicInfo);

    cbDev->Properties->Items->BeginUpdate();
    cbDev->Properties->Items->Add("全部");

    for (UINT32 i=0; i<mNEBasicInfo.Size(); i++)
    {
        NE_BASIC_INFO_T *pstInfo = mNEBasicInfo.GetValueByIndex(i);

        if (pstInfo->ulDevType == ulDevType
            && *(pstInfo->acDevName) != '\0')
        {
            cbDev->Properties->Items->Add(pstInfo->acDevName);
        }

    }

    cbDev->ItemIndex = -1;

    cbDev->Properties->Items->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormHistoryAlarm::ColumnAlarmClearTimeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[8].IsNull())
    {
        return;
    }

    UINT32  ulValue = (UINT32)ANode->Values[8];

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
void __fastcall TFormHistoryAlarm::ColumnAlarmIDGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[1].IsNull())
    {
        return;
    }

    UINT32  ulAlarmID = ANode->Values[1];

    Value = GetAlarmIDText(ulAlarmID);
}
//---------------------------------------------------------------------------
void __fastcall TFormHistoryAlarm::btnNextClick(TObject *Sender)
{
    deStartTime->Date = GetDateTime(m_ulEndTime);
    deEndTime->Clear();

    btnQueryClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormHistoryAlarm::btnPrevClick(TObject *Sender)
{
    deStartTime->Clear();
    deEndTime->Date = GetDateTime(m_ulStartTime);

    btnQueryClick(this);
}
//---------------------------------------------------------------------------

