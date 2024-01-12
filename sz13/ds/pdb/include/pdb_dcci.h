#ifndef _PDB_DCCI_H
#define _PDB_DCCI_H

class PDBNoSQLConn;

class PDBNoSQLEnv : public PDBEnvBase
{
public:
    PDBNoSQLEnv(VOID):PDBEnvBase(){};
    virtual PDBRET CreateEnv(VOID){return PDB_SUCCESS;};
    virtual PDBRET FreeEnv(VOID){return PDB_SUCCESS;};
    virtual PDBRET CreateConn();
    virtual PDBRET FreeConn(UINT32 ulConnID);
};

class PDBMySQLEnv : public PDBEnvBase
{
public:
    PDBMySQLEnv(VOID):PDBEnvBase(){};
    virtual PDBRET CreateEnv(VOID);
    virtual PDBRET FreeEnv(VOID);
    virtual PDBRET CreateConn();
    virtual PDBRET FreeConn(UINT32 ulConnID);

private:
    PDBRET Init(VOID);

};

class PDBODBCEnv : public PDBEnvBase
{
public:
    PDBODBCEnv(VOID):PDBEnvBase(){};
    virtual PDBRET CreateEnv(VOID);
    virtual PDBRET FreeEnv(VOID);
    virtual PDBRET CreateConn(VOID);
    virtual PDBRET FreeConn(UINT32 ulConnID);

private:
    PDBRET Init(VOID);

};

class PDBOracleEnv : public PDBEnvBase
{
public:
    PDBOracleEnv(VOID):PDBEnvBase(){};
    virtual PDBRET CreateEnv(VOID);
    virtual PDBRET FreeEnv(VOID);
    virtual PDBRET CreateConn(VOID);
    virtual PDBRET FreeConn(UINT32 ulConnID);

private:
    PDBRET Init(VOID);

};

class PDBNoSQLConn : public PDBConnBase
{
public:
    PDBNoSQLConn(PDBEnvBase *pEnv);
    ~PDBNoSQLConn(VOID);

    virtual PDBRET Connect(PDB_CONN_PARAM_T *pstConnParam, PDB_OPTION_T *pstDBOption);
    virtual PDBRET Disconnect(VOID);
    virtual BOOL   CheckConn(VOID){return TRUE;};
    virtual PDBRET ReConnect(VOID);

    virtual PDBRET Clear(VOID){return PDB_SUCCESS;};
    virtual PDBRET InitStmt(VOID){return PDB_SUCCESS;};
    virtual PDBRET CloseStmt(VOID){return PDB_SUCCESS;};

    virtual PDBRET BindByPos(UINT32 ulType, VOID *pData, INT32 iLen, UINT32 ulInoutType, INT32 *piLenInd){return PDB_SUCCESS;};
    virtual VOID   SetBatchExecParam(UINT32 ulSkipLen, UINT32 ulExecNum){};
    virtual PDBRET BindRsltByPos(UINT16 usRsltIndex, UINT16 usDataType, VOID *pBuf, UINT32 ulBufLen, BOOL *pbIsNull, INT32 *piLenInd){return PDB_SUCCESS;};
    virtual PDBRET OpenCursor(VOID){return PDB_SUCCESS;};
    virtual PDBRET Query(CHAR *szTransName, CHAR *szSQL){return PDB_SUCCESS;};
    virtual PDBRET BindRslt(VOID){return PDB_SUCCESS;};
    virtual PDBRET StoreClientRslt(VOID){return PDB_SUCCESS;};
    virtual PDBRET RowFetch(VOID){return PDB_SUCCESS;};
    virtual PDBRET FreeRslt(VOID){return PDB_SUCCESS;};
    virtual PDBRET Exec(CHAR* szTransName, CHAR* szSQL){return PDB_SUCCESS;};
    virtual PDBRET SetPrefetchSize(UINT32 ulPreFetchNum, UINT32 ulPreFetchMem){return PDB_SUCCESS;};
    virtual INT32  GetRowCount(VOID){return 0;};

    virtual PDBRET TransBegin(CHAR* szTransName){return PDB_SUCCESS;};
    virtual PDBRET TransCommit(CHAR* szTransName){return PDB_SUCCESS;};
    virtual PDBRET TransRollback(CHAR* szTransName){return PDB_SUCCESS;};
    virtual CHAR*  GetTransName(VOID){return (CHAR*)"NULL";};

    virtual BOOL   IsConnected(VOID);
    virtual BOOL   IsAlloced(VOID);
    virtual INT32  GetConnId(VOID);
    virtual VOID   SetConnId(INT32 iConnId);
    virtual BOOL   SetConnectStatus(BOOL bIsConnected);
    virtual BOOL   SetAllocStatus(BOOL bIsAlloced);
    virtual BOOL   SetConnInUseStatus(BOOL bIsInUse);
    virtual BOOL   IsConnInUse(VOID);

    virtual INT32  GetErrCode(VOID){return 0;};
    virtual CHAR*  GetErrInfo(CHAR* szErrInfo=NULL)
                   {
                        if (szErrInfo)
                        {
                            strcpy(szErrInfo, "");
                        }

                        return (CHAR *)"";
                    };
    virtual BYTE   GetErrGrade(PDBRET enErrCode){return PDB_NOR_ERR;};

    virtual HANDLE GetPDBHandle(VOID){return NULL;};


private:
    BOOL    m_bIsConnected;
    BOOL    m_bIsAlloced;
    BOOL    m_bIsInUse;
    INT32   m_iConnId;

    PDBRET Init(VOID);
};

class PDBMySQLConn : public PDBConnBase
{
public:
    PDBMySQLConn(PDBEnvBase *pEnv);
    ~PDBMySQLConn(VOID);

    virtual PDBRET Connect(PDB_CONN_PARAM_T *pstConnParam, PDB_OPTION_T *pstDBOption);
    virtual PDBRET Disconnect(VOID);
    virtual BOOL   CheckConn(VOID);
    virtual PDBRET ReConnect(VOID);

    virtual PDBRET Clear(VOID);
    virtual PDBRET InitStmt(VOID);
    virtual PDBRET CloseStmt(VOID);

    virtual PDBRET BindByPos(UINT32 ulType, VOID *pData, INT32 iLen, UINT32 ulInoutType, INT32 *piLenInd);
    virtual VOID   SetBatchExecParam(UINT32 ulSkipLen, UINT32 ulExecNum);

    virtual PDBRET BindRsltByPos(UINT16 usRsltIndex, UINT16 usDataType, VOID *pBuf, UINT32 ulBufLen, BOOL *pbIsNull, INT32 *piLenInd);
    virtual PDBRET OpenCursor(VOID);
    virtual PDBRET Query(CHAR *szTransName, CHAR *szSQL);
    virtual PDBRET BindRslt(VOID);
    virtual PDBRET StoreClientRslt(VOID);
    virtual PDBRET RowFetch(VOID);
    virtual PDBRET FreeRslt(VOID);

    virtual PDBRET Exec(CHAR* szTransName, CHAR* szSQL);
    virtual PDBRET SetPrefetchSize(UINT32 ulPreFetchNum, UINT32 ulPreFetchMem);
    virtual INT32  GetRowCount(VOID);

    virtual PDBRET TransBegin(CHAR* szTransName);
    virtual PDBRET TransCommit(CHAR* szTransName);
    virtual PDBRET TransRollback(CHAR* szTransName);
    virtual CHAR*  GetTransName(VOID);

    virtual BOOL   IsConnected(VOID);
    virtual BOOL   IsAlloced(VOID);
    virtual INT32  GetConnId(VOID);
    virtual VOID   SetConnId(INT32 iConnId);
    virtual BOOL   SetConnectStatus(BOOL bIsConnected);
    virtual BOOL   SetAllocStatus(BOOL bIsAlloced);
    virtual BOOL   SetConnInUseStatus(BOOL bIsInUse);
    virtual BOOL   IsConnInUse(VOID);

    virtual INT32  GetErrCode(VOID);
    virtual CHAR*  GetErrInfo(CHAR* szErrInfo=NULL);
    virtual BYTE   GetErrGrade(PDBRET enErrCode);
    virtual HANDLE GetPDBHandle(VOID);

    PDBRET ExecDDL(CHAR* szTransName, CHAR* szDDLCmd, ...);
    PDBRET ExecDDL(CHAR* szTransName, CHAR *szDDLCmd, va_list* vaMsg);
    BOOL   ExecProcedure(CHAR* szTransName, CHAR *szSQL, CHAR** &pszOutRslt);
    PDBRET CloseProcedure(VOID);
    PDBRET MultiDDLRowFetch(CHAR** &pszQryRslt);

    PDBRET ExecFile(CHAR* szTransName, CHAR* szSQLFile);

    BOOL   IsDBExists(CHAR* szTransName, CHAR* szDBName);
    BOOL   IsTableExists(CHAR* szTransName, CHAR* szDBName, CHAR* szTableName);

    PDBRET CreateDB(CHAR* szTransName, CHAR* szDBName);
    PDBRET UseDB(CHAR* szTransName, CHAR* szDBName);
    PDBRET DeleteDB(CHAR* szTransName, CHAR* szDBName);

private:

    /*连接初始化*/
    PDBRET Init(VOID);

    PDBRET CheckErr(PDBRET enRet);
    PDBRET CheckErrCode(PDBRET Status);
};

class PDBODBCConn : public PDBConnBase
{
public:
    PDBODBCConn(PDBEnvBase *pEnv);
    ~PDBODBCConn(VOID);

    virtual PDBRET Connect(PDB_CONN_PARAM_T *pstConnParam, PDB_OPTION_T *pstDBOption);
    virtual PDBRET Disconnect(VOID);
    virtual BOOL   CheckConn(VOID);
    virtual PDBRET ReConnect(VOID);

    virtual PDBRET Clear(VOID);
    virtual PDBRET InitStmt(VOID);
    virtual PDBRET CloseStmt(VOID);

    virtual PDBRET BindByPos(UINT32 ulType, VOID *pData, INT32 iLen, UINT32 ulInoutType, INT32 *piLenInd);
    virtual VOID   SetBatchExecParam(UINT32 ulSkipLen, UINT32 ulExecNum);
    virtual PDBRET BindRsltByPos(UINT16 usRsltIndex, UINT16 usDataType, VOID *pBuf, UINT32 ulBufLen, BOOL *pbIsNull, INT32 *piLenInd);
    virtual PDBRET OpenCursor(VOID);
    virtual PDBRET Query(CHAR *szTransName, CHAR *szSQL);
    virtual PDBRET BindRslt(VOID);
    virtual PDBRET StoreClientRslt(VOID);
    virtual PDBRET RowFetch(VOID);
    virtual PDBRET FreeRslt(VOID);

    virtual PDBRET Exec(CHAR* szTransName, CHAR* szSQL);
    virtual PDBRET SetPrefetchSize(UINT32 ulPreFetchNum, UINT32 ulPreFetchMem);
    virtual INT32  GetRowCount(VOID);

    virtual PDBRET TransBegin(CHAR* szTransName);
    virtual PDBRET TransCommit(CHAR* szTransName);
    virtual PDBRET TransRollback(CHAR* szTransName);
    virtual CHAR*  GetTransName(VOID);

    virtual BOOL   IsConnected(VOID);
    virtual BOOL   IsAlloced(VOID);
    virtual INT32  GetConnId(VOID);
    virtual VOID   SetConnId(INT32 iConnId);
    virtual BOOL   SetConnectStatus(BOOL bIsConnected);
    virtual BOOL   SetAllocStatus(BOOL bIsAlloced);
    virtual BOOL   SetConnInUseStatus(BOOL bIsInUse);
    virtual BOOL   IsConnInUse(VOID);

    virtual INT32  GetErrCode(VOID);
    virtual CHAR*  GetErrInfo(CHAR* szErrInfo=NULL);
    virtual BYTE   GetErrGrade(PDBRET enErrCode);

    virtual HANDLE GetPDBHandle(VOID);

    PDBRET ExecDDL(CHAR* szTransName, CHAR* szDDLCmd, ...);
    PDBRET ExecDDL(CHAR* szTransName, CHAR *szDDLCmd, va_list* vaMsg);
    BOOL   ExecProcedure(CHAR* szTransName, CHAR *szSQL, CHAR** &pszOutRslt);
    PDBRET CloseProcedure(VOID);
    PDBRET MultiDDLRowFetch(CHAR** &pszQryRslt);

    PDBRET ExecFile(CHAR* szTransName, CHAR* szSQLFile);

    BOOL   IsDBExists(CHAR* szTransName, CHAR* szDBName);
    BOOL   IsTableExists(CHAR* szTransName, CHAR* szDBName, CHAR* szTableName);

    PDBRET CreateDB(CHAR* szTransName, CHAR* szDBName);
    PDBRET UseDB(CHAR* szTransName, CHAR* szDBName);
    PDBRET DeleteDB(CHAR* szTransName, CHAR* szDBName);

private:

    /*连接初始化*/
    PDBRET Init(VOID);

    PDBRET CheckErr(PDBRET enRet);
    PDBRET CheckErrCode(PDBRET Status);
};

class PDBOracleConn : public PDBConnBase
{
public:
    PDBOracleConn(PDBEnvBase *pEnv);
    ~PDBOracleConn(VOID);

    virtual PDBRET Connect(PDB_CONN_PARAM_T *pstConnParam, PDB_OPTION_T *pstDBOption);
    virtual PDBRET Disconnect(VOID);
    virtual BOOL   CheckConn(VOID);
    virtual PDBRET ReConnect(VOID);

    virtual PDBRET Clear(VOID);
    virtual PDBRET InitStmt(VOID);
    virtual PDBRET CloseStmt(VOID);

    virtual PDBRET BindByPos(UINT32 ulType, VOID *pData, INT32 iLen, UINT32 ulInoutType, INT32 *piLenInd);
    virtual VOID   SetBatchExecParam(UINT32 ulSkipLen, UINT32 ulExecNum);

    virtual PDBRET BindRsltByPos(UINT16 usRsltIndex, UINT16 usDataType, VOID *pBuf, UINT32 ulBufLen, BOOL *pbIsNull, INT32 *piLenInd);
    virtual PDBRET OpenCursor(VOID);
    virtual PDBRET Query(CHAR *szTransName, CHAR *szSQL);
    virtual PDBRET BindRslt(VOID);
    virtual PDBRET StoreClientRslt(VOID);
    virtual PDBRET RowFetch(VOID);
    virtual PDBRET FreeRslt(VOID);

    virtual PDBRET Exec(CHAR* szTransName, CHAR* szSQL);
    virtual PDBRET SetPrefetchSize(UINT32 ulPreFetchNum, UINT32 ulPreFetchMem);
    virtual INT32  GetRowCount(VOID);

    virtual PDBRET TransBegin(CHAR* szTransName);
    virtual PDBRET TransCommit(CHAR* szTransName);
    virtual PDBRET TransRollback(CHAR* szTransName);
    virtual CHAR*  GetTransName(VOID);

    virtual BOOL   IsConnected(VOID);
    virtual BOOL   IsAlloced(VOID);
    virtual INT32  GetConnId(VOID);
    virtual VOID   SetConnId(INT32 iConnId);
    virtual BOOL   SetConnectStatus(BOOL bIsConnected);
    virtual BOOL   SetAllocStatus(BOOL bIsAlloced);
    virtual BOOL   SetConnInUseStatus(BOOL bIsInUse);
    virtual BOOL   IsConnInUse(VOID);

    virtual INT32  GetErrCode(VOID);
    virtual CHAR*  GetErrInfo(CHAR* szErrInfo=NULL);
    virtual BYTE   GetErrGrade(PDBRET enErrCode);

    virtual HANDLE GetPDBHandle(VOID);

private:

    /*连接初始化*/
    PDBRET Init(VOID);

    PDBRET CheckErr(PDBRET enRet);
    PDBRET CheckErrCode(PDBRET Status);
};


/*=========================================
Other Functions
=========================================*/
/* 其它宏 */
#define PDB_CHECK_ERR(enRet) \
{\
    if (CheckErr(enRet) != PDB_SUCCESS) \
        return CheckErrCode(enRet);\
}

/* 其它宏 */
#define PDB_CONN_STMT_HANDLE_CHECK(pstPDBHandle)  \
{\
    if ((pstPDBHandle) == NULL ) \
    {\
        return PDB_ERROR;\
    }\
    if ((pstPDBHandle)->stDCIHandle.DBHandles.ConnHandle == NULL || \
        (pstPDBHandle)->stDCIHandle.DBHandles.StmtHandle == NULL ) \
    {\
        (pstPDBHandle)->stDCIHandle.iErrCode = PDB_INVALID_HANDLE;\
        (pstPDBHandle)->stDCIHandle.ulErrLevel = PDB_ERROR_LEVEL_HIGH;\
        return PDB_ERROR;\
    }\
    else\
    {\
        (pstPDBHandle)->stDCIHandle.iErrCode = PDB_SUCCESS;\
        (pstPDBHandle)->stDCIHandle.ulErrLevel = PDB_ERROR_LEVEL_NULL;\
    }\
}

#define PDB_CONN_HANDLE_CHECK(pstPDBHandle)  \
{\
    if ((pstPDBHandle) == NULL ) \
    {\
        return PDB_ERROR;\
    }\
    if ((pstPDBHandle)->stDCIHandle.DBHandles.ConnHandle == NULL) \
    {\
        (pstPDBHandle)->stDCIHandle.iErrCode = PDB_INVALID_HANDLE;\
        (pstPDBHandle)->stDCIHandle.ulErrLevel = PDB_ERROR_LEVEL_HIGH;\
        return PDB_ERROR;\
    }\
    else\
    {\
        (pstPDBHandle)->stDCIHandle.iErrCode = PDB_SUCCESS;\
        (pstPDBHandle)->stDCIHandle.ulErrLevel = PDB_ERROR_LEVEL_NULL;\
    }\
}

#define PDB_ROWFETCH(enRet) \
    ((enRet = RowFetch()) != PDB_NO_DATA && enRet != PDB_ERROR )

#define PDB_ROWFETCH_ERR(enRet) \
    (enRet != PDB_SUCCESS && enRet != PDB_NO_DATA && enRet != PDB_CONTINUE)

#endif
