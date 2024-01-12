#ifndef AudioVolumeH
#define AudioVolumeH

VOID GetMicList(std::vector<CHAR *> &vMicList);
BOOL GetMicVolume(AnsiString strMicName, UINT32 *pulVolume);
BOOL SetMicVolume(AnsiString strMicName, UINT32 ulVolume);
int GetMicBoostLevel(AnsiString strName);
bool SetMicBoostLevel(AnsiString strName, double dLevel);
BOOL SetDefaultSpeaker(AnsiString strDev);
//BOOL GetSpeakerList(std::vector<AnsiString> &vList);
//BOOL GetMicList(std::vector<AnsiString> &vList);

#endif
