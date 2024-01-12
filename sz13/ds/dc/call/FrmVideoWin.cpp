//---------------------------------------------------------------------------

#include <vcl.h>
#include <set>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmVideoCall.h"
#include "FrmVideoWin.h"
#include "DlgSubUser.h"
#include "DlgSelectVideoChan.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxGroupBox"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "dxSkinBlue"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "MDIForm"
#pragma link "AdvSmoothPanel"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxClasses"
#pragma link "dxBar"
#pragma resource "*.dfm"
TFormVideoWin *FormVideoWin;

extern BOOL IsFullScreen();
extern void ExitFullScreen();
extern UINT32 GetVideoWinNum();
extern VECTOR<TFormVideoWin*>& GetAllVideoWin();

static TFormVideoWin                *g_SelectedGroupCallVideoWin = NULL;
static std::set<TFormVideoWin*>     g_sGroupCallVideoWin;

TFormVideoWin* GetSelectedGroupCallVideoWin()
{
    return g_SelectedGroupCallVideoWin;
}

//---------------------------------------------------------------------------
__fastcall TFormVideoWin::TFormVideoWin(TComponent* Owner)
    : TSubForm(Owner)
{
    m_bSelected = false;
    m_i64GroupID = INVALID_GROUP_ID;

    pmiSetVideoSource->Visible = ivNever;

    if (AllowVideoWall())
    {
        pmiVWallStart->Visible = ivAlways;
        pmiVWallStop->Visible = ivAlways;
    }
    else
    {
        pmiVWallStart->Visible = ivNever;
        pmiVWallStop->Visible = ivNever;
    }

    m_bVideoGroupCall = false;

    Init();

    if (GetVideoWinNum() == 0 ||
        g_sGroupCallVideoWin.size() == 0)
    {
        gbMain->Style->LookAndFeel->SkinName = UICfg::GetActiveVideoWinSkinName();
    }
    else
    {
        gbMain->Style->LookAndFeel->SkinName = UICfg::GetDefaultVideoWinSkinName();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoWin::SetVideoPlayWindow()
{
    if (!m_bPlaying)
    {
        return;
    }

    if (!eSDK_SetVideoPlayWindow(m_ulUserID, gbVideo->Handle))
    {
        Stop();
    }
}
//---------------------------------------------------------------------------
bool __fastcall TFormVideoWin::Play(UINT32 ulUserID, UINT32 ulVideoFormat, UINT32 ulCameraType,
                                    BOOL bNeedConfirm, BOOL bHasVoice)
{
    if (!P2P::PullVideo(ulUserID, ulVideoFormat, ulCameraType, bNeedConfirm, bHasVoice))
    {
        FormVideoCall->SaveP2PVideoCallInfo(CALL_TYPE_VIDEO_REALPLAY, APP::GetCurrDCUserID(), ulUserID, CALL_STATUS_DIAL_FAIL);
        Init();
        return false;
    }

    if (!eSDK_SetVideoPlayWindow(ulUserID, gbVideo->Handle))
    {
        P2P::StopVideo(ulUserID);
        FormVideoCall->SaveP2PVideoCallInfo(CALL_TYPE_VIDEO_REALPLAY, APP::GetCurrDCUserID(), ulUserID, CALL_STATUS_PLAY_FAIL);
        Init();
        return false;
    }

    FormVideoCall->SaveP2PVideoCallInfo(CALL_TYPE_VIDEO_REALPLAY, APP::GetCurrDCUserID(), ulUserID, CALL_STATUS_PLAY);

    m_bPlaying = true;
    m_ulUserID = ulUserID;
    m_ulVideoFormat = ulVideoFormat;
    m_ulCameraType = ulCameraType;
    m_bNeedConfirm = bNeedConfirm;
    m_bHasVoice = bHasVoice;

    m_ulCallType = CALL_TYPE_VIDEO_REALPLAY;
    m_bIsOutCalling = false;

    gbMain->PanelStyle->Active = false;

    if (m_bVideoGroupCall)
    {
        gbMain->Caption = m_strCaption + " - " + m_ulUserID;
    }
    else
    {
        gbMain->Caption = m_ulUserID;
    }

    btnStartVideoDispatch->Visible = true;
    btnStop->Visible = true;

    return true;
}
//---------------------------------------------------------------------------
bool __fastcall TFormVideoWin::Call(UINT32 ulUserID, UINT32 ulVideoFormat, UINT32 ulCameraType,
                                    BOOL bNeedConfirm, BOOL bHasVoice)
{
    if (!P2P::VideoCall(ulUserID, ulVideoFormat, ulCameraType, bNeedConfirm, bHasVoice))
    {
        FormVideoCall->SaveP2PVideoCallInfo(CALL_TYPE_VIDEO_P2P, APP::GetCurrDCUserID(), ulUserID, CALL_STATUS_DIAL_FAIL);
        Init();
        return false;
    }

    if (!eSDK_SetVideoPlayWindow(ulUserID, gbVideo->Handle))
    {
        P2P::StopVideo(ulUserID);
        FormVideoCall->SaveP2PVideoCallInfo(CALL_TYPE_VIDEO_P2P, APP::GetCurrDCUserID(), ulUserID, CALL_STATUS_PLAY_FAIL);
        Init();
        return false;
    }

    FormVideoCall->SaveP2PVideoCallInfo(CALL_TYPE_VIDEO_P2P, APP::GetCurrDCUserID(), ulUserID, CALL_STATUS_PLAY);

    m_bPlaying = true;
    m_ulUserID = ulUserID;
    m_ulVideoFormat = ulVideoFormat;
    m_ulCameraType = ulCameraType;
    m_bNeedConfirm = bNeedConfirm;
    m_bHasVoice = bHasVoice;

    m_ulCallType = CALL_TYPE_VIDEO_P2P;
    m_bIsOutCalling = true;

    gbMain->PanelStyle->Active = false;

    if (m_bVideoGroupCall)
    {
        gbMain->Caption = m_strCaption + " - " + m_ulUserID;
    }
    else
    {
        gbMain->Caption = m_ulUserID;
    }

    btnStartVideoDispatch->Visible = true;
    btnStop->Visible = true;

    return true;
}
//---------------------------------------------------------------------------
bool __fastcall TFormVideoWin::Play()
{
    if (!eSDK_SetVideoPlayWindow(m_ulUserID, gbVideo->Handle))
    {
        FormVideoCall->SaveP2PVideoCallInfo(m_ulCallType, m_ulUserID, APP::GetCurrDCUserID(), CALL_STATUS_PLAY_FAIL);
        Init(false);
        return false;
    }

    FormVideoCall->SaveP2PVideoCallInfo(m_ulCallType, m_ulUserID, APP::GetCurrDCUserID(), CALL_STATUS_PLAY);

    return true;
}
//---------------------------------------------------------------------------
bool __fastcall TFormVideoWin::Recv(UINT32 ulCallType, UINT32 ulUserID, UINT32 ulVideoFormat, UINT32 ulCameraType)
{
    if (!P2P::RecvVideo(ulUserID))
    {
        return false;
    }

    m_bPlaying = true;
    m_ulUserID = ulUserID;
    m_ulVideoFormat = ulVideoFormat;
    m_ulCameraType = ulCameraType;
    m_ulCallType = ulCallType;
    m_bIsOutCalling = false;

    gbMain->PanelStyle->Active = false;

    if (m_bVideoGroupCall)
    {
        gbMain->Caption = m_strCaption + " - " + m_ulUserID;
    }
    else
    {
        gbMain->Caption = m_ulUserID;
    }

    btnStartVideoDispatch->Visible = true;
    btnStop->Visible = true;

    return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoWin::Init(bool bCloseTitle)
{
    m_bPlaying = false;
    m_ulUserID = INVALID_USER_ID;
    m_ulCallType = CALL_TYPE_UNKNOWN;
    m_bIsOutCalling = false;
    m_iVideoChanID = -1;

    if (!m_bVideoGroupCall)
    {
        gbMain->Caption = "";
        gbMain->PanelStyle->Active = true;
        btnStop->Visible = false;
    }
    else
    {
        btnStop->Visible = true;
    }

    btnStartVideoDispatch->Visible = false;

    gbVideo->Invalidate();
    gbVideo->Update();

    m_sSelectUser.Clear();
}
//---------------------------------------------------------------------------
bool __fastcall TFormVideoWin::Stop()
{
    if (!m_bPlaying)
    {
        return true;
    }

    m_bPlaying = false;

    if (m_ulUserID != INVALID_USER_ID)
    {
        P2P::StopVideo(m_ulUserID);

        if (m_ulCallType == CALL_TYPE_VIDEO_REALPLAY)
        {
            FormVideoCall->SaveP2PVideoCallInfo(m_ulCallType, APP::GetCurrDCUserID(), m_ulUserID, CALL_STATUS_CLOSE);
        }
        else if (m_ulCallType == CALL_TYPE_VIDEO_RECV )
        {
            FormVideoCall->SaveP2PVideoCallInfo(m_ulCallType, m_ulUserID, APP::GetCurrDCUserID(), CALL_STATUS_CLOSE);
        }
        else if (m_ulCallType == CALL_TYPE_VIDEO_P2P)
        {
            // 被叫是呼出
            if (m_bIsOutCalling)
            {
                FormVideoCall->SaveP2PVideoCallInfo(m_ulCallType, APP::GetCurrDCUserID(), m_ulUserID, CALL_STATUS_CLOSE);
            }
            else
            {
                FormVideoCall->SaveP2PVideoCallInfo(m_ulCallType, m_ulUserID, APP::GetCurrDCUserID(), CALL_STATUS_CLOSE);
            }
        }
    }

//    Init();
    FormVideoCall->StopVideoPlay();

    return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoWin::PanelMainResize(TObject *Sender)
{
    int iBorderWidth = PanelMain->Fill->BorderWidth;

    gbMain->Left = iBorderWidth;
    gbMain->Top = iBorderWidth;
    gbMain->Width = PanelMain->Width - 2*iBorderWidth;
    gbMain->Height = PanelMain->Height - 2*iBorderWidth;
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoWin::btnStopClick(TObject *Sender)
{
    m_bCloseVideoGroupCall = true;
    Stop();
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoWin::btnFullWinClick(TObject *Sender)
{
    FormVideoCall->FullScreenVideoWin(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoWin::btnStartVideoDispatchClick(TObject *Sender)
{
    IntSet  sOld;
    IntSet  sNew;
    UINT32  ulValue;
    VECTOR<UINT32>  vUser;
    IntSet          sHideUser;

    sHideUser.Add(m_ulUserID);
    sHideUser.Add(APP::GetCurrDCUserID());

    sOld.Init(m_sSelectUser);

    DialogSubUser->Init(sOld, sHideUser);
    DialogSubUser->ShowModal();
    DialogSubUser->Get(sNew);

    sNew.GetList(vUser);

#if 0  // TODO
    // 去除相同的部分
    for (UINT32 i=0; i<vUser.size(); i++)
    {
        ulValue = vUser[i];
        if (sOld.Del(ulValue))
        {
            sNew.Del(ulValue);
        }
    }

    sOld.GetList(vUser);
    for (UINT32 i=0; i<vUser.size(); i++)
    {
        if (eSDK_StopVideoDispatch(m_ulUserID, vUser[i]))
        {
            m_sSelectUser.Del(vUser[i]);
        }
    }

    sNew.GetList(vUser);
#endif

    if (!eSDK_StartVideoDispatch(m_ulUserID, vUser, IsOriginalVideoFormat()))
    {
        ErrMsg("视频转发失败");
        return;
    }

    m_sSelectUser.Add(vUser);
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoWin::btnStopVideoDispatchClick(TObject *Sender)
{
    VECTOR<UINT32>  vUser;
    UINT32          ulUserID;

    m_sSelectUser.GetList(vUser);

    for (INT32 i=vUser.size()-1; i>=0; i--)
    {
        ulUserID = vUser[i];
        if (ulUserID == m_ulUserID ||
            ulUserID == APP::GetCurrDCUserID() )
        {
            continue;
        }

        if (eSDK_StopVideoDispatch(m_ulUserID, ulUserID))
        {
            m_sSelectUser.Del(ulUserID);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoWin::MyOnMessage(tagMSG &Msg, bool &Handled)
{
    if (Msg.message == WM_KEYDOWN && Msg.wParam == VK_ESCAPE)
    {
        if (IsFullScreen())
        {
            ExitFullScreen();
            Handled = true;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoWin::FormDestroy(TObject *Sender)
{
    if (m_bPlaying)
    {
        Stop();
    }

    if (m_bVideoGroupCall)
    {
        g_sGroupCallVideoWin.erase(this);

        if (g_SelectedGroupCallVideoWin == this)
        {
            g_SelectedGroupCallVideoWin = NULL;
        }

        // 恢复视频回传标题颜色
        if (g_sGroupCallVideoWin.size() == 0)
        {
            VECTOR<TFormVideoWin*>  &vWin = GetAllVideoWin();

            for (UINT32 i=0; i<vWin.size(); i++)
            {
                if (vWin[i] != this)
                {
                    vWin[i]->gbMain->Style->LookAndFeel->SkinName = UICfg::GetActiveVideoWinSkinName();
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoWin::pmiOriginalFormatClick(TObject *Sender)
{
    SetOriginalVideoFormat(pmiOriginalFormat->Down);
    SaveLocalConfInfo();
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoWin::pmMainPopup(TObject *Sender)
{
    pmiOriginalFormat->Down = IsOriginalVideoFormat();

    pmiVideoTransfer->Enabled = m_bPlaying;

    if (m_iVideoChanID < 0)
    {
        pmiVWallStart->Enabled = true;
        pmiVWallStop->Enabled = false;
    }
    else
    {
        pmiVWallStart->Enabled = false;
        pmiVWallStop->Enabled = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoWin::InitVideoGroupCall(INT64 i64GroupID, UINT32 ulVideoFormat, UINT32 ulCameraType, BOOL bNeedConfirm, BOOL bHasVoice)
{
    m_i64GroupID = i64GroupID;
    m_bVideoGroupCall = true;
    m_bCloseVideoGroupCall = false;
//    btnSetVideoSource->Visible = true;
    pmiSetVideoSource->Visible = ivAlways;

    m_ulVideoFormat = ulVideoFormat;
    m_ulCameraType = ulCameraType;
    m_bNeedConfirm = bNeedConfirm;
    m_bHasVoice = bHasVoice;

    btnStop->Visible = true;

    m_strCaption.printf("视频组呼:"FORMAT_I64, i64GroupID);
    gbMain->Caption = m_strCaption;
    gbMain->PanelStyle->Active = false;

    g_sGroupCallVideoWin.insert(this);
    if (g_sGroupCallVideoWin.size() == 1)
    {
        gbVideoClick(this);
    }
    else
    {
        gbMain->Style->LookAndFeel->SkinName = UICfg::GetDefaultVideoWinSkinName();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoWin::gbVideoClick(TObject *Sender)
{
    m_bSelected = true;

    if (!m_bVideoGroupCall ||
        g_SelectedGroupCallVideoWin == this)
    {
        return;
    }

    // 修改其它窗口的标题颜色
    VECTOR<TFormVideoWin*>  &vWin = GetAllVideoWin();

    for (UINT32 i=0; i<vWin.size(); i++)
    {
        if (vWin[i] != this)
        {
            vWin[i]->gbMain->Style->LookAndFeel->SkinName = UICfg::GetDefaultVideoWinSkinName();
        }
    }

    g_SelectedGroupCallVideoWin = this;
    gbMain->Style->LookAndFeel->SkinName = UICfg::GetActiveVideoWinSkinName();

}
//---------------------------------------------------------------------------

void __fastcall TFormVideoWin::gbVideoDragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept)
{
    Accept = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoWin::gbVideoDragDrop(TObject *Sender, TObject *Source, int X,
          int Y)
{
    UINT32  ulOriUserID = m_ulUserID;
    UINT32  ulUserID = FormVideoCall->GetDragUserID();

    if (ulUserID == INVALID_USER_ID)
    {
        return;
    }

    if (ulOriUserID != INVALID_USER_ID)
    {
        P2P::StopVideo(ulOriUserID);
    }

    if (!Play(ulUserID, m_ulVideoFormat, m_ulCameraType, m_bNeedConfirm, m_bHasVoice))
    {
        return;
    }

    if (m_bVideoGroupCall)
    {
        gos_sleep_1ms(500);

        // 自动分发
        VECTOR<UINT32>  vUser;
        VECTOR<INT64>   vGroup;

        if (!Group::GetGroupMember(m_i64GroupID, vUser, vGroup))
        {
            ErrMsg("视频转发失败");
            return;
        }

        for (UINT32 i=0; i<vGroup.size(); i++)
        {
            VECTOR<UINT32>  vTmpUser;
            VECTOR<INT64>   vTmpGroup;

            Group::GetGroupMember(vGroup[i], vTmpUser, vTmpGroup);
            AppendMember(vUser, vTmpUser);
        }

        if (!eSDK_StartVideoDispatch(ulUserID, vUser, IsOriginalVideoFormat()))
        {
            ErrMsg("视频转发失败");
            return;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoWin::pmiSetVideoSourceClick(TObject *Sender)
{
    FormVideoCall->pcMain->ActivePage = FormVideoCall->tsUser;
//
}
//---------------------------------------------------------------------------
bool __fastcall TFormVideoWin::IsPlaying()
{
    if (m_bVideoGroupCall)
    {
        return !m_bCloseVideoGroupCall;
    }

    return m_bPlaying;
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoWin::FormShow(TObject *Sender)
{
    gbMain->Style->Font->Size = 14;
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoWin::FormCreate(TObject *Sender)
{
    SetImage(pmiVWallStart, IMG_UP_LINE);
    SetImage(pmiVWallStop, IMG_STOP);
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoWin::pmiVWallStartClick(TObject *Sender)
{
    VECTOR<ESDK_VWALL_INFO_T>   vInfo;

    if (!eSDK_GetVWallInfo(vInfo))
    {
        InfoMsg("获取视频墙状态失败");
        return;
    }

    DialogSelectVideoChan->Init(m_ulUserID, vInfo);
    if (DialogSelectVideoChan->ShowModal() != mrOk)
    {
        return;
    }

    UINT32      ulVideoChanID = 0;

    DialogSelectVideoChan->Get(ulVideoChanID);

    if (!eSDK_VWallStart(m_ulUserID, ulVideoChanID))
    {
        InfoMsg("视频上墙失败");
        return;
    }

    m_iVideoChanID = ulVideoChanID;
}
//---------------------------------------------------------------------------
void __fastcall TFormVideoWin::pmiVWallStopClick(TObject *Sender)
{
    if (!eSDK_VWallStop(m_ulUserID, m_iVideoChanID))
    {
        InfoMsg("结束视频上墙失败");
        return;
    }

    m_iVideoChanID = -1;
}
//---------------------------------------------------------------------------

