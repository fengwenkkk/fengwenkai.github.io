#pragma once

#include <MMReg.h>
#include <InitGuid.h>
#include <dsound.h>

#include <vector>
#include <string>
using namespace std;

#define CAPTURE_FRAME_COUNT		16

class IDirectSoundCaptureEvent
{
public:
	virtual long OnDSCapturedData(unsigned char* data,unsigned long size,unsigned long samplerate) = 0;
};

struct DSoundDeviceInfo
{
	string name;
	GUID   guid;
	string desc;
};
class CDirectSoundAecCapture
{
public:
	CDirectSoundAecCapture(void);
	~CDirectSoundAecCapture(void);
public:
	vector<DSoundDeviceInfo> m_allRenderDevices;
	vector<DSoundDeviceInfo> m_allCaptureDevices;
private:
	HRESULT EnumDevices();
private:
	long						m_captureIndex;
	long						m_renderIndex;
	long						m_samplerate;

	LPDIRECTSOUNDFULLDUPLEX		pDSFD;
	LPDIRECTSOUNDCAPTUREBUFFER8 pDSCBuffer8;
	LPDIRECTSOUNDBUFFER8		pDSBuffer8;

	HANDLE						pCapThread;
	HANDLE						pEvent;
	long						frameSize;
	long						captureBufferSize;
	unsigned char*				pCaptureDataTemp;
	DSBPOSITIONNOTIFY			dsPosNotify[CAPTURE_FRAME_COUNT+1];

	
	IDirectSoundCaptureEvent	*_event;
	bool						runThreadFlag;
	DWORD						m_dwNextCapOffset;
public:
	long						_ProcCapture();
public:

	long GetCaptureDeviceCount();
	long GetCaptureDeviceName(long index,char* device);
	long SetCaptureDeviceByName(char* device);
	long SetCaptureDeviceByID(long device);

	long GetRenderDeviceCount();
	long GetRenderDeviceName(long index,char* device);
	long SetRenderDeviceByName(char* device);
	long SetRenderDeviceByID(long device);

	long SetCaptureMode(long captureMode,long samplerate);
	HRESULT Create();

	long GetCaptureData(unsigned char* data,/*in out*/unsigned long* size);

	long SetEvent(IDirectSoundCaptureEvent *receiver);
	long Start(void);
	long Stop(void);
};
