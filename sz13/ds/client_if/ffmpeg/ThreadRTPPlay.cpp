#include "g_include.h"
#include "ThreadRTPPlay.h"
#include "ffmpeg_rec.h"

extern int SDLPlay(char *szURL, void* hWndPlayer, int bTcpMode, int *piRunning, unsigned int *piPlayStatus);

ThreadRTPPlay::ThreadRTPPlay(BOOL bTcpMode, HWND pPara, UINT32 *pulPlayStatus):GThread((void*)pPara)
{
    m_bTcpMode = bTcpMode;
    *pulPlayStatus = SDL_PLAY_START;
    m_pulPlayStatus = pulPlayStatus;
    m_szURL = NULL;

    SDLInit();

    m_bRunning = TRUE;
}

VOID ThreadRTPPlay::Stop()
{
    m_bRunning = FALSE;
}

VOID ThreadRTPPlay::SetURL(CHAR *szURL)
{
    if (szURL)
    {
        GOS_FREE(m_szURL);
        m_szURL = strdup(szURL);
    }
}

GOS_THREAD_RET ThreadRTPPlay::ThreadEntry(VOID* pvPara)
{
    HANDLE  hWnd = pvPara;

    SDLPlay(m_szURL, (VOID*)hWnd, m_bTcpMode, &m_bRunning, m_pulPlayStatus);

    return 0;
}


