#ifndef AudioDevH
#define AudioDevH

void LoadAudioDevCfg();
void SaveAudioDevCfg();

BOOL InitAudioDevList();

VOID GetSpeakerList(std::vector<AnsiString> &vList);
VOID GetMicList(std::vector<AnsiString> &vList);

void GetAudioDev(AnsiString &strMainSpeaker, AnsiString &strMainMic, AnsiString &strSlaveSpeaker, AnsiString &strSlaveMic, AnsiString &strMonSpeaker);

AnsiString GetMainSpeaker();
AnsiString GetMainMic();
AnsiString GetSlaveSpeaker();
AnsiString GetSlaveMic();
AnsiString GetMonSpeaker();

AnsiString GetESDKMainSpeaker();
AnsiString GetESDKMainMic();
AnsiString GetESDKSlaveSpeaker();
AnsiString GetESDKSlaveMic();
AnsiString GetESDKMonSpeaker();

void SetMainSpeaker(AnsiString strDev);
void SetMainMic(AnsiString strDev);
void SetSlaveSpeaker(AnsiString strDev);
void SetSlaveMic(AnsiString strDev);
void SetMonSpeaker(AnsiString strDev);

BOOL AssignGroupSpeaker(INT64 i64GroupID, AnsiString strDev);
BOOL AssignGroupMic(INT64 i64GroupID, AnsiString strDev);
BOOL AssignUserSpeaker(UINT32 ulUserID, AnsiString strDev);
BOOL AssignUserMic(UINT32 ulUserID, AnsiString strDev);

BOOL SetAudioVolume(AnsiString strDev, UINT32 ulRealBrdVolume);

VOID SwitchSpeaker();
VOID SwitchMic();

BOOL IsMainSpeaker();
BOOL IsMainMic();

UINT32 GetMainSpeakerVolume     ();
UINT32 GetMainMicVolume         ();
UINT32 GetSlaveSpeakerVolume    ();
UINT32 GetSlaveMicVolume        ();
UINT32 GetMonVolume             ();
UINT32 GetRealBrdVolume         ();
UINT32 GetVoiceAlarmVolume      ();
INT32  GetIPHMicVolumeRate      ();
UINT32 GetP2PCallMicVolumeBoost ();

VOID SetMainSpeakerVolume     (UINT32 ulVolume);
VOID SetMainMicVolume         (UINT32 ulVolume);
VOID SetSlaveSpeakerVolume    (UINT32 ulVolume);
VOID SetSlaveMicVolume        (UINT32 ulVolume);
VOID SetMonVolume             (UINT32 ulVolume);
VOID SetRealBrdVolume         (UINT32 ulVolume);
VOID SetVoiceAlarmVolume      (UINT32 ulVolume);
VOID SetIPHMicVolumeRate      (INT32  iValue );
VOID SetP2PCallMicVolumeBoost (UINT32 ulValue );

#endif
