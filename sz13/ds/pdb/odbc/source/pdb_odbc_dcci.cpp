/******************************************************************************
ģ����          ��  Database C++ Call Interface (DCCI)
�ļ���          ��  pdb_dcci.cpp
����ļ�        ��
�ļ�ʵ�ֹ���    ��  ���ݿ����API,��װDCI�㺯��
����            ��
�汾            ��  V0.1
-------------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
******************************************************************************/

#include "g_include.h"

#include "pdb_def.h"
#include "pdb_util.h"
#include "pdb_error.h"
#include "pdb_common.h"
#include "pdb_dcci.h"
#include "pdb_odbc_core.h"

/**********************************************************************
 ��    ��: CreateEnv
 ��    ��: �������ݿ⻷��
 ����˵��:
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_ERROR
************************************************************************/
PDBRET PDBODBCEnv::CreateEnv(VOID)
{
    PDBRET enRet = PDB_ERROR;

    /*���价�����*/
    enRet = ODBCInit(&m_hEnv);
    if (enRet != PDB_SUCCESS)
    {
        return PDB_ERROR_CREATE_ENV;
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 ��    ��: FreeEnv
 ��    ��: �ͷ����ݿ⻷�������ݿ�����
 ����˵��:
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_ERROR
************************************************************************/
PDBRET PDBODBCEnv::FreeEnv(VOID)
{
    UINT32  i;

    for (i=0; i<m_ulConnNum; i++)
    {
        if (m_pConn[i])
        {
            FreeConn(m_pConn[i]->GetConnId());
        }
    }

    m_ulConnNum = 0;

    ODBCExit(m_hEnv);
    m_hEnv = NULL;

    DCIDeleteLock(m_hLockId);
    m_hLockId = INVALID_LOCK;

    return PDB_SUCCESS;
}

/*******************************************************************************
* ��������: CreateConn
* ��    ��: ����һ�����ݿ����ӣ�����Ӧ�õ����ݿ��ʼ���д���һ������������
* ��������: DCCI_RET_E
* ��    ��:
* ��������        ����              ����/���         ����
* **ppConn      DCCIVOID             OUT           ���ݿ�����
* ��������:
* ˵    ��:
*******************************************************************************/
PDBRET PDBODBCEnv::CreateConn()
{
    if (m_ulConnNum >= PDB_MAX_CONNECT_NUM)
    {
        return PDB_ERROR_CREATE_CONN;
    }

    if (!DCILock(m_hLockId, PDB_WAIT_FOREVER))
    {
        return PDB_ERROR_CREATE_CONN;
    }

    m_pConn[m_ulConnNum] = new PDBODBCConn(this);

    /* ָ����Ч���ж� */
    if (NULL == m_pConn[m_ulConnNum])
    {
        DCIUnlock(m_hLockId);
        return PDB_ERROR_CREATE_CONN;
    }

    /* m_pConn������ID��1��ʼ */
    m_pConn[m_ulConnNum]->SetConnId(m_ulConnNum+1); //((INVALID_INDEX == iMaxConnId)?1:(iMaxConnId+1));
    m_ulConnNum++;

    DCIUnlock(m_hLockId);

    return PDB_SUCCESS;
}

PDBRET PDBODBCEnv::FreeConn(UINT32 ulConnID)
{
    if (!DCILock(m_hLockId, PDB_WAIT_FOREVER))
    {
        return PDB_ERROR_FREE_CONN;
    }

    if (ulConnID >= m_ulConnNum)
    {
        DCIUnlock(m_hLockId);

        return PDB_ERROR_FREE_CONN;
    }

    if (m_pConn[ulConnID])
    {
        Disconnect(m_pConn[ulConnID]);

        PDBODBCConn     *pConn = (PDBODBCConn*)m_pConn[ulConnID];

        delete pConn;
        m_pConn[ulConnID] = NULL;
    }

    DCIUnlock(m_hLockId);

    return PDB_SUCCESS;
}

/**********************************************************************
 ��    ��: PDBODBCConn
 ��    ��: ���캯��
 ����˵��:
 ȫ�ֱ���:
 ���ؽ��:
************************************************************************/
PDBODBCConn::PDBODBCConn(PDBEnvBase *pEnv)
{
    m_pEnv = pEnv;

    Init();
}

PDBODBCConn::~PDBODBCConn()
{
    GOS_FREE(m_pPDBHandle);

    DCIDeleteLock(m_hConnectLockId);
    DCIDeleteLock(m_hAllocLockId);
    DCIDeleteLock(m_hConnLockId);

    m_hConnectLockId = NULL;
    m_hAllocLockId = NULL;
    m_hConnLockId = NULL;
}

/**********************************************************************
 ��    ��: Init
 ��    ��: ���ݿ����ӳ�ʼ��
 ����˵��:
 ȫ�ֱ���:
 ���ؽ��:
************************************************************************/
PDBRET PDBODBCConn::Init()
{
    memset(&m_stConnParam, 0, sizeof(m_stConnParam));
    memset(&m_stDBOption, 0, sizeof(m_stDBOption));

    m_bReConnect = FALSE;

    m_hConnectLockId = NULL;
    m_hAllocLockId = NULL;
    m_hConnLockId = NULL;

    m_ulLockTimeout = LOCK_TIMEOUT;

    m_pPDBHandle = (ODBC_PDB_HANDLE_T *)calloc(1, sizeof(ODBC_PDB_HANDLE_T));
    if (NULL == m_pPDBHandle)
    {
        return PDB_ERROR;
    }

    // ��ʼ��������
    m_hConnectLockId = DCICreateLock();
    if (NULL == m_hConnectLockId)
    {
        return PDB_ERROR;
    }

    m_hAllocLockId = DCICreateLock();
    if (NULL == m_hAllocLockId)
    {
        DCIDeleteLock(m_hConnectLockId);
        m_hConnectLockId = NULL;
        return PDB_ERROR;
    }

    m_hConnLockId = DCICreateLock();
    if (NULL == m_hConnLockId)
    {
        DCIDeleteLock(m_hConnectLockId);
        m_hConnectLockId = NULL;

        DCIDeleteLock(m_hAllocLockId);
        m_hAllocLockId = NULL;

        return PDB_ERROR;
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 ��    ��: CheckCurConn
 ��    ��: ��鵱ǰ�����Ƿ���Ч
 ����˵��:
 ȫ�ֱ���:

 ���ؽ��: TRUE or FALSE
************************************************************************/
BOOL PDBODBCConn::CheckConn() //PDBUINT32 ulLockId, PDBUINT32 ulLockTimeout
{
    BOOL            bOrgAlloced = FALSE;
    PDBRET          enRet = PDB_ERROR;
    CHAR            *szSql = (CHAR*)"select 1";
    BOOL            bRet = FALSE;
    CHAR            acErrInfo[1024];

    /*
    if (INVALID_LOCK == ulLockId)
    {
        return FALSE;
    }

    if (FALSE == DCILock(ulLockId, ulLockTimeout))
    {
        return FALSE;
    }
    */

    bOrgAlloced = IsAlloced();

    if (!bOrgAlloced)
    {
//        PDBLog(LOG_INFO, "PDBODBCConn::CheckConn set alloc status {true} begin[%ld] ", GetConnId());
        if (!SetAllocStatus(TRUE))
        {
            PDBLog(LOG_INFO, "PDBODBCConn::CheckConn set alloc status {true} fail[%ld]" , GetConnId());
            return FALSE;
        }
    }

    //DCIUnlock(ulLockId);

    /*
    if (FALSE == bDoCheck)
    {
        PDBLog(LOG_ERROR, "PDBODBCConn::CheckConn connect[%ld] is in use ", GetConnId());
        return TRUE;
    }*/

//    PDBLog(LOG_INFO, "PDBODBCConn::CheckConn begin real check[%ld] ", GetConnId());

    /* ��ʼ�����ݿ������ */
    enRet = InitStmt();

    if (PDB_SUCCESS == enRet)
    {
        enRet = Clear();
    }

/*  if (PDB_SUCCESS == enRet)
    {
        enRet = OpenCursor();
    } */

    if (PDB_SUCCESS == enRet)
    {
        enRet = Query((CHAR*)NULL, szSql);

        enRet = FreeRslt();
    }

    //if (PDB_SUCCESS == enRet)
    //{
    enRet = CloseStmt();
    //}

    if (PDB_SUCCESS != enRet)
    {
        /* ���������⣬���� */
        GetErrInfo(acErrInfo);
        PDBLog(LOG_INFO, "PDBODBCConn::CheckConn(%d) failed, error code: %u, error info: %s", GetConnId(), GetErrCode(), acErrInfo);

        if (PDB_SUCCESS != ReConnect())
        {
            GetErrInfo(acErrInfo);
            PDBLog(LOG_INFO, "PDBODBCConn::CheckConn(%d) reconnect failed, error code: %u, error info: %s", GetConnId(), GetErrCode(), acErrInfo);
        }
        //Prn("[fatal] PDBODBCConn::CheckConn connect is invalid reconnect ");
    }
    else
    {
        bRet = TRUE;
    }

    if (!SetAllocStatus(bOrgAlloced) &&
        FALSE == bOrgAlloced)
    {
        PDBLog(LOG_INFO, "PDBODBCConn::CheckConn set alloc status {%u} fail[%ld] ", bOrgAlloced, GetConnId());
        return FALSE;
    }

    return bRet;
}

/**********************************************************************
 ��    ��: ReConnect
 ��    ��: �����������ݿ�
 ����˵��:
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_ERROR
************************************************************************/
PDBRET PDBODBCConn::ReConnect()
{
    m_bReConnect = TRUE;

    return Connect(&m_stConnParam, &m_stDBOption);
}

/**********************************************************************
 ��    ��: Connect
 ��    ��: �������ݿ�����
 ����˵��: szServer     IN  ������IP
           usPort       IN  ���ݿ�ʵ���˿�
           szDBName     IN  ���ݿ���
           szUserName   IN  �û�
           szPassword   IN  ����
           pstDBOption  IN  ���ݿ����ָ��
           ȫ�ֱ���:
           ���ؽ��: PDB_SUCCESS or PDB_ERROR
************************************************************************/
PDBRET PDBODBCConn::Connect(PDB_CONN_PARAM_T *pstConnParam, PDB_OPTION_T *pstDBOption)
{
    //PDB_OPTION_T  stOption = {0};
    ODBC_DCI_HANDLE_T   *pstDCIHandle;
    ODBC_PDB_HANDLE_T   *pstPDBHandle = NULL;

    //Prn("PDBODBCConn::Connect ");

    if (NULL == m_pPDBHandle)
    {
        pstPDBHandle = (ODBC_PDB_HANDLE_T *)calloc(1, sizeof(ODBC_PDB_HANDLE_T));
        if(NULL == pstPDBHandle)
        {
            return PDB_ERROR;
        }

        m_pPDBHandle = pstPDBHandle;

    }
    else
    {
        pstPDBHandle = (ODBC_PDB_HANDLE_T *)m_pPDBHandle;
    }

    //    stOption.ulSelRowCount = 0;   /* һ�β�ѯ������¼���� */
    /*
    stOption.ulPrefetchRow = PDB_PREFETCH_ROW_NUM;
    stOption.ulPrefetchMem = 0;
    stOption.usBatchNum    = 0;
    */

    if (IsConnected()) //pstPDBHandle->bIsConnected
    {
        Disconnect();
    }

    pstDCIHandle = (ODBC_DCI_HANDLE_T *)&pstPDBHandle->stDCIHandle;
    pstDCIHandle->EnvHandle = m_pEnv->GetEnv();
    if (PDB_SUCCESS != ODBCConnect(pstDCIHandle, pstConnParam, pstDBOption))
    {
        SetConnectStatus(FALSE);
        //pstPDBHandle->bIsConnected = FALSE;

        PDBLog(LOG_ERROR, (CHAR*)pstDCIHandle->acErrInfo);
        /*
        OsLock(&g_hsDBConnLock);
        if (g_bDBConnNum > 0)
        g_bDBConnNum --;
        OsUnlock(&g_hsDBConnLock);
        */
        return PDB_ERROR;
    }

    // pstPDBHandle->iErrCode = 0;

    /* �������ı�alloc״̬ */
    if (!m_bReConnect)
    {
        SetAllocStatus(FALSE);
        //pstPDBHandle->bIsAlloced = FALSE;
    }
    /*
    else
    {
        if (pstPDBHandle->bIsAlloced)
        {
            printf("alloced");
        }
        else
        {
            printf("not alloced");
        }
    }
    */

    SetConnectStatus(TRUE);
    //pstPDBHandle->bIsConnected = TRUE;

    pstDCIHandle->ulExecNum = 1;
    //ODBCSetOption(pstDCIHandle, &stOption);

    memcpy(&m_stConnParam, pstConnParam, sizeof(m_stConnParam));
    memcpy(&m_stDBOption, pstDBOption, sizeof(m_stDBOption));

    m_bReConnect = FALSE;

    return PDB_SUCCESS;
}

/*=====================================================================
��    ��: Disconnect
��    ��: �����ݿ�Ͽ�����
�������:
�������:
ȫ�ֱ���:
���ؽ��:
======================================================================*/
PDBRET PDBODBCConn::Disconnect()
{
    PDBRET          enRet;
    ODBC_PDB_HANDLE_T   *pstHandle = NULL;

    //Prn("PDBODBCConn::Disconnect ");

    pstHandle = (ODBC_PDB_HANDLE_T*)m_pPDBHandle;

    PDB_CONN_HANDLE_CHECK(pstHandle);
        //PDB_CONN_STMT_HANDLE_CHECK(pstHandle);

    enRet = ODBCDisconnect((ODBC_DCI_HANDLE_T*)&pstHandle->stDCIHandle);

    if (enRet == PDB_SUCCESS)
    {
        SetConnectStatus(FALSE);
        //pstHandle->bIsConnected = FALSE;

        /* �������ı�alloc״̬ */
        if (!m_bReConnect)
        {
            SetAllocStatus(FALSE);
            //pstHandle->bIsAlloced = FALSE;
        }
        /*
        else
        {
            if (pstHandle->bIsAlloced)
            {
                printf("alloced");
            }
            else
            {
                printf("not alloced");
            }
        }
        */

        return PDB_SUCCESS;
    }
    else
    {
        return PDB_ERROR;
    }
}

/*=====================================================================
��    ��: Clear
��    ��: ���ݿ����ǰ�ĳ�ʼ��
�������: ��
�������:
ȫ�ֱ���:
���ؽ��: �ɹ�
======================================================================*/
PDBRET PDBODBCConn::Clear()
{
    ODBC_PDB_HANDLE_T   *pstHandle = NULL;
    ODBC_DCI_HANDLE_T   *pstDCIHandle = NULL;

    //Prn("PDBODBCConn::Clear ");

    pstHandle = (ODBC_PDB_HANDLE_T*)m_pPDBHandle;
    //PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    PDB_CONN_HANDLE_CHECK(pstHandle);
    pstDCIHandle = &pstHandle->stDCIHandle;

    //pstDCIHandle->stOption.ulPrefetchRow = PDB_PREFETCH_ROW_NUM;
    //pstDCIHandle->stOption.ulPrefetchMem = 0;

    /* Ĭ��Ϊ���β�����ִ��һ�� */
//    pstDCIHandle->stOption.usBatchNum =  0;

    pstDCIHandle->ulExecNum  = 1;
    pstDCIHandle->ulRowCount = 0;

    pstDCIHandle->iErrCode = 0;
    pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_NULL;
    pstDCIHandle->acErrInfo[0] = '\0';

    //pstDCIHandle->acTransName[0] = '\0';
    pstDCIHandle->acSQLCmd[0] = '\0';

    pstDCIHandle->ulBindNum = 0;
/*
    pstDCIHandle->ulSendBlobParamNum = 0;
    pstDCIHandle->ulBindRsltNum = 0; */
    pstDCIHandle->ulFetchedNum = 0;

    //pstDCIHandle->DBHandles.pstQryRlst = NULL;
    //pstDCIHandle->DBHandles.pszQryRow = NULL;

    return PDB_SUCCESS;
}

/*=====================================================================
��    ��: InitStmt
��    ��: ��ʼ�����ݿ������
�������: ��
�������:
ȫ�ֱ���:
���ؽ��: �ɹ�
======================================================================*/
PDBRET PDBODBCConn::InitStmt()
{
    PDBRET      enRet;
    ODBC_PDB_HANDLE_T   *pstHandle = NULL;

    //Prn("PDBODBCConn::InitStmt ");

    pstHandle = (ODBC_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_HANDLE_CHECK(pstHandle);

    enRet = ODBCInitStmt(&pstHandle->stDCIHandle);
    PDB_CHECK_ERR(enRet);

    return PDB_SUCCESS;
}

/*=====================================================================
��    ��: CloseStmt
��    ��: �ر����ݿ������
�������: ��
�������:
ȫ�ֱ���:
���ؽ��: �ɹ�
======================================================================*/
PDBRET PDBODBCConn::CloseStmt()
{
    PDBRET      enRet;
    ODBC_PDB_HANDLE_T   *pstHandle = NULL;

    //Prn("PDBODBCConn::CloseStmt ");
    pstHandle = (ODBC_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    enRet = ODBCCloseStmt(&pstHandle->stDCIHandle);
    PDB_CHECK_ERR(enRet);

    return PDB_SUCCESS;
}

/*=====================================================================
��    ��: BindByPos
��    ��: SQL����ı�������
�������:   ulType   --- �����������
            pData    --- �����������
            iLen     --- �����������
            bIsOutPara--- ���α�־
ȫ�ֱ���:
���ؽ��: PDB_SUCCESS | PDB_ERROR
======================================================================*/
PDBRET PDBODBCConn::BindByPos(UINT32 ulType, VOID *pData, INT32 iLen, UINT32 ulInoutType, INT32 *piLenInd)
{
    PDBRET          enRet;
//    UINT32          ulStrLen;
    UINT32          ulParaNo;
    ODBC_PDB_HANDLE_T   *pstHandle = NULL;

    //Prn("PDBODBCConn::BindByPos ");

    pstHandle = (ODBC_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);

    ulParaNo = pstHandle->stDCIHandle.ulBindNum;

    if (ulParaNo >= PDB_MAX_BIND_NUM)
    {
        return PDB_ERROR;
    }

   /* if (ulType == PDBT_STRING)
    {
        ulStrLen = strlen((CHAR*)pData) + 1;

        if (ulStrLen < iLen)
        {
            iLen = ulStrLen;
        }

        //iLen = strlen((CHAR*)pData)+1;
    } */

    enRet = ODBCBindPara(&pstHandle->stDCIHandle, (UINT16)ulParaNo, ulType, ulInoutType, pData, (UINT32)iLen, piLenInd);

    /*
    pstPDBHandle->stDCIHandle.astBindHandle[ulParaNo].ulType  = ulType;
    pstPDBHandle->stBindInfo.astBindPara[ulParaNo].pValue  = pData;
    pstPDBHandle->stBindInfo.astBindPara[ulParaNo].iLen    = iLen;
    pstPDBHandle->stBindInfo.astBindPara[ulParaNo].bIsOutPara  = bIsOutPara;
    */

    pstHandle->stDCIHandle.ulBindNum ++;
    PDB_CHECK_ERR(enRet);

    return PDB_SUCCESS;
}

/*====================================================================
������      : BindRsltByPos
����        : ������ѯʱ���α��Ӧ���ֶ�
�㷨ʵ��    :
����ȫ�ֱ���:   usRsltIndex -- �󶨽��������index
                usDataType --  �󶨽�������ж�Ӧ�ⲿ��������
                pBuf -- �󶨽�������л������׵�ַ
                pulBufLen -- �󶨽�������л���������ָ�루���/���Σ�
                pucNull -- ��ʶ�ֶ��Ƿ�Ϊ�յ�������ָ��
�������˵��:
����ֵ˵��  :
====================================================================*/
PDBRET PDBODBCConn::BindRsltByPos(UINT16    usRsltIndex,
                                  UINT16    usDataType,
                                  VOID      *pBuf,
                                  UINT32    ulBufLen,
                                  BOOL      *pbIsNull,
                                  INT32     *piLenInd)
{
    PDBRET          enRet;
    ODBC_PDB_HANDLE_T   *pdbHandle = NULL;

    pdbHandle = (ODBC_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pdbHandle);

    enRet = ODBCBindRsltByPos(&pdbHandle->stDCIHandle, usRsltIndex, usDataType, pBuf, ulBufLen, pbIsNull, piLenInd);
    PDB_CHECK_ERR(enRet);

    return PDB_SUCCESS;
}

/*=====================================================================
��    ��: OpenCursor
��    ��: �򿪲�ѯ���
�������:
�������: ��
ȫ�ֱ���: ��
���ؽ��: PDB_SUCCESS | PDB_ERROR
======================================================================*/
PDBRET PDBODBCConn::OpenCursor()
{
    PDBRET          enRet = PDB_SUCCESS;
    ODBC_DCI_HANDLE_T   *pstDCIHandle;
    ODBC_PDB_HANDLE_T   *pstHandle = NULL;

    pstHandle = (ODBC_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    pstDCIHandle = (ODBC_DCI_HANDLE_T *)&pstHandle->stDCIHandle;

    enRet = ODBCSetPrefetchNum(pstDCIHandle, pstDCIHandle->stOption.ulPrefetchRow);

    return CheckErr(enRet);
}

/*=====================================================================
��    ��: Query
��    ��:
�������:
�������: ��
ȫ�ֱ���: ��
���ؽ��: PDB_SUCCESS | PDB_ERROR
======================================================================*/
PDBRET PDBODBCConn::Query(CHAR *szTransName, CHAR *szSQL)
{
    PDBRET      enRet;
    ODBC_DCI_HANDLE_T   *pstDCIHandle;
    ODBC_PDB_HANDLE_T   *pstHandle = NULL;

    pstHandle = (ODBC_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    pstDCIHandle = (ODBC_DCI_HANDLE_T *)&pstHandle->stDCIHandle;

    if (NULL == szSQL || szSQL[0] == '\0')
    {
        PDBLog(LOG_ERROR, "PDBODBCConn::Query sql is null");
//      MYSQL_STMT_CLOSE(pstDCIHandle->DBHandles.StmtHandle);
        return PDB_ERROR_SET_ATTR;
    }

    if (szTransName == NULL || szTransName[0] == '\0')
    {
        /* �����Զ��ύ */
        enRet = ODBCSetAutoCommit(pstDCIHandle, TRUE);
        if (enRet != PDB_SUCCESS)
        {
            PDBLog(LOG_ERROR, "PDBODBCConn::Query set auto commit(true) fail ");
        //  MYSQL_STMT_CLOSE(pstDCIHandle->DBHandles.StmtHandle);
            return PDB_ERROR_SET_ATTR;
        }
        pstDCIHandle->acTransName[0] = '\0';
    }
    else
    {
        /* ����Ϊ���� */
        enRet = ODBCSetAutoCommit(pstDCIHandle, FALSE);
        if (enRet != PDB_SUCCESS)
        {
            PDBLog(LOG_ERROR, "PDBODBCConn::Query set auto commit(false) fail ");
        //  MYSQL_STMT_CLOSE(pstDCIHandle->DBHandles.StmtHandle);
            return PDB_ERROR_SET_ATTR;
        }
        strcpy((CHAR *)pstDCIHandle->acTransName, szTransName);
    }

    strncpy(pstDCIHandle->acSQLCmd, szSQL, PDB_MAX_SQL_LEN);
    pstDCIHandle->acSQLCmd[PDB_MAX_SQL_LEN] = '\0';

    pstDCIHandle->ulRowCount = 0;

    /* ��ʼ��stmt��prepare sql */
    enRet = ODBCSQLPrepare(pstDCIHandle);
    PDB_CHECK_ERR(enRet);

    /* Exec SQL Command */
    //pstPDBHandle->usExecNum = 1;
    enRet = ODBCQuery(pstDCIHandle);

    PDB_CHECK_ERR(enRet);
    /*
    ulRowCount = DCIGetRsltCount(pstDCIHandle, &hStmtHandle);

      if (ulRowCount > 0)
      pstPDBHandle->ulRowCount = ulRowCount;
      else
      pstPDBHandle->ulRowCount = 0;
    */

    switch (enRet)
    {
        case PDB_SUCCESS:
        case PDB_SUCCESS_WITH_INFO:
        case PDB_NO_DATA:
            return enRet;

        default:
            return PDB_ERROR;
    }
}

/*====================================================================
������      :BindRslt
����        :�󶨽��������RowFetchǰʹ��
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��:
����ֵ˵��  :
====================================================================*/
PDBRET PDBODBCConn::BindRslt()
{
    PDBRET          enRet;
    //PDBINT32        ulRowCount;
    ODBC_DCI_HANDLE_T   *pstDCIHandle;
    ODBC_PDB_HANDLE_T   *pstHandle = NULL;

    pstHandle = (ODBC_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    pstDCIHandle = (ODBC_DCI_HANDLE_T*)&pstHandle->stDCIHandle;

    enRet = ODBCBindRslt(pstDCIHandle);

    /*
    ulRowCount = DCulRowCount(pstDCIHandle);
    if (ulRowCount > 0)
    {
        pstDCIHandle->ulRowCount = ulRowCount;
    }
    else
    {
        pstDCIHandle->ulRowCount = 0;
    }
    */

    PDB_CHECK_ERR(enRet);

    return PDB_SUCCESS;
}

/*====================================================================
������      :BindRslt
����        :�󶨽��������RowFetchǰʹ��
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��:
����ֵ˵��  :
====================================================================*/
PDBRET PDBODBCConn::StoreClientRslt()
{
    PDBRET          enRet;
    INT32           ulRowCount;
    ODBC_DCI_HANDLE_T   *pstDCIHandle;
    ODBC_PDB_HANDLE_T   *pstHandle = NULL;

    pstHandle = (ODBC_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    pstDCIHandle = (ODBC_DCI_HANDLE_T*)&pstHandle->stDCIHandle;

    enRet = ODBCStoreRslt(pstDCIHandle);

    ulRowCount = 0;// DCIRowCount(pstDCIHandle);
    if (ulRowCount > 0)
    {
        pstDCIHandle->ulRowCount = ulRowCount;
    }
    else
    {
        pstDCIHandle->ulRowCount = 0;
    }

    PDB_CHECK_ERR(enRet);

    return PDB_SUCCESS;
}

/*====================================================================
������      :RowFetch
����        :�ӽ���л�ȡһ��
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��:   pstDCIHandle --- PDB CLI���ݿ�����
����ֵ˵��  :
====================================================================*/
PDBRET PDBODBCConn::RowFetch()
{
    PDBRET          enRet;
    ODBC_DCI_HANDLE_T   *pstDCIHandle;
    ODBC_PDB_HANDLE_T   *pstHandle = NULL;

    pstHandle = (ODBC_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    pstDCIHandle = (ODBC_DCI_HANDLE_T*)&pstHandle->stDCIHandle;

    enRet = ODBCRowFetch(pstDCIHandle);

    enRet = CheckErr(enRet);

    if (enRet == PDB_NO_DATA)
    {
        //pstDCIHandle->stOption.ulPrefetchRow = PDB_PREFETCH_ROW_NUM;
        //pstDCIHandle->stOption.ulPrefetchMem = 0;

        return PDB_NO_DATA;
    }

    if (enRet != PDB_SUCCESS && enRet != PDB_SUCCESS_WITH_INFO && enRet != PDB_CONTINUE)
    {
        //pstDCIHandle->stOption.ulPrefetchRow = PDB_PREFETCH_ROW_NUM;
        //pstDCIHandle->stOption.ulPrefetchMem = 0;

        return PDB_ERROR;
    }

    return PDB_CONTINUE;
}

/*====================================================================
������      :FreeRslt
����        :�󶨽��������StoreClientRslt��RowFetch��ʹ��
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��:
����ֵ˵��  :
====================================================================*/
PDBRET PDBODBCConn::FreeRslt()
{
    PDBRET          enRet;
    ODBC_DCI_HANDLE_T   *pstDCIHandle;
    ODBC_PDB_HANDLE_T   *pstHandle = NULL;

    pstHandle = (ODBC_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    pstDCIHandle = (ODBC_DCI_HANDLE_T*)&pstHandle->stDCIHandle;

    enRet = ODBCFreeRslt(pstDCIHandle);

    return CheckErr(enRet);
}

VOID PDBODBCConn::SetBatchExecParam(UINT32 ulSkipLen, UINT32 ulExecNum)
{
    ODBC_DCI_HANDLE_T   *pstDCIHandle;
    ODBC_PDB_HANDLE_T   *pstHandle = NULL;

    pstHandle = (ODBC_PDB_HANDLE_T*)m_pPDBHandle;
    if (pstHandle)
    {
        pstDCIHandle = (ODBC_DCI_HANDLE_T *)&pstHandle->stDCIHandle;

        pstDCIHandle->ulExecNum = ulExecNum;
        pstDCIHandle->ulBindSkipLen = ulSkipLen;
    }
}

/*=====================================================================
��    ��: Exec
��    ��:
�������:
�������: ��
ȫ�ֱ���: ��
���ؽ��: PDB_SUCCESS | PDB_ERROR
======================================================================*/
PDBRET PDBODBCConn::Exec(CHAR* szTransName, CHAR* szSQL)
{
    PDBRET          enRet;
    INT32           iRowCount;
    ODBC_DCI_HANDLE_T   *pstDCIHandle;
    ODBC_PDB_HANDLE_T   *pstHandle = NULL;

    pstHandle = (ODBC_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    pstDCIHandle = (ODBC_DCI_HANDLE_T *)&pstHandle->stDCIHandle;

    if (NULL == szSQL || szSQL[0] == '\0')
    {
        PDBLog(LOG_ERROR, "PDBODBCConn::Exec sql is null ");
//      MYSQL_STMT_CLOSE(pstDCIHandle->DBHandles.StmtHandle);
        return PDB_ERROR_SET_ATTR;
    }

    if (szTransName == NULL || szTransName[0] == '\0')
    {
        /* �����Զ��ύ */
        enRet = ODBCSetAutoCommit(pstDCIHandle, TRUE);
        if (enRet != PDB_SUCCESS)
        {
            PDBLog(LOG_ERROR, "PDBODBCConn::Exec set auto commit(true) fail ");
        //  MYSQL_STMT_CLOSE(pstDCIHandle->DBHandles.StmtHandle);
            return PDB_ERROR_SET_ATTR;
        }
        pstDCIHandle->acTransName[0] = '\0';
    }
    else
    {
        /* ����Ϊ���� */
        enRet = ODBCSetAutoCommit(pstDCIHandle, FALSE);
        if (enRet != PDB_SUCCESS)
        {
            PDBLog(LOG_ERROR, "PDBODBCConn::Exec set auto commit(false) fail ");
        //  MYSQL_STMT_CLOSE(pstDCIHandle->DBHandles.StmtHandle);
            return PDB_ERROR_SET_ATTR;
        }
        strcpy((CHAR *)pstDCIHandle->acTransName, szTransName);
    }

    strncpy(pstDCIHandle->acSQLCmd, szSQL, PDB_MAX_SQL_LEN);
    pstDCIHandle->acSQLCmd[PDB_MAX_SQL_LEN] = '\0';

    pstDCIHandle->ulRowCount = 0;

    /* ��ʼ��stmt��prepare sql */
    enRet = ODBCSQLPrepare(pstDCIHandle);
    PDB_CHECK_ERR(enRet);

    /* Exec SQL Command */
    enRet = ODBCExec(pstDCIHandle);
    iRowCount = ODBCRowCount(pstDCIHandle);

    if (iRowCount > 0)
    {
        pstDCIHandle->ulRowCount = iRowCount;
    }
    else
    {
        pstDCIHandle->ulRowCount = 0;
    }

    PDB_CHECK_ERR(enRet);

    /*
    if (PDB_SUCCESS == enRet)
    {
        PDBLog(LOG_INFO, "PDBODBCConn::Exec success ");
    }
    */

    switch (enRet)
    {
        case PDB_SUCCESS:
        case PDB_SUCCESS_WITH_INFO:
        case PDB_NO_DATA:
            return enRet;

        default:
            return PDB_ERROR;
    }
}

/**********************************************************************
��    ��: PDBRET SetPrefetchSize
��    ��: ����������ѯʱԤ��ȡ��¼�����ڴ��С��
�������: ulPreFetchNum    IN  Ԥ��ȡ��¼��
ulPreFetchMem    IN  Ԥ��ȡ��¼��С(0��ʾ������)
ȫ�ֱ���:
���ؽ��:
************************************************************************/
PDBRET PDBODBCConn::SetPrefetchSize(UINT32 ulPreFetchNum, UINT32 ulPreFetchMem)
{
    ODBC_PDB_HANDLE_T *pstHandle = NULL;

    pstHandle = (ODBC_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);

    pstHandle->stDCIHandle.stOption.ulPrefetchRow = ulPreFetchNum;
    pstHandle->stDCIHandle.stOption.ulPrefetchMem = ulPreFetchMem;

    return PDB_SUCCESS;
}

/*=====================================================================
 ��    ��: TransBegin
 ��    ��: ��ʼһ������
 �������: szTransName  --- ������
 �������:
 ȫ�ֱ���: ��
 ���ؽ��:
======================================================================*/
PDBRET PDBODBCConn::TransBegin(CHAR * szTransName)
{
    PDBRET          enRet = PDB_ERROR;
    ODBC_PDB_HANDLE_T   *pstHandle = NULL;

    pstHandle = (ODBC_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_HANDLE_CHECK(pstHandle);

    enRet = ODBCTransBegin(&pstHandle->stDCIHandle, szTransName);

    PDB_CHECK_ERR(enRet);

    return PDB_SUCCESS;
}

/*=====================================================================
��    ��: TransCommit
��    ��: �ύ���ݿ�
�������: szTransName    --- ������
�������:
ȫ�ֱ���: ��
���ؽ��:
======================================================================*/
PDBRET PDBODBCConn::TransCommit(CHAR * szTransName)
{
    PDBRET  enRet = PDB_ERROR;
    ODBC_PDB_HANDLE_T   *pstHandle = NULL;

    pstHandle = (ODBC_PDB_HANDLE_T*)m_pPDBHandle;

    PDB_CONN_HANDLE_CHECK(pstHandle);

    enRet = ODBCTransCommit(&pstHandle->stDCIHandle, szTransName);

    PDB_CHECK_ERR(enRet);

    return PDB_SUCCESS;
}

/*=====================================================================
��    ��: TransRollback
��    ��: �ع����ݿ�
�������: szTransName   --- ������
�������:
ȫ�ֱ���: ��
���ؽ��:
======================================================================*/
PDBRET PDBODBCConn::TransRollback(CHAR * szTransName)
{
    PDBRET  enRet = PDB_ERROR;
    ODBC_PDB_HANDLE_T   *pstHandle = NULL;

    pstHandle = (ODBC_PDB_HANDLE_T*)m_pPDBHandle;

    PDB_CONN_HANDLE_CHECK(pstHandle);

    enRet = ODBCTransRollback(&pstHandle->stDCIHandle, szTransName);

    PDB_CHECK_ERR(enRet);

    return PDB_SUCCESS;
}

/*====================================================================
������      : PDBGetErrGrade
����        :
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��:
����ֵ˵��  :
====================================================================*/
BYTE PDBODBCConn::GetErrGrade(PDBRET enErrCode)
{
    if (enErrCode == PDB_SUCCESS            ) return PDB_NOT_ERR;
    if (enErrCode == PDB_SUCCESS_WITH_INFO  ) return PDB_NOR_ERR;
    if (enErrCode == PDB_ERROR              ) return PDB_NOR_ERR;
    if (enErrCode == PDB_STILL_EXECUTING    ) return PDB_IMP_ERR;
    if (enErrCode == PDB_INVALID_HANDLE     ) return PDB_SYS_ERR;
    if (enErrCode == PDB_NO_DATA            ) return PDB_NOR_ERR;
    if (enErrCode == PDB_NEED_DATA          ) return PDB_IMP_ERR;
    if (enErrCode == PDB_CONTINUE           ) return PDB_NOR_ERR;

    return PDB_NOR_ERR;
}

/*====================================================================
������      : GetTransName
����        :
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��:
����ֵ˵��  :
====================================================================*/
CHAR* PDBODBCConn::GetTransName()
{
    if (NULL == m_pPDBHandle)
    {
        return NULL;
    }

    return ((ODBC_PDB_HANDLE_T*)m_pPDBHandle)->stDCIHandle.acTransName;
}

/*====================================================================
������      : GetRowCount
����        :
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��:
����ֵ˵��  :
====================================================================*/
INT32 PDBODBCConn::GetRowCount()
{
    if (NULL == m_pPDBHandle)
    {
        return INVALID_ROWCOUNT;
    }

    return ((ODBC_PDB_HANDLE_T*)m_pPDBHandle)->stDCIHandle.ulRowCount;
}

/*====================================================================
������      : SetConnectStatus
����        : ��������״̬
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��:
����ֵ˵��  :
====================================================================*/
BOOL PDBODBCConn::SetConnectStatus(BOOL bIsConnected)
{
    if (NULL == m_pPDBHandle)
    {
        return FALSE;
    }

    if (!DCILock(m_hConnectLockId, PDB_WAIT_FOREVER))
    {
        PDBLog(LOG_ERROR, "PDBODBCConn::SetConnectStatus lock fail ");
        return FALSE;
    }

    if (bIsConnected && ((ODBC_PDB_HANDLE_T*)m_pPDBHandle)->bIsConnected)
    {
        DCIUnlock(m_hConnectLockId);
        return FALSE;
    }

    ((ODBC_PDB_HANDLE_T*)m_pPDBHandle)->bIsConnected = bIsConnected;

    DCIUnlock(m_hConnectLockId);

    return TRUE;
}

/*====================================================================
������      : IsConnected
����        �Ƿ���������
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��:
����ֵ˵��  :
====================================================================*/
BOOL PDBODBCConn::IsConnected()
{
    BOOL        bIsConnected;

    if (NULL == m_pPDBHandle)
    {
        return FALSE;
    }

    if (!DCILock(m_hConnectLockId, PDB_WAIT_FOREVER))
    {
        return FALSE;
    }

    bIsConnected = ((ODBC_PDB_HANDLE_T*)m_pPDBHandle)->bIsConnected;

    DCIUnlock(m_hConnectLockId);

    return bIsConnected;
}

/*====================================================================
������      : IsAlloced
����        �����Ƿ��ѷ���
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��:
����ֵ˵��  :
====================================================================*/
BOOL PDBODBCConn::IsAlloced()
{
    BOOL        bIsAlloced;

    if (NULL == m_pPDBHandle)
    {
        return FALSE;
    }

    if (!DCILock(m_hAllocLockId, m_ulLockTimeout))
    {
        return FALSE;
    }

    bIsAlloced = ((ODBC_PDB_HANDLE_T*)m_pPDBHandle)->bIsAlloced;

    DCIUnlock(m_hAllocLockId);

    return bIsAlloced;
}

/*====================================================================
������      : SetAllocStatus
����        : �������ӵķ���״̬
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��:
����ֵ˵��  :
====================================================================*/
BOOL PDBODBCConn::SetAllocStatus(BOOL bIsAlloced)
{
//    PDBLog(LOG_INFO, "PDBODBCConn::SetAllocStatus ");

    if (NULL == m_pPDBHandle)
    {
        return FALSE;
    }

    if (!DCILock(m_hAllocLockId, m_ulLockTimeout))
    {
        PDBLog(LOG_ERROR, "PDBODBCConn::SetAllocStatus lock fail ");
        return FALSE;
    }

    if (bIsAlloced && ((ODBC_PDB_HANDLE_T*)m_pPDBHandle)->bIsAlloced)
    {
        DCIUnlock(m_hAllocLockId);
        return FALSE;
    }

/*
    if (!DCILock(m_hAllocLockId, m_ulLockTimeout))
    {
        PDBLog(LOG_ERROR, "PDBODBCConn::SetAllocStatus lock fail ");
        return FALSE;
    }

    PDBLog(LOG_ERROR, "PDBODBCConn::SetAllocStatus lock[%u] success ", GetConnId());
*/

    ((ODBC_PDB_HANDLE_T*)m_pPDBHandle)->bIsAlloced = bIsAlloced;

    DCIUnlock(m_hAllocLockId);
/*
    DCIUnlock(m_hAllocLockId);

    PDBLog(LOG_ERROR, "PDBODBCConn::SetAllocStatus unlock[%u] success ", GetConnId());
*/
    return TRUE;
}

/*====================================================================
������      : SetConnInUseStatus
����        : �������ӵ�ʹ��״̬
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��:
����ֵ˵��  :
====================================================================*/
BOOL PDBODBCConn::SetConnInUseStatus(BOOL bIsInUse)
{
//    PDBLog(LOG_INFO, "PDBODBCConn::SetConnInUseStatus ");

    if (NULL == m_pPDBHandle)
    {
        return FALSE;
    }

    if (!DCILock(m_hConnLockId, m_ulLockTimeout))
    {
        PDBLog(LOG_ERROR, "PDBODBCConn::SetConnInUseStatus lock fail");
        return FALSE;
    }

    if (bIsInUse && ((ODBC_PDB_HANDLE_T*)m_pPDBHandle)->bIsInUse)
    {
        DCIUnlock(m_hConnLockId);
        return FALSE;
    }

    ((ODBC_PDB_HANDLE_T*)m_pPDBHandle)->bIsInUse = bIsInUse;

    DCIUnlock(m_hConnLockId);

   // PDBLog(LOG_INFO, "PDBODBCConn::SetConnInUseStatus unlock[%u] success", GetConnId());

    return TRUE;
}

/*====================================================================
������      : IsConnInUse
����        �����Ƿ�ʹ����
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��:
����ֵ˵��  :
====================================================================*/
BOOL PDBODBCConn::IsConnInUse(VOID)
{
    BOOL        bIsInUse;

    if (NULL == m_pPDBHandle)
    {
        return TRUE;
    }

    if (!DCILock(m_hConnLockId, m_ulLockTimeout))
    {
        return TRUE;
    }

    bIsInUse = ((ODBC_PDB_HANDLE_T*)m_pPDBHandle)->bIsInUse;

    DCIUnlock(m_hConnLockId);

    return bIsInUse;
}

/*====================================================================
������      : GetConnId
����        : ��ȡ�����ӵ�ID
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��:
����ֵ˵��  :
====================================================================*/
INT32 PDBODBCConn::GetConnId(VOID)
{
    if (NULL == m_pPDBHandle)
    {
        return INVALID_INDEX;
    }

    return ((ODBC_PDB_HANDLE_T*)m_pPDBHandle)->iConnId;
}

/*====================================================================
������      : SetConnId
����        : ���ñ����ӵ�ID
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��:
����ֵ˵��  :
====================================================================*/
VOID PDBODBCConn::SetConnId(INT32 iConnId)
{
    if (NULL == m_pPDBHandle)
    {
        return;
    }

    ((ODBC_PDB_HANDLE_T*)m_pPDBHandle)->iConnId = iConnId;
}

/**********************************************************************
��    ��: CheckErr
��    ��: ��������
�������:
ȫ�ֱ���:
���ؽ��:
************************************************************************/
PDBRET PDBODBCConn::CheckErr(PDBRET enErrCode)
{
    PDBRET          enRet;
    ODBC_DCI_HANDLE_T   *pstDCIHandle;
    ODBC_PDB_HANDLE_T   *pstHandle = NULL;

    pstHandle = (ODBC_PDB_HANDLE_T*)m_pPDBHandle;
    if (NULL == pstHandle)
    {
        return PDB_ERROR;
    }

    pstDCIHandle = (ODBC_DCI_HANDLE_T*)&pstHandle->stDCIHandle;

    switch (enErrCode)
    {
        case PDB_SUCCESS:
        case PDB_SUCCESS_WITH_INFO:
            pstDCIHandle->acErrInfo[0] = '\0';
            enRet = PDB_SUCCESS;
            pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_NULL;
            break;

        case PDB_NO_DATA:
            sprintf(pstDCIHandle->acErrInfo, "Error - PDB_NO_DATA");
            enRet = PDB_NO_DATA;
            pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_LOW;
            break;

        case PDB_NEED_DATA:
            sprintf(pstDCIHandle->acErrInfo, "Error - PDB_NEED_DATA");
            enRet = PDB_NEED_DATA;
            pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_LOW;
            break;

        case PDB_INVALID_HANDLE:
            sprintf(pstDCIHandle->acErrInfo, "Error - PDB_INVALID_HANDLE");
            enRet = PDB_INVALID_HANDLE;
            pstDCIHandle->ulErrLevel = PDB_SYS_ERR;
            break;

        case PDB_STILL_EXECUTING:
            sprintf(pstDCIHandle->acErrInfo, "Error - PDB_STILL_EXECUTE");
            enRet = PDB_STILL_EXECUTING;
            pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_LOW;
            break;

        case PDB_CONTINUE:
            sprintf(pstDCIHandle->acErrInfo, "Error - PDB_CONTINUE");
            enRet = PDB_CONTINUE;
            pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_LOW;
            break;

        case PDB_ERROR_BIND:
            sprintf(pstDCIHandle->acErrInfo, "Error - PDB_ERROR_BIND");
            pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_LOW;
            enRet = PDB_ERROR_BIND;
            break;

        default:
            ODBCGetError(pstDCIHandle);
            pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_LOW;
            enRet = PDB_ERROR;
            break;
    }

    if (enRet != PDB_SUCCESS && enRet != PDB_NO_DATA)
    {
        if (pstDCIHandle->acErrInfo[0])
        {
            PDBLog(LOG_ERROR, pstDCIHandle->acErrInfo);
        }
    }

    return enRet;
}

/**********************************************************************
��    ��: CheckErrCode
��    ��: ��ȡ����ԭ����
�������:
ȫ�ֱ���:
���ؽ��:
************************************************************************/
PDBRET PDBODBCConn::CheckErrCode(PDBRET enErrCode)
{
    switch (enErrCode)
    {
        case PDB_NEED_DATA:
        case PDB_ERROR:
        case PDB_INVALID_HANDLE:
        case PDB_STILL_EXECUTING:
        case PDB_CONTINUE:
        case PDB_ERROR_BIND:
            return enErrCode;

        default:
            return PDB_ERROR;
    }
}

/**********************************************************************
��    ��: GetErrInfo
��    ��: ��ȡ����ԭ��
�������:
ȫ�ֱ���:
���ؽ��:
************************************************************************/
CHAR* PDBODBCConn::GetErrInfo(CHAR* szErrInfo)
{
    ODBC_DCI_HANDLE_T   *pstDCIHandle;
    ODBC_PDB_HANDLE_T   *pstHandle = NULL;

    if (szErrInfo)
    {
        *szErrInfo = '\0';
    }

    pstHandle = (ODBC_PDB_HANDLE_T*)m_pPDBHandle;
    if (NULL == pstHandle)
    {
        return "";
    }

    pstDCIHandle = (ODBC_DCI_HANDLE_T*)&pstHandle->stDCIHandle;
    if (NULL == pstDCIHandle)
    {
        return "";
    }

    if (szErrInfo)
    {
        strcpy(szErrInfo, pstDCIHandle->acErrInfo);
    }

    return pstDCIHandle->acErrInfo;
}

/**********************************************************************
��    ��: GetErrCode
��    ��: ��ȡԭʼ������
�������:
ȫ�ֱ���:
���ؽ��:
************************************************************************/
INT32 PDBODBCConn::GetErrCode()
{
    ODBC_DCI_HANDLE_T   *pstDCIHandle;
    ODBC_PDB_HANDLE_T   *pstHandle = NULL;

    pstHandle = (ODBC_PDB_HANDLE_T*)m_pPDBHandle;
    if (NULL == pstHandle)
    {
        return -1;
    }

    pstDCIHandle = (ODBC_DCI_HANDLE_T*)&pstHandle->stDCIHandle;
    if (NULL == pstDCIHandle)
    {
        return -1;
    }

    return pstDCIHandle->iErrCode;
}

HANDLE PDBODBCConn::GetPDBHandle(VOID)
{
    return m_pPDBHandle;
}
