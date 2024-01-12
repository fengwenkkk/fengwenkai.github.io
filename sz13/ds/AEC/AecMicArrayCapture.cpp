//#include "stdafx.h"
//#include <boost/foreach.hpp>
#include "AecMicArrayCapture.h"

#define FAILED_RETURN(hr)\
	if(FAILED(hr))\
		{\
		return hr;\
		}

#define CHECKHR(x) hr = x; if (FAILED(hr)) {return hr;}
#define CHECK_RET(hr, message) if (FAILED(hr)) { puts(message); goto exit;}

#define VBFALSE (VARIANT_BOOL)0
#define VBTRUE  (VARIANT_BOOL)-1

#define BUFFER_SIZE		32000
#define CAPTURE_CYCLE	20

#pragma warning(disable: 4018)
#pragma warning(disable: 4996)
#pragma comment(lib,"Msdmo.lib")
#pragma comment(lib,"dmoguids.lib")
#pragma comment(lib,"uuid.lib")
#pragma comment(lib,"amstrmid.lib")
#pragma comment(lib,"wmcodecdspuuid.lib")


void CALLBACK TimerOnCapture(UINT id, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	CAecMicArrayCapture* _this = (CAecMicArrayCapture*)dwUser;
	if(_this)
	{
		_this->ThreadCaptureEntry();
	}
}

CAecMicArrayCapture::CAecMicArrayCapture(void)
{
	INIT_REFCOUNT
	INIT_OBSERVER
    _allCaptureDevice = NULL;
    _allRenderDevice = NULL;

    _captureCount = 0;
    _renderCount = 0;
    _captureID = -1;
    _renderID = -1;
	_captureIDCur = -2;
	_renderIDCur = -2;

    _captureMode = SINGLE_CHANNEL_AEC;//SINGLE_CHANNEL_AEC;
    _samplerate = 16000;
    _bitsPerSample = 16;

    _bCntrClip = false;
    _bAGC = false;
    _bNoiseSup = false;
    _bFeatrModeOn = false;

    _frameSize = 0;

	_TimerID = 0;

    _pDMO = NULL;

    _outputBuffer.pBuffer = static_cast<IMediaBuffer*>(&_outputMediaBuffer);
    _outputBuffer.dwStatus = 0;

	_captureBuffer = (unsigned char*)malloc(BUFFER_SIZE);
}

CAecMicArrayCapture::~CAecMicArrayCapture(void)
{
	if(_pDMO)
	{
		_pDMO->Release();
		_pDMO = NULL;
	}
	if (_allCaptureDevice)
	{
		free(_allCaptureDevice);
		_allCaptureDevice = NULL;
	}
	if (_allRenderDevice)
	{
		free(_allRenderDevice);
		_allRenderDevice = NULL;
	}
	if(_captureBuffer)
	{
		free(_captureBuffer);
		_captureBuffer = NULL;
	}
}

HRESULT CAecMicArrayCapture::_Reset(void)
{
	if(_pDMO)
	{
		_pDMO->Release();
		_pDMO = NULL;
	}
	_captureIDCur = -2;
	_renderIDCur = -2;
	return S_OK;
}
long CAecMicArrayCapture::ResetCapture()
{
	Stop();
	_Reset();
	return 0;
}

long CAecMicArrayCapture::EnumDevices()
{
	if (_allCaptureDevice)
	{
		free(_allCaptureDevice);
		_allCaptureDevice = NULL;
	}
	if (_allRenderDevice)
	{
		free(_allRenderDevice);
		_allRenderDevice = NULL;
	}
    GetCaptureDeviceNum(_captureCount);
    _allCaptureDevice = (AUDIO_DEVICE_INFO*)malloc(sizeof(AUDIO_DEVICE_INFO) * _captureCount);
    EnumCaptureDevice(_captureCount,_allCaptureDevice);

    GetRenderDeviceNum(_renderCount);
    _allRenderDevice = (AUDIO_DEVICE_INFO*)malloc(sizeof(AUDIO_DEVICE_INFO) * _renderCount);
    EnumRenderDevice(_renderCount,_allRenderDevice);

    return 0;
}
long CAecMicArrayCapture::GetCaptureDeviceCount()
{
    return _captureCount;
}
long CAecMicArrayCapture::GetCaptureDeviceName(long index,char* device)
{
    if (index<0 || index >= _captureCount)
    {
        return -1;
    }
    AUDIO_DEVICE_INFO* info = _allCaptureDevice + index;
    USES_CONVERSION;
    strcpy(device,W2A(info->szDeviceName));
    return 0;
}
long CAecMicArrayCapture::SetCaptureDeviceByName(char* device)
{
    USES_CONVERSION;

    long index = 0;
    AUDIO_DEVICE_INFO* info = NULL;
    for (; index < _captureCount; index++)
    {
        info = _allCaptureDevice + index;
        if (strcmp(device,W2A(info->szDeviceName)) == 0)
        {
            break;
        }
    }
    if (index<_captureCount)
    {
        _captureID = index;
    }
    return 0;
}

long CAecMicArrayCapture::SetCaptureDeviceByID(long index)
{
    if (index<0 || index >= _captureCount)
    {
        return -1;
    }
    _captureID = index;
    return 0;
}

long CAecMicArrayCapture::GetRenderDeviceCount()
{
    return _renderCount;
}
long CAecMicArrayCapture::GetRenderDeviceName(long index,char* device)
{
    if (index<0 || index >= _renderCount)
    {
        return -1;
    }
    AUDIO_DEVICE_INFO* info = _allRenderDevice + index;
    USES_CONVERSION;
    strcpy(device,W2A(info->szDeviceName));
    return 0;
}
long CAecMicArrayCapture::SetRenderDeviceByName(char* device)
{
    USES_CONVERSION;

    long index = 0;
    AUDIO_DEVICE_INFO* info = NULL;
    for (; index < _renderCount; index++)
    {
        info = _allRenderDevice + index;
        if (strcmp(device,W2A(info->szDeviceName)) == 0)
        {
            break;
        }
    }
    if (index<_renderCount)
    {
        _renderID = index;
    }
    return 0;
}
long CAecMicArrayCapture::SetRenderDeviceByID(long index)
{
    if (index<0 || index >= _renderCount)
    {
        return -1;
    }
    _renderID = index;
    return 0;
}

long CAecMicArrayCapture::SetCaptureMode(long captureMode,long samplerate)
{
    if (captureMode == SINGLE_CHANNEL_AEC ||
        captureMode == ADAPTIVE_ARRAY_AND_AEC ||
        captureMode == OPTIBEAM_ARRAY_AND_AEC )
    {
        _captureMode = captureMode;
    }
    if (samplerate == 8000	||
        samplerate == 11025 ||
        samplerate == 16000	||
        samplerate == 22050	)
    {
        _samplerate = samplerate;
    }
    return 0;
}
long CAecMicArrayCapture::CreateCapture()
{
	if (!NeedReCreate())
	{
		return S_FALSE;
	}
	Stop();
	_Reset();
	

    HRESULT hr = CoCreateInstance(CLSID_CWMAudioAEC, NULL,
                                  CLSCTX_INPROC_SERVER, IID_IMediaObject, (void**)&_pDMO);
    FAILED_RETURN(hr)

    CComPtr<IPropertyStore> pPS = NULL;
    hr = _pDMO->QueryInterface(IID_IPropertyStore, (void**)&pPS);
    FAILED_RETURN(hr)

    PROPVARIANT pvSysMode;
    PropVariantInit(&pvSysMode);
    pvSysMode.vt = VT_I4;
    pvSysMode.lVal = (LONG)(_captureMode);
    CHECKHR(pPS->SetValue(MFPKEY_WMAAECMA_SYSTEM_MODE, pvSysMode));
    CHECKHR(pPS->GetValue(MFPKEY_WMAAECMA_SYSTEM_MODE, &pvSysMode));

	
	//pvSysMode.lVal = 128;
	//CHECKHR(pPS->SetValue(MFPKEY_WMAAECMA_FEATR_ECHO_LENGTH, pvSysMode));
 //   CHECKHR(pPS->GetValue(MFPKEY_WMAAECMA_FEATR_ECHO_LENGTH, &pvSysMode));
 //   PropVariantClear(&pvSysMode);

    if (_captureID >= 0 || _renderID >= 0)
    {
        PROPVARIANT pvDeviceId;
        PropVariantInit(&pvDeviceId);
        pvDeviceId.vt = VT_I4;
        pvDeviceId.lVal = (unsigned long)(_renderID<<16) + (unsigned long)(0x0000ffff & _captureID);
        CHECKHR(pPS->SetValue(MFPKEY_WMAAECMA_DEVICE_INDEXES, pvDeviceId));
        CHECKHR(pPS->GetValue(MFPKEY_WMAAECMA_DEVICE_INDEXES, &pvDeviceId));
        PropVariantClear(&pvDeviceId);
    }

    if ( _bFeatrModeOn )
    {
        PROPVARIANT pvFeatrModeOn;
        PropVariantInit(&pvFeatrModeOn);
        pvFeatrModeOn.vt = VT_BOOL;
        pvFeatrModeOn.boolVal = _bFeatrModeOn? VBTRUE : VBFALSE;
        CHECKHR(pPS->SetValue(MFPKEY_WMAAECMA_FEATURE_MODE, pvFeatrModeOn));
        CHECKHR(pPS->GetValue(MFPKEY_WMAAECMA_FEATURE_MODE, &pvFeatrModeOn));
        PropVariantClear(&pvFeatrModeOn);

        PROPVARIANT pvNoiseSup;
        PropVariantInit(&pvNoiseSup);
        pvNoiseSup.vt = VT_I4;
        pvNoiseSup.lVal = (LONG)_bNoiseSup;
        CHECKHR(pPS->SetValue(MFPKEY_WMAAECMA_FEATR_NS, pvNoiseSup));
        CHECKHR(pPS->GetValue(MFPKEY_WMAAECMA_FEATR_NS, &pvNoiseSup));
        PropVariantClear(&pvNoiseSup);

        PROPVARIANT pvAGC;
        PropVariantInit(&pvAGC);
        pvAGC.vt = VT_BOOL;
        pvAGC.boolVal = _bAGC ? VBTRUE : VBFALSE;
        CHECKHR(pPS->SetValue(MFPKEY_WMAAECMA_FEATR_AGC, pvAGC));
        CHECKHR(pPS->GetValue(MFPKEY_WMAAECMA_FEATR_AGC, &pvAGC));
        PropVariantClear(&pvAGC);

        PROPVARIANT pvCntrClip;
        PropVariantInit(&pvCntrClip);
        pvCntrClip.vt = VT_BOOL;
        pvCntrClip.boolVal = _bCntrClip? VBTRUE : VBFALSE;
        CHECKHR(pPS->SetValue(MFPKEY_WMAAECMA_FEATR_CENTER_CLIP, pvCntrClip));
        CHECKHR(pPS->GetValue(MFPKEY_WMAAECMA_FEATR_CENTER_CLIP, &pvCntrClip));
        PropVariantClear(&pvCntrClip);
    }

    WAVEFORMATEX wfxOut = {WAVE_FORMAT_PCM, 1, 16000, 32000, 2, 16, 0};
    wfxOut.nSamplesPerSec = _samplerate;
    wfxOut.nAvgBytesPerSec = wfxOut.nSamplesPerSec * wfxOut.wBitsPerSample / 8;

    DMO_MEDIA_TYPE mt;
    hr = MoInitMediaType(&mt, sizeof(WAVEFORMATEX));
    mt.majortype = MEDIATYPE_Audio;
    mt.subtype = MEDIASUBTYPE_PCM;
    mt.lSampleSize = 0;
    mt.bFixedSizeSamples = TRUE;
    mt.bTemporalCompression = FALSE;
    mt.formattype = FORMAT_WaveFormatEx;
    memcpy(mt.pbFormat, &wfxOut, sizeof(WAVEFORMATEX));

    CHECKHR(_pDMO->SetOutputType(0, &mt, 0))
    MoFreeMediaType(&mt);

	PROPVARIANT pvFrameSize;
	PropVariantInit(&pvFrameSize);
	pvFrameSize.vt = VT_I4;
	pvFrameSize.lVal = 320;
	CHECKHR(pPS->SetValue(MFPKEY_WMAAECMA_FEATR_FRAME_SIZE, pvFrameSize));
	CHECKHR(pPS->GetValue(MFPKEY_WMAAECMA_FEATR_FRAME_SIZE, &pvFrameSize));
	_frameSize = pvFrameSize.lVal;
	PropVariantClear(&pvFrameSize);

    CHECKHR(_pDMO->AllocateStreamingResources());

	_captureIDCur = _captureID;
	_renderIDCur = _renderID;

    return 0;
}

long CAecMicArrayCapture::GetCaptureData(unsigned char* data,unsigned long* size)
{
    ULONG cbProduced = 0;
	DWORD dwStatus = 0;
	unsigned long capturedSize = 0;
	unsigned long emptySize = *size;
	*size = 0;
    HRESULT hr = E_FAIL;
    do
    {
		Sleep(10);
        _outputMediaBuffer.Init((byte*)data, emptySize, 0);
        _outputBuffer.dwStatus = 0;
        hr = _pDMO->ProcessOutput(0, 1, &_outputBuffer, &dwStatus);

        if (hr == S_FALSE)
        {
            cbProduced = 0;
        }
        else
        {
            CHECKHR(_outputMediaBuffer.GetBufferAndLength(NULL, &cbProduced));
        }

        capturedSize += cbProduced;
		emptySize -= cbProduced;
		data += cbProduced;
    }
    while (_outputBuffer.dwStatus & DMO_OUTPUT_DATA_BUFFERF_INCOMPLETE);

	*size = capturedSize;
    
	return 0;
}

long CAecMicArrayCapture::ThreadCaptureEntry()
{
	ULONG cbProduced = 0;
	DWORD dwStatus = 0;
	unsigned long capturedSize = 0;
	unsigned long emptySize = BUFFER_SIZE;
	unsigned char* data = _captureBuffer;
    HRESULT hr = E_FAIL;
    do
    {
        _outputMediaBuffer.Init((byte*)data, emptySize, 0);
        _outputBuffer.dwStatus = 0;
        hr = _pDMO->ProcessOutput(0, 1, &_outputBuffer, &dwStatus);

        if (hr == S_FALSE)
        {
            cbProduced = 0;
        }
        else
        {
            CHECKHR(_outputMediaBuffer.GetBufferAndLength(NULL, &cbProduced));
        }

        capturedSize += cbProduced;
		emptySize -= cbProduced;
		data += cbProduced;
    }
    while (_outputBuffer.dwStatus & DMO_OUTPUT_DATA_BUFFERF_INCOMPLETE);

	if(!capturedSize)
	{
		return S_FALSE;
	}
	if (_SingleObserver)
	{
		_SingleObserver->OnCapturedData(_captureBuffer,capturedSize,_samplerate,_bitsPerSample, CAP_TYPE_AECMICARR);
	}
	else
	{
        set<IAecCaptureEvent*>::iterator it = _AllObservers.begin();
		//BOOST_FOREACH(IAecCaptureEvent *oberver,_AllObservers)
        for (; it!=_AllObservers.end(); it++)
		{
            IAecCaptureEvent *oberver = *it;
			oberver->OnCapturedData(_captureBuffer,capturedSize,_samplerate,_bitsPerSample, CAP_TYPE_AECMICARR);
		}
	}
	return 0;
}

long CAecMicArrayCapture::Start(void)
{
	if(_TimerID)
	{
		return 0;
	}
	timeBeginPeriod(CAPTURE_CYCLE);
	_TimerID = timeSetEvent(CAPTURE_CYCLE/2,CAPTURE_CYCLE,TimerOnCapture,(DWORD)this,TIME_CALLBACK_FUNCTION|TIME_PERIODIC|TIME_KILL_SYNCHRONOUS);
	return 0;
}


long CAecMicArrayCapture::Stop(void)
{
	if(_TimerID)
	{
		timeEndPeriod(CAPTURE_CYCLE);
		timeKillEvent(_TimerID);
		_TimerID = 0;
	}
	return 0;
}

long CAecMicArrayCapture::NeedReCreate()
{
	if (_captureID == _captureIDCur &&
		_renderID  == _renderIDCur)
	{
		return false;
	}
	else
	{
		return true;
	}
}