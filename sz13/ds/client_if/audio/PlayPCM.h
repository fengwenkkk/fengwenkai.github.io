#ifndef PlayPCMH
#define PlayPCMH

#if 1
HANDLE PCMPlayInit(UINT32 ulChanNum, UINT32 ulSampleRate, UINT32 ulBitsPerSample);
VOID PCMPlayStart(HANDLE hPcmPlay);
VOID PCMPlayPushData(HANDLE hPcmPlay, VOID *pvData, UINT32 ulLen);
VOID PCMPlayStop(HANDLE hPcmPlay);
#endif

#endif
