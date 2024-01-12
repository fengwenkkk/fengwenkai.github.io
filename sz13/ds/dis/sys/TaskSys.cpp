#include "DISPublic.h"
#include "TaskSys.h"

#define CHECK_TIMER         TIMER10
#define EV_CHECK_TIMER      GET_TIMER_EV(CHECK_TIMER)

BOOL                    g_bNTPValid = TRUE;

extern CHAR             *g_szConfigFile;

BOOL IsNTPValid()
{
    return g_bNTPValid;
}

TaskSys::TaskSys(UINT16 usDispatchID):GBaseTask(MODULE_SYS, (CHAR*)"SYS", usDispatchID)
{
    memset(m_aucNTPAddr, 0, sizeof(m_aucNTPAddr));

    RegisterMsg(EV_HARDWARE_RESET_REQ);
}

BOOL TaskSys::Init()
{
    APP_BASIC_CFG_T stAPPBasicCfg;

    GetAPPBasicCfg(stAPPBasicCfg);
    if (stAPPBasicCfg.acNTPServerAddr[0] == '\0')
    {
        TaskLog(LOG_ERROR, "NTP server addr is null");
        return FALSE;
    }

    if (GET_INT(m_aucNTPAddr) == 0)
    {
        if (!gos_get_ip(stAPPBasicCfg.acNTPServerAddr, m_aucNTPAddr))
        {
            TaskLog(LOG_ERROR, "Invalid NTP addr %s", stAPPBasicCfg.acNTPServerAddr);
            return FALSE;
        }

        if (gos_is_local_ip(m_aucNTPAddr))
        {
            SET_IP(m_aucNTPAddr, 127, 0, 0, 1);
        }
    }

    NTPSync();

    SetLoopTimer(CHECK_TIMER, 10000);
    SetTaskStatus(TASK_STATUS_WORK);

    TaskLog(LOG_INFO, "Task sys init successful!");

    return TRUE;
}

VOID TaskSys::OnGetSysResetReq(VOID *pvMsg, UINT32 ulMsgLen)
{
    CHAR                        acProcName[256];
    GET_SYS_RESET_RSP           stRsp;

    stRsp.bRet = gos_get_proc_name(acProcName, FALSE);

    if (strlen(gos_get_root_path()) < sizeof(stRsp.acWorkDir))
    {
        strcpy(stRsp.acWorkDir, gos_get_root_path());
    }

    strcpy(stRsp.acProcName, acProcName);

    SendRsp(&stRsp, sizeof(stRsp));

    gos_sleep(1);

    exit(0);
}

VOID TaskSys::NTPSync()
{
    static UINT32   m_ulLastNTPSyncTime = 0;
    UINT32          ulCurrTime = gos_get_uptime();
    UINT8           aucLocalAddr[4] = {127,0,0,1};
    APP_BASIC_CFG_T stAPPBasicCfg;

    GetAPPBasicCfg(stAPPBasicCfg);
    if (stAPPBasicCfg.ulNTPSyncPeriod == 0)
    {
        return;
    }

    if (gos_is_local_ip(m_aucNTPAddr) ||
        GET_INT(m_aucNTPAddr) == GET_INT(aucLocalAddr))
    {
        return;
    }

    if (m_ulLastNTPSyncTime > 0 && (ulCurrTime-m_ulLastNTPSyncTime) < stAPPBasicCfg.ulNTPSyncPeriod)
    {
        return;
    }

    INT32   iTime = gos_get_ntp_time(m_aucNTPAddr, stAPPBasicCfg.ulNTPServerPort);

    if (iTime < 0)
    {
        TaskMsgLog(LOG_INFO, "connect to NTP server " IP_FMT " failed!", IP_ARG(m_aucNTPAddr));
        g_bNTPValid = FALSE;
        return;
    }

    g_bNTPValid = TRUE;

    INT32   iTimeDiff = abs((INT32)(iTime - gos_get_current_time()));

    if (iTimeDiff < 10)
    {
        m_ulLastNTPSyncTime = ulCurrTime;
        return;
    }

    TaskLog(LOG_INFO, "the difference between local time and ntp server time is %u", iTimeDiff);

    if (m_ulLastNTPSyncTime == 0)
    {
        TaskLog(LOG_INFO, "sync time with NTP server");
    }

    m_ulLastNTPSyncTime = ulCurrTime;
    if (!gos_set_localtime(iTime))
    {
        TaskLog(LOG_ERROR, "NTP set localtime failed!");

        return;
    }

    TaskLog(LOG_INFO, "sync time with NTP server succ");
}

VOID TaskSys::OnCheckTimer()
{
    NTPSync();
}

VOID TaskSys::TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
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

                case EV_SYS_RESET_REQ:
                    OnGetSysResetReq(pvMsg, ulMsgLen);
                    break;

                case EV_HARDWARE_RESET_REQ:
                    gos_reboot();
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

