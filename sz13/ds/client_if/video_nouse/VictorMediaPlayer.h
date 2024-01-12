//---------------------------------------------------------------------------
// Copyright © Victor Chen, http://www.cppfans.com/
//---------------------------------------------------------------------------
#ifndef VictorMediaPlayerH
#define VictorMediaPlayerH
//---------------------------------------------------------------------------
#include <vcl.h>
//---------------------------------------------------------------------------
#define VOLUME_FULL     0L
#define VOLUME_SILENCE  -10000L
//---------------------------------------------------------------------------
namespace TVictorClasses {
//---------------------------------------------------------------------------
enum TVictorMediaPlayState
{
  vmpsInit,
  vmpsStopped,
  vmpsPaused,
  vmpsRunning,
};
//---------------------------------------------------------------------------
class EVictorMediaPlayer : public Exception
{
public:
  __property HRESULT hResult = { read = _hResult };
  EVictorMediaPlayer(UnicodeString sMsg, HRESULT hr);
private:
  HRESULT _hResult;
};
//---------------------------------------------------------------------------
class TVictorMediaPlayer : public TObject
{
public:
  __property UnicodeString         FileName     = { read = _wsFileName   , write = fSetFileName };
  __property TVictorMediaPlayState PlayState    = { read = _psCurrent                           };
  __property bool                  Active       = { read = fGetActive    , write = fSetActive   };
  __property LONG                  Volume       = { read = _lVolume      , write = fSetVolume   };
  __property HWND                  hWndVideo    = { read = _hWnd         , write = fSetHWnd     };
  __property HWND                  hWndPlaying  = { read = _hWndPlaying                         };
  __property bool                  AudioOnly    = { read = _bAudioOnly                          };
  __property bool                  AspectRatio  = { read = _bAspectRatio , write = fSetAspRatio };
  __property double                Duration     = { read = fGetDuration                         };
  __property double                Position     = { read = fGetPosition  , write = fSetPosition };

  __property bool LavSplitterOK = { read = fGetLavSplitterOK };
  __property bool LavVideoOK    = { read = fGetLavVideoOK    };
  __property bool LavAudioOK    = { read = fGetLavAudioOK    };

  __property void __fastcall (__closure *OnStateChanged)(TVictorMediaPlayer *Sender, TVictorMediaPlayState NewSt, TVictorMediaPlayState OldSt) = { read = _lpfnStateChanged, write = _lpfnStateChanged };

  void Play(void);
  void Pause(void);
  void Stop(void);

  void GetVideoSize(long &w, long &h);
  void SetVideoPos(long x, long y, long w, long h);
  void AdjustVideoSize(void);

  __fastcall TVictorMediaPlayer();
  __fastcall ~TVictorMediaPlayer();

private:
  HWND          _hWnd;
  HWND          _hMsgWnd;
  HWND          _hWndPlaying;
  UnicodeString _wsFileName;
  LONG          _lVolume; // VOLUME_SILENCE - VOLUME_FULL
  TVictorMediaPlayState _psCurrent;
  bool          _bAudioOnly;
  bool          _bAspectRatio;

  class TDirectShowInterfaces;
  TDirectShowInterfaces *_ds;

  void __fastcall (__closure *_lpfnStateChanged)(TVictorMediaPlayer *Sender, TVictorMediaPlayState NewSt, TVictorMediaPlayState OldSt);

  void PlayMovieInWindow(void);
  void CheckVisibility(void);
  BOOL GetFrameStepInterface(void);
  void SetPlayingHWnd(HWND h);
  HRESULT HandleGraphEvent(void);

  void fSetFileName(UnicodeString ws);
  void fOpen(void);
  void fClose(void);
  bool fGetActive(void) { return _psCurrent!=vmpsInit; }
  void fSetActive(bool b) { if(b){fOpen();} else{fClose();} }
  void fSetVolume(LONG NewVol); // VOLUME_SILENCE - VOLUME_FULL
  void fSetHWnd(HWND h);
  void fSetAspRatio(bool);

  double fGetRate(void);
  void fSetRate(double dRate);

  double fGetDuration(void);
  double fGetPosition(void);
  void fSetPosition(double);

  bool fGetLavSplitterOK(void);
  bool fGetLavVideoOK(void);
  bool fGetLavAudioOK(void);

protected:
  virtual void PlayStateChanged(TVictorMediaPlayState NewSt);
  virtual void __fastcall WndProc(Messages::TMessage &Message);
};
//---------------------------------------------------------------------------
} using namespace TVictorClasses;
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
