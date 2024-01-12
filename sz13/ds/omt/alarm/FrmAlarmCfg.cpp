//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "omc.h"
#include "ClientInit.h"
#include "main.h"
#include "FrmActiveAlarm.h"
#include "DlgSetAlarmLevel.h"
#include "FrmAlarmCfg.h"

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
#pragma link "cxContainer"
#pragma link "cxEdit"
#pragma link "cxGroupBox"
#pragma link "cxClasses"
#pragma link "dxBar"
#pragma link "cxButtons"
#pragma link "cxImageComboBox"
#pragma link "cxDropDownEdit"
#pragma link "cxLabel"
#pragma link "cxMaskEdit"
#pragma resource "*.dfm"

TFormAlarmCfg *FormAlarmCfg;

extern Dict g_DictDevType;
extern AnsiString GetDevTypeText(UINT32 ulDevType);
extern AnsiString GetAlarmIDText(UINT32 ulAlarmID);
extern AnsiString GetAlarmLevelText(UINT32 ulAlarmLevel);
extern TColor GetAlarmColor(UINT32 ulAlarmLevel);

extern VectorMap<UINT32, ALARM_LEVEL_CFG_T>    g_mAlarmLevelCfg;

__fastcall TFormAlarmCfg::TFormAlarmCfg(TComponent* Owner)
    : TSubForm(Owner)
{
	g_DictDevType.Init(cbDevType, TRUE);
    cbDevType->ItemIndex = 0;
    tlAlarmCfg->Parent = MainPanel;
	gbWorkPanel->Parent = MainPanel;
	InitView();
	InitMsgHandler(EV_OMT_OMC_RSP, MSG_OMT_GetAlarmLevelCfg, OnGetAlarmLevelRsp);
	tlAlarmCfg->OnCustomDrawDataCell =   tlAlarmCfgCustomDrawDataCell;
}
//---------------------------------------------------------------------------
void __fastcall TFormAlarmCfg::InitView()
{
	UINT32 ulDevType;
    UINT32 ulAlarmID;
    UINT32 ulAlarmLevel;
	VECTOR<ALARM_LEVEL_CFG_T>   vAlarmLevelCfg;

	Alarm::GetAlarmLevelCfg(vAlarmLevelCfg);

	if (tlAlarmCfg->SelectionCount > 0)
    {
        ulAlarmID = tlAlarmCfg->Selections[0]->Values[0];
        ulAlarmLevel = tlAlarmCfg->Selections[0]->Values[1];
    }

    TcxTreeListNode *SelectedNode = NULL;
    tlAlarmCfg->BeginUpdate();
    tlAlarmCfg->Clear();
	g_DictDevType.GetID(cbDevType, &ulDevType);

	for (UINT32 i=0; i<vAlarmLevelCfg.size(); i++)
    {

		if (vAlarmLevelCfg[i].ulDevType != ulDevType)
		{
			continue;
		}

		TcxTreeListNode *Node = tlAlarmCfg->Add();
		Node->Values[0] = vAlarmLevelCfg[i].ulAlarmID;
		Node->Values[1] = vAlarmLevelCfg[i].ulAlarmLevel;
		Node->Values[2] = vAlarmLevelCfg[i].bIsIgnore;

        if (!SelectedNode &&
			ulAlarmID == vAlarmLevelCfg[i].ulAlarmID &&
            ulAlarmLevel == vAlarmLevelCfg[i].ulAlarmLevel)
        {
            SelectedNode = Node;
        }
    }

    if (SelectedNode)
    {
        SelectedNode->MakeVisible();
    }

    tlAlarmCfg->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormAlarmCfg::ColumnAlarmIDGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[0].IsNull())
    {
        return;
    }

    UINT32  ulAlarmID = ANode->Values[0];

    Value = GetAlarmIDText(ulAlarmID);
}
//---------------------------------------------------------------------------
void __fastcall TFormAlarmCfg::ColumnAlarmLevelGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[1].IsNull())
    {
        return;
    }

    UINT32  ulAlarmLevel = (UINT32)ANode->Values[1];

    Value = GetAlarmLevelText(ulAlarmLevel);
}
//---------------------------------------------------------------------------
void __fastcall TFormAlarmCfg::ColumnAlarmStatusGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[2].IsNull())
    {
        return;
    }

    BOOL bIgnore  = (BOOL)ANode->Values[2];

    if (bIgnore)
    {
        Value = "忽略";
    }
    else
    {
        Value = "接受";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormAlarmCfg::pmiSetClick(TObject *Sender)
{
	if (tlAlarmCfg->SelectionCount == 0)
	{
		return;
	}

	TcxTreeListNode *Node = tlAlarmCfg->Selections[0];
	if(Node->Values[0].IsNull())
	{
		return;
	}

	UINT32  ulDevType = OMC_DEV_TYPE_ALL;
	g_DictDevType.GetID(cbDevType, &ulDevType);
	DialogSetAlarmLevel->Init(ulDevType, Node->Values[0], Node->Values[1]);
	if (DialogSetAlarmLevel->ShowModal() != mrOk)
	{
		return;
	}

	UINT32 ulSetAlarmID =  Node->Values[0];
	UINT32 ulSetAlarmLevel = 0;
	BOOL   bIgnore;

	DialogSetAlarmLevel->GetSetAlarmCfg(ulSetAlarmLevel,bIgnore);

	GJsonParam  ReqJsonParam;

	ReqJsonParam.Add("dev_type", ulDevType);
	ReqJsonParam.Add("alarm_id", ulSetAlarmID);
	ReqJsonParam.Add("alarm_level", ulSetAlarmLevel);
	ReqJsonParam.Add("alarm_status", bIgnore);

	if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_SetAlarmLevelCfg, ReqJsonParam))
	{
		WarnMsg("操作失败");
		return;
	}

	InfoMsg("操作成功");

	btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormAlarmCfg::pmiAddClick(TObject *Sender)
{
	UINT32  ulSetDevType = OMC_DEV_TYPE_ALL;
	g_DictDevType.GetID(cbDevType, &ulSetDevType);
	DialogSetAlarmLevel->Init(ulSetDevType);
	if (DialogSetAlarmLevel->ShowModal() != mrOk)
	{
		return;
	}

	UINT32 ulSetAlarmID = 0;
	UINT32 ulSetAlarmLevel = 0;
	BOOL   bSetAlarmStatus = FALSE;

	DialogSetAlarmLevel->GetSetAlarmCfg(ulSetAlarmID, ulSetAlarmLevel, bSetAlarmStatus);

	VECTOR<ALARM_LEVEL_CFG_T>   vAlarmLevelCfg;
	Alarm::GetAlarmLevelCfg(vAlarmLevelCfg);
	for (UINT32 i=0; i<vAlarmLevelCfg.size(); i++)
	{
		if ( vAlarmLevelCfg[i].ulDevType == ulSetDevType &&
			 vAlarmLevelCfg[i].ulAlarmID == ulSetAlarmID )
		{
			WarnMsg("告警配置已存在");
			return;
		}
	}

	GJsonParam  ReqJsonParam;

	ReqJsonParam.Add("dev_type", ulSetDevType);
	ReqJsonParam.Add("alarm_id", ulSetAlarmID);
	ReqJsonParam.Add("alarm_level", ulSetAlarmLevel);
	ReqJsonParam.Add("alarm_status", bSetAlarmStatus);

	if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_AddAlarmLevelCfg, ReqJsonParam))
	{
		ErrMsg("操作失败");
		return;
	}

	InfoMsg("操作成功");

	btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormAlarmCfg::tlAlarmCfgCustomDrawDataCell(TcxCustomTreeList *Sender,
          TcxCanvas *ACanvas, TcxTreeListEditCellViewInfo *AViewInfo,
          bool &ADone)
{
    if (AViewInfo->Node && !AViewInfo->Node->Values[1].IsNull() &&
        AViewInfo->Column->ItemIndex == 1 )
    {
        UINT32  ulAlarmLevel = AViewInfo->Node->Values[1];
        TColor  AlarmColor = GetAlarmColor(ulAlarmLevel);

        if (AlarmColor != 0)
        {
            ACanvas->Canvas->Brush->Color = AlarmColor;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormAlarmCfg::OnGetAlarmLevelRsp(UINT32 ulSeqID, GJson &Json)
{
    GJson           *pSubJson;
	VECTOR<GJson*>  &vSubJson = Json.GetSubJson();
    VECTOR<ALARM_LEVEL_CFG_T>    vAlarmCfg;

	for (UINT32 i=0; i<vSubJson.size(); i++)
	{
		ALARM_LEVEL_CFG_T   stCfg = {0};
        pSubJson = vSubJson[i];

		if (!pSubJson->GetValue("dev_type", &stCfg.ulDevType) ||
			!pSubJson->GetValue("alarm_id", &stCfg.ulAlarmID) ||
            !pSubJson->GetValue("alarm_level", &stCfg.ulAlarmLevel) ||
            !pSubJson->GetValue("alarm_status", &stCfg.bIsIgnore))
        {
            GosLog(LOG_ERROR, "parse alarm level failed: %s", pSubJson->GetText());
            return ;
        }

		vAlarmCfg.push_back(stCfg);
    }

    Alarm::InitAlarmLevelCfg(vAlarmCfg);
	UINT32 ulVecSize=   vSubJson.size();
	InitView();
}
//---------------------------------------------------------------------------
void __fastcall TFormAlarmCfg::FormCreate(TObject *Sender)
{
	SetImage(btnAdd, IMG_ADD);
	SetImage(btnDel, IMG_DEL);
	SetImage(btnSet, IMG_SET);
    SetImage(btnRefresh, IMG_REFRESH);
    SetImage(pmiSet, IMG_SET);
    SetImage(pmiRefresh, IMG_REFRESH);
}
//---------------------------------------------------------------------------
void __fastcall TFormAlarmCfg::cbDevTypePropertiesChange(TObject *Sender)
{
	InitView();
}
//---------------------------------------------------------------------------

void __fastcall TFormAlarmCfg::btnDelClick(TObject *Sender)
{
	if (tlAlarmCfg->SelectionCount == 0)
	{
		return;
	}

	TcxTreeListNode *Node = tlAlarmCfg->Selections[0];
	if(Node->Values[0].IsNull())
	{
		return;
	}

	UINT32  ulDevType = OMC_DEV_TYPE_ALL;
	UINT32  ulAlarmID =  Node->Values[0];

	g_DictDevType.GetID(cbDevType, &ulDevType);
	AnsiString  strDevName = cbDevType->Text;
	AnsiString  strAlarmName = Node->Texts[0];

	if (!AskMsg((CHAR*)"删除 %s设备的\n%s 配置吗?", strDevName.c_str(), strAlarmName.c_str()))
	{
		return;
    }

    GJsonParam  ReqJsonParam;
    GJson       RspJson;

	ReqJsonParam.Add("dev_type", ulDevType);
	ReqJsonParam.Add("alarm_id", ulAlarmID);

	if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_DelAlarmLevelCfg, ReqJsonParam))
    {
        WarnMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
	btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormAlarmCfg::btnRefreshClick(TObject *Sender)
{
	static UINT32 ulLastTime = 0;
	UINT32  ulCurrTime = gos_get_uptime_1ms();

	if (ulLastTime == 0 ||(ulLastTime+1000) < ulCurrTime)
	{
	   SendServer(EV_OMT_OMC_REQ, MSG_OMT_GetAlarmLevelCfg);
	   ulLastTime = gos_get_uptime_1ms();
	}
}
//---------------------------------------------------------------------------

