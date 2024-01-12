
#include "g_include.h"
#include "pdb_app.h"
//#include "oci.h"

PDBEnvBase  *g_pEnv = NULL;
PDBApp      *g_pApp;

#define  BIND_IN_BY_POS(pData, FieldType, FieldLen, ulInoutType) \
if (PDB_SUCCESS != pDbConn->BindByPos(FieldType, pData, FieldLen, ulInoutType, NULL)) \
{ \
    PDBLog(LOG_ERROR, " PDBApp::AnalyseSql bind in by pos fail "); \
    { \
        return NULL; \
    } \
}

void test_qry_perf()
{
    INT32      aiNum[4];
    FLOAT       adValue[4];
    INT32       iRowCount = 0;
    CHAR        acUser[32];
    CHAR        acObject[256];
    PDBRsltParam    Params;

    // AlarmSn,AlarmID, NeID, AlarmGenerateTime, AlarmInfo, AlarmConfirm, AlarmConfirmTime, AlarmConfirmInfo
/*    BOOL bRet = g_pApp->UpdateDirect("insert into perfdata values(%u,%u,%u,%u,%s,%u,%u,%s)",
                    1,2,3,4,"alarm 1", 5, 6, "auto"); */

    /*Params.Bind(&aiNum[0]);
    Params.Bind(&aiNum[1]);
    Params.Bind(&aiNum[2]);
    Params.Bind(&adValue[0]);
    Params.Bind(&adValue[1]);
    Params.Bind(&adValue[2]); */

//    Params.Bind(&adValue[3]);

    Params.Bind(acUser, sizeof(acUser));
    Params.Bind(acObject, sizeof(acObject));

    iRowCount = g_pApp->Query("select OperUser, operobject from operlog", Params);
    //iRowCount = g_pApp->Query("select PerfTime, NeID, CounterNum, Counter1,Counter2,Counter3 from perfdata", Params);
    if (iRowCount < 0)
    {
        return;
    }

    UINT32   ulTime = GetTickCount();

    while (PDB_CONTINUE == g_pApp->RowFetch())
    {
        /*
        for (UINT32 i=0; i<7; i++)
        {
            printf("iLen = %d isNull=%d\n", Params.GetLen(i), Params.IsNull(i));
        }*/

        iRowCount++;
    }

    printf("time = %d\n", GetTickCount() -ulTime);
    return;
}

void test_ins_perf()
{
    INT32       aiNum[4];
    FLOAT       adValue[4];
    PDBParam    Params;
    UINT32      ulTime = GetTickCount();

    INT32 iNum = 0;
    // AlarmSn,AlarmID, NeID, AlarmGenerateTime, AlarmInfo, AlarmConfirm, AlarmConfirmTime, AlarmConfirmInfo

    g_pApp->TransBegin();
    for (UINT32 i=1; i<=1000; i++)
    {
        aiNum[0] = i;
        aiNum[1] = 1;
        aiNum[2] = 3;
        adValue[0] = i;
        adValue[1] = i+1;
        adValue[2] = i+2;

        /*
        BIND_IN_BY_POS(&aiNum[0], PDBT_UINT32, sizeof(UINT32), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&aiNum[1], PDBT_UINT32, sizeof(UINT32), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&aiNum[2], PDBT_UINT32, sizeof(UINT32), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&adValue[0], PDBT_FLOAT, sizeof(FLOAT), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&adValue[1], PDBT_FLOAT, sizeof(FLOAT), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&adValue[2], PDBT_FLOAT, sizeof(FLOAT), PDB_PARAM_INPUT); */

        Params.Clear();
        Params.Bind(&aiNum[0]);
        Params.Bind(&aiNum[1]);
        Params.Bind(&aiNum[2]);
        Params.Bind(&adValue[0]);
        Params.Bind(&adValue[1]);
        Params.Bind(&adValue[2]);

        iNum += g_pApp->Update("insert into perfdata(PerfTime, NeID, CounterNum, Counter1,Counter2,Counter3) values(?,?,?,?,?,?)",
                                Params);

       /* BOOL bRet = g_pApp->UpdateDirect("insert into perfdata(PerfTime, NeID, CounterNum, Counter1,Counter2,Counter3) values(%u,%u,%u,%f,%f,%f)",
                                         i,1,3,1,2,3); */

        if (MOD(i, 1000) == 0)
        {
            g_pApp->TransCommit();
            printf("time = %d\n", GetTickCount() - ulTime);
            ulTime = GetTickCount();

            g_pApp->TransBegin();
        }
    }

    return;
}

void test_ins_alarm()
{
    UINT32      aiNum[16];
    CHAR        acAlarmInfo[128];
    CHAR        acConfirmInfo[128];
    CHAR        acClearInfo[128];
    INT32       iRowCount = 0;
    static CHAR     acSQL[1024*32];
    UINT32          ulLen = 0;
    UINT32          i,j;
    PDBParam        Params;
    // AlarmSn,AlarmID, NeID, AlarmGenerateTime, AlarmInfo, AlarmConfirm, AlarmConfirmTime, AlarmConfirmInfo

    Params.Bind(&aiNum[0]);
    Params.Bind(&aiNum[1]);
    Params.Bind(&aiNum[2]);
    Params.Bind(&aiNum[3]);
    Params.Bind(&aiNum[4]);
    Params.Bind(&aiNum[5]);
    Params.BindString(acAlarmInfo);//, sizeof(acAlarmInfo));
    Params.Bind(&aiNum[6]);
    Params.Bind(&aiNum[7]);
    Params.BindString(acConfirmInfo);//, sizeof(acConfirmInfo));
    Params.Bind(&aiNum[8]);
    Params.BindString(acClearInfo);//, sizeof(acClearInfo));
    Params.Bind(&aiNum[9]);
    Params.Bind(&aiNum[10]);

    UINT32  ulTime = GetTickCount();
    UINT32  ulTotalTime = ulTime;

    g_pApp->TransBegin();
    for (i=1; i<=1000; i++)
    {
        j = MOD(i,50);
        if (j == 1)
        {
            ulLen = 0;
        }

        aiNum[0] = i;
        aiNum[1] = MOD(i, 100);
        aiNum[2] = MOD(i, 1000);
        aiNum[3] = 4;
        aiNum[4] = 5;
        aiNum[5] = 6;
        sprintf(acAlarmInfo, "alarm %d", i);
        aiNum[6] = 7;
        aiNum[7] = 7;
        sprintf(acConfirmInfo, "auto %d", i);
        aiNum[8] = 7;
        sprintf(acClearInfo, "clear %d", i);
        aiNum[9] = 7;
        aiNum[10] = i;

#if 1
        iRowCount += g_pApp->Update("insert into historyalarm(AlarmID, AlarmObjectID, NeID, AlarmLevel, AlarmType, AlarmGenerateTime, "
                                    "AlarmInfo, AlarmConfirm, AlarmConfirmTime, AlarmConfirmInfo, AlarmClearTime, AlarmClearInfo, AlarmUpdateTime, AlarmGroupID) "
                                    "values(?,?,?,?,?,?,?,?,?,?,?,?,?,?)", Params);
#else
        ulLen += sprintf(acSQL+ulLen, "insert into historyalarm values(%u,%u,%u,%u,%u,%u,'%s',%u,%u,'%s',%u,'%s',%u)",
                aiNum[0], aiNum[1], aiNum[2], aiNum[3], aiNum[4], aiNum[5],
                acAlarmInfo, aiNum[6], aiNum[7], acConfirmInfo, aiNum[8], acClearInfo, aiNum[9]);
        if (j != 0)
        {
        //  continue;
        }

        iRowCount += g_pApp->Update(acSQL);
#endif
        if (MOD(i, 1000) == 0)
        {
            printf("time = %d\n", GetTickCount() - ulTime);
            ulTime = GetTickCount();
        }

        if (MOD(i, 1000) == 0)
        {
            g_pApp->TransCommit();
            g_pApp->TransBegin();
        }
    }

    ulTotalTime = GetTickCount() - ulTotalTime;
    printf("\n\ntotal time = %u\n", ulTotalTime);
    return;
}

void test_qry_alarm()
{
    UINT32      aiNum[8];
    CHAR        acAlarmInfo[128];
    CHAR        acConfirmInfo[128];
    CHAR        acClearInfo[128];
    INT32       iRowCount;
    PDBRsltParam    Params;
    // AlarmSn,AlarmID, NeID, AlarmGenerateTime, AlarmInfo, AlarmConfirm, AlarmConfirmTime, AlarmConfirmInfo

    memset(aiNum, 0, sizeof(aiNum));
    memset(acAlarmInfo, 0, sizeof(acAlarmInfo));
    memset(acConfirmInfo, 0, sizeof(acConfirmInfo));

    Params.Bind(&aiNum[0]);
    Params.Bind(&aiNum[1]);
    Params.Bind(&aiNum[2]);
    Params.Bind(&aiNum[3]);
    Params.Bind(&aiNum[4]);
    Params.Bind(&aiNum[5]);
    Params.Bind(acAlarmInfo, sizeof(acAlarmInfo));
    Params.Bind(&aiNum[6]);
    Params.Bind(&aiNum[7]);
    Params.Bind(acConfirmInfo, sizeof(acConfirmInfo));
    Params.Bind(&aiNum[8]);
    Params.Bind(acClearInfo, sizeof(acClearInfo));
    Params.Bind(&aiNum[9]);

    UINT32  ulTime = GetTickCount();

    iRowCount = g_pApp->Query("select * from historyalarm", Params);
    if (iRowCount < 0)
    {
        return;
    }

    while (PDB_CONTINUE == g_pApp->RowFetch())
    {
        iRowCount++;
    }

    g_pApp->CloseQuery();

    printf("time = %d num = %d\n", GetTickCount() - ulTime, iRowCount);

    return;
}

PDBEnvBase* GetPDBEnv(UINT32 ulDBType)
{
    PDBEnvBase  *pEnv = NULL;

    if (ulDBType & PDBT_ODBC)
    {
        pEnv = new PDBODBCEnv();
    }
    else
    {
        if (ulDBType == PDBT_MYSQL)
        {
            pEnv = new PDBMySQLEnv();
        }
#ifdef HAVE_ORACLE
        else if (ulDBType == PDBT_ORACLE)
        {
            pEnv = new PDBOracleEnv();
        }
#endif
    }

    return pEnv;
}

#ifdef TEST_OCI

int test_oci()
{

    char *user="system";
    char *password="Fritt_1957";
    char *dbName="orcl";
    //char *insertStmt = "insert into historyalarm values(1,1,1,4,5,6,'alarm 1',7,7,'auto 1',7,'clear 1',7)";
    char *insertStmt = "insert into historyalarm values(:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13)";
    int err;
    UINT32 ulErrCode;
    CHAR    acErrBuf[128];

    OCIEnv *envhpp;
    OCIError *errhpp;
    OCISvcCtx *svcCtx;
    OCIStmt *stmt;
    OCIBind *BindHandle;

    UINT32      aiNum[16];
    CHAR        acAlarmInfo[128];
    CHAR        acConfirmInfo[128];
    CHAR        acClearInfo[128];

    UINT32      i = 1;

    aiNum[0] = i;
    aiNum[1] = MOD(i, 100);
    aiNum[2] = MOD(i, 1000);
    aiNum[3] = 4;
    aiNum[4] = 5;
    aiNum[5] = 6;
    sprintf(acAlarmInfo, "alarm %d", i);
    aiNum[6] = 7;
    aiNum[7] = 7;
    sprintf(acConfirmInfo, "auto %d", i);
    aiNum[8] = 7;
    sprintf(acClearInfo, "clear %d", i);
    aiNum[9] = 7;

    //创建环境句柄
    err = OCIEnvCreate(&envhpp,OCI_OBJECT,(dvoid*)0,(dvoid*(*)(dvoid*,size_t))0,(dvoid * (*)(dvoid *, dvoid *, size_t))0,
        (void (*)(dvoid *, dvoid *)) 0, (size_t) 0, (dvoid **) 0 );

    //创建错误句柄
    err = OCIHandleAlloc(envhpp,(void**)&errhpp,OCI_HTYPE_ERROR,(size_t)0,(void**)0);

    //登陆服务器
    err = OCILogon(envhpp,errhpp,&svcCtx,(const OraText*)user,(ub4)strlen(user),(const OraText*)password,(ub4)strlen(password),
        (const OraText*)dbName,(ub4)strlen(dbName));

    //创建DML/DDL句柄
    err = OCIHandleAlloc(envhpp,(void**)&stmt,OCI_HTYPE_STMT,(size_t)0,(void**)0);

    //准备SQL语句
    err = OCIStmtPrepare(stmt,errhpp,(const OraText*)insertStmt,(ub4)strlen(insertStmt),OCI_NTV_SYNTAX,(ub4)OCI_DEFAULT);

    i = 1;
    OCIBindByPos(stmt, &BindHandle,errhpp,i++, &aiNum[0], 4, SQLT_UIN, 0,0,0,0,0, OCI_DEFAULT);
    OCIBindByPos(stmt, &BindHandle,errhpp,i++, &aiNum[1], 4, SQLT_UIN, 0,0,0,0,0, OCI_DEFAULT);
    OCIBindByPos(stmt, &BindHandle,errhpp,i++, &aiNum[2], 4, SQLT_UIN, 0,0,0,0,0, OCI_DEFAULT);
    OCIBindByPos(stmt, &BindHandle,errhpp,i++, &aiNum[3], 4, SQLT_UIN, 0,0,0,0,0, OCI_DEFAULT);
    OCIBindByPos(stmt, &BindHandle,errhpp,i++, &aiNum[4], 4, SQLT_UIN, 0,0,0,0,0, OCI_DEFAULT);
    OCIBindByPos(stmt, &BindHandle,errhpp,i++, &aiNum[5], 4, SQLT_UIN, 0,0,0,0,0, OCI_DEFAULT);
    OCIBindByPos(stmt, &BindHandle,errhpp,i++, acAlarmInfo, strlen(acAlarmInfo)+1, SQLT_STR, 0,0,0,0,0, OCI_DEFAULT);
    OCIBindByPos(stmt, &BindHandle,errhpp,i++, &aiNum[6], 4, SQLT_UIN, 0,0,0,0,0, OCI_DEFAULT);
    OCIBindByPos(stmt, &BindHandle,errhpp,i++, &aiNum[7], 4, SQLT_UIN, 0,0,0,0,0, OCI_DEFAULT);
    OCIBindByPos(stmt, &BindHandle,errhpp,i++, acConfirmInfo, strlen(acConfirmInfo)+1, SQLT_STR, 0,0,0,0,0, OCI_DEFAULT);
    OCIBindByPos(stmt, &BindHandle,errhpp,i++, &aiNum[8], 4, SQLT_UIN, 0,0,0,0,0, OCI_DEFAULT);
    OCIBindByPos(stmt, &BindHandle,errhpp,i++, acClearInfo, strlen(acClearInfo)+1, SQLT_STR, 0,0,0,0,0, OCI_DEFAULT);
    err = OCIBindByPos(stmt, &BindHandle,errhpp,i++, &aiNum[9], 4, SQLT_UIN, 0,0,0,0,0, OCI_DEFAULT);

    UINT32      ulTime = GetTickCount();

    //执行SQL语句
    for (i=0; i<10000; i++)
    {
        aiNum[0] = i;
        aiNum[1] = MOD(i, 100);
        aiNum[2] = MOD(i, 1000);
        aiNum[3] = 4;
        aiNum[4] = 5;
        aiNum[5] = 6;
        sprintf(acAlarmInfo, "alarm %d", 0*i);
        aiNum[6] = 7;
        aiNum[7] = 7;
        sprintf(acConfirmInfo, "auto %d", 0*i);
        aiNum[8] = 7;
        sprintf(acClearInfo, "clear %d", 0*i);
        aiNum[9] = 7;

        err = OCIStmtExecute(svcCtx,stmt,errhpp,(ub4)1,(ub4)0,(OCISnapshot*)0,(OCISnapshot*)0,OCI_DEFAULT);

        if (MOD(i+1, 400) == 0)
        {
            err = OCITransCommit(svcCtx,errhpp,0);

            printf("time = %u\n", GetTickCount() - ulTime);
            ulTime = GetTickCount();
        }
    }

//    err = OCIErrorGet((dvoid *) errhpp, (ub4)1, (text *) NULL, (int*)&ulErrCode, (unsigned char*)acErrBuf, (ub4) sizeof(acErrBuf), (ub4) OCI_HTYPE_ERROR);

    //提交当前事

    return 0;
}

int test_oci_qry()
{

    char *user="system";
    char *password="fritt.1957";
    char *dbName="inms";
    char *insertStmt = "select neid from historyalarm";
    int err;
    UINT32 ulErrCode;
    CHAR    acErrBuf[128];
    UINT32  ulNeID;

    OCIEnv *envhpp;
    OCIDefine     *defnp1;
    OCIError *errhpp;
    OCISvcCtx *svcCtx;
    OCIStmt *stmt;

    //创建环境句柄
    err = OCIEnvCreate(&envhpp,OCI_OBJECT,(dvoid*)0,(dvoid*(*)(dvoid*,size_t))0,(dvoid * (*)(dvoid *, dvoid *, size_t))0,
        (void (*)(dvoid *, dvoid *)) 0, (size_t) 0, (dvoid **) 0 );

    //创建错误句柄
    err = OCIHandleAlloc(envhpp,(void**)&errhpp,OCI_HTYPE_ERROR,(size_t)0,(void**)0);

    //登陆服务器
    err = OCILogon(envhpp,errhpp,&svcCtx,(const OraText*)user,(ub4)strlen(user),(const OraText*)password,(ub4)strlen(password),
        (const OraText*)dbName,(ub4)strlen(dbName));

    //创建DML/DDL句柄
    err = OCIHandleAlloc(envhpp,(void**)&stmt,OCI_HTYPE_STMT,(size_t)0,(void**)0);

    //准备SQL语句
    err = OCIStmtPrepare(stmt,errhpp,(const OraText*)insertStmt,(ub4)strlen(insertStmt),OCI_NTV_SYNTAX,(ub4)OCI_DEFAULT);

    //执行SQL语句
    err = OCIDefineByPos(stmt,&defnp1,errhpp,1, &ulNeID, sizeof(ulNeID), SQLT_UIN, 0,0,0, OCI_DEFAULT);

    err = OCIStmtExecute(svcCtx,stmt,errhpp,1,0,0,0,OCI_DEFAULT);

    err = OCIErrorGet((dvoid *) errhpp, (ub4)1, (text *) NULL, (int*)&ulErrCode,
                      (unsigned char*)acErrBuf, (ub4) sizeof(acErrBuf), (ub4) OCI_HTYPE_ERROR);

    err = OCIStmtFetch(stmt,errhpp, 1, OCI_FETCH_NEXT, OCI_DEFAULT);


    return 0;
}

#endif

void test_popen()
{
    CHAR    acBuf[512];
    CHAR    *szLine = NULL;
    FILE    *fp;

    //fp = _popen("e:\\temp.vbs", "r");
    fp = _popen("e:\\telnet.exe 192.168.100.7", "r");
    //fp = _popen("dir d: /w", "rt");
/*
    gos_sleep(2);
    fprintf(fp, "jjf\r\n");

    gos_sleep(2);
    fprintf(fp, "123456\r\n");

    gos_sleep(2);
    fprintf(fp, "/home/teldir/getinfo.sh\r\n"); */

    while(fp && !feof(fp))
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

        szLine = acBuf;
        printf("%s", szLine);
    }

    if (fp)
    {
        _pclose(fp);
    }
}

#define BUFSIZE  4096
int RunExecute(char *sCmdText)
{
    //int index = sUrlFile.find_last_of('_');
    //if (-1 == index) return -1;

    //在CreatePipe、CreateProcess等Create系列函数中，
    //通常都有一个SECURITY_ATTRIBUTES类型的参数
    SECURITY_ATTRIBUTES saAttr = {0};
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    //若把该参数的bInheritHandle项设为TRUE，
    //则它创建出来的句柄可被子进程继承。
    //例如，用CreatePipe创建的管道可用于CreateProcess创建的进程
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    //hChildWrite是子进程的输入句柄，hChildRead是父进程用于写入子进程输入的句柄
    HANDLE hChildRead, hChildWrite;
    CreatePipe(&hChildRead, &hChildWrite, &saAttr, 0);
    //设置子进程不能继承接收输入管道的另一端：hChildWrite
    SetHandleInformation(hChildWrite, HANDLE_FLAG_INHERIT, 0);
    //hPipeWrite是子进程的输出句柄，hPipeRead是父进程用于读取子进程输出的句柄
    HANDLE hPipeRead, hPipeWrite;
    CreatePipe(&hPipeRead, &hPipeWrite, &saAttr, 0);
    //设置子进程不能继承发送输出管道的另一端：hPipeRead
    SetHandleInformation(hPipeRead, HANDLE_FLAG_INHERIT, 0);

    //CreateProcess的第一个参数
    STARTUPINFO si = {0};
    si.cb = sizeof(STARTUPINFO);
    //将标准输出和错误输出定向到我们建立的hPipeWrite上
    si.hStdError = hPipeWrite;
    si.hStdOutput = hPipeWrite;
    //设置子进程接受StdIn以及StdOut的重定向
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;


    //CreateProcess的第二个参数
    PROCESS_INFORMATION pi = {0};

    BOOL bProcess = CreateProcess(NULL, (LPSTR)sCmdText, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi);

    BOOL flag = true;

    //关闭父进程的输入管道，否则若父进程没有输入，子进程会一直被挂起
    CloseHandle(hChildWrite);

    //关闭子进程的输出管道，否则若子进程没有输出，父进程会一直被挂起
    CloseHandle(hPipeWrite);

    char buffer[BUFSIZE+1] = {0};
    DWORD BytesRead = 0, BytesWritten;
    GString sOutText;

    while(flag)
    {
        buffer[BytesRead] = '\0';
        sOutText.Append(buffer);

        //从子进程读入
        flag = ReadFile(hPipeRead, buffer, BUFSIZE, &BytesRead, NULL);
    }
    CloseHandle(hPipeRead);

    //
    printf("====\n%s\n", sOutText.GetString());

    return true;
}

/*
void test_ins_multi_perf()
{
    INT32       aiNum[4];
    FLOAT       adValue[4];
    PDBParam    Params;
    UINT32      ulTime = GetTickCount();

    INT32 iNum = 0;
    // AlarmSn,AlarmID, NeID, AlarmGenerateTime, AlarmInfo, AlarmConfirm, AlarmConfirmTime, AlarmConfirmInfo

    g_pApp->TransBegin();
    iNum += g_pApp->Update("insert into perfdata(PerfTime, NeID, CounterNum, Counter1,Counter2,Counter3) values(1,1,1,1,1,1);"
                           "insert into perfdata(PerfTime, NeID, CounterNum, Counter1,Counter2,Counter3) values(2,2,2,2,2,2);");
    g_pApp->TransCommit();

    return;
}*/

typedef struct
{
    UINT32      a;
    UINT32      b;
    UINT32      c;
    DOUBLE      e;
    DOUBLE      f;
    DOUBLE      g;
}PERF_T;

void test_ins_multi_perf()
{
    INT32       aiNum[4];
    FLOAT       adValue[4];
    PDBParam    Params;
    UINT32      ulTime = GetTickCount();
    INT32           iNum = 0;
    UINT32          ulMaxBatchNum = 2000;
    UINT32          ulSkipLen = sizeof(PERF_T);
    PERF_T          data;
    PDB_PARAM_DESC_T astParamDesc[] = {
                            {PDBT_UINT32,   STRUCT_OFFSET(PERF_T, a),  sizeof(data.a)},
                            {PDBT_UINT32,   STRUCT_OFFSET(PERF_T, b),  sizeof(data.b)},
                            {PDBT_UINT32,   STRUCT_OFFSET(PERF_T, c),  sizeof(UINT32)},
                            {PDBT_DOUBLE,   STRUCT_OFFSET(PERF_T, e),  sizeof(DOUBLE)},
                            {PDBT_DOUBLE,   STRUCT_OFFSET(PERF_T, f),  sizeof(DOUBLE)},
                            {PDBT_DOUBLE,   STRUCT_OFFSET(PERF_T, g),  sizeof(DOUBLE)} };
    UINT32          ulParamNum = ARRAY_SIZE(astParamDesc);
    PDBBatchParam   Param(ulMaxBatchNum, ulSkipLen, astParamDesc, ulParamNum);
    UINT32          i;

    for (i=0; i<1000; i++)
    {
        data.a = i;
        data.b = i+1;
        data.c = i+2;
        data.e = i+3;
        data.f = i+4;
        data.g = i+5;
        Param.AddData(&data);
    }

    g_pApp->TransBegin();
    iNum += g_pApp->UpdateBatch("insert into perfdata(PerfTime, NeID, CounterNum, Counter1,Counter2,Counter3) values(?,?,?,?,?,?)",
                        &Param, 1000);
    g_pApp->TransCommit();

    return;
}


typedef struct
{
    CHAR                acNeID[32];          // 网元标识
    UINT32              ulAlarmID;          // 告警码
    UINT32              ulAlarmRaiseTime;

    UINT32              ulAlarmLevel;
    UINT32              ulAlarmType;
    UINT32              ulAlarmStatus;
    UINT32              ulPrevAlarmStatus;  // 上一个时刻的告警状态

    BOOL                bIsNotifyAlarm;

    CHAR                acAlarmTitle[64];
    CHAR                acAlarmInfo[128];
    UINT32              ulAlarmReasonID;
    CHAR                acAlarmReason[128];

    UINT32              ulAlarmClearTime;
    CHAR                acAlarmClearInfo[64];

    INT32               iSyncFlag;
}ALARM_INFO_T;

void test_ins_multi_alarm()
{
    INT32       aiNum[4];
    UINT32      ulTime = gos_get_uptime_1ms();
    INT32           iNum = 0;
    UINT32          ulMaxBatchNum = 10;
    UINT32          ulSkipLen = sizeof(ALARM_INFO_T);
    ALARM_INFO_T    data;
    ALARM_INFO_T    *pstData = (ALARM_INFO_T*)calloc(ulMaxBatchNum, sizeof(ALARM_INFO_T));
    PDB_PARAM_DESC_T astParamDesc[] = {
                            {PDBT_STRING,   STRUCT_OFFSET(ALARM_INFO_T, acNeID),              sizeof(data.acNeID)},
                            {PDBT_UINT32,   STRUCT_OFFSET(ALARM_INFO_T, ulAlarmID),           sizeof(UINT32)},
                            {PDBT_UINT32,   STRUCT_OFFSET(ALARM_INFO_T, ulAlarmRaiseTime),    sizeof(UINT32)},
                            {PDBT_UINT32,   STRUCT_OFFSET(ALARM_INFO_T, ulAlarmType),         sizeof(UINT32)},
                            {PDBT_UINT32,   STRUCT_OFFSET(ALARM_INFO_T, ulAlarmLevel),        sizeof(UINT32)},
                            {PDBT_STRING,   STRUCT_OFFSET(ALARM_INFO_T, acAlarmTitle),        sizeof(data.acAlarmTitle)},
                            {PDBT_STRING,   STRUCT_OFFSET(ALARM_INFO_T, acAlarmInfo),         sizeof(data.acAlarmInfo)},
                            {PDBT_STRING,   STRUCT_OFFSET(ALARM_INFO_T, acAlarmReason),       sizeof(data.acAlarmReason)},
                            {PDBT_INT32,    STRUCT_OFFSET(ALARM_INFO_T, iSyncFlag),           sizeof(INT32)},
                           /*
                            {PDBT_UINT32,   STRUCT_OFFSET(ALARM_INFO_T, ulAlarmRaiseTime),    sizeof(UINT32)},
                            {PDBT_UINT32,   STRUCT_OFFSET(ALARM_INFO_T, ulAlarmType),         sizeof(UINT32)},
                            {PDBT_UINT32,   STRUCT_OFFSET(ALARM_INFO_T, ulAlarmLevel),        sizeof(UINT32)},
                            {PDBT_STRING,   STRUCT_OFFSET(ALARM_INFO_T, acAlarmTitle),        sizeof(data.acAlarmTitle)},
                            {PDBT_STRING,   STRUCT_OFFSET(ALARM_INFO_T, acAlarmInfo),         sizeof(data.acAlarmInfo)},
                            {PDBT_STRING,   STRUCT_OFFSET(ALARM_INFO_T, acAlarmReason),       sizeof(data.acAlarmReason)},
                            {PDBT_INT32,    STRUCT_OFFSET(ALARM_INFO_T, iSyncFlag),           sizeof(INT32)},*/
                        };
    UINT32          ulParamNum = ARRAY_SIZE(astParamDesc);
    PDBBatchParam   Param(ulMaxBatchNum, ulSkipLen, astParamDesc, ulParamNum);
    UINT32          i;
    PDBParam        Params;
    const char      *szSQL = "insert into active_alarm(ne_id, alarm_id, alarm_time, alarm_type, alarm_level, alarm_title, "
        "alarm_info, alarm_reason, SyncFlag) values(?,?,?,?,?,?,?,?,?) "
        "ON DUPLICATE KEY UPDATE alarm_time=?, alarm_type=?, alarm_level=?, alarm_title=?, alarm_info=?, alarm_reason=?, SyncFlag=?";
    GString         strSQL;

    memset(&data, 0, sizeof(data));
    for (i=0; i<ulMaxBatchNum; i++)
    {
        sprintf(pstData[i].acNeID, "%u", i+1);
        pstData[i].ulAlarmID = MOD(i,20);
        pstData[i].ulAlarmRaiseTime = gos_get_rand();
        pstData[i].ulAlarmType = MOD(i, 5);
        pstData[i].ulAlarmLevel = MOD(i, 5);
        sprintf(pstData[i].acAlarmTitle, "alarm title %u", i+1);
        sprintf(pstData[i].acAlarmInfo, "告警信息告警信息告警信息告警信息告警信息告警信息");
        sprintf(pstData[i].acAlarmReason, "告警原因告警原因告警原因告警原因告警原因");
        pstData[i].iSyncFlag = gos_get_current_time();
        Param.AddData(pstData+i);

        Params.BindString(pstData[i].acNeID);
        Params.Bind(&pstData[i].ulAlarmID);
        Params.Bind(&pstData[i].ulAlarmRaiseTime);
        Params.Bind(&pstData[i].ulAlarmType);
        Params.Bind(&pstData[i].ulAlarmLevel);
        Params.BindString(pstData[i].acAlarmTitle);
        Params.BindString(pstData[i].acAlarmInfo);
        Params.BindString(pstData[i].acAlarmReason);
        Params.Bind(&pstData[i].iSyncFlag);

        Params.Bind(&pstData[i].ulAlarmRaiseTime);
        Params.Bind(&pstData[i].ulAlarmType);
        Params.Bind(&pstData[i].ulAlarmLevel);
        Params.BindString(pstData[i].acAlarmTitle);
        Params.BindString(pstData[i].acAlarmInfo);
        Params.BindString(pstData[i].acAlarmReason);
        Params.Bind(&pstData[i].iSyncFlag);

        if (i > 0)
        {
            strSQL.Append("\t");
        }
        strSQL.Append(szSQL);
    }

    strSQL.Append(";");

    int iSQLLen = strSQL.Length();

 //   printf("%s\n", strSQL.GetString());

    //const char *szSQLS = "insert into active_alarm(ne_id, alarm_id, alarm_time, alarm_type, alarm_level, alarm_title, alarm_info, alarm_reason, SyncFlag) values('1',1,0,0,0,'0','0','0',0) ON DUPLICATE KEY UPDATE alarm_time=1, alarm_type=1, alarm_level=1, alarm_title='1', alarm_info='1', alarm_reason='1', SyncFlag=1; insert into active_alarm(ne_id, alarm_id, alarm_time, alarm_type, alarm_level, alarm_title, alarm_info, alarm_reason, SyncFlag) values('2',2,0,0,0,'0','0','0',0) ON DUPLICATE KEY UPDATE alarm_time=1, alarm_type=1, alarm_level=1, alarm_title='1', alarm_info='1', alarm_reason='1', SyncFlag=1;";

    const char *szSQLS = "replace into active_alarm(ne_id, alarm_id) values('1',1);"
                         "replace into active_alarm(ne_id, alarm_id) values('2',1)"
                         ;

    g_pApp->TransBegin();
 //   iNum += g_pApp->UpdateBatch(strSQL.GetString(), &Param, ulMaxBatchNum);
    iNum += g_pApp->Update(szSQLS);
    g_pApp->TransCommit();

    ulTime = gos_get_uptime_1ms() - ulTime;

    printf("ins %u row in %u ms\n", ulMaxBatchNum, ulTime);
    return;
}

const UINT32    g_ulNeNum = 1000;
const UINT32    g_ulAlarmNum = 10;
const UINT32    g_ulMaxBatchNum = g_ulNeNum*g_ulAlarmNum;
ALARM_INFO_T    g_astData[g_ulMaxBatchNum];
GString         g_strSQL;

void init_data()
{
    UINT32  i = 0;
    for (UINT32 ulNeID=0; ulNeID<g_ulNeNum; ulNeID++)
    {
        for (UINT32 ulAlarmID=0; ulAlarmID<g_ulAlarmNum; ulAlarmID++)
        {
            sprintf(g_astData[i].acNeID, "%012u", ulNeID);
            g_astData[i].ulAlarmID = ulAlarmID;
            g_astData[i].ulAlarmRaiseTime = gos_get_rand();
            g_astData[i].ulAlarmType = MOD(i, 5);
            g_astData[i].ulAlarmLevel = MOD(i, 5);
            sprintf(g_astData[i].acAlarmTitle, "alarm title %u", i+1);
            sprintf(g_astData[i].acAlarmInfo, "告警信息告警信息告警信息告警信息告警信息告警信息");
            sprintf(g_astData[i].acAlarmReason, "告警原因告警原因告警原因告警原因告警原因");
            g_astData[i].iSyncFlag = gos_get_current_time();

            i++;
        }
    }
}

// replace into
void test_ins_multi_alarm1()
{
    INT32       aiNum[4];
    FLOAT       adValue[4];
    UINT32      ulTime = gos_get_uptime_1ms();
    INT32           iNum = 0;
    ALARM_INFO_T    data;
    UINT32          i;
    PDBParam        Params;
    const char      *szSQL = "replace into active_alarm(ne_id, alarm_id, alarm_time, alarm_type, alarm_level, alarm_title, "
        "alarm_info, alarm_reason, SyncFlag) values(?,?,?,?,?,?,?,?,?) ";
       // "ON DUPLICATE KEY UPDATE alarm_time=?, alarm_type=?, alarm_level=?, alarm_title=?, alarm_info=?, alarm_reason=?, SyncFlag=?";


    Params.BindString(data.acNeID);
    Params.Bind(&data.ulAlarmID);
    Params.Bind(&data.ulAlarmRaiseTime);
    Params.Bind(&data.ulAlarmType);
    Params.Bind(&data.ulAlarmLevel);
    Params.BindString(data.acAlarmTitle);
    Params.BindString(data.acAlarmInfo);
    Params.BindString(data.acAlarmReason);
    Params.Bind(&data.iSyncFlag);

    //const char *szSQLS = "insert into active_alarm(ne_id, alarm_id, alarm_time, alarm_type, alarm_level, alarm_title, alarm_info, alarm_reason, SyncFlag) values('1',1,0,0,0,'0','0','0',0) ON DUPLICATE KEY UPDATE alarm_time=1, alarm_type=1, alarm_level=1, alarm_title='1', alarm_info='1', alarm_reason='1', SyncFlag=1; insert into active_alarm(ne_id, alarm_id, alarm_time, alarm_type, alarm_level, alarm_title, alarm_info, alarm_reason, SyncFlag) values('2',2,0,0,0,'0','0','0',0) ON DUPLICATE KEY UPDATE alarm_time=1, alarm_type=1, alarm_level=1, alarm_title='1', alarm_info='1', alarm_reason='1', SyncFlag=1;";

    g_pApp->TransBegin();
 //   iNum += g_pApp->UpdateBatch(strSQL.GetString(), &Param, ulMaxBatchNum);
    for (UINT32 i=0; i<g_ulMaxBatchNum; i++)
    {
        memcpy(&data, &g_astData[i], sizeof(data));
        iNum += g_pApp->Update(szSQL, Params);

        if (MOD(i+1, 200) == 0)
        {
            g_pApp->TransCommit();
            g_pApp->TransBegin();
        }
    }

   g_pApp->TransCommit();

    ulTime = gos_get_uptime_1ms() - ulTime;

    printf("ins %u row in %u ms\n", g_ulMaxBatchNum, ulTime);
    return;
}

// insert into (),()
void test_ins_multi_alarm2()
{
    INT32       aiNum[4];
    FLOAT       adValue[4];
    UINT32      ulTime = gos_get_uptime_1ms();
    INT32           iNum = 0;
    ALARM_INFO_T    data;
    UINT32          i = 0;
    PDBParam        Params;
    static char     acSQL[31*1024];
    UINT32          ulSQLLen = 0;

    g_pApp->TransBegin();

    UINT32      ulBatchNum = 100;

    for (UINT32 j=0; j<g_ulMaxBatchNum/ulBatchNum; j++)
    {
        ulSQLLen = sprintf(acSQL, "insert into alarm1(ne_id, alarm_id, alarm_time, alarm_type, alarm_level, alarm_title, "
                             "alarm_info, alarm_reason, SyncFlag) values ");

        for (UINT32 k=0; k<ulBatchNum; k++)
        {
            memcpy(&data, &g_astData[i], sizeof(data));

            ulSQLLen += sprintf(acSQL+ulSQLLen, "%c ('%s',%u,%u,%u,%u,'%s','%s','%s',%u)",
                                k==0?' ':',',
                                data.acNeID,
                                data.ulAlarmID,
                                data.ulAlarmRaiseTime,
                                data.ulAlarmType,
                                data.ulAlarmLevel,
                                data.acAlarmTitle,
                                data.acAlarmInfo,
                                data.acAlarmReason,
                                data.iSyncFlag);



            i++;
        }

        iNum += g_pApp->Update(acSQL);

        g_pApp->TransCommit();
        g_pApp->TransBegin();
    }

    g_pApp->TransCommit();

    ulTime = gos_get_uptime_1ms() - ulTime;

    printf("ins %u row in %u ms\n", g_ulMaxBatchNum, ulTime);
    return;
}

// 多语句)
void test_ins_multi_alarm3()
{
    INT32       aiNum[4];
    FLOAT       adValue[4];
    UINT32      ulTime = gos_get_uptime_1ms();
    INT32           iNum = 0;
    ALARM_INFO_T    data;
    UINT32          i = 0;
    PDBParam        Params;
    static char     acSQL[31*1024];
    UINT32          ulSQLLen = 0;
    const char      *szSQL = "insert into alarm1(ne_id, alarm_id, alarm_time, alarm_type, alarm_level, alarm_title, "
        "alarm_info, alarm_reason, SyncFlag) values('%s',%u,%u,%u,%u,'%s','%s','%s',%u) ";

  //  g_pApp->TransBegin();

    UINT32      ulBatchNum = 2;

    for (UINT32 j=0; j<g_ulMaxBatchNum/ulBatchNum; j++)
    {
        ulSQLLen = sprintf(acSQL, "delimiter ;; \nbegin;");


        for (UINT32 k=0; k<ulBatchNum; k++)
        {
            memcpy(&data, &g_astData[i], sizeof(data));

            ulSQLLen += sprintf(acSQL+ulSQLLen, "replace into active_alarm(ne_id, alarm_id, alarm_time, alarm_type, alarm_level, alarm_title, "
                             "alarm_info, alarm_reason, SyncFlag) values('%s',%u,%u,%u,%u,'%s','%s','%s',%u);",
                                data.acNeID,
                                data.ulAlarmID,
                                data.ulAlarmRaiseTime,
                                data.ulAlarmType,
                                data.ulAlarmLevel,
                                data.acAlarmTitle,
                                data.acAlarmInfo,
                                data.acAlarmReason,
                                data.iSyncFlag);

            i++;
        }

        ulSQLLen += sprintf(acSQL+ulSQLLen, "\ncommit;;");

        iNum += g_pApp->Update(acSQL);

  //      g_pApp->TransCommit();
  //      g_pApp->TransBegin();
    }

 //   g_pApp->TransCommit();

    ulTime = gos_get_uptime_1ms() - ulTime;

    printf("ins %u row in %u ms\n", g_ulMaxBatchNum, ulTime);
    return;
}

#include <../mysql/include/mysql.h>
void free_conn(MYSQL *mysql)
{
    mysql_close(mysql);
    free(mysql);
}

int test_multi_stat()
{
    mysql_server_init(0, NULL, NULL);

    /* gcc -o chang_mysql mysql_access.c -L /usr/lib64/mysql -lmysqlclient */
    MYSQL *my_con=(MYSQL *)malloc(sizeof(MYSQL));
    my_con=mysql_init(my_con);
    if(!my_con)
    {
        printf("mysql_init error\n");
        return -1;
    }

    my_con=mysql_real_connect(my_con,"127.0.0.1","root","root","test",3306,NULL, CLIENT_MULTI_STATEMENTS);
    if(my_con)
    {
        printf("connect success!\n");
    }
    else{
        printf("connect fail![errno:%d][error:%s][sqlstat:%s]\n",mysql_errno(my_con),mysql_error(my_con),mysql_sqlstate(my_con));
    }

    /* 一次执行多条语句+读完整个resault集 */
    char alo_sql_str[4096];
    static char     acSQL[31*1024];

    init_data();

    sprintf(alo_sql_str,"replace into active_alarm(ne_id, alarm_id) values('000000000000',0);replace into active_alarm(ne_id, alarm_id) values('11111111',2220);");

    int     iNum = 0;
    UINT32      ulBatchNum = 100;
    UINT32      ulSQLLen = 0;
    UINT32      i = 0;
    int res;

    UINT32      ulTime = gos_get_uptime_1ms();

    for (UINT32 j=0; j<g_ulMaxBatchNum/ulBatchNum; j++)
    {
        for (UINT32 k=0; k<ulBatchNum; k++)
        {
            ulSQLLen += sprintf(acSQL+ulSQLLen, "replace into active_alarm(ne_id, alarm_id, alarm_time, alarm_type, alarm_level, alarm_title, "
                             "alarm_info, alarm_reason, SyncFlag) values('%s',%u,%u,%u,%u,'%s','%s','%s',%u);",
                                g_astData[i].acNeID,
                                g_astData[i].ulAlarmID,
                                g_astData[i].ulAlarmRaiseTime,
                                g_astData[i].ulAlarmType,
                                g_astData[i].ulAlarmLevel,
                                g_astData[i].acAlarmTitle,
                                g_astData[i].acAlarmInfo,
                                g_astData[i].acAlarmReason,
                                g_astData[i].iSyncFlag);

            i++;
        }

        res = mysql_query(my_con,acSQL);
     //   mysql_set_server_option(my_con,MYSQL_OPTION_MULTI_STATEMENTS_OFF);
        if (res != 0)
        {

            free_conn(my_con);
            return -1;
        }

        iNum += mysql_affected_rows(my_con);

        // 当使用执行多语句功能后，一定要读完整个resault集，否则会出现错误：erron:2014 error:Commands out of sync; you can't run this command now
        MYSQL_RES    *my_res; //查询结果
        do
        {
            my_res = mysql_store_result( my_con );
            if(my_res)
            {
                mysql_free_result(my_res);
            }
        }while( !mysql_next_result(my_con) );

        ulTime = gos_get_uptime_1ms() - ulTime;
        printf("time = %u\n", ulTime);

        ulTime = gos_get_uptime_1ms();
    }

#if 0
    // 执行多条语句：功能打开-多语句执行-功能关闭
 //   mysql_set_server_option(my_con,MYSQL_OPTION_MULTI_STATEMENTS_ON);
    int res=mysql_query(my_con,alo_sql_str);
 //   mysql_set_server_option(my_con,MYSQL_OPTION_MULTI_STATEMENTS_OFF);
    if(res != 0)
    {
        printf("insert fail,msg:[%s]", alo_sql_str );
        free_conn(my_con);
        return -1;
    }
    // 当使用执行多语句功能后，一定要读完整个resault集，否则会出现错误：erron:2014 error:Commands out of sync; you can't run this command now
    MYSQL_RES    *my_res; //查询结果
    do
    {
        my_res = mysql_store_result( my_con );
        if(my_res) mysql_free_result(my_res);
    }while( !mysql_next_result(my_con) );  /* more my_con? -1 = no, >0 = error, 0 = yes (keep looping) */
#endif

    ulTime = gos_get_uptime_1ms() - ulTime;
    printf("time = %u\n", ulTime);

    free_conn(my_con);
    return 0;
}

int main()
{
    BOOL        bRet = FALSE;
    UINT32      ulConNum = 5;
    UINT32      ulTimeout = 8;
//    ulTimeout = GET_INT("DTPM");

    PDB_CONN_PARAM_T    stConnParam = {0};

    //RunExecute("dir");

   // test_oci();

    /* 初始化数据库环境 */
#if 1 // ms sql
    //strcpy(stConnParam.acServer, "(DESCRIPTION=(ADDRESS_LIST=(ADDRESS=(PROTOCOL=TCP)(HOST=192.168.100.241)(PORT=1521)))(CONNECT_DATA=(SERVICE_NAME=orcl)))");
    strcpy(stConnParam.acServer, "127.0.0.1");
    //strcpy(stConnParam.acServer, "local_mssql");
    strcpy(stConnParam.acDBName, "test");
    strcpy(stConnParam.acUserName, "root");
    strcpy(stConnParam.acPassword, "root");
    stConnParam.usPort = 3306;
    stConnParam.ulDBType = PDBT_MYSQL;
    stConnParam.bUseDSN = TRUE;
#else
    strcpy(stConnParam.acServer, "inms");//192.168.100.238");
    strcpy(stConnParam.acDBName, "inms");
    strcpy(stConnParam.acUserName, "system");
    strcpy(stConnParam.acPassword, "fritt.1957");
    stConnParam.usPort = 1521;//3306;
    //stConnParam.ulDBType = PDBT_MYSQL | PDBT_ODBC;
    stConnParam.ulDBType = PDBT_ORACLE;
#endif

    test_multi_stat();

    g_pEnv = GetPDBEnv(stConnParam.ulDBType);

    bRet = g_pEnv->InitEnv(&stConnParam,
                    PDB_PREFETCH_ROW_NUM,
                    ulTimeout,
                    ulConNum);


    g_pApp = new PDBApp(g_pEnv);
    if (!g_pApp)
    {
        return -1;
    }

    bRet = g_pApp->AllocConn();

    //test_ins_alarm();
    for (UINT32 i=0; i<100; i++)
    {
        init_data();
        test_ins_multi_alarm1();
    }
//    test_ins_perf();

    // test_qry_perf();

    delete g_pApp;

    delete g_pEnv;

    return 0;
}
