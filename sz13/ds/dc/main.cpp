#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "eLTE_Types.h"
#include "eLTE_SDK.h"
#include "PTTDev.h"
#include "main.h"
#include "GTransFile.h"
#include "DlgExit.h"
#include "DlgLock.h"
#include "DlgSetPwd.h"

#include "ThreadOMA.h"
#include "ThreadRecFileUpload.h"
#include "DlgRealBrd.h"
#include "DlgRecBrd.h"

#include "FrmTrain.h"
#include "FrmTrainList.h"
#include "FrmStation.h"
#include "FrmGroup.h"
#include "FrmUser.h"
#include "FrmEmergencyCall.h"
#include "FrmVideoCall.h"
#include "FrmLatestSDS.h"
#include "FrmIPH.h"
#include "FrmInit.h"
#include "FrmTest.h"
#include "FrmCfg.h"
#include "FrmCDR.h"
#include "FrmReqCallInfo.h"
#include "FrmIPHCallInfo.h"
#include "FrmRecBrdInfo.h"
#include "FrmSDS.h"
#include "FrmQuery.h"
#include "FrmLocalRecManage.h"
#include "FrmLogin.h"
#include "FrmLog.h"
#include "FrmCallIn.h"
#include "FrmLocalCfg.h"

#include "DlgVideoCallCfg.h"
#include "DlgSelectUser.h"
#include "DlgNewPatchGroup.h"
#include "DlgNewDynamicGroup.h"
#include "DlgNewMultiGroup.h"
#include "DlgPatchGroupMember.h"
#include "DlgDynamicGroupMember.h"
#include "DlgStaticGroupMember.h"
#include "DlgSetPredefinedSMS.h"
#include "DlgSetSkin.h"
#include "DlgNewSDS.h"
#include "DlgNewSMS.h"
#include "DlgSetTrainGroup.h"
#include "DlgSelectVideoChan.h"
#include "DlgTrainInfo.h"

#include "FrmSDSTemplate.h"
#include "FrmSDSManage.h"
#include "DlgSetSDSTemplate.h"
#include "FrmInbox.h"
#include "FrmGroupCallWin.h"
#include "FrmP2PCallWin.h"

#include "FrmDCUserCfg.h"
#include "DlgSetDCUser.h"
#include "FrmDCOperatorCfg.h"
#include "DlgSetDCOperator.h"

#include "FrmTrainCfg.h"
#include "DlgSetTrain.h"
#include "FrmTrainUECfg.h"
#include "DlgSetTrainUE.h"

#include "FrmDepotCfg.h"
#include "DlgSetDepot.h"
#include "FrmStationCfg.h"
#include "DlgSetStation.h"
#include "FrmBrdZoneCfg.h"
#include "DlgSetBrdZone.h"
#include "FrmStationUECfg.h"
#include "DlgSetStationUE.h"
#include "FrmPortalUECfg.h"
#include "DlgSetPortalUE.h"
#include "FrmPredefinedSMSCfg.h"
#include "FrmRecBrdCfg.h"
#include "DlgSetRecBrd.h"
#include "FrmGroupCfg.h"

#include "DlgSetOperator.h"
#include "DlgSubUser.h"
#include "DlgSubGroup.h"
#include "FrmHalfDuplexP2PCallWin.h"

#include "FrmGroupCall.h"

#include "DlgSetTrainID.h"
#include "DlgSetTrainGroup.h"
#include "DlgSetTrainDepotID.h"

#include "FrmP2PCallInfo.h"
#include "FrmGroupCallInfo.h"
#include "FrmEmergencyCallInfo.h"
#include "FrmRealBrdInfo.h"
#include "DlgSetGroupInfo.h"
#include "AudioDev.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

// ======== 以下skin为本工程用到的所有skin
#pragma link "dxSkinBlue"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinMoneyTwins"
#pragma link "dxSkinOffice2010Blue"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinOffice2010Silver"
#pragma link "dxSkinOffice2013DarkGray"
#pragma link "dxSkinSharp"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "dxSkinVisualStudio2013Light"
// =========================================

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

BOOL    g_bSilentExit = FALSE;

static TdxBarLargeButton *g_btnCurrChecked = NULL;

static BOOL     g_bIsPTTDown = FALSE;
static UINT32   g_ulCheckMDCFailNum = 0;

UINT32              g_usTransFilePort = 50021;
static GTransFile   *g_TransFile = NULL;

static bool g_bWinLocked = false;

static PAGE_E      g_enCurrPage = PAGE_NONE;
static PAGE_E      g_enLastPage = PAGE_NONE;

extern ThreadOma    *g_ThreadOma;

extern VOID DCLogout();
extern void CheckMainListenOnPTTDial();
extern void CheckMainListenOnGroupCallOver();

ThreadRecFileUp *g_ThreadTransFile = NULL;

BOOL IsPTTKeyDown()
{
    static int  iVirtualKey = VK_F13;
    UINT16      usState = GetAsyncKeyState(iVirtualKey);

    if (usState & 0x8000)
    {
        return TRUE;
    }

    return FALSE;
}

void UpdateMainGroupCall(INT64 i64GroupID)
{
    int     iMaxLen = 10 + 20 + 20 + 10;
    int     iLen;

    if (!FormMain)
    {
        return;
    }

    if (i64GroupID == INVALID_GROUP_ID)
    {
        CheckMainListenOnGroupCallOver();
    }

    AnsiString  strGroup = "主通话组：";

    Group::SetMainGroupID(i64GroupID);
    if (i64GroupID != INVALID_GROUP_ID)
    {
        strGroup += Group::GetGroupDisplayName(i64GroupID);
    }
    else
    {
        FormMain->btnCurrSpeaker->Caption->Text = "";
    }

    iLen = iMaxLen - strGroup.Length();
    if (iLen > 0)
    {
        strGroup += AnsiString::StringOfChar(' ', iLen);
    }

    FormMain->btnMainGroup->Caption->Text = strGroup;

    if (i64GroupID != INVALID_GROUP_ID)
    {
        CheckMainListenOnPTTDial();
    }
}

void UpdateMainGroupSpeaker(INT64 i64GroupID, UINT32 ulNumber)
{
    int     iMaxLen = 8 + 8;
    int     iLen;

    if (GetCurrBrdGroupID() == i64GroupID &&
        i64GroupID != INVALID_GROUP_ID)
    {
        return;
    }

    if (!FormMain)
    {
        return;
    }

    if (Group::GetMainGroupID() != i64GroupID)
    {
        return;
    }

    AnsiString  strNumber = "话权人：";

    if (ulNumber != INVALID_USER_ID)
    {
        strNumber += User::GetUserAlias(ulNumber);
    }

    FormMain->btnCurrSpeaker->Caption->Text = strNumber;
}

void ClearMainGroupAndSpeaker()
{
    UpdateMainGroupCall(INVALID_GROUP_ID);
    UpdateMainGroupSpeaker(INVALID_GROUP_ID, INVALID_USER_ID);
}

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

    Form->WindowState = wsNormal;
    Form->Left = 0;
    Form->Top = 0;
    Form->Width = cx;
    Form->Height = cy;
}

VOID NTPSync()
{
    static UINT32   m_ulLastNTPSyncTime = 0;
    UINT32          ulCurrTime = gos_get_uptime();
    UINT8           aucLocalAddr[4] = {127,0,0,1};
    NTP_CFG_T       stNTPCfg;
    static UINT32   ulNTPSyncFailNum = 0;
    UINT32          ulMaxNTPSyncFailNum = 5;

    GetNTPCfg(stNTPCfg);

    if (stNTPCfg.ulNTPSyncPeriod == 0)
    {
        return;
    }

    if (m_ulLastNTPSyncTime > 0 &&
        (ulCurrTime-m_ulLastNTPSyncTime) < stNTPCfg.ulNTPSyncPeriod)
    {
        return;
    }

    if (gos_is_local_ip(stNTPCfg.aucNTPAddr) ||
        GET_INT(stNTPCfg.aucNTPAddr) == GET_INT(aucLocalAddr))
    {
        return;
    }

    INT32   iTime = gos_get_ntp_time(stNTPCfg.aucNTPAddr, stNTPCfg.usNTPPort);

    if (iTime < 0)
    {
        GosLog(LOG_WARN, "connect to NTP server " IP_FMT " failed!", IP_ARG(stNTPCfg.aucNTPAddr));

        ulNTPSyncFailNum++;
        if (ulNTPSyncFailNum >= ulMaxNTPSyncFailNum)
        {
            SetNTPValid(FALSE);
        }

        return;
    }

    SetNTPValid(TRUE);
    ulNTPSyncFailNum = 0;

    INT32   iTimeDiff = abs((INT32)(iTime - gos_get_current_time()));

    if (m_ulLastNTPSyncTime > 0 &&
        iTimeDiff <= 2)
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
    while(!APP::IsClosing())
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
    CreateOneForm(__classid(TDialogVideoCallCfg), &DialogVideoCallCfg);
    CreateOneForm(__classid(TDialogSelectUser), &DialogSelectUser);
    CreateOneForm(__classid(TDialogSubUser), &DialogSubUser);
    CreateOneForm(__classid(TDialogSubGroup), &DialogSubGroup);
    CreateOneForm(__classid(TFormCallIn), &FormCallIn);
    CreateOneForm(__classid(TFormUser), &FormUser);
    CreateOneForm(__classid(TFormVideoCall), &FormVideoCall);
    CreateOneForm(__classid(TDialogSelectVideoChan), &DialogSelectVideoChan);
    CreateOneForm(__classid(TDialogNewPatchGroup), &DialogNewPatchGroup);
    CreateOneForm(__classid(TDialogNewDynamicGroup), &DialogNewDynamicGroup);
    CreateOneForm(__classid(TDialogNewMultiGroup), &DialogNewMultiGroup);
    CreateOneForm(__classid(TDialogPatchGroupMember), &DialogPatchGroupMember);
    CreateOneForm(__classid(TDialogDynamicGroupMember), &DialogDynamicGroupMember);
    CreateOneForm(__classid(TDialogStaticGroupMember), &DialogStaticGroupMember);
    CreateOneForm(__classid(TDialogRealBrd), &DialogRealBrd);
    CreateOneForm(__classid(TDialogRecBrd), &DialogRecBrd);
    CreateOneForm(__classid(TFormGroup), &FormGroup);
    CreateOneForm(__classid(TFormIPH), &FormIPH);
    CreateOneForm(__classid(TDialogForm), &DialogForm);
    CreateOneForm(__classid(TDialogSetPredefinedSMS), &DialogSetPredefinedSMS);
    CreateOneForm(__classid(TDialogSetSkin), &DialogSetSkin);
    CreateOneForm(__classid(TDialogNewSDS), &DialogNewSDS);
    CreateOneForm(__classid(TDialogNewSMS), &DialogNewSMS);
    CreateOneForm(__classid(TFormTrain), &FormTrain);
    CreateOneForm(__classid(TFormTrainList), &FormTrainList);
    CreateOneForm(__classid(TDialogTrainInfo), &DialogTrainInfo);
    CreateOneForm(__classid(TFormStation), &FormStation);
    CreateOneForm(__classid(TFormLog), &FormLog);

    CreateOneForm(__classid(TFormSDSTemplate), &FormSDSTemplate);
    CreateOneForm(__classid(TFormSDSManage), &FormSDSManage);
    CreateOneForm(__classid(TDialogSetSDSTemplate), &DialogSetSDSTemplate);
    CreateOneForm(__classid(TFormLatestSDS), &FormLatestSDS);
    CreateOneForm(__classid(TFormCDR), &FormCDR);
    CreateOneForm(__classid(TFormReqCallInfo), &FormReqCallInfo);
    CreateOneForm(__classid(TFormIPHCallInfo), &FormIPHCallInfo);
    CreateOneForm(__classid(TFormRecBrdInfo), &FormRecBrdInfo);
    CreateOneForm(__classid(TFormLocalRecManage), &FormLocalRecManage);
    CreateOneForm(__classid(TFormGroupCallWin), &FormGroupCallWin);
    CreateOneForm(__classid(TFormP2PCallWin), &FormP2PCallWin);
    CreateOneForm(__classid(TFormGroupCall), &FormGroupCall);
    CreateOneForm(__classid(TFormLocalCfg), &FormLocalCfg);
    CreateOneForm(__classid(TDialogSetTrainID), &DialogSetTrainID);
    CreateOneForm(__classid(TDialogSetTrainGroup), &DialogSetTrainGroup);
    CreateOneForm(__classid(TDialogSetTrainDepotID), &DialogSetTrainDepotID);
    CreateOneForm(__classid(TFormSDS), &FormSDS);
    CreateOneForm(__classid(TFormPredefinedSMSCfg), &FormPredefinedSMSCfg);

    CreateOneForm(__classid(TFormP2PCallInfo), &FormP2PCallInfo);
    CreateOneForm(__classid(TFormGroupCallInfo), &FormGroupCallInfo);
    CreateOneForm(__classid(TFormEmergencyCallInfo), &FormEmergencyCallInfo);
    CreateOneForm(__classid(TFormRealBrdInfo), &FormRealBrdInfo);
    CreateOneForm(__classid(TDialogSetGroupInfo), &DialogSetGroupInfo);
    CreateOneForm(__classid(TDialogSetOperator), &DialogSetOperator);

    CreateOneForm(__classid(TFormQuery), &FormQuery);
    CreateOneForm(__classid(TFormHalfDuplexP2PCallWin), &FormHalfDuplexP2PCallWin);

    if (APP::HasDCType(DC_TYPE_MANAGER))
    {
        CreateOneForm(__classid(TFormCfg), &FormCfg);
        CreateOneForm(__classid(TFormDCUser), &FormDCUser);
        CreateOneForm(__classid(TDialogSetDCUser), &DialogSetDCUser);
        CreateOneForm(__classid(TFormDCOperatorCfg), &FormDCOperatorCfg);
        CreateOneForm(__classid(TDialogSetDCOperator), &DialogSetDCOperator);

        CreateOneForm(__classid(TFormTrainCfg), &FormTrainCfg);
        CreateOneForm(__classid(TDialogSetTrain), &DialogSetTrain);
        CreateOneForm(__classid(TFormTrainUECfg), &FormTrainUECfg);
        CreateOneForm(__classid(TDialogSetTrainUE), &DialogSetTrainUE);

        CreateOneForm(__classid(TFormDepotCfg), &FormDepotCfg);
        CreateOneForm(__classid(TDialogSetDepot), &DialogSetDepot);
        CreateOneForm(__classid(TFormStationCfg), &FormStationCfg);
        CreateOneForm(__classid(TDialogSetStation), &DialogSetStation);
        CreateOneForm(__classid(TFormBrdZoneCfg), &FormBrdZoneCfg);
        CreateOneForm(__classid(TDialogSetBrdZone), &DialogSetBrdZone);
        CreateOneForm(__classid(TFormStationUECfg), &FormStationUECfg);
        CreateOneForm(__classid(TDialogSetStationUE), &DialogSetStationUE);
        CreateOneForm(__classid(TFormPortalUECfg), &FormPortalUECfg);
        CreateOneForm(__classid(TDialogSetPortalUE), &DialogSetPortalUE);
        CreateOneForm(__classid(TFormRecBrdCfg), &FormRecBrdCfg);
        CreateOneForm(__classid(TDialogSetRecBrd), &DialogSetRecBrd);
        CreateOneForm(__classid(TFormGroupCfg), &FormGroupCfg);
    }

    if (!FormMsgDialog)
    {
        Application->CreateForm(__classid(TFormMsgDialog), &FormMsgDialog);
    }

    Application->CreateForm(__classid(TDialogSetPwd), &DialogSetPwd);

    GosLog(LOG_INFO, "Init all form successful");
}

//---------------------------------------------------------------------------
void ShowGroupCallWin(int iLeft, int iTop, INT64 i64GroupID, AnsiString strGroupName, BOOL bIsPatchGroup)
{
    if (!APP::IsUIInited() || !FormMain || !FormGroupCallWin)
    {
        return;
    }

    TWinControl     *ParentCtrl = FormMain;//FormMain->PanelWork;

    FormGroupCallWin->Parent = ParentCtrl;
    FormGroupCallWin->Init(i64GroupID, strGroupName, bIsPatchGroup);

    FormGroupCallWin->Caption = strGroupName;
    FormGroupCallWin->Left = iLeft;
    FormGroupCallWin->Top = iTop;

    if ((FormGroupCallWin->Left + FormGroupCallWin->Width) > (ParentCtrl->Width - 20) )
    {
        FormGroupCallWin->Left = ParentCtrl->Width - 20 - FormGroupCallWin->Width;
    }

    if ((FormGroupCallWin->Top + FormGroupCallWin->Height) > (ParentCtrl->Height - 20) )
    {
        FormGroupCallWin->Top  = ParentCtrl->Height - 20 - FormGroupCallWin->Height;
    }

    FormGroupCallWin->Visible = true;
    FormGroupCallWin->BringToFront();

    UpdateMainGroupCall(i64GroupID);
}
//---------------------------------------------------------------------------
void ShowGroupCallWin(INT64 i64GroupID, AnsiString strGroupName, BOOL bIsPatchGroup)
{
    int iLeft = 0;
    int iTop = 0;

    if (!APP::IsUIInited() || !FormMain || !FormGroupCallWin)
    {
        return;
    }

    TWinControl     *ParentCtrl = FormMain->PanelWork;

    iLeft = (ParentCtrl->Width - FormGroupCallWin->Width)/2;
    iTop = (ParentCtrl->Height - FormGroupCallWin->Height)/2;

    if (strGroupName.IsEmpty())
    {
        strGroupName = Group::GetGroupName(i64GroupID);
    }

    ShowGroupCallWin(iLeft, iTop, i64GroupID, strGroupName, bIsPatchGroup);
}

void ShowGroupCallWin(INT64 i64GroupID)
{
    AnsiString strGroupName = Group::GetGroupName(i64GroupID);

    ShowGroupCallWin(i64GroupID, strGroupName, FALSE);
}

void ShowGroupCallWin(int x, int y, INT64 i64GroupID)
{
    AnsiString strGroupName = Group::GetGroupName(i64GroupID);

    ShowGroupCallWin(x, y, i64GroupID, strGroupName);
}
//---------------------------------------------------------------------------
void RegisterDIS()
{
    GJsonParam      Param;

    Param.Add("DCUserID", APP::GetCurrDCUserID());

    SendServer(EV_DC_DIS_REQ, MSG_DCRegisterDIS, Param);
    GosLog(LOG_INFO, "DC register to DIS");
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
    if (enPage == PAGE_MAINLINE)    return btnMainLine;
    if (enPage == PAGE_DEPOT_TRAIN) return btnDepotTrain;
    if (enPage == PAGE_TRAIN)       return btnTrain;
    if (enPage == PAGE_STATION)     return btnStation;
    if (enPage == PAGE_GROUP)       return btnGroup;
    if (enPage == PAGE_USER )       return btnUser;

    if (enPage == PAGE_VIDEO)       return btnVideo;
    if (enPage == PAGE_SDS  )       return btnSDS;
    if (enPage == PAGE_IPH  )       return btnIPH;
    if (enPage == PAGE_QUERY )      return btnQuery;
    if (enPage == PAGE_SYS  )       return btnSys;
    if (enPage == PAGE_LOCALCFG)    return btnLocalCfg;
    if (enPage == PAGE_TEST )       return btnTest;

    return NULL;
}
//---------------------------------------------------------------------------
void ActiveForm(TForm *Form)
{
    if (!Form)
    {
        return;
    }

    if (!Form->Visible)
    {
        Form->Visible = true;
    }

    if (Form == FormSDS)
    {
        FormLatestSDS->RefreshSDSInfo();
    }

    Form->BringToFront();
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

    btn->LookAndFeel->SkinName = UICfg::GetLeftToolbarButtonSkinName();

    g_enLastPage = g_enCurrPage;
    g_enCurrPage = enPage;

    if (enPage != PAGE_SDS)
    {
        btn->Down = true;
    }

    if (enPage == PAGE_MAINLINE)
    {
        ActiveForm(FormTrain);
        FormTrain->ShowMainLine();
    }

    if (enPage == PAGE_DEPOT_TRAIN)
    {
        ActiveForm(FormTrain);
        FormTrain->ShowDepot();
    }

    if (enPage == PAGE_TRAIN)       ActiveForm(FormTrainList);
    if (enPage == PAGE_STATION)     ActiveForm(FormStation);
    if (enPage == PAGE_GROUP)       ActiveForm(FormGroup);
    if (enPage == PAGE_USER )       ActiveForm(FormUser);
    if (enPage == PAGE_VIDEO)       ActiveForm(FormVideoCall);
    if (enPage == PAGE_SDS)         ActiveForm(FormSDS);
    if (enPage == PAGE_IPH  )       ActiveForm(FormIPH);
    if (enPage == PAGE_QUERY)       ActiveForm(FormQuery);
    if (enPage == PAGE_LOCALCFG)    ActiveForm(FormLocalCfg);

    if (FormCfg && enPage == PAGE_SYS)
    {
        ActiveForm(FormCfg);
    }

    if (FormTest && enPage == PAGE_TEST)
    {
        ActiveForm(FormTest);
    }

    if (enPage == PAGE_IPH)
    {
        FormIPH->Active();
    }

    if (enPage == PAGE_MAINLINE ||
        enPage == PAGE_DEPOT_TRAIN ||
        enPage == PAGE_TRAIN    ||
        enPage == PAGE_STATION  ||
        enPage == PAGE_GROUP    ||
        enPage == PAGE_USER )
    {
        btnTopEmpty->Down = true;
    }
    else
    {
        btnLeftEmpty->Down = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnClientConnectionChangeMsg(TMessage Message)
{
    BOOL bConnected = (BOOL)Message.LParam;

    if (bConnected)
    {
        TimerRegisterDIS->Interval = 1;
        TimerRegisterDIS->Enabled = true;
    }
    else
    {
        UpdateATSStatus(FALSE);
        SetStatusButtonStatus(btnDISStatus, bConnected);
        SetConnectToDIS(bConnected);
    }

    if (FormTrainList)
    {
        FormTrainList->UpdateATSStatus();
    }
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

    SetFormParent(FormTrain, Parent);
    SetFormParent(FormTrainList, Parent);
    SetFormParent(FormStation, Parent);
    SetFormParent(FormGroup, Parent);
    SetFormParent(FormUser, Parent);
    SetFormParent(FormVideoCall, Parent);
    SetFormParent(FormIPH, Parent);
    SetFormParent(FormSDS, Parent);
    SetFormParent(FormQuery, Parent);
    SetFormParent(FormCallIn, PanelMon);
    SetFormParent(FormLocalRecManage, Parent);
    SetFormParent(FormLocalCfg, Parent);

    FormGroupCall->Parent = this;
    FormGroupCall->Visible = false;

    if (FormCfg)
    {
        SetFormParent(FormCfg, Parent);
        btnSys->Visible = true;
    }
    else
    {
        btnSys->Visible = false;
    }

    if (LocalCfg::GetInstance().GetTestMode())
    {
        CreateOneForm(__classid(TFormTest), &FormTest);
        SetFormParent(FormTest, Parent);

        btnTest->Visible = true;

        SplitterMain->Visible = PanelMon->Visible;
        PanelMon->Padding->Left = 0;
        PanelCallIn->Margins->Left = 0;
        PanelInbox->Margins->Left = 0;
    }
    else
    {
        btnTest->Visible = false;
        SplitterMain->Visible = false;
        PanelMon->Padding->Left = 8;
    }

    InitCtrlPos();
}

void __fastcall TFormMain::InitCtrlPos()
{
    int iScreenWidth = GetSystemMetrics(SM_CXSCREEN);

    if (iScreenWidth < 1600)
    {
        SplitterMain->Visible = true;
        PanelMon->Padding->Left = 0;
        PanelCallIn->Margins->Left = 0;
        PanelInbox->Margins->Left = 0;
    }

    TcxButton   *ToolbarBtn[] = {btnVideo, btnSDS, btnIPH, btnQuery, btnLocalCfg,
                                 btnSys, btnHelp, btnExit, btnTest};
    int         iLeft = 20;
    UINT32      i;
    int         iWidth = 110;

    if (iScreenWidth < 1600)
    {
        iWidth = 90;
    }

    for (i=0; i<ARRAY_SIZE(ToolbarBtn); i++)
    {
        if (ToolbarBtn[i]->Visible)
        {
            ToolbarBtn[i]->TabOrder = i;
            ToolbarBtn[i]->Left = iLeft;
            ToolbarBtn[i]->Width = iWidth;
            ToolbarBtn[i]->Height = 74;
            ToolbarBtn[i]->Top = (ToolbarBtn[i]->Parent->Height - ToolbarBtn[i]->Height)/2;

            iLeft += iWidth + 20;

            ToolbarBtn[i]->Font->Name = UICfg::GetToolbarFontName();
            ToolbarBtn[i]->Font->Size = 14;
            ToolbarBtn[i]->LookAndFeel->SkinName = UICfg::GetToolbarButtonSkinName();
        }
    }

    btnBackPage->TabOrder = 100;
    btnBackPage->Top = ToolbarBtn[0]->Top;
    btnBackPage->Width = ToolbarBtn[0]->Width;
    btnBackPage->Height = ToolbarBtn[0]->Height;
    btnBackPage->Font->Name = UICfg::GetToolbarFontName();
    btnBackPage->Font->Size = 14;
    btnBackPage->LookAndFeel->SkinName = UICfg::GetToolbarButtonSkinName();
    btnBackPage->SpeedButtonOptions->GroupIndex = 0;

    TcxButton   *LeftToolbarBtn[] = {btnMainLine, btnDepotTrain, btnTrain, btnStation, btnGroup, btnUser};
    int         iTop = 70;
    AnsiString  strTitle;

    btnMainLine->Visible = false;
    btnDepotTrain->Visible = false;
    btnTrain->Visible = false;
    btnStation->Visible = false;

    if (APP::HasDCType(DC_TYPE_TRAIN) ||
        APP::HasDCType(DC_TYPE_PASSENGER)||
        APP::HasDCType(DC_TYPE_SHUNT) ||
        APP::HasDCType(DC_TYPE_MASTER) )
    {
        btnMainLine->Visible = true;
        btnTrain->Visible = true;
        btnStation->Visible = true;
    }

    if (APP::HasDCType(DC_TYPE_DEPOT) ||
        APP::HasDCType(DC_TYPE_FIX)   ||
        APP::HasDCType(DC_TYPE_SHUNT) ||
        APP::HasDCType(DC_TYPE_MASTER))
    {
        btnDepotTrain->Visible = true;
        btnTrain->Visible = true;
    }

    if (btnMainLine->Visible)
    {
        btnMainLine->Down = true;
    }
    else if (btnDepotTrain->Visible)
    {
        btnDepotTrain->Down = true;
    }
    else if (btnTrain->Visible)
    {
        btnTrain->Down = true;
    }
    else if (btnStation->Visible)
    {
        btnStation->Down = true;
    }
    else
    {
        btnGroup->Down = true;
    }

    for (i=0; i<ARRAY_SIZE(LeftToolbarBtn); i++)
    {
        if (!LeftToolbarBtn[i]->Visible)
        {
            continue;
        }

        LeftToolbarBtn[i]->TabOrder = i;
        LeftToolbarBtn[i]->Top = iTop;
        LeftToolbarBtn[i]->Width = 136;
        LeftToolbarBtn[i]->Height = 80;
        LeftToolbarBtn[i]->Left = (LeftToolbarBtn[i]->Parent->Width - LeftToolbarBtn[i]->Width)/2;

        strTitle = LeftToolbarBtn[i]->Caption.Trim();
        if (strTitle.Length() == 6)
        {
            LeftToolbarBtn[i]->Caption = AnsiString(" ") + strTitle;
        }
        else if (strTitle.Length() == 4)
        {
            LeftToolbarBtn[i]->Caption = AnsiString("  ") + strTitle+AnsiString("  ");
        }

        iTop += 120;

        LeftToolbarBtn[i]->Font->Name = UICfg::GetToolbarFontName();
        LeftToolbarBtn[i]->Font->Size = 14;
        LeftToolbarBtn[i]->LookAndFeel->SkinName = UICfg::GetLeftToolbarButtonSkinName();
    }

    // status bar
    TWinControl     *StatusbarBtn[] = {btnDCType,
                                   btnMDCStatus, btnDISStatus, btnATSStatus, btnNTPStatus, btnSpeakerStatus, btnMicStatus,
                                   btnMainGroup, btnCurrSpeaker, btnDCUser, btnDCVer, btnCurrTime};
    int         aiLeft[] = {5,
                            180,240,300,360,420,520,
                            700, 1150, 1470, 1620, 1750};
    int         iHeight = 24;

    btnDCUser->Visible = true;
    btnDCVer->Visible = true;

    iTop = (PanelStatusBar->Height - iHeight)/2 + 2;
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

    SliderMic->Top = (PanelStatusBar->Height - SliderMic->Height)/2 + 2;
    SliderMic->Left = btnMicStatus->Left + 100 ;

    btnMainGroup->Width = 300;

    int     iDelta = iScreenWidth-1920;

    btnDCUser->Left += iDelta;
    btnDCVer->Left += iDelta;
    btnCurrTime->Left += iDelta;

    if (btnDCUser->Left <= btnCurrSpeaker->Left)
    {
        btnDCUser->Visible = false;
    }

    if (btnDCVer->Left <= btnCurrSpeaker->Left)
    {
        btnDCVer->Visible = false;
    }
}

void __fastcall TFormMain::InitSkin()
{
    int     iFontSize = 11;
    UINT32  i;

    {
        TWinControl     *Btn[] = {btnDCVer, btnDCUser,
                              btnMDCStatus, btnDISStatus, btnATSStatus, btnNTPStatus, btnSpeakerStatus, btnMicStatus,
                              btnMainGroup, btnCurrSpeaker, btnCurrTime};

        AnsiString  strClassName;

        for (i=0; i<ARRAY_SIZE(Btn); i++)
        {
            strClassName = Btn[i]->ClassName();
            if (strClassName == "TcxButton")
            {
                ((TcxButton*)Btn[i])->Font->Size = iFontSize;
                ((TcxButton*)Btn[i])->Font->Name = UICfg::GetStatusbarFontName();
                ((TcxButton*)Btn[i])->LookAndFeel->SkinName = UICfg::GetStatusbarButtonSkinName();
            }
            else if (strClassName == "TAdvSmoothPanel")
            {
                ((TAdvSmoothPanel*)Btn[i])->Caption->Font->Size = iFontSize;
                ((TAdvSmoothPanel*)Btn[i])->Caption->Font->Name = UICfg::GetStatusbarFontName();
            }
        }
    }

    {
        TcxButton   *Btn[] = {btnVideo, btnSDS, btnIPH, btnQuery, btnLocalCfg, btnSys, btnHelp, btnExit, btnTest};

        for (i=0; i<ARRAY_SIZE(Btn); i++)
        {
            Btn[i]->LookAndFeel->SkinName = UICfg::GetToolbarButtonSkinName();
        }
    }

    {
        TcxButton   *Btn[] = {btnMainLine, btnDepotTrain, btnTrain, btnStation, btnGroup, btnUser};

        for (i=0; i<ARRAY_SIZE(Btn); i++)
        {
            Btn[i]->LookAndFeel->SkinName = UICfg::GetLeftToolbarButtonSkinName();
        }
    }

    btnMinimize->LookAndFeel->SkinName = "VisualStudio2013Light";
}

void __fastcall TFormMain::OnGetESDKEvent(UINT8 *pucMsg, UINT32 ulMsgLen)
{
    ESDK_EVENT_T    *pstEvent = (ESDK_EVENT_T*)pucMsg;

    eSDKEventCallBack(pstEvent->iEventType, pstEvent->acMsg, pstEvent->ulMsgLen);
}

//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TSubForm(Owner)
{
    this->Visible = false;

    if (IsSimpleSMSMode())
    {
        btnSDS->Caption = "短信彩信";
    }

    g_TransFile = new GTransFile(g_usTransFilePort, TRUE);
    g_TransFile->Start();

    PanelStatusNormal->Visible = false;
    PanelStatusError->Visible  = false;
    PanelStatusDisable->Visible= false;

    btnATSStatus->Fill->Picture = PanelStatusDisable->Fill->Picture;
    btnMDCStatus->Fill->Picture = PanelStatusDisable->Fill->Picture;
    btnMicStatus->Caption->Text = IsMainMic()?"主麦克风":"副麦克风";

    PanelMon->Width = 480;

    btnDCType->Caption->Text = GetDCTypeName(APP::GetDCType()) + AnsiString(":") +
                               APP::GetCurrDCUserID();

    TColor  ColorBackground = UICfg::GetBackgroundColor();
    TColor  ColorLeftBackground = UICfg::GetLeftBackgroundColor();

    PanelToolbar->Fill->Color = ColorBackground;
    PanelToolbar->Fill->ColorTo = ColorBackground;
    PanelStatusBar->Fill->Color = ColorBackground;
    PanelStatusBar->Fill->ColorTo = ColorBackground;
    PanelLogo->Fill->Color = ColorBackground;
    PanelLogo->Fill->ColorTo = ColorBackground;
    PanelMon->Color = ColorBackground;

    PanelResource->Fill->Color = ColorLeftBackground;
    PanelResource->Fill->ColorTo = ColorLeftBackground;
    PanelResource->Fill->ColorMirror = PanelResource->Fill->ColorTo;
    PanelResource->Fill->ColorMirrorTo = PanelResource->Fill->Color;

    AnsiString strAppTitle = LocalCfg::GetInstance().GetAppTitle() + " - " + GetDCTypeName(APP::GetDCType());

    LocalCfg::GetInstance().SetAppTitle(strAppTitle);
    Application->Title = LocalCfg::GetInstance().GetAppTitle();
    this->Caption = Application->Title;
    WindowState = wsMaximized;
    Application->ShowMainForm = false;

    btnDCUser->Caption->Text = AnsiString("调度员：") + APP::GetDCLoginName();

    eSDK_StartLocalService();
    ShowCurrentTime();
    UpdateMainGroupCall();
    btnCurrSpeaker->Caption->Text = "";

    if (LocalCfg::GetInstance().ShowTitle())
    {
        this->BorderStyle = Forms::bsSizeable;
        this->WindowState = wsMaximized;
        btnMinimize->Visible = false;
    }
    else
    {
        this->BorderStyle = Forms::bsNone;
        btnMinimize->Visible = true;
        this->Left = 0;
        this->Top = 0;
    }

    InitMsgHandler(EV_ESDK_MSG, OnGetESDKEvent);

    InitMsgHandler(EV_APP_DIS_DC_REQ, OnGetAPPMsg);
    InitMsgHandler(EV_DC_DIS_RSP, MSG_DCRegisterDIS, OnDCRegisterDISRsp);
    InitMsgHandler(EV_DC_DIS_RSP, MSG_DISKickOffDC, OnDISKickOffDCReq);
    InitMsgHandler(EV_DC_DIS_RSP, MSG_DCConfirmSave, OnConfirmSaveRsp);

    InitMsgHandler(EV_DC_DIS_CFG_RSP, MSG_DCGetCfgDataVer, OnGetCfgDataVerRsp);

    VECTOR<CHAR*>   vCfgType;

    GetLoadCfgDataType(vCfgType);

    for (UINT32 i=0; i<vCfgType.size(); i++)
    {
        InitMsgHandler(EV_DC_DIS_RSP, vCfgType[i], OnLoadCfgDataRsp);
    }

    GosLog(LOG_INFO, "init main form successful");
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerInitTimer(TObject *Sender)
{
    if (!TimerInit->Enabled)
    {
        return;
    }

    TimerInit->Enabled = false;

    if (!APP::IsUIInited())
    {
        GosLog(LOG_INFO, "create all form");
        CreateAllForm();
        APP::SetUIInited();
    }

    btnIPH->Visible = APP::HasDCType(DC_TYPE_PASSENGER);
    InitAllCtrl();

    /// 进度条显示并保存初始化进度
    SetInitOver();

    /// 发送消息的类对象，设置为可以发送状态
    InnerMsgReady();

    btnDCVer->Caption->Text = AnsiString("版本：") + LocalCfg::GetInstance().GetDCVersion();
    imgLogo->Hint = AnsiString("版本：") + LocalCfg::GetInstance().GetDCVersion();

    char acTime[128];
    gos_get_text_time_ex(NULL, "%04d%d月%d日 %d:%02d:%02d", acTime);

    btnUser->Hint = "登录时间：" + AnsiString(acTime + 4);

    if (btnMainLine->Visible)
    {
        ActivePage(PAGE_MAINLINE);
    }
    else if (btnDepotTrain->Visible)
    {
        ActivePage(PAGE_DEPOT_TRAIN);
    }
    else
    {
        if (btnTrain->Visible)
        {
            ActivePage(PAGE_TRAIN);
        }
        else if (APP::GetDCType() == DC_TYPE_MANAGER)
        {
            ActivePage(PAGE_SYS);
        }
        else
        {
            ActivePage(PAGE_GROUP);
        }
    }

    delete FormLogin;
    FormLogin = NULL;

    Application->ShowMainForm = true;

    InitSkin();
    this->Visible = TRUE;

    gos_create_thread(NTPSyncThreadEntry, 4*1024, NULL);

    if (g_ThreadOma)
    {
        g_ThreadOma->Start();
    }

    Application->OnMessage = MyOnMessage;

    UINT32  ulAutoLockPeriod = LocalCfg::GetInstance().GetAutoLockPeriod();

    TimerAutoLock->Interval = ulAutoLockPeriod*1000;
    TimerAutoLock->Enabled = (ulAutoLockPeriod > 0);

    if (APP::AllowDISOffline())
    {
        UpdateATSStatus(FALSE);
        SetStatusButtonStatus(btnDISStatus, FALSE);
    }

    GosLog(LOG_INFO, "DC start successfully");
}

void TFormMain::eSDKEventCallBack(INT32 iEventType, char *szEvent, UINT32 ulSize)
{
    char    *szPos;

    if (LocalCfg::GetInstance().GetTestMode() && FormTest)
    {
        AnsiString  strEvent = Utf8ToAnsi(szEvent);

        FormTest->Log("EventType = " + AnsiString(iEventType));
        FormTest->Log(strEvent);
    }

    if (iEventType == ELTE_Event_NotifyUserStatus)      // 设备状态变化
    {
        EVENT_USER_STATUS_T     stEvent = {0};

        if (!ParseEventUserStatus(szEvent, &stEvent))
        {
            GosLog(LOG_ERROR, "parse NotifyUserStatus failed");
            return;
        }

        User::UpdateUserStatus(stEvent);
        FormUser->UpdateUser(stEvent);
        FormVideoCall->UpdateUser(stEvent);
    }
    else if (iEventType == ELTE_Event_NotifyP2pvideocallStatus)     // 实时视频浏览
    {
        EVENT_P2PVIDEO_STATUS_T     stEvent = {0};

        if (!ParseEventP2PVideoCallStatust(szEvent, &stEvent))
        {
            GosLog(LOG_ERROR, "parse NotifyP2pvideocallStatus failed");
            return;
        }

        FormVideoCall->UpdateP2PVideoCallStatus(stEvent);
        FormCallIn->NewP2PVideoCall(stEvent);
    }
    else if (iEventType == ELTE_Event_NotifyResourceStatus)         // 资源状态变化
    {
        EVENT_RESOURCE_STATUS_T     stEvent = {0};

        if (!ParseEventResourceStatus(szEvent, &stEvent))
        {
            GosLog(LOG_ERROR, "parse NotifyResourceStatus failed");
            return;
        }

        if (stEvent.ulStatusType  == RESOURCE_STATUS_TYPE_REG_PROXY &&
            stEvent.ulStatusValue == RESOURCE_STATUS_REGOK &&
            stEvent.i64ResourceID == APP::GetCurrDCUserID())
        {
            OnNativeSrvStatus(TRUE);
            eSDK_Logined();
            GosLog(LOG_INFO, "get the login notification of MDC");
            return;
        }

        // 派接组创建成功或失败
        if (stEvent.ulStatusValue == RESOURCE_STATUS_GRPPATCH_CREATEOK ||
            stEvent.ulStatusValue == RESOURCE_STATUS_GRPPATCH_CREATEFAIL )
        {
            // 刷新派接组界面
            PatchGroup::LoadPatchGroup(TRUE);
            FormGroup->RefreshPatchGroupPanel();
            return;
        }

        // 派接组删除成功或失败
        if (stEvent.ulStatusValue == RESOURCE_STATUS_GRPPATCH_CANCELOK ||
            stEvent.ulStatusValue == RESOURCE_STATUS_GRPPATCH_CANCELFAIL)
        {
            PatchGroup::LoadPatchGroup(TRUE);
            // 刷新派接组界面
            /// lijiancong: 这里不马上刷新随车通信组界面是因为在这个回调时刻，
            /// 能够查询到刚才被删除掉的派接组，刷新界面会添加一个节点到 tlGroup 表中
            /// FormGroup->RefreshPatchGroupPanel();
            return;
        }

        if (stEvent.ulStatusType == RESOURCE_STATUS_TYPE_USERDGNA)
        {
            //UpdateDynamicGroupStatus(stEvent);
        }
    }
    else if (iEventType == ELTE_Event_NotifyProvisionChange)        // 设备属性配置变更通知
    {
        if (strstr(szEvent, "<ChangeMO>29</ChangeMO>")) // PatchGroup
        {
            EVENT_CREATE_PATCH_GROUP_T  stEvent = {0};

            if (!ParseEventCreatePatchGroup(szEvent, &stEvent))
            {
                GosLog(LOG_ERROR, "parse event CreatePatchGroup failed");
                return;
            }

            if (strstr(szEvent, "<ChangeType>1</ChangeType>"))   // 创建PatchGroup
            {
                SetPatchGroupID(stEvent.acGroupName, stEvent.i64GroupID);
                FormGroup->AddPatchGroup(stEvent.acGroupName, stEvent.i64GroupID);
            }
        }
    }
    else if (iEventType == ELTE_Event_NotifyProvisionAllResync)     // 自动下载配置数据通知
    {
    }
    else if (iEventType == ELTE_Event_NotifyP2pcallStatus)          // 点呼状态变化
    {
        EVENT_P2PCALL_STATUS_T  stEvent = {0};

        if (!ParseEventP2PCallStatus(szEvent, &stEvent))
        {
            GosLog(LOG_ERROR, "parse NotifyP2pcallStatus failed");
            return;
        }

        FormUser->UpdateP2PCallStatus(stEvent);
        FormP2PCallWin->UpdateP2PCallStatus(stEvent);
        FormHalfDuplexP2PCallWin->UpdateP2PCallStatus(stEvent);
        FormCallIn->NewP2PCall(stEvent);
    }
    else if (iEventType == ELTE_Event_NotifyGroupStatus)            // 组呼状态变化
    {
        EVENT_GROUP_STATUS_T    stEvent = {0};

        if (!ParseEventGroupStatus(szEvent, &stEvent))
        {
            GosLog(LOG_ERROR, "parse NotifyGroupStatus failed");
            return;
        }

        if (stEvent.ulGroupCallStatus == PTT_IND_STATUS_EMERGENCY_BEGIN)  // 紧急呼叫
        {
            stEvent.ulCallType = CALL_TYPE_EMERGENCY_CALL;
            stEvent.bClosed = FALSE;
            stEvent.bIsEmergencyCall = TRUE;

            Group::SubJoinGroup(stEvent.i64GroupID);
            Group::NewEmergencyGroupCall(stEvent);

            FormCallIn->NewEmergencyCall(stEvent.i64GroupID, stEvent.ulSpeaker);

            LocalRec::StartGroupCallRecord(stEvent.i64GroupID, REC_TYPE_AUDIO);
        }
        else if (stEvent.ulGroupCallStatus == PTT_IND_STATUS_CLOSE_OK)  // 组呼结束
        {
            stEvent.ulCallType = Group::GetGroupCallType(stEvent.i64GroupID);//CALL_TYPE_UNKNOWN;
            stEvent.bClosed = TRUE;

            Group::CloseEmergencyGroupCall(stEvent.i64GroupID);

            FormCallIn->CloseEmergencyCall(stEvent.i64GroupID);
            FormCallIn->StopGroupCall(stEvent.i64GroupID);
            FormGroupCallWin->CloseGroupCall(stEvent.i64GroupID);
            FormTrain->CloseGroupCall(stEvent.i64GroupID);
            FormTrainList->CloseGroupCall(stEvent.i64GroupID);
            FormStation->CloseGroupCall(stEvent.i64GroupID);
            //TODO 还有哪些组呼是可以结束的

            LocalRec::StopGroupCallRecord(stEvent.i64GroupID);

            PatchGroup::CancelTmpPatchGroup(stEvent.i64GroupID);

            if (!IsShowCallWin())
            {
                Group::AutoExitGroup(stEvent.i64GroupID);
            }

            if (stEvent.i64GroupID == Group::GetMainGroupID())
            {
                CheckMainListenOnGroupCallOver();
                UpdateMainGroupCall();
            }

            Group::GroupCallOver(stEvent.i64GroupID);
        }
        else
        {
            if (Group::IsTrainBroadcastGroup(stEvent.i64GroupID))
            {
                stEvent.ulCallType = CALL_TYPE_TRAIN_BRD;
            }
            else
            {
                stEvent.ulCallType = CALL_TYPE_PTT;
            }

            Group::UpdateGroupCallStatus(stEvent);

            if (stEvent.ulGroupCallStatus == PTT_IND_STATUS_SNATCH ||   // 组呼建立成功
                stEvent.ulGroupCallStatus == PTT_IND_STATUS_TX_BEGIN)   // 话权变更
            {
                LocalRec::StartGroupCallRecord(stEvent.i64GroupID, REC_TYPE_AUDIO);

                FormCallIn->UpdateGroupCallStatus(stEvent);

                UpdateMainGroupSpeaker(stEvent.i64GroupID, stEvent.ulSpeaker);

                FormTrainList->NewGroupCall(stEvent.i64GroupID);

                Group::AutoExitGroup(stEvent.i64GroupID, stEvent.ulSpeaker);   // 其他调度台正在接听车载台/固定台时，本调度台自动退出
            }
            else if (stEvent.ulGroupCallStatus == PTT_IND_STATUS_TX_IDLE)   // 话权空闲
            {
                LocalRec::UpdateGroupCallRecord(stEvent.i64GroupID);
                FormCallIn->UpdateGroupCallStatus(stEvent);
            }
        }

        SaveGroupCallInfo(stEvent.ulCallType, stEvent.i64GroupID, stEvent.ulSpeaker);

        FormGroup->ShowGroupCallStatus(stEvent);
        FormGroupCallWin->UpdateGroupCallStatus(stEvent);
        FormGroupCall->UpdateGroupCallStatus(stEvent);

        return;
    }
    else if (iEventType == ELTE_Event_NotifyModuleStatus)           // 模块组件状态
    {
        EVENT_MODULE_STATUS_T       stEvent = {0};

        if (!ParseEventModuleStatus(szEvent, &stEvent))
        {
            GosLog(LOG_ERROR, "parse NotifyModuleStatus failed");
            return;
        }

        if (stEvent.ulModuleType == MODULE_TYPE_DC)
        {
            if (stEvent.ulModuleStatus == MODULE_STATE_KICK_OFF)
            {
                InfoMsg("本用户正在其他终端登录，\n系统将自动退出！");
                TimerExit->Enabled = true;
            }

            else if (stEvent.ulModuleStatus == MODULE_STATE_DISCONNECT) // 服务器未连接
            {
                SetMDCStatus(false);
            }
            else if (stEvent.ulModuleStatus == MODULE_STATE_RECONNECT)  // 服务器已连接
            {
                SetMDCStatus(true);
            }
        }
    }
    else if (iEventType == ELTE_Event_NotifyGISReport)  // GIS位置信息通知
    {
        EVENT_GIS_REPORT_T      stEvent = {0};

        GosLog(LOG_INFO, "get GIS info: %s", szEvent);

        if (!ParseEventGISReport(szEvent, &stEvent))
        {
            GosLog(LOG_ERROR, "parse NotifyGISReport failed");
            return;
        }

        GIS::UpdateGISInfo(stEvent);
        FormUser->UpdateGISInfo(stEvent);
    }
    else if (iEventType == ELTE_Event_NotifyGISStatus)  // GIS订阅状态通知
    {
     /* EVENT_GIS_REPORT_T      stEvent = {0};

        if (!ParseEventGISReport(szEvent, &stEvent))
        {
            GosLog(LOG_ERROR, "parse NotifyGISReport failed");
            return;
        }

        UpdateGISInfo(stEvent); */

        GosLog(LOG_INFO, "GIS sub status: %s", szEvent);
    }
    else if (iEventType == ELTE_Event_NotifySDSReport)              // 短信/彩信接收上报
    {
        EVENT_SDS_REPORT_T      stEvent = {0};

        if (!ParseEventSDSReport(szEvent, &stEvent))
        {
            GosLog(LOG_ERROR, "parse NotifySDSReport failed: %s", szEvent);
            return;
        }

        if (eSDKData::IsMMMsg(stEvent.ulSDSType))
        {
            FormLatestSDS->ShowNewSMS(stEvent);
        }
        else
        {
            FormLatestSDS->ShowNewSDS(stEvent);
        }
    }
    else if (iEventType == ELTE_Event_NotifySDSSendStatus)          // 短信/彩信发送状态通知
    {
        EVENT_SDS_SEND_STATUS_T         stEvent = {0};

        if (!ParseEventSDSSendStatus(szEvent, &stEvent))
        {
            GosLog(LOG_ERROR, "parse NotifySDSSendStatu failed");
            return;
        }

        FormLatestSDS->UpdateSDSSendStatus(stEvent);
    }
    else if (iEventType == ELTE_Event_NotifyServiceStatus)          // 本地服务停止通知
    {
        if (strstr(szEvent, "<NativeSrvStatus>1</NativeSrvStatus>"))
        {
            OnNativeSrvStatus(TRUE);
        }
        else
        {
            OnNativeSrvStatus(FALSE);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnNativeSrvStatus(BOOL bValid)
{
//  btnDCUser->Enabled = bValid;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SetMDCStatus(bool bConnected)
{
    SetStatusButtonStatus(btnMDCStatus, bConnected);

    SetMDCValid(bConnected);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ShowCurrentTime()
{
    char    acTime[128];

    gos_get_text_time_ex(NULL, "%04d-%02d-%02d  %02d:%02d:%02d", acTime);    // 日期和时间中间留2个空格

    btnCurrTime->Caption->Text = acTime;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerCheckTimer(TObject *Sender)
{
    SetStatusButtonStatus(btnNTPStatus, IsNTPValid());

    ShowCurrentTime();

    SendConfirmRequest();

    if (APP::IsUIInited())
    {
        BOOL bShow = IsShowCallWin();

        FormTrain->btnPTT->Visible = !bShow;
        FormTrainList->btnPTT->Visible = !bShow;
        FormStation->btnPTT->Visible = !bShow;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (!APP::IsClosing())
    {
        Action = caNone;
        btnExitClick(this);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::Exit()
{
    APP::SetCloseState(true);
    this->Visible = false;

    APP::Logout();
    eSDK_Cleanup();

    if (g_ThreadOma)
    {
        g_ThreadOma->Free();
    }

    DCLogout();
    gos_sleep_1ms(100);
    CloseApp();

    exit(0);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnGetAPPMsg(UINT8 *pucMsg, UINT32 ulMsgLen)
{
    CHAR        *szMsg = (CHAR*)pucMsg;
    GJson       Json;
    GJson       SubJson;
    BOOL        bRet = Json.Parse(szMsg);
    AnsiString  strMsgType;
    CHAR        *szMsgType;
    CHAR        *szMsgInfo;
    UINT32      ulSeqID;

    if (!bRet)
    {
        GosLog(LOG_ERROR, "invalid APP msg: %s", szMsg);
        return;
    }

    szMsgType = Json.GetValue("MsgName");
    if (!szMsgType)
    {
        GosLog(LOG_ERROR, "get MsgName failed: %s", szMsg);
        return;
    }

    if (!Json.GetValue("MsgSeqID", &ulSeqID))
    {
        GosLog(LOG_ERROR, "get MsgSeqID failed: %s", szMsg);
        return;
    }

    szMsgInfo = Json.GetValue("MsgInfo");
    if (!szMsgInfo)
    {
        GosLog(LOG_ERROR, "get MsgInfo failed: %s", szMsg);
        return;
    }

    if (!SubJson.Parse(szMsgInfo))
    {
        GosLog(LOG_ERROR, "parse MsgInfo failed: %s", szMsg);
        return;
    }

    strMsgType = szMsgType;
    if (strMsgType == MSG_APP_RequestCallReq)
    {
        if (FormCallIn)
        {
            FormCallIn->OnRequestCallReq(SubJson, ulSeqID);
        }
    }
    else if (strMsgType == MSG_APP_SetTrainPosReq)
    {
        if (FormCallIn)
        {
            FormCallIn->OnSetTrainPosReq(SubJson, ulSeqID);
        }
    }
    else if (strMsgType == MSG_APP_RegisterReq)
    {
        OnAppRegisterReq(SubJson, ulSeqID);
    }
    else
    {
        GosLog(LOG_ERROR, "invalid APP msg: %s", szMsg);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnAppRegisterReq(GJson &Json, UINT32 ulSeqID)
{
    UINT32  ulTrainUnitID;
    UINT32  ulDepotID;
    BOOL    bAutoRegister;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        GosLog(LOG_ERROR, "invalid train UnitID in msg APP_REGISTER");
        return;
    }

    if (!Json.GetValue("DepotID", &ulDepotID))
    {
        GosLog(LOG_ERROR, "invalid DepotID in msg APP_REGISTER");
        return;
    }

    if (!Json.GetValue("AutoRegister", &bAutoRegister))
    {
        GosLog(LOG_ERROR, "invalid AutoRegister in msg APP_REGISTER");
        return;
    }

    GosLog(LOG_INFO, "train %u register to depot %u", ulTrainUnitID, ulDepotID);

    if (!bAutoRegister && ulDepotID != DEPOT_ID_AUTO)
    {
        FormCallIn->TrainRegister(ulTrainUnitID, ulDepotID);
        return;
    }

    BOOL    bRegister = Depot::IsSameDepot(ulDepotID);//(APP::GetDepotID() == ulDepotID);

    Train::SetTrainRegisterInfo(ulTrainUnitID, bRegister, ulDepotID);

    GJsonParam      Param;

    Param.Add("TrainUnitID", ulTrainUnitID);
    Param.Add("DepotID", ulDepotID);
    Param.Add("DCUserID", APP::GetCurrDCUserID());

    SendServer(EV_APP_DIS_DC_RSP, MSG_APP_RegisterRsp, Param);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnAppUnregisterReq(GJson &Json, UINT32 ulSeqID)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerCheckServiceTimer(TObject *Sender)
{
    TimerCheckService->Interval = 10*1000;

    if (!LocalCfg::GetInstance().NeedLogin())
    {
        return;
    }

    UINT32  ulSvcStatus = eSDK_GetLocalServiceStatus();

    if (SERVICE_UNKNOW == ulSvcStatus)
    {
        GosLog(LOG_FATAL, "native services is not installed");

        SetMDCStatus(false);
        btnMDCStatus->Hint = "通信服务未安装";
        return;
    }

    if (ulSvcStatus == SERVICE_START_PENDING )
    {
        GosLog(LOG_FATAL, "native services is starting");
        return;
    }
    else if (ulSvcStatus == SERVICE_STOP_PENDING)
    {
        GosLog(LOG_FATAL, "native services is stopping");
        return;
    }
    else if (ulSvcStatus == SERVICE_CONTINUE_PENDING)
    {
        GosLog(LOG_FATAL, "native services is resuming");
        return;
    }

    if (ulSvcStatus == SERVICE_STOPPED ||
        ulSvcStatus == SERVICE_PAUSED )
    {
        GosLog(LOG_FATAL, "native services stopped");
        SetMDCStatus(false);
        btnMDCStatus->Hint = "通信服务未启动";
        eSDK_StartLocalService();

        TimerCheckService->Interval = 20*1000;

        return;
    }

    if (SERVICE_RUNNING == ulSvcStatus)
    {
        if (!APP::IsUIInited())
        {
            return;
        }

        if (!eSDK_IsLogined())
        {
            GosLog(LOG_FATAL, "relogin to MDC");
            eSDK_Cleanup();
            APP::Login(this->Handle);

            TimerCheckService->Interval = 30*1000;

            return;
        }
    }

    UINT8       aucLocalIP[4];
    IP_ADDR_T   stMDCAddr = LocalCfg::GetInstance().GetMdcIP();

    if (!GetLocalIP(stMDCAddr.aucIP, aucLocalIP) ||
        !eSDK_CheckMDC())
    {
        g_ulCheckMDCFailNum++;
        GosLog(LOG_FATAL, "check MDC failed, check failed num is %u",g_ulCheckMDCFailNum);
    }
    else
    {
        g_ulCheckMDCFailNum = 0;
    }

    if (g_ulCheckMDCFailNum > 6)
    {
        eSDK_Cleanup();
        eSDK_StopLocalService();
        SetMDCStatus(false);
        btnMDCStatus->Hint = "功能异常";

        return;
    }
    else
    {
        SetMDCStatus(true);
        btnMDCStatus->Hint = "正常";
    }

    CheckSpeakMic(FALSE);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::CheckSpeakMic(BOOL bForce)
{
    VECTOR<AnsiString>  strList;

    if (!bForce && g_ulCheckMDCFailNum > 0)
    {
        return;
    }

    if (!eSDK_GetSpeakerList(strList) ||
        strList.size() == 0)
    {
        SetStatusButtonStatus(btnSpeakerStatus, false);
    }
    else
    {
        if (IsMainSpeaker())
        {
            SetStatusButtonStatus(btnSpeakerStatus, true);
        }
        else
        {
            btnSpeakerStatus->Fill->Picture = PanelStatusWarn->Fill->Picture;
        }
    }

    if (!eSDK_GetMicList(strList) ||
        strList.size() == 0)
    {
        SetStatusButtonStatus(btnMicStatus, false);
    }
    else
    {
        if (IsMainMic())
        {
            SetStatusButtonStatus(btnMicStatus, true);
        }
        else
        {
            btnMicStatus->Fill->Picture = PanelStatusWarn->Fill->Picture;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerRegisterDISTimer(TObject *Sender)
{
    if (!TimerRegisterDIS->Enabled)
    {
        return;
    }

    TimerRegisterDIS->Interval = 1000*5;
    RegisterDIS();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnDCRegisterDISRsp(UINT32 ulSeqID, GJson &Json)
{
    TimerRegisterDIS->Enabled = false;

    if (APP::AllowDISOffline())
    {
        InfoMsg("调度服务器已在线，请注销重新登录!");

        g_bSilentExit = TRUE;
        Visible = false;

        DCRelogin();

        Exit();
    }
    else
    {
        SetStatusButtonStatus(btnDISStatus, TRUE);
        SetConnectToDIS(TRUE);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnDISKickOffDCReq(UINT32 ulSeqID, GJson &Json)
{
    GosLog(LOG_FATAL, "same dc user logined, auto close this dc");

    InfoMsg("本用户正在其他终端登录，\n系统将自动退出！");

    TimerExit->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerCheckDynamicGroupTimer(TObject *Sender)
{
    static bool  bLoadDynamicGroup = FALSE;

    TimerCheckDynamicGroup->Interval = 10*1000;

    if (!IsMDCValid())
    {
        return;
    }

    if (!bLoadDynamicGroup)
    {
        if (!DynamicGroup::LoadDynamicGroup())
        {
            GosLog(LOG_ERROR, "loading dynamic group failed");
            return;
        }

        bLoadDynamicGroup = true;
        DynamicGroup::InitDefaultDynamicGroup();
    }

    DynamicGroup::CheckDynamicGroup(GROUP_NAME_DC_BRD_TRAINS);  // 多选列车广播组
    DynamicGroup::CheckDynamicGroup(GROUP_NAME_REQ_CALL);       // 请呼动态组
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnHelpClick(TObject *Sender)
{
    AnsiString  strHelpFile = "调度台操作手册.pdf";

    ShellExecute(NULL, NULL, strHelpFile.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerExitTimer(TObject *Sender)
{
    g_bSilentExit = TRUE;
    Exit();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnMinimizeClick(TObject *Sender)
{
    Application->Minimize();
    btnEmpty->Down = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnMainLineClick(TObject *Sender)
{
    ActivePage(PAGE_MAINLINE);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnDepotTrainClick(TObject *Sender)
{
    ActivePage(PAGE_DEPOT_TRAIN);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnTrainClick(TObject *Sender)
{
    ActivePage(PAGE_TRAIN);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnStationClick(TObject *Sender)
{
    ActivePage(PAGE_STATION);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnGroupClick(TObject *Sender)
{
    ActivePage(PAGE_GROUP);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnUserClick(TObject *Sender)
{
    ActivePage(PAGE_USER);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnExitClick(TObject *Sender)
{
    if (!DialogExit)
    {
        Application->CreateForm(__classid(TDialogExit), &DialogExit);
    }
    else
    {
        DialogExit->Visible = false;
    }

    DialogExit->Init();
    DialogExit->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnVideoClick(TObject *Sender)
{
    ActivePage(PAGE_VIDEO);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnSDSClick(TObject *Sender)
{
    ActivePage(PAGE_SDS);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnIPHClick(TObject *Sender)
{
    ActivePage(PAGE_IPH);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnQueryClick(TObject *Sender)
{
    ActivePage(PAGE_QUERY);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnLocalCfgClick(TObject *Sender)
{
    ActivePage(PAGE_LOCALCFG);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnSysClick(TObject *Sender)
{
    ActivePage(PAGE_SYS);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnTestClick(TObject *Sender)
{
    ActivePage(PAGE_TEST);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnToolbarMouseEnter(TObject *Sender)
{
    TcxButton   *btn = (TcxButton*)Sender;

    btn->LookAndFeel->SkinName = UICfg::GetToolbarButtonEnterSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnToolbarMouseLeave(TObject *Sender)
{
    TcxButton   *btn = (TcxButton*)Sender;

    btn->LookAndFeel->SkinName = UICfg::GetToolbarButtonSkinName();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnBackPageClick(TObject *Sender)
{
    BackPage();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerPTTTimer(TObject *Sender)
{
    BOOL    bIsPTTDown = IsPTTKeyDown() || IsPTTDevDown();

    if (bIsPTTDown != g_bIsPTTDown)
    {
        if (bIsPTTDown)
        {
            GosLog(LOG_DETAIL, "PTTDev down");

            if (FormGroupCallWin->Visible)
            {
                FormGroupCallWin->OnPTTDial();
            }
            else if (FormHalfDuplexP2PCallWin->Visible)
            {
                FormHalfDuplexP2PCallWin->OnPTTDial();
            }
            else if (DialogRealBrd->Visible)
            {
                DialogRealBrd->OnPTTDial();
            }
            else if (g_enCurrPage == PAGE_MAINLINE)
            {
                FormTrain->OnPTTDial();
            }
            else if (g_enCurrPage == PAGE_DEPOT_TRAIN)
            {
                FormTrain->OnPTTDial();
            }
            else if (g_enCurrPage == PAGE_TRAIN)
            {
                FormTrainList->OnPTTDial();
            }
            else if (g_enCurrPage == PAGE_STATION)
            {
                FormStation->OnPTTDial();
            }
            else if (g_enCurrPage == PAGE_GROUP)
            {
                FormGroup->OnPTTDial();
            }
            else if (g_enCurrPage == PAGE_VIDEO)
            {
                FormVideoCall->OnPTTDial();
            }
        }
        else
        {
            GosLog(LOG_DETAIL, "PTTDev up");
            if (FormGroupCallWin->Visible)
            {
                FormGroupCallWin->OnPTTRelease();
            }
            else if (FormHalfDuplexP2PCallWin->Visible)
            {
                FormHalfDuplexP2PCallWin->OnPTTRelease();
            }
            else if (DialogRealBrd->Visible)
            {
                DialogRealBrd->OnPTTRelease();
            }
            else if (g_enCurrPage == PAGE_MAINLINE)
            {
                FormTrain->OnPTTRelease();
            }
            else if (g_enCurrPage == PAGE_DEPOT_TRAIN)
            {
                FormTrain->OnPTTRelease();
            }
            else if (g_enCurrPage == PAGE_TRAIN)
            {
                FormTrainList->OnPTTRelease();
            }
            else if (g_enCurrPage == PAGE_STATION)
            {
                FormStation->OnPTTRelease();
            }
            else if (g_enCurrPage == PAGE_GROUP)
            {
                FormGroup->OnPTTRelease();
            }
            else if (g_enCurrPage == PAGE_VIDEO)
            {
                FormVideoCall->OnPTTRelease();
            }
            else
            {
                PTT::Release();
            }
        }
    }

    g_bIsPTTDown = bIsPTTDown;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::UpdateATSStatus(BOOL bValid)
{
    if (APP::AllowDISOffline() ||
        !IsConnectToDIS() ||
        !bValid)
    {
        SetStatusButtonStatus(btnATSStatus, false);
        btnATSStatus->Hint = "异常";
        SetATSValid(FALSE);
    }
    else
    {
        SetStatusButtonStatus(btnATSStatus, true);
        btnATSStatus->Hint = "正常";
        SetATSValid(TRUE);
    }
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

    if (!Json.GetValue("DCUserID", &ulDCUserID))
    {
        GosLog(LOG_ERROR, "invalid DCUserID in msg %s", MSG_DCSetUESDSReply);
        return;
    }

    if (ulDCUserID != APP::GetCurrDCUserID())
    {
        GosLog(LOG_ERROR, "invalid DC user(%u) in msg %s", ulDCUserID, MSG_DCSetUESDSReply);
        return;
    }

    if (strcasecmp(szResult, "true") != 0)
    {
        return;
    }

    DeleteConfirmRequest(ulSeqID);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnGetCfgDataVerRsp(UINT32 ulSeqID, GJson &Json)
{
    ReloadCfgData(Json);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnLoadCfgDataRsp(UINT32 ulSeqID, GJson &Json)
{
    AnsiString  strMsgType = GetJsonMsgType();
 //   GJson       &JsonMsg = GetJsonMsg();    // 原始Json消息
    UINT32      ulCfgVer;

    if (!Json.GetValue("CfgVer", &ulCfgVer))
    {
        GosLog(LOG_ERROR, "invalid CfgVer on msg %s", Json.GetText());
        return;
    }

    CHAR        *szCfgData = Json.GetValue("CfgData");

    if (!szCfgData)
    {
        GosLog(LOG_ERROR, "invalid CfgData on msg %s", Json.GetText());
        return;
    }

    ::OnLoadCfgDataRsp(strMsgType, ulCfgVer, szCfgData);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::HandleException(TObject *Sender, Exception *E)
{
    AnsiString strErrInfo = E->Message;
    AnsiString strTraceInfo = E->StackTrace;

    GosLog(LOG_FATAL, "system exception: %s, trace info: %s", strErrInfo.c_str(), strTraceInfo.c_str());
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

    btn->LookAndFeel->SkinName = UICfg::GetToolbarButtonEnterSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnLeftToolbarMouseLeave(TObject *Sender)
{
    TcxButton   *btn = (TcxButton*)Sender;

    btn->LookAndFeel->SkinName = UICfg::GetLeftToolbarButtonSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnMicStatusClick(TObject *Sender)
{
    return;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SliderMicStateChanged(TObject *Sender, TAdvSmoothSliderState State,
          double Value)
{
    {
        //交换麦克风和扬声器
        SwitchSpeaker();
        SwitchMic();

        INT64   i64MainGroupID = Group::GetMainGroupID();

        if (i64MainGroupID != INVALID_GROUP_ID)
        {
            AssignGroupSpeaker(i64MainGroupID, GetESDKMainSpeaker());
            AssignGroupMic(i64MainGroupID, GetESDKMainMic());
        }

        Audio::SwitchUserSpeakerAndMic();

        btnMicStatus->Caption->Text = IsMainMic()?"主麦克风":"副麦克风";
        btnSpeakerStatus->Caption->Text = IsMainSpeaker()?"主扬声器":"副扬声器";

        CheckSpeakMic(TRUE);
    }
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

    CreateOneForm(__classid(TDialogLock), &DialogLock);

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
    g_bSilentExit = TRUE;

    Visible = false;
    DCRelogin();
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
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerGetCfgVerTimer(TObject *Sender)
{
    SendServer(EV_DC_DIS_CFG_REQ, MSG_DCGetCfgDataVer);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormResize(TObject *Sender)
{
    if (TimerInit->Enabled)
    {
        return;
    }

    TimerResize->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerResizeTimer(TObject *Sender)
{
    if (!TimerResize->Enabled)
    {
        return;
    }

    TimerResize->Enabled = false;

    if (FormIPH)
    {
        FormIPH->Active();
    }

    static int iWidth = -1;

    if (this->Width != iWidth)
    {
        InitCtrlPos();
        iWidth = this->Width;
    }
}
//---------------------------------------------------------------------------

