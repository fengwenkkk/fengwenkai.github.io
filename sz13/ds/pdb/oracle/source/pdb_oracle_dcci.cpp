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
#include "oci.h"

#include "pdb_def.h"
#include "pdb_util.h"
#include "pdb_error.h"
#include "pdb_common.h"
#include "pdb_dcci.h"
#include "pdb_oracle_core.h"

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
PDBRET PDBOracleEnv::CreateConn()
{
    if (m_ulConnNum >= PDB_MAX_CONNECT_NUM)
    {
        return PDB_ERROR_CREATE_CONN;
    }

    if (!DCILock(m_hLockId, PDB_WAIT_FOREVER))
    {
        return PDB_ERROR_CREATE_CONN;
    }

    m_pConn[m_ulConnNum] = new PDBOracleConn(this);

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

PDBRET PDBOracleEnv::FreeConn(UINT32 ulConnID)
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

        PDBOracleConn   *pConn = (PDBOracleConn*)m_pConn[ulConnID];

        delete pConn;
        m_pConn[ulConnID] = NULL;
    }

    DCIUnlock(m_hLockId);

    return PDB_SUCCESS;
}

/**********************************************************************
 ��    ��: CreateEnv
 ��    ��: �������ݿ⻷��
 ����˵��:
 ȫ�ֱ���:
 ���ؽ��: PDB_SUCCESS or PDB_ERROR
************************************************************************/
PDBRET PDBOracleEnv::CreateEnv(VOID)
{
    PDBRET enRet = PDB_ERROR;

    /*���价�����*/
    enRet = OracleInit(&m_hEnv);
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
PDBRET PDBOracleEnv::FreeEnv(VOID)
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

    OracleExit(m_hEnv);
    m_hEnv = NULL;

    DCIDeleteLock(m_hLockId);
    m_hLockId = INVALID_LOCK;

    return PDB_SUCCESS;
}

/**********************************************************************
 ��    ��: PDBOracleConn
 ��    ��: ���캯��
 ����˵��:
 ȫ�ֱ���:
 ���ؽ��:
************************************************************************/
PDBOracleConn::PDBOracleConn(PDBEnvBase *pEnv)
{
    m_pEnv = pEnv;

    Init();
}

PDBOracleConn::~PDBOracleConn()
{
    GOS_FREE(m_pPDBHandle);

    DCIDeleteLock(m_hConnectLockId);
    DCIDeleteLock(m_hAllocLockId);
    DCIDeleteLock(m_hConnLockId);
}

/**********************************************************************
 ��    ��: Init
 ��    ��: ���ݿ����ӳ�ʼ��
 ����˵��:
 ȫ�ֱ���:
 ���ؽ��:
************************************************************************/
PDBRET PDBOracleConn::Init()
{
    memset(&m_stConnParam, 0, sizeof(m_stConnParam));
    memset(&m_stDBOption, 0, sizeof(m_stDBOption));

    m_bReConnect = FALSE;

    m_hConnectLockId = NULL;
    m_hAllocLockId = NULL;
    m_hConnLockId = NULL;

    m_ulLockTimeout = LOCK_TIMEOUT;

    m_pPDBHandle = (ORACLE_PDB_HANDLE_T *)calloc(1, sizeof(ORACLE_PDB_HANDLE_T));
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
BOOL PDBOracleConn::CheckConn() //PDBUINT32 ulLockId, PDBUINT32 ulLockTimeout
{
    BOOL            bOrgAlloced = FALSE;
    PDBRET          enRet = PDB_ERROR;
    CHAR            *szSql = (CHAR*)"select 1 from dual";
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
//        PDBLog(LOG_INFO, "PDBOracleConn::CheckConn set alloc status {true} begin[%ld] ", GetConnId());
        if (!SetAllocStatus(TRUE))
        {
            PDBLog(LOG_INFO, "PDBOracleConn::CheckConn set alloc status {true} fail[%ld]" , GetConnId());
            return FALSE;
        }
    }

    //DCIUnlock(ulLockId);

    /*
    if (FALSE == bDoCheck)
    {
        PDBLog(LOG_ERROR, "PDBOracleConn::CheckConn connect[%ld] is in use ", GetConnId());
        return TRUE;
    }*/

//    PDBLog(LOG_INFO, "PDBOracleConn::CheckConn begin real check[%ld] ", GetConnId());

    /* ��ʼ�����ݿ������ */
    enRet = InitStmt();

    if (PDB_SUCCESS == enRet)
    {
        enRet = Clear();
    }

    /*if (PDB_SUCCESS == enRet)
    {
        enRet = OpenCursor();
    }*/

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
        PDBLog(LOG_INFO, "PDBOracleConn::CheckConn(%d) failed, error code: %u, error info: %s", GetConnId(), GetErrCode(), acErrInfo);

        if (PDB_SUCCESS != ReConnect())
        {
            GetErrInfo(acErrInfo);
            PDBLog(LOG_INFO, "PDBOracleConn::CheckConn(%d) reconnect failed, error code: %u, error info: %s", GetConnId(), GetErrCode(), acErrInfo);
        }
        //Prn("[fatal] PDBOracleConn::CheckConn connect is invalid reconnect ");
    }
    else
    {
        bRet = TRUE;
    }

    if (!SetAllocStatus(bOrgAlloced) &&
        FALSE == bOrgAlloced)
    {
        PDBLog(LOG_INFO, "PDBOracleConn::CheckConn set alloc status {%u} fail[%ld] ", bOrgAlloced, GetConnId());
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
PDBRET PDBOracleConn::ReConnect()
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
PDBRET PDBOracleConn::Connect(PDB_CONN_PARAM_T *pstConnParam, PDB_OPTION_T *pstDBOption)
{
    ORACLE_DCI_HANDLE_T *pstDCIHandle;
    ORACLE_PDB_HANDLE_T *pstPDBHandle = NULL;

    //Prn("PDBOracleConn::Connect ");

    if (NULL == m_pPDBHandle)
    {
        pstPDBHandle = (ORACLE_PDB_HANDLE_T *)calloc(1, sizeof(ORACLE_PDB_HANDLE_T));
        if(NULL == pstPDBHandle)
        {
            return PDB_ERROR;
        }

        m_pPDBHandle = pstPDBHandle;

    }
    else
    {
        pstPDBHandle = (ORACLE_PDB_HANDLE_T *)m_pPDBHandle;
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

    pstDCIHandle = &pstPDBHandle->stDCIHandle;
    pstDCIHandle->DBHandles.EnvHandle = (OCIEnv*)m_pEnv->GetEnv();
    if (PDB_SUCCESS != OracleConnect(pstDCIHandle, pstConnParam, pstDBOption))
    {
        SetConnectStatus(FALSE);
        //pstPDBHandle->bIsConnected = FALSE;

        PDBLog(LOG_ERROR, pstDCIHandle->acErrInfo);
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
    //OracleSetOption(pstDCIHandle, &stOption);

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
PDBRET PDBOracleConn::Disconnect()
{
    PDBRET              enRet;
    ORACLE_PDB_HANDLE_T *pstHandle = NULL;

    //Prn("PDBOracleConn::Disconnect ");

    pstHandle = (ORACLE_PDB_HANDLE_T*)m_pPDBHandle;

    PDB_CONN_HANDLE_CHECK(pstHandle);
        //PDB_CONN_STMT_HANDLE_CHECK(pstHandle);

    enRet = OracleDisconnect(&pstHandle->stDCIHandle);

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
PDBRET PDBOracleConn::Clear()
{
    ORACLE_PDB_HANDLE_T *pstHandle = NULL;
    ORACLE_DCI_HANDLE_T *pstDCIHandle = NULL;

    //Prn("PDBOracleConn::Clear ");

    pstHandle = (ORACLE_PDB_HANDLE_T*)m_pPDBHandle;
    //PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    PDB_CONN_HANDLE_CHECK(pstHandle);
    pstDCIHandle = &pstHandle->stDCIHandle;

    //pstDCIHandle->stOption.ulPrefetchRow = PDB_PREFETCH_ROW_NUM;
    //pstDCIHandle->stOption.ulPrefetchMem = 0;

    /* Ĭ��Ϊ���β�����ִ��һ�� */
//    pstDCIHandle->stOption.usBatchNum =  0;

    pstDCIHandle->ulExecNum  = 1;
    pstDCIHandle->ulBindSkipLen = 0;
    pstDCIHandle->ulRowCount = 0;

    pstDCIHandle->iErrCode = 0;
    pstDCIHandle->ulErrLevel = PDB_ERROR_LEVEL_NULL;
    pstDCIHandle->acErrInfo[0] = '\0';

    //pstDCIHandle->acTransName[0] = '\0';
    pstDCIHandle->acSQLCmd[0] = '\0';

    pstDCIHandle->ulBindNum = 0;
/*
    pstDCIHandle->ulSendBlobParamNum = 0;*/
    pstDCIHandle->ulBindRsltNum = 0;
    pstDCIHandle->ulFetchedNum = 0;

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
PDBRET PDBOracleConn::InitStmt()
{
    PDBRET              enRet;
    ORACLE_PDB_HANDLE_T *pstHandle = NULL;

    //Prn("PDBOracleConn::InitStmt ");

    pstHandle = (ORACLE_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_HANDLE_CHECK(pstHandle);

    enRet = OracleInitStmt(&pstHandle->stDCIHandle);
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
PDBRET PDBOracleConn::CloseStmt()
{
    PDBRET      enRet;
    ORACLE_PDB_HANDLE_T *pstHandle = NULL;

    //Prn("PDBOracleConn::CloseStmt ");
    pstHandle = (ORACLE_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    enRet = OracleCloseStmt(&pstHandle->stDCIHandle);
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
PDBRET PDBOracleConn::BindByPos(UINT32 ulType, VOID *pData, INT32 iLen, UINT32 ulInoutType, INT32 *piLenInd)
{
    PDBRET          enRet;
    UINT32          ulStrLen;
    UINT32          ulParaNo;
    ORACLE_PDB_HANDLE_T *pstHandle = NULL;

    //Prn("PDBOracleConn::BindByPos ");

    pstHandle = (ORACLE_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);

    ulParaNo = pstHandle->stDCIHandle.ulBindNum;

    if (ulParaNo >= PDB_MAX_BIND_NUM)
    {
        return PDB_ERROR;
    }

    if (ulType == PDBT_STRING)
    {
        ulStrLen = strlen((CHAR*)pData) + 1;

        if (ulStrLen < iLen)
        {
            iLen = ulStrLen;
        }

        //iLen = strlen((CHAR*)pData)+1;
    }

    enRet = OracleBindPara(&pstHandle->stDCIHandle, (UINT16)ulParaNo, ulType, ulInoutType, pData, (UINT32)iLen, piLenInd);

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
PDBRET PDBOracleConn::BindRsltByPos(UINT16    usRsltIndex,
                                  UINT16    usDataType,
                                  VOID      *pBuf,
                                  UINT32    ulBufLen,
                                  BOOL      *pbIsNull,
                                  INT32     *piLenInd)
{
    ORACLE_PDB_HANDLE_T *pdbHandle = NULL;

    pdbHandle = (ORACLE_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pdbHandle);

    pdbHandle->stDCIHandle.astBindRsltInfo[pdbHandle->stDCIHandle.ulBindRsltNum].pvData = pBuf;
    pdbHandle->stDCIHandle.astBindRsltInfo[pdbHandle->stDCIHandle.ulBindRsltNum].ulDataLen = ulBufLen;
    pdbHandle->stDCIHandle.astBindRsltInfo[pdbHandle->stDCIHandle.ulBindRsltNum].usDataType = usDataType;
    pdbHandle->stDCIHandle.astBindRsltInfo[pdbHandle->stDCIHandle.ulBindRsltNum].pbIsNull = pbIsNull;

    pdbHandle->stDCIHandle.apiLenInd[pdbHandle->stDCIHandle.ulBindRsltNum] = piLenInd;

    pdbHandle->stDCIHandle.ulBindRsltNum ++;
    //enRet = OracleBindRsltByPos(&pdbHandle->stDCIHandle, usRsltIndex, usDataType, pBuf, ulBufLen, pbIsNull, piLenInd);
    //PDB_CHECK_ERR(enRet);

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
PDBRET PDBOracleConn::OpenCursor()
{
    PDBRET          enRet = PDB_SUCCESS;
    ORACLE_DCI_HANDLE_T *pstDCIHandle;
    ORACLE_PDB_HANDLE_T *pstHandle = NULL;

    pstHandle = (ORACLE_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    pstDCIHandle = (ORACLE_DCI_HANDLE_T *)&pstHandle->stDCIHandle;

    enRet = OracleSetPrefetchNum(pstDCIHandle, pstDCIHandle->stOption.ulPrefetchRow);

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
PDBRET PDBOracleConn::Query(CHAR *szTransName, CHAR *szSQL)
{
    PDBRET      enRet;
    ORACLE_DCI_HANDLE_T *pstDCIHandle;
    ORACLE_PDB_HANDLE_T *pstHandle = NULL;

    pstHandle = (ORACLE_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    pstDCIHandle = (ORACLE_DCI_HANDLE_T *)&pstHandle->stDCIHandle;

    if (NULL == szSQL || szSQL[0] == '\0')
    {
        PDBLog(LOG_ERROR, "PDBOracleConn::Query sql is null");
//      MYSQL_STMT_CLOSE(pstDCIHandle->DBHandles.StmtHandle);
        return PDB_ERROR_SET_ATTR;
    }

    if (szTransName == NULL || szTransName[0] == '\0')
    {
        /* �����Զ��ύ */
        enRet = OracleSetAutoCommit(pstDCIHandle, TRUE);
        if (enRet != PDB_SUCCESS)
        {
            PDBLog(LOG_ERROR, "PDBOracleConn::Query set auto commit(true) fail ");
        //  MYSQL_STMT_CLOSE(pstDCIHandle->DBHandles.StmtHandle);
            return PDB_ERROR_SET_ATTR;
        }
        pstDCIHandle->acTransName[0] = '\0';
    }
    else
    {
        /* ����Ϊ���� */
        enRet = OracleSetAutoCommit(pstDCIHandle, FALSE);
        if (enRet != PDB_SUCCESS)
        {
            PDBLog(LOG_ERROR, "PDBOracleConn::Query set auto commit(false) fail ");
        //  MYSQL_STMT_CLOSE(pstDCIHandle->DBHandles.StmtHandle);
            return PDB_ERROR_SET_ATTR;
        }
        strcpy((CHAR *)pstDCIHandle->acTransName, szTransName);
    }

    strncpy(pstDCIHandle->acSQLCmd, szSQL, PDB_MAX_SQL_LEN);
    pstDCIHandle->acSQLCmd[PDB_MAX_SQL_LEN] = '\0';

    pstDCIHandle->ulRowCount = 0;

    /* ��ʼ��stmt��prepare sql */
    enRet = OracleSQLPrepare(pstDCIHandle);
    PDB_CHECK_ERR(enRet);

    /* Exec SQL Command */
    //pstPDBHandle->usExecNum = 1;
    enRet = OracleQuery(pstDCIHandle);

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
PDBRET PDBOracleConn::BindRslt()
{
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
PDBRET PDBOracleConn::StoreClientRslt()
{
/*  PDBRET          enRet;
    INT32           ulRowCount;
    ORACLE_DCI_HANDLE_T *pstDCIHandle;
    ORACLE_PDB_HANDLE_T *pstHandle = NULL;

    pstHandle = (ORACLE_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    pstDCIHandle = &pstHandle->stDCIHandle;

    enRet = OracleStoreRslt(pstDCIHandle);

    ulRowCount = 0;// DCIRowCount(pstDCIHandle);
    if (ulRowCount > 0)
    {
        pstDCIHandle->ulRowCount = ulRowCount;
    }
    else
    {
        pstDCIHandle->ulRowCount = 0;
    }

    PDB_CHECK_ERR(enRet); */

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
PDBRET PDBOracleConn::RowFetch()
{
    PDBRET          enRet;
    ORACLE_DCI_HANDLE_T *pstDCIHandle;
    ORACLE_PDB_HANDLE_T *pstHandle = NULL;

    pstHandle = (ORACLE_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    pstDCIHandle = &pstHandle->stDCIHandle;

    enRet = OracleRowFetch(pstDCIHandle);

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
PDBRET PDBOracleConn::FreeRslt()
{
    PDBRET          enRet;
    ORACLE_DCI_HANDLE_T *pstDCIHandle;
    ORACLE_PDB_HANDLE_T *pstHandle = NULL;

    pstHandle = (ORACLE_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    pstDCIHandle = &pstHandle->stDCIHandle;

    enRet = OracleFreeRslt(pstDCIHandle);

    return CheckErr(enRet);
}

VOID PDBOracleConn::SetBatchExecParam(UINT32 ulSkipLen, UINT32 ulExecNum)
{
    ORACLE_DCI_HANDLE_T *pstDCIHandle;
    ORACLE_PDB_HANDLE_T *pstHandle = NULL;

    pstHandle = (ORACLE_PDB_HANDLE_T*)m_pPDBHandle;
    if (pstHandle)
    {
        pstDCIHandle = (ORACLE_DCI_HANDLE_T *)&pstHandle->stDCIHandle;

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
PDBRET PDBOracleConn::Exec(CHAR* szTransName, CHAR* szSQL)
{
    PDBRET          enRet;
    INT32           iRowCount;
    ORACLE_DCI_HANDLE_T *pstDCIHandle;
    ORACLE_PDB_HANDLE_T *pstHandle = NULL;

    pstHandle = (ORACLE_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    pstDCIHandle = (ORACLE_DCI_HANDLE_T *)&pstHandle->stDCIHandle;

    if (NULL == szSQL || szSQL[0] == '\0')
    {
        PDBLog(LOG_ERROR, "PDBOracleConn::Exec sql is null ");
//      MYSQL_STMT_CLOSE(pstDCIHandle->DBHandles.StmtHandle);
        return PDB_ERROR_SET_ATTR;
    }

    if (szTransName == NULL || szTransName[0] == '\0')
    {
        /* �����Զ��ύ */
        enRet = OracleSetAutoCommit(pstDCIHandle, TRUE);
        if (enRet != PDB_SUCCESS)
        {
            PDBLog(LOG_ERROR, "PDBOracleConn::Exec set auto commit(true) fail ");
        //  MYSQL_STMT_CLOSE(pstDCIHandle->DBHandles.StmtHandle);
            return PDB_ERROR_SET_ATTR;
        }
        pstDCIHandle->acTransName[0] = '\0';
    }
    else
    {
        /* ����Ϊ���� */
        enRet = OracleSetAutoCommit(pstDCIHandle, FALSE);
        if (enRet != PDB_SUCCESS)
        {
            PDBLog(LOG_ERROR, "PDBOracleConn::Exec set auto commit(false) fail ");
        //  MYSQL_STMT_CLOSE(pstDCIHandle->DBHandles.StmtHandle);
            return PDB_ERROR_SET_ATTR;
        }
        strcpy((CHAR *)pstDCIHandle->acTransName, szTransName);
    }

    strncpy(pstDCIHandle->acSQLCmd, szSQL, PDB_MAX_SQL_LEN);
    pstDCIHandle->acSQLCmd[PDB_MAX_SQL_LEN] = '\0';

    pstDCIHandle->ulRowCount = 0;

    /* ��ʼ��stmt��prepare sql */
    enRet = OracleSQLPrepare(pstDCIHandle);
    PDB_CHECK_ERR(enRet);

    /* Exec SQL Command */
    enRet = OracleExec(pstDCIHandle);
    iRowCount = OracleRowCount(pstDCIHandle);

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
        PDBLog(LOG_INFO, "PDBOracleConn::Exec success ");
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
PDBRET PDBOracleConn::SetPrefetchSize(UINT32 ulPreFetchNum, UINT32 ulPreFetchMem)
{
    ORACLE_PDB_HANDLE_T *pstHandle = NULL;

    pstHandle = (ORACLE_PDB_HANDLE_T*)m_pPDBHandle;
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
PDBRET PDBOracleConn::TransBegin(CHAR * szTransName)
{
    PDBRET          enRet = PDB_ERROR;
    ORACLE_PDB_HANDLE_T *pstHandle = NULL;

    pstHandle = (ORACLE_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_HANDLE_CHECK(pstHandle);

    enRet = OracleTransBegin(&pstHandle->stDCIHandle, szTransName);

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
PDBRET PDBOracleConn::TransCommit(CHAR * szTransName)
{
    PDBRET  enRet = PDB_ERROR;
    ORACLE_PDB_HANDLE_T *pstHandle = NULL;

    pstHandle = (ORACLE_PDB_HANDLE_T*)m_pPDBHandle;

    PDB_CONN_HANDLE_CHECK(pstHandle);

    enRet = OracleTransCommit(&pstHandle->stDCIHandle, szTransName);

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
PDBRET PDBOracleConn::TransRollback(CHAR * szTransName)
{
    PDBRET  enRet = PDB_ERROR;
    ORACLE_PDB_HANDLE_T *pstHandle = NULL;

    pstHandle = (ORACLE_PDB_HANDLE_T*)m_pPDBHandle;

    PDB_CONN_HANDLE_CHECK(pstHandle);

    enRet = OracleTransRollback(&pstHandle->stDCIHandle, szTransName);

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
BYTE PDBOracleConn::GetErrGrade(PDBRET enErrCode)
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
CHAR* PDBOracleConn::GetTransName()
{
    if (NULL == m_pPDBHandle)
    {
        return NULL;
    }

    return ((ORACLE_PDB_HANDLE_T*)m_pPDBHandle)->stDCIHandle.acTransName;
}

/*====================================================================
������      : GetRowCount
����        :
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��:
����ֵ˵��  :
====================================================================*/
INT32 PDBOracleConn::GetRowCount()
{
    if (NULL == m_pPDBHandle)
    {
        return INVALID_ROWCOUNT;
    }

    return ((ORACLE_PDB_HANDLE_T*)m_pPDBHandle)->stDCIHandle.ulRowCount;
}

/*====================================================================
������      : SetConnectStatus
����        : ��������״̬
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��:
����ֵ˵��  :
====================================================================*/
BOOL PDBOracleConn::SetConnectStatus(BOOL bIsConnected)
{
    if (NULL == m_pPDBHandle)
    {
        return FALSE;
    }

    if (!DCILock(m_hConnectLockId, PDB_WAIT_FOREVER))
    {
        PDBLog(LOG_ERROR, "PDBOracleConn::SetConnectStatus lock fail ");
        return FALSE;
    }

    if (bIsConnected && ((ORACLE_PDB_HANDLE_T*)m_pPDBHandle)->bIsConnected)
    {
        DCIUnlock(m_hConnectLockId);
        return FALSE;
    }

    ((ORACLE_PDB_HANDLE_T*)m_pPDBHandle)->bIsConnected = bIsConnected;

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
BOOL PDBOracleConn::IsConnected()
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

    bIsConnected = ((ORACLE_PDB_HANDLE_T*)m_pPDBHandle)->bIsConnected;

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
BOOL PDBOracleConn::IsAlloced()
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

    bIsAlloced = ((ORACLE_PDB_HANDLE_T*)m_pPDBHandle)->bIsAlloced;

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
BOOL PDBOracleConn::SetAllocStatus(BOOL bIsAlloced)
{
//    PDBLog(LOG_INFO, "PDBOracleConn::SetAllocStatus ");

    if (NULL == m_pPDBHandle)
    {
        return FALSE;
    }

    if (!DCILock(m_hAllocLockId, m_ulLockTimeout))
    {
        PDBLog(LOG_ERROR, "PDBOracleConn::SetAllocStatus lock fail ");
        return FALSE;
    }

    if (bIsAlloced && ((ORACLE_PDB_HANDLE_T*)m_pPDBHandle)->bIsAlloced)
    {
        DCIUnlock(m_hAllocLockId);
        return FALSE;
    }

/*
    if (!DCILock(m_hAllocLockId, m_ulLockTimeout))
    {
        PDBLog(LOG_ERROR, "PDBOracleConn::SetAllocStatus lock fail ");
        return FALSE;
    }

    PDBLog(LOG_ERROR, "PDBOracleConn::SetAllocStatus lock[%u] success ", GetConnId());
*/

    ((ORACLE_PDB_HANDLE_T*)m_pPDBHandle)->bIsAlloced = bIsAlloced;

    DCIUnlock(m_hAllocLockId);
/*
    DCIUnlock(m_hAllocLockId);

    PDBLog(LOG_ERROR, "PDBOracleConn::SetAllocStatus unlock[%u] success ", GetConnId());
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
BOOL PDBOracleConn::SetConnInUseStatus(BOOL bIsInUse)
{
    //PDBLog(LOG_INFO, "PDBOracleConn::SetConnInUseStatus ");

    if (NULL == m_pPDBHandle)
    {
        return FALSE;
    }

    if (!DCILock(m_hConnLockId, m_ulLockTimeout))
    {
        PDBLog(LOG_ERROR, "PDBOracleConn::SetConnInUseStatus lock fail");
        return FALSE;
    }

    if (bIsInUse && ((ORACLE_PDB_HANDLE_T*)m_pPDBHandle)->bIsInUse)
    {
        DCIUnlock(m_hConnLockId);
        return FALSE;
    }

    ((ORACLE_PDB_HANDLE_T*)m_pPDBHandle)->bIsInUse = bIsInUse;

    DCIUnlock(m_hConnLockId);

    //PDBLog(LOG_INFO, "PDBOracleConn::SetConnInUseStatus unlock[%u] success", GetConnId());

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
BOOL PDBOracleConn::IsConnInUse(VOID)
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

    bIsInUse = ((ORACLE_PDB_HANDLE_T*)m_pPDBHandle)->bIsInUse;

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
INT32 PDBOracleConn::GetConnId(VOID)
{
    if (NULL == m_pPDBHandle)
    {
        return INVALID_INDEX;
    }

    return ((ORACLE_PDB_HANDLE_T*)m_pPDBHandle)->iConnId;
}

/*====================================================================
������      : SetConnId
����        : ���ñ����ӵ�ID
�㷨ʵ��    :
����ȫ�ֱ���:
�������˵��:
����ֵ˵��  :
====================================================================*/
VOID PDBOracleConn::SetConnId(INT32 iConnId)
{
    if (NULL == m_pPDBHandle)
    {
        return;
    }

    ((ORACLE_PDB_HANDLE_T*)m_pPDBHandle)->iConnId = iConnId;
}

/**********************************************************************
��    ��: CheckErr
��    ��: ��������
�������:
ȫ�ֱ���:
���ؽ��:
************************************************************************/
PDBRET PDBOracleConn::CheckErr(PDBRET enErrCode)
{
    PDBRET              enRet;
    ORACLE_DCI_HANDLE_T *pstDCIHandle;
    ORACLE_PDB_HANDLE_T *pstHandle = NULL;

    pstHandle = (ORACLE_PDB_HANDLE_T*)m_pPDBHandle;
    if (NULL == pstHandle)
    {
        return PDB_ERROR;
    }

    pstDCIHandle = &pstHandle->stDCIHandle;

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
            OracleGetError(pstDCIHandle);
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
PDBRET PDBOracleConn::CheckErrCode(PDBRET enErrCode)
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
CHAR* PDBOracleConn::GetErrInfo(CHAR* szErrInfo)
{
    ORACLE_DCI_HANDLE_T *pstDCIHandle;
    ORACLE_PDB_HANDLE_T *pstHandle = NULL;

    if (szErrInfo)
    {
        *szErrInfo = '\0';
    }

    pstHandle = (ORACLE_PDB_HANDLE_T*)m_pPDBHandle;
    if (NULL == pstHandle)
    {
        return (CHAR*)"";
    }

    pstDCIHandle = &pstHandle->stDCIHandle;
    if (NULL == pstDCIHandle)
    {
        return (CHAR*)"";
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
INT32 PDBOracleConn::GetErrCode()
{
    ORACLE_DCI_HANDLE_T *pstDCIHandle;
    ORACLE_PDB_HANDLE_T *pstHandle = NULL;

    pstHandle = (ORACLE_PDB_HANDLE_T*)m_pPDBHandle;
    if (NULL == pstHandle)
    {
        return -1;
    }

    pstDCIHandle = &pstHandle->stDCIHandle;
    if (NULL == pstDCIHandle)
    {
        return -1;
    }

    return pstDCIHandle->iErrCode;
}

HANDLE PDBOracleConn::GetPDBHandle(VOID)
{
    return m_pPDBHandle;
}
