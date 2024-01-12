#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "UICfg.h"
#include "ClientData.h"
#include "FrmP2PCallWin.h"
#include "FrmGroupCall.h"
#include "FrmGroup.h"
#include "FrmCallIn.h"
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
TFormP2PCallWin *FormP2PCallWin;

void __fastcall TFormP2PCallWin::InitSkin()
{
}
//---------------------------------------------------------------------------
__fastcall TFormP2PCallWin::TFormP2PCallWin(TComponent* Owner)
    : TSubForm(Owner)
{
    m_ulCallingNumber = INVALID_USER_ID;
}
//---------------------------------------------------------------------------
void __fastcall TFormP2PCallWin::Init(UINT32 ulCallingNumber, BOOL bEmergencyCall)
{
    FormCallIn->PauseVoiceAlarm();

    Caption = "个呼呼入";
    btnHangoff->Visible = false;
    btnCallTransfer->Enabled = true;

    LabelCallingNumber->Caption = ulCallingNumber;
    LabelStatus->Caption = "";
    edtTransferNumber->Text = "";

    m_ulCallingNumber = ulCallingNumber;

    if (bEmergencyCall)
    {
        gbMain->Style->LookAndFeel->SkinName = UICfg::GetEmergencyCallSkinName();
    }
    else
    {
        gbMain->Style->LookAndFeel->SkinName = GetDefaultSkinName();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormP2PCallWin::CallOver()
{
    TimerCheck->Enabled = false;

  //  SetP2PCalling(FALSE);
    if (m_ulCallingNumber != INVALID_USER_ID)
    {
        P2P::Hangup();
        m_ulCallingNumber = INVALID_USER_ID;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormP2PCallWin::FormClose(TObject *Sender, TCloseAction &Action)
{
    CallOver();

    Action = caHide;
}
//---------------------------------------------------------------------------
void __fastcall TFormP2PCallWin::btnHangoffClick(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------
void __fastcall TFormP2PCallWin::btnCallTransferClick(TObject *Sender)
{
    UINT32  ulTransferNumber;

    if (!ToInt(edtTransferNumber->Text, &ulTransferNumber))
    {
        LabelStatus->Caption = "转接号码非法";
        edtTransferNumber->SetFocus();
        return;
    }

    if (!P2P::CallTransfer(ulTransferNumber))
    {
        LabelStatus->Caption = "呼叫转接失败";
        return;
    }

    LabelStatus->Caption = "呼叫转接成功";
    btnCallTransfer->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormP2PCallWin::btnCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormP2PCallWin::UpdateP2PCallStatus(EVENT_P2PCALL_STATUS_T &stEvent)
{
    UINT32  ulStatus = 0xffffffff;
    BOOL    bClose = FALSE;

    // 判断主叫号码是否是当前呼叫的号码
    if (stEvent.ulCaller != m_ulCallingNumber)
    {
        return;
    }

    if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_ANSWERED )
    {
        LabelStatus->Caption = "被叫已接听";
    }
    else if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_PICKEDUP)        // 指示被叫端已接听
    {
        LabelStatus->Caption = "通话中";
    }
    else if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_HANGUPED_ACTIVE) // 指示已主动挂断
    {
        LabelStatus->Caption = "被叫已挂断";
        bClose = TRUE;
    }
    else if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_REMOTE_NOANSWER) // 对端无响应
    {
        LabelStatus->Caption = "被叫无响应";
        bClose = TRUE;
    }
    else if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_RELEASED_BUSY)   // 指示对端忙
    {
        LabelStatus->Caption = "被叫忙";
        bClose = TRUE;
    }
    else if (stEvent.ulP2PCallStatus == P2P_IND_STATUS_HANGUPED)    // 指示对端已挂断
    {
        LabelStatus->Caption = "被叫已挂断";
        bClose = TRUE;
    }
    else
    {
        return;
    }

    if (bClose)
    {
        CallOver();

        TimerCheck->Enabled = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormP2PCallWin::TimerCheckTimer(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormP2PCallWin::FormDeactivate(TObject *Sender)
{
    FormCallIn->ResumeVoiceAlarm();
}
//---------------------------------------------------------------------------

