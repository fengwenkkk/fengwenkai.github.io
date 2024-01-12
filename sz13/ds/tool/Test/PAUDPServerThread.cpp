//---------------------------------------------------------------------------

#pragma hdrstop

#include "g_include.h"
#include "pis.h"
//#include "InnerMsg.h"
#include "IPHThread.h"
#include "ds_public.h"
#include "PcmPlay.h"
#include "WavRec.h"
#include "PAUDPServerThread.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

UINT16  g_usPAUDPPort = PA_RECV_DC_PORT;            // 接收PCM

extern UINT32 dtp_get_fd_num(FDSET* fds);
extern SOCKET dtp_get_socket_by_fds(FDSET *fds, UINT32 ulIndex);

static UINT16  g_usPCMCaptureSampleRate = 16000;
static UINT8   g_ucPCMCaptureBitsPerSample = 16;
static UINT8   g_ucPCMCaptureChanNum = 1;

SOCKET      g_stPAPCMSocket = INVALID_SOCKET;

static WavRec  *g_pPAWavRec = NULL;

static void GetPAAudioRecFileName(CHAR *szFile)
{
    CHAR    acTime[32];

    gos_get_text_time_ex(NULL, "%u%02u%02u_%02u%02u%02u", acTime);

    sprintf(szFile, "%s\\%s\\%u", gos_get_root_path(), "data\\PA_rec", "PASimu");
    if (!gos_file_exist(szFile))
    {
        gos_create_redir(szFile);
    }

    // 第一级为调度台id、第二为文件， 1001_time.wav  (DC_时间）
    sprintf(szFile+strlen(szFile), "\\%u_%s.wav", "PASimu", acTime);
}

PAUDPThread::PAUDPThread(HWND hWnd):GThread(NULL)
{
    m_hWnd = hWnd;
    m_hPcmPlay = NULL;
}

BOOL PAUDPThread::Init()
{
//    UINT8   aucLocalAddr[] = {192,0,1,223};
    UINT8   aucLocalAddr[] = {0,0,0,0};

    g_stPAPCMSocket = gos_create_udp_server_socket(aucLocalAddr, g_usPAUDPPort);
    if (g_stPAPCMSocket == INVALID_SOCKET)
    {
        return FALSE;
    }

    CHAR    acRecFile[256];
    UINT32  ulPCMFrameLen = g_ucPCMCaptureBitsPerSample/8;

    GetPAAudioRecFileName(acRecFile);

    //g_pPAWavRec = new WavRec(acRecFile, g_usPCMCaptureSampleRate, ulPCMFrameLen, TRUE);
    g_pPAWavRec = new WavRec();
    g_pPAWavRec->Init(acRecFile, g_usPCMCaptureSampleRate, ulPCMFrameLen, TRUE);

    return TRUE;
}

INT32 PAUDPThread::ListenMsg(UINT32 ulTimeout)
{
    FDSET       fds;
    FDSET       fdsAll;
    TIMEVAL     stTimeout = {0};
    SOCKADDR    stAddr;
    INT32       iRet;
    INT32       iRecvLen;
    UINT32      ulFdCount;
    UINT32      ulTotalFdCount;
    UINT32      i;
    BOOL        bNeedClose;
    SOCKET      stSocket = g_stPAPCMSocket;
    PA_MSG_HDR_T    *pstMsgHdr = (PA_MSG_HDR_T*)m_aucRecvBuf;
    PA_PCM_DATA_T   *pstPCMData = (PA_PCM_DATA_T*)(pstMsgHdr+1);

    FD_ZERO(&fdsAll);
    FD_SET(stSocket, &fdsAll);

again:
    fds = fdsAll;

    stTimeout.tv_sec = ulTimeout;
    iRet = select(g_stPAPCMSocket+1, &fds, NULL, NULL, &stTimeout);
    if (iRet == 0)
    {
        goto again;
    }

    if (iRet < 0)
    {
        if (errno == EINTR)
        {
            goto again;
        }
    }

    if (FD_ISSET(g_stPAPCMSocket, &fds))
    {
        iRet = recv(g_stPAPCMSocket, m_aucRecvBuf, sizeof(m_aucRecvBuf)-1, 0);
        if (iRet <= 0)
        {
            goto again;
        }

        if ((UINT32)iRet <= sizeof(PA_MSG_HDR_T)+STRUCT_OFFSET(PA_PCM_DATA_T, aucPCMData))
        {
            GosLog(LOG_ERROR, "invalid pcm data len: %d", iRet);
            memset(m_aucRecvBuf, 0, sizeof(m_aucRecvBuf));
            goto again;
        }

        NTOHS_IT(pstMsgHdr->usMsgMagic);
        NTOHS_IT(pstMsgHdr->usMsgType);
        NTOHS_IT(pstMsgHdr->usMsgLen);

        NTOHL_IT(pstPCMData->ulPCMPkgIndex);  // PCM数据包序号  第一个数据包为0，后续递增
        NTOHS_IT(pstPCMData->usSampleRate);   // 采样频率（Hz） 8000/16000/44100等
        NTOHS_IT(pstPCMData->usPCMDataLen);

        if (pstMsgHdr->usMsgMagic != PIS_MSG_MAGIC)
        {
            GosLog(LOG_ERROR, "invalid PA PCM msg magic: %u", pstMsgHdr->usMsgMagic);
            memset(m_aucRecvBuf, 0, sizeof(m_aucRecvBuf));
            goto again;
        }

        //if ((UINT32)iRet != (sizeof(PA_MSG_HDR_T) + STRUCT_OFFSET(PA_PCM_DATA_T, aucPCMData) + pstPCMData->usPCMDataLen))
        //{
            //GosLog(LOG_ERROR, "invalid pcm pkg len: %d", iRet);
            //goto again;
        //}

        if (!m_hPcmPlay)
        {
            //m_hPcmPlay = PCMPlayInit(m_hWnd, pstPCMData->ucChanNum, pstPCMData->usSampleRate, pstPCMData->ucSampleBits);
            m_hPcmPlay = PCMPlayInit(m_hWnd, g_ucPCMCaptureChanNum, g_usPCMCaptureSampleRate, g_ucPCMCaptureBitsPerSample);
            PCMPlayStart(m_hPcmPlay);
        }

    //    UINT16 usCRC = gos_ansi_crc16(pstPCMData->aucPCMData, pstPCMData->usPCMDataLen );

    //    GosLog(LOG_ERROR, "tttttttt ID is %u data CRC is : %u tttttt",pstPCMData->ulPCMPkgIndex, usCRC);

        if (g_pPAWavRec)
        {
            g_pPAWavRec->Rec(TRUE, pstPCMData->aucPCMData, pstPCMData->usPCMDataLen);
        }

        PCMPlayPushData(m_hPcmPlay, pstPCMData->aucPCMData, pstPCMData->usPCMDataLen);

        memset(m_aucRecvBuf, 0, sizeof(m_aucRecvBuf));
    }

    goto again;
}

GOS_THREAD_RET PAUDPThread::ThreadEntry(void* pPara)
{
    UINT32      ulTimeout = 2;

    while(1)
    {
        ListenMsg(ulTimeout);
    }
}
