//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "AudioDev.h"
#include "ClientData.h"
#include "ThreadPlayAndRec.h"
#include "PIS.h"
#include "WavRec.h"
#include "ThreadIPH.h"
#include "MicCapture.h"
#include "FrmIPH.h"
#include "main.h"
#include "ffmpeg_rec.h"
#include "FrmCallIn.h"
#include "LocalCfg.h"
#include "LocalRec.h"
#include "AEC.h"
#include "ThreadPlayAECSound.h"
#include "AudioVolume.h"
#include "ThreadPcmPlay.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxCustomData"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxGroupBox"
#pragma link "cxInplaceContainer"
#pragma link "cxLabel"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxStyles"
#pragma link "cxTextEdit"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "cxTrackBar"
#pragma link "dxSkinBlue"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "MDIForm"
#pragma link "dxBar"
#pragma link "cxClasses"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxPC"
#pragma link "dxBarBuiltInMenu"
#pragma link "cxImageComboBox"
#pragma link "cxCheckBox"
#pragma link "cxImageComboBox"
#pragma link "cxSplitter"
#pragma resource "*.dfm"
TFormIPH *FormIPH;

//static UINT32  g_ulMaxIPHTalkTime = 300;
static GMutex   m_IPHStatuMutex;
static UINT32   m_ulIPHStatus = IPH_STATUS_OVER;

static UINT32   m_ulTrainUnitID = INVALID_TRAIN_UNIT_ID;
static UINT32   m_ulCarriageID = 0xff;
static UINT32   m_ulDoorID     = 0xff;
static UINT32   m_ulIPHDevID = 0xff;
static UINT8    m_aucIPHDevIP[4] = {0,0,0,0};
static UINT16   m_usIPHDevPort = IPH_RECV_DC_PORT;
static UINT16   g_usDCRecvPCMUDPPort = DC_RECV_IPH_PORT;            // DC接收PCM
static UINT32   m_ulPCMPkgIndex = 0;
static AnsiString m_strVideoUrl = "";

static UINT16   m_usPCMCaptureSampleRate = 16000;
static UINT8    m_ucPCMCaptureBitsPerSample = 16;
static UINT8    m_ucPCMCaptureChanNum = 1;

static int      m_iVideoWidth = -1;
static int      m_iVideoHeight = -1;

static UINT32   g_ulCheckIPHPkgTime = 60;   // 连续60秒未收到IPH音频自动挂断

UINT32          g_ulLastIPHPkgTime = 0;     // 最后收到的IPH语音包时间

static ThreadPlayAECSound   *g_ThreadPlayAECSound = NULL;
static ThreadPlayAndRec     *g_ThreadPlayAndRec = NULL;
static ThreadIPH            *g_ThreadIPH = NULL;
//static ThreadPcmPlay        *g_ThreadPcmPlay = NULL;
static SOCKET               g_SockIPHPcmData = NULL;

static MAP<AnsiString, UINT32>  g_mAcceptIPHInfo;

static IP_ADDR_T    g_sIPHDevIPPort;

static bool         m_bIPHAECMode = true;
static UINT32       g_ulIPHAckCheckCount = 0;

static TRAIN_INFO_T  *g_pstCurrTalkTrainInfo = NULL;

static TcxTreeListNode  *g_CurrIPHNode = NULL;
//static IPH_TALK_APPLY_REQ_INFO_T   g_stIPHtalkInfo = {0};

VOID OnMicData(UINT8 *pucBuf, UINT32 ulBufSize);

static VECTOR<IPH_INFO_T>               g_vIPHInfo;

WavRec      g_IPHWavRec;

MicCapture  *g_MicCapture = NULL;

static VOID GetIPHAudioOption()
{
    m_usPCMCaptureSampleRate = GetIPHAudioSampleRate();
    m_ucPCMCaptureBitsPerSample = GetIPHAudioBitsPerSample();

    return ;
}

AnsiString GetIPHKey(UINT32 ulTrainUnitID, UINT32 ulCarriageID, UINT32 ulIPHDevID)
{
    AnsiString      strKey = "";

    strKey.printf("%u-%u-%u", ulTrainUnitID, ulCarriageID, ulIPHDevID);

    return strKey;
}

static VOID CloseMic()
{
    if (g_MicCapture)
    {
        delete g_MicCapture;
        g_MicCapture = NULL;
    }
}

static BOOL OpenMic()
{
    if (m_bIPHAECMode)
    {
        return TRUE;
    }

    CloseMic();

    UINT32 ulIPHMaxPkgSize = GetIPHMaxPkgSize();  // 1000

    g_MicCapture = new MicCapture(OnMicData);
    if (!g_MicCapture->Open(m_usPCMCaptureSampleRate, m_ucPCMCaptureBitsPerSample,
                            m_ucPCMCaptureChanNum, ulIPHMaxPkgSize))
    {
        CloseMic();
        return FALSE;
    }

    return TRUE;
}

static VOID StartGrabAudio()
{
    if (m_bIPHAECMode)
    {
        AECStart("", "", m_usPCMCaptureSampleRate, m_ucPCMCaptureBitsPerSample);
        return;
    }

    if (!g_MicCapture)
    {
        GosLog(LOG_INFO, "start grab mic data failed, can't open mic");
        return;
    }

    g_MicCapture->Start();

    GosLog(LOG_INFO, "start grab mic data");
}

static VOID StopGrabAudio()
{
    if (m_bIPHAECMode)
    {
        AECStop();
        GosLog(LOG_INFO, "stop AEC mic grab");

        return;
    }

    CloseMic();

    GosLog(LOG_INFO, "stop grab mic data");
}

static void GetIPHVideoRecFileName(UINT32 ulTrainUnitID, UINT32 ulCarriageID, UINT32 ulIPHDevID, CHAR *szFile)
{
    CHAR    acTime[32];
    CHAR    acDir[256];

    gos_get_text_time_ex(NULL, "%u%02u%02u_%02u%02u%02u", acTime);

    LocalRec::GetIPHRecTmpDir(acDir);
    sprintf(szFile, "%s\\%u", acDir, APP::GetCurrDCUserID());
    if (!gos_file_exist(szFile))
    {
        gos_create_redir(szFile);
    }

    // 第一级为调度台id、第二为文件， 1001_101041_3_5_time.mp4  (DC_车体号_车厢_设备_时间）
    sprintf(szFile+strlen(szFile), "\\%u_%u_%u_%u_%s.mp4", APP::GetCurrDCUserID(),
            ulTrainUnitID, ulCarriageID, ulIPHDevID, acTime);
}

static void GetIPHAudioRecFileName(UINT32 ulTrainUnitID, UINT32 ulCarriageID, UINT32 ulIPHDevID, CHAR *szFile)
{
    CHAR    acTime[32];
    CHAR    acDir[256];

    gos_get_text_time_ex(NULL, "%u%02u%02u_%02u%02u%02u", acTime);

    LocalRec::GetIPHRecTmpDir(acDir);

    sprintf(szFile, "%s\\%u", acDir, APP::GetCurrDCUserID());
    if (!gos_file_exist(szFile))
    {
        gos_create_redir(szFile);
    }

    // 第一级为调度台id、第二为文件， 1001_101041_3_5_time.wav  (DC_车体号_车厢_设备_时间）
    sprintf(szFile+strlen(szFile), "\\%u_%u_%u_%u_%s.wav", APP::GetCurrDCUserID(),
            ulTrainUnitID, ulCarriageID, ulIPHDevID, acTime);
}

VOID StartIPHTalk()
{
    g_ulLastIPHPkgTime = gos_get_uptime();
    GosLog(LOG_INFO, "start capture MIC");

    StartGrabAudio();

    CHAR    acRecFile[256];
    UINT32  ulPCMFrameLen = m_ucPCMCaptureBitsPerSample/8;

    GetIPHAudioRecFileName(m_ulTrainUnitID, m_ulCarriageID, m_ulIPHDevID, acRecFile);

   // g_RecThread->StartRec(acRecFile, m_usPCMCaptureSampleRate, ulPCMFrameLen);
    g_IPHWavRec.Init(acRecFile, m_usPCMCaptureSampleRate, ulPCMFrameLen, IsIPHRecMergeMode());
}

//---------------------------------------------------------------------------
BOOL IsIPHStatus(UINT32 ulIPHStatus)
{
    m_IPHStatuMutex.Mutex();

    BOOL bSame = (m_ulIPHStatus == ulIPHStatus);

    m_IPHStatuMutex.Unmutex();

    return bSame;
}
//---------------------------------------------------------------------------
VOID SetIPHStatus(UINT32 ulIPHStatus)
{
    m_IPHStatuMutex.Mutex();

    m_ulIPHStatus = ulIPHStatus;

    GosLog(LOG_INFO, "set IPH status to %u", m_ulIPHStatus);

    m_IPHStatuMutex.Unmutex();
}

/*
VOID SetIPHStatus(IPH_STATUS_T *pstIPH)
{
    m_IPHStatuMutex.Mutex();

    m_ulIPHStatus = pstIPH->stIPHInfo.ulStatus;

    GosLog(LOG_INFO, "set IPH status to %u", m_ulIPHStatus);

    m_IPHStatuMutex.Unmutex();

    memcpy(&g_stCurrIPHStatus, pstIPH, sizeof(g_stCurrIPHStatus));
} */

//---------------------------------------------------------------------------
BOOL SendCleanIPHTalkReq(UINT32 ulTrainUnitID)
{
    UINT32      ulSeqID = 0;
    GJsonParam  JsonParam;

    JsonParam.Add("TrainUnitID", ulTrainUnitID);
    JsonParam.Add("CarriageID", IPH_TALK_RESET);
    JsonParam.Add("IPHDevID", IPH_TALK_RESET);

    SendServer(EV_DC_DIS_SA_REQ, MSG_DCStopIPHTalk, JsonParam);

    return TRUE;
}

//---------------------------------------------------------------------------
UINT32 SendStopIPHTalkReq()
{
    UINT32      ulSeqID = 0;
    GJsonParam  JsonParam;
    GJsonParam  JsonSubParam;

    if (IsIPHStatus(IPH_STATUS_OVER) ||
        m_ulTrainUnitID == INVALID_TRAIN_UNIT_ID)
    {
        return IPH_STOP_SUCC;
    }

    JsonParam.Add("TrainUnitID", m_ulTrainUnitID);
    JsonParam.Add("CarriageID", m_ulCarriageID);
    JsonParam.Add("IPHDevID", m_ulIPHDevID);

    SendServer(EV_DC_DIS_SA_REQ, MSG_DCStopIPHTalk, JsonParam);

    return IPH_STOP_SUCC;
}

static VOID SendIPHDataToDIS(UINT8 *pucBuf, UINT32 ulBufSize)
{
    static CHAR     *szPcmData = NULL;
    SOCKADDR_IN     stAddr = {0};
    UINT32          ulPcmDataMAXLen = GetIPHMaxPkgSize();
    UINT32          ulMsgHdrLen = sizeof(PIS_MSG_HDR_T)+STRUCT_OFFSET(IPH_PCM_DATA_T, aucPCMData);
    UINT32          ulPcmPkgSize = ulPcmDataMAXLen + ulMsgHdrLen;
    UINT32          ulPcmDataPkgSize = (2*ulPcmPkgSize)+1;

    GJsonParam          Param;

    if (!szPcmData)
    {
        szPcmData = (CHAR *)calloc(1, ulPcmDataPkgSize);
        if (!szPcmData)
        {
            GosLog(LOG_INFO, "szPcmData calloc error");
            return;
        }
    }

    while(ulBufSize > 0)
    {
        Param.Clear();

        Param.Add("TrainUnitID", m_ulTrainUnitID);

        UINT32  ulCurrPcmDataLen = ulBufSize;

        if (ulCurrPcmDataLen > ulPcmDataMAXLen)
        {
            ulCurrPcmDataLen = ulPcmDataMAXLen;
        }

        ulBufSize -= ulCurrPcmDataLen;

        gos_byte_to_hexstr(pucBuf, ulCurrPcmDataLen, szPcmData);
        pucBuf += ulCurrPcmDataLen;

        GosLog(LOG_INFO, "send IPH PCM to dis, datalen is:%d", strlen(szPcmData));

        Param.Add("IPHDevID", m_ulIPHDevID);
        Param.Add("CarriageID", m_ulCarriageID);
        Param.Add("PCMPkgTime", gos_get_current_time());
        Param.Add("DataIndex", m_ulPCMPkgIndex++);
        Param.Add("SampleRate", m_usPCMCaptureSampleRate);
        Param.Add("ChannelNum", m_ucPCMCaptureChanNum);
        Param.Add("SampleBits", m_ucPCMCaptureBitsPerSample);
        Param.Add("PCMDataLen", ulCurrPcmDataLen);
        Param.Add("PCMData", szPcmData);

        SendServer(EV_DC_DIS_SA_REQ, MSG_DCSendIPHAudioData, m_ulPCMPkgIndex-1, Param);
    }
}

VOID DirectSendIPHData(UINT8 *pucBuf, UINT32 ulBufSize)
{
    SOCKADDR_IN     stAddr = {0};
    PIS_MSG_HDR_T   *pstMsg = NULL;
    UINT32          ulPcmPkgSize = GetIPHMaxPkgSize();
    UINT32          ulMsgHdrLen = sizeof(PIS_MSG_HDR_T)+STRUCT_OFFSET(IPH_PCM_DATA_T, aucPCMData);

    if (!pstMsg)
    {
        pstMsg = (PIS_MSG_HDR_T*)calloc(1, ulPcmPkgSize);
        if (!pstMsg)
        {
            GosLog(LOG_ERROR, "malloc %u bytes PCM buffer failed", ulPcmPkgSize);
            return;
        }
    }

    gos_init_socket_addr(&stAddr, m_aucIPHDevIP, m_usIPHDevPort);

    IPH_PCM_DATA_T  *pstPcmData = (IPH_PCM_DATA_T*)(pstMsg+1);

    while(ulBufSize > 0)
    {
        UINT32  ulCurrPcmDataLen = ulBufSize;

        if (ulCurrPcmDataLen > ulPcmPkgSize)
        {
            ulCurrPcmDataLen = ulPcmPkgSize;
        }

        ulBufSize -= ulCurrPcmDataLen;

        UINT32  ulTotalMsgLen = ulMsgHdrLen + ulCurrPcmDataLen;

        pstMsg->usMsgMagic = HTONS(PIS_MSG_MAGIC);
        pstMsg->usMsgType = HTONS(EV_IPH_AUDIO_DOWNLOAD);
        pstMsg->usMsgLen = STRUCT_OFFSET(IPH_PCM_DATA_T, aucPCMData) + ulCurrPcmDataLen;
        HTONS_IT(pstMsg->usMsgLen);

        pstPcmData->usTrainUnitID = HTONS(m_ulTrainUnitID);
        pstPcmData->ucCarriageID = m_ulCarriageID;
        pstPcmData->ucIPHDevID = m_ulIPHDevID;
        pstPcmData->ulPCMPkgIndex = m_ulPCMPkgIndex++;  // PCM数据包序号，第一个数据包为0，后续递增
        pstPcmData->ulPCMPkgTime = gos_get_current_time();
        pstPcmData->usSampleRate = HTONS(m_usPCMCaptureSampleRate);
        pstPcmData->ucChanNum = m_ucPCMCaptureChanNum;
        pstPcmData->ucSampleBits = m_ucPCMCaptureBitsPerSample;
        pstPcmData->usPCMDataLen = HTONS(ulCurrPcmDataLen);

        HTONL_IT(pstPcmData->ulPCMPkgIndex);
        HTONL_IT(pstPcmData->ulPCMPkgTime);

        memcpy(pstPcmData->aucPCMData, pucBuf, ulCurrPcmDataLen);
        pucBuf += ulCurrPcmDataLen;

        if (!gos_udp_send(g_SockIPHPcmData, pstMsg, ulMsgHdrLen+ulCurrPcmDataLen, &stAddr))
        {
            GosLog(LOG_ERROR, "send IPH data to "IP_FMT "(%u) failed: %s",
                   IP_ARG(m_aucIPHDevIP), m_usIPHDevPort, gos_get_socket_err());
        }
    }
}

static void AutoAdjustVolume(UINT8 *pucData, UINT32 ulSize, UINT32 ulBitsPerSample, INT32 iRate)
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

// 发送IPH数据
VOID SendIPHData(UINT8 *pucBuf, UINT32 ulBufSize)
{
    if (!IsIPHStatus(IPH_STATUS_TALK))
    {
        GosLog(LOG_ERROR, "can't send IPH data, current IPH status is %u", m_ulIPHStatus);
        return;
    }

    INT32   iRate = GetIPHMicVolumeRate();

    AutoAdjustVolume(pucBuf, ulBufSize, m_ucPCMCaptureBitsPerSample, iRate);

    if (DirectSendIPHAudio())
    {
        DirectSendIPHData(pucBuf, ulBufSize);
    }
    else
    {
        SendIPHDataToDIS(pucBuf, ulBufSize);
    }

    g_IPHWavRec.Rec(TRUE, pucBuf, ulBufSize);

    GosLog(LOG_INFO, "Send IPH PCM data %u", ulBufSize);
}


namespace IPHDataBuffer
{

static UINT8    *m_pucIPHDataBuf = NULL;
static UINT32   m_ulCurrIPHDataLen = 0;

VOID Clear()
{
    if (m_pucIPHDataBuf)
    {
        m_ulCurrIPHDataLen = 0;
    }
}

VOID PushData(UINT8 *pucData, UINT32 ulLen)
{
    UINT32          ulMaxBufSize = GetIPHMaxPkgSize();
    INT32           iCurrSize = ulLen;
    UINT32          ulBufLen = 0;

    GosLog(LOG_DETAIL, "IPH PCMDataBuffer push data %u", ulLen);

    if (ulMaxBufSize == 0)
    {
        SendIPHData(pucData, ulLen);
        return;
    }

    if (!m_pucIPHDataBuf)
    {
        m_pucIPHDataBuf = (UINT8*)calloc(2, ulMaxBufSize);    // 在1、2字节采样都满足
    }

    while(1)
    {
        ulBufLen = ulMaxBufSize - m_ulCurrIPHDataLen;
        if (iCurrSize < (INT32)ulBufLen)
        {
            memcpy(m_pucIPHDataBuf+m_ulCurrIPHDataLen, pucData, iCurrSize);
            m_ulCurrIPHDataLen += iCurrSize;

            return;
        }

        memcpy(m_pucIPHDataBuf+m_ulCurrIPHDataLen, pucData, ulBufLen);
        SendIPHData(m_pucIPHDataBuf, ulMaxBufSize);

        m_ulCurrIPHDataLen = 0;
        pucData += ulBufLen;
        iCurrSize -= ulBufLen;
    }
}
} // namespace IPHDataBuffer

// 正常采集方式
VOID OnMicData(UINT8 *pucBuf, UINT32 ulBufSize)
{
    IPHDataBuffer::PushData(pucBuf, ulBufSize);
}

// AEC采集方式
static VOID OnAECCapData(UINT8 *pucData, UINT32 ulLen, UINT32 ulSampleRate, UINT32 ulBitsPerSample)
{
    IPHDataBuffer::PushData(pucData, ulLen);
}

void __fastcall TFormIPH::StopIPHTalk()
{
    g_ThreadIPH->StopPlay();

    GosLog(LOG_INFO, "stop playing IPH audio");

    StopGrabAudio();

    pmiAECMode->Enabled = true;

    g_IPHWavRec.StopRec();

    GosLog(LOG_INFO, "stop recording IPH audio");

    // 恢复缺省音量
    SetMicVolume(GetMainMic(), GetMainMicVolume());

    m_strVideoUrl = "";
    if (g_ThreadPlayAndRec)
    {
        g_ThreadPlayAndRec->Stop();
        GosLog(LOG_INFO, "stop playing IPH video");
    }

    if (FormCallIn)
    {
      //  FormCallIn->StopIPHTalk(m_ulTrainUnitID);
      //  PanelVideo->Visible = false;
    }

/*  m_ulTrainUnitID = 0xffff;
    m_ulCarriageID = 0xff;
    m_ulIPHDevID = 0xff; */
    m_ulPCMPkgIndex = 0;

    IPHDataBuffer::Clear();

    g_ulLastIPHPkgTime = 0;
    g_CurrIPHNode = NULL;

    // 将原来的监控声器设置为操作系统缺省扬声器
    SetDefaultSpeaker(GetMonSpeaker());

    GosLog(LOG_INFO, "set default speaker to mon speaker");

//  memset(m_aucIPHDevIP, 0, sizeof(m_aucIPHDevIP));
}

void __fastcall TFormIPH::InitWorkPanel()
{
    TcxButton   *btns[] = { btnHangoff,
                            btnHangup,
                            btnCleaner,
                          };

    gbWorkPanel->Width = 120;

    for (UINT32 i=0; i<ARRAY_SIZE(btns); i++)
    {
        btns[i]->TabOrder = i;
        btns[i]->Top = 50+95*i;
        btns[i]->Width = g_iMidToolbarButtonWidth;
        btns[i]->Height = g_iMidToolbarButtonHeight;
        btns[i]->Left = (btns[i]->Parent->Width - btns[i]->Width)/2;
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormIPH::InitSkin()
{
}
//---------------------------------------------------------------------------
__fastcall TFormIPH::TFormIPH(TComponent* Owner)
    : TSubForm(Owner)
{
    gos_time_init();

    InitWorkPanel();

//    m_ulIPHStatus = IPH_STATUS_WAIT;

    LoadIPHVideoSize(m_iVideoWidth, m_iVideoHeight);

    btnCleaner->Enabled = false;
    gbVideo->Visible = false;
    Splitter->Visible = false;
    gbVideo->Height = m_iVideoHeight+2;

    TimerCheckIPHAck->Enabled = false;

    pmiAECMode->Down = IsLocalIPHAECMode();

    // 允许AEC模式，控制菜单可见
    if (AllowIPHAECMode())
    {
        pmiAECMode->Visible = ivAlways;
    }
    else
    {
        pmiAECMode->Visible = ivNever;
    }

//    gbVideo->Height = 704/2+2;
//  ResizeVideoWin((TWinCtrl*)PanelVideo, m_iVideoWidth, m_iVideoHeight);

    GetIPHAudioOption();

    /*
    if (!g_ThreadPcmPlay)
    {
        g_ThreadPcmPlay = ThreadPcmPlay::GetInstance();
        g_ThreadPcmPlay->Start();
    } */

    g_ThreadIPH = new ThreadIPH(FormMain->Handle, g_usDCRecvPCMUDPPort, &g_IPHWavRec);
    g_ThreadIPH->Start();

    g_ThreadPlayAECSound = new ThreadPlayAECSound(this->Handle);

    g_SockIPHPcmData = gos_create_udp_socket();

    AECInit();
    AECSetOnCapturedDataCB(OnAECCapData);

   //   g_RecThread = new RecThread(g_bIPHRecMergeMode);
   //   g_RecThread->Start();

    InitMsgHandler(EV_SA_DIS_DC_REQ, MSG_SAApplyIPHTalk, OnApplyIPHTalkReq);
    InitMsgHandler(EV_SA_DIS_DC_REQ, MSG_SAStopIPHTalk, OnStopIPHTalkReq);
    InitMsgHandler(EV_SA_DIS_DC_REQ, MSG_SAIPHAudioUploadMsg, OnSAUPloadIPHAudioData);
    InitMsgHandler(EV_SA_DIS_DC_REQ, MSG_SAIPHApplyACKMsg,    OnSAIPHApplyACKMsg);
    InitMsgHandler(EV_DC_DIS_SA_RSP, MSG_DCGetActiveIPHTalk,  OnGetActiveIPHTalkRsp);

    SendServer(EV_DC_DIS_SA_REQ, MSG_DCGetActiveIPHTalk);
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::pmiHangoffClick(TObject *Sender)
{
    if (tlMain->AbsoluteCount == 1)
    {
        tlMain->AbsoluteItems[0]->Selected = true;
    }

    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tlMain->Selections[0];

    if (Node->Values[5].IsNull())
    {
        return;
    }

    UINT32 ulStatus = (UINT32)Node->Values[5];

    if (ulStatus != IPH_STATUS_WAIT/* &&
        ulStatus != IPH_STATUS_ACCEPT*/ )
    {
        return;
    }

    if (tlMain->AbsoluteCount > 0 &&
        IsIPHStatus(IPH_STATUS_TALK))
    {
        InfoMsg("请先挂断当前对讲");
        return;
    }

    UINT32          ulTrainUnitID = Node->Values[1];
    TRAIN_INFO_T   *pstTrain = Train::GetTrainInfo(ulTrainUnitID);

    if (!pstTrain)
    {
        InfoMsg("未知列车");
        return;
    }

    m_bIPHAECMode = (AllowIPHAECMode() && IsLocalIPHAECMode());
    pmiAECMode->Enabled = false;

    FormCallIn->AcceptIPHTalk(ulTrainUnitID);

    if (!OpenMic())
    {
        InfoMsg("打开麦克风失败");
        return;
    }

    UINT32  ulRet = SendIPHAccept(Node, FALSE);

    if (ulRet == IPH_CLOSED)
    {
        Node->Delete();
        InfoMsg("该对讲已关闭");
        return;
    }
    else if (ulRet == IPH_ACCEPT_FAIL)
    {
        InfoMsg("接听对讲失败");
        return;
    }
    //点击接听按钮后该对讲进入等待接听应答状态
    else if(ulRet == IPH_ACCEPT_WAIT_ACK)
    {
        SetIPHStatus(IPH_STATUS_WAIT_ACK);
        Node->Values[5] = IPH_STATUS_WAIT_ACK;

        m_ulTrainUnitID = Node->Values[1];
        m_ulCarriageID  = Node->Values[2];
        m_ulIPHDevID    = Node->Values[3];
        m_ulDoorID      = Node->Values[4];
        m_strVideoUrl   = Node->Texts[6];

        UINT32  ulIPHDevIP = Node->Values[7];

        SET_INT(m_aucIPHDevIP, ulIPHDevIP);

        g_CurrIPHNode = Node;

        UpdateButtonStatus();

        TimerCheckIPHAck->Enabled = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::pmiHangupClick(TObject *Sender)
{
    if (IsIPHStatus(IPH_STATUS_OVER))
    {
        return;
    }

    if (SendStopIPHTalkReq() == IPH_STOP_FAIL)
    {
//      WarnMsg("挂断失败！");
//      return;
    }

    SetIPHStatus(IPH_STATUS_OVER);

//  m_iVideoWidth = -1;
//  m_iVideoHeight = -1;
    TimerCheckRTP->Enabled = false;

    StopIPHTalk();

    gbVideo->Visible  = false;
    Splitter->Visible = false;

    for (int i=0; i<tlMain->AbsoluteCount; i++)
    {
        if (!tlMain->AbsoluteItems[i]->Values[1].IsNull() &&
            m_ulTrainUnitID == (UINT32)tlMain->AbsoluteItems[i]->Values[1] &&
            m_ulCarriageID == (UINT32)tlMain->AbsoluteItems[i]->Values[2] &&
            m_ulIPHDevID == (UINT32)tlMain->AbsoluteItems[i]->Values[3] )
        {
            tlMain->AbsoluteItems[i]->Delete();
            break;
        }
    }

    UpdateButtonStatus();
}

void __fastcall TFormIPH::ColumnStatusGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[5].IsNull())
    {
        return;
    }

    int iValue = (int)ANode->Values[5];

    if (iValue == IPH_STATUS_WAIT)
    {
        Value = "";
    }
    else if (iValue == IPH_STATUS_WAIT_ACK)
    {
        Value = "接听中";
    }
    else if (iValue == IPH_STATUS_TALK)
    {
        Value = "对讲中";
    }
    else if (iValue == IPH_STATUS_OVER)
    {
        Value = "结束";
    }
    else if(iValue == IPH_STATUS_ACK_ERR)
    {
         Value = "报警器未响应,请挂断";
    }
    else
    {
        Value = "";
    }
}
//---------------------------------------------------------------------------
BOOL __fastcall TFormIPH::SendIPHAccept()
{
    UINT32  ulSeqID = 0;

    GJsonParam         JsonSubParam;
    GJsonTupleParam    JsonTupleParam;

    JsonSubParam.Add("TrainUnitID", m_ulTrainUnitID);
    JsonSubParam.Add("CarriageID",  m_ulCarriageID);
    JsonSubParam.Add("DoorID",      m_ulDoorID);
    JsonSubParam.Add("IPHDevID",    m_ulIPHDevID);
    JsonSubParam.Add("ApplyResult", IPH_APPLY_ACCEPT);
    JsonSubParam.Add("VideoURL",    m_strVideoUrl.c_str());

    JsonTupleParam.Add(JsonSubParam);

    return SendServer(EV_DC_DIS_SA_REQ, MSG_DCAcceptIPHTalk, 0, JsonTupleParam);
}

//---------------------------------------------------------------------------
UINT32 __fastcall TFormIPH::SendIPHAccept(TcxTreeListNode *Node, BOOL bSyncMode)
{
    UINT32  ulSeqID = 0;
    UINT32  ulTrainUnitID   = Node->Values[1];
    UINT32  ulCarriageID    = Node->Values[2];
    UINT32  ulIPHDevID      = Node->Values[3];
    UINT32  ulDoorID        = Node->Values[4];
    AnsiString  strVideoURL = Node->Values[6];
    UINT8   aucDCAddr[4];   // 连接DIS的IP地址认为是能与车载台相通的
    UINT16  usDCPort;
    //GJsonParam  JsonParam;
    GJsonParam  JsonSubParam;
    //VECTOR<GJsonParam*>   vJsonTupleParam;
    GJsonTupleParam     JsonTupleParam;

    g_pRemoteMsg->GetClientAddr(aucDCAddr, &usDCPort);

    JsonSubParam.Add("TrainUnitID", ulTrainUnitID);
    JsonSubParam.Add("CarriageID", ulCarriageID);
    JsonSubParam.Add("DoorID", ulDoorID);
    JsonSubParam.Add("IPHDevID", ulIPHDevID);
    JsonSubParam.Add("ApplyResult", IPH_APPLY_ACCEPT);
    JsonSubParam.AddIP("AudioRecvAddr", aucDCAddr);
    JsonSubParam.Add("AudioRecvPort", g_usDCRecvPCMUDPPort);
    JsonSubParam.Add("VideoURL", strVideoURL.c_str());

    //vJsonTupleParam.push_back(&JsonSubParam);
    JsonTupleParam.Add(JsonSubParam);

    if (bSyncMode)
    {
        if (!SyncSendServer(EV_DC_DIS_SA_REQ, MSG_DCAcceptIPHTalk, JsonTupleParam))
        {
            GosLog(LOG_ERROR, "sync send MSG_DCAcceptIPHTalk failed");
            return IPH_ACCEPT_FAIL;
        }

        return IPH_ACCEPT_WAIT_ACK;
    }
    else
    {
        SendServer(EV_DC_DIS_SA_REQ, MSG_DCAcceptIPHTalk, 0, JsonTupleParam);
        return IPH_ACCEPT_WAIT_ACK;
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormIPH::StartVideo(AnsiString strUrl)
{
    if (strUrl.IsEmpty())
    {
        GosLog(LOG_ERROR, "IPH video is empty");
        return;
    }

  //    PanelVideo->Visible = true;
    HWND hWndPlayer = PanelVideo->Handle;

    CHAR    acRecFile[256];

    GetIPHVideoRecFileName(m_ulTrainUnitID, m_ulCarriageID, m_ulIPHDevID, acRecFile);

    if (!g_ThreadPlayAndRec)
    {
        g_ThreadPlayAndRec = new ThreadPlayAndRec(hWndPlayer, IsIPHVideoOnTcpMode());
        g_ThreadPlayAndRec->Start();

        UINT32  ulOpenTimeout = GetIPHVideoOpenTimeout();

        g_ThreadPlayAndRec->SetOpenTimeout(ulOpenTimeout);
    }

    GosLog(LOG_INFO, "start IPH video: %s", strUrl.c_str());

    g_ThreadPlayAndRec->SetURL(strUrl.c_str());
    //g_ThreadPlayAndRec->SetHandle(hWndPlayer);
    g_ThreadPlayAndRec->PlayAndRec(acRecFile);
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::gbIPHResize(TObject *Sender)
{
/*  PanelCtrl->Align = alNone;
    PanelCtrl->Left = 0;
    PanelCtrl->Top = gbIPH->Height - PanelCtrl->Height;
    PanelCtrl->Width = gbIPH->Width;

    PanelVideo->Height = PanelCtrl->Top - 15 - 4;   */
    //PanelVideo->Height = 50;

//  ResizeVideoWin((TWinCtrl*)PanelVideo, m_iVideoWidth, m_iVideoHeight);


}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::pmMainPopup(TObject *Sender)
{
    pmiHangoff->Enabled = false;
    pmiHangup->Enabled = false;

    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tlMain->Selections[0];

    if (Node->Values[5].IsNull())
    {
        return;
    }

    UINT32 ulStatus = (UINT32)Node->Values[5];

    if (ulStatus == IPH_STATUS_WAIT)
    {
        pmiHangoff->Enabled = true;
    }
  /*    else if (ulStatus == IPH_STATUS_ACCEPT)
    {
        pmiHangoff->Enabled = true;
    }*/
    else if (ulStatus == IPH_STATUS_TALK)
    {
        pmiHangup->Enabled = true;
    }
}
//---------------------------------------------------------------------------
TcxTreeListNode* __fastcall TFormIPH::GetIPHNode(UINT32 ulTrainUnitID, UINT32 ulCarriageID, UINT32 ulIPHDevID)
{
    TcxTreeListNode *Node = tlMain->Root->getFirstChild();

    for (; Node; Node=Node->getNextSibling())
    {
        if ((UINT32)Node->Values[1] == ulTrainUnitID &&
            (UINT32)Node->Values[2] == ulCarriageID &&
            (UINT32)Node->Values[3] == ulIPHDevID)
        {
            return Node;
        }
    }

    return NULL;
}

void DelIPHInfo(UINT32 ulTrainUnitID)
{
    for (int i=g_vIPHInfo.size()-1; i>=0; i--)
    {
        if (g_vIPHInfo[i].ulTrainUnitID == ulTrainUnitID)
        {
            g_vIPHInfo.erase(g_vIPHInfo.begin()+i);
        }
    }
}

void AddIPHInfo(IPH_INFO_T &stIPHInfo)
{
    g_vIPHInfo.push_back(stIPHInfo);
}

UINT32 GetIPHStatus(UINT32 ulTrainUnitID, UINT32 ulCarriageID, UINT32 ulIPHDevID)
{
    for (UINT32 i=0; i<g_vIPHInfo.size(); i++)
    {
        if (g_vIPHInfo[i].ulTrainUnitID == ulTrainUnitID &&
            g_vIPHInfo[i].ulCarriageID == ulCarriageID &&
            g_vIPHInfo[i].ulIPHDevID == ulIPHDevID )
        {
            return g_vIPHInfo[i].ulStatus;
        }
    }

    return IPH_STATUS_WAIT;
}

void FormatVideoURL(CHAR *szVideoURL)
{
    // rtsp://XXX.XXX.XXX.XXX:PORT/h264/ch1/main/av_stream
    // XXX.XXX.XXX.XXX为取流地址，即车载PIS服务器外网口IP地址

    char    *szPos = strstr(szVideoURL, "/main/");
    char    *szLast;

    if (szPos)
    {
        memmove(szPos, szPos+1, strlen(szPos+1)+1);
        memcpy(szPos, "/sub", 4);
    }
}

//---------------------------------------------------------------------------
VOID __fastcall TFormIPH::OnApplyIPHTalkReq(UINT32 ulSeqID, GJson &Json)
{
    TcxTreeListNode *Node;
    std::vector<GJson*> vSubJson = Json.GetSubJson();
    //VECTOR<IPH_INFO_T>  vIPHInfo;

    if (!APP::HasDCType(DC_TYPE_PASSENGER) )
    {
        GosLog(LOG_INFO, "ignore IPH req: %s", Json.GetText());
        return;
    }

    tlMain->BeginUpdate();

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        IPH_INFO_T  stIPHInfo = {0};
        GJson       *SubJson = vSubJson[i];

        stIPHInfo.ulApplyTime = gos_get_current_time();

        if (!SubJson->GetValue("TrainUnitID", &stIPHInfo.ulTrainUnitID))
        {
            GosLog(LOG_ERROR, "invalid TrainUnitID in msg APPLY_IPH_TALK_REQ");
            return;
        }

        if (!SubJson->GetValue("CarriageID", &stIPHInfo.ulCarriageID))
        {
            GosLog(LOG_ERROR, "invalid CarriageID in msg APPLY_IPH_TALK_REQ");
            return;
        }

        if (!SubJson->GetValue("DoorID", &stIPHInfo.ulDoorID))
        {
            GosLog(LOG_ERROR, "invalid DoorID in msg APPLY_IPH_TALK_REQ");
            return;
        }

        if (!SubJson->GetValue("IPHDevID", &stIPHInfo.ulIPHDevID))
        {
            GosLog(LOG_ERROR, "invalid IPHDevID in msg APPLY_IPH_TALK_REQ");
            return;
        }

        if (!SubJson->GetIP("IPHDevIP", stIPHInfo.aucIPHDevIP))
        {
            GosLog(LOG_ERROR, "invalid IPHDevIP in msg APPLY_IPH_TALK_REQ");
            return;
        }

        if (!SubJson->GetValue("VideoURL", stIPHInfo.acVideoURL, sizeof(stIPHInfo.acVideoURL)))
        {
            GosLog(LOG_ERROR, "invalid VideoURL in msg APPLY_IPH_TALK_REQ");
            return;
        }

        GosLog(LOG_INFO, "get IPH talk req: %u %u %u %s", stIPHInfo.ulTrainUnitID,
              stIPHInfo.ulCarriageID, stIPHInfo.ulIPHDevID, stIPHInfo.acVideoURL);

        FormatVideoURL(stIPHInfo.acVideoURL);

        stIPHInfo.ulStatus = GetIPHStatus(stIPHInfo.ulTrainUnitID, stIPHInfo.ulCarriageID, stIPHInfo.ulIPHDevID);

        Node = GetIPHNode(stIPHInfo.ulTrainUnitID, stIPHInfo.ulCarriageID, stIPHInfo.ulIPHDevID);
        if (!Node)
        {
            Node = tlMain->Add();

            Node->Values[0] = stIPHInfo.ulApplyTime;
            Node->Values[1] = stIPHInfo.ulTrainUnitID;
            Node->Values[2] = stIPHInfo.ulCarriageID;
            Node->Values[3] = stIPHInfo.ulIPHDevID;
            Node->Values[4] = stIPHInfo.ulDoorID;
            Node->Values[5] = IPH_STATUS_WAIT;
            Node->Values[6] = stIPHInfo.acVideoURL;
            Node->Values[7] = GET_INT(stIPHInfo.aucIPHDevIP);
            Node->Values[8] = INVALID_USER_ID;
            Node->Values[9] = gos_get_uptime();

            AnsiString      strIPHKey = GetIPHKey(stIPHInfo.ulTrainUnitID, stIPHInfo.ulCarriageID, stIPHInfo.ulIPHDevID);

            if (g_mAcceptIPHInfo.find(strIPHKey) == g_mAcceptIPHInfo.end() ||
                g_mAcceptIPHInfo[strIPHKey] == INVALID_USER_ID)
            {
                FormCallIn->NewIPHTalk(stIPHInfo.ulTrainUnitID);
            }
        }
        else
        {
            //Node->Values[0] = stIPHInfo.ulApplyTime;
            if (Node->Values[6].IsNull())
            {
                Node->Values[6] = stIPHInfo.acVideoURL;
            }
            else
            {
                String  strUrl = Node->Values[6];

                if (strUrl != stIPHInfo.acVideoURL)
                {
                    Node->Values[6] = stIPHInfo.acVideoURL;
                }
            }

  /*            if (stIPHInfo.ulStatus != IPH_STATUS_ACCEPT &&
                stIPHInfo.ulStatus != IPH_STATUS_TALK   &&
                stIPHInfo.ulStatus != IPH_STATUS_PREVIEW)
            {
                Node->Values[4] = IPH_STATUS_WAIT;
            }  */


            //如果收到重复报文，自动发送应答
            if (//stIPHInfo.ulStatus == IPH_STATUS_ACCEPT ||
                stIPHInfo.ulStatus == IPH_STATUS_TALK )
            {
                if (!Node->Values[8].IsNull() &&
                    (UINT32)Node->Values[8] == APP::GetCurrDCUserID())
                {
                  //    SendIPHAccept(Node, FALSE);
                }
            }
        }

/*
        if (i == 0)
        {
            DelIPHInfo(stIPHInfo.ulTrainUnitID);
        }

        AddIPHInfo(stIPHInfo);

        vIPHInfo.push_back(stIPHInfo);    */
    }

    tlMain->EndUpdate();

    btnCleaner->Enabled = true;

 //   FormMain->ActivePage(PAGE_IPH);
    UpdateButtonStatus();

 // ShowIPHInfo(vIPHInfo);
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::OnStopIPHTalkReq(UINT32 ulSeqID, GJson &Json)
{
    UINT32      ulTrainUnitID;
    UINT32      ulCarriageID;
    UINT32      ulIPHDevID;
    CHAR        *szVideoURL;
    TcxTreeListNode *Node;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        GosLog(LOG_ERROR, "invalid TrainUnitID in msg STOP_IPH_TALK_REQ");
        return;
    }

    if (!Json.GetValue("CarriageID", &ulCarriageID))
    {
        GosLog(LOG_ERROR, "invalid CarriageID in msg STOP_IPH_TALK_REQ");
        return;
    }

    if (!Json.GetValue("IPHDevID", &ulIPHDevID))
    {
        GosLog(LOG_ERROR, "invalid IPHDevID in msg STOP_IPH_TALK_REQ");
        return;
    }

    Node = GetIPHNode(ulTrainUnitID, ulCarriageID, ulIPHDevID);
    if (!Node)
    {
        GosLog(LOG_INFO, "can't find IPH req of %u, %u, %u in STOP_IPH_TALK_REQ",
            ulTrainUnitID, ulCarriageID, ulIPHDevID);
        return;
    }

    GosLog(LOG_INFO, "stop IPH %u, %u, %u",
            ulTrainUnitID, ulCarriageID, ulIPHDevID);

    Node->Values[5] = IPH_STATUS_OVER;
    if (ulTrainUnitID == m_ulTrainUnitID &&
        ulCarriageID  == m_ulCarriageID &&
        ulIPHDevID    == m_ulIPHDevID)
    {
        Node->Delete();

        SetIPHStatus(IPH_STATUS_OVER);

        StopIPHTalk();
        gbVideo->Visible = false;
        Splitter->Visible = false;
    }
    else
    {
        Node->Delete();
    }

    SendServer(EV_DC_DIS_SA_REQ, MSG_DCGetActiveIPHTalk);
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::OnGetActiveIPHTalkRsp(UINT32 ulSeqID, GJson &Json)
{
    TcxTreeListNode *Node;
    bool            bExist;
    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = Json.GetSubJson();
    AnsiString      strUrl;
    IPH_TALK_INFO_T stInfo = {0};
    VECTOR<IPH_TALK_INFO_T> vInfo;
    IntSet          sTrain;

    g_mAcceptIPHInfo.clear();

    tlMain->BeginUpdate();

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("ApplyTime", &stInfo.ulApplyTime) ||
            !pSubJson->GetValue("TrainUnitID", &stInfo.ulTrainUnitID) ||
            !pSubJson->GetValue("TrainID", stInfo.acTrainID, sizeof(stInfo.acTrainID)) ||
            !pSubJson->GetValue("CarriageID", &stInfo.ulCarriageID)   ||
            !pSubJson->GetValue("IPHDevID", &stInfo.ulIPHDevID) ||
            !pSubJson->GetValue("AcceptDCUserID", &stInfo.ulAcceptDCUserID) ||
            !pSubJson->GetValue("AcceptTime", &stInfo.ulAcceptTime) ||
            !pSubJson->GetValue("StopTime", &stInfo.ulStopTime) ||
            !pSubJson->GetValue("Status", &stInfo.ulStatus) ||
            !pSubJson->GetIP("IPHDevIP", stInfo.aucIPHDevIP) ||
            !pSubJson->GetValue("VideoURL", stInfo.acVideoURL, sizeof(stInfo.acVideoURL)))
        {
            GosLog(LOG_ERROR, "parse IPH talk info failed on %s", pSubJson->GetText());
            continue;
        }

        AnsiString      strIPHKey = GetIPHKey(stInfo.ulTrainUnitID, stInfo.ulCarriageID, stInfo.ulIPHDevID);

        if (stInfo.ulAcceptDCUserID != INVALID_USER_ID)
        {
            g_mAcceptIPHInfo[strIPHKey] = stInfo.ulAcceptDCUserID;
        }
        else if (g_mAcceptIPHInfo.find(strIPHKey) == g_mAcceptIPHInfo.end())
        {
            g_mAcceptIPHInfo[strIPHKey] = stInfo.ulAcceptDCUserID;
        }

        if (stInfo.ulAcceptDCUserID == INVALID_USER_ID)
        {
            sTrain.Add(stInfo.ulTrainUnitID);
        }

        GosLog(LOG_DETAIL, "Active IPH: %u %u %u %u %u" ,
                stInfo.ulTrainUnitID,
                stInfo.ulCarriageID,
                stInfo.ulIPHDevID,
                stInfo.ulStatus,
                stInfo.ulAcceptDCUserID);

        vInfo.push_back(stInfo);

        // 判断记录是否存在
        bExist = false;
        for (int j=0; j<tlMain->AbsoluteCount; j++)
        {
            Node = tlMain->AbsoluteItems[j];

            if ((UINT32)Node->Values[1] == stInfo.ulTrainUnitID &&
                (UINT32)Node->Values[2] == stInfo.ulCarriageID &&
                (UINT32)Node->Values[3] == stInfo.ulIPHDevID )
            {
                bExist = true;
       //         sTrain.Del(stInfo.ulTrainUnitID);

                break;
            }
        }

        if (!bExist)
        {
            // 如果不存在，并且未被接听，则显示
            if (stInfo.ulAcceptDCUserID == INVALID_USER_ID &&
                stInfo.ulStatus != IPH_STATUS_OVER )
            {
                Node = tlMain->Add();
                Node->Values[0] = stInfo.ulApplyTime;
                Node->Values[1] = stInfo.ulTrainUnitID;
                Node->Values[2] = stInfo.ulCarriageID;
                Node->Values[3] = stInfo.ulIPHDevID;
                Node->Values[5] = stInfo.ulStatus;
                Node->Values[6] = stInfo.acVideoURL;
                Node->Values[7] = GET_INT(stInfo.aucIPHDevIP);
                Node->Values[8] = stInfo.ulAcceptDCUserID;
                Node->Values[9] = gos_get_uptime();
            }
        }
        else
        {
            // 如果存在，并且接听者是其他调度台，则删除该记录
            if (stInfo.ulAcceptDCUserID != INVALID_USER_ID &&
                stInfo.ulAcceptDCUserID != APP::GetCurrDCUserID() )
            {
                Node->Delete();
            }
            // 如果存在，而且处于结束状态，则删除该记录
            else if (//stInfo.ulAcceptDCUserID == APP::GetCurrDCUserID() &&
                     stInfo.ulStatus == IPH_STATUS_OVER )
            {
                Node->Delete();
            }
            else
            {
                // 如果存在，并且未接听，或者接听者是本调度台，则更新
                strUrl = Node->Values[6];
//              Node->Values[0] = stInfo.ulApplyTime;
                if ((UINT32)Node->Values[5] != stInfo.ulStatus ||
                    strUrl != stInfo.acVideoURL ||
                    (UINT32)Node->Values[7] != GET_INT(stInfo.aucIPHDevIP) ||
                    (UINT32)Node->Values[8] != stInfo.ulAcceptDCUserID )
                {
                    Node->Values[5] = stInfo.ulStatus;
                    Node->Values[6] = stInfo.acVideoURL;
                    Node->Values[7] = GET_INT(stInfo.aucIPHDevIP);
                    Node->Values[8] = stInfo.ulAcceptDCUserID;
    //              Node->Values[9] = gos_get_uptime();
                }
            }
        }
    }

    // 删除已经不存在的记录
    for (int j=tlMain->AbsoluteCount-1; j>=0; j--)
    {
        Node = tlMain->AbsoluteItems[j];
        bExist = false;

        for (UINT32 i=0; i<vInfo.size(); i++)
        {
            IPH_TALK_INFO_T     *pstInfo = &vInfo[i];

            if ((UINT32)Node->Values[1] == pstInfo->ulTrainUnitID &&
                (UINT32)Node->Values[2] == pstInfo->ulCarriageID &&
                (UINT32)Node->Values[3] == pstInfo->ulIPHDevID )
            {
                bExist = true;
                break;
            }
        }

        if (!bExist)
        {
            // 如果记录在dis已经不存在，并且非正在接听的记录，则删除
            if ((UINT32)Node->Values[1] != m_ulTrainUnitID ||
                (UINT32)Node->Values[2] != m_ulCarriageID ||
                (UINT32)Node->Values[3] != m_ulIPHDevID ||
                (UINT32)Node->Values[8] != APP::GetCurrDCUserID())
            {
                Node->Delete();
            }
        }
    }

    tlMain->EndUpdate();

    if (tlMain->AbsoluteCount == 0)
    {
        pmiHangupClick(this);
    }

end:
    FormCallIn->UpdateIPHTalk(sTrain);//g_mAcceptIPHInfo);

    UpdateButtonStatus();
}
//---------------------------------------------------------------------------

void __fastcall TFormIPH::OnSAIPHApplyACKMsg(UINT32 ulSeqID, GJson &Json)
{
    TimerCheckIPHAck->Enabled = false;

    SetIPHStatus(IPH_STATUS_TALK);
    g_CurrIPHNode->Values[5] = IPH_STATUS_TALK;
    g_ulIPHAckCheckCount = 0;

    // 将主扬声器设置为操作系统缺省扬声器
    SetDefaultSpeaker(GetMainSpeaker());

    gbVideo->Visible = true;
    m_iVideoWidth  = -1;
    m_iVideoHeight = -1;
    StartVideo(m_strVideoUrl);

    PanelEmpty->Visible = false;
    StartIPHTalk();
    TimerCheckRTP->Enabled = true;
    g_CurrIPHNode->MakeVisible();

    char    acTitle[128];

    sprintf(acTitle, "列车%u 车厢%u 设备编号%u", m_ulTrainUnitID, m_ulCarriageID, m_ulIPHDevID);
    gbIPH->Caption = acTitle;

    UpdateButtonStatus(g_CurrIPHNode);
    pmiPlayVideo->Visible = ivNever;
    pmiStopVideo->Visible = ivAlways;

    g_ulIPHAckCheckCount = 0;

    return ;
}

//---------------------------------------------------------------------------
void __fastcall TFormIPH::OnSAUPloadIPHAudioData(UINT32 ulSeqID, GJson &Json)
{
    UINT32              ulTrainUnitID;
    UINT32              ulCarriageID;
    UINT32              ulIPHDevID;
    UINT32              ulDataIndex;
    UINT32              ulDataTime;
    UINT32              ulSampleRate;
    UINT32              ulChannelNum;
    UINT32              ulSampleBits;
    UINT32              ulPCMDataLen;
    UINT8               *pucPcmByteData = NULL;
    CHAR                *szPcmData = NULL;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        GosLog(LOG_ERROR, "invalid TrainUnitID in msg %s", Json.GetText());
        return;
    }

    if (!Json.GetValue("CarriageID", &ulCarriageID))
    {
        GosLog(LOG_ERROR, "invalid CarriageID in msg %s", Json.GetText());
        return;
    }

    if (!Json.GetValue("IPHDevID", &ulIPHDevID))
    {
        GosLog(LOG_ERROR, "invalid IPHDevID in msg %s", Json.GetText());
        return;
    }

    if (!Json.GetValue("DataIndex", &ulDataIndex))
    {
        GosLog(LOG_ERROR, "invalid DataIndex in msg %s", Json.GetText());
        return;
    }

    if (!Json.GetValue("Time", &ulDataTime))
    {
        GosLog(LOG_ERROR, "invalid Time in msg %s", Json.GetText());
        return;
    }

    if (!Json.GetValue("SampleRate", &ulSampleRate))
    {
        GosLog(LOG_ERROR, "invalid SampleRate in msg %s", Json.GetText());
        return;
    }

    if (!Json.GetValue("ChannelNum", &ulChannelNum))
    {
        GosLog(LOG_ERROR, "invalid ChannelNum in msg %s", Json.GetText());
        return;
    }

    if (!Json.GetValue("SampleBits", &ulSampleBits))
    {
        GosLog(LOG_ERROR, "invalid SampleBits in msg %s", Json.GetText());
        return;
    }

    if (!Json.GetValue("PCMDataLen", &ulPCMDataLen))
    {
        GosLog(LOG_ERROR, "invalid PCMDataLen in msg %s", Json.GetText());
        return;
    }

    szPcmData = Json.GetValue("PCMData");
    if (!szPcmData)
    {
        GosLog(LOG_ERROR, "invalid PCMData failed in msg %s", Json.GetText());
        return;
    }

    GosLog(LOG_INFO, "recv iph(%u %u %u) audio data length(%u), data_index(%u)",
                                ulTrainUnitID, ulCarriageID, ulIPHDevID, ulPCMDataLen, ulDataIndex);


    UINT32      ulPcmPkgSize = (strlen(szPcmData) / 2);

    GosLog(LOG_DETAIL, "real PCM data size is %u", ulPcmPkgSize);

    if (ulPcmPkgSize == 0 || ulPcmPkgSize > 64*1024)
    {
        GosLog(LOG_ERROR, "invalid PCM data length %u", ulPcmPkgSize);
        return;
    }

    if ( (pucPcmByteData = (char *)calloc(1, (ulPcmPkgSize+2))) == NULL )
    {
        GosLog(LOG_ERROR, "calloc fail in OnSAUPloadIPHAudioData");

        return ;
    }

    if (!gos_hexstr_to_byte(szPcmData, pucPcmByteData))
    {
        GosLog(LOG_ERROR, "convert pcm dat fail");

        free(pucPcmByteData);
        return ;
    }


//    AnsiString strPlayDevice = "";//    采用操作系统缺省扬声器

    /*
    if (!g_ThreadPcmPlay->IsPlaying())
    {

        g_ThreadPcmPlay->Play(strPlayDevice.c_str(), ulChannelNum, ulSampleRate, (ulSampleBits/8));
    }

    g_ThreadPcmPlay->PushData(paucPcmByteData, ulPCMDataLen);   */

    g_ThreadIPH->PushData(ulChannelNum, ulSampleRate, ulSampleBits, pucPcmByteData, ulPCMDataLen);

    free(pucPcmByteData);
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::tlMainFocusedNodeChanged(TcxCustomTreeList *Sender, TcxTreeListNode *APrevFocusedNode,
          TcxTreeListNode *AFocusedNode)
{
    UpdateButtonStatus();
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::UpdateButtonStatus(TcxTreeListNode *Node)
{
    if (tlMain->AbsoluteCount <= 0)
    {
        btnHangoff->Enabled = false;
        btnHangup->Enabled  = false;
        pmiHangoff->Enabled = false;
        pmiHangup->Enabled  = false;
        btnCleaner->Enabled = false;

        return ;
    }

    if (!Node)
    {
        btnHangoff->Enabled = true;
        btnHangup->Enabled  = false;
        pmiHangoff->Enabled = true;
        pmiHangup->Enabled  = false;

        return ;
    }

    //TcxTreeListNode     *Node = tlMain->Selections[0];

    if (Node->Values[5].IsNull() || Node->Values[1].IsNull())
    {
        return;
    }

    UINT32  ulStatus = (UINT32)Node->Values[5];
    UINT32  ulTainUnitID = (UINT32)Node->Values[1];

    if (ulStatus == IPH_STATUS_WAIT)
    {
        pmiHangup->Enabled = false;
        btnHangup->Enabled = false;
    }
    else if(ulStatus == IPH_STATUS_WAIT_ACK)
    {
        pmiHangup->Enabled = true;
        btnHangup->Enabled = true;
    }
    else if (ulStatus == IPH_STATUS_TALK)
    {
        pmiHangoff->Enabled = false;
        btnHangoff->Enabled = false;
        pmiHangup->Enabled = true;
        btnHangup->Enabled = true;
    }
    else if (ulStatus == IPH_STATUS_OVER)
    {
        pmiHangup->Enabled = false;
        btnHangup->Enabled = false;
        pmiHangoff->Enabled = false;
        btnHangoff->Enabled = false;
    }
    else if (ulStatus == IPH_STATUS_ACK_ERR)
    {
        pmiHangup->Enabled = true;
        btnHangup->Enabled = true;
        pmiHangoff->Enabled = false;
        btnHangoff->Enabled = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::UpdateButtonStatus()
{
    if (tlMain->AbsoluteCount <= 0)
    {
        btnHangoff->Enabled = false;
        btnHangup->Enabled  = false;
        pmiHangoff->Enabled = false;
        pmiHangup->Enabled  = false;
        btnCleaner->Enabled = false;

        return ;
    }

    btnHangoff->Enabled = true;
    btnHangup->Enabled  = false;
    pmiHangoff->Enabled = true;
    pmiHangup->Enabled  = false;

    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tlMain->Selections[0];

    if (Node->Values[5].IsNull() || Node->Values[1].IsNull())
    {
        return;
    }

    UINT32  ulStatus = (UINT32)Node->Values[5];
    UINT32  ulTainUnitID = (UINT32)Node->Values[1];

    if (ulStatus == IPH_STATUS_WAIT)
    {
        pmiHangup->Enabled = false;
        btnHangup->Enabled = false;
    }
    else if(ulStatus == IPH_STATUS_WAIT_ACK)
    {
        pmiHangup->Enabled = true;
        btnHangup->Enabled = true;
    }
    else if (ulStatus == IPH_STATUS_TALK)
    {
        pmiHangoff->Enabled = false;
        btnHangoff->Enabled = false;
        pmiHangup->Enabled = true;
        btnHangup->Enabled = true;
    }
    else if (ulStatus == IPH_STATUS_OVER)
    {
        pmiHangup->Enabled = false;
        btnHangup->Enabled = false;
        pmiHangoff->Enabled = false;
        btnHangoff->Enabled = false;
    }
    else if (ulStatus == IPH_STATUS_ACK_ERR)
    {
        pmiHangup->Enabled = true;
        btnHangup->Enabled = true;
        pmiHangoff->Enabled = false;
        btnHangoff->Enabled = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::TimerCheckTimer(TObject *Sender)
{
    UINT32  ulTime;
    UINT32  ulTrainUnitID;
    IntSet  sActive;
    IntSet  sDel;
    UINT32  ulStatus;
    TcxTreeListNode     *AutoHangupNode = NULL;

    if (tlMain->AbsoluteCount == 0)
    {
        return;
    }

    if (g_CurrIPHNode &&
        (g_ulLastIPHPkgTime+g_ulCheckIPHPkgTime) < gos_get_uptime())
    {
        AutoHangupNode = g_CurrIPHNode;

        sDel.Add(m_ulTrainUnitID);
        g_CurrIPHNode->Delete();
    }

    // 接听超时的由DIS服务器通知
/*    for (int i=tlMain->AbsoluteCount-1; i>=0; i--)
    {
     ulTime = (UINT32)tlMain->AbsoluteItems[i]->Values[9];
        ulTrainUnitID = (UINT32)tlMain->AbsoluteItems[i]->Values[1];
        ulStatus = (UINT32)tlMain->AbsoluteItems[i]->Values[5];

      if (ulStatus == IPH_STATUS_TALK &&
            (ulTime+GetMaxIPHTalkTime()) < gos_get_uptime() )
        {
            sDel.Add(ulTrainUnitID);
            if (ulTrainUnitID == m_ulTrainUnitID &&
                (UINT32)tlMain->AbsoluteItems[i]->Values[2] == m_ulCarriageID &&
                (UINT32)tlMain->AbsoluteItems[i]->Values[3] == m_ulIPHDevID )
            {
                AutoHangupNode = tlMain->AbsoluteItems[i];
            }
            else
            {
                tlMain->AbsoluteItems[i]->Delete();
            }
        }
        else
        {
            sActive.Add(ulTrainUnitID);
        }
    }  */

    RemoveCommonMember(sActive, sDel);

    for (UINT32 i=0; i<sDel.Size(); i++)
    {
        ulTrainUnitID = sDel.Get(i);
        FormCallIn->StopIPHTalk(ulTrainUnitID);
    }

    if (AutoHangupNode ||
        tlMain->AbsoluteCount == 0)
    {
        pmiHangupClick(this);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::TimerCheckRTPTimer(TObject *Sender)
{
    if (!g_ThreadPlayAndRec ||
        !TimerCheckRTP->Enabled)
    {
        return;
    }

    if (!m_strVideoUrl.IsEmpty() &&
        g_ThreadPlayAndRec )
    {
        g_ThreadPlayAndRec->KeepPlay();
    }

    int     iWidth;
    int     iHeight;

    g_ThreadPlayAndRec->GetVideoSize(&iWidth, &iHeight);
    if (iWidth <= 0 || iHeight <= 0)
    {
        return;
    }

    if (iWidth > 0 &&
        iWidth == m_iVideoWidth &&
        iHeight == m_iVideoHeight )
    {
        return;
    }

    m_iVideoWidth = iWidth;
    m_iVideoHeight = iHeight;

    ResizeVideoPanel();
    SaveIPHVideoSize(m_iVideoWidth, m_iVideoHeight);
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::FormCreate(TObject *Sender)
{
    SetImage(pmiHangoff, IMG_ACCEPT_CALL);
    SetImage(pmiHangup, IMG_REJECT_CALL);

    SetImage(pmiPlayVideo, IMG_PLAY);
    SetImage(pmiStopVideo, IMG_STOP);
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::FormClose(TObject *Sender, TCloseAction &Action)
{
    StopIPHTalk();

    if (g_ThreadPlayAndRec)
    {
        g_ThreadPlayAndRec->Free();
        g_ThreadPlayAndRec = NULL;
    }

    g_ThreadIPH->Stop();
    g_ThreadIPH = NULL;

    /*
    if (g_ThreadPcmPlay)
    {
        g_ThreadPcmPlay->Free();
        g_ThreadPcmPlay = NULL;
    }
     */

    CloseMic();
   //   g_RecThread->Free();
   //   g_RecThread = NULL;
    g_IPHWavRec.StopRec();
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::ColumnTrainGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[1].IsNull())
    {
        return;
    }

    UINT32          ulTrainUnitID = ANode->Values[1];
    TRAIN_INFO_T   *pstTrain = Train::GetTrainInfo(ulTrainUnitID);

    if (pstTrain && pstTrain->acTrainID[0])
    {
        Value = AnsiString(ulTrainUnitID) + AnsiString("(") +
                pstTrain->acTrainID + AnsiString(")");
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::ColumnTimeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[0].IsNull())
    {
        return;
    }

    UINT32          ulTime = ANode->Values[0];

    Value = GetTextTime(ulTime);
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::ColumnDCUserIDGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    if (ANode->Values[8].IsNull())
    {
        return;
    }

    if ((UINT32)ANode->Values[8] == INVALID_USER_ID)
    {
        Value = "";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::tlMainClick(TObject *Sender)
{
    UpdateButtonStatus();
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::ColumnDevPosGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    if (ANode->Values[2].IsNull() ||
        ANode->Values[4].IsNull() )
    {
        return;
    }

    UINT32  ulCarriageID = ANode->Values[2];
    UINT32  ulDoorID     = ANode->Values[4];

    AnsiString  strDoor;

    if (ulCarriageID == 1)
    {
        strDoor = "TC1司机室";
    }
    else if (ulCarriageID == 8)
    {
        strDoor = "TC2司机室";
    }
    else
    {
        strDoor += (AnsiString(ulDoorID) + AnsiString("号门"));
    }

    Value = strDoor;
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::ResizeVideoPanel()
{
    int iMediaWidth = m_iVideoWidth;
    int iMediaHeight = m_iVideoHeight;
    int iTotalWidth = PanelVideo->Parent->Width;
    int iTotalHeight = PanelVideo->Parent->Height;

    if (iMediaWidth <= 0 || iMediaHeight <= 0)
    {
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
void __fastcall TFormIPH::AutoAccept()
{
    if (tlMain->AbsoluteCount == 1)
    {
        pmiHangoffClick(this);
    }
}
void __fastcall TFormIPH::pmiAECModeClick(TObject *Sender)
{
    SetLocalIPHAECMode(pmiAECMode->Down);
    SaveLocalConfInfo();
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::PanelVideoMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    if (Button == mbRight)
    {
        pmVideo->PopupFromCursorPos();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::pmiPlayVideoClick(TObject *Sender)
{
    if (!g_ThreadPlayAndRec)
    {
        return;
    }

    pmiPlayVideo->Visible = ivNever;
    pmiStopVideo->Visible = ivAlways;

    TimerCheckRTP->Enabled = true;
    g_ThreadPlayAndRec->Resume();

    PanelEmpty->Visible = false;

    GosLog(LOG_INFO, "restart IPH video");
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::pmiStopVideoClick(TObject *Sender)
{
    if (g_ThreadPlayAndRec)
    {
        pmiPlayVideo->Visible = ivAlways;
        pmiStopVideo->Visible = ivNever;

        TimerCheckRTP->Enabled = false;

        g_ThreadPlayAndRec->Pause();

        GosLog(LOG_INFO, "pause IPH video");
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::Active()
{
    // 暂停状态
    if (pmiPlayVideo->Visible == ivAlways)
    {
        PanelEmpty->Visible = true;
        PanelEmpty->BringToFront();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::TimerGetIPHTimer(TObject *Sender)
{
    SendServer(EV_DC_DIS_SA_REQ, MSG_DCGetActiveIPHTalk);
}
//---------------------------------------------------------------------------
void __fastcall TFormIPH::TimerCheckIPHAckTimer(TObject *Sender)
{
    if (g_CurrIPHNode && g_ulIPHAckCheckCount >= IPH_CHECK_ACK_MAX_TIME)
    {
        SendStopIPHTalkReq();
        SetIPHStatus(IPH_STATUS_OVER);

        g_CurrIPHNode->Values[5] =  IPH_STATUS_ACK_ERR;
        TimerCheckIPHAck->Enabled = FALSE;

        //g_CurrIPHNode->Delete();
        //g_CurrIPHNode = NULL;

        UpdateButtonStatus(g_CurrIPHNode);
        g_ulIPHAckCheckCount = 0;
        return  ;
    }

    SendIPHAccept();
    g_ulIPHAckCheckCount++;
    return;
}
//---------------------------------------------------------------------------

void __fastcall TFormIPH::btnCleanerClick(TObject *Sender)
{
    VECTOR<UINT32> vTrainInfo;

    UINT32    ulTrainUnitID = 0;

    TcxTreeListNode *Node = tlMain->Root->getFirstChild();

    if (!Node->Values[1].IsNull())
    {
        ulTrainUnitID = Node->Values[1];
        vTrainInfo.push_back(ulTrainUnitID);
    }

    tlMain->BeginUpdate();
    while(Node)
    {
        if (!Node->Values[1].IsNull())
        {
            ulTrainUnitID = Node->Values[1];
            vTrainInfo.push_back(ulTrainUnitID);
        }

        TcxTreeListNode *TempNode = Node;
        Node=Node->getNextSibling();

        TempNode->Delete();
    }
    tlMain->EndUpdate();

    SetIPHStatus(IPH_STATUS_OVER);

    TimerCheckRTP->Enabled = false;

    StopIPHTalk();

    gbVideo->Visible  = false;
    Splitter->Visible = false;

    btnHangoff->Enabled = false;
    btnHangup->Enabled  = false;
    pmiHangoff->Enabled = false;
    pmiHangup->Enabled  = false;
    btnCleaner->Enabled = false;

    std::sort(vTrainInfo.begin(), vTrainInfo.end());
    vTrainInfo.erase(std::unique(vTrainInfo.begin(), vTrainInfo.end()), vTrainInfo.end());

    if (vTrainInfo.size() <= 0)
    {
        return ;
    }

    for (UINT32 i = 0; i < vTrainInfo.size(); i++)
    {
        SendCleanIPHTalkReq(vTrainInfo[i]);
        gos_sleep_1ms(5);
    }

    return ;
}
//---------------------------------------------------------------------------


void __fastcall TFormIPH::btnIPHPauseClick(TObject *Sender)
{
    UINT32    ulTrainUnitID = 0;
    UINT32    ulCarriageID  = 0;
    UINT32    ulIPHDevID    = 0;

    TcxTreeListNode     *Node = tlMain->Selections[0];
    if (!Node)
    {
        InfoMsg("请选取一条记录");
        return;
    }

    ulTrainUnitID = Node->Values[1];
    ulCarriageID  = Node->Values[2];
    ulIPHDevID    = Node->Values[3];

    GJsonParam    Param;
    Param.Add("TrainUnitID", ulTrainUnitID);
    Param.Add("CarriageID", ulCarriageID);
    Param.Add("IPHDevID", ulIPHDevID);

    SendServer(EV_DC_DIS_SA_REQ, MSG_DCPauseIPHTalkReq, Param);

    return ;
}
//---------------------------------------------------------------------------

void __fastcall TFormIPH::btnRenewIPHClick(TObject *Sender)
{
    pmiHangoffClick(this);
    return ;
}
//---------------------------------------------------------------------------

