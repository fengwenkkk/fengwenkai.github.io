#ifndef ThreadPcmPlayH
#define ThreadPcmPlayH

#include "g_include.h"

class ThreadPcmPlay :public GThread
{
public:
    ThreadPcmPlay();
    ~ThreadPcmPlay();

    static ThreadPcmPlay* GetInstance();
    VOID Play(CHAR *szDevice, UINT32 ulChanNum, UINT32 ulSampleRate, UINT32 ulBytesPerSample);
    VOID Stop();
    VOID Pause();
    VOID Clear();   // Çå³þ»º³åÆ÷
    VOID Free();
    VOID PushData(VOID* pvData, UINT32 ulLen);
    BOOL IsPlaying(){return m_bPlaying;};

    GOS_THREAD_RET ThreadEntry(VOID* pPara);

private:
    UINT32      m_ulThreadState;
    BOOL        m_bPlaying;
    UINT32      m_ulStatus;

    UINT32      m_ulMaxRunTime;
    UINT32      m_ulEndTime;

    UINT32      m_ulSampleRate;
    UINT32      m_ulChanNum;
    UINT32      m_ulBytesPerSample;

    CHAR        *m_szDevice;       // UTF8 ±àÂë
};

extern int SDLGetAudioDeviceNum(BOOL bIsCapture);
extern const char * SDLGetAudioDeviceName(BOOL bIsCapture, UINT32 iIndex);

#endif
