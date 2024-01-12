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

#include "FrmMediaPlayer.h"
#include "DlgSetTrainUE.h"
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
#pragma link "cxLabel"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxMemo"
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
#pragma link "cxProgressBar"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxTrackBar"
#pragma link "WMPLib_OCX"
#pragma resource "*.dfm"
TFormMediaPlayer *FormMediaPlayer;


AnsiString GetMediaLength(int iTime)
{
    if (iTime <= 0)
    {
        return "";
    }

    char    acText[64];

    if (iTime < 3600)
    {
        sprintf(acText, "%02d:%02d", iTime/60, MOD(iTime, 60));
    }
    else
    {
        sprintf(acText, "%d:%02d:%02d", iTime/3600, MOD(iTime/60, 60), MOD(iTime, 60));
    }

    return acText;
}


//---------------------------------------------------------------------------
__fastcall TFormMediaPlayer::TFormMediaPlayer(TComponent* Owner)
    : TSubForm(Owner)
{
    WindowsMediaPlayer->uiMode = "none";
    m_strFile = "";
    TimerPlay->Enabled = false;
    m_bChangingPos = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMediaPlayer::Init(char *szFile)
{
    CHAR    *szRawFile = gos_right_strchr(szFile, '/');

    if (!szRawFile)
    {
        szRawFile = szFile;
    }
    else
    {
        szRawFile++;
    }

    Caption = "录音文件 - " + String(szRawFile);

    m_bChangingPos = false;
    m_strFile = szFile;
}
//---------------------------------------------------------------------------
void __fastcall TFormMediaPlayer::btnCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMediaPlayer::FormClose(TObject *Sender, TCloseAction &Action)
{
    btnStopClick(this);
    m_strFile = "";

    TimerPlay->Enabled = false;
    m_bChangingPos = false;

    Action = caHide;
}
//---------------------------------------------------------------------------
void __fastcall TFormMediaPlayer::PlayFile(String strFile)
{
    if (strFile.IsEmpty())
    {
        return;
    }

    WindowsMediaPlayer->URL = strFile;
    WindowsMediaPlayer->controls->play();

    WindowsMediaPlayer->uiMode = "none";
    WindowsMediaPlayer->stretchToFit = true;
    WindowsMediaPlayer->fullScreen =  false;
    WindowsMediaPlayer->enableContextMenu = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMediaPlayer::btnPlayClick(TObject *Sender)
{
    if (WindowsMediaPlayer->playState == WMPPlayState::wmppsPaused)
    {
        WindowsMediaPlayer->controls->play();
        TimerPlay->Enabled = true;

        return;
    }

    PlayFile(m_strFile);
}
//---------------------------------------------------------------------------
void __fastcall TFormMediaPlayer::btnStopClick(TObject *Sender)
{
    WindowsMediaPlayer->controls->stop();

    TimerPlay->Enabled = false;
    m_bChangingPos = false;
}
//---------------------------------------------------------------------------

void __fastcall TFormMediaPlayer::btnPauseClick(TObject *Sender)
{
    TimerPlay->Enabled = false;

    WindowsMediaPlayer->controls->pause();
}
//---------------------------------------------------------------------------

void __fastcall TFormMediaPlayer::TimerPlayTimer(TObject *Sender)
{
    if (!m_bChangingPos && tbPlay)
    {
        tbPlay->Position = WindowsMediaPlayer->controls->currentPosition;
        tbPlay->Hint = WindowsMediaPlayer->controls->currentPositionString;
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormMediaPlayer::tbPlayMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    m_bChangingPos = true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMediaPlayer::tbPlayMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    int iPos = X - 12;
    int iMax = tbPlay->Width - 24;

    if (iPos > 0 && iMax > 50)
    {
        tbPlay->Position = tbPlay->Properties->Max*iPos/iMax;
    }

    WindowsMediaPlayer->controls->currentPosition = tbPlay->Position;
    m_bChangingPos = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMediaPlayer::WindowsMediaPlayerPlayStateChange(TObject *Sender, long NewState)
{
/*
[controls] wmp.controls //播放器基本控制
11 controls.play; //播放
12 controls.pause; //暂停
13 controls.stop; //停止
14 controls.currentPosition:double; //当前进度
15 controls.currentPositionString:string; //当前进度，字符串格式。如“00:23”
16 controls.fastForward; //快进
17 controls.fastReverse; //快退
18 controls.next; //下一曲
19 controls.previous; //上一曲

wmp.settings //播放器基本设置
22 settings.volume:integer; //音量，0-100
23 settings.autoStart:Boolean; //是否自动播放
24 settings.mute:Boolean; //是否静音
25 settings.playCount:integer; //播放次数

[currentMedia] wmp.currentMedia //当前媒体属性
28 currentMedia.duration:double; //媒体总长度
29 currentMedia.durationString:string; //媒体总长度，字符串格式。如“03:24”
30 currentMedia.getItemInfo(const string); //获取当前媒体信息"Title"=媒体标题，"Author"=艺术家，"Copyright"=版权信息，"Description"=媒体内容描述，"Duration"=持续时间（秒），"FileSize"=文件大小，"FileType"=文件类型，"sourceURL"=原始地址
31 currentMedia.setItemInfo(const string); //通过属性名设置媒体信息
32 currentMedia.name:string; //同 currentMedia.getItemInfo("Title")

playState:integer; //播放状态，1=停止，2=暂停，3=播放，6=正在缓冲，9=正在连接，10=准备就绪
enum class WMPPlayState
*/

// Player.settings.setMode("loop", true);///实现循环播放

    if (NewState == WMPPlayState::wmppsPlaying)
    {
        tbPlay->ShowHint = true;
   //       tbPlay->Properties->Max = WindowsMediaPlayer->currentMedia->duration;
        if (WindowsMediaPlayer->currentMedia)
        {
            INT32       iTime = WindowsMediaPlayer->currentMedia->duration;

            tbPlay->Properties->Max = iTime;
            LabelMediaLength->Caption = GetMediaLength(iTime);

            int iInterval = 1000*iTime/tbPlay->Width;

            if (iTime <= 0)
            {
                TimerPlay->Enabled = false;
            }
            else
            {
                TimerPlay->Interval = iInterval<=500?500:iInterval;
                TimerPlay->Enabled = true;

                btnPlay->Visible = false;
                btnPause->Visible = true;

//              tbPlay->Enabled = true;
            }

            //PanelMediaPlayerResize(this);
        }
    }
    else if (NewState == WMPPlayState::wmppsStopped)
    {
        btnPlay->Visible = true;
        btnPause->Visible = false;
        LabelMediaLength->Caption = "";

        PanelSide1->Align = alClient;

//      tbPlay->Enabled = false;
        tbPlay->Position = 0;
//      gbPlay->Visible = false;

        tbPlay->ShowHint = false;
    }
    else if (NewState == WMPPlayState::wmppsPaused)
    {
        btnPlay->Visible = true;
        btnPause->Visible = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMediaPlayer::tbPlayPropertiesGetPositionHint(TObject *Sender,
          const int APosition, UnicodeString &AHintText, bool &ACanShow, bool &AIsHintMultiLine)
{
    AHintText = GetMediaLength(tbPlay->Position);
}
//---------------------------------------------------------------------------
void __fastcall TFormMediaPlayer::FormShow(TObject *Sender)
{
    btnPlayClick(this);
}
//---------------------------------------------------------------------------

