//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"
#include "MsgDialog.h"
#include "ClientUtil.h"
#include "ATS.h"
#include "ds_public.h"
#include "pis.h"
#include "msg.h"
#include "PcmPlay.h"

#include "main.h"
#include "WavRec.h"
#include "MicCapture.h"
#include "AudioVolume.h"
#include "ThreadPcmPlay.h"

#include "speex/speex_echo.h"
#include "speex/speex_preprocess.h"

#include "AEC.h"
#include "ThreadPlayAECSound.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxGraphics"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "dxSkinLilian"
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
#pragma link "cxCheckBox"
#pragma link "cxCalendar"
#pragma link "dxStatusBar"
#pragma link "cxSpinEdit"
#pragma resource "*.dfm"

TFormMain *FormMain;


bool    g_bPlaying = false;

UINT32  m_ulSampleFreq = 16000;
UINT32  m_ulPCMFrameLen = 2;
UINT32  m_ulSendFrameNumPer100MillSecond = 100*m_ulSampleFreq/1000;
UINT8   *m_pucPCMData = NULL;
UINT8   *m_pucPCMData2 = NULL;
UINT32  m_ulCurrFrameIndex = 0;
UINT32  m_ulPCMFrameNum = 0;
UINT32  m_ulWavFileHdrLen = 78;
BOOL    g_bRecMode = FALSE;

ThreadPlayAECSound  *g_ThreadPlayAECSound = NULL;

WavRec      *g_pWavRec = NULL;
MicCapture  *g_MicCapture = NULL;

BOOL        g_bDevBusy = FALSE;
int         g_iMicVolume = 0;
int         g_iMicBoostLevel = 0;

ThreadPcmPlay   *g_ThreadPcmPlay = NULL;

#define NN      128           //10-20ms
#define TAIL    (NN*8)               // 100-500ms

SpeexEchoState          *g_SpeexEchoState = NULL;
SpeexPreprocessState    *g_SpeexPreprocessState = NULL;

INT64       g_i64StartTime = 0;

//extern BOOL GetAudioInputDevice(std::vector<AnsiString> &vDevList);
extern BOOL GetAudioDev(std::vector<AnsiString> &vDev);
extern BOOL SetAudioVolume(AnsiString strDev, UINT32 ulVolume);
extern BOOL GetAudioVolume(AnsiString strDev, UINT32 &ulVolume);

extern BOOL GetMicVolume(CHAR *szMicName, unsigned int *pulVolume);
extern void MicphoneBoost(bool bIsBoost);

UINT8       *g_pucPCMData = NULL;
UINT32      g_ulPCMNum = 0;
UINT32      g_ulPCMIndex = 0;

void LoadPCMFile(CHAR *szFile)
{
    GOS_FREE(g_pucPCMData);
    g_ulPCMNum = 0;
    g_ulPCMIndex = 0;

    FILE    *fp = fopen(szFile, "rb");

    if (!fp)
    {
        return;
    }

    fseek(fp, 0, SEEK_END);

    UINT32  ulLen = ftell(fp);

    g_pucPCMData = (UINT8*)calloc(1, ulLen);

    fseek(fp, 0, SEEK_SET);

    ulLen = fread(g_pucPCMData, 1, ulLen, fp);
    fclose(fp);

    if (!g_pucPCMData)
    {
        return;
    }

    g_ulPCMNum = ulLen/m_ulPCMFrameLen;
    g_ulPCMIndex = 0;
}

static INT32 GetVolume(Variant vValue)
{
    if (vValue.IsNull())
    {
        return 0;
    }

    int iValue = vValue;

    if (iValue < -20)
    {
        return -20;
    }

    if (iValue > 20)
    {
        return 20;
    }

    return iValue;
}

void AdjustVolume(UINT8 *pucData, UINT32 ulSize, UINT32 ulBitsPerSample, INT32 iRate)
{
    if (iRate == 0)
    {
        return;
    }

    if (ulSize == 0)
    {
        return;
    }

    double      dRate = pow((double)10, ((double)iRate)/10);

    if (ulBitsPerSample == 16)
    {
        if (MOD(ulSize, 2) == 1)
        {
            pucData[ulSize-1] = 0;
        }

        INT16   *psData = (INT16*)pucData;
        INT32   iData;

        for (UINT32 i=0; i<ulSize/2; i++)
        {
            iData = psData[i]*dRate;
            if (iData > 32767)
            {
                iData = 32767;
            }
            else if (iData < -32768)
            {
                iData = -32768;
            }

            psData[i] = (INT16)iData;
        }
    }
    else if (ulBitsPerSample == 8)
    {
        INT8    *pcData = (INT8*)pucData;
        INT32   iData;

        for (UINT32 i=0; i<ulSize; i++)
        {
            iData = pcData[i]*dRate;
            if (iData > 127)
            {
                iData = 127;
            }
            else if (iData < -128)
            {
                iData = -128;
            }

            pcData[i] = (INT8)iData;
        }
    }
}

class PCMDataBuffer
{
public:
    PCMDataBuffer(UINT32 ulMaxBufSize);
    VOID Init();
    VOID PushData(UINT8 *pucData, UINT32 ulLen);
    VOID GetData(UINT8 *pucData, UINT32 ulMaxLen, UINT32 &ulLen);

private:
    GMutex  m_Mutex;

    INT64   m_i64StartTime;     // 第一包数据
    INT64   m_i64LastSendTime;  // 上一次发送时间

    UINT8   *m_pucDataBuf;
    UINT32  m_ulCurrDataLen;
    UINT32  m_ulMaxBufSize;
};

PCMDataBuffer::PCMDataBuffer(UINT32 ulMaxBufSize)
{
    m_pucDataBuf = NULL;
    m_ulCurrDataLen = 0;
    m_ulMaxBufSize = ulMaxBufSize;

    if (m_ulMaxBufSize < 8000)
    {
        m_ulMaxBufSize = 8000;
    }

    m_pucDataBuf = (UINT8*)calloc(1, m_ulMaxBufSize);
}

VOID PCMDataBuffer::Init()
{
    m_ulCurrDataLen = 0;
    m_i64StartTime = 0;
    m_i64LastSendTime = 0;
}

VOID PCMDataBuffer::PushData(UINT8 *pucData, UINT32 ulLen)
{
    AUTO_LOCK(m_Mutex);

    UINT32          ulBufLen = m_ulMaxBufSize - m_ulCurrDataLen;    // 剩余缓冲区大小

    if (ulLen > ulBufLen)
    {
        GosLog(LOG_ERROR, "data buf is overflow, %u bytes are discarded", ulLen-ulBufLen);
        ulLen = ulBufLen;
    }

    memcpy(m_pucDataBuf+m_ulCurrDataLen, pucData, ulLen);
    m_ulCurrDataLen += ulLen;

    GosLog(LOG_DETAIL, "push data %u, total is %u", ulLen, m_ulCurrDataLen);
}

VOID PCMDataBuffer::GetData(UINT8 *pucData, UINT32 ulMaxLen, UINT32 &ulLen)
{
    AUTO_LOCK(m_Mutex);

    if (m_ulCurrDataLen > ulMaxLen)
    {
        ulLen = ulMaxLen;
    }
    else
    {
        ulLen = m_ulCurrDataLen;
    }

    memcpy(pucData, m_pucDataBuf, ulLen);
    m_ulCurrDataLen -= ulLen;

    if (m_ulCurrDataLen > 0)
    {
        memmove(m_pucDataBuf, m_pucDataBuf+ulLen, m_ulCurrDataLen);
    }

    GosLog(LOG_DETAIL, "pop data %u, current is %u", ulLen, m_ulCurrDataLen);
}


PCMDataBuffer       g_PCMDataBuf(8000);

static UINT32   g_ulFirstSendPCMTime = 0;
static UINT32   g_ulSendPCMNum = 0;

VOID OnAECCapData(unsigned char* data, unsigned int size, unsigned int samplerate, unsigned int bitsPerSample)
{
/*    if (!g_ThreadPcmPlay->IsPlaying())
    {
        g_ThreadPcmPlay->Play(1, m_ulSampleFreq, m_ulPCMFrameLen);
    }   */

    g_PCMDataBuf.PushData(data, size);

    if (!g_bPlaying)
    {
        return;
    }

    if (!FormMain)
    {
        return;
    }

    int iVolumeRate = GetVolume(FormMain->edtVolumeRate->Value);

    AdjustVolume(data, size, bitsPerSample, iVolumeRate);

    g_pWavRec->Rec(TRUE, data, size);

  //  FormMain->btnStopClick(NULL);
}

void StartRec()
{
    CHAR        acFile[256];

    sprintf(acFile, "%s/%s", gos_get_root_path(), "echo_test.wav");

    if (!g_pWavRec)
    {
        //g_pWavRec = new WavRec(acFile, m_ulSampleFreq, m_ulPCMFrameLen, TRUE);
        g_pWavRec = new WavRec();
        g_pWavRec->Init(acFile, m_ulSampleFreq, m_ulPCMFrameLen, FALSE);
    }
}

void StopRec()
{
    AECStop();

    if (g_pWavRec)
    {
        g_pWavRec->StopRec();
        delete g_pWavRec;
        g_pWavRec = NULL;
    }
}

VOID OnMicData(UINT8 *pucData, UINT32 ulSize)
{

/*    if (!g_ThreadPcmPlay->IsPlaying())
    {
        g_ThreadPcmPlay->Play(NULL, 1, m_ulSampleFreq, m_ulPCMFrameLen);
    }      */

    GosLog(LOG_INFO, "size = %u", ulSize);

    return;


    // 每秒数据为16000*2*1 = 32K

  //  UINT32      ulFrameNumPerTime = (m_ulSampleFreq/1000)*TimePlay->Interval;
    UINT8       *pucPlayData = g_pucPCMData+g_ulPCMIndex*m_ulPCMFrameLen;
    INT64       i64Time = (gos_get_uptime_1ns() - g_i64StartTime)/1000000;
    UINT32      ulFrameIndex = i64Time*m_ulSampleFreq;
    UINT32      ulFrameNum;
  //      g_ulFrameNum - g_ulPCMIndex;

#if 0
    if (ulFrameIndex > g_ulFrameNum)
    {
        ulFrameNum = g_ulFrameNum - g_ulPCMIndex;
        g_ulPCMIndex  = 0;
    }
    else
    {
        ulFrameNum = ulFrameIndex - g_ulPCMIndex;
        g_ulPCMIndex = ulFrameIndex;
    }
#endif

    UINT32      ulTotalFrameNum = ulSize/m_ulPCMFrameLen;

    g_ThreadPcmPlay->PushData(pucPlayData, ulTotalFrameNum*m_ulPCMFrameLen);

    for (UINT32 i=0; i<ulTotalFrameNum/NN; i++)
    {
        ulFrameNum = NN;
        if ((g_ulPCMIndex+ulFrameNum) >= g_ulPCMNum)
        {
            ulFrameNum = g_ulPCMNum - g_ulPCMIndex;
            g_ulPCMIndex = 0;
        }
        else
        {
            g_ulPCMIndex += ulFrameNum;
        }

        static short    asOutBuf[NN];
        const short     *psRef = (const short*)pucData;
        const short     *psEcho = (const short*)pucPlayData;
        UINT32          ulFrameLen = ulFrameNum*m_ulPCMFrameLen;

        if (g_pWavRec)
        {
           // g_pWavRec->Rec(FALSE, (UINT8*)psEcho, ulFrameLen);
        }

        speex_echo_cancellation(g_SpeexEchoState, psRef, psEcho, asOutBuf);
        speex_preprocess_run(g_SpeexPreprocessState, asOutBuf);

        if (g_pWavRec)
        {
            //memset(asOutBuf, 0, ulFrameLen);
            for (UINT32 j=0;j<ARRAY_SIZE(asOutBuf); j++)
            {
                asOutBuf[j] /= 100;
            }
            g_pWavRec->Rec(TRUE, (UINT8*)asOutBuf, ulFrameLen);
            g_pWavRec->Rec(FALSE, (UINT8*)pucData, ulFrameLen);
        }

        pucData += ulFrameLen;
        pucPlayData += ulFrameLen;
    }
}

VOID CloseMic()
{
    if (g_MicCapture)
    {
        delete g_MicCapture;
        g_MicCapture = NULL;
    }
}

BOOL OpenMic()
{
    CloseMic();

    g_MicCapture = new MicCapture(OnMicData);
    if (!g_MicCapture->Open(m_ulSampleFreq, m_ulPCMFrameLen*8, 1))
    {
        CloseMic();
        return FALSE;
    }

    return TRUE;
}

VOID StartGrabAudio()
{
    if (g_MicCapture)
    {
        g_MicCapture->Start();
    }
}

VOID StopGrabAudio()
{
    if (g_MicCapture)
    {
        g_MicCapture->Stop();
    }

    CloseMic();
}

//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TSubForm(Owner)
{
    MediaPlayer->Notify = true;

    GosInitLog();

    GLogFactory::GetInstance()->GetDefaultLogger()->SetLogLevel(LOG_DETAIL);

    gos_time_init();

    AECInit();

    InitDev();


   

  //  char *szCapture = "Internal Microphone (Cirrus Logic Superior High Definition Audio)";
  //  char *szRender = "扬声器 (Cirrus Logic Superior High Definition Audio)";

}

void __fastcall TFormMain::InitDev()
{
//    std::vector<std::string>     vSpeaker;
//    std::vector<std::string>     vMic;

    //    GetSpeakerList(vSpeaker);
//    GetMicList(vMic);

    AEC_DEV_T   stMic;
    AEC_DEV_T   stSpeaker;

    AECEnumDevice(&stMic, &stSpeaker);

    cbSpeaker->Properties->Items->Clear();
    for (UINT32 i=0; i<stSpeaker.ulNum; i++)
    {
        cbSpeaker->Properties->Items->Add(stSpeaker.aacName[i]);
    }

    if (stSpeaker.ulNum == 1)
    {
        cbSpeaker->ItemIndex = 0;
    }

    cbMic->Properties->Items->Clear();
    for (UINT32 i=0; i<stMic.ulNum; i++)
    {
        cbMic->Properties->Items->Add(stMic.aacName[i]);
    }

    if (stMic.ulNum == 1)
    {
        cbMic->ItemIndex = 0;
    }
}

static INT16 Merge(INT16 sLeft, INT16 sRight)
{
#if 0
    INT32   iValue = ((INT32)sLeft) + sRight;

    iVvalue /= 2;
    if (iValue > 0x7fff)
    {
        iValue = 0x7fff;
    }
    else if (iValue < -0x8000)
    {
        iValue = -0x8000;
    }

    return (INT16)iValue;

#else
    INT32   iValue = ((INT32)sLeft) + sRight;
    INT32   iValue2 = ((INT32)sLeft)*sRight;
    INT32   iValue3 = 32768;

    if (sLeft < 0 && sRight < 0)
    {
        iValue = iValue + iValue2/iValue3;
    }
    else
    {
        iValue = iValue - iValue2/iValue3;
    }

    if (iValue > 0x7fff)
    {
        iValue = 0x7fff;
    }
    else if (iValue < -0x8000)
    {
        iValue = -0x8000;
    }

    return (INT16)iValue;
#endif
}

BOOL MergePCMFile(CHAR *szFile1, CHAR *szFile2, CHAR *szFile)
{
    FILE    *fp1 = fopen(szFile1, "rb");
    FILE    *fp2 = fopen(szFile2, "rb");
    FILE    *fp = fopen(szFile, "w+b");
    UINT32  ulLen1 = 0;
    UINT32  ulLen2 = 0;
    UINT32  ulLen = 0;
    UINT8   *pucData1 = NULL;
    UINT8   *pucData2;
    UINT32  i;
    UINT32  ulSampleRate;
    UINT32  ulSampleBytes;
    UINT32  ulChunkSize;
    INT16   *psLeft;
    INT16   *psRight;
    INT16   sMerge;
    UINT32  ulChanNum;
    UINT32  ulDataLen = 0;
    BOOL    bRet = FALSE;

    if (!fp1 || !fp2 || !fp)
    {
        goto end;
    }

    fseek(fp1, 0, SEEK_END);
    ulLen = ftell(fp1);
    fseek(fp1, 0, SEEK_SET);

    fseek(fp2, 0, SEEK_END);
    ulLen = ftell(fp2);
    fseek(fp2, 0, SEEK_SET);

    ulLen = ulLen1<ulLen2?ulLen1:ulLen2;
    pucData1 = (UINT8*)calloc(1, 2*ulLen);
    if (!pucData1)
    {
        goto end;
    }

    pucData2 = pucData1+ulLen;
    fread(pucData1, 1, ulLen1, fp1);
    fread(pucData2, 1, ulLen2, fp2);

    ulChanNum = 1;
    ulSampleBytes = 2;

    for (UINT32 i=0; i<ulLen/ulSampleBytes; i++)
    {
        psLeft = (INT16*)(pucData1+i*ulSampleBytes);
        psRight = (INT16*)(pucData2+i*ulSampleBytes);
        sMerge = Merge(psLeft[0], psRight[0]);
        fwrite(&sMerge, 1, ulSampleBytes, fp);
    }

    bRet = TRUE;

end:
    if (fp1)
    {
        fclose(fp1);
    }

    if (fp2)
    {
        fclose(fp2);
    }

    if (fp)
    {
        fclose(fp);
    }

    GOS_FREE(pucData1);

    return bRet;
}

void CreateEmptyWavFile(CHAR *szFile)
{
    WavRec      Rec;
    UINT32      ulFrameNum = 16000*10*60;
    UINT32      ulLen = 2*ulFrameNum;
    UINT32      ulWavHdrLen = 44;
    INT16       *psData;

    if (gos_get_file_size(szFile) == (ulWavHdrLen+ulLen))
    {
        return;
    }

    psData = (INT16*)calloc(1, ulLen);

    Rec.Init(szFile, 16000, 2, TRUE);
    for (UINT32 i=0; i<ulFrameNum; i++)
    {
        if (MOD(i, 20) < 10)
        {
            psData[i] = 1;
        }
        else
        {
            psData[i] = -1;
        }
    }

    Rec.Rec(TRUE, (UINT8*)psData, ulLen);
    Rec.StopRec();

    free(psData);
}

//--------------------------------------------------------
void __fastcall TFormMain::btnStartClick(TObject *Sender)
{
    AnsiString  strCapture = cbMic->Text;
    AnsiString  strRender = cbSpeaker->Text;
    UINT32      ulSampleRate = 16000;
    UINT32      ulBitsPerSample = 16;

    GosLog(LOG_WARN, "start capt");

    if (!g_ThreadPlayAECSound)
    {
        g_ThreadPlayAECSound = new ThreadPlayAECSound(this->Handle);
    }

#if 0
    if (!g_ThreadPcmPlay)
    {
        g_ThreadPcmPlay = ThreadPcmPlay::GetInstance();
    }

    g_ThreadPcmPlay->Play(1, m_ulSampleFreq, m_ulPCMFrameLen);
 #endif

  //MergePCMFile("d:/temp/rec6.pcm", "d:/temp/music.pcm", "d:/temp/all.pcm");

    g_i64StartTime = gos_get_uptime_1ns();

    TimerPlay->Enabled = true;

    TimerSend->Interval = 200;
//    TimerSend->Enabled = true;
    g_ulSendPCMNum = 0;

 #if 0
 //   char    *szFile = "d:\\temp\\echo4.pcm"; //rec_brd.pcm";

 //   LoadPCMFile(szFile);
    CHAR    acFile[256];

    sprintf(acFile, "%s\\%s", gos_get_root_path(), "aec.wav");

 //   PlaySound(acFile, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

    CreateEmptyWavFile(acFile);

    try
    {
        if (FileExists(acFile))
        {
            MediaPlayer->FileName = acFile;
            MediaPlayer->Open();
            MediaPlayer->Notify = true;
            MediaPlayer->Play();
        }
    }
    catch(...)
    {
        return;
    }
#endif

 //   OpenMic();
 //   StartGrabAudio();

    StartRec();

    AECSetOnCapturedDataCB(OnAECCapData);

 //   AECStart(strCapture.c_str(), strRender.c_str(), ulSampleRate, ulBitsPerSample);
    AECStart("", "", ulSampleRate, ulBitsPerSample);

    g_bPlaying = true;

    return;

    btnStart->Enabled = false;
    btnStop->Enabled = true;
    btnPlay->Enabled = false;

    /*
    g_SpeexEchoState = speex_echo_state_init(NN, TAIL);
    g_SpeexPreprocessState = speex_preprocess_state_init(NN, m_ulSampleFreq);
    speex_echo_ctl(g_SpeexEchoState, SPEEX_ECHO_SET_SAMPLING_RATE, &m_ulSampleFreq);
    speex_preprocess_ctl(g_SpeexPreprocessState, SPEEX_PREPROCESS_SET_ECHO_STATE, g_SpeexEchoState);
    */
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnStopClick(TObject *Sender)
{
    if (g_ThreadPcmPlay)
    {
        g_ThreadPcmPlay->Stop();
    }

    g_bPlaying = false;

    if (MediaPlayer->Mode == mpPlaying)
    {
        MediaPlayer->Stop();
    }

    TimerPlay->Enabled = false;

    StopGrabAudio();

    StopRec();

    return;

    btnStart->Enabled = true;
    btnStop->Enabled = false;
    btnPlay->Enabled = true;

    /*
    if (!g_SpeexEchoState)
    {
        speex_echo_state_destroy(g_SpeexEchoState);
        speex_preprocess_state_destroy(g_SpeexPreprocessState);

        g_SpeexEchoState = NULL;
        g_SpeexPreprocessState = NULL;
    }         */
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    btnStopClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerPlayTimer(TObject *Sender)
{
    /////////////////////
    return;

    if (!g_ThreadPcmPlay)
    {
        return;
    }

    static INT16    *psPlayData = NULL;

    if (!psPlayData)
    {
        psPlayData = (INT16*)calloc(sizeof(INT16), m_ulSampleFreq);

        for (UINT32 i=0; i<m_ulSampleFreq; i++)
        {
            psPlayData[i] = gos_get_range_rand(-10,10);
        }
    }

    UINT32      ulFrameNum = (m_ulSampleFreq/1000)*TimerPlay->Interval;

    g_ThreadPcmPlay->PushData(psPlayData, ulFrameNum*m_ulPCMFrameLen);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnPlayClick(TObject *Sender)
{
    CHAR    acFile[256];

    sprintf(acFile, "%s\\%s", gos_get_root_path(), "echo_test.wav");

    PlaySound(acFile, NULL, SND_FILENAME | SND_ASYNC);

    /*
    try
    {
        if (FileExists(acFile))
        {
            MediaPlayer->FileName = acFile;
            MediaPlayer->Open();
            MediaPlayer->Play();
        }
    }
    catch(...)
    {
        return;
    }   */
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormShow(TObject *Sender)
{
    btnStart->SetFocus();
}
//---------------------------------------------------------------------------




void __fastcall TFormMain::MediaPlayerNotify(TObject *Sender)
{
    if (MediaPlayer->Position == MediaPlayer->Length)
    {
        MediaPlayer->Rewind();
        MediaPlayer->Play();
     /*
        static INT64   iTime = gos_get_uptime_1ns()/1000;

        INT64   iCurrTime = gos_get_uptime_1ns()/1000;
        GosLog(LOG_INFO, "play %d "FORMAT_I64, MediaPlayer->Length, iCurrTime-iTime);

        iTime = iCurrTime;   */
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerSendTimer(TObject *Sender)
{
    return;

    TimerSend->Interval = 1;

    UINT32  ulCurrTime = gos_get_uptime_1ms();

    UINT32  ulMaxSendDataLen = 1000;//GetPAMaxPkgSize();
   // UINT32  ulSendNum = 16000*2/ulMaxSendDataLen;
   // double  dPeriod = ((double)1000)/ulSendNum;
    UINT32  ulSendBytesPerSec = 16000*2;//32000
    double  dPeriod = ((double)1000*ulMaxSendDataLen)/ulSendBytesPerSec;   // 毫秒  , 1000/32 = 31.25
    UINT32  ulLen;

    if (g_ulSendPCMNum > 0 &&
        (ulCurrTime - g_ulFirstSendPCMTime) < g_ulSendPCMNum*dPeriod)
    {
        return;
    }

    if (0 == g_ulSendPCMNum)
    {
        g_ulFirstSendPCMTime = ulCurrTime;
    }

    GosLog(LOG_WARN, "%u %.2f", ulCurrTime - g_ulFirstSendPCMTime,
            (ulCurrTime - g_ulFirstSendPCMTime)/dPeriod);

    g_ulSendPCMNum++;

    UINT8       aucData[1000];

    g_PCMDataBuf.GetData(aucData, ulMaxSendDataLen, ulLen);
}
//---------------------------------------------------------------------------

