#include "g_include.h"

#include "PIS.h"
#include "ThreadIPH.h"
#include "PcmPlay.h"
#include "RecThread.h"
#include "ThreadPcmPlay.h"
#include "WavRec.h"

extern WavRec           *g_pWavRec;
extern VOID SetIPHStatus(UINT32 ulIPHStatus);

extern ThreadPcmPlay   *g_ThreadPcmPlay;

BOOL        g_bSDLPlayPcmMode = TRUE;

void FreeThreadPcmPlay()
{
    if (!g_ThreadPcmPlay)
    {
        delete g_ThreadPcmPlay;
        g_ThreadPcmPlay = NULL;
    }
}

ThreadIPH::ThreadIPH(HWND hWin, UINT16 usUDPPort):GThread(hWin)
{
    m_usUDPPort = usUDPPort;
    m_usTrainUnitID = 0xffff;
    m_ucCarriageID = 0xff;
    m_ucIPHDevID = 0xff;

    m_hWin = hWin;

    m_bRunning = TRUE;
    m_bPlaying = FALSE;
    m_hPcmPlay = NULL;
    m_bIsStoppingPlay = FALSE;
}

VOID ThreadIPH::StopPlay()
{
    m_Mutex.Mutex();

    m_usTrainUnitID = 0xffff;
    m_ucCarriageID = 0xff;
    m_ucIPHDevID = 0xff;
    m_bPlaying = FALSE;
    m_bIsStoppingPlay = TRUE;

    for (UINT32 i=0; i<10; i++)
    {
        gos_sleep_1ms(100);
        if (m_bIsStoppingPlay == FALSE)
        {
            break;
        }
    }

    m_Mutex.Unmutex();
}

VOID ThreadIPH::StartPlay(UINT16 usTrainUnitID, UINT8 ucCarriageID, UINT8 ucIPHDevID)
{
    m_Mutex.Mutex();

    m_usTrainUnitID = usTrainUnitID;
    m_ucCarriageID = ucCarriageID;
    m_ucIPHDevID = ucIPHDevID;
    m_bPlaying = TRUE;
    m_bIsStoppingPlay = FALSE;

    GosLog(LOG_INFO, "start recv IPH data: %u %u %u", usTrainUnitID, ucCarriageID, ucIPHDevID);

    m_Mutex.Unmutex();
}

VOID ThreadIPH::Stop()
{
    m_bRunning = FALSE;
    m_bIsStoppingPlay = TRUE;
    GosLog(LOG_INFO, "stop recv IPH data");
}

GOS_THREAD_RET ThreadIPH::ThreadEntry(void* pPara)
{
    SOCKET  stSock = gos_create_udp_server_socket(NULL, m_usUDPPort);
    INT32   iRet;
    UINT32  ulTimeout = 3000;
    SOCKADDR_IN stClientAddr;
    UINT32  ulPCMDataLen;
    UINT32  ulMaxMsgLen = 64*1024;
    PIS_MSG_HDR_T   *pstPISMsgHdr = (PIS_MSG_HDR_T*)calloc(1, ulMaxMsgLen);
    IPH_PCM_DATA_T  *pstPCMData = (IPH_PCM_DATA_T*)(pstPISMsgHdr+1);
    UINT32          ulLastTime = 0;

    while(m_bRunning)
    {
        if (m_bIsStoppingPlay)
        {
            m_bIsStoppingPlay = FALSE;
            GosLog(LOG_INFO, "stop recv IPH data");
        }

        iRet = gos_recv_udp_msg(stSock, pstPISMsgHdr, ulMaxMsgLen-1, ulTimeout, &stClientAddr);
        if (!m_bRunning)
        {
            break;
        }

        if (m_bPlaying)
        {
            if (iRet < 0)
            {
                GosLog(LOG_ERROR, "recv iph data failed: %s", gos_get_socket_err());

                CLOSE_SOCKET(stSock);
                stSock = gos_create_udp_server_socket(NULL, m_usUDPPort);
                continue;
            }

            if (iRet == 0)
            {
                GosLog(LOG_INFO, "recv iph data timeout");
                continue;
            }

            if ((ulLastTime + 5) < gos_get_uptime())
            {
                GosLog(LOG_INFO, "recving IPH data");
                ulLastTime = gos_get_uptime();
            }
        }
        else
        {
            if (m_hPcmPlay)
            {
                PCMPlayStop(m_hPcmPlay);
                m_hPcmPlay = NULL;
            }

            if (g_ThreadPcmPlay)
            {
                g_ThreadPcmPlay->Stop();
            }
            continue;
        }

        if ((UINT32)iRet <= sizeof(PIS_MSG_HDR_T)+STRUCT_OFFSET(IPH_PCM_DATA_T, aucPCMData))
        {
            GosLog(LOG_ERROR, "invalid pcm data len: %d", iRet);
            continue;
        }

        NTOHS_IT(pstPCMData->usTrainUnitID);
        NTOHL_IT(pstPCMData->ulPCMPkgIndex);  // PCM数据包序号  第一个数据包为0，后续递增
        NTOHL_IT(pstPCMData->ulPCMPkgTime);   // PCM数据包偏移时间  相对第一个PCM数据包的相对偏移时间，单位ms
        NTOHS_IT(pstPCMData->usSampleRate);   // 采样频率（Hz） 8000/16000/44100等
        NTOHS_IT(pstPCMData->usPCMDataLen);

        if ((UINT32)iRet != (sizeof(PIS_MSG_HDR_T) + STRUCT_OFFSET(IPH_PCM_DATA_T, aucPCMData) + pstPCMData->usPCMDataLen))
        {
            GosLog(LOG_ERROR, "invalid pcm pkg len: %d", iRet);
            continue;
        }

        if (pstPCMData->usTrainUnitID != m_usTrainUnitID ||
            pstPCMData->ucCarriageID  != m_ucCarriageID  ||
            pstPCMData->ucIPHDevID    != m_ucIPHDevID)
        {
            GosLog(LOG_ERROR, "invalid pcm data of IPH(%u, %u, %u)",
                    pstPCMData->usTrainUnitID, pstPCMData->ucCarriageID, pstPCMData->ucIPHDevID);
            continue;
        }

        GosLog(LOG_INFO, "recv IPH data: index = %u time = %u len= %u",
                pstPCMData->ulPCMPkgIndex,
                pstPCMData->ulPCMPkgTime,
                pstPCMData->usPCMDataLen);

        if (g_bSDLPlayPcmMode)
        {
            if (!g_ThreadPcmPlay->IsPlaying())
            {
                g_ThreadPcmPlay->Play(NULL, pstPCMData->ucChanNum, pstPCMData->usSampleRate, pstPCMData->ucSampleBits/8);
            }

            g_ThreadPcmPlay->PushData(pstPCMData->aucPCMData, pstPCMData->usPCMDataLen);
        }
        else
        {
            if (!m_hPcmPlay)
            {
                m_hPcmPlay = PCMPlayInit(m_hWin, pstPCMData->ucChanNum, pstPCMData->usSampleRate, pstPCMData->ucSampleBits);
                PCMPlayStart(m_hPcmPlay);
            }

            PCMPlayPushData(m_hPcmPlay, pstPCMData->aucPCMData, pstPCMData->usPCMDataLen);
        }

        if (g_pWavRec)
        {
            g_pWavRec->Rec(FALSE, pstPCMData->aucPCMData, pstPCMData->usPCMDataLen);
        }
    }

    gos_close_socket(&stSock);

    if (!m_hPcmPlay)
    {
        PCMPlayStop(m_hPcmPlay);
        m_hPcmPlay = NULL;
    }

    if (g_ThreadPcmPlay)
    {
        g_ThreadPcmPlay->Free();
    }

    GosLog(LOG_INFO, "thread iph is freed");

    return 0;
}
