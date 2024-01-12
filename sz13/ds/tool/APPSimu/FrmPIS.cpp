//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"
#include "MsgDialog.h"
#include "ClientUtil.h"
#include "ATS.h"
#include "pis.h"
#include "FrmPIS.h"
#include "PISThread.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxGraphics"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
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
#pragma link "cxPCdxBarPopupMenu"
#pragma link "dxSkinscxPCPainter"
#pragma link "cxGroupBox"
#pragma resource "*.dfm"

TFormPIS *FormPIS;

extern PISThread *g_pPISThread;
extern SOCKET g_stSASocket;


UINT32  m_ulSampleFreq = 16000;
UINT32  m_ulPCMFrameLen = 2;
UINT32  m_ulSendFrameNumPer100MillSecond = 100*m_ulSampleFreq/1000;
UINT8   *m_pucPCMData = NULL;
UINT32  m_ulCurrFrameIndex = 0;
UINT32  m_ulPCMFrameNum = 0;
UINT32  m_ulWavFileHdrLen = 78;
IPH_TALK_APPLY_RSP_INFO_T   m_stApplyInfo = {0};

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

//---------------------------------------------------------------------------
__fastcall TFormPIS::TFormPIS(TComponent* Owner)
    : TSubForm(Owner)
{
    if (!LoadPCMData())
    {
        ErrMsg("加载音频文件失败");
        exit(0);
    }

    InitInnerMsgHandler(MSG_PIS, OnPISMsg);
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::btnIPHApplyClick(TObject *Sender)
{
    PIS_MSG_HDR_T           *pstMsgHdr = (PIS_MSG_HDR_T*)m_aucMsgBuf;
    IPH_TALK_APPLY_REQ_T    *pstReq = (IPH_TALK_APPLY_REQ_T*)(pstMsgHdr+1);
    UINT32  ulMsgLen = 0;

    pstReq->ucNum = 2;
    pstReq->astApplyReq[0].usTrainUnitID = edtTrainUnitID1->Text.ToIntDef(0);
    pstReq->astApplyReq[0].ucCarriageID =  edtCarriageID1->Text.ToIntDef(0);
    pstReq->astApplyReq[0].ucIPHDevID = edtIPHDevID1->Text.ToIntDef(0);
    sprintf(pstReq->astApplyReq[0].acVideoUrl, "123");

    pstReq->astApplyReq[1].usTrainUnitID = edtTrainUnitID2->Text.ToIntDef(0);
    pstReq->astApplyReq[1].ucCarriageID = edtCarriageID2->Text.ToIntDef(0);
    pstReq->astApplyReq[1].ucIPHDevID = edtIPHDevID2->Text.ToIntDef(0);
    sprintf(pstReq->astApplyReq[1].acVideoUrl, "234");

    if (pstReq->astApplyReq[1].usTrainUnitID == 0)
    {
        pstReq->ucNum = 1;
    }

    for (UINT32 i=0; i<pstReq->ucNum; i++)
    {
        HTONS_IT(pstReq->astApplyReq[i].usTrainUnitID);
    }

    ulMsgLen = STRUCT_OFFSET(IPH_TALK_APPLY_REQ_T, astApplyReq) + pstReq->ucNum*sizeof(pstReq->astApplyReq[0]);
    pstMsgHdr->ucMsgMagic = PIS_MSG_MAGIC;
    pstMsgHdr->usMsgType = HTONS(EV_IPH_TALK_APPLY);
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
}

//---------------------------------------------------------------------------
void __fastcall TFormPIS::OnIPHTalkApplyRsp(IPH_TALK_APPLY_RSP_T *pstMsg)
{
    for (UINT32 i=0; i<pstMsg->ucNum; i++)
    {
        IPH_TALK_APPLY_RSP_INFO_T   *pstApplyInfo = &pstMsg->astApplyRsp[i];

        NTOHS_IT(pstApplyInfo->usTrainUnitID);
        NTOHS_IT(pstApplyInfo->usAudioRecvPort);

        if (edtTrainUnitID1->Text == pstApplyInfo->usTrainUnitID &&
            edtCarriageID1->Text == pstApplyInfo->ucCarriageID &&
            edtIPHDevID1->Text == pstApplyInfo->ucIPHDevID)
        {
            btnIPHhandup1->Enabled = true;
            edtDCAddr1->Text = GetTextIP(pstApplyInfo->aucAudioRecvAddr)+":" + AnsiString(pstApplyInfo->usAudioRecvPort);
        }
        else if (edtTrainUnitID2->Text == pstApplyInfo->usTrainUnitID &&
                 edtCarriageID2->Text == pstApplyInfo->ucCarriageID &&
                 edtIPHDevID2->Text == pstApplyInfo->ucIPHDevID)
        {
            btnIPHhandup2->Enabled = true;
            edtDCAddr1->Text = GetTextIP(pstApplyInfo->aucAudioRecvAddr)+":" + AnsiString(pstApplyInfo->usAudioRecvPort);
        }

        memcpy(&m_stApplyInfo, pstApplyInfo, sizeof(m_stApplyInfo));
        TimerSendPCM->Enabled = true;
        break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::OnIPHAudioDownload(IPH_PCM_DATA_T *pstMsg)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::OnIPHTalkStopRsp(IPH_TALK_STOP_RSP_T *pstMsg)
{

}

//---------------------------------------------------------------------------
void __fastcall TFormPIS::OnPISMsg(UINT8 *pucMsg, UINT32 ulMsgLen)
{
    PIS_MSG_HDR_T   *pstMsg = (PIS_MSG_HDR_T*)pucMsg;

    if (pstMsg->usMsgType == EV_IPH_TALK_APPLY)
    {
        OnIPHTalkApplyRsp((IPH_TALK_APPLY_RSP_T*)(pstMsg+1));
    }
    else if (pstMsg->usMsgType == EV_IPH_AUDIO_DOWNLOAD)        // 调度台音频下发
    {
        OnIPHAudioDownload((IPH_PCM_DATA_T*)(pstMsg+1));
    }
    else if (pstMsg->usMsgType == EV_IPH_STOP_TALK )            // 调度台结束对讲
    {
        OnIPHTalkStopRsp((IPH_TALK_STOP_RSP_T*)(pstMsg+1));
    }
}
void __fastcall TFormPIS::btnIPHhandup1Click(TObject *Sender)
{
    btnIPHhandup1->Enabled = false;
    TimerSendPCM->Enabled = false;
    m_ulCurrFrameIndex = 0;

    PIS_MSG_HDR_T           *pstMsgHdr = (PIS_MSG_HDR_T*)m_aucMsgBuf;
    IPH_TALK_STOP_REQ_T     *pstReq = (IPH_TALK_STOP_REQ_T*)(pstMsgHdr+1);
    UINT32                  ulMsgLen = sizeof(IPH_TALK_STOP_REQ_T);

    pstReq->usTrainUnitID = edtTrainUnitID1->Text.ToIntDef(0);
    pstReq->ucCarriageID =  edtCarriageID1->Text.ToIntDef(0);
    pstReq->ucIPHDevID = edtIPHDevID1->Text.ToIntDef(0);

    pstMsgHdr->ucMsgMagic = PIS_MSG_MAGIC;
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
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::btnIPHhandup2Click(TObject *Sender)
{
    btnIPHhandup2->Enabled = false;
    TimerSendPCM->Enabled = false;
    m_ulCurrFrameIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormPIS::TimerSendPCMTimer(TObject *Sender)
{
    SendPCMData();
}

BOOL UDPSend(VOID *pvMsg, UINT32 ulMsgLen)
{
    SOCKET          stSock = gos_create_udp_socket();
    SOCKADDR_IN     stServerAddr = {0};
    UINT32          AF_INET = 2;

    stServerAddr.sin_family = AF_INET;
    stServerAddr.sin_port   = HTONS(m_stApplyInfo.usAudioRecvPort);
    memcpy(&stServerAddr.sin_addr.s_addr, m_stApplyInfo.aucAudioRecvAddr, 4);

    return gos_udp_send(stSock, pvMsg, ulMsgLen, &stServerAddr);
}

//---------------------------------------------------------------------------
void __fastcall TFormPIS::SendPCMData()
{
    if ((m_ulCurrFrameIndex+m_ulSendFrameNumPer100MillSecond) >= m_ulPCMFrameNum)
    {
        m_ulCurrFrameIndex = 0;
        //TimerSendPCM->Enabled = false;
        //return;
    }

    UINT8   *pucCurrPCMData = m_pucPCMData + m_ulCurrFrameIndex*m_ulPCMFrameLen;
    UINT32  ulSize = m_ulSendFrameNumPer100MillSecond*m_ulPCMFrameLen;
    UINT32          ulMsgLen = sizeof(PIS_MSG_HDR_T) + STRUCT_OFFSET(IPH_PCM_DATA_T, aucPCMData) + ulSize;
    PIS_MSG_HDR_T   *pstMsgHdr = (PIS_MSG_HDR_T*)malloc(ulMsgLen);
    IPH_PCM_DATA_T  *pstPCMData = (IPH_PCM_DATA_T*)(pstMsgHdr+1);
    UINT32          ulCurrTime = gos_get_uptime_1ms();

    if (!pstMsgHdr)
    {
        return;
    }

    pstMsgHdr->ucMsgMagic = PIS_MSG_MAGIC;
    pstMsgHdr->usMsgType = HTONS(EV_IPH_AUDIO_UPLOAD);
    pstMsgHdr->usMsgLen = HTONS(ulSize);

    pstPCMData->usTrainUnitID = HTONS(m_stApplyInfo.usTrainUnitID);   // 车组号（车体号）
    pstPCMData->ucCarriageID = m_stApplyInfo.ucCarriageID;   // 车厢号
    pstPCMData->ucIPHDevID = m_stApplyInfo.ucIPHDevID;     // 报警器ID

    pstPCMData->ulPCMPkgIndex = HTONL(m_ulCurrFrameIndex);  // PCM数据包序号，第一个数据包为0，后续递增
    pstPCMData->ulPCMPkgTime = HTONL(ulCurrTime);   // PCM数据包偏移时间，相对第一个PCM数据包的相对偏移时间，单位ms
    pstPCMData->usSampleRate = HTONS(m_ulSampleFreq);   // 采样频率（Hz）   8000/16000/44100等
    pstPCMData->ucChanNum = 1;      // 采样声道数   1：单声道  2：双声道
    pstPCMData->ucSampleBits = 8*m_ulPCMFrameLen;   // 采样量化位数 8：8位（1字节） 16：16位（2字节）
    pstPCMData->usPCMDataLen = HTONS(ulSize);   // 数据长度(=usSampleRate*ucChanNum*ucSampleBits/8)
    memcpy(pstPCMData->aucPCMData, pucCurrPCMData, ulSize);

    if (UDPSend(pstMsgHdr, ulMsgLen))
    {
        m_ulCurrFrameIndex += m_ulSendFrameNumPer100MillSecond;
    }
}
//---------------------------------------------------------------------------





void __fastcall TFormPIS::TimerTimer(TObject *Sender)
{
    TimerSendPCM->Enabled = false;
}
//---------------------------------------------------------------------------


