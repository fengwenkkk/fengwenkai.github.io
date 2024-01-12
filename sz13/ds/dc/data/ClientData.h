#ifndef ClientDataH
#define ClientDataH

#include "ClientUtil.h"

typedef struct
{
    UINT8   aucNTPAddr[4];
    UINT16  usNTPPort;
    UINT32  ulNTPSyncPeriod;
}NTP_CFG_T;

extern Dict g_DictStationType;
extern Dict g_DictVideoFormat;

BOOL ManageOneDepot();
BOOL ManageOneDepot(BOOL bManageOneDepot);

Dict* GetDictSDSType();
Dict* GetDictSDSTemplateType();

CHAR* GetNativeServicePath();
VOID SetNativeServicePath(CHAR *szPath);

INT64 GetCurrBrdGroupID();

UINT32 GetMaxListenPeriod(); // 终端、群组监听最大时长
UINT32 GetGroupCallIdleTime();

UINT32 GetATSValidPeriod();
VOID SetATSValidPeriod(UINT32 ulValue);

UINT32 GetMaxGroupCallRecordTime();
UINT32 GetMaxGroupCallRecordIdleTime();

BOOL IsShowCallWin();
VOID SetShowCallWin(BOOL bValue);

AnsiString GetATSAddr1();
AnsiString GetATSAddr2();
VOID SetATSAddr1(AnsiString strAddr);
VOID SetATSAddr2(AnsiString strAddr);

BOOL IsOriginalVideoFormat();
VOID SetOriginalVideoFormat(BOOL bValue);
BOOL IsIPHVideoOnTcpMode();
VOID SetIPHVideoOnTcpMode(BOOL bValue);
UINT32 GetIPHVideoOpenTimeout();
VOID SetIPHVideoOpenTimeout(UINT32 ulValue);
BOOL IsIPHRecMergeMode();
VOID SetIPHRecMergeMode(BOOL bValue);

BOOL IsLocalIPHAECMode();
VOID SetLocalIPHAECMode(BOOL bValue);

BOOL DirectSendIPHAudio();
VOID SetDirectSendIPHAudio(BOOL bValue);
UINT32 GetIPHAudioSampleRate();
VOID SetIPHAudioSampleRate(UINT32 ulValue);
UINT32 GetIPHAudioBitsPerSample();
VOID SetIPHAudioBitsPerSample(UINT32 ulValue);

BOOL DirectSendPAAudio();
VOID SetDirectSendPAAudio(BOOL bValue);
UINT32 GetPAAudioSampleRate();
VOID SetPAAudioSampleRate(UINT32 ulValue);
UINT32 GetPAAudioBitsPerSample();
VOID SetPAAudioBitsPerSample(UINT32 ulValue);

VOID SetAllowPAAECMode(BOOL bEnabled);
BOOL AllowPAAECMode();
VOID SetAllowIPHAECMode(BOOL bEnabled);
BOOL AllowIPHAECMode();

UINT32 GetPAMaxPkgSize();
VOID SetPAMaxPkgSize(UINT32 ulValue);
UINT32 GetIPHMaxPkgSize();
VOID SetIPHMaxPkgSize(UINT32 ulValue);

VOID SetPADelaySendTime(UINT32 ulValue);
UINT32 GetPADelaySendTime();

VOID SetMaxIPHTalkTime(UINT32 ulValue);
UINT32 GetMaxIPHTalkTime();

VOID GetNTPCfg(NTP_CFG_T &stCfg);
VOID SetNTPCfg(NTP_CFG_T &stCfg);
BOOL SendSMSByRealMsg();
VOID SetSendSMSByRealMsg(BOOL bValue);
VOID SetSimpleSMSMode(BOOL bValue);
BOOL IsSimpleSMSMode();

BOOL AllowHalfDuplexP2PCall();
VOID SetAllowHalfDuplexP2PCall(BOOL bValue);

BOOL AllowVideoWall();
VOID SetAllowVideoWall(BOOL bValue);
BOOL AllowGISSub();
VOID SetAllowGISSub(BOOL bValue);
BOOL AllowP2PVideoCall();
VOID SetAllowP2PVideoCall(BOOL bValue);
BOOL AutoCloseMulticastCall();
VOID SetAutoCloseMulticastCall(BOOL bValue);

BOOL IsConnectToDIS();
VOID SetConnectToDIS(BOOL bConnected);
BOOL IsATSValid();
VOID SetATSValid(BOOL bValid);
BOOL IsNTPValid();
VOID SetNTPValid(BOOL bValid);
BOOL IsMDCValid();
VOID SetMDCValid(BOOL bValid);

AnsiString GetUserFuncTypeText(UINT32 ulType);  // 用户/终端类型分类

VOID GetDCType(VECTOR<ID_VALUE_T> &vDCType);
VOID GetDCType(UINT32 ulDCType, Dict *pDict);
AnsiString GetDCTypeText(UINT32 ulDCType);

AnsiString GetDCTypeName(UINT32 ulDCType);
void InitDCTypeDict(UINT32 ulDCType, Dict &DictDCType);

AnsiString GetCallStateText(UINT32 ulState);
AnsiString GetCallTypeText(UINT32 ulCallType, UINT32 ulReqCallType);
AnsiString GetP2PCallStatusText(UINT32 ulStatus);

CHAR* GetSDSTypeText(UINT32 ulType);

VOID LoadIPHVideoSize(int &iVideoWidth, int &iVideoHeight);
VOID SaveIPHVideoSize(int iVideoWidth, int iVideoHeight);

AnsiString GetStationTypeText(UINT32 ulStationType);

#endif
