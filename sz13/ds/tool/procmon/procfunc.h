//---------------------------------------------------------------------------
#ifndef PROC_FUNC_H
#define PROC_FUNC_H
//---------------------------------------------------------------------------

#define MAX_PROC_NUM    16

typedef struct
{
    CHAR            acProcName[256]; // 监控对象名称
    BOOL            bMonitorFlag;
    BOOL            bIsService;     // 是否是服务方式    .

    UINT32          ulCheckIntev;
    UINT32          ulLastCheckTime;

    UINT32          ulFailedTime;
    UINT32          ulMaxFailedTime;

    UINT32          ulMaxCpuUsage;
    UINT32          ulLastCheckCpuUsageTime;
    UINT32          ulCheckCpuUsagePeriod;
    UINT32          aulCpuUsage[10];

    CHAR            acWorkDir[256];
    CHAR            acMonFile[256];
    CHAR            acProcCmd[256]; // 服务名或者可执行文件名
    CHAR            acExeFile[256];
}MON_INFO_T;

//---------------------------------------------------------------------------

VOID CheckProcessStatus(MON_INFO_T &stProcInfo);
BOOL LoadMonInfo(CHAR *szCfgFile, VECTOR<MON_INFO_T> &vMonInfo);

//---------------------------------------------------------------------------

#endif
