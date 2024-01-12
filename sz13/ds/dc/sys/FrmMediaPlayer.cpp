//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmMediaPlayer.h"
#include "FrmLogin.h"
#include "main.h"
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
#pragma link "dxSkinGlassOceans"
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
#pragma link "cxCalendar"
#pragma link "cxDateUtils"
#pragma link "cxMaskEdit"
#pragma link "dxCore"
#pragma link "cxSpinEdit"
#pragma link "cxTimeEdit"
#pragma link "cxImageList"
#pragma link "cxClasses"
#pragma link "cxImageComboBox"
#pragma link "dxSkinsForm"
#pragma link "cxTrackBar"

#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "WMPLib_OCX"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxImage"
#pragma link "dxGDIPlusClasses"
#pragma link "cxSplitter"
#pragma resource "*.dfm"
TFormMediaPlayer *FormMediaPlayer;

extern AnsiString GetMediaLength(int iTime);

void __fastcall TFormMediaPlayer::InitSkin()
{
    tbPlay->Style->LookAndFeel->SkinName = UICfg::GetVolumeSkinName();
//    WindowsMediaPlayer->uiMode = "none";
}
//---------------------------------------------------------------------------
__fastcall TFormMediaPlayer::TFormMediaPlayer(TComponent* Owner)
    : TSubForm(Owner)
{
    m_bChangingPos = false;

    btnPause->Visible = false;
    btnPlay->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMediaPlayer::FormCreate(TObject *Sender)
{
    SetImage(btnPlay, IMG_PLAY);
    SetImage(btnPause, IMG_PAUSE);
    SetImage(btnStop, IMG_STOP);
}
//---------------------------------------------------------------------------
void __fastcall TFormMediaPlayer::Play()
{
    if (m_strFile.IsEmpty())
    {
        return;
    }

    if (WindowsMediaPlayer->URL == m_strFile)
    {
        if (WindowsMediaPlayer->playState == WMPPlayState::wmppsPaused)
        {
            WindowsMediaPlayer->controls->play();
            TimerPlay->Enabled = true;

            return;
        }
    }
    else
    {
        WindowsMediaPlayer->controls->stop();
    }

    WindowsMediaPlayer->URL = m_strFile;
    m_strFile = WindowsMediaPlayer->URL; // URL���ʽ���ļ���
    WindowsMediaPlayer->controls->play();

    WindowsMediaPlayer->uiMode = "none";
    WindowsMediaPlayer->stretchToFit = true;
    WindowsMediaPlayer->fullScreen =  false;
    WindowsMediaPlayer->enableContextMenu = false;

}
//---------------------------------------------------------------------------
void __fastcall TFormMediaPlayer::Stop()
{
    if (WindowsMediaPlayer->controls &&
        WindowsMediaPlayer->playState != WMPPlayState::wmppsStopped)
    {
        WindowsMediaPlayer->controls->stop();
    }

    TimerPlay->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMediaPlayer::btnPauseClick(TObject *Sender)
{
    TimerPlay->Enabled = false;
    WindowsMediaPlayer->controls->pause();
}
//---------------------------------------------------------------------------
void __fastcall TFormMediaPlayer::btnStopClick(TObject *Sender)
{
    Stop();
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
void __fastcall TFormMediaPlayer::WindowsMediaPlayerPlayStateChange(TObject *Sender,
          long NewState)
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
        PanelMediaPlayer->Visible = true;
        tbPlay->ShowHint = true;
        if (WindowsMediaPlayer->currentMedia)
        {
            INT32       iTime = WindowsMediaPlayer->currentMedia->duration;

            tbPlay->Properties->Max = iTime;
            LabelMediaLength->Caption = GetMediaLength(iTime);

            if (iTime <= 0 || tbPlay->Width <= 0)
            {
                TimerPlay->Enabled = false;
            }
            else
            {
                int iInterval = 1000*iTime/tbPlay->Width;

                TimerPlay->Interval = iInterval<=500?500:iInterval;
                TimerPlay->Enabled = true;

                btnPlay->Visible = false;
                btnPause->Visible = true;
                btnStop->Enabled = true;
            }

            PanelMediaPlayerResize(this);
        }
    }
    else if (NewState == WMPPlayState::wmppsStopped)
    {
        btnPlay->Visible = true;
        btnPause->Visible = false;
        btnStop->Enabled = false;
        LabelMediaLength->Caption = "";

        PanelSide1->Align = alClient;
        PanelMediaPlayer->Visible = false;

        tbPlay->Position = 0;
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
void __fastcall TFormMediaPlayer::PanelMediaPlayerResize(TObject *Sender)
{
    if (!WindowsMediaPlayer->currentMedia)
    {
        PanelSide1->Align = alClient;
        PanelSide1->BringToFront();

        return;
    }

    int iMediaWidth = WindowsMediaPlayer->currentMedia->imageSourceWidth;
    int iMediaHeight = WindowsMediaPlayer->currentMedia->imageSourceHeight;
    int iTotalWidth = WindowsMediaPlayer->Parent->Width;
    int iTotalHeight = WindowsMediaPlayer->Parent->Height;

    if (iMediaWidth <= 0 || iMediaHeight <= 0)
    {
        PanelSide1->Align = alClient;
        PanelSide1->BringToFront();

        return;
    }

    double  dRateOri = ((double)iTotalWidth)/iTotalHeight;
    double  dRate = ((double)iMediaWidth)/iMediaHeight;
    int     iPlayWidth;
    int     iPlayHeight;

    if (dRateOri > dRate)
    {
        iPlayHeight = iTotalHeight;
        iPlayWidth = iTotalHeight*iMediaWidth/iMediaHeight;

        PanelSide1->Align = alLeft;
        PanelSide1->Width = (iTotalWidth - iPlayWidth)/2;

        PanelSide2->Align = alRight;
        PanelSide2->Width = iTotalWidth - PanelSide1->Width - iPlayWidth;
    }
    else
    {
        iPlayWidth = iTotalWidth;
        iPlayHeight = iTotalWidth*iMediaHeight/iMediaWidth;

        PanelSide1->Align = alTop;
        PanelSide1->Height = (iTotalHeight - iPlayHeight)/2;

        PanelSide2->Align = alBottom;
        PanelSide2->Height = iTotalHeight - PanelSide1->Height - iPlayHeight;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMediaPlayer::FormClose(TObject *Sender, TCloseAction &Action)
{
    m_bChangingPos = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMediaPlayer::FormResize(TObject *Sender)
{
    gbPlay->Visible = !gbPlay->Visible;
    gbPlay->Visible = !gbPlay->Visible;
}
//---------------------------------------------------------------------------
void __fastcall TFormMediaPlayer::btnPlayClick(TObject *Sender)
{
    Play();
}
//---------------------------------------------------------------------------
void __fastcall TFormMediaPlayer::SetFile(AnsiString strFile)
{
    btnPause->Visible = false;
    btnPlay->Visible = true;
    btnStop->Enabled = false;

    if (!FileExists(strFile))
    {
        btnPlay->Enabled = false;
        return;
    }

    btnPlay->Enabled = true;
    m_strFile = strFile;
}

