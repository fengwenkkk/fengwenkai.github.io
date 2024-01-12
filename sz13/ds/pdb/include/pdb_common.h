#ifndef _PDB_COMMON_H
#define _PDB_COMMON_H

/*=====================================================================
Marco define
=====================================================================*/
#define INVALID_ROWCOUNT (-1)

#define PDB_PREFETCH_ROW_NUM    5000 //50000,100,1024

#define PDB_MAX_CONNECT_NUM     64

#define PDB_BATCH_EXEC_NUM      200 //6000  //20 50

#define PDB_MAX_TABLENUM        1024

#define PDB_NOT_ERR             0   /* 非错误 */
#define PDB_NOR_ERR             1   /* 一般错误 */
#define PDB_IMP_ERR             2   /* 严重错误 */
#define PDB_SYS_ERR             3   /* 系统错误 */

#define PDB_BIND_MARK           '?'

/* SQL语句参数类型标识 */
#define PDB_BINDPARA_MARK       ':'
#define PDB_INPARA_MARK         '%'
#define PDB_OUTPARA_MARK        '@'
#define PDB_REVERSE_MARK        '!'
#define PDB_TAB_MARK            9   /*Tab Key '\t'*/
#define PDB_SPACE_MARK          ' '
#define PDB_LIKE_MARK           '`'

#ifdef PDB_ORACLE
#define PDB_RETPARA_MARK        '#'
#endif

#define PDB_PARTID_FIELD_NAME       "PARTID"

#define PDB_INVALID_PARTID          ((UINT32)0xffffffff)

#define PDB_WAIT_FOREVER    0xFFFFFFFF  /* Infinite timeout */
#define PDB_NO_WAIT         (UINT32)0   /* don't wait */

typedef UINT32  (PDB_PARTID_FUNC)(VOID *pData);

/* MYSQL数据库连接参数信息 */
typedef struct
{
    CHAR                acServer[PDB_MAX_SERVER_LEN];   /* 服务器 */
    UINT32              ulPort;                         /* 端口 */
    CHAR                acDBName[PDB_MAX_DBNAME_LEN];   /* 数据库名 */
    CHAR                acUserName[PDB_MAX_USER_LEN];   /* 用户名 */
    CHAR                acPassword[PDB_MAX_PWD_LEN];    /* 密码 */
    PDB_OPTION_T        stOption;                       /* 数据库选项*/
}PDB_CONNPARA_T;

typedef struct
{
    UINT32              ulPos;
    INT32               iLen;
    UINT32              ulType;
    BOOL                bIsOutPara;
    VOID *              pValue;
}PDB_BIND_PARA_T;

typedef struct
{
    UINT32              ulBindParaNum;
    PDB_BIND_PARA_T     astBindPara[PDB_MAX_BIND_NUM];
}PDB_BIND_INFO_T;

/* 表字段信息 */
typedef struct
{
    CHAR                acFieldName[64];            /* 字段名称 */
    UINT32              ulFieldType;                /* 字段类型 */
    UINT32              ulFieldOffset;              /* 字段相对偏移 */
    UINT32              ulFieldLen;                 /* 字段长度 */
    UINT32              ulFieldStatus;
    UINT32              ulSqlOption;
}PDB_FIELD_INFO_T;

/* 表信息 */
typedef struct
{
    UINT32              ulTableID;                  /* 关系表序号 */
    CHAR                acDBName[64];
    CHAR                acTableName[64];            /* 数据库表名 */
    PDB_PARTID_FUNC     *pfPartIDFunc;              /* 生成分区关键字函数，如果为NULL，则为非分区表 */
    UINT32              ulMaxRecNum;
    UINT32              ulRecLen;                   /* 物理表结构有效长度 */
    UINT32              ulFieldNum;                 /* 字段数量 */
    UINT32              ulKeyFieldNum;              /* 主键字段个数 */
    UINT32              ulFirstKeyIndex;            /* 第一个主键字段序号 */
}PDB_TABLE_INFO_T;

typedef enum
{
    PDB_SQL_SYNTAX_QRY,
    PDB_SQL_SYNTAX_INS,
    PDB_SQL_SYNTAX_MOD,
    PDB_SQL_SYNTAX_DEL,

    PDB_SQL_SYNTAX_MULTI_MOD,
    PDB_SQL_SYNTAX_MULTI_INS,
    PDB_SQL_SYNTAX_MULTI_DEL,

    PDB_SQL_SYNTAX_NUM
}PDB_SQL_SYNTAX_E;

typedef struct
{
    CHAR            *aszSQL[PDB_SQL_SYNTAX_NUM];
}PDB_SQL_SYNTAX_T;

#define PDB_CHECK_TABLE \
{\
    if (ulTableID >= PDB_MAX_TABLEID) return PDB_ERROR;\
}


#define PDB_RETURN(enRet)       \
{                               \
    if((enRet) == PDB_SUCCESS)  \
        return TRUE;            \
    else                        \
        return FALSE;           \
}

/* 数据库语句合成 */
#define PDB_SQL_SELECT_TABLE \
    iOffset = sprintf(szSQL, "SELECT ")

#define PDB_SQL_UPDATE_TABLE \
    iOffset = sprintf(szSQL, "UPDATE %s SET ", pstTableInfo->acTableName);

#define PDB_SQL_DELETE_FROM_TABLE \
    iOffset = sprintf(szSQL, "DELETE FROM %s ", pstTableInfo->acTableName);

#define PDB_SQL_SELECT_FIELD \
{\
    for (ulFieldIndex = 0; ulFieldIndex < pstTableInfo->ulFieldNum; ulFieldIndex ++)\
    {\
        if (ulFieldIndex > 0) iOffset += sprintf(szSQL+iOffset, ","); \
        iOffset += sprintf(szSQL+iOffset, "%s", \
                           pstTableInfo->astField[ulFieldIndex].acFieldName);\
    }\
}

#define PDB_SQL_SELECT_BINDFIELD \
{\
    for (ulFieldIndex = 0; ulFieldIndex < pstTableInfo->ulFieldNum; ulFieldIndex ++)\
    {\
        if (ulFieldIndex > 0) iOffset += sprintf(szSQL+iOffset, "||"); \
        iOffset += sprintf(szSQL+iOffset, "%s", pstTableInfo->astField[ulFieldIndex].acFieldName);\
    }\
}

#define PDB_SQL_INSERT_FIELD \
{\
    iOffset += sprintf(szSQL+iOffset, "(");\
    for (ulFieldIndex = 0; ulFieldIndex < pstTableInfo->ulFieldNum; ulFieldIndex ++)\
    {\
        if (ulFieldIndex > 0) iOffset += sprintf(szSQL+iOffset, ",");\
        iOffset += sprintf(szSQL+iOffset, "%s", pstTableInfo->astField[ulFieldIndex].acFieldName);\
    }\
    iOffset += sprintf(szSQL+iOffset, ") VALUES(");\
    for (ulFieldIndex = 0; ulFieldIndex < pstTableInfo->ulFieldNum; ulFieldIndex ++)\
    {\
        if (ulFieldIndex > 0) iOffset += sprintf(szSQL+iOffset, ",");\
        iOffset += sprintf(szSQL+iOffset, "%s", BindMark());\
    }\
    iOffset += sprintf(szSQL+iOffset, ")");\
}


#define PDB_SQL_UPDATE_FIELD \
{\
    for (ulFieldIndex=0; ulFieldIndex<pstTableInfo->ulFieldNum; ulFieldIndex++) \
    {   \
        if (ulFieldIndex >= pstTableInfo->ulFirstKeyIndex &&    \
            ulFieldIndex < pstTableInfo->ulFirstKeyIndex + pstTableInfo->ulKeyFieldNum) \
        {   \
            continue;   \
        }   \
        if (ulFieldIndex > pstTableInfo->ulKeyFieldNum) iOffset += sprintf(szSQL+iOffset, ","); \
        iOffset += sprintf(szSQL+iOffset, "%s=%s", \
                           pstTableInfo->astField[ulFieldIndex].acFieldName,\
                           BindMark());\
    }\
}


#define PDB_SQL_FROM_TABLE \
    iOffset += sprintf(szSQL+iOffset, " FROM %s", pstTableInfo->acTableName)

#define PDB_SQL_WHERE_KEY \
{\
    for (ulFieldIndex = 0; ulFieldIndex < pstTableInfo->ulKeyFieldNum; ulFieldIndex ++)\
    {\
        iOffset += sprintf(szSQL+iOffset, (ulFieldIndex==0)?" WHERE ":" AND "); \
        iOffset += sprintf(szSQL+iOffset, "%s=%s", \
                        pstTableInfo->astField[pstTableInfo->ulFirstKeyIndex + ulFieldIndex].acFieldName,\
                        BindMark());\
    }\
}

class PDBConnBase;

/* PDB数据库环境类 */
class PDBEnvBase
{
public:
    PDBEnvBase();

    PDBRET InitEnv(PDB_CONN_PARAM_T *pstConnParam,
                   UINT32   ulPrefetchRow,
                   UINT32   ulTimeout,
                   UINT32   ulConNum);

    UINT32 GetDBType(){return m_ulDBType;};

    virtual PDBRET CreateEnv(VOID)=0;

    /*释放数据库环境*/
    virtual PDBRET FreeEnv(VOID)=0;

    /*建立连接*/
    virtual PDBRET CreateConn(VOID)=0;
    virtual PDBRET FreeConn(UINT32 ulConnID)=0;

    HANDLE GetEnv(VOID);

    /*获取一个未分配的数据库连接*/
    PDBRET AllocConn(VOID **ppConn, INT32 *piConnId);

    PDBRET FreeConn(INT32 iConnId);

    /*释放数据库连接*/
    PDBRET Disconnect(VOID *pConn);

    BOOL CheckConnSet(VOID);
    CHAR* GetCharacterSet(){return m_acCharacterSet;};

    PDBRET Init();

protected:
    UINT32              m_ulDBType;
    CHAR                m_acCharacterSet[32];
    PDBConnBase         *m_pConn[PDB_MAX_CONNECT_NUM];
    UINT32              m_ulConnNum;
    INT32               m_iCheckConnIndex;
    HANDLE              m_hLockId;
    UINT32              m_ulLockTimeout;
    HANDLE              m_hEnv;

};

/*PDB数据库连接类*/
class PDBConnBase
{
public:

    virtual PDBRET Connect(PDB_CONN_PARAM_T *pstConnParam, PDB_OPTION_T *pstDBOption)=0;
    virtual PDBRET Disconnect(VOID)=0;
    virtual BOOL   CheckConn(VOID)=0;
    virtual PDBRET ReConnect(VOID)=0;

    virtual PDBRET Clear(VOID)=0;
    virtual PDBRET InitStmt(VOID)=0;
    virtual PDBRET CloseStmt(VOID)=0;

    virtual PDBRET BindByPos(UINT32 ulType, VOID *pData, INT32 iLen, UINT32 ulInoutType, INT32 *piLenInd)=0;
    virtual VOID   SetBatchExecParam(UINT32 ulSkipLen, UINT32 ulExecNum)=0;

    virtual PDBRET BindRsltByPos(UINT16 usRsltIndex, UINT16 usDataType, VOID *pBuf, UINT32 ulBufLen, BOOL *pbIsNull, INT32 *piLenInd)=0;

    virtual PDBRET OpenCursor(VOID)=0;
    virtual PDBRET Query(CHAR *szTransName, CHAR *szSQL)=0;
    virtual PDBRET BindRslt(VOID)=0;
    virtual PDBRET StoreClientRslt(VOID)=0;
    virtual PDBRET RowFetch(VOID)=0;
    virtual PDBRET FreeRslt(VOID)=0;

    virtual PDBRET Exec(CHAR* szTransName, CHAR* szSQL)=0;
    virtual PDBRET SetPrefetchSize(UINT32 ulPreFetchNum, UINT32 ulPreFetchMem)=0;
    virtual INT32  GetRowCount(VOID)=0;

    virtual PDBRET TransBegin(CHAR * szTransName)=0;
    virtual PDBRET TransCommit(CHAR * szTransName)=0;
    virtual PDBRET TransRollback(CHAR * szTransName)=0;
    virtual CHAR*  GetTransName(VOID)=0;

    virtual BOOL   IsConnected(VOID)=0;
    virtual BOOL   IsAlloced(VOID)=0;
    virtual INT32  GetConnId(VOID)=0;
    virtual VOID   SetConnId(INT32 iConnId)=0;
    virtual BOOL   SetConnectStatus(BOOL bIsConnected)=0;
    virtual BOOL   SetAllocStatus(BOOL bIsAlloced)=0;
    virtual BOOL   SetConnInUseStatus(BOOL bIsInUse)=0;
    virtual BOOL   IsConnInUse(VOID)=0;

    virtual INT32  GetErrCode(VOID)=0;
    virtual CHAR*  GetErrInfo(CHAR* szErrInfo=NULL)=0;
    virtual BYTE   GetErrGrade(PDBRET enErrCode)=0;

    virtual HANDLE GetPDBHandle(VOID)=0;

protected:
    PDBEnvBase          *m_pEnv;
    HANDLE              m_pPDBHandle;

    PDB_CONN_PARAM_T    m_stConnParam;
    PDB_OPTION_T        m_stDBOption;

    HANDLE              m_hConnectLockId;
    HANDLE              m_hAllocLockId;
    HANDLE              m_hConnLockId;
    UINT32              m_ulLockTimeout;
    BOOL                m_bReConnect;

};

#define PDBLog(ulLogLevel, szFormat, ...)      do{ GosLog("PDB", ulLogLevel, szFormat,##__VA_ARGS__);}while(0) ///< 加上 do while(0) 防止，加上 ; 后编译器警告

#endif
