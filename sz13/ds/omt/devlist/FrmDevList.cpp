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
#include "FrmActiveAlarm.h"
#include "FrmDevInfo.h"
#include "FrmNeInfo.h"
#include "FrmDevList.h"
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
#pragma link "AdvSmoothPanel"
#pragma link "cxButtons"
#pragma link "cxImageList"
#pragma link "cxClasses"
#pragma link "dxBar"
#pragma resource "*.dfm"
TFormDevList *FormDevList;

int     m_iTopMargin = 10;
int     m_iRowMargin = 10;
int     m_iDevHeight = 112;
int     m_iDevWidth = 88;
AnsiString  g_strActiveTrainSkinName = "VisualStudio2013Light";
AnsiString  g_strInterLockStationSkinName = "VisualStudio2013Blue"; //"DevExpressDarkStyle";
AnsiString  g_strStationSkinName = "Office2010Blue";
//---------------------------------------------------------------------------
__fastcall TFormDevList::TFormDevList(TComponent* Owner)
    : TSubForm(Owner)
{
#ifdef HAVE_TAU
	btnTAU->Visible = true;
#endif
    InitDevList();
    InitMsgHandler(EV_OMT_OMC_RSP, MSG_OMT_GetClusterStatus, OnGetClusterStatusRsp);
}
//---------------------------------------------------------------------------
BOOL HasActiveAlarm(CHAR *szNeID)
{
    VectorMap<AnsiString, ALARM_INFO_T> mActiveAlarm;
    Alarm::GetActiveAlarm(mActiveAlarm);

    for (UINT32 i=0; i<mActiveAlarm.Size(); i++)
    {
        ALARM_INFO_T *pstInfo =  mActiveAlarm.GetValueByIndex(i);
        if (strcmp(pstInfo->acNeID,szNeID) == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TFormDevList::InitView(TcxButton *btnDev, UINT32 ulDevType)
{
    std::map<UINT32, UINT32> mDevAlarmCount;
    std::map<UINT32, UINT32> mDevOnlineCount;

    Alarm::GetDevAlarmCount(mDevAlarmCount);
    NEBasic::GetDevOnlineCount(mDevOnlineCount);

    if (mDevOnlineCount[ulDevType] <= 0)
    {
        // 该种类设备无在线设备
        btnDev->OptionsImage->ImageIndex = 0;
    }
    else if(mDevAlarmCount[ulDevType] > 0)
    {
        // 该种类设备存在活跃告警
        btnDev->OptionsImage->ImageIndex = 1;
    }
    else
    {
        // 该种类设备有在线设备且无活跃告警
        btnDev->OptionsImage->ImageIndex = 2;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormDevList::InitViewServer(TcxButton *btnDev, CHAR *szNeID, INT32 iOnlinSec, CHAR *szClusterStatus)
{
    if (iOnlinSec < 0)
    {
        // 调度服务器处于离线状态
        btnDev->OptionsImage->ImageIndex = 0;
        return;
    }
    else if(HasActiveAlarm(szNeID))
    {
        // 调度服务器在线，且有活跃告警
        btnDev->OptionsImage->ImageIndex = 1;
        return;
    }
	else if (strcmp(szClusterStatus,"主用") == 0||
			 strcmp(szClusterStatus,"独立运行") == 0)
    {
        // 调度服务器处于主用或者独立运行状态
        btnDev->OptionsImage->ImageIndex = 2;
        return;
    }
    else if (strcmp(szClusterStatus,"备用") == 0)
    {
        // 调度服务器处于备用状态
        btnDev->OptionsImage->ImageIndex = 3;
        return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormDevList::InitDevList()
{
    InitView(btnTX, OMC_DEV_TYPE_TX);
    InitView(btnFX, OMC_DEV_TYPE_FX);
    InitView(btnDC, OMC_DEV_TYPE_DC);
    InitView(btnREC, OMC_DEV_TYPE_REC);
    InitView(btnTAU, OMC_DEV_TYPE_TAU);
}
//---------------------------------------------------------------------------
void __fastcall TFormDevList::TimerRefreshTimer(TObject *Sender)
{
    InitDevList();
    RefreshServerStat();
}
//---------------------------------------------------------------------------
void __fastcall TFormDevList::btnDevMouseEnter(TObject *Sender)
{
    TcxButton   *btn = (TcxButton*)Sender;
    AnsiString   strNeID = btn->Caption.c_str();
    FormNeInfo->Init(strNeID);
    FormNeInfo->Parent = this;
    FormNeInfo->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormDevList::btnDevClick(TObject *Sender)
{
    TcxButton   *btn = (TcxButton*)Sender;

    // 设置鼠标按下效果
    if (!btn->Down)
    {
        btn->LookAndFeel->SkinName = g_strActiveTrainSkinName;
    }
    else
    {
        btn->LookAndFeel->SkinName = GetDefaultSkinName();
    }

    AnsiString strDevSort = btn->Caption;
    FormDevInfo->Init(strDevSort);
    FormDevInfo->Parent = this;
    FormDevInfo->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormDevList::PanelMapResize(TObject *Sender)
{
    int     iLeftSpace = 100;
    int     iTopSpace = 100;

    gbMap->Align = alNone;
    gbMap->Left = iLeftSpace;
    gbMap->Width = gbMap->Parent->Width - iLeftSpace;

    gbMap->Top = iTopSpace;
    gbMap->Height = gbMap->Parent->Height - iTopSpace;
}
//---------------------------------------------------------------------------
void __fastcall TFormDevList::OnGetClusterStatusRsp(UINT32 ulSeqID, GJson &RspJson)
{
    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = RspJson.GetSubJson();

    for (UINT32 i=0; i<vSubJson.size(); ++i)
    {
        CHAR        acNeID[32] = {0};
        CHAR        acDevName[32] = {0};
        CHAR        acClusterStatus[32] = {0};
        INT32       iOnlineSec = -1;

        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("NeID", acNeID, sizeof(acNeID)) ||
            !pSubJson->GetValue("设备名称", acDevName, sizeof(acDevName)) ||
            !pSubJson->GetValue("在线时长", &iOnlineSec) ||
            !pSubJson->GetValue("主备状态", acClusterStatus, sizeof(acClusterStatus)))
        {
            GosLog(LOG_INFO, "invalid msg: %s", RspJson.GetText());
            return;
        }

        if (0 == strcmp(acDevName, "CAD服务器1"))
        {
            InitViewServer(btnCAD1, acNeID, iOnlineSec, acClusterStatus);
        }
        else if (0 == strcmp(acDevName, "CAD服务器2"))
        {
            InitViewServer(btnCAD2, acNeID, iOnlineSec, acClusterStatus);
        }
        else
        {
            GosLog(LOG_ERROR, "invalid CAD name: %s", acDevName);
        }
    }
}

