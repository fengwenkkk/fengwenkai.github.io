#include "DISPublic.h"
#include "ats.h"
#include "pis.h"
#include "dtp_public.h"
#include "TaskAPP.h"

#include <string.h>

#pragma warning(disable : 4482)

#define CHECK_TIMER         TIMER10
#define EV_CHECK_TIMER      GET_TIMER_EV(CHECK_TIMER)

static BOOL                             g_bConstAppPort = FALSE;
static RawIntMap<PID_T>                 g_mAppPID(256);
static VECTOR<CELL_INFO_T>              g_vTmpCellInfo;
static VectorMap<UINT32, CELL_INFO_T>   g_mCurrCellInfo;
static VECTOR<AUTO_SDS_REPLY_INFO_T>    g_vAutoSDSReplyInfo;
static VECTOR<REQ_CALL_INFO_T>          g_vAPPRequestCallInfo;
static BOOL                             g_bAPPMsgUseAnsiString = FALSE;

extern BOOL dtp_msg_hton(DTP_MSG_HDR_T *pstMsgHdr);

REQ_CALL_INFO_T* GetReqCallInfo(UINT32 ulReqUserID, UINT32 ulReqCallType)
{
    for (UINT32 i=0; i<g_vAPPRequestCallInfo.size(); i++)
    {
        if (g_vAPPRequestCallInfo[i].ulReqUserID == ulReqUserID &&
            g_vAPPRequestCallInfo[i].ulReqCallType == ulReqCallType)
        {
            return &g_vAPPRequestCallInfo[i];
        }
    }

    return NULL;
}

VOID DelReqCallInfo(UINT32 ulReqUserID, UINT32 ulReqCallType)
{
    for (UINT32 i=0; i<g_vAPPRequestCallInfo.size(); i++)
    {
        if (g_vAPPRequestCallInfo[i].ulReqUserID == ulReqUserID &&
            g_vAPPRequestCallInfo[i].ulReqCallType == ulReqCallType)
        {
            g_vAPPRequestCallInfo.erase(g_vAPPRequestCallInfo.begin()+i);
            return;
        }
    }
}

VOID TaskAPP::AddOperLog(const CHAR *szLog, ...)
{
    if (!m_pDao)
    {
        return;
    }

    CHAR            acLog[4096];
    va_list         vaLog;

    va_start(vaLog, szLog);

    memset(acLog, 0, sizeof(acLog));
    vsnprintf(acLog, sizeof(acLog)-1, szLog, vaLog);

    va_end(vaLog);

    m_pDao->AddOperLog(gos_get_current_time(), m_ulDCUserID, m_acDCOperatorID, acLog);
}

/*
BOOL TaskAPP::InitAPPCfg()
{
    APP_BASIC_CFG_T     stAPPBasicCfg = {0};

    if (!m_pDao->GetAppCfg(&stAPPBasicCfg))
    {
        TaskLog(LOG_ERROR, "load app cfg failed");
        return FALSE;
    }

    APP_BASIC_CFG_T     stOriAPPBasicCfg = {0};

    GetAPPBasicCfg(stOriAPPBasicCfg);

    if (memcmp(&stOriAPPBasicCfg, &stAPPBasicCfg, sizeof(stAPPBasicCfg)) != 0)
    {
        InitAPPBasicCfg(stAPPBasicCfg);
        TaskLog(LOG_INFO, "load app cfg successfully");
    }

    return TRUE;
}*/

TaskAPP::TaskAPP(UINT16 usDispatchID):GBaseTask(MODULE_APP, "APP", usDispatchID)
{
    g_bAPPMsgUseAnsiString = GetLocalCfg().bAPPMsgUseAnsiString;

    m_ulDCUserID = INVALID_USER_ID;
    m_acDCOperatorID[0] = '\0';

    m_pDao = NULL;
    //m_bInitAppCfg = FALSE;
    m_stAppSock = INVALID_SOCKET;

    RegisterMsg(EV_APP_DIS_REQ);
    RegisterMsg(EV_APP_DIS_DC_RSP);
    RegisterMsg(EV_DC_DIS_APP_REQ);
}

TaskAPP::~TaskAPP()
{
}

VOID TaskAPP::LogAPPMsg(const CHAR *szMsgName, const CHAR *szMsg)
{
    if (strcmp(szMsgName, MSG_APP_GetDSCfgInfoRsp) == 0 ||
        strcmp(szMsgName, MSG_APP_GetNTPCfgRsp) == 0 ||
        strcmp(szMsgName, MSG_APP_GetATSInfoRsp) == 0 ||
        strcmp(szMsgName, MSG_APP_GetStationATSInfoRsp) == 0 )
    {
        TaskLog(LOG_DETAIL, "send APP Msg: %s", szMsg);
    }
    else
    {
        TaskMsgLog(LOG_INFO, "send APP Msg: %s", szMsg);
    }
}

BOOL TaskAPP::SendAPPRsp(const CHAR *szMsgName, CHAR *szRspMsg)
{
    GJsonParam      ReqParam;
    BOOL            bRet;

    ReqParam.Add("MsgName", (CHAR*)szMsgName);
    ReqParam.Add("MsgSeqID", m_ulAPPSeqID);
    ReqParam.Add("MsgInfo", szRspMsg, FALSE);

    LogAPPMsg(szMsgName, ReqParam.GetString());

    CHAR    *szMsg = ReqParam.GetString();

    if (g_bAPPMsgUseAnsiString)
    {
        return SendRsp(szMsg);
    }

    CHAR    *szUtf8Msg = (CHAR*)calloc(1, strlen(szMsg)*4 + 16);

    if (!szUtf8Msg)
    {
        return FALSE;
    }

    gos_ascii_to_utf8(szMsg, szUtf8Msg);

    if (GetLocalCfg().bDirectSendApp)
    {
        UINT16      usMsgID = GetMsgID();

        bRet = DirectSendToApp(EV_RSP(usMsgID), szUtf8Msg, strlen(szUtf8Msg)+1);
    }
    else
    {
        bRet = SendRsp(szUtf8Msg);
    }

    free(szUtf8Msg);

    return bRet;
}

BOOL TaskAPP::SendAPPReq(const CHAR *szMsgName, UINT32 ulUserID, GJsonParam &Param)
{
    return SendAPPReq(szMsgName, ulUserID, m_ulAPPSeqID, Param);
}

BOOL TaskAPP::SendAPPReq(const CHAR *szMsgName, UINT32 ulUserID, UINT32 ulSeqID, CHAR *szReqMsg)
{
    PID_T   *pstAPPPID = g_mAppPID.GetValue(ulUserID);

    if (!pstAPPPID)
    {
        TaskLog(LOG_ERROR, "unknown APP user %u", ulUserID);
        return FALSE;
    }

    GJsonParam      ReqParam;

    ReqParam.Add("MsgName", (CHAR*)szMsgName);
    ReqParam.Add("MsgSeqID", ulSeqID);
    ReqParam.Add("MsgInfo", szReqMsg, FALSE);

//    TaskLog(LOG_DETAIL, "send msg %s to APP %u", szMsgName, ulUserID);
    LogAPPMsg(szMsgName, ReqParam.GetString());

    CHAR    *szMsg = ReqParam.GetString();

    if (g_bAPPMsgUseAnsiString)
    {
        return SendClient(pstAPPPID, EV_APP_DIS_RSP, szMsg, strlen(szMsg)+1);
    }

    CHAR    *szUtf8Msg = (CHAR*)calloc(1, strlen(szMsg)*4 + 16);

    if (!szUtf8Msg)
    {
        return FALSE;
    }

    gos_ascii_to_utf8(szMsg, szUtf8Msg);

    BOOL bRet;

    if (GetLocalCfg().bDirectSendApp)
    {
        bRet = DirectSendToApp(EV_APP_DIS_RSP, szUtf8Msg, strlen(szUtf8Msg)+1);
    }
    else
    {
        bRet = SendClient(pstAPPPID, EV_APP_DIS_RSP, szUtf8Msg, strlen(szUtf8Msg)+1);
    }

    free(szUtf8Msg);

    return bRet;
}

BOOL TaskAPP::DirectSendToApp(UINT16 usMsgType, CHAR *szMsg, UINT32 ulMsgLen)
{
    DTP_MSG_T       *pstMsg = (DTP_MSG_T*)AllocBuffer(sizeof(DTP_MSG_T));
    UINT32          ulTotalMsgLen = sizeof(DTP_MSG_HDR_T)+ulMsgLen;
    SOCKADDR_IN     stAppAddr = {0};

    pstMsg->stMsgHdr.ulMsgMagic = DTP_MSG_MAGIC;
    pstMsg->stMsgHdr.usMsgVer = DTP_MSG_VER;

    pstMsg->stMsgHdr.usSenderModule = 0xffff;
    pstMsg->stMsgHdr.usReceiverModule = 0xffff;
    pstMsg->stMsgHdr.ulMsgLen  = ulTotalMsgLen;
    pstMsg->stMsgHdr.ulSeqID   = m_ulAPPSeqID;
    pstMsg->stMsgHdr.usMsgType = usMsgType;
    pstMsg->stMsgHdr.ucMsgPri  = 0;

    pstMsg->stMsgHdr.ucCompType = DTP_COMPRESS_TYPE_NONE;
    pstMsg->stMsgHdr.ucEncryptType = DTP_ENCRYPT_TYPE_NONE;

    pstMsg->stMsgHdr.usTimeout = 0;
    pstMsg->stMsgHdr.ucSenderType = 0;

    dtp_msg_hton(&pstMsg->stMsgHdr);

    if (ulMsgLen > 0)
    {
        memcpy(pstMsg->aucMsgBody, szMsg, ulMsgLen);
    }

    UINT8       aucClientAddr[4];
    UINT16      usClientPort = 0;
    UINT16      usClientID = GetMsgSenderInstID();

    GosGetClientAddr(usClientID, aucClientAddr, &usClientPort);
    if (g_bConstAppPort)
    {
        usClientPort = DEFAULT_APP_UDP_PORT;
    }

    stAppAddr.sin_family = AF_INET;
    stAppAddr.sin_port   = HTONS(usClientPort);
    memcpy(&stAppAddr.sin_addr.s_addr, aucClientAddr, 4);

    if (m_stAppSock == INVALID_SOCKET)
    {
        m_stAppSock = gos_create_udp_socket();
    }

    if (!gos_udp_send(m_stAppSock, (void *)pstMsg, ulTotalMsgLen, &stAppAddr))
    {
        TaskLog(LOG_ERROR, "send msg to app failed: %s", gos_get_socket_err());
        return FALSE;
    }

    return TRUE;
}

BOOL TaskAPP::Init()
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

/*    if (!m_bInitAppCfg)
    {
        if (!InitAPPCfg())
        {
            return FALSE;
        }

        m_bInitAppCfg = TRUE;
    }*/

    SetLoopTimer(CHECK_TIMER, 10*1000);

    SetTaskStatus(TASK_STATUS_WORK);

    TaskLog(LOG_INFO, "Task APP init successful!");

    CheckRegister();

    return TRUE;
}

VOID TaskAPP::OnServerDisconnectClient(UINT16 usClientID)
{
}

VOID TaskAPP::OnAppDisReq(CHAR *szMsg, UINT32 ulMsgLen)
{
    GJson       Json;
    GJson       SubJson;
    BOOL        bRet;
    CHAR        *szMsgType;
    CHAR        *szMsgInfo;
    CHAR        *szAnsiMsg = (CHAR*)AllocBuffer(64*1024);

    if (!g_bAPPMsgUseAnsiString)
    {
        gos_utf8_to_ascii(szMsg, szAnsiMsg);
        szMsg = szAnsiMsg;
    }

    bRet = Json.Parse(szMsg);
    if (!bRet)
    {
        TaskLog(LOG_ERROR, "parse MsgInfo failed: %s", szMsg);
        goto error;
    }

    szMsgType = Json.GetValue("MsgName");

    if (!szMsgType)
    {
        TaskLog(LOG_ERROR, "get MsgName failed: %s", szMsg);
        goto error;
    }

    if (!Json.GetValue("MsgSeqID", &m_ulAPPSeqID))
    {
        TaskLog(LOG_ERROR, "get MsgSeqID failed: %s", szMsg);
        goto error;
    }

    szMsgInfo = Json.GetValue("MsgInfo");
    if (!szMsgInfo)
    {
        TaskLog(LOG_ERROR, "get MsgInfo failed: %s", szMsg);
        goto error;
    }

    if (!SubJson.Parse(szMsgInfo))
    {
        TaskLog(LOG_ERROR, "parse MsgInfo failed: %s", szMsg);
        goto error;
    }

    // 以下消息为周期性消息
    if (strcmp(szMsgType, MSG_APP_GetStationATSInfoReq) == 0 ||
        strcmp(szMsgType, MSG_APP_GetDSCfgInfoReq) == 0 ||
        strcmp(szMsgType, MSG_APP_GetNTPCfgReq) == 0 ||
        strcmp(szMsgType, MSG_APP_GetATSInfoReq) == 0 ||
        strcmp(szMsgType, MSG_APP_ReportTrainInfo) == 0 )
    {
        TaskLog(LOG_DETAIL, "get APP msg: %s", szMsg);
    }
    else
    {
        // 其他为业务消息，单独存储
        TaskMsgLog(LOG_INFO, "get APP msg: %s", szMsg);
    }

    if (strcmp(szMsgType, MSG_APP_GetDSCfgInfoReq) == 0)
    {
        OnAppGetDSCfgInfoReq(szMsg, SubJson);
    }
    else if (strcmp(szMsgType, MSG_APP_GetNTPCfgReq) == 0)
    {
        OnAppGetNTPCfgReq(szMsg, SubJson);
    }
    else if (strcmp(szMsgType, MSG_APP_SetTrainGroupReq) == 0)
    {
        OnAppSetTrainGroupReq(szMsg, SubJson);
    }
    else if (strcmp(szMsgType, MSG_APP_SetTrainPos) == 0 || //TODO
             strcmp(szMsgType, MSG_APP_SetTrainPosReq) == 0)
    {
        OnAppSetTrainPosReq(szMsg, SubJson);
    }
    else if (strcmp(szMsgType, MSG_APP_RegisterReq) == 0)
    {
        OnAppRegisterReq(szMsg, SubJson);
    }
    else if (strcmp(szMsgType, MSG_APP_RequestCallReq) == 0)
    {
        OnAppRequestCallReq(szMsg, SubJson);
    }
    else if (strcmp(szMsgType, MSG_APP_GetATSInfoReq) == 0)
    {
        OnAppGetATSInfoReq(szMsg, SubJson);
    }
    else if (strcmp(szMsgType, MSG_APP_GetStationATSInfoReq) == 0)
    {
        OnAppGetStationATSInfoReq(szMsg, SubJson);
    }
    else if (strcmp(szMsgType, MSG_APP_ReportTrainInfo) == 0)
    {
        OnAppReportTrainInfo(szMsg, SubJson);
    }
    else if (strcmp(szMsgType, MSG_APP_BroadcastStartRsp) == 0)
    {
        OnAppBroadcastStartRsp(szMsg, SubJson);
    }
    else if (strcmp(szMsgType, MSG_APP_SDSReplyReq) == 0)
    {
        OnAppSDSReplyReq(szMsg, SubJson);
    }
    else if (strcmp(szMsgType, MSG_APP_AutoSDSReplyRsp) == 0)
    {
        OnAppAutoSDSReplyRsp(szMsg, SubJson);
    }
    else if (strcmp(szMsgType, MSG_APP_EmergencyCallReq) == 0)
    {
        OnAppEmergencyCallReq(szMsg, SubJson);
    }
    else
    {
        TaskLog(LOG_ERROR, "invalid APP msg: %s", szMsg);
        goto error;
    }

    return;

error:
    return;
}

// {"ulUserID":"123"}
VOID TaskAPP::OnAppGetDSCfgInfoReq(CHAR *szMsgType, GJson &Json)
{
    UINT32          ulUserID;
    GJsonParam      Param;
    GJsonTupleParam TupleParam;
    BOOL            bTrain = TRUE;
    TRAIN_UE_INFO_T stTrainUEInfo = {0};
    STATION_UE_INFO_T   stStationUEInfo = {0};
    STATION_INFO_T      stStationInfo = {0};
    APP_BASIC_CFG_T     stAPPBasicCfg;

    GetAPPBasicCfg(stAPPBasicCfg);

    if (!Json.GetValue("UserID", &ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szMsgType);
        return;
    }

    if (!GetTrainUEInfo(ulUserID, &stTrainUEInfo))
    {
        if (!GetStationUEInfo(ulUserID, &stStationUEInfo))
        {
            TaskLog(LOG_ERROR, "unknown UserID(%u) in msg %s", ulUserID, szMsgType);
            return;
        }

        if (!GetStationInfo(stStationUEInfo.ulStationID, &stStationInfo))
        {
            TaskLog(LOG_ERROR, "unknown station(%u)", stStationUEInfo.ulStationID);
            return;
        }

        bTrain = FALSE;
    }

    PID_T   *pstAPPPID = GetMsgSender();
    LOCAL_CFG_T &stLocalCfg = GetLocalCfg();

    g_mAppPID.Add(ulUserID, pstAPPPID);

    Param.Add("MaxSelfCheckNum", stLocalCfg.ulMaxAppSelfCheckNum);
    Param.Add("AllowEmergencyCall", stLocalCfg.bAllowEmergencyCall?"true":"false");
    Param.Add("PTTAllow", stAPPBasicCfg.bPTTAllow?"true":"false");
    Param.Add("BroascastTimeout", stAPPBasicCfg.ulBroadcastTimeout); // TODO 后续要删除
    Param.Add("BroadcastTimeout", stAPPBasicCfg.ulBroadcastTimeout);
    Param.Add("BroadcastVolume", stAPPBasicCfg.ulBroadcastVolume);
    Param.Add("HangoffTime", stAPPBasicCfg.ulHangoffTime);
    Param.Add("NTPServerAddr", stAPPBasicCfg.acNTPServerAddr);
    Param.Add("NTPServerPort", stAPPBasicCfg.ulNTPServerPort);
    Param.Add("NTPSyncPeriod", stAPPBasicCfg.ulNTPSyncPeriod);

    Param.Add("AutoLockTimeout", stLocalCfg.ulTXAutoLockTimeout);
    Param.Add("LockPwd", gos::base64::EncodeString(stLocalCfg.acTXLockPwd));

    if (bTrain)
    {
        VECTOR<DEPOT_INFO_T>  vDepotInfo;

        GetDepotInfo(vDepotInfo);

        TupleParam.Clear();
        for (UINT32 i=0; i<vDepotInfo.size(); i++)
        {
            GJsonParam  SubParam;

            SubParam.Add("TrainPosID", vDepotInfo[i].ulDepotID);
            SubParam.Add("TrainPosName", vDepotInfo[i].acDepotName);

            TupleParam.Add(SubParam);
        }

        Param.Add("TrainPos", TupleParam);
    }

    if (bTrain)
    {
        VECTOR<STATION_INFO_T>  vStationInfo;

        GetStationInfo(vStationInfo);

        TupleParam.Clear();
        for (UINT32 i=0; i<vStationInfo.size(); i++)
        {
            GJsonParam  SubParam;

            SubParam.Add("StationID", vStationInfo[i].ulStationID);
            SubParam.Add("StationName", vStationInfo[i].acStationName);
            SubParam.Add("TrainPosID", vStationInfo[i].ulDepotID);

            TupleParam.Add(SubParam);
        }

        Param.Add("Station", TupleParam);
    }

    TupleParam.Clear();

    if (bTrain)
    {
        TRAIN_INFO_T        stTrainInfo;

        if (!GetTrainInfo(stTrainUEInfo.ulTrainUnitID, stTrainInfo))
        {
            TaskLog(LOG_ERROR, "invalid train unit id %u", stTrainUEInfo.ulTrainUnitID);
            return;
        }

        GROUP_CFG_T     stGroupCfg = {0};

        if (!GetGroupCfg(&stGroupCfg))
        {
            TaskLog(LOG_ERROR, "get group cfg failed");
            return;
        }

        INT64   i64MainLineGroupID = stTrainInfo.i64DefaultGroupID;
        INT64   i64DepotGroupID = stTrainInfo.i64DefaultGroupID;

        Param.Add("TrainType", stTrainInfo.ulTrainType);
        Param.Add("TrainName", stTrainInfo.acTrainName);
        Param.Add("MainLineGroupID", i64MainLineGroupID);
        Param.Add("DepotGroupID", i64DepotGroupID);
        Param.Add("UplineTrainGroupID", stGroupCfg.i64UpLineTrainGroupID);
        Param.Add("DownlineTrainGroupID", stGroupCfg.i64DownLineTrainGroupID);
        Param.Add("AlllineTrainGroupID", stGroupCfg.i64MainLineTrainGroupID);

        VECTOR<INT64>   vGroup;

        vGroup.push_back(stGroupCfg.i64UpLineTrainBrdGroupID);
        vGroup.push_back(stGroupCfg.i64DownLineTrainBrdGroupID);
        vGroup.push_back(stGroupCfg.i64MainLineTrainBrdGroupID);

        for (UINT32 i=0; i<vGroup.size(); i++)
        {
            GJsonParam  SubParam;

            SubParam.Add("GroupID", vGroup[i]);
            TupleParam.Add(SubParam);
        }

        Param.Add("TrainBrdGroup", TupleParam);
    }
    else
    {
        INT64   i64StationGroupID = stStationUEInfo.i64DefaultGroupID;

        Param.Add("StationID", stStationInfo.ulStationID);
        Param.Add("StationName", stStationInfo.acStationName);
        Param.Add("StationGroupID", i64StationGroupID);
        Param.Add("StationInnerGroupID", i64StationGroupID);

        VECTOR<BRD_ZONE_INFO_T>     vZoneBdrInfo;

        GetBrdZoneInfo(stStationUEInfo.ulStationID, vZoneBdrInfo);

        for (UINT32 i=0; i<vZoneBdrInfo.size(); i++)
        {
            GJsonParam  SubParam;

            SubParam.Add("ZoneID", vZoneBdrInfo[i].ulZoneID);
            SubParam.Add("ZoneName", vZoneBdrInfo[i].acZoneName);
            SubParam.Add("GroupID", vZoneBdrInfo[i].i64GroupID);
            TupleParam.Add(SubParam);
        }

        Param.Add("StationBrdZone", TupleParam);

        Param.Add("FXAutoRebootPeriod", stAPPBasicCfg.ulFXAutoRebootPeriod);
        Param.Add("FXAutoRebootTime", stAPPBasicCfg.ulFXAutoRebootTime);
    }

    TupleParam.Clear();

    VECTOR<PREDEFINED_SMS_INFO_T>   vPredefinedSMS;

    GetPredefinedSMS(vPredefinedSMS);
    for (UINT32 i=0; i<vPredefinedSMS.size(); i++)
    {
        if (bTrain)
        {
            if (!(vPredefinedSMS[i].ulSMSTemplateType & SDS_TEMPLATE_TX))
            {
                continue;
            }
        }
        else
        {
            if (!(vPredefinedSMS[i].ulSMSTemplateType & SDS_TEMPLATE_FX))
            {
                continue;
            }
        }

        GJsonParam  SubParam;

        SubParam.Add("SMSID", vPredefinedSMS[i].ulSMSID);
        SubParam.Add("SDSType", vPredefinedSMS[i].ulSDSType);
        SubParam.Add("SMSInfo", vPredefinedSMS[i].acSMSInfo);
        TupleParam.Add(SubParam);
    }

    Param.Add("PredefinedSMS", TupleParam);

    if (bTrain)
    {
        Param.Add("AllowP2PCall", stLocalCfg.bAllowTXP2PCall?"true":"false");
    }
    else
    {
        Param.Add("AllowP2PCall", stLocalCfg.bAllowFXP2PCall?"true":"false");
    }

    SendAPPRsp(MSG_APP_GetDSCfgInfoRsp, Param);

    // 所有的APP消息统一集中打印，不要在函数内部再打印
    //TaskLog(LOG_DETAIL, "UserID(%u) get DS cfg successful", ulUserID);
}

VOID TaskAPP::OnAppGetNTPCfgReq(CHAR *szMsgInfo, GJson &Json)
{
    GJsonParam      Param;
    APP_BASIC_CFG_T stAPPBasicCfg;

    GetAPPBasicCfg(stAPPBasicCfg);
    Param.Add("NTPServerAddr", stAPPBasicCfg.acNTPServerAddr);
    Param.Add("NTPServerPort", stAPPBasicCfg.ulNTPServerPort);
    Param.Add("NTPSyncPeriod", stAPPBasicCfg.ulNTPSyncPeriod);
    Param.Add("Time", gos_get_current_time());

    SendAPPRsp(MSG_APP_GetNTPCfgRsp, Param);
}

UINT32 TaskAPP::GetReqCallDCType(UINT32 ulDCType, UINT32 ulTrainUnitID)
{
    TRAIN_INFO_T    stTrain;
    STATION_INFO_T  stStation;

    if (GetLocalCfg().iTransferTrackReqCallMode == 0 ||
        ulDCType != DC_TYPE_ALL)
    {
        return ulDCType;
    }

    if (!GetTrainInfo(ulTrainUnitID, stTrain))
    {
        TaskLog(LOG_ERROR, "invalid train %u of reqcall", ulTrainUnitID);
        return DC_TYPE_NONE;
    }

    if (!GetStationInfo(stTrain.ulStationID, &stStation))
    {
        TaskLog(LOG_ERROR, "unknown position of train %u", ulTrainUnitID);
        return DC_TYPE_NONE;
    }

    if (!stTrain.ulTransferTrackFlag)
    {
        if (stStation.ulDepotID == DEPOT_ID_MAINLINE)
        {
            ulDCType = DC_TYPE_TRAIN;   // 正线驶入转换轨时，缺省呼叫正线
        }
        else
        {
            ulDCType = DC_TYPE_DEPOT | DC_TYPE_FIX;
        }

        return ulDCType;
    }

    TaskLog(LOG_INFO, "train %u is on transfer track", ulTrainUnitID);

    if (GetLocalCfg().iTransferTrackReqCallMode > 0)
    {
        if (stStation.ulDepotID == DEPOT_ID_MAINLINE)
        {
            ulDCType = DC_TYPE_TRAIN;   // 正线驶入转换轨时，缺省呼叫正线
        }
        else
        {
            ulDCType = DC_TYPE_DEPOT | DC_TYPE_FIX;
        }
    }
    else
    {
        if (stStation.ulDepotID == DEPOT_ID_MAINLINE)
        {
            ulDCType = DC_TYPE_DEPOT | DC_TYPE_FIX;   // 正线驶入转换轨时，缺省呼叫车辆段
        }
        else
        {
            ulDCType = DC_TYPE_TRAIN;
        }
    }

    return ulDCType;
}

VOID TaskAPP::OnAppRequestCallReq(CHAR *szMsgType, GJson &Json)
{
    CHAR        *szDCType;
    CHAR        *szTrainList;
    UINT32      ulTrainUnitID;
    VECTOR<UINT32>      vTrainList;
    REQ_CALL_INFO_T     stInfo = {0};

    if (!Json.GetValue("UserID", &stInfo.ulReqUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("CallType", &stInfo.ulReqCallType))
    {
        TaskLog(LOG_ERROR, "invalid CallType in msg %s: %s", szMsgType, Json.GetText());
        return;
    }

    stInfo.ulDCType = DC_TYPE_ALL;
    szDCType = Json.GetValue("DCType");
    if (szDCType)
    {
        if (!gos_atou(szDCType, &stInfo.ulDCType))
        {
            TaskLog(LOG_ERROR, "invalid DCType in msg %s: %s", szMsgType, Json.GetText());
            return;
        }
    }

    if (stInfo.ulDCType == DC_TYPE_NONE)
    {
        stInfo.ulDCType = DC_TYPE_ALL;
    }

    if (stInfo.ulReqCallType == STATION_REQ_CALL_TRAIN)
    {
        szTrainList = Json.GetValue("TrainList");
        if (!szTrainList)
        {
            TaskLog(LOG_ERROR, "no TrainList in msg %s: %s", szMsgType, Json.GetText());
            return;
        }

        GJson       TrainJson;

        TrainJson.Parse(szTrainList);

        VECTOR<GJson*>      &vSubJson = TrainJson.GetSubJson();

        for (UINT32 i=0; i<vSubJson.size(); i++)
        {
            if (!vSubJson[i]->GetValue("TrainUnitID", &ulTrainUnitID))
            {
                TaskLog(LOG_ERROR, "invalid TrainUnitID in msg %s: %s", szMsgType, Json.GetText());
                return;
            }

            vTrainList.push_back(ulTrainUnitID);
        }
    }

    stInfo.ulReqTime = gos_get_current_time();
    stInfo.ulDCUserID = INVALID_USER_ID;

    if (stInfo.ulReqCallType == TRAIN_REQ_CALL_DC ||
        stInfo.ulReqCallType == TRAIN_EMERGENCY_REQ_CALL_DC ||
        stInfo.ulReqCallType == TRAIN_REQ_CALL_STATION )
    {
        TRAIN_UE_INFO_T     stUserInfo;

        if (!GetTrainUEInfo(stInfo.ulReqUserID, &stUserInfo))
        {
            TaskLog(LOG_ERROR, "unknown train user(%u) in msg %s: %s", stInfo.ulReqUserID, szMsgType, Json.GetText());
            return;
        }

        stInfo.ulCaller = stUserInfo.ulTrainUnitID;

        if (stInfo.ulReqCallType == TRAIN_REQ_CALL_DC ||
            stInfo.ulReqCallType == TRAIN_EMERGENCY_REQ_CALL_DC )
        {
            stInfo.ulDCType = GetReqCallDCType(stInfo.ulDCType, stUserInfo.ulTrainUnitID);
        }
    }
    else if (stInfo.ulReqCallType == STATION_REQ_CALL_DC ||
             stInfo.ulReqCallType == STATION_EMERGENCY_REQ_CALL_DC ||
             stInfo.ulReqCallType == STATION_REQ_CALL_TRAIN)
    {
        STATION_UE_INFO_T     stUserInfo;

        if (!GetStationUEInfo(stInfo.ulReqUserID, &stUserInfo))
        {
            TaskLog(LOG_ERROR, "invalid station user(%u) in msg %s: %s", stInfo.ulReqUserID, szMsgType, Json.GetText());
            return;
        }

        stInfo.ulCaller = stUserInfo.ulStationID;
    }
    else
    {
        TaskLog(LOG_ERROR, "invalid req call type(%u) in msg %s: %s", stInfo.ulReqCallType, szMsgType, Json.GetText());
        return;
    }

    m_pDao->AddReqCallInfo(stInfo);
    GJsonParam  Param;

    Param.Add("ReqTime", gos_get_current_time());
    Param.Add("UserID", stInfo.ulReqUserID);
    Param.Add("CallType", stInfo.ulReqCallType);
    Param.Add("DCType", stInfo.ulDCType);

    if (stInfo.ulReqCallType == STATION_REQ_CALL_TRAIN)
    {
        GJsonTupleParam     TupleParam;
        GJsonParam          TrainParam;

        for (UINT32 i=0; i<vTrainList.size(); i++)
        {
            TrainParam.Clear();
            TrainParam.Add("TrainUnitID", vTrainList[i]);
            TupleParam.Add(TrainParam);
        }

        Param.Add("TrainList", TupleParam);
    }

    SendAllClient(DIS_CLIENT_DC, EV_APP_DIS_DC_REQ, MSG_APP_RequestCallReq, m_ulAPPSeqID, Param);

    TaskLog(LOG_INFO, "get request call from %u, call type is %u", stInfo.ulReqUserID, stInfo.ulReqCallType);
}

VOID TaskAPP::OnAppSetTrainGroupReq(CHAR *szMsgType, GJson &Json)
{
}

VOID TaskAPP::OnAppRegisterReq(CHAR *szMsgType, GJson &Json)
{
    UINT32      ulTrainUnitID;
    CHAR        *szValue;
    UINT32      ulDepotID = DEPOT_ID_UNKNOWN;
    BOOL        bResult = FALSE;
    GJsonParam  Param;
    BOOL        bAutoRegister = FALSE;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "invalid train UnitID in msg %s", szMsgType);
        return;
    }

    szValue = Json.GetValue("DepotID");
    if (szValue)
    {
        if (!gos_atou(szValue, &ulDepotID))
        {
            TaskLog(LOG_ERROR, "invalid DepotID(%s) of train(%u) in msg %s", szValue, ulTrainUnitID, szMsgType);
            return;
        }
    }
    else
    {
        szValue = Json.GetValue("DCType");
        if (!gos_atou(szValue, &ulDepotID))
        {
            TaskLog(LOG_ERROR, "invalid DCType(%s) of train(%u) in msg %s", szValue, ulTrainUnitID, szMsgType);
            return;
        }
    }

    if (ulDepotID == DEPOT_ID_AUTO)
    {
        if (!GetTrainRegisterDepotID(ulTrainUnitID, &ulDepotID))
        {
            TaskLog(LOG_ERROR, "get registered depot of train(%u) failed", ulTrainUnitID);
            return;
        }

        bAutoRegister = TRUE;
    }

    SetRegisterInfo(ulTrainUnitID, TRUE, ulDepotID, bAutoRegister);

    TaskLog(LOG_INFO, "train %u is %s registering to depot %u", ulTrainUnitID, bAutoRegister?"auto":"", ulDepotID);

    if (bAutoRegister)
    {
        bResult = TRUE;
    }

    Param.Add("TrainUnitID", ulTrainUnitID);
    Param.Add("DepotID", ulDepotID);
    Param.Add("AutoRegister", bAutoRegister);

    SendAllClient(DIS_CLIENT_DC, EV_APP_DIS_DC_REQ, MSG_APP_RegisterReq, m_ulAPPSeqID, Param);
    bResult = TRUE;

    if (bAutoRegister)
    {
        Param.Clear();
        Param.Add("TrainUnitID", ulTrainUnitID);
        Param.Add("Result", bResult);
        Param.Add("TrainPos", ulDepotID);

        SendAPPRsp(MSG_APP_RegisterRsp, Param);
    }
}

// {"TrainUnitID":"123"}
VOID TaskAPP::OnAppGetATSInfoReq(CHAR *szMsgType, GJson &Json)
{
    GJsonParam      Param;
    TRAIN_INFO_T    stATSInfo = {0};
    UINT32          ulTrainUnitID = 0;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in %s", szMsgType);
        return;
    }

    if (!GetTrainInfo(ulTrainUnitID, stATSInfo))
    {
        TaskLog(LOG_ERROR, "get ATS info of train %d failed", ulTrainUnitID);
        return;
    }

    UINT32      ulPositionID;
    CHAR        acPositionName[64];
    CHAR        acStationName[128];
    CHAR        acDestStationName[128] = {0};
    UINT32      ulTime = gos_get_uptime() - stATSInfo.ulUptime;
    DEST_INFO_T    stDestInfo;

    GetTrainPosition(&stATSInfo, &ulPositionID, acPositionName);
    GetStationName(stATSInfo.ulStationID, acStationName);
    if (GetDestInfo(stATSInfo.acDestID, &stDestInfo))
    {
        GetStationName(stDestInfo.ulStationID, acDestStationName);
    }

    Param.Add("Time", ulTime);
    Param.Add("TrainUnitID", stATSInfo.ulTrainUnitID);
    Param.Add("TrainID", stATSInfo.acTrainID);
    Param.Add("DefaultGroupID", stATSInfo.i64DefaultGroupID);
    Param.Add("PositionID", ulPositionID);
    Param.Add("PositionName", acPositionName);
    Param.Add("Direction", stATSInfo.ulDirection);
    Param.Add("StationID", stATSInfo.ulStationID);
    Param.Add("StationName", acStationName);
    Param.Add("DestID", stATSInfo.acDestID);
//    Param.Add("DestName", acDestName);
    Param.Add("DestName", acDestStationName);//stDestStationInfo.acDestName);
    Param.Add("TransferTrack", stATSInfo.ulTransferTrackFlag);

    SendAPPRsp(MSG_APP_GetATSInfoRsp, Param);
}

VOID TaskAPP::OnAppGetStationATSInfoReq(CHAR *szMsgType, GJson &Json)
{
    GJsonParam      Param;
    GJsonTupleParam TupleParam;
    UINT32          ulStationID = 0;
    VECTOR<TRAIN_INFO_T>    vTrain;

    if (!Json.GetValue("StationID", &ulStationID))
    {
        TaskLog(LOG_ERROR, "invalid StationID in %s", szMsgType);
        return;
    }

    GetTrainInfoByStationID(ulStationID, vTrain);

    for (UINT32 i=0; i<vTrain.size(); i++)
    {
        Param.Clear();

        Param.Add("TrainUnitID", vTrain[i].ulTrainUnitID);
        Param.Add("TrainID", vTrain[i].acTrainID);
        Param.Add("TrainDirection", vTrain[i].ulDirection);
        Param.Add("UpsideStationID", vTrain[i].ulUpsideStationID);
        Param.Add("DownsideStationID", vTrain[i].ulDownsideStationID);

        TupleParam.Add(Param);
    }

    Param.Clear();
    Param.Add("TrainList", TupleParam);

    SendAPPRsp(MSG_APP_GetStationATSInfoRsp, Param);
}

VOID TaskAPP::OnAppReportTrainInfo(CHAR *szMsgType, GJson &Json)
{
    APP_TRAIN_INFO_T    stInfo = {0};
    TRAIN_INFO_T        stTrain;
    CHAR                *szValue;

    memset(&stInfo, 0, sizeof(stInfo));

    if (!Json.GetValue("UserID", &stInfo.ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("TC", &stInfo.ulTC))
    {
        TaskLog(LOG_ERROR, "invalid TC in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("TrainUnitID", &stInfo.ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("TrainID", stInfo.acTrainID, sizeof(stInfo.acTrainID)))
    {
        TaskLog(LOG_ERROR, "invalid TrainID in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("CurrentGroupID", &stInfo.i64CurrentGroupID))
    {
        TaskLog(LOG_ERROR, "invalid CurrentGroupID in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("StationID", &stInfo.ulStationID))
    {
        TaskLog(LOG_ERROR, "invalid StationID in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("CoverStatus", &stInfo.ulCoverStatus))
    {
        TaskLog(LOG_ERROR, "invalid CoverStatus in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("DriverID", &stInfo.ulDriverID))
    {
        TaskLog(LOG_ERROR, "invalid DriverID in msg %s", szMsgType);
        return;
    }

    szValue = Json.GetValue("CellID");
    stInfo.ulCellID = 0;
    if (szValue && szValue[0] != '\0' &&
        !gos_atou(szValue, &stInfo.ulCellID))
    {
        TaskLog(LOG_ERROR, "invalid CellID in msg %s", szMsgType);
        stInfo.ulCellID = 0;
    }

    if (!Json.GetValue("CallType", &stInfo.ulCallType))
    {
        TaskLog(LOG_ERROR, "invalid CallType in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("CallID", &stInfo.i64CallID))
    {
        TaskLog(LOG_ERROR, "invalid CallID in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("CurrentTime", &stInfo.ulCurrentTime))
    {
        TaskLog(LOG_ERROR, "invalid CurrentTime in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("Uptime", &stInfo.ulUptime))
    {
        TaskLog(LOG_ERROR, "invalid Uptime in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("SerialNumber", stInfo.acSerialNumber, sizeof(stInfo.acSerialNumber)))
    {
        TaskLog(LOG_ERROR, "invalid SerialNumber in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("APPVer", stInfo.acAPPVer, sizeof(stInfo.acAPPVer)))
    {
        TaskLog(LOG_ERROR, "invalid APPVer in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("IPHLinkStatus", stInfo.bIsIPHOnline))
    {
        TaskLog(LOG_ERROR, "invalid IPHLinkStatus in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("IPHRunningStatus", stInfo.ulIPHStatus))
    {
        TaskLog(LOG_ERROR, "invalid IPHRunningStatus in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("PALinkStatus", stInfo.bIsPAOnline))
    {
        TaskLog(LOG_ERROR, "invalid PALinkStatus in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("PARunningStatus", stInfo.ulPAStatus))
    {
        TaskLog(LOG_ERROR, "invalid PARunningStatus in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("PAMasterStatus", stInfo.bIsMasterPA))
    {
        TaskLog(LOG_ERROR, "invalid PAMasterStatus in msg %s", szMsgType);
        return;
    }

    if (!GetTrainInfo(stInfo.ulTrainUnitID, stTrain))
    {
        TaskLog(LOG_ERROR, "unknown train %u in msg %s", stInfo.ulTrainUnitID, szMsgType);
        return;
    }

    BOOL    bChanged = FALSE;

    if (stInfo.ulUserID == stTrain.astAppTrainInfo[0].ulUserID &&
        stInfo.ulCoverStatus != stTrain.astAppTrainInfo[0].ulCoverStatus)
    {
        bChanged = TRUE;
    }
    else if (stInfo.ulUserID == stTrain.astAppTrainInfo[1].ulUserID &&
             stInfo.ulCoverStatus != stTrain.astAppTrainInfo[1].ulCoverStatus)
    {
        bChanged = TRUE;
    }

    // 当设备盖状态有变化时才打印，避免大量重复打印
    if (bChanged)
    {
        TaskLog(LOG_DETAIL, "CoverStatus of %u at train %u is %u", stInfo.ulUserID, stInfo.ulTrainUnitID, stInfo.ulCoverStatus);
    }

    CELL_INFO_T     stCellInfo = {0};

    stCellInfo.ulTime = gos_get_current_time();
    stCellInfo.ulTrainUserID = stInfo.ulUserID;
    stCellInfo.ulTrainUnitID = stInfo.ulTrainUnitID;
    stCellInfo.ulCellID = stInfo.ulCellID;
    stCellInfo.ulStationID = stTrain.ulStationID;
    stCellInfo.ulUpsideStationID = stTrain.ulUpsideStationID;
    stCellInfo.ulDownsideStationID = stTrain.ulDownsideStationID;
    stCellInfo.ulTransferTrackFlag = stTrain.ulTransferTrackFlag;
    stCellInfo.ulTurnbackTrackFlag = stTrain.ulTurnbackTrackFlag;
    stCellInfo.ulDirection = stTrain.ulDirection;

    SaveCellInfo(stCellInfo);

    SetAppTrainInfo(stInfo);
}

VOID TaskAPP::SaveCellInfo(CELL_INFO_T &stInfo)
{
    if (stInfo.ulCellID == 0)
    {
        return;
    }

    static UINT32   ulLastSaveTime = 0;
    UINT32          ulTime = gos_get_uptime();
    CELL_INFO_T     *pstCellInfo = g_mCurrCellInfo.GetValue(stInfo.ulTrainUserID);

    // 数据是否有变化
    if (pstCellInfo &&
        pstCellInfo->ulStationID == stInfo.ulStationID &&
        pstCellInfo->ulUpsideStationID == stInfo.ulUpsideStationID &&
        pstCellInfo->ulDownsideStationID == stInfo.ulDownsideStationID &&
        pstCellInfo->ulTransferTrackFlag == stInfo.ulTransferTrackFlag &&
        pstCellInfo->ulTurnbackTrackFlag == stInfo.ulTurnbackTrackFlag &&
        pstCellInfo->ulDirection == stInfo.ulDirection &&
        pstCellInfo->ulCellID == stInfo.ulCellID )
    {
        return;
    }

    g_vTmpCellInfo.push_back(stInfo);
    if (g_vTmpCellInfo.size() >= 500 ||
        (ulTime-ulLastSaveTime) >= 30)
    {
        SaveCellInfoToDB();
        ulLastSaveTime = ulTime;
    }

    if (!pstCellInfo)
    {
        g_mCurrCellInfo.Add(stInfo.ulTrainUserID, stInfo);
    }
    else
    {
        memcpy(pstCellInfo, &stInfo, sizeof(stInfo));
    }
}

VOID TaskAPP::SaveCellInfoToDB()
{
    if (!m_pDao->AddCellInfo(g_vTmpCellInfo))
    {
        TaskLog(LOG_ERROR, "save cell info failed");
    }

    g_vTmpCellInfo.clear();
}

VOID TaskAPP::OnAppSetTrainPosReq(CHAR *szMsg, GJson &Json)
{
    UINT32              ulTrainUnitID;
    UINT32              ulDepotID;
    CHAR                acTrainID[16];
    CHAR                acDestID[8];
    UINT32              ulDirection;
    UINT32              ulStationID;
    UINT32              ulDriverID;
    GJson               SubJson;
    GJsonParam          Param;
    BOOL                bResult = TRUE;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in msg %s", szMsg);
        return;
    }

    if (!Json.GetValue("TrainPositionID", &ulDepotID))
    {
        TaskLog(LOG_ERROR, "invalid TrainPositionID in msg %s", szMsg);
        return;
    }

    if (!Json.GetValue("TrainID", acTrainID, sizeof(acTrainID)))
    {
        TaskLog(LOG_ERROR, "invalid TrainID in msg %s", szMsg);
        return;
    }

    if (!Json.GetValue("DestID", acDestID, sizeof(acDestID)))
    {
        TaskLog(LOG_ERROR, "invalid DestID in msg %s", szMsg);
        return;
    }

    if (!Json.GetValue("Direction", &ulDirection))
    {
        TaskLog(LOG_ERROR, "invalid Direction in msg %s", szMsg);
        return;
    }

    if (!Json.GetValue("StationID", &ulStationID))
    {
        TaskLog(LOG_ERROR, "invalid StationID in msg %s", szMsg);
        return;
    }

    if (!Json.GetValue("DriverID", &ulDriverID))
    {
        TaskLog(LOG_ERROR, "invalid DriverID in msg %s", szMsg);
        return;
    }

    TaskLog(LOG_INFO, "APP set train id(%s) to depot %u of train %u", acTrainID, ulDepotID, ulTrainUnitID);
    bResult = TRUE;

    Param.Add("TrainUnitID", ulTrainUnitID);
    Param.Add("DepotID", ulDepotID);
    Param.Add("TrainID", acTrainID);
    Param.Add("Direction", ulDirection);
    Param.Add("DestID", acDestID);
    Param.Add("StationID", ulStationID);
    Param.Add("DriverID", ulDriverID);

    SendAllClient(DIS_CLIENT_DC, EV_APP_DIS_DC_REQ, MSG_APP_SetTrainPosReq, m_ulAPPSeqID, Param);
}

VOID TaskAPP::OnAppDisDcRsp(CHAR *szMsg, UINT32 ulMsgLen)
{
    GJson       Json;
    GJson       SubJson;
    BOOL        bRet = Json.Parse(szMsg);
    CHAR        *szMsgType;
    CHAR        *szMsgInfo;

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "invalid APP rsp msg: %s", szMsg);
        goto error;
    }

    szMsgType = Json.GetValue("MsgName");

    if (!szMsgType)
    {
        TaskLog(LOG_ERROR, "get MsgName failed: %s", szMsg);
        goto error;
    }

    if (!Json.GetValue("MsgSeqID", &m_ulAPPSeqID))
    {
        TaskLog(LOG_ERROR, "get MsgSeqID failed: %s", szMsg);
        goto error;
    }

    szMsgInfo = Json.GetValue("MsgInfo");
    if (!szMsgInfo)
    {
        TaskLog(LOG_ERROR, "get MsgInfo failed: %s", szMsg);
        goto error;
    }

    if (!SubJson.Parse(szMsgInfo))
    {
        TaskLog(LOG_ERROR, "parse MsgInfo failed: %s", szMsg);
        goto error;
    }

    // 其他为业务消息，单独存储
    TaskMsgLog(LOG_INFO, "get APP rsp msg: %s", szMsg);

    if (strcmp(szMsgType, MSG_APP_RequestCallRsp) == 0)
    {
        OnAppDisDcRequestCallRsp(SubJson);
    }
    else if (strcmp(szMsgType, MSG_APP_RegisterRsp) == 0)
    {
        OnAppDisDcRegisterRsp(SubJson);
    }
    else if (strcmp(szMsgType, MSG_APP_UnregisterRsp) == 0)
    {
        OnAppDisDcUnregisterRsp(SubJson);
    }
    else
    {
        TaskLog(LOG_ERROR, "invalid APP rsp msg: %s", szMsg);
        goto error;
    }

    return;

error:
    return;
}

VOID TaskAPP::OnAppDisDcRequestCallRsp(GJson &Json)
{
    INT64       i64GroupID = 0;
    UINT32      ulReqTime = 0;
    UINT32      ulUserID = 0;
    UINT32      ulDCUserID = 0;
    UINT32      ulCallType = 0;
    UINT32      ulResult;
    GJsonParam  Param;
    UINT32      ulStationUserID = 0;
    VECTOR<UINT32>  vTrainUserID;

    if (!Json.GetValue("ReqTime", &ulReqTime))
    {
        TaskLog(LOG_ERROR, "invalid ReqTime in RequestCallRsp");
        return;
    }

    if (!Json.GetValue("UserID", &ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in RequestCallRsp");
        return;
    }

    if (!Json.GetValue("DCUserID", &ulDCUserID))
    {
        TaskLog(LOG_ERROR, "invalid DCUserID in RequestCallRsp");
        return;
    }

    if (!Json.GetValue("CallType", &ulCallType))
    {
        TaskLog(LOG_ERROR, "invalid CallType in RequestCallRsp");
        return;
    }

    if (!Json.GetValue("Result", &ulResult))
    {
        TaskLog(LOG_ERROR, "invalid Result in RequestCallRsp");
        return;
    }

    if (!Json.GetValue("GroupID", &i64GroupID))
    {
        TaskLog(LOG_ERROR, "invalid GroupID in RequestCallRsp");
        return;
    }

    if (ulCallType == TRAIN_REQ_CALL_STATION)
    {
        if (!Json.GetValue("StationUserID", &ulStationUserID))
        {
            TaskLog(LOG_ERROR, "invalid StationUserID in RequestCallRsp");
            return;
        }
    }
    else if (ulCallType == STATION_REQ_CALL_TRAIN)
    {
        CHAR    *szValue = Json.GetValue("TrainUserID");

        if (!szValue)
        {
            TaskLog(LOG_ERROR, "invalid TrainUserID in RequestCallRsp");
            return;
        }

        GJson       UserJson;
        UINT32      ulTrainUserID;

        UserJson.Parse(szValue);

        VECTOR<GJson*>      &vSubJson = UserJson.GetSubJson();

        for (UINT32 i=0; i<vSubJson.size(); i++)
        {
            if (!vSubJson[i]->GetValue("UserID", &ulTrainUserID))
            {
                TaskLog(LOG_ERROR, "invalid Train UserID in RequestCallRsp");
                return;
            }

            vTrainUserID.push_back(ulTrainUserID);
        }
    }

    Param.Add("UserID", ulUserID);
    Param.Add("DCUserID", ulDCUserID);
    Param.Add("CallType", ulCallType);
    Param.Add("Result", ulResult);
    Param.Add("GroupID", i64GroupID);

    SendAllClient(DIS_CLIENT_DC, EV_DIS_DC_IND, MSG_APP_RequestCallRsp, m_ulAPPSeqID, Param);

    if (!m_pDao->UpdateReqCallInfo(ulReqTime, ulUserID, ulDCUserID))
    {
        TaskLog(LOG_ERROR, "update req call of %u failed", ulUserID);
    }

    for (UINT32 i=0; i<3; i++)
    {
        if (i > 0)
        {
            gos_sleep_1ms(300);
        }

        SendAPPReq(MSG_APP_RequestCallRsp, ulUserID, Param);

        if (ulCallType == TRAIN_REQ_CALL_STATION)   // 通知固定台
        {
            SendAPPReq(MSG_APP_RequestCallRsp, ulStationUserID, Param);
        }
        else if (ulCallType == STATION_REQ_CALL_TRAIN)  // 通知车载台
        {
            for (UINT32 i=0; i<vTrainUserID.size(); i++)
            {
                SendAPPReq(MSG_APP_RequestCallRsp, vTrainUserID[i], Param);
            }
        }
    }

    TaskLog(LOG_INFO, "send request call rsp to %u", ulUserID);
}

VOID TaskAPP::OnAppDisDcRegisterRsp(GJson &Json)
{
    UINT32          ulTrainUnitID = 0;
    UINT32          ulDCUserID = 0;
    UINT32          ulDepotID = 0;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in RegisterRsp of DC");
        return;
    }

    if (!Json.GetValue("DepotID", &ulDepotID))
    {
        TaskLog(LOG_ERROR, "invalid DepotID in RegisterRsp of DC");
        return;
    }

    if (!Json.GetValue("DCUserID", &ulDCUserID))
    {
        TaskLog(LOG_ERROR, "invalid DCUserID in RegisterRsp of DC");
        return;
    }

    SetRegisterOverInfo(ulTrainUnitID, TRUE, ulDepotID, ulDCUserID);

    // 向APP发送应答消息
    GJsonParam  Param;

    Param.Add("TrainUnitID", ulTrainUnitID);
    Param.Add("Result", TRUE);
    Param.Add("TrainPos", ulDepotID);

    UINT32  ulUserID1 = INVALID_USER_ID;
    UINT32  ulUserID2 = INVALID_USER_ID;

    GetTrainUserID(ulTrainUnitID, &ulUserID1, &ulUserID2);

    if (ulUserID1 != INVALID_USER_ID)
    {
        SendAPPReq(MSG_APP_RegisterRsp, ulUserID1, Param);
    }

    if (ulUserID2 != INVALID_USER_ID)
    {
        SendAPPReq(MSG_APP_RegisterRsp, ulUserID2, Param);
    }

    // 注册结束，则修改数据库中状态
    if (!IsRegisterOver(ulTrainUnitID))
    {
        return;
    }

    TaskLog(LOG_INFO, "train(%u) register over", ulTrainUnitID);
}

VOID TaskAPP::OnAppDisDcUnregisterRsp(GJson &Json)
{
    UINT32          ulTrainUnitID = 0;
    UINT32          ulDCUserID = 0;
    UINT32          ulDCType = 0;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in UnRegisterRsp of DC");
        return;
    }

    if (!Json.GetValue("DCType", &ulDCType))
    {
        TaskLog(LOG_ERROR, "invalid DCType in UnRegisterRsp of DC");
        return;
    }

    if (!Json.GetValue("DCUserID", &ulDCUserID))
    {
        TaskLog(LOG_ERROR, "invalid DCUserID in UnRegisterRsp of DC");
        return;
    }

    SetRegisterOverInfo(ulTrainUnitID, FALSE, ulDCType, ulDCUserID);
    if (!IsRegisterOver(ulTrainUnitID))
    {
        return;
    }

    // TODO 返回APP应答消息

    TaskLog(LOG_ERROR, "train(%u) unregister over", ulTrainUnitID);
}

VOID TaskAPP::CheckRegister()
{
    VECTOR<REGISTER_INFO_T> vInfo;

    GetRegisterInfo(vInfo);

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        REGISTER_INFO_T     &stInfo = vInfo[i];

        if (stInfo.bRegisterOver)
        {
            continue;
        }

        GJsonParam      Param;
        PID_T           stPID;
        const CHAR      *szMsgName;

        Param.Add("TrainUnitID", stInfo.ulTrainUnitID);
        if (stInfo.bRegister)
        {
            if (stInfo.ulRegisterDepotID == DEPOT_ID_AUTO)
            {
                if (!GetTrainRegisterDepotID(stInfo.ulTrainUnitID, &stInfo.ulRegisterDepotID))
                {
                    TaskLog(LOG_ERROR, "get registered depot of train(%u) failed", stInfo.ulTrainUnitID);
                    continue;
                }
            }

            szMsgName = (CHAR*)MSG_APP_RegisterReq;
            Param.Add("DepotID", stInfo.ulRegisterDepotID);
            Param.Add("AutoRegister", stInfo.bAutoRegister);
        }
        else
        {
            szMsgName = MSG_APP_UnregisterReq;
        }

        for (UINT32 j=0; j<stInfo.ulDCNum; j++)
        {
            if (GetDCPID(stInfo.aulDCUser[j], &stPID))
            {
                // TODO DC异常断开后需要有处理
                if (!SendClient(&stPID, EV_APP_DIS_DC_REQ, szMsgName, m_ulAPPSeqID, Param))
                {
                    SetDCLogout(stInfo.aulDCUser[j]);
                }
            }
        }
    }
}

VOID TaskAPP::OnCheckTimer()
{
    m_pDao->CheckConn();

    // 主要资源在TaskCfg加载，本处简单处理，不切换到Init状态
    //InitAPPCfg();

    CheckRegister();

    SaveCellInfoToDB();

    UINT32      ulCurrTime = gos_get_uptime();
    UINT32      ulPeriod = 60;
    GJsonParam  Param;

    for (INT32 i=g_vAutoSDSReplyInfo.size()-1; i>=0; i--)
    {
        AUTO_SDS_REPLY_INFO_T       *pstInfo = &g_vAutoSDSReplyInfo[i];

        if (pstInfo->iCount < 0)
        {
            g_vAutoSDSReplyInfo.erase(g_vAutoSDSReplyInfo.begin()+i);
            continue;
        }

        if ((ulCurrTime - pstInfo->ulSendTime) >= ulPeriod)
        {
            Param.Clear();
            Param.Add("SDSID", pstInfo->ulSDSIndex);
            SendAPPReq(MSG_APP_AutoSDSReplyReq, pstInfo->ulUserID, m_ulAPPSeqID, Param);
            pstInfo->ulSendTime = gos_get_uptime();
            pstInfo->iCount--;
        }
    }
}

VOID TaskAPP::OnDcDisAppReq(CHAR *szReq, UINT32 ulMsgLen)
{
    GJson       Json;
    GJson       SubJson;
    BOOL        bRet = Json.Parse(szReq);
    CHAR        *szMsgType;
    CHAR        *szMsgInfo;
    UINT32      ulSeqID;
    UINT32      ulDCUserID;
    CHAR        *szDCOperatorID = NULL;

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "invalid DC to APP msg: %s", szReq);
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

    TaskMsgLog(LOG_INFO, "get DC to APP msg: %s", szReq);

    if (strcmp(szMsgType, MSG_DCBroadcastStart) == 0)
    {
        OnDCBroadcastStartReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCBroadcastEnd) == 0)
    {
        OnDCBroadcastEndReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetTrainGroup) == 0)
    {
        OnDCSetTrainGroupReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetTrainPos) == 0)
    {
        OnDCSetTrainPosReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCGetReqCallInfo) == 0)
    {
        OnDCGetReqCallInfo(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCCloseReqCall) == 0)
    {
        OnDCCloseReqCall(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_APP_EmergencyCallRsp) == 0)
    {
        OnDCEmergencyCallRsp(szReq, szMsgType, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSetDynamicGroup) == 0)
    {
        OnDCSetDynamicGroupReq(szReq, szMsgType, ulSeqID, SubJson);
    }
    else
    {
        TaskLog(LOG_ERROR, "invalid DC to APP msg: %s", szReq);
        goto error;
    }

    return;

error:
    return;
}

BOOL TaskAPP::SendDC(UINT32 ulDCUserID, UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &Param)
{
    PID_T               stPID;

    if (!GetDCPID(ulDCUserID, &stPID))
    {
        TaskLog(LOG_ERROR, "DC %u has not logined", ulDCUserID);
        return FALSE;
    }

    GJsonParam      ReqParam;

    ReqParam.Add("MsgName", (CHAR*)szMsgName);
    ReqParam.Add("MsgSeqID", m_ulAPPSeqID);
    ReqParam.Add("MsgInfo", Param);

    CHAR    *szMsg = ReqParam.GetString();

    return SendClient(&stPID, usMsgID, szMsg, strlen(szMsg)+1);
}

VOID TaskAPP::OnDCSetTrainGroupReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32              ulTrainUnitID;
    UINT32              ulTrainPos;
    GJson               SubJson;
    BOOL                bRet = FALSE;
    GJsonParam          Param;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("TrainPos", &ulTrainPos))
    {
        TaskLog(LOG_ERROR, "invalid TrainPos in msg %s", szReq);
        return;
    }

    bRet = SetTrainPos(ulTrainUnitID, ulTrainPos);
    if (!bRet)
    {
        TaskLog(LOG_ERROR, "set train pos of %u failed", ulTrainUnitID);
    }
    else
    {
        TaskLog(LOG_INFO, "set train pos %u of %u succ", ulTrainPos, ulTrainUnitID);
        AddOperLog("设置列车(%u)归属区域为%u成功", ulTrainUnitID, ulTrainPos);
    }

    Param.Add("Result", bRet?"Succ":"Fail");
    Param.Add("TrainUnitID", ulTrainUnitID);

    TaskMsgLog(LOG_INFO, "send DC rsp msg: %s", (CHAR*)Param);
    SendRsp(MSG_DCSetTrainGroup, ulSeqID, Param);

    if (bRet)
    {
        SendAllClient(DIS_CLIENT_DC, EV_DC_DIS_APP_RSP, MSG_DCSetTrainGroup, ulSeqID, Param);
    }

    // 通知车载台
    Param.Clear();
    Param.Add("TrainUnitID", ulTrainUnitID);
    Param.Add("Result", TRUE);
    Param.Add("TrainPos", ulTrainPos);

    UINT32  ulUserID1;
    UINT32  ulUserID2;

    GetTrainUserID(ulTrainUnitID, &ulUserID1, &ulUserID2);

    if (ulUserID1 != INVALID_USER_ID)
    {
        SendAPPReq(MSG_APP_RegisterRsp, ulUserID1, Param);
    }

    if (ulUserID2 != INVALID_USER_ID)
    {
        SendAPPReq(MSG_APP_RegisterRsp, ulUserID2, Param);
    }
}

VOID TaskAPP::OnDCSetTrainPosReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32              ulTrainUnitID;
    UINT32              ulDepotID;
    CHAR                acTrainID[16];
    CHAR                acDestID[8];
    UINT32              ulDirection;
    UINT32              ulStationID;
    UINT32              ulDriverID;
    GJson               SubJson;
    GJsonParam          Param;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DepotID", &ulDepotID))
    {
        TaskLog(LOG_ERROR, "invalid DepotID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("TrainID", acTrainID, sizeof(acTrainID)))
    {
        TaskLog(LOG_ERROR, "invalid TrainID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DestID", acDestID, sizeof(acDestID)))
    {
        TaskLog(LOG_ERROR, "invalid DestID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Direction", &ulDirection))
    {
        TaskLog(LOG_ERROR, "invalid Direction in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("StationID", &ulStationID))
    {
        TaskLog(LOG_ERROR, "invalid StationID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DriverID", &ulDriverID))
    {
        TaskLog(LOG_ERROR, "invalid DriverID in msg %s", szReq);
        return;
    }

    // 更新ATS信息、
    BOOL bRet = SetTrainPos(ulTrainUnitID, ulDepotID, acTrainID, acDestID, ulDirection, ulStationID, ulDriverID);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "DC set train id(%s) to depot %u of train %u failed", acTrainID, ulDepotID, ulTrainUnitID);
    }
    else
    {
        TaskLog(LOG_INFO, "DC set train id(%s) of train %u succ", acTrainID, ulTrainUnitID);
        AddOperLog("设置列车(%u)运行信息成功，归属区域%u, 车次号为%s, 目的地为%s, 方向为%u);", ulTrainUnitID, ulDepotID, acTrainID, acDestID, ulDirection);
    }

    Param.Add("Result", bRet?"Succ":"Fail");
    Param.Add("TrainUnitID", ulTrainUnitID);

    TaskMsgLog(LOG_INFO, "send DC rsp msg: %s", (CHAR*)Param);
    SendRsp(MSG_DCSetTrainPos, ulSeqID, Param);

    if (bRet)
    {
        SendAllClient(DIS_CLIENT_DC, EV_DC_DIS_APP_RSP, MSG_DCSetTrainPos, ulSeqID, Param);
    }
}

VOID TaskAPP::OnDCBroadcastStartReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32              ulTrainUserID;
    UINT32              ulDCUserID;
    UINT32              ulMaxPeriod;
    UINT32              ulBrdSeqID;
    GJson               SubJson;
    GJsonParam          Param;
    CHAR                *szUser;

    if (!Json.GetValue("BrdSeqID", &ulBrdSeqID))
    {
        TaskLog(LOG_ERROR, "invalid BrdSeqID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DCUserID", &ulDCUserID))
    {
        TaskLog(LOG_ERROR, "invalid DCUserID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("MaxPeriod", &ulMaxPeriod))
    {
        TaskLog(LOG_ERROR, "invalid MaxPeriod in msg %s", szReq);
        return;
    }

    szUser = Json.GetValue("TrainUser");
    if (!szUser)
    {
        TaskLog(LOG_ERROR, "get User failed: %s", szReq);
        return;
    }

    if (!SubJson.Parse(szUser))
    {
        TaskLog(LOG_ERROR, "parse User failed: %s", szReq);
        return;
    }

    VECTOR<GJson*>      &vSubJson = SubJson.GetSubJson();

    Param.Add("BrdSeqID", ulBrdSeqID);
    Param.Add("DCUserID", ulDCUserID);
    Param.Add("MaxPeriod", ulMaxPeriod);

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        if (!vSubJson[i]->GetValue("UserID", &ulTrainUserID))
        {
            TaskLog(LOG_ERROR, "invalid UserID in msg %s", MSG_DCBroadcastStart);
            continue;
        }

        Param.Add("TrainUsrID", ulTrainUserID);
        SendAPPReq(MSG_APP_BroadcastStartReq, ulTrainUserID, Param);

        TaskLog(LOG_INFO, "%u start train %u brd", gos_get_uptime_1ms(), ulTrainUserID);
    }
}

VOID TaskAPP::OnAppBroadcastStartRsp(CHAR *szMsg, GJson &Json)
{
    INT64               i64GroupID;
    UINT32              ulDCUserID;
    UINT32              ulTrainUserID;
    UINT32              ulBrdSeqID;
    GJson               SubJson;
    GJsonParam          Param;
    INT32               iResult;

    if (!Json.GetValue("BrdSeqID", &ulBrdSeqID))
    {
        TaskLog(LOG_ERROR, "invalid BrdSeqID in msg %s", szMsg);
        return;
    }

    if (!Json.GetValue("GroupID", &i64GroupID))
    {
        TaskLog(LOG_ERROR, "invalid GroupID in msg %s", szMsg);
        return;
    }

    if (!Json.GetValue("DCUserID", &ulDCUserID))
    {
        TaskLog(LOG_ERROR, "invalid DCUserID in msg %s", szMsg);
        return;
    }

    if (!Json.GetValue("TrainUserID", &ulTrainUserID))
    {
        TaskLog(LOG_ERROR, "invalid TrainUserID in msg %s", szMsg);
        return;
    }

    if (!Json.GetValue("Result", &iResult))
    {
        TaskLog(LOG_ERROR, "invalid Result in msg %s", szMsg);
        return;
    }

    Param.Add("BrdSeqID", ulBrdSeqID);
    Param.Add("GroupID", i64GroupID);
    Param.Add("DCUserID", ulDCUserID);
    Param.Add("TrainUserID", ulTrainUserID);
    Param.Add("Result", iResult);

    TaskLog(LOG_INFO, "%u recv train %u brd result: %d", gos_get_uptime_1ms(), ulTrainUserID, iResult);

    SendDC(ulDCUserID, EV_DC_DIS_RSP, MSG_DCBroadcastStart, Param);
}

VOID TaskAPP::OnDCBroadcastEndReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    INT64               i64GroupID;
    UINT32              ulTrainUserID;
    UINT32              ulDCUserID;
    UINT32              ulBrdSeqID;
    GJson               SubJson;
    GJsonParam          Param;
    CHAR                *szUser;

    if (!Json.GetValue("BrdSeqID", &ulBrdSeqID))
    {
        TaskLog(LOG_ERROR, "invalid BrdSeqID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("GroupID", &i64GroupID))
    {
        TaskLog(LOG_ERROR, "invalid GroupID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DCUserID", &ulDCUserID))
    {
        TaskLog(LOG_ERROR, "invalid DCUserID in msg %s", szReq);
        return;
    }

    szUser = Json.GetValue("TrainUser");
    if (!szUser)
    {
        TaskLog(LOG_ERROR, "get User failed: %s", szReq);
        return;
    }

    if (!SubJson.Parse(szUser))
    {
        TaskLog(LOG_ERROR, "parse User failed: %s", szReq);
        return;
    }

    VECTOR<GJson*>      &vSubJson = SubJson.GetSubJson();

    Param.Add("BrdSeqID", ulBrdSeqID);

    Param.Add("DCUserID", ulDCUserID);

    for (UINT32 j=0; j<5; j++)
    {
        for (UINT32 i=0; i<vSubJson.size(); i++)
        {
            if (!vSubJson[i]->GetValue("UserID", &ulTrainUserID))
            {
                TaskLog(LOG_ERROR, "invalid UserID in msg %s", MSG_DCBroadcastEnd);
                continue;
            }

            SendAPPReq(MSG_APP_BroadcastEndReq, ulTrainUserID, Param);

            if (j == 0)
            {
                TaskLog(LOG_INFO, "stop train %u brd of group: " FORMAT_I64, ulTrainUserID, i64GroupID);
            }
        }

        gos_sleep_1ms(100);
    }
}

VOID TaskAPP::OnAppSDSReplyReq(CHAR *szMsgType, GJson &Json)
{
    UINT32              ulUserID;
    UINT32              ulSDSIndex;
    UINT32              ulSDSID = 0;
    UINT32              ulTime = gos_get_current_time();
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

    if (!m_pDao->GetSDSIDBySDSIndex(ulSDSIndex, &ulSDSID))
    {
        TaskLog(LOG_ERROR, "get SDS of %u failed", ulSDSIndex);
    }
    else
    {
        if (!m_pDao->UpdateSDSReply(ulSDSID, ulUserID, ulTime))
        {
            TaskLog(LOG_ERROR, "failed to update SDSReply info of %u in msg %s", ulUserID, szMsgType);
        }
    }

    Param.Add("SDSID", ulSDSIndex);

    SendAPPRsp(MSG_APP_SDSReplyRsp, Param);

    TaskLog(LOG_INFO, "train user %u send SDSReply of SDS %u", ulUserID, ulSDSIndex);

    // 向另外一台车载台发送通知消息
    UINT32  ulUserID2 = GetAnotherTrainUser(ulUserID);

    if (ulUserID2 == INVALID_USER_ID)
    {
        TaskLog(LOG_ERROR, "get another train user of %u failed", ulUserID);
        return;
    }

    SendAPPReq(MSG_APP_AutoSDSReplyReq, ulUserID2, m_ulAPPSeqID, Param);

    AUTO_SDS_REPLY_INFO_T       stInfo = {0};
    UINT32                      ulMaxReqCount = 10;

    stInfo.ulSDSIndex = ulSDSIndex;
    stInfo.ulUserID = ulUserID2;
    stInfo.ulSendTime = gos_get_uptime();
    stInfo.iCount = ulMaxReqCount;

    g_vAutoSDSReplyInfo.push_back(stInfo);
}

VOID TaskAPP::OnAppAutoSDSReplyRsp(CHAR *szMsgType, GJson &Json)
{
    UINT32              ulUserID;
    UINT32              ulSDSID;
    UINT32              ulSDSIndex;
    UINT32              ulTime = gos_get_current_time();
    UINT32              ulAutoReply;

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

    if (!Json.GetValue("AutoReply", &ulAutoReply))
    {
        TaskLog(LOG_ERROR, "invalid AutoReply in msg %s", szMsgType);
        return;
    }

    TaskLog(LOG_INFO, "train user %u send AutoSDSReply of SDS %u", ulUserID, ulSDSIndex);

    if (!m_pDao->GetSDSIDBySDSIndex(ulSDSIndex, &ulSDSID))
    {
        TaskLog(LOG_ERROR, "get SDS of %u failed", ulSDSIndex);
    }
    else
    {
        if (ulAutoReply != 0 && !m_pDao->UpdateSDSReply(ulSDSID, ulUserID, ulTime))
        {
            TaskLog(LOG_ERROR, "failed to update SDSReply info of %u in msg %s", ulUserID, szMsgType);
        }
    }

    for (UINT32 i=0; i<g_vAutoSDSReplyInfo.size(); i++)
    {
        if (g_vAutoSDSReplyInfo[i].ulSDSIndex == ulSDSIndex &&
            g_vAutoSDSReplyInfo[i].ulUserID == ulUserID)
        {
            g_vAutoSDSReplyInfo.erase(g_vAutoSDSReplyInfo.begin()+i);
            break;
        }
    }
}

VOID TaskAPP::OnDCGetReqCallInfo(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    REQ_CALL_INFO_T     *pstInfo;
    GJsonParam          Param;
    GJsonTupleParam     TupleParam;

    for (UINT32 i=0; i<g_vAPPRequestCallInfo.size(); i++)
    {
        Param.Clear();

        pstInfo = &g_vAPPRequestCallInfo[i];
        Param.Add("UserID", pstInfo->ulReqUserID);
        Param.Add("CallType", pstInfo->ulReqCallType);
        Param.Add("ReqTime", pstInfo->ulReqTime);
        Param.Add("DCUserID", pstInfo->ulDCUserID);
        Param.Add("DCType", pstInfo->ulDCType);

        TupleParam.Add(Param);
    }

    TaskMsgLog(LOG_INFO, "send DC rsp msg: %s", (CHAR*)TupleParam); // 必须要强制转换
    SendRsp(szMsgType, ulSeqID, TupleParam);
}

VOID TaskAPP::OnDCCloseReqCall(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    UINT32          ulUserID;
    UINT32          ulCallType;
    UINT32          ulDCUserID;

    if (!Json.GetValue("UserID", &ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("CallType", &ulCallType))
    {
        TaskLog(LOG_ERROR, "invalid CallType in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("DCUserID", &ulDCUserID))
    {
        TaskLog(LOG_ERROR, "invalid DCUserID in msg %s", szMsgType);
        return;
    }

    REQ_CALL_INFO_T     *pstInfo = GetReqCallInfo(ulUserID, ulCallType);

    if (!pstInfo)
    {
        TaskLog(LOG_ERROR, "unknown UserID in msg %s", szMsgType);
        return;
    }

    if (pstInfo->ulDCUserID != ulDCUserID)
    {
        return;
    }

     DelReqCallInfo(ulUserID, ulCallType);
}

VOID TaskAPP::OnDCEmergencyCallRsp(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    UINT32          ulUserID;
    GJsonParam      Param;

    if (!Json.GetValue("UserID", &ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szReq);
        return;
    }

    TaskLog(LOG_FATAL, "dc %u ack emergency call to %u", m_ulDCUserID, ulUserID);

    SendAPPReq(MSG_APP_EmergencyCallRsp, ulUserID, Param);
}

VOID TaskAPP::OnAppEmergencyCallReq(CHAR *szMsg, GJson &Json)
{
    UINT32              ulUserID;
    UINT32              ulTime;
    UINT32              ulEB;

    if (!Json.GetValue("UserID", &ulUserID))
    {
        TaskLog(LOG_ERROR, "invalid UserID in msg %s", szMsg);
        return;
    }

    if (!Json.GetValue("Time", &ulTime))
    {
        TaskLog(LOG_ERROR, "invalid Time in msg %s", szMsg);
        return;
    }

    if (!Json.GetValue("EB", &ulEB))
    {
        TaskLog(LOG_ERROR, "invalid EB in msg %s", szMsg);
        return;
    }

    TaskLog(LOG_FATAL, "emergency call req from %u%s", ulUserID, ulEB?" by EB":"");

    SendAllClient(DIS_CLIENT_DC, EV_DIS_DC_IND, szMsg);
}

VOID TaskAPP::OnDCSetDynamicGroupReq(CHAR *szReq, CHAR *szMsgType, UINT32 ulSeqID, GJson &Json)
{
    GJson           SubJson;
    GJsonParam      Param;
    INT64           i64GroupID;
    UINT32          ulUserID;
    CHAR            *szValue;

    if (!Json.GetValue("GroupID", &i64GroupID))
    {
        TaskLog(LOG_ERROR, "invalid GroupID in msg %s", szReq);
        return;
    }

    szValue = Json.GetValue("AddUser");
    if (szValue)
    {
        if (!SubJson.Parse(szValue))
        {
            TaskLog(LOG_ERROR, "parse AddUser failed in msg: %s", szReq);
            return;
        }

        VECTOR<GJson*>      &vSubJson = SubJson.GetSubJson();

        Param.Add("GroupID", i64GroupID);

        for (UINT32 i=0; i<vSubJson.size(); i++)
        {
            if (!vSubJson[i]->GetValue("UserID", &ulUserID))
            {
                TaskLog(LOG_ERROR, "invalid UserID in msg %s", szReq);
                continue;
            }

            if (!SendAPPReq(MSG_APP_AddDynamicGroupReq, ulUserID, Param))
            {
                TaskLog(LOG_ERROR, "send add dynamic group to train ue %u failed", ulUserID);
            }
        }
    }

    szValue = Json.GetValue("DelUser");
    if (szValue)
    {
        if (!SubJson.Parse(szValue))
        {
            TaskLog(LOG_ERROR, "parse DelUser failed in msg: %s", szReq);
            return;
        }

        VECTOR<GJson*>      &vSubJson = SubJson.GetSubJson();

        Param.Add("GroupID", i64GroupID);

        for (UINT32 i=0; i<vSubJson.size(); i++)
        {
            if (!vSubJson[i]->GetValue("UserID", &ulUserID))
            {
                TaskLog(LOG_ERROR, "invalid UserID in msg %s", szReq);
                continue;
            }

            if (!SendAPPReq(MSG_APP_DelDynamicGroupReq, ulUserID, Param))
            {
                TaskLog(LOG_ERROR, "send del dynamic group to train ue %u failed", ulUserID);
            }
        }
    }

    TaskMsgLog(LOG_INFO, "dc %u set dynamic group "FORMAT_I64, m_ulDCUserID, i64GroupID);
}

VOID TaskAPP::TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
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

        case TASK_STATUS_WORK:
            switch(usMsgID)
            {
                case EV_CHECK_TIMER:
                    OnCheckTimer();
                    break;

                case EV_APP_DIS_REQ:
                    OnAppDisReq((CHAR*)pvMsg, ulMsgLen);
                    break;

                case EV_APP_DIS_DC_RSP:
                    OnAppDisDcRsp((CHAR*)pvMsg, ulMsgLen);
                    break;

                case EV_DC_DIS_APP_REQ:
                    OnDcDisAppReq((CHAR*)pvMsg, ulMsgLen);
                    break;

    /*            case EV_APP_UE_DIS_REQ:
                    OnUEDisReq((CHAR *)pvMsg, ulMsgLen);
                    break;*/

                default:
                    TaskLog(LOG_ERROR, "unknown msg %u", usMsgID);
                    break;
            }

            break;

        default:
            break;
    }
}
