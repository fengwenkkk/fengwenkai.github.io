//---------------------------------------------------------------------------

#ifndef ffmpeg_apiH
#define ffmpeg_apiH
//---------------------------------------------------------------------------



int SDLInit();
int SDLPlay(char *szURL, void* hWndPlayer, int bTcpMode, int *piRunning, unsigned int *piSDLPlayStatus);

int GetMediaFileLength(char *szFile);

#endif
