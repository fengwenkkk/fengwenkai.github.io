#include "g_include.h"
#include "ThreadPlayAECSound.h"
#include "PcmPlay.h"

// 本线程循环播放几乎为0音频流，
// 由于AEC功能要求扬声器必须要有声音输出，否则无法采集出数据

ThreadPlayAECSound::ThreadPlayAECSound(HWND hWnd):GThread(NULL)
{
    m_ulThreadState = THREAD_STATUS_NULL;
    m_hWnd = hWnd;
    m_bPlaying = FALSE;

    Start();
}

ThreadPlayAECSound::~ThreadPlayAECSound()
{

}

VOID ThreadPlayAECSound::Stop()
{
    m_bPlaying = false;

    while (m_ulThreadState == THREAD_STATUS_RUN)
    {
        gos_sleep_1ms(10);
    }
}

GOS_THREAD_RET ThreadPlayAECSound::ThreadEntry(VOID* pvPara)
{
    UINT32 ulChanNum = 1;
    UINT32 ulSampleRate = 16000;
    UINT32 ulSampleByte = 2;

    HANDLE hPlay = PCMPlayInit(m_hWnd, ulChanNum, ulSampleRate, ulSampleByte*8);

    if (!hPlay)
    {
        return 0;
    }

    m_ulThreadState = THREAD_STATUS_RUN;
    m_bPlaying = TRUE;

    UINT32  ulInterval = 500;
    UINT32  ulBufLen = (ulChanNum*ulSampleRate*ulSampleByte)*ulInterval/1000;
    static short  asData[16000];

    for (UINT32 i=0; i<ARRAY_SIZE(asData); i++)
    {
        if (MOD(i, 20) < 10)
        {
            asData[i] = -1;
        }
        else
        {
            asData[i] = 1;
        }
    }

    PCMPlayStart(hPlay);

    while (m_bPlaying)
    {
        PCMPlayPushData(hPlay, asData, ulBufLen);

        gos_sleep_1ms(ulInterval*0.9);    // 打个折扣
    }

    PCMPlayStop(hPlay);

    m_ulThreadState = THREAD_STATUS_NULL;

    return 0;
}

