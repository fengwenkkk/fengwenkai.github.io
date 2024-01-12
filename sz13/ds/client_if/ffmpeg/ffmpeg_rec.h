//---------------------------------------------------------------------------
#ifndef ffmpeg_recH
#define ffmpeg_recH
//---------------------------------------------------------------------------

#define SDL_PLAY_NULL       0
#define SDL_PLAY_START      1
#define SDL_PLAY_END        2
#define SDL_PLAY_ERROR      3
#define SDL_PLAY_BREAK      4

#define SFM_REFRESH_EVENT       (SDL_USEREVENT + 1)
#define SFM_BREAK_EVENT         (SDL_USEREVENT + 2)

typedef struct
{
    int     sdl_thread_exit;
    int     sdl_thread_pause;
    int     iThreadExited;
}SDL_THREAD_PARAM_T;

extern int SDLInit();

void* SDLCreateWindow(void *hWndPlayer);
void SDLFreeWindow(void *pScreen);
int SDLPlayAndRec(const char *szURL, const char *szFile,
                  void *pScreen, int bTcpMode, unsigned int ulOpenTimeout,
                  unsigned int ulMaxRunTime, int &bRunning, int &bPause,
                  unsigned int &ulPlayStatus, int &iVideoWidth, int &iVideoHeight);


#endif
