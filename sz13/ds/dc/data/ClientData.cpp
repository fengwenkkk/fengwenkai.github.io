#include "ClientInclude.h"

#include "eSDK.h"

static CHAR     *g_szNativeServicePath = NULL;

static BOOL     g_bManageOneDepot = FALSE;  // TRUE：段场调度台只能管理一个段场 FALSE：管理所有段场

static UINT32   g_ulMaxListenPeriod = 60;   // 最大侦听时长
static UINT32   g_ulGroupCallIdleTime = 60; // 组呼最大空闲时间，超时表示呼叫结束

static UINT32   g_ulATSValidPeriod = 15;    // 判断ATS断开的检查时长

static UINT32   g_ulMaxGroupCallRecordTime = 300;       // 组呼最大记录时长
static UINT32   g_ulMaxGroupCallRecordIdleTime = 15;    // 组呼无话权人最大等待时间，超过此值结束本次录音

static AnsiString   g_strATSAddr1 = "";  ///< ATS 服务的 IP 地址, 数据来源 dis(db: appcfg)
static AnsiString   g_strATSAddr2 = "";  ///< ATS 备用服务的 IP 地址, 数据来源 dis(db: appcfg)

static BOOL     g_bShowCallWin = TRUE;
static BOOL     g_bOriginalVideoFormat = FALSE;
static BOOL     g_bIPHVideoOnTcpMode = TRUE;        // RTSP视频流采用TCP方式（通常采用UDO方式）
static BOOL     g_ulIPHVideoOpenTimeout = 1;        // 打开IPH视频时间
static BOOL     g_bIPHRecMergeMode = TRUE;          // IPH音视频录音文件是否合并
static BOOL     g_bLocalIPHAECMode = FALSE;         // 调度台本地控制AEC开关
static BOOL     g_bDirectSendIPHAudio = TRUE;       // 是否直接发送对讲音频到IPH设备
static UINT32   g_ulIPHAudioSampleRate = 16000;
static UINT32   g_ulIPHAudioBitsPerSample = 16;

static BOOL     g_bDirectSendPAAudio = TRUE;       // 是否直接发送人工广播音频到PA设备
static UINT32   g_ulPAAudioSampleRate = 16000;
static UINT32   g_ulPAAudioBitsPerSample = 16;

static BOOL     g_bAllowIPHAECMode = FALSE;
static BOOL     g_bAllowPAAECMode = FALSE;

static UINT32   g_ulPAMaxPkgSize = 1000;
static UINT32   g_ulIPHMaxPkgSize = 1000;

static UINT32   g_ulPADelaySendTime = 0;

static UINT32   g_ulMaxIPHTalkTime = 0;         // IPH发起请求后最大等待时间

static BOOL     g_bSendSMSByRealMsg = FALSE;
static BOOL     g_bSimpleSMSMode = TRUE;
static BOOL     g_bAllowHalfDuplexP2PCall = FALSE;

static BOOL     g_bAllowVideoWall = FALSE;
static BOOL     g_bAllowGISSub = FALSE;
static BOOL     g_bAllowP2PVideoCall = TRUE;
static BOOL     g_bAutoCloseMulticastCall = TRUE;

static NTP_CFG_T    g_stNTPCfg = {{0,0,0,0}, 123, 60};  ///< NTP地址与更新周期， 数据来源: dis(db: appcfg)

static BOOL     g_bConnectToDIS = TRUE;
static BOOL     g_bATSValid = TRUE;
static BOOL     g_bNTPValid = TRUE;
static BOOL     g_bMDCValid = TRUE;

static ID_VALUE_T  s_astDCType[] = {
                                {DC_TYPE_TRAIN,     "行车调度台"},
                                {DC_TYPE_SHUNT,     "车辆调度台"},
                                {DC_TYPE_PASSENGER, "乘客调度台"},
                                {DC_TYPE_DEPOT,     "段场调度台"},
//                                {DC_TYPE_FIX,       "维修调度台"},
                                {DC_TYPE_ENV,       "设备调度台"},
//                                {DC_TYPE_POWER,     "电力调度台"},
                                {DC_TYPE_MANAGER,   "维护调度台"},
                                {DC_TYPE_MASTER,    "总控调度台"},
                                };

BOOL ManageOneDepot()
{
    return g_bManageOneDepot;
}

BOOL ManageOneDepot(BOOL bManageOneDepot)
{
    return g_bManageOneDepot = bManageOneDepot;
}

VOID GetDCType(VECTOR<ID_VALUE_T> &vDCType)
{
    vDCType.clear();

    for (UINT32 i=0; i<ARRAY_SIZE(s_astDCType); i++)
    {
        vDCType.push_back(s_astDCType[i]);
    }
}

VOID GetDCType(UINT32 ulDCType, Dict *pDict)
{
    pDict->Clear();

    for (UINT32 i=0; i<ARRAY_SIZE(s_astDCType); i++)
    {
        if ((s_astDCType[i].ulID & ulDCType) == s_astDCType[i].ulID)
        {
            pDict->Add(&s_astDCType[i]);
        }
    }
}

AnsiString GetDCTypeText(UINT32 ulDCType)
{
    AnsiString  strText = "";
    bool        bAll = true;

    for (UINT32 i=0; i<ARRAY_SIZE(s_astDCType); i++)
    {
        if ((s_astDCType[i].ulID & ulDCType) == s_astDCType[i].ulID)
        {
            if (strText.IsEmpty())
            {
                strText = s_astDCType[i].szValue;
            }
            else
            {
                strText += AnsiString(",") + AnsiString(s_astDCType[i].szValue);
            }
        }
        else
        {
            bAll = false;
        }
    }

    if (bAll)
    {
        return "所有调度台";
    }

    return strText;
}
/*
AnsiString GetDCTypeText(UINT32 ulDCType)
{
    if (ulDCType == DC_TYPE_ALL)
    {
        return "全功能调度台";
    }

    AnsiString  strText = "";

    for (UINT32 i=0; i<ARRAY_SIZE(s_astDCType); i++)
    {
        if ((s_astDCType[i].ulID & ulDCType) == s_astDCType[i].ulID)
        {
            if (strText.IsEmpty())
            {
                strText = s_astDCType[i].szValue;
            }
            else
            {
                strText += AnsiString(",") + AnsiString(s_astDCType[i].szValue);
            }
        }
    }

    return strText;
}
*/

CHAR* GetNativeServicePath()
{
    if (g_szNativeServicePath)
    {
        return g_szNativeServicePath;
    }
    else
    {
        return "";
    }
}

VOID SetNativeServicePath(CHAR *szPath)
{
    if (szPath)
    {
        g_szNativeServicePath = strdup(szPath);
    }
}

UINT32 GetMaxListenPeriod()
{
    return g_ulMaxListenPeriod;
}

UINT32 GetGroupCallIdleTime()
{
    return g_ulGroupCallIdleTime;
}

UINT32 GetATSValidPeriod()
{
    return g_ulATSValidPeriod;
}

VOID SetATSValidPeriod(UINT32 ulValue)
{
    g_ulATSValidPeriod = ulValue;
}

UINT32 GetMaxGroupCallRecordTime()
{
    return g_ulMaxGroupCallRecordTime;
}

UINT32 GetMaxGroupCallRecordIdleTime()
{
    return g_ulMaxGroupCallRecordIdleTime;
}

BOOL IsShowCallWin()
{
    return g_bShowCallWin;
}

VOID SetShowCallWin(BOOL bValue)
{
    g_bShowCallWin = bValue;
}

AnsiString GetATSAddr1()
{
    return g_strATSAddr1;
}

AnsiString GetATSAddr2()
{
    return g_strATSAddr2;
}

VOID SetATSAddr1(AnsiString strAddr)
{
    g_strATSAddr1 = strAddr;
}

VOID SetATSAddr2(AnsiString strAddr)
{
    g_strATSAddr2 = strAddr;
}

BOOL IsOriginalVideoFormat()
{
    return g_bOriginalVideoFormat;
}

VOID SetOriginalVideoFormat(BOOL bValue)
{
    g_bOriginalVideoFormat = bValue;
}

BOOL IsIPHVideoOnTcpMode()
{
    return g_bIPHVideoOnTcpMode;
}

VOID SetIPHVideoOnTcpMode(BOOL bValue)
{
    g_bIPHVideoOnTcpMode = bValue;
}

UINT32 GetIPHVideoOpenTimeout()
{
    return g_ulIPHVideoOpenTimeout;
}

VOID SetIPHVideoOpenTimeout(UINT32 ulValue)
{
    g_ulIPHVideoOpenTimeout = ulValue;
}

BOOL IsIPHRecMergeMode()
{
    return g_bIPHRecMergeMode;
}

VOID SetLocalIPHAECMode(BOOL bValue)
{
    g_bLocalIPHAECMode = bValue;
}

BOOL IsLocalIPHAECMode()
{
    return g_bLocalIPHAECMode;
}

VOID SetIPHRecMergeMode(BOOL bValue)
{
    g_bIPHRecMergeMode = bValue;
}

BOOL DirectSendIPHAudio()
{
    return g_bDirectSendIPHAudio;
}

VOID SetDirectSendIPHAudio(BOOL bValue)
{
    g_bDirectSendIPHAudio = bValue;
}

UINT32 GetIPHAudioSampleRate()
{
    return g_ulIPHAudioSampleRate;
}

VOID SetIPHAudioSampleRate(UINT32 ulValue)
{
    g_ulIPHAudioSampleRate = ulValue;
}

UINT32 GetIPHAudioBitsPerSample()
{
    return g_ulIPHAudioBitsPerSample;
}

VOID SetIPHAudioBitsPerSample(UINT32 ulValue)
{
    g_ulIPHAudioBitsPerSample = ulValue;
}

BOOL DirectSendPAAudio()
{
    return g_bDirectSendPAAudio;
}

VOID SetDirectSendPAAudio(BOOL bValue)
{
    g_bDirectSendPAAudio = bValue;
}

UINT32 GetPAAudioSampleRate()
{
    return g_ulPAAudioSampleRate;
}

VOID SetPAAudioSampleRate(UINT32 ulValue)
{
    g_ulPAAudioSampleRate = ulValue;
}

UINT32 GetPAAudioBitsPerSample()
{
    return g_ulPAAudioBitsPerSample;
}

VOID SetPAAudioBitsPerSample(UINT32 ulValue)
{
    g_ulPAAudioBitsPerSample = ulValue;
}

VOID SetAllowPAAECMode(BOOL bEnabled)
{
    g_bAllowPAAECMode = bEnabled;
}

BOOL AllowPAAECMode()
{
    return g_bAllowPAAECMode;
}

VOID SetAllowIPHAECMode(BOOL bEnabled)
{
    g_bAllowIPHAECMode = bEnabled;
}

BOOL AllowIPHAECMode()
{
    return g_bAllowIPHAECMode;
}

VOID SetPAMaxPkgSize(UINT32 ulValue)
{
    g_ulPAMaxPkgSize = ulValue;
}

UINT32 GetPAMaxPkgSize()
{
    return g_ulPAMaxPkgSize;
}

VOID SetIPHMaxPkgSize(UINT32 ulValue)
{
    g_ulIPHMaxPkgSize = ulValue;
}

UINT32 GetIPHMaxPkgSize()
{
    return g_ulIPHMaxPkgSize;
}

// 本版本不再支持延迟发送，通过采集模块控制采集间隔
VOID SetPADelaySendTime(UINT32 ulValue)
{
    g_ulPADelaySendTime = 0;// ulValue;
}

UINT32 GetPADelaySendTime()
{
    return g_ulPADelaySendTime;
}

VOID SetMaxIPHTalkTime(UINT32 ulValue)
{
    g_ulMaxIPHTalkTime = ulValue;
}

UINT32 GetMaxIPHTalkTime()
{
    return g_ulMaxIPHTalkTime;
}

VOID GetNTPCfg(NTP_CFG_T &stCfg)
{
    memcpy(&stCfg, &g_stNTPCfg, sizeof(stCfg));
}

VOID SetNTPCfg(NTP_CFG_T &stCfg)
{
    memcpy(&g_stNTPCfg, &stCfg, sizeof(stCfg));
}

BOOL SendSMSByRealMsg()
{
    return g_bSendSMSByRealMsg;
}

VOID SetSendSMSByRealMsg(BOOL bValue)
{
    g_bSendSMSByRealMsg = bValue;
}

VOID SetSimpleSMSMode(BOOL bValue)
{
    g_bSimpleSMSMode = bValue;
}

BOOL IsSimpleSMSMode()
{
    return g_bSimpleSMSMode;
}

BOOL AllowHalfDuplexP2PCall()
{
    return g_bAllowHalfDuplexP2PCall;
}

VOID SetAllowHalfDuplexP2PCall(BOOL bValue)
{
    g_bAllowHalfDuplexP2PCall = bValue;
}

BOOL AllowGISSub()
{
    return g_bAllowGISSub;
}

VOID SetAllowGISSub(BOOL bValue)
{
    g_bAllowGISSub = bValue;
}

BOOL AllowVideoWall()
{
    return g_bAllowVideoWall;
}

VOID SetAllowVideoWall(BOOL bValue)
{
    g_bAllowVideoWall = bValue;
}

BOOL AllowP2PVideoCall()
{
    return g_bAllowP2PVideoCall;
}

VOID SetAllowP2PVideoCall(BOOL bValue)
{
    g_bAllowP2PVideoCall = bValue;
}

BOOL AutoCloseMulticastCall()
{
    return g_bAutoCloseMulticastCall;
}

VOID SetAutoCloseMulticastCall(BOOL bValue)
{
    g_bAutoCloseMulticastCall = bValue;
}

BOOL IsConnectToDIS()
{
    return g_bConnectToDIS;
}

VOID SetConnectToDIS(BOOL bConnected)
{
    g_bConnectToDIS = bConnected;
}

BOOL IsATSValid()
{
    return g_bATSValid;
}

VOID SetATSValid(BOOL bValid)
{
    g_bATSValid = bValid;
}

BOOL IsNTPValid()
{
    return g_bNTPValid;
}

VOID SetNTPValid(BOOL bValid)
{
    g_bNTPValid = bValid;
}

BOOL IsMDCValid()
{
    return g_bMDCValid;
}

VOID SetMDCValid(BOOL bValid)
{
    g_bMDCValid = bValid;
}

AnsiString GetDCTypeName(UINT32 ulDCType)
{
    for (UINT32 i=0; i<ARRAY_SIZE(s_astDCType); i++)
    {
        if (s_astDCType[i].ulID == ulDCType)
        {
            return s_astDCType[i].szValue;
        }
    }

    return "普通调度台";
}

void InitDCTypeDict(UINT32 ulDCType, Dict &DictDCType)
{
    DictDCType.Clear();
    for (UINT32 i=0; i<ARRAY_SIZE(s_astDCType); i++)
    {
        if (ulDCType & s_astDCType[i].ulID)
        {
            DictDCType.Add(s_astDCType[i].ulID, s_astDCType[i].szValue);
        }
    }
}

AnsiString GetCallTypeText(UINT32 ulCallType, UINT32 ulReqCallType)
{
    if (ulCallType == CALL_TYPE_PTT)            return "组呼";
    if (ulCallType == CALL_TYPE_EMERGENCY_CALL) return "紧急呼叫";
    if (ulCallType == CALL_TYPE_TRAIN_BRD)      return "列车广播";
    if (ulCallType == CALL_TYPE_ZONE_BRD)       return "选区广播";

    if (ulCallType == TRAIN_REGISTER)           return "列车注册";
    if (ulCallType == TRAIN_SET_TRAINID)        return "设置车次号";

    if (ulCallType == CALL_TYPE_P2P)                    return "个呼";
    if (ulCallType == CALL_TYPE_P2P_EMERGENCY_CALL)     return "紧急个呼";
    if (ulCallType == CALL_TYPE_P2P_TRANSFER)           return "个呼转接";
    if (ulCallType == CALL_TYPE_LRBUS_REQ_CALL)         return "BUS请呼";
    if (ulCallType == CALL_TYPE_HALF_DUPLEX_P2P_CALL)   return "半双工语音点呼";

    if (ulCallType == CALL_TYPE_VIDEO_P2P)      return "视频个呼";
    if (ulCallType == CALL_TYPE_VIDEO_REALPLAY) return "视频上拉";
    if (ulCallType == CALL_TYPE_VIDEO_RECV)     return "视频回传";

    if (ulCallType == CALL_TYPE_VIDEO_DISPATCH) return "视频转发";
    if (ulCallType == CALL_TYPE_VIDEO_ONWALL)   return "视频上墙";
    if (ulCallType == CALL_TYPE_VIDEO_PTT)      return "视频组呼";

    if (ulCallType == CALL_TYPE_IPH)            return "乘客对讲";
    if (ulCallType == CALL_TYPE_ENV_LISTEN)     return "环境侦听";

    if (ulCallType == CALL_TYPE_REQ_CALL)
    {
        if (ulReqCallType == TRAIN_REQ_CALL_DC)             return "列车请呼";
        if (ulReqCallType == TRAIN_EMERGENCY_REQ_CALL_DC)   return "列车紧急请呼";
        if (ulReqCallType == TRAIN_REQ_CALL_STATION)        return "列车请呼车站";
        if (ulReqCallType == TRAIN_REQ_CALL_FRONT_TRAIN)    return "呼叫前车";
        if (ulReqCallType == TRAIN_REQ_CALL_BACK_TRAIN)     return "呼叫后车";
        if (ulReqCallType == STATION_REQ_CALL_DC)           return "车站请呼";
        if (ulReqCallType == STATION_EMERGENCY_REQ_CALL_DC) return "车站紧急请呼";
        if (ulReqCallType == STATION_REQ_CALL_TRAIN)        return "站管区呼叫";

        return "请呼";
    }

    return "";
}

AnsiString GetP2PCallStatusText(UINT32 ulStatus)
{
    if (ulStatus == CALL_STATUS_CLOSE)      return "已取消";
    if (ulStatus == CALL_STATUS_DIAL)       return "呼叫中";
    if (ulStatus == CALL_STATUS_DIAL_FAIL)  return "呼叫失败";
    if (ulStatus == CALL_STATUS_RECVED)     return "振铃中";
    if (ulStatus == CALL_STATUS_RECV_SUCC)  return "已接听";
    if (ulStatus == CALL_STATUS_RECV_FAIL)  return "接听失败";
    if (ulStatus == CALL_STATUS_PLAY)       return "开始播放";
    if (ulStatus == CALL_STATUS_PLAY_FAIL)  return "播放失败";
    if (ulStatus == CALL_STATUS_REJECT)     return "拒接";
    if (ulStatus == CALL_STATUS_BUSY)       return "结束";
    if (ulStatus == CALL_STATUS_HANGUP)     return "结束";
    if (ulStatus == CALL_STATUS_NOANSWER)   return "无应答";
    if (ulStatus == CALL_STATUS_TIMEOUT)    return "超时";

    return AnsiString("未知状态");
}

static ID_VALUE_T   s_astSDSType[] = {
                        {SDS_TYPE_NULL,             ""},
                        {SDS_TYPE_FORMAL_DISPATCH,  "书面命令"},
                        {SDS_TYPE_ORAL_DISPATCH,    "口头命令"},
                        {SDS_TYPE_OPERATE,          "运营信息"},
                        {SDS_TYPE_NORMAL,           "短信"},
                        {SDS_TYPE_STATUS,           "状态信息"},
                        {SDS_TYPE_EMERG_STATUS,     "紧急状态信息"},
                        {SDS_TYPE_SMS,              "彩信"},
                                };


Dict* GetDictSDSType()
{
    static Dict     *DictSDSType = NULL;
    static ID_VALUE_T   astSimpleSDSType[] = {
                        {SDS_TYPE_NULL,             ""},
                        {SDS_TYPE_NORMAL,           "短信"},
                        {SDS_TYPE_STATUS,           "状态信息"},
                        {SDS_TYPE_EMERG_STATUS,     "紧急状态信息"},
                        {SDS_TYPE_SMS,              "彩信"},
                                };

    if (!DictSDSType)
    {
        if (!IsSimpleSMSMode())
        {
            DictSDSType = new Dict(s_astSDSType, ARRAY_SIZE(s_astSDSType));
        }
        else
        {
            DictSDSType = new Dict(astSimpleSDSType, ARRAY_SIZE(astSimpleSDSType));
        }
    }

    return DictSDSType;
}

Dict* GetDictSDSTemplateType()
{
    static Dict     *DictSDSType = NULL;

    if (!DictSDSType)
    {
        if (!IsSimpleSMSMode())
        {
            ID_VALUE_T      astSDSType[] = {
                        {SDS_TYPE_FORMAL_DISPATCH,  "书面命令"},
                        {SDS_TYPE_ORAL_DISPATCH,    "口头命令"},
                        {SDS_TYPE_OPERATE,          "运营信息"},
                        {SDS_TYPE_NORMAL,           "短信"},
                        {SDS_TYPE_STATUS,           "状态信息"},
                        {SDS_TYPE_EMERG_STATUS,     "紧急状态信息"},
                                };

            DictSDSType = new Dict(astSDSType, ARRAY_SIZE(astSDSType));
        }
        else
        {
            ID_VALUE_T   astSimpleSDSType[] = {
                        {SDS_TYPE_NORMAL,           "短信"},
                        {SDS_TYPE_STATUS,           "状态信息"},
                        {SDS_TYPE_EMERG_STATUS,     "紧急状态信息"},
                                };

            DictSDSType = new Dict(astSimpleSDSType, ARRAY_SIZE(astSimpleSDSType));
        }
    }

    return DictSDSType;
}

CHAR* GetSDSTypeText(UINT32 ulType)
{
    Dict    *DictSDSType = GetDictSDSType();

    return DictSDSType->GetValue(ulType);
}

VOID LoadIPHVideoSize(int &iVideoWidth, int &iVideoHeight)
{
    LoadConfInfo("Param", "IPHVideoWidth", &iVideoWidth);
    LoadConfInfo("Param", "IPHVideoHeight", &iVideoHeight);

    if (iVideoWidth <= 0 || iVideoHeight <= 0)
    {
        iVideoWidth  = 704;
        iVideoHeight = 576;
    }
}

VOID SaveIPHVideoSize(int iVideoWidth, int iVideoHeight)
{
    SaveConfInfo("Param", "IPHVideoWidth", iVideoWidth);
    SaveConfInfo("Param", "IPHVideoHeight", iVideoHeight);
}

static ID_VALUE_T s_astStationType[] = {{STATION_TYPE_NORMAL,   "正线"},
                                        {STATION_TYPE_DEPOT,    "车辆段"},
                                        {STATION_TYPE_FIX,      "维修段"}};

Dict    g_DictStationType(s_astStationType, ARRAY_SIZE(s_astStationType));

AnsiString GetStationTypeText(UINT32 ulStationType)
{
    for (UINT32 i=0; i<ARRAY_SIZE(s_astStationType); i++)
    {
        if (s_astStationType[i].ulID == ulStationType)
        {
            return s_astStationType[i].szValue;
        }
    }

    return "";
}

static ID_VALUE_T  s_astVideoFormat[] = {   {VIDEO_FMT_CIF,     "CIF"},
                                            {VIDEO_FMT_QCIF,    "QCIF"},
                                            {VIDEO_FMT_D1,      "D1"},
                                            {VIDEO_FMT_720P,    "720P"},
                                            {VIDEO_FMT_1080P,   "1080P"}};

Dict g_DictVideoFormat(s_astVideoFormat, ARRAY_SIZE(s_astVideoFormat));
