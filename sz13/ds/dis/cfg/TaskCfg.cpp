#include "DISPublic.h"
#include "CfgDao.h"
#include "ats.h"
#include "TaskCfg.h"

#define CHECK_TIMER         TIMER10
#define EV_CHECK_TIMER      GET_TIMER_EV(CHECK_TIMER)

extern BOOL IsLocalAddr(const CHAR *szThreadName, UINT8 *pucAddr);

TaskCfg::TaskCfg(UINT16 usDispatchID):GBaseTask(MODULE_CFG, "CFG", usDispatchID)
{
    InitCfgDataVer();
    InitCfgDataVerMap();

    //m_ulRealBrdSeqID = 0;
    m_bCfgDataChanged = FALSE;

    m_ulDCUserID = INVALID_USER_ID;
    m_acDCOperatorID[0] = '\0';

    m_pDao = NULL;

    RegisterMsg(EV_DC_DIS_REQ);
    RegisterMsg(EV_DC_DIS_CFG_REQ);
}

BOOL TaskCfg::AddOperLog(const CHAR *szLog, ...)
{
    if (!m_pDao)
    {
        return FALSE;
    }

    CHAR            acLog[4096];
    va_list         vaLog;

    va_start(vaLog, szLog);

    memset(acLog, 0, sizeof(acLog));
    vsnprintf(acLog, sizeof(acLog)-1, szLog, vaLog);

    va_end(vaLog);

    return AddOperLog(gos_get_current_time(), acLog);
}

BOOL TaskCfg::AddOperLog(UINT32 ulLogTime, CHAR *szLog)
{
    if (!m_pDao)
    {
        return FALSE;
    }

    if (!m_pDao->AddOperLog(ulLogTime, m_ulDCUserID, m_acDCOperatorID, szLog))
    {
        TaskLog(LOG_ERROR, "%s add oper log failed: %s", m_acDCOperatorID, szLog);

        return FALSE;
    }
    else
    {
        TaskLog(LOG_INFO, "%s add oper log: %s", m_acDCOperatorID, szLog);
        return TRUE;
    }
}

VOID TaskCfg::KickOffDC(PID_T *pstPID)
{
    if (!GetLocalCfg().bAllowKickOffDC)
    {
        return;
    }

    GJsonParam  Param;

    SendClient(pstPID, EV_DC_DIS_RSP, MSG_DISKickOffDC, 0, Param);
}

BOOL TaskCfg::Init()
{
    if (!m_pDao)
    {
        m_pDao = GetCfgDao();
        if (!m_pDao)
        {
            TaskLog(LOG_ERROR, "get db dao failed");

            return FALSE;
        }
    }

    if (m_ulLoadGroupCfg == 0)
    {
        GROUP_CFG_T     stCfg;
        INT32           iSyncFlag;

        if (!m_pDao->GetGroupCfg(stCfg, iSyncFlag))
        {
            TaskLog(LOG_ERROR, "load group cfg failed");

            return FALSE;
        }

        SetGroupCfg(&stCfg);
        TaskLog(LOG_INFO, "load group cfg succ");
        SetCfgDataVer(m_ulLoadGroupCfg);
    }

    if (m_ulLoadStationInfo == 0)
    {
        VECTOR<STATION_INFO_T> vInfo;

        if (!m_pDao->GetStation(vInfo))
        {
            TaskLog(LOG_ERROR, "load station info failed");

            return FALSE;
        }

        InitStationInfo(vInfo);
        TaskLog(LOG_INFO, "load station info succ");
        SetCfgDataVer(m_ulLoadStationInfo);
    }

    if (m_ulLoadSectionInfo == 0)
    {
        VECTOR<SECTION_INFO_T> vInfo;

        if (!m_pDao->GetSection(vInfo))
        {
            TaskLog(LOG_ERROR, "load section info failed");

            return FALSE;
        }

        InitSectionInfo(vInfo);
        TaskLog(LOG_INFO, "load section info succ");
        SetCfgDataVer(m_ulLoadSectionInfo);
    }

    if (m_ulLoadDestInfo == 0)
    {
        VECTOR<DEST_INFO_T> vInfo;

        if (!m_pDao->GetDestInfo(vInfo))
        {
            TaskLog(LOG_ERROR, "load dest info failed");

            return FALSE;
        }

        InitDestInfo(vInfo);
        TaskLog(LOG_INFO, "load dest info succ");
        SetCfgDataVer(m_ulLoadDestInfo);
    }

    if (m_ulLoadDCUserInfo == 0)
    {
        VECTOR<DC_USER_INFO_T>      vInfo;

        if (!m_pDao->GetDCUser(vInfo))
        {
            TaskLog(LOG_ERROR, "load dc user info failed");

            return FALSE;
        }

        InitDCUserInfo(vInfo);
        TaskLog(LOG_INFO, "load dc user info succ");
        SetCfgDataVer(m_ulLoadDCUserInfo);
    }

    if (m_ulLoadDCOperatorInfo == 0)
    {
        VECTOR<DC_OPERATOR_INFO_T>      vInfo;

        if (!m_pDao->GetDCOperator(vInfo))
        {
            TaskLog(LOG_ERROR, "load dc operator info failed");

            return FALSE;
        }

        InitDCOperatorInfo(vInfo);

        DC_OPERATOR_INFO_T      stInfo;

        if (!GetDCOperatorInfo((CHAR*)"admin", stInfo))
        {
            strcpy(stInfo.acOperatorID, "admin");
            strcpy(stInfo.acName, "超级管理员");
            strcpy(stInfo.acPassword, "admin");
            stInfo.ulDCType = 0xffff;

            AddDCOperatorInfo(stInfo);
            m_pDao->AddDCOperator(stInfo);
        }

        TaskLog(LOG_INFO, "load dc user info succ");
        SetCfgDataVer(m_ulLoadDCOperatorInfo );
    }

    if (m_ulLoadDepotInfo == 0)
    {
        VECTOR<DEPOT_INFO_T>        vInfo;

        if (!m_pDao->GetDepotCfg(vInfo))
        {
            TaskLog(LOG_ERROR, "load depot info failed");

            return FALSE;
        }

        InitDepotInfo(vInfo);
        TaskLog(LOG_INFO, "load depot info succ");
        SetCfgDataVer(m_ulLoadDepotInfo);
    }

    if (m_ulLoadStationUEInfo == 0)
    {
        VECTOR<STATION_UE_INFO_T>       vInfo;

        if (!m_pDao->GetStationUE(vInfo))
        {
            TaskLog(LOG_ERROR, "load station user info failed");

            return FALSE;
        }

        InitStationUEInfo(vInfo);
        TaskLog(LOG_INFO, "load station user info succ");
        SetCfgDataVer(m_ulLoadStationUEInfo);
    }

    if (m_ulLoadPortalUEInfo == 0)
    {
        VECTOR<PORTAL_UE_INFO_T>        vInfo;

        if (!m_pDao->GetPortalUE(vInfo))
        {
            TaskLog(LOG_ERROR, "load portal user info failed");

            return FALSE;
        }

        InitPortalUEInfo(vInfo);
        TaskLog(LOG_INFO, "load portal user info succ");
        SetCfgDataVer(m_ulLoadPortalUEInfo);
    }

    if (m_ulLoadBrdZoneInfo == 0)
    {
        VECTOR<BRD_ZONE_INFO_T>     vInfo;

        if (!m_pDao->GetBrdZoneInfo(vInfo))
        {
            TaskLog(LOG_ERROR, "load broadcast zone info failed");

            return FALSE;
        }

        InitBrdZoneInfo(vInfo);
        TaskLog(LOG_INFO, "load broadcast zone info succ");
        SetCfgDataVer(m_ulLoadBrdZoneInfo);
    }

    if (m_ulLoadTrainInfo == 0)
    {
        VECTOR<TRAIN_T>     vInfo;

        if (!m_pDao->GetTrainInfo(vInfo))
        {
            TaskLog(LOG_ERROR, "load train info failed");

            return FALSE;
        }

        InitTrainInfo(vInfo);
        TaskLog(LOG_INFO, "load train zone info succ");
        SetCfgDataVer(m_ulLoadTrainInfo);
    }

    if (m_ulLoadTrainUEInfo == 0)
    {
        VECTOR<TRAIN_UE_INFO_T>     vInfo;

        if (!m_pDao->GetTrainUE(vInfo))
        {
            TaskLog(LOG_ERROR, "load train user info failed");

            return FALSE;
        }

        InitTrainUEInfo(vInfo);
        TaskLog(LOG_INFO, "load train user info succ");
        SetCfgDataVer(m_ulLoadTrainUEInfo);
    }

    if (m_ulLoadMultiGroupInfo == 0)
    {
        VECTOR<MULTI_GROUP_INFO_T>       vInfo;

        if (!m_pDao->GetMultiGroupInfo(vInfo))
        {
            TaskLog(LOG_ERROR, "load multi group info failed");

            return FALSE;
        }

        InitMultiGroupInfo(vInfo);
        TaskLog(LOG_INFO, "load multi group info succ");
        SetCfgDataVer(m_ulLoadMultiGroupInfo);
    }

    if (m_ulLoadPredefinedSMS == 0)
    {
        VECTOR<PREDEFINED_SMS_INFO_T>   vSMS;

        if (!m_pDao->GetPredefinedSMS(vSMS))
        {
            TaskLog(LOG_ERROR, "load predefine sms failed");
            return FALSE;
        }

        InitPredefinedSMS(vSMS);
        TaskLog(LOG_INFO, "load predefine sms successfully");
        SetCfgDataVer(m_ulLoadPredefinedSMS);
    }

    if (m_ulLoadRecBrdCfg == 0)
    {
        VECTOR<REC_BRD_CFG_T>       vInfo;

        if (!m_pDao->GetRecBrdCfg(vInfo))
        {
            TaskLog(LOG_ERROR, "load rec broadcast info failed");

            return FALSE;
        }

        InitRecBrdCfg(vInfo);
        TaskLog(LOG_INFO, "load rec broadcast info succ");
        SetCfgDataVer(m_ulLoadRecBrdCfg);
    }

    if (m_ulLoadAPPCfg == 0)
    {
        APP_BASIC_CFG_T     stInfo;

        if (!m_pDao->GetAppCfg(&stInfo))
        {
            TaskLog(LOG_ERROR, "load APP cfg info failed");

            return FALSE;
        }

        InitAPPBasicCfg(stInfo);
        TaskLog(LOG_INFO, "load APP cfg info succ, NTP addr is %s", stInfo.acNTPServerAddr);
        SetCfgDataVer(m_ulLoadAPPCfg);
    }

/*    if (!m_bGetRealBrdSeqID)
    {
        if (!m_pDao->GetMaxRealBrdSeqID(&m_ulRealBrdSeqID))
        {
            TaskLog(LOG_ERROR, "get max real brd seq id failed");

            return FALSE;
        }

        m_bGetRealBrdSeqID = TRUE;
    }*/

    SetLoopTimer(CHECK_TIMER, 10*1000);
    SetTaskStatus(TASK_STATUS_WORK);

    TaskLog(LOG_INFO, "Task Cfg init successful!");

    return TRUE;
}

VOID TaskCfg::OnServerDisconnectClient(UINT16 usClientID)
{
}

VOID TaskCfg::OnGetClientVer(VOID *pvMsg, UINT32 ulMsgLen)
{
}

VOID TaskCfg::OnDCDISReq(CHAR *szReq, UINT32 ulMsgLen)
{
    GJson       Json;
    GJson       SubJson;
    BOOL        bRet = Json.Parse(szReq);
    CHAR        *szMsgType;
    CHAR        *szMsgInfo;
    UINT32      ulSeqID;
    UINT32      ulDCUserID;
    CHAR        *szDCOperatorID;

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "invalid DC msg: %s", szReq);
        goto error;
    }

    szMsgType = Json.GetValue("MsgName");

    if (!szMsgType)
    {
        TaskLog(LOG_ERROR, "get MsgName failed: %s", szReq);
        goto error;
    }

    if (!Json.GetValue("MsgSeqID", &ulSeqID))
    {
        TaskLog(LOG_ERROR, "get MsgSeqID failed: %s", szReq);
        goto error;
    }

    if (!Json.GetValue("DCUserID", &ulDCUserID))
    {
        TaskLog(LOG_ERROR, "get DCUserID failed: %s", szReq);
        goto error;
    }

    szMsgInfo = Json.GetValue("MsgInfo");
    if (!szMsgInfo)
    {
        TaskLog(LOG_ERROR, "get MsgInfo failed: %s", szReq);
        goto error;
    }

    if (!SubJson.Parse(szMsgInfo))
    {
        TaskLog(LOG_ERROR, "parse MsgInfo failed: %s", szReq);
        goto error;
    }

    szDCOperatorID = Json.GetValue("DCOperatorID");
    if (!szDCOperatorID)
    {
        sprintf(m_acDCOperatorID, "%u", ulDCUserID);
    }
    else
    {
        memset(m_acDCOperatorID, 0, sizeof(m_acDCOperatorID));
        strncpy(m_acDCOperatorID, szDCOperatorID, sizeof(m_acDCOperatorID)-1);
    }

    m_ulDCUserID = ulDCUserID;

    if (strcmp(szMsgType, MSG_DCLoadTrainInfo) == 0)
    {
        static UINT32   ulLastLoadTrainInfoTime = 0;
        UINT32          ulCurrLoadTrainInfoTime = gos_get_uptime();

        if (ulLastLoadTrainInfoTime == 0 ||
            (UINT32)(ulCurrLoadTrainInfoTime - ulLastLoadTrainInfoTime) > 300)
        {
            TaskLog(LOG_INFO, "get DC msg: %s", szReq);
            ulLastLoadTrainInfoTime = ulCurrLoadTrainInfoTime;
        }
    }
    else
    {
        TaskLog(LOG_INFO, "get DC msg: %s", szReq);
    }

    if (strcmp(szMsgType, MSG_DCRegisterDIS) == 0)
    {
        OnDCRegisterDISReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadAPPCfg) == 0)
    {
        OnDCLoadAPPCfgReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadGroupCfg) == 0)
    {
        OnDCLoadGroupCfgReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadDCUserInfo) == 0)
    {
        OnDCLoadDCUserInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadDCOperatorInfo) == 0)
    {
        OnDCLoadDCOperatorInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadTrainUEInfo) == 0)
    {
        OnDCLoadTrainUEInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadStationUEInfo) == 0)
    {
        OnDCLoadStationUEInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadPortalUEInfo) == 0)
    {
        OnDCLoadPortalUEInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadGroupInfo) == 0)
    {
        OnDCLoadGroupInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadAutoJoinGroupInfo) == 0)
    {
        OnDCLoadAutoJoinGroupInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadSubGroupInfo) == 0)
    {
        OnDCLoadSubGroupInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadSubVideoGroupInfo) == 0)
    {
        OnDCLoadSubVideoGroupInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadSubUserInfo) == 0)
    {
        OnDCLoadSubUserInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadSubVideoUserInfo) == 0)
    {
        OnDCLoadSubVideoUserInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadBrdZoneInfo) == 0)
    {
        OnDCLoadBrdZoneInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadTrainInfo) == 0)
    {
        OnDCLoadTrainInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadStationInfo) == 0)
    {
        OnDCLoadStationInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadMultiGroupInfo) == 0)
    {
        OnDCLoadMultiGroupInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadP2PCallInfo) == 0)
    {
        OnDCLoadP2PCallInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadGroupCallInfo) == 0)
    {
        OnDCLoadGroupCallInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadReqCallInfo) == 0)
    {
        OnDCLoadReqCallInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadRecBrdInfo) == 0)
    {
        OnDCLoadRecBrdInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadRealBrdInfo) == 0)
    {
        OnDCLoadRealBrdInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadIPHTalkInfo) == 0)
    {
        OnDCLoadIPHTalkInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadOperLogInfo) == 0)
    {
        OnDCLoadOperLogInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSaveOperLog) == 0)
    {
        OnDCSaveOperLogReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSaveGroupStatus) == 0)
    {
        OnDCSaveGroupStatusReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSaveSubGroupInfo) == 0)
    {
        OnDCSaveSubGroupInfoReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSaveSubVideoGroupInfo) == 0)
    {
        OnDCSaveSubVideoGroupInfoReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSaveSubUserInfo) == 0)
    {
        OnDCSaveSubUserInfoReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSaveSubVideoUserInfo) == 0)
    {
        OnDCSaveSubVideoUserInfoReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSaveP2PCallInfo) == 0)
    {
        OnDCSaveP2PCallInfoReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSaveGroupCallInfo) == 0)
    {
        OnDCSaveGroupCallInfoReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSaveP2PVideoCallInfo) == 0)
    {
        OnDCSaveP2PVideoCallInfoReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSaveRealBrdInfo) == 0)
    {
        OnDCSaveRealBrdInfoReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSaveGroupInfo) == 0)
    {
        OnDCSaveGroupInfoReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadPredefinedSMS) == 0)
    {
        OnDCLoadPredefinedSMSReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadSDSTemplate) == 0)
    {
        OnDCLoadSDSTemplateReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCGetSDSInfo) == 0)
    {
        OnDCGetSDSInfoReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCGetLatestSDSInfo) == 0)
    {
        OnDCGetLatestSDSInfoReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCAddSDSInfo) == 0)
    {
        OnDCAddSDSInfoReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetSDSInfo) == 0)
    {
        OnDCSetSDSInfoReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCDelSDSInfo) == 0)
    {
        OnDCDelSDSInfoReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCGetSDSReply) == 0)
    {
        OnDCGetSDSReplyReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadRecBrdCfg) == 0)
    {
        OnDCLoadRecBrdCfgReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadDepotInfo) == 0)
    {
        OnDCLoadDepotInfoReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetTrainDepotID) == 0)
    {
        OnDCSetTrainDepotIDReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetUESDSReply) == 0)
    {
        OnDCSetUESDSReplyReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetUESDSRecv) == 0)
    {
        OnDCSetUESDSRecvReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSaveNewSDS) == 0)
    {
        OnDCSaveNewSDSReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSaveNewSMS) == 0)
    {
        OnDCSaveNewSMSReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetOperator) == 0)
    {
        OnDCSetOperatorReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSaveGISInfo) == 0)
    {
        OnDCSaveGISInfo(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSaveMultiGroupInfo) == 0)
    {
        OnDCSaveMultiGroupInfo(szReq, szMsgType, ulSeqID, SubJson);
    }
    // rec cli
    else if (strcmp(szMsgType, MSG_DCGetRecFileListReq) == 0)
    {
        OnDCGetRecFileListReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCGetRecFileListRsp) == 0)
    {
        OnDCGetRecFileListRsp(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadDestInfo) == 0)
    {
        OnDCLoadDestInfoReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLoadSectionInfo) == 0)
    {
        OnDCLoadSectionInfoReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else
    {
        TaskLog(LOG_ERROR, "unknown DC msg: %s", szReq);
        goto error;
    }

    return;

error:
    return;
}

VOID TaskCfg::OnDCRegisterDISReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32      ulDCUserID;

    if (!Json.GetValue("DCUserID", &ulDCUserID))
    {
        TaskLog(LOG_ERROR, "invalid DCUserID in msg %s", MSG_DCRegisterDIS);
        return;
    }

    GJsonParam  Param;
    PID_T       stPrevPID = {0};
    PID_T       *pstCurrPID = GetMsgSender();
    UINT8       aucPrevAddr[4];
    UINT8       aucCurrAddr[4];
    UINT16      usPort;

    GosGetClientAddr(pstCurrPID->usInstID, aucCurrAddr, &usPort);

    if (GetDCPID(ulDCUserID, &stPrevPID) &&
        pstCurrPID->usInstID != stPrevPID.usInstID)
    {
        KickOffDC(&stPrevPID);
        if (GosGetClientAddr(stPrevPID.usInstID, aucPrevAddr, &usPort) )
        {
            TaskLog(LOG_FATAL, "dc %u logined from " IP_FMT " will be kicked off", ulDCUserID, IP_ARG(aucPrevAddr));
        }
    }

    SetDCLogin(ulDCUserID, pstCurrPID);

    TaskLog(LOG_INFO, "dc %u from " IP_FMT " registered with client id %u", ulDCUserID, IP_ARG(aucCurrAddr), pstCurrPID->usInstID);

    Param.Add("Result", "Succ");

    SendRsp(MSG_DCRegisterDIS, ulSeqID, Param);
}

VOID TaskCfg::OnDCLoadDCUserInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonTupleParam     JsonTupleParam;
    VECTOR<DC_USER_INFO_T>  vInfo;

    GetDCUserInfo(vInfo);
    //ConvertDCUserToJson(vInfo, JsonTupleParam);
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        DC_USER_INFO_T  &stUser = vInfo[i];
        GJsonParam      JsonParam;
        CHAR            acStationList[256];

        ConvertStationList(stUser.aulStationList, stUser.ulStationNum, acStationList);

        JsonParam.Add("UserID", stUser.ulUserID);
        JsonParam.Add("Name", stUser.acName);
        JsonParam.Add("DCType", stUser.ulDCType);
        JsonParam.Add("DepotID", stUser.ulDepotID);
        JsonParam.Add("StationList", acStationList);

        JsonTupleParam.Add(JsonParam);
    }

    SendLoadCfgDataRsp(MSG_DCLoadDCUserInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadDCOperatorInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonTupleParam     JsonTupleParam;
    VECTOR<DC_OPERATOR_INFO_T>  vInfo;

    GetDCOperatorInfo(vInfo);
    //ConvertDCOperatorToJson(vInfo, JsonTupleParam);
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        DC_OPERATOR_INFO_T  &stUser = vInfo[i];
        GJsonParam          JsonParam;

        JsonParam.Add("OperatorID", stUser.acOperatorID);
        JsonParam.Add("Name", stUser.acName);
        JsonParam.Add("Password", stUser.acPassword);
        JsonParam.Add("DCType", stUser.ulDCType);

        JsonTupleParam.Add(JsonParam);
    }

    SendLoadCfgDataRsp(MSG_DCLoadDCOperatorInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadTrainUEInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonTupleParam     JsonTupleParam;
    VECTOR<TRAIN_UE_INFO_T> vInfo;

    GetTrainUEInfo(vInfo);
    //ConvertTrainUEToJson(vInfo, JsonTupleParam);
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        TRAIN_UE_INFO_T &stUser = vInfo[i];
        GJsonParam      JsonParam;

        JsonParam.Add("UserID", stUser.ulUserID);
        JsonParam.Add("Name", stUser.acName);
        JsonParam.Add("LanAddr", stUser.acLanAddr);
        JsonParam.Add("TrainUnitID", stUser.ulTrainUnitID);

        JsonTupleParam.Add(JsonParam);
    }

    SendLoadCfgDataRsp(MSG_DCLoadTrainUEInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadStationUEInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonTupleParam     JsonTupleParam;
    VECTOR<STATION_UE_INFO_T>   vInfo;

    GetStationUEInfo(vInfo);
    //ConvertStationUEToJson(vInfo, JsonTupleParam);
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        STATION_UE_INFO_T   &stUser = vInfo[i];
        GJsonParam          JsonParam;

        JsonParam.Add("UserID", stUser.ulUserID);
        JsonParam.Add("Name", stUser.acName);
        JsonParam.Add("LanAddr", stUser.acLanAddr);
        JsonParam.Add("StationID", stUser.ulStationID);
        JsonParam.Add("DefaultGroupID", stUser.i64DefaultGroupID);

        JsonTupleParam.Add(JsonParam);
    }

    SendLoadCfgDataRsp(MSG_DCLoadStationUEInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadPortalUEInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonTupleParam     JsonTupleParam;
    VECTOR<PORTAL_UE_INFO_T>    vInfo;

    GetPortalUEInfo(vInfo);
    //ConvertPortalUEToJson(vInfo, JsonTupleParam);
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        PORTAL_UE_INFO_T    &stUser = vInfo[i];
        GJsonParam          JsonParam;

        JsonParam.Add("UserID", stUser.ulUserID);
        JsonParam.Add("Name", stUser.acName);
        JsonParam.Add("Type", stUser.ulType);
        JsonParam.Add("FuncType", stUser.ulFuncType);
        JsonParam.Add("StationID", stUser.ulStationID);
        JsonParam.Add("Lon", stUser.dLon);
        JsonParam.Add("Lat", stUser.dLat);

        JsonTupleParam.Add(JsonParam);
    }

    SendLoadCfgDataRsp(MSG_DCLoadPortalUEInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadGroupInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    VECTOR<GROUP_INFO_T>    vInfo;
    GJsonTupleParam         JsonTupleParam;

    if (!m_pDao->GetGroupInfo(vInfo) )
    {
        TaskLog(LOG_ERROR, "load group info failed");
        return;
    }

    //ConvertGroupToJson(vInfo, JsonTupleParam);
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        GROUP_INFO_T    &stGroup = vInfo[i];
        GJsonParam      JsonParam;

        JsonParam.Add("GroupID", stGroup.i64GroupID);
        JsonParam.Add("Name", stGroup.acGroupName);
        JsonParam.Add("Type", stGroup.ulGroupType);
        JsonParam.Add("FuncType", stGroup.ulFuncType);

        JsonTupleParam.Add(JsonParam);
    }

    SendLoadCfgDataRsp(MSG_DCLoadGroupInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadAPPCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonParam          JsonParam;
    LOCAL_CFG_T         &stLocalCfg = GetLocalCfg();
    APP_BASIC_CFG_T     stAPPBasicCfg;

    GetAPPBasicCfg(stAPPBasicCfg);

    JsonParam.AddIP("ATS1Addr", stLocalCfg.astATSAddr[0].aucIP);
    JsonParam.AddIP("ATS2Addr", stLocalCfg.astATSAddr[1].aucIP);
    JsonParam.Add("ATSValidPeriod", stLocalCfg.ulATSValidPeriod);
    JsonParam.Add("NTPAddr", stAPPBasicCfg.acNTPServerAddr);
    JsonParam.Add("NTPPort", stAPPBasicCfg.ulNTPServerPort);
    JsonParam.Add("NTPSyncPeriod", stAPPBasicCfg.ulNTPSyncPeriod);

    JsonParam.Add("MinTmpPatchGroupNum", stLocalCfg.ulDCMinTmpPatchGroupNum);
    JsonParam.Add("AllowReloadCfg", stLocalCfg.bAllowDCReloadCfg);
    JsonParam.Add("AllowHalfDuplexP2PCall", stLocalCfg.bAllowHalfDuplexP2PCall);
    JsonParam.Add("IPHVideoOnTcpMode", stLocalCfg.bIPHVideoTcpMode);
    JsonParam.Add("IPHVideoOpenTimeout", stLocalCfg.ulIPHVideoOpenTimeout);
    JsonParam.Add("IPHRecMergeMode", stLocalCfg.bIPHRecMergeMode);
    JsonParam.Add("DirectSendIPHAudio", stLocalCfg.bDirectSendIPHAudio);
    JsonParam.Add("IPHAudioSampleRate", stLocalCfg.ulIPHAudioSampleRate);
    JsonParam.Add("IPHAudioBitsPerSample", stLocalCfg.ulIPHAudioBitsPerSample);
    JsonParam.Add("DirectSendPAAudio", stLocalCfg.bDirectSendPAAudio);
    JsonParam.Add("PAAudioSampleRate", stLocalCfg.ulPAAudioSampleRate);
    JsonParam.Add("PAAudioBitsPerSample", stLocalCfg.ulPAAudioBitsPerSample);

    JsonParam.Add("PAAECMode", stLocalCfg.bPAAECMode);
    JsonParam.Add("PAMaxPkgSize", stLocalCfg.ulPAMaxPkgSize);
    JsonParam.Add("IPHAECMode", stLocalCfg.bIPHAECMode);
    JsonParam.Add("IPHMaxPkgSize", stLocalCfg.ulIPHMaxPkgSize);
 //   JsonParam.Add("PADelaySendTime", stLocalCfg.ulPADelaySendTime);
    JsonParam.Add("MaxIPHTalkTime", stLocalCfg.ulMaxIPHTalkTime);

    JsonParam.Add("ManageOneDepot", stLocalCfg.bManageOneDepot);
    JsonParam.Add("AllowVideoWall", stLocalCfg.bAllowVideoWall);
    JsonParam.Add("AllowGISSub", stLocalCfg.bAllowGISSub);
    JsonParam.Add("AllowP2PVideoCall", stLocalCfg.bAllowP2PVideoCall);
    JsonParam.Add("AutoCloseMulticastCall", stLocalCfg.bAutoCloseMulticastCall);
    JsonParam.Add("SendSMSByRealMsg", stLocalCfg.bSendSMSByRealMsg);
    JsonParam.Add("SimpleSMSMode", stLocalCfg.bSimpleSMSMode);

    SendLoadCfgDataRsp(MSG_DCLoadAPPCfg, ulSeqID, JsonParam);
}

VOID TaskCfg::OnDCLoadGroupCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonParam          JsonParam;
    GROUP_CFG_T         stGroupCfg;

    if (!GetGroupCfg(&stGroupCfg))
    {
        TaskLog(LOG_ERROR, "get group cfg failed");
        return;
    }

    JsonParam.Add("UpLineTrainGroupID", stGroupCfg.i64UpLineTrainGroupID);
    JsonParam.Add("DownLineTrainGroupID", stGroupCfg.i64DownLineTrainGroupID);
    JsonParam.Add("MainLineTrainGroupID", stGroupCfg.i64MainLineTrainGroupID);
    JsonParam.Add("UpLineTrainBrdGroupID", stGroupCfg.i64UpLineTrainBrdGroupID);
    JsonParam.Add("DownLineTrainBrdGroupID", stGroupCfg.i64DownLineTrainBrdGroupID);
    JsonParam.Add("MainLineTrainBrdGroupID", stGroupCfg.i64MainLineTrainBrdGroupID);
    JsonParam.Add("AllStationGroupID", stGroupCfg.i64AllStationGroupID);
    JsonParam.Add("StartTempDynamicGroupID", stGroupCfg.i64StartTempDynamicGroupID);
    JsonParam.Add("EndTempDynamicGroupID", stGroupCfg.i64EndTempDynamicGroupID);
    JsonParam.Add("DriverGroupID", stGroupCfg.i64DriverGroupID);
    JsonParam.Add("DepotDriverGroupID", stGroupCfg.i64DepotDriverGroupID);

    SendLoadCfgDataRsp(MSG_DCLoadGroupCfg, ulSeqID, JsonParam);
}

VOID TaskCfg::OnDCLoadAutoJoinGroupInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    VECTOR<INT64>       vInfo;
    GJsonTupleParam     JsonTupleParam;

    if (!m_pDao->GetAutoJoinGroup(m_ulDCUserID, vInfo) )
    {
        TaskLog(LOG_ERROR, "load AutoJoinGroup info failed");
        return;
    }

    ConvertGroupIDToJson(vInfo, JsonTupleParam);

    SendLoadCfgDataRsp(MSG_DCLoadAutoJoinGroupInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadSubGroupInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    VECTOR<INT64>       vInfo;
    GJsonTupleParam     JsonTupleParam;

    if (!m_pDao->GetSubGroup(m_ulDCUserID, vInfo) )
    {
        TaskLog(LOG_ERROR, "load sub group info failed");
        return;
    }

    ConvertGroupIDToJson(vInfo, JsonTupleParam);

    SendLoadCfgDataRsp(MSG_DCLoadSubGroupInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadSubVideoGroupInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    VECTOR<INT64>       vInfo;
    GJsonTupleParam     JsonTupleParam;

    if (!m_pDao->GetSubVideoGroup(m_ulDCUserID, vInfo) )
    {
        TaskLog(LOG_ERROR, "load sub video group info failed");
        return;
    }

    ConvertGroupIDToJson(vInfo, JsonTupleParam);

    SendLoadCfgDataRsp(MSG_DCLoadSubVideoGroupInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadSubUserInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    VECTOR<UINT32>      vInfo;
    GJsonTupleParam     JsonTupleParam;

    if (!m_pDao->GetSubUser(m_ulDCUserID, vInfo) )
    {
        TaskLog(LOG_ERROR, "load sub user info failed");
        return;
    }

    ConvertUserIDToJson(vInfo, JsonTupleParam);

    SendLoadCfgDataRsp(MSG_DCLoadSubUserInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadSubVideoUserInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    VECTOR<UINT32>      vInfo;
    GJsonTupleParam     JsonTupleParam;

    if (!m_pDao->GetSubVideoUser(m_ulDCUserID, vInfo) )
    {
        TaskLog(LOG_ERROR, "load sub video user info failed");
        return;
    }

    ConvertUserIDToJson(vInfo, JsonTupleParam);

    SendLoadCfgDataRsp(MSG_DCLoadSubVideoUserInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadBrdZoneInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    VECTOR<BRD_ZONE_INFO_T> vInfo;
    GJsonTupleParam         TupleParam;

    if (!m_pDao->GetBrdZoneInfo(vInfo))
    {
        TaskLog(LOG_ERROR, "query BrdZoneInfo failed");
        return;
    }

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        GJsonParam  SubParam;

        SubParam.Add("StationID", vInfo[i].ulStationID);
        SubParam.Add("ZoneID", vInfo[i].ulZoneID);
        SubParam.Add("ZoneName", vInfo[i].acZoneName);
        SubParam.Add("GroupID", vInfo[i].i64GroupID);
        TupleParam.Add(SubParam);
    }

    SendLoadCfgDataRsp(MSG_DCLoadBrdZoneInfo, ulSeqID, TupleParam);
}

VOID TaskCfg::OnDCLoadTrainInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    VECTOR<TRAIN_INFO_T>    vInfo;
    GJsonTupleParam         JsonTupleParam;

    GetTrainInfo(vInfo);

    ConvertTrainToJson(vInfo, JsonTupleParam);

    SendLoadCfgDataRsp(MSG_DCLoadTrainInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadStationInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    VECTOR<STATION_INFO_T>  vInfo;
    GJsonTupleParam         JsonTupleParam;

    GetStationInfo(vInfo);
    //ConvertStationToJson(vInfo, JsonTupleParam);
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        STATION_INFO_T  &stStation = vInfo[i];
        GJsonParam      JsonParam;

        JsonParam.Add("StationIndex", stStation.ulStationIndex);
        JsonParam.Add("StationID", stStation.ulStationID);
        JsonParam.Add("StationName", stStation.acStationName);
        JsonParam.Add("StationType", stStation.ulStationType);
        JsonParam.Add("DepotID", stStation.ulDepotID);
        JsonParam.Add("InterLockStationID", stStation.ulInterLockStationID);
        JsonParam.Add("StationGroupID", stStation.i64StationGroupID);

        JsonTupleParam.Add(JsonParam);
    }

    SendLoadCfgDataRsp(MSG_DCLoadStationInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadDestInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    GJsonTupleParam         JsonTupleParam;
    VECTOR<DEST_INFO_T>     vInfo;

    GetDestInfo(vInfo);

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        GJsonParam              JsonParam;

        JsonParam.Add("DestID", vInfo[i].ulDestID);
        JsonParam.Add("DestName", vInfo[i].acDestName);
        JsonParam.Add("StationID", vInfo[i].ulStationID);

        JsonTupleParam.Add(JsonParam);
    }

    SendLoadCfgDataRsp(MSG_DCLoadDestInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadSectionInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    GJsonTupleParam         JsonTupleParam;
    VECTOR<SECTION_INFO_T>  vInfo;

    GetSectionInfo(FALSE, vInfo);

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        GJsonParam              JsonParam;

        JsonParam.Add("ID", vInfo[i].ulSectionID);
        JsonParam.Add("Name", vInfo[i].acSectionName);
        JsonParam.Add("StationID", vInfo[i].ulStationID);
        JsonParam.Add("Direction", vInfo[i].ulDirection);
        JsonParam.Add("InDepotTrack", vInfo[i].bInDepotTrack);
        JsonParam.Add("x1", vInfo[i].x1);

        if (vInfo[i].x1 >= 0)
        {
            JsonParam.Add("y1", vInfo[i].y1);
            JsonParam.Add("x2", vInfo[i].x2);
            JsonParam.Add("y2", vInfo[i].y2);
        }

        JsonTupleParam.Add(JsonParam);
    }

    SendLoadCfgDataRsp(MSG_DCLoadSectionInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadMultiGroupInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    VECTOR<MULTI_GROUP_INFO_T>  vInfo;
    GJsonTupleParam         JsonTupleParam;

    GetMultiGroupInfo(vInfo);
    //ConvertMultiGroupToJson(vInfo, JsonTupleParam);
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        MULTI_GROUP_INFO_T      &stInfo = vInfo[i];
        GJsonParam              JsonParam;

        JsonParam.Add("DCUserID", stInfo.ulDCUserID);
        JsonParam.Add("GroupID", stInfo.i64GroupID);
        JsonParam.Add("GroupName", stInfo.acGroupName);
        JsonParam.Add("GroupMember", stInfo.acGroupMember);

        JsonTupleParam.Add(JsonParam);
    }

    SendLoadCfgDataRsp(MSG_DCLoadMultiGroupInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadIPHTalkInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32                      ulStartTime;
    UINT32                      ulEndTime;
    GJsonTupleParam             JsonTupleParam;

    if (!Json.GetValue("StartTime", &ulStartTime))
    {
        TaskLog(LOG_ERROR, "invalid StartTime in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("EndTime", &ulEndTime))
    {
        TaskLog(LOG_ERROR, "invalid EndTime in msg %s", szReq);
        return;
    }

    if (!m_pDao->GetIPHTalkInfo(ulStartTime, ulEndTime, JsonTupleParam))
    {
        TaskLog(LOG_ERROR, "query IPH talk info failed");
        return;
    }

    SendLoadCfgDataRsp(MSG_DCLoadIPHTalkInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadOperLogInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32  ulDCUserID = INVALID_USER_ID;
    UINT32  ulMaxNum = 1000;
    UINT32  ulStartTime;
    UINT32  ulEndTime;
    GJsonTupleParam     JsonTupleParam;

    if (!Json.GetValue("StartTime", &ulStartTime))
    {
        TaskLog(LOG_ERROR, "invalid StartTime in msg %s", MSG_DCLoadOperLogInfo);
        return;
    }

    if (!Json.GetValue("EndTime", &ulEndTime))
    {
        TaskLog(LOG_ERROR, "invalid EndTime in msg %s", MSG_DCLoadOperLogInfo);
        return;
    }

    if (!Json.GetValue("DCUserID", &ulDCUserID))
    {
        TaskLog(LOG_ERROR, "invalid DCUserID in msg %s", MSG_DCLoadOperLogInfo);
        return;
    }

    if (!m_pDao->GetOperLogInfo(ulStartTime, ulEndTime, ulDCUserID, ulMaxNum, JsonTupleParam))
    {
        TaskLog(LOG_ERROR, "load p2p video call info of %u failed", ulDCUserID);
        return;
    }

    SendLoadCfgDataRsp(MSG_DCLoadOperLogInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadP2PCallInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32  ulStartTime;
    UINT32  ulEndTime;
    UINT32  ulCallType;
    UINT32  ulUserID;
    UINT32  ulMaxNum = 1000;
    GJsonTupleParam     JsonTupleParam;

    if (!Json.GetValue("StartTime", &ulStartTime))
    {
        TaskLog(LOG_ERROR, "invalid StartTime in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("EndTime", &ulEndTime))
    {
        TaskLog(LOG_ERROR, "invalid EndTime in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("CallType", &ulCallType))
    {
        TaskLog(LOG_ERROR, "invalid CallType in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("UserID", &ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szReq);
        return;
    }

    if (!m_pDao->GetP2PCallInfo(ulStartTime, ulEndTime, ulCallType, ulUserID, ulMaxNum, JsonTupleParam))
    {
        TaskLog(LOG_ERROR, "load p2p call info of %u failed", ulUserID);
        return;
    }

    SendLoadCfgDataRsp(MSG_DCLoadP2PCallInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadGroupCallInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32  ulDCUserID;
    INT64   i64GroupID;
    UINT32  ulStartTime;
    UINT32  ulEndTime;
    UINT32  ulCallType;
    UINT32  ulMaxNum = 1000;
    GJsonTupleParam     JsonTupleParam;

    if (!Json.GetValue("StartTime", &ulStartTime))
    {
        TaskLog(LOG_ERROR, "invalid StartTime in msg %s", MSG_DCLoadGroupCallInfo);
        return;
    }

    if (!Json.GetValue("EndTime", &ulEndTime))
    {
        TaskLog(LOG_ERROR, "invalid EndTime in msg %s", MSG_DCLoadGroupCallInfo);
        return;
    }

    if (!Json.GetValue("CallType", &ulCallType))
    {
        TaskLog(LOG_ERROR, "invalid CallType in msg %s", MSG_DCLoadGroupCallInfo);
        return;
    }

    if (!Json.GetValue("DCUserID", &ulDCUserID))
    {
        TaskLog(LOG_ERROR, "invalid DCUserID in msg %s", MSG_DCLoadGroupCallInfo);
        return;
    }

    if (!Json.GetValue("GroupID", &i64GroupID))
    {
        TaskLog(LOG_ERROR, "invalid GroupID in msg %s", MSG_DCLoadGroupCallInfo);
        return;
    }

    if (!m_pDao->GetGroupCallInfo(ulStartTime, ulEndTime, ulCallType, ulDCUserID, i64GroupID, ulMaxNum, JsonTupleParam))
    {
        TaskLog(LOG_ERROR, "load group call info of %u failed", ulDCUserID);
        return;
    }

    SendLoadCfgDataRsp(MSG_DCLoadGroupCallInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadReqCallInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32  ulReqCallType;
    UINT32  ulStartTime;
    UINT32  ulEndTime;
    UINT32  ulMaxNum = 1000;
    GJsonTupleParam     JsonTupleParam;

    if (!Json.GetValue("StartTime", &ulStartTime))
    {
        TaskLog(LOG_ERROR, "invalid StartTime in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("EndTime", &ulEndTime))
    {
        TaskLog(LOG_ERROR, "invalid EndTime in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("ReqCallType", &ulReqCallType))
    {
        TaskLog(LOG_ERROR, "invalid ReqCallType in msg %s", szReq);
        return;
    }

    if (!m_pDao->GetReqCallInfo(ulStartTime, ulEndTime, ulReqCallType, ulMaxNum, JsonTupleParam))
    {
        TaskLog(LOG_ERROR, "load req call info failed");
        return;
    }

    SendLoadCfgDataRsp(MSG_DCLoadReqCallInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadRecBrdInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32  ulStartTime;
    UINT32  ulEndTime;
    UINT32  ulMaxNum = 1000;
    GJsonTupleParam     JsonTupleParam;

    if (!Json.GetValue("StartTime", &ulStartTime))
    {
        TaskLog(LOG_ERROR, "invalid StartTime in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("EndTime", &ulEndTime))
    {
        TaskLog(LOG_ERROR, "invalid EndTime in msg %s", szReq);
        return;
    }

    if (!m_pDao->GetRecBrdInfo(ulStartTime, ulEndTime, ulMaxNum, JsonTupleParam))
    {
        TaskLog(LOG_ERROR, "load rec brd info failed");
        return;
    }

    SendLoadCfgDataRsp(MSG_DCLoadRecBrdInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadRealBrdInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32  ulStartTime;
    UINT32  ulEndTime;
    UINT32  ulMaxNum = 1000;
    GJsonTupleParam     JsonTupleParam;

    if (!Json.GetValue("StartTime", &ulStartTime))
    {
        TaskLog(LOG_ERROR, "invalid StartTime in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("EndTime", &ulEndTime))
    {
        TaskLog(LOG_ERROR, "invalid EndTime in msg %s", szReq);
        return;
    }

    if (!m_pDao->GetRealBrdInfo(ulStartTime, ulEndTime, ulMaxNum, JsonTupleParam))
    {
        TaskLog(LOG_ERROR, "load rec brd info failed");
        return;
    }

    SendLoadCfgDataRsp(MSG_DCLoadRealBrdInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadPredefinedSMSReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonTupleParam     JsonTupleParam;
    VECTOR<PREDEFINED_SMS_INFO_T>   vInfo;

    if (!m_pDao->GetPredefinedSMS(vInfo))
    {
        TaskLog(LOG_ERROR, "load predefined sms failed");
        return;
    }

    //ConvertPredefinedSMSToJson(vInfo, JsonTupleParam);
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        PREDEFINED_SMS_INFO_T   &stInfo = vInfo[i];
        GJsonParam              JsonParam;

        JsonParam.Add("SMSID", stInfo.ulSMSID);
        JsonParam.Add("SDSType", stInfo.ulSDSType);
        JsonParam.Add("SMSTemplateType", stInfo.ulSMSTemplateType);
        JsonParam.Add("SMSInfo", stInfo.acSMSInfo);

        JsonTupleParam.Add(JsonParam);
    }

    SendLoadCfgDataRsp(MSG_DCLoadPredefinedSMS, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadSDSTemplateReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonTupleParam     JsonTupleParam;
    VECTOR<SDS_TEMPLATE_T>  vInfo;

    if (!m_pDao->GetSDSTemplate(vInfo))
    {
        TaskLog(LOG_ERROR, "load sds template failed");
        return;
    }

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        GJsonParam              JsonParam;

        if (GetLocalCfg().bSimpleSMSMode)
        {
            if (vInfo[i].ulSDSType != SDS_TYPE_STATUS &&
                vInfo[i].ulSDSType != SDS_TYPE_EMERG_STATUS &&
                vInfo[i].ulSDSType != SDS_TYPE_NORMAL )
            {
                continue;
            }
        }

        JsonParam.Add("TemplateID", vInfo[i].ulTemplateID);
        JsonParam.Add("SDSType", vInfo[i].ulSDSType);
        JsonParam.Add("Title", vInfo[i].acTitle);
        JsonParam.Add("SDSInfo", vInfo[i].acSDSInfo);

        JsonTupleParam.Add(JsonParam);
    }

    SendLoadCfgDataRsp(MSG_DCLoadSDSTemplate, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCSaveGroupStatusReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    UINT32  ulDCUserID = m_ulDCUserID;
    INT64   i64GroupID;
    UINT32  ulGroupStatus;

    if (!Json.GetValue("GroupID", &i64GroupID))
    {
        TaskLog(LOG_ERROR, "invalid GroupID in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("GroupStatus", &ulGroupStatus))
    {
        TaskLog(LOG_ERROR, "invalid GroupStatus in msg %s", szMsgType);
        return;
    }

    if (!m_pDao->SaveGroupStatus(ulDCUserID, i64GroupID, ulGroupStatus))
    {
        TaskLog(LOG_ERROR, "save status of group " FORMAT_I64 " failed", i64GroupID);
        return;
    }

    GJsonParam  Param;

    Param.Add("DCUserID", ulDCUserID);
    Param.Add("GroupID", i64GroupID);
    Param.Add("GroupStatus", ulGroupStatus);
    Param.Add("Result", "true");

    SendRsp(szMsgType, ulSeqID, Param);

    SetCfgDataVer(m_ulLoadAutoJoinGroupInfo);
}

VOID TaskCfg::OnDCSaveSubGroupInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    CHAR    *szValue;
    INT64   i64GroupID;
    GJson   SubJson;
    BOOL    bRet = FALSE;
    VECTOR<INT64>   vDelGroup;
    VECTOR<INT64>   vAddGroup;

    // Del User
    szValue = Json.GetValue("DelGroup");
    if (!szValue)
    {
        TaskLog(LOG_ERROR, "invalid DelGroup in msg %s", szReq);
        return;
    }

    SubJson.Parse(szValue);

    VECTOR<GJson*>  &vDelSubJson = SubJson.GetSubJson();

    for (UINT32 i=0; i<vDelSubJson.size(); i++)
    {
        if (!vDelSubJson[i]->GetValue("GroupID", &i64GroupID))
        {
            TaskLog(LOG_ERROR, "invalid GroupID in msg %s", szReq);
            return;
        }

        vDelGroup.push_back(i64GroupID);
    }

    // Add User
    szValue = Json.GetValue("AddGroup");
    if (!szValue)
    {
        TaskLog(LOG_ERROR, "invalid AddGroup in msg %s", szReq);
        return;
    }

    SubJson.Parse(szValue);

    VECTOR<GJson*>  &vAddSubJson = SubJson.GetSubJson();

    for (UINT32 i=0; i<vAddSubJson.size(); i++)
    {
        if (!vAddSubJson[i]->GetValue("GroupID", &i64GroupID))
        {
            TaskLog(LOG_ERROR, "invalid GroupID in msg %s", szReq);
            return;
        }

        vAddGroup.push_back(i64GroupID);
    }

    if (!m_pDao->SaveSubGroup(m_ulDCUserID, vDelGroup, vAddGroup))
    {
        TaskLog(LOG_ERROR, "save sub group of %u failed", m_ulDCUserID);
        goto end;
    }

    bRet = TRUE;

end:
    GJsonParam  Param;

    Param.Add("Result", bRet?"Succ":"Fail");

    SendRsp(szMsgType, ulSeqID, Param);

    if (bRet)
    {
        SetCfgDataVer(m_ulLoadSubGroupInfo);
    }
}

VOID TaskCfg::OnDCSaveSubVideoGroupInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    CHAR    *szValue;
    INT64   i64GroupID;
    GJson   SubJson;
    BOOL    bRet = FALSE;
    VECTOR<INT64>   vDelGroup;
    VECTOR<INT64>   vAddGroup;

    // Del User
    szValue = Json.GetValue("DelGroup");
    if (!szValue)
    {
        TaskLog(LOG_ERROR, "invalid DelGroup in msg %s", szReq);
        return;
    }

    SubJson.Parse(szValue);

    VECTOR<GJson*>  &vDelSubJson = SubJson.GetSubJson();

    for (UINT32 i=0; i<vDelSubJson.size(); i++)
    {
        if (!vDelSubJson[i]->GetValue("GroupID", &i64GroupID))
        {
            TaskLog(LOG_ERROR, "invalid GroupID in msg %s", szReq);
            return;
        }

        vDelGroup.push_back(i64GroupID);
    }

    // Add User
    szValue = Json.GetValue("AddGroup");
    if (!szValue)
    {
        TaskLog(LOG_ERROR, "invalid AddGroup in msg %s", szReq);
        return;
    }

    SubJson.Parse(szValue);

    VECTOR<GJson*>  &vAddSubJson = SubJson.GetSubJson();

    for (UINT32 i=0; i<vAddSubJson.size(); i++)
    {
        if (!vAddSubJson[i]->GetValue("GroupID", &i64GroupID))
        {
            TaskLog(LOG_ERROR, "invalid GroupID in msg %s", szReq);
            return;
        }

        vAddGroup.push_back(i64GroupID);
    }

    if (!m_pDao->SaveSubVideoGroup(m_ulDCUserID, vDelGroup, vAddGroup))
    {
        TaskLog(LOG_ERROR, "save sub group of %u failed", m_ulDCUserID);
        goto end;
    }

    bRet = TRUE;

end:
    GJsonParam  Param;

    Param.Add("Result", bRet?"Succ":"Fail");

    SendRsp(szMsgType, ulSeqID, Param);

    if (bRet)
    {
        SetCfgDataVer(m_ulLoadSubVideoGroupInfo);
    }
}

VOID TaskCfg::OnDCSaveSubUserInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    CHAR    *szValue;
    UINT32  ulUserID;
    GJson   SubJson;
    BOOL    bRet = FALSE;
    VECTOR<UINT32>  vDelUser;
    VECTOR<UINT32>  vAddUser;

    // Del User
    szValue = Json.GetValue("DelUser");
    if (!szValue)
    {
        TaskLog(LOG_ERROR, "invalid DelUser in msg %s", szReq);
        return;
    }

    SubJson.Parse(szValue);

    VECTOR<GJson*>  &vDelSubJson = SubJson.GetSubJson();

    for (UINT32 i=0; i<vDelSubJson.size(); i++)
    {
        if (!vDelSubJson[i]->GetValue("UserID", &ulUserID))
        {
            TaskLog(LOG_ERROR, "invalid UserID in msg %s", szReq);
            return;
        }

        vDelUser.push_back(ulUserID);
    }

    // Add User
    szValue = Json.GetValue("AddUser");
    if (!szValue)
    {
        TaskLog(LOG_ERROR, "invalid AddUser in msg %s", szReq);
        return;
    }

    SubJson.Parse(szValue);

    VECTOR<GJson*>  &vAddSubJson = SubJson.GetSubJson();

    for (UINT32 i=0; i<vAddSubJson.size(); i++)
    {
        if (!vAddSubJson[i]->GetValue("UserID", &ulUserID))
        {
            TaskLog(LOG_ERROR, "invalid UserID in msg %s", szReq);
            return;
        }

        vAddUser.push_back(ulUserID);
    }

    if (!m_pDao->SaveSubUser(m_ulDCUserID, vDelUser, vAddUser))
    {
        TaskLog(LOG_ERROR, "save sub user of %u failed", m_ulDCUserID);
        goto end;
    }

    bRet = TRUE;

end:
    GJsonParam  Param;

    Param.Add("Result", bRet?"Succ":"Fail");

    SendRsp(szMsgType, ulSeqID, Param);

    if (bRet)
    {
        SetCfgDataVer(m_ulLoadSubUserInfo);
    }
}

VOID TaskCfg::OnDCSaveSubVideoUserInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    CHAR    *szValue;
    UINT32  ulUserID;
    GJson   SubJson;
    BOOL    bRet = FALSE;
    VECTOR<UINT32>  vDelUser;
    VECTOR<UINT32>  vAddUser;

    // Del User
    szValue = Json.GetValue("DelUser");
    if (!szValue)
    {
        TaskLog(LOG_ERROR, "invalid DelUser in msg %s", szReq);
        return;
    }

    SubJson.Parse(szValue);

    VECTOR<GJson*>  &vDelSubJson = SubJson.GetSubJson();

    for (UINT32 i=0; i<vDelSubJson.size(); i++)
    {
        if (!vDelSubJson[i]->GetValue("UserID", &ulUserID))
        {
            TaskLog(LOG_ERROR, "invalid UserID in msg %s", szReq);
            return;
        }

        vDelUser.push_back(ulUserID);
    }

    // Add User
    szValue = Json.GetValue("AddUser");
    if (!szValue)
    {
        TaskLog(LOG_ERROR, "invalid AddUser in msg %s", szReq);
        return;
    }

    SubJson.Parse(szValue);

    VECTOR<GJson*>  &vAddSubJson = SubJson.GetSubJson();

    for (UINT32 i=0; i<vAddSubJson.size(); i++)
    {
        if (!vAddSubJson[i]->GetValue("UserID", &ulUserID))
        {
            TaskLog(LOG_ERROR, "invalid UserID in msg %s", szReq);
            return;
        }

        vAddUser.push_back(ulUserID);
    }

    if (!m_pDao->SaveSubVideoUser(m_ulDCUserID, vDelUser, vAddUser))
    {
        TaskLog(LOG_ERROR, "save sub video user of %u failed", m_ulDCUserID);
        goto end;
    }

    bRet = TRUE;

end:
    GJsonParam  Param;

    Param.Add("Result", bRet?"Succ":"Fail");

    SendRsp(szMsgType, ulSeqID, Param);

    if (bRet)
    {
        SetCfgDataVer(m_ulLoadSubVideoUserInfo);
    }
}

VOID TaskCfg::OnDCSaveP2PCallInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    P2P_CALL_INFO_T     stInfo = {0};

    stInfo.ulSeqID = ulSeqID;

    if (!Json.GetValue("CallType", &stInfo.ulCallType))
    {
        TaskLog(LOG_ERROR, "invalid CallType in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("Caller", &stInfo.ulCaller))
    {
        TaskLog(LOG_ERROR, "invalid Caller in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("Callee", &stInfo.ulCallee))
    {
        TaskLog(LOG_ERROR, "invalid Callee in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("Time", &stInfo.ulTime))
    {
        TaskLog(LOG_ERROR, "invalid Time in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("Status", &stInfo.ulStatus))
    {
        TaskLog(LOG_ERROR, "invalid Status in msg %s", szMsgType);
        return;
    }

    if (!m_pDao->SaveP2PCallInfo(stInfo))
    {
        TaskLog(LOG_ERROR, "save p2p call of %u failed", stInfo.ulCaller);
        return;
    }

    SendConfirmSaveRsp(ulSeqID);
}

VOID TaskCfg::OnDCSaveGroupCallInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    GROUP_CALL_INFO_T   stInfo = {0};

    stInfo.ulDCUserID = m_ulDCUserID;

    if (!Json.GetValue("GroupID", &stInfo.i64GroupID))
    {
        TaskLog(LOG_ERROR, "invalid GroupID in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("CallType", &stInfo.ulCallType))
    {
        TaskLog(LOG_ERROR, "invalid CallType in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("Speaker", &stInfo.ulSpeaker))
    {
        TaskLog(LOG_ERROR, "invalid Speaker in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("Time", &stInfo.ulTime))
    {
        TaskLog(LOG_ERROR, "invalid Time in msg %s", szMsgType);
        return;
    }

    stInfo.ulSeqID = ulSeqID;

    if (!m_pDao->SaveGroupCallInfo(stInfo))
    {
        TaskLog(LOG_ERROR, "save group call of " FORMAT_I64 " failed", stInfo.i64GroupID);
        return;
    }

    SendConfirmSaveRsp(ulSeqID);
}

VOID TaskCfg::OnDCSaveP2PVideoCallInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    P2P_CALL_INFO_T     stInfo = {0};

    stInfo.ulSeqID = ulSeqID;

    if (!Json.GetValue("CallType", &stInfo.ulCallType))
    {
        TaskLog(LOG_ERROR, "invalid CallType in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("Caller", &stInfo.ulCaller))
    {
        TaskLog(LOG_ERROR, "invalid Caller in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("Callee", &stInfo.ulCallee))
    {
        TaskLog(LOG_ERROR, "invalid Callee in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("Time", &stInfo.ulTime))
    {
        TaskLog(LOG_ERROR, "invalid Time in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("Status", &stInfo.ulStatus))
    {
        TaskLog(LOG_ERROR, "invalid Status in msg %s", szMsgType);
        return;
    }

    if (!m_pDao->SaveP2PCallInfo(stInfo))
    {
        TaskLog(LOG_ERROR, "save p2p video call info failed", stInfo.ulCaller);
        return;
    }

    SendConfirmSaveRsp(ulSeqID);
}

VOID TaskCfg::SendConfirmSaveRsp(UINT32 ulSeqID)
{
    GJsonParam  Param;

    Param.Add("Result", "true");
    Param.Add("DCUserID", m_ulDCUserID);

    SendClient(GetMsgSender(), EV_DC_DIS_RSP, MSG_DCConfirmSave, ulSeqID, Param);
}

std::string ZipString(const CHAR *szStr)
{
    HANDLE      hZip = NULL;
    CHAR        *szZipped = NULL;
    CHAR        *szBase64 = NULL;
    std::string str;
    UINT32      ulZippedLen;
    INT32       iBase64Len;

    szZipped = (CHAR*)calloc(1, 64*3*1024);
    if (!szZipped)
    {
        return str;
    }

    szBase64 = szZipped + 64*1024;

    hZip = gos_comp_init();
    gos_comp(hZip, szStr, strlen(szStr)+1, szZipped, &ulZippedLen);

    if (ulZippedLen >= 64*1024)
    {
        goto end;
    }

    iBase64Len = 2*64*1024;
    if (!gos_base64_encode((UINT8*)szZipped, ulZippedLen, (UINT8*)szBase64, &iBase64Len))
    {
        goto end;
    }

    str = szBase64;

end:
    gos_comp_free(hZip);
    free(szZipped);

    return str;
}

BOOL TaskCfg::SendLoadCfgDataRsp(const CHAR *szMsgName, UINT32 ulSeqID, CHAR *szJsonMsg)
{
    GJsonParam      ReqParam;
    GJsonParam      CfgDataParam;
    UINT32          *pulCfgVer = NULL;

    if (m_mCfgDataVer.find(szMsgName) != m_mCfgDataVer.end())
    {
        pulCfgVer = m_mCfgDataVer[szMsgName];
    }

    CfgDataParam.Add("CfgType", szMsgName);
    CfgDataParam.Add("CfgVer",  pulCfgVer?*pulCfgVer:0);
    CfgDataParam.Add("CfgData", szJsonMsg, FALSE);

    ReqParam.Add("MsgName", szMsgName);
    ReqParam.Add("MsgSeqID", ulSeqID);

    CHAR            *szCfgData = CfgDataParam.GetString();
    BOOL            bZipped = FALSE;

    // 大数据包需要压缩后发送
    if (strlen(szCfgData) > GosGetSafeMsgSize())
    {
        bZipped = TRUE;
    }

    if (bZipped)
    {
        ReqParam.Add("Zipped", "true");

        std::string strMsgInfo = ZipString(szCfgData);

        ReqParam.Add("MsgInfo", strMsgInfo);
    }
    else
    {
        ReqParam.Add("MsgInfo", CfgDataParam);
    }

    CHAR    *szMsg = ReqParam.GetString();

    if (!SendRsp(szMsg))
    {
        TaskLog(LOG_ERROR, "send msg %s failed", szMsgName);
        return FALSE;
    }

    return TRUE;
}

VOID TaskCfg::OnDCSaveRealBrdInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    REAL_BRD_INFO_T     stInfo = {0};

    stInfo.ulDCUserID = m_ulDCUserID;
    if (!Json.GetValue("StartTime", &stInfo.ulStartTime))
    {
        TaskLog(LOG_ERROR, "invalid StartTime in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("EndTime", &stInfo.ulEndTime))
    {
        TaskLog(LOG_ERROR, "invalid EndTime in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("TrainList", stInfo.acTrainList, sizeof(stInfo.acTrainList)))
    {
        TaskLog(LOG_ERROR, "invalid TrainList in msg %s", szReq);
        return;
    }

    if (!m_pDao->SaveRealBrdInfo(stInfo))
    {
        TaskLog(LOG_ERROR, "save real brd info of %u failed", stInfo.ulDCUserID);
        return;
    }

    SendConfirmSaveRsp(ulSeqID);
}

VOID TaskCfg::OnDCSaveGroupInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    GROUP_INFO_T        stInfo = {0};

    stInfo.ulGroupType = STATIC_GROUP;
    if (!Json.GetValue("GroupID", &stInfo.i64GroupID))
    {
        TaskLog(LOG_ERROR, "invalid GroupID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("FuncType", &stInfo.ulFuncType))
    {
        TaskLog(LOG_ERROR, "invalid FuncType in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("GroupName", stInfo.acGroupName, sizeof(stInfo.acGroupName)))
    {
        TaskLog(LOG_ERROR, "invalid GroupName in msg %s", szReq);
        return;
    }

    if (!m_pDao->SaveGroupInfo(&stInfo))
    {
        TaskLog(LOG_ERROR, "save group info of  " FORMAT_I64 " failed", stInfo.i64GroupID);
        return;
    }

    SendConfirmSaveRsp(ulSeqID);
    SetCfgDataVer(m_ulLoadGroupInfo);
}

VOID TaskCfg::OnDCGetSDSInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    GJsonTupleParam     JsonTupleParam;
    VECTOR<SDS_INFO_T>  vInfo;
    UINT32              ulStartTime;
    UINT32              ulEndTime;
    UINT32              ulSDSType;
    CHAR                *szSendUserName;
    CHAR                *szKeyword;
    UINT32              ulMaxNum = 300;

    if (!Json.GetValue("StartTime", &ulStartTime))
    {
        TaskLog(LOG_ERROR, "invalid StartTime in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("EndTime", &ulEndTime))
    {
        TaskLog(LOG_ERROR, "invalid EndTime in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("SDSType", &ulSDSType))
    {
        TaskLog(LOG_ERROR, "invalid SDSType in msg %s", szMsgType);
        return;
    }

    szSendUserName = Json.GetValue("SendUserName");

    szKeyword = Json.GetValue("Keyword");

    if (!m_pDao->GetSDSInfo(ulStartTime, ulEndTime, ulSDSType, szSendUserName, szKeyword, ulMaxNum, vInfo))
    {
        TaskLog(LOG_ERROR, "load sds info failed");
        return;
    }

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        GJsonParam              JsonParam;

        JsonParam.Add("SDSID", vInfo[i].ulSDSID);
        JsonParam.Add("SendUserID", vInfo[i].ulSendUserID);
        JsonParam.Add("SendUserName", vInfo[i].acSendUserName);
        JsonParam.Add("SendTime", vInfo[i].ulSendTime);
        JsonParam.Add("SDSIndex", vInfo[i].ulSDSIndex);
        JsonParam.Add("SDSType", vInfo[i].ulSDSType);
        JsonParam.Add("SDSInfo", vInfo[i].acSDSInfo);
        JsonParam.Add("NeedReply", vInfo[i].bNeedReply);
        JsonParam.Add("RecvUserNum", vInfo[i].ulRecvUserNum);
        JsonParam.Add("RecvSuccUserNum", vInfo[i].ulRecvSuccUserNum);
        JsonParam.Add("ReplyUserNum", vInfo[i].ulReplyUserNum);
        JsonParam.Add("File", vInfo[i].acFile);

        JsonTupleParam.Add(JsonParam);
        if (JsonTupleParam.GetStringSize() >= GetSafeQueryRecLen())
        {
            break;
        }
    }

    SendLoadCfgDataRsp(MSG_DCGetSDSInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCGetLatestSDSInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    GJsonTupleParam     JsonTupleParam;
    VECTOR<SDS_INFO_T>  vInfo;
    UINT32              ulMaxNum = 300;

    if (!m_pDao->GetLatestSDSInfo(ulMaxNum, m_ulDCUserID, vInfo))
    {
        TaskLog(LOG_ERROR, "load latest sds info failed");
        return;
    }

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        GJsonParam              JsonParam;

        JsonParam.Add("SDSID", vInfo[i].ulSDSID);
        JsonParam.Add("SendUserID", vInfo[i].ulSendUserID);
        JsonParam.Add("SendUserName", vInfo[i].acSendUserName);
        JsonParam.Add("SendTime", vInfo[i].ulSendTime);
        JsonParam.Add("SDSIndex", vInfo[i].ulSDSIndex);
        JsonParam.Add("SDSType", vInfo[i].ulSDSType);
        JsonParam.Add("SDSInfo", vInfo[i].acSDSInfo);
        JsonParam.Add("NeedReply", vInfo[i].bNeedReply);
        JsonParam.Add("RecvUserNum", vInfo[i].ulRecvUserNum);
        JsonParam.Add("RecvSuccUserNum", vInfo[i].ulRecvSuccUserNum);
        JsonParam.Add("ReplyUserNum", vInfo[i].ulReplyUserNum);
        JsonParam.Add("File", vInfo[i].acFile);

        JsonTupleParam.Add(JsonParam);
        if (JsonTupleParam.GetStringSize() >= GetSafeQueryRecLen())
        {
            break;
        }
    }

    SendRsp(MSG_DCGetLatestSDSInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCAddSDSInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    SDS_INFO_T      stInfo = {0};
    UINT32          ulMaxSDSID = 0;
    BOOL            bRet = FALSE;
    UINT32          ulUserID;
    VECTOR<UINT32>  vUserID;
    UINT32          ulCurrDay;
    SDS_INDEX_T     stIndex;
    CHAR            *szUser = Json.GetValue("User");

    memset(&stIndex, 0, sizeof(stIndex));

    if (!szUser)
    {
        TaskLog(LOG_ERROR, "invalid User in msg %s", szMsgType);
        return;
    }

    GJson           UserJson;

    if (!UserJson.Parse(szUser))
    {
        TaskLog(LOG_ERROR, "invalid User in msg %s", szMsgType);
        return;
    }

    VECTOR<GJson*>  &SubJson = UserJson.GetSubJson();

    stInfo.ulSendTime = gos_get_current_time();
    ulCurrDay = gos_get_day_from_time(stInfo.ulSendTime);
    stInfo.bSendFlag = FALSE;

    if (!Json.GetValue("SendUserID", &stInfo.ulSendUserID))
    {
        TaskLog(LOG_ERROR, "invalid SendUserID in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("SendUserName", stInfo.acSendUserName, sizeof(stInfo.acSendUserName)))
    {
        TaskLog(LOG_ERROR, "invalid SendUserName in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("SDSType", &stInfo.ulSDSType))
    {
        TaskLog(LOG_ERROR, "invalid SDSType in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("SDSInfo", stInfo.acSDSInfo, sizeof(stInfo.acSDSInfo)))
    {
        TaskLog(LOG_ERROR, "invalid SDSInfo in msg %s", szMsgType);
        return;
    }

    DecodeJsonMsg(stInfo.acSDSInfo, sizeof(stInfo.acSDSInfo)-1);

    if (!Json.GetValue("File", stInfo.acFile, sizeof(stInfo.acFile)))
    {
        TaskLog(LOG_ERROR, "invalid File in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("NeedReply", &stInfo.bNeedReply))
    {
        TaskLog(LOG_ERROR, "invalid NeedReply in msg %s", szMsgType);
        return;
    }

    for (UINT32 i=0; i<SubJson.size(); i++)
    {
        if (!SubJson[i]->GetValue("UserID", &ulUserID))
        {
            TaskLog(LOG_ERROR, "invalid UserID of User in msg %s", szMsgType);
            return;
        }

        vUserID.push_back(ulUserID);
    }

    stInfo.ulRecvUserNum = vUserID.size();

    if (!m_pDao->GetMaxSDSID(&ulMaxSDSID))
    {
        goto end;
    }

    if (ulMaxSDSID < gos_get_current_time())
    {
        ulMaxSDSID = gos_get_current_time();
    }
    else
    {
        ulMaxSDSID++;
    }

    if (!m_pDao->GetSDSIndex(stIndex))
    {
        goto end;
    }

    stInfo.ulSDSID = ulMaxSDSID;
    if (stInfo.ulSDSType == SDS_TYPE_FORMAL_DISPATCH)
    {
        stInfo.ulSDSIndex = stIndex.ulFormalDispatchSDSIndex;
    }
    else if (stInfo.ulSDSType == SDS_TYPE_ORAL_DISPATCH )
    {
        if (ulCurrDay != stIndex.ulOralDispatchSDSDay)
        {
            stIndex.ulOralDispatchSDSIndex = ORAL_DISPATCH_SDS_INDEX_MIN;
        }

        stInfo.ulSDSIndex = stIndex.ulOralDispatchSDSIndex;
    }
    else if (stInfo.ulSDSType == SDS_TYPE_OPERATE)
    {
        if (ulCurrDay != stIndex.ulOperateSDSDay)
        {
            stIndex.ulOperateSDSIndex = OPERATE_SDS_INDEX_MIN;
        }

        stInfo.ulSDSIndex = stIndex.ulOperateSDSIndex;
    }

    m_pDao->TransBegin();

    if (!m_pDao->AddSDSInfo(stInfo) )
    {
        TaskLog(LOG_ERROR, "add sds info %s failed", stInfo.acSDSInfo);
        m_pDao->TransRollback();
        goto end;
    }

    if (stInfo.bNeedReply && !m_pDao->AddSDSReply(stInfo.ulSDSID, vUserID) )
    {
        TaskLog(LOG_ERROR, "add sds reply user of %s failed", stInfo.acSDSInfo);
        m_pDao->TransRollback();
        goto end;
    }

    if (stInfo.ulSDSType == SDS_TYPE_FORMAL_DISPATCH)
    {
        stIndex.ulFormalDispatchSDSIndex++;
        if (stIndex.ulFormalDispatchSDSIndex > FORMAL_DISPATCH_SDS_INDEX_MAX)
        {
            stIndex.ulFormalDispatchSDSIndex = FORMAL_DISPATCH_SDS_INDEX_MIN;
        }
    }
    else if (stInfo.ulSDSType == SDS_TYPE_ORAL_DISPATCH)
    {
        stIndex.ulOralDispatchSDSIndex++;
        if (stIndex.ulOralDispatchSDSIndex > ORAL_DISPATCH_SDS_INDEX_MAX)
        {
            stIndex.ulOralDispatchSDSIndex = ORAL_DISPATCH_SDS_INDEX_MIN;
        }
    }
    else if (stInfo.ulSDSType == SDS_TYPE_OPERATE)
    {
        stIndex.ulOperateSDSIndex++;
    }

    if (!m_pDao->UpdateSDSIndex(ulCurrDay, stIndex.ulFormalDispatchSDSIndex, stIndex.ulOralDispatchSDSIndex, stIndex.ulOperateSDSIndex) )
    {
        TaskLog(LOG_ERROR, "update sds index failed");
        m_pDao->TransRollback();
        goto end;
    }

    if (!m_pDao->TransCommit())
    {
        m_pDao->TransRollback();
        goto end;
    }

    bRet = TRUE;

end:
    GJsonParam  Param;

    if (bRet)
    {
        Param.Add("SDSID", stInfo.ulSDSID);
        Param.Add("SDSIndex", stInfo.ulSDSIndex);
    }

    SendRsp(szMsgType, ulSeqID, Param);
}

VOID TaskCfg::OnDCSetSDSInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    UINT32      ulSDSID = 0;
    BOOL        bSendFlag = TRUE;
    BOOL        bRet = FALSE;

    if (!Json.GetValue("SDSID", &ulSDSID))
    {
        TaskLog(LOG_ERROR, "invalid SDSID in msg %s", szMsgType);
        return;
    }

    if (!m_pDao->SetSDSInfo(ulSDSID, bSendFlag))
    {
        TaskLog(LOG_ERROR, "set sds info %u failed", ulSDSID);
        goto end;
    }

    bRet = TRUE;

end:
    GJsonParam  Param;

    Param.Add("Result", bRet?"Succ":"Fail");

    SendRsp(szMsgType, ulSeqID, Param);
}

VOID TaskCfg::OnDCDelSDSInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    UINT32      ulSDSID = 0;
    BOOL        bRet = FALSE;

    if (!Json.GetValue("SDSID", &ulSDSID))
    {
        TaskLog(LOG_ERROR, "invalid SDSID in msg %s", szMsgType);
        return;
    }

    if (!m_pDao->DelSDSInfo(ulSDSID))
    {
        TaskLog(LOG_ERROR, "del sds info %u failed", ulSDSID);
        goto end;
    }

    bRet = TRUE;

end:
    GJsonParam  Param;

    Param.Add("Result", bRet?"Succ":"Fail");

    SendRsp(szMsgType, ulSeqID, Param);
}

VOID TaskCfg::OnDCGetSDSReplyReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    GJsonTupleParam     JsonTupleParam;
    VECTOR<SDS_REPLY_T> vInfo;
    UINT32              ulSDSID;

    if (!Json.GetValue("SDSID", &ulSDSID))
    {
        TaskLog(LOG_ERROR, "invalid SDSID in msg %s", szMsgType);
        return;
    }

    if (!m_pDao->GetSDSReply(ulSDSID, vInfo))
    {
        TaskLog(LOG_ERROR, "load sds reply failed");
        return;
    }

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        GJsonParam              JsonParam;

        JsonParam.Add("SDSID", vInfo[i].ulSDSID);
        JsonParam.Add("UserID", vInfo[i].ulUserID);
        JsonParam.Add("RecvTime", vInfo[i].ulRecvTime);
        JsonParam.Add("SendResult", vInfo[i].ulSendResult);
        JsonParam.Add("ReplyTime", vInfo[i].ulReplyTime);

        JsonTupleParam.Add(JsonParam);
    }

    SendRsp(MSG_DCGetSDSReply, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadRecBrdCfgReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    GJsonTupleParam         JsonTupleParam;
    VECTOR<REC_BRD_CFG_T>   vInfo;

    GetRecBrdCfg(vInfo);

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        GJsonParam              JsonParam;

        JsonParam.Add("RecID", vInfo[i].ulRecID);
        JsonParam.Add("RecInfo", vInfo[i].acRecInfo);

        JsonTupleParam.Add(JsonParam);
    }

    SendLoadCfgDataRsp(MSG_DCLoadRecBrdCfg, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCLoadDepotInfoReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    GJsonTupleParam         JsonTupleParam;
    VECTOR<DEPOT_INFO_T>    vInfo;

    GetDepotInfo(vInfo);

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        GJsonParam              JsonParam;

        JsonParam.Add("DepotID", vInfo[i].ulDepotID);
        JsonParam.Add("DepotName", vInfo[i].acDepotName);
        JsonParam.Add("TrainDirection", vInfo[i].ulTrainDirection);

        JsonTupleParam.Add(JsonParam);
    }

    SendLoadCfgDataRsp(MSG_DCLoadDepotInfo, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnDCSetTrainDepotIDReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    VECTOR<UINT32>  vTrain;
    UINT32          ulDepotID;
    UINT32          ulTrainUnitID;
    UINT32          ulTrainDirection;
    CHAR            *szDestID;
    CHAR            *szTrain;

    if (!Json.GetValue("DestDepotID", &ulDepotID))
    {
        TaskLog(LOG_ERROR, "invalid DestDepotID in msg %s", szReq);
        return;
    }

    szDestID = Json.GetValue("DestID");
    if (!szDestID)
    {
        TaskLog(LOG_ERROR, "invalid DestID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("TrainDirection", &ulTrainDirection))
    {
        TaskLog(LOG_ERROR, "invalid TrainDirection in msg %s", szReq);
        return;
    }

    szTrain = Json.GetValue("Train");
    if (!szTrain)
    {
        TaskLog(LOG_ERROR, "invalid Train in msg %s", szReq);
        return;
    }

    GJson           TrainJson;

    if (!TrainJson.Parse(szTrain))
    {
        TaskLog(LOG_ERROR, "invalid Train in msg %s", szReq);
        return;
    }

    VECTOR<GJson*>  &SubJson = TrainJson.GetSubJson();
    CHAR            acTrainList[4096];
    INT32           iLen = 0;
    INT32           iCurrLen = 0;

    memset(acTrainList, 0, sizeof(acTrainList));
    for (UINT32 i=0; i<SubJson.size(); i++)
    {
        if (!SubJson[i]->GetValue("TrainUnitID", &ulTrainUnitID))
        {
            TaskLog(LOG_ERROR, "invalid TrainUnitID of User in msg %s", szReq);
            return;
        }

        vTrain.push_back(ulTrainUnitID);
        iLen = snprintf(acTrainList+iCurrLen, sizeof(acTrainList)-1-iCurrLen, "%s%u", i==0?"":",", ulTrainUnitID);
        if (iLen > 0)
        {
            iCurrLen += iLen;
        }
    }

    AddOperLog("批量挪车：%s", acTrainList);

    SetTrainPos(vTrain, ulDepotID, szDestID, ulTrainDirection);

    GJsonParam  Param;

    Param.Add("Result", "Succ");

    SendRsp(szMsgType, ulSeqID, Param);
}

VOID TaskCfg::OnDCSetUESDSReplyReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    UINT32              ulUserID;
    UINT32              ulSDSIndex;
    UINT32              ulTime;
    UINT32              ulSDSID = 0;
    GJsonParam          Param;

    if (!Json.GetValue("UserID", &ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("SDSID", &ulSDSIndex))
    {
        TaskLog(LOG_ERROR, "invalid SDSID in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("Time", &ulTime))
    {
        TaskLog(LOG_ERROR, "invalid Time in msg %s", szMsgType);
        return;
    }

    if (!m_pDao->GetSDSIDBySDSIndex(ulSDSIndex, &ulSDSID))
    {
        TaskLog(LOG_ERROR, "get SDS of %u failed", ulSDSIndex);
    }

    if (!m_pDao->UpdateSDSReply(ulSDSID, ulUserID, ulTime))
    {
        TaskLog(LOG_ERROR, "failed to update SDSReply info of %u in msg %s", ulUserID, szReq);
    }

    SendConfirmSaveRsp(ulSeqID);

    TaskLog(LOG_INFO, "UE %u send SDSReply of SDS %u", ulUserID, ulSDSID);
}

VOID TaskCfg::OnDCSetUESDSRecvReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    UINT32              ulUserID;
    UINT32              ulSendResult;
    UINT32              ulSDSID = 0;
    UINT32              ulTime;
    GJsonParam          Param;

    if (!Json.GetValue("UserID", &ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("SDSID", &ulSDSID))
    {
        TaskLog(LOG_ERROR, "invalid SDSID in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("SendResult", &ulSendResult))
    {
        TaskLog(LOG_ERROR, "invalid SendResult in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("Time", &ulTime))
    {
        TaskLog(LOG_ERROR, "invalid Time in msg %s", szMsgType);
        return;
    }

    if (!m_pDao->UpdateSDSRecv(ulSDSID, ulUserID, ulSendResult, ulTime))
    {
        TaskLog(LOG_ERROR, "failed to update SDSReply info of %u in msg %s", ulUserID, szReq);
    }

    SendConfirmSaveRsp(ulSeqID);

    TaskLog(LOG_INFO, "UE %u send SDSRecv of SDS %u", ulUserID, ulSDSID);
}

VOID TaskCfg::OnDCSaveNewSDSReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    SDS_INFO_T          stInfo = {0};
    GJsonParam          Param;
    UINT32              ulMaxSDSID = 0;

    if (!Json.GetValue("SendUserID", &stInfo.ulSendUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("SendUserName", stInfo.acSendUserName, sizeof(stInfo.acSendUserName)))
    {
        TaskLog(LOG_ERROR, "invalid SendUserName in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("SendTime", &stInfo.ulSendTime))
    {
        TaskLog(LOG_ERROR, "invalid SendTime in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("SDSInfo", stInfo.acSDSInfo, sizeof(stInfo.acSDSInfo)))
    {
        TaskLog(LOG_ERROR, "invalid SDSInfo in msg %s", szMsgType);
        return;
    }

    DecodeJsonMsg(stInfo.acSDSInfo, sizeof(stInfo.acSDSInfo)-1);

    stInfo.bSendFlag = FALSE;
    stInfo.ulSDSType = SDS_TYPE_NORMAL;
    stInfo.bNeedReply = FALSE;
    stInfo.ulRecvUserNum = 0;

    if (!m_pDao->GetMaxSDSID(&ulMaxSDSID))
    {
        TaskLog(LOG_INFO, "get max SDSID failed");
        return;
    }

    if (ulMaxSDSID < gos_get_current_time())
    {
        ulMaxSDSID = gos_get_current_time();
    }
    else
    {
        ulMaxSDSID++;
    }

    stInfo.ulSDSID = ulMaxSDSID;
    stInfo.ulRecvDCUserID = m_ulDCUserID;
    if (!m_pDao->AddSDSInfo(stInfo))
    {
        TaskLog(LOG_ERROR, "failed to add SDS info in msg %s", szReq);
        return;
    }

    SendConfirmSaveRsp(ulSeqID);

    TaskLog(LOG_INFO, "UE %u send SDS %s", stInfo.ulSendUserID, stInfo.acSDSInfo);
}

VOID TaskCfg::OnDCSaveNewSMSReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    SDS_INFO_T          stInfo = {0};
    GJsonParam          Param;
    UINT32              ulMaxSDSID = 0;

    if (!Json.GetValue("SendUserID", &stInfo.ulSendUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("SendUserName", stInfo.acSendUserName, sizeof(stInfo.acSendUserName)))
    {
        TaskLog(LOG_ERROR, "invalid SendUserName in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("SendTime", &stInfo.ulSendTime))
    {
        TaskLog(LOG_ERROR, "invalid SendTime in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("SMSInfo", stInfo.acSDSInfo, sizeof(stInfo.acSDSInfo)))
    {
        TaskLog(LOG_ERROR, "invalid SMSInfo in msg %s", szMsgType);
        return;
    }

    DecodeJsonMsg(stInfo.acSDSInfo, sizeof(stInfo.acSDSInfo)-1);

    if (!Json.GetValue("File", stInfo.acFile, sizeof(stInfo.acFile)))
    {
        TaskLog(LOG_ERROR, "invalid File in msg %s", szMsgType);
        return;
    }

    stInfo.bSendFlag = FALSE;
    stInfo.ulSDSType = SDS_TYPE_SMS;
    stInfo.bNeedReply = FALSE;
    stInfo.ulRecvUserNum = 0;

    if (!m_pDao->GetMaxSDSID(&ulMaxSDSID))
    {
        TaskLog(LOG_INFO, "get max SDSID failed");
        return;
    }

    if (ulMaxSDSID < gos_get_current_time())
    {
        ulMaxSDSID = gos_get_current_time();
    }
    else
    {
        ulMaxSDSID++;
    }

    stInfo.ulSDSID = ulMaxSDSID;
    stInfo.ulRecvDCUserID = m_ulDCUserID;
    if (!m_pDao->AddSDSInfo(stInfo))
    {
        TaskLog(LOG_ERROR, "failed to add SMS info in msg %s", szReq);
        return;
    }

    SendConfirmSaveRsp(ulSeqID);

    TaskLog(LOG_INFO, "UE %u send SMS %s", stInfo.ulSendUserID, stInfo.acSDSInfo);
}

VOID TaskCfg::OnDCSetOperatorReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    CHAR                acOperator[16];
    BOOL                bOffDuty;
    GJsonParam          Param;

    if (!Json.GetValue("Operator", acOperator, sizeof(acOperator)))
    {
        TaskLog(LOG_ERROR, "invalid Operator in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("Duty", &bOffDuty))
    {
        TaskLog(LOG_ERROR, "invalid Duty in msg %s", szMsgType);
        return;
    }

    SendConfirmSaveRsp(ulSeqID);

    if (bOffDuty)
    {
        AddOperLog("调度员交班：%s", acOperator);
        TaskLog(LOG_INFO, "operator %s is off duty", acOperator);
    }
    else
    {
        AddOperLog("调度员接班：%s", acOperator);
        TaskLog(LOG_INFO, "operator %s is on duty", acOperator);
    }
}

VOID TaskCfg::OnDCSaveGISInfo(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    GJsonParam          Param;
    UINT32              ulUserID = 0;
    DOUBLE              dLon;
    DOUBLE              dLat;

    if (!Json.GetValue("UserID", &ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Lon", &dLon))
    {
        TaskLog(LOG_ERROR, "invalid Lon in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Lat", &dLat))
    {
        TaskLog(LOG_ERROR, "invalid Lat in msg %s", szReq);
        return;
    }

    if (!m_pDao->SetPortalUEGISInfo(ulUserID, dLon, dLat))
    {
        TaskLog(LOG_ERROR, "save GIS info of %u failed", ulUserID);
        return;
    }

    SetPortalUEGISInfo(ulUserID, dLon, dLat);

    SendConfirmSaveRsp(ulSeqID);
}

VOID TaskCfg::OnDCSaveMultiGroupInfo(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    VECTOR<MULTI_GROUP_INFO_T>  vInfo;
    MULTI_GROUP_INFO_T          stOldInfo;

    for (UINT32 i=0; i<Json.GetSubJsonCount(); i++)
    {
        GJson   *SubJson = Json.GetSubJson(i);
        MULTI_GROUP_INFO_T  stInfo = {0};

        memset(&stInfo, 0, sizeof(stInfo));
        stInfo.ulDCUserID = m_ulDCUserID;
        if (!SubJson->GetValue("GroupID", &stInfo.i64GroupID))
        {
            TaskLog(LOG_ERROR, "invalid GroupID in msg %s", szReq);
            return;
        }

        if (!SubJson->GetValue("GroupName", stInfo.acGroupName, sizeof(stInfo.acGroupName)))
        {
            TaskLog(LOG_ERROR, "invalid GroupName in msg %s", szReq);
            return;
        }

        if (!SubJson->GetValue("GroupMember", stInfo.acGroupMember, sizeof(stInfo.acGroupMember)))
        {
            TaskLog(LOG_ERROR, "invalid GroupMember in msg %s", szReq);
            return;
        }

        if (stInfo.acGroupName[0] == '\0' &&
            GetMultiGroupInfo(stInfo.i64GroupID, stOldInfo))
        {
            strcpy(stInfo.acGroupName, stInfo.acGroupName);
        }

        vInfo.push_back(stInfo);
    }

    if (!m_pDao->SetMultiGroupInfo(m_ulDCUserID, vInfo))
    {
        TaskLog(LOG_ERROR, "save multi group info of %u failed", m_ulDCUserID);
        return;
    }

    ClearMultiGroupInfo(m_ulDCUserID);
    SetMultiGroupInfo(vInfo);

    SendConfirmSaveRsp(ulSeqID);
    SetCfgDataVer(m_ulLoadMultiGroupInfo);

    TaskLog(LOG_INFO, "save multi group info of %u succ", m_ulDCUserID);
}

VOID TaskCfg::OnDCGetRecFileListReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    SendAllClient(DIS_CLIENT_DC, EV_DC_DIS_RSP, szReq);
}

VOID TaskCfg::OnDCGetRecFileListRsp(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    SendAllClient(DIS_CLIENT_DC, EV_DC_DIS_RSP, szReq);
}

VOID TaskCfg::OnDCCfgReq(CHAR *szReq, UINT32 ulMsgLen)
{
    GJson       Json;
    GJson       SubJson;
    BOOL        bRet = Json.Parse(szReq);
    CHAR        *szMsgType;
    CHAR        *szMsgInfo;
    UINT32      ulSeqID;
    UINT32      ulDCUserID;
    CHAR        *szDCOperatorID;

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "invalid DC msg: %s", szReq);
        goto error;
    }

    szMsgType = Json.GetValue("MsgName");

    if (!szMsgType)
    {
        TaskLog(LOG_ERROR, "get MsgName failed: %s", szReq);
        goto error;
    }

    if (!Json.GetValue("MsgSeqID", &ulSeqID))
    {
        TaskLog(LOG_ERROR, "get MsgSeqID failed: %s", szReq);
        goto error;
    }

    if (!Json.GetValue("DCUserID", &ulDCUserID))
    {
        TaskLog(LOG_ERROR, "get DCUserID failed: %s", szReq);
        goto error;
    }

    szMsgInfo = Json.GetValue("MsgInfo");
    if (!szMsgInfo)
    {
        TaskLog(LOG_ERROR, "get MsgInfo failed: %s", szReq);
        goto error;
    }

    if (!SubJson.Parse(szMsgInfo))
    {
        TaskLog(LOG_ERROR, "parse MsgInfo failed: %s", szReq);
        goto error;
    }

    szDCOperatorID = Json.GetValue("DCOperatorID");
    if (!szDCOperatorID)
    {
        sprintf(m_acDCOperatorID, "%u", ulDCUserID);
    }
    else
    {
        memset(m_acDCOperatorID, 0, sizeof(m_acDCOperatorID));
        strncpy(m_acDCOperatorID, szDCOperatorID, sizeof(m_acDCOperatorID)-1);
    }

    TaskLog(LOG_INFO, "get DC msg: %s", szReq);

    m_ulDCUserID = ulDCUserID;

    if (strcmp(szMsgType, MSG_DCLogin) == 0)
    {
        OnDCLogin(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCLogout) == 0)
    {
        OnDCLogout(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCOperatorLogin) == 0)
    {
        OnDCOperatorLogin(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCOperatorLogout) == 0)
    {
        OnDCOperatorLogout(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCGetCfgDataVer) == 0)
    {
        OnDCGetCfgDataVer(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetPwd) == 0)
    {
        OnDCSetPwdReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCAddDCUser) == 0)
    {
        OnDCAddDCUserReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCDelDCUser) == 0)
    {
        OnDCDelDCUserReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetDCUser) == 0)
    {
        OnDCSetDCUserReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCAddDCOperator) == 0)
    {
        OnDCAddDCOperatorReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCDelDCOperator) == 0)
    {
        OnDCDelDCOperatorReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetDCOperator) == 0)
    {
        OnDCSetDCOperatorReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCAddDepotCfg) == 0)
    {
        OnDCAddDepotCfgReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCDelDepotCfg) == 0)
    {
        OnDCDelDepotCfgReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetDepotCfg) == 0)
    {
        OnDCSetDepotCfgReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetGroupCfg) == 0)
    {
        OnDCSetGroupCfgReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCAddStation) == 0)
    {
        OnDCAddStationReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCDelStation) == 0)
    {
        OnDCDelStationReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetStation) == 0)
    {
        OnDCSetStationReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCAddStationUE) == 0)
    {
        OnDCAddStationUEReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCDelStationUE) == 0)
    {
        OnDCDelStationUEReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetStationUE) == 0)
    {
        OnDCSetStationUEReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCAddPortalUE) == 0)
    {
        OnDCAddPortalUEReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCDelPortalUE) == 0)
    {
        OnDCDelPortalUEReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetPortalUE) == 0)
    {
        OnDCSetPortalUEReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCAddBrdZone) == 0)
    {
        OnDCAddBrdZoneReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCDelBrdZone) == 0)
    {
        OnDCDelBrdZoneReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetBrdZone) == 0)
    {
        OnDCSetBrdZoneReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCAddTrain) == 0)
    {
        OnDCAddTrainReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCDelTrain) == 0)
    {
        OnDCDelTrainReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetTrain) == 0)
    {
        OnDCSetTrainReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCAddTrainUE) == 0)
    {
        OnDCAddTrainUEReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCDelTrainUE) == 0)
    {
        OnDCDelTrainUEReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetTrainUE) == 0)
    {
        OnDCSetTrainUEReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCAddPredefinedSMS) == 0)
    {
        OnDCAddPredefinedSMSReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCDelPredefinedSMS) == 0)
    {
        OnDCDelPredefinedSMSReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetPredefinedSMS) == 0)
    {
        OnDCSetPredefinedSMSReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCAddSDSTemplate) == 0)
    {
        OnDCAddSDSTemplateReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCDelSDSTemplate) == 0)
    {
        OnDCDelSDSTemplateReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetSDSTemplate) == 0)
    {
        OnDCSetSDSTemplateReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCAddRecBrdCfg) == 0)
    {
        OnDCAddRecBrdCfgReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCDelRecBrdCfg) == 0)
    {
        OnDCDelRecBrdCfgReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetRecBrdCfg) == 0)
    {
        OnDCSetRecBrdCfgReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCAddDestInfo) == 0)
    {
        OnDCAddDestInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCDelDestInfo) == 0)
    {
        OnDCDelDestInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetDestInfo) == 0)
    {
        OnDCSetDestInfoReq(szReq, ulSeqID, SubJson);
    }
    else
    {
        TaskLog(LOG_ERROR, "invalid DC cfg msg: %s", szReq);
        goto error;
    }

    return;

error:
    return;
}

VOID TaskCfg::OnDCLogin(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    DC_USER_INFO_T      stUser = {0};
    GJsonParam          JsonParam;

    if (!Json.GetValue("User", &stUser.ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid User in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Pwd", stUser.acName, sizeof(stUser.acName)))
    {
        TaskLog(LOG_ERROR, "invalid Pwd in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DCType", &stUser.ulDCType))
    {
        TaskLog(LOG_ERROR, "invalid DCType in msg %s", szReq);
        return;
    }

    BOOL bRet = CheckDCUser(stUser.ulUserID, stUser.ulDCType);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "dc %u login failed", stUser.ulUserID);

        JsonParam.Add("Result", "Fail");
    }
    else
    {
        PID_T       *pstClient = GetMsgSender();
        PID_T       stPrevPID;
        UINT8       aucPrevAddr[4] = {0};
        UINT8       aucCurrAddr[4] = {0};
        UINT16      usPort;

        GosGetClientAddr(pstClient->usInstID, aucCurrAddr, &usPort);

        // 判断是否要踢出其它已登录的同名用户
        if (GetDCPID(stUser.ulUserID, &stPrevPID) &&
            pstClient->usInstID != stPrevPID.usInstID )
        {
            KickOffDC(&stPrevPID);
            if (GosGetClientAddr(stPrevPID.usInstID, aucPrevAddr, &usPort) )
            {
                TaskLog(LOG_FATAL, "dc %u logined from " IP_FMT " will be kicked off", stUser.ulUserID, IP_ARG(aucPrevAddr));
            }
        }

        JsonParam.Add("Result", "Succ");

        SetDCLogin(stUser.ulUserID, pstClient);
        AddOperLog("调度台登录");

        TaskLog(LOG_INFO, "dc %u from " IP_FMT " logined with client id %u", stUser.ulUserID, IP_ARG(aucCurrAddr), pstClient->usInstID);
    }

    SendRsp(MSG_DCLogin, ulSeqID, JsonParam);
}

VOID TaskCfg::OnDCLogout(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32          ulDCUserID = 0;
    GJsonParam      JsonParam;

    if (!Json.GetValue("User", &ulDCUserID))
    {
        TaskLog(LOG_ERROR, "invalid User in msg %s", szReq);
        return;
    }

    TaskLog(LOG_INFO, "dc %u logout", ulDCUserID);

    JsonParam.Add("Result", "Succ");

    if (m_ulDCUserID != INVALID_USER_ID)
    {
        AddOperLog("调度台退出");
    }
}

VOID TaskCfg::OnDCOperatorLogin(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    DC_OPERATOR_INFO_T  stUser = {0};
    GJsonParam          JsonParam;
    CHAR                acErrInfo[256];
    UINT32              ulDCUserID = m_ulDCUserID;

    if (!Json.GetValue("User", stUser.acOperatorID, sizeof(stUser.acOperatorID)))
    {
        TaskLog(LOG_ERROR, "invalid User in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Pwd", stUser.acPassword, sizeof(stUser.acPassword)))
    {
        TaskLog(LOG_ERROR, "invalid Pwd in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DCType", &stUser.ulDCType))
    {
        TaskLog(LOG_ERROR, "invalid DCType in msg %s", szReq);
        return;
    }

    std::string     strPwd = gos::base64::DecodeString(stUser.acPassword);

    BOOL bRet = CheckDCOperator(stUser.acOperatorID, (CHAR*)strPwd.c_str(), stUser.ulDCType, acErrInfo);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "dc %s login failed", stUser.acOperatorID);

        JsonParam.Add("Result", "Fail");
        JsonParam.Add("ErrInfo", acErrInfo);
    }
    else
    {
        PID_T       *pstClient = GetMsgSender();
        PID_T       stPrevPID;
        UINT8       aucPrevAddr[4] = {0};
        UINT8       aucCurrAddr[4] = {0};
        UINT16      usPort;

        GosGetClientAddr(pstClient->usInstID, aucCurrAddr, &usPort);

        // 判断是否要踢出其它已登录的同名用户
        if (GetDCPID(ulDCUserID, &stPrevPID) &&
            pstClient->usInstID != stPrevPID.usInstID)
        {
            KickOffDC(&stPrevPID);
            if (GosGetClientAddr(stPrevPID.usInstID, aucPrevAddr, &usPort))
            {
                TaskLog(LOG_FATAL, "dc %u logined from " IP_FMT " will be kicked off", ulDCUserID,
                        IP_ARG(aucPrevAddr));
            }
        }

        JsonParam.Add("Result", "Succ");
        SetDCLogin(ulDCUserID, pstClient);
        strcpy(m_acDCOperatorID, stUser.acOperatorID);
        AddOperLog("调度员登录");

        TaskLog(LOG_INFO, "dc %s login succ", stUser.acOperatorID);
    }

    SendRsp(MSG_DCOperatorLogin, ulSeqID, JsonParam);
}

VOID TaskCfg::OnDCOperatorLogout(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    CHAR            *szOperatorID;
    GJsonParam      JsonParam;

    szOperatorID = Json.GetValue("User");
    if (!szOperatorID || *szOperatorID == '\0')
    {
        TaskLog(LOG_ERROR, "invalid User in msg %s", szReq);
        return;
    }

    TaskLog(LOG_INFO, "dc %s logout", szOperatorID);

    JsonParam.Add("Result", "Succ");

    if (m_ulDCUserID != INVALID_USER_ID)
    {
        AddOperLog("调度员退出");
    }
}

VOID TaskCfg::OnDCAddDCOperatorReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    DC_OPERATOR_INFO_T  stInfo = {0};
    GJsonParam          JsonParam;

    if (!Json.GetValue("OperatorID", stInfo.acOperatorID, sizeof(stInfo.acOperatorID)))
    {
        TaskLog(LOG_ERROR, "invalid OperatorID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Name", stInfo.acName, sizeof(stInfo.acName)))
    {
        TaskLog(LOG_ERROR, "invalid Name in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Password", stInfo.acPassword, sizeof(stInfo.acPassword)))
    {
        TaskLog(LOG_ERROR, "invalid Password in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DCType", &stInfo.ulDCType))
    {
        TaskLog(LOG_ERROR, "invalid DCType in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->AddDCOperator(stInfo);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "add dc operator(%s) to db failed", stInfo.acOperatorID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        AddDCOperatorInfo(stInfo);
    }

    SendRsp(MSG_DCAddDCOperator, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadDCOperatorInfo);

    AddOperLog("创建调度员%s%s", stInfo.acOperatorID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCDelDCOperatorReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    CHAR            *szOperatorID;
    GJsonParam      JsonParam;

    szOperatorID = Json.GetValue("OperatorID");
    if (!szOperatorID || szOperatorID[0] == '\0')
    {
        TaskLog(LOG_ERROR, "invalid OperatorID in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->DelDCOperator(szOperatorID);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "delete dc operator(%s) from db failed", szOperatorID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        DelDCOperatorInfo(szOperatorID);
    }

    SendRsp(MSG_DCDelDCOperator, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadDCOperatorInfo);

    AddOperLog("删除调度员%s%s", szOperatorID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCSetDCOperatorReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    DC_OPERATOR_INFO_T      stInfo = {0};
    GJsonParam              JsonParam;

    if (!Json.GetValue("OperatorID", stInfo.acOperatorID, sizeof(stInfo.acOperatorID)))
    {
        TaskLog(LOG_ERROR, "invalid OperatorID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Name", stInfo.acName, sizeof(stInfo.acName)))
    {
        TaskLog(LOG_ERROR, "invalid Name in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Password", stInfo.acPassword, sizeof(stInfo.acPassword)))
    {
        TaskLog(LOG_ERROR, "invalid Password in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DCType", &stInfo.ulDCType))
    {
        TaskLog(LOG_ERROR, "invalid DCType in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->SetDCOperator(stInfo, SYNC_ADD);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "set dc operator(%s) to db failed", stInfo.acOperatorID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        SetDCOperatorInfo(stInfo);
    }

    SendRsp(MSG_DCSetDCOperator, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadDCOperatorInfo);

    AddOperLog("设置调度员%s%s", stInfo.acOperatorID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCAddDepotCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    DEPOT_INFO_T        stDepot;
    GJsonParam          JsonParam;

    if (!Json.GetValue("DepotID", &stDepot.ulDepotID))
    {
        TaskLog(LOG_ERROR, "invalid DepotID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DepotName", stDepot.acDepotName, sizeof(stDepot.acDepotName)))
    {
        TaskLog(LOG_ERROR, "invalid DepotName in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("TrainDirection", &stDepot.ulTrainDirection))
    {
        TaskLog(LOG_ERROR, "invalid TrainDirection in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->AddDepotCfg(stDepot);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "add depot %s failed", stDepot.acDepotName);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");

        AddDepotInfo(stDepot);
    }

    SendRsp(MSG_DCAddDepotCfg, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadDepotInfo);

    AddOperLog("增加车辆段<%s>%s", stDepot.acDepotName, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCSetDepotCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    DEPOT_INFO_T        stDepot;
    GJsonParam          JsonParam;

    if (!Json.GetValue("DepotID", &stDepot.ulDepotID))
    {
        TaskLog(LOG_ERROR, "invalid DepotID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DepotName", stDepot.acDepotName, sizeof(stDepot.acDepotName)))
    {
        TaskLog(LOG_ERROR, "invalid DepotName in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("TrainDirection", &stDepot.ulTrainDirection))
    {
        TaskLog(LOG_ERROR, "invalid TrainDirection in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->SetDepotCfg(stDepot);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "set depot %u failed", stDepot.ulDepotID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");

        SetDepotInfo(stDepot);
    }

    SendRsp(MSG_DCSetDepotCfg, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadDepotInfo);

    AddOperLog("修改车辆段%u%s", stDepot.ulDepotID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCDelDepotCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32              ulDepotID;
    GJsonParam          JsonParam;

    if (!Json.GetValue("DepotID", &ulDepotID))
    {
        TaskLog(LOG_ERROR, "invalid DepotID in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->DelDepotCfg(ulDepotID);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "del depot %u failed", ulDepotID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");

        DelDepotInfo(ulDepotID);
    }

    SendRsp(MSG_DCDelDepotCfg, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadDepotInfo);

    AddOperLog("删除车辆段%u%s", ulDepotID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCSetGroupCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonParam          JsonParam;
    GROUP_CFG_T         stGroupCfg = {0};

    if (!Json.GetValue("UpLineTrainGroupID", &stGroupCfg.i64UpLineTrainGroupID))
    {
        TaskLog(LOG_ERROR, "invalid UpLineTrainGroupID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DownLineTrainGroupID", &stGroupCfg.i64DownLineTrainGroupID))
    {
        TaskLog(LOG_ERROR, "invalid DownLineTrainGroupID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("MainLineTrainGroupID", &stGroupCfg.i64MainLineTrainGroupID))
    {
        TaskLog(LOG_ERROR, "invalid MainLineTrainGroupID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("UpLineTrainBrdGroupID", &stGroupCfg.i64UpLineTrainBrdGroupID))
    {
        TaskLog(LOG_ERROR, "invalid UpLineTrainBrdGroupID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DownLineTrainBrdGroupID", &stGroupCfg.i64DownLineTrainBrdGroupID))
    {
        TaskLog(LOG_ERROR, "invalid DownLineTrainBrdGroupID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("MainLineTrainBrdGroupID", &stGroupCfg.i64MainLineTrainBrdGroupID))
    {
        TaskLog(LOG_ERROR, "invalid MainLineTrainBrdGroupID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("AllStationGroupID", &stGroupCfg.i64AllStationGroupID))
    {
        TaskLog(LOG_ERROR, "invalid AllStationGroupID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("StartTempDynamicGroupID", &stGroupCfg.i64StartTempDynamicGroupID))
    {
        TaskLog(LOG_ERROR, "invalid StartTempDynamicGroupID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("EndTempDynamicGroupID", &stGroupCfg.i64EndTempDynamicGroupID))
    {
        TaskLog(LOG_ERROR, "invalid EndTempDynamicGroupID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DriverGroupID", &stGroupCfg.i64DriverGroupID))
    {
        TaskLog(LOG_ERROR, "invalid DriverGroupID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DepotDriverGroupID", &stGroupCfg.i64DepotDriverGroupID))
    {
        TaskLog(LOG_ERROR, "invalid DepotDriverGroupID in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->SetGroupCfg(stGroupCfg, SYNC_ADD);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "set group cfg failed");
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");

        SetGroupCfg(&stGroupCfg);
    }

    SendRsp(MSG_DCSetGroupCfg, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadGroupCfg);

    AddOperLog("修改主要通话组%s", bRet?"成功":"失败");
}

VOID TaskCfg::OnDCSetPwdReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    CHAR                acUser[32];
    CHAR                acOldPwd[32];
    CHAR                acNewPwd[32];
    GJsonParam          JsonParam;

    if (!Json.GetValue("User", acUser, sizeof(acUser)))
    {
        TaskLog(LOG_ERROR, "invalid User in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("OldPwd", acOldPwd, sizeof(acOldPwd)))
    {
        TaskLog(LOG_ERROR, "invalid OldPwd in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("NewPwd", acNewPwd, sizeof(acNewPwd)))
    {
        TaskLog(LOG_ERROR, "invalid NewPwd in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->SetDCOperatorPwd(acUser, acOldPwd, acNewPwd);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "set pwd of dc user(%s) failed", acUser);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        SetDCOperatorPwd(acUser, acNewPwd);
    }

    SendRsp(MSG_DCSetPwd, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadDCOperatorInfo);

    AddOperLog("修改调度员%s密码%s", acUser, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCGetCfgDataVer(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    SendGetCfgDataVerMsg(FALSE, ulSeqID);
}

VOID TaskCfg::OnDCAddDCUserReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    DC_USER_INFO_T      stUser = {0};
    GJsonParam          JsonParam;
    CHAR                acStationList[256];

    if (!Json.GetValue("UserID", &stUser.ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("UserName", stUser.acName, sizeof(stUser.acName)))
    {
        TaskLog(LOG_ERROR, "invalid UserName in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DCType", &stUser.ulDCType))
    {
        TaskLog(LOG_ERROR, "invalid DCType in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("StationList", acStationList, sizeof(acStationList)))
    {
        TaskLog(LOG_ERROR, "invalid StationList in msg %s", szReq);
        return;
    }

    if (!ConvertStationList(acStationList, stUser.aulStationList, ARRAY_SIZE(stUser.aulStationList),
                            stUser.ulStationNum))
    {
        TaskLog(LOG_ERROR, "invalid StationList in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->AddDCUser(stUser);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "add dc user(%u) to db failed", stUser.ulUserID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        AddDCUserInfo(stUser);
    }

    SendRsp(MSG_DCAddDCUser, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadDCUserInfo);

    AddOperLog("创建调度台%u%s", stUser.ulUserID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCDelDCUserReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32          ulUserID;
    GJsonParam      JsonParam;

    if (!Json.GetValue("UserID", &ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->DelDCUser(ulUserID);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "delete dc user(%u) from db failed", ulUserID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        DelDCUserInfo(ulUserID);
    }

    SendRsp(MSG_DCDelDCUser, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadDCUserInfo);

    AddOperLog("删除调度台%u%s", ulUserID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCSetDCUserReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    DC_USER_INFO_T      stUser = {0};
    GJsonParam          JsonParam;
    CHAR                acStationList[256];

    if (!Json.GetValue("UserID", &stUser.ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("UserName", stUser.acName, sizeof(stUser.acName)))
    {
        TaskLog(LOG_ERROR, "invalid UserName in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DCType", &stUser.ulDCType))
    {
        TaskLog(LOG_ERROR, "invalid DCType in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("StationList", acStationList, sizeof(acStationList)))
    {
        TaskLog(LOG_ERROR, "invalid StationList in msg %s", szReq);
        return;
    }

    if (!ConvertStationList(acStationList, stUser.aulStationList, ARRAY_SIZE(stUser.aulStationList),
                            stUser.ulStationNum))
    {
        TaskLog(LOG_ERROR, "invalid StationList in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->AddDCUser(stUser);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "set dc user(%u) to db failed", stUser.ulUserID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        SetDCUserInfo(stUser);
    }

    SendRsp(MSG_DCSetDCUser, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadDCUserInfo);

    AddOperLog("设置调度台%u%s", stUser.ulUserID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCAddDestInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    DEST_INFO_T      stInfo = {0};
    GJsonParam          JsonParam;

    if (!Json.GetValue("StationID", &stInfo.ulStationID))
    {
        TaskLog(LOG_ERROR, "invalid StationID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DestName", stInfo.acDestName, sizeof(stInfo.acDestName)))
    {
        TaskLog(LOG_ERROR, "invalid DestName in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DestinationCode", &stInfo.ulDestID))
    {
        TaskLog(LOG_ERROR, "invalid DestinationCode in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->AddDestInfo(stInfo);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "add dest station code info(%u) to db failed", stInfo.ulDestID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        AddDestInfo(stInfo);
    }

    SendRsp(MSG_DCAddDestInfo, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadDestInfo);

    AddOperLog("增加目的地码%u(%s)%s", stInfo.ulDestID, stInfo.acDestName, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCDelDestInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32          ulDestID;
    GJsonParam      JsonParam;

    if (!Json.GetValue("DestID", &ulDestID))
    {
        TaskLog(LOG_ERROR, "invalid DestID in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->DelDestInfo(ulDestID);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "delete DestID(%u) from db failed", ulDestID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        DelDestInfo(ulDestID);
    }

    SendRsp(MSG_DCDelDestInfo, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadDestInfo);

    AddOperLog("删除目的地码%u%s", ulDestID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCSetDestInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    DEST_INFO_T      stInfo = {0};
    GJsonParam          JsonParam;

    if (!Json.GetValue("StationID", &stInfo.ulStationID))
    {
        TaskLog(LOG_ERROR, "invalid StationID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DestName", stInfo.acDestName, sizeof(stInfo.acDestName)))
    {
        TaskLog(LOG_ERROR, "invalid DestName in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DestinationCode", &stInfo.ulDestID))
    {
        TaskLog(LOG_ERROR, "invalid DestinationCode in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->SetDestInfo(stInfo, SYNC_ADD);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "set dest station code(%u) to db failed", stInfo.ulDestID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        SetDestInfo(stInfo);
    }

    SendRsp(MSG_DCSetDestInfo, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadDestInfo);

    AddOperLog("设置目的地码%u%s", stInfo.ulDestID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCAddStationReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    STATION_INFO_T      stInfo = {0};
    GJsonParam          JsonParam;

    if (!Json.GetValue("StationID", &stInfo.ulStationID))
    {
        TaskLog(LOG_ERROR, "invalid StationID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("StationName", stInfo.acStationName, sizeof(stInfo.acStationName)))
    {
        TaskLog(LOG_ERROR, "invalid StationName in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("StationType", &stInfo.ulStationType))
    {
        TaskLog(LOG_ERROR, "invalid StationType in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("InterLockStationID", &stInfo.ulInterLockStationID))
    {
        TaskLog(LOG_ERROR, "invalid InterLockStationID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("StationGroupID", &stInfo.i64StationGroupID))
    {
        TaskLog(LOG_ERROR, "invalid StationGroupID in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->AddStation(stInfo);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "add station(%u) to db failed", stInfo.ulStationID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        AddStationInfo(stInfo);
    }

    SendRsp(MSG_DCAddStation, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadStationInfo);

    AddOperLog("创建车站%u(%s)%s", stInfo.ulStationID, stInfo.acStationName, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCDelStationReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32          ulStationID;
    GJsonParam      JsonParam;

    if (!Json.GetValue("StationID", &ulStationID))
    {
        TaskLog(LOG_ERROR, "invalid StationID in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->DelStation(ulStationID);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "delete station(%u) from db failed", ulStationID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        DelStationInfo(ulStationID);
    }

    SendRsp(MSG_DCDelStation, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadStationInfo);

    AddOperLog("删除车站%u%s", ulStationID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCSetStationReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    STATION_INFO_T      stInfo = {0};
    GJsonParam          JsonParam;

    if (!Json.GetValue("StationID", &stInfo.ulStationID))
    {
        TaskLog(LOG_ERROR, "invalid StationID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("StationName", stInfo.acStationName, sizeof(stInfo.acStationName)))
    {
        TaskLog(LOG_ERROR, "invalid StationName in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("StationType", &stInfo.ulStationType))
    {
        TaskLog(LOG_ERROR, "invalid StationType in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("InterLockStationID", &stInfo.ulInterLockStationID))
    {
        TaskLog(LOG_ERROR, "invalid InterLockStationID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("StationGroupID", &stInfo.i64StationGroupID))
    {
        TaskLog(LOG_ERROR, "invalid StationGroupID in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->AddStation(stInfo);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "set station(%u) to db failed", stInfo.ulStationID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        SetStationInfo(stInfo);
    }

    SendRsp(MSG_DCSetStation, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadStationInfo);

    AddOperLog("设置车站%u%s", stInfo.ulStationID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCAddStationUEReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    STATION_UE_INFO_T   stInfo = {0};
    GJsonParam          JsonParam;
    BOOL                bRet = FALSE;

    if (!Json.GetValue("UserID", &stInfo.ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szReq);
        return;
    }

    if (GetStationUEInfo(stInfo.ulUserID, &stInfo))
    {
        TaskLog(LOG_ERROR, "station UE %u is already exist", stInfo.ulUserID);
        goto end;
    }

    if (!Json.GetValue("Name", stInfo.acName, sizeof(stInfo.acName)))
    {
        TaskLog(LOG_ERROR, "invalid Name in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("LanAddr", stInfo.acLanAddr, sizeof(stInfo.acLanAddr)))
    {
        TaskLog(LOG_ERROR, "invalid LanAddr in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("StationID", &stInfo.ulStationID))
    {
        TaskLog(LOG_ERROR, "invalid StationID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DefaultGroupID", &stInfo.i64DefaultGroupID))
    {
        TaskLog(LOG_ERROR, "invalid DefaultGroupID in msg %s", szReq);
        return;
    }

    bRet = m_pDao->AddStationUE(stInfo);

end:
    if (!bRet)
    {
        TaskLog(LOG_ERROR, "add station UE(%u) to db failed", stInfo.ulUserID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        AddStationUEInfo(stInfo);
    }

    SendRsp(MSG_DCAddStationUE, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadStationUEInfo);

    AddOperLog("增加固定台%u%s", stInfo.ulUserID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCDelStationUEReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32          ulUserID;
    GJsonParam      JsonParam;

    if (!Json.GetValue("UserID", &ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->DelStationUE(ulUserID);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "delete station user(%u) from db failed", ulUserID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        DelStationUEInfo(ulUserID);
    }

    SendRsp(MSG_DCDelStationUE, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadStationUEInfo);

    AddOperLog("删除固定台%u%s", ulUserID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCSetStationUEReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    STATION_UE_INFO_T   stInfo = {0};
    GJsonParam          JsonParam;

    if (!Json.GetValue("UserID", &stInfo.ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szReq);
        return;
    }

    if (!GetStationUEInfo(stInfo.ulUserID, &stInfo))
    {
        TaskLog(LOG_ERROR, "unknown UserID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Name", stInfo.acName, sizeof(stInfo.acName)))
    {
        TaskLog(LOG_ERROR, "invalid Name in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("LanAddr", stInfo.acLanAddr, sizeof(stInfo.acLanAddr)))
    {
        TaskLog(LOG_ERROR, "invalid LanAddr in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("StationID", &stInfo.ulStationID))
    {
        TaskLog(LOG_ERROR, "invalid StationID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DefaultGroupID", &stInfo.i64DefaultGroupID))
    {
        TaskLog(LOG_ERROR, "invalid DefaultGroupID in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->SetStationUE(stInfo);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "set station user(%u) to db failed", stInfo.ulUserID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        SetStationUEInfo(stInfo);
    }

    SendRsp(MSG_DCSetStationUE, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadStationUEInfo);

    AddOperLog("设置固定台%u%s", stInfo.ulUserID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCAddPortalUEReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    PORTAL_UE_INFO_T    stInfo = {0};
    GJsonParam          JsonParam;
    BOOL                bRet = FALSE;

    if (!Json.GetValue("UserID", &stInfo.ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szReq);
        return;
    }

    if (GetPortalUEInfo(stInfo.ulUserID, &stInfo))
    {
        TaskLog(LOG_ERROR, "portal UE %u is already exist", stInfo.ulUserID);
        goto end;
    }

    if (!Json.GetValue("Name", stInfo.acName, sizeof(stInfo.acName)))
    {
        TaskLog(LOG_ERROR, "invalid Name in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Type", &stInfo.ulType))
    {
        TaskLog(LOG_ERROR, "invalid Type in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("FuncType", &stInfo.ulFuncType))
    {
        TaskLog(LOG_ERROR, "invalid FuncType in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("StationID", &stInfo.ulStationID))
    {
        TaskLog(LOG_ERROR, "invalid StationID in msg %s", szReq);
        return;
    }

    bRet = m_pDao->AddPortalUE(stInfo);

end:
    if (!bRet)
    {
        TaskLog(LOG_ERROR, "add portal UE(%u) to db failed", stInfo.ulUserID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        AddPortalUEInfo(stInfo);
    }

    SendRsp(MSG_DCAddPortalUE, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadPortalUEInfo);

    AddOperLog("增加手持台%u%s", stInfo.ulUserID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCDelPortalUEReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32          ulUserID;
    GJsonParam      JsonParam;

    if (!Json.GetValue("UserID", &ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->DelPortalUE(ulUserID);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "delete portal user(%u) from db failed", ulUserID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        DelPortalUEInfo(ulUserID);
    }

    SendRsp(MSG_DCDelPortalUE, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadPortalUEInfo);

    AddOperLog("删除手持台%u%s", ulUserID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCSetPortalUEReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    PORTAL_UE_INFO_T    stInfo = {0};
    GJsonParam          JsonParam;

    if (!Json.GetValue("UserID", &stInfo.ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szReq);
        return;
    }

    if (!GetPortalUEInfo(stInfo.ulUserID, &stInfo))
    {
        TaskLog(LOG_ERROR, "unknown UserID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Name", stInfo.acName, sizeof(stInfo.acName)))
    {
        TaskLog(LOG_ERROR, "invalid Name in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Type", &stInfo.ulType))
    {
        TaskLog(LOG_ERROR, "invalid Type in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("FuncType", &stInfo.ulFuncType))
    {
        TaskLog(LOG_ERROR, "invalid FuncType in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("StationID", &stInfo.ulStationID))
    {
        TaskLog(LOG_ERROR, "invalid StationID in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->SetPortalUE(stInfo);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "set portal user(%u) to db failed", stInfo.ulUserID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        SetPortalUEInfo(stInfo);
    }

    SendRsp(MSG_DCSetPortalUE, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadPortalUEInfo);

    AddOperLog("设置手持台%u%s", stInfo.ulUserID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCAddBrdZoneReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    BRD_ZONE_INFO_T     stInfo = {0};
    GJsonParam          JsonParam;

    if (!Json.GetValue("StationID", &stInfo.ulStationID))
    {
        TaskLog(LOG_ERROR, "invalid StationID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("ZoneID", &stInfo.ulZoneID))
    {
        TaskLog(LOG_ERROR, "invalid ZoneID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("ZoneName", stInfo.acZoneName, sizeof(stInfo.acZoneName)))
    {
        TaskLog(LOG_ERROR, "invalid ZoneName in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("GroupID", &stInfo.i64GroupID))
    {
        TaskLog(LOG_ERROR, "invalid GroupID in msg %s", szReq);
        return;
    }

    BOOL bRet = FALSE;

    if (IsBrdZoneExist(stInfo.ulStationID, stInfo.ulZoneID))
    {
        TaskLog(LOG_ERROR, "broadcast zone(%u) of station %u is existed", stInfo.ulZoneID, stInfo.ulStationID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        bRet = m_pDao->AddBrdZoneInfo(stInfo);
        if (!bRet)
        {
            TaskLog(LOG_ERROR, "add broadcast zone(%u) of station %u to db failed", stInfo.ulZoneID, stInfo.ulStationID);
            JsonParam.Add("Result", "Fail");
        }
        else
        {
            JsonParam.Add("Result", "Succ");
            AddBrdZoneInfo(stInfo);
        }
    }

    SendRsp(MSG_DCAddBrdZone, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadBrdZoneInfo);

    AddOperLog("车站%u增加站区%u%s", stInfo.ulStationID, stInfo.ulZoneID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCDelBrdZoneReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32          ulStationID;
    UINT32          ulZoneID;
    GJsonParam      JsonParam;

    if (!Json.GetValue("StationID", &ulStationID))
    {
        TaskLog(LOG_ERROR, "invalid StationID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("ZoneID", &ulZoneID))
    {
        TaskLog(LOG_ERROR, "invalid ZoneID in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->DelBrdZoneInfo(ulStationID, ulZoneID);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "delete broadcast zone(%u) of station %u from db failed", ulStationID, ulZoneID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        DelBrdZoneInfo(ulStationID, ulZoneID);
    }

    SendRsp(MSG_DCDelBrdZone, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadBrdZoneInfo);

    AddOperLog("车站%u删除站区%u%s", ulStationID, ulZoneID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCSetBrdZoneReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    BRD_ZONE_INFO_T stInfo = {0};
    GJsonParam      JsonParam;

    if (!Json.GetValue("StationID", &stInfo.ulStationID))
    {
        TaskLog(LOG_ERROR, "invalid StationID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("ZoneID", &stInfo.ulZoneID))
    {
        TaskLog(LOG_ERROR, "invalid ZoneID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("ZoneName", stInfo.acZoneName, sizeof(stInfo.acZoneName)))
    {
        TaskLog(LOG_ERROR, "invalid ZoneName in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("GroupID", &stInfo.i64GroupID))
    {
        TaskLog(LOG_ERROR, "invalid GroupID in msg %s", szReq);
        return;
    }

    BOOL    bRet = FALSE;

    if (!IsBrdZoneExist(stInfo.ulStationID, stInfo.ulZoneID))
    {
        TaskLog(LOG_ERROR, "unknown broadcast zone(%u) of station %u", stInfo.ulZoneID, stInfo.ulStationID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        bRet = m_pDao->SetBrdZoneInfo(stInfo);
        if (!bRet)
        {
            TaskLog(LOG_ERROR, "set broadcast zone(%u) of station %u to db failed", stInfo.ulZoneID, stInfo.ulStationID);
            JsonParam.Add("Result", "Fail");
        }
        else
        {
            JsonParam.Add("Result", "Succ");
            SetBrdZoneInfo(stInfo);

            AddOperLog("车站%u设置站区%u%s", stInfo.ulStationID, stInfo.ulZoneID, bRet?"成功":"失败");
        }
    }

    SendRsp(MSG_DCSetBrdZone, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadBrdZoneInfo);
}

VOID TaskCfg::OnDCAddTrainReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    TRAIN_T             stInfo = {0};
    GJsonParam          JsonParam;

    if (!Json.GetValue("TrainUnitID", &stInfo.ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("TrainType", &stInfo.ulTrainType))
    {
        TaskLog(LOG_ERROR, "invalid TrainType in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("TrainName", stInfo.acTrainName, sizeof(stInfo.acTrainName)))
    {
        TaskLog(LOG_ERROR, "invalid TrainName in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DefaultGroupID", &stInfo.i64DefaultGroupID) ||
        stInfo.i64DefaultGroupID  == INVALID_GROUP_ID)
    {
        TaskLog(LOG_ERROR, "invalid DefaultGroupID in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->AddTrainInfo(stInfo);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "add train(%u) to db failed", stInfo.ulTrainUnitID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        AddTrainInfo(stInfo);
    }

    SendRsp(MSG_DCAddTrain, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadTrainInfo);

    AddOperLog("增加列车%u%s", stInfo.ulTrainUnitID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCDelTrainReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32          ulTrainUnitID;
    GJsonParam      JsonParam;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->DelTrainInfo(ulTrainUnitID);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "delete train(%u) from db failed", ulTrainUnitID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        DelTrainInfo(ulTrainUnitID);
    }

    SendRsp(MSG_DCDelTrain, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadTrainInfo);

    AddOperLog("删除列车%u%s", ulTrainUnitID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCSetTrainReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    TRAIN_T         stTrain;
    GJsonParam      JsonParam;

    if (!Json.GetValue("TrainUnitID", &stTrain.ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("TrainType", &stTrain.ulTrainType))
    {
        TaskLog(LOG_ERROR, "invalid TrainType in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("TrainName", stTrain.acTrainName, sizeof(stTrain.acTrainName)))
    {
        TaskLog(LOG_ERROR, "invalid TrainName in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DefaultGroupID", &stTrain.i64DefaultGroupID) ||
        stTrain.i64DefaultGroupID  == INVALID_GROUP_ID)
    {
        TaskLog(LOG_ERROR, "invalid DefaultGroupID in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->SetTrainInfo(stTrain);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "set train(%u) to db failed", stTrain.ulTrainUnitID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        SetTrainInfo(stTrain);
    }

    SendRsp(MSG_DCSetTrain, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadTrainInfo);

    AddOperLog("设置列车%u%s", stTrain.ulTrainUnitID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCAddTrainUEReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    TRAIN_UE_INFO_T     stInfo = {0};
    GJsonParam          JsonParam;
    BOOL                bRet = FALSE;

    if (!Json.GetValue("UserID", &stInfo.ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szReq);
        return;
    }

    if (GetTrainUEInfo(stInfo.ulUserID, &stInfo))
    {
        TaskLog(LOG_ERROR, "train UE %u is already exist", stInfo.ulUserID);
        goto end;
    }

    if (!Json.GetValue("Name", stInfo.acName, sizeof(stInfo.acName)))
    {
        TaskLog(LOG_ERROR, "invalid Name in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("LanAddr", stInfo.acLanAddr, sizeof(stInfo.acLanAddr)))
    {
        TaskLog(LOG_ERROR, "invalid LanAddr in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("TrainUnitID", &stInfo.ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in msg %s", szReq);
        return;
    }

    bRet = m_pDao->AddTrainUE(stInfo);

end:
    if (!bRet)
    {
        TaskLog(LOG_ERROR, "add train UE(%u) to db failed", stInfo.ulUserID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        AddTrainUEInfo(stInfo);
    }

    SendRsp(MSG_DCAddTrainUE, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadTrainUEInfo);

    AddOperLog("增加车载台%u%s", stInfo.ulUserID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCDelTrainUEReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32          ulUserID;
    GJsonParam      JsonParam;

    if (!Json.GetValue("UserID", &ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->DelTrainUE(ulUserID);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "delete train user(%u) from db failed", ulUserID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        DelTrainUEInfo(ulUserID);
    }

    SendRsp(MSG_DCDelTrainUE, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadTrainUEInfo);

    AddOperLog("删除车载台%u%s", ulUserID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCSetTrainUEReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    TRAIN_UE_INFO_T     stInfo = {0};
    GJsonParam          JsonParam;

    if (!Json.GetValue("UserID", &stInfo.ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szReq);
        return;
    }

    if (!GetTrainUEInfo(stInfo.ulUserID, &stInfo))
    {
        TaskLog(LOG_ERROR, "unknown UserID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Name", stInfo.acName, sizeof(stInfo.acName)))
    {
        TaskLog(LOG_ERROR, "invalid Name in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("LanAddr", stInfo.acLanAddr, sizeof(stInfo.acLanAddr)))
    {
        TaskLog(LOG_ERROR, "invalid LanAddr in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("TrainUnitID", &stInfo.ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->SetTrainUE(stInfo);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "set train user(%u) to db failed", stInfo.ulUserID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        SetTrainUEInfo(stInfo);
    }

    SendRsp(MSG_DCSetTrainUE, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadTrainUEInfo);

    AddOperLog("设置车载台%u%s", stInfo.ulUserID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCAddPredefinedSMSReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    PREDEFINED_SMS_INFO_T   stInfo = {0};
    GJsonParam              JsonParam;

    stInfo.ulSMSID = GetMaxSMSID()+1;
    if (!Json.GetValue("SMSTemplateType", &stInfo.ulSMSTemplateType))
    {
        TaskLog(LOG_ERROR, "invalid SMSTemplateType in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("SDSType", &stInfo.ulSDSType))
    {
        TaskLog(LOG_ERROR, "invalid SDSType in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("SMSInfo", stInfo.acSMSInfo, sizeof(stInfo.acSMSInfo)))
    {
        TaskLog(LOG_ERROR, "invalid SMSInfo in msg %s", szReq);
        return;
    }

    DecodeJsonMsg(stInfo.acSMSInfo, sizeof(stInfo.acSMSInfo)-1);

    BOOL bRet = m_pDao->AddPredefinedSMS(stInfo);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "add Predefined sms(%s) to db failed", stInfo.acSMSInfo);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        SetPredefinedSMS(stInfo);
    }

    SendRsp(MSG_DCAddPredefinedSMS, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadPredefinedSMS);

    AddOperLog("增加预定义短消息%s,编号:%u,内容:%s", bRet?"成功":"失败", stInfo.ulSMSID, stInfo.acSMSInfo);
}

VOID TaskCfg::OnDCDelPredefinedSMSReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32          ulSMSID;
    GJsonParam      JsonParam;

    if (!Json.GetValue("SMSID", &ulSMSID))
    {
        TaskLog(LOG_ERROR, "invalid SMSID in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->DelPredefinedSMS(ulSMSID);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "delete predefined sms(%u) from db failed", ulSMSID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        DelPredefinedSMS(ulSMSID);
    }

    SendRsp(MSG_DCDelPredefinedSMS, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadPredefinedSMS);

    AddOperLog("删除预定义短消息%u%s", ulSMSID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCSetPredefinedSMSReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    PREDEFINED_SMS_INFO_T   stInfo = {0};
    GJsonParam              JsonParam;

    if (!Json.GetValue("SMSID", &stInfo.ulSMSID))
    {
        TaskLog(LOG_ERROR, "invalid SMSID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("SMSTemplateType", &stInfo.ulSMSTemplateType))
    {
        TaskLog(LOG_ERROR, "invalid SMSTemplateType in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("SDSType", &stInfo.ulSDSType))
    {
        TaskLog(LOG_ERROR, "invalid SDSType in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("SMSInfo", stInfo.acSMSInfo, sizeof(stInfo.acSMSInfo)))
    {
        TaskLog(LOG_ERROR, "invalid SMSInfo in msg %s", szReq);
        return;
    }

    DecodeJsonMsg(stInfo.acSMSInfo, sizeof(stInfo.acSMSInfo)-1);

    BOOL bRet = m_pDao->SetPredefinedSMS(stInfo);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "set predefined sms(%s) to db failed", stInfo.acSMSInfo);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        SetPredefinedSMS(stInfo);
    }

    SendRsp(MSG_DCSetPredefinedSMS, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadPredefinedSMS);

    AddOperLog("设置预定义短消息%s,编号:%u,内容:%s", bRet?"成功":"失败", stInfo.ulSMSID, stInfo.acSMSInfo);
}

VOID TaskCfg::OnDCAddSDSTemplateReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    SDS_TEMPLATE_T      stInfo = {0};
    GJsonParam          JsonParam;

    if (!m_pDao->GetMaxSDSTemplateID(&stInfo.ulTemplateID))
    {
        return;
    }

    stInfo.ulTemplateID++;
    if (!Json.GetValue("SDSType", &stInfo.ulSDSType))
    {
        TaskLog(LOG_ERROR, "invalid SDSType in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Title", stInfo.acTitle, sizeof(stInfo.acTitle)))
    {
        TaskLog(LOG_ERROR, "invalid Title in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("SDSInfo", stInfo.acSDSInfo, sizeof(stInfo.acSDSInfo)))
    {
        TaskLog(LOG_ERROR, "invalid SDSInfo in msg %s", szReq);
        return;
    }

    DecodeJsonMsg(stInfo.acSDSInfo, sizeof(stInfo.acSDSInfo)-1);

    BOOL bRet = m_pDao->AddSDSTemplate(stInfo);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "add sds template(%s) to db failed", stInfo.acTitle);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
    }

    SendRsp(MSG_DCAddSDSTemplate, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadSDSTemplate);

    AddOperLog("增加短信模板%s,编号:%u,内容:%s", bRet?"成功":"失败", stInfo.ulTemplateID, stInfo.acTitle);
}

VOID TaskCfg::OnDCDelSDSTemplateReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32          ulTemplateID;
    GJsonParam      JsonParam;

    if (!Json.GetValue("TemplateID", &ulTemplateID))
    {
        TaskLog(LOG_ERROR, "invalid TemplateID in msg %s", szReq);
        return;
    }

    BOOL    bRet = m_pDao->DelSDSTemplate(ulTemplateID);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "delete sds template(%u) from db failed", ulTemplateID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
    }

    SendRsp(MSG_DCDelSDSTemplate, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadSDSTemplate);

    AddOperLog("删除短信模板%u%s", ulTemplateID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCSetSDSTemplateReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    SDS_TEMPLATE_T      stInfo = {0};
    GJsonParam          JsonParam;

    if (!Json.GetValue("TemplateID", &stInfo.ulTemplateID))
    {
        TaskLog(LOG_ERROR, "invalid TemplateID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("SDSType", &stInfo.ulSDSType))
    {
        TaskLog(LOG_ERROR, "invalid SDSType in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Title", stInfo.acTitle, sizeof(stInfo.acTitle)))
    {
        TaskLog(LOG_ERROR, "invalid Title in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("SDSInfo", stInfo.acSDSInfo, sizeof(stInfo.acSDSInfo)))
    {
        TaskLog(LOG_ERROR, "invalid SDSInfo in msg %s", szReq);
        return;
    }

    DecodeJsonMsg(stInfo.acSDSInfo, sizeof(stInfo.acSDSInfo)-1);

    BOOL    bRet = m_pDao->SetSDSTemplate(stInfo);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "set sds template(%s) to db failed", stInfo.acTitle);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
    }

    SendRsp(MSG_DCSetSDSTemplate, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadSDSTemplate);

    AddOperLog("设置短信模板%s,编号:%u,内容:%s", bRet?"成功":"失败", stInfo.ulTemplateID, stInfo.acTitle);
}

VOID TaskCfg::OnDCAddRecBrdCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    REC_BRD_CFG_T       stInfo = {0};
    GJsonParam          JsonParam;

    if (!Json.GetValue("RecID", &stInfo.ulRecID))
    {
        TaskLog(LOG_ERROR, "invalid RecID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("RecInfo", stInfo.acRecInfo, sizeof(stInfo.acRecInfo)))
    {
        TaskLog(LOG_ERROR, "invalid RecInfo in msg %s", szReq);
        return;
    }

    BOOL    bRet = m_pDao->AddRecBrdCfg(stInfo);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "add rec brd cfg(%s) to db failed", stInfo.acRecInfo);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");

        AddRecBrdCfg(stInfo);
    }

    SendRsp(MSG_DCAddRecBrdCfg, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadRecBrdCfg);

    AddOperLog("增加预录音广播%s,编号:%u,内容:%s", bRet?"成功":"失败", stInfo.ulRecID, stInfo.acRecInfo);
}

VOID TaskCfg::OnDCDelRecBrdCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32          ulRecID;
    GJsonParam      JsonParam;

    if (!Json.GetValue("RecID", &ulRecID))
    {
        TaskLog(LOG_ERROR, "invalid RecID in msg %s", szReq);
        return;
    }

    BOOL    bRet = m_pDao->DelRecBrdCfg(ulRecID);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "delete rec brd cfg(%u) from db failed", ulRecID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");

        DelRecBrdCfg(ulRecID);
    }

    SendRsp(MSG_DCDelRecBrdCfg, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadRecBrdCfg);

    AddOperLog("删除预录音广播%u%s", ulRecID, bRet?"成功":"失败");
}

VOID TaskCfg::OnDCSetRecBrdCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    REC_BRD_CFG_T      stInfo = {0};
    GJsonParam          JsonParam;

    if (!Json.GetValue("RecID", &stInfo.ulRecID))
    {
        TaskLog(LOG_ERROR, "invalid RecID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("RecInfo", stInfo.acRecInfo, sizeof(stInfo.acRecInfo)))
    {
        TaskLog(LOG_ERROR, "invalid RecInfo in msg %s", szReq);
        return;
    }

    BOOL    bRet = m_pDao->SetRecBrdCfg(stInfo);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "set rec brd cfg(%s) to db failed", stInfo.acRecInfo);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");

        SetRecBrdCfg(stInfo);
    }

    SendRsp(MSG_DCSetRecBrdCfg, ulSeqID, JsonParam);
    SetCfgDataVer(m_ulLoadRecBrdCfg);

    AddOperLog("设置预录音广播%s,编号:%u,内容:%s", bRet?"成功":"失败", stInfo.ulRecID, stInfo.acRecInfo);
}

VOID TaskCfg::OnDCSaveOperLogReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonParam          JsonParam;
    CHAR                *szOperLog;
    UINT32              ulOperTime;

    szOperLog = Json.GetValue("OperLog");
    if (!szOperLog || *szOperLog == '\0')
    {
        TaskLog(LOG_ERROR, "operlog is empty in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("OperTime", &ulOperTime))
    {
        TaskLog(LOG_ERROR, "invalid OperTime in msg %s", szReq);
        return;
    }

    if (!AddOperLog(ulOperTime, szOperLog))
    {
        return;
    }

    SendConfirmSaveRsp(ulSeqID);
}

VOID TaskCfg::CheckAutoClose()
{
    INT32   iAutoCloseHour = GetLocalCfg().iAutoCloseHour;

    if (iAutoCloseHour < 0)
    {
        return;
    }

    if (gos_get_uptime() < 61*60) // 启动后1小时内不重启,避免在自动重启的时点反复重启
    {
        return;
    }

    GOS_DATETIME_T  stCurrTime;

    gos_get_localtime(NULL, &stCurrTime);
    if ((INT32)stCurrTime.ucHour != iAutoCloseHour)
    {
        return;
    }

    TaskLog(LOG_FATAL, "auto exit by system config");
    exit(0);
}

VOID TaskCfg::OnSwitchToMaster()
{
    InitCfgDataVer();

    SetTaskStatus(TASK_STATUS_INIT);
    SetTimer(TIMER_INIT, 10);

    TaskLog(LOG_FATAL, "switch to init status");
    //Init();
}

VOID TaskCfg::CheckClusterStatus()
{
    if (GetLocalCfg().ulClusterDefaultStatus == CLUSTER_STATUS_STAND_ALONE)
    {
        return;
    }

    static UINT32   ulLastStatus = CLUSTER_STATUS_STAND_ALONE;
    UINT32          ulCurrStatus = CLUSTER_STATUS_SLAVE;

    if (IsLocalAddr(GetTaskName(), GetLocalCfg().aucClusterMasterAddr))
    {
        ulCurrStatus = CLUSTER_STATUS_MASTER;
    }

    if (ulLastStatus == CLUSTER_STATUS_SLAVE &&
        ulCurrStatus == CLUSTER_STATUS_MASTER)
    {
        OnSwitchToMaster();
    }

    ulLastStatus = ulCurrStatus;
}

VOID TaskCfg::InitCfgDataVer()
{
    m_ulLoadGroupCfg = 0;
    m_ulLoadStationInfo = 0;
    m_ulLoadSectionInfo = 0;
    m_ulLoadDCUserInfo = 0;
    m_ulLoadDCOperatorInfo = 0;
    m_ulLoadStationUEInfo = 0;
    m_ulLoadPortalUEInfo = 0;
    m_ulLoadBrdZoneInfo = 0;
    m_ulLoadTrainInfo = 0;
    m_ulLoadTrainUEInfo = 0;
    m_ulLoadRecBrdCfg = 0;
    m_ulLoadDepotInfo = 0;
    m_ulLoadMultiGroupInfo = 0;
    m_ulLoadPredefinedSMS = 0;
    m_ulLoadSDSTemplate = 0;

    m_ulLoadAPPCfg = 0;
    m_ulLoadGroupInfo = 0;
    m_ulLoadAutoJoinGroupInfo = 0;
    m_ulLoadSubGroupInfo = 0;
    m_ulLoadSubVideoGroupInfo = 0;
    m_ulLoadSubUserInfo = 0;
    m_ulLoadSubVideoUserInfo = 0;

    //m_ulLoadStationPlatformInfo = 0;
    m_ulLoadDestInfo = 0;
    m_ulLoadSectionInfo = 0;

    SetCfgDataVer(m_ulLoadGroupInfo        );
    SetCfgDataVer(m_ulLoadAutoJoinGroupInfo);
    SetCfgDataVer(m_ulLoadSubGroupInfo     );
    SetCfgDataVer(m_ulLoadSubVideoGroupInfo);
    SetCfgDataVer(m_ulLoadSubUserInfo      );
    SetCfgDataVer(m_ulLoadSubVideoUserInfo );
}

VOID TaskCfg::SetCfgDataVer(UINT32 &ulVer)
{
    UINT32 ulNewVer = gos_get_current_time();

    if (ulVer < ulNewVer)
    {
        ulVer = ulNewVer;
    }
    else
    {
        ulVer++;
    }

    m_bCfgDataChanged = TRUE;

    //SendGetCfgDataVerMsg(TRUE, 0);
}

VOID TaskCfg::InitCfgDataVerMap()
{
    m_mCfgDataVer[MSG_DCLoadAPPCfg             ] = &m_ulLoadAPPCfg             ;
    m_mCfgDataVer[MSG_DCLoadGroupCfg           ] = &m_ulLoadGroupCfg           ;
    m_mCfgDataVer[MSG_DCLoadDCUserInfo         ] = &m_ulLoadDCUserInfo         ;
    m_mCfgDataVer[MSG_DCLoadDCOperatorInfo     ] = &m_ulLoadDCOperatorInfo     ;
    m_mCfgDataVer[MSG_DCLoadStationUEInfo      ] = &m_ulLoadStationUEInfo      ;
    m_mCfgDataVer[MSG_DCLoadTrainUEInfo        ] = &m_ulLoadTrainUEInfo        ;
    m_mCfgDataVer[MSG_DCLoadPortalUEInfo       ] = &m_ulLoadPortalUEInfo       ;
    m_mCfgDataVer[MSG_DCLoadGroupInfo          ] = &m_ulLoadGroupInfo          ;
    m_mCfgDataVer[MSG_DCLoadAutoJoinGroupInfo  ] = &m_ulLoadAutoJoinGroupInfo  ;
    m_mCfgDataVer[MSG_DCLoadSubGroupInfo       ] = &m_ulLoadSubGroupInfo       ;
    m_mCfgDataVer[MSG_DCLoadSubVideoGroupInfo  ] = &m_ulLoadSubVideoGroupInfo  ;
    m_mCfgDataVer[MSG_DCLoadSubUserInfo        ] = &m_ulLoadSubUserInfo        ;
    m_mCfgDataVer[MSG_DCLoadSubVideoUserInfo   ] = &m_ulLoadSubVideoUserInfo   ;
    m_mCfgDataVer[MSG_DCLoadMultiGroupInfo     ] = &m_ulLoadMultiGroupInfo     ;
    m_mCfgDataVer[MSG_DCLoadTrainInfo          ] = &m_ulLoadTrainInfo          ;
    m_mCfgDataVer[MSG_DCLoadDepotInfo          ] = &m_ulLoadDepotInfo          ;
    m_mCfgDataVer[MSG_DCLoadStationInfo        ] = &m_ulLoadStationInfo        ;
    m_mCfgDataVer[MSG_DCLoadBrdZoneInfo        ] = &m_ulLoadBrdZoneInfo        ;
    m_mCfgDataVer[MSG_DCLoadPredefinedSMS      ] = &m_ulLoadPredefinedSMS      ;
    m_mCfgDataVer[MSG_DCLoadSDSTemplate        ] = &m_ulLoadSDSTemplate        ;
    m_mCfgDataVer[MSG_DCLoadRecBrdCfg          ] = &m_ulLoadRecBrdCfg          ;
//    m_mCfgDataVer[MSG_DCLoadStationPlatformInfo] = &m_ulLoadStationPlatformInfo;
    m_mCfgDataVer[MSG_DCLoadDestInfo           ] = &m_ulLoadDestInfo;
    m_mCfgDataVer[MSG_DCLoadSectionInfo        ] = &m_ulLoadSectionInfo;
}

VOID TaskCfg::SendGetCfgDataVerMsg(BOOL bSendAllDC, UINT32 ulSeqID)
{
    GJsonParam          Param;

    if (!GetLocalCfg().bAllowDCReloadCfg)
    {
        return;
    }

    std::map<std::string, UINT32*>::iterator it = m_mCfgDataVer.begin();

    for (; it!=m_mCfgDataVer.end(); it++)
    {
        UINT32  *pulCfgVer = it->second;

        Param.Add(it->first.c_str(), *pulCfgVer);
    }

    if (bSendAllDC)
    {
        SendAllClient(DIS_CLIENT_DC, EV_DC_DIS_RSP, MSG_DCGetCfgDataVer, ulSeqID, Param);
    }
    else
    {
        SendRsp(MSG_DCGetCfgDataVer, ulSeqID, Param);
    }
}

VOID TaskCfg::OnCheckTimer()
{
    CheckClusterStatus();

    CheckAutoClose();

    m_pDao->CheckConn();
}

BOOL TaskCfg::SendDC(UINT32 ulDCUserID, UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &Param)
{
    PID_T stPID;

    if (!GetDCPID(ulDCUserID, &stPID))
    {
        TaskLog(LOG_ERROR, "DC %u has not logined", ulDCUserID);
        return FALSE;
    }

    GJsonParam ReqParam;

    ReqParam.Add("MsgName", (CHAR *)szMsgName);
    ReqParam.Add("MsgSeqID", 0);
    ReqParam.Add("MsgInfo", Param);

    CHAR *szMsg = ReqParam.GetString();

    return SendClient(&stPID, usMsgID, szMsg, strlen(szMsg) + 1);
}

VOID TaskCfg::TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    UINT32  ulTaskStatus = GetTaskStatus();

    switch(ulTaskStatus)
    {
        case TASK_STATUS_IDLE:
            switch(usMsgID)
            {
                case EV_TASK_INIT_REQ:
                    SendRsp();
                    SetTaskStatus(TASK_STATUS_INIT);
                    SetTimer(TIMER_INIT, 0);
                    break;

                default:
                    break;
            }

            break;

        case TASK_STATUS_INIT:
            switch(usMsgID)
            {
                case EV_INIT_IND:
                    if (!Init())
                    {
                        SetTimer(TIMER_INIT, 1000);
                        TaskLog(LOG_FATAL, "Init failed!");
                        gos_sleep(1);
                    }

                    break;

                default:
                    break;
            }

            break;

        case TASK_STATUS_STANDBY:
            break;

        case TASK_STATUS_WORK:
            switch(usMsgID)
            {
                case EV_CHECK_TIMER:
                    OnCheckTimer();
                    break;

                case EV_DC_DIS_CFG_REQ:
                    OnDCCfgReq((CHAR*)pvMsg, ulMsgLen);
                    break;

                case EV_DC_DIS_REQ:
                    OnDCDISReq((CHAR*)pvMsg, ulMsgLen);
                    break;

                default:
                    TaskLog(LOG_ERROR, "unknown msg %u", usMsgID);
                    break;
            }

            if (m_bCfgDataChanged)
            {
                SendGetCfgDataVerMsg(TRUE, 0);
                m_bCfgDataChanged = FALSE;
            }

            break;

        default:
            break;
    }
}

