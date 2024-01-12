#pragma once

//#include "AecCaptureInterface.h"
#include <vector>

#define API_CALL    __stdcall

#ifndef DLL_API
#define DLL_API __declspec(dllimport)
#endif

#define EXTERN_API  extern "C"

#define MAX_AEC_DEV_NUM     16

typedef struct  
{
    UINT32      ulNum;
    CHAR        aacName[MAX_AEC_DEV_NUM][260];
}AEC_DEV_T;

typedef VOID (*AEC_ON_CAPTURE_DATA)(unsigned char* data, UINT32 size, UINT32 ulSampleRate, UINT32 ulBitsPerSample);

EXTERN_API DLL_API BOOL API_CALL AECInit();
EXTERN_API DLL_API BOOL API_CALL AECEnumDevice(AEC_DEV_T *pstMic, AEC_DEV_T *pstSpeaker);
EXTERN_API DLL_API VOID API_CALL AECSetOnCapturedDataCB(AEC_ON_CAPTURE_DATA fOnCaptureData);
EXTERN_API DLL_API BOOL API_CALL AECStart(char *szCapture, char *szRender, UINT32 ulSampleRate, UINT32 ulBitsPerSample);
EXTERN_API DLL_API VOID API_CALL AECStop();



