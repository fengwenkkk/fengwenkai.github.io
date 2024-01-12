#include "DISPublic.h"
#include "TaskTXMon.h"

#define CHECK_TIMER         TIMER10
#define EV_CHECK_TIMER      GET_TIMER_EV(CHECK_TIMER)

typedef struct
{
    UINT32      ulRecID;
    UINT32      ulBrdSeqID;
    UINT32      ulBrdLoopTime;
    UINT32      ulTrainUnitID;
    UINT32      ulSendUptime;
}REC_BRD_SEND_INFO1_T;

TaskTXMon::TaskTXMon(UINT16 usDispatchID, TELNET_INFO_T *pstInfo, UINT32 ulTrainUnitID, UINT32 ulTC)
    : GBaseTask(MODULE_APP + usDispatchID, "TX", usDispatchID)
{
    CHAR    acName[32];

    sprintf(acName, "Mon%u", usDispatchID);
    SetTaskName(acName);

    m_pDao = NULL;
    m_ulCellID = 0;

    m_pTelnet = new GTelnet(pstInfo);

    m_ulTrainUnitID = ulTrainUnitID;
    m_ulTC = ulTC;
}

BOOL TaskTXMon::Init()
{
    SetLoopTimer(CHECK_TIMER, 5*1000);

    TaskLog(LOG_INFO, "Task init successful!");

    SetTaskStatus(TASK_STATUS_WORK);

    return TRUE;
}

VOID TaskTXMon::OnServerDisconnectClient(UINT16 usClientID)
{
}

VOID TaskTXMon::OnCheckTimer()
{
    m_pTelnet->Command((CHAR*)"nvramcli get APP_STATUS_CellID\r\n", (CHAR*)"=");
    CHAR    *szValue = (CHAR*)m_pTelnet->GetValue();

    UINT32  ulCellID = 0;
    m_pTelnet->GetValue((CHAR*)"APP_STATUS_CellID=", (CHAR*)"", (CHAR*)"", &ulCellID);

    if (ulCellID == 0)
    {
        return;
    }

    if (ulCellID == m_ulCellID)
    {
        return;
    }

    m_ulCellID = ulCellID;

    TaskLog(LOG_INFO, "time = %u, train = %u, TC = %u, cell = %u", gos_get_current_time(), m_ulTrainUnitID, m_ulTC, ulCellID);
}

VOID TaskTXMon::TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
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

                default:
                    TaskLog(LOG_ERROR, "unknown msg %u", usMsgID);
                    break;
            }

            break;

        default:
            break;
    }
}
