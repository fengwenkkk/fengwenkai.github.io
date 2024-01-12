#include "ClientInclude.h"
#include "ds_public.h"
#include "pis.h"
#include "msg.h"
#include "AudioVolume.h"
#include "eSDK.h"

static AnsiString   g_strMainSpeaker = "";
static AnsiString   g_strMainMic = "";
static AnsiString   g_strSlaveSpeaker = "";
static AnsiString   g_strSlaveMic = "";
static AnsiString   g_strMonSpeaker = "";

static BOOL         g_bUseMainSpeaker = TRUE;
static BOOL         g_bUseMainMic = TRUE;

static UINT32       g_ulMainSpeakerVolume = 70;
static UINT32       g_ulMainMicVolume = 70;
static UINT32       g_ulSlaveSpeakerVolume = 70;
static UINT32       g_ulSlaveMicVolume = 70;
static UINT32       g_ulMonVolume = 100;
static UINT32       g_ulRealBrdVolume = 80;
static UINT32       g_ulVoiceAlarmVolume = 60;
static INT32        g_iIPHMicVolumeRate = 0;
static UINT32       g_ulP2PCallMicVolumeBoost = 20;         // 点呼时需要增加的麦克风增加，缺省20

static MAP<AnsiString, AnsiString>     m_mSpeaker;
static MAP<AnsiString, AnsiString>     m_mMic;

extern BOOL GetAudioDev(std::vector<AnsiString> &vDev);
extern BOOL GetAudioVolume(AnsiString strDev, UINT32 &ulVolume);
extern BOOL SetAudioVolume(AnsiString strDev, UINT32 ulVolume);

extern BOOL eSDK_AssignGroupSpeaker(INT64 i64GroupID, CHAR *szSpeaker);
extern BOOL eSDK_AssignGroupMic(INT64 i64GroupID, CHAR *szMic);
extern BOOL eSDK_AssignUserSpeaker(UINT32 ulUserID, CHAR *szSpeaker);
extern BOOL eSDK_AssignUserMic(UINT32 ulUserID, CHAR *szMic);

BOOL IsSameAudioDev(AnsiString strShortDevName, AnsiString strDevName)
{
    CHAR    *szDev = strShortDevName.c_str();

    if (gos_str_like(szDev, "WINSND:"))
    {
        szDev += strlen("WINSND:");
        szDev = gos_left_trim_string(szDev);
    }

    return gos_str_like(strDevName.c_str(), szDev);
}

void LoadAudioDevCfg()
{
    g_strMainSpeaker = LoadConfInfo("Sound", "MainSpeaker");
    g_strMainMic = LoadConfInfo("Sound", "MainMic");
    g_strSlaveSpeaker = LoadConfInfo("Sound", "SlaveSpeaker");
    g_strSlaveMic = LoadConfInfo("Sound", "SlaveMic");
    g_strMonSpeaker = LoadConfInfo("Sound", "MonSpeaker");

    LoadConfInfo("Sound", "MainSpeakerVolume", (INT32*)&g_ulMainSpeakerVolume, 1, 100);
    LoadConfInfo("Sound", "MainMicVolume", (INT32*)&g_ulMainMicVolume, 1, 100);
    LoadConfInfo("Sound", "SlaveSpeakerVolume", (INT32*)&g_ulSlaveSpeakerVolume, 1, 100);
    LoadConfInfo("Sound", "SlaveMicVolume", (INT32*)&g_ulSlaveMicVolume, 1, 100);
    LoadConfInfo("Sound", "MonVolume", (INT32*)&g_ulMonVolume, 1, 100);
    LoadConfInfo("Sound", "RealBrdVolume", (INT32*)&g_ulRealBrdVolume, 1, 100);
    LoadConfInfo("Sound", "VoiceAlarmVolume", (INT32*)&g_ulVoiceAlarmVolume, 1, 100);
    LoadConfInfo("Sound", "IPHMicVolumeRate", (INT32*)&g_iIPHMicVolumeRate, -20, 20);

    GosLog(LOG_INFO, "main speaker is %s", g_strMainSpeaker.c_str());
    GosLog(LOG_INFO, "main mic is %s", g_strMainMic.c_str());
    GosLog(LOG_INFO, "slave speaker is %s", g_strSlaveSpeaker.c_str());
    GosLog(LOG_INFO, "slave mic is %s", g_strSlaveMic.c_str());
    GosLog(LOG_INFO, "mon speaker is %s", g_strMonSpeaker.c_str());
    GosLog(LOG_INFO, "use %s speaker", g_bUseMainSpeaker?"main":"slave");
    GosLog(LOG_INFO, "use %s mic", g_bUseMainMic?"main":"slave");
}

void SaveAudioDevCfg()
{
    SaveConfInfo("Sound", "MainSpeaker", g_strMainSpeaker.c_str());
    SaveConfInfo("Sound", "MainMic", g_strMainMic.c_str());
    SaveConfInfo("Sound", "SlaveSpeaker", g_strSlaveSpeaker.c_str());
    SaveConfInfo("Sound", "SlaveMic", g_strSlaveMic.c_str());
    SaveConfInfo("Sound", "MonSpeaker", g_strMonSpeaker.c_str());
    SaveConfInfo("Sound", "UseMainSpeaker", g_bUseMainSpeaker?"true":"false");
    SaveConfInfo("Sound", "UseMainMic", g_bUseMainMic?"true":"false");

    SaveConfInfo("Sound", "MainSpeakerVolume", g_ulMainSpeakerVolume);
    SaveConfInfo("Sound", "MainMicVolume", g_ulMainMicVolume);
    SaveConfInfo("Sound", "SlaveSpeakerVolume", g_ulSlaveSpeakerVolume);
    SaveConfInfo("Sound", "SlaveMicVolume", g_ulSlaveMicVolume);
    SaveConfInfo("Sound", "MonVolume", g_ulMonVolume);
    SaveConfInfo("Sound", "RealBrdVolume", g_ulRealBrdVolume);
    SaveConfInfo("Sound", "VoiceAlarmVolume", g_ulVoiceAlarmVolume);
    SaveConfInfo("Sound", "IPHMicVolumeRate", g_iIPHMicVolumeRate);
}

BOOL InitAudioDevList()
{
    std::vector<AnsiString>     vSpeaker;
    std::vector<AnsiString>     vMic;
    std::vector<AnsiString>     vAudioDev;

    if (!eSDK_GetSpeakerList(vSpeaker))
    {
        GosLog(LOG_ERROR, "get speaker list failed");
        return FALSE;
    }

    if (!eSDK_GetMicList(vMic))
    {
        GosLog(LOG_ERROR, "get mic list failed");
        return FALSE;
    }

    if (!GetAudioDev(vAudioDev))
    {
        GosLog(LOG_ERROR, "get audio dev list failed");
        return FALSE;
    }

    UINT32  i, j;
    AnsiString strDev;
    AnsiString strKey;

    m_mSpeaker.clear();
    m_mMic.clear();

    for (i=0; i<vSpeaker.size(); i++)
    {
        strDev = vSpeaker[i];
        strKey = strDev;
        for (j=0; j<vAudioDev.size(); j++)
        {
            if (IsSameAudioDev(strDev, vAudioDev[j]))
            {
                strKey = vAudioDev[j];
                vAudioDev.erase(vAudioDev.begin()+j);

                break;
            }
        }

        m_mSpeaker[strKey] = strDev;
        GosLog(LOG_INFO, "speaker: %s -> %s", strKey.c_str(), strDev.c_str());
    }

    for (i=0; i<vMic.size(); i++)
    {
        strDev = vMic[i];
        strKey = strDev;
        for (j=0; j<vAudioDev.size(); j++)
        {
            if (IsSameAudioDev(strDev, vAudioDev[j]))
            {
                strKey = vAudioDev[j];
                vAudioDev.erase(vAudioDev.begin()+j);

                break;
            }
        }

        m_mMic[strKey] = strDev;
        GosLog(LOG_INFO, "mic: %s -> %s", strKey.c_str(), strDev.c_str());
    }

    return TRUE;
}

VOID GetSpeakerList(std::vector<AnsiString> &vList)
{
    vList.clear();

    MAP<AnsiString, AnsiString>::iterator  it;

    for (it=m_mSpeaker.begin(); it!=m_mSpeaker.end(); it++)
    {
        vList.push_back(it->first);
    }
}

VOID GetMicList(std::vector<AnsiString> &vList)
{
    vList.clear();

    MAP<AnsiString, AnsiString>::iterator  it;

    for (it=m_mMic.begin(); it!=m_mMic.end(); it++)
    {
        vList.push_back(it->first);
    }
}

void GetAudioDev(AnsiString &strMainSpeaker, AnsiString &strMainMic, AnsiString &strSlaveSpeaker, AnsiString &strSlaveMic, AnsiString &strMonSpeaker)
{
    strMainSpeaker = g_strMainSpeaker;
    strMainMic = g_strMainMic;
    strSlaveSpeaker = g_strSlaveSpeaker;
    strSlaveMic = g_strSlaveMic;
    strMonSpeaker = g_strMonSpeaker;
}

AnsiString GetMainSpeaker()
{
    return g_bUseMainSpeaker?g_strMainSpeaker:g_strSlaveSpeaker;
}

AnsiString GetMainMic()
{
    return g_bUseMainMic?g_strMainMic:g_strSlaveMic;
}

AnsiString GetSlaveSpeaker()
{
    return g_bUseMainSpeaker?g_strSlaveSpeaker:g_strMainSpeaker;
}

AnsiString GetSlaveMic()
{
    return g_bUseMainMic?g_strSlaveMic:g_strMainMic;
}

AnsiString GetMonSpeaker()
{
    return g_strMonSpeaker;
}

AnsiString GetESDKMainSpeaker()
{
    return m_mSpeaker[GetMainSpeaker()];
}

AnsiString GetESDKMainMic()
{
    return m_mMic[GetMainMic()];
}

AnsiString GetESDKSlaveSpeaker()
{
    return m_mSpeaker[GetSlaveSpeaker()];
}

AnsiString GetESDKSlaveMic()
{
    return m_mMic[GetSlaveMic()];
}

AnsiString GetESDKMonSpeaker()
{
    if (m_mSpeaker.find(g_strMonSpeaker) == m_mSpeaker.end())
    {
        return "";
    }

    return m_mSpeaker[g_strMonSpeaker];
}

void SetMainSpeaker(AnsiString strDev)
{
    g_strMainSpeaker = strDev;
}

void SetMainMic(AnsiString strDev)
{
    g_strMainMic = strDev;
}

void SetSlaveSpeaker(AnsiString strDev)
{
    g_strSlaveSpeaker = strDev;
}

void SetSlaveMic(AnsiString strDev)
{
    g_strSlaveMic = strDev;
}

void SetMonSpeaker(AnsiString strDev)
{
    g_strMonSpeaker = strDev;

    SetDefaultSpeaker(strDev);
}

BOOL AssignGroupSpeaker(INT64 i64GroupID, AnsiString strDev)
{
    return eSDK_AssignGroupSpeaker(i64GroupID, strDev.c_str());
}

BOOL AssignGroupMic(INT64 i64GroupID, AnsiString strDev)
{
    return eSDK_AssignGroupMic(i64GroupID, strDev.c_str());
}

BOOL AssignUserSpeaker(UINT32 ulUserID, AnsiString strDev)
{
    return eSDK_AssignUserSpeaker(ulUserID, strDev.c_str());
}

BOOL AssignUserMic(UINT32 ulUserID, AnsiString strDev)
{
    return eSDK_AssignUserMic(ulUserID, strDev.c_str());
}

VOID SwitchSpeaker()
{
    if (g_strMainSpeaker == g_strSlaveSpeaker)
    {
        return;
    }

    g_bUseMainSpeaker = !g_bUseMainSpeaker;

    /*
    AnsiString  strTmp = g_strMainSpeaker;

    g_strMainSpeaker = g_strSlaveSpeaker;
    g_strSlaveSpeaker = strTmp;    */

    SaveAudioDevCfg();

    Group::SetDefaultGroupSpeaker();
}

VOID SwitchMic()
{
    if (g_strMainMic == g_strSlaveMic)
    {
        return;
    }

    /*
    AnsiString  strTmp = g_strMainMic;

    g_strMainMic = g_strSlaveMic;
    g_strSlaveMic = strTmp;
                                     */
    g_bUseMainMic = !g_bUseMainMic;

    SaveAudioDevCfg();

    Group::SetDefaultGroupMic();
}

BOOL IsMainSpeaker()
{
    return g_bUseMainSpeaker;
}

BOOL IsMainMic()
{
    return g_bUseMainMic;
}

UINT32 GetMainSpeakerVolume     (){return g_ulMainSpeakerVolume        ;};
UINT32 GetMainMicVolume         (){return g_ulMainMicVolume            ;};
UINT32 GetSlaveSpeakerVolume    (){return g_ulSlaveSpeakerVolume       ;};
UINT32 GetSlaveMicVolume        (){return g_ulSlaveMicVolume           ;};
UINT32 GetMonVolume             (){return g_ulMonVolume                ;};
UINT32 GetRealBrdVolume         (){return g_ulRealBrdVolume            ;};
UINT32 GetVoiceAlarmVolume      (){return g_ulVoiceAlarmVolume         ;};
INT32  GetIPHMicVolumeRate      (){return g_iIPHMicVolumeRate          ;};
UINT32 GetP2PCallMicVolumeBoost (){return g_ulP2PCallMicVolumeBoost    ;};

VOID SetMainSpeakerVolume     (UINT32 ulVolume){g_ulMainSpeakerVolume     = ulVolume;};
VOID SetMainMicVolume         (UINT32 ulVolume){g_ulMainMicVolume         = ulVolume;};
VOID SetSlaveSpeakerVolume    (UINT32 ulVolume){g_ulSlaveSpeakerVolume    = ulVolume;};
VOID SetSlaveMicVolume        (UINT32 ulVolume){g_ulSlaveMicVolume        = ulVolume;};
VOID SetMonVolume             (UINT32 ulVolume){g_ulMonVolume             = ulVolume;};
VOID SetRealBrdVolume         (UINT32 ulVolume){g_ulRealBrdVolume         = ulVolume;};
VOID SetVoiceAlarmVolume      (UINT32 ulVolume){g_ulVoiceAlarmVolume      = ulVolume;};
VOID SetIPHMicVolumeRate      (INT32  iValue  ){g_iIPHMicVolumeRate       = iValue;  };
VOID SetP2PCallMicVolumeBoost (UINT32 ulValue ){g_ulP2PCallMicVolumeBoost = ulValue; };

