#if 0 // ·ÏÆú

#include "GPublic.h"

extern CHAR* GetLogLevel(UINT32 ulLogLevel);
extern UINT32 GetLogLevel(CHAR *szLogLevel);

#define MAXLOGNUM   0xffffff

GLISTNODE *g_CurrentNode = NULL;

VOID* MyMemAlloc(UINT32 ulSize)
{
    GBufferLog *pBufferLog = GBufferLog::GetInstance();

    return gos_mem_alloc(pBufferLog->m_MemPool, ulSize);
}

BOOL MySetFunc(VOID* pvSrc, UINT32 ulSize, VOID *pvDst)
{
    LOG_MODULE_BUFFER_T *pstSrc = (LOG_MODULE_BUFFER_T*)pvSrc;
    LOG_MODULE_BUFFER_T *pstDst = (LOG_MODULE_BUFFER_T*)pvDst;

    memcpy(pstDst, pstSrc, sizeof(LOG_MODULE_BUFFER_T));
    pstDst->szLog = (CHAR*)(pstDst+1);
    strcpy(pstDst->szLog, pstSrc->szLog);

    return TRUE;
}

VOID MyMemFree(VOID *pAddr)
{
    gos_mem_free(pAddr);
}

VOID AddToLogBuffer(UINT32 ulTime, CHAR *szLogModule, UINT32 ulLogLevel, CHAR *szLog, UINT32 ulLen)
{
    GBufferLog          *pBufferLog = GBufferLog::GetInstance();
    LOG_MODULE_BUFFER_T stLog = {0};

    if (!pBufferLog)
    {
        return;
    }

    stLog.ulLen = sizeof(LOG_MODULE_BUFFER_T)+ulLen+1;
    stLog.szLog = szLog;

    stLog.ulTime = ulTime;

    if (strcmp(szLogModule, "") == 0)
    {
        szLogModule = (CHAR*)"UNKNOWN";
    }

    strncpy(stLog.acLogModule, szLogModule, sizeof(stLog.acLogModule)-1);
    stLog.ulLogLevel = ulLogLevel;

    pBufferLog->Add(&stLog);
}

BOOL GosInitBufferLog(UINT32 ulMaxLogNum)
{
    if (0 == ulMaxLogNum)
    {
        return TRUE;
    }

    GBufferLog *pBufferLog = GBufferLog::GetInstance(ulMaxLogNum);
    if (NULL == pBufferLog)
    {
        return FALSE;
    }

    GLogFactory     *pLogFactory = GLogFactory::GetInstance();

    if (!pLogFactory)
    {
        return FALSE;
    }

    Logger *pLogger = pLogFactory->GetDefaultLogger();

    pLogger->SetTriggerFunc(AddToLogBuffer);

    return TRUE;
}

GBufferLog::GBufferLog()
{
    m_pLogMutex = NULL;
    m_ulDefaultLen = 64;                //100000*64=640k
    m_ulCount = 0;
    m_MemPool = NULL;
    m_pLogList = NULL;
}

GBufferLog::~GBufferLog()
{
    if (m_pLogMutex)
    {
        delete m_pLogMutex;
    }

    if (m_MemPool)
    {
        gos_mempool_free(m_MemPool);
    }

    if (m_pLogList)
    {
        delete m_pLogList;
    }
}

static GBufferLog   *s_pBufferLog = NULL;

GBufferLog* GBufferLog::GetInstance()
{
    return s_pBufferLog;
}

GBufferLog* GBufferLog::GetInstance(UINT32 ulMaxLogNum)
{
    if (!s_pBufferLog)
    {
        s_pBufferLog = new GBufferLog();
        if (!s_pBufferLog)
        {
            return NULL;
        }
    }

    if (!s_pBufferLog->Init(ulMaxLogNum))
    {
        delete s_pBufferLog;
        s_pBufferLog = NULL;
    }

    return s_pBufferLog;
}

BOOL GBufferLog::Init(UINT32 ulMaxLogNum)
{
    if (ulMaxLogNum == 0)
    {
        return FALSE;
    }

    m_ulMaxLogNum = ulMaxLogNum;

    m_pLogMutex = new GMutex();
    if (!m_pLogMutex)
    {
        return FALSE;
    }

    if (!InitMemPool())
    {
        return FALSE;
    }

    if (!InitList())
    {
        return FALSE;
    }

    return TRUE;
}

static UINT32 FormatMaxLogNum(UINT32 ulMaxLogNum)
{
    UINT32 ulMod = ulMaxLogNum%10;
    if (ulMod)
    {
        return ulMaxLogNum;
    }

    return ulMaxLogNum - ulMod;
}

BOOL GBufferLog::InitMemPool()
{
    UINT32 ulLogNum = 0;

    if (m_ulMaxLogNum <= 1000)
    {
        m_ulMaxLogNum = 1000;
    }

    ulLogNum = FormatMaxLogNum(m_ulMaxLogNum);
    GOS_MEM_POOL_T astMemPool[] = {{64, (UINT32)(0.3*ulLogNum)}, {96, (UINT32)(0.5*ulLogNum)}, {128, (UINT32)(0.2*ulLogNum)}};  //8.5M


    m_MemPool = gos_mempool_init(astMemPool, ARRAY_SIZE(astMemPool), TRUE, TRUE);

    if (!m_MemPool)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL GBufferLog::InitList()
{
    m_pLogList = new GList(0, m_ulMaxLogNum, sizeof(LOG_MODULE_BUFFER_T)+1024, MySetFunc, MyMemAlloc, MyMemFree, NULL);     //MemPool

    if (!m_pLogList)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL GBufferLog::Add(LOG_MODULE_BUFFER_T *pLog)
{
    if (!m_pLogMutex->Mutex())
    {
        return FALSE;
    }

    if (m_ulCount >= m_ulMaxLogNum)
    {
        m_pLogList->Remove();
        m_ulCount--;
    }

    if (!m_pLogList->Add(pLog, pLog->ulLen))
    {
        return FALSE;
    }

    m_ulCount++;

    m_pLogMutex->Unmutex();

    return TRUE;
}

//SysLogTask
GSysLogTask::GSysLogTask(UINT16 usDispatchID):GBaseTask(MODULE_SYS_LOG, (CHAR*)"SYSLOG", usDispatchID)
{
    RegisterMsg(EV_SYSLOG_INIT_LOG_IND);
    RegisterMsg(EV_SYSLOG_GET_LOG_REQ);
    RegisterMsg(EV_SYSLOG_GET_LOG_CFG_REQ);
    RegisterMsg(EV_SYSLOG_SET_LOG_CFG_REQ);

    SetTaskStatus(TASK_STATUS_WORK);
}

VOID GSysLogTask::TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    switch(usMsgID)
    {
        case EV_SYSLOG_INIT_LOG_IND:
            OnInitLogReq();
            break;

        case EV_SYSLOG_GET_LOG_REQ:
            OnGetLogReq(pvMsg, ulMsgLen);
            break;

        case EV_SYSLOG_GET_LOG_CFG_REQ:
            OnGetLogCfgReq(pvMsg, ulMsgLen);
            break;

        case EV_SYSLOG_SET_LOG_CFG_REQ:
            OnSetLogCfgReq(pvMsg, ulMsgLen);
            break;

        default:
            break;
    }
}

VOID GSysLogTask::OnInitLogReq()
{
    GBufferLog  *pBufferLog = GBufferLog::GetInstance();
    GList       *pList;

    if (!pBufferLog)
    {
        return;
    }

    pList = pBufferLog->GetList();
    g_CurrentNode = pList->GetHeadNode();
}

VOID GSysLogTask::OnGetLogReq(VOID *pvMsg, UINT32 ulMsgLen)
{
    GBufferLog  *pBufferLog = GBufferLog::GetInstance();
    GList       *pList = NULL;
    GLISTNODE   *pNode = NULL;
    GET_SYSLOG_REQ_T    *pstReq = (GET_SYSLOG_REQ_T*)pvMsg;
    GET_SYSLOG_RSP_T    *pstRsp = (GET_SYSLOG_RSP_T*)AllocBuffer(sizeof(GET_SYSLOG_RSP_T));
    UINT32              ulLogLen;
    SYSLOG_INFO_T       *pstSysLogInfo;
    UINT32              ulRspMsgLen;

    if (!pstRsp)
    {
        return;
    }

    if (!pBufferLog)
    {
        pstRsp->ulLogDataLen = 0;
        pstRsp->ulLogNum = 0;
        goto end;
    }

    pList       = pBufferLog->GetList();
    pNode       = g_CurrentNode;

    NTOHL_IT(pstReq->ulStartTime);
    NTOHL_IT(pstReq->ulEndTime);
    NTOHL_IT(pstReq->ulLogLevel);

    while(pNode)
    {
        LOG_MODULE_BUFFER_T *pstLog = (LOG_MODULE_BUFFER_T*)pList->GetNodeData(pNode);

        //ÅÐ¶ÏÌõ¼þ
        if ((pstReq->ulEndTime == 0xffffffff || (pstReq->ulEndTime != 0 && pstLog->ulTime < pstReq->ulEndTime)) &&
            (pstReq->ulStartTime == 0xffffffff || (pstReq->ulEndTime != 0 && pstLog->ulTime > pstReq->ulStartTime)) &&
            (pstReq->ulLogLevel == LOG_ALL || (pstReq->ulLogLevel != LOG_NULL && pstLog->ulLogLevel == pstReq->ulLogLevel)) &&
            (pstReq->acLogModule[0] == '\0' || strcmp(pstLog->acLogModule, pstReq->acLogModule) == 0))
        {
            pstSysLogInfo = (SYSLOG_INFO_T*)&pstRsp->aucLogData[pstRsp->ulLogDataLen];

            pstSysLogInfo->ulTime = HTONL(pstLog->ulTime);
            pstSysLogInfo->ulLogLevel = HTONL(pstLog->ulLogLevel);
            strcpy(pstSysLogInfo->acLogModule, pstLog->acLogModule);
            strcpy(pstSysLogInfo->acLog, pstLog->szLog);

            ulLogLen = STRUCT_OFFSET(SYSLOG_INFO_T, acLog) + strlen(pstSysLogInfo->acLog)+1;
            pstRsp->ulLogNum ++;
            pstRsp->ulLogDataLen += ulLogLen;
        }

        if ((pstRsp->ulLogDataLen + 1024) > sizeof(GET_SYSLOG_RSP_T))
        {
            Log(LOG_ERROR, "LogData buffer is full.");
            break;
        }

        pNode = pList->GetNextNode(pNode);
        g_CurrentNode = pNode;
    }

end:
    ulRspMsgLen = STRUCT_OFFSET(GET_SYSLOG_RSP_T,aucLogData) + pstRsp->ulLogDataLen;
    HTONL_IT(pstRsp->ulLogNum);
    HTONL_IT(pstRsp->ulLogDataLen);

    SendRsp(pstRsp, ulRspMsgLen);
}

VOID GSysLogTask::OnGetLogCfgReq(VOID *pvMsg, UINT32 ulMsgLen)
{
    GET_SYSLOG_CFG_RSP_T    *pstRsp = (GET_SYSLOG_CFG_RSP_T*)AllocBuffer(sizeof(GET_SYSLOG_CFG_RSP_T));
    Logger                  *pLogger = GLogFactory::GetInstance()->GetDefaultLogger();
    VECTOR<UINT32>          vModuleList;
    UINT32                  i;
    UINT32                  ulSize;

    vModuleList = pLogger->GetModuleList();
    ulSize = vModuleList.size();

    for (i=0; i<ulSize; i++)
    {
        pLogger->GetModule(vModuleList[i], pstRsp->astLogCfg[i].acLogModule, pstRsp->astLogCfg[i].ulLogLevel);
        HTONL_IT(pstRsp->astLogCfg[i].ulLogLevel);
    }

    pstRsp->ulLogModuleNum = ulSize;
    HTONL_IT(pstRsp->ulLogModuleNum);

    ulMsgLen = STRUCT_OFFSET(GET_SYSLOG_CFG_RSP_T, astLogCfg) +ulSize*sizeof(SYSLOG_CFG_T);
    SendRsp(pstRsp, ulMsgLen);
}

VOID GSysLogTask::OnSetLogCfgReq(VOID *pvMsg, UINT32 ulMsgLen)
{
    SET_SYSLOG_CFG_REQ_T    *pstReq = (SET_SYSLOG_CFG_REQ_T*)pvMsg;
    SET_SYSLOG_CFG_RSP_T    stRsp = {0};
    UINT32                  ulModuleNum;
    UINT32                  ulCount = 0;
    Logger                  *pLogger = GLogFactory::GetInstance()->GetDefaultLogger();

    ulModuleNum = NTOHL(pstReq->ulLogModuleNum);

    for (UINT32 i=0; i<ulModuleNum; i++)
    {
        if(pLogger->SetLogLevel(pstReq->astLogCfg[i].acLogModule, NTOHL(pstReq->astLogCfg[i].ulLogLevel)))
        {
            ulCount ++;
        }
    }

    if (ulCount == ulModuleNum)
    {
        stRsp.bRet = TRUE;
    }
    else
    {
        stRsp.bRet = TRUE;
    }

    ulMsgLen = sizeof(SET_SYSLOG_CFG_RSP_T);

    SendRsp(&stRsp, ulMsgLen);
}

#endif
