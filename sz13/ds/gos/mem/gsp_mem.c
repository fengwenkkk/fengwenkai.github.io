#include "gos_api.h"
#include "gos_func.h"
#include "gos_mem.h"
#include "gos_time.h"
#include "gsp_mem.h"

static GOS_MEM_POOL_T   g_astGspMemPool[] = {
#if 0
    { 8,                1024*256},      // 128K
    { 16,               1024*256},      // 256K
    { 64,               1024*256},      // 1M
    { 128,              1024*256},      // 1M
    { 256,              1024*256},      // 1M
    { 512,              1024*256},      // 1M
    { 1024,              1024*256},     // 1M
#else
    { 8,                1024*16},       // 128K
    { 16,               1024*16},       // 256K
    { 64,               1024*16},       // 1M
    { 128,              1024*5},        // 1M
    { 256,              1024*4},        // 1M
    { 512,              1024*2},        // 64M
    { 1024,             1024*2},        // 2M
    { 2048,             1024},          // 2M
    { 8192,             256},           // 2M
#endif
};

#define INVALID_TID         0

typedef struct
{
    UINT32              ulTID;
    UINT32              ulDynaAllocSize;
    HANDLE              hMemPool;

    UINT32              ulAllocNoDebugTime;
    UINT32              ulFreeNoDebugTime;
    UINT32              ulAllocTime;
    UINT32              ulReallocTime;
    UINT32              ulZallocTime;
    UINT32              ulFreeTime;
    UINT32              ulNewTime;
    UINT32              ulNew0Time;
}GSP_MEM_POOL_LIST_T;

static GSP_MEM_POOL_LIST_T  g_astGspMemPoolList[32] = {0};

HANDLE gsp_get_mem_pool(VOID)
{
    UINT32  ulTID = gos_get_thread_id();
    UINT32  i;
    HANDLE  hMemPool = NULL;

    for (i=0; i<ARRAY_SIZE(g_astGspMemPoolList); i++)
    {
        if (g_astGspMemPoolList[i].ulTID == INVALID_TID)
        {
            g_astGspMemPoolList[i].ulTID = ulTID;
        }

        if (g_astGspMemPoolList[i].ulTID == ulTID)
        {
            if (g_astGspMemPoolList[i].hMemPool)
            {
                return g_astGspMemPoolList[i].hMemPool;
            }
            else
            {
                g_astGspMemPoolList[i].hMemPool = gos_mempool_init(g_astGspMemPool, ARRAY_SIZE(g_astGspMemPool), TRUE, TRUE);
                if (!g_astGspMemPoolList[i].hMemPool)
                {
                    printf("\n init engine mem pool failed, %u \n", ulTID);
                    return NULL;
                }

                gos_mempool_set_dyna_alloc_size(g_astGspMemPoolList[i].hMemPool, g_astGspMemPoolList[i].ulDynaAllocSize);
                hMemPool = g_astGspMemPoolList[i].hMemPool;

                printf("\n init engine mem pool ok, %u %08X \n", ulTID, hMemPool);
                return hMemPool;
            }
        }
    }

    return hMemPool;
}

DLL_EXPORT VOID gsp_mem_set_dyna_alloc_size(UINT32 ulDynaAllocSize)
{
    UINT32      i;

    for (i=0; i<ARRAY_SIZE(g_astGspMemPoolList); i++)
    {
        if (g_astGspMemPoolList[i].ulTID == INVALID_TID)
        {
            g_astGspMemPoolList[i].ulDynaAllocSize = ulDynaAllocSize;
        }

        if (g_astGspMemPoolList[i].hMemPool)
        {
            gos_mempool_set_dyna_alloc_size(g_astGspMemPoolList[i].hMemPool, ulDynaAllocSize);
        }
    }
}

DLL_EXPORT VOID gsp_mem_print_usage(VOID *f)
{
    UINT32      i;
    FILE        *fp = f;

    if (!fp)
    {
        fp = stdout;
    }

    for (i=0; i<ARRAY_SIZE(g_astGspMemPoolList); i++)
    {
        if (g_astGspMemPoolList[i].ulTID == INVALID_TID)
        {
            break;
        }

        gos_mem_print(g_astGspMemPoolList[i].hMemPool, NULL);
    }

    fprintf(fp, "\n");
}

DLL_EXPORT VOID *gsp_mem_alloc_ex(UINT32 ulSize, CHAR *szFile, UINT32 ulLine)
{
    VOID        *pMem;
    HANDLE      hMemPool = gsp_get_mem_pool();

    if (!hMemPool)
    {
        return NULL;
    }

    pMem = gos_mem_alloc_ex(hMemPool, ulSize, szFile, ulLine);

    return pMem;
}

DLL_EXPORT VOID *gsp_mem_alloc(UINT32 ulSize)
{
    UINT32  ulTID = gos_get_thread_id();
    UINT32  i;
    HANDLE  hMemPool = NULL;

    for (i=0; i<ARRAY_SIZE(g_astGspMemPoolList); i++)
    {
        if (g_astGspMemPoolList[i].ulTID == ulTID)
        {
            if (g_astGspMemPoolList[i].hMemPool)
            {
                return gos_mem_alloc_ex(g_astGspMemPoolList[i].hMemPool, ulSize, NULL, 0);
            }
            else
            {
start:
                g_astGspMemPoolList[i].hMemPool = gos_mempool_init(g_astGspMemPool, ARRAY_SIZE(g_astGspMemPool), TRUE, TRUE);
                if (!g_astGspMemPoolList[i].hMemPool)
                {
                    return NULL;
                }

                gos_mempool_set_dyna_alloc_size(g_astGspMemPoolList[i].hMemPool, g_astGspMemPoolList[i].ulDynaAllocSize);
                return gos_mem_alloc_ex(g_astGspMemPoolList[i].hMemPool, ulSize, NULL, 0);
            }
        }
        else if (g_astGspMemPoolList[i].ulTID == INVALID_TID)
        {
            g_astGspMemPoolList[i].ulTID = ulTID;
            goto start;
        }
    }

    return NULL;
}

DLL_EXPORT VOID gsp_mem_free_ex(VOID *pMem, CHAR *szFile, UINT32 ulLine)
{
    gos_mem_free_ex(pMem, szFile, ulLine);
}

DLL_EXPORT VOID *gsp_mem_zalloc_ex(UINT32 ulSize, CHAR *szFile, UINT32 ulLine)
{
    HANDLE      hMemPool = gsp_get_mem_pool();

    if (!hMemPool)
    {
        return NULL;
    }

    return gos_mem_calloc_ex(hMemPool, 1, ulSize, szFile, ulLine);
}

DLL_EXPORT VOID *gsp_mem_zalloc(UINT32 ulBlock, UINT32 ulSize)
{
    UINT32  ulTID = gos_get_thread_id();
    UINT32  i;
    HANDLE  hMemPool = NULL;

    for (i=0; i<ARRAY_SIZE(g_astGspMemPoolList); i++)
    {
        if (g_astGspMemPoolList[i].ulTID == ulTID)
        {
            if (g_astGspMemPoolList[i].hMemPool)
            {
                return gos_mem_calloc_ex(g_astGspMemPoolList[i].hMemPool, ulBlock, ulSize, NULL, 0);
            }
            else
            {
start:
                g_astGspMemPoolList[i].hMemPool = gos_mempool_init(g_astGspMemPool, ARRAY_SIZE(g_astGspMemPool), TRUE, TRUE);
                if (!g_astGspMemPoolList[i].hMemPool)
                {
                    return NULL;
                }

                gos_mempool_set_dyna_alloc_size(g_astGspMemPoolList[i].hMemPool, g_astGspMemPoolList[i].ulDynaAllocSize);
                return gos_mem_calloc_ex(g_astGspMemPoolList[i].hMemPool, ulBlock, ulSize, NULL, 0);
            }
        }
        else if (g_astGspMemPoolList[i].ulTID == INVALID_TID)
        {
            g_astGspMemPoolList[i].ulTID = ulTID;
            goto start;
        }
    }

    return NULL;
}

DLL_EXPORT VOID *gsp_mem_realloc_ex(VOID *pMem, UINT32 ulSize, CHAR *szFile, UINT32 ulLine)
{
    HANDLE      hMemPool = gsp_get_mem_pool();

    if (!hMemPool)
    {
        return NULL;
    }

    return gos_mem_realloc_ex(hMemPool, pMem, ulSize, szFile, ulLine);
}

DLL_EXPORT VOID *gsp_mem_realloc(VOID *pMem, UINT32 ulSize)
{
    UINT32  ulTID = gos_get_thread_id();
    UINT32  i;
    HANDLE  hMemPool = NULL;

    for (i=0; i<ARRAY_SIZE(g_astGspMemPoolList); i++)
    {
        if (g_astGspMemPoolList[i].ulTID == ulTID)
        {
            if (g_astGspMemPoolList[i].hMemPool)
            {
                return gos_mem_realloc_ex(g_astGspMemPoolList[i].hMemPool, pMem, ulSize, NULL, 0);
            }
            else
            {
start:
                g_astGspMemPoolList[i].hMemPool = gos_mempool_init(g_astGspMemPool, ARRAY_SIZE(g_astGspMemPool), TRUE, TRUE);
                if (!g_astGspMemPoolList[i].hMemPool)
                {
                    return pMem;
                }

                gos_mempool_set_dyna_alloc_size(g_astGspMemPoolList[i].hMemPool, g_astGspMemPoolList[i].ulDynaAllocSize);
                return gos_mem_realloc_ex(g_astGspMemPoolList[i].hMemPool, pMem, ulSize, NULL, 0);
            }
        }
        else if (g_astGspMemPoolList[i].ulTID == INVALID_TID)
        {
            g_astGspMemPoolList[i].ulTID = ulTID;
            goto start;
        }
    }

    return pMem;
}

DLL_EXPORT VOID *gsp_mem_new(UINT32 ulBlock, UINT32 ulSize, CHAR *szFile, UINT32 ulLine)
{
    VOID        *pMem;
    HANDLE      hMemPool;

    if (ulBlock > 1 && ulBlock > 0xffffffff/ulSize)
    {
        return NULL;
    }

    hMemPool = gsp_get_mem_pool();
    if (!hMemPool)
    {
        return NULL;
    }

    pMem = gos_mem_alloc_ex(hMemPool, ulBlock*ulSize, szFile, ulLine);

    return pMem;
}

DLL_EXPORT VOID *gsp_mem_new_0(UINT32 ulBlock, UINT32 ulSize, CHAR *szFile, UINT32 ulLine)
{
    VOID        *pMem;
    HANDLE      hMemPool = gsp_get_mem_pool();

    if (!hMemPool)
    {
        return NULL;
    }

    pMem = gos_mem_calloc_ex(hMemPool, ulBlock, ulSize, szFile, ulLine);
    return pMem;
}

DLL_EXPORT VOID *gsp_mem_alloc_nodebug(UINT32 ulSize)
{
    VOID        *pMem;
    HANDLE      hMemPool = gsp_get_mem_pool();

    if (!hMemPool)
    {
        return NULL;
    }


    pMem = gos_mem_alloc_ex(hMemPool, ulSize, NULL, 0);
    return pMem;
}

DLL_EXPORT VOID gsp_mem_free_nodebug(VOID *pMem)
{
    gos_mem_free_ex(pMem, NULL, 0);
}
