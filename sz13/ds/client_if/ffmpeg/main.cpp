//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"
#include "MsgDialog.h"
#include "ClientUtil.h"

#include "main.h"
#include "ffmpeg_rec.h"
#include "ThreadRTPPlay.h"
#include "ThreadPlayAndRec.h"
#include "FrmPlayAndRec.h"
#include "ffmpeg_pcm2wav.h"
#include "ThreadPcmPlay.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxGraphics"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "dxSkinBlack"
#pragma link "dxSkinBlue"
#pragma link "dxSkinCaramel"
#pragma link "dxSkinCoffee"
#pragma link "dxSkinDarkRoom"
#pragma link "dxSkinDarkSide"
#pragma link "dxSkinFoggy"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkiniMaginary"
#pragma link "dxSkinLilian"
#pragma link "dxSkinLiquidSky"
#pragma link "dxSkinLondonLiquidSky"
#pragma link "dxSkinMcSkin"
#pragma link "dxSkinMoneyTwins"
#pragma link "dxSkinOffice2007Black"
#pragma link "dxSkinOffice2007Blue"
#pragma link "dxSkinOffice2007Green"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinOffice2007Silver"
#pragma link "dxSkinOffice2010Black"
#pragma link "dxSkinOffice2010Blue"
#pragma link "dxSkinOffice2010Silver"
#pragma link "dxSkinPumpkin"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSeven"
#pragma link "dxSkinSharp"
#pragma link "dxSkinSilver"
#pragma link "dxSkinSpringTime"
#pragma link "dxSkinStardust"
#pragma link "dxSkinSummer2008"
#pragma link "dxSkinValentine"
#pragma link "dxSkinXmas2008Blue"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxDropDownEdit"
#pragma link "cxEdit"
#pragma link "cxMaskEdit"
#pragma link "cxTextEdit"
#pragma link "cxCustomData"
#pragma link "cxInplaceContainer"
#pragma link "cxStyles"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "dxSkinsForm"
#pragma link "cxPC"
#pragma link "dxSkinscxPCPainter"
#pragma link "cxGroupBox"
#pragma link "cxClasses"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinDevExpressStyle"
#pragma link "dxSkinHighContrast"
#pragma link "dxSkinMetropolis"
#pragma link "dxSkinMetropolisDark"
#pragma link "dxSkinOffice2013DarkGray"
#pragma link "dxSkinOffice2013LightGray"
#pragma link "dxSkinOffice2013White"
#pragma link "dxSkinOffice2016Colorful"
#pragma link "dxSkinOffice2016Dark"
#pragma link "dxSkinSevenClassic"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinTheAsphaltWorld"
#pragma link "dxSkinTheBezier"
#pragma link "dxSkinVisualStudio2013Blue"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "dxSkinVisualStudio2013Light"
#pragma link "dxSkinVS2010"
#pragma link "dxSkinWhiteprint"
#pragma link "cxMemo"
#pragma link "dxBarBuiltInMenu"
#pragma link "cxDrawTextUtils"
#pragma link "dxBkgnd"
#pragma link "dxPrnDev"
#pragma link "dxPrnPg"
#pragma link "dxPSCompsProvider"
#pragma link "dxPSCore"
#pragma link "dxPScxEditorProducers"
#pragma link "dxPScxExtEditorProducers"
#pragma link "dxPScxPageControlProducer"
#pragma link "dxPSEdgePatterns"
#pragma link "dxPSEngn"
#pragma link "dxPSFillPatterns"
#pragma link "dxPSGlbl"
#pragma link "dxPSPDFExport"
#pragma link "dxPSPDFExportCore"
#pragma link "dxPSPrVwAdv"
#pragma link "dxPSPrVwRibbon"
#pragma link "dxPSPrVwStd"
#pragma link "dxPSUtl"
#pragma link "dxWrap"
#pragma link "cxLabel"
#pragma resource "*.dfm"

TFormMain *FormMain;

//extern void ffmpeg_start_rec(char *szRtspUrl, char *szFile, void *hWndPlayer);
ThreadPlayAndRec      *g_pThread = NULL;
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TSubForm(Owner)
{
    GosInitLog();

}

UINT32 m_ulThreadRTPPlayStatus = SDL_PLAY_START;
ThreadRTPPlay   *g_ThreadRTPPlay = NULL;
TFramePlayAndRec *g_Frame = NULL;
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnStartClick(TObject *Sender)
{
    AnsiString  strUrl = edtURL->Text;
    AnsiString  strFile = edtFile->Text;
    BOOL        bTcpMode = FALSE;

#if 0
    g_ThreadRTPPlay = new ThreadRTPPlay(bTcpMode, PanelVideo->Handle, &m_ulThreadRTPPlayStatus);

    g_ThreadRTPPlay->SetURL(strUrl.c_str());

    g_ThreadRTPPlay->Start();

#else
   /*   g_pThread = new ThreadFfmpegPlayAndRec(bTcpMode, PanelVideo->Handle);

    g_pThread->SetURL(strUrl.c_str(), szFile);

    g_pThread->Start();  */

    if (!g_Frame)
    {
        g_Frame = NewFramePlayAndRec(PanelVideo);
    }

    g_Frame->Start(strUrl.c_str(), strFile.c_str(), bTcpMode);
#endif
    //ffmpeg_start_rec(szRtspUrl, szFile, PanelVideo->Handle);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnStopClick(TObject *Sender)
{
    g_Frame->Stop();
    /*
    if (g_pThread)
    {
        g_pThread->Stop();
       //   delete g_pThread;
        g_pThread = NULL;
    }    */
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::cxButton1Click(TObject *Sender)
{
    FfmpegRecPCM    *Rec = new FfmpegRecPCM();
    char            *szOutFile = "e:\\pcm.wav";
    char            *szPcmFile = "e:\\test.pcm";
    FILE            *fp = fopen(szPcmFile, "rb");
    static UINT8    aucData[1024*1024];

    if (!fp)
    {
        return;
    }

    int iTotalLen = fread(aucData, 1, sizeof(aucData), fp);
    fclose(fp);

    Rec->Start(szOutFile);

    int iFrameLen = 2;

    for (int i=0; i<iTotalLen/iFrameLen; i++)
    {
        Rec->RecPcmData(aucData+iFrameLen*i, iFrameLen);
    }

    Rec->Stop();

    delete Rec;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::cxButton2Click(TObject *Sender)
{
    g_Frame->GrabPic();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnSDLPlayClick(TObject *Sender)
{
    ThreadPcmPlay   *pThreadPcmPlay = new ThreadPcmPlay();

    char            *szDev = (CHAR*)"ÑïÉùÆ÷ (Cirrus Logic Superior High Definition Audio)";
    char            *szPcmFile = "d:\\temp\\echo4.pcm";
    FILE            *fp = fopen(szPcmFile, "rb");
    static UINT8    aucData[1024*1024];

    szDev = (CHAR*)"¶ú»ú (Cirrus Logic Superior High Definition Audio)";

    char    *szTmp = gos_ascii_to_utf8(szDev, NULL);
    int     iLen = strlen(szTmp);
    char    *szDevice = (char*)SDLGetAudioDeviceName(FALSE, 0);

    if (strcmp(szDevice, szTmp) != 0)
    {
        szDevice = (char*)SDLGetAudioDeviceName(FALSE, 1);

        if (strcmp(szDevice, szTmp) != 0)
        {
            return;
        }
    }

    if (!fp)
    {
        return;
    }

    int iTotalLen = fread(aucData, 1, sizeof(aucData), fp);
    fclose(fp);

    int iFrameNum = 16000/40;
    int iFrameLen = 2*iFrameNum;

    pThreadPcmPlay->Start();
    pThreadPcmPlay->Play(szDev, 1, 16000, 2);
    for (int i=0; i<iTotalLen/iFrameLen; i++)
    {
        pThreadPcmPlay->PushData(aucData+iFrameLen*i, iFrameLen);
        gos_sleep_1ms(1000/40);
    }

    delete pThreadPcmPlay;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::cxButton3Click(TObject *Sender)
{
    int is_capture = 0;
    int iNum = SDLGetAudioDeviceNum(is_capture);
    char *szDevice;

    for (int i=0; i<iNum; i++)
    {
        szDevice = (CHAR*)SDLGetAudioDeviceName(is_capture, i);

       gos_save_string_to_file("d:\\sdl.log", szDevice);
    }
}
//---------------------------------------------------------------------------

