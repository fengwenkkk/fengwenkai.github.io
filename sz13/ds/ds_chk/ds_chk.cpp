#include "g_include.h"
#include "GPublic.h"

#include "msg.h"
#include "ThreadCheck.h"

#ifdef _OSWIN32_
#include <new.h>
#else
#include <new>
#endif

CHAR            *g_szVersion = (CHAR*)"v2.1.0e";
CHAR            *g_szConfigFile = (CHAR*)"ds_chk.ini";

PLAT_CFG_T      g_stLocalCfg = {0};

BOOL LoadConfig(PLAT_CFG_T *pstCfg, UINT16 usDefaultServerPort=GOS_DEFAULT_SERVER_PORT)
{
    CHAR        acFile[512];
    GConfFile   *pConf;
    BOOL        bRet = FALSE;
    CHAR        *szValue = NULL;

    sprintf(acFile, "%s/%s", gos_get_root_path(), g_szConfigFile);

    if (!gos_file_exist(acFile))
    {
        GosLog(LOG_ERROR, "file %s does not exist", acFile);
        return FALSE;
    }

    pConf = new GConfFile(acFile);

    szValue = pConf->Get("msg_max_wait_time");
    if (szValue)
    {
        if (!gos_atou(szValue, &pstCfg->ulMsgMaxWaitTime))
        {
            pstCfg->ulMsgMaxWaitTime = 0;
        }

        if (pstCfg->ulMsgMaxWaitTime > 0 && pstCfg->ulMsgMaxWaitTime < 60)
        {
            pstCfg->ulMsgMaxWaitTime = 60;
        }
    }

    bRet = TRUE;

//end:
    delete pConf;
    return bRet;
}

#ifdef _OSWIN32_
static int NewHandler(size_t nSize)
#else
static void NewHandler()
#endif
{
    UINT32  ulNum = 4;
    VOID    *pvValue;

#ifdef _OSWIN32_
    GosLog(LOG_FATAL, "new %u bytes failed", nSize);
#endif

    for (UINT32 i=0; i<32; i++)
    {
        pvValue = malloc(ulNum);
        if (pvValue == NULL)
        {
            GosLog(LOG_FATAL, "malloc %u bytes failed", ulNum);
            getchar();
            break;
        }
        else
        {
            free(pvValue);
            ulNum *= 2;
        }
    }

#ifdef _OSWIN32_
    return 0;
#endif
}

static VOID SetNewHandler(VOID)
{
#ifdef _OSWIN32_
    _set_new_handler(NewHandler);
#else
    std::set_new_handler(NewHandler);
#endif
}

static VOID OnServerExit(VOID)
{
    Logger  *pLogger = GLogFactory::GetInstance()->GetDefaultLogger();

    pLogger->Flush();
}

#ifdef _OSWIN32_
static BOOL WINAPI ProcExitHandler(DWORD dwCtrlType)
{
    if (dwCtrlType == CTRL_C_EVENT ||
        dwCtrlType == CTRL_BREAK_EVENT )
    {
        return TRUE;
    }

    OnServerExit();

    return FALSE;
}
#else
static void ProcExitHandler(int sig)
{
    OnServerExit();
}
#endif

static VOID SetProcExitHandler(VOID)
{
#ifdef _OSWIN32_
    SetConsoleCtrlHandler(ProcExitHandler, TRUE);
#else
    signal(SIGPIPE, ProcExitHandler);
#endif
}

BOOL ServerInit()
{
    CHAR        acDir[260];

    sprintf(acDir, "%s/log", gos_get_root_path());
    if (!gos_file_exist(acDir))
    {
        if (!gos_create_dir(acDir))
        {
            GosLog(LOG_FATAL, "create dir %s failed!", acDir);
            return FALSE;
        }
    }

    GosLog(LOG_FATAL, "Init successful!");

    return TRUE;
}

int main(int argc, char **argv)
{
    if (gos_is_proc_running())
    {
        printf("current proc is already running\n");

        exit(-1);
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

    if (GosLoadPlatConf(&g_stLocalCfg, g_szConfigFile))
    {
        GosInitLog(&g_stLocalCfg.stLogCfg);
    }
    else
    {
        GosInitLog();
    }

    SetNewHandler();
    SetProcExitHandler();

    GosLog(LOG_FATAL, "ds_chk version: %s", g_szVersion);

    if (!LoadConfig(&g_stLocalCfg))
    {
        GosLog(LOG_FATAL, "load plat config file %s failed", g_szConfigFile);
        return -1;
    }

    if (!GosLoadPlatConf(&g_stLocalCfg, g_szConfigFile))
    {
        GosLog(LOG_FATAL, "load config file %s failed", g_szConfigFile);
        return -1;
    }

    CLUSTER_CFG_T       stClusterCfg = {0};

    stClusterCfg.ulDefaultStatus = CLUSTER_STATUS_STAND_ALONE;

    ///ÔØÈëÖ÷±¸ÅäÖÃ
    if (!LoadClusterConfig(g_szConfigFile, stClusterCfg))
    {
        GosLog(LOG_FAIL, "load cluster config failed");
        return FALSE;
    }

    if (!ServerInit())
    {
        return -1;
    }

    if (!StartThreadCheck(stClusterCfg))
    {
        return -1;
    }

    while(1)
    {
        gos_sleep(1);
    }

    return 0;
}
