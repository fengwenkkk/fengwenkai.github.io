#ifndef _PDB_ERROR_H
#define _PDB_ERROR_H


static const CHAR g_aszODBCFatalErrorCodes[][6]={
    "08001",
    "08002",
    "08003",
    "08004",
    "08007",
    "08S01"
};

static const CHAR g_aszODBCNormalErrorCodes[][6]={
    "HY001",
    "HY003",
    "HY009",
    "HY010",
    "HY012",
    "HY013",
    "HY014",
    "HY015",
    "HY018"
};

static const UINT32 g_aulOCIFatalErrorCodes[] = {
    18,     /* maximum number of sessions exceeded */
    19,     /* maximum number of session licenses exceeded */
    20,     /*: maximum number of processes num exceeded */
    1017,   /* invalid username/password; logon denied */
    1033,   /* ORACLE initialization or shutdown in progress */
    1034,   /* ORACLE not available */
    1089,   /* immediate shutdown in progress - no operations are permitted */
    1090,   /* shutdown in progress - connection is not permitted */
    1092,   /* Oracle instance terminated. Disconnection forced */
    1094,   /* ALTER DATABASE CLOSE in progress. Connections not permitted */
    3113,   /* end-of-file on communication channel */
    3114,   /* not connected to ORACLE */
    12154,  /* could not resolve service name  */
    12514,  /* TNS监听进程不能解析在连接描述符中给出的 SERVICE_NAME */
    12535,  /* TNS:operation timed out */
    12571,  /* TNS:packet writer failure */
    12541,  /* TNS:no listener */
    12560,  /* TNS:protocol adapter error */
    12500,  /* TNS:listener failed to start a dedicated server process */
};


static const UINT32 g_aulOCINormalErrorCodes[] = {
    57
};

#endif

