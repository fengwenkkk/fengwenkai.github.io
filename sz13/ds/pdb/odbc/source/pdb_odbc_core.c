/******************************************************************************
模块名          ：  Database Call Interface (DCI)
文件名          ：  pdb_odbc_core.c
相关文件        ：
文件实现功能    ：  数据库底层调用方法，实现不同数据库访问的相同调用界面接口
作者            ：
版本            ：  V0.1
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
******************************************************************************/

#include "g_include.h"

#ifdef WIN32
#include <string.h>
#endif

#include "sql.h"
#include "sqlext.h"

#include "pdb_def.h"
#include "pdb_util.h"
#include "pdb_error.h"
#include "pdb_odbc_core.h"

BYTE        ODBCGetErrorLevel(CHAR *szErrState);

#define ODBC_DCI_CONN_STMT_HANDLE_CHECK  \
do{\
    if (pstDCIHandle == NULL ) \
    {\
        return PDB_INVALID_HANDLE;\
    }\
    if( pstDCIHandle->DBHandles.ConnHandle  == NULL || \
        pstDCIHandle->DBHandles.StmtHandle == NULL )  \
    {\
        pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_HIGH;\
        return PDB_ERROR;\
    }\
    else \
        pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_LOW;\
}while(0)

#define ODBC_DCI_CONN_HANDLE_CHECK  \
do{\
    if (pstDCIHandle == NULL ) \
    {\
        return PDB_INVALID_HANDLE;\
    }\
    if( pstDCIHandle->DBHandles.ConnHandle == NULL)  \
    {\
        pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_HIGH;\
        return PDB_ERROR;\
    }\
    else \
        pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_LOW;\
}while(0)

/**********************************************************************
 函    数: BOOL ODBCCheckHandle
 功    能: 检查数据库连接句柄和指针
 输入参数: PDBPROC  数据库连接指针

 全局变量: 无
 返回结果: TRUE/FALSE
************************************************************************/
/*BOOL ODBCCheckHandle(ODBC_DCI_HANDLE_T *pstDCIHandle)
{
    if(NULL == pstDCIHandle ||
       NULL == pstDCIHandle->DBHandles.ConnHandle ||
       NULL == pstDCIHandle->DBHandles.StmtHandle )
    {
        return FALSE;
    }

    return TRUE;
}*/

/**********************************************************************
 函    数: DCIInit
 功    能: 初始化DCI调用环境
 输入参数:
 全局变量: 无
 返回结果: PDB_SUCCESS or PDB_ERROR
************************************************************************/
PDBRET ODBCInit(HANDLE *phEnv)
{
    HANDLE  hEnv = NULL;

    if (!phEnv)
    {
        PDB_ERROR;
    }

    if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) != SQL_SUCCESS)
    {
        return PDB_ERROR_CREATE_ENV;
    }

    if (SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, SQL_IS_INTEGER) != SQL_SUCCESS)
    {
//        PDBLog(LOG_WARN, "操作系统不支持ODBC3.0");
    }

    *phEnv = hEnv;

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: DCIExit
 功    能: 退出DCI调用环境
 输入参数:
 全局变量:
 返回结果: PDB_SUCCESS
************************************************************************/
PDBRET ODBCExit(HANDLE hEnv)
{
    if (SQLFreeHandle(SQL_HANDLE_ENV, hEnv) != SQL_SUCCESS)
    {
        return PDB_ERROR;
    }

    return PDB_SUCCESS;
}

/*******************************************************************************
* 函数名称: ODBCDiagConnError
* 功    能: 诊断错误原因
* 函数类型: VOID
* 参    数:
* 参数名称        类型              输入/输出         描述
*
* 函数返回:
* 说    明:
*******************************************************************************/
VOID ODBCDiagConnError(ODBC_DCI_HANDLE_T *pstDCIHandle)
{
    CHAR            acErrState[8];
    SQLSMALLINT     sErrMsgLen;
    SQLRETURN       sRet;

    memset(pstDCIHandle->acErrInfo, 0, sizeof(pstDCIHandle->acErrInfo));
    pstDCIHandle->iErrCode = 0;
    pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_NULL;

    if (NULL == pstDCIHandle || NULL == pstDCIHandle->DBHandles.ConnHandle)
    {
        return;
    }

    sRet = SQLGetDiagRec(SQL_HANDLE_DBC, pstDCIHandle->DBHandles.ConnHandle, 1,
                         (SQLCHAR*)acErrState,
                         (SQLINTEGER*)&pstDCIHandle->iErrCode,
                         (SQLCHAR*)pstDCIHandle->acErrInfo,
                         sizeof(pstDCIHandle->acErrInfo),
                         &sErrMsgLen);
    if (SQL_SUCCESS != sRet)
    {
        return;
    }

    pstDCIHandle->ulErrLevel = ODBCGetErrorLevel(acErrState);

    return;
}

/*******************************************************************************
* 函数名称: ODBCDiagStmtError
* 功    能: 诊断STMT错误
* 函数类型: VOID
* 参    数:
* 参数名称        类型              输入/输出         描述
*
* 函数返回:
* 说    明:
*******************************************************************************/
VOID ODBCDiagStmtError(ODBC_DCI_HANDLE_T *pstDCIHandle)
{
    CHAR            acErrState[8];
    SQLSMALLINT     sErrMsgLen;
    SQLRETURN       sRet;

    memset(pstDCIHandle->acErrInfo, 0, sizeof(pstDCIHandle->acErrInfo));
    pstDCIHandle->iErrCode = 0;
    pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_NULL;

    if (NULL == pstDCIHandle || NULL == pstDCIHandle->DBHandles.StmtHandle)
    {
        return;
    }

    sRet = SQLGetDiagRec(SQL_HANDLE_STMT, pstDCIHandle->DBHandles.StmtHandle, 1,
                         (SQLCHAR*)acErrState,
                         (SQLINTEGER*)&pstDCIHandle->iErrCode,
                         (SQLCHAR*)pstDCIHandle->acErrInfo,
                         sizeof(pstDCIHandle->acErrInfo),
                         &sErrMsgLen);
    if (SQL_SUCCESS != sRet)
    {
        return;
    }

    pstDCIHandle->ulErrLevel = ODBCGetErrorLevel(acErrState);

    return;
}

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
PDBRET ODBCSetAutoCommit(ODBC_DCI_HANDLE_T *pstDCIHandle, BOOL bAutoCommit)
{
    SQLRETURN   sRet = -1;
    SQLINTEGER  lAttr = -1;

    if (NULL == pstDCIHandle)
    {
        return PDB_ERROR_SET_ATTR;
    }

    if (bAutoCommit)
    {
        lAttr = SQL_AUTOCOMMIT_ON;
    }
    else
    {
        lAttr = SQL_AUTOCOMMIT_OFF;
    }

    sRet = SQLSetConnectAttr(pstDCIHandle->DBHandles.ConnHandle,
                             SQL_ATTR_AUTOCOMMIT,
                             (SQLPOINTER)lAttr, 0);
    if ((sRet != SQL_SUCCESS) &&
        (sRet != SQL_SUCCESS_WITH_INFO))
    {
        ODBCDiagConnError(pstDCIHandle);
        return PDB_ERROR_SET_ATTR;
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: DCIConnect
 功    能: 通过PDB CLI建立数据库连接
 输入参数: pstDCIHandle     --- DCI数据库连接
           szServer         --- 数据库服务名称
           szUserName       --- 用户帐号
           szPassword       --- 用户口令
           pstOption        --- 数据库连接选项
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET ODBCConnect
(
    ODBC_DCI_HANDLE_T   *pstDCIHandle,
    PDB_CONN_PARAM_T    *pstConnParam,
    PDB_OPTION_T        *pstOption
)
{
    SQLRETURN       sRetCode = -1;
    CHAR            acConnStr[512];
    HANDLE          hConn = NULL;
    UINT32          ulMySLQAllowMultiStmt = 0x4000000;

    if (pstDCIHandle == NULL)
    {
        return PDB_INVALID_HANDLE;
    }

    if (!(pstConnParam->ulDBType & PDBT_ODBC))
    {
        return PDB_ERROR_PARA;
    }

    pstDCIHandle->iErrCode   = PDB_ERROR;
    pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_HIGH;

    //DCIDisconnect(pstDCIHandle);

    memcpy(&pstDCIHandle->stOption, pstOption, sizeof(PDB_OPTION_T));
    memcpy(&pstDCIHandle->stConnParam, pstConnParam, sizeof(PDB_CONN_PARAM_T));

    /* Allocate connection handle */
    pstDCIHandle->DBHandles.ConnHandle = NULL;
    pstDCIHandle->DBHandles.StmtHandle = NULL;

    sRetCode = SQLAllocHandle(SQL_HANDLE_DBC, pstDCIHandle->EnvHandle, &hConn);
    if ((sRetCode != SQL_SUCCESS) &&
        (sRetCode != SQL_SUCCESS_WITH_INFO))
    {
        SQLFreeHandle(SQL_HANDLE_DBC, hConn);
        return PDB_ERROR_CREATE_CONN;
    }

    pstDCIHandle->DBHandles.ConnHandle = hConn;

    /* Set login timeout(seconds). */
    sRetCode = SQLSetConnectAttr(hConn,
                                 SQL_ATTR_CONNECTION_TIMEOUT,
                                 (SQLPOINTER)pstOption->ulTimeout, 0);
    if ((sRetCode != SQL_SUCCESS) &&
        (sRetCode != SQL_SUCCESS_WITH_INFO))
    {
        ODBCDiagConnError(pstDCIHandle);

        SQLFreeHandle(SQL_HANDLE_DBC, hConn);
        pstDCIHandle->DBHandles.ConnHandle = NULL;
        return PDB_ERROR_CREATE_CONN;
    }

    if (pstConnParam->bUseDSN)
    {
        /* 建立连接 */
        sRetCode = SQLConnect(hConn, (SQLCHAR*)pstConnParam->acServer,   SQL_NTS,
                                     (SQLCHAR*)pstConnParam->acUserName, SQL_NTS,
                                     (SQLCHAR*)pstConnParam->acPassword, SQL_NTS);
    }
    else
    {
        /* Connect to data source */
        switch (pstConnParam->ulDBType)
        {
            case (PDBT_MYSQL | PDBT_ODBC):
                sprintf((CHAR*)acConnStr,
                        "DRIVER={MySQL ODBC 5.1 Driver};"
                        "SERVER=%s;PORT=%d;UID=%s;PASSWORD=%s;DATABASE=%s;AllowMultiQueries=true;OPTION=%d",
                        pstConnParam->acServer, pstConnParam->usPort,
                        pstConnParam->acUserName, pstConnParam->acPassword, pstConnParam->acDBName, ulMySLQAllowMultiStmt|3);
                break;

            case PDBT_MSSQL:
                sprintf((CHAR*)acConnStr,
                        "DRIVER={SQL Server};"
                        "SERVER=%s;UID=%s;PWD=%s;DATABASE=%s",
                        pstConnParam->acServer, pstConnParam->acUserName, pstConnParam->acPassword, pstConnParam->acDBName);
                break;

            case (PDBT_ORACLE | PDBT_ODBC):
                sprintf(acConnStr,
                        "DRIVER={Microsoft ODBC for Oracle};"
                        "UID=%s;PWD=%s;SERVER=%s;",
                        pstConnParam->acUserName, pstConnParam->acPassword, pstConnParam->acServer);

                break;

            default:
                SQLFreeHandle(SQL_HANDLE_DBC, hConn);
                pstDCIHandle->DBHandles.ConnHandle = NULL;
                return PDB_ERROR_PARA;
        }

        /* 建立连接 */
        sRetCode = SQLDriverConnect(hConn, 0,
                                    (SQLCHAR*)acConnStr, SQL_NTS,
                                    0, 0, 0,
                                    SQL_DRIVER_NOPROMPT);
    }

    if ((sRetCode != SQL_SUCCESS) &&
        (sRetCode != SQL_SUCCESS_WITH_INFO))
    {
        ODBCDiagConnError(pstDCIHandle);

        SQLFreeHandle(SQL_HANDLE_DBC, hConn);
        pstDCIHandle->DBHandles.ConnHandle = NULL;
        return PDB_ERROR_CREATE_CONN;
    }

    /* 建立STMT句柄 */
    sRetCode = SQLAllocHandle(SQL_HANDLE_STMT, pstDCIHandle->DBHandles.ConnHandle, &pstDCIHandle->DBHandles.StmtHandle);
    if ((sRetCode != SQL_SUCCESS) &&
        (sRetCode != SQL_SUCCESS_WITH_INFO))
    {
        ODBCDiagConnError(pstDCIHandle);

        ODBCCloseStmt(pstDCIHandle);
        SQLFreeHandle(SQL_HANDLE_DBC, pstDCIHandle->DBHandles.ConnHandle);
        pstDCIHandle->DBHandles.ConnHandle = NULL;

        return PDB_ERROR_CREATE_STMT;
    }

    pstDCIHandle->iErrCode = 0;

    return PDB_SUCCESS;
}


/**********************************************************************
 函    数: DCIStmtInit
 功    能: 通过PDB CLI建立数据库连接的语句句柄
 输入参数: pstDCIHandle     --- DCI数据库连接
 全局变量:
 返回结果: PDB_SUCCESS or PDB_ERROR_CREATE_STMT or PDB_ERROR
************************************************************************/
PDBRET ODBCInitStmt(ODBC_DCI_HANDLE_T *pstDCIHandle)
{
    SQLRETURN       sRetCode;

    ODBC_DCI_CONN_HANDLE_CHECK;

    SQLFreeStmt(pstDCIHandle->DBHandles.StmtHandle, SQL_CLOSE);

    if (pstDCIHandle->DBHandles.StmtHandle)
    {
        //ODBCCloseStmt(pstDCIHandle);
        return PDB_SUCCESS;
    }

    sRetCode = SQLAllocHandle(SQL_HANDLE_STMT, pstDCIHandle->DBHandles.ConnHandle, &pstDCIHandle->DBHandles.StmtHandle);
    if ((sRetCode != SQL_SUCCESS) &&
        (sRetCode != SQL_SUCCESS_WITH_INFO))
    {
        ODBCDiagConnError(pstDCIHandle);

        ODBCCloseStmt(pstDCIHandle);
        SQLFreeHandle(SQL_HANDLE_DBC, pstDCIHandle->DBHandles.ConnHandle);
        pstDCIHandle->DBHandles.ConnHandle = NULL;

        return PDB_ERROR_CREATE_STMT;
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: DCIStmtClose
 功    能: 通过PDB CLI关闭数据库连接的语句句柄
 输入参数: pstDCIHandle     --- DCI数据库连接
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET ODBCCloseStmt(ODBC_DCI_HANDLE_T *pstDCIHandle)
{
    if (pstDCIHandle == NULL)
    {
        return PDB_INVALID_HANDLE;
    }

    if (pstDCIHandle->DBHandles.StmtHandle)
    {
        SQLCloseCursor(pstDCIHandle->DBHandles.StmtHandle);
        SQLFreeHandle(SQL_HANDLE_STMT, pstDCIHandle->DBHandles.StmtHandle);

        pstDCIHandle->DBHandles.StmtHandle = NULL;
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: PDBRET ODBCDisconnect(ODBC_DCI_HANDLE_T* pstDCIHandle)
 功    能: 关闭数据库连接
 输入参数: pstDCIHandle --- PDB CLI数据库连接
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR or PDB_INVALID_HANDLE
************************************************************************/
PDBRET ODBCDisconnect(ODBC_DCI_HANDLE_T* pstDCIHandle)
{
    SQLRETURN   sRetCode;

    if (pstDCIHandle == NULL)
    {
        return PDB_SUCCESS;
    }

    pstDCIHandle->iErrCode = 0;
    pstDCIHandle->acSQLCmd[0] = '\0';

    /* 检查是否有STMT句柄 */
    if (NULL != pstDCIHandle->DBHandles.StmtHandle)
    {
        SQLCloseCursor(pstDCIHandle->DBHandles.StmtHandle);
        sRetCode = SQLFreeHandle(SQL_HANDLE_STMT, pstDCIHandle->DBHandles.StmtHandle);
        if ((sRetCode != SQL_SUCCESS) &&
            (sRetCode != SQL_SUCCESS_WITH_INFO))
        {
            ODBCDiagConnError(pstDCIHandle);
            return PDB_ERROR_FREE_STMT;
        }

        pstDCIHandle->DBHandles.StmtHandle = NULL;
    }

    /* 如果连接句柄为空 */
    if (NULL == pstDCIHandle->DBHandles.ConnHandle)
    {
        return PDB_SUCCESS;
    }

    /* 释放连接 */
    sRetCode = SQLDisconnect(pstDCIHandle->DBHandles.ConnHandle);
    if (sRetCode != SQL_SUCCESS)
    {
        ODBCDiagConnError(pstDCIHandle);
        return PDB_ERROR_FREE_STMT;
    }

    /* 释放句柄 */
    sRetCode = SQLFreeHandle(SQL_HANDLE_DBC, pstDCIHandle->DBHandles.ConnHandle);
    if ((sRetCode != SQL_SUCCESS) &&
        (sRetCode != SQL_SUCCESS_WITH_INFO))
    {
        ODBCDiagConnError(pstDCIHandle);
        return PDB_ERROR_FREE_STMT;
    }

    pstDCIHandle->DBHandles.ConnHandle = NULL;

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
PDBRET ODBCSQLPrepare(ODBC_DCI_HANDLE_T *pstDCIHandle)
{
    SQLRETURN   sRet = -1;

    ODBC_DCI_CONN_STMT_HANDLE_CHECK;

    //if (pstDCIHandle->ulBindNum > 0)
    {
        sRet = SQLPrepare(pstDCIHandle->DBHandles.StmtHandle, (SQLCHAR*)pstDCIHandle->acSQLCmd, SQL_NTS);
        if ((sRet != SQL_SUCCESS) &&
            (sRet != SQL_SUCCESS_WITH_INFO))
        {
            ODBCDiagStmtError(pstDCIHandle);

            return PDB_ERROR_STMT_PREPARE;
        }
    }

    return PDB_SUCCESS;
}

/*******************************************************************************
* 函数名称: ODBCSetStmtAttr
* 功    能: 设置STMT属性
* 函数类型: PDBRET
* 参    数:
* 参数名称        类型              输入/输出         描述
* ulRowNum       DCCIUINT32           IN             行数
* 函数返回:
* 说    明:
*******************************************************************************/
PDBRET ODBCSetStmtAttr(ODBC_DCI_HANDLE_T* pstDCIHandle, PDB_STMT_ATTR_E enAttr, VOID *pvValue, UINT32 ulStrLen)
{
    SQLRETURN           sRet = -1;
    SQLINTEGER          lAttr = -1;

    /* 指针检查 */
    if (NULL == pvValue)
    {
        return PDB_ERROR_SET_ATTR;
    }

    switch(enAttr)
    {
        case ODBC_ATTR_PARAM_BIND_OFFSET_PTR:
            lAttr = SQL_ATTR_PARAM_BIND_OFFSET_PTR;
            break;

        case ODBC_ATTR_PARAM_BIND_TYPE:
            lAttr = SQL_ATTR_PARAM_BIND_TYPE;
            break;

        case ODBC_ATTR_PARAM_OPERATION_PTR:
            lAttr = SQL_ATTR_PARAM_OPERATION_PTR;
            break;

        case ODBC_ATTR_PARAM_STATUS_PTR:
            lAttr = SQL_ATTR_PARAM_STATUS_PTR;
            break;

        case ODBC_ATTR_PARAMS_PROCESSED_PTR:
            lAttr = SQL_ATTR_PARAMS_PROCESSED_PTR;
            break;

        case ODBC_ATTR_PARAMSET_SIZE:
            lAttr = SQL_ATTR_PARAMSET_SIZE;
            break;

        case ODBC_ATTR_ROW_ARRAY_SIZE:
            lAttr = SQL_ATTR_ROW_ARRAY_SIZE;
            break;

        case ODBC_ATTR_ROW_BIND_OFFSET_PTR:
            lAttr = SQL_ATTR_ROW_BIND_OFFSET_PTR;
            break;

        case ODBC_ATTR_ROW_BIND_TYPE:
            lAttr = SQL_ATTR_ROW_BIND_TYPE;
            break;

        case ODBC_ATTR_ROW_OPERATION_PTR:
            lAttr = SQL_ATTR_ROW_OPERATION_PTR;
            break;

        case ODBC_ATTR_ROW_STATUS_PTR:
            lAttr = SQL_ATTR_ROW_STATUS_PTR;
            break;

        case ODBC_ATTR_ROWS_FETCHED_PTR:
            lAttr = SQL_ATTR_ROWS_FETCHED_PTR;
            break;

        default:
            return PDB_ERROR_SET_ATTR;
    }

    sRet = SQLSetStmtAttr(pstDCIHandle->DBHandles.StmtHandle, lAttr, (SQLPOINTER)pvValue, ulStrLen);
    if ((sRet != SQL_SUCCESS) &&
        (sRet != SQL_SUCCESS_WITH_INFO))
    {
        ODBCDiagStmtError(pstDCIHandle);

        return PDB_ERROR_SET_ATTR;
    }

/*    if (ODBC_ATTR_PARAM_BIND_TYPE == enAttr)
    {
        SetSkipLen(*(DCCIUINT16 *)pvValue);
    }

    if (ODBC_ATTR_ROW_BIND_TYPE == enAttr)
    {
        SetRsltSkipLen(*(DCCIUINT16 *)pvValue);
    }
*/

    return PDB_SUCCESS;
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
PDBRET ODBCSetPrefetchNum(ODBC_DCI_HANDLE_T* pstDCIHandle, UINT32 ulRowNum)
{
    PDBRET    enRet;

    if (0 == ulRowNum)
    {
        return PDB_ERROR_SET_ATTR;
    }

    enRet = ODBCSetStmtAttr(pstDCIHandle, ODBC_ATTR_ROW_ARRAY_SIZE, (VOID*)&ulRowNum, sizeof(ulRowNum));
    if (PDB_SUCCESS != enRet)
    {
        return enRet;
    }

    pstDCIHandle->stOption.ulPrefetchRow = ulRowNum;

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
PDBRET ODBCQuery(ODBC_DCI_HANDLE_T* pstDCIHandle)
{
    SQLRETURN   sRet = -1;

    ODBC_DCI_CONN_STMT_HANDLE_CHECK;

    pstDCIHandle->ulFetchedNum = 0;

    /*
    if (pstDCIHandle->ulBindNum > 0)
    {
        if (MYSQL_OK != mysql_stmt_bind_param(hStmtHandle->MySQLStmtHandle, pstDCIHandle->astBindHandle))
        {
            ODBCDiagStmtError(pstDCIHandle);

            return PDB_ERROR_BIND;
        }
    }*/

    /* 发送大数据blob */
/*  if (PDB_SUCCESS != DCISendBlobData(pstDCIHandle, hStmtHandle))
    {
         return PDB_ERROR_BIND;
    } */

    // 光标方式
    /*
    if (PDB_SUCCESS != DCISetPrefetchNum(hStmtHandle, pstDCIHandle->stOption.ulPrefetchRow))
    {
        return PDB_ERROR_BIND;
    }
    */

    sRet = SQLExecute(pstDCIHandle->DBHandles.StmtHandle);
    if ((sRet != SQL_SUCCESS) &&
        (sRet != SQL_SUCCESS_WITH_INFO))
    {
        ODBCDiagStmtError(pstDCIHandle);

        return PDB_ERROR_EXEC;
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: DCIExec
 功    能: 通过PDB CLI执行已准备好的非查询类SQL语句
 输入参数: pstDCIHandle --- PDB CLI数据库连接类型
           hStmtHandle  --- 语句句柄
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET ODBCExec(ODBC_DCI_HANDLE_T* pstDCIHandle)
{
    SQLRETURN   sRet = -1;

    ODBC_DCI_CONN_STMT_HANDLE_CHECK;

    pstDCIHandle->ulExecErrCount = 0;

    /*
    if (pstDCIHandle->ulBindNum > 0)
    {
        if (MYSQL_OK != mysql_stmt_bind_param(hStmtHandle->MySQLStmtHandle, pstDCIHandle->astBindHandle))
        {
            ODBCDiagStmtError(pstDCIHandle);
            pstDCIHandle->ulExecErrCount ++;

            return PDB_ERROR_BIND;
        }
    }*/

    /* 发送大数据blob */
/*  if (PDB_SUCCESS != DCISendBlobData(pstDCIHandle, hStmtHandle))
    {
        pstDCIHandle->ulExecErrCount ++;
        return PDB_ERROR_BIND;
    }*/

    sRet = SQLExecute(pstDCIHandle->DBHandles.StmtHandle);
    /*
    if (pstDCIHandle->ulBindNum > 0)
    {
        sRet = SQLExecute(pstDCIHandle->DBHandles.StmtHandle);
    }
    else
    {
        sRet = SQLExecDirect(pstDCIHandle->DBHandles.StmtHandle, pstDCIHandle->acSQLCmd, SQL_NTS);
    }*/

    if ((sRet != SQL_SUCCESS) &&
        (sRet != SQL_SUCCESS_WITH_INFO) &&
        (sRet != SQL_NO_DATA))
    {
        ODBCDiagStmtError(pstDCIHandle);

        return PDB_ERROR_EXEC;
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: DCIBindRsltByPos
 功    能: 通过PDB CLI执行以准备好的SQL语句
 输入参数: pstDCIHandle --- PDB CLI数据库连接
           usRsltIndex  --- 绑定结果集的列index
           usRsltType   ---  绑定结果集的列对应外部数据类型
           pRsltBuf     --- 绑定结果集的列缓冲区长度指针
           ulRsltBufLen --- 绑定结果集的列缓冲区长度
           piLenInd     --- 长度或标识
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET ODBCBindRsltByPos(ODBC_DCI_HANDLE_T    *pstDCIHandle,
                        UINT16          usRsltIndex,
                        UINT16          usRsltType,
                        VOID            *pRsltBuf,
                        UINT32          ulRsltBufLen,
                        BOOL            *pbIsNull,
                        INT32           *piLenInd)
{
    SQLRETURN       sRet = -1;
    SQLSMALLINT     sTargetType;

    ODBC_DCI_CONN_STMT_HANDLE_CHECK;

    if (0 == usRsltIndex || usRsltIndex > PDB_MAX_BIND_NUM)
    {
        return PDB_ERROR_BIND;
    }

    if (NULL == pRsltBuf)
    {
        return PDB_ERROR_PARA;
    }

    switch(usRsltType)
    {
        case PDBT_UINT8:
            sTargetType = SQL_C_UTINYINT;
            break;

        case PDBT_UINT16:
            sTargetType = SQL_C_USHORT;
            break;

        case PDBT_UINT32:
            sTargetType = SQL_C_ULONG;
            break;

        case PDBT_INT8:
            sTargetType = SQL_C_TINYINT;
            break;

        case PDBT_INT16:
            sTargetType = SQL_C_SHORT;
            break;

        case PDBT_INT32:
            sTargetType = SQL_C_LONG;
            break;

        case PDBT_INT64:
        case PDBT_UINT64:
            sTargetType = SQL_C_NUMERIC;
            break;

        case PDBT_FLOAT:
            sTargetType = SQL_C_FLOAT;
            break;

        case PDBT_DOUBLE:
            sTargetType = SQL_C_DOUBLE;
            break;

        case PDBT_STRING:
            sTargetType = SQL_C_CHAR;
            break;

        case PDBT_BLOB:
            sTargetType = SQL_C_BINARY;
            break;

        default:
            return PDB_ERROR_PARA;

    }

    sRet = SQLBindCol(pstDCIHandle->DBHandles.StmtHandle, usRsltIndex, sTargetType, pRsltBuf,
                      (SQLINTEGER)ulRsltBufLen, (SQLINTEGER*)piLenInd);
    if ((sRet != SQL_SUCCESS) &&
        (sRet != SQL_SUCCESS_WITH_INFO))
    {
        ODBCDiagStmtError(pstDCIHandle);

        return PDB_ERROR_BIND;
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: PDBRET ODBCCancel
 功    能: 终止当前正在执行的数据库操作并为下一次数据库操作做好准备
 输入参数: pstDCIHandle --- PDB CLI数据库连接
           hStmtHandle  --- 语句句柄
           Mode         --- Cancel方式 PDB_UNBIND PDB_KEEPBIND
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET ODBCCancel(ODBC_DCI_HANDLE_T *pstDCIHandle, BYTE bMode)
{
    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: DCIBindRslt
 功    能: 绑定数据库查询操作返回的结果集，在DCIRowFetch前使用
 输入参数: pstDCIHandle --- PDB CLI数据库连接
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET ODBCBindRslt(ODBC_DCI_HANDLE_T* pstDCIHandle)
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
PDBRET ODBCStoreRslt(ODBC_DCI_HANDLE_T* pstDCIHandle)
{
    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: DCIRowFetch
 功    能: 通过PDB CLI提取数据库查询操作返回的结果集
 输入参数: pstDCIHandle --- PDB CLI数据库连接
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET ODBCRowFetch(ODBC_DCI_HANDLE_T* pstDCIHandle)
{
    SQLRETURN       sRet;

    ODBC_DCI_CONN_STMT_HANDLE_CHECK;

    sRet = SQLFetch(pstDCIHandle->DBHandles.StmtHandle);

    switch(sRet)
    {
        case SQL_SUCCESS:
            break;

        case SQL_SUCCESS_WITH_INFO:
            ODBCDiagStmtError(pstDCIHandle);
            break;

        case SQL_NO_DATA_FOUND:
            return PDB_NO_DATA;

        default:
            ODBCDiagStmtError(pstDCIHandle);
            return PDB_ERROR_FETCH;
    }

    pstDCIHandle->ulFetchedNum++;

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: DCIBindRslt
 功    能: 释放数据库查询操作返回的结果集，在DCIRowFetch全部结束后使用
 输入参数: pstDCIHandle --- PDB CLI数据库连接
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET ODBCFreeRslt(ODBC_DCI_HANDLE_T* pstDCIHandle)
{
    ODBC_DCI_CONN_STMT_HANDLE_CHECK;

    if (SQL_SUCCESS != SQLCloseCursor(pstDCIHandle->DBHandles.StmtHandle))
    {
        ODBCDiagStmtError(pstDCIHandle);
        return PDB_ERROR_FREE_RSLT;
    }

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
PDBRET ODBCTransBegin(ODBC_DCI_HANDLE_T *pstDCIHandle,
                      CHAR          *szTransName)
{
    ODBC_DCI_CONN_HANDLE_CHECK;

    if (PDB_SUCCESS != ODBCSetAutoCommit(pstDCIHandle, FALSE))
    {
        return PDB_ERROR_SET_ATTR;
    }

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
PDBRET ODBCTransRollback(ODBC_DCI_HANDLE_T    *pstDCIHandle,
                        CHAR            *szTransName)
{
    SQLRETURN   sRet = -1;

    ODBC_DCI_CONN_HANDLE_CHECK;

    sRet = SQLEndTran(SQL_HANDLE_DBC, pstDCIHandle->DBHandles.ConnHandle, SQL_ROLLBACK);
    if ((sRet != SQL_SUCCESS) &&
        (sRet != SQL_SUCCESS_WITH_INFO))
    {
        ODBCDiagConnError(pstDCIHandle);

        return PDB_ERROR_ROLLBACK;
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: DCITransCommit
 功    能: 按指定事务名提交一个数据库事务
 输入参数: pstDCIHandle --- PDB CLI数据库连接
           szTransName  --- 事务名称
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET ODBCTransCommit(ODBC_DCI_HANDLE_T    *pstDCIHandle,
                      CHAR          *szTransName)
{
    SQLRETURN   sRet = -1;

    ODBC_DCI_CONN_HANDLE_CHECK;

    sRet = SQLEndTran(SQL_HANDLE_DBC, pstDCIHandle->DBHandles.ConnHandle, SQL_COMMIT);
    if ((sRet != SQL_SUCCESS) &&
        (sRet != SQL_SUCCESS_WITH_INFO))
    {
        ODBCDiagConnError(pstDCIHandle);

        return PDB_ERROR_ROLLBACK;
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: ODBCRowCount
 功    能: 获得当前命令影响的记录行数
 输入参数: pstDCIHandle --- PDB CLI数据库连接
           hStmtHandle  --- 语句句柄
 全局变量:
 返回结果: 当前命令影响的记录行数 对于SELECT语句只有当全部数据FETCH后有效
************************************************************************/
INT32 ODBCRowCount(ODBC_DCI_HANDLE_T* pstDCIHandle)
{
    SQLRETURN       sRet = -1;
    SQLINTEGER      iNum = -1;

    if (pstDCIHandle == NULL )
    {
        pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_HIGH;\
        return PDB_INVALID_HANDLE;\
    }\
    if( pstDCIHandle->DBHandles.ConnHandle  == NULL || \
        pstDCIHandle->DBHandles.StmtHandle == NULL )  \
    {\
        pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_HIGH;\
        return PDB_ERROR;\
    }\
    else \
        pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_LOW;\

    sRet = SQLRowCount(pstDCIHandle->DBHandles.StmtHandle, &iNum);
    if ((sRet != SQL_SUCCESS) &&
        (sRet != SQL_SUCCESS_WITH_INFO))
    {
        ODBCDiagStmtError(pstDCIHandle);

        return -1;
    }

    return iNum;
}

/**********************************************************************
 函    数: DCIBindPara
 功    能: SQL语句参数绑定
 输入参数: pstDCIHandle     --- 目标数据库连接
           usParaIndex      --- 参数编号，从0开始顺序递增。
           usParaType       --- 该参数的SQL数据类型
           pParaValue       --- 该参数的数据缓冲区的指针(字符串为其长度)
           ulParaLen        --- 该数据缓冲区的长度
           piLenInd         --- 长度或标志
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR or PDB_INVALID_HANDLE
************************************************************************/
PDBRET ODBCBindPara(ODBC_DCI_HANDLE_T   *pstDCIHandle,
                   UINT16       usParaIndex,
                   UINT32       ulParaType,
                   UINT32       ulInOutType,
                   VOID         *pParaValue,
                   UINT32       ulParaLen,
                   INT32        *piLenInd)
{
    SQLRETURN           sRet;
    SQLSMALLINT         sInOutType;
    SQLSMALLINT         sCType;
    SQLSMALLINT         sSqlType;

    ODBC_DCI_CONN_STMT_HANDLE_CHECK;

    if (usParaIndex >= PDB_MAX_BIND_NUM /*||
        pstDCIHandle->ulSendBlobParamNum >= PDB_MAX_BIND_NUM ||
        usParaIndex > ARRAY_SIZE(pstDCIHandle->pavParent)*/)
    {
        return PDB_ERROR_BIND;
    }

    /* 参数有效性检查 */
    if (NULL == pParaValue &&
        PDBT_NULL != ulParaType)
    {
        return PDB_ERROR_BIND;
    }

/*  if (PDBT_BLOB == ulParaType
         || PDBT_LONGBLOB == ulParaType)

    {
        pstDCIHandle->pavParent[usParaIndex] = pParent;
        //pstDCIHandle->ulBlobParamNum++;

        if (ulParaLen > PDB_MAX_SEND_BLOB_SIZE)
        {
            pstDCIHandle->aulBindIndex[pstDCIHandle->ulSendBlobParamNum] = usParaIndex;
            pstDCIHandle->ulSendBlobParamNum++;
        }
    }

    memset(pstBind, 0, sizeof(MYSQL_BIND));
    pstBind->buffer        = (VOID*)pParaValue;
    pstBind->buffer_length = (UINT32)0;
    pstBind->is_null       = (my_bool*)NULL;
    pstBind->is_unsigned   = (my_bool)FALSE;
    pstBind->length        = (UINT32*)NULL;       */

    switch(ulParaType)
    {
        case PDBT_UINT8:
            sCType   = SQL_C_UTINYINT;
            sSqlType = SQL_TINYINT; //SQL_CHAR
            break;

        case PDBT_UINT16:
            sCType   = SQL_C_USHORT;
            sSqlType = SQL_SMALLINT;
            break;

        case PDBT_UINT32:
            sCType   = SQL_C_ULONG;
            sSqlType = SQL_INTEGER;
            break;

        case PDBT_INT8:
            sCType   = SQL_C_TINYINT;
            sSqlType = SQL_TINYINT;
            break;

        case PDBT_INT16:
            sCType   = SQL_C_SHORT;
            sSqlType = SQL_SMALLINT;
            break;

        case PDBT_INT32:
            sCType   = SQL_C_LONG;
            sSqlType = SQL_INTEGER;
            break;

        case PDBT_FLOAT:
            sCType   = SQL_C_FLOAT;
            sSqlType = SQL_FLOAT;
            break;

        case PDBT_DOUBLE:
            sCType   = SQL_C_DOUBLE;
            sSqlType = SQL_DOUBLE;
            break;

        case PDBT_STRING:
            sCType   = SQL_C_CHAR;
            sSqlType = SQL_VARCHAR;

            if (PDB_PARAM_INPUT == ulInOutType || PDB_PARAM_INPUT_OUTPUT == ulInOutType)
            {
                *piLenInd = SQL_NTS;
            }

            break;

        default:
            return PDB_ERROR_BIND;
    }

    if (PDB_PARAM_INPUT == ulInOutType)
    {
        sInOutType = SQL_PARAM_INPUT;
    }
    else if (PDB_PARAM_OUTPUT == ulInOutType)
    {
        sInOutType = SQL_PARAM_OUTPUT;
    }
    else if (PDB_PARAM_INPUT_OUTPUT == ulInOutType)
    {
        sInOutType = SQL_PARAM_INPUT_OUTPUT;
    }
    else
    {
        return PDB_ERROR_BIND;
    }

    sRet = SQLBindParameter(pstDCIHandle->DBHandles.StmtHandle, (SQLSMALLINT)(usParaIndex+1), sInOutType, sCType, sSqlType,
                            (SQLUINTEGER)ulParaLen, 0, pParaValue, ulParaLen, (SQLLEN*)piLenInd);
    if ((sRet != SQL_SUCCESS) &&
        (sRet != SQL_SUCCESS_WITH_INFO))
    {
        ODBCDiagStmtError(pstDCIHandle);

        return PDB_ERROR_BIND;
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: PDBRET ODBCGetError
 功    能: 根据PDB CLI错误码获取PDB错误信息
 输入参数: pstDCIHandle     --- PDB CLI数据库连接
           nResult      --- PDB CLI返回结果
           StmtHandle   --- 语句句柄指针
           BufferLen    --- 错误信息缓冲区长度
 输出参数: ErrorInfo    --- 错误信息
           pucErrLevel    --- 错误级别
 全局变量:
 返回结果: PDB_SUCCESS | PDB_ERROR | PDB_INVALID_HANDLE | PDB_NO_DATA
************************************************************************/
PDBRET ODBCGetError(ODBC_DCI_HANDLE_T *pstDCIHandle)
{
    SQLRETURN   sRet;
    SQLSMALLINT sErrMsgLen;
    CHAR        acErrState[8];

    if (NULL == pstDCIHandle)
    {
        return PDB_INVALID_HANDLE;
    }

    pstDCIHandle->iErrCode = 0;
    pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_NULL;
    memset(pstDCIHandle->acErrInfo, 0, sizeof(pstDCIHandle->acErrInfo));

    if (NULL == pstDCIHandle->DBHandles.ConnHandle)
    {
        return PDB_INVALID_HANDLE;
    }

    sRet = SQLGetDiagRec(SQL_HANDLE_DBC, pstDCIHandle->DBHandles.ConnHandle, 1,
                         (SQLCHAR*)acErrState,
                         (SQLINTEGER*)&pstDCIHandle->iErrCode,
                         (SQLCHAR*)pstDCIHandle->acErrInfo,
                         sizeof(pstDCIHandle->acErrInfo),
                         &sErrMsgLen);
    if (SQL_SUCCESS != sRet)
    {
        return PDB_ERROR;
    }

    if (pstDCIHandle->iErrCode != 0)
    {
        pstDCIHandle->ulErrLevel = ODBCGetErrorLevel(acErrState);
        return PDB_ERROR;
    }

    sRet = SQLGetDiagRec(SQL_HANDLE_STMT, pstDCIHandle->DBHandles.StmtHandle, 1,
                         (SQLCHAR*)acErrState,
                         (SQLINTEGER*)&pstDCIHandle->iErrCode,
                         (SQLCHAR*)pstDCIHandle->acErrInfo,
                         sizeof(pstDCIHandle->acErrInfo),
                         &sErrMsgLen);
    if (SQL_SUCCESS != sRet)
    {
        return PDB_ERROR;
    }

    if (pstDCIHandle->iErrCode != 0)
    {
        pstDCIHandle->ulErrLevel = ODBCGetErrorLevel(acErrState);
        return PDB_ERROR;
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: ODBCGetErrorLevel
 功    能: 根据ODBC错误码确定错误级别
 输入参数: szErrState --- ODBC错误状态码
 全局变量:
 返回结果: 错误级别
************************************************************************/
BYTE ODBCGetErrorLevel(CHAR *szErrState)
{
    UINT32      i;

    if (*szErrState == '\0')
    {
        return PDB_ERROR_LEVEL_NULL;
    }

    for (i=0; i<ARRAY_SIZE(g_aszODBCFatalErrorCodes); i++)
    {
        if (strcmp(szErrState, g_aszODBCFatalErrorCodes[i]) == 0)
        {
            return PDB_ERROR_LEVEL_HIGH;
        }
    }

    for (i=0; i<ARRAY_SIZE(g_aszODBCNormalErrorCodes); i++)
    {
        if (strcmp(szErrState, g_aszODBCNormalErrorCodes[i]) == 0)
        {
            return PDB_ERROR_LEVEL_MID;
        }
    }

    return PDB_ERROR_LEVEL_LOW;
}
