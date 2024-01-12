
#include <windows.h>
#include <dmo.h>
#include <Mmsystem.h>
#include <objbase.h>
#include <mediaobj.h>
#include <uuids.h>
#include <propidl.h>
#include <wmcodecdsp.h>


class CBaseMediaBuffer : public IMediaBuffer {
public:
    CBaseMediaBuffer() {}
    CBaseMediaBuffer(BYTE *pData, ULONG ulSize, ULONG ulData) :
        m_pData(pData), m_ulSize(ulSize), m_ulData(ulData), m_cRef(1) {}
    STDMETHODIMP_(ULONG) AddRef() {
        return InterlockedIncrement((long*)&m_cRef);
    }
    STDMETHODIMP_(ULONG) Release() {
        long l = InterlockedDecrement((long*)&m_cRef);
        if (l == 0)
            delete this;
        return l;
    }
    STDMETHODIMP QueryInterface(REFIID riid, void **ppv) {
        if (riid == IID_IUnknown) {
            AddRef();
            *ppv = (IUnknown*)this;
            return NOERROR;
        }
        else if (riid == IID_IMediaBuffer) {
            AddRef();
            *ppv = (IMediaBuffer*)this;
            return NOERROR;
        }
        else
            return E_NOINTERFACE;
    }
    STDMETHODIMP SetLength(DWORD ulLength) { m_ulData = ulLength; return NOERROR; }
    STDMETHODIMP GetMaxLength(DWORD *pcbMaxLength) { *pcbMaxLength = m_ulSize; return NOERROR; }
    STDMETHODIMP GetBufferAndLength(BYTE **ppBuffer, DWORD *pcbLength) {
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

class CStaticMediaBuffer : public CBaseMediaBuffer {
public:
    STDMETHODIMP_(ULONG) AddRef() { return 2; }
    STDMETHODIMP_(ULONG) Release() { return 1; }
    void Init(BYTE *pData, ULONG ulSize, ULONG ulData) {
        m_pData = pData;
        m_ulSize = ulSize;
        m_ulData = ulData;
    }
};

#include <propsys.h>

DWORD WINAPI initAEC(LPVOID lpParam){
    DWORD dwStatus;
    HRESULT hr = S_OK;
    IUnknown* pUnk = NULL;
    IMediaObject* pDMO = NULL;
    IPropertyStore* pPS = NULL;
    CoCreateInstance(CLSID_CWMAudioAEC,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_IMediaObject,
        (void**)&pDMO);
    pDMO->QueryInterface(IID_IPropertyStore, (void**)&pPS);

    // Set DMO system mode
    LONG system_mode = SINGLE_CHANNEL_AEC; // AEC only mode
    PROPVARIANT pvSysMode;
    PropVariantInit(&pvSysMode);
    pvSysMode.vt = VT_I4;
    pvSysMode.lVal = (LONG)(system_mode);
    pPS->SetValue(MFPKEY_WMAAECMA_SYSTEM_MODE, pvSysMode);
    pPS->GetValue(MFPKEY_WMAAECMA_SYSTEM_MODE, &pvSysMode);
    PropVariantClear(&pvSysMode);


    int iSpkDevIdx =0;
    int iMicDevIdx = pThis->micIndex;
    PROPVARIANT pvDeviceId;
    PropVariantInit(&pvDeviceId);
    pvDeviceId.vt = VT_I4;
    TRACE("%20s %5d \n", "iSpkDevIdx is", iSpkDevIdx << 16);
    pvDeviceId.lVal = (unsigned long)(iSpkDevIdx << 16) + (unsigned long)(0x0000ffff & iMicDevIdx);
    pPS->SetValue(MFPKEY_WMAAECMA_DEVICE_INDEXES, pvDeviceId);
    pPS->GetValue(MFPKEY_WMAAECMA_DEVICE_INDEXES, &pvDeviceId);
    PropVariantClear(&pvDeviceId);


    
    
    BOOL bFeatrModeOn = true;
    BOOL bNoiseSup = 1;                 // turn noise suppression on/off
    BOOL bAGC = 1;                      // turn digital auto gain control on/off
    BOOL bCntrClip = 1;
    /*
    Feature Mode is - 1
    Noise suppresion is     1
    AGC is     0
    Center clip is     0
    */


    if (bFeatrModeOn)
    {
        // Turn on feature modes
        PROPVARIANT pvFeatrModeOn;
        PropVariantInit(&pvFeatrModeOn);
        pvFeatrModeOn.vt = VT_BOOL;
        pvFeatrModeOn.boolVal = bFeatrModeOn ?VBTRUE : VBFALSE;
        pPS->SetValue(MFPKEY_WMAAECMA_FEATURE_MODE, pvFeatrModeOn);
        pPS->GetValue(MFPKEY_WMAAECMA_FEATURE_MODE, &pvFeatrModeOn);
        TRACE("%20s %5d \n", "Feature Mode is", pvFeatrModeOn.boolVal);
        PropVariantClear(&pvFeatrModeOn);

        // Turn on/off noise suppression  噪声抑制
        PROPVARIANT pvNoiseSup;
        PropVariantInit(&pvNoiseSup);
        pvNoiseSup.vt = VT_I4;
        pvNoiseSup.lVal = (LONG)bNoiseSup;
        pPS->SetValue(MFPKEY_WMAAECMA_FEATR_NS, pvNoiseSup);
        pPS->GetValue(MFPKEY_WMAAECMA_FEATR_NS, &pvNoiseSup);
        TRACE("%20s %5d \n", "Noise suppresion is", pvNoiseSup.lVal);
        PropVariantClear(&pvNoiseSup);

        // Turn on/off AGC 自动增益控制
        PROPVARIANT pvAGC;
        PropVariantInit(&pvAGC);
        pvAGC.vt = VT_BOOL;
        pvAGC.boolVal = bAGC ? VBTRUE : VBFALSE;
        pPS->SetValue(MFPKEY_WMAAECMA_FEATR_AGC, pvAGC);
        pPS->GetValue(MFPKEY_WMAAECMA_FEATR_AGC, &pvAGC);
        TRACE("%20s %5d \n", "AGC is", pvAGC.boolVal);
        PropVariantClear(&pvAGC);

        // Turn on/off center clip 中心削波
        PROPVARIANT pvCntrClip;
        PropVariantInit(&pvCntrClip);
        pvCntrClip.vt = VT_BOOL;
        pvCntrClip.boolVal = bCntrClip ? VBTRUE : VBFALSE;
        pPS->SetValue(MFPKEY_WMAAECMA_FEATR_CENTER_CLIP, pvCntrClip);
        pPS->GetValue(MFPKEY_WMAAECMA_FEATR_CENTER_CLIP, &pvCntrClip);
        TRACE("%20s %5d \n", "Center clip is", (BOOL)pvCntrClip.boolVal);
        PropVariantClear(&pvCntrClip);
    }

    

    DMO_MEDIA_TYPE mt;
    WAVEFORMATEX wfxOut;

    wfxOut.wFormatTag = WAVE_FORMAT_PCM;
    wfxOut.nSamplesPerSec = 22050;
    wfxOut.wBitsPerSample = 16;
    wfxOut.nChannels = 1;
    wfxOut.nBlockAlign = (wfxOut.wBitsPerSample * wfxOut.nChannels) / 8;
    wfxOut.nAvgBytesPerSec = wfxOut.nBlockAlign * wfxOut.nSamplesPerSec;
    wfxOut.cbSize = 0;

    hr = MoInitMediaType(&mt, sizeof(WAVEFORMATEX));
    

    mt.majortype = MEDIATYPE_Audio;
    mt.subtype = MEDIASUBTYPE_PCM;
    mt.lSampleSize = 0;
    mt.bFixedSizeSamples = TRUE;
    mt.bTemporalCompression = FALSE;
    mt.formattype = FORMAT_WaveFormatEx;
    memcpy(mt.pbFormat, &wfxOut, sizeof(WAVEFORMATEX));

    hr = pDMO->SetOutputType(0, &mt, 0);    
    MoFreeMediaType(&mt);

    // allocate output buffer
    int cOutputBufLen = wfxOut.nSamplesPerSec * wfxOut.nBlockAlign;
    BYTE *pbOutputBuffer = new BYTE[cOutputBufLen];
    

    // Create a DMO output buffer object
    // main loop to get microphone output from the DMO
    CStaticMediaBuffer outputBuffer;
    DMO_OUTPUT_DATA_BUFFER OutputBufferStruct = { 0 };
    OutputBufferStruct.pBuffer = &outputBuffer;

    // main loop to get microphone output from the DMO
    ULONG cbProduced = 0;
    while (pThis->isAEC)
    {
        //char *data = new char[2048];
        
        do{
            //outputBuffer.Init((byte*)data, 2048, 0);
            outputBuffer.Init((byte*)pbOutputBuffer, cOutputBufLen, 0);
            OutputBufferStruct.dwStatus = 0;
            hr = pDMO->ProcessOutput(0, 1,&OutputBufferStruct,    &dwStatus);
            

            if (hr == S_FALSE) {
                cbProduced = 0;
            }
            else {
                hr = outputBuffer.GetBufferAndLength(NULL, &cbProduced);
                
            }

        
        } while (OutputBufferStruct.dwStatus & DMO_OUTPUT_DATA_BUFFERF_INCOMPLETE);


        char * tmpBuff = new char[2048];
        memcpy(tmpBuff, pbOutputBuffer, 2048);


        EnterCriticalSection(&pThis->queLock);
        pThis->audioQue.push(tmpBuff);
        if (pThis->audioQue.size() >50){
            
            delete[]pThis->audioQue.front();
            pThis->audioQue.pop();
        }
        LeaveCriticalSection(&pThis->queLock);

        Sleep(40); //sleep 10ms
    }


    if (pDMO)
    {
        pDMO->Release();
        pDMO = NULL;
    }
    if (pPS)
    {
        pPS->Release();
        pPS = NULL;
    }


    return 0;

}

 

