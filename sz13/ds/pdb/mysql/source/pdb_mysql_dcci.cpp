/******************************************************************************
模块名          ：  Database C++ Call Interface (DCCI)
文件名          ：  pdb_dcci.cpp
相关文件        ：
文件实现功能    ：  数据库访问API,封装DCI层函数
作者            ：
版本            ：  V0.1
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
******************************************************************************/

#include "g_include.h"

#include "mysql.h"

#include "pdb_def.h"
#include "pdb_util.h"
#include "pdb_error.h"
#include "pdb_common.h"
#include "pdb_dcci.h"
#include "pdb_mysql_core.h"

/*******************************************************************************
* 函数名称: CreateConn
* 功    能: 创建一个数据库连接，可在应用的数据库初始化中创建一定数量的连接
* 函数类型: DCCI_RET_E
* 参    数:
* 参数名称        类型              输入/输出         描述
* **ppConn      DCCIVOID             OUT           数据库连接
* 函数返回:
* 说    明:
*******************************************************************************/
PDBRET PDBMySQLEnv::CreateConn()
{
    if (m_ulConnNum >= PDB_MAX_CONNECT_NUM)
    {
        return PDB_ERROR_CREATE_CONN;
    }

    if (!DCILock(m_hLockId, PDB_WAIT_FOREVER))
    {
        return PDB_ERROR_CREATE_CONN;
    }

    m_pConn[m_ulConnNum] = new PDBMySQLConn(this);

    /* 指针有效性判断 */
    if (NULL == m_pConn[m_ulConnNum])
    {
        DCIUnlock(m_hLockId);
        return PDB_ERROR_CREATE_CONN;
    }

    /* m_pConn的连接ID从1开始 */
    m_pConn[m_ulConnNum]->SetConnId(m_ulConnNum+1); //((INVALID_INDEX == iMaxConnId)?1:(iMaxConnId+1));
    m_ulConnNum++;

    DCIUnlock(m_hLockId);

    return PDB_SUCCESS;
}


PDBRET PDBMySQLEnv::FreeConn(UINT32 ulConnID)
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

        PDBMySQLConn   *pConn = (PDBMySQLConn*)m_pConn[ulConnID];

        delete pConn;
        m_pConn[ulConnID] = NULL;
    }

    DCIUnlock(m_hLockId);

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: CreateEnv
 功    能: 创建数据库环境
 参数说明:
 全局变量:
 返回结果: PDB_SUCCESS or PDB_ERROR
************************************************************************/
PDBRET PDBMySQLEnv::CreateEnv(VOID)
{
    PDBRET enRet = PDB_ERROR;

    /*分配环境句柄*/
    enRet = MySQLInit(&m_hEnv);
    if (enRet != PDB_SUCCESS)
    {
        return PDB_ERROR_CREATE_ENV;
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: FreeEnv
 功    能: 释放数据库环境，数据库连接
 参数说明:
 全局变量:
 返回结果: PDB_SUCCESS or PDB_ERROR
************************************************************************/
PDBRET PDBMySQLEnv::FreeEnv(VOID)
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

    MySQLExit(m_hEnv);
    m_hEnv = NULL;

    DCIDeleteLock(m_hLockId);
    m_hLockId = INVALID_LOCK;

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: PDBMySQLConn
 功    能: 构造函数
 参数说明:
 全局变量:
 返回结果:
************************************************************************/
PDBMySQLConn::PDBMySQLConn(PDBEnvBase *pEnv)
{
    m_pEnv = pEnv;

    Init();
}

PDBMySQLConn::~PDBMySQLConn()
{
    GOS_FREE(m_pPDBHandle);

    DCIDeleteLock(m_hConnectLockId);
    DCIDeleteLock(m_hAllocLockId);
    DCIDeleteLock(m_hConnLockId);
}

/**********************************************************************
 函    数: Init
 功    能: 数据库连接初始化
 参数说明:
 全局变量:
 返回结果:
************************************************************************/
PDBRET PDBMySQLConn::Init()
{
    memset(&m_stConnParam, 0, sizeof(m_stConnParam));
    memset(&m_stDBOption, 0, sizeof(m_stDBOption));

    m_bReConnect = FALSE;

    m_hConnectLockId = NULL;
    m_hAllocLockId = NULL;
    m_hConnLockId = NULL;

    m_ulLockTimeout = LOCK_TIMEOUT;

    m_pPDBHandle = (MYSQL_PDB_HANDLE_T *)calloc(1, sizeof(MYSQL_PDB_HANDLE_T));
    if (NULL == m_pPDBHandle)
    {
        return PDB_ERROR;
    }

    // 初始化连接锁
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
 函    数: CheckCurConn
 功    能: 检查当前连接是否有效
 参数说明:
 全局变量:

 返回结果: TRUE or FALSE
************************************************************************/
BOOL PDBMySQLConn::CheckConn() //PDBUINT32 ulLockId, PDBUINT32 ulLockTimeout
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
        //PDBLog(LOG_INFO, "PDBMySQLConn::CheckConn set alloc status {true} begin[%ld] ", GetConnId());
        if (!SetAllocStatus(TRUE))
        {
            PDBLog(LOG_INFO, "PDBMySQLConn::CheckConn set alloc status {true} fail[%ld]" , GetConnId());
            return FALSE;
        }
    }

    //DCIUnlock(ulLockId);

    /*
    if (FALSE == bDoCheck)
    {
        PDBLog(LOG_ERROR, "PDBMySQLConn::CheckConn connect[%ld] is in use ", GetConnId());
        return TRUE;
    }*/

//    PDBLog(LOG_INFO, "PDBMySQLConn::CheckConn begin real check[%ld] ", GetConnId());

    /* 初始化数据库语句句柄 */
    enRet = InitStmt();

    if (PDB_SUCCESS == enRet)
    {
        Clear();
        enRet = Query((CHAR*)NULL, szSql);
        FreeRslt();
    }

    CloseStmt();

    if (PDB_SUCCESS != enRet)
    {
        /* 连接有问题，重连 */
        GetErrInfo(acErrInfo);
        PDBLog(LOG_INFO, "PDBMySQLConn::CheckConn(%d) failed, error code: %u, error info: %s", GetConnId(), GetErrCode(), acErrInfo);

        if (PDB_SUCCESS != ReConnect())
        {
            GetErrInfo(acErrInfo);
            PDBLog(LOG_INFO, "PDBMySQLConn::CheckConn(%d) reconnect failed, error code: %u, error info: %s", GetConnId(), GetErrCode(), acErrInfo);
        }
        //Prn("[fatal] PDBMySQLConn::CheckConn connect is invalid reconnect ");
    }
    else
    {
        bRet = TRUE;
    }

    if (!SetAllocStatus(bOrgAlloced) &&
        FALSE == bOrgAlloced)
    {
        PDBLog(LOG_INFO, "PDBMySQLConn::CheckConn set alloc status {%u} fail[%ld] ", bOrgAlloced, GetConnId());
        return FALSE;
    }

    return bRet;
}

/**********************************************************************
 函    数: ReConnect
 功    能: 重新连接数据库
 参数说明:
 全局变量:
 返回结果: PDB_SUCCESS or PDB_ERROR
************************************************************************/
PDBRET PDBMySQLConn::ReConnect()
{
    m_bReConnect = TRUE;

    return Connect(&m_stConnParam, &m_stDBOption);
}

/**********************************************************************
 函    数: Connect
 功    能: 创建数据库连接
 参数说明: szServer     IN  服务器IP
           usPort       IN  数据库实例端口
           szDBName     IN  数据库名
           szUserName   IN  用户
           szPassword   IN  密码
           pstDBOption  IN  数据库参数指针
           全局变量:
           返回结果: PDB_SUCCESS or PDB_ERROR
************************************************************************/
PDBRET PDBMySQLConn::Connect(PDB_CONN_PARAM_T *pstConnParam, PDB_OPTION_T *pstDBOption)
{
    //PDB_OPTION_T  stOption = {0};
    MYSQL_DCI_HANDLE_T  *pstDCIHandle;
    MYSQL_PDB_HANDLE_T  *pstPDBHandle = NULL;

    //Prn("PDBMySQLConn::Connect ");

    if (NULL == m_pPDBHandle)
    {
        pstPDBHandle = (MYSQL_PDB_HANDLE_T *)calloc(1, sizeof(MYSQL_PDB_HANDLE_T));
        if(NULL == pstPDBHandle)
        {
            return PDB_ERROR;
        }

        m_pPDBHandle = pstPDBHandle;

    }
    else
    {
        pstPDBHandle = (MYSQL_PDB_HANDLE_T *)m_pPDBHandle;
    }

    //    stOption.ulSelRowCount = 0;   /* 一次查询的最多记录行数 */
    /*
    stOption.ulPrefetchRow = PDB_PREFETCH_ROW_NUM;
    stOption.ulPrefetchMem = 0;
    stOption.usBatchNum    = 0;
    */

    if (IsConnected()) //pstPDBHandle->bIsConnected
    {
        Disconnect();
    }

    pstDCIHandle = (MYSQL_DCI_HANDLE_T *)&pstPDBHandle->stDCIHandle;
    pstDCIHandle->EnvHandle = m_pEnv->GetEnv();
    if (PDB_SUCCESS != MySQLConnect(pstDCIHandle, pstConnParam, pstDBOption))
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

    /* 重连不改变alloc状态 */
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

    CHAR    *szCharacterSet = m_pEnv->GetCharacterSet();

    if (szCharacterSet[0])
    {
        CHAR    acSQL[128];

        sprintf(acSQL, "set names %s", szCharacterSet);
        if (PDB_SUCCESS != Exec(NULL, acSQL))
        {
            return PDB_ERROR_CREATE_CONN;
        }
    }

    SetConnectStatus(TRUE);
    //pstPDBHandle->bIsConnected = TRUE;

    pstDCIHandle->ulExecNum = 1;
    //MySQLSetOption(pstDCIHandle, &stOption);

    memcpy(&m_stConnParam, pstConnParam, sizeof(m_stConnParam));
    memcpy(&m_stDBOption, pstDBOption, sizeof(m_stDBOption));

    m_bReConnect = FALSE;

    return PDB_SUCCESS;
}

/*=====================================================================
函    数: Disconnect
功    能: 与数据库断开连接
输入参数:
输出参数:
全局变量:
返回结果:
======================================================================*/
PDBRET PDBMySQLConn::Disconnect()
{
    PDBRET          enRet;
    MYSQL_PDB_HANDLE_T  *pstHandle = NULL;

    //Prn("PDBMySQLConn::Disconnect ");

    pstHandle = (MYSQL_PDB_HANDLE_T*)m_pPDBHandle;

    PDB_CONN_HANDLE_CHECK(pstHandle);
        //PDB_CONN_STMT_HANDLE_CHECK(pstHandle);

    enRet = MySQLDisconnect((MYSQL_DCI_HANDLE_T*)&pstHandle->stDCIHandle);

    if (enRet == PDB_SUCCESS)
    {
        SetConnectStatus(FALSE);
        //pstHandle->bIsConnected = FALSE;

        /* 重连不改变alloc状态 */
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
函    数: Clear
功    能: 数据库操作前的初始化
输入参数: 无
输出参数:
全局变量:
返回结果: 成功
======================================================================*/
PDBRET PDBMySQLConn::Clear()
{
    MYSQL_PDB_HANDLE_T  *pstHandle = NULL;
    MYSQL_DCI_HANDLE_T  *pstDCIHandle = NULL;

    //Prn("PDBMySQLConn::Clear ");

    pstHandle = (MYSQL_PDB_HANDLE_T*)m_pPDBHandle;
    //PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    PDB_CONN_HANDLE_CHECK(pstHandle);
    pstDCIHandle = &pstHandle->stDCIHandle;

    //pstDCIHandle->stOption.ulPrefetchRow = PDB_PREFETCH_ROW_NUM;
    //pstDCIHandle->stOption.ulPrefetchMem = 0;

    /* 默认为单次操作，执行一次 */
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
    pstDCIHandle->ulSendBlobParamNum = 0;*/
    pstDCIHandle->ulBindRsltNum = 0;
    pstDCIHandle->ulFetchedNum = 0;

    memset(pstDCIHandle->astBindRsltHandle, 0, sizeof(pstDCIHandle->astBindRsltHandle));
    memset(pstDCIHandle->astBindHandle, 0, sizeof(pstDCIHandle->astBindHandle));

    pstDCIHandle->DBHandles.pstQryRlst = NULL;
    pstDCIHandle->DBHandles.pszQryRow = NULL;

    return PDB_SUCCESS;
}

/*=====================================================================
函    数: InitStmt
功    能: 初始化数据库语句句柄
输入参数: 无
输出参数:
全局变量:
返回结果: 成功
======================================================================*/
PDBRET PDBMySQLConn::InitStmt()
{
    PDBRET      enRet;
    MYSQL_PDB_HANDLE_T  *pstHandle = NULL;

    //Prn("PDBMySQLConn::InitStmt ");

    pstHandle = (MYSQL_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_HANDLE_CHECK(pstHandle);

    enRet = MySQLInitStmt(&pstHandle->stDCIHandle);
    PDB_CHECK_ERR(enRet);

    return PDB_SUCCESS;
}

/*=====================================================================
函    数: CloseStmt
功    能: 关闭数据库语句句柄
输入参数: 无
输出参数:
全局变量:
返回结果: 成功
======================================================================*/
PDBRET PDBMySQLConn::CloseStmt()
{
    PDBRET      enRet;
    MYSQL_PDB_HANDLE_T  *pstHandle = NULL;

    //Prn("PDBMySQLConn::CloseStmt ");
    pstHandle = (MYSQL_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    enRet = MySQLCloseStmt(&pstHandle->stDCIHandle);
    PDB_CHECK_ERR(enRet);

    return PDB_SUCCESS;
}

/*=====================================================================
函    数: BindByPos
功    能: SQL命令的变量输入
输入参数:   ulType   --- 输入变量类型
            pData    --- 输入变量数据
            iLen     --- 输入变量长度
            bIsOutPara--- 出参标志
全局变量:
返回结果: PDB_SUCCESS | PDB_ERROR
======================================================================*/
PDBRET PDBMySQLConn::BindByPos(UINT32 ulType, VOID *pData, INT32 iLen, UINT32 ulInoutType, INT32 *piLenInd)
{
    PDBRET          enRet;
    UINT32          ulStrLen;
    UINT32          ulParaNo;
    MYSQL_PDB_HANDLE_T  *pstHandle = NULL;

    //Prn("PDBMySQLConn::BindByPos ");

    pstHandle = (MYSQL_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);

    ulParaNo = pstHandle->stDCIHandle.ulBindNum;

    if (ulParaNo >= PDB_MAX_BIND_NUM)
    {
        return PDB_ERROR;
    }

    if (ulType == PDBT_STRING)
    {
        ulStrLen = strlen((CHAR*)pData);// + 1;

        if (ulStrLen < iLen)
        {
            iLen = ulStrLen;
        }

        //iLen = strlen((CHAR*)pData)+1;
    /*    if (iLen == 0)
        {
            pData = (VOID*)" ";
            iLen = 1;
        }*/
    }

    enRet = MySQLBindPara(&pstHandle->stDCIHandle, (UINT16)ulParaNo, ulType, ulInoutType, pData, (UINT32)iLen, piLenInd);

    PDB_CHECK_ERR(enRet);

    return PDB_SUCCESS;
}

/*====================================================================
函数名      : BindRsltByPos
功能        : 批量查询时绑定游标对应的字段
算法实现    :
引用全局变量:   usRsltIndex -- 绑定结果集的列index
                usDataType --  绑定结果集的列对应外部数据类型
                pBuf -- 绑定结果集的列缓冲区首地址
                pulBufLen -- 绑定结果集的列缓冲区长度指针（入参/出参）
                pucNull -- 标识字段是否为空的列数组指针
输入参数说明:
返回值说明  :
====================================================================*/
PDBRET PDBMySQLConn::BindRsltByPos(UINT16    usRsltIndex,
                                  UINT16    usDataType,
                                  VOID      *pBuf,
                                  UINT32    ulBufLen,
                                  BOOL      *pbIsNull,
                                  INT32     *piLenInd)
{
    PDBRET          enRet;
    MYSQL_PDB_HANDLE_T  *pdbHandle = NULL;

    pdbHandle = (MYSQL_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pdbHandle);

    enRet = MySQLBindRsltByPos(&pdbHandle->stDCIHandle, usRsltIndex, usDataType, pBuf, ulBufLen, pbIsNull, piLenInd);
    PDB_CHECK_ERR(enRet);

    return PDB_SUCCESS;
}

/*=====================================================================
函    数: OpenCursor
功    能: 打开查询光标
输入参数:
输出参数: 无
全局变量: 无
返回结果: PDB_SUCCESS | PDB_ERROR
======================================================================*/
PDBRET PDBMySQLConn::OpenCursor()
{
    PDBRET          enRet = PDB_SUCCESS;
    MYSQL_DCI_HANDLE_T  *pstDCIHandle;
    MYSQL_PDB_HANDLE_T  *pstHandle = NULL;

    pstHandle = (MYSQL_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    pstDCIHandle = (MYSQL_DCI_HANDLE_T *)&pstHandle->stDCIHandle;

    enRet = MySQLSetPrefetchNum(pstDCIHandle, pstDCIHandle->stOption.ulPrefetchRow);

    return CheckErr(enRet);
}

/*=====================================================================
函    数: Query
功    能:
输入参数:
输出参数: 无
全局变量: 无
返回结果: PDB_SUCCESS | PDB_ERROR
======================================================================*/
PDBRET PDBMySQLConn::Query(CHAR *szTransName, CHAR *szSQL)
{
    PDBRET      enRet;
    MYSQL_DCI_HANDLE_T  *pstDCIHandle;
    MYSQL_PDB_HANDLE_T  *pstHandle = NULL;

    pstHandle = (MYSQL_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    pstDCIHandle = (MYSQL_DCI_HANDLE_T *)&pstHandle->stDCIHandle;

    if (NULL == szSQL || szSQL[0] == '\0')
    {
        PDBLog(LOG_ERROR, "PDBMySQLConn::Query sql is null");
//      MYSQL_STMT_CLOSE(pstDCIHandle->DBHandles.StmtHandle);
        return PDB_ERROR_SET_ATTR;
    }

    if (szTransName == NULL || szTransName[0] == '\0')
    {
        /* 设置自动提交 */
        enRet = MySQLSetAutoCommit(pstDCIHandle, TRUE);
        if (enRet != PDB_SUCCESS)
        {
            PDBLog(LOG_ERROR, "PDBMySQLConn::Query set auto commit(true) fail ");
        //  MYSQL_STMT_CLOSE(pstDCIHandle->DBHandles.StmtHandle);
            return PDB_ERROR_SET_ATTR;
        }
        pstDCIHandle->acTransName[0] = '\0';
    }
    else
    {
        /* 设置为事务 */
        enRet = MySQLSetAutoCommit(pstDCIHandle, FALSE);
        if (enRet != PDB_SUCCESS)
        {
            PDBLog(LOG_ERROR, "PDBMySQLConn::Query set auto commit(false) fail ");
        //  MYSQL_STMT_CLOSE(pstDCIHandle->DBHandles.StmtHandle);
            return PDB_ERROR_SET_ATTR;
        }
        strcpy((CHAR *)pstDCIHandle->acTransName, szTransName);
    }

    strncpy(pstDCIHandle->acSQLCmd, szSQL, PDB_MAX_SQL_LEN);
    pstDCIHandle->acSQLCmd[PDB_MAX_SQL_LEN] = '\0';

    pstDCIHandle->ulRowCount = 0;

    /* 初始化stmt，prepare sql */
    enRet = MySQLSQLPrepare(pstDCIHandle);
    PDB_CHECK_ERR(enRet);

    /* Exec SQL Command */
    //pstPDBHandle->usExecNum = 1;
    enRet = MySQLQuery(pstDCIHandle);

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
函数名      :BindRslt
功能        :绑定结果集，在RowFetch前使用
算法实现    :
引用全局变量:
输入参数说明:
返回值说明  :
====================================================================*/
PDBRET PDBMySQLConn::BindRslt()
{
    PDBRET          enRet;
    //PDBINT32        ulRowCount;
    MYSQL_DCI_HANDLE_T  *pstDCIHandle;
    MYSQL_PDB_HANDLE_T  *pstHandle = NULL;

    pstHandle = (MYSQL_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    pstDCIHandle = (MYSQL_DCI_HANDLE_T*)&pstHandle->stDCIHandle;

    enRet = MySQLBindRslt(pstDCIHandle);

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
函数名      :BindRslt
功能        :绑定结果集，在RowFetch前使用
算法实现    :
引用全局变量:
输入参数说明:
返回值说明  :
====================================================================*/
PDBRET PDBMySQLConn::StoreClientRslt()
{
    PDBRET          enRet;
    INT32           ulRowCount;
    MYSQL_DCI_HANDLE_T  *pstDCIHandle;
    MYSQL_PDB_HANDLE_T  *pstHandle = NULL;

    pstHandle = (MYSQL_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    pstDCIHandle = (MYSQL_DCI_HANDLE_T*)&pstHandle->stDCIHandle;

    enRet = MySQLStoreRslt(pstDCIHandle);

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
函数名      :RowFetch
功能        :从结果中获取一列
算法实现    :
引用全局变量:
输入参数说明:   pstDCIHandle --- PDB CLI数据库连接
返回值说明  :
====================================================================*/
PDBRET PDBMySQLConn::RowFetch()
{
    PDBRET          enRet;
    MYSQL_DCI_HANDLE_T  *pstDCIHandle;
    MYSQL_PDB_HANDLE_T  *pstHandle = NULL;

    pstHandle = (MYSQL_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    pstDCIHandle = (MYSQL_DCI_HANDLE_T*)&pstHandle->stDCIHandle;

    enRet = MySQLRowFetch(pstDCIHandle);

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

 /*   CHAR    *szValue;

    for (UINT32 i=0; i<pstDCIHandle->ulBindRsltNum; i++)
    {
        if (pstDCIHandle->astBindRsltHandle[i].buffer_type == MYSQL_TYPE_VAR_STRING)
        {
            szValue = (CHAR*)pstDCIHandle->astBindRsltHandle[i].buffer;
            if (szValue && szValue[0] == ' ' && szValue[1] == '\0')
            {
                *szValue = '\0';
            }
        }
    }*/

    return PDB_CONTINUE;
}

/*====================================================================
函数名      :FreeRslt
功能        :绑定结果集，在StoreClientRslt及RowFetch后使用
算法实现    :
引用全局变量:
输入参数说明:
返回值说明  :
====================================================================*/
PDBRET PDBMySQLConn::FreeRslt()
{
    PDBRET          enRet;
    MYSQL_DCI_HANDLE_T  *pstDCIHandle;
    MYSQL_PDB_HANDLE_T  *pstHandle = NULL;

    pstHandle = (MYSQL_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    pstDCIHandle = (MYSQL_DCI_HANDLE_T*)&pstHandle->stDCIHandle;

    enRet = MySQLFreeRslt(pstDCIHandle);

    return CheckErr(enRet);
}

VOID PDBMySQLConn::SetBatchExecParam(UINT32 ulSkipLen, UINT32 ulExecNum)
{
    MYSQL_DCI_HANDLE_T  *pstDCIHandle;
    MYSQL_PDB_HANDLE_T  *pstHandle = NULL;

    pstHandle = (MYSQL_PDB_HANDLE_T*)m_pPDBHandle;
    if (pstHandle)
    {
        pstDCIHandle = (MYSQL_DCI_HANDLE_T *)&pstHandle->stDCIHandle;

        pstDCIHandle->ulExecNum = ulExecNum;
        pstDCIHandle->ulBindSkipLen = ulSkipLen;
    }
}

/*=====================================================================
函    数: Exec
功    能:
输入参数:
输出参数: 无
全局变量: 无
返回结果: PDB_SUCCESS | PDB_ERROR
======================================================================*/
PDBRET PDBMySQLConn::Exec(CHAR* szTransName, CHAR* szSQL)
{
    PDBRET          enRet;
    INT32           iRowCount;
    MYSQL_DCI_HANDLE_T  *pstDCIHandle;
    MYSQL_PDB_HANDLE_T  *pstHandle = NULL;

    pstHandle = (MYSQL_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);
    pstDCIHandle = (MYSQL_DCI_HANDLE_T *)&pstHandle->stDCIHandle;

    if (NULL == szSQL || szSQL[0] == '\0')
    {
        PDBLog(LOG_ERROR, "PDBMySQLConn::Exec sql is null ");
//      MYSQL_STMT_CLOSE(pstDCIHandle->DBHandles.StmtHandle);
        return PDB_ERROR_SET_ATTR;
    }

    if (szTransName == NULL || szTransName[0] == '\0')
    {
        /* 设置自动提交 */
        enRet = MySQLSetAutoCommit(pstDCIHandle, TRUE);
        if (enRet != PDB_SUCCESS)
        {
            PDBLog(LOG_ERROR, "PDBMySQLConn::Exec set auto commit(true) fail ");
        //  MYSQL_STMT_CLOSE(pstDCIHandle->DBHandles.StmtHandle);
            return PDB_ERROR_SET_ATTR;
        }
        pstDCIHandle->acTransName[0] = '\0';
    }
    else
    {
        /* 设置为事务 */
        enRet = MySQLSetAutoCommit(pstDCIHandle, FALSE);
        if (enRet != PDB_SUCCESS)
        {
            PDBLog(LOG_ERROR, "PDBMySQLConn::Exec set auto commit(false) fail ");
        //  MYSQL_STMT_CLOSE(pstDCIHandle->DBHandles.StmtHandle);
            return PDB_ERROR_SET_ATTR;
        }
        strcpy((CHAR *)pstDCIHandle->acTransName, szTransName);
    }

    strncpy(pstDCIHandle->acSQLCmd, szSQL, PDB_MAX_SQL_LEN);
    pstDCIHandle->acSQLCmd[PDB_MAX_SQL_LEN] = '\0';

    pstDCIHandle->ulRowCount = 0;

    /* 初始化stmt，prepare sql */
    enRet = MySQLSQLPrepare(pstDCIHandle);
    PDB_CHECK_ERR(enRet);

    /* Exec SQL Command */
    enRet = MySQLExec(pstDCIHandle);
    iRowCount = MySQLRowCount(pstDCIHandle);

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
        PDBLog(LOG_INFO, "PDBMySQLConn::Exec success ");
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
函    数: PDBRET SetPrefetchSize
功    能: 设置批量查询时预提取记录数（内存大小）
输入参数: ulPreFetchNum    IN  预提取记录数
ulPreFetchMem    IN  预提取记录大小(0表示不限制)
全局变量:
返回结果:
************************************************************************/
PDBRET PDBMySQLConn::SetPrefetchSize(UINT32 ulPreFetchNum, UINT32 ulPreFetchMem)
{
    MYSQL_PDB_HANDLE_T *pstHandle = NULL;

    pstHandle = (MYSQL_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_STMT_HANDLE_CHECK(pstHandle);

    pstHandle->stDCIHandle.stOption.ulPrefetchRow = ulPreFetchNum;
    pstHandle->stDCIHandle.stOption.ulPrefetchMem = ulPreFetchMem;

    return PDB_SUCCESS;
}

/*=====================================================================
 函    数: TransBegin
 功    能: 开始一个事务
 输入参数: szTransName  --- 事务名
 输出参数:
 全局变量: 无
 返回结果:
======================================================================*/
PDBRET PDBMySQLConn::TransBegin(CHAR * szTransName)
{
    PDBRET          enRet = PDB_ERROR;
    MYSQL_PDB_HANDLE_T  *pstHandle = NULL;

    pstHandle = (MYSQL_PDB_HANDLE_T*)m_pPDBHandle;
    PDB_CONN_HANDLE_CHECK(pstHandle);

    enRet = MySQLTransBegin(&pstHandle->stDCIHandle, szTransName);

    PDB_CHECK_ERR(enRet);

    return PDB_SUCCESS;
}

/*=====================================================================
函    数: TransCommit
功    能: 提交数据库
输入参数: szTransName    --- 事务名
输出参数:
全局变量: 无
返回结果:
======================================================================*/
PDBRET PDBMySQLConn::TransCommit(CHAR * szTransName)
{
    PDBRET  enRet = PDB_ERROR;
    MYSQL_PDB_HANDLE_T  *pstHandle = NULL;

    pstHandle = (MYSQL_PDB_HANDLE_T*)m_pPDBHandle;

    PDB_CONN_HANDLE_CHECK(pstHandle);

    enRet = MySQLTransCommit(&pstHandle->stDCIHandle, szTransName);

    PDB_CHECK_ERR(enRet);

    return PDB_SUCCESS;
}

/*=====================================================================
函    数: TransRollback
功    能: 回滚数据库
输入参数: szTransName   --- 事务名
输出参数:
全局变量: 无
返回结果:
======================================================================*/
PDBRET PDBMySQLConn::TransRollback(CHAR * szTransName)
{
    PDBRET  enRet = PDB_ERROR;
    MYSQL_PDB_HANDLE_T  *pstHandle = NULL;

    pstHandle = (MYSQL_PDB_HANDLE_T*)m_pPDBHandle;

    PDB_CONN_HANDLE_CHECK(pstHandle);

    enRet = MySQLTransRollback(&pstHandle->stDCIHandle, szTransName);

    PDB_CHECK_ERR(enRet);

    return PDB_SUCCESS;
}

/*====================================================================
函数名      : PDBGetErrGrade
功能        :
算法实现    :
引用全局变量:
输入参数说明:
返回值说明  :
====================================================================*/
BYTE PDBMySQLConn::GetErrGrade(PDBRET enErrCode)
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
函数名      : GetTransName
功能        :
算法实现    :
引用全局变量:
输入参数说明:
返回值说明  :
====================================================================*/
CHAR* PDBMySQLConn::GetTransName()
{
    if (NULL == m_pPDBHandle)
    {
        return NULL;
    }

    return ((MYSQL_PDB_HANDLE_T*)m_pPDBHandle)->stDCIHandle.acTransName;
}

/*====================================================================
函数名      : GetRowCount
功能        :
算法实现    :
引用全局变量:
输入参数说明:
返回值说明  :
====================================================================*/
INT32 PDBMySQLConn::GetRowCount()
{
    if (NULL == m_pPDBHandle)
    {
        return INVALID_ROWCOUNT;
    }

    return ((MYSQL_PDB_HANDLE_T*)m_pPDBHandle)->stDCIHandle.ulRowCount;
}

/*====================================================================
函数名      : SetConnectStatus
功能        : 设置连接状态
算法实现    :
引用全局变量:
输入参数说明:
返回值说明  :
====================================================================*/
BOOL PDBMySQLConn::SetConnectStatus(BOOL bIsConnected)
{
    if (NULL == m_pPDBHandle)
    {
        return FALSE;
    }

    if (!DCILock(m_hConnectLockId, PDB_WAIT_FOREVER))
    {
        PDBLog(LOG_ERROR, "PDBMySQLConn::SetConnectStatus lock fail ");
        return FALSE;
    }

    if (bIsConnected && ((MYSQL_PDB_HANDLE_T*)m_pPDBHandle)->bIsConnected)
    {
        DCIUnlock(m_hConnectLockId);
        return FALSE;
    }

    ((MYSQL_PDB_HANDLE_T*)m_pPDBHandle)->bIsConnected = bIsConnected;

    DCIUnlock(m_hConnectLockId);

    return TRUE;
}

/*====================================================================
函数名      : IsConnected
功能        是否连接正常
算法实现    :
引用全局变量:
输入参数说明:
返回值说明  :
====================================================================*/
BOOL PDBMySQLConn::IsConnected()
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

    bIsConnected = ((MYSQL_PDB_HANDLE_T*)m_pPDBHandle)->bIsConnected;

    DCIUnlock(m_hConnectLockId);

    return bIsConnected;
}

/*====================================================================
函数名      : IsAlloced
功能        连接是否已分配
算法实现    :
引用全局变量:
输入参数说明:
返回值说明  :
====================================================================*/
BOOL PDBMySQLConn::IsAlloced()
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

    bIsAlloced = ((MYSQL_PDB_HANDLE_T*)m_pPDBHandle)->bIsAlloced;

    DCIUnlock(m_hAllocLockId);

    return bIsAlloced;
}

/*====================================================================
函数名      : SetAllocStatus
功能        : 设置连接的分配状态
算法实现    :
引用全局变量:
输入参数说明:
返回值说明  :
====================================================================*/
BOOL PDBMySQLConn::SetAllocStatus(BOOL bIsAlloced)
{
//    PDBLog(LOG_INFO, "PDBMySQLConn::SetAllocStatus ");

    if (NULL == m_pPDBHandle)
    {
        return FALSE;
    }

    if (!DCILock(m_hAllocLockId, m_ulLockTimeout))
    {
        PDBLog(LOG_ERROR, "PDBMySQLConn::SetAllocStatus lock fail ");
        return FALSE;
    }

    if (bIsAlloced && ((MYSQL_PDB_HANDLE_T*)m_pPDBHandle)->bIsAlloced)
    {
        DCIUnlock(m_hAllocLockId);
        return FALSE;
    }

/*
    if (!DCILock(m_hAllocLockId, m_ulLockTimeout))
    {
        PDBLog(LOG_ERROR, "PDBMySQLConn::SetAllocStatus lock fail ");
        return FALSE;
    }

    PDBLog(LOG_ERROR, "PDBMySQLConn::SetAllocStatus lock[%u] success ", GetConnId());
*/

    ((MYSQL_PDB_HANDLE_T*)m_pPDBHandle)->bIsAlloced = bIsAlloced;

    DCIUnlock(m_hAllocLockId);
/*
    DCIUnlock(m_hAllocLockId);

    PDBLog(LOG_ERROR, "PDBMySQLConn::SetAllocStatus unlock[%u] success ", GetConnId());
*/
    return TRUE;
}

/*====================================================================
函数名      : SetConnInUseStatus
功能        : 设置连接的使用状态
算法实现    :
引用全局变量:
输入参数说明:
返回值说明  :
====================================================================*/
BOOL PDBMySQLConn::SetConnInUseStatus(BOOL bIsInUse)
{
    //PDBLog(LOG_INFO, "PDBMySQLConn::SetConnInUseStatus ");

    if (NULL == m_pPDBHandle)
    {
        return FALSE;
    }

    if (!DCILock(m_hConnLockId, m_ulLockTimeout))
    {
        PDBLog(LOG_ERROR, "PDBMySQLConn::SetConnInUseStatus lock fail");
        return FALSE;
    }

    if (bIsInUse && ((MYSQL_PDB_HANDLE_T*)m_pPDBHandle)->bIsInUse)
    {
        DCIUnlock(m_hConnLockId);
        return FALSE;
    }

    ((MYSQL_PDB_HANDLE_T*)m_pPDBHandle)->bIsInUse = bIsInUse;

    DCIUnlock(m_hConnLockId);

    //PDBLog(LOG_INFO, "PDBMySQLConn::SetConnInUseStatus unlock[%u] success", GetConnId());

    return TRUE;
}

/*====================================================================
函数名      : IsConnInUse
功能        连接是否使用中
算法实现    :
引用全局变量:
输入参数说明:
返回值说明  :
====================================================================*/
BOOL PDBMySQLConn::IsConnInUse(VOID)
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

    bIsInUse = ((MYSQL_PDB_HANDLE_T*)m_pPDBHandle)->bIsInUse;

    DCIUnlock(m_hConnLockId);

    return bIsInUse;
}

/*====================================================================
函数名      : GetConnId
功能        : 获取本连接的ID
算法实现    :
引用全局变量:
输入参数说明:
返回值说明  :
====================================================================*/
INT32 PDBMySQLConn::GetConnId(VOID)
{
    if (NULL == m_pPDBHandle)
    {
        return INVALID_INDEX;
    }

    return ((MYSQL_PDB_HANDLE_T*)m_pPDBHandle)->iConnId;
}

/*====================================================================
函数名      : SetConnId
功能        : 设置本连接的ID
算法实现    :
引用全局变量:
输入参数说明:
返回值说明  :
====================================================================*/
VOID PDBMySQLConn::SetConnId(INT32 iConnId)
{
    if (NULL == m_pPDBHandle)
    {
        return;
    }

    ((MYSQL_PDB_HANDLE_T*)m_pPDBHandle)->iConnId = iConnId;
}

/**********************************************************************
函    数: CheckErr
功    能: 错误处理函数
输入参数:
全局变量:
返回结果:
************************************************************************/
PDBRET PDBMySQLConn::CheckErr(PDBRET enErrCode)
{
    PDBRET          enRet;
    MYSQL_DCI_HANDLE_T  *pstDCIHandle;
    MYSQL_PDB_HANDLE_T  *pstHandle = NULL;

    pstHandle = (MYSQL_PDB_HANDLE_T*)m_pPDBHandle;
    if (NULL == pstHandle)
    {
        return PDB_ERROR;
    }

    pstDCIHandle = (MYSQL_DCI_HANDLE_T*)&pstHandle->stDCIHandle;

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
            MySQLGetError(pstDCIHandle);
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
函    数: CheckErrCode
功    能: 获取错误原因码
输入参数:
全局变量:
返回结果:
************************************************************************/
PDBRET PDBMySQLConn::CheckErrCode(PDBRET enErrCode)
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
函    数: GetErrInfo
功    能: 获取错误原因
输入参数:
全局变量:
返回结果:
************************************************************************/
CHAR* PDBMySQLConn::GetErrInfo(CHAR* szErrInfo)
{
    MYSQL_DCI_HANDLE_T  *pstDCIHandle;
    MYSQL_PDB_HANDLE_T  *pstHandle = NULL;

    if (szErrInfo)
    {
        *szErrInfo = '\0';
    }

    pstHandle = (MYSQL_PDB_HANDLE_T*)m_pPDBHandle;
    if (NULL == pstHandle)
    {
        return (CHAR *)"";
    }

    pstDCIHandle = (MYSQL_DCI_HANDLE_T*)&pstHandle->stDCIHandle;
    if (NULL == pstDCIHandle)
    {
        return (CHAR *)"";
    }

    if (szErrInfo)
    {
        strcpy(szErrInfo, pstDCIHandle->acErrInfo);
    }

    return pstDCIHandle->acErrInfo;
}

/**********************************************************************
函    数: GetErrCode
功    能: 获取原始错误码
输入参数:
全局变量:
返回结果:
************************************************************************/
INT32 PDBMySQLConn::GetErrCode()
{
    MYSQL_DCI_HANDLE_T  *pstDCIHandle;
    MYSQL_PDB_HANDLE_T  *pstHandle = NULL;

    pstHandle = (MYSQL_PDB_HANDLE_T*)m_pPDBHandle;
    if (NULL == pstHandle)
    {
        return -1;
    }

    pstDCIHandle = (MYSQL_DCI_HANDLE_T*)&pstHandle->stDCIHandle;
    if (NULL == pstDCIHandle)
    {
        return -1;
    }

    return pstDCIHandle->iErrCode;
}

HANDLE PDBMySQLConn::GetPDBHandle(VOID)
{
    return m_pPDBHandle;
}
