#pragma once

#include <dmo.h>
#include <Mmsystem.h>
#include <mediaobj.h>
#include <wmcodecdsp.h>
#include <uuids.h>
#include <map>
#include <set>
#include "AecKsBinder.h"
#include "AecCaptureInterface.h"

using namespace std;

class CBaseMediaBuffer : public IMediaBuffer
{
public:
    CBaseMediaBuffer() {}
    CBaseMediaBuffer(BYTE *pData, ULONG ulSize, ULONG ulData) :
            m_pData(pData), m_ulSize(ulSize), m_ulData(ulData), m_cRef(1) {}
    STDMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement((long*)&m_cRef);
    }
    STDMETHODIMP_(ULONG) Release()
    {
        long l = InterlockedDecrement((long*)&m_cRef);
        if (l == 0)
            delete this;
        return l;
    }
    STDMETHODIMP QueryInterface(REFIID riid, void **ppv)
    {
        if (riid == IID_IUnknown)
        {
            AddRef();
            *ppv = (IUnknown*)this;
            return NOERROR;
        }
        else if (riid == IID_IMediaBuffer)
        {
            AddRef();
            *ppv = (IMediaBuffer*)this;
            return NOERROR;
        }
        else
            return E_NOINTERFACE;
    }
    STDMETHODIMP SetLength(DWORD ulLength)
    {
        m_ulData = ulLength;
        return NOERROR;
    }
    STDMETHODIMP GetMaxLength(DWORD *pcbMaxLength)
    {
        *pcbMaxLength = m_ulSize;
        return NOERROR;
    }
    STDMETHODIMP GetBufferAndLength(BYTE **ppBuffer, DWORD *pcbLength)
    {
        if (ppBuffer) *ppBuffer = m_pData;
        if (pcbLength) *pcbLength = m_ulData;
        return NOERROR;
    }
protected:
    BYTE *m_pData;
    ULONG m_ulSize;
    ULONG m_ulData;
    ULONG m_cRef;
};

class CStaticMediaBuffer : public CBaseMediaBuffer
{
public:
    STDMETHODIMP_(ULONG) AddRef()
    {
        return 2;
    }
    STDMETHODIMP_(ULONG) Release()
    {
        return 1;
    }
    void Init(BYTE *pData, ULONG ulSize, ULONG ulData)
    {
        m_pData = pData;
        m_ulSize = ulSize;
        m_ulData = ulData;
    }
};

class CAecMicArrayCapture : public IAecCapture
{
public:
    CAecMicArrayCapture(void);
    ~CAecMicArrayCapture(void);
private:
    AUDIO_DEVICE_INFO		*_allCaptureDevice;
    AUDIO_DEVICE_INFO		*_allRenderDevice;

    UINT					_captureCount;
    UINT					_renderCount;
    long					_captureID;
    long					_renderID;
    long					_captureIDCur;
    long					_renderIDCur;

    long					_captureMode;
    long					_samplerate;
    long					_bitsPerSample;

    bool					_bCntrClip;
    bool					_bAGC;
    bool					_bNoiseSup;
    bool					_bFeatrModeOn;

    long					_frameSize;

    unsigned long			_TimerID;

    IMediaObject			*_pDMO;
    unsigned char*			_captureBuffer;
    CStaticMediaBuffer		_outputMediaBuffer;
    DMO_OUTPUT_DATA_BUFFER	_outputBuffer;

private:
    HRESULT _Reset(void);
public:
    long ThreadCaptureEntry();
public:
    IMPLEMENT_REFCOUNT
    IMPLEMENT_OBSERVER(IAecCaptureEvent)
    virtual long __stdcall ResetCapture();
    virtual long __stdcall EnumDevices();

    virtual long __stdcall GetCaptureDeviceCount();
    virtual long __stdcall GetCaptureDeviceName(long index,char* device);
    virtual long __stdcall SetCaptureDeviceByName(char* device);
    virtual long __stdcall SetCaptureDeviceByID(long device);

    virtual long __stdcall GetRenderDeviceCount();
    virtual long __stdcall GetRenderDeviceName(long index,char* device);
    virtual long __stdcall SetRenderDeviceByName(char* device);
    virtual long __stdcall SetRenderDeviceByID(long device);

    virtual long __stdcall SetCaptureMode(long captureMode,long samplerate);
    virtual long __stdcall CreateCapture();
    virtual long __stdcall GetCaptureData(unsigned char* data,/*in out*/unsigned long* size);
    virtual long __stdcall Start(void);
    virtual long __stdcall Stop(void);

    virtual long __stdcall NeedReCreate();
};
