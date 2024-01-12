#include "ClientInclude.h"

#include "eSDK.h"

static CHAR     *g_szNativeServicePath = NULL;

static BOOL     g_bManageOneDepot = FALSE;  // TRUE���γ�����ֻ̨�ܹ���һ���γ� FALSE���������жγ�

static UINT32   g_ulMaxListenPeriod = 60;   // �������ʱ��
static UINT32   g_ulGroupCallIdleTime = 60; // ���������ʱ�䣬��ʱ��ʾ���н���

static UINT32   g_ulATSValidPeriod = 15;    // �ж�ATS�Ͽ��ļ��ʱ��

static UINT32   g_ulMaxGroupCallRecordTime = 300;       // �������¼ʱ��
static UINT32   g_ulMaxGroupCallRecordIdleTime = 15;    // ����޻�Ȩ�����ȴ�ʱ�䣬������ֵ��������¼��

static AnsiString   g_strATSAddr1 = "";  ///< ATS ����� IP ��ַ, ������Դ dis(db: appcfg)
static AnsiString   g_strATSAddr2 = "";  ///< ATS ���÷���� IP ��ַ, ������Դ dis(db: appcfg)

static BOOL     g_bShowCallWin = TRUE;
static BOOL     g_bOriginalVideoFormat = FALSE;
static BOOL     g_bIPHVideoOnTcpMode = TRUE;        // RTSP��Ƶ������TCP��ʽ��ͨ������UDO��ʽ��
static BOOL     g_ulIPHVideoOpenTimeout = 1;        // ��IPH��Ƶʱ��
static BOOL     g_bIPHRecMergeMode = TRUE;          // IPH����Ƶ¼���ļ��Ƿ�ϲ�
static BOOL     g_bLocalIPHAECMode = FALSE;         // ����̨���ؿ���AEC����
static BOOL     g_bDirectSendIPHAudio = TRUE;       // �Ƿ�ֱ�ӷ��ͶԽ���Ƶ��IPH�豸
static UINT32   g_ulIPHAudioSampleRate = 16000;
static UINT32   g_ulIPHAudioBitsPerSample = 16;

static BOOL     g_bDirectSendPAAudio = TRUE;       // �Ƿ�ֱ�ӷ����˹��㲥��Ƶ��PA�豸
static UINT32   g_ulPAAudioSampleRate = 16000;
static UINT32   g_ulPAAudioBitsPerSample = 16;

static BOOL     g_bAllowIPHAECMode = FALSE;
static BOOL     g_bAllowPAAECMode = FALSE;

static UINT32   g_ulPAMaxPkgSize = 1000;
static UINT32   g_ulIPHMaxPkgSize = 1000;

static UINT32   g_ulPADelaySendTime = 0;

static UINT32   g_ulMaxIPHTalkTime = 0;         // IPH������������ȴ�ʱ��

static BOOL     g_bSendSMSByRealMsg = FALSE;
static BOOL     g_bSimpleSMSMode = TRUE;
static BOOL     g_bAllowHalfDuplexP2PCall = FALSE;

static BOOL     g_bAllowVideoWall = FALSE;
static BOOL     g_bAllowGISSub = FALSE;
static BOOL     g_bAllowP2PVideoCall = TRUE;
static BOOL     g_bAutoCloseMulticastCall = TRUE;

static NTP_CFG_T    g_stNTPCfg = {{0,0,0,0}, 123, 60};  ///< NTP��ַ��������ڣ� ������Դ: dis(db: appcfg)

static BOOL     g_bConnectToDIS = TRUE;
static BOOL     g_bATSValid = TRUE;
static BOOL     g_bNTPValid = TRUE;
static BOOL     g_bMDCValid = TRUE;

static ID_VALUE_T  s_astDCType[] = {
                                {DC_TYPE_TRAIN,     "�г�����̨"},
                                {DC_TYPE_SHUNT,     "��������̨"},
                                {DC_TYPE_PASSENGER, "�˿͵���̨"},
                                {DC_TYPE_DEPOT,     "�γ�����̨"},
//                                {DC_TYPE_FIX,       "ά�޵���̨"},
                                {DC_TYPE_ENV,       "�豸����̨"},
//                                {DC_TYPE_POWER,     "��������̨"},
                                {DC_TYPE_MANAGER,   "ά������̨"},
                                {DC_TYPE_MASTER,    "�ܿص���̨"},
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
        return "���е���̨";
    }

    return strText;
}
/*
AnsiString GetDCTypeText(UINT32 ulDCType)
{
    if (ulDCType == DC_TYPE_ALL)
    {
        return "ȫ���ܵ���̨";
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

// ���汾����֧���ӳٷ��ͣ�ͨ���ɼ�ģ����Ʋɼ����
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

    return "��ͨ����̨";
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
    if (ulCallType == CALL_TYPE_PTT)            return "���";
    if (ulCallType == CALL_TYPE_EMERGENCY_CALL) return "��������";
    if (ulCallType == CALL_TYPE_TRAIN_BRD)      return "�г��㲥";
    if (ulCallType == CALL_TYPE_ZONE_BRD)       return "ѡ���㲥";

    if (ulCallType == TRAIN_REGISTER)           return "�г�ע��";
    if (ulCallType == TRAIN_SET_TRAINID)        return "���ó��κ�";

    if (ulCallType == CALL_TYPE_P2P)                    return "����";
    if (ulCallType == CALL_TYPE_P2P_EMERGENCY_CALL)     return "��������";
    if (ulCallType == CALL_TYPE_P2P_TRANSFER)           return "����ת��";
    if (ulCallType == CALL_TYPE_LRBUS_REQ_CALL)         return "BUS���";
    if (ulCallType == CALL_TYPE_HALF_DUPLEX_P2P_CALL)   return "��˫���������";

    if (ulCallType == CALL_TYPE_VIDEO_P2P)      return "��Ƶ����";
    if (ulCallType == CALL_TYPE_VIDEO_REALPLAY) return "��Ƶ����";
    if (ulCallType == CALL_TYPE_VIDEO_RECV)     return "��Ƶ�ش�";

    if (ulCallType == CALL_TYPE_VIDEO_DISPATCH) return "��Ƶת��";
    if (ulCallType == CALL_TYPE_VIDEO_ONWALL)   return "��Ƶ��ǽ";
    if (ulCallType == CALL_TYPE_VIDEO_PTT)      return "��Ƶ���";

    if (ulCallType == CALL_TYPE_IPH)            return "�˿ͶԽ�";
    if (ulCallType == CALL_TYPE_ENV_LISTEN)     return "��������";

    if (ulCallType == CALL_TYPE_REQ_CALL)
    {
        if (ulReqCallType == TRAIN_REQ_CALL_DC)             return "�г����";
        if (ulReqCallType == TRAIN_EMERGENCY_REQ_CALL_DC)   return "�г��������";
        if (ulReqCallType == TRAIN_REQ_CALL_STATION)        return "�г������վ";
        if (ulReqCallType == TRAIN_REQ_CALL_FRONT_TRAIN)    return "����ǰ��";
        if (ulReqCallType == TRAIN_REQ_CALL_BACK_TRAIN)     return "���к�";
        if (ulReqCallType == STATION_REQ_CALL_DC)           return "��վ���";
        if (ulReqCallType == STATION_EMERGENCY_REQ_CALL_DC) return "��վ�������";
        if (ulReqCallType == STATION_REQ_CALL_TRAIN)        return "վ��������";

        return "���";
    }

    return "";
}

AnsiString GetP2PCallStatusText(UINT32 ulStatus)
{
    if (ulStatus == CALL_STATUS_CLOSE)      return "��ȡ��";
    if (ulStatus == CALL_STATUS_DIAL)       return "������";
    if (ulStatus == CALL_STATUS_DIAL_FAIL)  return "����ʧ��";
    if (ulStatus == CALL_STATUS_RECVED)     return "������";
    if (ulStatus == CALL_STATUS_RECV_SUCC)  return "�ѽ���";
    if (ulStatus == CALL_STATUS_RECV_FAIL)  return "����ʧ��";
    if (ulStatus == CALL_STATUS_PLAY)       return "��ʼ����";
    if (ulStatus == CALL_STATUS_PLAY_FAIL)  return "����ʧ��";
    if (ulStatus == CALL_STATUS_REJECT)     return "�ܽ�";
    if (ulStatus == CALL_STATUS_BUSY)       return "����";
    if (ulStatus == CALL_STATUS_HANGUP)     return "����";
    if (ulStatus == CALL_STATUS_NOANSWER)   return "��Ӧ��";
    if (ulStatus == CALL_STATUS_TIMEOUT)    return "��ʱ";

    return AnsiString("δ֪״̬");
}

static ID_VALUE_T   s_astSDSType[] = {
                        {SDS_TYPE_NULL,             ""},
                        {SDS_TYPE_FORMAL_DISPATCH,  "��������"},
                        {SDS_TYPE_ORAL_DISPATCH,    "��ͷ����"},
                        {SDS_TYPE_OPERATE,          "��Ӫ��Ϣ"},
                        {SDS_TYPE_NORMAL,           "����"},
                        {SDS_TYPE_STATUS,           "״̬��Ϣ"},
                        {SDS_TYPE_EMERG_STATUS,     "����״̬��Ϣ"},
                        {SDS_TYPE_SMS,              "����"},
                                };


Dict* GetDictSDSType()
{
    static Dict     *DictSDSType = NULL;
    static ID_VALUE_T   astSimpleSDSType[] = {
                        {SDS_TYPE_NULL,             ""},
                        {SDS_TYPE_NORMAL,           "����"},
                        {SDS_TYPE_STATUS,           "״̬��Ϣ"},
                        {SDS_TYPE_EMERG_STATUS,     "����״̬��Ϣ"},
                        {SDS_TYPE_SMS,              "����"},
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
                        {SDS_TYPE_FORMAL_DISPATCH,  "��������"},
                        {SDS_TYPE_ORAL_DISPATCH,    "��ͷ����"},
                        {SDS_TYPE_OPERATE,          "��Ӫ��Ϣ"},
                        {SDS_TYPE_NORMAL,           "����"},
                        {SDS_TYPE_STATUS,           "״̬��Ϣ"},
                        {SDS_TYPE_EMERG_STATUS,     "����״̬��Ϣ"},
                                };

            DictSDSType = new Dict(astSDSType, ARRAY_SIZE(astSDSType));
        }
        else
        {
            ID_VALUE_T   astSimpleSDSType[] = {
                        {SDS_TYPE_NORMAL,           "����"},
                        {SDS_TYPE_STATUS,           "״̬��Ϣ"},
                        {SDS_TYPE_EMERG_STATUS,     "����״̬��Ϣ"},
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

static ID_VALUE_T s_astStationType[] = {{STATION_TYPE_NORMAL,   "����"},
                                        {STATION_TYPE_DEPOT,    "������"},
                                        {STATION_TYPE_FIX,      "ά�޶�"}};

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
