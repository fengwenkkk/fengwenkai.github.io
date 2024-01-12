#define DLL_API __declspec(dllexport)

#include "AecMicArrayCapture.h"
#include "WaveFile.h"
#include "AecCaptureInterface.h"
#include "AEC.h"

#include <vector>

#define CAPTURE     ((CAecMicArrayCapture*)m_Capture)


AEC_ON_CAPTURE_DATA     g_fOnCaptureData = NULL;

class AECCtrl : public IAecCaptureEvent
{
// 构造
public:
	AECCtrl();

// 实现
protected:
	CAecMicArrayCapture m_Capture;
	//BYTE*				m_CapturedData;
    
    long                m_ulSampleRate;
 //   long                m_ulBufferSize;
    long                m_ulBitsPerSample;

    char                *m_szCapture;
    char                *m_szRender;

  //  CWaveFile			wavFile;
  //  std::vector<char*>  m_vRenderDev;
  //  std::vector<char*>  m_vCaptureDev;

public:
    BOOL __stdcall SetParam(char *szCapture, char *szRender, long ulSampleRate, long ulBitsPerSample);
    BOOL __stdcall Start();
    VOID __stdcall Stop();
    VOID __stdcall EnumDevice(std::vector<std::string> &vMic, std::vector<std::string> &vSpeaker);
   
    virtual long __stdcall OnCapturedData(unsigned char* data,unsigned long size,unsigned long samplerate,unsigned long bitsPerSample,long type);
};

AECCtrl::AECCtrl()
{
   // m_ulBufferSize = 12800;
    m_ulSampleRate = 16000;

    //m_CapturedData = new BYTE[m_ulBufferSize];

    m_szCapture = NULL;
    m_szRender = NULL;

    CoInitialize(NULL);
    m_Capture.SetObServer(this);
}

void __stdcall AECCtrl::EnumDevice(std::vector<std::string> &vMic, std::vector<std::string> &vSpeaker)
{
    char    szDevice[MAX_PATH];

/*    for (UINT32 i=0; i<m_vCaptureDev.size(); i++)
    {
        free(m_vCaptureDev[i]);
    }

    for (UINT32 i=0; i<m_vRenderDev.size(); i++)
    {
        free(m_vRenderDev[i]);
    }*/

    vMic.clear();
    vSpeaker.clear();

    std::string     strDev;
    long            index;

    m_Capture.EnumDevices();
	for (index=0; index<m_Capture.GetCaptureDeviceCount(); index++)
	{
		m_Capture.GetCaptureDeviceName(index, szDevice);
        strDev = szDevice;
		vMic.push_back(strDev);
	}

	for (index=0; index<m_Capture.GetRenderDeviceCount(); index++)
	{
		m_Capture.GetRenderDeviceName(index, szDevice);
		strDev = szDevice;
        vSpeaker.push_back(strDev);
	}
}

long __stdcall AECCtrl::OnCapturedData(unsigned char* data,unsigned long size,unsigned long samplerate,unsigned long bitsPerSample,long type)
{ 
    if (g_fOnCaptureData)
    {
        g_fOnCaptureData(data, size, samplerate, bitsPerSample);
    }

//    wavFile.WriteData(data, size);
    return 0;
}

BOOL __stdcall AECCtrl::SetParam(char *szCapture, char *szRender, long ulSampleRate, long ulBitsPerSample)
{
  //  char *szCapture = "Internal Microphone (Cirrus Logic Superior High Definition Audio)";
  //  char *szRender = "扬声器 (Cirrus Logic Superior High Definition Audio)";
    if (!szCapture || 
        !szRender )
    {
        return FALSE;
    }

    if (m_szCapture)
    {
        free(m_szCapture);
    }
    
    if (m_szRender)
    {
        free(m_szRender);
    }

    m_szCapture = strdup(szCapture);
    m_szRender = strdup(szRender);

    m_ulSampleRate = ulSampleRate;
    m_ulBitsPerSample = ulBitsPerSample;

    m_Capture.SetCaptureDeviceByName(m_szCapture);
 	m_Capture.SetRenderDeviceByName(m_szRender);  
	m_Capture.SetCaptureMode(SINGLE_CHANNEL_AEC, m_ulSampleRate);
    
    HRESULT hr = m_Capture.CreateCapture();
	
    if(FAILED(hr))
	{
		return FALSE;
	}

    return TRUE;
}

BOOL __stdcall AECCtrl::Start()
{
    m_Capture.Start();

    return TRUE;
}

VOID __stdcall AECCtrl::Stop()
{
    m_Capture.Stop();
}

AECCtrl     *g_AECCtrl = NULL;

DLL_API BOOL API_CALL AECInit()//char *szCapture, char *szRender, UINT32 ulSampleRate, UINT32 ulBitsPerSample)
{
    if (!g_AECCtrl)
    {
        g_AECCtrl = new AECCtrl(); 
    }

 /*   if (!g_AECCtrl->SetParam(szCapture, szRender, ulSampleRate, ulBitsPerSample))
    {
        goto fail;
    } */

    return TRUE;
}

DLL_API BOOL API_CALL AECEnumDevice(AEC_DEV_T *pstMic, AEC_DEV_T *pstSpeaker)
{
    if (!g_AECCtrl)
    {
        return FALSE;
    }

    std::vector<std::string>    vMic;
    std::vector<std::string>    vSpeaker;

    g_AECCtrl->EnumDevice(vMic, vSpeaker);

    memset(pstMic, 0, sizeof(AEC_DEV_T));
    pstMic->ulNum = vMic.size();
    if (pstMic->ulNum > MAX_AEC_DEV_NUM)
    {
        pstMic->ulNum = MAX_AEC_DEV_NUM;
    }

    for (UINT32 i=0; i<pstMic->ulNum; i++)
    {
        strncpy(pstMic->aacName[i], vMic[i].c_str(), sizeof(pstMic->aacName[i])-1);
    }

    memset(pstSpeaker, 0, sizeof(AEC_DEV_T));
    pstSpeaker->ulNum = vSpeaker.size();
    if (pstSpeaker->ulNum > MAX_AEC_DEV_NUM)
    {
        pstSpeaker->ulNum = MAX_AEC_DEV_NUM;
    }

    for (UINT32 i=0; i<pstSpeaker->ulNum; i++)
    {
        strncpy(&pstSpeaker->aacName[i][0], vSpeaker[i].c_str(), sizeof(pstSpeaker->aacName[i])-1);
    }

    return TRUE;
}

/*
DLL_API BOOL API_CALL AECSetParam(char *szCapture, char *szRender, UINT32 ulSampleRate, UINT32 ulBitsPerSample)
{
    if (!g_AECCtrl)
    {
        return FALSE;
    }

    return g_AECCtrl->SetParam(szCapture, szRender, ulSampleRate, ulBitsPerSample);
} */

DLL_API VOID API_CALL AECSetOnCapturedDataCB(AEC_ON_CAPTURE_DATA fOnCaptureData)
{
    g_fOnCaptureData = fOnCaptureData;
}

DLL_API BOOL API_CALL AECStart(char *szCapture, char *szRender, UINT32 ulSampleRate, UINT32 ulBitsPerSample)
{
    if (!g_AECCtrl)
    {
        return FALSE;
    }

    if (!g_AECCtrl->SetParam(szCapture, szRender, ulSampleRate, ulBitsPerSample))
    {
        return FALSE;
    }

    return g_AECCtrl->Start();
}

DLL_API VOID API_CALL AECStop()
{
    if (!g_AECCtrl)
    {
        return;
    }

    g_AECCtrl->Stop();
}

#if 0

#if 0
#include "WaveFile.h"
CWaveFile			wavFile;

VOID OnCapData(unsigned char* data, unsigned int size, unsigned int samplerate, unsigned int bitsPerSample)
{
    static BOOL bInit = false;
    
    if (!bInit)
    {
        bInit = TRUE;

        WAVEFORMATEX fmt;
	    memset(&fmt,0,sizeof(fmt));
	    fmt.wFormatTag = WAVE_FORMAT_PCM;
	    fmt.nChannels = 1;
	    fmt.nSamplesPerSec = samplerate;
        fmt.wBitsPerSample = 16;
	    fmt.nAvgBytesPerSec = fmt.nChannels * fmt.nSamplesPerSec * fmt.wBitsPerSample / 8;
	    fmt.nBlockAlign = fmt.nChannels * fmt.wBitsPerSample / 8;

	    wavFile.Create("d:\\capture.wav", &fmt);
    }

    wavFile.WriteData(data, size);
}
#endif

void test_AEC()
{
    char *szCapture = "Internal Microphone (Cirrus Logic Superior High Definition Audio)";
    char *szRender = "扬声器 (Cirrus Logic Superior High Definition Audio)";
    UINT32 ulSampleRate = 16000;
    UINT32 ulBitsPerSample = 16;

    AECInit();//szCapture, szRender, ulSampleRate, ulBitsPerSample);

    AEC_DEV_T   stMic;
    AEC_DEV_T   stSpeaker;

    AECEnumDevice(&stMic, &stSpeaker);

    for (UINT32 i=0; i<stMic.ulNum; i++)
    {
        printf("%s\n", stMic.aacName[i]);
    }

    for (UINT32 i=0; i<stSpeaker.ulNum; i++)
    {
        printf("%s\n", stSpeaker.aacName[i]);
    }


//    AECSetOnCapturedDataCB(OnCapData);

//    AECStart();

    Sleep(10*1000);

    AECStop();
//    wavFile.Close();
}

#endif