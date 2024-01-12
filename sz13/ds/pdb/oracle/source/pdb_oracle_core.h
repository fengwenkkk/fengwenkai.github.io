#ifndef _PDB_ORACLE_CORE_H
#define _PDB_ORACLE_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push)
#pragma pack(1)
typedef struct
{
    VOID                *pvData;
    UINT32              ulDataLen;
    UINT16              usDataType;
}OCI_BIND_INFO_T;

typedef struct
{
    VOID                *pvData;
    UINT32              ulDataLen;
    UINT16              usDataType;
    BOOL                *pbIsNull;
}OCI_DEFINE_INFO_T;

typedef struct
{
    OCIEnv              *EnvHandle;
    OCIServer           *ServerHandle;
    OCIError            *ErrorHandle;
    OCISvcCtx           *SvcCtxHandle;
    //OCISession            *SessionHandle;
    OCISession          *ConnHandle;
    OCIStmt             *StmtHandle;
    OCITrans            *TransHandle;
    OCIDefine           *DefineHandle;
    //OCIBind               *BindHandle;
}OCI_HANDLES_T;

typedef struct
{
/*  PDBCHAR             acServiceName[PDB_MAX_SVC_LEN];
    PDBCHAR             acDatabaseName[PDB_MAX_DATABASE_LEN];
    PDBCHAR             acUserName[PDB_MAX_USER_LEN];
    PDBCHAR             acPassword[PDB_MAX_PWD_LEN]; */

    PDB_CONN_PARAM_T    stConnParam;
    OCI_HANDLES_T       DBHandles;

    CHAR                acTransName[PDB_MAX_TRANS_LEN];

    PDB_OPTION_T        stOption;

    UINT32              ulBindSkipLen;
    UINT32              ulExecNum;      /* 批量执行的次数 */
    UINT32              ulRowCount;
    UINT32              ulBindNum;
    OCI_BIND_INFO_T     astBindInfo[PDB_MAX_BIND_NUM];

    UINT32              ulBindRsltNum;
    OCI_DEFINE_INFO_T   astBindRsltInfo[PDB_MAX_BIND_NUM];
    UINT32              ulFetchedNum;

    INT32               *apiLenInd[PDB_MAX_BIND_NUM];

    CHAR                acSQLCmd[PDB_MAX_SQL_LEN+1];

    CHAR                acErrInfo[PDB_MAX_ERR_LEN]; /* 错误信息 */
    UINT32              ulErrLevel;                 /* 错误级别 */
    INT32               iErrCode;                   /* 错误码 */
}ORACLE_DCI_HANDLE_T;

/* 连接Handle */
typedef struct
{
    BOOL                bIsAlloced;     /* 是否被分配了 */
    BOOL                bIsConnected;   /* 是否已经建立连接 */
    BOOL                bIsInUse;       /* 连接是否使用中 */
    INT32               iConnId;        /* 连接ID*/

    ORACLE_DCI_HANDLE_T stDCIHandle;

}ORACLE_PDB_HANDLE_T;

#pragma pack(pop)

BOOL   OracleCheckHandle(ORACLE_DCI_HANDLE_T *pstHandle);
PDBRET OracleInit(HANDLE *phEnv);
PDBRET OracleExit(HANDLE hEnv);
PDBRET OracleConnect(ORACLE_DCI_HANDLE_T *pstDCIHandle, PDB_CONN_PARAM_T  *pstConnParam, PDB_OPTION_T *pstOption);
PDBRET OracleDisconnect(ORACLE_DCI_HANDLE_T *pstDCIHandle);

PDBRET OracleInitStmt(ORACLE_DCI_HANDLE_T *pstDCIHandle);
PDBRET OracleCloseStmt(ORACLE_DCI_HANDLE_T *pstDCIHandle);
PDBRET OracleSQLPrepare(ORACLE_DCI_HANDLE_T *pstDCIHandle);
PDBRET OracleSetPrefetchNum(ORACLE_DCI_HANDLE_T *pstDCIHandle, UINT32 ulRowNum);
PDBRET OracleSetAutoCommit(ORACLE_DCI_HANDLE_T *pstDCIHandle, BOOL bAutoCommit);
PDBRET OracleQuery(ORACLE_DCI_HANDLE_T *pstDCIHandle);
PDBRET OracleExec(ORACLE_DCI_HANDLE_T *pstDCIHandle);
PDBRET OracleBindRsltByPos(ORACLE_DCI_HANDLE_T  *pstDCIHandle,
                        UINT16          usRsltIndex,
                        UINT16          usRsltType,
                        VOID            *pRsltBuf,
                        UINT32          ulRsltBufLen,
                        BOOL            *pbIsNull,
                        INT32           *piLenInd);
PDBRET OracleRowFetch(ORACLE_DCI_HANDLE_T* pstDCIHandle);
PDBRET OracleFreeRslt(ORACLE_DCI_HANDLE_T* pstDCIHandle);
PDBRET OracleTransBegin(ORACLE_DCI_HANDLE_T* pstDCIHandle, CHAR *szTransName);
PDBRET OracleTransRollback(ORACLE_DCI_HANDLE_T* pstDCIHandle, CHAR *szTransName);
PDBRET OracleTransCommit(ORACLE_DCI_HANDLE_T* pstDCIHandle, CHAR *szTransName);
INT32  OracleRowCount(ORACLE_DCI_HANDLE_T* pstDCIHandle);
PDBRET OracleBindPara(ORACLE_DCI_HANDLE_T   *pstDCIHandle,
                   UINT16       usParaIndex,
                   UINT32       ulParaType,
                   UINT32       ulInoutType,
                   VOID         *pParaValue,
                   UINT32       ulParaLen,
                   INT32        *piLenInd);
PDBRET OracleBindArrayOfStruct(ORACLE_DCI_HANDLE_T* pstPDBHandle, UINT32 ulSkipLen);
PDBRET OracleGetError(ORACLE_DCI_HANDLE_T    *pstDCIHandle);

#ifdef __cplusplus
}
#endif

#endif
