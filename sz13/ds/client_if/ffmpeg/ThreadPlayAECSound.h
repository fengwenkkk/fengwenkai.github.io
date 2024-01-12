#ifndef ThreadPlayAECSoundH
#define ThreadPlayAECSoundH

#include "g_include.h"

class ThreadPlayAECSound :public GThread
{
public:
    ThreadPlayAECSound(HWND hWnd);
    VOID Stop();

    GOS_THREAD_RET ThreadEntry(VOID* pPara);

private:
    UINT32      m_ulThreadState;
    BOOL        m_bPlaying;
    HWND        m_hWnd;

    ~ThreadPlayAECSound();
};

#endif
