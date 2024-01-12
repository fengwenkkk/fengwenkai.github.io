//---------------------------------------------------------------------------
#ifndef PROC_FUNC_H
#define PROC_FUNC_H
//---------------------------------------------------------------------------

#define MAX_PROC_NUM    16

typedef struct
{
    CHAR            acProcName[256]; // ��ض�������
    BOOL            bMonitorFlag;
    BOOL            bIsService;     // �Ƿ��Ƿ���ʽ    .

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
    CHAR            acProcCmd[256]; // ���������߿�ִ���ļ���
    CHAR            acExeFile[256];
}MON_INFO_T;

//---------------------------------------------------------------------------

VOID CheckProcessStatus(MON_INFO_T &stProcInfo);
BOOL LoadMonInfo(CHAR *szCfgFile, VECTOR<MON_INFO_T> &vMonInfo);

//---------------------------------------------------------------------------

#endif
