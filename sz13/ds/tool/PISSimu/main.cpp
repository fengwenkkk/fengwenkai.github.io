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
#include "AEC.h"

#include "main.h"
#include "WavRec.h"
#include "MicCapture.h"
#include "PISThread.h"
#include "ThreadIPH.h"
#include "ThreadPcmPlay.h"
#include "PATCPServerThread.h"
#include "PAUDPServerThread.h"
#include "DiscoverThread.h"
#include "AudioVolume.h"
#include "ThreadPlayAECSound.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxGraphics"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "dxSkinBlue"
#pragma link "dxSkinOffice2010Blue"
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
#pragma link "dxSkinLilian"
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
#pragma resource "*.dfm"

TFormPIS *FormPIS;

bool            g_bTestMicVolume = false;
//DiscoverThread  *g_pDiscoverThread = NULL;
PISThread       *g_pPISThread = NULL;
ThreadIPH       *g_pThreadIPH = NULL;
ThreadPlayAECSound  *g_ThreadPlayAECSound = NULL;
PATCPThread     *g_pThreadTCPPA = NULL;
extern SOCKET   g_stSASocket;        // PIS client socket
extern SOCKET   g_stSARealBrdSocket; // PA client socket

bool        g_bAECMode = true;
bool		g_bTalking = false;

INT64       g_i64Volume1 = 0;
INT64       g_i64Volume2 = 0;
UINT32      g_ulVolume1Num = 0;
UINT32      g_ulVolume2Num = 0;

UINT32  m_ulSampleFreq = 16000;
UINT32  m_ulPCMFrameLen = 2;
UINT32  m_ulSendFrameNumPer100MillSecond = 100*m_ulSampleFreq/1000;
UINT8   *m_pucPCMData = NULL;
UINT8   *m_pucPCMData2 = NULL;
UINT32  m_ulCurrFrameIndex = 0;
UINT32  m_ulPCMFrameNum = 0;
UINT32  m_ulWavFileHdrLen = 78;
BOOL    g_bRecMode = FALSE;
IPH_TALK_APPLY_RSP_INFO_T   m_stApplyInfo = {0};

UINT32  g_ulRecID = 0xffffffff;
UINT32  g_ulBrdSeqID = 0xffffffff;

extern SOCKET       g_stIPHSocket;

WavRec      *g_pWavRec = NULL;
MicCapture  *g_MicCapture = NULL;

BOOL        g_bDevBusy = FALSE;
int         g_iMicVolume = 0;
int         g_iMicBoostLevel = 0;

UINT32      g_ulWaitAckCarriageID = 0;
UINT32      g_ulWaitAckIPHDevID = 0;

ThreadPcmPlay   *g_ThreadPcmPlay   = NULL;
PAUDPThread     *g_ThreadPAUDPPlay = NULL;

HANDLE          g_PCMPlay = NULL;   // 用于AEC自动录音使用

extern TColor GetSkinColor(AnsiString strSkinName);

void InitChart(TChart *cht)
{
    AnsiString strSkinName = GetDefaultSkinName();
    TColor  ColorBackground = GetSkinColor(strSkinName);

    cht->Title->Visible                = false;
    cht->Title->Font->Color            = clNavy;
    cht->Title->Font->Size             = 12;

//    cht->Align                       = alClient;
//  cht->Walls->Visible                = FALSE;
    cht->ParentColor                   = false;
    cht->ParentBackground              = false;

    cht->Gradient->StartColor = ColorBackground; // RGB(234,234,234);
    cht->Gradient->EndColor = ColorBackground;  //RGB(234,234,234);
    cht->Gradient->MidColor = ColorBackground;  //RGB(255,255,255);
    cht->Gradient->Visible             = true;

 //   cht->Zoom->Allow                   = false;
 //   cht->AllowPanning                  = Teeprocs::pmNone;

 //   cht->BevelInner                    = bvNone;
 //   cht->BevelOuter                    = bvNone;
 //   cht->BevelWidth                    = 1;

    cht->LeftAxis->Grid->Style         = psDot;
    cht->LeftAxis->Grid->SmallDots     = true;
    cht->LeftAxis->Grid->Color         = RGB(192,192,192); //clBlue;
    cht->LeftAxis->Grid->Width         = 1;
    cht->LeftAxis->Axis->Width         = 1;
    cht->LeftAxis->LabelsFont->Color   = clNavy;
/*
    cht->LeftAxis->RoundFirstLabel     = false;

    if (cht->LeftAxis->AxisValuesFormat == "#,##0.0")
    {
        cht->LeftAxis->AxisValuesFormat = "0.0";
    }
    else if (cht->LeftAxis->AxisValuesFormat == "#,##0")
    {
        cht->LeftAxis->AxisValuesFormat = "0";
    }

   */

   //   cht->BottomAxis->Visible           = TRUE;
    cht->BottomAxis->Grid->Width       = 1;
    cht->BottomAxis->Grid->Style       = psDot;
    cht->BottomAxis->Grid->SmallDots   = true;
    cht->BottomAxis->Grid->Color       = TColor(0x00FFB9B9);//chtMain->BottomAxis->Grid->Color;
    cht->BottomAxis->Axis->Width       = 1;
    cht->BottomAxis->LabelsFont->Color = clNavy;

    /*
    cht->BottomAxis->RoundFirstLabel   = false;
    cht->BottomAxis->Automatic         = true;
              */

    cht->BackWall->Color = 12615680;
    cht->BackWall->Pen->Color = -1325465600;
    cht->Frame->Color = cht->BackWall->Pen->Color;//-1325465600;

    cht->RightAxis->Axis->Visible = false;
    cht->TopAxis->Axis->Visible = false;

}

void StartRec()
{
    CHAR        acFile[256];

    sprintf(acFile, "%s/%s", gos_get_root_path(), "rec_test.wav");

    if (!g_pWavRec)
    {
        //g_pWavRec = new WavRec(acFile, m_ulSampleFreq, m_ulPCMFrameLen, TRUE);
        g_pWavRec = new WavRec();
        g_pWavRec->Init(acFile, m_ulSampleFreq, m_ulPCMFrameLen, TRUE);
    }
}

void StopRec()
{
    if (g_pWavRec)
    {
        g_pWavRec->StopRec();
        delete g_pWavRec;
        g_pWavRec = NULL;
    }
}

BOOL UDPSend(VOID *pvMsg, UINT32 ulMsgLen)
{
    static SOCKET   stSock = INVALID_SOCKET;
    SOCKADDR_IN     stServerAddr = {0};

    if (stSock == INVALID_SOCKET)
    {
        stSock = gos_create_udp_socket();
    }

    gos_init_socket_addr(&stServerAddr, m_stApplyInfo.aucAudioRecvAddr, m_stApplyInfo.usAudioRecvPort);

    return gos_udp_send(stSock, pvMsg, ulMsgLen, &stServerAddr);
}

#define SMOOTH_NUM  52
INT16   g_asSmoothData[SMOOTH_NUM];

INT16 SmoothData(INT16 sValue)
{
    // 去掉最大值 &去掉最小值
    memmove(g_asSmoothData, &g_asSmoothData[1], (SMOOTH_NUM-1)*sizeof(INT16));
    g_asSmoothData[SMOOTH_NUM-1] = sValue;

    INT32       iMin = g_asSmoothData[0];
    INT32       iMax = g_asSmoothData[0];
    INT32       iSum = g_asSmoothData[0];

    for (int i=1; i<SMOOTH_NUM; i++)
    {
        if (iMin > g_asSmoothData[i])
        {
            iMin = g_asSmoothData[i];
        }

        if (iMax < g_asSmoothData[i])
        {
            iMax = g_asSmoothData[i];
        }

        iSum += g_asSmoothData[i];
    }

    iSum -= iMin;
    iSum -= iMax;
    sValue = iSum / (SMOOTH_NUM-2);

    g_asSmoothData[SMOOTH_NUM-1] = sValue;

    return sValue;
}

VOID AutoAdjustVolume(UINT8 *pucData, UINT32 ulSize)
{
    INT16      *psData = (INT16*)pucData;
    DOUBLE      dRate = 1;
    DOUBLE      dMax = 32767;
    DOUBLE      dMin = -32768;
    DOUBLE      dNewValue;
    DOUBLE      dCurrVolume = Power(10, ((DOUBLE)g_iMicBoostLevel)/20);  // 1 - 32

    // MIC音量在无麦克风加强时，最大音量值为7000，而不少32768/32 = 1024
    dCurrVolume /= 32;
    dCurrVolume = dCurrVolume*g_iMicVolume/100;
    if (dCurrVolume < 0.01)
    {
        dRate = 100;
    }
    else
    {
        dRate = 1/dCurrVolume;
    }

    dRate /= 7;    // 7 = 7000/1024
    dRate *= 0.9;   // 再做一下音量保护

    dRate = 1;

    if (!pucData && ulSize > 0)
    {
        return;
    }

    if (!pucData || ulSize > 4000)
    {
        ulSize += 0;
    }

    for (UINT32 i=0; i<ulSize/sizeof(INT16); i++)
    {
        dNewValue = ((DOUBLE)psData[i])*dRate;
        if (dNewValue > dMax)
        {
            psData[i] = dMax;
        }
        else if (dNewValue < dMin)
        {
            psData[i] = dMin;
        }
        else
        {
            psData[i] = dNewValue;
        }

      //    psData[i] = SmoothData(psData[i]);
    }
}

INT16 CountVolume(UINT8 *pucData, UINT32 ulSize)
{
    INT16      *psData = (INT16*)pucData;

    for (UINT32 i=0; i<ulSize/sizeof(INT16); i++)
    {
        if (psData[i] >= 0)
        {
            g_i64Volume1 += psData[i];
            g_ulVolume1Num++;
        }
        else
        {
            g_i64Volume2 -= psData[i];
            g_ulVolume2Num++;
        }

      //    psData[i] = SmoothData(psData[i]);
    }
}

VOID OnMicData(UINT8 *pucData, UINT32 ulSize)
{
    if (g_bTestMicVolume)
    {
        CountVolume(pucData, ulSize);
        return;
    }

  //  AutoAdjustVolume(pucData, ulSize);
  //  CountVolume(pucData, ulSize);

    if (g_pWavRec)
    {
        g_pWavRec->Rec(TRUE, pucData, ulSize);
    }

    UINT32          ulDataLen = STRUCT_OFFSET(IPH_PCM_DATA_T, aucPCMData) + ulSize;
    UINT32          ulMsgLen = sizeof(PIS_MSG_HDR_T) + ulDataLen;
    PIS_MSG_HDR_T   *pstMsgHdr = (PIS_MSG_HDR_T*)malloc(ulMsgLen);
    IPH_PCM_DATA_T  *pstPCMData = (IPH_PCM_DATA_T*)(pstMsgHdr+1);
    UINT32          ulCurrTime = gos_get_uptime_1ms();
    UINT32          ulIndex = m_ulCurrFrameIndex/1600;

    if (!pstMsgHdr)
    {
        return;
    }

    pstMsgHdr->usMsgMagic = HTONS(PIS_MSG_MAGIC);
    pstMsgHdr->usMsgType = HTONS(EV_IPH_AUDIO_UPLOAD);
    pstMsgHdr->usMsgLen = HTONS(ulDataLen);

    pstPCMData->usTrainUnitID = HTONS(m_stApplyInfo.usTrainUnitID);   // 车组号（车体号）
    pstPCMData->ucCarriageID = m_stApplyInfo.ucCarriageID;   // 车厢号
    pstPCMData->ucIPHDevID = m_stApplyInfo.ucIPHDevID;     // 报警器ID

    pstPCMData->ulPCMPkgIndex = HTONL(ulIndex);  // PCM数据包序号，第一个数据包为0，后续递增
    pstPCMData->ulPCMPkgTime = HTONL(ulCurrTime);   // PCM数据包偏移时间，相对第一个PCM数据包的相对偏移时间，单位ms
    pstPCMData->usSampleRate = HTONS(m_ulSampleFreq);   // 采样频率（Hz）   8000/16000/44100等
    pstPCMData->ucChanNum = 1;      // 采样声道数   1：单声道  2：双声道
    pstPCMData->ucSampleBits = 8*m_ulPCMFrameLen;   // 采样量化位数 8：8位（1字节） 16：16位（2字节）
    pstPCMData->usPCMDataLen = HTONS(ulSize);   // 数据长度(=usSampleRate*ucChanNum*ucSampleBits/8)
    memcpy(pstPCMData->aucPCMData, pucData, ulSize);

    UDPSend(pstMsgHdr, ulMsgLen);

    GosLog(LOG_INFO, "send IPH data, index = %u, size= %u, time = %u",
            ulIndex, ulSize, ulCurrTime);
}

VOID OnAECCapData(UINT8 *pucData, UINT32 ulLen, UINT32 ulSampleRate, UINT32 ulBitsPerSample)
{
    OnMicData(pucData, ulLen);
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

INT32 ToInt(Variant var)
{
    if (var.IsNull())
    {
        return -1;
    }

    AnsiString strValue = var;

    return strValue.ToIntDef(-1);
}

AnsiString GetIPText(UINT8 *pucAddr)
{
    char    acText[32];

    sprintf(acText, IP_FMT, IP_ARG(pucAddr));

    return acText;
}

BOOL LoadPCMData()
{
    CHAR    acFile[256];
    FILE    *fp;
    INT32   iSize;

    if (m_pucPCMData)
    {
        return TRUE;
    }

    sprintf(acFile, "%s/%s", gos_get_root_path(), "test1.wav");
    iSize = gos_get_file_size(acFile);
    if (iSize <= m_ulWavFileHdrLen)
    {
        return FALSE;
    }

    iSize -= m_ulWavFileHdrLen;
    m_ulPCMFrameNum = iSize/m_ulPCMFrameLen;
    m_pucPCMData = (UINT8*)calloc(1, iSize);
    if (!m_pucPCMData)
    {
        fclose(fp);
        return FALSE;
    }

    fp = fopen(acFile, "rb");
    if (!fp)
    {
        GOS_FREE(m_pucPCMData);
        return FALSE;
    }

    fseek(fp, SEEK_SET, m_ulWavFileHdrLen);
    if (iSize != fread(m_pucPCMData, 1, iSize, fp))
    {
        fclose(fp);
        GOS_FREE(m_pucPCMData);
        return FALSE;
    }

    fclose(fp);
    return TRUE;
}

BOOL LoadPCMData2()
{
    CHAR    acFile[256];
    FILE    *fp;
    INT32   iSize;

    if (m_pucPCMData2)
    {
        return TRUE;
    }

    sprintf(acFile, "%s/%s", gos_get_root_path(), "test2.wav");
    iSize = gos_get_file_size(acFile);
    if (iSize <= m_ulWavFileHdrLen)
    {
        return FALSE;
    }

    iSize -= m_ulWavFileHdrLen;

    if (m_ulPCMFrameNum > iSize/m_ulPCMFrameLen)
    {
        m_ulPCMFrameNum = iSize/m_ulPCMFrameLen;
    }

    m_pucPCMData2 = (UINT8*)calloc(1, iSize);
    if (!m_pucPCMData2)
    {
        fclose(fp);
        return FALSE;
    }

    fp = fopen(acFile, "rb");
    if (!fp)
    {
        GOS_FREE(m_pucPCMData2);
        return FALSE;
    }

    fseek(fp, SEEK_SET, m_ulWavFileHdrLen);
    if (iSize != fread(m_pucPCMData2, 1, iSize, fp))
    {
        fclose(fp);
        GOS_FREE(m_pucPCMData2);
        return FALSE;
    }

    fclose(fp);
    return TRUE;
}

//---------------------------------------------------------------------------
__fastcall TFormPIS::TFormPIS(TComponent* Owner)
    : TSubForm(Owner)
{
    GosInitLog();
    InitLocalAddr();
    InitPARunningStatus();

    btnBrdBusy->Enabled = false;
    btnRealBrdStart->Enabled = false;

    InitMsgHandler(MSG_PIS, OnPISMsg);
    InitMsgHandler(MSG_PA_TCP, OnPATCPMsg);
    //InitMsgHandler(MSG_DISCOVER, OnDiscoverMsg);

    InitChart(cht);

    AECInit();

    AECSetOnCapturedDataCB(OnAECCapData);

    g_ThreadPlayAECSound = new ThreadPlayAECSound(this->Handle);

    memset(g_asSmoothData, 0, sizeof(g_asSmoothData));
}

  BOOL __fastcall TFormPIS::SendPATCPMsg(UINT16 usMsgType, void *pvMsg, UINT32 ulMsgLen)
{
    UINT32         ulPkgLen = sizeof(PA_MSG_HDR_T)+ ulMsgLen;
    PA_MSG_HDR_T   *pstMsgHdr = (PA_MSG_HDR_T*)calloc(1, ulPkgLen);
    PA_REAL_BRD_RSP_T *pstStartRsp = (PA_REAL_BRD_RSP_T*)(pstMsgHdr+1);  //Test

    if (!pstMsgHdr)
    {
        InfoMsg("发送消息失败");
        return false;
    }

    memcpy((pstMsgHdr+1), pvMsg, ulMsgLen);

    ///GosLog(LOG_INFO, "Start real PA Rsp of %u, result is %u",
            ///ntohl(pstStartRsp->ulBrdSeqID), pstStartRsp->ucBrdResult);

    pstMsgHdr->usMsgMagic = HTONS(PIS_MSG_MAGIC);
    pstMsgHdr->usMsgType  = HTONS(usMsgType);
    pstMsgHdr->usMsgLen   = HTONS(ulMsgLen);

    //ulMsgLen += sizeof(PA_MSG_HDR_T);

    if (g_stSARealBrdSocket == INVALID_SOCKET)
    {
        InfoMsg("发送消息失败：车载台未接入");
        free(pstMsgHdr);
        return FALSE;
    }

    if (!gos_tcp_send(g_stSARealBrdSocket, pstMsgHdr, ulPkgLen))
    {
        InfoMsg("发送消息失败");
        free(pstMsgHdr);
        return FALSE;
    }

    free(pstMsgHdr);
    return TRUE;
}

bool __fastcall TFormPIS::SendMsg(UINT16 usMsgType, void *pvMsg, UINT32 ulMsgLen)
{
    PIS_MSG_HDR_T           *pstMsgHdr = (PIS_MSG_HDR_T*)m_aucMsgBuf;

    memcpy(pstMsgHdr+1, pvMsg, ulMsgLen);

    pstMsgHdr->usMsgMagic = HTONS(PIS_MSG_MAGIC);
    pstMsgHdr->usMsgType = HTONS(usMsgType);
    pstMsgHdr->usMsgLen = HTONS(ulMsgLen);

    ulMsgLen += sizeof(PIS_MSG_HDR_T);

    if (g_stSASocket == INVALID_SOCKET)
    {
        InfoMsg("发送消息失败：车载台未接入");
        return false;
    }

    if (!gos_tcp_send(g_stSASocket, pstMsgHdr, ulMsgLen))
    {
        InfoMsg("发送消息失败");
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::btnIPHApplyClick(TObject *Sender)
{
    PIS_MSG_HDR_T           *pstMsgHdr = (PIS_MSG_HDR_T*)m_aucMsgBuf;
    IPH_TALK_APPLY_REQ_T    *pstReq = (IPH_TALK_APPLY_REQ_T*)(pstMsgHdr+1);
    UINT32  ulMsgLen = 0;

    AnsiString   strIPHRecvAudioIP =  edtIPHRecvAudioAddr->Text;
    UINT32       ulIPHRecvAudioPort = edtIPHRecvAudioPort->Text.ToIntDef(0);

    pstReq->ucNum = 2;
    pstReq->astApplyReq[0].usTrainUnitID = edtTrainUnitID->Text.ToIntDef(0);
    pstReq->astApplyReq[0].ucCarriageID =  edtCarriageID1->Text.ToIntDef(0);
    pstReq->astApplyReq[0].ucIPHDevID = edtIPHDevID1->Text.ToIntDef(0);
    pstReq->astApplyReq[0].usIPHRecvAudioPort = ulIPHRecvAudioPort;//edtIPHRecvAudioPort->Text.ToIntDef(0);
    pstReq->astApplyReq[0].usIPHDevDoorID = edtIPHDoorID1->Text.ToIntDef(0);
    gos_get_ip((const char*)strIPHRecvAudioIP.c_str(), pstReq->astApplyReq[0].aucIPHRecvAudioAddr);

    if (edtIPHUrl1->Text.Length() > (sizeof(pstReq->astApplyReq[0].acVideoUrl)-1))
    {
        InfoMsg("URL 长度超出预期");
        return;
    }
    StrCpy(pstReq->astApplyReq[0].acVideoUrl, edtIPHUrl1->Text);
    memset(pstReq->astApplyReq[0].acCCTVVideoUrl, 0, sizeof(pstReq->astApplyReq[0].acCCTVVideoUrl));



    pstReq->astApplyReq[1].usTrainUnitID = pstReq->astApplyReq[0].usTrainUnitID;//edtTrainUnitID2->Text.ToIntDef(0);
    pstReq->astApplyReq[1].ucCarriageID = edtCarriageID2->Text.ToIntDef(0);
    pstReq->astApplyReq[1].ucIPHDevID = edtIPHDevID2->Text.ToIntDef(0);
    pstReq->astApplyReq[1].usIPHRecvAudioPort = ulIPHRecvAudioPort; //edtIPHRecvAudioPort->Text.ToIntDef(0);
    pstReq->astApplyReq[1].usIPHDevDoorID = edtIPHDoorID2->Text.ToIntDef(0);
    gos_get_ip((const char*)strIPHRecvAudioIP.c_str(), pstReq->astApplyReq[1].aucIPHRecvAudioAddr);

    if (edtIPHUrl2->Text.Length() > (sizeof(pstReq->astApplyReq[1].acVideoUrl)-1))
    {
        InfoMsg("URL 长度超出预期");
        return;
    }
    StrCpy(pstReq->astApplyReq[1].acVideoUrl, edtIPHUrl2->Text);
    memset(pstReq->astApplyReq[1].acCCTVVideoUrl, 0, sizeof(pstReq->astApplyReq[1].acCCTVVideoUrl));



    edtIPHStatus1->Text = "申请";
    edtIPHStatus2->Text = "申请";

    if (pstReq->astApplyReq[1].ucCarriageID == 0)
    {
        pstReq->ucNum = 1;
        edtIPHStatus2->Text = "";
    }

    for (UINT32 i=0; i<pstReq->ucNum; i++)
    {
        HTONS_IT(pstReq->astApplyReq[i].usTrainUnitID);
        HTONS_IT(pstReq->astApplyReq[i].usIPHRecvAudioPort);
        HTONS_IT(pstReq->astApplyReq[i].usIPHDevDoorID);
    }

    ulMsgLen = STRUCT_OFFSET(IPH_TALK_APPLY_REQ_T, astApplyReq) + pstReq->ucNum*sizeof(pstReq->astApplyReq[0]);

    SendMsg(EV_IPH_TALK_APPLY, pstReq, ulMsgLen);

    //if (!g_pThreadIPH)
	//{
	//	g_pThreadIPH = new ThreadIPH(this->Handle, DC_RECV_IPH_PORT);
	//}

    //g_pThreadIPH->Start();
	//g_pThreadIPH->StartPlay(1, 1, 1);
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::OnSAGetTrainInfoReq()
{
    PIS_GET_TRAIN_INFO_RSP_T    stRsp = {0};

    if (cbAutoMode->Checked)
    {
        stRsp.ucDriverMode = DRIVE_MODE_AUTO;
    }
    else
    {
        stRsp.ucDriverMode = DRIVE_MODE_MANUAL;
    }

    if (cbOpenDevCover->Checked)
    {
        stRsp.ucCoverStatus = COVER_STATUS_OPEN;
    }
    else
    {
        stRsp.ucCoverStatus = COVER_STATUS_CLOSE;
    }

	mmGetTrainInfo->Text = AnsiString(gos_get_text_time(NULL)) + " : recv SA get train_info req";

    SendPATCPMsg(EV_PIS_GET_TRAIN_INFO, &stRsp, sizeof(stRsp));
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::OnSAGetPASysInfoReq()
{
	PA_GET_SYS_STATUS_RSP_T    stRsp = {0};

	if (cbPAMaster->Checked)
	{
		stRsp.ucIsMaster = TRUE;
	}
	else
	{
		stRsp.ucIsMaster = FALSE;
	}

	if (cbPARunningStatus->ItemIndex == 0)
	{
		stRsp.ucStatus = PA_STATUS_NA;
	}
	else if(cbPARunningStatus->ItemIndex == 1)
	{
		stRsp.ucStatus = PA_STATUS_IDLE;
	}
	else if(cbPARunningStatus->ItemIndex == 2)
	{
		stRsp.ucStatus = PA_STATUS_WORK;
	}
	else
	{
        GosLog(LOG_ERROR, "unknow pa running status");
	}

	mmGetTrainInfo->Text = AnsiString(gos_get_text_time(NULL)) + " : recv PA heartbeat req";

	SendPATCPMsg(EV_PA_GET_SYS_STATUS, &stRsp, sizeof(stRsp));

    ///GosLog(LOG_INFO, "send to pa heartbeat rsp");
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::OnSAGetIPHSysInfoReq()
{
	IPH_GET_SYS_STATUS_RSP_T    stRsp = {0};

	if (cbIPHRunningStatus->Checked)
	{
		stRsp.ucStatus = IPH_STATUS_NORMAL;
	}
	else
	{
		stRsp.ucStatus = IPH_STATUS_ABNORMAL;
	}

	mmGetTrainInfo->Text = AnsiString(gos_get_text_time(NULL)) + " : recv IPH heartbeat req";

	SendMsg(EV_PIS_HEART_BEAT, &stRsp, sizeof(stRsp));
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::OnSASelfCheckReportedReq(PIS_SELFCHECK_STATUS_REQ_T *MsgData)
{
	if (!MsgData)
	{
		GosLog(LOG_ERROR, "invaild msg data for PIS_SELFCHECK_STATUS_REQ_T");
        return;
	}

	CHAR    aucBuf[128] = {0};
	sprintf(aucBuf, "%s:app self_check:%u", gos_get_text_time(NULL), MsgData->ucCheckStatus);
	mmGetTrainInfo->Text = aucBuf;
	return;
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::OnIPHTalkApplyRsp(IPH_TALK_APPLY_RSP_T *pstMsg)
{
    btnIPHTalkAck->Enabled = true;

    for (UINT32 i=0; i<pstMsg->ucNum; i++)
    {
        IPH_TALK_APPLY_RSP_INFO_T   *pstApplyInfo = &pstMsg->astApplyRsp[i];

        NTOHS_IT(pstApplyInfo->usTrainUnitID);
        NTOHS_IT(pstApplyInfo->usAudioRecvPort);

        if (edtTrainUnitID->Text == pstApplyInfo->usTrainUnitID &&
            edtCarriageID1->Text == pstApplyInfo->ucCarriageID &&
            edtIPHDevID1->Text == pstApplyInfo->ucIPHDevID)
        {
            btnIPHhandup1->Enabled = true;
            edtDCAddr1->Text = GetTextIP(pstApplyInfo->aucAudioRecvAddr)+":" + AnsiString(pstApplyInfo->usAudioRecvPort);
        }
        else if (edtTrainUnitID->Text == pstApplyInfo->usTrainUnitID &&
                 edtCarriageID2->Text == pstApplyInfo->ucCarriageID &&
                 edtIPHDevID2->Text == pstApplyInfo->ucIPHDevID)
        {
            btnIPHhandup2->Enabled = true;
            edtDCAddr1->Text = GetTextIP(pstApplyInfo->aucAudioRecvAddr)+":" + AnsiString(pstApplyInfo->usAudioRecvPort);
        }

        g_ulWaitAckCarriageID = pstApplyInfo->ucCarriageID;
        g_ulWaitAckIPHDevID = pstApplyInfo->ucIPHDevID;

        memcpy(&m_stApplyInfo, pstApplyInfo, sizeof(m_stApplyInfo));
        break;
    }

    //if (g_bAECMode)
    //{
    //    BOOL bRet = AECStart("", "", m_ulSampleFreq, m_ulPCMFrameLen);
    //
    //    if (!bRet)
    //    {
    //        GosLog(LOG_ERROR, "AEC start failed");
    //    }
    //
    //    g_bTalking = true;
    //}
    //else
    //{
    //    if (!OpenMic())
    //    {
    //        InfoMsg("打开麦克风失败");
    //        return;
    //    }
    //
    //    StartGrabAudio();
    //}

    LabelMicHint->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::OnIPHAudioDownload(IPH_PCM_DATA_T *pstMsg)
{
    if (edtCarriageID1->Text == pstMsg->ucCarriageID &&
        edtIPHDevID1->Text == pstMsg->ucIPHDevID )
    {
        edtIPHAudio1->Text = GetTextTime(pstMsg->ulPCMPkgTime);
    }
    else if (edtCarriageID2->Text == pstMsg->ucCarriageID &&
             edtIPHDevID2->Text == pstMsg->ucIPHDevID )
    {
        edtIPHAudio2->Text = GetTextTime(pstMsg->ulPCMPkgTime);
    }

    INT64   i64Time = gos_get_uptime_1ns();

    NTOHS_IT(pstMsg->usPCMDataLen);

    if (!g_pWavRec)
    {
        StartRec();
    }

    if (g_pWavRec)
    {
        g_pWavRec->Rec(FALSE, pstMsg->aucPCMData, pstMsg->usPCMDataLen);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::OnLTEPauseIPHTalk(IPH_TALK_PAUSE_REQ_T *pstMsg)
{
    btnIPHTalkAck->Enabled = false;
    btnIPHhandup1->Enabled = false;
    btnIPHhandup2->Enabled = false;
    TimerSendPCM->Enabled = false;
    btnIPHTalkAck->Enabled = false;
    m_ulCurrFrameIndex = 0;

    UINT8     aucMsgBuf[128] = {0};
    UINT32    ulTrainUnitID =  ntohs(pstMsg->usTrainUnitID);
    UINT32    ulCarriageID  =  pstMsg->ucCarriageID;
    UINT32    ulIPHDevID    =  pstMsg->ucIPHDevID;

    PIS_MSG_HDR_T           *pstMsgHdr = (PIS_MSG_HDR_T*)aucMsgBuf;
    IPH_TALK_PAUSE_RSP_T    *pstReq = (IPH_TALK_PAUSE_RSP_T*)(pstMsgHdr+1);
    UINT32                  ulMsgLen = sizeof(IPH_TALK_PAUSE_RSP_T);

    pstReq->usTrainUnitID = ulTrainUnitID;
    pstReq->ucCarriageID  = ulCarriageID;
    pstReq->ucIPHDevID    = ulIPHDevID;
    pstReq->ucIPHPauseResult = 0;

    if (cbIPHPauseFail->Checked)
    {
        pstReq->ucIPHPauseResult = 1;// 0:挂起成功正常， 其他值：挂起失败
    }

    HTONS_IT(pstReq->usTrainUnitID);

    pstMsgHdr->usMsgMagic = HTONS(PIS_MSG_MAGIC);
    pstMsgHdr->usMsgType = HTONS(EV_IPH_STOP_TALK);
    pstMsgHdr->usMsgLen = HTONS(ulMsgLen);

    ulMsgLen += sizeof(PIS_MSG_HDR_T);

    if (g_stSASocket == INVALID_SOCKET)
    {
        InfoMsg("发送消息失败：车载台未接入");
        return;
    }

    if (!gos_tcp_send(g_stSASocket, pstMsgHdr, ulMsgLen))
    {
        InfoMsg("发送消息失败");
	}

	g_pThreadIPH->StopPlay();

    return ;
}

//---------------------------------------------------------------------------
void __fastcall TFormPIS::OnIPHTalkStopRsp(IPH_TALK_STOP_RSP_T *pstMsg)
{

    if (pstMsg->ucCarriageID == IPH_TALK_RESET &&
        pstMsg->ucIPHDevID   == IPH_TALK_RESET)
    {
        GosLog(LOG_INFO, "recv iph talk clean req");
    }

    if (edtCarriageID1->Text == pstMsg->ucCarriageID &&
        edtIPHDevID1->Text == pstMsg->ucIPHDevID )
    {
        edtIPHStatus1->Text = "结束";
    }
    else if (edtCarriageID2->Text == pstMsg->ucCarriageID &&
             edtIPHDevID2->Text == pstMsg->ucIPHDevID )
    {
        edtIPHStatus2->Text = "结束";
    }

	g_pThreadIPH->StopPlay();

    if (g_bAECMode)
    {
        AECStop();
        g_bTalking = false;
    }
    else
    {
        StopGrabAudio();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::OnPAStartRecBrdReq(PA_REC_BRD_REQ_T *pstMsg)
{
    NTOHL_IT(pstMsg->ulRecID);
    NTOHL_IT(pstMsg->ulBrdSeqID);

    g_ulRecID = pstMsg->ulRecID;
    g_ulBrdSeqID = pstMsg->ulBrdSeqID;

    char    acInfo[128];

    sprintf(acInfo, "RecID=%u, BrdSeqID=%u, BrdTime=%u", pstMsg->ulRecID, pstMsg->ulBrdSeqID, pstMsg->ucBrdLoopTime);

    edtCurrRecID->Text = pstMsg->ulRecID;
    mmBrd->Lines->Append(acInfo);
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::OnPAGetCfgReq()
{
    ID_VALUE_T      astCfg[] = {
                    {1, (CHAR*)"乘客们，本次列车因故将稍作停留，请您耐心等待，谢谢配合。"},
                    {2, (CHAR*)"全体乘客请注意，由于故障，本次列车将退出运营，请乘客们尽快下车，给您的出行带来不便，我们深表歉意，敬请谅解。"},
                    {3, (CHAR*)"全体乘客请注意，现发生紧急情况，请不要惊慌，听从工作人员指挥，有序撤离。"},
                    {4, (CHAR*)"乘客们，现在客流较大，列车比较拥挤，下车乘客请及早准备，抓紧下车，上车乘客请尽量往里走，谢谢配合。"},
                    };

    UINT32      ulMsgLen = 0;
    CHAR        acMsg[4*1024];
    UINT16      *pusTmp;
    CHAR        *szTmp = acMsg;
    UINT32      ulLen;

    pusTmp = (UINT16*)szTmp;
    *pusTmp = htons(ARRAY_SIZE(astCfg));

    szTmp += 2;
    ulMsgLen = 2;
    for (UINT32 i=0; i<ARRAY_SIZE(astCfg); i++)
    {
        ulLen = strlen(astCfg[i].szValue)+1;

        pusTmp = (UINT16*)szTmp;
        *pusTmp++ = htons(astCfg[i].ulID);
        *pusTmp++ = htons(ulLen);

        szTmp = (CHAR*)pusTmp;
        memcpy(szTmp, astCfg[i].szValue, ulLen);
        szTmp += ulLen;
        ulMsgLen += 2+2+ulLen;
    }

    SendPATCPMsg(EV_PA_REC_CFG, acMsg, ulMsgLen);
}

void __fastcall TFormPIS::OnRealBrdStartReq(PA_REAL_BRD_REQ_T *pstMsg)
{
    //if (FALSE == g_bDevBusy)
    {
        btnBrdBusy->Enabled = true;
        btnRealBrdStart->Enabled = true;

        g_ulBrdSeqID = NTOHL(pstMsg->ulBrdSeqID);
        edtCurrRealBrdID->Text = g_ulBrdSeqID;

        edtRealBrdCarriageID->Text = pstMsg->ucCarriageID;
        //GosLog(LOG_INFO, "this real brd carriage is %u", pstMsg->ucCarriageID);

        //if (g_ThreadPAUDPPlay == NULL)
        //{
            //g_ThreadPAUDPPlay = new PAUDPThread(FormPIS->Handle);
        //}

        if (g_ThreadPAUDPPlay)
        {
            g_ThreadPAUDPPlay->Start();
        }

        GosLog(LOG_INFO, "PAUDPThread is start!");
        g_bDevBusy = TRUE;
    }
    //else
    //{
       //btnBrdBusyClick(this);
    //}

}

void __fastcall TFormPIS::OnRealBrdStopReq(PA_STOP_REAL_BRD_REQ_T *pstMsg)
{
    if (g_ThreadPAUDPPlay)
    {
        g_ThreadPAUDPPlay->Stop();
    }

    PA_STOP_REAL_BRD_RSP_T  stRsp;

    stRsp.ulBrdSeqID = pstMsg->ulBrdSeqID;

    GosLog(LOG_INFO, "stop real broadcast %u", ntohl(pstMsg->ulBrdSeqID));

    SendPATCPMsg(EV_PA_STOP_REAL_BRD, &stRsp, sizeof(stRsp));
}

void __fastcall TFormPIS::OnPATCPMsg(UINT8 *pucMsg, UINT32 ulMsgLen)
{
    PA_MSG_HDR_T    *pstMsg = (PA_MSG_HDR_T*)pucMsg;
    VOID            *pvMsgData = (VOID*)(pstMsg+1);

	UINT16    usMsgType = pstMsg->usMsgType;

	GosLog(LOG_DETAIL, "recv PA tcp msg type:%#x", usMsgType);

	if (usMsgType == EV_PA_START_REAL_BRD)
    {
        OnRealBrdStartReq((PA_REAL_BRD_REQ_T*)pvMsgData);
    }
    else if (usMsgType == EV_PA_STOP_REAL_BRD)
    {
        OnRealBrdStopReq((PA_STOP_REAL_BRD_REQ_T*)pvMsgData);
    }
	else if (usMsgType == EV_PA_REC_BRD )           // 调度台预录音广播
    {
        OnPAStartRecBrdReq((PA_REC_BRD_REQ_T*)pvMsgData);
    }
	else if (usMsgType == EV_PA_REC_CFG )           // 调度台预录音广播配置
    {
        OnPAGetCfgReq();
    }
	else if (usMsgType == EV_PIS_GET_TRAIN_INFO)
    {
		OnSAGetTrainInfoReq();
    }
	else if (usMsgType == EV_PA_GET_SYS_STATUS)
    {
		OnSAGetPASysInfoReq();
	}
	else if (usMsgType == EV_PIS_HEART_BEAT)
    {
		OnSAGetIPHSysInfoReq();
	}
	else if (usMsgType == EV_PIS_SELFCHECK_STATUS)
    {
		OnSASelfCheckReportedReq((PIS_SELFCHECK_STATUS_REQ_T*)pvMsgData);
	}
    else
    {
		GosLog(LOG_ERROR, "unknown PA msg type :%#x", usMsgType);
    }

}

//---------------------------------------------------------------------------
void __fastcall TFormPIS::OnPISMsg(UINT8 *pucMsg, UINT32 ulMsgLen)
{
    PIS_MSG_HDR_T   *pstMsg = (PIS_MSG_HDR_T*)pucMsg;
    VOID            *pvMsgData = (VOID*)(pstMsg+1);

    if (pstMsg->usMsgType == EV_PIS_HEART_BEAT)
    {
		OnSAGetIPHSysInfoReq();
    }
    else if (pstMsg->usMsgType == EV_IPH_TALK_APPLY)
    {
        OnIPHTalkApplyRsp((IPH_TALK_APPLY_RSP_T*)pvMsgData);
    }
    else if (pstMsg->usMsgType == EV_IPH_AUDIO_DOWNLOAD)        // 调度台音频下发
    {
        OnIPHAudioDownload((IPH_PCM_DATA_T*)pvMsgData);
    }
    else if (pstMsg->usMsgType == EV_IPH_STOP_TALK )            // 调度台结束对讲
    {
        OnIPHTalkStopRsp((IPH_TALK_STOP_RSP_T*)pvMsgData);
    }
    else if (pstMsg->usMsgType == EV_IPH_PAUSE_TALK )
    {
        OnLTEPauseIPHTalk((IPH_TALK_PAUSE_REQ_T*)pvMsgData);
    }
    else
    {
        GosLog(LOG_ERROR, "unknown PIS msg %u", pstMsg->usMsgType);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::OnGetSelfCheckReq(PIS_SELFCHECK_STATUS_REQ_T *pstMsg)
{
    if (0 == pstMsg->ucCheckStatus)
    {
        edtSelfCheckInfo->Text = "0:正常";
    }
    else
    {
        edtSelfCheckInfo->Text = AnsiString(pstMsg->ucCheckStatus) + ":失败";
    }

}
//---------------------------------------------------------------------------
/*void __fastcall TFormPIS::OnDiscoverMsg(UINT8 *pucMsg, UINT32 ulMsgLen)
{
    PIS_FIND_PIS_REQ_T  *pstMsg = (PIS_FIND_PIS_REQ_T*)pucMsg;

    edtCurrAddr->Text = GetIPText(pstMsg->aucPISAddr);
    edtGateway->Text = GetIPText(pstMsg->aucGateway);
    edtSAAddr->Text = GetIPText(pstMsg->aucLocalAddr);

    static UINT8    aucPISAddr[4] = {0,0,0,0};

    if (g_pPISThread)
    {
        if (memcmp(pstMsg->aucPISAddr, aucPISAddr, sizeof(aucPISAddr)) != 0)
        {
            delete g_pPISThread;
            g_pPISThread = NULL;
        }
    }

    btnCreateTcpSocketClick(this);
}*/

void __fastcall TFormPIS::btnIPHhandup1Click(TObject *Sender)
{
    btnIPHhandup1->Enabled = false;
    TimerSendPCM->Enabled = false;
    btnIPHTalkAck->Enabled = false;
    m_ulCurrFrameIndex = 0;

    PIS_MSG_HDR_T           *pstMsgHdr = (PIS_MSG_HDR_T*)m_aucMsgBuf;
    IPH_TALK_STOP_REQ_T     *pstReq = (IPH_TALK_STOP_REQ_T*)(pstMsgHdr+1);
    UINT32                  ulMsgLen = sizeof(IPH_TALK_STOP_REQ_T);

    pstReq->usTrainUnitID = edtTrainUnitID->Text.ToIntDef(0);
    pstReq->ucCarriageID =  edtCarriageID1->Text.ToIntDef(0);
    pstReq->ucIPHDevID = edtIPHDevID1->Text.ToIntDef(0);

    HTONS_IT(pstReq->usTrainUnitID);

    pstMsgHdr->usMsgMagic = HTONS(PIS_MSG_MAGIC);
    pstMsgHdr->usMsgType = HTONS(EV_IPH_STOP_TALK);
    pstMsgHdr->usMsgLen = HTONS(ulMsgLen);

    ulMsgLen += sizeof(PIS_MSG_HDR_T);

    if (g_stSASocket == INVALID_SOCKET)
    {
        InfoMsg("发送消息失败：车载台未接入");
        return;
    }

    if (!gos_tcp_send(g_stSASocket, pstMsgHdr, ulMsgLen))
    {
        InfoMsg("发送消息失败");
	}

	g_pThreadIPH->StopPlay();
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::btnIPHhandup2Click(TObject *Sender)
{
    btnIPHhandup2->Enabled = false;
    TimerSendPCM->Enabled  = false;
    btnIPHTalkAck->Enabled = false;
	m_ulCurrFrameIndex = 0;

    g_pThreadIPH->StopPlay();
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::TimerSendPCMTimer(TObject *Sender)
{
    SendPCMData();
}

//---------------------------------------------------------------------------
void __fastcall TFormPIS::SendPCMData()
{
    if ((m_ulCurrFrameIndex+m_ulSendFrameNumPer100MillSecond) >= m_ulPCMFrameNum)
    {
        m_ulCurrFrameIndex = 0;
    }

    UINT8   *pucCurrPCMData = m_pucPCMData + m_ulCurrFrameIndex*m_ulPCMFrameLen;
    UINT32  ulSize = m_ulSendFrameNumPer100MillSecond*m_ulPCMFrameLen;

    OnMicData(pucCurrPCMData, ulSize);

    m_ulCurrFrameIndex += m_ulSendFrameNumPer100MillSecond;
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::btnRecBrdRspClick(TObject *Sender)
{
    PA_REC_BRD_RSP_T        stRsp;

    stRsp.ulRecID = HTONL(g_ulRecID);
    stRsp.ulBrdSeqID = HTONL(g_ulBrdSeqID);
    stRsp.ucRet = REC_BRD_RESULT_SUCC;

    SendPATCPMsg(EV_PA_REC_BRD, &stRsp, sizeof(stRsp));

  /*
    PIS_MSG_HDR_T           *pstMsgHdr = (PIS_MSG_HDR_T*)m_aucMsgBuf;
    PA_REC_BRD_RSP_T        *pstReq = (PA_REC_BRD_RSP_T*)(pstMsgHdr+1);
    UINT32                  ulMsgLen = sizeof(PA_REC_BRD_RSP_T);

    pstReq->ulRecID = HTONL(g_ulRecID);
    pstReq->ulBrdSeqID = HTONL(g_ulBrdSeqID);
    pstReq->ucRet = REC_BRD_RESULT_SUCC;

    pstMsgHdr->usMsgMagic = HTONS(PIS_MSG_MAGIC);
    pstMsgHdr->usMsgType = HTONS(EV_PA_REC_BRD);
    pstMsgHdr->usMsgLen = HTONS(ulMsgLen);

    ulMsgLen += sizeof(PIS_MSG_HDR_T);

    if (g_stSARealBrdSocket == INVALID_SOCKET)
    {
        InfoMsg("发送消息失败：车载台未接入");
        return;
    }

    if (!gos_tcp_send(g_stSARealBrdSocket, pstMsgHdr, ulMsgLen))
    {
        InfoMsg("发送消息失败");
    }   */
}

//---------------------------------------------------------------------------
void __fastcall TFormPIS::TimerInitTimer(TObject *Sender)
{
    TimerInit->Interval = 2000;
    return;

    if (!g_pThreadIPH)
    {
        GosLog(LOG_INFO, "start thread IPH");
/*      g_pThreadIPH = new ThreadIPH(this->Handle, DC_RECV_IPH_PORT);

        g_pThreadIPH->Start();
        g_pThreadIPH->StartPlay(1, 1, 1);    */
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::InitLocalAddr()
{
    GOS_ADAPTER_INFO_T  astAdapterInfo[16];
    UINT32              ulNum = 0;
    AnsiString          strIP;

    cbLocalAddr->Properties->Items->Clear();
    if (!gos_get_adapter_info(astAdapterInfo, ARRAY_SIZE(astAdapterInfo), &ulNum))
    {
        return;
    }

    for (UINT32 i=0; i<ulNum; i++)
    {
        strIP = GetIPText(astAdapterInfo[i].aucAddr);
        cbLocalAddr->Properties->Items->Add(strIP);

        if (strIP.Pos("192.0.") > 0)
        {
            cbLocalAddr->ItemIndex = i;
        }
    }
}
//--------------------------------------------------------
void __fastcall TFormPIS::InitPARunningStatus()
{
	cbPARunningStatus->Properties->Items->Clear();

	cbPARunningStatus->Properties->Items->Add("PA不可用");
	cbPARunningStatus->ItemIndex = 0;

    cbPARunningStatus->Properties->Items->Add("PA空闲");
	cbPARunningStatus->ItemIndex = 1;

	cbPARunningStatus->Properties->Items->Add("PA广播中");
	cbPARunningStatus->ItemIndex = 2;

    return;
}

//---------------------------------------------------------------------------
void __fastcall TFormPIS::btnStartDiscoverClick(TObject *Sender)
{
    //if (g_pDiscoverThread)
    //{
    //    delete g_pDiscoverThread;
    //}

    UINT8   aucLocalAddr[4];
    UINT8   aucTrainAddr[4];

    if (!GetIP(cbLocalAddr->Text, aucLocalAddr))
    {
        ErrMsg("本地地址错误");
        return;
    }

    memset(aucTrainAddr, 0, sizeof(aucTrainAddr));
    if (!edtTrainAddr->Text.IsEmpty())
    {
        if (!GetIP(edtTrainAddr->Text, aucTrainAddr))
        {
            ErrMsg("指定车载台地址错误");
            return;
        }
    }

    //g_pDiscoverThread = new DiscoverThread(this->Handle, aucLocalAddr, aucTrainAddr);
    //g_pDiscoverThread->Start();
}

UINT32      m_ulStartRecTime = 0;


#pragma pack(push, 1)
typedef struct
{
 //   UINT8           aucExt[2];
}WAVE_FORMAT_T;

typedef struct
{
    UINT8           aucChunkID[4];      // 文件头标识，一般就是" RIFF" 四个字母
    UINT32          ulChunkSize;        // 整个数据文件的大小，不包括上面ID和Size本身    = 4 + ( 8 + SubChunk1Size ) + ( 8 + SubChunk2Size )
    UINT8           aucRiffFormat[4];   // 一般就是" WAVE" 四个字母

    UINT8           aucFormat[4];       // 格式说明块，本字段一般就是"fmt "
    UINT32          ulForamtLen;        // 本数据块的大小，不包括ID和Size字段本身 （对于PCM，值为16）
    UINT16          usFormatTag;
    UINT16          usChannels;
    UINT32          ulSamplesPerSec;
    UINT32          ulAvgBytesPerSec;
    UINT16          usBlockAlign;
    UINT16          usBitsPerSample;

//  UINT8           aucFact[4];
//  UINT32          ulBlockLen;
//    UINT32            ulBlockNum;

    UINT8           aucData[4];
    UINT32          ulDataLen;
}WAVE_FILE_HDR_T;

#pragma pack(pop)

//WAVE文件一般有四种块，它们依次是：RIFF块、格式块、附加块（可选），数据块
typedef struct
{
    unsigned char Ckid[4];  //RIFF标识
    unsigned long FileSize; //文件大小
    unsigned char FccType[4]; //WAVE标志
}RIFF_FORMAT;

//WAV音频头
typedef struct
{
    unsigned char Ckid[4]; //fmt
    unsigned long CkLen; //块大小    16
    unsigned short wFormatTag;          //音频格式一般为WAVE_FORMAT_PCM

    unsigned short nChannels;       //采样声道数
    unsigned long nSamplesPerSec;   //采样率
    unsigned long nAvgBytesPerSec;  //每秒字节数  采样率*采样精度/8(字节位数)
    unsigned short nBlockAlign;     //块大小 采样字节*声道数
    unsigned short wBitsPerSample;  //采样精度 采样精度/8 = 采样字节
}WAVE_FORMAT;

//wave数据块
typedef struct
{
    unsigned char Wdid[4]; //data 标志
    unsigned long WdSize; //块大小
    //unsigned char* Wdbuf; //数据指针 有符号
}WAVE_DATA;


void test_rec()
{
    FILE *fp = fopen("d:\\test_rec.wav", "wb+");
    UINT32  ulFrameNum = 16000*20; //m_ulPCMFrameNum

    WAVE_FILE_HDR_T     stFileHdr = {0};
    UINT32              ulOffsetChunkSize = STRUCT_OFFSET(WAVE_FILE_HDR_T, ulChunkSize);
    UINT32              ulOffsetDataLen= STRUCT_OFFSET(WAVE_FILE_HDR_T, ulDataLen);
    UINT32              ulBytesPerSample = 2;
    UINT32              ulChanNum = 2;
    UINT32              ulSamplesPerSec = 16000;

    memcpy(stFileHdr.aucChunkID, "RIFF", 4);
    stFileHdr.ulChunkSize = 36;     // 要变化
    memcpy(stFileHdr.aucRiffFormat, "WAVE", 4);

    memcpy(stFileHdr.aucFormat, "fmt ", 4);
    stFileHdr.ulForamtLen = 16;                  //块大小    16
    stFileHdr.usFormatTag = 1;              //音频格式一般为WAVE_FORMAT_PCM
    stFileHdr.usChannels = ulChanNum;               //采样声道数
    stFileHdr.ulSamplesPerSec = ulSamplesPerSec;    //采样率
    stFileHdr.ulAvgBytesPerSec = ulSamplesPerSec*ulBytesPerSample*ulChanNum;    //88200每秒字节数  采样率*采样精度（指的bit位数）/8(字节位数)
    stFileHdr.usBlockAlign = ulChanNum*ulBytesPerSample;            //块大小 = 采样字节*声道数
    stFileHdr.usBitsPerSample = ulBytesPerSample*8;

    memcpy(stFileHdr.aucData, "data", 4);
    stFileHdr.ulDataLen = 0;

#if 0
    RIFF_FORMAT     head;//RIFF块

    UINT32  ulFrameNum = 16000*10; //m_ulPCMFrameNum
    int     size = ulFrameNum*m_ulPCMFrameLen*2;

    memcpy(head.Ckid, "RIFF", 4);
    head.FileSize = size + 36;
    memcpy(head.FccType, "WAVE", 4);
    fwrite(&head, sizeof(head),1, fw); //写入head

    WAVE_FORMAT fmt;//格式块
    memcpy(fmt.Ckid, "fmt ", 4);
    fmt.CkLen = 16;                  //块大小    16
    fmt.wFormatTag = 1;             //音频格式一般为WAVE_FORMAT_PCM
    fmt.nChannels = 2;              //采样声道数
    fmt.nSamplesPerSec = 16000; //采样率
    fmt.nAvgBytesPerSec = 16000*2*fmt.nChannels;    //88200每秒字节数  采样率*采样精度（指的bit位数）/8(字节位数)
    fmt.nBlockAlign = 4;            //块大小 = 采样字节*声道数
    fmt.wBitsPerSample = 2*8;       //采样深度 = 采样字节*位数  (采样精度/8 = 采样字节)
    //fmt.cbSize = 0;
    fwrite(&fmt, sizeof(fmt),1, fw); //写入fmt

    WAVE_DATA wdata;         //数据块
    memcpy(wdata.Wdid, "data", 4);
    wdata.WdSize = size;
    fwrite(&wdata, sizeof(WAVE_DATA), 1, fw); //写入data
    fflush(fw);

    /*
    for (int i = (size-1); i>-1; i--)
    {
        fwrite((void*)(&data[i]), 1, 1, fw);//pcm数据部分为低字节在前高字节在后
    }

                                                */
    for (UINT32 i=0; i<ulFrameNum; i++)
    {
        fwrite(m_pucPCMData+i*m_ulPCMFrameLen, m_ulPCMFrameLen, 1, fw); //写入data
        fwrite(m_pucPCMData+i*m_ulPCMFrameLen, m_ulPCMFrameLen, 1, fw); //写入data
    }

    fclose(fw);
#endif

    fwrite(&stFileHdr, 1, sizeof(stFileHdr), fp);

    UINT32  ulChunkSize = 36;
    UINT32  ulDataLen = 0;

    for (UINT32 i=0; i<ulFrameNum; i++)
    {
        fseek(fp, sizeof(WAVE_FILE_HDR_T)+i*2*m_ulPCMFrameLen, SEEK_SET);

        fwrite(m_pucPCMData+i*m_ulPCMFrameLen, 1, m_ulPCMFrameLen, fp); //写入data
        fwrite(m_pucPCMData+i*m_ulPCMFrameLen, 1, m_ulPCMFrameLen, fp); //写入data

        ulDataLen += m_ulPCMFrameLen*2;
        ulChunkSize = 36 + ulDataLen;

        fseek(fp, ulOffsetChunkSize, SEEK_SET);
        fwrite(&ulChunkSize, 1, sizeof(ulChunkSize), fp); //写入data

        fseek(fp, ulOffsetDataLen, SEEK_SET);
        fwrite(&ulDataLen, 1, sizeof(ulDataLen), fp); //写入data
    }

    fclose(fp);
}


//---------------------------------------------------------------------------
void __fastcall TFormPIS::btnStartRecClick(TObject *Sender)
{
/*  test_rec();
    return;
  */

    CHAR        acFile[] = "d:\\test4.wav";

#if 0
    FILE        *fp = fopen(acFile, "rb");
    WAVE_FILE_HDR_T     stFileHdr = {0};

    fread(&stFileHdr, 1, sizeof(stFileHdr), fp);

    fclose(fp);

    return;
#endif

    if (!g_pWavRec)
    {
        StartRec();
    }

    if (!LoadPCMData2())
    {
        InfoMsg("打开文件失败");
        //edtRecTime->Text = "打开文件失败";
        return;
    }

    btnStartRec->Enabled = false;
    btnStopRec->Enabled = true;
    TimerRec->Enabled = true;

    m_ulCurrFrameIndex = 0;
    m_ulStartRecTime = gos_get_uptime_1ms();
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::TimerRecTimer(TObject *Sender)
{
    return;

    static bool bLocal = false;

    bLocal = !bLocal;

    UINT32  ulSendFrameNumPerSecond = m_ulSendFrameNumPer100MillSecond*10;

    if ((m_ulCurrFrameIndex+ulSendFrameNumPerSecond) >= m_ulPCMFrameNum/2)
    {
        m_ulCurrFrameIndex = 0;
    }

  //    m_pucPCMData2 = m_pucPCMData;

    UINT8   *pucCurrPCMData = m_pucPCMData + m_ulCurrFrameIndex*m_ulPCMFrameLen;
    UINT8   *pucCurrPCMData2 = m_pucPCMData2 + (m_ulCurrFrameIndex+m_ulPCMFrameNum/2)*m_ulPCMFrameLen;
    UINT32  ulSize = ulSendFrameNumPerSecond*m_ulPCMFrameLen;
    INT64   i64Time = gos_get_uptime_1ns();

    m_ulCurrFrameIndex += ulSendFrameNumPerSecond;
//  if (bLocal)
    {
        g_pWavRec->Rec(TRUE, pucCurrPCMData, ulSize);
    }
  //    else
    {
        g_pWavRec->Rec(FALSE, pucCurrPCMData2, ulSize);
    }

    double dTime = gos_get_uptime_1ms() - m_ulStartRecTime;

    edtRecTime->Text = DoubleText(dTime/1000, 1);
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::btnStopRecClick(TObject *Sender)
{
    g_pWavRec->StopRec();
    delete g_pWavRec;
    g_pWavRec = NULL;

    btnStartRec->Enabled = true;
    btnStopRec->Enabled = false;
    TimerRec->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::cxButton1Click(TObject *Sender)
{
    m_stApplyInfo.usTrainUnitID = 1;
    m_stApplyInfo.ucCarriageID = 1;
    m_stApplyInfo.ucIPHDevID = 1;

  //    SET_IP(m_stApplyInfo.aucAudioRecvAddr, 127,0,0,1);
  //    m_stApplyInfo.usAudioRecvPort = DC_RECV_IPH_PORT;

    GosLog(LOG_INFO, "Open Mic: %u", gos_get_uptime_1ms());

    OpenMic();

    GosLog(LOG_INFO, "start grab audio: %u", gos_get_uptime_1ms());

    StartGrabAudio();
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::cxButton2Click(TObject *Sender)
{
    CloseMic();
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::cxButton3Click(TObject *Sender)
{
    m_stApplyInfo.usTrainUnitID = 1;
    m_stApplyInfo.ucCarriageID = 1;
    m_stApplyInfo.ucIPHDevID = 1;

 // SET_IP(m_stApplyInfo.aucAudioRecvAddr, 127,0,0,1);
  //    m_stApplyInfo.usAudioRecvPort = DC_RECV_IPH_PORT;

    if (!LoadPCMData())
    {
        InfoMsg("加载音频文件失败");
        return;
    }
    TimerSendPCM->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::cxButton4Click(TObject *Sender)
{
    TimerSendPCM->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::cxButton5Click(TObject *Sender)
{
    char    *szMic = "麦克风 (Realtek Audio)";

    GetMicVolume(szMic, (UINT32*)&g_iMicVolume);
    g_iMicBoostLevel = GetMicBoostLevel(szMic);

    CloseMic();
    OpenMic();
    StartGrabAudio();

    StartRec();
}
//---------------------------------------------------------------------------

void __fastcall TFormPIS::cxButton6Click(TObject *Sender)
{
    StopRec();

    CloseMic();
    StopGrabAudio();

    LabelMicHint->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::FormClose(TObject *Sender, TCloseAction &Action)
{
    StopRec();

    if (g_ThreadPcmPlay)
    {
        g_ThreadPcmPlay->Free();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::edtWavFilePropertiesButtonClick(TObject *Sender, int AButtonIndex)
{
    if (!OpenDialog->Execute())
    {
        return;
    }

    edtWavFile->Text = OpenDialog->FileName;

    btnAnaClick(this);
}

TLineSeries     *SeriesRight = NULL;
TLineSeries     *SeriesLeft = NULL;

//---------------------------------------------------------------------------
void __fastcall TFormPIS::btnAnaClick(TObject *Sender)
{
#if 1
    if (!FileExists(edtWavFile->Text))
    {
        return;
    }

    AnsiString  strFile = edtWavFile->Text;
    FILE    *fp = fopen(strFile.c_str(), "rb");

    if (!fp)
    {
        InfoMsg("打开文件失败");
        return;
    }

    if (!SeriesRight)
    {
        SeriesLeft = new TLineSeries(cht);
        SeriesRight = new TLineSeries(cht);

        cht->AddSeries(SeriesLeft);
        cht->AddSeries(SeriesRight);
    }

    SeriesLeft->Clear();
    SeriesRight->Clear();

    WAVE_FILE_HDR_T     stFileHdr = {0};

    fread(&stFileHdr, 1, sizeof(stFileHdr), fp);

    if (stFileHdr.ulDataLen == 0)
    {
        return;
    }

    UINT32  ulSampleBytes = stFileHdr.usBitsPerSample/8;

    edtChanNum->Text = stFileHdr.usChannels;
    edtSampleBytes->Text = ulSampleBytes;

    UINT32  ulRecNum = stFileHdr.ulDataLen/(stFileHdr.usChannels*ulSampleBytes);
    double  dTime = ((double)ulRecNum)/stFileHdr.ulSamplesPerSec;

    edtWavLength->Text = DoubleText(dTime, 1);

    if (stFileHdr.usChannels == 1)
    {
        SeriesRight->Visible = false;
    }
    else
    {
        SeriesRight->Visible = true;
    }

    INT16   asValue[2];

    SeriesLeft->BeginUpdate();
    SeriesRight->BeginUpdate();
    for (UINT32 i=0; i<ulRecNum; i++)
    {
        fread(asValue, 1, stFileHdr.usChannels*ulSampleBytes, fp);

        SeriesLeft->AddXY(i, asValue[0]);
        if (stFileHdr.usChannels == 2)
        {
            SeriesRight->AddXY(i, asValue[1]);
        }
    }

    SeriesLeft->EndUpdate();
    SeriesRight->EndUpdate();

    fclose(fp);
#endif
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::btnCreateTcpSocketClick(TObject *Sender)
{
    if (!g_pPISThread)
    {
        UINT8        aucPISAddr[] = {0,0,0,0};

        g_pPISThread = new PISThread(this->Handle, aucPISAddr);
        if (!g_pPISThread->Init())
        {
            delete g_pPISThread;
            g_pPISThread = NULL;

           //   ErrMsg();
            LabelInfo->Caption = "创建TCP服务器失败";

            return;
        }

        g_pPISThread->Start();

        LabelInfo->Caption = "创建TCP服务器成功";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::btnPlayPCMClick(TObject *Sender)
{
    UINT32 ulSampleRate = 16000;
    UINT32 ulChanNum = 1;
    UINT32 ulBytesPerSample = 2;


    btnPlayPCM->Enabled = false;
    btnStopPCM->Enabled = true;

    TimerPlayPCM->Enabled = true;

    m_ulCurrFrameIndex = m_ulPCMFrameNum/3;
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::TimerPlayPCMTimer(TObject *Sender)
{
    if (!g_ThreadPcmPlay || !TimerPlayPCM->Enabled)
    {
        return;
    }

    UINT32  ulSize = m_ulSendFrameNumPer100MillSecond*m_ulPCMFrameLen;

    if (btnStopPCM->Enabled == false)
    {
        return;
        static UINT8    *pucEmptyPCMData = NULL;

        if (!pucEmptyPCMData)
        {
            pucEmptyPCMData = (UINT8*)calloc(1, ulSize);
        }

        g_ThreadPcmPlay->PushData(pucEmptyPCMData, ulSize);

        return;
    }

    if ((m_ulCurrFrameIndex+m_ulSendFrameNumPer100MillSecond) >= m_ulPCMFrameNum)
    {
        m_ulCurrFrameIndex = 0;
    }

    UINT8   *pucCurrPCMData = m_pucPCMData + m_ulCurrFrameIndex*m_ulPCMFrameLen;

    m_ulCurrFrameIndex += m_ulSendFrameNumPer100MillSecond;

    g_ThreadPcmPlay->PushData(pucCurrPCMData, ulSize);
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::btnStopPCMClick(TObject *Sender)
{
    if (g_ThreadPcmPlay)
    {
        g_ThreadPcmPlay->Stop();
    }

    TimerPlayPCM->Enabled = false;

    btnPlayPCM->Enabled = true;
    btnStopPCM->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TFormPIS::btnInitPCMClick(TObject *Sender)
{
    UINT32 ulSampleRate = 16000;
    UINT32 ulChanNum = 1;
    UINT32 ulBytesPerSample = 2;

    g_ThreadPcmPlay->Play(NULL, ulChanNum, ulSampleRate, ulBytesPerSample);

    btnPlayPCM->Enabled = true;
    btnStopPCM->Enabled = false;

    TimerPlayPCM->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::btnPausePCMClick(TObject *Sender)
{
    TimerPlayPCM->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TFormPIS::cxButton7Click(TObject *Sender)
{
    TimerPlayPCM->Enabled = true;
}
//---------------------------------------------------------------------------

//extern BOOL GetAudioInputDevice(std::vector<AnsiString> &vDevList);
extern BOOL GetAudioDev(std::vector<AnsiString> &vDev);
extern BOOL SetAudioVolume(AnsiString strDev, UINT32 ulVolume);
extern BOOL GetAudioVolume(AnsiString strDev, UINT32 &ulVolume);

void __fastcall TFormPIS::cxButton8Click(TObject *Sender)
{
    std::vector<AnsiString> vDev;

    GetAudioDev(vDev);

    mmAudioList->Text = "";
    for (UINT32 i=0; i<vDev.size(); i++)
    {
        mmAudioList->Lines->Append(vDev[i]);
    }

}
//---------------------------------------------------------------------------

void __fastcall TFormPIS::btnSetVolumeClick(TObject *Sender)
{
    int iVolume = edtAudioVolume->Text.ToIntDef(0);
    AnsiString  strDev = edtAudioDev->Text;

    SetAudioVolume(strDev, iVolume);

}
//---------------------------------------------------------------------------
extern BOOL GetMicVolume(CHAR *szMicName, unsigned int *pulVolume);
extern void MicphoneBoost(bool bIsBoost);

void __fastcall TFormPIS::btnGetVolumeClick(TObject *Sender)
{
    UINT32  ulVolume = 0;
    AnsiString  strDev = edtAudioDev->Text;

    GetAudioVolume(strDev, ulVolume);
    edtAudioVolume->Text = ulVolume;
 /*
    unsigned int ulMicVolume = 0;
    GetMicVolume(strDev, &ulMicVolume);

    MicphoneBoost(false);   */


    UINT32      ulTime = gos_get_uptime_1ms();

    edtAudioVolume->Text = GetMicBoostLevel(strDev);

    ulTime = gos_get_uptime_1ms() - ulTime;
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::btnSaveChartClick(TObject *Sender)
{
    CHAR    acFile[256];

    StrCpy(acFile, edtWavFile->Text);
    strcat(acFile, ".bmp");

    cht->SaveToBitmapFile(acFile);

//
#if 0  // 分隔文件
    FILE        *fp = fopen("c:\\error_iph.wav", "rb");

    UINT8   *pucData = NULL;
    UINT32  i;
    UINT32  ulLen;
    UINT32      ulChunkSize;
    WAVE_FILE_HDR_T stHdr = {0};
    UINT32      ulOffsetChunkSize = STRUCT_OFFSET(WAVE_FILE_HDR_T, ulChunkSize);
    UINT32      ulOffsetDataLen = STRUCT_OFFSET(WAVE_FILE_HDR_T, ulDataLen);

    fread(&stHdr, 1, sizeof(stHdr), fp);
    ulLen = stHdr.ulDataLen;
    pucData = (UINT8*)calloc(1, ulLen);
    fread(pucData, 1, ulLen, fp);
    fclose(fp);

    fp = fopen("c:\\error_iph_left.wav", "wb");

    UINT32  ulSampleBytes = stHdr.usBitsPerSample/8;
    WriteWavHdr(fp, 1, stHdr.ulSamplesPerSec, ulSampleBytes);

    UINT32  ulNum = ulLen/(2*ulSampleBytes);
    ulLen = 0;
    for (UINT32 i=0; i<ulNum; i++)
    {
        if (GET_SHORT(pucData+2*i*ulSampleBytes))
        {
            fwrite(pucData+2*i*ulSampleBytes, 1, ulSampleBytes, fp);
            ulLen += ulSampleBytes;
        }
    }

    stHdr.ulDataLen += ulLen;
    ulChunkSize = 36 + ulLen;

    fseek(fp, ulOffsetChunkSize, SEEK_SET);
    fwrite(&ulChunkSize, 1, sizeof(ulChunkSize), fp);

    fseek(fp, ulOffsetDataLen, SEEK_SET);
    fwrite(&ulLen, 1, sizeof(ulLen), fp);

    fclose(fp);

    fp = fopen("c:\\error_iph_right.wav", "wb");
    WriteWavHdr(fp, 1, stHdr.ulSamplesPerSec, ulSampleBytes);
    ulLen = 0;
    for (UINT32 i=0; i<ulNum; i++)
    {
        if (GET_SHORT(pucData+(2*i+1)*ulSampleBytes))
        {
            fwrite(pucData+(2*i+1)*ulSampleBytes, 1, ulSampleBytes, fp);
            ulLen += ulSampleBytes;
        }
    }

    stHdr.ulDataLen += ulLen;
    ulChunkSize = 36 + ulLen;

    fseek(fp, ulOffsetChunkSize, SEEK_SET);
    fwrite(&ulChunkSize, 1, sizeof(ulChunkSize), fp);

    fseek(fp, ulOffsetDataLen, SEEK_SET);
    fwrite(&ulLen, 1, sizeof(ulLen), fp);


    fclose(fp);

    return;
#endif
  //    MergeWavFile("c:\\error_iph_left.wav", "c:\\error_iph_right.wav", "c:\\ok_iph.wav", TRUE);

#if 0
    CHAR        *szFile = "c:\\ok_iph.wav";
    CHAR        *szFile1 = "c:\\error_iph_left.wav";
    CHAR        *szFile2 = "c:\\error_iph_right.wav";
    WavRec      Rec(szFile, 16000, 2, TRUE);
    FILE        *fp;
    WAVE_FILE_HDR_T stHdr = {0};
    UINT32      ulLen;
    UINT8       *pucData1;
    UINT8       *pucData2;
    UINT32      ulOffsetChunkSize = STRUCT_OFFSET(WAVE_FILE_HDR_T, ulChunkSize);
    UINT32      ulOffsetDataLen = STRUCT_OFFSET(WAVE_FILE_HDR_T, ulDataLen);

    fp = fopen(szFile1, "rb");
    fread(&stHdr, 1, sizeof(stHdr), fp);
    ulLen = stHdr.ulDataLen;
    pucData1 = (UINT8*)calloc(1, ulLen);
    fread(pucData1, 1, ulLen, fp);
    fclose(fp);
    Rec.Rec(TRUE, pucData1, ulLen);

    fp = fopen(szFile2, "rb");
    fread(&stHdr, 1, sizeof(stHdr), fp);
    ulLen = stHdr.ulDataLen;
    pucData2 = (UINT8*)calloc(1, ulLen);
    fread(pucData2, 1, ulLen, fp);
    fclose(fp);
    Rec.Rec(FALSE, pucData2, ulLen);

    Rec.StopRec();

    free(pucData1);
    free(pucData2);
#endif
}

extern void GetMicList(std::vector<CHAR *> &vMicList);
//---------------------------------------------------------------------------
void __fastcall TFormPIS::cxButton10Click(TObject *Sender)
{
    std::vector<CHAR *>     vMicList;

    GetMicList(vMicList);

    mmAudioList->Text = "";
    for (UINT32 i=0; i<vMicList.size(); i++)
    {
        mmAudioList->Lines->Append(vMicList[i]);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::TimerTestMicVolumeTimer(TObject *Sender)
{
    static int  i = 0;

    CloseMic();

 // MediaPlayer->Stop();

    if (i > 0)
    {
        g_bTestMicVolume = false;
        MediaPlayer->Stop();
        char    acLog[256];

        sprintf(acLog, "%d, " FORMAT_I64 ", %d, %.2f, " FORMAT_I64 ", %d, %.2f",
                i, g_i64Volume1, g_ulVolume1Num, ((double)g_i64Volume1)/g_ulVolume1Num,
                   g_i64Volume2, g_ulVolume2Num, ((double)g_i64Volume2)/g_ulVolume2Num);
        mmMicVolume->Lines->Append(acLog);
    }

    MediaPlayer->Open();
    MediaPlayer->Play();
    gos_sleep_1ms(2000);

    g_bTestMicVolume = true;

    g_i64Volume1 = 0;
    g_i64Volume2 = 0;
    g_ulVolume1Num = 0;
    g_ulVolume2Num = 0;

    i++;
    if (i > 100)
    {
       //   i = 1;
        g_bTestMicVolume = false;
        TimerTestMicVolume->Enabled = false;
        g_bTestMicVolume = false;
        MediaPlayer->Stop();

        return;
    }

    char    *szMic = "麦克风 (Realtek Audio)";

    SetMicVolume(szMic, i);

    OpenMic();
    StartGrabAudio();

    edtCurrMicVolume->Text = i;
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::cxButton11Click(TObject *Sender)
{
    TimerTestMicVolume->Enabled = true;
    g_bTestMicVolume = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::cxButton12Click(TObject *Sender)
{
    TimerTestMicVolume->Enabled = false;
    g_bTestMicVolume = false;
    MediaPlayer->Stop();
}
//---------------------------------------------------------------------------

void __fastcall TFormPIS::btnCreatSockClick(TObject *Sender)
{
    if (!g_pThreadTCPPA)
    {
        //UINT8        aucPISAddr[] = {0,0,0,0};

        g_pThreadTCPPA = new PATCPThread(this->Handle);
        if (!g_pThreadTCPPA->Init())
        {
            delete g_pThreadTCPPA;
            g_pThreadTCPPA = NULL;

            return;
        }

        g_pThreadTCPPA->Start();
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormPIS::btnRealBrdStartClick(TObject *Sender)
{
    PA_REAL_BRD_RSP_T    stRsp;

    stRsp.ulBrdSeqID = HTONL(g_ulBrdSeqID);//edtCurrRealBrdID->Text.ToInt();
    stRsp.usPARecvAudioPort = edtPARecvAudioPort->Text.ToIntDef(0);
    HTONS_IT(stRsp.usPARecvAudioPort);
    stRsp.ucBrdResult = 1;


    SendPATCPMsg(EV_PA_START_REAL_BRD, &stRsp, sizeof(stRsp));
}
//---------------------------------------------------------------------------

void __fastcall TFormPIS::btnBrdBusyClick(TObject *Sender)
{
    PA_REAL_BRD_RSP_T    stRsp;

    stRsp.ulBrdSeqID = HTONL(g_ulBrdSeqID);//edtCurrRealBrdID->Text.ToInt();
    stRsp.ucBrdResult = 0;

    SendPATCPMsg(EV_PA_START_REAL_BRD, &stRsp, sizeof(stRsp));
}
//---------------------------------------------------------------------------

void __fastcall TFormPIS::cbAECModePropertiesChange(TObject *Sender)
{
	if (g_bTalking)
	{
		return;
	}
	
    g_bAECMode = cbAECMode->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TFormPIS::cxButton13Click(TObject *Sender)
{
    if (g_bAECMode)
    {
        BOOL bRet = AECStart("", "", m_ulSampleFreq, m_ulPCMFrameLen);

        if (!bRet)
        {
            GosLog(LOG_ERROR, "AEC start failed");
        }

        StartRec();

        /*
        static ThreadPcmPlay    *pThreadPcmPlay = NULL;

        if (!pThreadPcmPlay)
        {
            pThreadPcmPlay = new ThreadPcmPlay();
            pThreadPcmPlay->Start();

            pThreadPcmPlay->Play(1, m_ulSampleFreq, m_ulPCMFrameLen);
        }  */

        g_bTalking = true;
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormPIS::btnPlayWavClick(TObject *Sender)
{
    AnsiString  strFile = edtWavFile->Text;

    if (FileExists(strFile))
    {
        PlaySound(strFile.c_str(), NULL, SND_FILENAME | SND_ASYNC);
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormPIS::btnOpenBmpFileClick(TObject *Sender)
{
    CHAR    acFile[256];

    StrCpy(acFile, edtWavFile->Text);
    strcat(acFile, ".bmp");

    if (!FileExists(acFile))
    {
        return;
    }

    char    acValue[1024];

    sprintf(acValue, "/n,/select,%s", acFile);
    ShellExecute(NULL, "open", "Explorer.exe", acValue, NULL, SW_SHOWDEFAULT);

}
//---------------------------------------------------------------------------


void __fastcall TFormPIS::btnIPHDevResetClick(TObject *Sender)
{
    PIS_MSG_HDR_T           *pstMsgHdr = (PIS_MSG_HDR_T*)m_aucMsgBuf;
    IPH_DEV_RESET_REQ_T     *pstReq = (IPH_DEV_RESET_REQ_T*)(pstMsgHdr+1);
    UINT32  ulMsgLen = sizeof(pstReq->ucIPHResetStatus);

    pstReq->ucIPHResetStatus = 0; ///IPH复位

    SendMsg(EV_IPH_DEV_RESET, pstReq, ulMsgLen);

    return ;
}
//---------------------------------------------------------------------------


void __fastcall TFormPIS::btnIPHTalkAckClick(TObject *Sender)
{

    IPH_TALK_APPLY_ACK_T    stIPHTalkACK;

    UINT16    usIPHRecvAudioPort = edtIPHRecvAudioPort->Text.ToIntDef(0);
    stIPHTalkACK.usTrainUnitID = edtTrainUnitID->Text.ToIntDef(0);
    stIPHTalkACK.ucCarriageID  = g_ulWaitAckCarriageID;
    stIPHTalkACK.ucIPHDevID    = g_ulWaitAckIPHDevID;
    stIPHTalkACK.usIPHDevDoorID = 0;

    if ( edtCarriageID1->Text == stIPHTalkACK.ucCarriageID &&
         edtIPHDevID1->Text == stIPHTalkACK.ucIPHDevID)
    {
        stIPHTalkACK.usIPHDevDoorID = edtIPHDoorID1->Text.ToIntDef(0);
    }
    else if( edtCarriageID2->Text == stIPHTalkACK.ucCarriageID &&
             edtIPHDevID2->Text == stIPHTalkACK.ucIPHDevID)
    {
        stIPHTalkACK.usIPHDevDoorID = edtIPHDoorID2->Text.ToIntDef(0);
    }

    stIPHTalkACK.usTrainUnitID = htons(stIPHTalkACK.usTrainUnitID);
    stIPHTalkACK.usIPHDevDoorID = htons(stIPHTalkACK.usIPHDevDoorID);

    if (g_bAECMode)
    {
        BOOL bRet = AECStart("", "", m_ulSampleFreq, m_ulPCMFrameLen);

        if (!bRet)
        {
            GosLog(LOG_ERROR, "AEC start failed");
        }

        g_bTalking = true;
    }
    else
    {
        if (!OpenMic())
        {
            InfoMsg("打开麦克风失败");
            return;
        }

        StartGrabAudio();
    }

    if (!g_pThreadIPH)
	{
		g_pThreadIPH = new ThreadIPH(this->Handle, usIPHRecvAudioPort);
	}

    g_pThreadIPH->Start();
	g_pThreadIPH->StartPlay(1, 1, 1);

    SendMsg(EV_IPH_TALK_APPLY_ACK, &stIPHTalkACK, sizeof(stIPHTalkACK));
    return ;
}
//---------------------------------------------------------------------------

