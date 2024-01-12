#pragma once

#include "g_include.h"

HANDLE PCMPlayInit(HWND hWnd, UINT32 ulChanNum, UINT32 ulSampleRate, UINT32 ulBitsPerSample);
VOID PCMPlayStart(HANDLE hPcmPlay);
VOID PCMPlayPushData(HANDLE hPcmPlay, VOID *pvData, UINT32 ulLen);
VOID PCMPlayStop(HANDLE hPcmPlay);

