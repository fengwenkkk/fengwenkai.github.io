#ifndef _H_AEC_CAPTURE_INTERFACE_
#define _H_AEC_CAPTURE_INTERFACE_

#define RET_SUCCESSED		0
#define RET_INVALID_RECIVER -1

/************************************************************************/
/* Ref Count                                                            */
/************************************************************************/
#define IMPLEMENT_REFCOUNT \
	long _RefCount; \
	virtual long __stdcall AddRef()\
{\
	InterlockedIncrement(&_RefCount);\
	return _RefCount;\
}\
	virtual long __stdcall Release()\
{\
	InterlockedDecrement(&_RefCount);\
	if (!_RefCount)\
	{\
	delete this;\
	return 0;\
	}\
	return _RefCount;\
}

#define INIT_REFCOUNT _RefCount=1;

/************************************************************************/
/* Observer                                                             */
/************************************************************************/
#define IMPLEMENT_OBSERVER(EventType) \
	EventType*		_SingleObserver;\
	set<EventType*>	_AllObservers;\
	virtual long __stdcall SetObServer(EventType* Reciver)\
{\
	if (!Reciver)\
	{\
	return RET_INVALID_RECIVER;\
	}\
    _AllObservers.clear();\
	_AllObservers.insert(Reciver);\
	_SingleObserver = Reciver;\
	return RET_SUCCESSED;\
}\
	virtual long __stdcall _AppendObServer(EventType* Reciver)\
{\
	if (!Reciver)\
	{\
	return RET_INVALID_RECIVER;\
	}\
	_AllObservers.insert(Reciver);\
	if (1 == _AllObservers.size())\
	{\
	_SingleObserver = Reciver;\
	}\
	else\
	{\
	_SingleObserver = NULL;\
	}\
	return RET_SUCCESSED;\
}\
	virtual long __stdcall _RemoveObServer(EventType* Reciver)\
{\
	if (!Reciver)\
	{\
	_AllObservers.clear();\
	_SingleObserver = NULL;\
	}\
	else\
	{\
	_AllObservers.erase(Reciver);\
	if (1 == _AllObservers.size())\
		{\
		_SingleObserver = *(_AllObservers.begin());\
		}\
		else if(0 == _AllObservers.size())\
		{\
		_SingleObserver = NULL;\
		}\
	}\
	return RET_SUCCESSED;\
}

#define INIT_OBSERVER _SingleObserver=NULL;
#define CAP_TYPE_AECMICARR		0x1
#define CAP_TYPE_DIRECTSOUND	0x2
class IAecCaptureEvent
{
public:
	virtual long __stdcall OnCapturedData(unsigned char* data,unsigned long size,unsigned long samplerate,unsigned long bitsPerSample,long type) = 0;
};

class IAecCapture
{
public:
	virtual long __stdcall AddRef() = 0;
	virtual long __stdcall Release() = 0;
	virtual long __stdcall ResetCapture() = 0;
	virtual long __stdcall EnumDevices() = 0;

	virtual long __stdcall GetCaptureDeviceCount() = 0;
	virtual long __stdcall GetCaptureDeviceName(long index,char* device) = 0;
	virtual long __stdcall SetCaptureDeviceByName(char* device) = 0;
	virtual long __stdcall SetCaptureDeviceByID(long device) = 0;

	virtual long __stdcall GetRenderDeviceCount() = 0;
	virtual long __stdcall GetRenderDeviceName(long index,char* device) = 0;
	virtual long __stdcall SetRenderDeviceByName(char* device) = 0;
	virtual long __stdcall SetRenderDeviceByID(long device) = 0;

	virtual long __stdcall SetCaptureMode(long captureMode,long samplerate) = 0;
	virtual long __stdcall CreateCapture() = 0;
	virtual long __stdcall GetCaptureData(unsigned char* data,/*in out*/unsigned long* size) = 0;

	
	virtual long __stdcall Start(void) = 0;
	virtual long __stdcall Stop(void) = 0;

	virtual long __stdcall _AppendObServer(IAecCaptureEvent* Reciver) = 0;
	virtual long __stdcall _RemoveObServer(IAecCaptureEvent* Reciver) = 0;
    virtual long __stdcall SetObServer(IAecCaptureEvent* Reciver) = 0;

	virtual long __stdcall NeedReCreate() = 0;
};


#endif