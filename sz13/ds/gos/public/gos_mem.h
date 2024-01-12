#ifndef _GOS_MEM_H
#define _GOS_MEM_H

//#define MEM_DEBUG

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    UINT32                  ulBlockSize;    /* 内存块大小 */
    UINT32                  ulBlockNum;     /* 内存块个数 */
}GOS_MEM_POOL_T;

typedef struct
{
    UINT32                  ulPID;
    HANDLE                  hMemPool;
}GOS_MEM_POOL_LIST_T;

HANDLE  gos_mempool_init            (GOS_MEM_POOL_T *pstMemPool,
                                     UINT32 ulMemPoolNum,
                                     BOOL bCanDynamicAlloc,
                                     BOOL bUseLock);
UINT32  gos_mempool_get_alloc_size  (HANDLE hMemPool);
VOID    gos_mempool_set_dyna_alloc_size(HANDLE hMemPool, UINT32 ulDynaAllocSize);
VOID    gos_mempool_free            (HANDLE hMemPool);
VOID*   gos_mem_alloc               (HANDLE hMemPool, UINT32 ulMemSize);
VOID*   gos_mem_realloc             (HANDLE hMemPool, VOID *pMem, UINT32 ulMemSize);
VOID*   gos_mem_calloc              (HANDLE hMemPool, UINT32 ulBlockNum, UINT32 ulBlockSize);
VOID    gos_mem_free                (VOID *pAddr);
BOOL    gos_mem_get_usage           (HANDLE hMemPool, UINT32 ulBlockIndex,
                                     UINT32 *pulUsedBlockNum, UINT32 *pulMaxUsedBlockNum);

VOID    gos_mem_print               (HANDLE hMemPool, FILE *f);
VOID    gos_mem_print_usage         (HANDLE hMemPool, FILE *f, BOOL bPrintAll);

#ifndef MEM_DEBUG
#define gos_mem_alloc_ex(hMemPool, ulMemSize, szFile, ulLine)                gos_mem_alloc(hMemPool, ulMemSize)
#define gos_mem_realloc_ex(hMemPool, pMem, ulMemSize, szFile, ulLine)        gos_mem_realloc(hMemPool, pMem, ulMemSize)
#define gos_mem_calloc_ex(hMemPool, ulBlockNum, ulBlockSize, szFile, ulLine) gos_mem_calloc(hMemPool, ulBlockNum, ulBlockSize)
#define gos_mem_free_ex(pAddr, szFile, ulLine)                               gos_mem_free(pAddr)
#else
VOID*   gos_mem_alloc_ex    (HANDLE hMemPool, UINT32 ulMemSize, CHAR *szFile, UINT32 ulLine);
VOID*   gos_mem_realloc_ex  (HANDLE hMemPool, VOID *pMem, UINT32 ulMemSize, CHAR *szFile, UINT32 ulLine);
VOID*   gos_mem_calloc_ex   (HANDLE hMemPool, UINT32 ulBlockNum, UINT32 ulBlockSize, CHAR *szFile, UINT32 ulLine);
VOID    gos_mem_free_ex     (VOID *pAddr, CHAR *szFile, UINT32 ulLine);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _GOS_MEM_H */
/******************************* 头文件结束 **********************************/
