#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "ds_public.h"
#include "main.h"
#include "GTransFile.h"
#include "FrmInit.h"
#include "FrmHome.h"
#include "FrmDevList.h"
#include "FrmSystemManagement.h"
#include "FrmTopo.h"
#include "FrmCfg.h"
#include "FrmAlarm.h"
#include "FrmHistoryAlarm.h"
#include "FrmAlarmReport.h"
#include "FrmPerf.h"
#include "FrmLogin.h"
#include "DlgSetNe.h"
#include "FrmNeInfo.h"
#include "FrmOperLog.h"
#include "FrmDevFile.h"
#include "FrmSwCfg.h"
#include "DlgSetSw.h"
#include "FrmSwUpdate.h"
#include "FrmTextReader.h"
#include "FrmMediaPlayer.h"
#include "FrmAlarmCfg.h"
#include "FrmActiveAlarm.h"
#include "DlgSetAlarmLevel.h"
#include "DlgImportFile.h"
#include "FrmUserCfg.h"
#include "DlgSetOmtOperator.h"
#include "FrmAlarmCfg.h"
#include "FrmDevInfo.h"
#include "FrmVersion.h"
#include "DlgLock.h"
#include "DlgExit.h"
#include "FrmLog.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxControls"
#pragma link "cxCustomData"
#pragma link "cxDropDownEdit"
#pragma link "cxGraphics"
#pragma link "cxInplaceContainer"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxPC"
#pragma link "cxStyles"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"

#pragma link "dxSkinsCore"
#pragma link "dxSkinscxPCPainter"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxStatusBar"
#pragma link "dxSkinsForm"
#pragma link "cxRadioGroup"
#pragma link "cxContainer"
#pragma link "cxEdit"
#pragma link "cxLabel"
#pragma link "cxGroupBox"
#pragma link "cxCheckBox"
#pragma link "cxButtonEdit"
#pragma link "cxMaskEdit"
#pragma link "cxTextEdit"
#pragma link "cxVGrid"
#pragma link "dxBar"
#pragma link "cxMemo"
#pragma link "cxImage"
#pragma link "dxGDIPlusClasses"
#pragma link "cxCheckComboBox"
#pragma link "dximctrl"
#pragma link "cxTrackBar"
#pragma link "cxHeader"
#pragma link "cxTreeView"
#pragma link "MDIForm"
#pragma link "cxImageComboBox"
#pragma link "AdvSmoothLabel"
#pragma link "cxClasses"
#pragma link "cxImageList"
#pragma link "dxBarBuiltInMenu"

#pragma link "dxSkinBlueprint"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinSeven"
#pragma link "dxSkinSharp"
#pragma link "dxSkinBlue"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinOffice2007Blue"
#pragma link "dxSkinOffice2010Blue"
#pragma link "dxSkinVisualStudio2013Blue"
#pragma link "dxSkinVisualStudio2013Light"
#pragma link "dxSkinOffice2013DarkGray"
#pragma link "dxSkinDevExpressStyle"
#pragma link "dxSkinVS2010"
#pragma link "dxSkinMoneyTwins"
#pragma link "dxSkinLilian"
#pragma link "dxSkinGlassOceans"

#pragma link "cxSplitter"
#pragma link "cxLocalization"
#pragma link "dxBevel"
#pragma link "AdvSmoothPanel"
#pragma link "cxHint"
#pragma link "dxCustomHint"
#pragma link "dxScreenTip"
#pragma link "AdvPicture"
#pragma link "AdvSmoothSlider"
#pragma link "AdvSmoothButton"
#pragma resource "*.dfm"
TFormMain *FormMain;

CHAR        *g_szVersion = (CHAR*)"v2.1.2";
AnsiString  g_strAppTitle = "轨道交通无线集群综合网管系统";

BOOL    g_bConnectToServer = TRUE;
BOOL    g_bShowTitle = FALSE;
BOOL    g_bSilentExit = FALSE;
BOOL    g_bClosing = FALSE;
BOOL    g_bInitAllForm = FALSE;
UINT8   g_aucLocalIP[4] = {0,0,0,0};
TColor  g_ColorBackground = (TColor)RGB(80,100,136); //RGB(80,100,136);//102,145,196);  68 90 126
TColor  g_ColorLeftBackground = (TColor)RGB(90,123,165);//119,140,172); //RGB(80,100,136);//102,145,196);  68 90 126
TdxBarLargeButton *g_btnCurrChecked = NULL;

extern UINT32   g_usTransFilePort;
extern UINT32   g_usLocalTransFilePort;
GTransFile      *g_TransFile = NULL;
UINT8           g_aucDISClientAddr[4] = {0,0,0,0};
extern NTP_CFG_T        g_stNTPCfg;
AnsiString g_strToolbarFontName = "微软雅黑";
AnsiString g_strStatusbarFontName = "宋体";

extern UINT32      g_ulOperType;
extern CHAR        g_acOMCVersion[32];
extern VOID ClientLogout();
extern VOID SaveDCOperator(CHAR *szOperator, BOOL bOffDuty);
static bool g_bWinLocked = false;
static BOOL     g_bNTPValid = TRUE;

void ClientConnectionChange(bool bConnected)
{
    if (!FormMain)
    {
        return;
    }

    UINT32  ulConnected = bConnected?1:0;

    PostMessage(FormMain->Handle, EV_CONNECTION_CHANGE_MSG, 0, (LPARAM)ulConnected);
}

VOID NTPSync()
{
    static UINT32   m_ulLastNTPSyncTime = 0;
    UINT32          ulCurrTime = gos_get_uptime();
    UINT8           aucLocalAddr[4] = {127,0,0,1};
    static UINT32   ulNTPSyncFailNum = 0;
    UINT32          ulMaxNTPSyncFailNum = 5;

    if (g_stNTPCfg.ulNTPSyncPeriod == 0)
    {
        return;
    }

    if (m_ulLastNTPSyncTime > 0 &&
        (ulCurrTime-m_ulLastNTPSyncTime) < g_stNTPCfg.ulNTPSyncPeriod)
    {
        return;
    }

    if (gos_is_local_ip(g_stNTPCfg.aucNTPAddr) ||
        GET_INT(g_stNTPCfg.aucNTPAddr) == GET_INT(aucLocalAddr))
    {
        return;
    }

    INT32   iTime = gos_get_ntp_time(g_stNTPCfg.aucNTPAddr, g_stNTPCfg.usNTPPort);

    if (iTime < 0)
    {
        GosLog(LOG_WARN, "connect to NTP server " IP_FMT " failed!", IP_ARG(g_stNTPCfg.aucNTPAddr));

        ulNTPSyncFailNum++;
        if (ulNTPSyncFailNum >= ulMaxNTPSyncFailNum)
        {
            //SetNTPValid(FALSE);
            g_bNTPValid = FALSE;
        }

        return;
    }

    //SetNTPValid(TRUE);
    g_bNTPValid = TRUE;
    ulNTPSyncFailNum = 0;

    INT32   iTimeDiff = abs((INT32)(iTime - gos_get_current_time()));

    if (iTimeDiff < 10)
    {
        m_ulLastNTPSyncTime = ulCurrTime;
        return;
    }

    GosLog(LOG_INFO, "the difference between local time and ntp server time is %u", iTimeDiff);

    if (m_ulLastNTPSyncTime == 0)
    {
        GosLog(LOG_INFO, "sync time with NTP server");
    }

    m_ulLastNTPSyncTime = ulCurrTime;
    if (!gos_set_localtime(iTime))
    {
        GosLog(LOG_ERROR, "NTP set localtime failed!");

        return;
    }

    GosLog(LOG_INFO, "sync time with NTP server succ");
}

GOS_THREAD_RET NTPSyncThreadEntry(VOID *pvData)
{
    while(!g_bSilentExit)
    {
        NTPSync();
        gos_sleep(1);
    }

    return 0;
}

void CreateOneForm(System::Classes::TComponentClass InstanceClass, void *Form, bool bSetParent=false)
{
    Application->CreateForm(InstanceClass, Form);

//  if (bSetParent && FormMain)
//    {
//      SetFormParent(*((TForm**)Form), FormMain->PanelWork);
//    }

    SetInitInfo("系统界面");
}

void CreateDialogLockForm(System::Classes::TComponentClass InstanceClass, void *Form, bool bSetParent=false)
{
    Application->CreateForm(InstanceClass, Form);
}

void CreateAllForm()
{
//  CreateOneForm(__classid(TFormHome), &FormHome);
    CreateOneForm(__classid(TFormDevList), &FormDevList, true);
    CreateOneForm(__classid(TFormCfg), &FormCfg, true);
    CreateOneForm(__classid(TFormVersion), &FormVersion, true);
    CreateOneForm(__classid(TFormHistoryAlarm), &FormHistoryAlarm, true);
    CreateOneForm(__classid(TFormAlarmReport), &FormAlarmReport, true);
    CreateOneForm(__classid(TFormPerf), &FormPerf, true);
    CreateOneForm(__classid(TDialogSetNe), &DialogSetNe);
    CreateOneForm(__classid(TFormNeInfo), &FormNeInfo);
    CreateOneForm(__classid(TFormOperLog), &FormOperLog, true);
    CreateOneForm(__classid(TFormDevFile), &FormDevFile, true);
    CreateOneForm(__classid(TFormSwCfg), &FormSwCfg, true);
    CreateOneForm(__classid(TDialogSetSw), &DialogSetSw);
    CreateOneForm(__classid(TFormSwUpdate), &FormSwUpdate, true);
    CreateOneForm(__classid(TFormTextReader), &FormTextReader);
    CreateOneForm(__classid(TFormMediaPlayer), &FormMediaPlayer);
	CreateOneForm(__classid(TFormAlarmCfg), &FormAlarmCfg);
    CreateOneForm(__classid(TFormActiveAlarm), &FormActiveAlarm);
    CreateOneForm(__classid(TDialogSetAlarmLevel), &DialogSetAlarmLevel);
    CreateOneForm(__classid(TFormAlarm), &FormAlarm,true);
    CreateOneForm(__classid(TDialogImportFile), &DialogImportFile);
    CreateOneForm(__classid(TFormUserCfg), &FormUserCfg,true);
    CreateOneForm(__classid(TDialogSetOmtOperator), &DialogSetOmtOperator);
    CreateOneForm(__classid(TFormDevInfo), &FormDevInfo);
	CreateOneForm(__classid(TFormLog), &FormLog);

	if (g_ulOperType == OPER_TYPE_ADMIN)
    {
        CreateOneForm(__classid(TFormSystem), &FormSystem, true);
    }

    if (!FormMsgDialog)
    {
        Application->CreateForm(__classid(TFormMsgDialog), &FormMsgDialog);
    }

    GosLog(LOG_INFO, "Init all form successful");
}

void SetFormParent(TForm *Child, TWinControl *Parent)
{
    if (!Child)
    {
        return;
    }

    Child->Visible = false;

    Child->Parent = Parent;
    Child->Align = alClient;
    Child->BorderIcons >> biSystemMenu;
    Child->BorderIcons >> biMinimize;
    Child->BorderIcons >> biMaximize;

    Child->FormStyle = Forms::fsNormal;

    Child->BorderStyle = Forms::bsNone;
    Child->WindowState = wsMaximized;

    Child->Visible = true;
}

//---------------------------------------------------------------------------
void CheckBtn(TdxBarLargeButton *btn)
{
    if (g_btnCurrChecked == btn)
    {
        return;
    }

    if (g_btnCurrChecked)
    {
        g_btnCurrChecked->Down = false;
    }

    g_btnCurrChecked = btn;
    g_btnCurrChecked->Down = true;
}
//---------------------------------------------------------------------------
TcxButton* __fastcall TFormMain::GetToolbarButton(PAGE_E enPage)
{
    if (enPage == PAGE_HOME)            return btnHome;
    if (enPage == PAGE_CFG  )           return btnCfg;
    if (enPage == PAGE_ALARM)           return btnAlarm;
    if (enPage == PAGE_PERF )           return btnPerf;
//  if (enPage == PAGE_SYS  )           return btnSys;
    if (enPage == PAGE_DEV_FILE)        return btnDevFile;
    if (enPage == PAGE_SW_UPDATE)       return btnSw;
    if (enPage == PAGE_SYS_CFG)         return btnSysCfg;
    if (enPage == PAGE_USERMANGE)       return btnOmtMange;
    if (enPage == PAGE_STATIONCFG)      return btnStationCfg;

    return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ActiveForm(TForm *Form)
{
    if (!Form)
    {
        return;
    }

    if (!Form->Visible)
    {
        Form->Visible = true;
    }

    TWinControl *Parent = PanelWork;

    SetFormParent(Form, Parent);

    Form->BringToFront();
}

PAGE_E      g_enCurrPage = PAGE_NONE;
PAGE_E      g_enLastPage = PAGE_NONE;

//---------------------------------------------------------------------------
void __fastcall TFormMain::SetStatusButtonStatus(TAdvSmoothPanel *btn, bool bConnected)
{
    btn->ShowHint = true;

    if (bConnected)
    {
        btn->Fill->Picture = PanelStatusNormal->Fill->Picture;
        btn->Hint = "正常";
    }
    else
    {
        btn->Fill->Picture = PanelStatusError->Fill->Picture;
        btn->Hint = "断开";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::BackPage()
{
    ActivePage(g_enLastPage);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ActivePage(PAGE_E enPage)
{
    TcxButton   *btn = GetToolbarButton(enPage);

    if (!btn)
    {
        return;
    }

    btn->LookAndFeel->SkinName = g_strLeftToolbarButtonSkinName;

    g_enLastPage = g_enCurrPage;
    g_enCurrPage = enPage;

    btn->Down = true;

    if (enPage == PAGE_HOME)        ActiveForm(FormDevList);//FormHome);
    if (enPage == PAGE_CFG)         ActiveForm(FormCfg);
    if (enPage == PAGE_ALARM)       ActiveForm(FormAlarm);
    if (enPage == PAGE_PERF)        ActiveForm(FormPerf);
    if (enPage == PAGE_DEV_FILE)    ActiveForm(FormDevFile);
    if (enPage == PAGE_SW_UPDATE)   ActiveForm(FormSwCfg);
	if (enPage == PAGE_USERMANGE)   ActiveForm(FormUserCfg);
	if (enPage == PAGE_SYS_CFG)     ActiveForm(FormSystem);

}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnClientConnectionChangeMsg(TMessage Message)
{
    g_bConnectToServer = (BOOL)Message.LParam;

    SetStatusButtonStatus(btnOMCStatus, g_bConnectToServer);
}

void __fastcall TFormMain::InitAllCtrl()
{
    TWinControl *Parent = PanelWork;

    SetFormParent(FormDevList, Parent);
    SetFormParent(FormCfg, Parent);
    SetFormParent(FormVersion, Parent);
    SetFormParent(FormHistoryAlarm, Parent);
    SetFormParent(FormAlarmReport, Parent);
    SetFormParent(FormPerf, Parent);
    SetFormParent(FormDevFile, Parent);
    SetFormParent(FormSwCfg, Parent);
    SetFormParent(FormSwUpdate, Parent);
    SetFormParent(FormAlarm, Parent);
	SetFormParent(FormUserCfg, Parent);
	SetFormParent(FormSystem, Parent);

    TcxButton   *ToolbarBtn[] = {btnHome, btnCfg, btnAlarm, btnPerf, btnDevFile,btnSysCfg, btnExit};
    int         iLeft = 20;
    UINT32      i;
    int         iScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int         iWidth = 110;

    if (iScreenWidth < 1600)
    {
        iWidth = 90;
    }

    for ( i=0; i<ARRAY_SIZE(ToolbarBtn); i++)
    {
        if (ToolbarBtn[i]->Visible)
        {
            ToolbarBtn[i]->TabOrder = i;
            ToolbarBtn[i]->Left = iLeft;
            ToolbarBtn[i]->Width = iWidth;
            ToolbarBtn[i]->Height = 74;
            ToolbarBtn[i]->Top = (ToolbarBtn[i]->Parent->Height - ToolbarBtn[i]->Height)/2;

            iLeft += iWidth + 20;

            ToolbarBtn[i]->Font->Name = g_strToolbarFontName;
            ToolbarBtn[i]->Font->Size = 14;
            ToolbarBtn[i]->LookAndFeel->SkinName = g_strToolbarButtonSkinName;
        }
    }

    btnBackPage->TabOrder = 100;
    btnBackPage->Top = ToolbarBtn[0]->Top;
    btnBackPage->Width = ToolbarBtn[0]->Width;
    btnBackPage->Height = ToolbarBtn[0]->Height;
    btnBackPage->Font->Name = g_strToolbarFontName;
    btnBackPage->Font->Size = 14;
    btnBackPage->LookAndFeel->SkinName = g_strToolbarButtonSkinName;
    btnBackPage->SpeedButtonOptions->GroupIndex = 0;

    btnSysCfg->Visible = false;

	if (g_ulOperType == OPER_TYPE_ADMIN ||
		g_ulOperType == OPER_TYPE_MANAGE)
    {
        btnSysCfg->Visible = true;
    }

    // status bar
    TWinControl     *StatusbarBtn[] = {btnOMCVer, btnVer, btnCurrUser, btnOMCStatus, btnNTPStatus, btnCurrTime};
    int         aiLeft[] = {5,200,460,580,700,1750};
    int         iHeight = 24;
    int         iTop = (PanelStatusBar->Height - iHeight)/2 + 2;

    for (i=0; i<ARRAY_SIZE(StatusbarBtn); i++)
    {
        if (!StatusbarBtn[i]->Visible)
        {
            continue;
        }

        StatusbarBtn[i]->Top = iTop;
        StatusbarBtn[i]->Height = iHeight;
        StatusbarBtn[i]->Left = aiLeft[i];

        if ((i+1) < ARRAY_SIZE(StatusbarBtn))
        {
            StatusbarBtn[i]->Width = aiLeft[i+1] - aiLeft[i];
        }
    }
}

void __fastcall TFormMain::InitSkin()
{
    int     iFontSize = 11;
    UINT32  i;

    {
        TWinControl     *Btn[] = {btnCurrUser, btnVer, btnOMCStatus, btnNTPStatus,  btnCurrTime};
        AnsiString      strClassName;

        for (i=0; i<ARRAY_SIZE(Btn); i++)
        {
            strClassName = Btn[i]->ClassName();
            if (strClassName == "TcxButton")
            {
                ((TcxButton*)Btn[i])->Font->Size = iFontSize;
                ((TcxButton*)Btn[i])->Font->Name = g_strStatusbarFontName;
                ((TcxButton*)Btn[i])->LookAndFeel->SkinName = g_strStatusbarButtonSkinName;
            }
            else if (strClassName == "TAdvSmoothPanel")
            {
                ((TAdvSmoothPanel*)Btn[i])->Caption->Font->Size = iFontSize;
                ((TAdvSmoothPanel*)Btn[i])->Caption->Font->Name = g_strStatusbarFontName;
            }
        }
    }

    {
        TcxButton   *Btn[] = {btnHome, btnCfg, btnAlarm, btnPerf, btnSysCfg, btnHelp, btnExit};

        for (i=0; i<ARRAY_SIZE(Btn); i++)
        {
            Btn[i]->LookAndFeel->SkinName = g_strToolbarButtonSkinName;
        }
    }

    btnMinimize->LookAndFeel->SkinName = "VisualStudio2013Light";
}

//---------------------------------------------------------------------------
 void __fastcall TFormMain::SetBtnDisenable()
 {
    btnSw->Enabled        = false;
    //btnLocalCfg->Enabled  = false;
 }

//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TSubForm(Owner)
{
    this->Visible = false;

    g_TransFile = new GTransFile(g_usLocalTransFilePort, TRUE);
    if (!g_TransFile->Init())
    {
        GosLog(LOG_ERROR, "init TransFile failed");
        CloseApp();
        return ;
    }
    g_TransFile->Start();

    g_pRemoteMsg->GetClientAddr(g_aucDISClientAddr, NULL);

    PanelStatusNormal->Visible= false;
    PanelStatusError->Visible= false;
    PanelStatusDisable->Visible= false;

    btnCurrUser->Caption->Text = AnsiString("用户:") + AnsiString(g_stLocalCfg.acUserName);

    PanelToolbar->Fill->Color = g_ColorBackground;
    PanelToolbar->Fill->ColorTo = g_ColorBackground;
    PanelStatusBar->Fill->Color = g_ColorBackground;
    PanelStatusBar->Fill->ColorTo = g_ColorBackground;
    PanelLogo->Fill->Color = g_ColorBackground;
    PanelLogo->Fill->ColorTo = g_ColorBackground;

//  g_strAppTitle += AnsiString(" - ") + GetDCTypeName(g_ulDCType);
    Application->Title = g_strAppTitle;
    this->Caption = Application->Title;
    WindowState = wsMaximized;
    Application->ShowMainForm = FALSE;

    //PanelBottom->Height = 48;
    ShowCurrentTime();

    if (g_bShowTitle)
    {
        this->BorderStyle = Forms::bsSizeable;
        btnMinimize->Visible = false;
    }
    else
    {
        this->BorderStyle = Forms::bsNone;
        btnMinimize->Visible = true;
        this->Left = 0;
        this->Top = 0;
    }

//  if (g_stLocalCfg.ulOmtUserType == OMT_USER_TYPE_ROUSER)
//  {
//      SetBtnDisenable();
//  }

    InitMsgHandler(EV_OMT_OMC_RSP, MSG_DCConfirmSave, OnConfirmSaveRsp);
    InitMsgHandler(EV_OMT_OMC_RSP, MSG_OMC_KickOutOMT, OnOMCKickOutOMTReq);
    GosLog(LOG_INFO, "init main form successful");
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerInitAllFormTimer(TObject *Sender)
{
    if (!TimerInitAllForm->Enabled)
    {
        return;
    }

    TimerInitAllForm->Enabled = false;

    if (!g_bInitAllForm)
    {
        GosLog(LOG_INFO, "create all form");
        CreateAllForm();
        g_bInitAllForm = TRUE;
    }

    TimerInit->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerInitTimer(TObject *Sender)
{
    if (!TimerInit->Enabled)
    {
        return;
    }

    TimerInit->Enabled = false;

    InitAllCtrl();

    SetInitOver();

    if (FormInit)
    {
        delete FormInit;
    }

    //FormLogin->ModalResult = mrOk;

    InnerMsgReady();

    btnVer->Caption->Text = AnsiString("终端版本:") + AnsiString(g_szVersion);
    btnOMCVer->Caption->Text = AnsiString("服务器版本:") + AnsiString(g_acOMCVersion);
    char    acTime[128];
    gos_get_text_time_ex(NULL, "%04d%d月%d日 %d:%02d:%02d", acTime);

    ActivePage(PAGE_HOME);

    InitSkin();
    this->Visible = TRUE;

    gos_create_thread(NTPSyncThreadEntry, 4*1024, NULL);

    UINT32  ulAutoLockPeriod = GetAutoLockPeriod();
    TimerAutoLock->Interval = ulAutoLockPeriod*1000;
    TimerAutoLock->Enabled = (ulAutoLockPeriod > 0);

    GosLog(LOG_INFO, "omt start succfully");
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SetServerStatus(bool bConnected)
{
    SetStatusButtonStatus(btnOMCStatus, bConnected);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ShowCurrentTime()
{
    char    acTime[128];

    gos_get_text_time_ex(NULL, "%04d-%02d-%02d  %02d:%02d:%02d", acTime);    // 日期和时间中间留2个空格

    btnCurrTime->Caption->Text = acTime;
}
void __fastcall TFormMain::TimerCheckTimer(TObject *Sender)
{
    SetStatusButtonStatus(btnNTPStatus, g_bNTPValid);
    ShowCurrentTime();

    SendConfirmRequest();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (!g_bSilentExit && !AskMsg("退出网管终端吗？"))
    {
        Action = caNone;
        return;
    }

    g_bClosing = true;
    this->Visible = false;

    ClientLogout();
    CloseApp();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerExitTimer(TObject *Sender)
{
    g_bSilentExit = TRUE;
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnMinimizeClick(TObject *Sender)
{
    Application->Minimize();
    btnEmpty->Down = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::Exit()
{
    g_bClosing = true;
    this->Visible = false;

    ClientLogout();
    gos_sleep_1ms(100);
    CloseApp();
}

void __fastcall TFormMain::btnExitClick(TObject *Sender)
{
    //Close();
    if (!DialogExit)
    {
        Application->CreateForm(__classid(TDialogExit), &DialogExit);
    }

    DialogExit->Init();
    DialogExit->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnHomeClick(TObject *Sender)
{
    ActivePage(PAGE_HOME);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnCfgClick(TObject *Sender)
{
    ActivePage(PAGE_CFG);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnAlarmClick(TObject *Sender)
{
    ActivePage(PAGE_ALARM);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnPerfClick(TObject *Sender)
{
    ActivePage(PAGE_PERF);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnSysCfgClick(TObject *Sender)
{
    ActivePage(PAGE_SYS_CFG);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnDevFileClick(TObject *Sender)
{
    ActivePage(PAGE_DEV_FILE);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnSwClick(TObject *Sender)
{
    ActivePage(PAGE_SW_UPDATE);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnStationCfgClick(TObject *Sender)
{
     ActivePage(PAGE_STATIONCFG);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnToolbarMouseEnter(TObject *Sender)
{
    TcxButton   *btn = (TcxButton*)Sender;

    btn->LookAndFeel->SkinName = "Blueprint";
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnToolbarMouseLeave(TObject *Sender)
{
    TcxButton   *btn = (TcxButton*)Sender;

    btn->LookAndFeel->SkinName = g_strToolbarButtonSkinName;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnBackPageClick(TObject *Sender)
{
    BackPage();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnConfirmSaveRsp(UINT32 ulSeqID, GJson &Json)
{
    DeleteConfirmRequest(ulSeqID);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnOMCKickOutOMTReq(UINT32 ulSeqID, GJson &Json)
{
    GosLog(LOG_FATAL, "same omt user logined, auto close this omt");

    InfoMsg("本用户正在其他终端登录，\n系统将自动退出!");

    TimerExit->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::HandleException(TObject *Sender, Exception *E)
{
    AnsiString strErrInfo = E->Message;

    GosLog(LOG_FATAL, "system exception: %s", strErrInfo.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormCreate(TObject *Sender)
{
    Application->OnException = HandleException;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnLeftToolbarMouseEnter(TObject *Sender)
{
    TcxButton   *btn = (TcxButton*)Sender;

    btn->LookAndFeel->SkinName = "Blueprint";
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnLeftToolbarMouseLeave(TObject *Sender)
{
    TcxButton   *btn = (TcxButton*)Sender;

    btn->LookAndFeel->SkinName = g_strLeftToolbarButtonSkinName;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnOmtMangeClick(TObject *Sender)
{
    ActivePage(PAGE_USERMANGE);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::LockWindow()
{
    if (g_bWinLocked)
    {
        return;
    }

    g_bWinLocked = true;
    GosLog(LOG_INFO, "auto lock start");

    TimerAutoLock->Enabled = false;

    Application->CreateForm(__classid(TDialogLock), &DialogLock);
    //CreateDialogLockForm(__classid(TDialogLock), &DialogLock);
    this->Enabled = false;

    DialogLock->Init();
    DialogLock->ShowModal();
    DialogLock->FormStyle = fsStayOnTop;

    delete DialogLock;
    DialogLock = NULL;

    this->Enabled = true;
    TimerAutoLock->Enabled = true;
    g_bWinLocked = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerAutoLockTimer(TObject *Sender)
{
    LockWindow();
}
//---------------------------------------------------------------------------

