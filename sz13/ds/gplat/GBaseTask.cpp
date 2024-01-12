#include "GPublic.h"
#include "DTPClient.h"

extern DTPClient* GosGetDTPClient(PEER_ADDR_T *pstServerAddr);

VOID GBaseTask::SetTaskName(const CHAR *szTaskName)
{
    if (IS_NULL_STRING(szTaskName))
    {
        szTaskName = (CHAR*)"UNKNOWN";
    }

    if (m_szTaskName)
    {
        free(m_szTaskName);
    }
    m_szTaskName = strdup(szTaskName);

    Logger  *pLogger = GLogFactory::GetInstance()->GetDefaultLogger();
    pLogger->SetLogModule(m_usTaskID, m_szTaskName);
}

GBaseTask::GBaseTask(UINT16 usTaskID, const CHAR *szTaskName, UINT16 usDispatchID, BOOL bCheckTask)
{
    if (usTaskID == INVALID_TASK_ID || usTaskID == LOCAL_INST_ID)
    {
        GosLog(LOG_FATAL, "Task id %u is invalid", usTaskID);
        exit(-1);
    }

    m_szTaskName = NULL;
    m_usTaskID = usTaskID;
    m_usDispatchID = usDispatchID;
    m_ulTaskStatus = TASK_STATUS_IDLE;

    m_ulBufSize = 0;
    m_pvBuf = NULL;
    m_ulLocalMsgSeqID = 0;
    m_ulMsgStartTime = 0;
    m_ulMsgEndTime = 0;
    m_bCheckTask = bCheckTask;

    m_usMsgID = 0;
    m_pvMsgData = NULL;
    m_ulMsgLen = 0;

    SetTaskName(szTaskName);
}

VOID* GBaseTask::AllocBuffer(UINT32 ulBufSize)
{
    if (m_ulBufSize >= ulBufSize)
    {
        memset(m_pvBuf, 0, ulBufSize);
        return m_pvBuf;
    }

    FreeBuffer();

    m_pvBuf = calloc(1, ulBufSize);
    if (!m_pvBuf)
    {
        return NULL;
    }

    m_ulBufSize = ulBufSize;

    return m_pvBuf;
}

VOID GBaseTask::FreeBuffer()
{
    if (m_pvBuf)
    {
        free(m_pvBuf);
        m_pvBuf = NULL;
    }

    m_ulBufSize = 0;
}

VOID GBaseTask::RegisterMsg(UINT16 usMsgID, BOOL bUniqueMsg, GOS_UNIQUE_MSG_CMP_FUNC *pfUniqueMsgCmp)
{
    TASK_MSG_DESC_T     stMsgDesc = {0};

    stMsgDesc.usMsgID = usMsgID;
    stMsgDesc.ucUniqueMsg = bUniqueMsg;
    stMsgDesc.pfUniqueMsgCmp = pfUniqueMsgCmp;

    m_vMsgList.push_back(stMsgDesc);
}

std::vector<TASK_MSG_DESC_T>& GBaseTask::GetMsgList()
{
    return m_vMsgList;
}

BOOL GBaseTask::SetTimer(UINT16 usTimerID, UINT32 ulTimeout, UINT32 ulPara)
{
    UINT16      usTaskID = GetTaskID();
    GTimer      *pTimer;

    if (usTaskID == INVALID_TASK_ID)
    {
        return FALSE;
    }

    pTimer = GTimer::GetInstance();
    if (!pTimer)
    {
        return FALSE;
    }

    return pTimer->SetTimer(usTaskID, usTimerID, ulTimeout, ulPara);
}

BOOL GBaseTask::SetTimer(UINT16 usTimerID, UINT32 ulTimeout, VOID *pvPara)
{
    UINT16      usTaskID = GetTaskID();
    GTimer      *pTimer;

    if (usTaskID == INVALID_TASK_ID)
    {
        return FALSE;
    }

    pTimer = GTimer::GetInstance();
    if (!pTimer)
    {
        return FALSE;
    }

    return pTimer->SetTimer(usTaskID, usTimerID, ulTimeout, pvPara);
}

BOOL GBaseTask::SetLoopTimer(UINT16 usTimerID, UINT32 ulTimeout, UINT32 ulPara)
{
    UINT16      usTaskID = GetTaskID();
    GTimer      *pTimer;

    if (usTaskID == INVALID_TASK_ID)
    {
        return FALSE;
    }

    pTimer = GTimer::GetInstance();
    if (!pTimer)
    {
        return FALSE;
    }

    return pTimer->SetLoopTimer(usTaskID, usTimerID, ulTimeout, ulPara);
}

BOOL GBaseTask::SetLoopTimer(UINT16 usTimerID, UINT32 ulTimeout, VOID *pvPara)
{
    UINT16      usTaskID = GetTaskID();
    GTimer      *pTimer;

    if (usTaskID == INVALID_TASK_ID)
    {
        return FALSE;
    }

    pTimer = GTimer::GetInstance();
    if (!pTimer)
    {
        return FALSE;
    }

    return pTimer->SetLoopTimer(usTaskID, usTimerID, ulTimeout, pvPara);
}

BOOL GBaseTask::ClearTimer(UINT16 usTimerID)
{
    UINT16      usTaskID = GetTaskID();
    GTimer      *pTimer;

    if (usTaskID == INVALID_TASK_ID)
    {
        return FALSE;
    }

    pTimer = GTimer::GetInstance();
    if (!pTimer)
    {
        return FALSE;
    }

    return pTimer->ClearTimer(usTaskID, usTimerID);
}

VOID GBaseTask::GetSelfPID(PID_T *pstPID)
{
    pstPID->usInstID = GosGetInstID();
    pstPID->usTaskID = GetTaskID();
}

BOOL GBaseTask::SendServer(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    return SendServer(INVALID_TASK_ID, usMsgID, pvMsg, ulMsgLen);
}

BOOL GBaseTask::SendServer(UINT16 usServerTaskID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    return GosSendServer(m_usTaskID, usServerTaskID, usMsgID, pvMsg, ulMsgLen);
}

BOOL GBaseTask::SendServer(PID_T *pstServerPID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    return GosSendServer(m_usTaskID, pstServerPID, usMsgID, pvMsg, ulMsgLen);
}

BOOL GBaseTask::SendServer(PEER_ADDR_T *pstServerAddr, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    DTPClient   *pDTPClient = GosGetDTPClient(pstServerAddr);

    if (!pDTPClient)
    {
        TaskLog(LOG_ERROR, "send msg(%u) to server failed, invalid server addr:" IP_FMT ":%u(%s)",
            usMsgID, IP_ARG(pstServerAddr->aucIP), pstServerAddr->usPort, pstServerAddr->ucTCPMode?"tcp":"udp");
        return FALSE;
    }

    if (!pDTPClient->IsConnected())
    {
        TaskLog(LOG_ERROR, "send msg(%u) to server failed, server is disconnected:" IP_FMT ":%u(%s)",
            usMsgID, IP_ARG(pstServerAddr->aucIP), pstServerAddr->usPort, pstServerAddr->ucTCPMode?"tcp":"udp");
        return FALSE;
    }

    if (!pDTPClient->SendReq(m_usTaskID, INVALID_TASK_ID, usMsgID, pvMsg, ulMsgLen))
    {
        TaskLog(LOG_ERROR, "send msg(%u) to server failed, server addr:" IP_FMT ":%u(%s)",
            usMsgID, IP_ARG(pstServerAddr->aucIP), pstServerAddr->usPort, pstServerAddr->ucTCPMode?"tcp":"udp");

        return FALSE;
    }

    return TRUE;
}

BOOL GBaseTask::SendServer(PEER_ADDR_T *pstServerAddr, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulSeqID)
{
    DTPClient   *pDTPClient = GosGetDTPClient(pstServerAddr);

    if (!pDTPClient)
    {
        TaskLog(LOG_ERROR, "send msg(%u) to server failed, invalid server addr:" IP_FMT ":%u(%s)",
            usMsgID, IP_ARG(pstServerAddr->aucIP), pstServerAddr->usPort, pstServerAddr->ucTCPMode?"tcp":"udp");

        return FALSE;
    }

    if (!pDTPClient->IsConnected())
    {
        TaskLog(LOG_ERROR, "send msg(%u) to server failed, server is disconnected:" IP_FMT ":%u(%s)",
            usMsgID, IP_ARG(pstServerAddr->aucIP), pstServerAddr->usPort, pstServerAddr->ucTCPMode?"tcp":"udp");

        return FALSE;
    }

    if (!pDTPClient->Send(m_usTaskID, INVALID_TASK_ID, usMsgID, ulSeqID, pvMsg, ulMsgLen))
    {
        TaskLog(LOG_ERROR, "send msg(%u) to server failed, server addr:" IP_FMT ":%u(%s)",
            usMsgID, IP_ARG(pstServerAddr->aucIP), pstServerAddr->usPort, pstServerAddr->ucTCPMode?"tcp":"udp");

        return FALSE;
    }

    return TRUE;
}

BOOL GBaseTask::SendLocal(UINT16 usReceiverTaskID, UINT16 usMsgID, CHAR *szMsg)
{
    return SendLocal(usReceiverTaskID, usMsgID, (VOID*)szMsg, szMsg?strlen(szMsg)+1:0);
}

BOOL GBaseTask::SendLocal(UINT16 usReceiverTaskID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    PID_T   stReceiver = {0};

    stReceiver.usInstID = LOCAL_INST_ID;
    stReceiver.usTaskID = usReceiverTaskID;

    return GosSendLocal(&stReceiver, usMsgID, pvMsg, ulMsgLen, GetReqMsgSeqID());
}

UINT32 GBaseTask::GetReqMsgSeqID()
{
    return m_ulLocalMsgSeqID++;
}

BOOL GBaseTask::SendRsp()
{
    return SendRsp(NULL, 0);
}

BOOL GBaseTask::SendRsp(CHAR *szMsg)
{
    if (!szMsg)
    {
        return FALSE;
    }

    return SendRsp(szMsg, strlen(szMsg)+1);
}

BOOL GBaseTask::SendRsp(VOID *pvMsg, UINT32 ulMsgLen)
{
    BOOL    bRet = FALSE;

    if (LOCAL_INST_ID == m_stSender.usInstID)
    {
        bRet= SendLocal(m_stSender.usTaskID, EV_RSP(m_usMsgID), pvMsg, ulMsgLen);
    }
    else
    {
        bRet = GosSendRsp(m_usTaskID, &m_stSender, m_ulMsgSeqID, EV_RSP(m_usMsgID), pvMsg, ulMsgLen);
    }

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "send rsp failed, peer inst is %u", m_stSender.usInstID);
    }

    return bRet;
}

BOOL GBaseTask::SendRsp(const CHAR *szMsgName, UINT32 ulSeqID, BOOL bRet)
{
    GJsonParam      ReqParam;

    ReqParam.Add("Result", bRet ? "Succ":"Fail");

    return SendRsp(szMsgName, ulSeqID, ReqParam);
}

BOOL GBaseTask::SendRsp(const CHAR *szMsgName, UINT32 ulSeqID, GJsonParam &Param)
{
    GJsonParam      ReqParam;

    ReqParam.Add("MsgName", szMsgName);
    ReqParam.Add("MsgSeqID", ulSeqID);
    ReqParam.Add("MsgInfo", Param);

    CHAR    *szMsg = ReqParam.GetString();

    return SendRsp(szMsg);
}

BOOL GBaseTask::SendRsp(const CHAR *szMsgName, UINT32 ulSeqID, const std::string &strSend)
{
    GJsonParam      ReqParam;

    ReqParam.Add("MsgName", szMsgName);
    ReqParam.Add("MsgSeqID", ulSeqID);
    ReqParam.Add("MsgInfo", strSend, FALSE);

    CHAR    *szMsg = ReqParam.GetString();

    return SendRsp(szMsg);
}

BOOL GBaseTask::SendRsp(const CHAR *szMsgName, UINT32 ulSeqID, GJsonTupleParam &Param)
{
    return SendRsp(szMsgName, ulSeqID, std::string(Param.GetString()));
}

BOOL GBaseTask::SendClient(PID_T *pstPID, UINT16 usMsgID, const CHAR *szMsgName, UINT32 ulSeqID, GJsonParam &Param)
{
    GJsonParam      ReqParam;

    ReqParam.Add("MsgName", szMsgName);
    ReqParam.Add("MsgSeqID", ulSeqID);
    ReqParam.Add("MsgInfo", Param);

    CHAR    *szMsg = ReqParam.GetString();

    return SendClient(pstPID, usMsgID, szMsg, strlen(szMsg)+1, ulSeqID);
}

BOOL GBaseTask::SendClient(PID_T *pstPID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulSeqID)
{
    return GosSendClient(m_usTaskID, pstPID, ulSeqID, usMsgID, pvMsg, ulMsgLen);
}

BOOL GBaseTask::SendClient(PID_T *pstPID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    return GosSendClient(m_usTaskID, pstPID, usMsgID, pvMsg, ulMsgLen);
}

VOID GBaseTask::SendAllClient(UINT8 ucClientType, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    GosSendAllClient(m_usTaskID, INVALID_TASK_ID, ucClientType, usMsgID, pvMsg, ulMsgLen);
}

VOID GBaseTask::SendAllClient(UINT8 ucClientType, UINT16 usMsgID, CHAR *szMsg)
{
    if (!szMsg)
    {
        return;
    }

    SendAllClient(ucClientType, usMsgID, szMsg, strlen(szMsg)+1);
}

VOID GBaseTask::SendAllClient(UINT8 ucClientType, UINT16 usMsgID, const CHAR *szMsgName, UINT32 ulSeqID, GJsonParam &Param)
{
    GJsonParam      ReqParam;

    ReqParam.Add("MsgName", szMsgName);
    ReqParam.Add("MsgSeqID", ulSeqID);
    ReqParam.Add("MsgInfo", Param);

    CHAR    *szMsg = ReqParam.GetString();

    SendAllClient(ucClientType, usMsgID, szMsg);
}

VOID GBaseTask::SendAllClient(UINT8 ucClientType, UINT16 usMsgID, const CHAR *szMsgName, UINT32 ulSeqID, const std::string &strMsgInfo)
{
    GJsonParam      ReqParam;

    ReqParam.Add("MsgName", szMsgName);
    ReqParam.Add("MsgSeqID", ulSeqID);
    ReqParam.Add("MsgInfo", strMsgInfo, FALSE);

    CHAR    *szMsg = ReqParam.GetString();

    SendAllClient(ucClientType, usMsgID, szMsg);
}

VOID GBaseTask::SendAllClient(UINT8 ucClientType, UINT16 usMsgID, const CHAR *szMsgName, UINT32 ulSeqID, GJsonTupleParam &Param)
{
    GJsonParam      ReqParam;

    ReqParam.Add("MsgName", szMsgName);
    ReqParam.Add("MsgSeqID", ulSeqID);
    ReqParam.Add("MsgInfo", Param);

    CHAR    *szMsg = ReqParam.GetString();

    SendAllClient(ucClientType, usMsgID, szMsg);
}

/* 向客户端发送广播消息*/
VOID GBaseTask::SendBroadcast(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
     GosSendBroadcast(m_usTaskID, &m_stSender, usMsgID, pvMsg, ulMsgLen);
}

BOOL GBaseTask::SyncSendClient(PID_T *pstPID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen, VOID *pvRspMsg, UINT32 ulMaxRspLen, UINT32 *ulRspMsgLen, UINT32 ulTimeout)
{
    return GosSyncSendClient(m_usTaskID, pstPID, usMsgID, pvMsg, ulMsgLen, pvRspMsg, ulMaxRspLen, ulRspMsgLen, ulTimeout);
}

VOID GBaseTask::_TaskLog(const CHAR *szFile, const CHAR *szFunc, UINT32 ulLine, UINT32 ulLogLevel, const CHAR *szFormat, ...)
{
    va_list         vaLog;
    CHAR            acLog[4096];

    va_start(vaLog, szFormat);

    memset(acLog, 0, sizeof(acLog));
    vsnprintf(acLog, sizeof(acLog)-1, szFormat, vaLog);

    va_end(vaLog);

    gos_log(szFile, szFunc, ulLine, m_szTaskName, ulLogLevel, acLog);
}

VOID GBaseTask::_TaskMsgLog(const CHAR *szFile, const CHAR *szFunc, UINT32 ulLine, UINT32 ulLogLevel, const CHAR *szFormat, ...)
{
    va_list         vaLog;
    CHAR            acLog[4096];

    va_start(vaLog, szFormat);

    memset(acLog, 0, sizeof(acLog));
    vsnprintf(acLog, sizeof(acLog)-1, szFormat, vaLog);

    va_end(vaLog);

    gos_msg_log(szFile, szFunc, ulLine, m_szTaskName, ulLogLevel, acLog);
}

VOID GBaseTask::OnTaskCheck()
{
    if (m_stSender.usTaskID != MODULE_PLAT)
    {
        TaskLog(LOG_ERROR, "Error send task(%u) of TaskCheckMsg", m_stSender.usTaskID);
    }

    SendRsp();
}

UINT16 GBaseTask::GetServerID(PEER_ADDR_T *pstServerAddr)
{
    DTPClient *pstDTP = GosGetDTPClient(pstServerAddr);
    if (!pstDTP)
    {
        return INVALID_INST_ID;
    }

    return pstDTP->GetServerID();
}

BOOL GBaseTask::Start(PEER_ADDR_T *pstServerAddr)
{
    return GosStartDTPClient(pstServerAddr);
}

BOOL GBaseTask::Start(PEER_ADDR_T *pstServerAddr, UINT16 ulServerID)
{
    return GosStartDTPClient(pstServerAddr, ulServerID);
}

BOOL GBaseTask::Stop(PEER_ADDR_T *pstServerAddr)
{
    return GosStopDTPClient(pstServerAddr);
}

