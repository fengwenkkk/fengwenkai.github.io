/******************************************************************************
模块名          ：  Database Call Interface (DCI)
文件名          ：  pdb_mysql_core.c
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

#include "mysql.h"

#include "pdb_def.h"
#include "pdb_util.h"
#include "pdb_error.h"
#include "pdb_mysql_core.h"

#define MYSQL_OK            0

BYTE        MySQLGetErrorLevel(UINT32 ulErrCode);

#define DCI_CONN_STMT_HANDLE_CHECK  \
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

#define DCI_CONN_HANDLE_CHECK  \
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

#define DCI_BIND_RSLT(pstBind, buf_type, buf_len) \
{                                                   \
    memset(pstBind, 0, sizeof(MYSQL_BIND));         \
    pstBind->buffer_type   = buf_type;              \
    pstBind->buffer_length = (UINT32)buf_len;       \
    pstBind->is_null       = (my_bool*)NULL;        \
}

/**********************************************************************
 函    数: BOOL    PDBCheckPROC
 功    能: 检查数据库连接句柄和指针
 输入参数: PDBPROC  数据库连接指针

 全局变量: 无
 返回结果: TRUE/FALSE
************************************************************************/
BOOL MySQLCheckHandle(MYSQL_DCI_HANDLE_T *pstDCIHandle)
{
    if(NULL == pstDCIHandle ||
       NULL == pstDCIHandle->DBHandles.ConnHandle ||
       NULL == pstDCIHandle->DBHandles.StmtHandle )
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
PDBRET MySQLInit(HANDLE *phEnv)
{
    if (MYSQL_OK != mysql_server_init(0, NULL, NULL))
    /*if (MYSQL_OK != my_init() &&
        MYSQL_OK != mysql_server_init(0, NULL, NULL) //&&
        MYSQL_OK != mysql_thread_init()
        )
        */
    {
        return PDB_ERROR;
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: DCIExit
 功    能: 退出DCI调用环境
 输入参数:
 全局变量:
 返回结果: PDB_SUCCESS
************************************************************************/
PDBRET MySQLExit(HANDLE hEnv)
{
    mysql_server_end();

    return PDB_SUCCESS;
}

/*******************************************************************************
* 函数名称: MySQLDiagConnError
* 功    能: 诊断错误原因
* 函数类型: VOID
* 参    数:
* 参数名称        类型              输入/输出         描述
*
* 函数返回:
* 说    明:
*******************************************************************************/
VOID MySQLDiagConnError(MYSQL_DCI_HANDLE_T *pstDCIHandle)
{
    CHAR        *szErrInfo = NULL;
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

    return;
}

/*******************************************************************************
* 函数名称: MySQLDiagStmtError
* 功    能: 诊断STMT错误
* 函数类型: VOID
* 参    数:
* 参数名称        类型              输入/输出         描述
*
* 函数返回:
* 说    明:
*******************************************************************************/
VOID MySQLDiagStmtError(MYSQL_DCI_HANDLE_T *pstDCIHandle)
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
PDBRET MySQLSetAutoCommit(MYSQL_DCI_HANDLE_T *pstDCIHandle, BOOL bAutoCommit)
{
    /* 局部变量定义 */
    my_bool     bRet = MYSQL_OK;

    if (NULL == pstDCIHandle)
    {
        return PDB_ERROR_SET_ATTR;
    }
/*
    if (pstDCIHandle->bAutoCommit == bAutoCommit)
    {
        return PDB_SUCCESS;
    }
*/
    bRet = mysql_autocommit((MYSQL*)pstDCIHandle->DBHandles.ConnHandle, (my_bool)bAutoCommit);
    if (bRet != MYSQL_OK)
    {
        MySQLDiagConnError(pstDCIHandle);
        return PDB_ERROR_SET_ATTR;
    }

//    pstDCIHandle->bAutoCommit = bAutoCommit;

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
PDBRET MySQLConnect
(
    MYSQL_DCI_HANDLE_T  *pstDCIHandle,
    PDB_CONN_PARAM_T    *pstConnParam,
    PDB_OPTION_T        *pstOption
)
{
    MYSQL       *hMySQLHandle = NULL;

    //MYSQL_DCI_HANDLE_T *pstDCIHandle;

    /*
    if(NULL == *pst_DCIHandle)
    {
        *pst_DCIHandle = (MYSQL_DCI_HANDLE_T *)malloc(sizeof(MYSQL_DCI_HANDLE_T));
        if(NULL == *pst_DCIHandle)
        {
            return enRet;
        }
        memset(*pst_DCIHandle, 0, sizeof(MYSQL_DCI_HANDLE_T));
    }

    pstDCIHandle = *pst_DCIHandle;
    */

    if(pstDCIHandle == NULL)
    {
        return PDB_INVALID_HANDLE;
    }

    pstDCIHandle->iErrCode   = PDB_ERROR;
    pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_HIGH;

    //DCIDisconnect(pstDCIHandle);

    memcpy(&pstDCIHandle->stOption, pstOption, sizeof(PDB_OPTION_T));
    memcpy(&pstDCIHandle->stConnParam, pstConnParam, sizeof(PDB_CONN_PARAM_T));

    /* Allocate connection handle */
    hMySQLHandle = mysql_init(NULL);//&MySQLHandle);
    if (NULL == hMySQLHandle)
    {
        return PDB_ERROR_CREATE_CONN;
    }

    /*设置MYSQL选项*/
    mysql_options(hMySQLHandle,MYSQL_OPT_CONNECT_TIMEOUT, (CHAR*)&pstDCIHandle->stOption.ulTimeout);

    pstDCIHandle->DBHandles.ConnHandle = hMySQLHandle;
    pstDCIHandle->DBHandles.StmtHandle = NULL;
    if (NULL == mysql_real_connect(hMySQLHandle,
                                   pstDCIHandle->stConnParam.acServer,
                                   pstDCIHandle->stConnParam.acUserName,
                                   pstDCIHandle->stConnParam.acPassword,
                                   pstDCIHandle->stConnParam.acDBName,
                                   pstDCIHandle->stConnParam.usPort,
                                   NULL, CLIENT_MULTI_STATEMENTS|CLIENT_FOUND_ROWS))
    {
        MySQLDiagConnError(pstDCIHandle);
        mysql_close(hMySQLHandle);
        pstDCIHandle->DBHandles.ConnHandle = NULL;

        return PDB_ERROR_CREATE_CONN;
    }

    /* 创建语句句柄 */
    pstDCIHandle->DBHandles.StmtHandle = mysql_stmt_init(hMySQLHandle);
    if (NULL == pstDCIHandle->DBHandles.StmtHandle)
    {
        MySQLDiagConnError(pstDCIHandle);

        mysql_stmt_close(pstDCIHandle->DBHandles.StmtHandle);
        mysql_close(hMySQLHandle);
        pstDCIHandle->DBHandles.StmtHandle = NULL;
        pstDCIHandle->DBHandles.ConnHandle = NULL;

        return PDB_ERROR_CREATE_STMT;
    }

    /* 设置事务提交模式 */
    /*
    enRet = DCISetAutoCommit(pstDCIHandle, FALSE);
    if (enRet != PDB_SUCCESS)
    {
        //mysql_stmt_close(pstDCIHandle->DBHandles.StmtHandle);
        mysql_close(hMySQLHandle);

        return PDB_ERROR_SET_ATTR;
    }
    */

    pstDCIHandle->iErrCode = 0;

    memset(pstDCIHandle->acTransName, 0, sizeof(pstDCIHandle->acTransName));

    return PDB_SUCCESS;
}


/**********************************************************************
 函    数: DCIStmtInit
 功    能: 通过PDB CLI建立数据库连接的语句句柄
 输入参数: pstDCIHandle     --- DCI数据库连接
 全局变量:
 返回结果: PDB_SUCCESS or PDB_ERROR_CREATE_STMT or PDB_ERROR
************************************************************************/
PDBRET MySQLInitStmt(MYSQL_DCI_HANDLE_T *pstDCIHandle)
{
    DCI_CONN_HANDLE_CHECK;

    if (pstDCIHandle->DBHandles.StmtHandle)
    {
        return PDB_SUCCESS;
    }

    pstDCIHandle->DBHandles.StmtHandle = mysql_stmt_init(pstDCIHandle->DBHandles.ConnHandle);
    if (NULL == pstDCIHandle->DBHandles.StmtHandle)
    {
        MySQLDiagConnError(pstDCIHandle);
        mysql_close(pstDCIHandle->DBHandles.ConnHandle);

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
PDBRET MySQLCloseStmt(MYSQL_DCI_HANDLE_T *pstDCIHandle)
{
    if (pstDCIHandle == NULL)
    {
        return PDB_INVALID_HANDLE;
    }

    if (pstDCIHandle->DBHandles.StmtHandle)
    {
        mysql_stmt_close(pstDCIHandle->DBHandles.StmtHandle);
        pstDCIHandle->DBHandles.StmtHandle = NULL;
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: PDBRET MySQLDisconnect(MYSQL_DCI_HANDLE_T* pstDCIHandle)
 功    能: 关闭数据库连接
 输入参数: pstDCIHandle --- PDB CLI数据库连接
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR or PDB_INVALID_HANDLE
************************************************************************/
PDBRET MySQLDisconnect(MYSQL_DCI_HANDLE_T* pstDCIHandle)
{
    my_bool     bRetCode = FALSE;

    if (pstDCIHandle == NULL)
    {
        return PDB_SUCCESS;
    }

    pstDCIHandle->iErrCode = 0;
    pstDCIHandle->acSQLCmd[0] = '\0';

    /* 检查是否有STMT句柄 */
    if (NULL != pstDCIHandle->DBHandles.StmtHandle)
    {
        bRetCode = mysql_stmt_close((MYSQL_STMT*)pstDCIHandle->DBHandles.StmtHandle);
        if (bRetCode != MYSQL_OK)
        {
            return PDB_ERROR_FREE_STMT;
        }

        pstDCIHandle->DBHandles.StmtHandle = NULL;
    }

    /* 检查是否有连接句柄为空 */
    if (NULL != pstDCIHandle->DBHandles.ConnHandle)
    {
        mysql_close((MYSQL*)pstDCIHandle->DBHandles.ConnHandle);
        pstDCIHandle->DBHandles.ConnHandle = NULL;
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
PDBRET MySQLSQLPrepare(MYSQL_DCI_HANDLE_T *pstDCIHandle)
{
    INT32       iRet;
    //PDBUINT32 ulParamCount;
    //PDBUINT32 ulColumnCount;
    //MYSQL_RES     *pPrepareMetaRlt;

    DCI_CONN_STMT_HANDLE_CHECK;

    //DCIInitStmt(pstDCIHandle);

    // 设置多查询执行
    /*
    iRet = mysql_set_server_option(pstDCIHandle->DBHandles.ConnHandle, MYSQL_OPTION_MULTI_STATEMENTS_ON);
    if (iRet != MYSQL_OK)
    {
        MySQLDiagStmtError(pstDCIHandle);
        return PDB_ERROR_STMT_PREPARE;
    }
    */

    iRet = mysql_stmt_prepare(pstDCIHandle->DBHandles.StmtHandle,
                              pstDCIHandle->acSQLCmd, strlen(pstDCIHandle->acSQLCmd));
    if (iRet != MYSQL_OK)
    {
        MySQLDiagStmtError(pstDCIHandle);
        return PDB_ERROR_STMT_PREPARE;
    }

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
PDBRET MySQLSetPrefetchNum(MYSQL_DCI_HANDLE_T* pstDCIHandle, UINT32 ulRowNum)
{
    UINT32 ulCursorType = (UINT32)CURSOR_TYPE_READ_ONLY;

    DCI_CONN_STMT_HANDLE_CHECK;

    /* 检查ulRowNum是否等于0 */
    if (0 == ulRowNum)
    {
        return PDB_ERROR_SET_ATTR;
    }

    if (MYSQL_OK != mysql_stmt_attr_set(pstDCIHandle->DBHandles.StmtHandle, STMT_ATTR_CURSOR_TYPE, (VOID*)&ulCursorType))
    {
        return PDB_ERROR_SET_ATTR;
    }

    if (MYSQL_OK != mysql_stmt_attr_set(pstDCIHandle->DBHandles.StmtHandle, STMT_ATTR_PREFETCH_ROWS, (VOID*)&ulRowNum))
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
PDBRET MySQLQuery(MYSQL_DCI_HANDLE_T* pstDCIHandle)
{
    DCI_CONN_STMT_HANDLE_CHECK;

    if (pstDCIHandle->ulBindNum > 0)
    {
        if (MYSQL_OK != mysql_stmt_bind_param(pstDCIHandle->DBHandles.StmtHandle, pstDCIHandle->astBindHandle))
        {
            MySQLDiagStmtError(pstDCIHandle);

            return PDB_ERROR_BIND;
        }
    }

    /* 发送大数据blob */
    /*if (PDB_SUCCESS != DCISendBlobData(pstDCIHandle, hStmtHandle))
    {
         return PDB_ERROR_BIND;
    }*/

    // 光标方式
    /*
    if (PDB_SUCCESS != DCISetPrefetchNum(hStmtHandle, pstDCIHandle->stOption.ulPrefetchRow))
    {
        return PDB_ERROR_BIND;
    }
    */

    if (MYSQL_OK != mysql_stmt_execute(pstDCIHandle->DBHandles.StmtHandle))
    {
        MySQLDiagStmtError(pstDCIHandle);

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
PDBRET MySQLExec(MYSQL_DCI_HANDLE_T* pstDCIHandle)
{
    DCI_CONN_STMT_HANDLE_CHECK;

    pstDCIHandle->ulExecErrCount = 0;

    if (pstDCIHandle->ulBindNum > 0)
    {
/*        for (i=0; i<pstDCIHandle->ulBindNum; i++)
        {
            if (pstDCIHandle->astBindHandle[i].buffer_type == MYSQL_TYPE_VAR_STRING)
            {
                szText = pstDCIHandle->astBindHandle[i].buffer;
                if (*szText == '\0')
                {
                 //   strcpy(szText, " ");
                    pstDCIHandle->astBindHandle[i].buffer_length = 1;
                }
            }
        } */

        if (MYSQL_OK != mysql_stmt_bind_param(pstDCIHandle->DBHandles.StmtHandle, pstDCIHandle->astBindHandle))
        {
            MySQLDiagStmtError(pstDCIHandle);
            pstDCIHandle->ulExecErrCount ++;

            return PDB_ERROR_BIND;
        }
    }

    /* 发送大数据blob */
    /*
    if (PDB_SUCCESS != DCISendBlobData(pstDCIHandle, hStmtHandle))
    {
        pstDCIHandle->ulExecErrCount ++;
        return PDB_ERROR_BIND;
    } */

    if (MYSQL_OK != mysql_stmt_execute(pstDCIHandle->DBHandles.StmtHandle))
    {
        MySQLDiagStmtError(pstDCIHandle);
        pstDCIHandle->ulExecErrCount ++;

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
           ulRsltBufLen --- 绑定结果集的列缓冲区长度指针
           pucNull      --- 标识字段是否为空的列数组指针
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET MySQLBindRsltByPos(MYSQL_DCI_HANDLE_T    *pstDCIHandle,
                          UINT16                usRsltIndex,
                          UINT16                usRsltType,
                          VOID                  *pRsltBuf,
                          UINT32                ulRsltBufLen,
                          BOOL                  *pbIsNull,
                          INT32                 *piLenInd)
{
    MYSQL_BIND      *pstBind;

    DCI_CONN_STMT_HANDLE_CHECK;

    if (0 == usRsltIndex || usRsltIndex > PDB_MAX_BIND_NUM)
    {
        return PDB_ERROR_BIND;
    }

    /* 参数有效性检查 */
    if (NULL == pRsltBuf)
    {
        return PDB_ERROR_PARA;
    }

    pstBind = &pstDCIHandle->astBindRsltHandle[usRsltIndex-1];

    switch(usRsltType)
    {
        case PDBT_INT8:
        case PDBT_UINT8:
            DCI_BIND_RSLT(pstBind, MYSQL_TYPE_TINY, sizeof(INT8));
            break;

        case PDBT_INT16:
        case PDBT_UINT16:
            DCI_BIND_RSLT(pstBind, MYSQL_TYPE_SHORT, sizeof(INT16));
            break;

        case PDBT_INT32:
        case PDBT_UINT32:
            DCI_BIND_RSLT(pstBind, MYSQL_TYPE_LONG, sizeof(INT32));
            break;

        case PDBT_INT64:
        case PDBT_UINT64:
            DCI_BIND_RSLT(pstBind, MYSQL_TYPE_LONGLONG, sizeof(INT64));
            break;

        case PDBT_FLOAT:
            DCI_BIND_RSLT(pstBind, MYSQL_TYPE_FLOAT, sizeof(FLOAT));
            break;

        case PDBT_DOUBLE:
            DCI_BIND_RSLT(pstBind, MYSQL_TYPE_DOUBLE, sizeof(DOUBLE));
            break;

        case PDBT_STRING:
            DCI_BIND_RSLT(pstBind, MYSQL_TYPE_VAR_STRING, ulRsltBufLen);
            break;

        default:
            return PDB_ERROR_PARA;
    }

    *piLenInd = 0;
    *pbIsNull = FALSE;

    pstBind->buffer = (CHAR*)pRsltBuf;
    pstBind->length = (unsigned long*)piLenInd;
    pstBind->is_null = (my_bool*)pbIsNull;

    pstDCIHandle->ulBindRsltNum = usRsltIndex;

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: PDBRET MySQLCancel
 功    能: 终止当前正在执行的数据库操作并为下一次数据库操作做好准备
 输入参数: pstDCIHandle --- PDB CLI数据库连接
           hStmtHandle  --- 语句句柄
           Mode         --- Cancel方式 PDB_UNBIND PDB_KEEPBIND
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET MySQLCancel(MYSQL_DCI_HANDLE_T *pstDCIHandle, BYTE bMode)
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
PDBRET MySQLBindRslt(MYSQL_DCI_HANDLE_T* pstDCIHandle)
{
    INT32       iRet;
    UINT32      ulFieldCount;

    DCI_CONN_STMT_HANDLE_CHECK;

    if (pstDCIHandle->ulBindRsltNum == 0)
    {
        return PDB_ERROR_BIND;
    }

    ulFieldCount = mysql_stmt_field_count(pstDCIHandle->DBHandles.StmtHandle);
    if (ulFieldCount != pstDCIHandle->ulBindRsltNum)
    {
        return PDB_ERROR_BIND;
    }

    iRet = mysql_stmt_bind_result(pstDCIHandle->DBHandles.StmtHandle, pstDCIHandle->astBindRsltHandle);
    if (iRet != MYSQL_OK)
    {

        MySQLDiagStmtError(pstDCIHandle);

        return PDB_ERROR_BIND;
    }

    /* 客户端缓存方式 */
    /*
    iRet = mysql_stmt_store_result(hStmt);//pstDCIHandle->DBHandles.StmtHandle);
    if (iRet != MYSQL_OK)
    {
        MySQLDiagStmtError(pstDCIHandle);
        mysql_stmt_free_result(hStmt);//pstDCIHandle->DBHandles.StmtHandle);

        return PDB_ERROR_BIND;
    }
    */

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: DCIStoreRslt
 功    能: 缓存数据库预处理查询返回的结果集到客户端，在DCIBindRslt之后、DCIRowFetch前使用
 输入参数: pstDCIHandle --- PDB CLI数据库连接
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET MySQLStoreRslt(MYSQL_DCI_HANDLE_T* pstDCIHandle)
{
    INT32       iRet;

    DCI_CONN_STMT_HANDLE_CHECK;
    iRet = mysql_stmt_store_result(pstDCIHandle->DBHandles.StmtHandle);
    if (iRet != MYSQL_OK)
    {
        MySQLDiagStmtError(pstDCIHandle);
        mysql_stmt_free_result(pstDCIHandle->DBHandles.StmtHandle);

        return PDB_ERROR_BIND;
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: DCIRowFetch
 功    能: 通过PDB CLI提取数据库查询操作返回的结果集
 输入参数: pstDCIHandle --- PDB CLI数据库连接
 全局变量:
 返回结果: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET MySQLRowFetch(MYSQL_DCI_HANDLE_T* pstDCIHandle)
{
    INT32       iRet;

    DCI_CONN_STMT_HANDLE_CHECK;

/*
    if (!mysql_stmt_result_metadata(hStmt))
    {
        return PDB_ERROR_FETCH;
    }
*/
    if (pstDCIHandle->ulBindRsltNum == 0)
    {
        return PDB_ERROR_BIND;
    }

     /*
    iRet = mysql_stmt_bind_result(hStmt, pstDCIHandle->astBindRsltHandle);
        //pstDCIHandle->DBHandles.StmtHandle, pstDCIHandle->astBindRsltHandle);
    if (iRet != MYSQL_OK)
    {
        MySQLDiagStmtError(pstDCIHandle);

        return PDB_ERROR_BIND;
    }

    iRet = mysql_stmt_store_result(hStmt);//pstDCIHandle->DBHandles.StmtHandle);
    if (iRet != MYSQL_OK)
    {
        MySQLDiagStmtError(pstDCIHandle);
        mysql_stmt_free_result(hStmt);//pstDCIHandle->DBHandles.StmtHandle);

        return PDB_ERROR_BIND;
    }
    */

    iRet = mysql_stmt_fetch(pstDCIHandle->DBHandles.StmtHandle);

    if (MYSQL_NO_DATA == iRet)
    {
        mysql_stmt_free_result(pstDCIHandle->DBHandles.StmtHandle);
        return PDB_NO_DATA;
    }

    if (iRet != MYSQL_OK && iRet != MYSQL_DATA_TRUNCATED)
    {
        MySQLDiagStmtError(pstDCIHandle);
        mysql_stmt_free_result(pstDCIHandle->DBHandles.StmtHandle);

        return PDB_ERROR_FETCH;
    }

    //pstDCIHandle->ulFetchedNum = 0;
#if 0
    for(ulLoop=0; ulLoop<pstDCIHandle->ulBindRsltNum; ulLoop++)
    {
        iRet = mysql_stmt_fetch_column(hStmt, &pstDCIHandle->astBindRsltHandle[ulLoop],
                                       ulLoop, 0);

        if (iRet != MYSQL_OK)
        {
            MySQLDiagStmtError(pstDCIHandle);
            mysql_stmt_free_result(hStmt);

            return PDB_ERROR_FETCH;
        }
    }
#endif

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
PDBRET MySQLFreeRslt(MYSQL_DCI_HANDLE_T* pstDCIHandle)
{
    DCI_CONN_STMT_HANDLE_CHECK;

    mysql_stmt_free_result(pstDCIHandle->DBHandles.StmtHandle);
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
PDBRET MySQLTransBegin( MYSQL_DCI_HANDLE_T  *pstDCIHandle,
                      CHAR          *szTransName)
{
    if (szTransName == NULL || szTransName[0] == '\0')
    {
        return PDB_ERROR;
    }

    //DCI_CONN_STMT_HANDLE_CHECK;
    DCI_CONN_HANDLE_CHECK;

    /*不支持嵌套事务*/
    if(pstDCIHandle->acTransName[0] != '\0')
    {
        strcpy(pstDCIHandle->acErrInfo, "Nesting transcation unsupport!");
        pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_LOW;

        return PDB_ERROR;
    }

    /* 事务名称太长 */
    if (strlen(szTransName)+1 >= PDB_MAX_TRANS_LEN)
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
PDBRET MySQLTransRollback(MYSQL_DCI_HANDLE_T *pstDCIHandle, CHAR *szTransName)
{
    if (szTransName == NULL || szTransName[0] == '\0' )
    {
        return PDB_ERROR_ROLLBACK;
    }

    //DCI_CONN_STMT_HANDLE_CHECK;
    DCI_CONN_HANDLE_CHECK;

    /* check the transaction name */
    if(strcmp(szTransName, pstDCIHandle->acTransName) != 0)
    {
        sprintf(pstDCIHandle->acErrInfo, "Transaction %s not exist!", szTransName);
        pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_LOW;

        return PDB_ERROR_ROLLBACK;
    }

    if (MYSQL_OK != mysql_rollback(pstDCIHandle->DBHandles.ConnHandle))
    {
        MySQLDiagConnError(pstDCIHandle);

        return PDB_ERROR_ROLLBACK;
    }

    pstDCIHandle->acTransName[0] = '\0';

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
PDBRET MySQLTransCommit(MYSQL_DCI_HANDLE_T *pstDCIHandle, CHAR *szTransName)
{
    DCI_CONN_HANDLE_CHECK;

    if (szTransName == NULL || szTransName[0] == '\0' )
    {
        return PDB_ERROR_COMMIT;
    }

    /* check the transaction name */
    if(strcmp(szTransName, pstDCIHandle->acTransName) != 0)
    {
        sprintf(pstDCIHandle->acErrInfo, "Tranasction %s not exist!", szTransName);
        pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_HIGH;

        return PDB_ERROR_COMMIT;
    }

    if (MYSQL_OK != mysql_commit(pstDCIHandle->DBHandles.ConnHandle))
    {
        MySQLDiagConnError(pstDCIHandle);

        return PDB_ERROR_COMMIT;
    }

    pstDCIHandle->acTransName[0] = '\0';

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: MySQLRowCount
 功    能: 获得当前命令影响的记录行数
 输入参数: pstDCIHandle --- PDB CLI数据库连接
           hStmtHandle  --- 语句句柄
 全局变量:
 返回结果: 当前命令影响的记录行数 对于SELECT语句只有当全部数据FETCH后有效
************************************************************************/
INT32 MySQLRowCount(MYSQL_DCI_HANDLE_T* pstDCIHandle)
{
    UINT64          ullRowCount = 0;

    DCI_CONN_STMT_HANDLE_CHECK;

    ullRowCount = mysql_stmt_affected_rows(pstDCIHandle->DBHandles.StmtHandle);
    if (ullRowCount > (UINT64)0x7FFFFFFF)
    {
        return 0x7FFFFFFF;
    }

    return (INT32)ullRowCount;
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
PDBRET MySQLBindPara(MYSQL_DCI_HANDLE_T *pstDCIHandle,
                   UINT16       usParaIndex,
                   UINT32       ulParaType,
                   UINT32       ulInOutType,
                   VOID         *pParaValue,
                   UINT32       ulParaLen,
                   INT32        *piLenInd)
{
    MYSQL_BIND      *pstBind;
    static my_bool  bIsNull = TRUE;
    //static my_bool  bIsNotNull = FALSE;

    DCI_CONN_STMT_HANDLE_CHECK;

    if (usParaIndex >= PDB_MAX_BIND_NUM ||
        pstDCIHandle->ulSendBlobParamNum >= PDB_MAX_BIND_NUM ||
        usParaIndex > ARRAY_SIZE(pstDCIHandle->pavParent))
    {
        return PDB_ERROR_BIND;
    }

    /* 参数有效性检查 */
    if (NULL == pParaValue &&
        PDBT_NULL != ulParaType)
    {
        return PDB_ERROR_BIND;
    }

    pstBind = &pstDCIHandle->astBindHandle[usParaIndex];

    /*
    if (PDBT_BLOB == ulParaType
         //|| PDBT_MEDIUMBLOB == ulParaType
         || PDBT_LONGBLOB == ulParaType)

    {
        pstDCIHandle->pavParent[usParaIndex] = pParent;
        //pstDCIHandle->ulBlobParamNum++;

        if (ulParaLen > PDB_MAX_SEND_BLOB_SIZE)
        {
            pstDCIHandle->aulBindIndex[pstDCIHandle->ulSendBlobParamNum] = usParaIndex;
            pstDCIHandle->ulSendBlobParamNum++;
        }
    } */

    memset(pstBind, 0, sizeof(MYSQL_BIND));
    pstBind->buffer        = (VOID*)pParaValue;
    pstBind->buffer_length = 0;
    pstBind->is_null       = NULL;
    pstBind->is_unsigned   = (my_bool)FALSE;
    pstBind->length        = NULL;

    switch(ulParaType)
    {
        case PDBT_INT8:
            pstBind->buffer_type = MYSQL_TYPE_TINY;
            break;

        case PDBT_UINT8:
            pstBind->buffer_type = MYSQL_TYPE_TINY;
            pstBind->is_unsigned = (my_bool)TRUE;
            break;

        case PDBT_INT16:
            pstBind->buffer_type = MYSQL_TYPE_SHORT;
            break;

        case PDBT_UINT16:
            pstBind->buffer_type = MYSQL_TYPE_SHORT;
            pstBind->is_unsigned = (my_bool)TRUE;
            break;

        case PDBT_INT32:
            pstBind->buffer_type = MYSQL_TYPE_LONG;
            break;

        case PDBT_UINT32:
            pstBind->buffer_type = MYSQL_TYPE_LONG;
            pstBind->is_unsigned = (my_bool)TRUE;
            break;

        case PDBT_INT64:
            pstBind->buffer_type = MYSQL_TYPE_LONGLONG;
            break;

        case PDBT_FLOAT:
            pstBind->buffer_type = MYSQL_TYPE_FLOAT;
            break;

        case PDBT_DOUBLE:
            pstBind->buffer_type = MYSQL_TYPE_DOUBLE;
            break;

        case PDBT_STRING:
            pstBind->buffer_type = MYSQL_TYPE_VAR_STRING; //MYSQL_TYPE_STRING
            pstBind->buffer_length = ulParaLen;
            break;

        case PDBT_UINT64:
            pstBind->buffer_type = MYSQL_TYPE_LONGLONG;
            pstBind->is_unsigned = (my_bool)TRUE;
            break;

        case PDBT_NULL:
            pstBind->buffer_type = MYSQL_TYPE_NULL;
            pstBind->is_null = (my_bool*)&bIsNull; //TRUE;
            break;

        default:
            return PDB_ERROR_BIND;
    }

    pstDCIHandle->ulBindNum = (usParaIndex+1);

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: PDBRET MySQLGetError
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
PDBRET MySQLGetError(MYSQL_DCI_HANDLE_T *pstDCIHandle)
{
    INT32       ulNativeErrCode = 0;
    CHAR        *szErrInfo = NULL;

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

    ulNativeErrCode = mysql_errno(pstDCIHandle->DBHandles.ConnHandle);
    if (MYSQL_OK != ulNativeErrCode)
    {
        pstDCIHandle->ulErrLevel = MySQLGetErrorLevel(ulNativeErrCode);
        szErrInfo = (CHAR*)mysql_error(pstDCIHandle->DBHandles.ConnHandle);
        if (NULL == szErrInfo)
        {
            sprintf(pstDCIHandle->acErrInfo, "Native error code: %u", ulNativeErrCode);
        }
        else
        {
            strncpy(pstDCIHandle->acErrInfo, szErrInfo, sizeof(pstDCIHandle->acErrInfo)-1);
            pstDCIHandle->acErrInfo[sizeof(pstDCIHandle->acErrInfo)-1] = '\0';
        }

        pstDCIHandle->iErrCode = ulNativeErrCode;

        return PDB_SUCCESS;
    }

    /* 检查STMT错误 */
    if (NULL == pstDCIHandle->DBHandles.StmtHandle)
    {
        return PDB_SUCCESS;//PDB_INVALID_HANDLE;
    }

    ulNativeErrCode = mysql_stmt_errno(pstDCIHandle->DBHandles.StmtHandle);
    if (MYSQL_OK != ulNativeErrCode)
    {
        pstDCIHandle->ulErrLevel = MySQLGetErrorLevel(ulNativeErrCode);
        szErrInfo = (CHAR*)mysql_stmt_error(pstDCIHandle->DBHandles.StmtHandle);
        if (NULL == szErrInfo)
        {
            sprintf(pstDCIHandle->acErrInfo, "Native error code: %u", ulNativeErrCode);
        }
        else
        {
            strncpy(pstDCIHandle->acErrInfo, szErrInfo, sizeof(pstDCIHandle->acErrInfo)-1);
            pstDCIHandle->acErrInfo[sizeof(pstDCIHandle->acErrInfo)-1] = '\0';
        }

        pstDCIHandle->iErrCode = ulNativeErrCode;
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: MySQLGetErrorLevel
 功    能: 根据ODBC错误码确定错误级别
 输入参数: ulErrCode --- 错误码
 全局变量:
 返回结果: 错误级别
************************************************************************/
BYTE MySQLGetErrorLevel(UINT32 ulErrCode)
{
/*    UINT32      i;
    UINT32      ulErrNumber = 0;

    if (ulErrCode == 1)
    {
        return PDB_PRIMARYKEY_VIOLATED;
    }

    ulErrNumber = ARRAY_SIZE(g_aulOCIFatalErrorCodes);
    for(i=0; i<ulErrNumber; i++)
    {
        if(ulErrCode == g_aulOCIFatalErrorCodes[i])
            return PDB_ERROR_LEVEL_HIGH;
    }

    ulErrNumber = ARRAY_SIZE(g_aulOCINormalErrorCodes);
    for(i=0; i<ulErrNumber; i++)
    {
        if(ulErrCode == g_aulOCINormalErrorCodes[i])
            return PDB_ERROR_LEVEL_MID;
    } */

    return PDB_ERROR_LEVEL_LOW;
}
