#include "GPublic.h"
#include "dtp_public.h"

static GTimer           *m_pInstance = NULL;

UINT16 GosGetTimerMsgID(UINT16 usTimerID)
{
    UINT16      usMsgID = EV_TIMER0 + usTimerID;

    return usMsgID;
}

GTimer* GTimer::GetInstance()
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new GTimer();

        m_pInstance->Start();
    }

    return m_pInstance;
}

GTimer::GTimer():GThread(NULL)
{
    m_hTimerInfo = gos_list_init(32, 1024, sizeof(TIMER_INFO_T), NULL, gos_malloc, gos_free, NULL);
    m_Mutex = new GMutex();
}

GOS_THREAD_RET GTimer::ThreadEntry(void* pPara)
{
    UINT32  ulLastTime = gos_get_uptime_1ms();
    UINT32  ulCurrTime = 0;
    INT32   iTime = 0;
    UINT32  ulCheckPeriod = 200;

    while(1)
    {
        CheckTimer();

        if ((int)ulCheckPeriod > iTime)
        {
            gos_sleep_1ms(ulCheckPeriod-iTime/2);
        }

        ulCurrTime = gos_get_uptime_1ms();
        iTime = ulCurrTime - ulLastTime - ulCheckPeriod;
        ulLastTime += ulCheckPeriod;
    }
}

BOOL GTimer::SetTimer(BOOL bLoopTimer, UINT16 usTaskID, UINT16 usTimerID, UINT32 ulTimeout, UINT32 ulParaType, TIMER_PARA_U unPara)
{
    if (usTimerID >= TIMER_NUM)
    {
        return FALSE;
    }

    TIMER_INFO_T        stTimerInfo = {0};

    if (ulTimeout == 0)
    {
        ulTimeout = 10;
    }

    if (ulTimeout > (3600*24*1000))
    {
        ulTimeout = 3600*24*1000;
    }

    stTimerInfo.bLoopTimer = bLoopTimer;
    stTimerInfo.usTaskID = usTaskID;
    stTimerInfo.usTimerID = usTimerID;
    stTimerInfo.ulTimeout = ulTimeout;
    stTimerInfo.ulParaType = ulParaType;
    stTimerInfo.unPara = unPara;
    stTimerInfo.ulStartTime = gos_get_uptime_1ms();

    GosLog(LOG_INFO, "create %s %u of task %u", bLoopTimer?"loop timer":"timer", usTimerID, usTaskID);

    if (!m_Mutex->Mutex())
    {
        return FALSE;
    }

    GLISTNODE   *pNode = gos_list_add(m_hTimerInfo, &stTimerInfo, sizeof(stTimerInfo));

    m_Mutex->Unmutex();

    if (bLoopTimer)
    {
        GProc       *pProc = GosGetProc(usTaskID);
        UINT16      usMsgID = GosGetTimerMsgID(usTimerID);

        if (pProc)
        {
            pProc->InitUniqueMsg(usMsgID);
        }
    }

    return pNode!=NULL;
}

BOOL GTimer::SetTimer(UINT16 usTaskID, UINT16 usTimerID, UINT32 ulTimeout, UINT32 ulPara)
{
    TIMER_PARA_U    unPara = {0};

    unPara.ulPara = ulPara;

    return SetTimer(FALSE, usTaskID, usTimerID, ulTimeout, TIMER_PARA_INT, unPara);
}

BOOL GTimer::SetTimer(UINT16 usTaskID, UINT16 usTimerID, UINT32 ulTimeout, VOID *pvPara)
{
    TIMER_PARA_U    unPara = {0};

    unPara.pvPara = pvPara;

    return SetTimer(FALSE, usTaskID, usTimerID, ulTimeout, TIMER_PARA_PTR, unPara);
}

BOOL GTimer::SetLoopTimer(UINT16 usTaskID, UINT16 usTimerID, UINT32 ulTimeout, UINT32 ulPara)
{
    TIMER_PARA_U    unPara = {0};

    unPara.ulPara = ulPara;

    return SetTimer(TRUE, usTaskID, usTimerID, ulTimeout, TIMER_PARA_INT, unPara);
}

BOOL GTimer::SetLoopTimer(UINT16 usTaskID, UINT16 usTimerID, UINT32 ulTimeout, VOID *pvPara)
{
    TIMER_PARA_U    unPara = {0};

    unPara.pvPara = pvPara;

    return SetTimer(TRUE, usTaskID, usTimerID, ulTimeout, TIMER_PARA_PTR, unPara);
}

VOID GTimer::DispatchTimerMsg(TIMER_INFO_T *pstTimerInfo)
{
    MSG_T           stMsg = {0};
    TIMER_MSG_T     stTimerMsg = {0};
    UINT32          ulMsgLen = sizeof(TIMER_MSG_T);
    GMsgPool        *pMsgPool = GMsgPool::GetInstance();

    if (!pMsgPool)
    {
        return;
    }

    stTimerMsg.ulParaType = pstTimerInfo->ulParaType;
    stTimerMsg.unPara = pstTimerInfo->unPara;

    stMsg.stSender.usInstID = LOCAL_INST_ID;
    stMsg.stSender.usTaskID = pstTimerInfo->usTaskID;

    stMsg.stReceiver.usInstID = LOCAL_INST_ID;
    stMsg.stReceiver.usTaskID = pstTimerInfo->usTaskID;

    stMsg.usMsgID = GosGetTimerMsgID(pstTimerInfo->usTimerID);
    stMsg.ulTimeout = 0;
    stMsg.ucMsgPri = MSG_PRI_IMP;

    stMsg.ulMsgLen = ulMsgLen;

    GosDispatchMsg(&stMsg, &stTimerMsg, ulMsgLen);
}

BOOL GTimer::ClearTimer(UINT16 usTaskID, UINT16 usTimerID)
{
    TIMER_INFO_T        *pstTimerInfo;
    GLISTNODE           *pNode;
    GLISTNODE           *pNextNode;
    BOOL                bRet = FALSE;

    if (!m_Mutex->Mutex())
    {
        return FALSE;
    }

    pNode = gos_list_get_head_node(m_hTimerInfo);

    while(pNode)
    {
        pstTimerInfo = (TIMER_INFO_T*)gos_list_get_node_data(pNode);
        pNextNode = gos_list_get_next_node(m_hTimerInfo, pNode);

        if (pstTimerInfo->usTaskID == usTaskID && pstTimerInfo->usTimerID == usTimerID)
        {
            gos_list_remove_node(m_hTimerInfo, pNode);
            bRet = TRUE;
            break;
        }

        pNode = pNextNode;
    }

    m_Mutex->Unmutex();

    return bRet;
}

VOID GTimer::CheckTimer()
{
    TIMER_INFO_T        *pstTimerInfo;
    UINT32              ulCurrTime = gos_get_uptime_1ms();
    UINT32              ulDeltaTime;
    GLISTNODE           *pNode;
    GLISTNODE           *pNextNode;

    if (!m_Mutex->Mutex())
    {
        return;
    }

    pNode = gos_list_get_head_node(m_hTimerInfo);

    while(pNode)
    {
        pstTimerInfo = (TIMER_INFO_T*)gos_list_get_node_data(pNode);
        pNextNode = gos_list_get_next_node(m_hTimerInfo, pNode);

        ulDeltaTime = (UINT32)(ulCurrTime - pstTimerInfo->ulStartTime);
        if (ulDeltaTime >= pstTimerInfo->ulTimeout)
        {
            /* ³¬Ê± */
            DispatchTimerMsg(pstTimerInfo);

            if (!pstTimerInfo->bLoopTimer)
            {
                gos_list_remove_node(m_hTimerInfo, pNode);
            }
            else
            {
                pstTimerInfo->ulStartTime = ulCurrTime;
            }
        }

        pNode = pNextNode;
    }

    m_Mutex->Unmutex();
}

