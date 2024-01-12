#include "g_include.h"

#include "PIS.h"
#include "WavRec.h"
#include "ThreadIPH.h"
#include "RecThread.h"
#include "ThreadPcmPlay.h"
#include "AudioDev.h"

static ThreadPcmPlay    *g_ThreadPcmPlay = NULL;

extern UINT32           g_ulLastIPHPkgTime;

ThreadIPH::ThreadIPH(HWND hWin, UINT16 usUDPPort, WavRec *pWavRec):GThread(hWin)
{
    m_pWavRec = pWavRec;

    m_usUDPPort = usUDPPort;
    m_usTrainUnitID = 0xffff;
    m_ucCarriageID = 0xff;
    m_ucIPHDevID = 0xff;

    m_hWin = hWin;

    m_bRunning = TRUE;
    m_bPlaying = FALSE;
    m_bIsStoppingPlay = FALSE;

    m_strPlayDevice = "";//GetMainSpeaker();     采用操作系统缺省扬声器

    if (!g_ThreadPcmPlay)
    {
        g_ThreadPcmPlay = ThreadPcmPlay::GetInstance();
        g_ThreadPcmPlay->Start();

        // ThreadPcmPlay->Play 执行后，AEC功能才能正常运行
       // g_ThreadPcmPlay->Play(1, 16000, 2);
    }
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

VOID ThreadIPH::PushData(UINT8 ucChanNum, UINT16 usSampleRate, UINT8 ucSampleBits,
                         UINT8 *pucPCMData, UINT16 usPCMDataLen)
{
    if (!g_ThreadPcmPlay->IsPlaying())
    {
//            g_ThreadPcmPlay->Clear();
        g_ThreadPcmPlay->Play(m_strPlayDevice.c_str(), ucChanNum, usSampleRate, ucSampleBits/8);
    }

    g_ThreadPcmPlay->PushData(pucPCMData, usPCMDataLen);

    if (m_pWavRec)
    {
        m_pWavRec->Rec(FALSE, pucPCMData, usPCMDataLen);
    }
}

GOS_THREAD_RET ThreadIPH::ThreadEntry(void* pPara)
{
    SOCKET  stSock = gos_create_udp_server_socket(NULL, m_usUDPPort);
    INT32   iRet;
    UINT32  ulTimeout = 300;
    SOCKADDR_IN stClientAddr;
    UINT32  ulPCMDataLen;
    UINT32  ulMaxMsgLen = 64*1024;
    PIS_MSG_HDR_T   *pstPISMsgHdr = (PIS_MSG_HDR_T*)calloc(1, ulMaxMsgLen);
    IPH_PCM_DATA_T  *pstPCMData = (IPH_PCM_DATA_T*)(pstPISMsgHdr+1);
 //   UINT32          ulLastTime = 0;

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
                GosLog(LOG_ERROR, "recv iph data timeout");
                continue;
            }

            /*
            g_ulLastIPHPkgTime = gos_get_uptime();
            if ((ulLastTime + 5) < g_ulLastIPHPkgTime)
            {
                GosLog(LOG_INFO, "recving IPH data");
                ulLastTime = g_ulLastIPHPkgTime;
            }  */
        }
        else
        {
            if (g_ThreadPcmPlay)
            {
//                g_ThreadPcmPlay->Clear();
                g_ThreadPcmPlay->Stop();
            }

  /*            CLOSE_SOCKET(stSock);
            stSock = gos_create_udp_server_socket(NULL, m_usUDPPort);   */

            continue;
        }

        if ((UINT32)iRet <= sizeof(PIS_MSG_HDR_T)+STRUCT_OFFSET(IPH_PCM_DATA_T, aucPCMData))
        {
            GosLog(LOG_ERROR, "invalid IPH data len: %d", iRet);
            continue;
        }

        NTOHS_IT(pstPCMData->usTrainUnitID);
        NTOHL_IT(pstPCMData->ulPCMPkgIndex);  // PCM数据包序号  第一个数据包为0，后续递增
        NTOHL_IT(pstPCMData->ulPCMPkgTime);   // PCM数据包偏移时间  相对第一个PCM数据包的相对偏移时间，单位ms
        NTOHS_IT(pstPCMData->usSampleRate);   // 采样频率（Hz） 8000/16000/44100等
        NTOHS_IT(pstPCMData->usPCMDataLen);

        if ((UINT32)iRet != (sizeof(PIS_MSG_HDR_T) + STRUCT_OFFSET(IPH_PCM_DATA_T, aucPCMData) + pstPCMData->usPCMDataLen))
        {
            GosLog(LOG_ERROR, "invalid IPH pkg len: %d", iRet);
            continue;
        }

        if (pstPCMData->usTrainUnitID != m_usTrainUnitID ||
            pstPCMData->ucCarriageID  != m_ucCarriageID  ||
            pstPCMData->ucIPHDevID    != m_ucIPHDevID)
        {
            GosLog(LOG_ERROR, "invalid IPH data (%u, %u, %u)",
                    pstPCMData->usTrainUnitID, pstPCMData->ucCarriageID, pstPCMData->ucIPHDevID);
            continue;
        }

        g_ulLastIPHPkgTime = gos_get_uptime();

        GosLog(LOG_INFO, "recv IPH data (%u, %u, %u): index = %u time = %u len= %u",
                pstPCMData->usTrainUnitID, pstPCMData->ucCarriageID, pstPCMData->ucIPHDevID,
                pstPCMData->ulPCMPkgIndex, pstPCMData->ulPCMPkgTime, pstPCMData->usPCMDataLen);

        if (!g_ThreadPcmPlay->IsPlaying())
        {
//            g_ThreadPcmPlay->Clear();
            g_ThreadPcmPlay->Play(m_strPlayDevice.c_str(), pstPCMData->ucChanNum, pstPCMData->usSampleRate, pstPCMData->ucSampleBits/8);
        }

        g_ThreadPcmPlay->PushData(pstPCMData->aucPCMData, pstPCMData->usPCMDataLen);

        if (m_pWavRec)
        {
            m_pWavRec->Rec(FALSE, pstPCMData->aucPCMData, pstPCMData->usPCMDataLen);
        }
    }

    gos_close_socket(&stSock);

    if (g_ThreadPcmPlay)
    {
        g_ThreadPcmPlay->Free();
    }

    GosLog(LOG_INFO, "thread iph is freed");

    return 0;
}
