#include "g_include.h"
#include "pis.h"
#include "InnerMsg.h"
#include "IPHThread.h"

#include "PcmPlay.h"

UINT16  g_usIPHUDPPort = IPH_RECV_DC_PORT;          // 接收PCM
UINT16  g_usDCUDPPort = DC_RECV_IPH_PORT;           // 调度台接收PCM

extern UINT32 dtp_get_fd_num(FDSET* fds);
extern SOCKET dtp_get_socket_by_fds(FDSET *fds, UINT32 ulIndex);

SOCKET      g_stIPHSocket = INVALID_SOCKET;

IPHThread::IPHThread(HWND hWnd):GThread(NULL)
{
    m_hWnd = hWnd;
    m_hPcmPlay = NULL;
}

INT32 IPHThread::ListenMsg(UINT32 ulTimeout)
{
    FDSET       fds;
    FDSET       fdsAll;
    TIMEVAL     stTimeout = {0};
    SOCKET      stClientSocket = INVALID_SOCKET;
    SOCKADDR    stAddr;
    INT32       iAddrLen = sizeof(SOCKADDR);
    INT32       iRet;
    INT32       iRecvLen;
    INT32       iErrCode = 0;
    UINT32      ulFdCount;
    UINT32      ulTotalFdCount;
    UINT32      i;
    INT32       iNoDelay = 1;
    BOOL        bNeedClose;
    SOCKET      stSocket = g_stIPHSocket;
    PIS_MSG_HDR_T   *pstMsgHdr = (PIS_MSG_HDR_T*)m_aucRecvBuf;
    IPH_PCM_DATA_T  *pstPCMData = (IPH_PCM_DATA_T*)(pstMsgHdr+1);

    FD_ZERO(&fdsAll);
    FD_SET(stSocket, &fdsAll);

again:
    fds = fdsAll;

    stTimeout.tv_sec = ulTimeout;
    iRet = select(g_stIPHSocket+1, &fds, NULL, NULL, &stTimeout);
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

    if (FD_ISSET(g_stIPHSocket, &fds))
    {
        iRet = recv(g_stIPHSocket, m_aucRecvBuf, sizeof(m_aucRecvBuf)-1, 0);
//      iRet = gos_recv_udp_msg(g_stIPHSocket, m_aucRecvBuf, sizeof(m_aucRecvBuf)-1, 2000, &stClientAddr);
        if (iRet <= 0)
        {
            goto again;
        }

        if ((UINT32)iRet <= sizeof(PIS_MSG_HDR_T)+STRUCT_OFFSET(IPH_PCM_DATA_T, aucPCMData))
        {
            GosLog(LOG_ERROR, "invalid pcm data len: %d", iRet);
            goto again;
        }

        if (pstMsgHdr->ucMsgMagic != PIS_MSG_MAGIC)
        {
            GosLog(LOG_ERROR, "invalid iph msg magic: %u", pstMsgHdr->ucMsgMagic);
            goto again;
        }

        NTOHS_IT(pstMsgHdr->usMsgType);
        NTOHS_IT(pstMsgHdr->usMsgLen);

        NTOHS_IT(pstPCMData->usTrainUnitID);
        NTOHL_IT(pstPCMData->ulPCMPkgIndex);  // PCM数据包序号  第一个数据包为0，后续递增
        NTOHL_IT(pstPCMData->ulPCMPkgTime);   // PCM数据包偏移时间  相对第一个PCM数据包的相对偏移时间，单位ms
        NTOHS_IT(pstPCMData->usSampleRate);   // 采样频率（Hz） 8000/16000/44100等
        NTOHS_IT(pstPCMData->usPCMDataLen);

        if ((UINT32)iRet != (sizeof(PIS_MSG_HDR_T) + STRUCT_OFFSET(IPH_PCM_DATA_T, aucPCMData) + pstPCMData->usPCMDataLen))
        {
            GosLog(LOG_ERROR, "invalid pcm pkg len: %d", iRet);
            goto again;
        }

        if (!m_hPcmPlay)
        {
            m_hPcmPlay = PCMPlayInit(m_hWnd, pstPCMData->ucChanNum, pstPCMData->usSampleRate, pstPCMData->ucSampleBits);
            PCMPlayStart(m_hPcmPlay);
            //SetIPHStatus(IPH_STATUS_TALK);
        }

        PCMPlayPushData(m_hPcmPlay, pstPCMData->aucPCMData, pstPCMData->usPCMDataLen);
    //        HandleRecvMsg();
    }

    goto again;
}

VOID IPHThread::HandleRecvMsg()
{
    PIS_MSG_HDR_T   *pstMsgHdr = (PIS_MSG_HDR_T*)m_aucRecvBuf;
    UINT32          ulMsgLen = sizeof(PIS_MSG_HDR_T)+pstMsgHdr->usMsgLen;
    INNER_MSG_T     *pstMsg = (INNER_MSG_T*)calloc(1, STRUCT_OFFSET(INNER_MSG_T, aucMsg)+ulMsgLen);

    if (!pstMsg)
    {
        return;
    }

    pstMsg->ulMsgType = MSG_PIS;
    pstMsg->ulMsgLen = ulMsgLen;
    memcpy(pstMsg->aucMsg, m_aucRecvBuf, pstMsg->ulMsgLen);

    if (!PostMessage(m_hWnd, WM_INNER_MSG, 0, (LPARAM)pstMsg))
    {
        free(pstMsg);
    }
}

BOOL IPHThread::Init()
{
    g_stIPHSocket = gos_create_udp_server_socket(NULL, g_usIPHUDPPort);
    if (g_stIPHSocket == INVALID_SOCKET)
    {
        return FALSE;
    }

    return TRUE;
}

GOS_THREAD_RET IPHThread::ThreadEntry(void* pPara)
{
    UINT32      ulTimeout = 2;

    while(1)
    {
        ListenMsg(ulTimeout);
    }
}
