#include "g_include.h"
#include "ThreadPlayAndRec.h"
#include "ffmpeg_rec.h"

#define THREAD_STATE_WAIT       0
#define THREAD_STATE_RUNNING    THREAD_STATUS_RUN
#define THREAD_STATE_CLOSING    2
#define THREAD_STATE_FREE       3

ThreadPlayAndRec::ThreadPlayAndRec(HWND pPara, BOOL bTcpMode):GThread((void*)pPara)
{
    m_bTcpMode = bTcpMode;
    m_szURL = NULL;
    m_szFile = NULL;
    m_ulStatus = SDL_PLAY_NULL;
    m_ulOpenTimeout = 1;

    m_iVideoWidth = -1;
    m_iVideoHeight = -1;

    m_pScreen = NULL;

    m_ulThreadState = THREAD_STATE_WAIT;
    m_bPlaying = FALSE;
    m_bPause = FALSE;

    m_hWnd = pPara;

    SDLInit();
}

VOID ThreadPlayAndRec::Init()
{
    SDLFreeWindow(m_pScreen);

    ::ShowWindow((HWND)m_hWnd, SW_SHOW);
    m_pScreen = SDLCreateWindow(m_hWnd);
}

VOID ThreadPlayAndRec::Free()
{
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

VOID ThreadPlayAndRec::Pause()
{
    m_bPlaying = TRUE;
    m_bPause = TRUE;
}

VOID ThreadPlayAndRec::Resume()
{
    m_bPlaying = TRUE;
    m_bPause = FALSE;
}

VOID ThreadPlayAndRec::Stop()
{
    m_iVideoWidth = -1;
    m_iVideoHeight = -1;

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

VOID ThreadPlayAndRec::SetURL(CHAR *szURL)
{
    if (szURL)
    {
        GOS_FREE(m_szURL);
        m_szURL = strdup(szURL);
    }

    m_iVideoWidth = -1;
    m_iVideoHeight = -1;
}

VOID ThreadPlayAndRec::SetOpenTimeout(UINT32 ulTimeout)
{
    m_ulOpenTimeout = ulTimeout;
}

VOID ThreadPlayAndRec::Play()
{
    PlayAndRec(NULL);
}

VOID ThreadPlayAndRec::PlayAndRec(CHAR *szFile)
{
    Stop();

    m_ulMaxRunTime = 0;
    m_ulEndTime = 0xffffffff;
    GOS_FREE(m_szFile);
    if (szFile && *szFile != '\0')
    {
        m_szFile = strdup(szFile);
    }

    Init();

    m_bPlaying = TRUE;
    m_bPause = FALSE;
}

VOID ThreadPlayAndRec::Preview(UINT32 ulMaxRunTime)
{
    Stop();

    m_ulMaxRunTime = ulMaxRunTime;
    m_ulEndTime = gos_get_uptime() + ulMaxRunTime;
    GOS_FREE(m_szFile);

    Init();

    m_bPlaying = TRUE;
    m_bPause = FALSE;
}

VOID ThreadPlayAndRec::KeepPlay()
{
    m_bPlaying = TRUE;
}

VOID ThreadPlayAndRec::SetHandle(HANDLE hWnd)
{
    m_hWnd = hWnd;
}

GOS_THREAD_RET ThreadPlayAndRec::ThreadEntry(VOID* pvPara)
{
    UINT32      ulCount = 0;

    m_ulStatus = SDL_PLAY_NULL;

    m_hWnd = pvPara;

    m_ulThreadState = THREAD_STATE_RUNNING;
    while(m_ulThreadState == THREAD_STATE_RUNNING)
    {
        if (MOD(ulCount, 1000) == 0)
        {
            GosLog(LOG_INFO, "ThreadPlayAndRec status: %u %u", m_ulStatus, m_bPlaying);
        }

        ulCount++;

        if (m_bPlaying &&
            (m_ulMaxRunTime == 0 || gos_get_uptime() < m_ulEndTime) )
        {
            GosLog(LOG_INFO, "ThreadPlayAndRec start work");
            SDLPlayAndRec(m_szURL, m_szFile, m_pScreen, m_bTcpMode,
                          m_ulOpenTimeout, m_ulMaxRunTime, m_bPlaying, m_bPause,
                          m_ulStatus, m_iVideoWidth, m_iVideoHeight);
            gos_sleep_1ms(200);
            GosLog(LOG_INFO, "ThreadPlayAndRec status: %u", m_ulStatus);
        }

        if (m_ulThreadState != THREAD_STATE_RUNNING)
        {
            break;
        }

        gos_sleep_1ms(10);
    }

    m_ulThreadState = THREAD_STATE_FREE;

    GosLog(LOG_FATAL, "ThreadPlayAndRec is freed");

    return 0;
}

VOID ThreadPlayAndRec::GetVideoSize(INT32 *piWidth, INT32 *piHeight)
{
    *piWidth = m_iVideoWidth;
    *piHeight = m_iVideoHeight;
}
