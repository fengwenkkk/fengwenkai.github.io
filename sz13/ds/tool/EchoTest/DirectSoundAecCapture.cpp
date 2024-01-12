#include "g_include.h"
#include "DirectSoundAecCapture.h"

#include <mmsystem.h>
#include "dsound.h"

#pragma warning(disable:4996)

#pragma comment(lib,"dsound.lib")

DWORD CaptureThreadEntry(LPVOID data)
{
	CDirectSoundAecCapture* capture = (CDirectSoundAecCapture*)data;
	capture->_ProcCapture();
	return 0;
}


BOOL CALLBACK DSoundEnum(LPGUID lpGUID,
                         LPCSTR lpszDesc,
                         LPCSTR lpszDrvName,
                         LPVOID lpContext)
{
    CDirectSoundAecCapture* pThis = (CDirectSoundAecCapture*)lpContext;
    DSoundDeviceInfo info;
    info.name = lpszDrvName;
    info.desc = lpszDesc;
    if (lpGUID)
    {
        info.guid = *lpGUID;
    }
    else
    {
        info.guid = GUID_NULL;
    }
    pThis->m_allRenderDevices.push_back(info);
    return TRUE;
}
BOOL CALLBACK DSoundCapEnum(LPGUID lpGUID,
                            LPCSTR lpszDesc,
                            LPCSTR lpszDrvName,
                            LPVOID lpContext)
{
    CDirectSoundAecCapture* pThis = (CDirectSoundAecCapture*)lpContext;
    DSoundDeviceInfo info;
    info.name = lpszDrvName;
    info.desc = lpszDesc;
    if (lpGUID)
    {
        info.guid = *lpGUID;
    }
    else
    {
        info.guid = GUID_NULL;
    }
    pThis->m_allCaptureDevices.push_back(info);
    return TRUE;
}
CDirectSoundAecCapture::CDirectSoundAecCapture(void)
{
    m_captureIndex = -1;
    m_renderIndex = -1;
	m_samplerate = 16000;

	frameSize = 640;
	captureBufferSize = frameSize * CAPTURE_FRAME_COUNT;
	pCaptureDataTemp = NULL;
	pCaptureDataTemp = (unsigned char*)malloc(captureBufferSize);
	pEvent = NULL;
	runThreadFlag = true;
	
	::CoInitialize(NULL);
    EnumDevices();
}
CDirectSoundAecCapture::~CDirectSoundAecCapture(void)
{
	if(pEvent)
	{
		CloseHandle(pEvent);
		pEvent = NULL;
	}
	if(pCaptureDataTemp)
	{
		free(pCaptureDataTemp);
		pCaptureDataTemp = NULL;
	}
}
HRESULT CDirectSoundAecCapture::EnumDevices()
{
    if (FAILED(DirectSoundEnumerateA(DSoundEnum,(LPVOID)this)))
    {
        return E_FAIL;
    }
    if (FAILED(DirectSoundCaptureEnumerateA(DSoundCapEnum,(LPVOID)this)))
    {
        return E_FAIL;
    }
    return S_OK;
}


long CDirectSoundAecCapture::GetCaptureDeviceCount()
{
    return m_allCaptureDevices.size();
}
long CDirectSoundAecCapture::GetCaptureDeviceName(long index,char* device)
{
    if ((size_t)index >= m_allCaptureDevices.size())
    {
        return -1;
    }
    strcpy(device,m_allCaptureDevices[index].desc.c_str());
    return 0;
}
long CDirectSoundAecCapture::SetCaptureDeviceByName(char* device)
{
    long index = 0;
    long count = m_allCaptureDevices.size();
    while (index<count)
    {
        if (m_allCaptureDevices[index].desc == device)
        {
            break;
        }
        index++;
    }
    if (index<count)
    {
        m_captureIndex = index;
    }
    return 0;
}
long CDirectSoundAecCapture::SetCaptureDeviceByID(long device)
{
    if ((size_t)device >= m_allCaptureDevices.size())
    {
        return -1;
    }
    m_captureIndex = device;
    return 0;
}

long CDirectSoundAecCapture::GetRenderDeviceCount()
{
    return m_allRenderDevices.size();
}
long CDirectSoundAecCapture::GetRenderDeviceName(long index,char* device)
{
    if ((size_t)index >= m_allRenderDevices.size())
    {
        return -1;
    }
    strcpy(device,m_allRenderDevices[index].desc.c_str());
    return 0;
}
long CDirectSoundAecCapture::SetRenderDeviceByName(char* device)
{
    long index = 0;
    long count = m_allRenderDevices.size();
    while (index<count)
    {
        if (m_allRenderDevices[index].desc == device)
        {
            break;
        }
        index++;
    }
    if (index<count)
    {
        m_renderIndex = index;
    }
    return 0;
}
long CDirectSoundAecCapture::SetRenderDeviceByID(long device)
{
    if ((size_t)device >= m_allRenderDevices.size())
    {
        return -1;
    }
    m_captureIndex = device;
    return 0;
}

long CDirectSoundAecCapture::SetCaptureMode(long captureMode,long samplerate)
{
	return 16000;
}
HRESULT CDirectSoundAecCapture::Create()
{
    GUID guidCapture = GUID_NULL;
    GUID guidRender = GUID_NULL;
    if (m_captureIndex>0)
    {
        guidCapture = m_allCaptureDevices[m_captureIndex].guid;
    }
    if (m_renderIndex>0)
    {
        guidRender = m_allRenderDevices[m_renderIndex].guid;
    }

    DSCEFFECTDESC dsced[2];
    ZeroMemory( &dsced[0], sizeof( DSCEFFECTDESC )*2 );
    dsced[0].dwSize				= sizeof(DSCEFFECTDESC);
    dsced[0].dwFlags            = DSCFX_LOCSOFTWARE;
    dsced[0].guidDSCFXClass     = GUID_DSCFX_CLASS_AEC;
    dsced[0].guidDSCFXInstance  = GUID_DSCFX_MS_AEC;
    dsced[0].dwReserved1        = 0;
    dsced[0].dwReserved2        = 0;
    dsced[1].dwSize             = sizeof(DSCEFFECTDESC);
    dsced[1].dwFlags            = DSCFX_LOCSOFTWARE;
    dsced[1].guidDSCFXClass     = GUID_DSCFX_CLASS_NS;
    dsced[1].guidDSCFXInstance  = GUID_DSCFX_MS_NS;
    dsced[1].dwReserved1        = 0;
    dsced[1].dwReserved2        = 0;

	WAVEFORMATEX wfxOut = {WAVE_FORMAT_PCM, 1, 16000, 32000, 2, 16, 0};

	DSCBUFFERDESC dscbd;
	ZeroMemory(&dscbd,sizeof(DSCBUFFERDESC));
	dscbd.dwSize				= sizeof(DSCBUFFERDESC);
	dscbd.dwFlags				= DSCBCAPS_CTRLFX;
	dscbd.dwBufferBytes			= captureBufferSize;
	dscbd.dwReserved            = 0;
	dscbd.lpwfxFormat			= &wfxOut;
	dscbd.dwFXCount				= 2;
	dscbd.lpDSCFXDesc			= dsced;

	DSBUFFERDESC dsobd;
	ZeroMemory(&dsobd,sizeof(DSBUFFERDESC));
	dsobd.dwSize				= sizeof(DSBUFFERDESC);
	dsobd.dwFlags				= DSBCAPS_CTRLFX;
	dsobd.dwBufferBytes			= captureBufferSize;
	dsobd.lpwfxFormat			= &wfxOut;
	dsobd.dwReserved			= 0;
	dsobd.dwFlags 				= DSBCAPS_GETCURRENTPOSITION2 
								| DSBCAPS_GLOBALFOCUS
								| DSBCAPS_CTRLPOSITIONNOTIFY 
								| DSBCAPS_LOCSOFTWARE 
								| DSBCAPS_CTRLFREQUENCY;


	HWND hWnd = GetForegroundWindow();
	if (hWnd == NULL)
		hWnd = GetDesktopWindow();

	HRESULT hr = DirectSoundFullDuplexCreate8(&guidCapture,&guidRender,&dscbd,&dsobd,hWnd,
		DSSCL_PRIORITY,&pDSFD,&pDSCBuffer8,&pDSBuffer8,NULL);
	if (FAILED(hr))
	{
		dscbd.dwFlags				= 0;
		dscbd.dwFXCount				= 0;
		dscbd.lpDSCFXDesc			= NULL;
		hr = DirectSoundFullDuplexCreate8(&guidCapture,&guidRender,&dscbd,&dsobd,hWnd,
			DSSCL_PRIORITY,&pDSFD,&pDSCBuffer8,&pDSBuffer8,NULL);
		if (FAILED(hr))
		{
			dsobd.dwFlags &= ~DSBCAPS_CTRLFREQUENCY;
			hr = DirectSoundFullDuplexCreate8(&guidCapture,&guidRender,&dscbd,&dsobd,hWnd,
				DSSCL_PRIORITY,&pDSFD,&pDSCBuffer8,&pDSBuffer8,NULL);
		}
	}
	if (FAILED(hr))
	{
		return -1;
	}

   //	CComPtr<IDirectSoundCaptureFXAec8> pAec = NULL;

    CComPtr<IDirectSoundCaptureFXAec  > pAec = NULL;
	hr = pDSCBuffer8->GetObjectInPath(GUID_DSCFX_CLASS_AEC,0,IID_IDirectSoundCaptureFXAec8,(LPVOID*)&pAec);
	if(pAec)
	{
		DSCFXAec aecMode = {TRUE,TRUE,DSCFX_AEC_MODE_FULL_DUPLEX };
		hr = pAec->SetAllParameters(&aecMode);
	}

	pEvent = CreateEvent( NULL, FALSE, FALSE,NULL );
	CComPtr<IDirectSoundNotify8> pNotify;
	hr = pDSCBuffer8->QueryInterface(IID_IDirectSoundNotify8,(LPVOID*)&pNotify);
	for (long index = 0;index<CAPTURE_FRAME_COUNT;index++)
	{
		dsPosNotify[index].dwOffset = index*frameSize + frameSize - 1;
		dsPosNotify[index].hEventNotify = pEvent;
	}
	hr = pNotify->SetNotificationPositions(CAPTURE_FRAME_COUNT,dsPosNotify);
    return hr;
}

long CDirectSoundAecCapture::Start(void)
{
	m_dwNextCapOffset = 0;
	runThreadFlag = true;
	pCapThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)(CaptureThreadEntry),(LPVOID)this,0,NULL);
	pDSCBuffer8->Start(DSCBSTART_LOOPING);
	return 0;
}


long CDirectSoundAecCapture::Stop(void)
{
	m_dwNextCapOffset = 0;
	runThreadFlag = false;
	::SetEvent(pEvent);
	WaitForSingleObject(pCapThread,INFINITE);
	return 0;
}

long CDirectSoundAecCapture::SetEvent(IDirectSoundCaptureEvent *receiver)
{
	_event = receiver;
	return 0;
}

long CDirectSoundAecCapture::_ProcCapture()
{
	if( NULL == pDSCBuffer8 )
		return S_FALSE; 

	HRESULT hr;
	VOID*   pbCaptureData    = NULL;
	DWORD   dwCaptureLength;
	VOID*   pbCaptureData2   = NULL;
	DWORD   dwCaptureLength2;
	DWORD   dwReadPos;
	DWORD   dwCapturePos;
	unsigned long capFrameSize = 0;
	while(runThreadFlag)
	{
		WaitForSingleObject(pEvent,INFINITE);
		if(!runThreadFlag)
		{
			break;
		}

		capFrameSize = 0;
		pbCaptureData    = NULL;
		pbCaptureData2   = NULL;
		
		LONG lLockSize; 
		

		if( FAILED( hr = pDSCBuffer8->GetCurrentPosition( &dwCapturePos, &dwReadPos ) ) )
			continue; 
		lLockSize = dwReadPos - m_dwNextCapOffset;
		if( lLockSize < 0 )
			lLockSize += captureBufferSize; 

		lLockSize -= (lLockSize % frameSize); 
		if( lLockSize == 0 )
			continue; 

		if( FAILED( hr = pDSCBuffer8->Lock( m_dwNextCapOffset, lLockSize,
			&pbCaptureData, &dwCaptureLength, 
			&pbCaptureData2, &dwCaptureLength2, 0L ) ) )
			continue ; 

		if(_event) {
			memcpy(pCaptureDataTemp,pbCaptureData,dwCaptureLength); 
			capFrameSize = dwCaptureLength;
		}

		m_dwNextCapOffset += dwCaptureLength; 
		m_dwNextCapOffset %= captureBufferSize;
		if( pbCaptureData2 != NULL ) {
			if(_event) {
				memcpy(pCaptureDataTemp + capFrameSize,pbCaptureData2,dwCaptureLength2); 
				capFrameSize += dwCaptureLength2;
			} 
			m_dwNextCapOffset += dwCaptureLength2; 
			m_dwNextCapOffset %= captureBufferSize; 
		} 
		pDSCBuffer8->Unlock( pbCaptureData,  dwCaptureLength, pbCaptureData2, dwCaptureLength2 ); 
		if(_event)
		{
			_event->OnDSCapturedData(pCaptureDataTemp,capFrameSize,16000);
		}
	}
	
	return 0;
}
