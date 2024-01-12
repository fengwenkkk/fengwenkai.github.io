/******************************************************************************
ģ����          ��  Database Call Interface (DCI)
�ļ���          ��  pdb_mysql_core.c
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
 ��    ��: BOOL    PDBCheckPROC
 ��    ��: ������ݿ����Ӿ����ָ��
 �������: PDBPROC  ���ݿ�����ָ��

 ȫ�ֱ���: ��
 ���ؽ��: TRUE/FALSE
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
 ��    ��: DCIInit
 ��    ��: ��ʼ��DCI���û���
 �������:
 ȫ�ֱ���: ��
 ���ؽ��: PDB_SUCCESS or PDB_ERROR
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
 ��    ��: DCIExit
 ��    ��: �˳�DCI���û���
 �������:
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS
************************************************************************/
PDBRET MySQLExit(HANDLE hEnv)
{
    mysql_server_end();

    return PDB_SUCCESS;
}

/*******************************************************************************
* ��������: MySQLDiagConnError
* ��    ��: ��ϴ���ԭ��
* ��������: VOID
* ��    ��:
* ��������        ����              ����/���         ����
*
* ��������:
* ˵    ��:
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
* ��������: MySQLDiagStmtError
* ��    ��: ���STMT����
* ��������: VOID
* ��    ��:
* ��������        ����              ����/���         ����
*
* ��������:
* ˵    ��:
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
* ��������: SetCommitMode
* ��    ��: �����Զ��ύģʽ
* ��������: PDBRET
* ��    ��:
* ��������        ����              ����/���         ����
* enCommitMode   DCCI_COMMITMODE_E    IN             �Զ��ύģʽ
* ��������:
* ˵    ��:
*******************************************************************************/
PDBRET MySQLSetAutoCommit(MYSQL_DCI_HANDLE_T *pstDCIHandle, BOOL bAutoCommit)
{
    /* �ֲ��������� */
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
 ��    ��: DCIConnect
 ��    ��: ͨ��PDB CLI�������ݿ�����
 �������: pstDCIHandle     --- DCI���ݿ�����
           szServer         --- ���ݿ��������
           szDBName         --- ���ݿ�����
           szUserName       --- �û��ʺ�
           szPassword       --- �û�����
           pstOption        --- ���ݿ�����ѡ��
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
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

    /*����MYSQLѡ��*/
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

    /* ��������� */
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

    /* ���������ύģʽ */
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
 ��    ��: DCIStmtInit
 ��    ��: ͨ��PDB CLI�������ݿ����ӵ������
 �������: pstDCIHandle     --- DCI���ݿ�����
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_ERROR_CREATE_STMT or PDB_ERROR
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
 ��    ��: DCIStmtClose
 ��    ��: ͨ��PDB CLI�ر����ݿ����ӵ������
 �������: pstDCIHandle     --- DCI���ݿ�����
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
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
 ��    ��: PDBRET MySQLDisconnect(MYSQL_DCI_HANDLE_T* pstDCIHandle)
 ��    ��: �ر����ݿ�����
 �������: pstDCIHandle --- PDB CLI���ݿ�����
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR or PDB_INVALID_HANDLE
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

    /* ����Ƿ���STMT��� */
    if (NULL != pstDCIHandle->DBHandles.StmtHandle)
    {
        bRetCode = mysql_stmt_close((MYSQL_STMT*)pstDCIHandle->DBHandles.StmtHandle);
        if (bRetCode != MYSQL_OK)
        {
            return PDB_ERROR_FREE_STMT;
        }

        pstDCIHandle->DBHandles.StmtHandle = NULL;
    }

    /* ����Ƿ������Ӿ��Ϊ�� */
    if (NULL != pstDCIHandle->DBHandles.ConnHandle)
    {
        mysql_close((MYSQL*)pstDCIHandle->DBHandles.ConnHandle);
        pstDCIHandle->DBHandles.ConnHandle = NULL;
    }

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
PDBRET MySQLSQLPrepare(MYSQL_DCI_HANDLE_T *pstDCIHandle)
{
    INT32       iRet;
    //PDBUINT32 ulParamCount;
    //PDBUINT32 ulColumnCount;
    //MYSQL_RES     *pPrepareMetaRlt;

    DCI_CONN_STMT_HANDLE_CHECK;

    //DCIInitStmt(pstDCIHandle);

    // ���ö��ѯִ��
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
* ��������: SetPrefetchNum
* ��    ��: ����Ԥ��ȡ����
* ��������: PDBRET
* ��    ��:
* ��������        ����              ����/���         ����
* ulRowNum       DCCIUINT32           IN             ����
* ��������:
* ˵    ��:
*******************************************************************************/
PDBRET MySQLSetPrefetchNum(MYSQL_DCI_HANDLE_T* pstDCIHandle, UINT32 ulRowNum)
{
    UINT32 ulCursorType = (UINT32)CURSOR_TYPE_READ_ONLY;

    DCI_CONN_STMT_HANDLE_CHECK;

    /* ���ulRowNum�Ƿ����0 */
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
 ��    ��: DCIQuery
 ��    ��: ͨ��PDB CLIִ����׼���õ�SQL��ѯ���
 �������: pstDCIHandle --- PDB CLI���ݿ���������
           hStmtHandle  --- �����
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
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

    /* ���ʹ�����blob */
    /*if (PDB_SUCCESS != DCISendBlobData(pstDCIHandle, hStmtHandle))
    {
         return PDB_ERROR_BIND;
    }*/

    // ��귽ʽ
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
 ��    ��: DCIExec
 ��    ��: ͨ��PDB CLIִ����׼���õķǲ�ѯ��SQL���
 �������: pstDCIHandle --- PDB CLI���ݿ���������
           hStmtHandle  --- �����
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
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

    /* ���ʹ�����blob */
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
 ��    ��: DCIBindRsltByPos
 ��    ��: ͨ��PDB CLIִ����׼���õ�SQL���
 �������: pstDCIHandle --- PDB CLI���ݿ�����
           usRsltIndex  --- �󶨽��������index
           usRsltType   ---  �󶨽�������ж�Ӧ�ⲿ��������
           pRsltBuf     --- �󶨽�������л���������ָ��
           ulRsltBufLen --- �󶨽�������л���������ָ��
           pucNull      --- ��ʶ�ֶ��Ƿ�Ϊ�յ�������ָ��
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
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

    /* ������Ч�Լ�� */
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
 ��    ��: PDBRET MySQLCancel
 ��    ��: ��ֹ��ǰ����ִ�е����ݿ������Ϊ��һ�����ݿ��������׼��
 �������: pstDCIHandle --- PDB CLI���ݿ�����
           hStmtHandle  --- �����
           Mode         --- Cancel��ʽ PDB_UNBIND PDB_KEEPBIND
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET MySQLCancel(MYSQL_DCI_HANDLE_T *pstDCIHandle, BYTE bMode)
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

    /* �ͻ��˻��淽ʽ */
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
 ��    ��: DCIStoreRslt
 ��    ��: �������ݿ�Ԥ�����ѯ���صĽ�������ͻ��ˣ���DCIBindRslt֮��DCIRowFetchǰʹ��
 �������: pstDCIHandle --- PDB CLI���ݿ�����
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
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
 ��    ��: DCIRowFetch
 ��    ��: ͨ��PDB CLI��ȡ���ݿ��ѯ�������صĽ����
 �������: pstDCIHandle --- PDB CLI���ݿ�����
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
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
 ��    ��: DCIBindRslt
 ��    ��: �ͷ����ݿ��ѯ�������صĽ��������DCIRowFetchȫ��������ʹ��
 �������: pstDCIHandle --- PDB CLI���ݿ�����
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
************************************************************************/
PDBRET MySQLFreeRslt(MYSQL_DCI_HANDLE_T* pstDCIHandle)
{
    DCI_CONN_STMT_HANDLE_CHECK;

    mysql_stmt_free_result(pstDCIHandle->DBHandles.StmtHandle);
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
PDBRET MySQLTransBegin( MYSQL_DCI_HANDLE_T  *pstDCIHandle,
                      CHAR          *szTransName)
{
    if (szTransName == NULL || szTransName[0] == '\0')
    {
        return PDB_ERROR;
    }

    //DCI_CONN_STMT_HANDLE_CHECK;
    DCI_CONN_HANDLE_CHECK;

    /*��֧��Ƕ������*/
    if(pstDCIHandle->acTransName[0] != '\0')
    {
        strcpy(pstDCIHandle->acErrInfo, "Nesting transcation unsupport!");
        pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_LOW;

        return PDB_ERROR;
    }

    /* ��������̫�� */
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
 ��    ��: DCITransRollback
 ��    ��: ��ָ���������ع�һ�����ݿ�����
 �������: pstDCIHandle --- PDB CLI���ݿ�����
           szTransName  --- ��������
 ȫ�ֱ���: ��
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
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
 ��    ��: DCITransCommit
 ��    ��: ��ָ���������ύһ�����ݿ�����
 �������: pstDCIHandle --- PDB CLI���ݿ�����
           szTransName  --- ��������
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR
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
 ��    ��: MySQLRowCount
 ��    ��: ��õ�ǰ����Ӱ��ļ�¼����
 �������: pstDCIHandle --- PDB CLI���ݿ�����
           hStmtHandle  --- �����
 ȫ�ֱ���:
 ���ؽ��: ��ǰ����Ӱ��ļ�¼���� ����SELECT���ֻ�е�ȫ������FETCH����Ч
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
 ��    ��: DCIBindPara
 ��    ��: SQL��������
 �������: pstDCIHandle     --- Ŀ�����ݿ�����
           usParaIndex      --- ������ţ���0��ʼ˳�������
           usParaType       --- �ò�����SQL��������
           pParaValue       --- �ò��������ݻ�������ָ��(�ַ���Ϊ�䳤��)
           usParaLen        --- �����ݻ������ĳ���
           pParent          --- ����BLOB�󶨣�Ϊ��������ָ��
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_SUCCESS_INFO or PDB_ERROR or PDB_INVALID_HANDLE
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

    /* ������Ч�Լ�� */
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
 ��    ��: PDBRET MySQLGetError
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

    /* ���STMT���� */
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
 ��    ��: MySQLGetErrorLevel
 ��    ��: ����ODBC������ȷ�����󼶱�
 �������: ulErrCode --- ������
 ȫ�ֱ���:
 ���ؽ��: ���󼶱�
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
