#include "g_include.h"
#include "GPublic.h"
#include "procfunc.h"

VECTOR<MON_INFO_T>      g_vMonInfo;
BOOL                    g_bRunning = TRUE;
UINT32                  g_ulPrintCheckTime = 60;

VOID StartMonitor()
{
    UINT32  i;
    CHAR    *szCfgFile = (CHAR*)"procmon.ini";
    UINT32  ulTime = gos_get_uptime();

    printf("procmon is start...\n");

    PLAT_CFG_T stPlatCfg = {0};

    stPlatCfg.bMultiInst = FALSE;
    stPlatCfg.stLogCfg.ulLogLevel = LOG_INFO;
    stPlatCfg.stLogCfg.bLogToStdout = TRUE;
    stPlatCfg.stLogCfg.bLogToFile = TRUE;
    stPlatCfg.stLogCfg.ulLogFileNum = 10;
    stPlatCfg.stLogCfg.ulLogFileSize = 4096;
    stPlatCfg.stLogCfg.ulLogBufTime = 0;
    stPlatCfg.ulMaxSysLogNum = 0;

    GosInitLog(&stPlatCfg.stLogCfg);

    /*
    sprintf(acFile, "%s/../plat_info.conf", gos_get_root_path());

    if (!gos_file_exist(acFile))
    {
        GosLog(LOG_FATAL, "file %s not exist", acFile);
        return;
    }

    pConf = new GConfFile(acFile);
    if (!pConf)
    {
        GosLog(LOG_FATAL, "new GConFile failed when %s", acFile);
        return;
    }

    pConf->Get("subnet_id", &iSubNetID);

    if (!szCfgFile)
    {
        CHAR    acCfgFile[512];

        if (iSubNetID < MIN_REMOTE_SERVER_ID)
        {
            sprintf(acCfgFile, "procmon_main.xml");
        }
        else
        {
            sprintf(acCfgFile, "procmon.xml");
        }

        szCfgFile = acCfgFile;
    }
    */

    if (!LoadMonInfo(szCfgFile, g_vMonInfo))
    {
        GosLog(LOG_ERROR, "Load monitor info failed");

        return;
    }

    GosLog(LOG_INFO, "Loading monitor info successful");

    gos_init_svcenv();

    g_bRunning = TRUE;
    while(g_bRunning)
    {
        for (i=0; i<g_vMonInfo.size(); i++)
        {
            CheckProcessStatus(g_vMonInfo[i]);
        }

        gos_sleep(1);

        if ((ulTime+g_ulPrintCheckTime) < gos_get_uptime())
        {
            GosLog(LOG_INFO, "checking ...");
            ulTime = gos_get_uptime();
        }
    }
}

VOID StopMonitor()
{
    g_bRunning = FALSE;
}

#ifndef _BCB_
int main(int argc, char **argv)
{
    if (gos_is_proc_running())
    {
        printf("proc is already running\n");
        return -1;
    }

    BOOL    bLogToStdout = FALSE;

    for (int i=1; i<argc; i++)
    {
        if (strcmp(argv[i], "-l") == 0)
        {
            bLogToStdout = TRUE;
            break;
        }
    }

    gos_run_daemon(bLogToStdout);

    StartMonitor();
}

#else
GOS_THREAD_RET mon_thread(VOID *pvData)
{
    StartMonitor();

    return 0;
}

VOID StartMonitorThread()
{
    gos_create_thread(mon_thread, 64*1024, NULL);
}

#endif

