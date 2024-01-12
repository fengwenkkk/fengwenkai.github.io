#ifndef G_PROC_H
#define G_PROC_H

#define GOS_RUN_EXCEPTION               0

#define SERVICE_UNKNOW                  0xffffffff

#ifndef _OSWIN32_
#define SERVICE_STOPPED                 0x00000001
#define SERVICE_START_PENDING           0x00000002
#define SERVICE_STOP_PENDING            0x00000003
#define SERVICE_RUNNING                 0x00000004
#define SERVICE_CONTINUE_PENDING        0x00000005
#define SERVICE_PAUSE_PENDING           0x00000006
#define SERVICE_PAUSED                  0x00000007
#endif

typedef enum
{
    APP_STATUS_NORMAL       = 0x01,
    APP_STATUS_ERROR        = 0x02,
    APP_STATUS_WARNING      = 0x04,
    APP_STATUS_NOAUTOSTART  = 0x08,     //不自动重启
    APP_STATUS_NOMON        = 0x16,     //不监控该服务
}APP_STATUS_E;

BOOL gos_get_proc_path(const CHAR *szRawProcName, CHAR *szProcPath);

VOID gos_start_process(const CHAR *szWorkDir, const CHAR *szProcCmd);
VOID gos_stop_process(const CHAR* szProcCmd);
BOOL gos_is_proc_exists(const CHAR *szProcName, UINT32 ulPID);

BOOL gos_init_svcenv();
VOID gos_free_svcenv();

BOOL gos_stop_service(const CHAR* szSvcName);
VOID gos_start_service(const CHAR* szSvcName);

UINT32 gos_query_svcstatus(const CHAR* szSvcName);

#endif

