#include "g_include.h"
#include "GPublic.h"
#include "TaskMain.h"
#include "TaskMake.h"

#define TIMER_INIT          TIMER0
#define EV_INIT_IND         EV_TIMER0

#define TIMER_CHECK         TIMER11
#define EV_TIMER_CHECK_IND  GET_TIMER_EV(TIMER_CHECK)

TaskMake::TaskMake(UINT16 usDispatchID, UINT16 usTaskIndex):GBaseTask(100+usTaskIndex, "Make", usDispatchID)
{
    CHAR    acTaskName[32];

    sprintf(acTaskName, "Make%u", usTaskIndex);
    SetTaskName(acTaskName);

    m_bLoaded = FALSE;
}

BOOL TaskMake::Init()
{
    SetTaskStatus(TASK_STATUS_WORK);

    SetTimer(TIMER_CHECK, 1000);

    Log(LOG_INFO, "Task init successful!");

    return TRUE;
}

VOID TaskMake::TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
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

                default:
                    Log(LOG_ERROR, "unknown msg %u", usMsgID);
                    break;
            }

            break;

        default:
            break;
    }
}
