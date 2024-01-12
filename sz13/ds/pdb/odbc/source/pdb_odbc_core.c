/******************************************************************************
ģ����          ��  Database Call Interface (DCI)
�ļ���          ��  pdb_odbc_core.c
����ļ�        ��
�ļ�ʵ�ֹ���    ��  ���ݿ�ײ���÷�����ʵ�ֲ�ͬ���ݿ���ʵ���ͬ���ý���ӿ�
����            ��
�汾            ��  V0.1
-------------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
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
 ��    ��: BOOL ODBCCheckHandle
 ��    ��: ������ݿ����Ӿ����ָ��
 �������: PDBPROC  ���ݿ�����ָ��

 ȫ�ֱ���: ��
 ���ؽ��: TRUE/FALSE
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
 ��    ��: DCIInit
 ��    ��: ��ʼ��DCI���û���
 �������:
 ȫ�ֱ���: ��
 ���ؽ��: PDB_SUCCESS or PDB_ERROR
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
//        PDBLog(LOG_WARN, "����ϵͳ��֧��ODBC3.0");
    }

    *phEnv = hEnv;

    return PDB_SUCCESS;
}

/**********************************************************************
 ��    ��: DCIExit
 ��    ��: �˳�DCI���û���
 �������:
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS
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
* ��������: ODBCDiagConnError
* ��    ��: ��ϴ���ԭ��
* ��������: VOID
* ��    ��:
* ��������        ����              ����/���         ����
*
* ��������:
* ˵    ��:
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
* ��������: ODBCDiagStmtError
* ��    ��: ���STMT����
* ��������: VOID
* ��    ��:
* ��������        ����              ����/���         ����
*
* ��������:
* ˵    ��:
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
* ��������: SetCommitMode
* ��    ��: �����Զ��ύģʽ
* ��������: PDBRET
* ��    ��:
* ��������        ����              ����/���         ����
* enCommitMode   DCCI_COMMITMODE_E    IN             �Զ��ύģʽ
* ��������:
* ˵    ��:
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
 ��    ��: DCIConnect
 ��    ��: ͨ��PDB CLI�������ݿ�����
 �������: pstDCIHandle     --- DCI���ݿ�����
           szServer         --- ���ݿ��������
           szUserName       --- �û��ʺ�
           szPassword       --- �û�����
           pstOption        --- ���ݿ�����ѡ��
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
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
        /* �������� */
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

        /* �������� */
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

    /* ����STMT��� */
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
 ��    ��: DCIStmtInit
 ��    ��: ͨ��PDB CLI�������ݿ����ӵ������
 �������: pstDCIHandle     --- DCI���ݿ�����
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_ERROR_CREATE_STMT or PDB_ERROR
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
 ��    ��: DCIStmtClose
 ��    ��: ͨ��PDB CLI�ر����ݿ����ӵ������
 �������: pstDCIHandle     --- DCI���ݿ�����
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
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
 ��    ��: PDBRET ODBCDisconnect(ODBC_DCI_HANDLE_T* pstDCIHandle)
 ��    ��: �ر����ݿ�����
 �������: pstDCIHandle --- PDB CLI���ݿ�����
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR or PDB_INVALID_HANDLE
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

    /* ����Ƿ���STMT��� */
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

    /* ������Ӿ��Ϊ�� */
    if (NULL == pstDCIHandle->DBHandles.ConnHandle)
    {
        return PDB_SUCCESS;
    }

    /* �ͷ����� */
    sRetCode = SQLDisconnect(pstDCIHandle->DBHandles.ConnHandle);
    if (sRetCode != SQL_SUCCESS)
    {
        ODBCDiagConnError(pstDCIHandle);
        return PDB_ERROR_FREE_STMT;
    }

    /* �ͷž�� */
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
 ��    ��: DCISQLPrepare
 ��    ��: Ԥ����SQL���
 �������: pstDCIHandle     --- PDB CLI���ݿ���������
           hStmtHandle      --- �����
           szStatementText  --- SQL���
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
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
* ��������: ODBCSetStmtAttr
* ��    ��: ����STMT����
* ��������: PDBRET
* ��    ��:
* ��������        ����              ����/���         ����
* ulRowNum       DCCIUINT32           IN             ����
* ��������:
* ˵    ��:
*******************************************************************************/
PDBRET ODBCSetStmtAttr(ODBC_DCI_HANDLE_T* pstDCIHandle, PDB_STMT_ATTR_E enAttr, VOID *pvValue, UINT32 ulStrLen)
{
    SQLRETURN           sRet = -1;
    SQLINTEGER          lAttr = -1;

    /* ָ���� */
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
* ��������: SetPrefetchNum
* ��    ��: ����Ԥ��ȡ����
* ��������: PDBRET
* ��    ��:
* ��������        ����              ����/���         ����
* ulRowNum       DCCIUINT32           IN             ����
* ��������:
* ˵    ��:
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
 ��    ��: DCIQuery
 ��    ��: ͨ��PDB CLIִ����׼���õ�SQL��ѯ���
 �������: pstDCIHandle --- PDB CLI���ݿ���������
           hStmtHandle  --- �����
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
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

    /* ���ʹ�����blob */
/*  if (PDB_SUCCESS != DCISendBlobData(pstDCIHandle, hStmtHandle))
    {
         return PDB_ERROR_BIND;
    } */

    // ��귽ʽ
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
 ��    ��: DCIExec
 ��    ��: ͨ��PDB CLIִ����׼���õķǲ�ѯ��SQL���
 �������: pstDCIHandle --- PDB CLI���ݿ���������
           hStmtHandle  --- �����
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
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

    /* ���ʹ�����blob */
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
 ��    ��: DCIBindRsltByPos
 ��    ��: ͨ��PDB CLIִ����׼���õ�SQL���
 �������: pstDCIHandle --- PDB CLI���ݿ�����
           usRsltIndex  --- �󶨽��������index
           usRsltType   ---  �󶨽�������ж�Ӧ�ⲿ��������
           pRsltBuf     --- �󶨽�������л���������ָ��
           ulRsltBufLen --- �󶨽�������л���������
           piLenInd     --- ���Ȼ��ʶ
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
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
 ��    ��: PDBRET ODBCCancel
 ��    ��: ��ֹ��ǰ����ִ�е����ݿ������Ϊ��һ�����ݿ��������׼��
 �������: pstDCIHandle --- PDB CLI���ݿ�����
           hStmtHandle  --- �����
           Mode         --- Cancel��ʽ PDB_UNBIND PDB_KEEPBIND
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET ODBCCancel(ODBC_DCI_HANDLE_T *pstDCIHandle, BYTE bMode)
{
    return PDB_SUCCESS;
}

/**********************************************************************
 ��    ��: DCIBindRslt
 ��    ��: �����ݿ��ѯ�������صĽ��������DCIRowFetchǰʹ��
 �������: pstDCIHandle --- PDB CLI���ݿ�����
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET ODBCBindRslt(ODBC_DCI_HANDLE_T* pstDCIHandle)
{
    return PDB_SUCCESS;
}

/**********************************************************************
 ��    ��: DCIStoreRslt
 ��    ��: �������ݿ�Ԥ�����ѯ���صĽ�������ͻ��ˣ���DCIBindRslt֮��DCIRowFetchǰʹ��
 �������: pstDCIHandle --- PDB CLI���ݿ�����
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET ODBCStoreRslt(ODBC_DCI_HANDLE_T* pstDCIHandle)
{
    return PDB_SUCCESS;
}

/**********************************************************************
 ��    ��: DCIRowFetch
 ��    ��: ͨ��PDB CLI��ȡ���ݿ��ѯ�������صĽ����
 �������: pstDCIHandle --- PDB CLI���ݿ�����
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
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
 ��    ��: DCIBindRslt
 ��    ��: �ͷ����ݿ��ѯ�������صĽ��������DCIRowFetchȫ��������ʹ��
 �������: pstDCIHandle --- PDB CLI���ݿ�����
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
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
 ��    ��: DCITransBegin
 ��    ��: ��ָ����������ʼһ�����ݿ�����
 �������: pstDCIHandle --- PDB CLI���ݿ�����
           szTransName  --- ��������
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
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
 ��    ��: DCITransRollback
 ��    ��: ��ָ���������ع�һ�����ݿ�����
 �������: pstDCIHandle --- PDB CLI���ݿ�����
           szTransName  --- ��������
 ȫ�ֱ���: ��
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
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
 ��    ��: DCITransCommit
 ��    ��: ��ָ���������ύһ�����ݿ�����
 �������: pstDCIHandle --- PDB CLI���ݿ�����
           szTransName  --- ��������
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
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
 ��    ��: ODBCRowCount
 ��    ��: ��õ�ǰ����Ӱ��ļ�¼����
 �������: pstDCIHandle --- PDB CLI���ݿ�����
           hStmtHandle  --- �����
 ȫ�ֱ���:
 ���ؽ��: ��ǰ����Ӱ��ļ�¼���� ����SELECT���ֻ�е�ȫ������FETCH����Ч
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
 ��    ��: DCIBindPara
 ��    ��: SQL��������
 �������: pstDCIHandle     --- Ŀ�����ݿ�����
           usParaIndex      --- ������ţ���0��ʼ˳�������
           usParaType       --- �ò�����SQL��������
           pParaValue       --- �ò��������ݻ�������ָ��(�ַ���Ϊ�䳤��)
           ulParaLen        --- �����ݻ������ĳ���
           piLenInd         --- ���Ȼ��־
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR or PDB_INVALID_HANDLE
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

    /* ������Ч�Լ�� */
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
 ��    ��: PDBRET ODBCGetError
 ��    ��: ����PDB CLI�������ȡPDB������Ϣ
 �������: pstDCIHandle     --- PDB CLI���ݿ�����
           nResult      --- PDB CLI���ؽ��
           StmtHandle   --- �����ָ��
           BufferLen    --- ������Ϣ����������
 �������: ErrorInfo    --- ������Ϣ
           pucErrLevel    --- ���󼶱�
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS | PDB_ERROR | PDB_INVALID_HANDLE | PDB_NO_DATA
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
 ��    ��: ODBCGetErrorLevel
 ��    ��: ����ODBC������ȷ�����󼶱�
 �������: szErrState --- ODBC����״̬��
 ȫ�ֱ���:
 ���ؽ��: ���󼶱�
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
