#include "DISPublic.h"
#include "TaskSA.h"
#include "TaskOCC.h"
#include "pis.h"

#define CHECK_TIMER         TIMER10
#define EV_CHECK_TIMER      GET_TIMER_EV(CHECK_TIMER)

VECTOR<REC_BRD_SEND_INFO_T>     g_vRecBrdSendInfo;
VECTOR<REAL_BRD_SEND_INFO_T>    g_vRealBrdSendInfo;

VECTOR<IPH_TALK_INFO_T>     g_vIPHApplyInfo;
VECTOR<SA_PID_T>            g_vSAPID;

static UINT32       g_ulIPHOverHoldTime = 2000;         // IPH结束2秒之内的不处理新的请求
static UINT32       g_ulIPHWaitAcceptTime = 90*1000;    // 等待调度台接听IPH请求的最大时间

static BOOL IsNOCCRecBrd(UINT32 ulBrdSeqID)
{
    return (ulBrdSeqID & 0x01);    //奇数
}

static UINT32 GetBrdSeqID(UINT32 ulBrdSeqID, BOOL bIsNOCCBrd)
{
    if (ulBrdSeqID & 0x01)
    {
        if (bIsNOCCBrd)
        {
            return ulBrdSeqID;
        }
        else
        {
            return ulBrdSeqID+1;
        }
    }
    else
    {
        if (bIsNOCCBrd)
        {
            return ulBrdSeqID+1;
        }
        else
        {
            return ulBrdSeqID;
        }
    }
}


namespace BrdInfo
{
VECTOR<BRD_MAP_INFO_T>      g_vBrdMap;

BRD_MAP_INFO_T* GetBrdInfo(BOOL bIsRealBrd, UINT32 ulBrdSeqID)
{
    for (UINT32 i=0; i<g_vBrdMap.size(); i++)
    {
        if (g_vBrdMap[i].ulBrdSeqID == ulBrdSeqID &&
            g_vBrdMap[i].bIsRealBrd == bIsRealBrd)
        {
            return &g_vBrdMap[i];
        }
    }

    return NULL;
}

BRD_MAP_INFO_T* GetBrdInfoByPAIndex(BOOL bIsRealBrd, UINT32 ulPAIndex)
{
    for (UINT32 i=0; i<g_vBrdMap.size(); i++)
    {
        if (g_vBrdMap[i].ulNOCCPAIndex == ulPAIndex &&
            g_vBrdMap[i].bIsRealBrd == bIsRealBrd)
        {
            return &g_vBrdMap[i];
        }
    }

    return NULL;
}

VOID NewBrdInfo(BRD_MAP_INFO_T &stInfo)
{
    BRD_MAP_INFO_T  *pstIfno = GetBrdInfo(stInfo.bIsRealBrd, stInfo.ulBrdSeqID);

    if (pstIfno)
    {
        return;
    }

    GosLog("SA", LOG_INFO, "new %s brd info %u %u", stInfo.bIsRealBrd?"real":"rec", stInfo.ulBrdSeqID, stInfo.ulNOCCPAIndex);

    g_vBrdMap.push_back(stInfo);
}

VOID DelBrdInfo(BOOL bIsRealBrd, UINT32 ulBrdSeqID)
{
    for (UINT32 i=0; i<g_vBrdMap.size(); i++)
    {
        if (g_vBrdMap[i].ulBrdSeqID == ulBrdSeqID &&
            g_vBrdMap[i].bIsRealBrd == bIsRealBrd)
        {
            GosLog("SA", LOG_INFO, "delete %s brd info %u", bIsRealBrd?"real":"rec", ulBrdSeqID);

            g_vBrdMap.erase(g_vBrdMap.begin()+i);
            return;
        }
    }

    GosLog("SA", LOG_ERROR, "unknown %s brd info %u", bIsRealBrd?"real":"rec", ulBrdSeqID);
}

UINT32 GetBrdDCUserID(BOOL bIsRealBrd, UINT32 ulBrdSeqID)
{
    BRD_MAP_INFO_T  *pstIfno = GetBrdInfo(bIsRealBrd, ulBrdSeqID);

    if (!pstIfno)
    {
        return INVALID_USER_ID;
    }
    else
    {
        return pstIfno->ulDCUserID;
    }
}

VOID Check()
{
    UINT32      ulCurrTime = gos_get_uptime();
    UINT32      ulBrdTimeout = 300;

    for (INT32 i=g_vBrdMap.size()-1; i>=0; i--)
    {
        if ((ulCurrTime-g_vBrdMap[i].ulStartTime) > ulBrdTimeout)
        {
            GosLog("SA", LOG_WARN, "%s brd %u is timeout", g_vBrdMap[i].bIsRealBrd?"real":"rec", g_vBrdMap[i].ulBrdSeqID);
            g_vBrdMap.erase(g_vBrdMap.begin() + i);
        }
    }
}
}//namespace BrdInfo

VOID TaskSA::PrintIPHInfo()
{
    for (UINT32 i=0; i<g_vIPHApplyInfo.size(); i++)
    {
        TaskLog(LOG_DETAIL, "current IPH talk: %u %u %u %u %u",
                g_vIPHApplyInfo[i].ulTrainUnitID,
                g_vIPHApplyInfo[i].ulCarriageID,
                g_vIPHApplyInfo[i].ulIPHDevID,
                g_vIPHApplyInfo[i].ulAcceptDCUserID,
                g_vIPHApplyInfo[i].ulStatus);
    }
}

BOOL TaskSA::NewIPHTalk(IPH_TALK_INFO_T &stApplyInfo)
{
    for (UINT32 i=0; i<g_vIPHApplyInfo.size(); i++)
    {
        if (g_vIPHApplyInfo[i].ulTrainUnitID == stApplyInfo.ulTrainUnitID &&
            g_vIPHApplyInfo[i].ulCarriageID == stApplyInfo.ulCarriageID &&
            g_vIPHApplyInfo[i].ulIPHDevID == stApplyInfo.ulIPHDevID)
        {
            if (g_vIPHApplyInfo[i].ulStatus != IPH_STATUS_OVER ||
                (stApplyInfo.ulStopUptime > 0 && stApplyInfo.ulStopUptime < (g_vIPHApplyInfo[i].ulApplyUptime + g_ulIPHOverHoldTime)) ) // 结束2秒之内的不处理
            {
                strcpy(g_vIPHApplyInfo[i].acVideoURL, stApplyInfo.acVideoURL);
                memcpy(g_vIPHApplyInfo[i].aucIPHDevIP, stApplyInfo.aucIPHDevIP, sizeof(g_vIPHApplyInfo[i].aucIPHDevIP));

                return FALSE;
            }
            else
            {
                memcpy(&g_vIPHApplyInfo[i], &stApplyInfo, sizeof(stApplyInfo));

                TaskLog(LOG_INFO, "new IPH talk apply: %u %u %u", stApplyInfo.ulTrainUnitID, stApplyInfo.ulCarriageID, stApplyInfo.ulIPHDevID);

                return TRUE;
            }
        }
    }

    g_vIPHApplyInfo.push_back(stApplyInfo);

    TaskLog(LOG_INFO, "new IPH talk apply: %u %u %u", stApplyInfo.ulTrainUnitID, stApplyInfo.ulCarriageID, stApplyInfo.ulIPHDevID);

    PrintIPHInfo();

    return TRUE;
}

BOOL TaskSA::CleanIPHTalk(UINT32 ulTrainUnitID, VECTOR<IPH_TALK_INFO_T> &vIPHInfo)
{

    for (UINT32 i=0; i<g_vIPHApplyInfo.size(); i++)
    {
        if (g_vIPHApplyInfo[i].ulStatus      != IPH_STATUS_OVER &&
            g_vIPHApplyInfo[i].ulTrainUnitID == ulTrainUnitID )
        {
            TaskLog(LOG_INFO, "IPH talk(%u,%u,%u) is stopped",
                ulTrainUnitID, g_vIPHApplyInfo[i].ulCarriageID, g_vIPHApplyInfo[i].ulIPHDevID);

            g_vIPHApplyInfo[i].ulStopTime = gos_get_current_time();
            g_vIPHApplyInfo[i].ulStopUptime = gos_get_uptime_1ms();
            g_vIPHApplyInfo[i].ulStatus = IPH_STATUS_OVER;

            IPH_TALK_INFO_T    stIPHInfo = g_vIPHApplyInfo[i];
            vIPHInfo.push_back(stIPHInfo);
        }
    }

    return TRUE;
}

BOOL TaskSA::StopIPHTalk(UINT32 ulTrainUnitID, UINT32 ulCarriageID, UINT32 ulIPHDevID)
{
    GJsonParam      JsonParam;

    JsonParam.Add("TrainUnitID", ulTrainUnitID);
    JsonParam.Add("CarriageID", ulCarriageID);
    JsonParam.Add("IPHDevID", ulIPHDevID);

    if (!SendTrainSA(MSG_DCStopIPHTalk, ulTrainUnitID, JsonParam))
    {
        TaskLog(LOG_ERROR, "send stop iph(%u %u %u) to sa fail", ulTrainUnitID, ulCarriageID, ulIPHDevID);
        return FALSE;
    }

    TaskLog(LOG_INFO, "send stop iph(%u %u %u) to sa succ", ulTrainUnitID, ulCarriageID, ulIPHDevID);
    return TRUE;
}

UINT32 TaskSA::StopIPHTalk(UINT32 ulTrainUnitID, UINT32 ulCarriageID, UINT32 ulIPHDevID, IPH_TALK_INFO_T *pstInfo)
{
    BOOL    bRet = FALSE;

    for (UINT32 i=0; i<g_vIPHApplyInfo.size(); i++)
    {
        if (g_vIPHApplyInfo[i].ulStatus      != IPH_STATUS_OVER &&
            g_vIPHApplyInfo[i].ulTrainUnitID == ulTrainUnitID   &&
            g_vIPHApplyInfo[i].ulCarriageID  == ulCarriageID    &&
            g_vIPHApplyInfo[i].ulIPHDevID    == ulIPHDevID)
        {
            TaskLog(LOG_INFO, "IPH talk(%u,%u,%u) is stopped",
                    ulTrainUnitID, ulCarriageID, ulIPHDevID);

            g_vIPHApplyInfo[i].ulStopTime = gos_get_current_time();
            g_vIPHApplyInfo[i].ulStopUptime = gos_get_uptime_1ms();
            g_vIPHApplyInfo[i].ulStatus = IPH_STATUS_OVER;
            memcpy(pstInfo, &g_vIPHApplyInfo[i], sizeof(IPH_TALK_INFO_T));

            bRet = TRUE;
            break;
        }
    }

    if (!bRet)
    {
        TaskLog(LOG_INFO, "IPH talk(%u,%u,%u) is stopped, but without record",
                    ulTrainUnitID, ulCarriageID, ulIPHDevID);
    }
    else
    {
        TaskLog(LOG_INFO, "IPH talk(%u,%u,%u) is stopped",
            ulTrainUnitID, ulCarriageID, ulIPHDevID);
    }

    PrintIPHInfo();

    return bRet;
}

IPH_TALK_INFO_T* TaskSA::AcceptIPHTalk(UINT32 ulTrainUnitID, UINT32 ulCarriageID, UINT32 ulIPHDevID, UINT32 ulDCUserID)
{
    IPH_TALK_INFO_T *pstInfo = NULL;

    for (UINT32 i=0; i<g_vIPHApplyInfo.size(); i++)
    {
        if (g_vIPHApplyInfo[i].ulTrainUnitID == ulTrainUnitID)
        {
          //  g_vIPHApplyInfo[i].ulAcceptDCUserID = ulDCUserID;

            if (g_vIPHApplyInfo[i].ulCarriageID == ulCarriageID &&
                g_vIPHApplyInfo[i].ulIPHDevID == ulIPHDevID)
            {
                g_vIPHApplyInfo[i].ulAcceptDCUserID = ulDCUserID;   // 修改成每个IPH各调度台独立接听

                if (g_vIPHApplyInfo[i].ulStatus == IPH_STATUS_WAIT_ACK)
                {
                    TaskLog(LOG_INFO, "IPH talk(%u,%u,%u) is wait ack",
                    ulTrainUnitID, ulCarriageID, ulIPHDevID);

                    return NULL;
                }

                g_vIPHApplyInfo[i].ulGetACKTime = 0;
                g_vIPHApplyInfo[i].ulAcceptTime = gos_get_current_time();

                //g_vIPHApplyInfo[i].ulStatus = IPH_STATUS_TALK;
                g_vIPHApplyInfo[i].ulStatus = IPH_STATUS_WAIT_ACK; //设置IPH状态为等待接听应答
                pstInfo = &g_vIPHApplyInfo[i];
            }
        }
    }

    if (!pstInfo)
    {
        TaskLog(LOG_ERROR, "IPH talk(%u,%u,%u) is accepted by %u, but without record",
               ulTrainUnitID, ulCarriageID, ulIPHDevID, ulDCUserID);
    }
    else
    {
        TaskLog(LOG_INFO, "IPH talk(%u,%u,%u) is accepted by %u",
            ulTrainUnitID, ulCarriageID, ulIPHDevID, ulDCUserID);
    }

    PrintIPHInfo();

    return pstInfo;
}

BOOL TaskSA::CheckIPHTalkInfo()
{
    LOCAL_CFG_T &stLocalCfg = GetLocalCfg();
    UINT32      ulMaxIPHTalkTime = (5+stLocalCfg.ulMaxIPHTalkTime)*1000;  // 比客户端多几秒钟，保证正常情况下由客户端发起超时处理
    UINT32      ulMAXIPHWaitAcceptTime = (stLocalCfg.ulMAXIPHWaitAcceptTime * 1000);
    BOOL        bRet = FALSE;
    UINT32      ulCurrTime = gos_get_uptime_1ms();

    for (INT32 i=(g_vIPHApplyInfo.size()-1); i>=0; i--)
    {
        if (g_vIPHApplyInfo[i].ulStatus == IPH_STATUS_WAIT)
        {
            if ((g_vIPHApplyInfo[i].ulApplyUptime + ulMAXIPHWaitAcceptTime) < ulCurrTime)
            {
                TaskLog(LOG_WARN, "IPH talk(%u,%u,%u) at wait status is timeout",
                        g_vIPHApplyInfo[i].ulTrainUnitID,
                        g_vIPHApplyInfo[i].ulCarriageID,
                        g_vIPHApplyInfo[i].ulIPHDevID);

                //增加对IPH的结束通知
                if (!StopIPHTalk(g_vIPHApplyInfo[i].ulTrainUnitID, g_vIPHApplyInfo[i].ulCarriageID, g_vIPHApplyInfo[i].ulIPHDevID))
                {
                    continue;
                }

                g_vIPHApplyInfo.erase(g_vIPHApplyInfo.begin()+i);
                bRet = TRUE;
            }
        }
        else if (g_vIPHApplyInfo[i].ulStatus == IPH_STATUS_OVER)
        {
            if ((g_vIPHApplyInfo[i].ulStopUptime + g_ulIPHOverHoldTime) < ulCurrTime)
            {
                TaskLog(LOG_WARN, "IPH talk(%u,%u,%u) is not ready",
                        g_vIPHApplyInfo[i].ulTrainUnitID,
                        g_vIPHApplyInfo[i].ulCarriageID,
                        g_vIPHApplyInfo[i].ulIPHDevID);

                //增加对IPH的结束通知
                if (!StopIPHTalk(g_vIPHApplyInfo[i].ulTrainUnitID, g_vIPHApplyInfo[i].ulCarriageID, g_vIPHApplyInfo[i].ulIPHDevID))
                {
                    continue;
                }

                g_vIPHApplyInfo.erase(g_vIPHApplyInfo.begin()+i);
                bRet = TRUE;
            }
        }
        else if (g_vIPHApplyInfo[i].ulStatus == IPH_STATUS_TALK)
        {
            if ((g_vIPHApplyInfo[i].ulAcceptTime + ulMaxIPHTalkTime) < ulCurrTime)
            {
                TaskLog(LOG_WARN, "IPH talk(%u,%u,%u) at talking status is timeout",
                        g_vIPHApplyInfo[i].ulTrainUnitID,
                        g_vIPHApplyInfo[i].ulCarriageID,
                        g_vIPHApplyInfo[i].ulIPHDevID);

                //增加对IPH的结束通知
                if (!StopIPHTalk(g_vIPHApplyInfo[i].ulTrainUnitID, g_vIPHApplyInfo[i].ulCarriageID, g_vIPHApplyInfo[i].ulIPHDevID))
                {
                    continue;
                }

                g_vIPHApplyInfo.erase(g_vIPHApplyInfo.begin()+i);
                bRet = TRUE;
            }
        }
    }

    return bRet;
}

VOID TaskSA::NewRecBrdInfo(REC_BRD_SEND_INFO_T &stInfo)
{
    for (UINT32 i=0; i<g_vRecBrdSendInfo.size(); i++)
    {
        if (g_vRecBrdSendInfo[i].ulTrainUnitID == stInfo.ulTrainUnitID &&
            g_vRecBrdSendInfo[i].ulBrdSeqID == stInfo.ulBrdSeqID )
        {
            memcpy(&g_vRecBrdSendInfo[i], &stInfo, sizeof(stInfo));
            return;
        }
    }

    g_vRecBrdSendInfo.push_back(stInfo);
}

VOID TaskSA::DelRecBrdInfo(UINT32 ulTrainUnitID, UINT32 ulBrdSeqID)
{
    for (UINT32 i=0; i<g_vRecBrdSendInfo.size(); i++)
    {
        if (g_vRecBrdSendInfo[i].ulTrainUnitID == ulTrainUnitID &&
            g_vRecBrdSendInfo[i].ulBrdSeqID == ulBrdSeqID )
        {
            g_vRecBrdSendInfo.erase(g_vRecBrdSendInfo.begin()+i);
            return;
        }
    }
}

VOID TaskSA::NewRealBrdInfo(REAL_BRD_SEND_INFO_T &stInfo)
{
    for (UINT32 i=0; i<g_vRealBrdSendInfo.size(); i++)
    {
        if (g_vRealBrdSendInfo[i].ulTrainUnitID == stInfo.ulTrainUnitID &&
            g_vRealBrdSendInfo[i].ulBrdSeqID == stInfo.ulBrdSeqID )
        {
            memcpy(&g_vRealBrdSendInfo[i], &stInfo, sizeof(stInfo));
            return;
        }
    }

    g_vRealBrdSendInfo.push_back(stInfo);
}

VOID TaskSA::DelRealBrdInfo(/*UINT32 ulTrainUnitID, */UINT32 ulBrdSeqID)
{
    for (UINT32 i=0; i<g_vRealBrdSendInfo.size(); i++)
    {
        if (//g_vRealBrdSendInfo[i].ulTrainUnitID == ulTrainUnitID &&
            g_vRealBrdSendInfo[i].ulBrdSeqID == ulBrdSeqID )
        {
            g_vRealBrdSendInfo.erase(g_vRealBrdSendInfo.begin()+i);
            return;
        }
    }
}

VOID TaskSA::GetRealBrdTrains(UINT32 ulBrdSeqID, VECTOR<UINT32> &vTrainUnitID)
{
    vTrainUnitID.clear();
    for (UINT32 i=0; i<g_vRealBrdSendInfo.size(); i++)
    {
        if (g_vRealBrdSendInfo[i].ulBrdSeqID == ulBrdSeqID )
        {
            vTrainUnitID.push_back(g_vRealBrdSendInfo[i].ulTrainUnitID);
        }
    }
}

VOID TaskSA::GetSAPID(UINT32 ulTrainUnitID, VECTOR<PID_T> &vPID)
{
    for (UINT32 i=0; i<g_vSAPID.size(); i++)
    {
        if (g_vSAPID[i].ulTrainUnitID == ulTrainUnitID)
        {
            vPID.push_back(g_vSAPID[i].stSAPID);
        }
    }
}

VOID TaskSA::SaveSAPID(UINT32 ulTrainUnitID, CHAR *szSAID, PID_T *pstPID)
{
    UINT32  ulUpdateTime = gos_get_uptime();

    for (UINT32 i=0; i<g_vSAPID.size(); i++)
    {
        if (strcmp(g_vSAPID[i].acSAID, szSAID) == 0)
        {
            if (g_vSAPID[i].ulTrainUnitID != ulTrainUnitID)
            {
                TaskLog(LOG_ERROR, "train unit id %u is different with before (%u), SA id is %s",
                        g_vSAPID[i].ulTrainUnitID, ulTrainUnitID, szSAID);

                g_vSAPID[i].ulTrainUnitID = ulTrainUnitID;
            }

            if (memcmp(&g_vSAPID[i].stSAPID, pstPID, sizeof(PID_T)) != 0)
            {
                TaskLog(LOG_INFO, "update train %s unit id %u", szSAID, ulTrainUnitID);
                memcpy(&g_vSAPID[i].stSAPID, pstPID, sizeof(PID_T));
            }

            g_vSAPID[i].ulUpdateTime = ulUpdateTime;
            return;
        }
    }

    SA_PID_T    stSAPID = {0};

    stSAPID.ulTrainUnitID = ulTrainUnitID;
    strncpy(stSAPID.acSAID, szSAID, sizeof(stSAPID.acSAID)-1);
    stSAPID.ulUpdateTime = ulUpdateTime;
    memcpy(&stSAPID.stSAPID, pstPID, sizeof(PID_T));

    TaskLog(LOG_INFO, "add train unit id %u", ulTrainUnitID);

    g_vSAPID.push_back(stSAPID);
}

VOID TaskSA::CheckSAPID()
{
    UINT32      ulMaxWaitTime = 30;
    UINT32      ulDeadLine = gos_get_uptime() - ulMaxWaitTime;

    for (UINT32 i=0; i<g_vSAPID.size(); i++)
    {
        if (g_vSAPID[i].ulUpdateTime < ulDeadLine)
        {
            TaskLog(LOG_WARN, "SA(%u, %s) is disconnect",
                   g_vSAPID[i].ulTrainUnitID, g_vSAPID[i].acSAID);

            g_vSAPID.erase(g_vSAPID.begin()+i);

            return;
        }
    }
}

BOOL TaskSA::SendTrainSA(const CHAR *szMsgName, UINT32 ulTrainUnitID, GJsonParam &Param)
{
    GJsonParam      ReqParam;
    UINT32          ulSeqID = gos_get_current_time();

    ReqParam.Add("MsgName", (CHAR*)szMsgName);
    ReqParam.Add("TrainUnitID", ulTrainUnitID);
    ReqParam.Add("MsgSeqID", ulSeqID);
    ReqParam.Add("MsgInfo", Param);

    CHAR    *szMsg = ReqParam.GetString();

    return SendTrainSA(ulTrainUnitID, szMsg);
}

BOOL TaskSA::SendTrainSA(UINT32 ulTrainUnitID, CHAR *szReq)
{
    VECTOR<PID_T>       vPID;

    GetSAPID(ulTrainUnitID, vPID);

    if (vPID.size() == 0)
    {
        TaskMsgLog(LOG_ERROR, "send SA of %u failed: %s", ulTrainUnitID, szReq);
        return FALSE;
    }

    BOOL    bRet = FALSE;

    for (UINT32 i=0; i<vPID.size(); i++)
    {
        if (SendClient(&vPID[i], EV_SA_DIS_DC_RSP, szReq, strlen(szReq)+1))
        {
            bRet = TRUE;
            if (i == 0)
            {
                TaskMsgLog(LOG_INFO, "send SA of %u succ: %s", ulTrainUnitID, szReq);
            }
        }
        else
        {
            TaskMsgLog(LOG_ERROR, "send SA of %u failed: %s", ulTrainUnitID, szReq);
        }
    }

    return bRet;
}

BOOL TaskSA::SendDC(UINT32 ulDCUserID, UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &Param)
{
    PID_T stPID;

    if (!GetDCPID(ulDCUserID, &stPID))
    {
        TaskLog(LOG_ERROR, "DC %u has not logined", ulDCUserID);
        return FALSE;
    }

    GJsonParam ReqParam;

    ReqParam.Add("MsgName", (CHAR *)szMsgName);
    ReqParam.Add("MsgSeqID", GetReqMsgSeqID() );
    ReqParam.Add("MsgInfo", Param);

    CHAR *szMsg = ReqParam.GetString();

    return SendClient(&stPID, usMsgID, szMsg, strlen(szMsg) + 1);
}

TaskSA::TaskSA(UINT16 usDispatchID):GBaseTask(MODULE_SA, "SA", usDispatchID)
{
    m_pDao = NULL;
    m_ulDCUserID = INVALID_USER_ID;

    RegisterMsg(EV_SA_DIS_REQ);
    RegisterMsg(EV_DC_DIS_SA_REQ);
    RegisterMsg(EV_NOCC_REQ);
}

BOOL TaskSA::Init()
{
    GJsonTupleParam         JsonTupleParam;

    if (!m_pDao)
    {
        m_pDao = GetCfgDao();
        if (!m_pDao)
        {
            TaskLog(LOG_ERROR, "get db dao failed");

            return FALSE;
        }
    }

    SetLoopTimer(CHECK_TIMER, 3*1000);

    SetTaskStatus(TASK_STATUS_WORK);

    TaskLog(LOG_INFO, "Task SA init successful!");

    return TRUE;
}

VOID TaskSA::OnServerDisconnectClient(UINT16 usClientID)
{
}

VOID TaskSA::OnSAReq(CHAR *szReq, UINT32 ulMsgLen)
{
    GJson       Json;
    GJson       SubJson;
    BOOL        bRet;
    CHAR        *szMsgType;
    CHAR        *szMsgInfo;
    UINT32      ulSeqID;
    UINT32      ulTrainUnitID = INVALID_TRAIN_UNIT_ID;
    CHAR        acSAID[128] = {0};

    if (!szReq || ulMsgLen == 0)
    {
        return;
    }

    bRet = Json.Parse(szReq);
    if (!bRet)
    {
        TaskLog(LOG_ERROR, "invalid SA msg: %s", szReq);
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

    szMsgInfo = Json.GetValue("MsgInfo");
    if (!szMsgInfo)
    {
        TaskLog(LOG_ERROR, "get MsgInfo failed: %s", szReq);
        goto error;
    }

    if (Json.GetValue("SAID", acSAID, sizeof(acSAID)) )
    {
        if (Json.GetValue("TrainUnitID", &ulTrainUnitID))
        {
            SaveSAPID(ulTrainUnitID, acSAID, GetMsgSender());
        }
    }

    if (!SubJson.Parse(szMsgInfo))
    {
        TaskLog(LOG_ERROR, "parse MsgInfo failed: %s", szReq);
        goto error;
    }

    /*
    if (ulTrainUnitID == INVALID_TRAIN_UNIT_ID)
    {
        TaskLog(LOG_INFO, "get SA msg %s from %s", szMsgType, acSAID);
    }
    else
    {
        TaskLog(LOG_INFO, "get SA msg %s from train %u %s", szMsgType, ulTrainUnitID, acSAID);
    }*/

    // 周期性消息
    if (strcmp(szMsgType, MSG_SAHeartBeat) == 0 ||
        strcmp(szMsgType, MSG_SAGetNTPCfg) == 0 ||
        strcmp(szMsgType, MSG_SAGetCfg) == 0 )
    {
        TaskLog(LOG_DETAIL, "get SA msg: %s", szReq);
    }
    else
    {
        // 其他为业务消息，单独存储
        TaskMsgLog(LOG_INFO, "get SA msg: %s", szReq);
    }

    if (strcmp(szMsgType, MSG_SAHeartBeat) == 0)
    {
        OnSAHeartBeatReq(szMsgInfo, SubJson);
    }
    else if (strcmp(szMsgType, MSG_SAGetNTPCfg) == 0)
    {
        OnSAGetNTPCfgReq(szReq, SubJson);
    }
    else if (strcmp(szMsgType, MSG_SAGetCfg) == 0)
    {
        OnSAGetCfgReq(szReq, SubJson);
    }
    else if (strcmp(szMsgType, MSG_SAApplyIPHTalk) == 0)
    {
        OnSAApplyIPHTalkReq(szReq, SubJson);
    }
    else if (strcmp(szMsgType, MSG_SAStopIPHTalk) == 0)
    {
        OnSAStopIPHTalkReq(szReq, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCStartRecBrd) == 0)
    {
        OnSAStartRecBrdRsp(szReq, SubJson);
    }
    else if (strcmp(szMsgType, MSG_SAGetRecBrdCfg) == 0)
    {
        OnSAGetRecBrdCfgRsp(szReq, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCStartRealBrd) == 0)
    {
        OnSAStartRealBrdRsp(szReq, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCStopRealBrd) == 0)
    {
        OnSAStopRealBrdRsp(szReq, SubJson);
    }
    else if (strcmp(szMsgType, MSG_SAIPHApplyACKMsg) == 0)
    {
        OnSAIPHApplyACKMsg(szReq, SubJson);
    }
    else if (strcmp(szMsgType, MSG_SAIPHPauseRspMsg) == 0)
    {
        OnSAPauseIPHTalkRsp(szReq, SubJson);
    }
    else if (strcmp(szMsgType, MSG_SAIPHDevRestReqMsg) == 0)
    {
        OnSAIPHDevResetReq(szReq, SubJson);
    }
    else if (strcmp(szMsgType, MSG_SAIPHAudioUploadMsg) == 0)
    {
        OnSAUpLoadIPHAudioReq(szReq, SubJson);
    }
    else
    {
        TaskLog(LOG_ERROR, "invalid SA msg: %s", szReq);
        goto error;
    }

//    TaskLog(LOG_DETAIL, "SA msg done: %s", szMsgType);

    return;

error:
    return;
}

VOID TaskSA::OnSAHeartBeatReq(CHAR *szMsgInfo, GJson &Json)
{
    UINT32      ulTrainUnitID;
    CHAR        acSAID[64];
    UINT32      ulSATCid;
    IPH_AUDIO_NET_INFO_T stIPHAudioNetInfo;


    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID) )
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in MsgInfo: %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("SAID", acSAID, sizeof(acSAID)) )
    {
        TaskLog(LOG_ERROR, "invalid SAID in MsgInfo: %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("MasterStatus", &stIPHAudioNetInfo.ulMasterStatus) )
    {
        TaskLog(LOG_ERROR, "invalid MasterStatus in MsgInfo: %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("IPHStatus", &stIPHAudioNetInfo.ulIPHStatus) )
    {
        TaskLog(LOG_ERROR, "invalid IPHStatus in MsgInfo: %s", szMsgInfo);
        return;
    }

    if (!Json.GetIP("IPHAddr", stIPHAudioNetInfo.aucIPHAddr) )
    {
        TaskLog(LOG_ERROR, "invalid IPHAddr in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("TCID", &ulSATCid) )
    {
        TaskLog(LOG_ERROR, "invalid TCID in msg %s", szMsgInfo);
        return;
    }

    // 缺省TC1为主用
    if (stIPHAudioNetInfo.ulMasterStatus == PA_MASTER_STATUS)
    {
        SetIPHAudioAddr(ulTrainUnitID, stIPHAudioNetInfo);
    }

    SaveSAPID(ulTrainUnitID, acSAID, GetMsgSender());

    GJsonParam      Param;

    Param.Add("Time", gos_get_current_time());
    SendRsp(MSG_SAHeartBeat, 0, Param);
}

VOID TaskSA::OnSAGetNTPCfgReq(CHAR *szMsgInfo, GJson &Json)
{
    UINT32      ulTrainUnitID;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID) )
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in MsgInfo: %s", szMsgInfo);
        return;
    }

    GJsonParam      Param;
    APP_BASIC_CFG_T stAPPBasicCfg;

    GetAPPBasicCfg(stAPPBasicCfg);

    Param.Add("NTPServerAddr", stAPPBasicCfg.acNTPServerAddr);
    Param.Add("NTPServerPort", stAPPBasicCfg.ulNTPServerPort);
    Param.Add("NTPSyncPeriod", stAPPBasicCfg.ulNTPSyncPeriod);

    // Param在宏内部不会自动转换成char*， 需要显式转换
    TaskMsgLog(LOG_INFO, "send SA rsp msg: %s", (CHAR*)Param);
    SendRsp(MSG_SAGetNTPCfg, GetMsgSeqID(), Param);
}

VOID TaskSA::OnSAGetCfgReq(CHAR *szMsgInfo, GJson &Json)
{
    GJsonParam      Param;
    APP_BASIC_CFG_T stAPPBasicCfg;
    LOCAL_CFG_T     &stLocalCfg = GetLocalCfg();

    GetAPPBasicCfg(stAPPBasicCfg);

    Param.Add("ResetButtonCheckTime", stLocalCfg.ulRadioResetButtonCheckTime);
    Param.Add("ResetAPPCheckTime", stLocalCfg.ulRadioResetAPPCheckTime);
    Param.Add("NTPServerAddr", stAPPBasicCfg.acNTPServerAddr);
    Param.Add("NTPServerPort", stAPPBasicCfg.ulNTPServerPort);
    Param.Add("NTPSyncPeriod", stAPPBasicCfg.ulNTPSyncPeriod);

    Param.Add("IPHVideoProxyMode", stLocalCfg.bIPHVideoProxyMode);
    Param.Add("SendSelfCheckMsgPeriod", stLocalCfg.ulSendSelfCheckMsgPeriod);
    Param.Add("TXDataCaptureMode", stLocalCfg.ulTXDataCaptureMode);

    TaskMsgLog(LOG_INFO, "send SA rsp msg: %s", (CHAR*)Param);

    SendRsp(MSG_SAGetCfg, GetMsgSeqID(), Param);
}

/*
{"MsgName":" SAApplyIPHTalkReq ",  // 接口名称
"MsgSeqID":"123",          // 接口消息序列号
"MsgInfo":                 // 接口信息
[{"TrainUnitID":"123",
"CarriageID":"2",
"IPHDevID":"3",
"VideoURL":"rtsp://192.168.30.189:11003/h264/ch1/main/av_stream"},
{"TrainUnitID":"123",
"CarriageID":"2",
"IPHDevID":"4",
"VideoURL":"rtsp://192.168.30.189:11004/h264/ch1/main/av_stream"}]
}
*/
VOID TaskSA::OnSAApplyIPHTalkReq(CHAR *szReq, GJson &Json)
{
    VECTOR<GJson*>           &vSubJson = Json.GetSubJson();
    GJson                    *SubJson;
    UINT32                   ulDuplicatedNum = 0;
    BOOL                     bIsNewApply;
    CHAR                     acSAID[64];
    IPH_TALK_INFO_T          stApplyInfo = {0};
    STATION_INFO_T           stStation = {0};
    TRAIN_INFO_T             stTrainInfo = {0};
    UINT32                   ulDCUserIDForDepotIPH = GetLocalCfg().ulDCUserIDForDepotIPH;
    VECTOR<IPH_TALK_INFO_T>  vIPHApplyInfo;

    memset(&stApplyInfo, 0, sizeof(stApplyInfo));
    stApplyInfo.ulApplyTime = gos_get_current_time();

    if (vSubJson.size() == 0)
    {
        GosLog(LOG_ERROR, "invalid iph apply num in msg:%s", szReq);
        return;
    }

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        SubJson = vSubJson[i];

        stApplyInfo.ulStatus = IPH_STATUS_WAIT;
        stApplyInfo.ulApplyUptime = gos_get_uptime_1ms();
        if (!SubJson->GetValue("TrainUnitID", &stApplyInfo.ulTrainUnitID))
        {
            TaskLog(LOG_ERROR, "invalid TrainUnitID in msg %s", szReq);
            return;
        }

        if (i == 0)
        {
            if (!GetTrainInfo(stApplyInfo.ulTrainUnitID, stTrainInfo))
            {
                TaskLog(LOG_ERROR, "unknown train %u in msg %s", stApplyInfo.ulTrainUnitID, szReq);
                return;
            }

            strcpy(stApplyInfo.acTrainID, stTrainInfo.acTrainID);

            if (!GetStationInfo(stTrainInfo.ulStationID, &stStation))
            {
                TaskLog(LOG_ERROR, "unknown position of train %u in msg %s", stApplyInfo.ulTrainUnitID, szReq);
                return;
            }

            if (ulDCUserIDForDepotIPH == INVALID_USER_ID)
            {
                if (stStation.ulDepotID != DEPOT_ID_MAINLINE)
                {
                    TaskLog(LOG_WARN, "IPH from depot train %u is ignored", stApplyInfo.ulTrainUnitID);
                    return;
                }
            }
        }

        if (!SubJson->GetValue("SAID", acSAID, sizeof(acSAID)))
        {
            TaskLog(LOG_ERROR, "invalid SAID in msg %s", szReq);
            return;
        }

        if (!SubJson->GetValue("CarriageID", &stApplyInfo.ulCarriageID))
        {
            TaskLog(LOG_ERROR, "invalid CarriageID in msg %s", szReq);
            return;
        }

        if (!SubJson->GetValue("DoorID", &stApplyInfo.ulIPHDevDoorID))
        {
            TaskLog(LOG_ERROR, "invalid DoorID in msg %s", szReq);
            return;
        }

        if (!SubJson->GetValue("IPHDevID", &stApplyInfo.ulIPHDevID))
        {
            TaskLog(LOG_ERROR, "invalid IPHDevID in msg %s", szReq);
            return;
        }

        if (!SubJson->GetIP("IPHDevIP", stApplyInfo.aucIPHDevIP))
        {
            TaskLog(LOG_ERROR, "invalid IPHDevID in msg %s", szReq);
            return;
        }

        if (!SubJson->GetValue("VideoURL", stApplyInfo.acVideoURL, sizeof(stApplyInfo.acVideoURL)))
        {
            TaskLog(LOG_ERROR, "invalid VideoURL in msg %s", szReq);
            return;
        }

        bIsNewApply = NewIPHTalk(stApplyInfo);
        if (bIsNewApply)
        {
            vIPHApplyInfo.push_back(stApplyInfo);

            TaskLog(LOG_INFO, "recv new IPH req(%u, %u, %u)", stApplyInfo.ulTrainUnitID, stApplyInfo.ulCarriageID, stApplyInfo.ulIPHDevID);
        }
        else
        {
            TaskLog(LOG_DETAIL, "recv duplicated IPH req(%u, %u, %u)", stApplyInfo.ulTrainUnitID, stApplyInfo.ulCarriageID, stApplyInfo.ulIPHDevID);
            ulDuplicatedNum ++;
        }

        if (i == 0)
        {
            SaveSAPID(stApplyInfo.ulTrainUnitID, acSAID, GetMsgSender());
        }
    }

    if (ulDuplicatedNum > 0)
    {
        TaskLog(LOG_INFO, "recv %u duplicated IPH reqs from %u", ulDuplicatedNum, stApplyInfo.ulTrainUnitID);
    }

    if (vIPHApplyInfo.size())
    {
        m_pDao->NewIPHTalk(vIPHApplyInfo);
        TaskLog(LOG_DETAIL, "save iph talk info to db over");
    }

    // 车辆段IPH只发送给特定的调度台
    if (stStation.ulDepotID != DEPOT_ID_MAINLINE &&
        ulDCUserIDForDepotIPH != INVALID_USER_ID)
    {
        PID_T   stPID = {0};

        if (GetDCPID(ulDCUserIDForDepotIPH, &stPID))
        {
            SendClient(&stPID, EV_SA_DIS_DC_REQ, szReq, strlen(szReq)+1);
        }
    }
    else
    {
        SendAllClient(DIS_CLIENT_DC, EV_SA_DIS_DC_REQ, szReq, strlen(szReq)+1);
    }
}

VOID TaskSA::OnSAStopIPHTalkReq(CHAR *szReq, GJson &Json)
{
    UINT32      ulTrainUnitID;
    UINT32      ulCarriageID;
    UINT32      ulIPHDevID;
    CHAR        acSAID[64];
    IPH_TALK_INFO_T stInfo;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in msg STOP_IPH_TALK_REQ");
        return;
    }

    if (!Json.GetValue("SAID", acSAID, sizeof(acSAID)))
    {
        TaskLog(LOG_ERROR, "invalid SAID in msg STOP_IPH_TALK_REQ");
        return;
    }

    if (!Json.GetValue("CarriageID", &ulCarriageID))
    {
        TaskLog(LOG_ERROR, "invalid CarriageID in msg STOP_IPH_TALK_REQ");
        return;
    }

    if (!Json.GetValue("IPHDevID", &ulIPHDevID))
    {
        TaskLog(LOG_ERROR, "invalid IPHDevID in msg STOP_IPH_TALK_REQ");
        return;
    }

    TaskLog(LOG_INFO, "recv IPH stop talk req(%u, %u, %u)", ulTrainUnitID, ulCarriageID, ulIPHDevID);

    if (ulIPHDevID == IPH_TALK_RESET &&
        ulCarriageID == IPH_TALK_RESET)
    {
        GosLog(LOG_DETAIL, "get iph clean rsp");
        return ;
    }

    if (StopIPHTalk(ulTrainUnitID, ulCarriageID, ulIPHDevID, &stInfo))
    {
        m_pDao->UpdateIPHTalk(&stInfo);
    }

    SaveSAPID(ulTrainUnitID, acSAID, GetMsgSender());

    SendAllClient(DIS_CLIENT_DC, EV_SA_DIS_DC_REQ, szReq, strlen(szReq)+1);
}

VOID TaskSA::OnSAStartRecBrdRsp(CHAR *szReq, GJson &Json)
{
    CHAR        acSAID[64];
    UINT32      ulRecID;
    REC_BRD_REPLY_T stInfo = {0};

    stInfo.ulReplyTime = gos_get_current_time();
    if (!Json.GetValue("TrainUnitID", &stInfo.ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("SAID", acSAID, sizeof(acSAID)))
    {
        TaskLog(LOG_ERROR, "invalid SAID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("RecID", &ulRecID))
    {
        TaskLog(LOG_ERROR, "invalid RecID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("BrdSeqID", &stInfo.ulBrdSeqID))
    {
        TaskLog(LOG_ERROR, "invalid BrdSeqID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Result", &stInfo.iResult))
    {
        TaskLog(LOG_ERROR, "invalid Result in msg %s", szReq);
        return;
    }

    DelRecBrdInfo(stInfo.ulTrainUnitID, stInfo.ulBrdSeqID);

    TaskLog(LOG_INFO, "recv PA start rec broadcast rsp(%u, %u, %u, %d)", stInfo.ulTrainUnitID, ulRecID, stInfo.ulBrdSeqID, stInfo.iResult);

    SaveSAPID(stInfo.ulTrainUnitID, acSAID, GetMsgSender());

    m_pDao->UpdateRecBrdReply(stInfo);

    if (!IsNOCCRecBrd(stInfo.ulBrdSeqID))
    {
        UINT32  ulDCUserID = BrdInfo::GetBrdDCUserID(FALSE, stInfo.ulBrdSeqID);
        PID_T   stPID;

        if (!GetDCPID(ulDCUserID, &stPID))
        {
            TaskLog(LOG_ERROR, "unknown dc of rec brd %u", stInfo.ulBrdSeqID);
            return;
        }

        SendClient(&stPID, EV_SA_DIS_DC_REQ, szReq, strlen(szReq)+1);
        return;
    }

    // NOCC 广播
    BRD_MAP_INFO_T  *pstBrdInfo = BrdInfo::GetBrdInfo(FALSE, stInfo.ulBrdSeqID);

    if (!pstBrdInfo)
    {
        TaskLog(LOG_WARN, "unknown rec brd %u", stInfo.ulBrdSeqID);
        return;
    }

    GJsonParam      Param;

    Param.Add("PAIndex", pstBrdInfo->ulNOCCPAIndex);
    Param.Add("TrainID", stInfo.ulTrainUnitID);
    Param.Add("Result", stInfo.iResult==REC_BRD_RESULT_SUCC?1:0);   // 1：成功 0：失败

    TaskMsgLog(LOG_INFO, "rec PA of %u %s", pstBrdInfo->ulNOCCPAIndex, stInfo.iResult==REC_BRD_RESULT_SUCC?"succ":"fail");

    SendNOCCMsg(MSG_OCC_StartTrainRecPARsp, pstBrdInfo->ulMsgSeqID, Param);

    BrdInfo::DelBrdInfo(FALSE, stInfo.ulBrdSeqID);
}

VOID TaskSA::OnSAGetRecBrdCfgRsp(CHAR *szReq, GJson &Json)
{
    GJson               *SubJson;
    CHAR                *szRecInfo;
    UINT32              ulTrainUnitID;
    REC_BRD_CFG_T       stCfg = {0};
    VECTOR<REC_BRD_CFG_T>   vCfg;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in msg %s", szReq);
        return;
    }

    szRecInfo = Json.GetValue("RecBrdCfg");
    if (!szRecInfo)
    {
        TaskLog(LOG_ERROR, "no RecBrdCfg in msg %s", szReq);
        return;
    }

    GJson               RecJson(szRecInfo);
    VECTOR<GJson*>      &vSubJson = RecJson.GetSubJson();

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        SubJson = vSubJson[i];

        memset(&stCfg, 0, sizeof(stCfg));
        if (!SubJson->GetValue("RecID", &stCfg.ulRecID))
        {
            TaskLog(LOG_ERROR, "invalid RecID in msg %s", szReq);
            return;
        }

        szRecInfo = SubJson->GetValue("RecInfo");
        if (!szRecInfo || *szRecInfo == '\0')
        {
            TaskLog(LOG_ERROR, "invalid RecInfo in msg %s", szReq);
            return;
        }
        strncpy(stCfg.acRecInfo, szRecInfo, sizeof(stCfg.acRecInfo)-1);

        vCfg.push_back(stCfg);

        TaskLog(LOG_DETAIL, "rec brd cfg %u: %s", stCfg.ulRecID, stCfg.acRecInfo);
    }

    /*
    if (!m_pDao->SetRecBrdCfg(vCfg))
    {
        TaskLog(LOG_ERROR, "set rec brd cfg failed");
    }

    InitRecBrdCfg(vCfg); */

    TaskLog(LOG_INFO, "rec brd cfg: %s", szReq);

    SendAllClient(DIS_CLIENT_DC, EV_DC_DIS_RSP, szReq);

    CHAR        *szMsgInfo = Json.GetText();

    SendNOCCMsg(MSG_OCC_GetPARecCfgRsp, 0, szMsgInfo);
}

//处理人工广播开始请求的应答  ZWJ
VOID TaskSA::OnSAStartRealBrdRsp(CHAR *szMsgInfo, GJson &Json)
{
    UINT32      ulDCUserID;
    UINT32      ulTrainUnitID;
    UINT32      ulBrdSeqID;
    INT32       iResult;
    GJson       SubJson;
    UINT16      usPARecvAudioPort;
    GJsonParam  Param;

    if (!Json.GetValue("BrdSeqID", &ulBrdSeqID))
    {
        TaskLog(LOG_ERROR, "invalid BrdSeqID in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("DCUserID", &ulDCUserID))
    {
        TaskLog(LOG_ERROR, "invalid DCUserID in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("Result", &iResult))
    {
        TaskLog(LOG_ERROR, "invalid Result in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("PARecvAudioPort", &usPARecvAudioPort))
    {
        TaskLog(LOG_ERROR, "invalid PARecvAudioPort in msg %s", szMsgInfo);
        return;
    }

    Param.Add("BrdSeqID", ulBrdSeqID);
    Param.Add("DCUserID", ulDCUserID);
    Param.Add("TrainUnitID", ulTrainUnitID);
    Param.Add("Result", iResult);
    Param.Add("PARecvAudioPort", usPARecvAudioPort);

    TaskLog(LOG_INFO, "recv train %u brd result: %d", ulTrainUnitID, iResult);

    if (ulDCUserID != INVALID_USER_ID)
    {
        SendDC(ulDCUserID, EV_SA_DIS_DC_REQ, MSG_DCStartRealBrd, Param);
        return;
    }

    // NOCC 广播
    BRD_MAP_INFO_T  *pstBrdInfo = BrdInfo::GetBrdInfo(TRUE, ulBrdSeqID);

    if (!pstBrdInfo)
    {
        TaskLog(LOG_WARN, "unknown real PA %u", ulBrdSeqID);
        return;
    }

    Param.Clear();
    Param.Add("PAIndex", pstBrdInfo->ulNOCCPAIndex);
    Param.Add("TrainID", ulTrainUnitID);
    Param.Add("Result", iResult==REC_BRD_RESULT_SUCC?1:0);   // 1：成功 0：失败

    TaskMsgLog(LOG_INFO, "start real PA of %u %s", pstBrdInfo->ulNOCCPAIndex, iResult==REC_BRD_RESULT_SUCC?"succ":"fail");

    SendNOCCMsg(MSG_OCC_StartTrainRealPARsp, pstBrdInfo->ulMsgSeqID, Param);
}

VOID TaskSA::OnSAStopRealBrdRsp(CHAR *szMsgInfo, GJson &Json)
{
    // TODO
}

BOOL GetWaitACKIPHTalkInfo(UINT32 ulTrainUnitID, UINT32 ulCarriageID, UINT32 ulIPHDevID, IPH_TALK_INFO_T* &pstIPHTalkInfo)
{
    for (UINT32 i=0; i<g_vIPHApplyInfo.size(); i++)
    {
        if (g_vIPHApplyInfo[i].ulTrainUnitID == ulTrainUnitID &&
            g_vIPHApplyInfo[i].ulCarriageID  == ulCarriageID  &&
            g_vIPHApplyInfo[i].ulIPHDevID    == ulIPHDevID    &&
            g_vIPHApplyInfo[i].ulStatus      == IPH_STATUS_WAIT_ACK)
        {
            pstIPHTalkInfo = &g_vIPHApplyInfo[i];

            return TRUE;
        }
    }

    return FALSE;
}

VOID TaskSA::OnSAIPHApplyACKMsg(CHAR *szMsgInfo, GJson &Json)
{
    UINT32                  ulTrainUnitID;
//    CHAR                    acSAID[64];
    UINT32                  ulIPHCarriageID;
    UINT32                  ulIPHDevID;
//    UINT16                  usIPHDevDoorID;
    IPH_TALK_INFO_T         *pstIPHTalkInfo = NULL;
    GJsonParam              Param;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("CarriageID", &ulIPHCarriageID))
    {
        TaskLog(LOG_ERROR, "invalid CarriageID in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("IPHDevID", &ulIPHDevID))
    {
        TaskLog(LOG_ERROR, "invalid IPHDevID in msg %s", szMsgInfo);
        return;
    }

    if (!GetWaitACKIPHTalkInfo(ulTrainUnitID, ulIPHCarriageID, ulIPHDevID, pstIPHTalkInfo))
    {
        GosLog(LOG_ERROR, "IPH(%u %u %u ) is not wait ACK dc", ulTrainUnitID, ulIPHCarriageID, ulIPHDevID);
        return ;
    }

    pstIPHTalkInfo->ulStatus = IPH_STATUS_TALK;
    pstIPHTalkInfo->ulGetACKTime = gos_get_current_time();

    PID_T   stPID;

    if (!GetDCPID(pstIPHTalkInfo->ulAcceptDCUserID, &stPID))
    {
        GosLog(LOG_ERROR, "not find dc(%u) pid in OnSAIPHApplyACKMsg", pstIPHTalkInfo->ulAcceptDCUserID);
        return;
    }

    SendClient(&stPID, EV_SA_DIS_DC_REQ, szMsgInfo, strlen(szMsgInfo)+1);

    GosLog(LOG_DETAIL, "send iph talk ack to dc(%u), msg is %s", pstIPHTalkInfo->ulAcceptDCUserID, szMsgInfo);

    return ;
}

VOID TaskSA::OnSAPauseIPHTalkRsp(CHAR *szMsgInfo, GJson &Json)
{
    CHAR               acSAID[64];
    UINT32             ulTrainUnitID;
    UINT32             ulCarriageID;
    UINT32             ulIPHDevID;
    UINT32             ulIPHTalkPauseRet;

    Json.GetValue("TrainUnitID", &ulTrainUnitID);
    Json.GetValue("SAID", acSAID, sizeof(acSAID));
    Json.GetValue("CarriageID", &ulCarriageID);
    Json.GetValue("IPHDevID", &ulIPHDevID);
    Json.GetValue("IPHPauseRet", &ulIPHTalkPauseRet);

    if (ulIPHTalkPauseRet != IPH_PAUSE_SUCC)
    {
        GosLog(LOG_ERROR, "iph(%u %u %u) pause fail, ret is %u", ulTrainUnitID, ulCarriageID, ulIPHDevID, ulIPHTalkPauseRet);

        // TODO 是否 1、发给DC让DC重发pause请求， 2、DIS自己重发
        return ;
    }

    GosLog(LOG_INFO, "get iph(%u %u %u) pause succ", ulTrainUnitID, ulCarriageID, ulIPHDevID);

    return ;
}

BOOL GetAccpetIPHDCUser(UINT32 ulTrainUnitID, UINT32 ulCarriageID, UINT32 ulIPHDevID, UINT32 &ulDCUserID)
{
    for (UINT32 i=0; i<g_vIPHApplyInfo.size(); i++)
    {
        if (g_vIPHApplyInfo[i].ulTrainUnitID == ulTrainUnitID &&
            g_vIPHApplyInfo[i].ulCarriageID  == ulCarriageID  &&
            g_vIPHApplyInfo[i].ulIPHDevID    == ulIPHDevID    &&
            g_vIPHApplyInfo[i].ulStatus      == IPH_STATUS_TALK)
        {
            ulDCUserID = g_vIPHApplyInfo[i].ulAcceptDCUserID;

            return TRUE;
        }
    }

    return FALSE;
}

VOID TaskSA::OnSAUpLoadIPHAudioReq(CHAR *szMsgInfo, GJson &Json)
{
    UINT32              ulDCUserID;
    UINT32              ulTrainUnitID;
    UINT32              ulCarriageID;
    UINT32              ulIPHDevID;
    GJsonParam          Param;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("CarriageID", &ulCarriageID))
    {
        TaskLog(LOG_ERROR, "invalid CarriageID in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("IPHDevID", &ulIPHDevID))
    {
        TaskLog(LOG_ERROR, "invalid IPHDevID in msg %s", szMsgInfo);
        return;
    }

    if (!GetAccpetIPHDCUser(ulTrainUnitID, ulCarriageID, ulIPHDevID, ulDCUserID))
    {
        GosLog(LOG_ERROR, "IPH(%u %u %u ) is not accpet dc", ulTrainUnitID, ulCarriageID, ulIPHDevID);
        return ;
    }

    PID_T   stPID;

    if (!GetDCPID(ulDCUserID, &stPID))
    {
        GosLog(LOG_ERROR, "not find dc pid in OnSAUpLoadIPHAudioReq");
        return;
    }

    SendClient(&stPID, EV_SA_DIS_DC_REQ, szMsgInfo, strlen(szMsgInfo)+1);

    GosLog(LOG_DETAIL, "send iph audio to dc, msg is %s", szMsgInfo);

    return ;
}

VOID TaskSA::OnSAIPHDevResetReq(CHAR *szMsgInfo, GJson &Json)
{
    UINT32                  ulTrainUnitID;
    CHAR                    acSAID[64];
    UINT8                   ucIPHresetRet;
    GJsonParam              Param;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("SAID", acSAID, sizeof(acSAID)))
    {
        TaskLog(LOG_ERROR, "invalid Result in msg %s", szMsgInfo);
        return;
    }

    if (!Json.GetValue("IPHResetRet", &ucIPHresetRet))
    {
        TaskLog(LOG_ERROR, "invalid PARecvAudioPort in msg %s", szMsgInfo);
        return;
    }

    Param.Add("IPHResetRsp", 0);

    if (!SendTrainSA(MSG_DCIPHDevResetRsp, ulTrainUnitID, Param))
    {
        TaskLog(LOG_ERROR, "send iph reset rsp to train %u failed", ulTrainUnitID);
    }

    return ;
}

VOID TaskSA::OnDCDISSAReq(CHAR *szReq, UINT32 ulMsgLen)
{
    GJson       Json;
    GJson       SubJson;
    BOOL        bRet;
    CHAR        *szMsgType;
    CHAR        *szMsgInfo;
    UINT32      ulSeqID;

    if (!szReq || ulMsgLen == 0)
    {
        return;
    }

    bRet = Json.Parse(szReq);
    if (!bRet)
    {
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

    if (!Json.GetValue("DCUserID", &m_ulDCUserID))
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

    // 以下消息为周期性消息
    if (strcmp(szMsgType, MSG_APP_ReportTrainInfo) == 0 )
    {
        TaskLog(LOG_DETAIL, "get APP msg: %s", szReq);
    }
    else
    {
        // 其他为业务消息，单独存储
        TaskMsgLog(LOG_INFO, "get DC to SA msg: %s", szReq);
    }

    if (strcmp(szMsgType, MSG_DCAcceptIPHTalk) == 0)
    {
        OnDCAcceptIPHTalkReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCStopIPHTalk) == 0)
    {
        OnDCStopIPHTalkReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCStartRecBrd) == 0)
    {
        OnDCStartRecBrdReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCGetActiveIPHTalk) == 0)
    {
        OnDCGetActiveIPHTalkReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCGetRecBrdCfg) == 0)  // 从列车广播系统获取预录音广播配置
    {
        OnDCGetRecBrdCfgReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCGetMaxRealBrdSeqID) == 0)
    {
        OnDCGetMaxRealBrdSeqIDReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCStartRealBrd) == 0)
    {
        OnDCStartRealBrdReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCStopRealBrd) == 0)
    {
        OnDCStopRealBrdReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSendRealBrdAudioData) == 0)
    {
        OnDCSendRealBrdAudioDataReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCSendIPHAudioData) == 0)
    {
        OnDCSendIPHAudioDataReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_DCPauseIPHTalkReq) == 0)
    {
        OnDCPauseIPHTalkReq(szReq, ulSeqID, SubJson);
    }
    else
    {
        TaskLog(LOG_ERROR, "unknown DC to SA msg: %s", szReq);
        goto error;
    }

 //   TaskLog(LOG_INFO, "DC to SA msg done: %s", szMsgType);

    return;

error:
    TaskLog(LOG_ERROR, "invalid DC to SA msg: %s", szReq);

    return;
}

VOID TaskSA::OnDCGetMaxRealBrdSeqIDReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonParam          Param;
    UINT32              ulBrdSeqID = GetRealBrdSeqID(FALSE);

    Param.Add("MaxSeqID", ulBrdSeqID);
    Param.Add("Result", "true");

    SendRsp(MSG_DCGetMaxRealBrdSeqID, ulSeqID, Param);
}

VOID TaskSA::OnDCAcceptIPHTalkReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    VECTOR<GJson*>      &vSubJson = Json.GetSubJson();
    UINT32              ulTrainUnitID = 0xffffffff;
    UINT32              ulCarriageID;
    UINT32              ulIPHDevID;
    IPH_TALK_INFO_T     *pstInfo;
    BOOL                bRet = FALSE;

    if (vSubJson.size() == 0)
    {
        return;
    }

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        GJson   *SubJson = vSubJson[i];

        if (!SubJson->GetValue("TrainUnitID", &ulTrainUnitID))
        {
            TaskLog(LOG_ERROR, "get TrainUnitID of MsgInfo failed: %s", szReq);
            return;
        }

        if (!SubJson->GetValue("CarriageID", &ulCarriageID))
        {
            TaskLog(LOG_ERROR, "get CarriageID of MsgInfo failed: %s", szReq);
            return;
        }

        if (!SubJson->GetValue("IPHDevID", &ulIPHDevID))
        {
            TaskLog(LOG_ERROR, "get IPHDevID of MsgInfo failed: %s", szReq);
            return;
        }

        if (i == 0)
        {
            bRet = SendTrainSA(ulTrainUnitID, szReq);
            if (!bRet)
            {
                TaskLog(LOG_ERROR, "send DC %u AcceptIPHTalk msg to sa failed: %u %u %u", m_ulDCUserID, ulTrainUnitID, ulCarriageID, ulIPHDevID);
                break;
            }

            TaskLog(LOG_INFO, "send DC %u AcceptIPHTalk msg to sa succ: %u %u %u", m_ulDCUserID, ulTrainUnitID, ulCarriageID, ulIPHDevID);
        }

        pstInfo = AcceptIPHTalk(ulTrainUnitID, ulCarriageID, ulIPHDevID, m_ulDCUserID);
        if (pstInfo)
        {
            m_pDao->UpdateIPHTalk(pstInfo);
        }
    }

    //通知其他调度台该IPH请求已经被接听
    NotifyIPHTalkInfo();
}

VOID TaskSA::OnDCStopIPHTalkReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32          ulTrainUnitID = 0xffffffff;
    UINT32          ulCarriageID;
    UINT32          ulIPHDevID;
    VECTOR<IPH_TALK_INFO_T>  vIPHInfo;
    IPH_TALK_INFO_T stInfo;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "get TrainUnitID of MsgInfo failed: %s", szReq);
        return;
    }

    if (!Json.GetValue("CarriageID", &ulCarriageID))
    {
        TaskLog(LOG_ERROR, "get CarriageID of MsgInfo failed: %s", szReq);
        return;
    }

    if (!Json.GetValue("IPHDevID", &ulIPHDevID))
    {
        TaskLog(LOG_ERROR, "get IPHDevID of MsgInfo failed: %s", szReq);
        return;
    }

    BOOL    bRet = SendTrainSA(ulTrainUnitID, szReq);

    if (ulCarriageID == IPH_TALK_RESET &&
        ulIPHDevID   == IPH_TALK_RESET)
    {
        TaskLog(LOG_INFO, "get clean iph req of train(%u)", ulTrainUnitID);
        CleanIPHTalk(ulTrainUnitID, vIPHInfo);

        for (UINT32 i=0; i<vIPHInfo.size(); i++)
        {
            m_pDao->UpdateIPHTalk(&vIPHInfo[i]);
        }
    }
    else
    {
        if (StopIPHTalk(ulTrainUnitID, ulCarriageID, ulIPHDevID, &stInfo))
        {
            m_pDao->UpdateIPHTalk(&stInfo);
        }
    }

    NotifyIPHTalkInfo();
}

//向SA发送实时广播请求
VOID TaskSA::OnDCStartRealBrdReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJson       SubJson;
    CHAR        *szTrain;
    UINT32      ulBrdSeqID;
    UINT32      ulDCUserID = m_ulDCUserID;
    UINT32      ulTrainUnitID;
    UINT8       ucCarriageID;

    if (!Json.GetValue("BrdSeqID", &ulBrdSeqID))
    {
        TaskLog(LOG_ERROR, "invalid BrdSeqID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("CarriageID", &ucCarriageID))
    {
        TaskLog(LOG_ERROR, "get CarriageID failed on msg %s", szReq);
        ucCarriageID = PA_REAL_BRD_ALL_CARRIAGE; //TODO：缺省认为人工广播为全车广播
    }

 /*   if (!Json.GetValue("MaxPeriod", &ulMaxPeriod))
    {
        TaskLog(LOG_ERROR, "invalid MaxPeriod in msg %s", szReq);
        return;
    }*/

    szTrain = Json.GetValue("Train");
    if (!szTrain)
    {
        TaskLog(LOG_ERROR, "get Train failed: %s", szReq);
        return;
    }

    if (!SubJson.Parse(szTrain))
    {
        TaskLog(LOG_ERROR, "parse Train failed in msg: %s", szReq);
        return;
    }

    VECTOR<GJson *> &vSubJson = SubJson.GetSubJson();

    TaskLog(LOG_INFO, "start real broadcast, seq ID is : %d", ulBrdSeqID);

    BRD_MAP_INFO_T  *pstBrdInfo = BrdInfo::GetBrdInfo(TRUE, ulBrdSeqID);

    if (pstBrdInfo)
    {
        TaskLog(LOG_WARN, "real broadcast %u is exist", ulBrdSeqID);
        return;
    }

    BRD_MAP_INFO_T      stRecBrdMap = {0};

    stRecBrdMap.bIsRealBrd = TRUE;
    stRecBrdMap.ulNOCCPAIndex = 0;
    stRecBrdMap.ulBrdSeqID = ulBrdSeqID;
    stRecBrdMap.ulMsgSeqID = ulSeqID;
    stRecBrdMap.ulDCUserID = ulDCUserID;
    stRecBrdMap.ulStartTime = gos_get_uptime();

    BrdInfo::NewBrdInfo(stRecBrdMap);

    for (UINT32 i = 0; i < vSubJson.size(); i++)
    {
        if (!vSubJson[i]->GetValue("TrainUnitID", &ulTrainUnitID))
        {
            TaskLog(LOG_ERROR, "invalid TrainUnitID in msg %s", szReq);
            return;
        }

        REAL_BRD_SEND_INFO_T stSendInfo = {0};

        stSendInfo.ulBrdSeqID = ulBrdSeqID;
        stSendInfo.ulTrainUnitID = ulTrainUnitID;
        stSendInfo.ulDCUserID = ulDCUserID;
        stSendInfo.ulSendUptime = gos_get_uptime();

        NewRealBrdInfo(stSendInfo);

        GJsonParam Param;

        Param.Add("BrdSeqID", ulBrdSeqID);
        Param.Add("DCUserID", ulDCUserID);
        Param.Add("CarriageID", ucCarriageID);

        if (!SendTrainSA(MSG_DCStartRealBrd, ulTrainUnitID, Param))
        {
            TaskLog(LOG_ERROR, "send start real broadcast to train %u failed", ulTrainUnitID);
        }
    }
}

VOID TaskSA::OnDCStopRealBrdReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJson  SubJson;
    CHAR   *szTrain;
    GJsonParam Param;
    UINT32 ulBrdSeqID;
    UINT32 ulDCUserID = m_ulDCUserID;
    UINT32 ulTrainUnitID;

    if (!Json.GetValue("BrdSeqID", &ulBrdSeqID))
    {
        TaskLog(LOG_ERROR, "invalid BrdSeqID in msg %s", szReq);
        return;
    }

    szTrain = Json.GetValue("Train");
    if (!szTrain)
    {
        TaskLog(LOG_ERROR, "get Train failed: %s", szReq);
        return;
    }

    if (!SubJson.Parse(szTrain))
    {
        TaskLog(LOG_ERROR, "parse Train failed in msg: %s", szReq);
        return;
    }

    BRD_MAP_INFO_T  *pstBrdInfo = BrdInfo::GetBrdInfo(TRUE, ulBrdSeqID);

    if (!pstBrdInfo)
    {
        TaskLog(LOG_ERROR, "unknown real brd %u to be stopped", ulBrdSeqID);
        return;
    }

    VECTOR<GJson *> &vSubJson = SubJson.GetSubJson();

    Param.Add("BrdSeqID", ulBrdSeqID);
    Param.Add("DCUserID", ulDCUserID);

    for (UINT32 i = 0; i < vSubJson.size(); i++)
    {
        if (!vSubJson[i]->GetValue("TrainUnitID", &ulTrainUnitID))
        {
            TaskLog(LOG_ERROR, "invalid TrainUnitID in msg %s", szReq);
            return;
        }

        if (!SendTrainSA(MSG_DCStopRealBrd, ulTrainUnitID, Param))
        {
            TaskLog(LOG_ERROR, "send stop real broadcast to train %u failed", ulTrainUnitID);
        }
    }

    BrdInfo::DelBrdInfo(TRUE, ulBrdSeqID);
    DelRealBrdInfo(ulBrdSeqID);
}

VOID TaskSA::OnDCPauseIPHTalkReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32          ulTrainUnitID;
    UINT32          ulCarriageID;
    UINT32          ulIPHDevID;
    GJsonParam      Param;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "get TrainUnitID failed in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("CarriageID", &ulCarriageID))
    {
        TaskLog(LOG_ERROR, "get CarriageID failed in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("IPHDevID", &ulIPHDevID))
    {
        TaskLog(LOG_ERROR, "get IPHDevID failed in msg %s", szReq);
        return;
    }

    Param.Clear();
    Param.Add("TrainUnitID", ulTrainUnitID);
    Param.Add("CarriageID", ulCarriageID);
    Param.Add("IPHDevID", ulIPHDevID);

    if (!SendTrainSA(MSG_DCPauseIPHTalkReq, ulTrainUnitID, Param))
    {
        TaskLog(LOG_ERROR, "send dc pause iph to train %u failed", ulTrainUnitID);
    }

    return ;
}

VOID TaskSA::OnDCSendIPHAudioDataReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32          ulTrainUnitID;
    GJson           SubJson;
    GJsonParam      Param;
    CHAR            *szPcmData;
    UINT32          ulDataIndex;
    UINT32          ulSampleRate;
    UINT32          ulChannelNum;
    UINT32          ulSampleBits;
    UINT32          ulIPHDevID;
    UINT32          ulCarriageID;
    UINT32          ulPCMPkgTime;
    UINT32          ulDataLen;
    UINT8           aucDevIP[4];

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "get TrainUnitID failed in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("IPHDevID", &ulIPHDevID))
    {
        TaskLog(LOG_ERROR, "get IPHDevID failed in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("CarriageID", &ulCarriageID))
    {
        TaskLog(LOG_ERROR, "get CarriageID failed in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("PCMPkgTime", &ulPCMPkgTime))
    {
        TaskLog(LOG_ERROR, "get PCMPkgTime failed in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("PCMDataLen", &ulDataLen))
    {
        TaskLog(LOG_ERROR, "get PCMDataLen failed in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DataIndex", &ulDataIndex))
    {
        TaskLog(LOG_ERROR, "get DataIndex failed in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("SampleRate", &ulSampleRate))
    {
        TaskLog(LOG_ERROR, "get SampleRate failed in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("ChannelNum", &ulChannelNum))
    {
        TaskLog(LOG_ERROR, "get ChannelNum failed in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("SampleBits", &ulSampleBits))
    {
        TaskLog(LOG_ERROR, "get SampleBits failed in msg %s", szReq);
        return;
    }

    if (!Json.GetIP("DevAddr", aucDevIP))
    {
        TaskLog(LOG_ERROR, "get DevAddr failed in msg %s", szReq);
        return;
    }

    szPcmData = Json.GetValue("PCMData");
    if (!szPcmData)
    {
        TaskLog(LOG_ERROR, "no PCMData failed in msg %s", szReq);
        return;
    }

    Param.Clear();

    Param.Add("TrainUnitID", ulTrainUnitID);
    Param.Add("CarriageID",  ulCarriageID);
    Param.Add("IPHDevID",    ulIPHDevID);
    Param.Add("DataIndex",   ulDataIndex);
    Param.Add("SampleRate",  ulSampleRate);
    Param.Add("ChannelNum",  ulChannelNum);
    Param.Add("SampleBits",  ulSampleBits);
    Param.Add("PCMPkgTime",  ulPCMPkgTime);
    Param.Add("PCMDataLen",  ulDataLen);
    Param.AddIP("DevAddr",   aucDevIP);
    Param.Add("PCMData",     szPcmData);

    if (!SendTrainSA(MSG_DCSendIPHAudioData, ulTrainUnitID, Param))
    {
        TaskLog(LOG_ERROR, "send iph audio to train %u failed", ulTrainUnitID);
    }
    else
    {
        TaskLog(LOG_DETAIL, "send iph audio %u to train %u succ", ulDataIndex, ulTrainUnitID);
    }
}

VOID TaskSA::OnDCSendRealBrdAudioDataReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32          ulTrainUnitID;
    GJson           SubJson;
    GJsonParam      Param;
    VECTOR<UINT32>  vTrainUnitID;
    CHAR            *szPcmData;
    UINT32          ulBrdSeqID;
    UINT32          ulDataIndex;
    UINT32          ulSampleRate;
    UINT32          ulChannelNum;
    UINT32          ulSampleBits;

    if (!Json.GetValue("BrdSeqID", &ulBrdSeqID))
    {
        TaskLog(LOG_ERROR, "get BrdSeqID failed in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DataIndex", &ulDataIndex))
    {
        TaskLog(LOG_ERROR, "get DataIndex failed in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("SampleRate", &ulSampleRate))
    {
        TaskLog(LOG_ERROR, "get SampleRate failed in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("ChannelNum", &ulChannelNum))
    {
        TaskLog(LOG_ERROR, "get ChannelNum failed in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("SampleBits", &ulSampleBits))
    {
        TaskLog(LOG_ERROR, "get SampleBits failed in msg %s", szReq);
        return;
    }

    szPcmData = Json.GetValue("PCMData");
    if (!szPcmData)
    {
        TaskLog(LOG_ERROR, "no PCMData failed in msg %s", szReq);
        return;
    }

    BRD_MAP_INFO_T  *pstBrdInfo = BrdInfo::GetBrdInfo(TRUE, ulBrdSeqID);

    if (!pstBrdInfo)
    {
        TaskLog(LOG_ERROR, "unknown real PA %u to send audio", ulBrdSeqID);
        return;
    }

    GetRealBrdTrains(ulBrdSeqID, vTrainUnitID);

    for (UINT32 i=0; i<vTrainUnitID.size(); i++)
    {
        ulTrainUnitID = vTrainUnitID[i];

        Param.Clear();
        Param.Add("BrdSeqID", ulBrdSeqID);  // 替换成本地维护的BrdSeqID
        Param.Add("DataIndex", ulDataIndex);
        Param.Add("SampleRate", ulSampleRate);
        Param.Add("ChannelNum", ulChannelNum);
        Param.Add("SampleBits", ulSampleBits);
        Param.Add("PCMData", szPcmData);

        if (!SendTrainSA(MSG_DCSendRealBrdAudioData, ulTrainUnitID, Param))
        {
            TaskLog(LOG_ERROR, "send real broadcast audio to train %u failed", ulTrainUnitID);
        }
        else
        {
            TaskLog(LOG_DETAIL, "send real broadcast audio %u to train %u succ", ulDataIndex, ulTrainUnitID);
        }
    }
}

// NOCC BRD采用奇数，其他采用偶数
UINT32 TaskSA::GetRecBrdSeqID(BOOL bIsNOCCBrd)
{
    static UINT32   ulLastRecBrdSeqID = 0;
    UINT32          ulRecBrdSeqID;
    UINT32          ulCurrTime = gos_get_current_time();

    if (!m_pDao->GetMaxRecBrdSeqID(&ulRecBrdSeqID))
    {
        TaskLog(LOG_ERROR, "get max rec brd seq id failed");

        ulRecBrdSeqID = ulCurrTime;
    }

    // 必须大于上次值
    if (ulRecBrdSeqID <= ulLastRecBrdSeqID)
    {
        ulRecBrdSeqID = ::GetBrdSeqID(ulLastRecBrdSeqID+1, bIsNOCCBrd);
    }

    // 必须大于当前时间值
    if (ulRecBrdSeqID < ulCurrTime)
    {
        ulRecBrdSeqID = ::GetBrdSeqID(ulCurrTime, bIsNOCCBrd);
    }

    ulLastRecBrdSeqID = ulRecBrdSeqID;

    return ulLastRecBrdSeqID;
}

// NOCC BRD采用奇数，其他采用偶数
UINT32 TaskSA::GetRealBrdSeqID(BOOL bIsNOCCBrd)
{
    static UINT32   ulLastRealBrdSeqID = 0;
    UINT32          ulRealBrdSeqID;
    UINT32          ulCurrTime = gos_get_current_time();

    if (!m_pDao->GetMaxRealBrdSeqID(&ulRealBrdSeqID))
    {
        TaskLog(LOG_ERROR, "get max real brd seq id failed");

        ulRealBrdSeqID = ulCurrTime;
    }

    // 必须大于上次值
    if (ulRealBrdSeqID <= ulLastRealBrdSeqID)
    {
        ulRealBrdSeqID = ::GetBrdSeqID(ulLastRealBrdSeqID+1, bIsNOCCBrd);
    }

    // 必须大于当前时间值
    if (ulRealBrdSeqID < ulCurrTime)
    {
        ulRealBrdSeqID = ::GetBrdSeqID(ulCurrTime, bIsNOCCBrd);
    }

    ulLastRealBrdSeqID = ulRealBrdSeqID;

    return ulLastRealBrdSeqID;
}

VOID TaskSA::OnDCStartRecBrdReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    REC_BRD_INFO_T  stInfo = {0};
    UINT32          ulTrainUnitID;
    GJson           SubJson;
    CHAR            *szTrain;
    GJsonParam      Param;
    VECTOR<UINT32>  vTrainUnitID;
    UINT32          ulRecBrdSeqID = 0;

    ulRecBrdSeqID = GetRecBrdSeqID(FALSE);

    stInfo.ulBrdSeqID = ulRecBrdSeqID;
    stInfo.ulTime = gos_get_current_time();
    stInfo.ulDCUserID = m_ulDCUserID;

    if (!Json.GetValue("RecID", &stInfo.ulRecID))
    {
        TaskLog(LOG_ERROR, "invalid RecID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("BrdTime", &stInfo.ulBrdLoopTime))
    {
        TaskLog(LOG_ERROR, "invalid BrdTime in msg %s", szReq);
        return;
    }

    szTrain = Json.GetValue("Train");
    if (!szTrain)
    {
        TaskLog(LOG_ERROR, "get Train failed: %s", szReq);
        return;
    }

    if (!SubJson.Parse(szTrain))
    {
        TaskLog(LOG_ERROR, "parse Train failed in msg: %s", szReq);
        return;
    }

    UINT32              ulLen = 0;
    REC_BRD_SEND_INFO_T stSendInfo = {0};
    VECTOR<GJson*>      &vSubJson = SubJson.GetSubJson();
    REC_BRD_CFG_T       stRecBrdCfg;

    if (!GetRecBrdCfg(stInfo.ulRecID, stRecBrdCfg))
    {
        TaskLog(LOG_ERROR, "invalid rec id(%u) in msg: %s", stInfo.ulRecID);
        return;
    }

    BRD_MAP_INFO_T      stRecBrdMap = {0};

    stRecBrdMap.bIsRealBrd = FALSE;
    stRecBrdMap.ulNOCCPAIndex = 0;
    stRecBrdMap.ulBrdSeqID = stInfo.ulBrdSeqID;
    stRecBrdMap.ulMsgSeqID = ulSeqID;
    stRecBrdMap.ulDCUserID = stInfo.ulDCUserID;
    stRecBrdMap.ulStartTime = gos_get_uptime();

    BrdInfo::NewBrdInfo(stRecBrdMap);

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        if (!vSubJson[i]->GetValue("TrainUnitID", &ulTrainUnitID))
        {
            TaskLog(LOG_ERROR, "invalid TrainUnitID in msg %s", szReq);
            return;
        }

        Param.Clear();
        Param.Add("RecID", stInfo.ulRecID);
        Param.Add("BrdSeqID", stInfo.ulBrdSeqID);
        Param.Add("BrdTime", stInfo.ulBrdLoopTime);

        vTrainUnitID.push_back(ulTrainUnitID);

        stSendInfo.ulRecID = stInfo.ulRecID;
        stSendInfo.ulBrdSeqID = stInfo.ulBrdSeqID;
        stSendInfo.ulBrdLoopTime = stInfo.ulBrdLoopTime;
        stSendInfo.ulTrainUnitID = ulTrainUnitID;
        stSendInfo.ulSendUptime = gos_get_uptime();

        NewRecBrdInfo(stSendInfo);

        if (!SendTrainSA(MSG_DCStartRecBrd, ulTrainUnitID, Param))
        {
            TaskLog(LOG_ERROR, "send rec broadcast to train %u failed", ulTrainUnitID);
        }

        if ((ulLen + 14) < sizeof(stInfo.acTrainList))
        {
            ulLen += sprintf(stInfo.acTrainList+ulLen, "%s%u", i==0?"":",", ulTrainUnitID);
        }
    }

    strcpy(stInfo.acRecInfo, stRecBrdCfg.acRecInfo);
    m_pDao->AddRecBrdInfo(&stInfo, vTrainUnitID);
}

VOID TaskSA::NotifyIPHTalkInfo()
{
    GJsonTupleParam         JsonTupleParam;

    ConvertIPHTalkToJson(g_vIPHApplyInfo, TRUE, JsonTupleParam);

    SendAllClient(DIS_CLIENT_DC, EV_DC_DIS_SA_RSP, MSG_DCGetActiveIPHTalk, 0, JsonTupleParam);
}

VOID TaskSA::OnDCGetActiveIPHTalkReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonTupleParam         JsonTupleParam;

    ConvertIPHTalkToJson(g_vIPHApplyInfo, TRUE, JsonTupleParam);

    if (g_vIPHApplyInfo.size() > 0)
    {
        TaskMsgLog(LOG_INFO, "send DC rsp msg: %s", (CHAR*)JsonTupleParam);
    }

    SendRsp(MSG_DCGetActiveIPHTalk, ulSeqID, JsonTupleParam);
}

VOID TaskSA::OnDCGetRecBrdCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonParam      Param;
    UINT32          ulStart = gos_get_rand();
    UINT32          ulSendCount = 0;
    VECTOR<TRAIN_INFO_T> vTrainInfo;

    GetTrainInfo(vTrainInfo);
    for (UINT32 i=0; i<vTrainInfo.size(); i++)
    {
        UINT32  ulIndex = ulStart + i;

        ulIndex = MOD(ulIndex, vTrainInfo.size());
        TRAIN_INFO_T    &stTrain = vTrainInfo[ulIndex];

        if ((stTrain.ulAppReportTime + 60) > gos_get_uptime())
        {
            if (!SendTrainSA(MSG_SAGetRecBrdCfg, stTrain.ulTrainUnitID, Param))
            {
                TaskLog(LOG_ERROR, "send get rec broadcast cfg to train %u failed", stTrain.ulTrainUnitID);
                ulSendCount ++;

                if (ulSendCount >= 5)
                {
                    return;
                }
            }
            else
            {
                return;
            }
        }
    }
}

VOID TaskSA::OnCheckTimer()
{
    if (CheckIPHTalkInfo())
    {
        NotifyIPHTalkInfo();
    }

    CheckRecBrdInfo();

    BrdInfo::Check();
}

VOID TaskSA::CheckRecBrdInfo()
{
    UINT32          ulCurrTime = gos_get_uptime();
    UINT32          ulSendTimeout = 30;

    for (int i=g_vRecBrdSendInfo.size()-1; i>=0; i--)
    {
        REC_BRD_SEND_INFO_T     &stInfo = g_vRecBrdSendInfo[i];

        if ((stInfo.ulSendUptime + ulSendTimeout) < ulCurrTime)
        {
            TaskLog(LOG_ERROR, "cancel sending rec broadcast to train %u", stInfo.ulTrainUnitID);
            g_vRecBrdSendInfo.erase(g_vRecBrdSendInfo.begin()+i);
            continue;
        }

        GJsonParam      Param;

        Param.Add("RecID", stInfo.ulRecID);
        Param.Add("BrdSeqID", stInfo.ulBrdSeqID);
        Param.Add("BrdTime", stInfo.ulBrdLoopTime);

        if (!SendTrainSA(MSG_DCStartRecBrd, stInfo.ulTrainUnitID, Param))
        {
            TaskLog(LOG_ERROR, "send rec broadcast to train %u failed", stInfo.ulTrainUnitID);
        }
    }
}

VOID TaskSA::CheckRealBrdInfo()
{
    UINT32          ulCurrTime = gos_get_uptime();
    UINT32          ulSendTimeout = 30;

    for (int i=g_vRealBrdSendInfo.size()-1; i>=0; i--)
    {
        REAL_BRD_SEND_INFO_T     &stInfo = g_vRealBrdSendInfo[i];

        if ((stInfo.ulSendUptime + ulSendTimeout) < ulCurrTime)
        {
            TaskLog(LOG_ERROR, "cancel sending real broadcast to train %u", stInfo.ulTrainUnitID);
            g_vRealBrdSendInfo.erase(g_vRealBrdSendInfo.begin()+i);
            continue;
        }

        GJsonParam      Param;

        Param.Add("BrdSeqID", stInfo.ulBrdSeqID);
        Param.Add("DCUserID", stInfo.ulDCUserID);

        if (!SendTrainSA(MSG_DCStartRealBrd, stInfo.ulTrainUnitID, Param))
        {
            TaskLog(LOG_ERROR, "send real broadcast to train %u failed", stInfo.ulTrainUnitID);
        }
    }
}

VOID TaskSA::OnNOCCReq(CHAR *szReq, UINT32 ulMsgLen)
{
    GJson       Json;
    GJson       SubJson;
    BOOL        bRet;
    CHAR        *szMsgType;
    CHAR        *szMsgInfo;
    UINT32      ulSeqID;

    if (!szReq || ulMsgLen == 0)
    {
        return;
    }

    bRet = Json.Parse(szReq);
    if (!bRet)
    {
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

    // 以下消息为周期性消息
    if (strcmp(szMsgType, MSG_OCC_HeartBeatRsp) == 0 ||
        strcmp(szMsgType, MSG_OCC_PushTrainRealPAAudioReq) == 0)
    {
        //TaskLog(LOG_DETAIL, "get NOCC msg: %s", szReq);
    }
    else
    {
        // 其他为业务消息，单独存储
        TaskMsgLog(LOG_INFO, "get NOCC msg: %s", szReq);
    }

    if (strcmp(szMsgType, MSG_OCC_StartTrainRecPAReq) == 0)
    {
        OnNOCCStartTrainRecPAReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_OCC_StartTrainRealPAReq) == 0)
    {
        OnNOCCStartTrainRealPAReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_OCC_StopTrainRealPAReq) == 0)
    {
        OnNOCCStopTrainRealPAReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_OCC_PushTrainRealPAAudioReq) == 0)
    {
        OnNOCCPushTrainRealPAAudioReq(szReq, ulSeqID, SubJson);
    }
    else
    {
        TaskLog(LOG_ERROR, "unknown NOCC msg: %s", szReq);
        goto error;
    }

    return;

error:
    TaskLog(LOG_ERROR, "invalid NOCC msg: %s", szReq);

    return;
}

/*
VOID TaskSA::OnNOCCGetPARecCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonParam      Param;
    UINT32          ulTrainUnitID;

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "invalid TrainUnitID in msg %s", szReq);
        return;
    }

    if (!SendTrainSA(MSG_DCGetRecBrdCfg, ulTrainUnitID, Param))
    {
        TaskMsgLog(LOG_ERROR, "send get rec broadcast cfg to train %u failed", ulTrainUnitID);
        return;
    }

    TaskMsgLog(LOG_INFO, "send get rec broadcast cfg to train %u succ", ulTrainUnitID);
} */

VOID TaskSA::OnNOCCStartTrainRecPAReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    REC_BRD_INFO_T  stInfo = {0};
    UINT32          ulTrainUnitID;
    GJson           SubJson;
    CHAR            *szTrain;
    GJsonParam      Param;
    VECTOR<UINT32>  vTrainUnitID;
    UINT32          ulPAType;
    UINT32          ulPAIndex;
    UINT32          ulRecBrdSeqID = 0;

    ulRecBrdSeqID = GetRecBrdSeqID(TRUE);
    stInfo.ulBrdSeqID = ulRecBrdSeqID;
    stInfo.ulTime = gos_get_current_time();
    stInfo.ulDCUserID = INVALID_USER_ID;

    if (!Json.GetValue("PAIndex", &ulPAIndex))
    {
        TaskLog(LOG_ERROR, "invalid PAIndex in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("PARecID", &stInfo.ulRecID))
    {
        TaskLog(LOG_ERROR, "invalid PARecID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("PALoopNum", &stInfo.ulBrdLoopTime))
    {
        TaskLog(LOG_ERROR, "invalid PALoopNum in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("PAType", &ulPAType))
    {
        TaskLog(LOG_ERROR, "invalid PAType in msg %s", szReq);
        return;
    }

    if (ulPAType == OCC_PA_TYPE_MAINLINE )
    {
        GetMainLineTrainUnitID(vTrainUnitID);
    }
    else if (ulPAType == OCC_PA_TYPE_UPLINE )
    {
        GetUpLineTrainUnitID(vTrainUnitID);
    }
    else if (ulPAType == OCC_PA_TYPE_DOWNLINE )
    {
        GetDownLineTrainUnitID(vTrainUnitID);
    }
    else if (ulPAType == OCC_PA_TYPE_NORMAL )
    {
        szTrain = Json.GetValue("Trains");
        if (!szTrain)
        {
            TaskLog(LOG_ERROR, "get Trains failed: %s", szReq);
            return;
        }

        if (!SubJson.Parse(szTrain))
        {
            TaskLog(LOG_ERROR, "parse Trains failed in msg: %s", szReq);
            return;
        }

        VECTOR<GJson*>      &vSubJson = SubJson.GetSubJson();

        for (UINT32 i=0; i<vSubJson.size(); i++)
        {
            if (!vSubJson[i]->GetValue("TrainID", &ulTrainUnitID))
            {
                TaskLog(LOG_ERROR, "invalid TrainID in msg %s", szReq);
                return;
            }

            vTrainUnitID.push_back(ulTrainUnitID);
        }
    }
    else
    {
        TaskLog(LOG_ERROR, "unknown PAType in msg %s", szReq);
        return;
    }

    // 建立NOCC的PAIndex与OCC的RecBrdSeqID之间的对应关系
    BRD_MAP_INFO_T  *pstBrdInfo = BrdInfo::GetBrdInfoByPAIndex(FALSE, ulPAIndex);

    if (pstBrdInfo)
    {
        TaskLog(LOG_WARN, "rec PA %u is exist", ulPAIndex);
        return;
    }

    BRD_MAP_INFO_T      stRecBrdMap = {0};

    stRecBrdMap.bIsRealBrd = FALSE;
    stRecBrdMap.ulNOCCPAIndex = ulPAIndex;
    stRecBrdMap.ulBrdSeqID = stInfo.ulBrdSeqID;
    stRecBrdMap.ulMsgSeqID = ulSeqID;
    stRecBrdMap.ulDCUserID = INVALID_USER_ID;
    stRecBrdMap.ulStartTime = gos_get_uptime();

    BrdInfo::NewBrdInfo(stRecBrdMap);

    UINT32              ulLen = 0;
    REC_BRD_SEND_INFO_T stSendInfo = {0};
    REC_BRD_CFG_T       stRecBrdCfg;

    if (!GetRecBrdCfg(stInfo.ulRecID, stRecBrdCfg))
    {
        TaskLog(LOG_ERROR, "invalid rec id(%u) in msg: %s", stInfo.ulRecID);
        return;
    }

    for (UINT32 i=0; i<vTrainUnitID.size(); i++)
    {
        ulTrainUnitID = vTrainUnitID[i];

        Param.Clear();
        Param.Add("RecID", stInfo.ulRecID);
        Param.Add("BrdSeqID", stInfo.ulBrdSeqID);//stInfo.ulTime);
        Param.Add("BrdTime", stInfo.ulBrdLoopTime);

        stSendInfo.ulRecID = stInfo.ulRecID;
        stSendInfo.ulBrdSeqID = stInfo.ulBrdSeqID;
        stSendInfo.ulBrdLoopTime = stInfo.ulBrdLoopTime;
        stSendInfo.ulTrainUnitID = ulTrainUnitID;
        stSendInfo.ulSendUptime = gos_get_uptime();

        NewRecBrdInfo(stSendInfo);

        if (!SendTrainSA(MSG_DCStartRecBrd, ulTrainUnitID, Param))
        {
            TaskLog(LOG_ERROR, "send rec broadcast to train %u failed", ulTrainUnitID);
        }

        if ((ulLen + 14) < sizeof(stInfo.acTrainList))
        {
            ulLen += sprintf(stInfo.acTrainList+ulLen, "%s%u", i==0?"":",", ulTrainUnitID);
        }
    }

    strcpy(stInfo.acRecInfo, stRecBrdCfg.acRecInfo);
    m_pDao->AddRecBrdInfo(&stInfo, vTrainUnitID);
}

VOID TaskSA::OnNOCCStartTrainRealPAReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    REAL_BRD_INFO_T stInfo = {0};
    UINT32          ulTrainUnitID;
    GJson           SubJson;
    CHAR            *szTrain;
    GJsonParam      Param;
    VECTOR<UINT32>  vTrainUnitID;
    UINT32          ulPAType;
    UINT32          ulPAIndex;

    stInfo.ulBrdSeqID = GetRealBrdSeqID(TRUE);
    stInfo.ulStartTime = gos_get_current_time();
    stInfo.ulDCUserID = INVALID_USER_ID;

    if (!Json.GetValue("PAIndex", &ulPAIndex))
    {
        TaskLog(LOG_ERROR, "invalid PAIndex in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("PAType", &ulPAType))
    {
        TaskLog(LOG_ERROR, "invalid PAType in msg %s", szReq);
        return;
    }

    if (ulPAType == OCC_PA_TYPE_MAINLINE )
    {
        GetMainLineTrainUnitID(vTrainUnitID);
    }
    else if (ulPAType == OCC_PA_TYPE_UPLINE )
    {
        GetUpLineTrainUnitID(vTrainUnitID);
    }
    else if (ulPAType == OCC_PA_TYPE_DOWNLINE )
    {
        GetDownLineTrainUnitID(vTrainUnitID);
    }
    else if (ulPAType == OCC_PA_TYPE_NORMAL )
    {
        szTrain = Json.GetValue("Trains");
        if (!szTrain)
        {
            TaskLog(LOG_ERROR, "get Trains failed: %s", szReq);
            return;
        }

        if (!SubJson.Parse(szTrain))
        {
            TaskLog(LOG_ERROR, "parse Trains failed in msg: %s", szReq);
            return;
        }

        VECTOR<GJson*>      &vSubJson = SubJson.GetSubJson();

        for (UINT32 i=0; i<vSubJson.size(); i++)
        {
            if (!vSubJson[i]->GetValue("TrainID", &ulTrainUnitID))
            {
                TaskLog(LOG_ERROR, "invalid TrainID in msg %s", szReq);
                return;
            }

            vTrainUnitID.push_back(ulTrainUnitID);
        }
    }
    else
    {
        TaskLog(LOG_ERROR, "unknown PAType in msg %s", szReq);
        return;
    }

    // 建立NOCC的PAIndex与OCC的BrdSeqID之间的对应关系
    BRD_MAP_INFO_T  *pstBrdInfo = BrdInfo::GetBrdInfoByPAIndex(TRUE, ulPAIndex);

    if (pstBrdInfo)
    {
        TaskLog(LOG_WARN, "real PA %u is exist", ulPAIndex);
        return;
    }

    BRD_MAP_INFO_T      stRecBrdMap = {0};

    stRecBrdMap.bIsRealBrd = TRUE;
    stRecBrdMap.ulNOCCPAIndex = ulPAIndex;
    stRecBrdMap.ulBrdSeqID = stInfo.ulBrdSeqID;
    stRecBrdMap.ulMsgSeqID = ulSeqID;
    stRecBrdMap.ulDCUserID = INVALID_USER_ID;
    stRecBrdMap.ulStartTime = gos_get_uptime();

    BrdInfo::NewBrdInfo(stRecBrdMap);

    UINT32              ulLen = 0;
    REAL_BRD_SEND_INFO_T stSendInfo = {0};

    for (UINT32 i=0; i<vTrainUnitID.size(); i++)
    {
        ulTrainUnitID = vTrainUnitID[i];

        Param.Clear();
        Param.Add("BrdSeqID", stInfo.ulBrdSeqID);
        Param.Add("DCUserID", stRecBrdMap.ulDCUserID);

        stSendInfo.ulBrdSeqID = stInfo.ulBrdSeqID;
        stSendInfo.ulDCUserID = stRecBrdMap.ulDCUserID;
        stSendInfo.ulTrainUnitID = ulTrainUnitID;
        stSendInfo.ulSendUptime = gos_get_uptime();

        NewRealBrdInfo(stSendInfo);

        if (!SendTrainSA(MSG_DCStartRealBrd, ulTrainUnitID, Param))
        {
            TaskLog(LOG_ERROR, "send start real broadcast to train %u failed", ulTrainUnitID);
        }

        if ((ulLen + 14) < sizeof(stInfo.acTrainList))
        {
            ulLen += sprintf(stInfo.acTrainList+ulLen, "%s%u", i==0?"":",", ulTrainUnitID);
        }
    }

    //m_pDao->AddRealBrdInfo(&stInfo, vTrainUnitID);
}

VOID TaskSA::OnNOCCStopTrainRealPAReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32          ulTrainUnitID;
    GJson           SubJson;
    GJsonParam      Param;
    VECTOR<UINT32>  vTrainUnitID;
    UINT32          ulPAIndex;

    if (!Json.GetValue("PAIndex", &ulPAIndex))
    {
        TaskLog(LOG_ERROR, "invalid PAIndex in msg %s", szReq);
        return;
    }

    // 建立NOCC的PAIndex与OCC的BrdSeqID之间的对应关系
    BRD_MAP_INFO_T  *pstBrdInfo = BrdInfo::GetBrdInfoByPAIndex(TRUE, ulPAIndex);

    if (!pstBrdInfo)
    {
        TaskLog(LOG_ERROR, "unknown PA %u to be stopped", ulPAIndex);
        return;
    }

    UINT32  ulBrdSeqID = pstBrdInfo->ulBrdSeqID;

    GetRealBrdTrains(ulBrdSeqID, vTrainUnitID);

    for (UINT32 i=0; i<vTrainUnitID.size(); i++)
    {
        ulTrainUnitID = vTrainUnitID[i];

        Param.Clear();
        Param.Add("BrdSeqID", ulBrdSeqID);

        if (!SendTrainSA(MSG_DCStopRealBrd, ulTrainUnitID, Param))
        {
            TaskLog(LOG_ERROR, "send stop real broadcast to train %u failed", ulTrainUnitID);
        }
    }

    BrdInfo::DelBrdInfo(TRUE, ulBrdSeqID);
    DelRealBrdInfo(ulBrdSeqID);

    //m_pDao->AddRealBrdInfo(&stInfo, vTrainUnitID);
}

VOID TaskSA::OnNOCCPushTrainRealPAAudioReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    UINT32          ulTrainUnitID;
    GJson           SubJson;
    GJsonParam      Param;
    VECTOR<UINT32>  vTrainUnitID;
    CHAR            *szPcmData;
    UINT32          ulPAIndex;
    UINT32          ulDataIndex;
    UINT32          ulSampleRate;
    UINT32          ulChannelNum;
    UINT32          ulSampleBits;

    if (!Json.GetValue("PAIndex", &ulPAIndex))
    {
        TaskLog(LOG_ERROR, "get PAIndex failed in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("DataIndex", &ulDataIndex))
    {
        TaskLog(LOG_ERROR, "get DataIndex failed in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("SampleRate", &ulSampleRate))
    {
        TaskLog(LOG_ERROR, "get SampleRate failed in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("ChannelNum", &ulChannelNum))
    {
        TaskLog(LOG_ERROR, "get ChannelNum failed in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("SampleBits", &ulSampleBits))
    {
        TaskLog(LOG_ERROR, "get SampleBits failed in msg %s", szReq);
        return;
    }

    szPcmData = Json.GetValue("PCMData");
    if (!szPcmData)
    {
        TaskLog(LOG_ERROR, "no PCMData failed in msg %s", szReq);
        return;
    }

    BRD_MAP_INFO_T  *pstBrdInfo = BrdInfo::GetBrdInfoByPAIndex(TRUE, ulPAIndex);

    if (!pstBrdInfo)
    {
        TaskLog(LOG_ERROR, "unknown real PA %u to send audio", ulPAIndex);
        return;
    }

    GetRealBrdTrains(pstBrdInfo->ulBrdSeqID, vTrainUnitID);

    for (UINT32 i=0; i<vTrainUnitID.size(); i++)
    {
        ulTrainUnitID = vTrainUnitID[i];

        Param.Clear();
        Param.Add("BrdSeqID", pstBrdInfo->ulBrdSeqID);  // 替换成本地维护的BrdSeqID
        Param.Add("DataIndex", ulDataIndex);
        Param.Add("SampleRate", ulSampleRate);
        Param.Add("ChannelNum", ulChannelNum);
        Param.Add("SampleBits", ulSampleBits);
        Param.Add("PCMData", szPcmData);

        if (!SendTrainSA(MSG_DCSendRealBrdAudioData, ulTrainUnitID, Param))
        {
            TaskLog(LOG_ERROR, "send real broadcast audio to train %u failed", ulTrainUnitID);
        }
        else
        {
            TaskLog(LOG_DETAIL, "send real broadcast audio %u to train %u succ", ulDataIndex, ulTrainUnitID);
        }
    }
}

VOID TaskSA::TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
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
                case EV_TASK_CHECK_REQ:
                    SendRsp();
                    break;

                case EV_CHECK_TIMER:
                    OnCheckTimer();
                    break;

                case EV_SA_DIS_REQ:
                    OnSAReq((CHAR*)pvMsg, ulMsgLen);
                    break;

                case EV_DC_DIS_SA_REQ:
                    OnDCDISSAReq((CHAR*)pvMsg, ulMsgLen);
                    break;

                case EV_NOCC_REQ:
                    OnNOCCReq((CHAR*)pvMsg, ulMsgLen);
                    break;

                default:
                    TaskLog(LOG_ERROR, "unknown msg %u", usMsgID);
                    break;
            }

            break;

        default:
            break;
    }
}
