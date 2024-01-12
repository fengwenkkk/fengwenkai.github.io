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

    Caption = "¼���ļ� - " + String(szRawFile);

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
[controls] wmp.controls //��������������
11 controls.play; //����
12 controls.pause; //��ͣ
13 controls.stop; //ֹͣ
14 controls.currentPosition:double; //��ǰ����
15 controls.currentPositionString:string; //��ǰ���ȣ��ַ�����ʽ���硰00:23��
16 controls.fastForward; //���
17 controls.fastReverse; //����
18 controls.next; //��һ��
19 controls.previous; //��һ��

wmp.settings //��������������
22 settings.volume:integer; //������0-100
23 settings.autoStart:Boolean; //�Ƿ��Զ�����
24 settings.mute:Boolean; //�Ƿ���
25 settings.playCount:integer; //���Ŵ���

[currentMedia] wmp.currentMedia //��ǰý������
28 currentMedia.duration:double; //ý���ܳ���
29 currentMedia.durationString:string; //ý���ܳ��ȣ��ַ�����ʽ���硰03:24��
30 currentMedia.getItemInfo(const string); //��ȡ��ǰý����Ϣ"Title"=ý����⣬"Author"=�����ң�"Copyright"=��Ȩ��Ϣ��"Description"=ý������������"Duration"=����ʱ�䣨�룩��"FileSize"=�ļ���С��"FileType"=�ļ����ͣ�"sourceURL"=ԭʼ��ַ
31 currentMedia.setItemInfo(const string); //ͨ������������ý����Ϣ
32 currentMedia.name:string; //ͬ currentMedia.getItemInfo("Title")

playState:integer; //����״̬��1=ֹͣ��2=��ͣ��3=���ţ�6=���ڻ��壬9=�������ӣ�10=׼������
enum class WMPPlayState
*/

// Player.settings.setMode("loop", true);///ʵ��ѭ������

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

