/******************************************************************************
ģ����          ��  Database C++ Call Interface (DCCI)
�ļ���          ��  pdb_app.cpp
����ļ�        ��
�ļ�ʵ�ֹ���    ��  ���ݿ��ϲ�Ӧ�÷��ʽӿ�
����            ��
�汾            ��  V0.1
-------------------------------------------------------------------------------
�޸ļ�¼:
��  ��      �汾        �޸���      �޸�����
******************************************************************************/

#include "g_include.h"
#include "pdb_app.h"

/**********************************************************************
    �궨��
************************************************************************/

/* ��λ�ð󶨽�������� */
#define BIND_RSLT_BY_POS(usRsltIndex, pData, FieldType, pFieldLen, pbIsNull, piLenInd) \
if (PDB_SUCCESS != m_pPDBConn->BindRsltByPos(usRsltIndex+1, FieldType, pData, pFieldLen, piLenInd)) \
{ \
    PDBLog(LOG_ERROR, "PDBApp::AnalyseSql bind rslt by pos fail "); \
    { \
        return NULL; \
    } \
};\
usRsltIndex++;

/* ��λ�ð���� */
#define  BIND_IN_BY_POS(pData, FieldType, FieldLen, ulInoutType) \
if (PDB_SUCCESS != m_pPDBConn->BindByPos(FieldType, pData, FieldLen, ulInoutType, NULL)) \
{ \
    PDBLog(LOG_ERROR, "PDBApp::AnalyseSql bind in by pos fail "); \
    { \
        return NULL; \
    } \
}

/* ��select���ĳ��� */
#define BIND_OUT_PARAM(argp, ParamType, PDBType) \
{\
    argp = va_arg(*pvaParaList, ParamType*); \
    if (NULL == argp) \
    { \
        PDBLog(LOG_ERROR, "PDBApp::AnalyseSql [ssql select outpara] %s is null ", #argp); \
        {\
            return NULL; \
        }\
    } \
    ulFieldLen = sizeof(ParamType); \
    BIND_RSLT_BY_POS(usRsltIndex, argp, PDBType, ulFieldLen, &argpaiLenInd[usRsltIndex]);\
}

#define SINGLE_BIND_IN(argp, ParamType, PDBType) \
{\
    argp = va_arg(*pvaParaList, ParamType*); \
    if (NULL == argp) \
    { \
        PDBLog(LOG_ERROR, "PDBApp::AnalyseSql [ssql bindpara] %s is null ", #argp); \
        { \
            return NULL; \
        } \
    } \
    BIND_IN_BY_POS(argp, PDBType, sizeof(ParamType), FALSE);\
}

/* ��������κ� */
#define BATCH_BIND_IN \
{\
    BIND_IN_BY_POS(argpBatchData+argpaulPos[m], ulPdbType, argpaulPSize[m], FALSE); \
    if (m == (argiFieldNum-1)) \
    { \
        m = 0; \
        argpBatchData += ulSkipLen;\
    } \
    else \
    { \
        m++; \
    }\
}

#define BIND_BLOB_OUTPARAM(PDBType) \
{ \
    argpstBlob = va_arg(*pvaParaList, PDB_BLOB_T*); \
    if (NULL == argpstBlob || NULL == argpstBlob->pucData \
        || argpstBlob->ulLen < 0) \
    { \
        PDBLog(LOG_ERROR, "PDBApp::AnalyseSql [select outpara] %s is null or bytesize err ", #PDBType); \
        {\
            return NULL; \
        }\
    } \
    memset(argpstBlob->pucData, 0, argpstBlob->ulLen); \
    BIND_RSLT_BY_POS(usRsltIndex, argpstBlob->pucData, PDBType, &argpstBlob->ulLen, &argpaiLenInd[usRsltIndex]); \
}

/* BLOB�Ͳ����İ� */
#define BIND_BLOB_PARAM(PDBType) \
{\
    if (cParaType == PDB_INPARA_MARK) \
    { \
        argpstBlob = va_arg(*pvaParaList, PDB_BLOB_T*); \
        if (NULL == argpstBlob || NULL == argpstBlob->pucData) \
        { \
            PDBLog(LOG_ERROR, "PDBApp::AnalyseSql [in para] %s is null or bytesize err ", #PDBType); \
            {\
                return NULL; \
            }\
        } \
        j += sprintf(szTmpSQL + j, "'"); \
        for (k = 0; k < argpstBlob->ulLen; k ++) \
        { \
            j += sprintf(szTmpSQL + j, "%02X", (argpstBlob->pucData)[k]); \
            if (j > (PDB_MAX_SQL_LEN-1024))    \
            {\
                return NULL; \
            }\
        } \
        j += sprintf(szTmpSQL + j, "'"); \
    } \
    else if (cParaType == PDB_BINDPARA_MARK) \
    { \
        if (TRUE == bIsBatch) \
        { \
            ulPdbType = PDBType; \
            BIND_BLOB_IN_BY_POS(argpBatchData+argpaulPos[m], ulPdbType, FALSE); \
            if (m == (argiFieldNum-1)) \
            { \
                m = 0; \
                argpBatchData += ulSkipLen;\
            } \
            else \
            { \
                m++; \
            } \
        } \
        else \
        { \
            argpstBlob = va_arg(*pvaParaList, PDB_BLOB_T*); \
            if (NULL == argpstBlob || NULL == argpstBlob->pucData \
                || argpstBlob->ulLen <= 0) \
            { \
                PDBLog(LOG_ERROR, "PDBApp::AnalyseSql [ssql bindin para] %s is null or bytesize err ", #PDBType); \
                {\
                    return NULL; \
                }\
            } \
            BIND_IN_BY_POS(argpstBlob->pucData, PDBType, argpstBlob->ulLen, FALSE); \
        } \
    }\
}

/* BLOB�Ͳ����İ� */
#define SINGLE_BIND_BLOB_PARAM(PDBType) \
{\
    if (cParaType == PDB_INPARA_MARK) \
    { \
        argpstBlob = va_arg(*pvaParaList, PDB_BLOB_T*); \
        if (NULL == argpstBlob || NULL == argpstBlob->pucData) \
        { \
            PDBLog(LOG_ERROR, "PDBApp::AnalyseSql [in para] %s is null or bytesize err ", #PDBType); \
            {\
                return NULL; \
            }\
        } \
        j += sprintf(szTmpSQL + j, "'"); \
        for (k = 0; k < argpstBlob->ulLen; k ++) \
        { \
            j += sprintf(szTmpSQL + j, "%02X", (argpstBlob->pucData)[k]); \
            if (j > (PDB_MAX_SQL_LEN-1024))    \
            {\
                return NULL; \
            }\
        } \
        j += sprintf(szTmpSQL + j, "'"); \
    } \
    else if (cParaType == PDB_BINDPARA_MARK) \
    { \
        argpstBlob = va_arg(*pvaParaList, PDB_BLOB_T*); \
        if (NULL == argpstBlob || NULL == argpstBlob->pucData \
            || argpstBlob->ulLen <= 0) \
        { \
            PDBLog(LOG_ERROR, "PDBApp::AnalyseSql [ssql bindin para] %s is null or bytesize err ", #PDBType); \
            {\
                return NULL; \
            }\
        } \
        BIND_IN_BY_POS(argpstBlob->pucData, PDBType, argpstBlob->ulLen, FALSE); \
    }\
}

BOOL PDBRsltParam::Bind(UINT16 usParamType, VOID *pvParamValue, UINT32 ulParamSize)
{
    PDB_PARAM_T     stParam = {0};

    if (!pvParamValue)
    {
        return FALSE;
    }

    if (usParamType == PDBT_INT8 ||
        usParamType == PDBT_UINT8 )
    {
        ulParamSize = sizeof(INT8);
    }
    else if (usParamType == PDBT_INT16 ||
             usParamType == PDBT_UINT16 )
    {
        ulParamSize = sizeof(INT16);
    }
    else if (usParamType == PDBT_INT32 ||
             usParamType == PDBT_UINT32 )
    {
        ulParamSize = sizeof(INT32);
    }
    else if (usParamType == PDBT_INT64 ||
             usParamType == PDBT_UINT64 )
    {
        ulParamSize = sizeof(INT64);
    }
    else if (usParamType == PDBT_FLOAT)
    {
        ulParamSize = sizeof(FLOAT);
    }
    else if (usParamType == PDBT_DOUBLE)
    {
        ulParamSize = sizeof(DOUBLE);
    }
    else if (usParamType == PDBT_STRING)
    {
        if (0 == ulParamSize)
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }

    stParam.usParamType = usParamType;      /* ������������������ */
    stParam.ulParamSize = ulParamSize;      /* �����ĳ���(�ַ�����BYTE����) */
    stParam.iLenInd = 0;
    stParam.bIsNull = FALSE;
    stParam.pvParamValue = pvParamValue;

    m_vParams.push_back(stParam);

    return TRUE;
}

PDBParam::PDBParam()
{
    Clear();
}

BOOL PDBParam::Bind(UINT16 usParamType, VOID *pvParamValue, UINT32 ulInoutType)
{
    return Bind(usParamType, pvParamValue, 0, ulInoutType);
}

BOOL PDBParam::Bind(UINT16 usParamType, VOID *pvParamValue, UINT32 ulParamSize, UINT32 ulInoutType)
{
    PDB_PARAM_T     stParam = {0};

    if (m_iBindErrIndex >= 0)
    {
        return FALSE;
    }

    m_iBindErrIndex = m_vParams.size();

    if (!pvParamValue)
    {
        return FALSE;
    }

    if (ulInoutType != PDB_PARAM_INPUT &&
        ulInoutType != PDB_PARAM_OUTPUT &&
        ulInoutType != PDB_PARAM_INPUT_OUTPUT)
    {
        return FALSE;
    }

    if (usParamType == PDBT_INT8 ||
        usParamType == PDBT_UINT8 )
    {
        ulParamSize = sizeof(INT8);
    }
    else if (usParamType == PDBT_INT16 ||
             usParamType == PDBT_UINT16 )
    {
        ulParamSize = sizeof(INT16);
    }
    else if (usParamType == PDBT_INT32 ||
             usParamType == PDBT_UINT32 )
    {
        ulParamSize = sizeof(INT32);
    }
    else if (usParamType == PDBT_INT64 ||
             usParamType == PDBT_UINT64 )
    {
        ulParamSize = sizeof(INT64);
    }
    else if (usParamType == PDBT_FLOAT)
    {
        ulParamSize = sizeof(FLOAT);
    }
    else if (usParamType == PDBT_DOUBLE)
    {
        ulParamSize = sizeof(DOUBLE);
    }
    else if (usParamType == PDBT_STRING)
    {
/*        if (0 == ulParamSize)
        {
            ulParamSize = 1;
            pvParamValue = " ";
        //    return FALSE;
        } */
    }
    else
    {
        return FALSE;
    }

    stParam.usParamType = usParamType;      /* ������������������ */
    stParam.ulParamSize = ulParamSize;      /* �����ĳ���(�ַ�����BYTE����) */
    stParam.iLenInd = 0;
    stParam.bIsNull = FALSE;
    stParam.pvParamValue = pvParamValue;
    stParam.ulInoutType = ulInoutType;

    m_vParams.push_back(stParam);

    m_iBindErrIndex = -1;

    return TRUE;
}

INT32 PDBParam::GetLen(UINT32 ulFieldIndex)
{
    if (ulFieldIndex >= m_vParams.size())
    {
        return 0;
    }

    if (m_vParams[ulFieldIndex].iLenInd < 0)
    {
        return 0;
    }

    return m_vParams[ulFieldIndex].iLenInd;
}

VECTOR<PDB_PARAM_T>& PDBParam::GetParams()
{
    if (m_iBindErrIndex >= 0)
    {
        m_vParams.clear();
    }

    return m_vParams;
}

VOID PDBParam::Clear()
{
    m_vParams.clear();
    m_iBindErrIndex = -1;
}


/**********************************************************************
 ��    ��: PDBBatchParam
 ��    ��: ���캯��
 ����˵��:
 ȫ�ֱ���:
 ���ؽ��:
************************************************************************/
PDBBatchParam::PDBBatchParam(UINT32 ulMaxBatchNum, UINT32 ulSkipLen, PDB_PARAM_DESC_T *pstParamDesc, UINT32 ulParamNum)
{
    UINT32  ulRecordLen = 0;
    UINT32  i;

    m_ulMaxBatchNum = ulMaxBatchNum;
    m_ulSkipLen = ulSkipLen;
    m_pucData = NULL;

    if (ulMaxBatchNum == 0 || ulParamNum == 0 || !pstParamDesc)
    {
        goto fail;
    }

    for (i=0; i<ulParamNum; i++)
    {
        m_vParams.push_back(pstParamDesc[i]);
        ulRecordLen = pstParamDesc[i].ulParamOffset + pstParamDesc[i].ulParamSize;

        if (ulRecordLen > ulSkipLen)
        {
            goto fail;
        }
    }

    m_pucData = (UINT8*)calloc(ulSkipLen, ulMaxBatchNum);
    if (!m_pucData)
    {
        goto fail;
    }

    m_ulCurrNum = 0;
    m_ulStartIndex = 0;

    return;

fail:
    m_vParams.clear();
}

PDBBatchParam::~PDBBatchParam()
{
    if (m_pucData)
    {
        free(m_pucData);
    }
}

BOOL PDBBatchParam::AddData(VOID *pData, BOOL bReplaceFirst)
{
    if (0 == m_ulMaxBatchNum)
    {
        return FALSE;
    }

    if (m_ulCurrNum == m_ulMaxBatchNum)
    {
        if (!bReplaceFirst)
        {
            return FALSE;
        }

        m_ulStartIndex = MOD(m_ulStartIndex+1, m_ulMaxBatchNum);
        m_ulCurrNum--;
    }

    UINT32  ulAppendIndex = MOD(m_ulStartIndex + m_ulCurrNum, m_ulMaxBatchNum);
    UINT8   *pucAppendData = m_pucData + m_ulSkipLen*ulAppendIndex;

    memcpy(pucAppendData, pData, m_ulSkipLen);
    m_ulCurrNum++;

    return TRUE;
}

// ��ȡ���������ݼ�¼��
UINT32 PDBBatchParam::GetBatchDataNum(UINT32 ulMaxNum)
{
    if (0 == m_ulMaxBatchNum)
    {
        return 0;
    }

    UINT32      ulEndIndex = MOD(m_ulStartIndex + m_ulCurrNum, m_ulMaxBatchNum);
    UINT32      ulNum = 0;

    if (ulEndIndex < m_ulStartIndex)
    {
        ulNum = m_ulMaxBatchNum - m_ulStartIndex;
    }
    else
    {
        ulNum = m_ulCurrNum;
    }

    if (ulNum > ulMaxNum)
    {
        ulNum = ulMaxNum;
    }

    return ulNum;
}

VOID* PDBBatchParam::GetBatchData()
{
    UINT8   *pucData = m_pucData + m_ulSkipLen*m_ulStartIndex;

    return (VOID*)pucData;
}

VOID PDBBatchParam::RemoveBatchData(UINT32 ulDataNum)
{
    if (ulDataNum >= m_ulCurrNum)
    {
        Clear();
    }
    else
    {
        if (0 == m_ulMaxBatchNum)
        {
            m_ulStartIndex = 0;
            m_ulCurrNum = 0;

            return;
        }

        m_ulStartIndex = MOD(m_ulStartIndex+ulDataNum, m_ulMaxBatchNum);
        m_ulCurrNum -= ulDataNum;
    }
}

BOOL PDBBatchParam::IsFull()
{
    return m_ulCurrNum == m_ulMaxBatchNum;
}

VOID PDBBatchParam::Clear()
{
    m_ulCurrNum = 0;
    m_ulStartIndex = 0;
}

/**********************************************************************
 ��    ��: PDBApp
 ��    ��: ���캯��
 ����˵��:
 ȫ�ֱ���:
 ���ؽ��:
************************************************************************/
PDBApp::PDBApp(PDBEnvBase *pEnv)
{
    m_pPDBEnv = pEnv;
    Init();
}

/**********************************************************************
 ��    ��: ~PDBApp
 ��    ��: ��������
 ����˵��:
 ȫ�ֱ���:
 ���ؽ��:
************************************************************************/
PDBApp::~PDBApp()
{
    if (m_pPDBConn)
    {
        m_pPDBConn = NULL;
    }
}

/**********************************************************************
 ��    ��: CheckConnStat
 ��    ��: ������ݿ�����״̬�Ƿ�����
 ����˵��:
 ȫ�ֱ���:
 ���ؽ��:
***********************************************************************/
BOOL PDBApp::CheckConnStat(VOID)
{
    if (NULL == m_pPDBConn)
    {
        return TRUE;
    }

    if (m_pPDBConn->IsConnected() &&
        !m_pPDBConn->IsConnInUse())
    {
        /* ÿ�μ��һ�������δʹ�õ� */
        //PDBLog(LOG_DETAIL, "PDBApp::CheckConnStat begin check[%ld] ", m_ulConnId);

        if (m_pPDBConn->SetConnInUseStatus(TRUE))
        {
            //PDBLog(LOG_DETAIL, "PDBApp::CheckConnStat set in use status true and check[%ld] ", m_ulConnId);

            /*�����ɹ��ٽ������Ӽ��*/
            if (!m_pPDBConn->CheckConn())
            {
                PDBLog(LOG_ERROR, "check conn %u failed", m_ulConnId);
            }

            m_pPDBConn->SetConnInUseStatus(FALSE);

          //  PDBLog(LOG_DETAIL, "PDBApp::CheckConnStat set in use status false[%ld] ", m_ulConnId);
        }

        //PDBLog(LOG_DETAIL, "PDBApp::CheckConnStat end check[%ld] ", m_ulConnId);
    }

    if (!m_pPDBConn->IsConnected())
    {
        if (m_pPDBConn->SetConnInUseStatus(TRUE))
        {
            /* MYSQL�������Ϲ� */
           /* PDB_HANDLE_T* pstHandle = (PDB_HANDLE_T*)m_pPDBConn->GetPDBHandle();

            if (!pstHandle)
            {
                PDBLog(LOG_INFO, "PDBApp::CheckConnStat connect[%ld] pdb handle fail ", m_ulConnId);
            }*/

            if (PDB_SUCCESS != m_pPDBConn->ReConnect())
            {
                PDBLog(LOG_ERROR, "conn %u reconnect failed", m_ulConnId);
            }

            m_pPDBConn->SetConnInUseStatus(FALSE);
        }
    }

    return TRUE;
}

/**********************************************************************
 ��    ��: Init
 ��    ��: ��ʼ��
 ����˵��:
 ȫ�ֱ���:
 ���ؽ��: TRUE or FALSE
************************************************************************/
BOOL PDBApp::Init(VOID)
{
    m_pPDBConn = NULL;

    m_ulConnId = INVALID_INDEX;

    m_acTransName[0] = '\0';

    return TRUE;
}

/**********************************************************************
 ��    ��: AllocConn
 ��    ��: ����һ�����õ�����
 ����˵��:  pEnv -- ��ص����ݿ⻷��
 ȫ�ֱ���:

 ���ؽ��: TRUE or FALSE
************************************************************************/
BOOL PDBApp::AllocConn() //, INT32 *piConnId
{
    PDBRET      eRet;
    INT32       *piConnId;

    piConnId = &m_ulConnId;

    if (m_pPDBConn)
    {
        UINT32  ulDBType = m_pPDBEnv->GetDBType();

        if (ulDBType == PDBT_ORACLE)
        {
#ifdef HAVE_ORACLE
            PDBOracleConn   *pConn = (PDBOracleConn*)m_pPDBConn;

            delete pConn;
#endif
        }
        else if (ulDBType == PDBT_MYSQL)
        {
#ifdef HAVE_MYSQL
            PDBMySQLConn    *pConn = (PDBMySQLConn*)m_pPDBConn;

            delete pConn;
#endif
        }
        else if (ulDBType == PDBT_NOSQL)
        {
            PDBNoSQLConn    *pConn = (PDBNoSQLConn*)m_pPDBConn;

            delete pConn;
        }
        else if (ulDBType & PDBT_ODBC)
        {
#ifdef WIN32
#ifdef HAVE_ODBC
            PDBODBCConn     *pConn = (PDBODBCConn*)m_pPDBConn;

            delete pConn;
#endif
#endif
        }
        else
        {
            return FALSE;
        }

        m_pPDBConn = NULL;
    }

    if (NULL == m_pPDBEnv)
    {
        PDBLog(LOG_ERROR, "PDBApp::AllocConn pEnv is null ");
        return FALSE;
    }

    /* ��ȡһ��������ݿ����� */
    eRet = m_pPDBEnv->AllocConn((VOID**)&m_pPDBConn, piConnId);
    if (PDB_SUCCESS != eRet)
    {
        PDBLog(LOG_ERROR, "PDBApp::AllocConn(%d) fail ", *piConnId);
        return FALSE;
    }

    if (NULL == m_pPDBConn)
    {
        PDBLog(LOG_ERROR, "PDBApp::AllocConn(%d) is null ", *piConnId);
        return FALSE;
    }

    //PDBLog(LOG_INFO, "PDBApp::AllocConn(%d) alloc state[%ld] ", *piConnId, m_pPDBConn->IsAlloced());

    return TRUE;
}

/**********************************************************************
 ��    ��: FreeConn
 ��    ��: ����һ�����õ�����
 ����˵��:  ppConn -- ����������ݿ�����
 ȫ�ֱ���:

 ���ؽ��: TRUE or FALSE
************************************************************************/
BOOL PDBApp::FreeConn()
{
    BOOL bRet = FALSE;

    if (NULL == m_pPDBEnv)
    {
        PDBLog(LOG_ERROR, "PDBApp::FreeConn fail pEnv is null ");
        return FALSE;
    }

    if (NULL == m_pPDBConn)
    {
        PDBLog(LOG_ERROR, "PDBApp::FreeConn fail m_pPDBConn is null ");
        return FALSE;
    }

    m_acTransName[0] = '\0';

    bRet = m_pPDBEnv->FreeConn(m_pPDBConn->GetConnId());
    //PDBLog(LOG_INFO, "PDBApp::FreeConn setDisAlloc[%ld] ", m_pPDBConn->GetConnId());

    return TRUE;
}

/*=====================================================================
 ��    ��: TransBegin
 ��    ��: ��ʼһ������
 �������: szTransName  --- ������
 �������:
 ȫ�ֱ���: ��
 ���ؽ��:
======================================================================*/
BOOL PDBApp::TransBegin()//CHAR* szTransName)
{
    PDBRET      enRet = PDB_ERROR;
    CHAR        acTransName[32];

    if (!m_pPDBConn)
    {
        return FALSE;
    }

    /* ����ʼ */
    if (m_acTransName[0] != '\0')
    {
        PDBLog(LOG_ERROR, "PDBApp::TransBegin trans has begin ");
        return FALSE;
    }

    sprintf(acTransName, "Trans%u", m_ulConnId);

    if (!m_pPDBConn->SetConnInUseStatus(TRUE))
    {
        PDBLog(LOG_ERROR, "PDBApp::TransBegin [%ld] set conn in use status true fail ", m_pPDBConn->GetConnId());
        return FALSE;
    }

    enRet = m_pPDBConn->TransBegin(acTransName);
    if (PDB_SUCCESS != enRet)
    {
        PDBLog(LOG_ERROR, "PDBApp::TransBegin trans begin error ");

        if (!m_pPDBConn->SetConnInUseStatus(FALSE))
        {
            PDBLog(LOG_ERROR, "PDBApp::TransBegin [%ld] set conn in use status false fail ", m_pPDBConn->GetConnId());
        }

        return FALSE;
    }

    strcpy(m_acTransName, acTransName);

    return TRUE;
}

/*=====================================================================
��    ��: TransCommit
��    ��: �ύ���ݿ�
�������: szTransName    --- ������
�������:
ȫ�ֱ���: ��
���ؽ��:
======================================================================*/
BOOL PDBApp::TransCommit()//CHAR* szTransName)
{
    PDBRET  enRet = PDB_ERROR;
    BOOL    bRet = FALSE;
    CHAR    acTransName[32];

    if (!m_pPDBConn)
    {
        return FALSE;
    }

    if (m_acTransName[0] == '\0')
    {
        PDBLog(LOG_ERROR, "PDBApp::TransCommit trans has not begin ");
        return FALSE;
    }

    sprintf(acTransName, "Trans%u", m_ulConnId);

    if (strcmp(acTransName, m_acTransName) != 0)
    {
        PDBLog(LOG_ERROR, "PDBApp::TransCommit trans %s not exist!", acTransName);
        return FALSE;
    }

    enRet = m_pPDBConn->TransCommit(acTransName);
    if (PDB_SUCCESS != enRet)
    {
        PDBLog(LOG_ERROR, "PDBApp::TransCommit commit error ");
        bRet = FALSE;
        goto end;
    }

    bRet = TRUE;

end:

    if (!m_pPDBConn->SetConnInUseStatus(FALSE))
    {
        PDBLog(LOG_ERROR, "PDBApp::TransCommit [%ld] set conn in use status false fail ", m_pPDBConn->GetConnId());
    }

    m_acTransName[0] = '\0';

    return bRet;
}

/*=====================================================================
��    ��: TransRollback
��    ��: �ع����ݿ�
�������: szTransName   --- ������
�������:
ȫ�ֱ���: ��
���ؽ��:
======================================================================*/
BOOL PDBApp::TransRollback()//CHAR* szTransName)
{
    PDBRET  enRet = PDB_ERROR;
    BOOL    bRet = FALSE;
    CHAR    acTransName[32];

    if (!m_pPDBConn)
    {
        return FALSE;
    }

    if (m_acTransName[0] == '\0')
    {
        PDBLog(LOG_ERROR, "PDBApp::TransRollback trans has not begin ");
        return FALSE;
    }

    sprintf(acTransName, "Trans%u", m_ulConnId);
    if (strcmp(acTransName, m_acTransName) != 0)
    {
        PDBLog(LOG_ERROR, "PDBApp::TransRollback trans %s not exist!", acTransName);
        return FALSE;
    }

    enRet = m_pPDBConn->TransRollback(acTransName);
    if (PDB_SUCCESS != enRet)
    {
        PDBLog(LOG_ERROR, "PDBApp::TransRollback error ");
        bRet = FALSE;
    }

    bRet = TRUE;

    if (!m_pPDBConn->SetConnInUseStatus(FALSE))
    {
        PDBLog(LOG_ERROR, "PDBApp::TransRollback [%ld] set conn in use status false fail ", m_pPDBConn->GetConnId());
    }

    m_acTransName[0] = '\0';

    return bRet;
}

/**********************************************************************
 ��    ��: Exec
 ��    ��: ������sql���ִ��,����ִ���ޱ���ҷ�select�ĵ���sql���
 ����˵��:  szSQL -- ��sql���
 ȫ�ֱ���:

 ���ؽ��: sql���Ӱ������� INVALID_ROWCOUNT(�д���) or ROWCOUNT(>=0)
************************************************************************/
INT32 PDBApp::Exec(const CHAR *szSQL)
{
    PDBRET  enRet = PDB_ERROR;
    BOOL    bTrans = TRUE;

    if (NULL == m_pPDBConn)
    {
        PDBLog(LOG_ERROR, "PDBApp::Exec m_pPDBConn is null ");
        return INVALID_ROWCOUNT;
    }

    if (FALSE == m_pPDBConn->IsConnected())
    {
        PDBLog(LOG_ERROR, "PDBApp::Exec [%ld] is not connected ", m_pPDBConn->GetConnId());
        return INVALID_ROWCOUNT;
    }

    if (FALSE == m_pPDBConn->IsAlloced())
    {
        PDBLog(LOG_ERROR, "PDBApp::Exec [%ld] is not alloced ", m_pPDBConn->GetConnId());
        return INVALID_ROWCOUNT;
    }

    if (m_acTransName[0] == '\0')
    {
        bTrans = FALSE;
    }

    if (!bTrans && !m_pPDBConn->SetConnInUseStatus(TRUE))
    {
        PDBLog(LOG_ERROR, "PDBApp::Exec [%ld] set conn in use status true fail ", m_pPDBConn->GetConnId());
        return INVALID_ROWCOUNT;
    }

    /* ��ʼ�����ݿ������ */
    enRet = m_pPDBConn->InitStmt();
    if (PDB_SUCCESS != enRet)
    {
        PDBLog(LOG_ERROR, "PDBApp::Exec init stmt error ");
        goto end;
    }

    /* ��ʼ�������Ӳ��� */
    enRet = m_pPDBConn->Clear();
    if (PDB_SUCCESS != enRet)
    {
        PDBLog(LOG_ERROR, "PDBApp::Exec clear error ");
        goto end;
    }

    /* ִ��SQL��� */
    enRet = m_pPDBConn->Exec(m_acTransName, (CHAR*)szSQL);
    if (PDB_SUCCESS != enRet)
    {
        PDBLog(LOG_ERROR, "PDBApp::Exec error: %s", m_pPDBConn->GetErrInfo());
        goto end;
    }

end:

    if (!bTrans && !m_pPDBConn->SetConnInUseStatus(FALSE))
    {
        PDBLog(LOG_ERROR, "PDBApp::Exec [%ld] set conn in use status false fail ", m_pPDBConn->GetConnId());
    }

    if (PDB_SUCCESS != enRet)
    {
        return INVALID_ROWCOUNT;
    }

    /* ִ�н���������Ӱ������ */
    if (m_pPDBEnv->GetDBType() == PDBT_NOSQL)
    {
        return 1;
    }

    return m_pPDBConn->GetRowCount();
}

/**********************************************************************
 ��    ��: Update
 ��    ��: ����ִ��insert/modify/delete������sql���
 ����˵��:  szSQL -- ָ����ʽ��sql���
            ...   -- ����ָ��(������������ΪIN)
                     ���������
 ȫ�ֱ���:

 ���ؽ��: sql���Ӱ������� INVALID_ROWCOUNT(�д���) or ROWCOUNT(>=0)
************************************************************************/
INT32 PDBApp::Update(BOOL bAutoCommit, CHAR *szSQL)
{
    static VECTOR<PDB_PARAM_T> vParam;

    return Update(bAutoCommit, szSQL, vParam);
}

INT32 PDBApp::Update(BOOL bAutoCommit, CHAR *szSQL, VECTOR<PDB_PARAM_T> &vParam)
{
    PDBRET      enRet = PDB_ERROR;
    BOOL        bTrans = FALSE;
    CHAR        *szConvertedSQL = NULL;

    if (NULL == m_pPDBConn)
    {
        PDBLog(LOG_ERROR, "PDBApp::Update m_pPDBConn is null ");
        return INVALID_ROWCOUNT;
    }

    if (FALSE == m_pPDBConn->IsConnected())
    {
        PDBLog(LOG_ERROR, "PDBApp::Update [%ld] is not connected ", m_pPDBConn->GetConnId());
        goto end;
    }

    if (FALSE == m_pPDBConn->IsAlloced())
    {
        PDBLog(LOG_ERROR, "PDBApp::Update [%ld] is not alloced ", m_pPDBConn->GetConnId());
        goto end;
    }

    if (bAutoCommit)
    {
        m_acTransName[0] = '\0';
        bTrans = FALSE;
    }
    else if (m_acTransName[0] == '\0')
    {
        bTrans = FALSE;
    }
    else
    {
        bTrans = TRUE;
    }

    if (!bTrans && !m_pPDBConn->SetConnInUseStatus(TRUE))
    {
        PDBLog(LOG_ERROR, "PDBApp::Update [%ld] set conn in use status true fail ", m_pPDBConn->GetConnId());
        goto end;
    }

    /* ��ʼ�����ݿ������ */
    enRet = m_pPDBConn->InitStmt();
    if (PDB_SUCCESS != enRet)
    {
        PDBLog(LOG_ERROR, "PDBApp::Update init stmt error ");
        goto end;
    }

    /*  ��ʼ�������Ӳ��� */
    enRet = m_pPDBConn->Clear();
    if (PDB_SUCCESS != enRet)
    {
        PDBLog(LOG_ERROR, "PDBApp::Update clear error ");
        goto end;
    }

    /* ����sql��䲢�������� */
    szConvertedSQL = BindUpdateParam(szSQL, vParam);
    if (!szConvertedSQL)
    {
        PDBLog(LOG_ERROR, "PDBApp::Update analyse sql error ");
        enRet = PDB_ERROR;
        goto end;
    }

    /* ִ��SQL��� */
    enRet = m_pPDBConn->Exec(m_acTransName, szConvertedSQL);
    //PDB_FREE(szConvertedSQL);
    if (PDB_SUCCESS != enRet)
    {
        PDBLog(LOG_ERROR, "PDBApp::Update error: %s",  m_pPDBConn->GetErrInfo());
        goto end;
    }

end:
    if (!bTrans && !m_pPDBConn->SetConnInUseStatus(FALSE))
    {
        PDBLog(LOG_ERROR, "PDBApp::Update [%ld] set conn in use status false fail ", m_pPDBConn->GetConnId());
    }

    if (PDB_SUCCESS != enRet)
    {
        CheckConnStat();

        return INVALID_ROWCOUNT;
    }

    /* ִ�н���������Ӱ������ */
    if (m_pPDBEnv->GetDBType() == PDBT_NOSQL)
    {
        return 1;
    }

    return m_pPDBConn->GetRowCount();
}

INT32 PDBApp::Update(const CHAR *szSQL)
{
    INT32 iRet = Update(FALSE, (CHAR*)szSQL);

    if (iRet <= 0)
    {
        CheckConnStat();
    }

    return iRet;
}

INT32 PDBApp::Update(const CHAR *szSQL, PDBParam &Param)
{
    INT32 iRet = Update(FALSE, (CHAR*)szSQL, Param.GetParams());

    if (iRet <= 0)
    {
        CheckConnStat();
    }

    return iRet;
}

INT32 PDBApp::UpdateDirect(const CHAR *szSQL)
{
    INT32 iRet = Update(TRUE, (CHAR*)szSQL);

    if (iRet <= 0)
    {
        CheckConnStat();
    }

    return iRet;
}

INT32 PDBApp::UpdateDirect(const CHAR *szSQL, PDBParam &Param)
{
    INT32 iRet = Update(TRUE, (CHAR*)szSQL, Param.GetParams());

    if (iRet <= 0)
    {
        CheckConnStat();
    }

    return iRet;
}

INT32 PDBApp::UpdateBatch(const CHAR *szSQL, PDBBatchParam *Param, UINT32 ulExecNum)
{
    return UpdateBatch(FALSE, (CHAR*)szSQL, Param, ulExecNum);
}

INT32 PDBApp::UpdateBatchDirect(const CHAR *szSQL, PDBBatchParam *Param, UINT32 ulExecNum)
{
    return UpdateBatch(TRUE, (CHAR*)szSQL, Param, ulExecNum);
}

INT32 PDBApp::UpdateBatch(BOOL bAutoCommit, CHAR *szSQL, PDBBatchParam *Param, UINT32 ulExecNum)
{
    PDBRET      enRet = PDB_ERROR;
    BOOL        bTrans;
    CHAR        *szConvertedSQL = NULL;

    ulExecNum = Param->GetBatchDataNum(ulExecNum);
    if (ulExecNum == 0)
    {
        return 0;
    }

    if (NULL == m_pPDBConn)
    {
        PDBLog(LOG_ERROR, "PDBApp::UpdateBatch m_pPDBConn is null ");
        return INVALID_ROWCOUNT;
    }

    if (FALSE == m_pPDBConn->IsConnected())
    {
        PDBLog(LOG_ERROR, "PDBApp::UpdateBatch [%ld] is not connected ", m_pPDBConn->GetConnId());
        return INVALID_ROWCOUNT;
    }

    if (FALSE == m_pPDBConn->IsAlloced())
    {
        PDBLog(LOG_ERROR, "PDBApp::UpdateBatch [%ld] is not alloced ", m_pPDBConn->GetConnId());
        return INVALID_ROWCOUNT;
    }

    if (bAutoCommit)
    {
        m_acTransName[0] = '\0';
        bTrans = FALSE;
    }
    else if (m_acTransName[0] == '\0')
    {
        bTrans = FALSE;
    }
    else
    {
        bTrans = TRUE;
    }

    if (!bTrans && !m_pPDBConn->SetConnInUseStatus(TRUE))
    {
        PDBLog(LOG_ERROR, "PDBApp::UpdateBatch [%ld] set conn in use status true fail ", m_pPDBConn->GetConnId());
        return INVALID_ROWCOUNT;
    }

    /* ��ʼ�����ݿ������ */
    enRet = m_pPDBConn->InitStmt();
    if (PDB_SUCCESS != enRet)
    {
        PDBLog(LOG_ERROR, "PDBApp::UpdateBatch init stmt error ");
        goto end;
    }

    /*  ��ʼ�������Ӳ��� */
    enRet = m_pPDBConn->Clear();
    if (PDB_SUCCESS != enRet)
    {
        PDBLog(LOG_ERROR, "PDBApp::UpdateBatch clear error ");
        goto end;
    }

    /* ����sql��䲢�������� */
    szConvertedSQL = BindUpdateBatchParam(szSQL, Param);
    if (!szConvertedSQL)
    {
        PDBLog(LOG_ERROR, "PDBApp::UpdateBatch analyse sql error ");
        enRet = PDB_ERROR;
        goto end;
    }

    m_pPDBConn->SetBatchExecParam(Param->GetSkipLen(), ulExecNum);

    /* ִ��SQL��� */
    enRet = m_pPDBConn->Exec(m_acTransName, szConvertedSQL);
    if (PDB_SUCCESS != enRet)
    {
        PDBLog(LOG_ERROR, "PDBApp::UpdateBatch error: %s",  m_pPDBConn->GetErrInfo());
        goto end;
    }

end:
    /* �ر�stmt */
    if (!bTrans && !m_pPDBConn->SetConnInUseStatus(FALSE))
    {
        PDBLog(LOG_ERROR, "PDBApp::UpdateBatch [%ld] set conn in use status false fail ", m_pPDBConn->GetConnId());
    }

    if (PDB_SUCCESS != enRet)
    {
        return INVALID_ROWCOUNT;
    }

    /* ִ�н���������Ӱ������ */
    if (m_pPDBEnv->GetDBType() == PDBT_NOSQL)
    {
        return 1;
    }

    return m_pPDBConn->GetRowCount();
}

/**********************************************************************
 ��    ��: Query
 ��    ��: ����ִ��select������sql���,Ĭ��Ϊ��귽ʽ��ѯ
 ����˵��:
 ȫ�ֱ���:  szSQL -- ָ����ʽ��SQL���
            ...   -- ����ָ��(��Ϊsql�������ͣ�������������ΪIN/OUT��
                              ��Ϊsql������ͣ�������������ΪIN)
                     ���������

 ���ؽ��: ��ѯ���Ӱ������� INVALID_ROWCOUNT(�д���) or ROWCOUNT(>=0)
           ����ѯ�޷���ȡ������������0
************************************************************************/
INT32 PDBApp::Query(const CHAR *szSQL, PDBRsltParam &Param)
{
    UINT32  ulQryType = PDB_QRY_USE_CACHE;
    PDBRET  enRet = PDB_ERROR;
    BOOL    bTrans = TRUE;
    INT32   iRowCount = INVALID_ROWCOUNT;
    CHAR    *szConvertedSQL = NULL;

    /* �����Ƿ��ȡ�б�־ */
    m_ulQryType = ulQryType;
    m_bIsFetchReady = FALSE;

    if (NULL == m_pPDBConn)
    {
        PDBLog(LOG_ERROR, "PDBApp::Query m_pPDBConn is null ");
        return INVALID_ROWCOUNT;
    }

    if (FALSE == m_pPDBConn->IsConnected())
    {
        PDBLog(LOG_ERROR, "PDBApp::Query [%ld] is not connected ", m_pPDBConn->GetConnId());
        goto fail;
    }

    if (FALSE == m_pPDBConn->IsAlloced())
    {
        PDBLog(LOG_ERROR, "PDBApp::Query [%ld] is not alloced ", m_pPDBConn->GetConnId());
        goto fail;
    }

    if (m_acTransName[0] == '\0')
    {
        bTrans = FALSE;
    }

    if (!bTrans && !m_pPDBConn->SetConnInUseStatus(TRUE))
    {
        PDBLog(LOG_ERROR, "PDBApp::Query [%ld] set conn in use status true fail ", m_pPDBConn->GetConnId());
        goto fail;
    }

    /* ��ʼ�����ݿ������ */
    enRet = m_pPDBConn->InitStmt();
    if (PDB_SUCCESS != enRet)
    {
        PDBLog(LOG_ERROR, "PDBApp::Query init stmt error ");

        goto fail;
    }

    /*  ��ʼ�������Ӳ��� */
    enRet = m_pPDBConn->Clear();
    if (PDB_SUCCESS != enRet)
    {
        PDBLog(LOG_ERROR, "PDBApp::Query clear error ");

        goto fail;
    }

    /* sql���������Ұ󶨲��� */
    szConvertedSQL = BindQueryParam((CHAR*)szSQL, Param.GetParams());
    if (!szConvertedSQL)
    {
        PDBLog(LOG_ERROR, "PDBApp::Query analyse sql error ");

        goto fail;
    }

    /* �������ݿ��ѯ */
    /* ���ù�귽ʽ(���ܺͿͻ��˻��淽ʽͬʱʹ��) */
    if (ulQryType == PDB_QRY_USE_CURSOR)
    {
        enRet = m_pPDBConn->OpenCursor();
        if (PDB_SUCCESS != enRet)
        {
            PDBLog(LOG_ERROR, "PDBApp::Query open qry cursor error ");

            goto fail;
        }
    }

    enRet = m_pPDBConn->Query(m_acTransName, szConvertedSQL);

    if (PDB_SUCCESS != enRet)
    {
        PDBLog(LOG_ERROR, "PDBApp::Query error: %s", m_pPDBConn->GetErrInfo());

        goto fail;
    }

    /* �󶨽���� */
    enRet = m_pPDBConn->BindRslt();
    if (PDB_SUCCESS != enRet)
    {
        PDBLog(LOG_ERROR, "PDBApp::Query bind result error: %s", m_pPDBConn->GetErrInfo());

        goto fail;
    }

    /* �����Ƿ��ȡ�б�־ */
    m_bIsFetchReady = TRUE;

    /* ȫ����ȡ�������鿴Ӱ������ */
    iRowCount = 0;

    return iRowCount;

fail:
    CloseQuery();
    if (!bTrans)
    {
        m_pPDBConn->SetConnInUseStatus(FALSE);
    }

    CheckConnStat();

    return INVALID_ROWCOUNT;
}

/**********************************************************************
 ��    ��: RowFetch
 ��    ��: ��Query�����ɹ���ʹ���ñ�����������ȡһ�е�Ӧ��ָ�������ݽṹ��
 ����˵��:
 ȫ�ֱ���:

 ���ؽ��: ����һ�л������ݣ�����PDB_CONTINUE;
           �������ݣ�����PDB_NO_DATA;
           ���д��󣬷���PDB_ERROR
************************************************************************/
PDBRET PDBApp::RowFetch()
{
    if (NULL == m_pPDBConn)
    {
        PDBLog(LOG_ERROR, "PDBApp::RowFetch m_pPDBConn is null ");
        return PDB_ERROR;
    }

    if (FALSE == m_bIsFetchReady)
    {
        PDBLog(LOG_ERROR, "PDBApp::RowFetch qry result is not ready ");
        return PDB_ERROR;
    }

    return m_pPDBConn->RowFetch();
}

/**********************************************************************
 ��    ��: CloseQuery
 ��    ��: ��RowFetchȫ����������ñ��������ͷ����ݿ���Դ
 ����˵��:
 ȫ�ֱ���:

 ���ؽ��: TRUE or FALSE
************************************************************************/
BOOL PDBApp::CloseQuery(VOID)
{
    PDBRET      enRet = PDB_ERROR;

    if (NULL == m_pPDBConn)
    {
        PDBLog(LOG_ERROR, "PDBApp::CloseQuery m_pPDBConn is null ");
        return FALSE;
    }

    enRet = m_pPDBConn->FreeRslt();
    if (PDB_SUCCESS != enRet)
    {
        PDBLog(LOG_ERROR, "PDBApp::CloseQuery free qry rlt error ");
        enRet = PDB_ERROR;
    }

    if (m_acTransName[0] == '\0')
    {
        if (!m_pPDBConn->SetConnInUseStatus(FALSE))
        {
            PDBLog(LOG_ERROR, "PDBApp::CloseQuery [%ld] set conn in use status false fail ", m_pPDBConn->GetConnId());
        }
    }

    if (PDB_SUCCESS != enRet)
    {
        return FALSE;
    }

    return TRUE;
}

/**********************************************************************
 ��    ��: BindQueryParam
 ��    ��: ��ѯ�����������
 ����˵��:
 ȫ�ֱ���:
 ���ؽ��: TRUE or FALSE
************************************************************************/
CHAR* PDBApp::BindQueryParam(CHAR *szSQL, VECTOR<PDB_PARAM_T> &vParam)
{
    UINT16          usRsltIndex;

    m_acTmpSQL[0] = '\0';

    if (szSQL == NULL || szSQL[0] == '\0'
        || strlen(szSQL) > PDB_MAX_SQL_LEN)
    {
        PDBLog(LOG_ERROR, "PDBApp:: szSQl is error ");
        return NULL;
    }

    strcpy(m_acTmpSQL, szSQL);
    if (!ConvertSql(m_acTmpSQL))
    {
        return NULL;
    }

    /* ����� */
    for (usRsltIndex=0; usRsltIndex<vParam.size(); usRsltIndex++)
    {
        if (PDB_SUCCESS != m_pPDBConn->BindRsltByPos(usRsltIndex+1, vParam[usRsltIndex].usParamType, vParam[usRsltIndex].pvParamValue,
                                                     vParam[usRsltIndex].ulParamSize, &vParam[usRsltIndex].bIsNull, &vParam[usRsltIndex].iLenInd))
        {
            PDBLog(LOG_ERROR, "PDBApp::bind rslt %u fail", usRsltIndex);
            return NULL;
        }
    }

    return m_acTmpSQL;
}

/**********************************************************************
 ��    ��: BindUpdateParam(
 ��    ��: ������
 ����˵��:
 ȫ�ֱ���:
 ���ؽ��: TRUE or FALSE
************************************************************************/
CHAR* PDBApp::BindUpdateParam(CHAR *szSQL, VECTOR<PDB_PARAM_T> &vParam)
{
    UINT32          i;

    m_acTmpSQL[0] = '\0';

    if (szSQL == NULL || szSQL[0] == '\0'
        || strlen(szSQL) > PDB_MAX_SQL_LEN)
    {
        PDBLog(LOG_ERROR, "PDBApp:: szSQl is error ");
        return NULL;
    }

    strcpy(m_acTmpSQL, szSQL);
    if (!ConvertSql(m_acTmpSQL))
    {
        return NULL;
    }

    /* ����� */
    for (i=0; i<vParam.size(); i++)
    {
        if (PDB_SUCCESS != m_pPDBConn->BindByPos(vParam[i].usParamType, vParam[i].pvParamValue, vParam[i].ulParamSize,
                                                 vParam[i].ulInoutType,  &vParam[i].iLenInd))
        {
            PDBLog(LOG_ERROR, "PDBApp::bind param %u fail", i);
            return NULL;
        }
    }

    return m_acTmpSQL;

}

CHAR* PDBApp::BindUpdateBatchParam(CHAR *szSQL, PDBBatchParam *Param)
{
    UINT32                      i;
    VECTOR<PDB_PARAM_DESC_T>    &vParam = Param->GetParams();
    UINT8                       *pucData = (UINT8*)Param->GetBatchData();

    m_acTmpSQL[0] = '\0';

    if (szSQL == NULL || szSQL[0] == '\0'
        || strlen(szSQL) > PDB_MAX_SQL_LEN)
    {
        PDBLog(LOG_ERROR, "PDBApp:: szSQl is error ");
        return NULL;
    }

    strcpy(m_acTmpSQL, szSQL);
    if (!ConvertSql(m_acTmpSQL))
    {
        return NULL;
    }

    /* ����� */
    for (i=0; i<vParam.size(); i++)
    {
        if (PDB_SUCCESS != m_pPDBConn->BindByPos(vParam[i].usParamType,
                                                 pucData + vParam[i].ulParamOffset,
                                                 vParam[i].ulParamSize,
                                                 PDB_PARAM_INPUT, NULL))
        {
            PDBLog(LOG_ERROR, "PDBApp::bind param %u fail", i);
            return NULL;
        }
    }

    return m_acTmpSQL;

}

/**********************************************************************
��    ��: ConvertSql
��    ��: ���ݿ�ű�ת��Ϊ��׼��ʽ
�������:
ȫ�ֱ���:
���ؽ��:
************************************************************************/
BOOL PDBApp::ConvertSql(CHAR *szSQL)
{
    CHAR            *szTmpSQL = NULL;
    UINT32          i;
    UINT32          j = 0;
    UINT32          ulBindIndex = 1;

    if (m_pPDBEnv->GetDBType() != PDBT_ORACLE)
    {
        return TRUE;
    }

    if (!szTmpSQL)
    {
        szTmpSQL = (CHAR*)calloc(1, PDB_MAX_SQL_LEN*2);
        if (!szTmpSQL)
        {
            return FALSE;
        }
    }

    j = 0;
    for(i=0; ;i++)
    {
        if (szSQL[i] == '\0')
        {
            szTmpSQL[j++] = '\0';
            break;
        }

        if (szSQL[i] == PDB_BIND_MARK)
        {
            j += sprintf(szTmpSQL+j, ":%u", ulBindIndex++);
        }
        else
        {
            szTmpSQL[j++] = szSQL[i];
        }
    }

    if (strlen(szTmpSQL) > PDB_MAX_SQL_LEN)
    {
        free(szTmpSQL);
        return FALSE;
    }

    strcpy(szSQL, szTmpSQL);
    free(szTmpSQL);

    return TRUE;
}

INT32 PDBApp::GetErrCode(VOID)
{
    if (NULL == m_pPDBConn)
    {
        PDBLog(LOG_ERROR, "PDBApp::GetErrCode m_pPDBConn is null ");
        return -1;
    }

    return m_pPDBConn->GetErrCode();
}

CHAR* PDBApp::GetErrInfo(CHAR* szErrInfo)
{
    if (NULL == m_pPDBConn)
    {
        PDBLog(LOG_ERROR, "PDBApp::m_pPDBConn is null");
        if (szErrInfo)
        {
            strcpy(szErrInfo, "m_pPDBConn is NULL");
        }

        return (CHAR *)"m_pPDBConn is NULL";
    }

    return m_pPDBConn->GetErrInfo(szErrInfo);
}

PDBRET PDBApp::ReConnect(VOID)
{
    if (NULL == m_pPDBConn)
    {
        PDBLog(LOG_ERROR, "PDBApp::ReConnect m_pPDBConn is null ");
        return PDB_ERROR;
    }

    return m_pPDBConn->ReConnect();
}

