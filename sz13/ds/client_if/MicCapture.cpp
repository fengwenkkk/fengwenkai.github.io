#include "windows.h"
#include "MicCapture.h"
#include <mmsystem.h>
#include <dsound.h>

#include "process.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#endif

#ifndef MAX
#define MAX(a,b)        ( (a) > (b) ? (a) : (b) )
#endif

#define NUM_REC_NOTIFICATIONS   16

class CAdoFrameHandler
{
public:
    virtual void AdoFrameData(BYTE* pBuffer, long lBufferSize) = 0;
};

class CCaptureAudio
{
public:
    BOOL        m_bRecording ;  //recording now ? also used by event recv thread

protected:
    LPDIRECTSOUNDCAPTURE8    m_pCapDev ;   //capture device ptr
    LPDIRECTSOUNDCAPTUREBUFFER m_pCapBuf ;   //capture loop buffer ptr
    LPDIRECTSOUNDNOTIFY8    m_pNotify ;   //capture auto-notify event callback handler ptr
    GUID                m_guidCapDevId ;  //capture device id
    WAVEFORMATEX        m_wfxInput;   //input wave format description struct
    DSBPOSITIONNOTIFY   m_aPosNotify[NUM_REC_NOTIFICATIONS + 1]; //notify flag array
    HANDLE              m_hNotifyEvent;   //notify event
    BOOL                m_abInputFmtSupported[20];
    DWORD               m_dwCapBufSize;  //capture loop buffer size
    DWORD               m_dwNextCapOffset;//offset in loop buffer
    DWORD               m_dwNotifySize;  //notify pos when loop buffer need to emit the event
    CAdoFrameHandler*   m_frame_handler ; // outer frame data dealer ptr

public: // callback func to add enum devices string name
    static BOOL CALLBACK enum_dev_proc(LPGUID lpGUID, LPCTSTR lpszDesc, LPCTSTR lpszDrvName, LPVOID lpContext ) ;
    static UINT notify_capture_thd(LPVOID data) ;

protected:
    HRESULT InitDirectSound(GUID dev_id = GUID_NULL) ;
    HRESULT FreeDirectSound() ;
    HRESULT InitNotifications() ;
    HRESULT CreateCaptureBuffer(WAVEFORMATEX * wfx) ;
    HRESULT StartOrStopRecord(BOOL bStartRec) ;
    HRESULT RecordCapturedData() ;
    void    SetWavFormat(WAVEFORMATEX * wfx) ;

    int m_iSampleRate;
    int m_iBitsPerSample;
    int m_iChanNum;

public:
    CCaptureAudio(int iSampleRate, int iBitsPerSample, int iChanNum, int iBufSize);
    ~CCaptureAudio(void);

    BOOL EnumDevices(HWND hList) ;
    BOOL Open(void) ;
    BOOL Close() ;
    void GrabAudioFrames(BOOL bGrabAudioFrames, CAdoFrameHandler* frame_handler) ;
};

HANDLE  m_threadHandle = NULL;

CCaptureAudio::CCaptureAudio(int iSampleRate, int iBitsPerSample, int iChanNum, int iBufSize)
{
    if (FAILED(CoInitialize(NULL))) /*, COINIT_APARTMENTTHREADED)))*/
    {
        return;
    }

    m_pCapDev = NULL;
    m_pCapBuf = NULL;
    m_pNotify = NULL;
    // set default wave format PCM
    ZeroMemory( &m_wfxInput, sizeof(m_wfxInput));
    m_wfxInput.wFormatTag = WAVE_FORMAT_PCM;
    m_guidCapDevId = GUID_NULL;
    m_bRecording = FALSE;
    m_hNotifyEvent = NULL;

    m_iSampleRate = iSampleRate;
    m_iBitsPerSample = iBitsPerSample;
    m_iChanNum = iChanNum;
    m_dwNotifySize = iBufSize;
}

CCaptureAudio::~CCaptureAudio(void)
{
    CoUninitialize();
}

BOOL CALLBACK CCaptureAudio::enum_dev_proc(LPGUID lpGUID, LPCTSTR lpszDesc,
                                           LPCTSTR lpszDrvName, LPVOID lpContext)
{
    HWND   hList = (HWND)lpContext;
    LPGUID lpTemp = NULL;

    if(!hList)
    {
        return FALSE;
    }

    if (lpGUID != NULL)
    {
        // NULL only for "Primary Sound Driver".
        if ((lpTemp = (LPGUID)malloc(sizeof(GUID))) == NULL)
        {
            return TRUE;
        }

        memcpy(lpTemp, lpGUID, sizeof(GUID));
    }

    ::SendMessage(hList, CB_ADDSTRING, 0,(LPARAM)lpszDesc);
    ::SendMessage(hList, LB_SETITEMDATA, 0, (LPARAM)lpTemp);
    free(lpTemp);

    return TRUE;
}

UINT CCaptureAudio::notify_capture_thd(LPVOID data)
{
    CCaptureAudio * pado = static_cast<CCaptureAudio *>(data);
    MSG   msg;
 // HRESULT hr;
    DWORD dwResult;

    while(pado->m_bRecording)
    {
        dwResult = MsgWaitForMultipleObjects( 1, &(pado->m_hNotifyEvent), FALSE, INFINITE, QS_ALLEVENTS );
        switch( dwResult )
        {
            case WAIT_OBJECT_0 + 0:
                // g_hNotificationEvents[0] is signaled
                // This means that DirectSound just finished playing
                // a piece of the buffer, so we need to fill the circular
                // buffer with new sound from the wav file
                if( FAILED( pado->RecordCapturedData() ) )
                {
                //  AfxMessageBox("Error handling DirectSound notifications.");
                  pado->m_bRecording = FALSE;
                }
                break;

            case WAIT_OBJECT_0 + 1:
                // Windows messages are available
                while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
                {
                    TranslateMessage( &msg );
                    DispatchMessage( &msg );
                    if( msg.message == WM_QUIT )
                    {
                        pado->m_bRecording = FALSE;
                    }
                }
                break;
        }
    }

   //   AfxEndThread(0, TRUE);
    ::CloseHandle(m_threadHandle);

    return 0;
}

BOOL CCaptureAudio::EnumDevices(HWND hList)
{
    if (FAILED(DirectSoundCaptureEnumerate (
        (LPDSENUMCALLBACK)(CCaptureAudio::enum_dev_proc),
        (VOID*)&hList)))
    {
        return FALSE;
    }

    return TRUE;
}

/**
 * @brief           创建DirectSound对象
 * @return          BOOL 失败返回false
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *  
 * @endcode
 * @note
 */
BOOL CCaptureAudio::Open(void)
{
    HRESULT hr = S_OK;

    if (!m_bRecording)
    {
        hr = InitDirectSound();
    }

    if (hr != S_OK)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CCaptureAudio::Close()
{
    HRESULT hr = FreeDirectSound();

    CloseHandle(m_hNotifyEvent);

    return (FAILED(hr)) ? FALSE : TRUE;
}

HRESULT CCaptureAudio::InitDirectSound(GUID dev_id)
{
    HRESULT hr;

    m_guidCapDevId = dev_id;
    ZeroMemory( &m_aPosNotify, sizeof(DSBPOSITIONNOTIFY) * (NUM_REC_NOTIFICATIONS + 1) );

    m_dwCapBufSize = 0;
  //  m_dwNotifySize = 0;

    // Create IDirectSoundCapture using the preferred capture device
    hr = DirectSoundCaptureCreate(&m_guidCapDevId, &m_pCapDev, NULL);

    if (!m_pCapDev)
    {
        return S_FALSE;
    }

    // init wave format
    SetWavFormat(&m_wfxInput);

    return (FAILED(hr)) ? S_FALSE : S_OK;
}

HRESULT CCaptureAudio::FreeDirectSound()
{
    // Release DirectSound interfaces
    SAFE_RELEASE( m_pNotify );
    SAFE_RELEASE( m_pCapBuf );
    SAFE_RELEASE( m_pCapDev );

    return S_OK;
}

HRESULT CCaptureAudio::CreateCaptureBuffer(WAVEFORMATEX * wfx)
{
//  HRESULT hr;
    DSCBUFFERDESC dscbd;

    SAFE_RELEASE( m_pNotify );
    SAFE_RELEASE( m_pCapBuf );

    // Set the notification size
 //   m_dwNotifySize = MAX( 1024, wfx->nAvgBytesPerSec / 8 );  // 4000
    m_dwNotifySize -= m_dwNotifySize % wfx->nBlockAlign;
    // Set the buffer sizes
    m_dwCapBufSize = m_dwNotifySize * NUM_REC_NOTIFICATIONS;
    SAFE_RELEASE( m_pNotify );
    SAFE_RELEASE( m_pCapBuf );

    // Create the capture buffer
    ZeroMemory( &dscbd, sizeof(dscbd) );
    dscbd.dwSize        = sizeof(dscbd);
    dscbd.dwBufferBytes = m_dwCapBufSize;
    dscbd.lpwfxFormat   = wfx; // Set the format during creatation

    if (!m_pCapDev)
    {
        return FALSE;
    }

    if( FAILED( m_pCapDev->CreateCaptureBuffer( &dscbd, &m_pCapBuf, NULL ) ) )
    {
        return S_FALSE;
    }

    m_dwNextCapOffset = 0;
    if( FAILED( InitNotifications() ) )
    {
        return S_FALSE;
    }

    return S_OK;
}

HRESULT CCaptureAudio::InitNotifications()
{
 // HRESULT hr;
    int i;

    if( NULL == m_pCapBuf )
    {
        return S_FALSE;
    }

    // create auto notify event
    m_hNotifyEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

    // Create a notification event, for when the sound stops playing
    if( FAILED( m_pCapBuf->QueryInterface( IID_IDirectSoundNotify, (VOID**)&m_pNotify ) ) )
    {
        return S_FALSE;
    }

    // Setup the notification positions
    for( i = 0; i < NUM_REC_NOTIFICATIONS; i++ )
    {
        m_aPosNotify[i].dwOffset = (m_dwNotifySize * i) + m_dwNotifySize - 1;
        m_aPosNotify[i].hEventNotify = m_hNotifyEvent;
    }

    // Tell DirectSound when to notify us. the notification will come in the from
    // of signaled events that are handled in WinMain()
    if( FAILED( m_pNotify->SetNotificationPositions( NUM_REC_NOTIFICATIONS, m_aPosNotify ) ) )
    {
        return S_FALSE;
    }

    return S_OK;
}

unsigned int __stdcall __notify_capture_thd(LPVOID data)
{
    return CCaptureAudio::notify_capture_thd(data);
}

HRESULT CCaptureAudio::StartOrStopRecord(BOOL bStartRec)
{
//  HRESULT hr;

    if( bStartRec )
    {
        // Create a capture buffer, and tell the capture
        // buffer to start recording
        if( FAILED( CreateCaptureBuffer( &m_wfxInput ) ) )
        {
            return S_FALSE;
        }

        if( FAILED( m_pCapBuf->Start( DSCBSTART_LOOPING ) ) )
        {
            return S_FALSE;
        }

        // create notify event recv thread
       //   AfxBeginThread(CCaptureAudio::notify_capture_thd, (LPVOID)(this));
        DWORD threadId = 0;

        m_threadHandle = (HANDLE)::_beginthreadex(NULL, 0, __notify_capture_thd, (LPVOID)this, 0, (unsigned int*)&threadId);

    }
    else
    {
        // Stop the capture and read any data that
        // was not caught by a notification
        if( NULL == m_pCapBuf )
        {
            return S_OK;
        }

        // wait until the notify_event_thd thread exit and release the resources.
        Sleep(500);
        // Stop the buffer, and read any data that was not
        // caught by a notification
        if( FAILED( m_pCapBuf->Stop() ) )
        {
            return S_OK;
        }

        if( FAILED( RecordCapturedData() ) )
        {
            return S_FALSE;
        }
    }

    return S_OK;
}

HRESULT CCaptureAudio::RecordCapturedData()
{
 // HRESULT hr;
    VOID*   pbCaptureData    = NULL;
    DWORD   dwCaptureLength;
    VOID*   pbCaptureData2   = NULL;
    DWORD   dwCaptureLength2;
    DWORD   dwReadPos;
    DWORD   dwCapturePos;
    LONG lLockSize;

    if( NULL == m_pCapBuf )
    {
        return S_FALSE;
    }

    if( FAILED( m_pCapBuf->GetCurrentPosition( &dwCapturePos, &dwReadPos ) ) )
    {
        return S_FALSE;
    }

    lLockSize = dwReadPos - m_dwNextCapOffset;
    if( lLockSize < 0 )
    {
        lLockSize += m_dwCapBufSize;
    }

    // Block align lock size so that we are always write on a boundary
    lLockSize -= (lLockSize % m_dwNotifySize);
    if( lLockSize == 0 )
    {
        return S_FALSE;
    }

    // Lock the capture buffer down
    if( FAILED( m_pCapBuf->Lock( m_dwNextCapOffset, lLockSize,
        &pbCaptureData, &dwCaptureLength,
        &pbCaptureData2, &dwCaptureLength2, 0L ) ) )
    {
        return S_FALSE;
    }

    // call the outer data handler
    if(m_frame_handler)
    {
        m_frame_handler->AdoFrameData((BYTE*)pbCaptureData, dwCaptureLength);
    }

    // Move the capture offset along
    m_dwNextCapOffset += dwCaptureLength;
    m_dwNextCapOffset %= m_dwCapBufSize; // Circular buffer
    if( pbCaptureData2 != NULL )
    {
        // call the outer data handler
        if(m_frame_handler)
        {
            m_frame_handler->AdoFrameData((BYTE*)pbCaptureData, dwCaptureLength);
        }
        // Move the capture offset along
        m_dwNextCapOffset += dwCaptureLength2;
        m_dwNextCapOffset %= m_dwCapBufSize; // Circular buffer
    }

    // Unlock the capture buffer
    m_pCapBuf->Unlock( pbCaptureData,  dwCaptureLength, pbCaptureData2, dwCaptureLength2 );
    return S_OK;
}

void CCaptureAudio::SetWavFormat(WAVEFORMATEX * wfx)
{
    // get the default capture wave formate
    ZeroMemory(wfx, sizeof(WAVEFORMATEX));

    wfx->wFormatTag = WAVE_FORMAT_PCM;
    // 16KHz, 16 bits PCM, Mono
    wfx->nSamplesPerSec = m_iSampleRate;// 16000
    wfx->wBitsPerSample = m_iBitsPerSample;//16;
    wfx->nChannels  = m_iChanNum;//1;
    wfx->nBlockAlign = wfx->nChannels * ( wfx->wBitsPerSample / 8 );  // 2
    wfx->nAvgBytesPerSec = wfx->nBlockAlign * wfx->nSamplesPerSec;    // 32000
}

void CCaptureAudio::GrabAudioFrames(BOOL bGrabAudioFrames, CAdoFrameHandler* frame_handler)
{
    m_frame_handler = frame_handler;
    m_bRecording = bGrabAudioFrames;
    StartOrStopRecord(m_bRecording);
}

#if 0
/*
使用的时候，也很简单，我这里声明了一个纯虚类CAdoFrameHandler，这个类专门是用来让使用者重载它的纯虚函数的，
只要重载了以后，设置正确，就可以自动开始采集音频数据了。注意，在这个类里面，我用的是8KHz,16Bits,Mono单声道的PCM数据采集。
使用的时候，首先：
#include "CaptureAudio.h"
然后：
*/
class CMyClass : public CAdoFrameHandler
{

public: // override the CAdoFrameHandler
    void AdoFrameData(BYTE* pBuffer, long lBufferSize);  // 这个类重载一下，就可以采集了

protected:
    CCaptureAudio   m_cap_ado; // 这个对象就是用来采集音频数据的
};

在OnInitDialog类中，我们可以使用如下初始化方法：

打开mic，同时初始化directsound：
m_cap_ado.Open();

开始采集声音就是：
m_cap_ado.GrabAudioFrames(TRUE, this);
调用它以后，只要你重载了上面的那个函数，directsound就会周期性的从麦克采集数据,然后调用该函数。

停止声音采集是：
m_cap_ado.GrabAudioFrames(FALSE, NULL);

关闭mic，同时释放directsound：
m_cap_ado.Close();

#endif

class MICCap : public CAdoFrameHandler
{
public: // override the CAdoFrameHandler
    MICCap(int iSampleRate, int iBitsPerSample, int iChanNum, int iBufSize);
    ~MICCap();

    void AdoFrameData(BYTE* pBuffer, long lBufferSize) ;  // 这个类重载一下，就可以采集了

//protected:
    ON_MIC_DATA     m_pfOnMicData;
    CCaptureAudio   *m_Capture; // 这个对象就是用来采集音频数据的
};

//MicCap    *g_MicCap = NULL;

MICCap::MICCap(int iSampleRate, int iBitsPerSample, int iChanNum, int iBufSize)
{
    m_Capture = new CCaptureAudio(iSampleRate, iBitsPerSample, iChanNum, iBufSize);
}

MICCap::~MICCap()
{
    if (m_Capture)
    {
        delete m_Capture;
    }
}

void MICCap::AdoFrameData(BYTE* pBuf, long ulBufSize)
{
    if (m_pfOnMicData)
    {
        m_pfOnMicData((unsigned char*)pBuf, ulBufSize);
    }

//  ::OnMicData((UINT8*)pBuf, (UINT32)ulBufSize);
}

MicCapture::MicCapture(ON_MIC_DATA pfOnMicData)
{
    m_MicCap = NULL;
    m_pfOnMicData = pfOnMicData;
}

MicCapture::~MicCapture()
{
    Close();
}

/**
 * @brief           new出麦克声音采集对象m_MicCap并将音频数据处理函数注册到m_MicCap
 * @param           iSampleRate    [in] 采样频率 16000
 * @param           iBitsPerSample    [in] 采样位数 16
 * @param           iChanNum    [in] 声道数  // 1单声道
 * @param           iBufSize    [in] 缓冲区大小
 * @return          BOOL 
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *  
 * @endcode
 * @note
 */
BOOL MicCapture::Open(int iSampleRate, int iBitsPerSample, int iChanNum, int iBufSize)
{
    Close();

    m_MicCap = new MICCap(iSampleRate, iBitsPerSample, iChanNum, iBufSize);
    ((MICCap*)m_MicCap)->m_pfOnMicData = m_pfOnMicData;

    if (!((MICCap*)m_MicCap)->m_Capture)
    {
        return FALSE;
    }

    if (!((MICCap*)m_MicCap)->m_Capture->Open())//如果创建DirectSound对象失败直接false
    {
        Close();
        return FALSE;
    }

    return TRUE;
}

void MicCapture::Close()
{
    Stop();
    if (m_MicCap)
    {
        MICCap  *pMicCap = (MICCap*)m_MicCap;

        if (pMicCap->m_Capture)
        {
            pMicCap->m_Capture->Close();
        }

        delete pMicCap;
        m_MicCap = NULL;
    }
}

BOOL MicCapture::Start()
{
    if (!m_MicCap)
    {
        return FALSE;
    }

    if (!((MICCap*)m_MicCap)->m_Capture)
    {
        return FALSE;
    }

    ((MICCap*)m_MicCap)->m_Capture->GrabAudioFrames(TRUE, ((MICCap*)m_MicCap));

    return TRUE;
}

void MicCapture::Stop()
{
    if (m_MicCap && ((MICCap*)m_MicCap)->m_Capture)
    {
        ((MICCap*)m_MicCap)->m_Capture->GrabAudioFrames(FALSE, NULL);
    }
}

