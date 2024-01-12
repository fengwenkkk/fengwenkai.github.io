/******************************************************************************
模块名          ：  Database Call Interface (DCI)
文件名          ：  pdb_oracle_core.c
相关文件        ：
文件实现功能    ：  数据库底层调用方法，实现不同数据库访问的相同调用界面接口
作者            ：
版本            ：  V0.1
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
******************************************************************************/

#include "g_include.h"

/*#ifdef WIN32
#include <string.h>
#endif
*/

#include "oci.h"

#include "pdb_def.h"
#include "pdb_util.h"
#include "pdb_error.h"
#include "pdb_oracle_core.h"

typedef sword   OCIRET;

#define DCI_HANDLE_CHECK  \
do{\
    if (pstDCIHandle == NULL ) \
    {\
        pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_HIGH;\
        return PDB_INVALID_HANDLE;\
    }\
    if (pstDCIHandle->DBHandles.ConnHandle == NULL|| \
        pstDCIHandle->DBHandles.ServerHandle == NULL || \
        pstDCIHandle->DBHandles.SvcCtxHandle == NULL || \
        pstDCIHandle->DBHandles.ErrorHandle  == NULL )  \
    {\
        pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_HIGH;\
        return PDB_ERROR;\
    }\
    else \
        pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_LOW;\
}while(0)

#define DCI_STATUS_CHECK \
if (iOCIRet != PDB_SUCCESS && iOCIRet != PDB_SUCCESS_WITH_INFO)\
{\
    OracleGetError(pstDCIHandle);\
    OracleDisconnect(pstDCIHandle);\
    pstDCIHandle = NULL;\
    return PDB_ERROR_CREATE_CONN;\
}

#define DCI_RETURN \
{                                                                           \
    if (iOCIRet == OCI_SUCCESS)             return PDB_SUCCESS;             \
    if (iOCIRet == OCI_SUCCESS_WITH_INFO)   return PDB_SUCCESS_WITH_INFO;   \
    if (iOCIRet == OCI_ERROR)               return PDB_ERROR;               \
    if (iOCIRet == OCI_STILL_EXECUTING)     return PDB_STILL_EXECUTING;     \
    if (iOCIRet == OCI_INVALID_HANDLE)      return PDB_INVALID_HANDLE;      \
    if (iOCIRet == OCI_NO_DATA)             return PDB_NO_DATA;             \
    if (iOCIRet == OCI_NEED_DATA)           return PDB_NEED_DATA;           \
    if (iOCIRet == OCI_CONTINUE)            return PDB_CONTINUE;            \
    return PDB_ERROR;                                                       \
}

#define DCI_RETURN_ERR \
if (iOCIRet != PDB_SUCCESS && iOCIRet != PDB_SUCCESS_WITH_INFO) \
{\
    DCI_RETURN;\
}

static UINT32 OracleGetErrorLevel(UINT32 ulErrCode)
{
    UINT32      i;
    UINT32      ulErrNumber = 0;

    if (ulErrCode == 1)
    {
        return PDB_PRIMARYKEY_VIOLATED;
    }

    ulErrNumber = ARRAY_SIZE(g_aulOCIFatalErrorCodes);
    for (i=0; i<ulErrNumber; i++)
    {
        if (ulErrCode == g_aulOCIFatalErrorCodes[i])
        {
            return PDB_ERROR_LEVEL_HIGH;
        }
    }

    ulErrNumber = ARRAY_SIZE(g_aulOCINormalErrorCodes);
    for (i=0; i<ulErrNumber; i++)
    {
        if (ulErrCode == g_aulOCINormalErrorCodes[i])
        {
            return PDB_ERROR_LEVEL_MID;
        }
    }

    return PDB_ERROR_LEVEL_LOW;
}

PDBRET OracleGetError(ORACLE_DCI_HANDLE_T *pstDCIHandle)
{
    CHAR        acErrBuf[PDB_MAX_ERR_LEN] = {0};
    UINT32      ulErrCode;
    OCIRET      iOCIRet;
    UINT32      ulMsgNum = 1;
    UINT32      ulErrLevel;
    UINT32      ulLen = 0;

    /*
    if (szErrorInfo == NULL ||
        pulErrLevel == NULL ||
        usBufferLen == 0 ||
        usBufferLen > PDB_MAX_ERR_LEN )
    {
        return PDB_ERROR;
    }

    memset(szErrorInfo, 0, usBufferLen); */
    pstDCIHandle->acErrInfo[0] = '\0';

    if (pstDCIHandle == NULL)
    {
        pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_HIGH;
        return PDB_INVALID_HANDLE;
    }

    pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_LOW;

    if (pstDCIHandle->DBHandles.ErrorHandle == NULL)
    {
        return PDB_INVALID_HANDLE;
    }

    while(1)
    {
        iOCIRet = OCIErrorGet((dvoid *) pstDCIHandle->DBHandles.ErrorHandle, (ub4) ulMsgNum++, (text *) NULL, &ulErrCode,
                            acErrBuf, (ub4) sizeof(acErrBuf), (ub4) OCI_HTYPE_ERROR);
        if (iOCIRet == OCI_ERROR ||
            iOCIRet == OCI_INVALID_HANDLE ||
            iOCIRet == OCI_NO_DATA)
        {
            break;
        }

        /*get the most serious error*/
        ulErrLevel = OracleGetErrorLevel(ulErrCode);
        if (pstDCIHandle->ulErrLevel < ulErrLevel)
        {
            pstDCIHandle->ulErrLevel = ulErrLevel;
        }

        /*added error code*/
        pstDCIHandle->iErrCode = ulErrCode;

        ulLen += snprintf(pstDCIHandle->acErrInfo+ulLen, sizeof(pstDCIHandle->acErrInfo)-ulLen-1, "%s", acErrBuf);
    }

    if (ulLen > 0)
    {
        return PDB_SUCCESS;
    }

    /* environment handle (for errors on OCIEnvInit(), OCIHandleAlloc()) */
    ulMsgNum = 1;
    while(1)
    {
        iOCIRet = OCIErrorGet((dvoid *) pstDCIHandle->DBHandles.EnvHandle, (ub4) ulMsgNum++, (text *) NULL, &ulErrCode,
                               acErrBuf, (ub4) sizeof(acErrBuf), (ub4) OCI_HTYPE_ERROR);

        if (iOCIRet == OCI_ERROR ||
            iOCIRet == OCI_INVALID_HANDLE ||
            iOCIRet == OCI_NO_DATA)
        {
            break;
        }

        ulErrLevel = OracleGetErrorLevel(ulErrCode);
        if (pstDCIHandle->ulErrLevel < ulErrLevel)
        {
            pstDCIHandle->ulErrLevel = ulErrLevel;
        }

        /*added error code*/
        pstDCIHandle->iErrCode = ulErrCode;

        ulLen += snprintf(pstDCIHandle->acErrInfo+ulLen, sizeof(pstDCIHandle->acErrInfo)-ulLen-1, "%s", acErrBuf);
    }

    return PDB_SUCCESS;
}

static UINT16 OracleGetValueType(UINT32 ulValueType)
{
    if (ulValueType == PDBT_STRING)     return SQLT_STR;
    if (ulValueType == PDBT_INT8)       return SQLT_INT;
    if (ulValueType == PDBT_UINT8)      return SQLT_UIN;
    if (ulValueType == PDBT_INT16)      return SQLT_INT;
    if (ulValueType == PDBT_UINT16)     return SQLT_UIN;
    if (ulValueType == PDBT_INT32)      return SQLT_INT;
    if (ulValueType == PDBT_UINT32)     return SQLT_UIN;
    if (ulValueType == PDBT_FLOAT)      return SQLT_FLT;
    if (ulValueType == PDBT_DOUBLE)     return SQLT_FLT;

    return PDBT_UNKNOW;
}

/**********************************************************************
 函    数: BOOL    PDBCheckPROC
 功    能: 检查数据库连接句柄和指针
 输入参数: PDBPROC  数据库连接指针

 全局变量: 无
 返回结果: TRUE/FALSE
************************************************************************/
BOOL OracleCheckHandle(ORACLE_DCI_HANDLE_T *pstDCIHandle)
{
    if (pstDCIHandle == NULL ||
       //pstHandle->DBHandles.BindHandle   == NULL ||
       //pstHandle->DBHandles.DefineHandle == NULL ||
       pstDCIHandle->DBHandles.ErrorHandle  == NULL ||
       pstDCIHandle->DBHandles.ServerHandle == NULL ||
       pstDCIHandle->DBHandles.ConnHandle== NULL ||
       pstDCIHandle->DBHandles.StmtHandle   == NULL ||
       pstDCIHandle->DBHandles.SvcCtxHandle == NULL /*||
       pstHandle->DBHandles.TransHandle  == NULL */  )
    {
        return FALSE;
    }

    return TRUE;
}

/**********************************************************************
 函    数: DCIInit
 功    能: 初始化DCI调用环境
 输入参数:
 全局变量: 无
 返回结果: PDB_SUCCESS or PDB_ERROR
************************************************************************/
PDBRET OracleInit(HANDLE *phEnv)
{
    OCIRET  iOCIRet = PDB_ERROR;
    static OCIEnv  *pOCIEnv = NULL;

    printf("Oracle Init...\n");

    if (NULL == pOCIEnv)
    {
        iOCIRet = OCIEnvCreate(&pOCIEnv, OCI_THREADED|OCI_OBJECT, NULL, NULL, NULL, NULL, 0, NULL);
        if (iOCIRet != OCI_SUCCESS && iOCIRet != OCI_SUCCESS_WITH_INFO)
        {
            printf("OCIEnvCreate failed: %d\n", iOCIRet);
            return PDB_ERROR;
        }
    }

    printf("Oracle Init OK!\n");

    *phEnv = (HANDLE)pOCIEnv;

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: DCIExit
 功    能: 退出DCI调用环境
 输入参数:
 全局变量:
 返回结果: PDB_SUCCESS
************************************************************************/
PDBRET OracleExit(HANDLE hEnv)
{
    OCITerminate(OCI_DEFAULT);
    return PDB_SUCCESS;
}

/*******************************************************************************
* 函数名称: OracleDiagConnError
* 功    能: 诊断错误原因
* 函数类型: VOID
* 参    数:
* 参数名称        类型              输入/输出         描述
*
* 函数返回:
* 说    明:
*******************************************************************************/
VOID OracleDiagConnError(ORACLE_DCI_HANDLE_T *pstDCIHandle)
{
   /* CHAR        *szErrInfo = NULL;
    UINT32      ulNativeErrCode = 0;

    if (NULL == pstDCIHandle || NULL == pstDCIHandle->DBHandles.ConnHandle)
    {
        return;
    }

    ulNativeErrCode = mysql_errno(pstDCIHandle->DBHandles.ConnHandle);
    pstDCIHandle->ulErrLevel = MySQLGetErrorLevel(ulNativeErrCode);
    szErrInfo = (CHAR*)mysql_error(pstDCIHandle->DBHandles.ConnHandle);

    pstDCIHandle->iErrCode = ulNativeErrCode;

    if (0 == ulNativeErrCode &&
        (NULL == szErrInfo ||
        szErrInfo[0] == '\0'))
    {
        sprintf(pstDCIHandle->acErrInfo, "");
        return;
    }

    if (NULL == szErrInfo)
    {
        sprintf(pstDCIHandle->acErrInfo, "Native error code: %u", ulNativeErrCode);
    }
    else
    {
        strncpy(pstDCIHandle->acErrInfo, szErrInfo, sizeof(pstDCIHandle->acErrInfo)-1);
        pstDCIHandle->acErrInfo[sizeof(pstDCIHandle->acErrInfo)-1] = '\0';
    }

    return; */
}

/*******************************************************************************
* 函数名称: DCIDiagStmtError
* 功    能: 诊断STMT错误
* 函数类型: VOID
* 参    数:
* 参数名称        类型              输入/输出         描述
*
* 函数返回:
* 说    明:
*******************************************************************************/
/*VOID DCIDiagStmtError(ORACLE_DCI_HANDLE_T *pstDCIHandle)
{

    CHAR        *szErrInfo = NULL;
    INT32       ulNativeErrCode = 0;

    if (NULL == pstDCIHandle || NULL == pstDCIHandle->DBHandles.StmtHandle)
    {
        return;
    }

    ulNativeErrCode = mysql_stmt_errno((MYSQL_STMT*)pstDCIHandle->DBHandles.StmtHandle);
    pstDCIHandle->ulErrLevel = MySQLGetErrorLevel(ulNativeErrCode);
    szErrInfo = (CHAR*)mysql_stmt_error((MYSQL_STMT*)pstDCIHandle->DBHandles.StmtHandle);

    pstDCIHandle->iErrCode = ulNativeErrCode;

    if (0 == ulNativeErrCode &&
        (NULL == szErrInfo ||
         szErrInfo[0] == '\0'))
    {
        sprintf(pstDCIHandle->acErrInfo, "");
        return;
    }

    if (NULL == szErrInfo)
    {
        sprintf(pstDCIHandle->acErrInfo, "Native error code: %u", ulNativeErrCode);
    }
    else
    {
        strncpy(pstDCIHandle->acErrInfo, szErrInfo, sizeof(pstDCIHandle->acErrInfo)-1);
        pstDCIHandle->acErrInfo[sizeof(pstDCIHandle->acErrInfo)-1] = '\0';
    }

    return;
}*/

/*******************************************************************************
* 函数名称: SetCommitMode
* 功    能: 设置自动提交模式
* 函数类型: PDBRET
* 参    数:
* 参数名称        类型              输入/输出         描述
* enCommitMode   DCCI_COMMITMODE_E    IN             自动提交模式
* 函数返回:
* 说    明:
*******************************************************************************/
PDBRET OracleSetAutoCommit(ORACLE_DCI_HANDLE_T *pstDCIHandle, BOOL bAutoCommit)
{
    if (bAutoCommit)
    {
        pstDCIHandle->acTransName[0] = '\0';
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: DCIConnect
 功    能: 通过PDB CLI建立数据库连接
 输入参数: pstDCIHandle     --- DCI数据库连接
           szServer         --- 数据库服务名称
           szDBName         --- 数据库名称
           szUserName       --- 用户帐号
           szPassword       --- 用户口令
           pstOption        --- 数据库连接选项
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET OracleConnect
(
    ORACLE_DCI_HANDLE_T *pstDCIHandle,
    PDB_CONN_PARAM_T    *pstConnParam,
    PDB_OPTION_T        *pstOption
)
{
    OCIRET      iOCIRet = OCI_ERROR;
    CHAR        acServer[512];

    if (pstDCIHandle == NULL)
    {
        return PDB_INVALID_HANDLE;
    }

    pstDCIHandle->iErrCode   = PDB_ERROR;
    pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_HIGH;
    memcpy(&pstDCIHandle->stOption, pstOption, sizeof(PDB_OPTION_T));
    memcpy(&pstDCIHandle->stConnParam, pstConnParam, sizeof(PDB_CONN_PARAM_T));

    sprintf(acServer, "(DESCRIPTION=(ADDRESS_LIST=(ADDRESS=(PROTOCOL=TCP)(HOST=%s)(PORT=%u)))(CONNECT_DATA=(SERVICE_NAME=%s)))",
            pstDCIHandle->stConnParam.acServer,
            pstDCIHandle->stConnParam.usPort,
            pstDCIHandle->stConnParam.acDBName);

   // strcpy(acServer,  pstDCIHandle->stConnParam.acDBName);

    /*alloc OCI error handle*/
    iOCIRet = OCIHandleAlloc(pstDCIHandle->DBHandles.EnvHandle, (void **)&pstDCIHandle->DBHandles.ErrorHandle, OCI_HTYPE_ERROR, 0, NULL);
    DCI_STATUS_CHECK;

    /*connect to oracle server*/
    iOCIRet = OCIHandleAlloc(pstDCIHandle->DBHandles.EnvHandle, (void **)&pstDCIHandle->DBHandles.ServerHandle,
                           OCI_HTYPE_SERVER, 0, NULL);
    DCI_STATUS_CHECK;

    iOCIRet = OCIServerAttach(pstDCIHandle->DBHandles.ServerHandle, pstDCIHandle->DBHandles.ErrorHandle,
                              acServer, strlen(acServer), OCI_DEFAULT);
    DCI_STATUS_CHECK;

    /*connect oracle server to ServiceContext*/
    iOCIRet = OCIHandleAlloc(pstDCIHandle->DBHandles.EnvHandle, (void **)&pstDCIHandle->DBHandles.SvcCtxHandle,
                           OCI_HTYPE_SVCCTX, 0, NULL);
    DCI_STATUS_CHECK;

    iOCIRet = OCIAttrSet(pstDCIHandle->DBHandles.SvcCtxHandle, OCI_HTYPE_SVCCTX, pstDCIHandle->DBHandles.ServerHandle,
                       0, OCI_ATTR_SERVER, pstDCIHandle->DBHandles.ErrorHandle);
    DCI_STATUS_CHECK;

    /*alloc a session handle*/
    iOCIRet = OCIHandleAlloc(pstDCIHandle->DBHandles.EnvHandle, (void **)&pstDCIHandle->DBHandles.ConnHandle,
                           OCI_HTYPE_SESSION, 0, NULL);
    DCI_STATUS_CHECK;

    /*set login*/
    iOCIRet = OCIAttrSet(pstDCIHandle->DBHandles.ConnHandle, OCI_HTYPE_SESSION, pstDCIHandle->stConnParam.acUserName, strlen(pstDCIHandle->stConnParam.acUserName),
                       OCI_ATTR_USERNAME, pstDCIHandle->DBHandles.ErrorHandle);
    DCI_STATUS_CHECK;

    /*set password*/
    iOCIRet = OCIAttrSet(pstDCIHandle->DBHandles.ConnHandle, OCI_HTYPE_SESSION, pstDCIHandle->stConnParam.acPassword, strlen(pstDCIHandle->stConnParam.acPassword),
                       OCI_ATTR_PASSWORD, pstDCIHandle->DBHandles.ErrorHandle);
    DCI_STATUS_CHECK;

    /*oracle session begin*/
    iOCIRet = OCISessionBegin(pstDCIHandle->DBHandles.SvcCtxHandle, pstDCIHandle->DBHandles.ErrorHandle,
                            pstDCIHandle->DBHandles.ConnHandle, OCI_CRED_RDBMS, OCI_DEFAULT);
    DCI_STATUS_CHECK;

    /*connect oracle session to service context*/
    iOCIRet = OCIAttrSet(pstDCIHandle->DBHandles.SvcCtxHandle, OCI_HTYPE_SVCCTX, pstDCIHandle->DBHandles.ConnHandle, 0,
                       OCI_ATTR_SESSION, pstDCIHandle->DBHandles.ErrorHandle);
    DCI_STATUS_CHECK;

    iOCIRet = OCIHandleAlloc(pstDCIHandle->DBHandles.EnvHandle, (void **)&pstDCIHandle->DBHandles.StmtHandle, OCI_HTYPE_STMT, 0, NULL);
    DCI_STATUS_CHECK;

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: DCIStmtInit
 功    能: 通过PDB CLI建立数据库连接的语句句柄
 输入参数: pstDCIHandle     --- DCI数据库连接
 全局变量:
 返回结果: PDB_SUCCESS or PDB_ERROR_CREATE_STMT or PDB_ERROR
************************************************************************/
PDBRET OracleInitStmt(ORACLE_DCI_HANDLE_T *pstDCIHandle)
{
    OCIRET      iOCIRet = OCI_ERROR;

    DCI_HANDLE_CHECK;

    if (pstDCIHandle->DBHandles.StmtHandle)
    {
        return PDB_SUCCESS;
    }

    iOCIRet = OCIHandleAlloc(pstDCIHandle->DBHandles.EnvHandle,
                             (void **)&pstDCIHandle->DBHandles.StmtHandle, OCI_HTYPE_STMT, 0, NULL);

    DCI_RETURN; // PDB_ERROR_CREATE_STMT
}

/**********************************************************************
 函    数: DCIStmtClose
 功    能: 通过PDB CLI关闭数据库连接的语句句柄
 输入参数: pstDCIHandle     --- DCI数据库连接
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET OracleCloseStmt(ORACLE_DCI_HANDLE_T *pstDCIHandle)
{
    if (pstDCIHandle == NULL)
    {
        return PDB_INVALID_HANDLE;
    }

    if (pstDCIHandle->DBHandles.StmtHandle)
    {
        OCIHandleFree(pstDCIHandle->DBHandles.StmtHandle, OCI_HTYPE_STMT);
        pstDCIHandle->DBHandles.StmtHandle = NULL;
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: PDBRET OracleDisconnect(ORACLE_DCI_HANDLE_T* pstDCIHandle)
 功    能: 关闭数据库连接
 输入参数: pstDCIHandle --- PDB CLI数据库连接
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR or PDB_INVALID_HANDLE
************************************************************************/
PDBRET OracleDisconnect(ORACLE_DCI_HANDLE_T* pstDCIHandle)
{
    OCIRET    iOCIRet = OCI_ERROR;

    if (pstDCIHandle == NULL)
    {
        return PDB_SUCCESS;
    }

    pstDCIHandle->iErrCode = 0;
    pstDCIHandle->acSQLCmd[0] = '\0';

    if (pstDCIHandle->DBHandles.StmtHandle != NULL)
    {
        OCIHandleFree(pstDCIHandle->DBHandles.StmtHandle, OCI_HTYPE_STMT);
        pstDCIHandle->DBHandles.StmtHandle = NULL;
    }

    if (pstDCIHandle->DBHandles.ConnHandle)
    {
        iOCIRet = OCISessionEnd(pstDCIHandle->DBHandles.SvcCtxHandle,
                        pstDCIHandle->DBHandles.ErrorHandle,
                        pstDCIHandle->DBHandles.ConnHandle,
                        OCI_DEFAULT);  /* 结束OCI连接上的OCI会话Session */
        DCI_RETURN_ERR;

        iOCIRet = OCIHandleFree(pstDCIHandle->DBHandles.ConnHandle, OCI_HTYPE_SESSION);
        DCI_RETURN_ERR;

        pstDCIHandle->DBHandles.ConnHandle = NULL;
    }

    if (pstDCIHandle->DBHandles.ServerHandle)
    {
        iOCIRet = OCIServerDetach(pstDCIHandle->DBHandles.ServerHandle,
                                pstDCIHandle->DBHandles.ErrorHandle,
                                OCI_DEFAULT); /* 中断数据库连接 */

        DCI_RETURN_ERR;

        iOCIRet = OCIHandleFree(pstDCIHandle->DBHandles.ServerHandle, OCI_HTYPE_SERVER);
        DCI_RETURN_ERR;

        pstDCIHandle->DBHandles.ServerHandle = NULL;
    }

    if (pstDCIHandle->DBHandles.SvcCtxHandle)
    {
        iOCIRet = OCIHandleFree(pstDCIHandle->DBHandles.SvcCtxHandle, OCI_HTYPE_SVCCTX);
        DCI_RETURN_ERR;

        pstDCIHandle->DBHandles.SvcCtxHandle = NULL;
    }

    if (pstDCIHandle->DBHandles.ErrorHandle)
    {
        iOCIRet = OCIHandleFree(pstDCIHandle->DBHandles.ErrorHandle, OCI_HTYPE_ERROR);
        DCI_RETURN_ERR;

        pstDCIHandle->DBHandles.ErrorHandle = NULL;
    }

    if (pstDCIHandle->DBHandles.EnvHandle)
    {
        OCIHandleFree(pstDCIHandle->DBHandles.EnvHandle, OCI_HTYPE_ENV);
        pstDCIHandle->DBHandles.EnvHandle = NULL;
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: DCISQLPrepare
 功    能: 预编译SQL语句
 输入参数: pstDCIHandle     --- PDB CLI数据库连接类型
           hStmtHandle      --- 语句句柄
           szStatementText  --- SQL语句
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET OracleSQLPrepare(ORACLE_DCI_HANDLE_T *pstDCIHandle)
{
    OCIRET    iOCIRet = OCI_ERROR;

    DCI_HANDLE_CHECK;

    //FormatOraSql(szStatementText, pstDCIHandle->acTempSQL);
    //StrConvert(szStatementText, pstDCIHandle->acOutSQL, pstDCIHandle->aucTmpBuffer);
    iOCIRet = OCIStmtPrepare(pstDCIHandle->DBHandles.StmtHandle,
                             pstDCIHandle->DBHandles.ErrorHandle,
                             pstDCIHandle->acSQLCmd,
                             strlen(pstDCIHandle->acSQLCmd),
                             OCI_NTV_SYNTAX, OCI_DEFAULT);

    DCI_RETURN;
}

/*******************************************************************************
* 函数名称: SetPrefetchNum
* 功    能: 设置预提取行数
* 函数类型: PDBRET
* 参    数:
* 参数名称        类型              输入/输出         描述
* ulRowNum       DCCIUINT32           IN             行数
* 函数返回:
* 说    明:
*******************************************************************************/
PDBRET OracleSetPrefetchNum(ORACLE_DCI_HANDLE_T* pstDCIHandle, UINT32 ulRowNum)
{
    /* 检查ulRowNum是否等于0 */
    if (0 == ulRowNum)
    {
        return PDB_ERROR_SET_ATTR;
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: DCIQuery
 功    能: 通过PDB CLI执行已准备好的SQL查询语句
 输入参数: pstDCIHandle --- PDB CLI数据库连接类型
           hStmtHandle  --- 语句句柄
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET OracleQuery(ORACLE_DCI_HANDLE_T* pstDCIHandle)
{
    return OracleExec(pstDCIHandle);
}

static UINT16 OracleSQLType(ORACLE_DCI_HANDLE_T* pstDCIHandle)
{
    OCIRET      iOCIRet;
    UINT16      usSQLType;

    if (pstDCIHandle == NULL)
    {
        return PDB_SQL_UNKNOWN;
    }

    iOCIRet = OCIAttrGet(pstDCIHandle->DBHandles.StmtHandle, OCI_HTYPE_STMT, &usSQLType, NULL,
                       OCI_ATTR_STMT_TYPE, pstDCIHandle->DBHandles.ErrorHandle);
    if (iOCIRet != OCI_SUCCESS && iOCIRet != OCI_SUCCESS_WITH_INFO)
    {
        return PDB_SQL_UNKNOWN;
    }

    if (usSQLType == OCI_STMT_SELECT)   return PDB_SQL_SELECT;
    if (usSQLType == OCI_STMT_UPDATE)   return PDB_SQL_UPDATE;
    if (usSQLType == OCI_STMT_DELETE)   return PDB_SQL_DELETE;
    if (usSQLType == OCI_STMT_INSERT)   return PDB_SQL_INSERT;
    if (usSQLType == OCI_STMT_CREATE)   return PDB_SQL_CREATE;
    if (usSQLType == OCI_STMT_DROP)     return PDB_SQL_DROP;
    if (usSQLType == OCI_STMT_ALTER)    return PDB_SQL_ALTER;
    if (usSQLType == OCI_STMT_BEGIN)    return PDB_SQL_BEGIN;
    if (usSQLType == OCI_STMT_DECLARE)  return PDB_SQL_DECLARE;

    return PDB_SQL_UNKNOWN;
}

/**********************************************************************
 函    数: DCIExec
 功    能: 通过PDB CLI执行已准备好的非查询类SQL语句
 输入参数: pstDCIHandle --- PDB CLI数据库连接类型
           hStmtHandle  --- 语句句柄
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET OracleExec(ORACLE_DCI_HANDLE_T* pstDCIHandle)
{
    OCIRET      iOCIRet = OCI_ERROR;
    OCIBind     *pBindHandle = NULL;
    UINT32      ulExecMode;
    UINT16      usIters;
    UINT32      i;

    DCI_HANDLE_CHECK;

    if (pstDCIHandle->acTransName[0] == '\0')
    {
        ulExecMode = OCI_COMMIT_ON_SUCCESS; /*当前操作为离散操作*/
    }
    else
    {
        ulExecMode = OCI_DEFAULT; /*当前操作处于事务序列中*/
    }

    if (OracleSQLType(pstDCIHandle) == PDB_SQL_SELECT)
    {
        ulExecMode = OCI_DEFAULT;
        usIters = 0;
    }
    else
    {
        usIters = pstDCIHandle->ulExecNum;//stOption.usBatchNum;
        if (usIters == 0)
        {
            usIters = 1;
        }

        if (usIters > 1) /* 只有在操作多条记录时才使用批量错误模式 */
        {
            ulExecMode |= OCI_BATCH_ERRORS;
        }
    }

    for (i=0; i<pstDCIHandle->ulBindNum; i++)
    {
        iOCIRet = OCIBindByPos(pstDCIHandle->DBHandles.StmtHandle,
                         &pBindHandle,
                         pstDCIHandle->DBHandles.ErrorHandle,
                         i+1,
                         pstDCIHandle->astBindInfo[i].pvData,
                         pstDCIHandle->astBindInfo[i].ulDataLen,
                         pstDCIHandle->astBindInfo[i].usDataType,
                         NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
        DCI_RETURN_ERR;

        if (usIters > 1)
        {
            iOCIRet = OCIBindArrayOfStruct(pBindHandle, pstDCIHandle->DBHandles.ErrorHandle, pstDCIHandle->ulBindSkipLen, 0,0,0);
            DCI_RETURN_ERR;
        }
    }

    if (usIters == 0) /*表示是查询语句*/
    {
        /* Set prefetch row number */
        iOCIRet = OCIAttrSet(pstDCIHandle->DBHandles.StmtHandle,
                            OCI_HTYPE_STMT,
                            &pstDCIHandle->stOption.ulPrefetchRow, 0,
                            OCI_ATTR_PREFETCH_ROWS,
                            pstDCIHandle->DBHandles.ErrorHandle);
        DCI_RETURN_ERR;

        for (i=0; i<pstDCIHandle->ulBindRsltNum; i++)
        {
            iOCIRet = OracleBindRsltByPos(pstDCIHandle, (UINT16)i,
                                pstDCIHandle->astBindRsltInfo[i].usDataType,
                                pstDCIHandle->astBindRsltInfo[i].pvData,
                                pstDCIHandle->astBindRsltInfo[i].ulDataLen,
                                pstDCIHandle->astBindRsltInfo[i].pbIsNull,
                                pstDCIHandle->apiLenInd[i]);
            DCI_RETURN_ERR;
        }
    }

    iOCIRet = OCIStmtExecute(pstDCIHandle->DBHandles.SvcCtxHandle, pstDCIHandle->DBHandles.StmtHandle,
                             pstDCIHandle->DBHandles.ErrorHandle, usIters, 0, NULL, NULL, ulExecMode);

    DCI_RETURN;
}

/**********************************************************************
 函    数: DCIBindRsltByPos
 功    能: 通过PDB CLI执行以准备好的SQL语句
 输入参数: pstDCIHandle --- PDB CLI数据库连接
           usRsltIndex  --- 绑定结果集的列index
           usRsltType   ---  绑定结果集的列对应外部数据类型
           pRsltBuf     --- 绑定结果集的列缓冲区长度指针
           ulRsltBufLen --- 绑定结果集的列缓冲区长度指针
           pucNull      --- 标识字段是否为空的列数组指针
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET OracleBindRsltByPos(ORACLE_DCI_HANDLE_T  *pstDCIHandle,
                          UINT16                usRsltIndex,
                          UINT16                usRsltType,
                          VOID                  *pRsltBuf,
                          UINT32                ulRsltBufLen,
                          BOOL                  *pbIsNull,
                          INT32                 *piLenInd)
{
    OCIRET          iOCIRet;
    UINT16          usOCIValueType;
    static UINT16   usColCode = 0;
    static INT16    sIndVar = 0;

    if (/*0 == usRsltIndex || */usRsltIndex > PDB_MAX_BIND_NUM)
    {
        return PDB_ERROR_BIND;
    }

    /* 参数有效性检查 */
    if (NULL == pRsltBuf)
    {
        return PDB_ERROR_PARA;
    }

    DCI_HANDLE_CHECK;

    usOCIValueType = OracleGetValueType(usRsltType);
    iOCIRet = OCIDefineByPos(pstDCIHandle->DBHandles.StmtHandle,
                           &pstDCIHandle->DBHandles.DefineHandle,
                           pstDCIHandle->DBHandles.ErrorHandle,
                           usRsltIndex+1, pRsltBuf, ulRsltBufLen,
                           usOCIValueType, &sIndVar, (ub2*)piLenInd, /*&usColCode*/NULL, OCI_DEFAULT);

    //pstDCIHandle->ulBindRsltNum = usRsltIndex;

    //pstDCIHandle->apiLenInd[usRsltIndex-1] = piLenInd;

    DCI_RETURN;
}

/**********************************************************************
 函    数: PDBRET OracleCancel
 功    能: 终止当前正在执行的数据库操作并为下一次数据库操作做好准备
 输入参数: pstDCIHandle --- PDB CLI数据库连接
           hStmtHandle  --- 语句句柄
           Mode         --- Cancel方式 PDB_UNBIND PDB_KEEPBIND
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET OracleCancel(ORACLE_DCI_HANDLE_T *pstDCIHandle, BYTE bMode)
{
    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: DCIStoreRslt
 功    能: 缓存数据库预处理查询返回的结果集到客户端，在DCIBindRslt之后、DCIRowFetch前使用
 输入参数: pstDCIHandle --- PDB CLI数据库连接
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET OracleStoreRslt(ORACLE_DCI_HANDLE_T* pstDCIHandle)
{
 /*   INT32       iRet;

    DCI_CONN_STMT_HANDLE_CHECK;
    iRet = mysql_stmt_store_result(pstDCIHandle->DBHandles.StmtHandle);
    if (iRet != MYSQL_OK)
    {
        DCIDiagStmtError(pstDCIHandle);
        mysql_stmt_free_result(pstDCIHandle->DBHandles.StmtHandle);

        return PDB_ERROR_BIND;
    }*/

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: DCIRowFetch
 功    能: 通过PDB CLI提取数据库查询操作返回的结果集
 输入参数: pstDCIHandle --- PDB CLI数据库连接
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET OracleRowFetch(ORACLE_DCI_HANDLE_T* pstDCIHandle)
{
    OCIRET      iOCIRet;
    UINT32      i;
    INT32       *piLenInd;
    ub2         *psLenInd;

    DCI_HANDLE_CHECK;

    if (pstDCIHandle->ulBindRsltNum == 0)
    {
        return PDB_ERROR_BIND;
    }

    for (i=0; i<pstDCIHandle->ulBindRsltNum; i++)
    {
        memset(pstDCIHandle->astBindRsltInfo[i].pvData, 0, pstDCIHandle->astBindRsltInfo[i].ulDataLen);
    }

    iOCIRet = OCIStmtFetch(pstDCIHandle->DBHandles.StmtHandle,
                         pstDCIHandle->DBHandles.ErrorHandle, 1,
                         OCI_FETCH_NEXT, OCI_DEFAULT);
    DCI_RETURN_ERR;

    pstDCIHandle->ulFetchedNum++;

    for (i=0; i<pstDCIHandle->ulBindRsltNum; i++)
    {
        piLenInd = pstDCIHandle->apiLenInd[i];
        psLenInd = (ub2*)piLenInd;
        *piLenInd = *psLenInd;

        if (pstDCIHandle->astBindRsltInfo[i].usDataType == PDBT_STRING)
        {
            if (*piLenInd == 0 ||
                *pstDCIHandle->astBindRsltInfo[i].pbIsNull)
            {
                *((CHAR*)pstDCIHandle->astBindRsltInfo[i].pvData) = '\0';
            }
        }
    }

    DCI_RETURN;
}

/**********************************************************************
 函    数: DCIBindRslt
 功    能: 释放数据库查询操作返回的结果集，在DCIRowFetch全部结束后使用
 输入参数: pstDCIHandle --- PDB CLI数据库连接
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET OracleFreeRslt(ORACLE_DCI_HANDLE_T* pstDCIHandle)
{
/*  DCI_CONN_STMT_HANDLE_CHECK;

    mysql_stmt_free_result(pstDCIHandle->DBHandles.StmtHandle); */
    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: DCITransBegin
 功    能: 按指定事务名开始一个数据库事务
 输入参数: pstDCIHandle --- PDB CLI数据库连接
           szTransName  --- 事务名称
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET OracleTransBegin(ORACLE_DCI_HANDLE_T *pstDCIHandle, CHAR *szTransName)
{
//    OCIRET        iOCIRet;

    if (szTransName == NULL || szTransName[0] == '\0')
    {
        return PDB_ERROR;
    }

    DCI_HANDLE_CHECK;

    /*不支持嵌套事务*/
    if (pstDCIHandle->acTransName[0] != '\0')
    {
        strcpy(pstDCIHandle->acErrInfo, "Nesting transcation unsupport!");
        pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_LOW;

        return PDB_ERROR;
    }

    /* 事务名称太长 */
    if (strlen(szTransName) >= PDB_MAX_TRANS_LEN)
    {
        strcpy(pstDCIHandle->acErrInfo, "Transaction name too long!");
        pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_LOW;

        return PDB_ERROR;
    }

    strcpy(pstDCIHandle->acTransName, szTransName);

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: DCITransRollback
 功    能: 按指定事务名回滚一个数据库事务
 输入参数: pstDCIHandle --- PDB CLI数据库连接
           szTransName  --- 事务名称
 全局变量: 无
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET OracleTransRollback(ORACLE_DCI_HANDLE_T *pstDCIHandle, CHAR *szTransName)
{
    OCIRET      iOCIRet;

    if (szTransName == NULL || szTransName[0] == '\0' )
    {
        return PDB_ERROR;
    }

    DCI_HANDLE_CHECK;

    /* check the transaction name */
    if (strcmp(szTransName, pstDCIHandle->acTransName) != 0)
    {
        sprintf(pstDCIHandle->acErrInfo, "Transaction %s not exist!", szTransName);
        pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_LOW;

        return PDB_ERROR;
    }

    iOCIRet = OCITransRollback(pstDCIHandle->DBHandles.SvcCtxHandle,
                               pstDCIHandle->DBHandles.ErrorHandle, OCI_DEFAULT);

    pstDCIHandle->acTransName[0] = '\0';

    DCI_RETURN;
}

/**********************************************************************
 函    数: DCITransCommit
 功    能: 按指定事务名提交一个数据库事务
 输入参数: pstDCIHandle --- PDB CLI数据库连接
           szTransName  --- 事务名称
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET OracleTransCommit(ORACLE_DCI_HANDLE_T *pstDCIHandle, CHAR *szTransName)
{
    OCIRET      iOCIRet;

    if (szTransName == NULL || szTransName[0] == '\0' )
    {
        return PDB_ERROR;
    }

    DCI_HANDLE_CHECK;

    /* check the transaction name */
    if (strcmp(szTransName, pstDCIHandle->acTransName) != 0)
    {
        sprintf(pstDCIHandle->acErrInfo, "Tranasction %s not exist!", szTransName);
        pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_HIGH;

        return PDB_ERROR;
    }

    iOCIRet = OCITransCommit(pstDCIHandle->DBHandles.SvcCtxHandle,
                             pstDCIHandle->DBHandles.ErrorHandle, OCI_DEFAULT);

    pstDCIHandle->acTransName[0] = '\0';

    DCI_RETURN;
}

/**********************************************************************
 函    数: MySQLRowCount
 功    能: 获得当前命令影响的记录行数
 输入参数: pstDCIHandle --- PDB CLI数据库连接
           hStmtHandle  --- 语句句柄
 全局变量:
 返回结果: 当前命令影响的记录行数 对于SELECT语句只有当全部数据FETCH后有效
************************************************************************/
INT32 OracleRowCount(ORACLE_DCI_HANDLE_T* pstDCIHandle)
{
    OCIRET      iOCIRet;
    INT32       iRowCount = 0;

    DCI_HANDLE_CHECK;

    iOCIRet = OCIAttrGet(pstDCIHandle->DBHandles.StmtHandle, OCI_HTYPE_STMT,
                         &iRowCount, NULL, OCI_ATTR_ROW_COUNT,
                         pstDCIHandle->DBHandles.ErrorHandle);

    return iRowCount;
}

/**********************************************************************
 函    数: DCIBindPara
 功    能: SQL语句参数绑定
 输入参数: pstDCIHandle     --- 目标数据库连接
           usParaIndex      --- 参数编号，从0开始顺序递增。
           usParaType       --- 该参数的SQL数据类型
           pParaValue       --- 该参数的数据缓冲区的指针(字符串为其长度)
           usParaLen        --- 该数据缓冲区的长度
           pParent          --- 用于BLOB绑定，为父数据区指针
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR or PDB_INVALID_HANDLE
************************************************************************/
PDBRET OracleBindPara(ORACLE_DCI_HANDLE_T   *pstDCIHandle,
                   UINT16       usParaIndex,
                   UINT32       ulParaType,
                   UINT32       ulInOutType,
                   VOID         *pParaValue,
                   UINT32       ulParaLen,
                   INT32        *piLenInd)
{

//    OCIRET                iOCIRet;
    UINT16              usOCIValueType;
    OCI_BIND_INFO_T     *pstBindInfo;
    static INT16        sIndVar = 0;

    DCI_HANDLE_CHECK;

    /*
    if (usParaIndex >= PDB_MAX_BIND_NUM ||
//      pstDCIHandle->ulSendBlobParamNum >= PDB_MAX_BIND_NUM ||
        usParaIndex > ARRAY_SIZE(pstDCIHandle->pavParent))
    {
        return PDB_ERROR_BIND;
    }*/

    /* 参数有效性检查 */
    if (NULL == pParaValue &&
        PDBT_NULL != ulParaType)
    {
        return PDB_ERROR_BIND;
    }

    usOCIValueType = OracleGetValueType(ulParaType);

    pstBindInfo = &pstDCIHandle->astBindInfo[usParaIndex];

    pstBindInfo->pvData = pParaValue;
    pstBindInfo->ulDataLen = ulParaLen;
    pstBindInfo->usDataType = usOCIValueType;

    /*
    iOCIRet = OCIBindByPos(pstDCIHandle->DBHandles.StmtHandle,
                         &pstDCIHandle->DBHandles.BindHandle,
                         pstDCIHandle->DBHandles.ErrorHandle,
                         usParaIndex+1,
                         pParaValue,
                         ulParaLen,
                         usOCIValueType,
                         &sIndVar, NULL, NULL, 0, NULL, OCI_DEFAULT);

    DCI_RETURN; */

    pstDCIHandle->ulBindNum = usParaIndex+1; // 从1开始

    return PDB_SUCCESS;
}

void oci_test()
{
    OCIEnv *myenvhp;       /* the environment handle */
    OCIServer *mysrvhp;    /* the server handle */
    OCIError *myerrhp;     /* the error handle */
    OCISession *myusrhp;   /* user session handle */
    OCISvcCtx *mysvchp;    /* the  service handle */

    char        *server = "orcl";
    char        *user = "system";
    char        *pwd = "Fritt_1957";
    sword ret;

    //server = "(DESCRIPTION=(ADDRESS_LIST=(ADDRESS=(PROTOCOL=TCP)(HOST=192.168.100.58)(PORT=1521)))(CONNECT_DATA=(SERVER=DEDICATED)(SERVICE_NAME=orcl)))";
    //server = "(DESCRIPTION=(ADDRESS_LIST=(ADDRESS=(PROTOCOL=TCP)(HOST=192.168.100.150)(PORT=1521)))(CONNECT_DATA=(SERVER=DEDICATED)(SERVICE_NAME=orcl))";
    //server = "192.168.100.150:1521/orcl";

    printf("OCIEnv Create\n");

    server = "(DESCRIPTION=(ADDRESS_LIST=(ADDRESS=(PROTOCOL=TCP)(HOST=192.168.100.241)(PORT=1521)))(CONNECT_DATA=(SERVICE_NAME=orcl)))";

    /* initialize the mode to be the threaded and object environment */
    ret = OCIEnvCreate(&myenvhp, OCI_THREADED|OCI_OBJECT, (dvoid *)0,
                    0, 0, 0, (size_t) 0, (dvoid **)0);

    if (ret != 0)
    {
        printf("error\n");
    }
    else printf("OCIEnvCreate OK!\n");


    /* allocate a server handle */
    ret = OCIHandleAlloc ((dvoid *)myenvhp, (dvoid **)&mysrvhp,
      OCI_HTYPE_SERVER, 0, (dvoid **) 0);
    if (ret != 0) printf("error1\n");

      /* allocate an error handle */
    ret = OCIHandleAlloc ((dvoid *)myenvhp, (dvoid **)&myerrhp,
      OCI_HTYPE_ERROR, 0, (dvoid **) 0);
    if (ret != 0) printf("error2\n");

      /* create a server context */
    ret = OCIServerAttach (mysrvhp, myerrhp, (text *)server,
      strlen (server), OCI_DEFAULT);
    if (ret != 0) printf("error3\n");

     /* allocate a service handle */
    ret = OCIHandleAlloc ((dvoid *)myenvhp, (dvoid **)&mysvchp,
      OCI_HTYPE_SVCCTX, 0, (dvoid **) 0);
    if (ret != 0) printf("error4\n");

    /* set the server attribute in the service context handle*/
    ret = OCIAttrSet ((dvoid *)mysvchp, OCI_HTYPE_SVCCTX,
       (dvoid *)mysrvhp, (ub4) 0, OCI_ATTR_SERVER, myerrhp);
    if (ret != 0) printf("error5\n");

      /* allocate a user session handle */
    ret = OCIHandleAlloc ((dvoid *)myenvhp, (dvoid **)&myusrhp,
     OCI_HTYPE_SESSION, 0, (dvoid **) 0);
    if (ret != 0) printf("error6\n");

      /* set user name attribute in user session handle */
    ret = OCIAttrSet ((dvoid *)myusrhp, OCI_HTYPE_SESSION,
      (dvoid *)user, (ub4)strlen(user),
      OCI_ATTR_USERNAME, myerrhp);
    if (ret != 0) printf("error7\n");

      /* set password attribute in user session handle */
    ret = OCIAttrSet ((dvoid *)myusrhp, OCI_HTYPE_SESSION,
      (dvoid *)pwd, (ub4)strlen(pwd),
      OCI_ATTR_PASSWORD, myerrhp);
    if (ret != 0) printf("error8\n");

    printf("exit\n");

    return;
}
