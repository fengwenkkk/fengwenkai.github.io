#include "g_include.h"

#ifdef _OSWIN32_
#include <tlhelp32.h>
#include <psapi.h>
#else
#endif

#include "g_proc.h"

#ifdef _OSWIN32_
#ifndef _BCB_
#if _MSC_VER < 1400

typedef enum _SC_STATUS_TYPE
{
    SC_STATUS_PROCESS_INFO      = 0
} SC_STATUS_TYPE;

typedef struct _SERVICE_STATUS_PROCESS
{
    DWORD   dwServiceType;
    DWORD   dwCurrentState;
    DWORD   dwControlsAccepted;
    DWORD   dwWin32ExitCode;
    DWORD   dwServiceSpecificExitCode;
    DWORD   dwCheckPoint;
    DWORD   dwWaitHint;
    DWORD   dwProcessId;
    DWORD   dwServiceFlags;
} SERVICE_STATUS_PROCESS, *LPSERVICE_STATUS_PROCESS;
#endif
#endif

SC_HANDLE   g_schSCManager = NULL;
#endif

BOOL gos_get_proc_path(const CHAR *szRawProcName, CHAR *szProcPath)
{
#ifdef _OSWIN32_
    CHAR            *szFile;
    PROCESSENTRY32  pe32;
    HANDLE          hProcessSnap;

    *szProcPath = '\0';
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hProcessSnap == (HANDLE)-1)
    {
        return FALSE;
    }

    memset(&pe32, 0, sizeof(pe32));
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if(Process32First(hProcessSnap, &pe32))
    {
        do
        {
            szFile = strrchr(pe32.szExeFile, ' ');
            szFile = szFile?szFile+1:pe32.szExeFile;

            if (strcasecmp(szFile, szRawProcName) == 0)
            {
                HANDLE  h_Process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE, pe32.th32ProcessID);

                if (!GetModuleFileNameEx(h_Process, NULL, szProcPath, 256))
                {
                    continue;
                }

                INT32 iLen = strlen(szProcPath);
                --iLen;
                for ( ; iLen>0; iLen--)
                {
                    if (szProcPath[iLen] == '\\' ||
                        szProcPath[iLen] == '/' )
                    {
                        szProcPath[iLen] = '\0';
                        break;
                    }
                }

                break;
            }
        }while(Process32Next(hProcessSnap,&pe32));
    }

    CloseHandle(hProcessSnap);

    return *szProcPath != '\0';
#else
    *szProcPath = '\0';
    return FALSE;
#endif
}

CHAR *GetRawFile(const CHAR *szProcName)
{
    if (!szProcName || *szProcName == '\0')
    {
        return (CHAR*)"";
    }

    CHAR *szRawProcName = (CHAR*)szProcName + strlen(szProcName);
    for (; szRawProcName>=szProcName; szRawProcName--)
    {
        if (*szRawProcName == '/' ||
            *szRawProcName == '\\')
        {
            szRawProcName ++;
            return szRawProcName;
        }
    }

    return (CHAR*)szProcName;
}

VOID gos_start_process(const CHAR *szWorkDir, const CHAR *szProcCmd)
{
    CHAR    acCmd[1024];

#ifdef _OSWIN32_
    chdir(szWorkDir);

    sprintf(acCmd, "start %s", szProcCmd);
    gos_system(acCmd);

    chdir(gos_get_root_path());
#else
    chdir(szWorkDir);

    sprintf(acCmd, "%s 2>/dev/null &", szProcCmd);
    gos_system(acCmd);
#endif
}

//---------------------------------------------------------------------------
VOID gos_stop_process(const CHAR* szProcName)
{
    CHAR            *szRawProcName = GetRawFile(szProcName);

    if (!szRawProcName || *szRawProcName == '\0')
    {
        return;
    }

#ifdef _OSWIN32_
    CHAR            *szFile;
    PROCESSENTRY32  pe32;
    HANDLE          hProcessSnap;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hProcessSnap == (HANDLE)-1)
    {
        return;
    }

    memset(&pe32, 0, sizeof(pe32));
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if(Process32First(hProcessSnap, &pe32))
    {
        do
        {
            szFile = strrchr(pe32.szExeFile, ' ');
            szFile = szFile?szFile+1:pe32.szExeFile;

            if(strcasecmp(szFile, szRawProcName) == 0)
            {
                TerminateProcess(OpenProcess(PROCESS_ALL_ACCESS, 0, pe32.th32ProcessID), 0);
                //break;
            }
        }while(Process32Next(hProcessSnap,&pe32));
    }

    CloseHandle(hProcessSnap);

#else
    CHAR    acCmd[256];
    CHAR    acBuf[1024];
    CHAR    *szStart;
    CHAR    *szEnd;
    FILE    *fp;
    INT32   iProcID;
    UINT32  i;

#ifdef _BUSYBOX_
    sprintf(acCmd, "ps |grep %s | grep -v grep 2>/dev/null", szRawProcName);
#else
    sprintf(acCmd, "ps  -ef |grep %s | grep -v grep 2>/dev/null", szRawProcName);
#endif

    fp = popen(acCmd, "r");
    if (!fp)
    {
        return;
    }

    while(!feof(fp))
    {
        memset(acBuf, 0, sizeof(acBuf));
        if (fgets(acBuf, sizeof(acBuf), fp) == 0)
        {
            break;
        }

        for (i=0; acBuf[i]!='\0'; i++)
        {
            if (gos_isspace(acBuf[i]))
            {
                acBuf[i] = ' ';
            }
        }

        // root      7909  7908  0 03:33 pts/2    00:00:03 /home/fritt/inms/agent/nms_agent_v1.0.0 -subnet_id=16
        if (acBuf[sizeof(acBuf)-1] != 0)
        {
            continue;
        }

        szStart = gos_left_trim_string(acBuf);
        szStart = strchr(szStart, ' ');
        if (!szStart)
        {
            continue;
        }

        szStart = gos_left_trim_string(szStart+1);
        // szStart now like: 7909  7908  0 03:33 pts/2

        szEnd = strchr(szStart, ' ');
        if (!szEnd)
        {
            continue;
        }

        // szEnd now like:   7908  0 03:33 pts/2

        *szEnd = '\0';
        if (gos_atoi(szStart, &iProcID))
        {
            sprintf(acCmd, "kill -9 %d", iProcID);
            gos_system(acCmd);
        }
    }

    pclose(fp);

#endif
}

//---------------------------------------------------------------------------
BOOL gos_init_svcenv()
{
#ifdef _OSWIN32_
    // Get a handle to the SCM database.
    if (g_schSCManager)
    {
        return TRUE;
    }

    g_schSCManager = OpenSCManager(NULL,                    // local computer
                                   NULL,                    // servicesActive database
                                   SC_MANAGER_ALL_ACCESS);  // full access rights

    if (NULL == g_schSCManager)
    {
        //gos_write_log("OpenSCManager failed");
        return FALSE;
    }
#endif

    return TRUE;
}

//---------------------------------------------------------------------------
VOID FreeSvcEnv()
{
#ifdef _OSWIN32_
    if (g_schSCManager != NULL)
    {
        CloseServiceHandle(g_schSCManager);
    }
#endif
}

//---------------------------------------------------------------------------
VOID gos_start_service(const CHAR* szSvcName)
{
#ifdef _OSWIN32_
    SERVICE_STATUS          ssStatus;
    SC_HANDLE               schService;

    if (NULL == g_schSCManager)
    {
        return;
    }

    // Get a handle to the service.
    schService = OpenService(g_schSCManager,       // SCM database
                             szSvcName,            // name of service
                             SERVICE_ALL_ACCESS);  // full access

    if (schService == NULL)
    {
        return;
    }

    // Check the status in case the service is not stopped.

    if (!QueryServiceStatus(schService,  // handle to service
                            &ssStatus))  // size needed if buffer is too small
    {
        //gos_write_log("QueryServiceStatusEx failed");
        CloseServiceHandle(schService);
        return;
    }

    // Check if the service is already running. It would be possible
    // to stop the service here, but for simplicity this example just returns.

    if(ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
    {
        //gos_write_log("Cannot start the service because it is already running");
        CloseServiceHandle(schService);
        return;
    }

    // Wait for the service to stop before attempting to start it.

    while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
    {
        break;  //  don't wait the stop pending
    }

    // Attempt to start the service.
    if (!StartService(schService,  // handle to service
                      0,           // number of arguments
                      NULL))       // no arguments
    {
        //gos_write_log("StartService failed");
        CloseServiceHandle(schService);
        return;
    }

    // Check the status until the service is no longer start pending.

    if (!QueryServiceStatus(schService,  // handle to service
                            &ssStatus))  // if buffer too small
    {
        CloseServiceHandle(schService);
        return;
    }

    // Save the tick count and initial checkpoint.

    while (ssStatus.dwCurrentState == SERVICE_START_PENDING)
    {
        break; // don't wait the start pending
    }

    CloseServiceHandle(schService);

#else
    CHAR    acCmd[256];

    sprintf(acCmd, "service %s restart", szSvcName);
    gos_system(acCmd);
#endif
}
//---------------------------------------------------------------------------
BOOL gos_stop_service(const CHAR* szSvcName)
{
#ifdef _OSWIN32_
    SERVICE_STATUS          ssStatus;
    SC_HANDLE               schService;

    if (NULL == g_schSCManager)
    {
        return FALSE;
    }

    // Get a handle to the service.
    schService = OpenService(g_schSCManager,       // SCM database
                             szSvcName,            // name of service
                             SERVICE_ALL_ACCESS);  // full access

    if (schService == NULL)
    {
        return FALSE;
    }

    // Check the status in case the service is not stopped.
    if (!QueryServiceStatus(schService,  // handle to service
                            &ssStatus))  // size needed if buffer is too small
    {
        CloseServiceHandle(schService);
        return FALSE;
    }

    // Check if the service is already running. It would be possible
    // to stop the service here, but for simplicity this example just returns.
    if(ssStatus.dwCurrentState == SERVICE_STOPPED || ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
    {
        CloseServiceHandle(schService);
        return TRUE;
    }

    // Attempt to stop the service.
    SERVICE_STATUS  CurrentServiceStatus;
    if (!ControlService(schService, SERVICE_CONTROL_STOP, &CurrentServiceStatus))
    {
        //gos_write_log("StartService failed");
        CloseServiceHandle(schService);
        return FALSE;
    }

    CloseServiceHandle(schService);

#else
    CHAR    acCmd[256];

    sprintf(acCmd, "service %s stop", szSvcName);
    gos_system(acCmd);
#endif

    return TRUE;
}
//---------------------------------------------------------------------------
UINT32 gos_query_svcstatus(const CHAR* szSvcName)
{
#ifdef _OSWIN32_
    SERVICE_STATUS          ssStatus;
    SC_HANDLE               schService;

    if (NULL == g_schSCManager)
    {
        return SERVICE_UNKNOW;
    }

    // Get a handle to the service.
    schService = OpenService(g_schSCManager, szSvcName, SERVICE_QUERY_STATUS);
    if (schService == NULL)
    {
        return SERVICE_UNKNOW;
    }

    // Check the status in case the service is not stopped.
    if (!QueryServiceStatus(schService,  // handle to service
                            &ssStatus))  // size needed if buffer is too small
    {
        //gos_write_log("QueryServiceStatusEx failed");
        CloseServiceHandle(schService);
        return SERVICE_UNKNOW;
    }

    CloseServiceHandle(schService);
    return ssStatus.dwCurrentState;

#else
    CHAR    acCmd[256];
    CHAR    acBuf[1024];
    CHAR    *szLine;
    FILE    *fp;

    sprintf(acCmd, "service %s status 2>/dev/null", szSvcName);
//  system(acCmd);

    fp = popen(acCmd, "r");
    if (!fp)
    {
        return SERVICE_UNKNOW;
    }

    while(!feof(fp))
    {
        memset(acBuf, 0, sizeof(acBuf));
        if (fgets(acBuf, sizeof(acBuf), fp) == 0)
        {
            break;
        }

        if (acBuf[sizeof(acBuf)-1] != 0)
        {
            continue;
        }

        if (strstr(acBuf, "running"))
        {
            pclose(fp);
            return SERVICE_RUNNING;
        }
        else if (strstr(acBuf, "exited") ||
                 strstr(acBuf, "dead"))
        {
            pclose(fp);
            return SERVICE_STOPPED;
        }
    }

    pclose(fp);

    return SERVICE_UNKNOW;
#endif
}

// 判断进程是否存在，如果进程PID非0，过滤掉该进程
BOOL gos_is_proc_exists(const CHAR *szProcName, UINT32 ulPID)
{
    CHAR    *szRawProcName = GetRawFile(szProcName);
    BOOL    bExist = FALSE;

    if (!szRawProcName || *szRawProcName == '\0')
    {
        return FALSE;
    }

#ifdef _OSWIN32_

    HANDLE hProcessSnap;
    HANDLE hProcess;
    PROCESSENTRY32 pe32;

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( hProcessSnap == INVALID_HANDLE_VALUE )
    {
        return FALSE;
    }

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof( PROCESSENTRY32 );

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if( !Process32First( hProcessSnap, &pe32 ) )
    {
        CloseHandle( hProcessSnap );          // clean the snapshot object
        return FALSE;
    }

    do
    {
        // Retrieve the priority class.
        hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID );
        if( hProcess != NULL )
        {
            CloseHandle( hProcess );
        }

        if (strcasecmp(szRawProcName, pe32.szExeFile) != 0)
        {
            continue;
        }

        // 如果PID大于0，并且与该进程ID相同（就是调用者的PID），则跳过
        if (ulPID > 0 && ulPID == pe32.th32ProcessID)
        {
            continue;
        }

        bExist = TRUE;
        break;
    } while( Process32Next( hProcessSnap, &pe32 ) );

    CloseHandle( hProcessSnap );
    return bExist;

#else
    CHAR    acCmd[256];
    CHAR    acBuf[1024];
    CHAR    *szLine;
    FILE    *fp;
    CHAR    *szTmp;
    CHAR    acPID[16];

    sprintf(acPID, "%u", ulPID);
#ifdef _BUSYBOX_
    sprintf(acCmd, "ps |grep %s | grep -v grep 2>/dev/null", szRawProcName);
#else
    sprintf(acCmd, "ps -ef |grep %s | grep -v grep 2>/dev/null", szRawProcName);
#endif

    fp = popen(acCmd, "r");
    if (!fp)
    {
        return FALSE;
    }

    while(!feof(fp))
    {
        memset(acBuf, 0, sizeof(acBuf));
        if (fgets(acBuf, sizeof(acBuf), fp) == 0)
        {
            break;
        }

        if (acBuf[sizeof(acBuf)-1] != 0)
        {
            continue;
        }

        szLine = strstr(acBuf, szRawProcName);
        if (!szLine)
        {
            continue;
        }

        // 如果PID大于0，并且与该进程ID相同（就是调用者的PID），则跳过
        if (ulPID > 0 && strstr(acBuf, acPID) != NULL)
        {
            continue;
        }

        while(1)
        {
            if (gos_isspace(*(szLine-1)) ||
                *(szLine-1) == '/')
            {
                szTmp = szLine + strlen(szRawProcName);
                if (*szTmp == '\0' ||
                    gos_isspace(*szTmp) )
                {
                    bExist = TRUE;
                    goto end;
                }
            }

            szLine += strlen(szRawProcName);
            szLine = strstr(szLine, szRawProcName);
            if (!szLine)
            {
                break;
            }
        }
    }

end:
    pclose(fp);

    return bExist;
#endif
}

/**
 * @brief           判断本进程同名进程是否存在，在本进程中调用
 * @return          BOOL    返回TRUE, 则为本同名程序在运行，反之返回FALSE
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @par 示例:
 * @code
 * if(gos_is_proc_running())
 * {
 *      std::cout << "本程序正在运行！" << std::endl;
 * }
 * @endcode
 * @note
 */
BOOL gos_is_proc_running(VOID)
{
    CHAR    acProcName[256];
    UINT32  ulPID = gos_get_pid();

    gos_get_proc_name(acProcName, TRUE);

    return gos_is_proc_exists(acProcName, ulPID);
}

