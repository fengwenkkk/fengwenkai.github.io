#include "RecPublic.h"
#include "TaskRec.h"

#define CHECK_TIMER         TIMER10
#define EV_CHECK_TIMER      GET_TIMER_EV(CHECK_TIMER)

#define MODULE_REC      101

TaskRec::TaskRec(UINT16 usDispatchID):GBaseTask(MODULE_REC, (CHAR*)"REC", usDispatchID)
{
}

BOOL TaskRec::Init()
{
    SetLoopTimer(CHECK_TIMER, 10000);
    SetTaskStatus(TASK_STATUS_WORK);

    Log(LOG_INFO, "Task init successful!");

    return TRUE;
}

VOID TaskRec::OnCheckTimer()
{
}

VOID TaskRec::TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
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
                Log(LOG_FATAL, "Init failed!");
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

        default:
            Log(LOG_ERROR, "unknown msg %u", usMsgID);
            break;
        }

        break;

    default:
        break;
    }
}

