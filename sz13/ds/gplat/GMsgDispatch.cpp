#include "GPublic.h"
#include "DTPServer.h"
#include "DTPClient.h"

static GSysTask                         *m_pSysTask = NULL;

static IntMap<GBaseTask>        m_mTask(16);        /* TaskID->Task */
static IntMap<GBaseTask>        m_mMsgTask(1024);   /* MsgID->Task */
static IntMap<GProc>            m_mTaskProc(16);    /* TaskID->Proc */
static IntMap<GProc>            m_mThreadProc(16);  /* ThreadID->Proc */

static GMutex                   *m_pMapMutex = NULL;    /* 资源锁 */
static UINT32                   m_ulMsgMaxWaitTime = 60*5;  /* socket异步消息最大等待时间 */

static UINT32                   m_ulMaxMsgNodeNum = 512;    /* proc 消息队列最大个数 */

static BOOL                     m_bExitWhenMsgQueueIsFull = FALSE;    /* 消息队列满系统自动退出 */

extern BOOL _InitDTPMemAlloc();

extern BOOL _InitDTPServer(UINT16 usServerID, UINT8 *pucAddr, UINT16 usPort);
extern VOID _RunDTPServer();
extern VOID _FreeDTPServer();

extern DTPClient* _InitDTPClient(UINT8 *pucAddr, UINT16 usPort, BOOL bTCPMode);
extern VOID _RunDTPClient();
extern VOID _FreeDTPClient();

VOID GosSetExitWhenMsgQueueIsFull(BOOL bExitWhenMsgQueueIsFull)
{
    m_bExitWhenMsgQueueIsFull = bExitWhenMsgQueueIsFull;
}

VOID GosSetMsgMaxWaitTime(UINT32 ulMsgMaxWaitTime)
{
    m_ulMsgMaxWaitTime = ulMsgMaxWaitTime;
}

VOID GosSetMaxMsgNodeNum(UINT32 ulMaxMsgNodeNum)
{
    if (0 == ulMaxMsgNodeNum)
    {
        return;
    }

    m_ulMaxMsgNodeNum = ulMaxMsgNodeNum;
}

UINT32 GosGetMaxMsgNodeNum()
{
    return m_ulMaxMsgNodeNum;
}

VOID SocketMsgHandle(MSG_T *pstMsgHdr, VOID *pvMsgBody, UINT32 ulMsgBodyLen)
{
    if (pstMsgHdr->ulTimeout == 0)
    {
        pstMsgHdr->ulTimeout = m_ulMsgMaxWaitTime;
    }

    GosDispatchMsg(pstMsgHdr, pvMsgBody, ulMsgBodyLen);
}

BOOL MsgDispatchInit(UINT16 usInstID, IP_ADDR_T *pstSelfAddr, PEER_ADDR_T *pstPeerAddr, UINT32 ulPeerNum)
{
    if (!m_pMapMutex)
    {
        m_pMapMutex = new GMutex();
    }

    if (!_InitDTPMemAlloc())
    {
        return FALSE;
    }

    if (pstSelfAddr)
    {
        if (!_InitDTPServer(usInstID, pstSelfAddr->aucIP, pstSelfAddr->usPort))
        {
            return FALSE;
        }
    }

    for (UINT32 i=0; i<ulPeerNum; i++)
    {
        if (!_InitDTPClient(pstPeerAddr[i].aucIP, pstPeerAddr[i].usPort, pstPeerAddr[i].ucTCPMode))
        {
            _FreeDTPClient();
            _FreeDTPServer();

            return FALSE;
        }
    }

    if (!m_pSysTask)
    {
        m_pSysTask = new GSysTask(0xffff);
        GosRegisterTask(m_pSysTask);
    }

    return TRUE;
}

VOID MsgDispatchStart()
{
    _RunDTPServer();
    _RunDTPClient();
}

CHAR* GosGetTaskName(UINT16 usTaskID)
{
    VECTOR<GBaseTask*>  vTask;
    CHAR                *szTaskName = (CHAR*)"";

    if (!m_pMapMutex->Mutex())
    {
        return (CHAR*)"";
    }

    m_mTask.GetValueList(vTask);

    for (UINT32 i=0; i<vTask.size(); i++)
    {
        GBaseTask   *pTask = vTask[i];

        if (usTaskID == pTask->GetTaskID())
        {
            szTaskName = pTask->GetTaskName();
            break;
        }
    }

    m_pMapMutex->Unmutex();
    return szTaskName;
}

BOOL GosGetTaskStatus(std::vector<GOS_TASK_STATUS_T> &vTaskStatus)
{
    VECTOR<GBaseTask*>   vTask;

    if (!m_pMapMutex->Mutex())
    {
        return FALSE;
    }

    m_mTask.GetValueList(vTask);

    for (UINT32 i=0; i<vTask.size(); i++)
    {
        GBaseTask           *pTask = vTask[i];
        GOS_TASK_STATUS_T   stStatus = {0};

        strncpy(stStatus.acTaskName, pTask->GetTaskName(), sizeof(stStatus.acTaskName)-1);
        stStatus.usTaskID = pTask->GetTaskID();
        stStatus.usMsgID = pTask->GetMsgID();
        pTask->GetMsgSender(&stStatus.stSenderPID);
        stStatus.ulMsgStartTime = pTask->GetMsgStartTime();
        stStatus.ulMsgEndTime = pTask->GetMsgEndTime();
    }

    m_pMapMutex->Unmutex();
    return TRUE;
}

BOOL GosGetTaskStatus(UINT32 ulTaskID, GOS_TASK_STATUS_T *pstTaskStatus)
{
    GBaseTask   *pTask;

    if (!m_pMapMutex->Mutex())
    {
        return FALSE;
    }

    pTask = m_mTask.GetValue(ulTaskID);
    if (pTask)
    {
        strncpy(pstTaskStatus->acTaskName, pTask->GetTaskName(), sizeof(pstTaskStatus->acTaskName)-1);
        pstTaskStatus->usTaskID = pTask->GetTaskID();
        pstTaskStatus->usMsgID = pTask->GetMsgID();
        pTask->GetMsgSender(&pstTaskStatus->stSenderPID);
        pstTaskStatus->ulMsgStartTime = pTask->GetMsgStartTime();
        pstTaskStatus->ulMsgEndTime = pTask->GetMsgEndTime();
    }

    m_pMapMutex->Unmutex();
    return (pTask != NULL);
}

BOOL GosRegisterTask(GBaseTask *pTask)
{
    UINT16      usDispatchID = pTask->GetDispatchID();
    UINT16      usTaskID = pTask->GetTaskID();
    GProc       *pProc = GProc::GetProc(usDispatchID);
    UINT32      ulThreadID;

    pProc->Suspend();

    pTask->SetDispatchID(usDispatchID);

    if (!m_pMapMutex->Mutex())
    {
        return FALSE;
    }

    ulThreadID = pProc->GetThreadID();
    if (m_mTaskProc.GetValue(usTaskID))
    {
        GosLog(LOG_FATAL, "Task id(%u) of %s is registered", usTaskID, pTask->GetTaskName());
        exit(-1);
    }

    m_mTaskProc.Add(usTaskID, pProc);
    m_mThreadProc.Add(ulThreadID, pProc);

    UINT16                                  usMsgID;
    std::vector<TASK_MSG_DESC_T>            vMsgList = pTask->GetMsgList();
    std::vector<TASK_MSG_DESC_T>::iterator  it = vMsgList.begin();

    for (; it!=vMsgList.end(); it++)
    {
        usMsgID = it->usMsgID;

        GBaseTask   *pPreTask = m_mMsgTask.GetValue(usMsgID);

        if (pPreTask)
        {
            GosLog(LOG_ERROR, "msg %d has registered at task %s", usMsgID, pPreTask->GetTaskName());
            m_pMapMutex->Unmutex();

            exit(-1);
            //return FALSE;
        }

        m_mMsgTask.Add(usMsgID, pTask);

        if (it->ucUniqueMsg)
        {
            pProc->InitUniqueMsg(usMsgID, it->pfUniqueMsgCmp);
        }
    }

    pProc->InitUniqueMsg(EV_TASK_INIT_REQ);
    pProc->InitUniqueMsg(EV_TASK_CHECK_REQ);

    m_mTask.Add(usTaskID, pTask);

    m_pMapMutex->Unmutex();

    m_pSysTask->RegisterTask(pTask);

    return TRUE;
}

GBaseTask* GetTask(UINT16 usTaskID, UINT16 usMsgID)
{
    GBaseTask       *pTask = NULL;

    if (!m_pMapMutex->Mutex())
    {
        return NULL;
    }

    if (usTaskID == INVALID_TASK_ID)
    {
        pTask = m_mMsgTask.GetValue(usMsgID);
    }
    else
    {
        pTask = m_mTask.GetValue(usTaskID);
    }

    m_pMapMutex->Unmutex();

    return pTask;
}

GProc* GosGetProc(UINT16 usTaskID)
{
    GProc       *pProc = NULL;

    if (!m_pMapMutex->Mutex())
    {
        return FALSE;
    }

    pProc = m_mTaskProc.GetValue(usTaskID);

    m_pMapMutex->Unmutex();

    return pProc;
}

typedef VOID (*GOS_MSG_PREHANDLE)(MSG_T *pstMsgHdr, VOID *pvMsg, UINT32 ulMsgLen);

static GOS_MSG_PREHANDLE   g_fMsgPreHandle = NULL;

VOID GosRegisterMsgPreHandle(GOS_MSG_PREHANDLE pfHandle)
{
    g_fMsgPreHandle = pfHandle;
}

BOOL GosDispatchMsg(MSG_T *pstMsgHdr, VOID *pvMsg, UINT32 ulMsgLen)
{
    GBaseTask   *pTask = NULL;
    GProc       *pProc = NULL;
    GMsgPool    *pMsgPool = GMsgPool::GetInstance();
    UINT16      usTaskID;

    if (!pMsgPool)
    {
        return FALSE;
    }

    if (!m_pMapMutex->Mutex())
    {
        return FALSE;
    }

    usTaskID = pstMsgHdr->stReceiver.usTaskID;
    if (usTaskID == INVALID_TASK_ID)
    {
        pTask = m_mMsgTask.GetValue(pstMsgHdr->usMsgID);
        if (!pTask)
        {
            m_pMapMutex->Unmutex();
            return FALSE;
        }

        usTaskID = pTask->GetTaskID();
    }

    pProc = m_mTaskProc.GetValue(usTaskID);

    m_pMapMutex->Unmutex();

    if (g_fMsgPreHandle)
    {
        g_fMsgPreHandle(pstMsgHdr, pvMsg, ulMsgLen);
    }

    if (!pProc)
    {
        return FALSE;
    }

    pProc->AddMsg(usTaskID, pstMsgHdr, pvMsg, ulMsgLen);
    pProc->Resume();

    return TRUE;
}

VOID GosDispatchSystemMsg(MSG_T *pstMsg)
{
    GProc       *pProc;
    UINT32      i;
    UINT16      usTaskID;
    GMsgPool    *pMsgPool = GMsgPool::GetInstance();
    VECTOR<UINT32>                  vTask;

    if (!pMsgPool || !m_pMapMutex->Mutex())
    {
        return;
    }

    m_mTaskProc.GetKeyList(vTask);

    std::vector<GProc *>    vProc;

    for (i=0; i<vTask.size(); i++)
    {
        usTaskID = vTask[i];

        pProc = m_mTaskProc.GetValue(usTaskID);
        if (!pProc)
        {
            continue;
        }

        if (!pProc->AddMsg(usTaskID, pstMsg))
        {
        }
        else
        {
            vProc.push_back(pProc);
        }
    }

    m_pMapMutex->Unmutex();

    for (i=0; i<vProc.size(); i++)
    {
        vProc[i]->Resume();
    }
}

GProc* _GetCurrProc()
{
    UINT32      ulThreadID = gos_get_thread_id();

    if (!m_pMapMutex->Mutex())
    {
        return NULL;
    }

    GProc   *pProc = m_mThreadProc.GetValue(ulThreadID);

    m_pMapMutex->Unmutex();

    return pProc;
}

UINT16 GosGetTaskID()
{
    GProc*  pProc = _GetCurrProc();

    if (!pProc)
    {
        return INVALID_TASK_ID;
    }

    return pProc->GetTaskID();
}

BOOL GosStart(PLAT_CFG_T *pstPlatCfg)
{
    GTimer          *pTimer;
    GMsgPool        *pMsgPool;
    static BOOL     bInit = FALSE;
    GLogFactory     *pLogFactory = GLogFactory::GetInstance();

    if (pstPlatCfg->bMultiInst == FALSE)
    {
        if (gos_is_proc_running())
        {
            GosLog(LOG_FATAL, "current proc is running");

            return FALSE;
        }
    }

    if (!pLogFactory)
    {
        return FALSE;
    }
    GosInitLog(&pstPlatCfg->stLogCfg);

#if 0//def _DEBUG
    pLogger->SetLogLevel(LOG_DETAIL);
    pLogger->SetLogToStdout(TRUE);
#endif

    IP_ADDR_T   *pstSelfAddr = pstPlatCfg->bServerMode?&pstPlatCfg->stSelfAddr:NULL;

    if (!MsgDispatchInit(pstPlatCfg->usInstID, pstSelfAddr, pstPlatCfg->astPeerAddr, pstPlatCfg->ulPeerNum))
    {
        GosLog(MODULE_PLAT, LOG_FATAL, "Init MsgDispath failed!");
        return FALSE;
    }

    GosLog(MODULE_PLAT, LOG_INFO, "Init MsgDispath successful!");

    pTimer = GTimer::GetInstance();
    if (!pTimer)
    {
        GosLog(MODULE_PLAT, LOG_FATAL, "Init Timer failed!");
        return FALSE;
    }

    GosLog(MODULE_PLAT, LOG_INFO, "Init Timer successful!");

    pMsgPool = GMsgPool::GetInstance();
    if (!pMsgPool)
    {
        GosLog(MODULE_PLAT, LOG_FATAL, "Init MsgPool failed!");
        return FALSE;
    }

    GosLog(MODULE_PLAT, LOG_INFO, "Init MsgPool successful!");

    if (!bInit)
    {
        bInit = TRUE;
        MsgDispatchStart();
        GosLog(MODULE_PLAT, LOG_FATAL, "Base init successful!");
    }

    GosInitMsgLogger();

    return TRUE;
}

BOOL GosGetProcList(VECTOR<UINT16> &vProcID)
{
    vProcID.clear();

    if (!m_pMapMutex->Mutex())
    {
        return FALSE;
    }

    VECTOR<GProc*>  vProc;

    m_mThreadProc.GetValueList(vProc);
    for (UINT32 i=0; i<vProc.size(); i++)
    {
        vProcID.push_back(vProc[i]->GetThreadID());
    }

    m_pMapMutex->Unmutex();

    return TRUE;
}

