#include "g_include.h"
#include "GPublic.h"
#include "procfunc.h"

CHAR *GetRootPath(VOID)
{
    static BOOL bInit = FALSE;
    static CHAR acRootPath[512];

    if (!bInit)
    {
#ifdef _OSWIN32_
        if (GetModuleFileName(NULL, acRootPath, sizeof(acRootPath)-1) == 0)
        {
            acRootPath[0] = '\0';
        }

        CHAR *szTmp = acRootPath + strlen(acRootPath);
        for (; szTmp>=acRootPath; szTmp--)
        {
            if (*szTmp == '/' ||
                *szTmp == '\\')
            {
                *szTmp = '\0';
                break;
            }
        }
#else
        if (getcwd(acRootPath, sizeof(acRootPath)-1) == NULL )
        {
            acRootPath[0] = '\0';
        }
#endif

        bInit = TRUE;
    }

    return acRootPath;
}

/*
[mysql]
service_name = mysql
check_intev = 3
max_failed_time = 5
exe_file = mysqld.exe
cmd = mysql

[dis]
check_intev = 3
max_failed_time = 5
work_dir = ./dis
exe_file = dis.exe
#cmd = dis.exe
*/
BOOL LoadMonInfo(CHAR *szCfgFile, VECTOR<MON_INFO_T> &vMonInfo)
{
    CHAR        acFile[512];
    INT32       iIsService;
    INT32       iCheckIntev;
    INT32       iMaxFailedTime;
    CHAR        *szWorkDir;
    CHAR        *szProcCmd;
    CHAR        *szMonFile;
    CHAR        *szExeFile;
    BOOL        bRet = FALSE;
    MON_INFO_T  stInfo;

    sprintf(acFile, "%s/%s", gos_get_root_path(), szCfgFile);

    printf("load file %s\n", acFile);
    if (!gos_file_exist(acFile))
    {
        GosLog(LOG_ERROR, "config file %s is not exist", acFile);
        return FALSE;
    }

    VECTOR<UINT32>  vGroupID;
    GConfFile       Conf(acFile);
    CHAR            acGroupName[128];

    Conf.GetGroupIDList(vGroupID);

    for (UINT32 i=0; i<vGroupID.size(); i++)
    {
        if (!Conf.GetGroupName(vGroupID[i], acGroupName, sizeof(acGroupName)))
        {
            GosLog(LOG_ERROR, "get group name of %u failed", vGroupID[i]);
            continue;
        }

        memset(&stInfo, 0, sizeof(stInfo));

        szProcCmd = Conf.Get(acGroupName, "service_name");
        if (IS_NULL_STRING(szProcCmd))
        {
            iIsService = 0;
        }
        else
        {
            iIsService = 1;
        }

        iCheckIntev = -1;
        Conf.Get(acGroupName, "check_intev", &iCheckIntev);
        if (iCheckIntev < 0)
        {
            GosLog(LOG_ERROR, "invalid CheckIntev: %d", iCheckIntev);
            goto fail;
        }

        iMaxFailedTime = -1;
        Conf.Get(acGroupName, "max_failed_time", &iMaxFailedTime);
        if (iMaxFailedTime <= 0)
        {
            GosLog(LOG_ERROR, "invalid MaxFailedTime: %d", iMaxFailedTime);
            goto fail;
        }

        if (iIsService == 0)
        {
            szWorkDir = Conf.Get(acGroupName, "work_dir");
            if (IS_NULL_STRING(szWorkDir))
            {
                GosLog(LOG_ERROR, "miss WorkDir!");
                goto fail;
            }

            szExeFile = Conf.Get(acGroupName, "exe_file");
            if (IS_NULL_STRING(szExeFile))
            {
                GosLog(LOG_ERROR, "miss ExeFile!");
                goto fail;
            }

            szProcCmd = Conf.Get(acGroupName, "proc_cmd");
            if (IS_NULL_STRING(szProcCmd))
            {
                szProcCmd = szExeFile;
            }

            szMonFile = NULL;
        }
        else
        {
            szWorkDir = NULL;
            szExeFile = NULL;
            szMonFile = NULL;
        }


        strcpy(stInfo.acProcName, acGroupName);
        stInfo.bMonitorFlag = (iCheckIntev > 0);
        stInfo.bIsService = iIsService;

        stInfo.ulCheckIntev = iCheckIntev;
        stInfo.ulLastCheckTime = (UINT32)(-iCheckIntev-1);

        stInfo.ulFailedTime = 0;
        stInfo.ulMaxFailedTime = iMaxFailedTime;

        if (szWorkDir && *szWorkDir == '/')
        {
            szWorkDir++;
        }

        if (szExeFile && *szExeFile == '/')
        {
            szExeFile++;
        }

        if (*szProcCmd == '/')
        {
            szProcCmd++;
        }

        if (szMonFile && *szMonFile == '/')
        {
            szMonFile++;
        }

        if (!iIsService)
        {
            snprintf(stInfo.acWorkDir, sizeof(stInfo.acWorkDir)-1, "%s/%s", gos_get_root_path(), szWorkDir);
            gos_format_path(stInfo.acWorkDir);

            if (szMonFile && *szMonFile != '\0')
            {
                snprintf(stInfo.acMonFile, sizeof(stInfo.acMonFile)-1, "%s/%s", stInfo.acWorkDir, szMonFile);
            }
            snprintf(stInfo.acExeFile, sizeof(stInfo.acExeFile)-1, "%s", szExeFile);
            snprintf(stInfo.acProcCmd, sizeof(stInfo.acProcCmd)-1, "%s/%s", stInfo.acWorkDir, szProcCmd);
        }
        else
        {
            strncpy(stInfo.acProcCmd, szProcCmd, sizeof(stInfo.acProcCmd)-1);
        }

        //
        stInfo.ulMaxCpuUsage = 80;
        stInfo.ulCheckCpuUsagePeriod = 0;
        Conf.Get(acGroupName, "max_cpu_usage", stInfo.ulMaxCpuUsage, &stInfo.ulMaxCpuUsage);
        Conf.Get(acGroupName, "check_cpu_usage_time", stInfo.ulCheckCpuUsagePeriod, &stInfo.ulCheckCpuUsagePeriod);

        if (stInfo.ulCheckCpuUsagePeriod < 60)
        {
            stInfo.ulCheckCpuUsagePeriod = 60;
        }

        stInfo.ulCheckCpuUsagePeriod /= ARRAY_SIZE(stInfo.aulCpuUsage);

        gos_format_path(stInfo.acExeFile);
        gos_format_path(stInfo.acProcCmd);

        vMonInfo.push_back(stInfo);

        if (iIsService == 0)
        {
            GosLog(LOG_INFO, "Monitoring %s: %s", stInfo.acProcName, stInfo.acProcCmd);
        }
        else
        {
            GosLog(LOG_INFO, "Monitoring service %s: %s", stInfo.acProcName, stInfo.acProcCmd);
        }
    }

    bRet = TRUE;

fail:

    return bRet;
}

//---------------------------------------------------------------------------
VOID RestartProcess(MON_INFO_T &stProcInfo)
{
    UINT32  ulSvcStatus;
    CHAR    acPath[512];

    if (stProcInfo.ulFailedTime < stProcInfo.ulMaxFailedTime)
    {
        return;
    }

    while(1)
    {
        if (stProcInfo.bIsService)
        {
            ulSvcStatus = gos_query_svcstatus(stProcInfo.acProcCmd);
            if (SERVICE_RUNNING == ulSvcStatus )
            {
                return;
            }

        //    gos_stop_service(stProcInfo.acProcCmd);
            if (SERVICE_STOPPED == ulSvcStatus )
            {
                break;
            }
        }
        else
        {
            if (!gos_is_proc_exists(stProcInfo.acExeFile, 0))
            {
                break;
            }

            gos_stop_process(stProcInfo.acExeFile);
        }

        gos_sleep_1ms(100);
    }

    if (stProcInfo.bIsService)
    {
        GosLog(LOG_INFO, "RestartProcess: service %s has stopped!", stProcInfo.acProcName);
        gos_start_service(stProcInfo.acProcCmd);
        GosLog(LOG_FATAL, "RestartProcess: service %s is starting!", stProcInfo.acProcCmd);
    }
    else
    {
        GosLog(LOG_INFO, "RestartProcess: process %s has stopped!", stProcInfo.acProcName);
        sprintf(acPath, "%s/%s", stProcInfo.acWorkDir, stProcInfo.acExeFile);
        if (gos_file_exist(acPath))
        {
            gos_start_process(stProcInfo.acWorkDir, stProcInfo.acProcCmd);
            GosLog(LOG_FATAL, "RestartProcess: process %s is starting!", stProcInfo.acProcCmd);
        }
    }

    stProcInfo.ulFailedTime = 0;
}

INT32 GetPID(CHAR *szExeName)
{
#ifdef _OSWIN32_
    return -1;
#else
    FILE            *fp = NULL;
    CHAR            acBuf[64];
    UINT32          ulPID = 0;
    CHAR            acCmd[256];

    sprintf(acCmd, "pgrep %s -u root", szExeName);
    fp = fopen(acCmd, "r");
    if (NULL == fp)
    {
        return -1;
    }

    memset(acBuf, 0, sizeof(acBuf));
    if (!fgets(acBuf, sizeof(acBuf)-1, fp))
    {
        fclose(fp);
        return -1;
    }

    fclose(fp);
    if (!gos_atou(acBuf, &ulPID))
    {
        return -1;
    }

    return (INT32)ulPID;
#endif
}

INT32 GetCpuUsage(CHAR *szExeFile)
{
#ifdef _OSWIN32_
    return -1;
#else
    FILE            *fp = NULL;
    CHAR            acBuf[64];
    DOUBLE          dCpuUsage;
    CHAR            acCmd[256];
    INT32           iPID = GetPID(szExeFile);

    if (iPID <= 0)
    {
        return -1;
    }

    sprintf(acCmd, "top -b -n 1 -p %d | awk -v pid=%d '{if ($1 == pid) print $9}'", iPID, iPID);
    fp = fopen(acCmd, "r");
    if (NULL == fp)
    {
        return -1;
    }

    memset(acBuf, 0, sizeof(acBuf));
    if (!fgets(acBuf, sizeof(acBuf)-1, fp))
    {
        fclose(fp);
        return -1;
    }

    fclose(fp);
    if (!gos_atof(acBuf, &dCpuUsage))
    {
        return -1;
    }

    return (INT32)dCpuUsage;
#endif
}

VOID CheckServiceStatus(MON_INFO_T &stProcInfo)
{
    UINT32 ulStatus = gos_query_svcstatus(stProcInfo.acProcCmd);

    /*INT32 iCpuUsage = GetCpuUsage(stProcInfo.acExeFile);

    if (iCpuUsage < 0)
    {
        iCpuUsage = 0;
    }

    memmove(&stProcInfo.aulCpuUsage[1], stProcInfo.aulCpuUsage, sizeof(stProcInfo.aulCpuUsage)-sizeof(stProcInfo.aulCpuUsage[0]));
    stProcInfo.aulCpuUsage[0] = GetCpuUsage(stProcInfo.acExeFile); */

    if (SERVICE_RUNNING == ulStatus)
    {
        stProcInfo.ulFailedTime = 0;
        return;
    }

 /*   if (SERVICE_UNKNOW == ulStatus)
    {
        return;
    } */

    if (SERVICE_STOPPED == ulStatus)
    {
        stProcInfo.ulFailedTime = stProcInfo.ulMaxFailedTime+1;
    }
    else
    {
        stProcInfo.ulFailedTime++;
    }

    RestartProcess(stProcInfo);
    //GosLog(LOG_ERROR, "CheckServiceStatus: check %s failed!", stProcInfo.acProcName);

    return;
}

VOID CheckCpuUsage(MON_INFO_T &stProcInfo)
{
    INT32   iCpuUsage = GetCpuUsage(stProcInfo.acExeFile);

    if (iCpuUsage < 0)
    {
        iCpuUsage = 0;
    }

    memmove(&stProcInfo.aulCpuUsage[1], stProcInfo.aulCpuUsage, sizeof(stProcInfo.aulCpuUsage)-sizeof(stProcInfo.aulCpuUsage[0]));
    stProcInfo.aulCpuUsage[0] = GetCpuUsage(stProcInfo.acExeFile);

    UINT32      ulCurrCpuUsage = 0;

    for (UINT32 i=0; i<ARRAY_SIZE(stProcInfo.aulCpuUsage); i++)
    {
        ulCurrCpuUsage += stProcInfo.aulCpuUsage[i];
    }

     ulCurrCpuUsage /= ARRAY_SIZE(stProcInfo.aulCpuUsage);
     if (ulCurrCpuUsage < stProcInfo.ulMaxCpuUsage)
     {
         return;
     }

    if (stProcInfo.bIsService)
    {
        GosLog(LOG_INFO, "service %s cpu(%u) is too high, restart service", stProcInfo.acProcName, ulCurrCpuUsage);
        gos_stop_service(stProcInfo.acProcCmd);
    }
    else
    {
        GosLog(LOG_INFO, "process %s cpu(%u) is too high, restart process", stProcInfo.acProcName, ulCurrCpuUsage);
        gos_stop_process(stProcInfo.acExeFile);
    }
}

//---------------------------------------------------------------------------
VOID CheckProcessStatus(MON_INFO_T &stProcInfo)
{
    INT32   iFlag = 0;
    FILE    *fp = NULL;
    UINT32  ulCurrTime = gos_get_uptime();

    if (stProcInfo.acProcName[0] == '\0' ||
        stProcInfo.ulCheckIntev == 0)
    {
        return;
    }

    if (ulCurrTime-stProcInfo.ulLastCheckTime < stProcInfo.ulCheckIntev)
    {
        return;
    }

    GosLog(LOG_DETAIL, "check %s %s", stProcInfo.acProcName, stProcInfo.acProcCmd);
    stProcInfo.ulLastCheckTime = ulCurrTime;

    CheckCpuUsage(stProcInfo);

    if (stProcInfo.bIsService)
    {
        CheckServiceStatus(stProcInfo);
        return;
    }

    if (!gos_is_proc_exists(stProcInfo.acExeFile, 0))
    {
        stProcInfo.ulFailedTime = stProcInfo.ulMaxFailedTime;
        RestartProcess(stProcInfo);
    }

    if (stProcInfo.acMonFile[0] == '\0')
    {
        return;
    }

    fp = fopen(stProcInfo.acMonFile, "rb+");
    if (NULL == fp)
    {
        stProcInfo.ulFailedTime++;
        GosLog(LOG_ERROR, "CheckProcessStatus: open file %s failed!", stProcInfo.acMonFile);
        return;
    }

    size_t ulLen = fread(&iFlag, 1, sizeof(iFlag), fp);

    if(GOS_RUN_EXCEPTION == iFlag)
    {
        fclose(fp);

        stProcInfo.ulFailedTime++;
        RestartProcess(stProcInfo);
        //GosLog(LOG_ERROR, "CheckProcessStatus: check %s failed!", stProcInfo.acProcName);
        return ;
    }
    else
    {
        char acBuf[64];

        stProcInfo.ulFailedTime = 0;
        memset(acBuf, 0, sizeof(acBuf));
        fseek(fp, 0, SEEK_SET);
        fwrite(acBuf, 1, sizeof(acBuf), fp);
        fclose(fp);
    }

    return;
}
//---------------------------------------------------------------------------
