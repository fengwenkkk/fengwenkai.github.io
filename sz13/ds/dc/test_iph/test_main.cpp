//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"

#include "pis.h"
#include "ThreadIPH.h"
#include "RecThread.h"
#include "test_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

UINT16  m_usUDPPort = 12345;
UINT8   m_aucUDPAddr[4] = {127,0,0,1};

UINT32  m_ulSampleFreq = 16000;
UINT32  m_ulPCMFrameLen = 2;
UINT32  m_ulSendSizePerSec = m_ulSampleFreq*m_ulPCMFrameLen;
UINT8   *m_pucPCMData = NULL;
UINT32  m_ulPCMDataPos = 0;
UINT32  m_ulPCMDataLen = 0;

UINT32  m_ulWavFileHdrLen = 78;

#define STAT_NUM    11
UINT32  m_aulSendTime[STAT_NUM];

ThreadIPH   *g_ThreadIPH = NULL;
RecThread   *g_RecThread = NULL;

BOOL LoadPCMData()
{
    CHAR    acFile[256];
    FILE    *fp;
    INT32   iSize;

    sprintf(acFile, "%s/%s", gos_get_root_path(), "test1.wav");
    iSize = gos_get_file_size(acFile);
    if (iSize <= m_ulWavFileHdrLen)
    {
        return FALSE;
    }

    iSize -= m_ulWavFileHdrLen;
    m_ulPCMDataLen = iSize;
    m_pucPCMData = (UINT8*)calloc(1, iSize);
    if (!m_pucPCMData)
    {
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

BOOL UDPSend(VOID *pvMsg, UINT32 ulMsgLen)
{
    SOCKET          stSock = gos_create_udp_socket();
    SOCKADDR_IN     stServerAddr = {0};

    stServerAddr.sin_family = AF_INET;
    stServerAddr.sin_port   = HTONS(m_usUDPPort);
    memcpy(&stServerAddr.sin_addr.s_addr, m_aucUDPAddr, 4);

    return gos_udp_send(stSock, pvMsg, ulMsgLen, &stServerAddr);
}

//---------------------------------------------------------------------------
void __fastcall TForm1::SendPCMData()
{
    UINT8   *pucCurrPCMData = m_pucPCMData + m_ulPCMDataPos;
    UINT32  ulPCMSize = 1.3*m_ulSendSizePerSec*TimerSend->Interval/1000;

    if ((m_ulPCMDataPos+ulPCMSize) >= m_ulPCMDataLen)
    {
        m_ulPCMDataPos = 0;
    }

    UINT32          ulMsgLen = sizeof(PIS_MSG_HDR_T) + STRUCT_OFFSET(IPH_PCM_DATA_T, aucPCMData) + ulPCMSize;
    PIS_MSG_HDR_T   *pstMsgHdr = (PIS_MSG_HDR_T*)malloc(ulMsgLen);
    IPH_PCM_DATA_T  *pstPCMData = (IPH_PCM_DATA_T*)(pstMsgHdr+1);
    UINT32          ulCurrTime = gos_get_uptime_1ms();

    if (!pstMsgHdr)
    {
        return;
    }

    pstMsgHdr->ucMsgMagic = PIS_MSG_MAGIC;
    pstMsgHdr->usMsgType = HTONS(EV_IPH_AUDIO_UPLOAD);
    pstMsgHdr->usMsgLen = HTONS(STRUCT_OFFSET(IPH_PCM_DATA_T, aucPCMData) + ulPCMSize);

    pstPCMData->usTrainUnitID = HTONS(1);   // 车组号（车体号）
    pstPCMData->ucCarriageID = 1;   // 车厢号
    pstPCMData->ucIPHDevID = 1;     // 报警器ID

    pstPCMData->ulPCMPkgIndex = HTONL(0);  // PCM数据包序号，第一个数据包为0，后续递增
    pstPCMData->ulPCMPkgTime = HTONL(ulCurrTime);   // PCM数据包偏移时间，相对第一个PCM数据包的相对偏移时间，单位ms
    pstPCMData->usSampleRate = HTONS(m_ulSampleFreq);   // 采样频率（Hz）   8000/16000/44100等
    pstPCMData->ucChanNum = 1;      // 采样声道数   1：单声道  2：双声道
    pstPCMData->ucSampleBits = 8*m_ulPCMFrameLen;   // 采样量化位数 8：8位（1字节） 16：16位（2字节）
    pstPCMData->usPCMDataLen = HTONS(ulPCMSize);   // 数据长度(=usSampleRate*ucChanNum*ucSampleBits/8)
    memcpy(pstPCMData->aucPCMData, pucCurrPCMData, ulPCMSize);

    if (UDPSend(pstMsgHdr, ulMsgLen))
    {
        m_ulPCMDataPos += ulPCMSize;
    }
}

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
    GosInitLog();
    LoadPCMData();

    g_ThreadIPH = new ThreadIPH(this->Handle, m_usUDPPort);
    g_ThreadIPH->Start();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnSendClick(TObject *Sender)
{
    btnSend->Enabled = false;
    btnStop->Enabled = true;

    TimerSend->Enabled = true;

    g_ThreadIPH->StartPlay(1,1,1);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnStopClick(TObject *Sender)
{
    btnSend->Enabled = true;
    btnStop->Enabled = false;

    TimerSend->Enabled = false;

    g_ThreadIPH->StopPlay();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::TimerSendTimer(TObject *Sender)
{
    SendPCMData();
}
//---------------------------------------------------------------------------

