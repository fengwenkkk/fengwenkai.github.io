#include "eLTE_SDK.h"

#include "ClientInclude.h"
#include "msg.h"

#include "eSDK.h"
#include "XML.h"
#include "SimpleXml.h"

#define ESDKLog(ulLogLevel, ...)      GosLog("SDK", ulLogLevel, ##__VA_ARGS__)

#define ESDK_SERVICE_NAME   "HUAWEI eSDK eLTE Service"

#define ESDK_LOG(iRet)\
{\
    if (iRet != 0)\
    {\
        GosLog("SDK", LOG_ERROR, "%s errcode: %d", __FUNC__, iRet);\
    }\
}

#define ESDK_RET(iRet)\
{\
    ESDK_LOG(iRet);\
    return (iRet == 0);\
}

//#define MDC_CHECK
#define MDC_CHECK\
{\
    if (!g_bMDCLogined)\
    {\
        return FALSE;\
    }\
}

#define MRS_CHECK\
{\
    if (!g_bMDCLogined || !g_bMRSLogined)\
    {\
        return FALSE;\
    }\
}

#define ESDK_ERR_NO_SUBSCRIPTION    (-40011)


static BOOL     g_bESDKInit = FALSE;
static BOOL     g_bMRSLogined = FALSE;
static BOOL     g_bMDCLogined = FALSE;
static CHAR     g_acCurrDCUserID[128] = {0};

#define IntToStr(x)     AnsiString(x).c_str()

class XmlParam
{
public:
    XmlParam()
    {
        m_strParam = "";
    }

    ~XmlParam()
    {
    }

    void Add(AnsiString strName, AnsiString strValue)
    {
        m_strParam += GetNameValue(strName, strValue);
    }

    void Add(AnsiString strName, XmlParam &Param)
    {
        Add(strName, Param.GetString());
    }

    AnsiString GetString()
    {
        return m_strParam;
    }

    const char* ToStr()
    {
        m_strWholeParam = GetNameValue("Content", GetString());

        return m_strWholeParam.c_str();
    }

    operator const char*(){return ToStr();};

private:
    AnsiString  m_strParam;
    AnsiString  m_strWholeParam;

    AnsiString GetNameValue(AnsiString strName, AnsiString strValue)
    {
        return "<" + strName + ">" + strValue + "</" + strName + ">";
    }
};

AnsiString EncodeSMSMsg(AnsiString strMsgInfo)
{
    ReplaceString(strMsgInfo, "\"", "&quot;");

    return strMsgInfo;
}

/*static*/ AnsiString DecodeSMSMsg(AnsiString strSDSText)
{
    char    *aszFmt[][2] = {{"&#x20;", " "},   // 空格
                            {"&#x09;", "\t"},  // TAB
                            {"&#x0D;", "\r"},  // 回车
                            {"&#x0A;", "\n"},  // 换行
                            {"&apos;", "'"},   // 单引号，还要加上双引号才能生效?apos;?
                            {"&quot;", "\""},  // 双引卿
                            {"&lt;"  , "<"},
                            {"&gt;"  , ">"},
                            {"&amp;" , "&"},
                         //   {"&#8230;", "..."},// 省略
                           };
    AnsiString  strFormatedInfo = strSDSText;

    for (UINT32 i=0; i<ARRAY_SIZE(aszFmt); i++)
    {
        ReplaceString(strFormatedInfo, aszFmt[i][0], aszFmt[i][1]);
    }

    return strFormatedInfo;
}

VOID eSDK_Free(CHAR *str)
{
    if (str)
    {
        ELTE_SDK_ReleaseBuffer(str);
    }
}

BOOL eSDK_IsLogined()
{
    return g_bMDCLogined;
}

VOID eSDK_Logined()
{
    g_bMDCLogined = TRUE;
}

BOOL eSDK_IsMRSLogined()
{
    return g_bMRSLogined;
}

VOID eSDK_StartLocalService()
{
    UINT32  ulSvcStatus = eSDK_GetLocalServiceStatus();

    if (SERVICE_RUNNING == ulSvcStatus)
    {
        return;
    }

    GosLog(LOG_INFO, "start local native service");
    gos_start_service(ESDK_SERVICE_NAME);
}

VOID eSDK_StopLocalService()
{
    UINT32  ulSvcStatus = eSDK_GetLocalServiceStatus();

    if (SERVICE_STOPPED == ulSvcStatus)
    {
        return;
    }

    GosLog(LOG_INFO, "stop local native service");
    gos_stop_service(ESDK_SERVICE_NAME);
}

UINT32 eSDK_GetLocalServiceStatus()
{
    return gos_query_svcstatus(ESDK_SERVICE_NAME);
}

BOOL eSDK_Init(eSDK_EventCallBack fEventCallBack)
{
    INT32 iRet = 0;

    if (!g_bESDKInit)
    {
        iRet = ELTE_SDK_Init(0);
        if (iRet != 0)
        {
            ESDKLog(LOG_ERROR, "ELTE_SDK_Init failed, err code is %d", iRet);
            return FALSE;
        }

        iRet = ELTE_SDK_SetEventCallBack((EventCallBack)fEventCallBack, NULL);
        if (iRet != 0)
        {
            return FALSE;
        }

        g_bESDKInit = TRUE;
    }

    return TRUE;
}

VOID eSDK_Cleanup()
{
    ELTE_SDK_Cleanup();
    g_bESDKInit = FALSE;
    g_bMRSLogined = FALSE;
    g_bMDCLogined = FALSE;
}

BOOL eSDK_Login(CHAR* szUserID, CHAR* szPwd, UINT8* pucServerIP, UINT16 usServerSIPPort, UINT8* pucLocalIP)
{
    CHAR    acServerIP[32];
    CHAR    acLocalIP[32];
    AnsiString  strUserID = szUserID;

    if (g_bMDCLogined)
    {
        return TRUE;
    }

    g_bMRSLogined = FALSE;

    sprintf(acServerIP, IP_FMT, IP_ARG(pucServerIP));
    sprintf(acLocalIP, IP_FMT, IP_ARG(pucLocalIP));

    ESDKLog(LOG_INFO, "login: %s %s %s %s %u", szUserID, szPwd, acServerIP, acLocalIP, usServerSIPPort);

    INT32   iRet = ELTE_SDK_Login(szUserID, szPwd, acServerIP, acLocalIP, usServerSIPPort);

    if (iRet == 0)
    {
        g_bMDCLogined = TRUE;
        strcpy(g_acCurrDCUserID, szUserID);
    }

    ESDK_RET(iRet);
}

BOOL eSDK_LoginMRS(UINT8* pucServerIP)
{
    if (g_bMRSLogined)
    {
        return TRUE;
    }

    INT32   iRet;
    CHAR    acServerIP[32];

    sprintf(acServerIP, IP_FMT, IP_ARG(pucServerIP));
    iRet = ELTE_SDK_ProvisionManagerInitMRS(acServerIP);
    if (iRet == 0)
    {
        g_bMRSLogined = TRUE;
    }

    ESDK_RET(iRet);
}

BOOL eSDK_Logout(ELTE_CHAR* szUserID)
{
    INT32   iRet = ELTE_SDK_Logout(szUserID);

    if (iRet == 0)
    {
        g_bMDCLogined = FALSE;
        g_bMRSLogined = FALSE;

        g_acCurrDCUserID[0] = '\0';
    }

    ESDK_RET(iRet);
}

AnsiString eSDK_GetVer()
{
    ELTE_CHAR   *szChar = NULL;
    ELTE_INT32  iRet = ELTE_SDK_GetVersion(&szChar);
    AnsiString  strVer = "";

    ESDK_LOG(iRet);
    if (iRet == 0)
    {
        strVer = szChar;
    }

    eSDK_Free(szChar);

    return strVer;
}

VOID eSDK_TriggerStatusReport(BOOL bEnable)
{
    INT32   iStatus = bEnable?1:0;

    ELTE_SDK_TriggerStatusReport(iStatus);
}

BOOL eSDK_GetSpeakerList(std::vector<AnsiString> &vInfo)
{
    MDC_CHECK;

    ELTE_CHAR   *szInfo = NULL;
    INT32       iRet = ELTE_SDK_EnumSpeakers(&szInfo);
    char        *szName;
    XML         Xml(szInfo, TRUE);
    std::vector<XML*>   vList;

    eSDK_Free(szInfo);
    vInfo.clear();

    ESDK_LOG(iRet);
    if (iRet != 0)
    {
        return FALSE;
    }

    Xml.GetNode("Content")->GetNode("SpeakersArray")->GetNode("SpearkersInfo", vList);
    for (UINT32 i=0; i<vList.size(); i++)
    {
        szName = vList[i]->GetAttr("SperakerName");
        if (!szName)
        {
            return FALSE;
        }

        vInfo.push_back(szName);
    }

    return TRUE;
}

/*
MicrophonesArray 克风列表。
MicrophonesInfo  麦克风信息。
MicrophoneName
*/

BOOL eSDK_GetMicList(std::vector<AnsiString> &vInfo)
{
    MDC_CHECK;

    ELTE_CHAR   *szInfo = NULL;
    INT32       iRet = ELTE_SDK_EnumMicrophones(&szInfo);
    char        *szName;
    XML         Xml(szInfo, TRUE);
    std::vector<XML*>   vList;

    eSDK_Free(szInfo);
    vInfo.clear();

    ESDK_LOG(iRet);
    if (iRet != 0)
    {
        return FALSE;
    }

    Xml.GetNode("Content")->GetNode("MicrophonesArray")->GetNode("MicrophonesInfo", vList);
    for (UINT32 i=0; i<vList.size(); i++)
    {
        szName = vList[i]->GetAttr("MicrophoneName");
        if (!szName)
        {
            return FALSE;
        }

        vInfo.push_back(szName);
    }

    return TRUE;
}

BOOL eSDK_IsCameraValid()
{
    MDC_CHECK;

    ELTE_CHAR   *szInfo = NULL;
    INT32       iRet = ELTE_SDK_GetLocalCameraStatus(g_acCurrDCUserID, &szInfo);
    char        *szStatus;
    XML         Xml(szInfo, TRUE);

    eSDK_Free(szInfo);

    ESDK_LOG(iRet);
    if (iRet != 0)
    {
        return FALSE;
    }

    szStatus = Xml.GetNode("Content")->GetNode("LocalCameraStatus")->GetAttr("Status");
    if (!szStatus || strcmp(szStatus, "1") != 0)
    {
        return FALSE;
    }

    return TRUE;
}

// <Content><UserID>7007</UserID><UserName></UserName><StatusType>16</StatusType><StatusValue>4011</StatusValue><AttachingGroup>0</AttachingGroup></Content>
BOOL ParseEventUserStatus(CHAR *szEvent, EVENT_USER_STATUS_T *pstEvent)
{
    SimpleXml       Xml(szEvent);

    memset(pstEvent, 0, sizeof(EVENT_USER_STATUS_T));

    if (!Xml.GetField("UserID", &pstEvent->ulUserNumber) ||
        !Xml.GetField("StatusType", &pstEvent->ulStatusType) ||
        !Xml.GetField("StatusValue", &pstEvent->ulStatus) )
    {
        return FALSE;
    }

    return TRUE;
}


/*
//xml code
<Content>
<GroupID>1001</GroupID>
<GroupCallStatus>1006</GroupCallStatus>
<Speaker>4120</Speaker>
<LocalPort>64082</LocalPort>
<RemotePort>28034</RemotePort>
<RemoteIP>120.9.22.172</RemoteIP>
<SoundPtype>-1</SoundPtype>
<SpeakerName>4120</SpeakerName>
<CallID>0</CallID>
<SignalError>-1</SignalError>
<FromString></FromString>
<ToString></ToString>
</Content>*/


BOOL ParseEventP2PCallStatus(CHAR *szEvent, EVENT_P2PCALL_STATUS_T *pstEvent)
{
    SimpleXml       Xml(szEvent);

    memset(pstEvent, 0, sizeof(EVENT_P2PCALL_STATUS_T));

    if (!Xml.GetField("P2pcallStatus", &pstEvent->ulP2PCallStatus) ||
        !Xml.GetField("Caller", &pstEvent->ulCaller) ||
        !Xml.GetField("Callee", &pstEvent->ulCallee) ||
        !Xml.GetField("CallID", &pstEvent->ulCallID) )
    {
        return FALSE;
    }

    return TRUE;
}

BOOL ParseEventGroupStatus(CHAR *szEvent, EVENT_GROUP_STATUS_T *pstEvent)
{
    SimpleXml       Xml(szEvent);

    memset(pstEvent, 0, sizeof(EVENT_GROUP_STATUS_T));

    if (!Xml.GetField("GroupID", &pstEvent->i64GroupID) ||
        !Xml.GetField("GroupCallStatus", &pstEvent->ulGroupCallStatus) ||
        !Xml.GetField("Speaker", &pstEvent->ulSpeaker) ||
        !Xml.GetField("SoundPtype", &pstEvent->iSoundType) ||
        !Xml.GetField("SpeakerName", pstEvent->acSpeakerName, sizeof(pstEvent->acSpeakerName)))
    {
        return FALSE;
    }

    pstEvent->ulSpeakTime = gos_get_uptime();

    return TRUE;
}

BOOL ParseEventResourceStatus(CHAR *szEvent, EVENT_RESOURCE_STATUS_T *pstEvent)
{
    SimpleXml       Xml(szEvent);

    memset(pstEvent, 0, sizeof(EVENT_RESOURCE_STATUS_T));

    if (!Xml.GetField("ResourceID", &pstEvent->i64ResourceID) ||
        !Xml.GetField("ResourceName", pstEvent->acResourceName, sizeof(pstEvent->acResourceName)) ||
        !Xml.GetField("StatusType", &pstEvent->ulStatusType) ||
        !Xml.GetField("StatusValue", &pstEvent->ulStatusValue) ||
        !Xml.GetField("AttachingGroup", &pstEvent->i64AttachingGroupID)  )
    {
        return FALSE;
    }

    Xml.GetField("Speaker", pstEvent->acSpeaker, sizeof(pstEvent->acSpeaker));
    Xml.GetField("CameraID", &pstEvent->ulCameraID);

    return TRUE;
}

/*
<Content>
    <ChangeType>1</ChangeType>
    <ChangeMO>29</ChangeMO>
    <PatchGroupInfo>
        <GroupNumber>9999999999993</GroupNumber>
        <SetupDcId>1002</SetupDcId>
        <PGPriority>15</PGPriority>
        <DcPatchIndex>1</DcPatchIndex>
        <PGName>test_patch_40711</PGName>
        <VPNID>65535</VPNID>
    </PatchGroupInfo>
</Content>*/

static void FormatPatchGroupName(CHAR *szName, UINT32 ulDCPatchIndex)
{
    if (*szName == '\0')
    {
        return;
    }

    CHAR    acEnd[12];
    CHAR    *szEnd;

    sprintf(acEnd, "%u", ulDCPatchIndex);
    szEnd = gos_right_strstr(szName, acEnd);
    if (szEnd)
    {
        if (strlen(szEnd) == strlen(acEnd))
        {
            *szEnd = '\0';
        }
    }
}

BOOL ParseEventCreatePatchGroup(CHAR *szEvent, EVENT_CREATE_PATCH_GROUP_T *pstEvent)
{
    XML     xml(szEvent, TRUE);
    XML     *SubXML;

    SubXML = xml.GetNode("Content")->GetNode("PatchGroupInfo");

    memset(pstEvent, 0, sizeof(EVENT_CREATE_PATCH_GROUP_T));

    if (!SubXML->GetAttr("GroupNumber", &pstEvent->i64GroupID) ||
        !SubXML->GetAttr("PGName", pstEvent->acGroupName, sizeof(pstEvent->acGroupName)) ||
        !SubXML->GetAttr("DcPatchIndex", &pstEvent->ulPatchIndex) ||
        !SubXML->GetAttr("SetupDcId", &pstEvent->iDCUserID))
    {
        return FALSE;
    }

    FormatPatchGroupName(pstEvent->acGroupName, pstEvent->ulPatchIndex);

    return TRUE;
}

/*
SDSType     短数据类型   如果短数据类型为0001，表示可以点对点或发群组短信。
                         如果短数据类型为0002，表示终端群发给调度台的状态短信。
                         如果短数据类型为0004，表示可以点对点或发群组短信或者彩信。
SdsContent 消息内容。
SdsFrom 发件人。
SdsTo 收件人。
MsgState 短信状态：  0：在线。   1：离线。
EmergGroupID 紧急状态的群组ID。
EmergUeID 紧急状态的终端ID。
GroupID 群组ID。
SdsSubject 消息主题。
SdsDirection 接发短信标志。
SdsDate 消息发送日期。
SdsTime 消息发送时间。
StatusCode 短信的状态码（普通短信不适用）
*/
BOOL ParseEventSDSReport(CHAR *szEvent, EVENT_SDS_REPORT_T *pstEvent)
{
    XML     xml(szEvent, TRUE);
    XML     *SubXML;

    SubXML = xml.GetNode("Content");

    memset(pstEvent, 0, sizeof(EVENT_SDS_REPORT_T));

    if (!SubXML->GetAttr("SdsType", &pstEvent->ulSDSType) ||
        !SubXML->GetAttr("SdsContent", pstEvent->acContent, sizeof(pstEvent->acContent))||
        !SubXML->GetAttr("SdsFrom", pstEvent->acFrom, sizeof(pstEvent->acFrom))||
    //  !SubXML->GetAttr("MsgState", &pstEvent->ulMsgState) ||
    //  !SubXML->GetAttr("SdsSubject", pstEvent->acSubject, sizeof(pstEvent->acSubject))||
        !SubXML->GetAttr("SdsDate", pstEvent->acSendDate, sizeof(pstEvent->acSendDate))||
        !SubXML->GetAttr("SdsTime", pstEvent->acSendTime, sizeof(pstEvent->acSendTime)) )
    {
        return FALSE;
    }

    AnsiString  strDecodeMsg = DecodeSMSMsg(pstEvent->acContent);

    strncpy(pstEvent->acContent, strDecodeMsg.c_str(), sizeof(pstEvent->acContent)-1);

    VECTOR<XML*>    vFile;

    SubXML->GetNode("SdsMmsFileNameList", vFile);

    for (UINT32 i=0; i<vFile.size(); i++)
    {
        SubXML = vFile[i];
        if (!SubXML->GetAttr("SdsMmsFileName", pstEvent->acFile, sizeof(pstEvent->acFile)))
        {
            break;
        }
    }

    return TRUE;
}

BOOL ParseEventSDSSendStatus(CHAR *szEvent, EVENT_SDS_SEND_STATUS_T *pstEvent)
{
    XML     xml(szEvent, TRUE);
    XML     *SubXML;
    CHAR    acValue[32];

    SubXML = xml.GetNode("Content");

    memset(pstEvent, 0, sizeof(EVENT_SDS_SEND_STATUS_T));

    if (!SubXML->GetAttr("SdsReceiver", pstEvent->acReceiver, sizeof(pstEvent->acReceiver)) ||
        !SubXML->GetAttr("SdsSubject", pstEvent->acMsgID, sizeof(pstEvent->acMsgID))||
        !SubXML->GetAttr("SdsDirection", acValue, sizeof(acValue)) )
    {
        return FALSE;
    }

    CHAR    *szValue = SubXML->GetAttr("SdsRetCode");

    if (!szValue)
    {
        return FALSE;
    }

    if (strcasecmp(szValue, "0xff") == 0)
    {
        pstEvent->ulRetCode = 0xff;
    }
    else
    {
        if (!gos_atou(szValue, &pstEvent->ulRetCode))
        {
            return FALSE;
        }
    }

    if (strcmp(acValue, "true") == 0)
    {
        pstEvent->bSendFlag = TRUE;
    }
    else
    {
        pstEvent->bSendFlag = FALSE;
    }

    return TRUE;
}

/*
<Content><CallStatus>3011</CallStatus>
<Callee>1001</Callee><Caller>4071</Caller>
<LocalAudioPort>0</LocalAudioPort><LocalVideoPort>0</LocalVideoPort><RemoteAudioPort>0</RemoteAudioPort>
<RemoteVideoPort>0</RemoteVideoPort><RemoteIp>0.0.0.0</RemoteIp><Uri>4071</Uri><Channel>65535</Channel>
<SoundMute>0</SoundMute><UserConfirm>1</UserConfirm><Camera>1</Camera>
<SoundPtype>-1</SoundPtype>
<VideoFormatType>4</VideoFormatType>
<CallID>0</CallID><SignalError>-1</SignalError><FromString>4071</FromString>
<ToString>1001</ToString><Ptz>2</Ptz><isP2PVideoDial>0</isP2PVideoDial></Content>
*/
BOOL ParseEventP2PVideoCallStatust(CHAR *szEvent, EVENT_P2PVIDEO_STATUS_T *pstEvent)
{
    XML     xml(szEvent, TRUE);
    XML     *SubXML;

    SubXML = xml.GetNode("Content");

    memset(pstEvent, 0, sizeof(EVENT_P2PVIDEO_STATUS_T));

    if (!SubXML->GetAttr("CallStatus", &pstEvent->ulStatus) ||
        !SubXML->GetAttr("Callee", &pstEvent->ulCallee) ||
        !SubXML->GetAttr("Caller", &pstEvent->ulCaller) ||
        !SubXML->GetAttr("Camera", &pstEvent->iCameraType) ||
        !SubXML->GetAttr("VideoFormatType", &pstEvent->ulVideoFormat) ||
        !SubXML->GetAttr("isP2PVideoDial", &pstEvent->bIsP2PVideoDial) )
    {
        return FALSE;
    }

    return TRUE;
}

BOOL ParseEventModuleStatus(CHAR *szEvent, EVENT_MODULE_STATUS_T *pstEvent)
{
    XML     xml(szEvent, TRUE);
    XML     *SubXML;

    SubXML = xml.GetNode("Content");

    memset(pstEvent, 0, sizeof(EVENT_MODULE_STATUS_T));

    if (!SubXML->GetAttr("ModuleType", &pstEvent->ulModuleType) ||
        !SubXML->GetAttr("ModuleStatus", &pstEvent->ulModuleStatus) )
    {
        return FALSE;
    }

    return TRUE;
}

/*
<Content><ResourceID>5071</ResourceID>
<Time>1586420606530</Time>
<Altitude>-54.0</Altitude>
<Latitude>31.025428771972656</Latitude>
<Longtitude>121.44967901706696</Longtitude>
<Direction>24.890625</Direction>
<Speed>0.0</Speed><TriggerCode>0</TriggerCode>
<ReportStatus>0</ReportStatus><GpsStar>-1</GpsStar><PLMNID>
</PLMNID><DeviceType>ALKAID</DeviceType>
<LocatMode>null</LocatMode></Content>"
*/
BOOL ParseEventGISReport(CHAR *szEvent, EVENT_GIS_REPORT_T *pstEvent)
{
    XML     xml(szEvent, TRUE);
    XML     *SubXML;
    CHAR    acValue[64];

    SubXML = xml.GetNode("Content");

    memset(pstEvent, 0, sizeof(EVENT_GIS_REPORT_T));

    if (!SubXML->GetAttr("ResourceID", &pstEvent->ulUserID) ||
//      !SubXML->GetAttr("Time", &pstEvent->ulTime) ||
//      !SubXML->GetAttr("Altitude", &pstEvent->dAltitude) ||
        !SubXML->GetAttr("Latitude", &pstEvent->dLatitude) ||
        !SubXML->GetAttr("Longtitude", &pstEvent->dLongtitude)
//      !SubXML->GetAttr("TriggerCode", &pstEvent->ulReportType) ||
 //     !SubXML->GetAttr("CELLID", &pstEvent->ulCellID) ||
 //     !SubXML->GetAttr("PLMNID", &pstEvent->ulPLMN) ||
//      !SubXML->GetAttr("DeviceType", &pstEvent->ulDeviceType) ||
//      !SubXML->GetAttr("LocatMode", &pstEvent->ulLocatMode) ||
//      !SubXML->GetAttr("ReportStatus", &pstEvent->ulReportStatus) ||
//      !SubXML->GetAttr("Speed", &pstEvent->dSpeed)
        )
    {
        return FALSE;
    }

    if (SubXML->GetAttr("CELLID", acValue, sizeof(acValue)) )
    {
        gos_atou(acValue, &pstEvent->ulCellID);
    }

    if (SubXML->GetAttr("PLMNID", acValue, sizeof(acValue)) )
    {
        gos_atou(acValue, &pstEvent->ulPLMN);
    }

    return TRUE;
}

/*
<Content><PatchGroupInfoList>
<PatchGroupInfo><GroupNumber>9999999999923</GroupNumber><SetupDcId>55556</SetupDcId><PGPriority>15</PGPriority><DcPatchIndex>1</DcPatchIndex><PGName>PCH-201912111125157351</PGName><VPNID>65535</VPNID></PatchGroupInfo>
<PatchGroupInfo><GroupNumber>9999999999925</GroupNumber><SetupDcId>1001</SetupDcId><PGPriority>15</PGPriority><DcPatchIndex>1</DcPatchIndex><PGName>派接组21</PGName><VPNID>65535</VPNID></PatchGroupInfo>
</PatchGroupInfoList></Content>
*/
BOOL eSDK_GetPatchGroups(VectorMap<INT64, SDK_PATCH_GROUP_INFO_T> &vPatchGroup)
{
    MDC_CHECK;

    CHAR    *szInfo = NULL;
    INT32   iRet = ELTE_SDK_GetPatchGroups(g_acCurrDCUserID, &szInfo);
    XML     xml(szInfo, TRUE);
    XML     *SubXML;
    std::vector<XML*> vList;

    eSDK_Free(szInfo);
    ESDK_LOG(iRet);
    if (iRet != 0)
    {
        return FALSE;
    }

    xml.GetNode("Content")->GetNode("PatchGroupInfoList")->GetNode("PatchGroupInfo", vList);

    vPatchGroup.Clear();
    for (UINT32 i=0; i<vList.size(); i++)
    {
        SDK_PATCH_GROUP_INFO_T  stInfo = {0};

        SubXML = vList[i];

        if (!SubXML->GetAttr("GroupNumber", &stInfo.i64GroupID) ||
            !SubXML->GetAttr("SetupDcId", &stInfo.ulDCUserID) ||
            !SubXML->GetAttr("PGPriority", &stInfo.ulPriority) ||
            !SubXML->GetAttr("DcPatchIndex", &stInfo.ulPatchIndex) ||
            !SubXML->GetAttr("PGName", stInfo.acGroupName, sizeof(stInfo.acGroupName)))
        {
            return FALSE;
        }

        FormatPatchGroupName(stInfo.acGroupName, stInfo.ulPatchIndex);

        vPatchGroup.Add(stInfo.i64GroupID, stInfo);
    }

    return TRUE;
}

BOOL eSDK_GetDcGroups(std::vector<DC_GROUP_INFO_T> &vDcGroup)
{
    MDC_CHECK;

    CHAR    *szInfo = NULL;
    INT32   iRet = ELTE_SDK_GetDcGroups(g_acCurrDCUserID, &szInfo);
    XML     xml(szInfo, TRUE);
    XML     *SubXML;
    std::vector<XML*> vList;

    eSDK_Free(szInfo);
    ESDK_LOG(iRet);
    if (iRet != 0)
    {
        return FALSE;
    }

    xml.GetNode("Content")->GetNode("GroupInfoList")->GetNode("GroupInfo", vList);

    vDcGroup.clear();
    for (UINT32 i=0; i<vList.size(); i++)
    {
        DC_GROUP_INFO_T stInfo = {0};

        SubXML = vList[i];

        if (!SubXML->GetAttr("GroupID", &stInfo.i64GroupID) ||
            !SubXML->GetAttr("GroupCategory", &stInfo.ulGroupType) ||
            !SubXML->GetAttr("GroupPriority", &stInfo.ulPriority) ||
            !SubXML->GetAttr("GroupName", stInfo.acGroupName, sizeof(stInfo.acGroupName)))
        {
            return FALSE;
        }

        stInfo.ulGroupStatus = GROUP_STATUS_UNKNOWN;
        vDcGroup.push_back(stInfo);
    }

    return TRUE;
}

BOOL eSDK_GetDcGroups(UINT32 ulGroupType, std::vector<DC_GROUP_INFO_T> &vDcGroup)
{
    MDC_CHECK;

    CHAR    *szInfo = NULL;
    INT32   iRet = ELTE_SDK_GetDcGroupsByType(g_acCurrDCUserID, IntToStr(ulGroupType), &szInfo);
    XML     xml(szInfo, TRUE);
    XML     *SubXML;
    std::vector<XML*> vList;

    eSDK_Free(szInfo);
    ESDK_LOG(iRet);
    if (iRet != 0)
    {
        return FALSE;
    }

    xml.GetNode("Content")->GetNode("GroupInfoList")->GetNode("GroupInfo", vList);

    vDcGroup.clear();
    for (UINT32 i=0; i<vList.size(); i++)
    {
        DC_GROUP_INFO_T stInfo = {0};

        SubXML = vList[i];

        if (!SubXML->GetAttr("GroupID", &stInfo.i64GroupID) ||
            !SubXML->GetAttr("GroupCategory", &stInfo.ulGroupType) ||
            !SubXML->GetAttr("GroupPriority", &stInfo.ulPriority) ||
            !SubXML->GetAttr("GroupName", stInfo.acGroupName, sizeof(stInfo.acGroupName)))
        {
            return FALSE;
        }

        vDcGroup.push_back(stInfo);
    }

    return TRUE;
}

UINT32 eSDK_GetGroupCreator(INT64 i64GroupID)
{
    MDC_CHECK;

    CHAR    *szInfo = NULL;
    INT32   iRet = ELTE_SDK_GetGroupInfo(IntToStr(i64GroupID), &szInfo);
    XML     xml(szInfo, TRUE);
    XML     *SubXML;
    UINT32  ulCreator = INVALID_USER_ID;

    eSDK_Free(szInfo);
    ESDK_LOG(iRet);
    if (iRet != 0)
    {
        return INVALID_USER_ID;
    }

    SubXML = xml.GetNode("Content")->GetNode("GroupInfo");

    if (!SubXML->GetAttr("GroupCreator", &ulCreator) )
    {
        return INVALID_USER_ID;
    }

    return ulCreator;
}

BOOL eSDK_GetDcUsers(std::vector<DC_INFO_T> &vDcUser)
{
    MDC_CHECK;

    CHAR    *szInfo = NULL;
    INT32   iRet = ELTE_SDK_GetDcUsers(g_acCurrDCUserID, &szInfo);
    XML     xml(szInfo, TRUE);
    XML     *SubXML;
    std::vector<XML*> vList;

    eSDK_Free(szInfo);
    ESDK_LOG(iRet);
    if (iRet != 0)
    {
        return FALSE;
    }

    xml.GetNode("Content")->GetNode("UserInfoList")->GetNode("UserInfo", vList);

    vDcUser.clear();
    for (UINT32 i=0; i<vList.size(); i++)
    {
        DC_INFO_T   stInfo = {0};

        SubXML = vList[i];

        if (!SubXML->GetAttr("UserID", &stInfo.ulUserID) ||
            !SubXML->GetAttr("UserCategory", &stInfo.ulUserCategory) ||
            !SubXML->GetAttr("UserPriority", &stInfo.ulUserPriority) ||
            !SubXML->GetAttr("UserName", stInfo.acUserName, sizeof(stInfo.acUserName)))
        {
            return FALSE;
        }

        vDcUser.push_back(stInfo);
    }

    return TRUE;
}

BOOL eSDK_GetGroupMember(INT64 i64GroupID, VECTOR<UINT32> &vUser, VECTOR<INT64> &vGroup)
{
    MDC_CHECK;

    CHAR    *szInfo = NULL;
    INT32   iRet = ELTE_SDK_GetGroupUsers(IntToStr(i64GroupID), &szInfo);
    XML     xml(szInfo, TRUE);
    UINT32  ulMemberTypeGroup = 0;
    UINT32  ulMemberTypeUser = 1;
    UINT32  ulMemberType;
    UINT32  ulUserID;
    INT64   i64MemberGroupID;
    XML     *SubXML;
    std::vector<XML*>   vList;

    eSDK_Free(szInfo);
    ESDK_LOG(iRet);
    if (iRet != 0)
    {
        ESDKLog(LOG_ERROR, "eSDK_GetGroupMember of "FORMAT_I64" failed", i64GroupID);
        return FALSE;
    }

    xml.GetNode("Content")->GetNode("GroupUserInfoList")->GetNode("GroupUserInfo", vList);

    vUser.clear();
    for (UINT32 i=0; i<vList.size(); i++)
    {
        SubXML = vList[i];

        if (!SubXML->GetAttr("UserID", &ulUserID) ||
            !SubXML->GetAttr("GroupID", &i64MemberGroupID) ||
            !SubXML->GetAttr("MemberType", &ulMemberType) )
        {
            return FALSE;
        }

        if (ulMemberType == ulMemberTypeUser)
        {
            vUser.push_back(ulUserID);
        }
        else if (ulMemberType == ulMemberTypeGroup)
        {
            vGroup.push_back(i64MemberGroupID);
        }
    }

    return TRUE;
}

BOOL eSDK_GetCurrDcUserInfo(CURR_DC_INFO_T &stInfo)
{
    MDC_CHECK;

    CHAR    *szInfo = NULL;
    INT32   iRet = ELTE_SDK_GetDcInfo(g_acCurrDCUserID, &szInfo);
    XML     xml(szInfo, TRUE);
    XML     *SubXML;

    eSDK_Free(szInfo);
    ESDK_LOG(iRet);
    if (iRet != 0)
    {
        return FALSE;
    }

    SubXML = xml.GetNode("Content")->GetNode("DcInfo");

    memset(&stInfo, 0, sizeof(stInfo));

    if (!SubXML->GetAttr("Priviledge", &stInfo.ulPriviledge) ||
        !SubXML->GetAttr("Role", &stInfo.ulRole) )
    {
        return FALSE;
    }

    return TRUE;
}

BOOL eSDK_CheckMDC()
{
    CURR_DC_INFO_T  stDcInfo;

    return eSDK_GetCurrDcUserInfo(stDcInfo);
}

BOOL eSDK_GetPatchGroupMembers(INT64 i64PatchGroupID, VECTOR<INT64> &vMember)
{
    MDC_CHECK;

    CHAR    *szInfo = NULL;
    INT32   iRet = ELTE_SDK_GetGroupMemberByPatchId(IntToStr(i64PatchGroupID), &szInfo);
    XML     xml(szInfo, TRUE);
    INT64   i64MemberGroupID;
    XML     *SubXML;
    std::vector<XML*>   vList;

    eSDK_Free(szInfo);
    ESDK_LOG(iRet);
    if (iRet != 0)
    {
        ESDKLog(LOG_ERROR, "Get Patch Group("FORMAT_I64") member failed! ", i64PatchGroupID);
        return FALSE;
    }

    xml.GetNode("Content")->GetNode("PatchGroupMemberList")->GetNode("PatchGroupMember", vList);

    vMember.clear();
    for (UINT32 i=0; i<vList.size(); i++)
    {
        SubXML = vList[i];

        if (!SubXML->GetAttr("MemberGroupId", &i64MemberGroupID) )
        {
            ESDKLog(LOG_ERROR, "Get xml MemberGroupId failed!");
            return FALSE;
        }

        vMember.push_back(i64MemberGroupID);
    }

    return TRUE;
}

//=====================组管理=============================================
/**
 * @brief 该接口作用为使当前登录的调度台用户订阅某个群组，仅针对群组操作相关业务。
 *
 * @param i64GroupID    需要订阅的目标群组ID。
 * @return BOOL
 *
 * @note
 * 使用说明:
 * 1. 调用此接口前应确保成功调用ELTE_SDK_Init(初始化SDK)接口，且登录鉴权成功。
 * 2. 调度台订阅某个群组，调度台在对某个群组进行操作或接收组呼信息前，必须订阅该群组，且一次只能订阅一个群组。
 * 3. 该接口可以与ELTE_SDK_JoinGroup(迟后加入某群组群组会话)接口配对使用。
 * 4. 该接口与ELTE_SDK_UnSubscribeGroup(取消订阅群组)接口配对使用。
 * 5. 回调消息参考ELTE_Event_NotifyResourceStatus(资源状态变化事件通知)。
 */
BOOL eSDK_SubGroup(INT64 i64GroupID)
{
    MDC_CHECK;

    INT32   iRet = ELTE_SDK_SubscribeGroup(IntToStr(i64GroupID));

    ESDK_RET(iRet);
}

/**
 * @brief 该接口用于当前已订阅并自动加入群组的调度台用户退出订阅某个群组。
 *
 * @param i64GroupID 需要取消订阅的目标群组ID。
 * @return BOOL
 *
 * @note
 * 使用说明:
 * 1. 调用此接口前应确保成功调用ELTE_SDK_Init(初始化SDK)接口，且登录鉴权成功。
 * 2. 调度台退订某个群组后，将无法接收该群组组呼或者发起该群组组呼，
 *    同时也不能对该群组进行缜密监听，除非调度台再次订阅该群组。
 * 3. 该接口与ELTE_SDK_SubJoinGroup(订阅并加入群组)接口配对使用。
 * 4. 回调消息参考ELTE_Event_NotifyResourceStatus(资源状态变化事件通知)
 */
BOOL eSDK_UnSubGroup(INT64 i64GroupID)
{
    MDC_CHECK;

    INT32   iRet = ELTE_SDK_UnSubscribeGroup(IntToStr(i64GroupID));

    ESDK_RET(iRet);
}

/**
 * @brief 该接口作用为使当前登录的调度台用户订阅并自动加入某个群组的业务，仅针对群组操作相关业务。
 *
 * @param i64GroupID 需要订阅并加入的目标群组ID。
 * @return BOOL
 *
 * @note
 * 使用说明:
 * 1. 调用此接口前应确保成功调用ELTE_SDK_Init(初始化SDK)接口，且登录鉴权成功。
 * 2. 调度台订阅并加入某个群组的业务，调度台在对某个群组进行操作或接收组呼信息前，
 *    必须订阅该群组，且一次只能订阅一个群组。
 * 3. 订阅并迟后加入群组，并不是成为群组成员，而是以非群组成员的身份参与群组业务。
 * 4. 该接口与ELTE_SDK_UnSubscribeGroup(取消订阅群组)接口配对使用。
 * 5. 回调消息参考ELTE_Event_NotifyResourceStatus(资源状态变化事件通知)。
 */
BOOL eSDK_SubJoinGroup(INT64 i64GroupID)
{
    MDC_CHECK;

    INT32   iRet = ELTE_SDK_SubJoinGroup(IntToStr(i64GroupID));

    if (iRet != 0)
    {
        ESDKLog(LOG_ERROR, "sub join group "FORMAT_I64" failed", i64GroupID);
    }

    ESDK_RET(iRet);
}

/**
 * @brief 该接口用于迟后加入某群组群组会话。
 *
 * @param i64GroupID    迟后加入的目标群组ID。
 * @return BOOL
 *
 * @note
 * 使用说明：
 * 1. 调用此接口前应确保成功调用ELTE_SDK_Init(初始化SDK)接口，且登录鉴权成功。
 * 2. 该接口与ELTE_SDK_SubscribeGroup(订阅群组)接口配对使用。
 */
BOOL eSDK_JoinGroup(INT64 i64GroupID)
{
    MDC_CHECK;

    INT32   iRet = ELTE_SDK_JoinGroup(IntToStr(i64GroupID));

    ESDK_RET(iRet);
}

/**
 * @brief 发起组呼
 *
 * @param i64GroupID    群组ID
 * @param bRetry
 * @return BOOL
 */
BOOL eSDK_PTTDial(INT64 i64GroupID, BOOL bRetry)
{
    if (i64GroupID == INVALID_GROUP_ID)
    {
        return FALSE;
    }

    MDC_CHECK;

    /// 发起组呼，或者抢权
    INT32   iRet = ELTE_SDK_PTTDial(IntToStr(i64GroupID));

    ESDK_LOG(iRet);

    if (iRet == 0)
    {
        /// 发起组呼成功，则返回真
        return TRUE;
    }

    /// 发起组呼失败后
    /// 订阅并迟后加入
    iRet = ELTE_SDK_SubJoinGroup(IntToStr(i64GroupID));
    ESDK_LOG(iRet);

    if (iRet == 0)
    {
        /// 发起组呼，或者抢权
        iRet = ELTE_SDK_PTTDial(IntToStr(i64GroupID));
    }

    ESDK_RET(iRet);
}

/**
 * @brief 组呼释放话权，如果没有订阅该组直接订阅该组
 *
 * @param i64GroupID 组的ID
 * @return BOOL
 */
BOOL eSDK_PTTRelease(INT64 i64GroupID)
{
    if (i64GroupID == INVALID_GROUP_ID)
    {
        return TRUE;
    }

    MDC_CHECK;
    /// 组呼模式中发起组呼者或抢权成功者释放话语权
    INT32   iRet = ELTE_SDK_PTTRelease(IntToStr(i64GroupID));

    if (iRet == ESDK_ERR_NO_SUBSCRIPTION)
    {
        /// 如果没有订阅这个群组则订阅这个群组
        if (eSDK_SubGroup(i64GroupID))
        {
            gos_sleep_1ms(300);
            iRet = ELTE_SDK_PTTRelease(IntToStr(i64GroupID));
        }
    }

    ESDK_RET(iRet);
}

BOOL eSDK_PTTHangup(INT64 i64GroupID)
{
    if (i64GroupID == INVALID_GROUP_ID)
    {
        return TRUE;
    }

    MDC_CHECK;

    INT32   iRet = ELTE_SDK_PTTHangup(IntToStr(i64GroupID));

    ESDK_RET(iRet);
}

/**
 * @brief 该接口用于调度台用户对已经存在的群组发起紧急呼叫。
 *        发起紧急组呼成功后，终端用户的界面一般显示为紧急组呼界面。
 *
 * @param i64GroupID 组ID
 * @return BOOL
 *
 * @note
 * 使用说明:
 * 1. 调用此接口前应确保成功调用ELTE_SDK_Init(初始化SDK)接口，且登录鉴权成功。
 * 2. 该用户是操作对象群组的成员。
 * 3. 使用该接口时，确保当前群组需处于被订阅的状态。
 * 4. 该接口不能用于临时群组。
 */
BOOL eSDK_PTTEmergency(INT64 i64GroupID)
{
    if (i64GroupID == INVALID_GROUP_ID)
    {
        return FALSE;
    }

    MDC_CHECK;

    INT32   iRet = ELTE_SDK_PTTEmergency(IntToStr(i64GroupID));

    ESDK_RET(iRet);
}

/**
 * @brief 该接口用于调度台用户强拆某个群组的组呼，该群组的当前活动呼叫会被强制结束。
 *
 * @param i64GroupID
 * @return BOOL
 *
 * @note
 * 使用说明:
 * 1. 调用此接口前应确保成功调用ELTE_SDK_Init(初始化SDK)接口，且登录鉴权成功。
 * 2. 使用该接口时，无需订阅当前群组。
 * 3. 强拆组呼的前提是已经存在一路成功发起的组呼需要被强拆。
 * 4. 该接口调用成功后，需要处理ELTE_Event_NotifyGroupStatus(组呼状态变化事件通知)返回的事件消息
 */
BOOL eSDK_GroupBreakoff(INT64 i64GroupID)
{
    if (i64GroupID == INVALID_GROUP_ID)
    {
        return TRUE;
    }

    MDC_CHECK;

    INT32   iRet = ELTE_SDK_GroupBreakoff(IntToStr(i64GroupID));

    ESDK_RET(iRet);
}

BOOL eSDK_AssignGroupSpeaker(INT64 i64GroupID, CHAR *szSpeaker)
{
    RawByteString   strSpeaker = AnsiToUtf8(szSpeaker);
    /// 设置群组的扬声器
    INT32   iRet = ELTE_SDK_AssignSpeaker4Group(IntToStr(i64GroupID), strSpeaker.c_str());

    if (iRet == 0)
    {
        ESDKLog(LOG_INFO, "assign speaker to group "FORMAT_I64 " succ: %s", i64GroupID, szSpeaker);
    }
    else
    {
        ESDKLog(LOG_ERROR, "assign speaker to group "FORMAT_I64 " failed: %s", i64GroupID, szSpeaker);
    }

    ESDK_RET(iRet);
}

BOOL eSDK_AssignGroupMic(INT64 i64GroupID, CHAR *szMic)
{
    RawByteString   strMic = AnsiToUtf8(szMic);
    INT32   iRet = ELTE_SDK_AssignMic4Group(IntToStr(i64GroupID), strMic.c_str());

    if (iRet == 0)
    {
        ESDKLog(LOG_INFO, "assign MIC to group "FORMAT_I64 " succ: %s", i64GroupID, szMic);
    }
    else
    {
        ESDKLog(LOG_ERROR, "assign MIC to group "FORMAT_I64 " failed: %s", i64GroupID, szMic);
    }

    ESDK_RET(iRet);
}

BOOL eSDK_AssignUserSpeaker(UINT32 ulUserID, CHAR *szSpeaker)
{
    RawByteString   strSpeaker = AnsiToUtf8(szSpeaker);
    INT32   iRet = ELTE_SDK_AssignSpeaker4Group(IntToStr(ulUserID), strSpeaker.c_str());

    if (iRet == 0)
    {
        ESDKLog(LOG_INFO, "assign speaker to user %u succ: %s", ulUserID, szSpeaker);
    }
    else
    {
        ESDKLog(LOG_ERROR, "assign speaker to user %u failed: %s", ulUserID, szSpeaker);
    }

    ESDK_RET(iRet);
}

BOOL eSDK_AssignUserMic(UINT32 ulUserID, CHAR *szMic)
{
    RawByteString   strMic = AnsiToUtf8(szMic);
    INT32   iRet = ELTE_SDK_AssignMic4Group(IntToStr(ulUserID), strMic.c_str());

    if (iRet == 0)
    {
        ESDKLog(LOG_INFO, "assign MIC to to user %u succ: %s", ulUserID, szMic);
    }
    else
    {
        ESDKLog(LOG_ERROR, "assign MIC to to user %u succ: %s", ulUserID, szMic);
    }

    ESDK_RET(iRet);
}

/**
 * @brief 该接口用于调度台向其他用户发起点对点语音呼叫，呼叫的对象可以为手持终端或者
 *        是在线的调度台用户，若呼叫不存在的用户号码或者不在线的用户则点呼不能发起成功。
 *
 * @param ulUserID  点呼的目标资源ID，可以为手持终端ID或调度台用户ID。
 * @return BOOL
 */
BOOL eSDK_P2PDial(UINT32 ulUserID)
{
    MDC_CHECK;

    INT32   iRet = ELTE_SDK_P2PDial(IntToStr(ulUserID));

    ESDK_RET(iRet);
}

/**
 * @brief 该接口用于调度台挂断与其他用户之间的点对点语音呼叫，
 *        挂断语音呼叫后，该路语音点呼挂断，点呼双方的状态变为空闲。
 *
 * @param ulNumber 发起语音点呼的资源ID，可以为手持终端ID、调度台用户ID，
 *                 例如，用户挂断一路来自手持终端A的语音点呼通话，此时该参数填写终端A的资源ID。
 * @return BOOL
 */
BOOL eSDK_P2PHangup(UINT32 ulNumber)
{
    MDC_CHECK;

    INT32   iRet = ELTE_SDK_P2PHangup(IntToStr(ulNumber));

    ESDK_RET(iRet);
}

/**
 * @brief 该接口用于用户接收点对点语音呼叫，并停止被呼叫的调度台的呼叫提醒振铃。
 *        接收后语音点呼建立，双方的状态变为通话中。
 *
 * @param ulNumber 发起语音点呼的资源ID，可以为手持终端ID或调度台用户ID，
 *                 例如，用户接收一路来自手持终端A的语音点呼，
 *                 此时该参数填写终端A的资源ID。
 * @return BOOL
 */
BOOL eSDK_P2PRecv(UINT32 ulNumber)
{
    MDC_CHECK;

    INT32   iRet = ELTE_SDK_P2PRecv(IntToStr(ulNumber));

    ESDK_RET(iRet);
}

/**
 * @brief 该接口用于用户拒接某个用户发来的语言点呼，并停止被呼叫的调度台的呼叫提醒振铃。
 *
 * @param ulNumber 发起语音点呼的资源ID，可以为手持终端ID或调度台用户ID，
 *                 例如，用户拒接一路来自手持终端A的语音点呼，此时该参数填写终端A的资源ID。
 * @return BOOL
 */
BOOL eSDK_P2PReject(UINT32 ulNumber)
{
    MDC_CHECK;

    INT32   iRet = ELTE_SDK_P2PReject(IntToStr(ulNumber));

    ESDK_RET(iRet);
}

/**
 * @brief 该接口用于调度台对某个正在进行的点呼通话强行介入，
 *        目的是使调度台与原通话其中一方用户进行语音点呼，
 *        抢话成功后被抢话的用户原来的通话被拆除并与发起抢话的用户建立新的通话。
 *
 * @param ulNumber 呼叫中被拆除通话方的资源ID，可以为手持终端ID或调度台用户ID，
 *                 例如，两用户正在点呼通话中，其中手持终端A通话被调度台用户抢话，
 *                 此时该参数填写终端A的资源ID。
 * @return BOOL
 *
 * @note 1. 调用此接口前应确保成功调用ELTE_SDK_Init(初始化SDK)接口，且登录鉴权成功。
 *       2. 发起抢话的用户优先级要高于通话双方的优先级。
 *       3. 该接口调用成功后，需要处理ELTE_Event_NotifyP2pcallStatus(点呼状态变化事件通知)返回的事件消息。
 */
BOOL eSDK_P2PBreakin(UINT32 ulNumber)
{
    MDC_CHECK;

    INT32   iRet = ELTE_SDK_P2PBreakin(IntToStr(ulNumber));

    ESDK_RET(iRet);
}

/**
 * @brief 该接口用于调度台作为第三方强行拆除某个用户当前正在进行的通话。
 *        强拆点呼成功后，原有的通话被取消，原通话的双方状态变为空闲。
 *
 * @param ulNumber
 * @return BOOL
 */
BOOL eSDK_P2PBreakoff(UINT32 ulNumber)
{
    MDC_CHECK;

    INT32   iRet = ELTE_SDK_P2PBreakoff(IntToStr(ulNumber));

    ESDK_RET(iRet);
}

/**
 * @brief 该接口用于调度台用户发起人工转接
 *
 * @param ulCallingNumber  资源ID，即发起人工转接的调度员的资源ID
 * @param ulObjectUserNumber 需要转接的号码ID
 * @return BOOL 转接成功或是失败
 *
 * @note 例如：有三个用户A、B和C，其中A是调度台用户，B和C为手持终端用户，
 *            A向用户B发起点呼，点呼接通后调度台A将与B用户的通话转接给C用户，
 *            转接成功后，B与C直接通话，A自动释放该点呼业务。
 */
BOOL eSDK_P2PTransfer(UINT32 ulCallingNumber, UINT32 ulObjectUserNumber)
{
    MDC_CHECK;

    XmlParam    Param;

    Param.Add("DcID", g_acCurrDCUserID);    ///< 发起人工转接的调度员用户ID
    Param.Add("SpeakerID", ulCallingNumber);    ///< 当前正在通话中的对端的ID
    Param.Add("ObjectID", ulObjectUserNumber);  ///< 需要转接的号码ID

    INT32   iRet = ELTE_SDK_P2PTransfer(g_acCurrDCUserID, Param);

    ESDK_RET(iRet);
}

BOOL eSDK_P2PHalfDuplexDial(UINT32 ulUserID)
{
    MDC_CHECK;

    INT32   iRet = ELTE_SDK_P2PHalfDpxDial(IntToStr(ulUserID));

    ESDK_RET(iRet);
}

// 释放话权
BOOL eSDK_P2PHalfDuplexRelease(UINT32 ulUserID)
{
    MDC_CHECK;

    INT32   iRet = ELTE_SDK_P2PHalfDpxRelease(IntToStr(ulUserID));

    ESDK_RET(iRet);
}

//=================PatchGroup===============================================
std::map<AnsiString, INT64> g_mPatchGroupNameID;

INT64 GetPatchGroupID(CHAR *szGroupName)
{
    if (g_mPatchGroupNameID.find(szGroupName) == g_mPatchGroupNameID.end())
    {
        return INVALID_GROUP_ID;
    }

    return g_mPatchGroupNameID[szGroupName];
}

/**
 * @brief           获取派接组的详细信息
 * @param           i64PatchGroupID     [in]    要查询的派接组ID
 * @param           stPatchGroupInfo    [out]   派接组详细信息
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
BOOL eSDK_GetPatchGroupInfo(INT64 i64PatchGroupID, SDK_PATCH_GROUP_INFO_T &stInfo)
{
    MDC_CHECK;

    CHAR    *szInfo = NULL;
    INT32   iRet = ELTE_SDK_GetPatchGroupInfo(IntToStr(i64PatchGroupID), &szInfo);
    XML     xml(szInfo, TRUE);
    XML     *SubXML;
    std::vector<XML*>   vList;

    eSDK_Free(szInfo);
    ESDK_LOG(iRet);
    if (iRet != 0)
    {
        ESDKLog(LOG_ERROR, "Get Patch Group(%lld) info failed", i64PatchGroupID);
        return FALSE;
    }

    SubXML = xml.GetNode("Content")->GetNode("PatchGroupInfo");

    if (!SubXML ||
        !SubXML->GetAttr("GroupNumber", &stInfo.i64GroupID) ||
        !SubXML->GetAttr("SetupDcId", &stInfo.ulDCUserID) ||
        !SubXML->GetAttr("PGPriority", &stInfo.ulPriority) ||
        !SubXML->GetAttr("DcPatchIndex", &stInfo.ulPatchIndex) ||
        !SubXML->GetAttr("PGName", stInfo.acGroupName, sizeof(stInfo.acGroupName)) )
    {
        ESDKLog(LOG_ERROR, "Parse xml failed!");
        return FALSE;
    }

    FormatPatchGroupName(stInfo.acGroupName, stInfo.ulPatchIndex);

    return TRUE;
}

VOID SetPatchGroupID(CHAR *szGroupName, INT64 i64GroupID)
{
    ESDKLog(LOG_DETAIL, "PatchGroupName(%s) <-> PatchGroupID("FORMAT_I64")", szGroupName, i64GroupID);

    g_mPatchGroupNameID[szGroupName] = i64GroupID;
}

/**
 * @brief 该接口用于调度台用户发起创建派接组。
 *        例如，当群组A中的某一用户L（用户L必须是调度台用户）发起派接到群组B，
 *        用户L发起组呼通话，则群组B也能接收到L的组呼通话，即使L不是群组B的成员。
 *
 * @param szGroupName 派接组名称，组成为数字或字母，长度不超过32字符。
 * @return BOOL
 *
 * @note
 * 使用说明：
 * 1. 调用此接口前应确保成功调用ELTE_SDK_Init(初始化SDK)接口，且登录鉴权成功。
 * 2. 目前派接组成员只支持普通群组。
 * 3. 创建派接组时，不可添加该群组成员 。
 * 4. 回调消息参考ELTE_Event_NotifyResourceStatus(资源状态变化事件通知)。
 */
BOOL eSDK_CreatePatchGroup(CHAR *szGroupName)
{
    MDC_CHECK;

    XmlParam    Param;

    Param.Add("PatchGroupID", "");
    Param.Add("PatchGroupName", szGroupName);
    Param.Add("PatchGroupMemberList", "");

    INT32 iRet = ELTE_SDK_CreatePatchGroup(Param);

    if (iRet == 0)
    {
        ESDKLog(LOG_INFO, "create patch group %s successful", szGroupName);
    }
    else
    {
        ESDKLog(LOG_ERROR, "create patch group %s failed", szGroupName);
    }

    ESDK_RET(iRet);
}

/**
 * @brief 该接口用于取消派接组，若调度员A发起取消派接组，则调度员A只可以取消自己创建的派接组。
 *
 * @param i64PatchGroupID 派接组的组号。通过ELTE_Event_NotifyResourceStatus(资源状态变化事件通知)
 *                        返回消息中读出的组号，即事件消息体中的ResourceName。
 * @return BOOL
 *
 * @note
 * 1. 调用此接口前应确保成功调用ELTE_SDK_Init(初始化SDK)接口，且登录鉴权成功。
 * 2. 取消派接组只有该派接组的创建者才可以取消。
 * 3. 回调消息参考ELTE_Event_NotifyResourceStatus(资源状态变化事件通知)。
 */
BOOL eSDK_CancelPatchGroup(INT64 i64PatchGroupID)
{
    MDC_CHECK;

    INT32 iRet = ELTE_SDK_CancelPatchGroup(IntToStr(i64PatchGroupID));

    ESDK_LOG(iRet);
    if (iRet != 0)
    {
        return FALSE;
    }

    std::map<AnsiString, INT64>::iterator   it = g_mPatchGroupNameID.begin();

    for (; it!=g_mPatchGroupNameID.end(); it++)
    {
        if (it->second == i64PatchGroupID)
        {
            g_mPatchGroupNameID.erase(it);
            break;
        }
    }

    return TRUE;
}

BOOL eSDK_ClearPatchGroupMember(INT64 i64GroupID, CHAR *szGroupName)
{
    VECTOR<INT64>   vGroup;

    if (!eSDK_GetPatchGroupMembers(i64GroupID, vGroup))
    {
        return FALSE;
    }

    return eSDK_DelPatchGroupMember(i64GroupID, szGroupName, vGroup);
}

/**
 * @brief 该接口用于增加派接组成员，增加的派接组成员属于普通群组。
 *
 * @param i64GroupID    派接组的组号
 * @param szGroupName   派接组名称，组成为数字或字母，长度不超过32字符。
 * @param vGroup        派接组成员列表，最多支持20个普通组被派接
 * @return BOOL
 *
 * @note
 * 首次创建的派接组组号默认设置为0，以后系统为此派接组自动分配一个派接组号。
 * 该派接组相关接口均可以通过ELTE_Event_NotifyResourceStatus(资源状态变化事件通知)返回的消息中读出的组号进行派接操作。
 *
 * 使用说明：
 * 1. 调用此接口前应确保成功调用ELTE_SDK_Init(初始化SDK)接口，且登录鉴权成功。
 * 2. 仅有该派接组的创建者才拥有权限执行该操作。
 * 3. 派接组添加的一个成员不能是一个多选组的成员
 */
BOOL eSDK_AddPatchGroupMember(INT64 i64GroupID, CHAR *szGroupName, VECTOR<INT64> &vGroup)
{
    MDC_CHECK;

    if (vGroup.size() == 0)
    {
        ESDKLog(LOG_WARN, "no member to be added to patch group "FORMAT_I64, i64GroupID);
        return TRUE;
    }

    if (vGroup.size() > ESDK_MAX_PATCH_GROUP_MEMBER_NUM)
    {
        ESDKLog(LOG_ERROR, "too many patch group member of " FORMAT_I64, i64GroupID);
        return FALSE;
    }

    XmlParam    Param;
    XmlParam    SubParam;

    for (UINT32 i=0; i<vGroup.size(); i++)
    {
        SubParam.Add("PatchGroupMember", vGroup[i]);    ///< 派接组成员，为普通群组
    }

    Param.Add("DcID", g_acCurrDCUserID);    ///< 创建派接组的调度员ID，必须是当前登录的ID。
    Param.Add("PatchGroupID", i64GroupID);  ///< 派接组的组号。
    Param.Add("PatchGroupName", szGroupName);   ///< 派接组名称，组成为数字或字母，长度不超过32字符。
    Param.Add("PatchGroupMemberList", SubParam);    ///< 派接组成员列表，最多支持20个普通组被派接。

    INT32 iRet = ELTE_SDK_AddPatchGroupMember(Param);

    if (iRet == 0)
    {
        ESDKLog(LOG_INFO, "add patch group "FORMAT_I64" member successful", i64GroupID);
    }
    else
    {
        ESDKLog(LOG_ERROR, "add patch group "FORMAT_I64" member failed, member is %s", i64GroupID, SubParam.ToStr());
    }

    ESDK_RET(iRet);
}

/**
 * @brief 该接口用于调度员删除自己创建的派接组内成员。
 *
 * @param i64GroupID 派接组的组号
 * @param szGroupName 派接组名称，组成为数字或字母，长度不超过32字符
 * @param vGroup
 * @return BOOL
 *
 * @note
 * 使用说明：
 * 1. 调用此接口前应确保成功调用ELTE_SDK_Init(初始化SDK)接口，且登录鉴权成功。
 * 2. 删除派接组成员只有该派接组的创建者才可以删除。
 */
BOOL eSDK_DelPatchGroupMember(INT64 i64GroupID, CHAR *szGroupName, VECTOR<INT64> &vGroup)
{
    MDC_CHECK;

    if (vGroup.size() == 0)
    {
        return TRUE;
    }

    XmlParam    Param;
    XmlParam    SubParam;

    for (UINT32 i=0; i<vGroup.size(); i++)
    {
        SubParam.Add("PatchGroupMember", vGroup[i]);
    }

    Param.Add("DcID", g_acCurrDCUserID);    ///< 创建派接组的调度员ID，必须是当前登录的ID。
    Param.Add("PatchGroupID", i64GroupID);  ///< 派接组的组号。
    Param.Add("PatchGroupName", szGroupName);   ///< 派接组名称，组成为数字或字母，长度不超过32字符。
    Param.Add("PatchGroupMemberList", SubParam);    ///< 派接组成员列表，最多支持20个普通组被派接。

    INT32 iRet = ELTE_SDK_DeletePatchGroupMember(Param);

 /*   if (iRet == 0)
    {
        AnsiString  strGroupName = szGroupName;

        g_mPatchGroupNameID.erase(strGroupName);
    }  */

    if (iRet == 0)
    {
        ESDKLog(LOG_INFO, "delete patch group member of "FORMAT_I64" successful", i64GroupID);
    }
    else
    {
        ESDKLog(LOG_ERROR, "delete patch group member of "FORMAT_I64" failed", i64GroupID);
    }

    ESDK_RET(iRet);
}
//========================DynamicGroup========================================
BOOL eSDK_GetDynamicGroupMember(INT64 i64GroupID, VECTOR<UINT32> &vUser, VECTOR<INT64> &vGroup)
{
    MDC_CHECK;

    CHAR    *szInfo = NULL;
    INT32   iRet = ELTE_SDK_GetDynamicGroupMembers(IntToStr(i64GroupID), &szInfo);
    XML     xml(szInfo, TRUE);
    UINT32  ulMemberTypeGroup = 0;
    UINT32  ulMemberTypeUser = 1;
    UINT32  ulMemberType;
    UINT32  ulUserID;
    INT64   i64MemberGroupID;
    XML     *SubXML;
    std::vector<XML*>   vList;

    eSDK_Free(szInfo);
    ESDK_LOG(iRet);
    if (iRet != 0)
    {
        return FALSE;
    }

    xml.GetNode("Content")->GetNode("GroupUserInfoList")->GetNode("GroupUserInfo", vList);

    vUser.clear();
    for (UINT32 i=0; i<vList.size(); i++)
    {
        SubXML = vList[i];

        if (!SubXML->GetAttr("UserID", &ulUserID) ||
            !SubXML->GetAttr("GroupID", &i64MemberGroupID) ||
            !SubXML->GetAttr("MemberType", &ulMemberType) )
        {
            return FALSE;
        }

        if (ulMemberType == ulMemberTypeUser)
        {
            vUser.push_back(ulUserID);
        }
        else if (ulMemberType == ulMemberTypeGroup)
        {
            vGroup.push_back(i64MemberGroupID);
        }
    }

    return TRUE;
}

BOOL eSDK_GetDynamicGroupMember(INT64 i64GroupID, VECTOR<UINT32> &vUser)
{
    VECTOR<INT64>   vGroup;

    return eSDK_GetDynamicGroupMember(i64GroupID, vUser, vGroup);
}

/**
 * @brief 该接口用于创建动态群组，包括群组号，创建该动态群组的调度台用户ID，群组成员
 *        列表，用户成员列表等信息；创建一个动态群组会有一定的时延。
 *
 * @param i64GroupID 动态群组ID，取值范围60000~63999。该参数可以不填，由系统自动生成。
 * @param vGroupID 静态群组成员列表，最大值为8个。
 * @param vUserID 静态群组成员列表，最大值为8个。
 * @param ulPriority 动态组优先级，取值范围是1～15，数字越小优先级越高。
 * @param ulMaxPeriod 动态组的最大通话时长，取值范围是1～66535秒。
 * @param szGroupName 动态群组别名（数字或字母），字符串长度小于等于255个字节（需服务器支持）。
 * @return BOOL
 *
 * @note
 * 接口使用步骤如下：
 * 1. 调用此接口前应确保成功调用ELTE_SDK_Init(初始化SDK)接口，且登录鉴权成功。
 * 2. 填写动态群组信息和动态群组组员信息，以作为该接口的参数。
 * 3. 调用该接口发送动态群组指令给调度机。
 * 4. SDK等待接收调度机返回的结果，如果失败则直接返回失败原因；否则通知“OP_OK_ACCEPTED”，即0值。
 * 5. 回调消息参考ELTE_Event_NotifyResourceStatus(资源状态变化事件通知)。
 */
BOOL _eSDK_CreateDynamicGroup(INT64 i64GroupID, VECTOR<INT64> &vGroupID, VECTOR<UINT32> &vUserID, UINT32 ulPriority, UINT32 ulMaxPeriod, CHAR *szGroupName)
{
    MDC_CHECK;

    XmlParam    Param;
    XmlParam    GroupParam;
    XmlParam    UserParam;

    if (i64GroupID == INVALID_GROUP_ID)
    {
        return FALSE;
    }

    for (UINT32 i=0; i<vGroupID.size(); i++)
    {
        GroupParam.Add("GroupID", vGroupID[i]);
    }

    for (UINT32 i=0; i<vUserID.size(); i++)
    {
        UserParam.Add("UserID", vUserID[i]);
    }

    Param.Add("GroupID", i64GroupID);   ///< 动态群组ID，取值范围60000~63999。该参数可以不填，由系统自动生成。
    Param.Add("DcID", g_acCurrDCUserID);    ///< 创建该动态群组的调度台用户ID。
    Param.Add("Alias", szGroupName);    ///< 动态群组别名（数字或字母），字符串长度小于等于255个字节（需服务器支持）。
    Param.Add("Priority", ulPriority);  ///< 动态组优先级，取值范围是1～15，数字越小优先级越高。
    Param.Add("MaxPeriod", ulMaxPeriod);    ///< 动态组的最大通话时长，取值范围是1～66535秒。
    Param.Add("GroupList", GroupParam); ///< 静态群组成员列表，最大值为8个。
    Param.Add("UserList", UserParam);   ///< 用户成员列表，最大值为200个，分别传入用户ID。

    //创建动态群组
    INT32 iRet = ELTE_SDK_CreateDynamicGroup(Param);

    if (iRet == 0)
    {
        ESDKLog(LOG_INFO, "create dynamic group "FORMAT_I64 " successful", i64GroupID);
    }
    else
    {
        ESDKLog(LOG_ERROR, "create dynamic group "FORMAT_I64 " failed", i64GroupID);
    }

    ESDK_RET(iRet);
}

BOOL eSDK_CreateDynamicGroup(INT64 i64GroupID, VECTOR<INT64> &vGroupID, VECTOR<UINT32> &vUserID, UINT32 ulPriority, UINT32 ulMaxPeriod, CHAR *szGroupName)
{
    if (i64GroupID == INVALID_GROUP_ID)
    {
        ESDKLog(LOG_ERROR, "create dynamic group failed, group id can't be "FORMAT_I64, INVALID_GROUP_ID);
        return FALSE;
    }

    return _eSDK_CreateDynamicGroup(i64GroupID, vGroupID, vUserID, ulPriority, ulMaxPeriod, szGroupName);
}

BOOL eSDK_CreateDynamicGroup(VECTOR<INT64> &vGroupID, VECTOR<UINT32> &vUserID, UINT32 ulPriority, UINT32 ulMaxPeriod, CHAR *szGroupName)
{
    return _eSDK_CreateDynamicGroup(INVALID_GROUP_ID, vGroupID, vUserID, ulPriority, ulMaxPeriod, szGroupName);
}

BOOL eSDK_CreateDynamicGroup(INT64 i64GroupID, VECTOR<UINT32> &vUserID, UINT32 ulPriority, UINT32 ulMaxPeriod, CHAR *szGroupName)
{
    VECTOR<INT64>   vGroupID;

    return eSDK_CreateDynamicGroup(i64GroupID, vGroupID, vUserID, ulPriority, ulMaxPeriod, szGroupName);
}

BOOL eSDK_CreateDynamicGroup(INT64 i64GroupID, UINT32 ulPriority, UINT32 ulMaxPeriod, CHAR *szGroupName)
{
    VECTOR<UINT32>  vUserID;
    VECTOR<INT64>   vGroupID;

    return eSDK_CreateDynamicGroup(i64GroupID, vGroupID, vUserID, ulPriority, ulMaxPeriod, szGroupName);
}

/**
 * @brief 该接口用于动态群组的创建者根据群组ID删除动态群组。
 *
 * @param i64GroupID 需删除的目标动态群组ID。
 * @return BOOL
 *
 * @note
 * 使用说明:
 * 1. 调用此接口前应确保成功调用ELTE_SDK_Init(初始化SDK)接口，且登录鉴权成功。
 * 2. 只有该动态组的创建者才能删除该动态群组。
 * 3. 删除动态群组前，必须已成功调用ELTE_SDK_SubJoinGroup(订阅并加入群组)接口订阅该群组。
 * 4. 回调消息参考ELTE_Event_NotifyResourceStatus(资源状态变化事件通知)。
 */
BOOL eSDK_CancelDynamicGroup(INT64 i64GroupID)
{
    MDC_CHECK;

    INT32 iRet = ELTE_SDK_CancelDynamicGroup(IntToStr(i64GroupID));

    ESDK_RET(iRet);
}

/**
 * @brief 该接口用于修改动态群组，可以对动态组增加用户或者删除用户。
 *
 * @param i64GroupID    动态组信息
 * @param vAddUserID    待添加用户列表
 * @param vDelUserID    待删除用户列表
 * @return BOOL
 *
 * @note
 * 使用说明:
 * 1. 调用此接口前应确保成功调用ELTE_SDK_Init(初始化SDK)接口，且登录鉴权成功。
 * 2. 在调用该接口之前，确保目标动态群组的创建者为当前登录的调度台用户且该动态群组处于被订阅状态。
 * 3. 调用该接口前，可通过调用ELTE_SDK_GetGroupUsers(获取某个群组成员列表)接口，从该列表中获取待操作的动态群组成员信息。
 * 4. 回调消息参考ELTE_Event_NotifyResourceStatus(资源状态变化事件通知)。
 * 5. 如果要删除的用户是成员组的用户，需要删除成员组才能删除该用户。
 */
BOOL eSDK_SetDynamicGroupUser(INT64 i64GroupID, VECTOR<UINT32> &vAddUserID, VECTOR<UINT32> &vDelUserID)
{
    MDC_CHECK;

    XmlParam    TopParam;
    XmlParam    Param;
    XmlParam    AddParam;
    XmlParam    DelParam;

    for (UINT32 i=0; i<vAddUserID.size(); i++)
    {
        AddParam.Add("AddUserID", vAddUserID[i]);
    }

    for (UINT32 i=0; i<vDelUserID.size(); i++)
    {
        DelParam.Add("DeleteUserID", vDelUserID[i]);
    }

    Param.Add("GroupID", i64GroupID);   ///< 动态组ID
    Param.Add("AddUserList", AddParam); ///< 待添加用户列表
    Param.Add("DeleteUserList", DelParam);  ///< 待删除用户列表

    TopParam.Add("DynamicGroupInfo", Param);    ///< 动态组信息

    INT32 iRet = ELTE_SDK_ModifyDynamicGroup(g_acCurrDCUserID, TopParam);

    if (iRet == 0)
    {
        ESDKLog(LOG_INFO, "set dynamic group "FORMAT_I64 " member successful", i64GroupID);
    }
    else
    {
        ESDKLog(LOG_ERROR, "set dynamic group "FORMAT_I64 " member failed,"
                "add member is %s, delete member is %s", i64GroupID, AddParam.ToStr(), DelParam.ToStr());
    }

    ESDK_RET(iRet);
}

BOOL eSDK_AddDynamicGroupUser(INT64 i64GroupID, VECTOR<UINT32> &vUserID)
{
    MDC_CHECK;

    VECTOR<UINT32>  vEmptyUserID;

    return eSDK_SetDynamicGroupUser(i64GroupID, vUserID, vEmptyUserID);
}

BOOL eSDK_DelDynamicGroupUser(INT64 i64GroupID, VECTOR<UINT32> &vUserID)
{
    MDC_CHECK;

    VECTOR<UINT32>  vEmptyUserID;

    return eSDK_SetDynamicGroupUser(i64GroupID, vEmptyUserID, vUserID);
}

BOOL eSDK_DelDynamicGroupUser(INT64 i64GroupID, UINT32 ulUserID)
{
    MDC_CHECK;

    VECTOR<UINT32>  vUserID;

    vUserID.push_back(ulUserID);

    return eSDK_DelDynamicGroupUser(i64GroupID, vUserID);
}
//====================多选组 ============================================
// SDK存在问题，创建后第一次返回内容正确，但不是UTF8编码，是ANSI编码，
// 重启调度台后查询，如果是中英文混合的名称，英文字符后的没截断
BOOL eSDK_GetMultiGroup(VECTOR<SDK_MULTI_GROUP_INFO_T> &vGroupInfo)
{
    MDC_CHECK;

    CHAR    *szInfo = NULL;
    INT32   iRet = ELTE_SDK_GetMultiSelectedGrpInfoVec(g_acCurrDCUserID, &szInfo);
    XML     xml(szInfo, TRUE);
    XML     *SubXML;
    AnsiString strValue;
    VECTOR<AnsiString>  vGroup;
    std::vector<XML*>   vList;

    eSDK_Free(szInfo);
    ESDK_LOG(iRet);
    if (iRet != 0)
    {
        return FALSE;
    }

    xml.GetNode("Content")->GetNode("MultiSelectedGrpInfoVec")->GetNode("MultiSelectedGrpInfo", vList);

    vGroupInfo.clear();
    for (UINT32 i=0; i<vList.size(); i++)
    {
        SDK_MULTI_GROUP_INFO_T  stInfo;
        INT64                   i64GroupID;

        SubXML = vList[i];

        memset(&stInfo, 0, sizeof(stInfo));
        if (!SubXML->GetAttr("MultiGrpId", &stInfo.i64GroupID) ||
            !SubXML->GetAttr("SetupDcId", &stInfo.ulDCUserID) ||
            !SubXML->GetAttr("MultiGrpName", stInfo.acGroupName, sizeof(stInfo.acGroupName)) ||
            !SubXML->GetAttr("MemberGrpList", strValue) )
        {
            return FALSE;
        }

        if (!ParseStringTuple(strValue, ",", vGroup))
        {
            return FALSE;
        }

        for (UINT32 i=0; i<vGroup.size(); i++)
        {
            if (!ToInt64(vGroup[i], &i64GroupID))
            {
                return FALSE;
            }

            if (stInfo.ulMemberNum >= ARRAY_SIZE(stInfo.ai64MemberGroupID))
            {
                break;
            }
            stInfo.ai64MemberGroupID[stInfo.ulMemberNum++] = i64GroupID;
        }

        vGroupInfo.push_back(stInfo);
    }

    return TRUE;
}

BOOL eSDK_CreateMultiGroup(INT64 i64GroupID, CHAR *szGroupName, VECTOR<INT64> &vGroupID)
{
    MDC_CHECK;

    XmlParam    Param;
    GString     strParam;
    CHAR        acParam[32];

    if (vGroupID.size() > ESDK_MAX_MULTI_GROUP_MEMBER_NUM)
    {
        ESDKLog(LOG_ERROR, "create multi group %s failed, too many patch group member(%u)", szGroupName, vGroupID.size());
        return FALSE;
    }

    for (UINT32 i=0; i<vGroupID.size(); i++)
    {
        sprintf(acParam, "%s"FORMAT_I64, i==0?"":",", vGroupID[i]);
        strParam.Append(acParam);
    }

    if (i64GroupID == INVALID_GROUP_ID)
    {
        Param.Add("MultiGrpId", "");
    }
    else
    {
        Param.Add("MultiGrpId", i64GroupID);
    }

    if (!szGroupName)
    {
        szGroupName = "";
    }
    Param.Add("MultiGrpName", szGroupName);
    Param.Add("MultiGrpMemberList", strParam.GetString());

    INT32 iRet = ELTE_SDK_MultiGrpCreate(Param);

    if (iRet == 0)
    {
        ESDKLog(LOG_INFO, "create multi group "FORMAT_I64 " successful", i64GroupID);
    }
    else
    {
        ESDKLog(LOG_ERROR, "create multi group "FORMAT_I64 " failed, member is %s", i64GroupID, strParam.GetString());
    }

    ESDK_RET(iRet);
}

BOOL eSDK_CancelMultiGroup(INT64 i64GroupID)
{
    MDC_CHECK;

    INT32 iRet = ELTE_SDK_CancelMultiGrp(IntToStr(i64GroupID));

    ESDK_RET(iRet);
}

BOOL eSDK_AddMultiGroupMember(INT64 i64GroupID, VECTOR<INT64> &vGroupID)
{
    MDC_CHECK;

    XmlParam    Param;
    GString     strParam;
    CHAR        acParam[32];

    for (UINT32 i=0; i<vGroupID.size(); i++)
    {
        sprintf(acParam, "%s"FORMAT_I64, i==0?"":",", vGroupID[i]);
        strParam.Append(acParam);
    }

    Param.Add("MultiGrpId", i64GroupID);
//    Param.Add("MultiGrpName", szGroupName);
    Param.Add("MultiGrpMemberList", strParam.GetString());

    INT32 iRet = ELTE_SDK_AddMultiGrpMember(Param);

    ESDK_RET(iRet);
}

BOOL eSDK_DelMultiGroupMember(INT64 i64GroupID, VECTOR<INT64> &vGroupID)
{
    MDC_CHECK;

    XmlParam    Param;
    GString     strParam;
    CHAR        acParam[32];

    for (UINT32 i=0; i<vGroupID.size(); i++)
    {
        sprintf(acParam, "%s"FORMAT_I64, i==0?"":",", vGroupID[i]);
        strParam.Append(acParam);
    }

    Param.Add("MultiGrpId", i64GroupID);
    Param.Add("MultiGrpMemberList", strParam.GetString());

    INT32 iRet = ELTE_SDK_DeleteMultiGrpMember(Param);

    ESDK_RET(iRet);
}

BOOL eSDK_PTTDialMultiGroup(INT64 i64GroupID)
{
    MDC_CHECK;

    INT32 iRet = ELTE_SDK_PTTDialMultiGrp(IntToStr(i64GroupID));

    ESDK_RET(iRet);
}

BOOL eSDK_PTTReleaseMultiGroup(INT64 i64GroupID)
{
    MDC_CHECK;

    INT32 iRet = ELTE_SDK_PTTReleaseMultiGrp(IntToStr(i64GroupID));

    ESDK_RET(iRet);
}

//====================Msg ============================================
BOOL eSDK_SendMsg(AnsiString strMsgInfo, UINT32 ulMsgID, VECTOR<UINT32> &vUserID, VECTOR<INT64> &vGroupID)
{
    return eSDK_SendMsg(strMsgInfo, "", ulMsgID, vUserID, vGroupID);
}

BOOL eSDK_SendMsg(AnsiString strMsgInfo, AnsiString strFile, UINT32 ulMsgID, VECTOR<UINT32> &vUserID, VECTOR<INT64> &vGroupID)
{
    MDC_CHECK;

    XmlParam    Param;
    AnsiString  strReceiver = "";
    AnsiString  strEncodedMsgInfo = EncodeSMSMsg(strMsgInfo);

    if (strEncodedMsgInfo.Length() > MAX_MM_SDS_TEXT_LEN)
    {
        return FALSE;
    }

    for (UINT32 i=0; i<vUserID.size(); i++)
    {
        if (i > 0)
        {
            strReceiver += ";";
        }
        strReceiver += vUserID[i];
    }

    for (UINT32 i=0; i<vGroupID.size(); i++)
    {
        if (!strReceiver.IsEmpty())
        {
            strReceiver += ";";
        }
        strReceiver += vGroupID[i];
    }

    //Param.Add("SDSType", "0004");//SDS_TYPE_MM_SMS);
    Param.Add("MsgBody", strEncodedMsgInfo);
    if (!strFile.IsEmpty())
    {
        AnsiString  strAttachFile = "<AttachFile>"+strFile+"</AttachFile>";

        Param.Add("SDSType", SDS_TYPE_TEXT_SMS);//SDS_TYPE_MM_SMS);
        Param.Add("AttachFileList", strAttachFile);
    }
    else
    {
        Param.Add("SDSType", SDS_TYPE_TEXT_NORMAL);
    }

    INT64   i64MsgID = ulMsgID;

    i64MsgID *= 1000; // ms, 对端根据本字段自动显示成发送时间

    Param.Add("Receiver", strReceiver);
    Param.Add("MsgId", i64MsgID);

    INT32 iRet = ELTE_SDK_SDSSendMessage(g_acCurrDCUserID, Param);

    ESDK_RET(iRet);
}

BOOL eSDK_SendRealMsg(AnsiString strMsgInfo, UINT32 ulSDSID, VECTOR<UINT32> &vUserID)
{
    MDC_CHECK;

    XmlParam    Param;
    AnsiString  strReceiver = "";
    UINT32      ulSDSType = 201; // 可以点对点发即时消息
    AnsiString  strEncodedMsgInfo = EncodeSMSMsg(strMsgInfo);

    if (strEncodedMsgInfo.Length() > MAX_REAL_SDS_TEXT_LEN)
    {
        return FALSE;
    }

    for (UINT32 i=0; i<vUserID.size(); i++)
    {
        if (i > 0)
        {
            strReceiver += ";";
        }
        strReceiver += vUserID[i];
    }

    Param.Add("SDSType", ulSDSType);
    Param.Add("MsgBody", strEncodedMsgInfo);
    Param.Add("Receiver", strReceiver);
    Param.Add("MsgId", ulSDSID);

    INT32 iRet = ELTE_SDK_RealTimeMsgSend(g_acCurrDCUserID, Param);

    ESDK_RET(iRet);
}

BOOL eSDK_SendRealMsg(AnsiString strMsgInfo, UINT32 ulSDSID, VECTOR<INT64> &vGroupID)
{
    MDC_CHECK;

    XmlParam    Param;
    AnsiString  strReceiver = "";
    UINT32      ulSDSType = 202; // 可以发群组即时消息
    AnsiString  strEncodedMsgInfo = EncodeSMSMsg(strMsgInfo);

    if (strEncodedMsgInfo.Length() > MAX_REAL_SDS_TEXT_LEN)
    {
        return FALSE;
    }

    for (UINT32 i=0; i<vGroupID.size(); i++)
    {
        if (i > 0)
        {
            strReceiver += ";";
        }
        strReceiver += vGroupID[i];
    }

    Param.Add("SDSType", ulSDSType);
    Param.Add("MsgBody", strEncodedMsgInfo);
    Param.Add("Receiver", strReceiver);
    Param.Add("MsgId", ulSDSID);

    INT32 iRet = ELTE_SDK_RealTimeMsgSend(g_acCurrDCUserID, Param);

    ESDK_RET(iRet);
}

BOOL eSDK_ReplyRealMsg(VECTOR<UINT32> &vUserID)
{
    MDC_CHECK;

    XmlParam    Param;
    AnsiString  strReceiver = "";

    for (UINT32 i=0; i<vUserID.size(); i++)
    {
        if (i > 0)
        {
            strReceiver += ";";
        }
        strReceiver += vUserID[i];
    }

    Param.Add("SDSType", 203);
    Param.Add("Receiver", strReceiver);

    INT32 iRet = ELTE_SDK_ReplyMsgSend(g_acCurrDCUserID, Param);

    ESDK_RET(iRet);
}

BOOL eSDK_ReplyRealMsg(VECTOR<INT64> &vGroupID)
{
    MDC_CHECK;

    XmlParam    Param;
    AnsiString  strReceiver = "";

    for (UINT32 i=0; i<vGroupID.size(); i++)
    {
        if (i > 0)
        {
            strReceiver += ";";
        }
        strReceiver += vGroupID[i];
    }

    Param.Add("SDSType", 203);
    Param.Add("Receiver", strReceiver);

    INT32 iRet = ELTE_SDK_ReplyMsgSend(g_acCurrDCUserID, Param);

    ESDK_RET(iRet);
}

BOOL eSDK_ReplyRealMsg(UINT32 ulUserID)
{
    VECTOR<UINT32>  vUserID;

    vUserID.push_back(ulUserID);

    return eSDK_ReplyRealMsg(vUserID);
}

BOOL eSDK_ReplyRealMsg(INT64 i64GroupID)
{
    VECTOR<INT64>   vGroupID;

    vGroupID.push_back(i64GroupID);

    return eSDK_ReplyRealMsg(vGroupID);
}


//====================Record============================================
BOOL eSDK_StartRecord(INT64 i64GroupID, UINT32 ulRecType, UINT32 ulRecCallType)
{
    MDC_CHECK;

    XmlParam    Param;
    XmlParam    SubParam;

    SubParam.Add("tasktype", ulRecType);
    SubParam.Add("calltype", ulRecCallType);

    Param.Add("RecordParam", SubParam);

    INT32 iRet = ELTE_SDK_StartRecord(IntToStr(i64GroupID), Param);

    ESDK_RET(iRet);
}

BOOL eSDK_StopRecord(INT64 i64GroupID, UINT32 ulRecType, UINT32 ulRecCallType)
{
    MDC_CHECK;

    XmlParam    Param;
    XmlParam    SubParam;

    SubParam.Add("tasktype", ulRecType);
    SubParam.Add("calltype", ulRecCallType);

    Param.Add("RecordParam", SubParam);

    INT32 iRet = ELTE_SDK_StopRecord(IntToStr(i64GroupID), Param);

    ESDK_RET(iRet);
}

//====================DiscreetListen============================================

BOOL eSDK_StartGroupDiscreetListen(INT64 i64GroupID)
{
    MDC_CHECK;

    INT32 iRet = ELTE_SDK_StartDiscreetListen(IntToStr(i64GroupID));

    ESDK_RET(iRet);
}

BOOL eSDK_StartUserDiscreetListen(UINT32 ulUserID)
{
    MDC_CHECK;

    INT32 iRet = ELTE_SDK_StartDiscreetListen(IntToStr(ulUserID));

    ESDK_RET(iRet);
}

BOOL eSDK_StopGroupDiscreetListen(INT64 i64GroupID)
{
    MDC_CHECK;

    INT32 iRet = ELTE_SDK_StopDiscreetListen(IntToStr(i64GroupID));

    ESDK_RET(iRet);
}

BOOL eSDK_StopUserDiscreetListen(UINT32 ulUserID)
{
    MDC_CHECK;

    INT32 iRet = ELTE_SDK_StopDiscreetListen(IntToStr(ulUserID));

    ESDK_RET(iRet);
}

//====================Env Listen============================================
BOOL eSDK_StartEnvListen(UINT32 ulUserID)
{
    MDC_CHECK;

    INT32 iRet = ELTE_SDK_StartEnvironmentListen(IntToStr(ulUserID));

    ESDK_RET(iRet);
}

BOOL eSDK_StopEnvListen(UINT32 ulUserID)
{
    MDC_CHECK;

    INT32 iRet = ELTE_SDK_P2PHangup(IntToStr(ulUserID));

    ESDK_RET(iRet);
}

//====================GIS Sub============================================
BOOL eSDK_GISSub(VECTOR<UINT32> &vUserID)
{
    MDC_CHECK;

    XmlParam    Param;
    XmlParam    SubParam;
    AnsiString  strResourceList = "";

    if (vUserID.size() > 200)
    {
        return FALSE;
    }

    for (UINT32 i=0; i<vUserID.size(); i++)
    {
        if (i > 0)
        {
            strResourceList += ":";
        }
        strResourceList += vUserID[i];
    }

    SubParam.Add("SubType", 7);     // 7：代表订阅。 8：代表不订阅。
    SubParam.Add("ExpiredTime", 60);    // 订阅 ：0 ~ 60分钟；超过过期时间需要重新订阅。 去订阅 ：-1 。
    SubParam.Add("AutoResub", 1);       //  0：默认值。 1：自动重新订阅：周期超时前，自动订阅
    SubParam.Add("ResourceList", strResourceList);  // 资源列表，多个资源ID用分号分隔，最多200个
    SubParam.Add("Subscriber", "");
    Param.Add("GISParam", SubParam);

    INT32 iRet = ELTE_SDK_GISSubscribe("0", Param);

    ESDK_RET(iRet);
}

BOOL eSDK_GISSub(UINT32 ulUserID)
{
    VECTOR<UINT32>  vUserID;

    vUserID.push_back(ulUserID);

    return eSDK_GISSub(vUserID);
}

BOOL eSDK_GISUnSub(VECTOR<UINT32> &vUserID)
{
    MDC_CHECK;

    XmlParam    Param;
    XmlParam    SubParam;
    AnsiString  strResourceList = "";

    if (vUserID.size() > 200)
    {
        return FALSE;
    }

    for (UINT32 i=0; i<vUserID.size(); i++)
    {
        if (i > 0)
        {
            strResourceList += ":";
        }
        strResourceList += vUserID[i];
    }

    SubParam.Add("SubType", 8);         // 7：代表订阅。 8：代表不订阅。
    SubParam.Add("ExpiredTime", -1);    // 订阅 ：0 ~ 60分钟；超过过期时间需要重新订阅。 去订阅 ：-1 。
    SubParam.Add("AutoResub", 0);       //  0：默认值。 1：自动重新订阅：周期超时前，自动订阅
    SubParam.Add("ResourceList", strResourceList);  // 资源列表，多个资源ID用分号分隔，最多200个
    SubParam.Add("Subscriber", "");
    Param.Add("GISParam", SubParam);

    INT32 iRet = ELTE_SDK_GISSubscribe("0", Param);

    ESDK_RET(iRet);
}

BOOL eSDK_GISUnSub(UINT32 ulUserID)
{
    VECTOR<UINT32>  vUserID;

    vUserID.push_back(ulUserID);

    return eSDK_GISUnSub(vUserID);
}

//====================DiscreetListen============================================
BOOL eSDK_VolumeControl(INT64 i64ResourceID, UINT32 ulVolumn)
{
    MDC_CHECK;

    if (ulVolumn > 100)
    {
        return FALSE;
    }

    INT32 iRet = ELTE_SDK_VolumeControl(IntToStr(i64ResourceID), IntToStr(ulVolumn));

    ESDK_RET(iRet);
}

BOOL eSDK_VolumeMute(INT64 i64ResourceID, UINT32 ulCallType)
{
    MDC_CHECK;

    INT32 iRet = ELTE_SDK_VolMute(IntToStr(i64ResourceID), IntToStr(ulCallType));

    ESDK_RET(iRet);
}

BOOL eSDK_VolumeUnMute(INT64 i64ResourceID, UINT32 ulCallType)
{
    MDC_CHECK;

    INT32 iRet = ELTE_SDK_VolUnMute(IntToStr(i64ResourceID), IntToStr(ulCallType));

    ESDK_RET(iRet);
}


//====================VIDEO============================================
BOOL eSDK_StartVideoCall(UINT32 ulNumber, UINT32 ulVideoFormat, UINT32 ulCameraType,
                         BOOL bNeedConfirm, BOOL bHasVoice)
{
    MDC_CHECK;

    CHAR    acParam[1024];
    UINT32  ulUserConfirmType;
    UINT32  ulMuteType;
    CHAR    acNumber[32];

    sprintf(acNumber, "%u", ulNumber);

    if (bHasVoice)
    {
        ulMuteType = VIDEO_VOICE_OPEN;  // 需要伴音
    }
    else
    {
        ulMuteType = VIDEO_VOICE_MUTE;  // 不需要伴音
    }

    if (bNeedConfirm)
    {
        ulUserConfirmType = VIDEO_CALL_CONFIRM_NEED; // 需要用户确认
    }
    else
    {
        ulUserConfirmType = VIDEO_CALL_CONFIRM_NONE; // 不需要用户确认
    }

    XmlParam    Param;
    XmlParam    SubParam;

    SubParam.Add("VideoFormat", ulVideoFormat);
    SubParam.Add("CameraType", ulCameraType);
    SubParam.Add("UserConfirmType", ulUserConfirmType);
    SubParam.Add("MuteType", ulMuteType);

    Param.Add("VideoParam", SubParam);

    INT32   iRet = ELTE_SDK_StartVideoDial(acNumber, Param);

    ESDK_RET(iRet);
}

BOOL eSDK_StartRealPlay(UINT32 ulNumber, UINT32 ulVideoFormat, UINT32 ulCameraType,
                        BOOL bNeedConfirm, BOOL bHasVoice)
{
    MDC_CHECK;

    CHAR    acParam[1024];
    UINT32  ulUserConfirmType;
    UINT32  ulMuteType;
    CHAR    acNumber[32];

    sprintf(acNumber, "%u", ulNumber);

    if (bHasVoice)
    {
        ulMuteType = VIDEO_VOICE_OPEN;  // 需要伴音
    }
    else
    {
        ulMuteType = VIDEO_VOICE_MUTE;  // 不需要伴音
    }

    if (bNeedConfirm)
    {
        ulUserConfirmType = VIDEO_CALL_CONFIRM_NEED; // 需要用户确认
    }
    else
    {
        ulUserConfirmType = VIDEO_CALL_CONFIRM_NONE; // 不需要用户确认
    }

    XmlParam    Param;
    XmlParam    SubParam;

    SubParam.Add("VideoFormat", ulVideoFormat);
    SubParam.Add("CameraType", ulCameraType);
    SubParam.Add("UserConfirmType", ulUserConfirmType);
    SubParam.Add("MuteType", ulMuteType);

    Param.Add("VideoParam", SubParam);

    INT32   iRet = ELTE_SDK_StartRealPlay(acNumber, Param);

    ESDK_RET(iRet);
}

BOOL eSDK_StopRealPlay(UINT32 ulNumber)
{
    MDC_CHECK;

    INT32   iRet =  ELTE_SDK_StopRealPlay(IntToStr(ulNumber));

    ESDK_RET(iRet);
}

// 接受视频点呼\回传\视频分发
BOOL eSDK_RecvVideoPlay(UINT32 ulNumber)//, HANDLE hPlayWnd)
{
    MDC_CHECK;

    CHAR    acNumber[32];

    sprintf(acNumber, "%u", ulNumber);

    INT32   iRet =  ELTE_SDK_RecvVideoPlay(acNumber);

    ESDK_RET(iRet);
}

BOOL eSDK_SetVideoPlayWindow(UINT32 ulNumber, HANDLE hPlayWnd)
{
    MDC_CHECK;

    CHAR    acNumber[32];

    sprintf(acNumber, "%u", ulNumber);

    INT32   iRet =  ELTE_SDK_SetPlayWindow(acNumber, hPlayWnd);

    ESDK_LOG(iRet);
    if (iRet != 0)
    {
        ELTE_SDK_StopRealPlay(acNumber);
        return FALSE;
    }

    return TRUE;
}

BOOL eSDK_P2PVideoReject(UINT32 ulNumber)
{
    MDC_CHECK;

    INT32   iRet =  ELTE_SDK_P2PVideoReject(IntToStr(ulNumber));

    ESDK_RET(iRet);
}

BOOL eSDK_StartVideoDispatch(UINT32 ulSrcUserID, VECTOR<UINT32> vDstUserID, BOOL bOriginalVideoFormat)
{
    if (vDstUserID.size() == 0)
    {
        return TRUE;
    }

    MDC_CHECK;

    XmlParam    Param;
    XmlParam    SubParam;

    for (UINT32 i=0; i<vDstUserID.size(); i++)
    {
        SubParam.Add("Dstviewer", vDstUserID[i]);
    }

    Param.Add("Fmtvalue", bOriginalVideoFormat?"NO":"CIF");
    Param.Add("DispatchNum", ulSrcUserID);
    Param.Add("Dstviewerlist", SubParam);
    Param.Add("Channel", "");

    INT32 iRet = ELTE_SDK_StartVideoDispatch(IntToStr(ulSrcUserID), Param);

    ESDK_RET(iRet);
}

BOOL eSDK_StopVideoDispatch(UINT32 ulSrcUserID, UINT32 ulDstUserID)
{
    MDC_CHECK;

    XmlParam    Param;

    Param.Add("ResourceId", g_acCurrDCUserID);
    Param.Add("UserId", ulDstUserID);

    INT32 iRet = ELTE_SDK_StopVideoDispatch(IntToStr(ulSrcUserID), Param);

    ESDK_RET(iRet);
}

//====================视频上墙============================================
BOOL eSDK_GetVWallInfo(VECTOR<ESDK_VWALL_INFO_T> &vInfo)
{
    MDC_CHECK;

    CHAR    *szInfo = NULL;
    INT32   iRet = ELTE_SDK_GetDcVWallIDList(&szInfo);
    XML     xml(szInfo, TRUE);
    XML     *SubXML;
    std::vector<XML*> vList;

    vInfo.clear();
    eSDK_Free(szInfo);
    ESDK_LOG(iRet);
    if (iRet != 0)
    {
        return FALSE;
    }

    xml.GetNode("Content")->GetNode("VWallIDList")->GetNode("VWallID", vList);
    for (UINT32 i=0; i<vList.size(); i++)
    {
      /*      <DstObjId>99910001</DstObjId>
            <IDState>1</IDState>
            <Alias>IVS1</Alias>
        */
        ESDK_VWALL_INFO_T   stInfo = {0};

        if (!vList[i]->GetAttr("DstObjId", &stInfo.ulVideoChanID) ||
            !vList[i]->GetAttr("IDState", &stInfo.ulState) ||
            !vList[i]->GetAttr("Alias", stInfo.acAlias, sizeof(stInfo.acAlias)) )
        {
            return FALSE;
        }

        vInfo.push_back(stInfo);
    }

    return TRUE;
}

BOOL eSDK_VWallStart(UINT32 ulUserID, UINT32 ulVideoChanID)
{
    MDC_CHECK;

    XmlParam    Param;
    XmlParam    SubParam;

    SubParam.Add("DstObjId", ulVideoChanID);
    SubParam.Add("StrFmt", "");

    Param.Add("VideoParam", SubParam);

    INT32 iRet = ELTE_SDK_VWallStart(IntToStr(ulUserID), Param);

    ESDK_RET(iRet);
}

BOOL eSDK_VWallStop(UINT32 ulUserID, UINT32 ulVideoChanID)
{
    MDC_CHECK;

    XmlParam    Param;
    XmlParam    SubParam;

    SubParam.Add("DstObjId", ulVideoChanID);

    Param.Add("VideoParam", SubParam);

    INT32 iRet = ELTE_SDK_VWallStop(IntToStr(ulUserID), Param);

    ESDK_RET(iRet);
}

//====================录音录像============================================
/*
RECQueryInfo 录音录像查询信息。
CallType    呼叫类型：
0： ENUM_REC_P2P_AUDIO，语音点呼。
l 1： ENUM_REC_P2P_VIDEO，视频点呼。
l 2： ENUM_REC_VIDEO_UPLOAD，视频回传。
l 3： ENUM_REC_PTT，群组呼叫
Caller 主叫号码，默认值可填-1。
Callee 被叫号码，默认值可填-1。
ResourceID  资源ID（包括群组ID和终端ID），默认值可填-1。
StartSec   查询开始时间 yyyy-MM-ddHH:mm:ss，例如： 2001-01-02 00:00:00。
EndSec      查询结束时间
*/

BOOL eSDK_GetRecFileList(UINT32 ulRecType, INT32 iCaller, INT32 iCallee, INT64 i64ResourceID,
                         UINT32 ulStartTime, UINT32 ulEndTime, VECTOR<ESDK_REC_FILE_INFO_T> &vRecList)
{
    MRS_CHECK;

    CHAR        *szFileList = NULL;
    INT32       iRet;
    XmlParam    Param;
    XmlParam    SubParam;
    CHAR        acStartTime[32];
    CHAR        acEndTime[32];

    gos_asctime(ulStartTime, acStartTime);
    gos_asctime(ulEndTime, acEndTime);

    SubParam.Add("CallType", ulRecType);
    SubParam.Add("Caller", iCaller);
    SubParam.Add("Callee", iCallee);
    SubParam.Add("ResourceID", i64ResourceID);
    SubParam.Add("StartSec", acStartTime);
    SubParam.Add("EndSec", acEndTime);

    Param.Add("RECQueryInfo", SubParam);

    iRet = ELTE_SDK_GetUserRECFileInfoList(Param, &szFileList);

 /* if (g_bTestMode)
    {
        CHAR            acLogFile[256];

        sprintf(acLogFile, "%s/%s", gos_get_root_path(), "sdk_rec.log");

        FILE            *fp = fopen(acLogFile, "w+t");

        if (fp)
        {
            fprintf(fp, "%s\n\n%s", Param.ToStr(), szFileList?szFileList:"");
            fclose(fp);
        }
    }*/

    ESDK_LOG(iRet);
    if (iRet != 0)
    {
        eSDK_Free(szFileList);
        return FALSE;
    }

    XML             Xml(szFileList, TRUE);
    VECTOR<XML*>    vSubXml;
    XML             *SubXml;
    CHAR            *szValue;

    eSDK_Free(szFileList);

    Xml.GetNode("Content")->GetNode("RECFileInfoList")->GetNode("RECFileInfo", vSubXml);
    for (UINT32 i=0; i<vSubXml.size(); i++)
    {
        ESDK_REC_FILE_INFO_T    stRec;

        SubXml = vSubXml[i];

        if (!SubXml->GetAttr("CallType", &stRec.ulRecType) ||
            !SubXml->GetAttr("Caller", &stRec.ulCaller) ||
            !SubXml->GetAttr("Callee", &stRec.ulCallee) ||
            !SubXml->GetAttr("ResourceID", &stRec.i64ResourceID) )
        {
            return FALSE;
        }

        szValue = SubXml->GetAttr("StartSec");
        stRec.ulStartTime = gos_text_time_to_int(szValue);
        if (stRec.ulStartTime == 0xffffffff)
        {
            return FALSE;
        }

        szValue = SubXml->GetAttr("EndSec");
        stRec.ulEndTime = gos_text_time_to_int(szValue);
        if (stRec.ulEndTime == 0xffffffff)
        {
            return FALSE;
        }

        szValue = SubXml->GetAttr("UrlFTP");
        if (!szValue)
        {
            return FALSE;
        }
        stRec.strHttpUrl = szValue;

        szValue = SubXml->GetAttr("UrlRTSP");
        if (!szValue)
        {
            return FALSE;
        }
        stRec.strRtspUrl = szValue;

        vRecList.push_back(stRec);
    }

    return TRUE;
}

//====================MISC============================================
AnsiString GetGroupCallStatusText(UINT32 ulStatus)
{
    if (ulStatus == PTT_IND_STATUS_UNKNOWN         )    return "";
    if (ulStatus == PTT_IND_STATUS_RECEIVED        )    return "组呼被接收";
    if (ulStatus == PTT_IND_STATUS_REQUEST         )    return "";
    if (ulStatus == PTT_IND_STATUS_ACCEPT          )    return "话权接收。";
    if (ulStatus == PTT_IND_STATUS_WAITING         )    return "";
    if (ulStatus == PTT_IND_STATUS_REJECT          )    return "话权拒绝";
    if (ulStatus == PTT_IND_STATUS_SNATCH          )    return "组呼建立成功";
    if (ulStatus == PTT_IND_STATUS_HANGUP_OK       )    return "调度台离开组呼";
    if (ulStatus == PTT_IND_STATUS_CLOSE_OK        )    return "组呼结束";
    if (ulStatus == PTT_IND_STATUS_CANCEL_OK       )    return "组呼建立失败";
    if (ulStatus == PTT_IND_STATUS_PREMPTED        )    return "组呼被替换";
    if (ulStatus == PTT_IND_STATUS_TX_BEGIN        )    return "话权变更";
    if (ulStatus == PTT_IND_STATUS_TX_IDLE         )    return "话权空闲";
    if (ulStatus == PTT_IND_STATUS_MEDIAPLANE_ERROR)    return "组呼创建失败";
    if (ulStatus == PTT_IND_STATUS_EMERGENCY_BEGIN )    return "收到一路紧急组呼";
    if (ulStatus == PTT_IND_STATUS_EMERGENCY_UPDATE)    return "转变为紧急组呼";
    if (ulStatus == PTT_IND_STATUS_LICENSE_LIMIT   )    return "证书受限";
    if (ulStatus == PTT_IND_STATUS_SIGNAL_ERROR    )    return "SIP错误";

    return "";
}

BOOL GetLocalIP(UINT8 *pucServerIP, UINT32 ulTimeout, UINT8 *pucLocalIP)
{
    DOUBLE  dDelay = 0;
    UINT16  usSeq = 0;
    UINT32  ulTTL = 0;

    if (!gos_ping(pucServerIP, 64, ulTimeout, &dDelay, &usSeq, &ulTTL, pucLocalIP))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL GetLocalIP(UINT8 *pucServerIP, UINT8 *pucLocalIP)
{
    UINT32  aulTimeout[] = {200, 500, 1000};

    for (UINT32 i=0; i<ARRAY_SIZE(aulTimeout); i++)
    {
        if (GetLocalIP(pucServerIP, aulTimeout[i], pucLocalIP))
        {
            return TRUE;
        }
    }

    return FALSE;
}

#if 0

BOOL Test_Rec()
{
    CHAR            acFile[] = "c:\\rec_1110.dat";
    FILE            *fp = fopen(acFile, "rb");

    CHAR            acFileList[10*1024];

    memset(acFileList, 0, sizeof(acFileList));
    fread(acFileList, 1, sizeof(acFileList), fp);
    fclose(fp);

    XML             Xml(acFileList, TRUE);
    VECTOR<XML*>    vSubXml;
    XML             *SubXml;
    CHAR            *szValue;

    Xml.GetNode("Content")->GetNode("RECFileInfoList")->GetNode("RECFileInfo", vSubXml);
    for (UINT32 i=0; i<vSubXml.size(); i++)
    {
        ESDK_REC_FILE_INFO_T    stRec;

        SubXml = vSubXml[i];

        if (!SubXml->GetAttr("CallType", &stRec.ulRecType) ||
            !SubXml->GetAttr("Caller", &stRec.ulCaller) ||
            !SubXml->GetAttr("Callee", &stRec.ulCallee) ||
            !SubXml->GetAttr("ResourceID", &stRec.i64ResourceID) )
        {
            return FALSE;
        }

        szValue = SubXml->GetAttr("StartSec");
        stRec.ulStartTime = gos_text_time_to_int(szValue);
        if (stRec.ulStartTime == 0xffffffff)
        {
            return FALSE;
        }

        szValue = SubXml->GetAttr("EndSec");
        stRec.ulEndTime = gos_text_time_to_int(szValue);
        if (stRec.ulEndTime == 0xffffffff)
        {
            return FALSE;
        }

        szValue = SubXml->GetAttr("UrlFTP");
        if (!szValue)
        {
            return FALSE;
        }
        stRec.strHttpUrl = szValue;

        szValue = SubXml->GetAttr("UrlRTSP");
        if (!szValue)
        {
            return FALSE;
        }
        stRec.strRtspUrl = szValue;

//      vRecList.push_back(stRec);
    }

    return TRUE;
}


#endif

