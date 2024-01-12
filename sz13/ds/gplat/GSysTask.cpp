#include "GPublic.h"

extern BOOL GosGetProcList(VECTOR<UINT16> &vProcID);
extern BOOL GosMsgStat(UINT16 usProcID, MAP<UINT32, UINT32> &mProcMsg);

GSysTask::GSysTask(UINT16 usDispatchID):GBaseTask(MODULE_PLAT, "PLAT", usDispatchID)
{
    m_Mutex = new GMutex();

    SetTaskStatus(TASK_STATUS_WORK);
    SetLoopTimer(TIMER0, 300);
    SetLoopTimer(TIMER1, 30*1000);
}

VOID GSysTask::OnTaskInitMsg(VOID *pvMsg, UINT32 ulMsgLen)
{
    if (!m_Mutex->Mutex())
    {
        return;
    }

    PID_T                               stSender;
    std::vector<TASK_INFO_T>::iterator  it = m_vTaskInfo.begin();

    GetMsgSender(&stSender);

    for (; it!=m_vTaskInfo.end(); it++)
    {
        if (it->usTaskID == stSender.usTaskID)
        {
            it->bInited = TRUE;
            break;
        }
    }

    m_Mutex->Unmutex();
}

VOID GSysTask::OnTaskCheckMsg(VOID *pvMsg, UINT32 ulMsgLen)
{
    if (!m_Mutex->Mutex())
    {
        return;
    }

    PID_T                               stSender;
    std::vector<TASK_INFO_T>::iterator  it = m_vTaskInfo.begin();

    GetMsgSender(&stSender);
    for (; it!=m_vTaskInfo.end(); it++)
    {
        if (it->usTaskID == stSender.usTaskID)
        {
            it->ulLastEchoTime = gos_get_uptime_1ms();
            break;
        }
    }

    m_Mutex->Unmutex();
}

VOID GSysTask::TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    switch(usMsgID)
    {
        case EV_TIMER0:
            CheckTask();
            break;

        case EV_TIMER1:
            break;

        case EV_TASK_INIT_RSP:
            OnTaskInitMsg(pvMsg, ulMsgLen);
            break;

        case EV_TASK_CHECK_RSP:
            OnTaskCheckMsg(pvMsg, ulMsgLen);
            break;

        default:
            break;
    }
}

BOOL GSysTask::RegisterTask(GBaseTask *pTask)
{
    TASK_INFO_T     stTaskInfo = {0};

    stTaskInfo.usTaskID = pTask->GetTaskID();
    stTaskInfo.bInited = FALSE;
    stTaskInfo.bCheckTask = pTask->GetCheckTask();
    stTaskInfo.ulLastCheckTime = 0;
    stTaskInfo.ulLastEchoTime = 0;

    if (pTask == this)
    {
        stTaskInfo.bInited = TRUE;
    }

    if (!m_Mutex->Mutex())
    {
        return FALSE;
    }

    m_vTaskInfo.push_back(stTaskInfo);

    m_Mutex->Unmutex();

    return TRUE;
}

VOID GSysTask::SendTaskInitMsg(UINT16 usTaskID)
{
    SendLocal(usTaskID, EV_TASK_INIT_REQ, NULL, 0);
}

VOID GSysTask::SendTaskCheckMsg(UINT16 usTaskID)
{
    SendLocal(usTaskID, EV_TASK_CHECK_REQ, NULL, 0);
}

VOID GSysTask::CheckTask()
{
    std::vector<TASK_INFO_T>::iterator      it;
    UINT32                                  ulCurrTime = gos_get_uptime_1ms();
    UINT32                                  ulInitPeriod = 1000;
    UINT32                                  ulCheckPeriod = 10000;
    UINT32                                  ulTimeout = 30000;
    UINT32                                  ulNoRspTime;
    FLOAT                                   fTimeRate;
    GOS_TASK_STATUS_T                       stTaskStatus;

    if (!m_Mutex->Mutex())
    {
        return;
    }

    for (it=m_vTaskInfo.begin(); it!=m_vTaskInfo.end(); it++)
    {
        if (it->usTaskID == GetTaskID())
        {
            continue;
        }

        ulCurrTime = gos_get_uptime_1ms();

        if (!it->bInited)
        {
            fTimeRate = ((FLOAT)(ulCurrTime - it->ulLastCheckTime))/ulInitPeriod;
            if (fTimeRate > 0.97)
            {
                SendTaskInitMsg(it->usTaskID);
                it->ulLastCheckTime = ulCurrTime;
            }
        }
        else if (it->bCheckTask)
        {
            fTimeRate = ((FLOAT)(ulCurrTime - it->ulLastCheckTime))/ulCheckPeriod;
            if (fTimeRate > 0.97)
            {
                SendTaskCheckMsg(it->usTaskID);
                it->ulLastCheckTime = ulCurrTime;

                if (GosGetTaskStatus(it->usTaskID, &stTaskStatus))
                {
                    ulNoRspTime = gos_get_uptime() - stTaskStatus.ulMsgStartTime;

                    if (stTaskStatus.ulMsgStartTime > 0 &&
                        stTaskStatus.ulMsgEndTime == 0 &&
                        ulNoRspTime > ulTimeout)
                    {
                        TaskLog(LOG_FATAL, "task %s current MsgID=%u, StartTime=%u, RunTime=%u, SendInst=%u, SendTask=%u",
                            stTaskStatus.acTaskName, stTaskStatus.usMsgID,
                            stTaskStatus.ulMsgStartTime, ulNoRspTime,
                            stTaskStatus.stSenderPID.usInstID, stTaskStatus.stSenderPID.usTaskID);
                    }
                }
            }
        }
    }

    m_Mutex->Unmutex();
}

BOOL GSysTask::GetMsgStat()
{
    return TRUE;
}
