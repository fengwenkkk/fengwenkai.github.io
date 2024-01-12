#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmHalfDuplexP2PCallWin.h"
#include "FrmGroupCall.h"
#include "FrmGroup.h"
#include "FrmCallIn.h"
#include "AudioDev.h"
#include "main.h"
#include "FrmUser.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxGroupBox"
#pragma link "cxLabel"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxTrackBar"
#pragma link "dxSkinBlue"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "AdvSmoothPanel"
#pragma link "cxRadioGroup"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "dxZoomTrackBar"
#pragma link "cxImage"
#pragma link "dxGDIPlusClasses"
#pragma link "cxClasses"
#pragma link "dxSkinsForm"
#pragma link "dxBevel"
#pragma link "cxTextEdit"
#pragma resource "*.dfm"
TFormHalfDuplexP2PCallWin *FormHalfDuplexP2PCallWin;

//---------------------------------------------------------------------------
void ShowHalfDuplexP2PCallWin(UINT32 ulUserID)
{
    if (!APP::IsUIInited() ||
        !FormMain ||
        !FormHalfDuplexP2PCallWin)
    {
        return;
    }

    if (P2P::IsCalling())
    {
        InfoMsg("正在通话中");
        return;
    }

    if (FormMain != FormHalfDuplexP2PCallWin->Parent)
    {
        FormHalfDuplexP2PCallWin->Parent = FormMain; //gbMain;
//      FormHalfDuplexP2PCallWin->Left = FormMain->PanelMon->Left + 50;
//      FormHalfDuplexP2PCallWin->Top = FormMain->PanelMon->Top + FormMain->PanelCallIn->Height
 //                             - FormHalfDuplexP2PCallWin->Height - 80;
    }

    FormUser->StopListen();

    P2P::SetCalling();

    FormHalfDuplexP2PCallWin->Init(ulUserID);
    FormHalfDuplexP2PCallWin->BringToFront();
    FormHalfDuplexP2PCallWin->Show();
}
//---------------------------------------------------------------------------
void __fastcall TFormHalfDuplexP2PCallWin::InitSkin()
{

}
//---------------------------------------------------------------------------
__fastcall TFormHalfDuplexP2PCallWin::TFormHalfDuplexP2PCallWin(TComponent* Owner)
    : TSubForm(Owner)
{
    m_ulUserID = INVALID_USER_ID;
    m_ulIdleTime = 0;

    gbMain->Style->LookAndFeel->SkinName = GetDefaultSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormHalfDuplexP2PCallWin::Init(UINT32 ulUserID)
{
    FormCallIn->PauseVoiceAlarm();

    m_ulUserID = ulUserID;
    m_ulIdleTime = gos_get_uptime();

    LabelUserName->Caption = User::GetUserAlias(ulUserID);
    LabelUserID->Caption = ulUserID;
    LabelStatus->Caption = "";

    gbMain->Style->LookAndFeel->SkinName = GetDefaultSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormHalfDuplexP2PCallWin::btnPTTMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    OnPTTDial();
}
//---------------------------------------------------------------------------
void __fastcall TFormHalfDuplexP2PCallWin::btnPTTMouseLeave(TObject *Sender)
{
    btnPTT->LookAndFeel->SkinName = GetDefaultSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormHalfDuplexP2PCallWin::btnPTTMouseEnter(TObject *Sender)
{
    btnPTT->LookAndFeel->SkinName = UICfg::GetPTTSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormHalfDuplexP2PCallWin::btnPTTMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    OnPTTRelease();
}
//---------------------------------------------------------------------------
void __fastcall TFormHalfDuplexP2PCallWin::FormShow(TObject *Sender)
{
    btnPTT->Font->Size = 24;
    btnPTT->Font->Name = "Arial";
}
//---------------------------------------------------------------------------
void __fastcall TFormHalfDuplexP2PCallWin::FormClose(TObject *Sender, TCloseAction &Action)
{
    P2P::Hangup(m_ulUserID);

    FormCallIn->StopP2PCall(m_ulUserID);

    FormUser->CallOver();

    m_ulUserID = INVALID_USER_ID;
    Action = caHide;
}
//---------------------------------------------------------------------------
void __fastcall TFormHalfDuplexP2PCallWin::btnHangupClick(TObject *Sender)
{
    Close();

    /*if (P2PHangup(m_ulUserID))
    {
        Close();
    }
    else
    {
        LabelStatus->Caption = "挂断失败";
    }   */
}
//---------------------------------------------------------------------------
void __fastcall TFormHalfDuplexP2PCallWin::UpdateP2PCallStatus(EVENT_P2PCALL_STATUS_T &stEvent)
{
    if (stEvent.ulCaller != m_ulUserID &&
        stEvent.ulCallee != m_ulUserID )
    {
        return;
    }

    if (stEvent.ulCallee == INVALID_USER_ID)
    {
        return;
    }

    GosLog(LOG_INFO, "half p2p caller %u, callee %u, call status %u",
            stEvent.ulCaller,
            stEvent.ulCallee,
            stEvent.ulP2PCallStatus);

    if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_HALFDUPLEX_BEGIN )    // 指示收到半双工点呼请求
    {
        LabelStatus->Caption = "呼入请求";
    }
    else if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_TX_BEGIN)        // 指示半双工点呼开始
    {
        LabelStatus->Caption = "对方说话";
    }
    else if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_TX_IDLE)     // 指示半双工点呼空闲
    {
        LabelStatus->Caption = "空闲";
        LabelSpeaker->Caption = "";
    }
    else if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_TX_VOICE) // 指示半双工点呼音频
    {
        LabelStatus->Caption = "通话中";
    }
    else if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_FC_ACCEPTED) // 指示半双工点呼被接收
    {
        LabelStatus->Caption = "本地说话";
    }
    else if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_FC_REJECTED) // 指示半双工点呼被拒绝
    {
        LabelStatus->Caption = "";
    }

    if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_HANGUPED_ACTIVE   || // 指示已主动挂断
        stEvent.ulP2PCallStatus == P2P_IND_STATUS_CANCELLED         || // 指示点呼请求已取消(调度台不接而超时中止）
        stEvent.ulP2PCallStatus == P2P_IND_STATUS_RELEASED          || // 指示对端拒绝接听(调度台主叫）
        stEvent.ulP2PCallStatus == P2P_IND_STATUS_REMOTE_NOANSWER   || // 对端无响应
        stEvent.ulP2PCallStatus == P2P_IND_STATUS_RELEASED_BUSY     || // 指示对端忙
        stEvent.ulP2PCallStatus == P2P_IND_STATUS_HANGUPED          || // 指示对端已挂断
        stEvent.ulP2PCallStatus == P2P_IND_STATUS_FC_REJECTED)         // 指示对端拒绝
    {
        LabelStatus->Caption = "通话结束";

        Close();

        //m_ulUserID = INVALID_USER_ID;

//      StopP2PCallRecord(stEvent.ulCaller);

        FormCallIn->StopP2PCall(stEvent.ulCaller);
        FormCallIn->StopP2PCall(stEvent.ulCallee);

        FormUser->CallOver();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormHalfDuplexP2PCallWin::FormHide(TObject *Sender)
{
    FormCallIn->ResumeVoiceAlarm();

    if (m_ulUserID != INVALID_USER_ID)
    {
//      AssignUserSpeaker(m_ulUserID, GetESDKMonSpeaker());
        m_ulUserID = INVALID_USER_ID;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormHalfDuplexP2PCallWin::OnPTTDial()
{
    if (m_ulUserID == INVALID_USER_ID)
    {
        return;
    }

    if (!eSDK_P2PHalfDuplexDial(m_ulUserID))
    {
        LabelStatus->Caption = "抢权失败";
        return;
    }

ok:
    btnPTT->LookAndFeel->SkinName = UICfg::GetPTTDownSkinName();
    LabelStatus->Caption = "通话中";
}
//---------------------------------------------------------------------------
void __fastcall TFormHalfDuplexP2PCallWin::OnPTTRelease()
{
    eSDK_P2PHalfDuplexRelease(m_ulUserID);
    btnPTT->LookAndFeel->SkinName = UICfg::GetPTTSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormHalfDuplexP2PCallWin::CloseP2PCall(UINT32 ulUserID)
{
/*    if (ulUserID != m_ulUserID)
    {
        return;
    }

    m_ulUserID = INVALID_USER_ID;
    LabelStatus->Caption = "呼叫结束";   */
}


