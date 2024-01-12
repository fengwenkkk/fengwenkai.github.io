#include "g_include.h"
#include "ds_public.h"
#include "msg.h"
#include "DISPublic.h"
#include "TaskOCC.h"

TaskOCC     *g_TaskOCC = NULL;

BOOL SendNOCCMsg(const CHAR *szMsgName, UINT32 ulSeqID, CHAR *szMsgInfo)
{
    if (!g_TaskOCC)
    {
        return FALSE;
    }

    return g_TaskOCC->Send(szMsgName, ulSeqID, szMsgInfo);
}

TaskOCC::TaskOCC(UINT16 usDispatchID):GBaseTask(MODULE_OCC, "OCC", usDispatchID, FALSE)
{
    m_usLocalPort = 0;
    memset(&m_stNOCCAddr, 0, sizeof(m_stNOCCAddr));

    m_bRunning = TRUE;
    m_ulSendHeartBeatMsgPeriod = 3000;

    m_stSock = INVALID_SOCKET;
}

BOOL TaskOCC::Init()
{
    if (!InitCfg())
    {
        return FALSE;
    }

    if (!InitSocket())
    {
        return FALSE;
    }

    g_TaskOCC = this;

    return TRUE;
}

BOOL TaskOCC::InitCfg()
{
    static BOOL bInit = FALSE;

    if (bInit)
    {
        return TRUE;
    }

    LOCAL_CFG_T       &stLocalCfg = GetLocalCfg();

    m_usLocalPort = stLocalCfg.usOCCPort;

    gos_init_socket_addr(&m_stNOCCAddr, stLocalCfg.aucNOCCAddr, stLocalCfg.usNOCCPort);

    bInit = TRUE;

    return TRUE;
}

BOOL TaskOCC::InitSocket()
{
    if (m_stSock != INVALID_SOCKET)
    {
        return TRUE;
    }

    UINT8   aucLocalAddr[4] = {0,0,0,0};

    m_stSock = gos_create_udp_server_socket(aucLocalAddr, m_usLocalPort);
    if (m_stSock == INVALID_SOCKET)
    {
        TaskLog(LOG_ERROR, "create udp socket on port %u failed", m_usLocalPort);
        return FALSE;
    }

    TaskLog(LOG_INFO, "Task OCC init successful!");

    return TRUE;
}

VOID TaskOCC::OnServerDisconnectClient(UINT16 usClientID)
{
}

INT32 TaskOCC::RecvMsg()
{
    UINT32      ulTimeout = 1000;
    SOCKADDR_IN stClientAddr;

    memset(m_aucMsg, 0, sizeof(m_aucMsg));

    INT32 iSize = gos_recv_udp_msg(m_stSock, m_aucMsg, sizeof(m_aucMsg)-1, ulTimeout, &stClientAddr);

    return iSize;
}

VOID TaskOCC::Run()
{
    INT32       iRet;
    UINT32      ulLastSendTime = 0;
    UINT32      ulTime;

    while(m_bRunning)
    {
        ulTime = gos_get_uptime_1ms();
        if (ulLastSendTime == 0 ||
            (ulTime - ulLastSendTime) > m_ulSendHeartBeatMsgPeriod)
        {
            SendHeartBeatMsg();
            ulLastSendTime = gos_get_uptime_1ms();
        }

        iRet = RecvMsg();
        if (iRet <= 0)
        {
            continue;
        }

        OnRecvMsg((CHAR*)m_aucMsg);
    }

    CLOSE_SOCKET(m_stSock);
}

BOOL TaskOCC::Send(const CHAR *szMsgName, UINT32 ulSeqID, CHAR *szMsgInfo)
{
    GJsonParam      ReqParam;

    ReqParam.Add("MsgName", (CHAR*)szMsgName);
    ReqParam.Add("MsgSeqID", ulSeqID);
    ReqParam.Add("MsgEncoding", "ANSI");
    ReqParam.Add("MsgInfo", szMsgInfo, FALSE);

    CHAR    *szReqMsg = ReqParam.GetString();
/*    CHAR    *szUtf8Msg = (CHAR*)calloc(1, strlen(szMsg)*4 + 16);

    if (!szUtf8Msg)
    {
        return FALSE;
    }

    gos_ascii_to_utf8(szMsg, szUtf8Msg); */

    BOOL bRet = gos_udp_send(m_stSock, szReqMsg, strlen(szReqMsg)+1, &m_stNOCCAddr);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "send msg to NOCC failed");
    }

   // GOS_FREE(szUtf8Msg);

    return bRet;
}

VOID TaskOCC::SendHeartBeatMsg()
{
    GJsonParam       Param;

    Param.Add("LineID", GetLocalCfg().ulLineID);
    Param.Add("Time", gos_get_current_time());

    Send(MSG_OCC_HeartBeatReq, 0, Param);
}

VOID TaskOCC::OnRecvMsg(CHAR *szReq)
{
    GJson       Json;
    GJson       SubJson;
    BOOL        bRet = Json.Parse(szReq);
    CHAR        *szMsgType;
    CHAR        *szMsgEncoding;
    CHAR        *szMsgInfo;
    CHAR        *szAnsiMsgInfo = NULL;
    UINT32      ulSeqID;
    BOOL        bMsgEncodingUTF8 = FALSE;

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "invalid NOCC msg: %s", szReq);
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

    szMsgEncoding = Json.GetValue("MsgEncoding");
    if (!szMsgEncoding)
    {
        TaskLog(LOG_ERROR, "get MsgEncoding failed: %s", szReq);
        goto error;
    }

    if (strcasecmp(szMsgEncoding, "UTF8") == 0 ||
        strcasecmp(szMsgEncoding, "UTF-8") == 0 )
    {
        bMsgEncodingUTF8 = TRUE;
    }

    szMsgInfo = Json.GetValue("MsgInfo");
    if (!szMsgInfo)
    {
        TaskLog(LOG_ERROR, "get MsgInfo failed: %s", szReq);
        goto error;
    }

    if (bMsgEncodingUTF8)
    {
        szAnsiMsgInfo = gos_utf8_to_ascii(szMsgInfo, NULL);
        if (!szAnsiMsgInfo)
        {
            TaskLog(LOG_WARN, "convert MsgInfo to ANSI failed: %s", szReq);
        }
        else
        {
            szMsgInfo = szAnsiMsgInfo;
        }
    }

    if (!SubJson.Parse(szMsgInfo))
    {
        GOS_FREE(szAnsiMsgInfo);
        TaskLog(LOG_ERROR, "parse MsgInfo failed: %s", szReq);
        goto error;
    }

    GOS_FREE(szAnsiMsgInfo);

    if (strcmp(szMsgType, MSG_OCC_GetATSInfoReq) == 0)
    {
#ifndef DEBUG
        TaskMsgLog(LOG_DETAIL, "get NOCC msg: %s", szReq);
#endif
    }
    else
    {
        TaskMsgLog(LOG_INFO, "get NOCC msg: %s", szReq);
    }

    if (strcmp(szMsgType, MSG_OCC_HeartBeatRsp) == 0)
    {
        OnGetHeartBeatRsp(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_OCC_GetLineInfoReq) == 0)
    {
        OnGetLineInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_OCC_GetATSInfoReq) == 0)
    {
        OnGetATSInfoReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_OCC_GetPARecCfgReq) == 0)
    {
        OnGetPARecCfgReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_OCC_StartTrainRecPAReq) == 0)
    {
        OnStartTrainRecPAReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_OCC_StartTrainRealPAReq) == 0)
    {
        OnStartTrainRealPAReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_OCC_StopTrainRealPAReq) == 0)
    {
        OnStopTrainRealPAReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_OCC_PushTrainRealPAAudioReq) == 0)
    {
        OnPushTrainRealPAAudioReq(szReq, ulSeqID, SubJson);
    }
    else
    {
        TaskLog(LOG_ERROR, "invalid NOCC msg: %s", szReq);
        goto error;
    }

    return;

error:
    return;
}

VOID TaskOCC::OnGetHeartBeatRsp(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
}

VOID TaskOCC::OnGetATSInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonTupleParam     TupleParam;
    GJsonParam          Param;
    VECTOR<TRAIN_INFO_T>    vInfo;

    GetTrainInfo(vInfo);

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        TRAIN_INFO_T    &stTrain = vInfo[i];

        Param.Clear();

        INT32   iTrainOrder = GetTrainOrder(&stTrain);
        UINT32  ulDriveMode = ConvertDriveMode(stTrain.ulDriveMode);
        UINT32  ulTrainStatus = GetTrainStatus(stTrain);
        UINT32  ulLostRealLocation = GetLostRealLocation(stTrain.ulLostRealLocation, stTrain.ulUptime); 

        Param.Add("TrainID", stTrain.ulTrainUnitID);
        Param.Add("TrainServiceID", stTrain.acTrainID);
        Param.Add("Direction", stTrain.ulDirection);
        Param.Add("StationID", stTrain.ulStationID);
        Param.Add("DriveMode", ulDriveMode);
        Param.Add("TrainPos", iTrainOrder);
        Param.Add("OnDepotTrack", stTrain.ulDepotTrackFlag);
        Param.Add("OnTurnbackTrack", stTrain.ulTurnbackTrackFlag);
        Param.Add("OnTransferTrack", stTrain.ulTransferTrackFlag);
        Param.Add("LostRealLocation", ulLostRealLocation);
        Param.Add("TrainStatus", ulTrainStatus);
        Param.Add("HomeGroupID", stTrain.i64DefaultGroupID);

        TupleParam.Add(Param);
    }

    Send(MSG_OCC_GetATSInfoRsp, ulSeqID, TupleParam);
}

VOID TaskOCC::OnGetLineInfoReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonParam          Param;

    Param.Add("RealPAType", OCC_REAL_PA_TYPE_PCM);

    Send(MSG_OCC_GetLineInfoRsp, ulSeqID, Param);
}

VOID TaskOCC::OnGetPARecCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    VECTOR<REC_BRD_CFG_T>   vCfg;
    GJsonTupleParam         TupleParam;

    GetRecBrdCfg(vCfg);

    for (UINT32 i=0; i<vCfg.size(); i++)
    {
        GJsonParam              Param;

        Param.Add("PARecID", vCfg[i].ulRecID);
        Param.Add("PARecInfo", vCfg[i].acRecInfo);

        TupleParam.Add(Param);
    }

    Send(MSG_OCC_GetPARecCfgRsp, ulSeqID, TupleParam);
}

VOID TaskOCC::OnStartTrainRecPAReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    SendLocal(MODULE_SA, EV_NOCC_REQ, szReq);
}

VOID TaskOCC::OnStartTrainRealPAReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    SendLocal(MODULE_SA, EV_NOCC_REQ, szReq);
}

VOID TaskOCC::OnStopTrainRealPAReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    SendLocal(MODULE_SA, EV_NOCC_REQ, szReq);
}

VOID TaskOCC::OnPushTrainRealPAAudioReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    SendLocal(MODULE_SA, EV_NOCC_REQ, szReq);
}

VOID TaskOCC::TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
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
                    else
                    {
                        Run();
                    }

                    break;

                default:
                    break;
            }

            break;

        default:
            break;
    }
}
