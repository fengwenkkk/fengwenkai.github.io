//---------------------------------------------------------------------------
// Copyright © Victor Chen, http://www.cppfans.com/
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "VictorMediaPlayer.h"

//#include <tchar.h>

#pragma option push
#  pragma option -w-8012 // [bcc32 Warning] amvideo.h(382): W8012 Comparing signed and unsigned values
#  include <dshow.h>
#pragma option pop

#include <initguid.h> // Enable DEFINE_GUID()
//---------------------------------------------------------------------------
namespace TVictorClasses {
//---------------------------------------------------------------------------
#define WM_GRAPHNOTIFY (WM_USER+13)
//---------------------------------------------------------------------------
#ifdef sprintf
#  undef sprintf
#endif
#ifdef swprintf
#  undef swprintf
#endif
//---------------------------------------------------------------------------
#define _TRY(x)     x;

#if 0

#define _TRY(x)                                                     \
{                                                                   \
  HRESULT hr;                                                       \
  if(FAILED(hr=(x)))                                                \
   {                                                                \
     String sMsg;                                                   \
     sMsg.sprintf(_T("FAILED(hr=0x%x) in ") _T(#x) _T("\n\0"), hr); \
     throw EVictorMediaPlayer(sMsg, hr);                            \
   }                                                                \
}

#endif

//---------------------------------------------------------------------------
DEFINE_GUID(CLSID_LavSplitter_Source,0xB98D13E7,0x55DB,0x4385,0xA3,0x3D,0x09,0xFD,0x1B,0xA2,0x63,0x38); // {B98D13E7-55DB-4385-A33D-09FD1BA26338}
DEFINE_GUID(CLSID_LavVideoDecoder   ,0xEE30215D,0x164F,0x4A92,0xA4,0xEB,0x9D,0x4C,0x13,0x39,0x0F,0x9F); // {EE30215D-164F-4A92-A4EB-9D4C13390F9F}
DEFINE_GUID(CLSID_LavAudioDecoder   ,0xE8E73B6B,0x4CB3,0x44A4,0xBE,0x99,0x4F,0x7B,0xCB,0x96,0xE4,0x91); // {E8E73B6B-4CB3-44A4-BE99-4F7BCB96E491}
//---------------------------------------------------------------------------
///////////////////////////// EVictorMediaPlayer ////////////////////////////
//---------------------------------------------------------------------------
EVictorMediaPlayer::EVictorMediaPlayer(UnicodeString sMsg, HRESULT hr)
 : Exception(sMsg)
{
  _hResult = hr;
}
//---------------------------------------------------------------------------
///////////////// TVictorMediaPlayer::TDirectShowInterfaces /////////////////
//---------------------------------------------------------------------------
class TVictorMediaPlayer::TDirectShowInterfaces
{
public:
  IGraphBuilder   *lpGraphBuilder;
  IMediaControl   *lpMediaControl;
  IMediaEventEx   *lpMediaEventEx;
  IMediaSeeking   *lpMediaSeeking;
  IMediaPosition  *lpMediaPosition;
  IVideoWindow    *lpVideoWindow;
  IBasicAudio     *lpBasicAudio;
  IBasicVideo     *lpBasicVideo;
  IVideoFrameStep *lpVideoFrameStep;

  DWORD dwGraphRegister;

  IBaseFilter       *lpLavSplitterSource;
  IFileSourceFilter *lpFileSourceFilter;
  IBaseFilter       *lpLavVideoDecoder;
  IBaseFilter       *lpLavAudioDecoder;
  IBaseFilter       *lpVideoRenderer;
  IBaseFilter       *lpAudioRender;

  bool bLavSplitterOK;
  bool bLavVideoOK;
  bool bLavAudioOK;

  HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
  void RemoveFromRot(DWORD pdwRegister);

  HRESULT AddFilterByCLSID(IGraphBuilder *pGraph, REFGUID clsid, IBaseFilter **ppF, LPCWSTR wszName=NULL);
  HRESULT GetUnconectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin);
  HRESULT ConnectFilters(IGraphBuilder *pGraph, IPin *pOut, IBaseFilter *pDest);
  HRESULT ConnectFilters( IGraphBuilder *pGraph, IBaseFilter *pSrc, IBaseFilter *pDest);

  bool AddFilters(LPCOLESTR pszFileName, const AM_MEDIA_TYPE *pmt);
  void RemoveFilters(void);
  void CloseInterfaces(void);

  TDirectShowInterfaces();
  ~TDirectShowInterfaces();
};
//---------------------------------------------------------------------------
///////////////////////////// TVictorMediaPlayer ////////////////////////////
//---------------------------------------------------------------------------
__fastcall TVictorMediaPlayer::TVictorMediaPlayer()
{
    _hWnd = NULL;
    _hMsgWnd = AllocateHWnd(WndProc);
    _hWndPlaying = NULL;

    _lVolume = VOLUME_FULL;
    _psCurrent = vmpsInit;
    _bAudioOnly = false;
    _bAspectRatio = true;

    _lpfnStateChanged = NULL;

    _ds = new TDirectShowInterfaces;
}
//---------------------------------------------------------------------------
__fastcall TVictorMediaPlayer::~TVictorMediaPlayer()
{
    fClose();
    if(_ds)
    {
        delete _ds;
        _ds = NULL;
    }

    DeallocateHWnd(_hMsgWnd);
}
//---------------------------------------------------------------------------
void TVictorMediaPlayer::PlayStateChanged(TVictorMediaPlayState NewSt)
{
    if(_psCurrent != NewSt)
    {
        TVictorMediaPlayState OldSt = _psCurrent;
        _psCurrent = NewSt;

        if(_lpfnStateChanged)
        {
            _lpfnStateChanged(this, NewSt, OldSt);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TVictorMediaPlayer::WndProc(Messages::TMessage &Message)
{
    if(Message.Msg == WM_GRAPHNOTIFY)
    {
        HandleGraphEvent();
    }

    Message.Result = DefWindowProc(_hMsgWnd, Message.Msg, Message.WParam, Message.LParam);
}
//---------------------------------------------------------------------------
void TVictorMediaPlayer::PlayMovieInWindow(void)
{
  // Get the interface for DirectShow's GraphBuilder
    _TRY(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&_ds->lpGraphBuilder))

    _TRY(_ds->AddToRot(_ds->lpGraphBuilder, &_ds->dwGraphRegister))
    if(!_ds->AddFilters(_wsFileName.c_str(), NULL))
    {
        // Have the graph builder construct its the appropriate graph automatically
         _TRY(_ds->lpGraphBuilder->RenderFile(_wsFileName.c_str(), NULL))
    }

    // QueryInterface for DirectShow interfaces
    _TRY(_ds->lpGraphBuilder->QueryInterface(IID_IMediaControl, (void **)&_ds->lpMediaControl))
    _TRY(_ds->lpGraphBuilder->QueryInterface(IID_IMediaEventEx, (void **)&_ds->lpMediaEventEx))
    _TRY(_ds->lpGraphBuilder->QueryInterface(IID_IMediaSeeking, (void **)&_ds->lpMediaSeeking))
    _TRY(_ds->lpGraphBuilder->QueryInterface(IID_IMediaPosition, (void **)&_ds->lpMediaPosition))

    // Query for video interfaces, which may not be relevant for audio files
    _TRY(_ds->lpGraphBuilder->QueryInterface(IID_IVideoWindow, (void **)&_ds->lpVideoWindow))
    _TRY(_ds->lpGraphBuilder->QueryInterface(IID_IBasicVideo, (void **)&_ds->lpBasicVideo))

    // Query for audio interfaces, which may not be relevant for video-only files
    _TRY(_ds->lpGraphBuilder->QueryInterface(IID_IBasicAudio, (void **)&_ds->lpBasicAudio))

    // Is this an audio-only file (no video component)?
    CheckVisibility();

    // Have the graph signal event via window callbacks for performance
    _TRY(_ds->lpMediaEventEx->SetNotifyWindow((OAHWND)_hMsgWnd, WM_GRAPHNOTIFY, 0))

    if(!_bAudioOnly)
    {
        SetPlayingHWnd(_hWnd);
        GetFrameStepInterface();
    }

    // Run the graph to play the media file
    PlayStateChanged(vmpsStopped);
}
//---------------------------------------------------------------------------
// Some video renderers support stepping media frame by frame with the IVideoFrameStep interface.
// See the interface documentation for more details on frame stepping.
BOOL TVictorMediaPlayer::GetFrameStepInterface(void)
{
  HRESULT hr;
  IVideoFrameStep *pFSTest = NULL;

  hr = _ds->lpGraphBuilder->QueryInterface(__uuidof(IVideoFrameStep), (PVOID *)&pFSTest); // Get the frame step interface, if supported
  if(FAILED(hr))
    return FALSE;

  hr = pFSTest->CanStep(0L, NULL); // Check if this decoder can step
  if (hr == S_OK)
   {
     _ds->lpVideoFrameStep = pFSTest;  // Save interface to global variable for later use
     return TRUE;
   }
  else
   {
     pFSTest->Release();
     return FALSE;
   }
}
//---------------------------------------------------------------------------
void TVictorMediaPlayer::CheckVisibility(void)
{
  long lVisible;
  HRESULT hr;

  if ((!_ds->lpVideoWindow) || (!_ds->lpBasicVideo))
   {
     _bAudioOnly = TRUE; // Audio-only files have no video interfaces.  This might also be a file whose video component uses an unknown video codec.
     return;
   }
  else
   {
     _bAudioOnly = FALSE; // Clear the global flag
   }

  hr = _ds->lpVideoWindow->get_Visible(&lVisible);
  if(FAILED(hr))
   {
     if (hr == E_NOINTERFACE)   // If this is an audio-only clip, get_Visible() won't work.
     {                          // Also, if this video is encoded with an unsupported codec,
       _bAudioOnly = TRUE;      // we won't see any video, although the audio will work if it is
     }                          // of a supported format.
     else
     {
       // _Msg(_T("Failed(%08lx) in lpVideoWindow->get_Visible()!\r\n"), hr);
     }
   }
}
//---------------------------------------------------------------------------
void TVictorMediaPlayer::GetVideoSize(long &w, long &h)
{
    w=0;
    h=0;
    if(_ds->lpBasicVideo)
    {
        _ds->lpBasicVideo->GetVideoSize(&w, &h);
    }
}
//---------------------------------------------------------------------------
void TVictorMediaPlayer::SetVideoPos(long x, long y, long w, long h)
{
    if(_ds->lpVideoWindow)
    {
        _ds->lpVideoWindow->SetWindowPosition(x,y,w,h);
    }
}
//---------------------------------------------------------------------------
void TVictorMediaPlayer::fSetAspRatio(bool b)
{
    _bAspectRatio = b;
    AdjustVideoSize();
}
//---------------------------------------------------------------------------
void TVictorMediaPlayer::AdjustVideoSize(void)
{
    long vw, vh;
    GetVideoSize(vw,vh);
    if(vw>0 && vh>0)
    {
        HWND h = hWndPlaying;
        if(!h)
        {
            h=_hMsgWnd;
        }

        TRect r;
        GetClientRect(h,&r);
        int ww = r.Width(), hh = r.Height();

        if(_bAspectRatio)
        {
            int wa = vw*hh/vh;
            int ha = vh*ww/vw;
            if(ww>wa)ww=wa;
            if(hh>ha)hh=ha;
        }
        SetVideoPos((r.Width()-ww)/2,(r.Height()-hh)/2,ww,hh);
    }
}
//---------------------------------------------------------------------------
void TVictorMediaPlayer::Pause(void)
{
    if(_ds->lpMediaControl)
    {
        if(SUCCEEDED(_ds->lpMediaControl->Pause()))
        {
            PlayStateChanged(vmpsPaused);
        }
    }
}
//---------------------------------------------------------------------------
void TVictorMediaPlayer::Stop(void)
{
    if ((!_ds->lpMediaControl) || (!_ds->lpMediaSeeking))
    {
        return;
    }

    if((PlayState == vmpsPaused) || (PlayState == vmpsRunning)) // Stop and reset postion to beginning
    {
        LONGLONG pos = 0;
        _ds->lpMediaControl->Stop();
        PlayStateChanged(vmpsStopped);
        _ds->lpMediaSeeking->SetPositions(&pos, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning); // Seek to the beginning
        _ds->lpMediaControl->Pause(); // Display the first frame to indicate the reset condition
    }
}
//---------------------------------------------------------------------------
void TVictorMediaPlayer::fSetFileName(UnicodeString ws)
{
    fClose();
    _wsFileName = ws;
}
//---------------------------------------------------------------------------
void TVictorMediaPlayer::fClose(void)
{
    if(_ds->lpMediaControl)
    {
        _ds->lpMediaControl->Stop(); //Stop media playback
    }

    PlayStateChanged(vmpsStopped); // Clear global flags
    _ds->CloseInterfaces(); // Free DirectShow interfaces
    PlayStateChanged(vmpsInit); // No current media state
}
//---------------------------------------------------------------------------
void TVictorMediaPlayer::fOpen(void)
{
    fClose();
    try
    {
        PlayMovieInWindow(); // Start playing the media file
    }
    catch(Exception &e)
    {
        fClose(); // If we couldn't play the clip, clean up

        EVictorMediaPlayer *emp = dynamic_cast<EVictorMediaPlayer*>(&e);
        if(emp)
        {
            throw EVictorMediaPlayer(emp->Message, emp->hResult);
        }
        else
        {
            throw Exception(e.Message);
        }
    }

    fSetVolume(_lVolume);
}
//---------------------------------------------------------------------------
void TVictorMediaPlayer::Play(void)
{
    if(PlayState == vmpsInit)
    {
        fOpen();
    }

    if((PlayState == vmpsPaused) || (PlayState == vmpsStopped))
    {
        if(_ds->lpMediaControl)
        {
            if(SUCCEEDED(_ds->lpMediaControl->Run()))
            {
                PlayStateChanged(vmpsRunning);
            }
        }
    }
}
//---------------------------------------------------------------------------
void TVictorMediaPlayer::fSetVolume(LONG NewVol)
{
  _lVolume = NewVol;
  if(_ds->lpGraphBuilder && _ds->lpBasicAudio)
   {
     // Read current volume
     LONG lOldVol;
     HRESULT hr = _ds->lpBasicAudio->get_Volume(&lOldVol);
     if(hr == E_NOTIMPL)
      {
        // Fail quietly if this is a video-only media file
        return;
      }
     else if (FAILED(hr))
      {
        // _Msg(_T("Failed to read audio volume!  hr=0x%x\r\n"), hr);
        return;
      }
     // Set new volume
     hr = _ds->lpBasicAudio->put_Volume(_lVolume);
     if(FAILED(hr))
      {
        //ignore this error
      }
   }
}
//---------------------------------------------------------------------------
void TVictorMediaPlayer::fSetHWnd(HWND h)
{
  SetPlayingHWnd(_hWnd=h);
}
//---------------------------------------------------------------------------
void TVictorMediaPlayer::SetPlayingHWnd(HWND h)
{
  _hWndPlaying = h?h:_hMsgWnd; // 如果没设窗口句柄，此时可能是不希望显示视频，把视频显示到隐藏的消息处理窗口里面，避免出现不可控的视频窗口
  if(_ds->lpVideoWindow)
   {
     _ds->lpVideoWindow->put_Visible(OAFALSE);
     _ds->lpVideoWindow->put_Owner((OAHWND)_hWndPlaying);
     _ds->lpVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
     AdjustVideoSize(); // _ds->lpVideoWindow->SetWindowPosition(0,0,r.Width(),r.Height());
     _ds->lpVideoWindow->put_MessageDrain((OAHWND)_hWndPlaying);
     _ds->lpVideoWindow->put_Visible(OATRUE);
   }
}
//---------------------------------------------------------------------------
double TVictorMediaPlayer::fGetRate(void)
{
  if(_ds->lpMediaPosition) // If the IMediaPosition interface exists, use it to set rate
   {
     double dRate = 1.0;
     if(SUCCEEDED(_ds->lpMediaPosition->get_Rate(&dRate)))
      {
        return dRate;
      }
   }
  return 1.0;
}
//---------------------------------------------------------------------------
void TVictorMediaPlayer::fSetRate(double r)
{
  if (_ds->lpMediaPosition) // If the IMediaPosition interface exists, use it to set rate
   {
        if(SUCCEEDED(_ds->lpMediaPosition->put_Rate(r)))
      {
      }
   }
}
//---------------------------------------------------------------------------
REFTIME TVictorMediaPlayer::fGetDuration(void)
{
    REFTIME d = 0;
    if(_ds->lpMediaPosition)
    {
        _ds->lpMediaPosition->get_Duration(&d);
    }

    return d;
}
//---------------------------------------------------------------------------
double TVictorMediaPlayer::fGetPosition(void)
{
  REFTIME d = 0;
  if(_ds->lpMediaPosition)
    _ds->lpMediaPosition->get_CurrentPosition(&d);
  return d;
}
//---------------------------------------------------------------------------
void TVictorMediaPlayer::fSetPosition(double d)
{
  if(_ds->lpMediaPosition)
    _ds->lpMediaPosition->put_CurrentPosition(d);
}
//---------------------------------------------------------------------------
bool TVictorMediaPlayer::fGetLavSplitterOK(void)
{
  return _ds->bLavSplitterOK;
}
//---------------------------------------------------------------------------
bool TVictorMediaPlayer::fGetLavVideoOK(void)
{
  return _ds->bLavVideoOK;
}
//---------------------------------------------------------------------------
bool TVictorMediaPlayer::fGetLavAudioOK(void)
{
  return _ds->bLavAudioOK;
}
//---------------------------------------------------------------------------
HRESULT TVictorMediaPlayer::HandleGraphEvent(void)
{
  HRESULT hr = S_OK;

  if(_ds->lpMediaEventEx)
   {
     LONG evCode, evParam1, evParam2;
     while(SUCCEEDED(_ds->lpMediaEventEx->GetEvent(&evCode, (LONG_PTR *)&evParam1, (LONG_PTR *)&evParam2, 0)))
      {
        hr = _ds->lpMediaEventEx->FreeEventParams(evCode, evParam1, evParam2); // Free memory associated with callback, since we're not using it
        switch(evCode) // If this is the end of the clip
         {
           case EC_COMPLETE:
            {
//            LONGLONG pos=0;
//            hr = _ds->lpMediaSeeking->SetPositions(&pos, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning); // Reset to first frame of movie
//            if(FAILED(hr))
//             {
//               // If seeking failed, just stop and restart playback
//               hr = _ds->lpMediaControl->Stop();
//               hr = _ds->lpMediaControl->Run();
//             }
              PlayStateChanged(vmpsStopped);
            } break;
         }
      }
   }
  return hr;
}
//---------------------------------------------------------------------------
///////////////// TVictorMediaPlayer::TDirectShowInterfaces /////////////////
//---------------------------------------------------------------------------
TVictorMediaPlayer::TDirectShowInterfaces::TDirectShowInterfaces()
{
  lpGraphBuilder = NULL;
  lpMediaControl = NULL;
  lpMediaEventEx = NULL;
  lpMediaSeeking = NULL;
  lpMediaPosition = NULL;
  lpVideoWindow = NULL;
  lpBasicAudio = NULL;
  lpBasicVideo = NULL;
  lpVideoFrameStep = NULL;

  dwGraphRegister = 0;

  lpLavSplitterSource = NULL;
  lpFileSourceFilter = NULL;
  lpLavVideoDecoder = NULL;
  lpLavAudioDecoder = NULL;
  lpVideoRenderer = NULL;
  lpAudioRender = NULL;

  bLavVideoOK = false;
  bLavAudioOK = false;
}
//---------------------------------------------------------------------------
TVictorMediaPlayer::TDirectShowInterfaces::~TDirectShowInterfaces()
{
  CloseInterfaces();
}
//---------------------------------------------------------------------------
void TVictorMediaPlayer::TDirectShowInterfaces::CloseInterfaces(void)
{
  RemoveFilters();

  if(lpMediaEventEx)lpMediaEventEx->SetNotifyWindow(NULL, 0, 0); //Disable event callbacks

  if(lpVideoFrameStep){ lpVideoFrameStep->Release(); lpVideoFrameStep = NULL; }
  if(lpBasicVideo    ){ lpBasicVideo    ->Release(); lpBasicVideo     = NULL; }
  if(lpBasicAudio    ){ lpBasicAudio    ->Release(); lpBasicAudio     = NULL; }
  if(lpVideoWindow   ){ lpVideoWindow   ->Release(); lpVideoWindow    = NULL; }
  if(lpMediaPosition ){ lpMediaPosition ->Release(); lpMediaPosition  = NULL; }
  if(lpMediaSeeking  ){ lpMediaSeeking  ->Release(); lpMediaSeeking   = NULL; }
  if(lpMediaEventEx  ){ lpMediaEventEx  ->Release(); lpMediaEventEx   = NULL; }
  if(lpMediaControl  ){ lpMediaControl  ->Release(); lpMediaControl   = NULL; }

  RemoveFromRot(dwGraphRegister);
  dwGraphRegister = 0;

  if(lpGraphBuilder){ lpGraphBuilder->Release(); lpGraphBuilder=NULL; }
}
//---------------------------------------------------------------------------
HRESULT TVictorMediaPlayer::TDirectShowInterfaces::AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister)
{
  IMoniker * pMoniker;
  IRunningObjectTable *pROT;
  if(FAILED(GetRunningObjectTable(0, &pROT)))
   {
     return E_FAIL;
   }
  WCHAR wsz[256];
  swprintf(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());
  HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
  if(SUCCEEDED(hr))
   {
     hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph, pMoniker, pdwRegister);
     pMoniker->Release();
   }
  pROT->Release();
  return hr;
}
//---------------------------------------------------------------------------
void TVictorMediaPlayer::TDirectShowInterfaces::RemoveFromRot(DWORD pdwRegister)
{
  IRunningObjectTable *pROT;
  if(SUCCEEDED(GetRunningObjectTable(0, &pROT)))
   {
     pROT->Revoke(pdwRegister);
     pROT->Release();
   }
}
//---------------------------------------------------------------------------
// Create a filter by CLSID and add it to the graph. https://msdn.microsoft.com/en-us/library/dd373416
HRESULT TVictorMediaPlayer::TDirectShowInterfaces::AddFilterByCLSID(IGraphBuilder *pGraph, REFGUID clsid, IBaseFilter **ppF, LPCWSTR wszName)
{
  *ppF = 0;
  IBaseFilter *pFilter = NULL;

//HRESULT hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFilter));
  HRESULT hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pFilter);
  if(SUCCEEDED(hr))
   {
     hr = pGraph->AddFilter(pFilter, wszName);
     if(SUCCEEDED(hr))
      {
        *ppF = pFilter;
        (*ppF)->AddRef();
      }
     else
      {
        pFilter->Release();
      }
   }
  return hr;
}
//---------------------------------------------------------------------------
HRESULT TVictorMediaPlayer::TDirectShowInterfaces::GetUnconectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin)
{
  *ppPin = 0;
  IEnumPins *pEnum = 0;
  IPin *pPin = 0;

  HRESULT hr = pFilter->EnumPins(&pEnum);
  if(FAILED(hr))
   {
     return hr;
   }
  hr = pEnum->Reset();
  if(FAILED(hr))
   {
     //
   }
  while(pEnum->Next(1, &pPin, NULL) == S_OK)
   {
     PIN_DIRECTION ThisPinDirection;
     pPin->QueryDirection(&ThisPinDirection);
     if(ThisPinDirection == PinDir)
      {
        IPin *pTemp = 0;
        hr = pPin->ConnectedTo(&pTemp);
        if(SUCCEEDED(hr))
         {
           pTemp->Release();
         }
        else
         {
           pEnum->Release();
           *ppPin = pPin;
           return S_OK;
         }
      }
     pPin->Release();
   }
  pEnum->Release();
  return E_FAIL;
}
//---------------------------------------------------------------------------
HRESULT TVictorMediaPlayer::TDirectShowInterfaces::ConnectFilters(IGraphBuilder *pGraph, IPin *pOut, IBaseFilter *pDest )
{
  if((pGraph == NULL) || (pOut == NULL) || (pDest == NULL))
    return E_POINTER;

  PIN_DIRECTION PinDir;
  pOut->QueryDirection(&PinDir); // PINDIR_OUTPUT

  IPin *pIn = 0;
  HRESULT hr = GetUnconectedPin(pDest,PINDIR_INPUT,&pIn);
  if(FAILED(hr))
    return hr;

  hr = pGraph->Connect(pOut,pIn);
  pIn->Release();
  return hr;
}
//---------------------------------------------------------------------------
HRESULT TVictorMediaPlayer::TDirectShowInterfaces::ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IBaseFilter *pDest)
{
  if((pGraph == NULL) || (pSrc == NULL) || (pDest == NULL))
    return E_POINTER;

  IPin *pOut = 0;
  HRESULT hr = GetUnconectedPin(pSrc, PINDIR_OUTPUT, &pOut);
  if(FAILED(hr))
    return hr;

  hr = ConnectFilters(pGraph, pOut, pDest);
  pOut->Release();
  return hr;
}
//---------------------------------------------------------------------------
bool TVictorMediaPlayer::TDirectShowInterfaces::AddFilters(LPCOLESTR pszFileName, const AM_MEDIA_TYPE *pmt)
{
  if(SUCCEEDED(AddFilterByCLSID(lpGraphBuilder, CLSID_LavSplitter_Source, &lpLavSplitterSource))) // LAV 分离器
   {
     if(SUCCEEDED(lpLavSplitterSource->QueryInterface(IID_IFileSourceFilter,(void **)&lpFileSourceFilter)))
      {
        if(SUCCEEDED(lpFileSourceFilter->Load(pszFileName, pmt)))
         {
           bLavSplitterOK = true;
         }
      }
   }

  if(bLavSplitterOK)
   {
     if(SUCCEEDED(AddFilterByCLSID(lpGraphBuilder, CLSID_LavVideoDecoder, &lpLavVideoDecoder))) // LAV 视频解码器
      {
        if(SUCCEEDED(ConnectFilters(lpGraphBuilder, lpLavSplitterSource, lpLavVideoDecoder)))
         {
           if(SUCCEEDED(AddFilterByCLSID(lpGraphBuilder, CLSID_VideoMixingRenderer9, &lpVideoRenderer))) // VMR-9 视频渲染器
            {
              if(SUCCEEDED(ConnectFilters(lpGraphBuilder, lpLavVideoDecoder, lpVideoRenderer)))
               {
                 bLavVideoOK = true;
               }
            }
         }
      }
     if(SUCCEEDED(AddFilterByCLSID(lpGraphBuilder, CLSID_LavAudioDecoder, &lpLavAudioDecoder))) // LAV 音频解码器
      {
        if(SUCCEEDED(ConnectFilters(lpGraphBuilder, lpLavSplitterSource, lpLavAudioDecoder)))
         {
           if(SUCCEEDED(AddFilterByCLSID(lpGraphBuilder, CLSID_AudioRender, &lpAudioRender))) // 音频渲染器
            {
              if(SUCCEEDED(ConnectFilters(lpGraphBuilder, lpLavAudioDecoder, lpAudioRender)))
               {
                 bLavAudioOK = true;
               }
            }
         }
      }
     return true;
   }

  RemoveFilters();
  return false;
}
//---------------------------------------------------------------------------
void TVictorMediaPlayer::TDirectShowInterfaces::RemoveFilters(void)
{
  bLavVideoOK = false;
  bLavAudioOK = false;

  if(lpAudioRender      ){ lpAudioRender      ->Release(); lpAudioRender       = NULL; }
  if(lpVideoRenderer    ){ lpVideoRenderer    ->Release(); lpVideoRenderer     = NULL; }
  if(lpLavAudioDecoder  ){ lpLavAudioDecoder  ->Release(); lpLavAudioDecoder   = NULL; }
  if(lpLavVideoDecoder  ){ lpLavVideoDecoder  ->Release(); lpLavVideoDecoder   = NULL; }
  if(lpFileSourceFilter ){ lpFileSourceFilter ->Release(); lpFileSourceFilter  = NULL; }
  if(lpLavSplitterSource){ lpLavSplitterSource->Release(); lpLavSplitterSource = NULL; }
}
//---------------------------------------------------------------------------
} //namespace TVictorClasses
//---------------------------------------------------------------------------
