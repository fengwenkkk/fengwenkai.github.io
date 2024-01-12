#include "GPublic.h"
#include "GMsgPool.h"

static GMsgPool     *m_pInstance = NULL;

GMsgPool* GMsgPool::GetInstance()
{
    if (!m_pInstance)
    {
        m_pInstance = new GMsgPool();
        if (!m_pInstance->m_MemPool)
        {
            delete m_pInstance;
            m_pInstance = NULL;
        }
    }

    return m_pInstance;
}

GMsgPool::GMsgPool()
{
    GOS_MEM_POOL_T astMemPool[] = { {1024,      128},   // 128K
                                    {16*1024,   8},     // 128K
                                    {64*1024,   4},     // 256K
                                    {128*1024,  4}};    // 512K

    m_Mutex = new GMutex();
    m_MemPool = gos_mempool_init(astMemPool, ARRAY_SIZE(astMemPool), TRUE, TRUE);

    m_ulMsgNum = 0;
    m_u64MsgSize = 0;
}

GMsgPool::~GMsgPool()
{
    if (m_Mutex)
    {
        delete m_Mutex;
    }

    gos_mempool_free(m_MemPool);
    m_MemPool = NULL;
}

VOID GMsgPool::StatMsg(UINT32 ulMsgLen, BOOL bAlloc)
{
    if (m_Mutex->Mutex())
    {
        if (bAlloc)
        {
            m_ulMsgNum++;
            m_u64MsgSize += ulMsgLen;
        }
        else
        {
            m_ulMsgNum--;
            m_u64MsgSize -= ulMsgLen;
        }

        m_Mutex->Unmutex();
    }
}

BOOL GMsgPool::GetMsgStat(UINT32 *pulMsgNum, UINT64 *pu64MsgSize)
{
    if (!m_Mutex->Mutex())
    {
        return FALSE;
    }

    *pulMsgNum = m_ulMsgNum;
    *pu64MsgSize = m_u64MsgSize;

    m_Mutex->Unmutex();

    return TRUE;
}

MSG_T* GMsgPool::Alloc(MSG_T *pstMsgHdr, VOID *pvMsgBody, UINT32 ulMsgBodyLen)
{
    UINT32      ulMsgNodeLen = MSG_HDR_LEN + ulMsgBodyLen;
    MSG_T       *pstMsgNode;

    pstMsgNode = (MSG_T*)gos_mem_alloc(m_MemPool, ulMsgNodeLen);
    if (!pstMsgNode)
    {
        return NULL;
    }

    memcpy(pstMsgNode, pstMsgHdr, MSG_HDR_LEN);
    if (ulMsgBodyLen)
    {
        memcpy(pstMsgNode->aucMsgData, pvMsgBody, ulMsgBodyLen);
    }

    StatMsg(ulMsgNodeLen, TRUE);

    return pstMsgNode;
}

MSG_T* GMsgPool::Alloc(MSG_T *pstMsg)
{
    UINT32      ulMsgNodeLen = MSG_HDR_LEN + pstMsg->ulMsgLen;
    MSG_T       *pstMsgNode;

    pstMsgNode = (MSG_T*)gos_mem_alloc(m_MemPool, ulMsgNodeLen);
    if (!pstMsgNode)
    {
        return NULL;
    }

    memcpy(pstMsgNode, pstMsg, ulMsgNodeLen);

    StatMsg(ulMsgNodeLen, TRUE);

    return pstMsgNode;
}

VOID GMsgPool::Free(MSG_T* pstMsg)
{
    StatMsg(MSG_HDR_LEN + pstMsg->ulMsgLen, FALSE);

    gos_mem_free(pstMsg);
}

