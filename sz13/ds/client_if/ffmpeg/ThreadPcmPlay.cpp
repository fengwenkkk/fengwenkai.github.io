#include "g_include.h"
#include "ThreadPcmPlay.h"
#include "ffmpeg_rec.h"

extern int SDLPlayPcm(char *szDevice, UINT32 ulSampleRate, UINT32 ulChanNum,
                      UINT32 ulBytesPerSample, BOOL *pbRunning, UINT32 *pulPlayStatus);

#define THREAD_STATE_WAIT       0
#define THREAD_STATE_RUNNING    1
#define THREAD_STATE_CLOSING    2
#define THREAD_STATE_FREE       3

#define SAMPLE_BUF_NUM      1024

static GMutex   g_Mutex;

UINT32          g_ulMaxBufLen = 1024*1024;
UINT32          g_ulCurrBufLen = 0;
static UINT8    *g_pucDataBuf = NULL;

int read_pcm_data(unsigned char *pvData, unsigned int ulLen)
{
    int     iLen = -1;

    if (!pvData || ulLen == 0)
    {
        return -1;
    }

    g_Mutex.Mutex();

    if (g_ulCurrBufLen >= ulLen)
    {
        memcpy(pvData, g_pucDataBuf, ulLen);
        g_ulCurrBufLen -= ulLen;

        if (g_ulCurrBufLen > 0)
        {
            memmove(g_pucDataBuf, g_pucDataBuf+ulLen, g_ulCurrBufLen);
        }

        iLen = ulLen;
        GosLog(LOG_DETAIL, "read data %u, curr len is %u", ulLen, g_ulCurrBufLen);
    }
   /*   else
    {
        static UINT8    *pucSilenceDataBuf = NULL;
        static UINT32   ulSilenceDataLen = 0;

        if (ulLen > ulSilenceDataLen)
        {
            GOS_FREE(pucSilenceDataBuf);
            pucSilenceDataBuf = (UINT8*)calloc(1, ulLen);
            ulSilenceDataLen = ulLen;
        }

        memcpy(pvData, pucSilenceDataBuf, ulLen);

        GosLog(LOG_INFO, "read silence data");
    } */

    g_Mutex.Unmutex();

    return iLen;
}

ThreadPcmPlay* ThreadPcmPlay::GetInstance()
{
    g_Mutex.Mutex();

    static ThreadPcmPlay*   pInstance = NULL;

    if (!pInstance)
    {
        pInstance = new ThreadPcmPlay();
        pInstance->Start();
    }

    g_Mutex.Unmutex();

    return pInstance;
}

ThreadPcmPlay::ThreadPcmPlay():GThread(NULL)
{
    m_ulThreadState = THREAD_STATE_WAIT;
    m_bPlaying = FALSE;
    m_ulStatus = SDL_PLAY_NULL;

    m_ulSampleRate = 0;
    m_ulChanNum = 0;
    m_ulBytesPerSample = 0;
    m_szDevice = NULL;
}

ThreadPcmPlay::~ThreadPcmPlay()
{
    Free();
}

VOID ThreadPcmPlay::Free()
{
    GOS_FREE(g_pucDataBuf);

    if (m_ulThreadState == THREAD_STATE_WAIT ||
        m_ulThreadState == THREAD_STATE_FREE )
    {
        return;
    }

    m_ulThreadState = THREAD_STATE_CLOSING;
    Stop();
    while(1)
    {
        if (m_ulThreadState == THREAD_STATE_FREE)
        {
            break;
        }

        gos_sleep_1ms(1);
    }
}

VOID ThreadPcmPlay::Stop()
{
    g_Mutex.Mutex();

    g_ulCurrBufLen = 0;

    g_Mutex.Unmutex();

    if (!m_bPlaying)
    {
        return;
    }

    m_bPlaying = FALSE;
    while(1)
    {
        if (m_ulStatus != SDL_PLAY_START)
        {
            break;
        }

        gos_sleep_1ms(1);
    }
}

VOID ThreadPcmPlay::Pause()
{
    g_Mutex.Mutex();

    g_ulCurrBufLen = 0;

    g_Mutex.Unmutex();

    m_bPlaying = FALSE;
}

VOID ThreadPcmPlay::Clear()
{
    g_Mutex.Mutex();

    g_ulCurrBufLen = 0;

    g_Mutex.Unmutex();
}

VOID ThreadPcmPlay::Play(CHAR *szDevice, UINT32 ulChanNum, UINT32 ulSampleRate, UINT32 ulBytesPerSample)
{
    g_Mutex.Mutex();
    if (!g_pucDataBuf)
    {
        g_pucDataBuf = (UINT8*)calloc(1, g_ulMaxBufLen);
        g_ulCurrBufLen = 0;
    }

    g_Mutex.Unmutex();

    if (m_ulChanNum != ulChanNum ||
        m_ulSampleRate != ulChanNum ||
        m_ulBytesPerSample != ulChanNum)
    {
        Stop();
    }

    m_ulMaxRunTime = 0;
    m_ulEndTime = 0xffffffff;

    m_ulSampleRate = ulSampleRate;
    m_ulChanNum = ulChanNum;
    m_ulBytesPerSample = ulBytesPerSample;

    m_bPlaying = TRUE;

    GOS_FREE(m_szDevice);
    if (!IS_NULL_STRING(szDevice))
    {
        m_szDevice = gos_ascii_to_utf8(szDevice, NULL);
    }
}

VOID ThreadPcmPlay::PushData(VOID* pvData, UINT32 ulLen)
{
    if (!m_bPlaying || !pvData || ulLen == 0)
    {
        return;
    }

    g_Mutex.Mutex();

    if ((g_ulCurrBufLen+ulLen) <= g_ulMaxBufLen)
    {
        memcpy(g_pucDataBuf+g_ulCurrBufLen, pvData, ulLen);
        g_ulCurrBufLen += ulLen;

        GosLog(LOG_DETAIL, "PCM play push data %u, curr len is %u", ulLen, g_ulCurrBufLen);
    }
    else
    {
        GosLog(LOG_ERROR, "PCM play push data error, buf is full");
    }

    g_Mutex.Unmutex();
}

GOS_THREAD_RET ThreadPcmPlay::ThreadEntry(VOID* pvPara)
{
    m_ulStatus = SDL_PLAY_NULL;

    m_ulThreadState = THREAD_STATE_RUNNING;
    while(m_ulThreadState == THREAD_STATE_RUNNING)
    {
        if (m_bPlaying)
        {
            GosLog(LOG_INFO, "ThreadPcmPlay start play");
            SDLPlayPcm(m_szDevice, m_ulSampleRate, m_ulChanNum,
                       m_ulBytesPerSample, &m_bPlaying, &m_ulStatus);
            GosLog(LOG_INFO, "ThreadPcmPlay play over");
        }

        gos_sleep_1ms(100);
    }

    m_ulThreadState = THREAD_STATE_FREE;

    return 0;
}

