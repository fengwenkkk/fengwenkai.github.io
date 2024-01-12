#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "ds_public.h"
#include "main.h"
#include "GTransFile.h"
#include "DlgExit.h"
#include "DlgLock.h"
#include "DlgSetPwd.h"

//#include "ThreadOMA.h"

#include "FrmInit.h"
#include "FrmRecManage.h"
#include "FrmLogin.h"

#include "DlgSetSkin.h"

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
#pragma link "trayicon"
#pragma resource "*.dfm"
TFormMain *FormMain;

CHAR        *g_szVersion = (CHAR*)"V2.0.1b";
AnsiString  g_strAppTitle = "无线集群录音录像系统";

BOOL    g_bConnectToServer = TRUE;
BOOL    g_bShowTitle = FALSE;
BOOL    g_bTestMode = FALSE;
BOOL    g_bSilentExit = FALSE;
BOOL    g_bClosing = FALSE;
BOOL    g_bInitAllForm = FALSE;
BOOL    g_bNeedLogin = TRUE;
UINT8   g_aucLocalIP[4] = {0,0,0,0};
TColor  g_ColorBackground = (TColor)RGB(80,100,136); //RGB(80,100,136);//102,145,196);  68 90 126
TColor  g_ColorLeftBackground = (TColor)RGB(90,123,165);//119,140,172); //RGB(80,100,136);//102,145,196);  68 90 126
TdxBarLargeButton *g_btnCurrChecked = NULL;

INT64   g_i64MainGroupID = INVALID_GROUP_ID;
BOOL    g_bIsPTTDown = FALSE;
UINT32  g_ulCheckMDCFailNum = 0;

BOOL    g_bATSValid = TRUE;
BOOL    g_bNTPValid = TRUE;
BOOL    g_bMDCValid = TRUE;

UINT8           g_aucDISClientAddr[4] = {0,0,0,0};

///extern ThreadOma    *g_ThreadOma;

AnsiString g_strToolbarFontName = "微软雅黑";
AnsiString g_strStatusbarFontName = "宋体";

bool g_bWinLocked = false;

extern UINT32       g_ulAutoLockPeriod;
extern UINT8        g_aucNTPAddr[4];
extern UINT16       g_usNTPPort;
extern UINT32       g_ulNTPSyncPeriod;

extern UINT32       g_ulOperType;

extern void CreateAllForm();

extern VOID ClientLogout();

void ClientConnectionChange(bool bConnected)
{
    if (!FormMain)
    {
        return;
    }

    UINT32  ulConnected = bConnected?1:0;

    PostMessage(FormMain->Handle, EV_CONNECTION_CHANGE_MSG, 0, (LPARAM)ulConnected);
}

void FullScreen(TForm *Form)
{
    int cx = GetSystemMetrics(SM_CXSCREEN);
    int cy = GetSystemMetrics(SM_CYSCREEN);

    HWND hWnd = Form->Handle;

    Form->WindowState = wsNormal;
  //    Form->FormStyle = fsStayOnTop;

    Form->Left = 0;
    Form->Top = 0;
    Form->Width = cx;
    Form->Height = cy;
    /*
    SetWindowPos(hWnd, HWND_TOPMOST, 0,0, cx, cy, SWP_FRAMECHANGED);//这里顶层

    ::SetForegroundWindow(GetDesktopWindow());
    ::SetForegroundWindow(hWnd);

    Form->Repaint(); */
}


VOID NTPSync()
{
    static UINT32   m_ulLastNTPSyncTime = 0;
    UINT32          ulCurrTime = gos_get_uptime();
    UINT8           aucLocalAddr[4] = {127,0,0,1};

    if (g_ulNTPSyncPeriod == 0)
    {
        return;
    }

    if (m_ulLastNTPSyncTime > 0 && (ulCurrTime-m_ulLastNTPSyncTime) < g_ulNTPSyncPeriod)
    {
        return;
    }

    if (gos_is_local_ip(g_aucNTPAddr) ||
        GET_INT(g_aucNTPAddr) == GET_INT(aucLocalAddr))
    {
        return;
    }

    INT32   iTime = gos_get_ntp_time(g_aucNTPAddr, g_usNTPPort);

    if (iTime < 0)
    {
        GosLog(LOG_INFO, "connect to NTP server " IP_FMT " failed!", IP_ARG(g_aucNTPAddr));
        g_bNTPValid = FALSE;
        return;
    }

    g_bNTPValid = TRUE;

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
    while(!g_bClosing)
    {
        NTPSync();
        gos_sleep(1);
    }

    return 0;
}

void CreateOneForm(System::Classes::TComponentClass InstanceClass, void *Reference)
{
    Application->CreateForm(InstanceClass, Reference);
    SetInitInfo("系统界面");
}

void CreateAllForm()
{
    if (!FormMsgDialog)
    {
        Application->CreateForm(__classid(TFormMsgDialog), &FormMsgDialog);
    }

    Application->CreateForm(__classid(TDialogSetPwd), &DialogSetPwd);

    GosLog(LOG_INFO, "Init all form successful");
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
void __fastcall TFormMain::OnClientConnectionChangeMsg(TMessage Message)
{
    g_bConnectToServer = (BOOL)Message.LParam;

    SetStatusButtonStatus(btnServerStatus, g_bConnectToServer);

    /*
    if (ulConnected)
    {
        g_bConnected = TRUE;

        MainStatusBar->Panels->Items[2]->Text = "连接状态：正常";
        MainStatusBar->Panels->Items[2]->PanelStyle->Font = cxStyleConnect->Font;

        GosLog(LOG_INFO, "server is connected");

        USR_LOGIN_CHECK_REQ  stReq = {0};
        USR_LOGIN_CHECK_RSP  stRsp = {0};

        UINT32  ulRspLen;

        StrCpy(stReq.acUser, g_strUser);
        StrCpy(stReq.acPwd, g_strUserPasswd);

        if (g_pRemoteMsg->SyncSend(EV_USER_RE_LOGIN_REQ, &stReq, sizeof(stReq), &stRsp, sizeof(stRsp), &ulRspLen, 5))
        {
            NTOHL_IT(stRsp.ulRetCode);
            if (stRsp.ulRetCode == USER_CHECK_OK)
            {
                g_bHasSQL = stRsp.bHasSQL;
            }
            else
            {
                tmrWaitTimer(NULL);
            }
        }
        else
        {
            tmrWaitTimer(NULL);
        }

        ClientUpdate *pClient = ClientUpdate::GetInstance();

        if (!pClient)
        {
            GosLog(LOG_ERROR, "Init ClientUpdate failed (ClientUpdateInit)");
            return;
        }

        if (!pClient->GetClientVerInfo())
        {
            GosLog(LOG_ERROR, "Get Client VerInfo failed (ClientUpdateInit)");
            return;
        }

        if (!CheckVersion())
        {
            GosLog(LOG_ERROR, "Get Client VerInfo failed (ClientUpdateInit)");
            ErrMsg("检测到新版本，客户端需要重启!");
            Application->Terminate();
        }

        btnManuMode->Enabled = TRUE;
        btnAutoMode->Enabled = FALSE;
    }
    else
    {
        g_bConnected = FALSE;

        MainStatusBar->Panels->Items[2]->Text = "连接状态：断开";
        MainStatusBar->Panels->Items[2]->PanelStyle->Font = cxstylDisconnect->Font;

        //tmrWaitTimer(NULL);

        GosLog(LOG_INFO, "server is disconnect...");

        g_FormCfg->SetAllSubNetStatus(NE_DISCONNECT);
    }         */
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

void __fastcall TFormMain::InitAllCtrl()
{
    TWinControl *Parent = PanelWork;

    CreateOneForm(__classid(TFormRecManage), &FormRecManage);
    SetFormParent(FormRecManage, Parent);

    int     iTop = 14;

    btnServerStatus->Top = iTop;
    btnNTPStatus->Top = iTop;
    btnUser->Top = iTop;
    btnVer->Top = iTop;
    btnCurrTime->Top = iTop;
}

void __fastcall TFormMain::InitSkin()
{
    int     iFontSize = 11;
    UINT32  i;

    {
        TWinControl     *Btn[] = {btnServerStatus, btnNTPStatus,
                                  btnVer, btnUser, btnCurrTime};

        AnsiString  strClassName;

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
}

//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TSubForm(Owner)
{
    this->Visible = false;

    g_pRemoteMsg->GetClientAddr(g_aucDISClientAddr, NULL);

    btnUser->Caption->Text = AnsiString("用户:") + AnsiString(g_strDCOperatorName);

    PanelStatusBar->Fill->Color = g_ColorBackground;
    PanelStatusBar->Fill->ColorTo = g_ColorBackground;

    Application->Title = g_strAppTitle;
    this->Caption = Application->Title;
    WindowState = wsMaximized;
    Application->ShowMainForm = false;

    //PanelBottom->Height = 48;

    ShowCurrentTime();

    this->BorderStyle = Forms::bsSizeable;

 // InitMsgHandler(EV_DC_DIS_RSP, MSG_DISKickOffDC, OnDISKickOffDCReq);

    InitMsgHandler(EV_CLIENT_RSP, MSG_DCConfirmSave, OnConfirmSaveRsp);

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
void __fastcall TFormMain::InitRecCli()
{
    SetFormParent(FormRecManage, PanelWork);

    btnNTPStatus->Visible = false;
   //   btnUser->Visible = false;
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

    delete FormInit;
    FormInit = NULL;

    InnerMsgReady();

    btnVer->Caption->Text = AnsiString("版本：") + AnsiString(g_szVersion);

    char    acTime[128];
    gos_get_text_time_ex(NULL, "%04d%d月%d日 %d:%02d:%02d", acTime);

    btnUser->Hint = "登录时间："+AnsiString(acTime+4);

    InitRecCli();

    delete FormLogin;
    FormLogin = NULL;

    Application->ShowMainForm = true;

    InitSkin();
    this->Visible = TRUE;

    gos_create_thread(NTPSyncThreadEntry, 4*1024, NULL);

    ///if (g_ThreadOma)
    ///{
    ///    GLogFactory::GetInstance()->GetDefaultLogger()->SetLogModule(MODULE_OMA, (CHAR*)"OMA");
    ///    g_ThreadOma->Start();
    ///}

    Application->OnMessage = MyOnMessage;

    TimerAutoLock->Interval = g_ulAutoLockPeriod*1000;
    TimerAutoLock->Enabled = (g_ulAutoLockPeriod > 0);
   // PostMessage(this->Handle,WM_SYSCOMMAND, SC_MAXIMIZE,0);

    GosLog(LOG_INFO, "rec cli start succfully");
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
    ShowCurrentTime();

    SendConfirmRequest();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (g_bClosing)
    {
        return;
    }

    Action = caNone;
    if (!DialogExit)
    {
        Application->CreateForm(__classid(TDialogExit), &DialogExit);
    }

    DialogExit->Init();
    DialogExit->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::Exit()
{
/*
    CHAR *szInfo = "退出调度台吗？";

    if (!g_bSilentExit && !AskMsg(szInfo))
    {
        Action = caNone;
        return;
    } */

    g_bClosing = true;
    this->Visible = false;

    ///if (g_ThreadOma)
    ///{
    ///    g_ThreadOma->Free();
    ///}

    ClientLogout();
    gos_sleep_1ms(100);
    CloseApp();
}
//---------------------------------------------------------------------------
/*void __fastcall TFormMain::OnDISKickOffDCReq(UINT32 ulSeqID, GJson &Json)
{
    GosLog(LOG_FATAL, "same dc user logined, auto close this dc");

    // TODO
    return;

    TimerExit->Enabled = true;

    InfoMsg("本用户正在其他终端登录，\n系统将自动退出！");
}  */
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerExitTimer(TObject *Sender)
{
    g_bSilentExit = TRUE;
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnConfirmSaveRsp(UINT32 ulSeqID, GJson &Json)
{
    UINT32  ulDCUserID;
    char    *szResult = Json.GetValue("Result");

    if (!szResult)
    {
        return;
    }

    if (!Json.GetValue("User", &ulDCUserID))
    {
        GosLog(LOG_ERROR, "invalid User in msg %s", Json.GetText());
        return;
    }

    if (strcasecmp(szResult, "true") != 0)
    {
        return;
    }

    DeleteConfirmRequest(ulSeqID);
}

void __fastcall TFormMain::HandleException(TObject *Sender, Exception *E)
{
    AnsiString strErrInfo = E->Message;
    AnsiString strTraceInfo = E->StackTrace;

    GosLog(LOG_FATAL, "system exception: %s, trace ifno: %s", strErrInfo.c_str(), strTraceInfo.c_str());

//  Application->MessageBox(errorstr.c_str(),"发生异常",MB_OK);
}
void __fastcall TFormMain::FormCreate(TObject *Sender)
{
    Application->OnException = HandleException;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MyOnMessage(tagMSG &Msg, bool &Handled)
{
    if ((Msg.message >= WM_KEYFIRST && Msg.message <= WM_KEYLAST) ||
        (Msg.message >= WM_MOUSEFIRST && Msg.message <= WM_MOUSELAST) )
    {
        TimerAutoLock->Enabled = false;
        TimerAutoLock->Enabled = true;

//      GosLog(LOG_INFO, "key %d", Msg.message);
    }
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

//  PanelLockWin->BringToFront();
  /*
    this->WindowState = wsNormal;
    this->Width = 1;
    this->Height = 1; */

   //   DialogLock = new TDialogLock(Application);
    CreateOneForm(__classid(TDialogLock), &DialogLock);

    this->Enabled = false;

    DialogLock->Init();
    DialogLock->ShowModal();
    DialogLock->FormStyle = fsStayOnTop;

    delete DialogLock;
    DialogLock = NULL;

    this->Enabled = true;
//    this->WindowState = wsMaximized;

    TimerAutoLock->Enabled = true;
    g_bWinLocked = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::PanelWorkMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    if (Button != mbRight)
    {
        return;
    }

    pmSys->PopupFromCursorPos();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerAutoLockTimer(TObject *Sender)
{
    LockWindow();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::pmiLockWindowClick(TObject *Sender)
{
    LockWindow();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::pmiLogoutClick(TObject *Sender)
{
    g_bSilentExit = true;

    Visible = false;
    AppRelogin();
    Exit();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::pmiSetPwdClick(TObject *Sender)
{
    DialogSetPwd->Init();
    DialogSetPwd->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::PanelStatusBarMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    if (Button != mbRight)
    {
        return;
    }

    pmSys->PopupFromCursorPos();
}
//---------------------------------------------------------------------------



