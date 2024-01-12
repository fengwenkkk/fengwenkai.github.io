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
#include "pdb_def.h"
#include "pdb_util.h"
#include "pdb_error.h"
#include "pdb_common.h"
#include "pdb_dcci.h"

/**********************************************************************
 函    数: PDBEnvBase
 功    能: 构造函数
 参数说明:
 全局变量:
 返回结果:
************************************************************************/
PDBEnvBase::PDBEnvBase()
{
    Init();
}

/**********************************************************************
 函    数: Init
 功    能: 初始化数据库环境
 参数说明:
 全局变量:
 返回结果: PDB_SUCCESS or PDB_ERROR
************************************************************************/
PDBRET PDBEnvBase::Init()
{
    UINT32  i;

    m_hEnv = NULL;
    m_acCharacterSet[0] = '\0';
    m_ulLockTimeout = LOCK_TIMEOUT;
    m_iCheckConnIndex = 0;

    m_ulConnNum = 0;
    for (i=0; i<PDB_MAX_CONNECT_NUM; i++)
    {
        m_pConn[i] = NULL;
    }

    /* 创建连接互斥锁 */
    m_hLockId = DCICreateLock();
    if (NULL == m_hLockId)
    {
        return PDB_ERROR;
    }

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: InitEnv
 功    能: 初始化数据库环境，创建数据库连接
 参数说明:
 全局变量:
 返回结果: PDB_SUCCESS or PDB_ERROR
************************************************************************/
PDBRET PDBEnvBase::InitEnv(PDB_CONN_PARAM_T *pstConnParam,
                       UINT32   ulPrefetchRow,
                       UINT32   ulTimeout,
                       UINT32   ulConnNum)
{
    UINT32          i;
    PDBRET          enRet = PDB_ERROR;
    PDB_OPTION_T    stDBOption = {0};

    m_ulDBType = pstConnParam->ulDBType;
    strcpy(m_acCharacterSet, pstConnParam->acCharacterSet);

    if (ulConnNum > PDB_MAX_CONNECT_NUM ||
        ulConnNum == 0)
    {
        return PDB_ERROR_CREATE_CONN;
    }

    stDBOption.ulTimeout = ulTimeout;
    stDBOption.ulPrefetchRow = ulPrefetchRow;

    /*初始化数据库环境*/
    if (PDB_SUCCESS != CreateEnv())
    {
        PDBLog(LOG_ERROR, "create Env failed!");
        return PDB_ERROR_CREATE_ENV;
    }

    /* 创建数据库连接 */
    for (i=0; i<ulConnNum; i++)
    {
        enRet = CreateConn();
        if (PDB_SUCCESS != enRet)
        {
            PDBLog(LOG_ERROR, "create connection failed!");
            return PDB_ERROR_CREATE_CONN;
        }

        enRet = m_pConn[i]->Connect(pstConnParam, &stDBOption);
        if (PDB_SUCCESS != enRet)
        {
            PDBLog(LOG_ERROR, "connect to db server failed");

            return PDB_ERROR_CREATE_CONN;
        }
    }

    return PDB_SUCCESS;
}

HANDLE PDBEnvBase::GetEnv(VOID)
{
    return m_hEnv;
}

/*******************************************************************************
* 函数名称: FreeConn
* 功    能: 放弃一个已分配的连接
* 函数类型: DCCI_RET_E
* 参    数:
* 参数名称        类型              输入/输出         描述
* iConnId      PDBINT32             INPUT           数据库连接ID

* 函数返回:
* 说    明:
*******************************************************************************/
PDBRET PDBEnvBase::FreeConn(INT32 iConnId)
{
    if (iConnId <= 0 || iConnId > PDB_MAX_CONNECT_NUM)
    {
        return PDB_ERROR;
    }

    if (!DCILock(m_hLockId, PDB_WAIT_FOREVER))
    {
        return PDB_ERROR;
    }

    /* ConnId从1开始 */
    if (!(m_pConn[iConnId-1]->SetAllocStatus(FALSE)))
    {
        DCIUnlock(m_hLockId);
        return PDB_ERROR_FREE_CONN;
    }

    DCIUnlock(m_hLockId);

    return PDB_SUCCESS;
}

/*******************************************************************************
* 函数名称: AllocConn
* 功    能: 在需进行数据库连接的时候调用，在维护的数据库连接vector中以获取空闲数据库连接
* 函数类型: DCCI_RET_E
* 参    数:
* 参数名称        类型              输入/输出         描述
* **ppConn      DCCIVOID             OUT           数据库连接
* *pulConnId    PDBINT32             IN/OUT        数据库连接ID(入参为-1表示待获取,出参为获取到的连接ID)
* 函数返回:
* 说    明:
*******************************************************************************/
PDBRET PDBEnvBase::AllocConn(VOID **ppConn, INT32 *piConnId)
{
    UINT32  i;
    UINT32  ulAskConnId;
    PDBConnBase *pConn;

    ulAskConnId = *piConnId;

    /* 指针有效性判断 */
    if (NULL == ppConn)
    {
        return PDB_ERROR_PARA;
    }

    if (NULL == piConnId)
    {
        return PDB_ERROR_PARA;
    }

    if (!DCILock(m_hLockId, m_ulLockTimeout))
    {
        return PDB_ERROR;
    }

    /* ConnId从1开始 */
    if (INVALID_INDEX != (*piConnId))
    {
        if (0 == *piConnId ||
            *piConnId > PDB_MAX_CONNECT_NUM)
        {
            DCIUnlock(m_hLockId);
            return PDB_ERROR;
        }

        pConn = m_pConn[(*piConnId)-1];

        if (TRUE == pConn->IsConnected() &&
            FALSE == pConn->IsAlloced())
        {
            if (pConn->SetAllocStatus(TRUE))
            {
                *ppConn = (VOID*)pConn;

                *piConnId = pConn->GetConnId();

                DCIUnlock(m_hLockId);
                return PDB_SUCCESS;
            }
        }
    }

    /* 遍历，找到一个未分配的数据库连接 */
    for (i=0; i<m_ulConnNum; i++)
    {
        if ((TRUE  == m_pConn[i]->IsConnected()) &&
            (FALSE == m_pConn[i]->IsAlloced()))
        {
            if (m_pConn[i]->SetAllocStatus(TRUE))
            {
                *piConnId = m_pConn[i]->GetConnId();
                *ppConn = (VOID*)m_pConn[i];

                DCIUnlock(m_hLockId);
                return PDB_SUCCESS;
            }
        }
    }

    PDBLog(LOG_WARN, "no free conn!");

    DCIUnlock(m_hLockId);

    return PDB_ERROR_ALLOC_CONN;
}

/**********************************************************************
 函    数: Disconnect
 功    能: 释放数据库连接
 参数说明:
 全局变量:
 返回结果: PDB_SUCCESS or PDB_ERROR
************************************************************************/
PDBRET PDBEnvBase::Disconnect(VOID *pConn)
{
    PDBRET enRet = PDB_ERROR;

    /*指针有效性判断*/
    if (NULL == pConn)
    {
        return PDB_ERROR_PARA;
    }

    enRet = ((PDBConnBase*)pConn)->Disconnect();

    return enRet;
}

/**********************************************************************
 函    数: CheckConnSet
 功    能: 检查数据库连接池
 参数说明:
 全局变量:
 返回结果: TRUE or FALSE
************************************************************************/
BOOL PDBEnvBase::CheckConnSet(VOID)
{
    if (m_ulConnNum == 0)
    {
        return TRUE;
    }

    if (!DCILock(m_hLockId, m_ulLockTimeout))
    {
        return FALSE;
    }

    if (m_iCheckConnIndex > m_ulConnNum ||
        m_iCheckConnIndex < 0)
    {
        m_iCheckConnIndex = 0;
    }

    if (NULL != m_pConn[m_iCheckConnIndex] &&
        (TRUE  == m_pConn[m_iCheckConnIndex]->IsConnected()) &&
        (FALSE == m_pConn[m_iCheckConnIndex]->IsConnInUse())) //FALSE == m_pConn[m_iCheckConnIndex]->IsAlloced()
    {
        PDBLog(LOG_INFO, "CheckConnSet begin check[%ld]", m_iCheckConnIndex+1);

        if (m_pConn[m_iCheckConnIndex]->SetConnInUseStatus(TRUE))
        {
            PDBLog(LOG_INFO, "CheckConnSet set in use status true and check[%ld]", m_iCheckConnIndex+1);

            // 加锁成功再进行连接检查
            m_pConn[m_iCheckConnIndex]->CheckConn(); //m_hLockId, m_ulLockTimeout

            m_pConn[m_iCheckConnIndex]->SetConnInUseStatus(FALSE);

            PDBLog(LOG_INFO, "CheckConnSet set in use status false[%ld]", m_iCheckConnIndex+1);
        }

        PDBLog(LOG_INFO, "CheckConnSet end check[%ld]", m_iCheckConnIndex+1);
    }

    if (NULL != m_pConn[m_iCheckConnIndex] &&
        (FALSE  == m_pConn[m_iCheckConnIndex]->IsConnected()))
    {
        if (m_pConn[m_iCheckConnIndex]->SetConnInUseStatus(TRUE))
        {
            /* 数据库服务器断过 */
         /* MYSQL_PDB_HANDLE_T  *pstHandle  = (MYSQL_PDB_HANDLE_T*)m_pConn[m_iCheckConnIndex]->GetPDBHandle();

            if (!pstHandle)
            {
                PDBLog(LOG_INFO, "CheckConnSet connect[%ld] pdb handle fail", m_iCheckConnIndex+1);
            } */

            if (PDB_SUCCESS != m_pConn[m_iCheckConnIndex]->ReConnect())
            {
                PDBLog(LOG_INFO, "CheckConnSet restore connect[%ld] fail ", m_iCheckConnIndex+1);
            }

            m_pConn[m_iCheckConnIndex]->SetConnInUseStatus(FALSE);
        }
    }

    if (0 != m_iCheckConnIndex &&
        0 == m_iCheckConnIndex % m_ulConnNum)
    {
        m_iCheckConnIndex = m_iCheckConnIndex % m_ulConnNum;
    }
    else
    {
        m_iCheckConnIndex++;
    }

    DCIUnlock(m_hLockId);

    return TRUE;
}

PDBRET PDBNoSQLEnv::CreateConn()
{
    if (m_ulConnNum >= PDB_MAX_CONNECT_NUM)
    {
        return PDB_ERROR_CREATE_CONN;
    }

    if (!DCILock(m_hLockId, PDB_WAIT_FOREVER))
    {
        return PDB_ERROR_CREATE_CONN;
    }

    m_pConn[m_ulConnNum] = new PDBNoSQLConn(this);

    /* 指针有效性判断 */
    if (NULL == m_pConn[m_ulConnNum])
    {
        DCIUnlock(m_hLockId);
        return PDB_ERROR_CREATE_CONN;
    }

    /* m_pConn的连接ID从1开始 */
    m_pConn[m_ulConnNum]->SetConnId(m_ulConnNum+1);
    m_ulConnNum++;

    DCIUnlock(m_hLockId);

    return PDB_SUCCESS;
}

PDBRET PDBNoSQLEnv::FreeConn(UINT32 ulConnID)
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

        PDBNoSQLConn   *pConn = (PDBNoSQLConn*)m_pConn[ulConnID];

        delete pConn;
        m_pConn[ulConnID] = NULL;
    }

    DCIUnlock(m_hLockId);

    return PDB_SUCCESS;
}

/**********************************************************************
 函    数: PDBNoSQLConn
 功    能: 构造函数
 参数说明:
 全局变量:
 返回结果:
************************************************************************/
PDBNoSQLConn::PDBNoSQLConn(PDBEnvBase *pEnv)
{
    m_bIsConnected = FALSE;
    m_bIsAlloced = FALSE;
    m_bIsInUse = FALSE;
    m_iConnId = 0;

    Init();
}

PDBNoSQLConn::~PDBNoSQLConn()
{
    DCIDeleteLock(m_hConnectLockId);
    DCIDeleteLock(m_hAllocLockId);
    DCIDeleteLock(m_hConnLockId);
}

PDBRET PDBNoSQLConn::Init()
{
    memset(&m_stConnParam, 0, sizeof(m_stConnParam));
    memset(&m_stDBOption, 0, sizeof(m_stDBOption));

    m_bReConnect = FALSE;

    m_hConnectLockId = NULL;
    m_hAllocLockId = NULL;
    m_hConnLockId = NULL;

    m_ulLockTimeout = LOCK_TIMEOUT;

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

PDBRET PDBNoSQLConn::Connect(PDB_CONN_PARAM_T *pstConnParam, PDB_OPTION_T *pstDBOption)
{
    if (IsConnected())
    {
        Disconnect();
    }

    if (!m_bReConnect)
    {
        SetAllocStatus(FALSE);
    }

    SetConnectStatus(TRUE);

    memcpy(&m_stConnParam, pstConnParam, sizeof(m_stConnParam));
    memcpy(&m_stDBOption, pstDBOption, sizeof(m_stDBOption));

    m_bReConnect = FALSE;

    return PDB_SUCCESS;
}

PDBRET PDBNoSQLConn::Disconnect()
{
    SetConnectStatus(FALSE);

    if (!m_bReConnect)
    {
        SetAllocStatus(FALSE);
    }

    return PDB_SUCCESS;
}

PDBRET PDBNoSQLConn::ReConnect()
{
    m_bReConnect = TRUE;

    return Connect(&m_stConnParam, &m_stDBOption);
}

BOOL PDBNoSQLConn::SetConnectStatus(BOOL bIsConnected)
{
    if (!DCILock(m_hConnectLockId, PDB_WAIT_FOREVER))
    {
        PDBLog(LOG_ERROR, "PDBNoSQLConn::SetConnectStatus lock fail ");
        return FALSE;
    }

    if (bIsConnected && m_bIsConnected)
    {
        DCIUnlock(m_hConnectLockId);
        return FALSE;
    }

    m_bIsConnected = bIsConnected;

    DCIUnlock(m_hConnectLockId);

    return TRUE;
}

BOOL PDBNoSQLConn::IsConnected()
{
    BOOL        bIsConnected;

    if (!DCILock(m_hConnectLockId, PDB_WAIT_FOREVER))
    {
        return FALSE;
    }

    bIsConnected = m_bIsConnected;

    DCIUnlock(m_hConnectLockId);

    return bIsConnected;
}

BOOL PDBNoSQLConn::IsAlloced()
{
    BOOL        bIsAlloced;

    if (!DCILock(m_hAllocLockId, m_ulLockTimeout))
    {
        return FALSE;
    }

    bIsAlloced = m_bIsAlloced;

    DCIUnlock(m_hAllocLockId);

    return bIsAlloced;
}

BOOL PDBNoSQLConn::SetAllocStatus(BOOL bIsAlloced)
{
    if (!DCILock(m_hAllocLockId, m_ulLockTimeout))
    {
        PDBLog(LOG_ERROR, "PDBNoSQLConn::SetAllocStatus lock fail ");
        return FALSE;
    }

    if (bIsAlloced && m_bIsAlloced)
    {
        DCIUnlock(m_hAllocLockId);
        return FALSE;
    }

    m_bIsAlloced = bIsAlloced;

    DCIUnlock(m_hAllocLockId);

    return TRUE;
}

BOOL PDBNoSQLConn::SetConnInUseStatus(BOOL bIsInUse)
{
    if (!DCILock(m_hConnLockId, m_ulLockTimeout))
    {
        PDBLog(LOG_ERROR, "PDBNoSQLConn::SetConnInUseStatus lock fail");
        return FALSE;
    }

    if (bIsInUse && m_bIsInUse)
    {
        DCIUnlock(m_hConnLockId);
        return FALSE;
    }

    m_bIsInUse = bIsInUse;

    DCIUnlock(m_hConnLockId);

    return TRUE;
}

BOOL PDBNoSQLConn::IsConnInUse(VOID)
{
    BOOL        bIsInUse;

    if (!DCILock(m_hConnLockId, m_ulLockTimeout))
    {
        return TRUE;
    }

    bIsInUse = m_bIsInUse;

    DCIUnlock(m_hConnLockId);

    return bIsInUse;
}

INT32 PDBNoSQLConn::GetConnId(VOID)
{
    return m_iConnId;
}

VOID PDBNoSQLConn::SetConnId(INT32 iConnId)
{
    m_iConnId = iConnId;
}
