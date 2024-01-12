#ifndef ThreadPlayAndRecH
#define ThreadPlayAndRecH

#include "g_include.h"

class ThreadPlayAndRec :public GThread
{
public:
    ThreadPlayAndRec(HWND pPara, BOOL bTcpMode=FALSE);

    VOID Play();
    VOID PlayAndRec(CHAR *szFile);
    VOID Preview(UINT32 ulMaxRunTime);
    VOID Pause();
    VOID Resume();
    VOID Stop();
    VOID Free();

    VOID GetVideoSize(INT32 *piWidth, INT32 *piHeight);

    GOS_THREAD_RET ThreadEntry(VOID* pPara);
    VOID SetURL(CHAR *szURL);
    VOID SetOpenTimeout(UINT32 ulTimeout);
    VOID KeepPlay();
    BOOL IsPlaying(){return m_bPlaying;};
    VOID SetHandle(HANDLE  hWnd);

private:
    BOOL        m_bTcpMode;
    UINT32      m_ulThreadState;
    BOOL        m_bPlaying;
    BOOL        m_bPause;
    UINT32      m_ulStatus;
    CHAR        *m_szURL;
    CHAR        *m_szFile;
    INT32       m_iVideoWidth;
    INT32       m_iVideoHeight;
    UINT32      m_ulOpenTimeout;
    UINT32      m_ulMaxRunTime;
    UINT32      m_ulEndTime;
    HANDLE      m_hWnd;

    VOID        *m_pScreen;

    ~ThreadPlayAndRec(){};

    VOID Init();
};


#endif
