#include "g_include.h"
#include "g_mem.h"

GMemPool* GMemPool::GetInstance(GOS_MEM_POOL_T *pstMemPool, UINT32 ulMemPoolNum, BOOL bCanDynamicAlloc, BOOL bUseLock)
{
    GMemPool *pInstance = NULL;

    pInstance = new GMemPool();
    if (!pInstance)
    {
        return NULL;
    }

    if (!pInstance->Init(pstMemPool, ulMemPoolNum, bCanDynamicAlloc, bUseLock))
    {
        delete pInstance;
        return NULL;
    }

    return pInstance;
}

VOID* GMemPool::MemAlloc(UINT32 ulSize)
{
    return gos_mem_alloc(m_hPool, ulSize);
}

VOID GMemPool::MemFree(VOID* ptr)
{
    gos_mem_free(ptr);
}

BOOL GMemPool::Init(GOS_MEM_POOL_T *pstMemPool, UINT32 ulMemPoolNum, BOOL bCanDynamicAlloc, BOOL bUseLock)
{
    m_hPool = gos_mempool_init(pstMemPool, ulMemPoolNum, bCanDynamicAlloc, bUseLock);
    if (!m_hPool)
    {
        return FALSE;
    }

    return TRUE;
}

VOID GMemPool::Free()
{
    gos_mempool_free(m_hPool);
}

