#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmGroupCallWin.h"
#include "FrmGroupCall.h"
#include "FrmGroup.h"
#include "FrmCallIn.h"
#include "AudioDev.h"
#include "main.h"
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
TFormGroupCallWin *FormGroupCallWin;

void __fastcall TFormGroupCallWin::InitSkin()
{
}
//---------------------------------------------------------------------------
__fastcall TFormGroupCallWin::TFormGroupCallWin(TComponent* Owner)
    : TSubForm(Owner)
{
    m_i64GroupID = INVALID_GROUP_ID;
    m_bEmergencyCall = false;
    m_ulIdleTime = 0;

    gbMain->Style->LookAndFeel->SkinName = GetDefaultSkinName();

    m_iLeft = Left;
    m_iTop = Top;
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallWin::SetEmergenceyCall()
{
    m_bEmergencyCall = true;

    gbMain->LookAndFeel->SkinName = UICfg::GetEmergencyCallSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallWin::CloseEmergenceyCall()
{
    m_bEmergencyCall = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallWin::Init(INT64 i64GroupID, AnsiString strName, BOOL bIsPatchGroup)
{
    FormCallIn->PauseVoiceAlarm();

    m_bIsPatchGroup = bIsPatchGroup;
    m_i64PatchGroupID = INVALID_GROUP_ID;
    m_i64GroupID = i64GroupID;
    m_ulIdleTime = gos_get_uptime();

    if (m_bIsPatchGroup)
    {
        m_i64PatchGroupID = i64GroupID;
        btnDispatchCall->Enabled = false;
    }
    else
    {
        btnDispatchCall->Enabled = true;
    }

    Caption = "组呼";
    LabelGroupName->Caption = strName;
    LabelGroupID->Caption = i64GroupID;
    LabelStatus->Caption = "";

    UINT32 ulCallType = Group::GetGroupCallType(i64GroupID);

    if (ulCallType == CALL_TYPE_EMERGENCY_CALL)
    {
        gbMain->Style->LookAndFeel->SkinName = UICfg::GetEmergencyCallSkinName();
    }
    else
    {
        gbMain->Style->LookAndFeel->SkinName = GetDefaultSkinName();
    }

    UpdateMainGroupCall(i64GroupID);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallWin::btnPTTMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    OnPTTDial();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallWin::btnPTTMouseLeave(TObject *Sender)
{
    btnPTT->LookAndFeel->SkinName = GetDefaultSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallWin::btnPTTMouseEnter(TObject *Sender)
{
    btnPTT->LookAndFeel->SkinName = UICfg::GetPTTSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallWin::btnPTTMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    OnPTTRelease();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallWin::tbVolumePropertiesChange(TObject *Sender)
{
    if (m_i64GroupID == INVALID_GROUP_ID)
    {
        return;
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormGroupCallWin::FormShow(TObject *Sender)
{
//  InitSkin();
    btnPTT->Font->Size = 24;
    btnPTT->Font->Name = UICfg::GetButtonPTTFontName();

    Group::SubJoinGroup(m_i64GroupID);
    AssignGroupSpeaker(m_i64GroupID, GetESDKMainSpeaker());
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallWin::FormClose(TObject *Sender, TCloseAction &Action)
{
    Action = caHide;
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallWin::btnBreakoffClick(TObject *Sender)
{
    // 强制拆除组呼
    eSDK_GroupBreakoff(m_i64GroupID);

    // 如果组呼包含车载台、固定台，发送通知消息
    FormCallIn->StopGroupCall(m_i64GroupID);
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallWin::btnCloseGroupCallWinClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallWin::btnHangupClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallWin::UpdateGroupCallStatus(EVENT_GROUP_STATUS_T &stEvent)
{
    if (stEvent.i64GroupID != m_i64GroupID)
    {
        return;
    }

    LabelSpeaker->Caption = User::GetUserAlias(stEvent.ulSpeaker);

    if (LabelSpeaker->Caption.IsEmpty())
    {
        m_ulIdleTime = gos_get_uptime();
    }
    else
    {
        m_ulIdleTime = 0;
    }

    if (!LabelSpeaker->Caption.IsEmpty())
    {
        LabelStatus->Caption = "通话中";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallWin::btnGroupMemberClick(TObject *Sender)
{
    if (FormGroup)
    {
        FormGroup->SetGroupMember(m_i64GroupID);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallWin::FormHide(TObject *Sender)
{
    FormCallIn->ResumeVoiceAlarm();

    AssignGroupSpeaker(m_i64GroupID, GetESDKMonSpeaker());

    Group::AutoExitGroup(m_i64GroupID);

    UpdateMainGroupCall();

    if (m_i64PatchGroupID != INVALID_GROUP_ID)
    {
        PatchGroup::CancelPatchGroup(m_i64PatchGroupID);
        m_i64PatchGroupID = INVALID_GROUP_ID;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallWin::TimerCheckTimer(TObject *Sender)
{
    if (m_ulIdleTime > 0)
    {
        UINT32  ulTime = gos_get_uptime();
        UINT32  ulGroupCallIdleTime = GetGroupCallIdleTime();

        if (m_i64PatchGroupID != INVALID_GROUP_ID)
        {
            ulGroupCallIdleTime = 10;
        }

        if ((m_ulIdleTime+ulGroupCallIdleTime) < ulTime)
        {
            m_ulIdleTime = 0;
            FormCallIn->StopGroupCall(m_i64GroupID);
            Close();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallWin::OnPTTDial()
{
    if (!PTT::Dial(m_i64GroupID))
    {
        LabelStatus->Caption = "抢权失败";
        return;
    }

    btnPTT->LookAndFeel->SkinName = UICfg::GetPTTDownSkinName();
    LabelStatus->Caption = "通话中";
}

void __fastcall TFormGroupCallWin::OnPTTRelease()
{
    PTT::Release();
    btnPTT->LookAndFeel->SkinName = UICfg::GetPTTSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCallWin::CloseGroupCall(INT64 i64GroupID)
{
    return;
}

