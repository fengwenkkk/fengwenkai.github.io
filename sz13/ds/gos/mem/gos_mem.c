#include "gos_api.h"
#include "gos_func.h"
#include "gos_mem.h"

#define GOS_MEM_MAGIC       0xf1e3d5c7
typedef struct GOS_MEM_BLOCK_T
{
    UINT32                  ulMagic;        /* �ڴ��Magic Number������У���Ƿ����ڴ����ʼ��ַ */

    UINT8                   ucUsed;
    UINT8                   ucDynamicAlloc; /* ��̬�����־ */
    UINT16                  usUserID;       /* �ڴ�ʹ���߱�ţ�ͨ��Ϊ����Ż��̺߳� */

#ifdef MEM_DEBUG
    CHAR                    *szFile;
    UINT32                  ulLine;
#endif

    UINT32                  ulMemSize;      /* �����ڴ�Ĵ�С */

    struct GOS_MEM_BLOCK_T  *pstNext;

    UINT32                  ulBlockIndex;   /* �ڴ���±� */
    VOID                    *pPool;         /* ָ��GOS_MEM_POOL_INFO_T�ṹ */

    UINT8                   aucAddr[4];     /* �ڴ��ʵ��ռ�õĿռ䣬��С����4�ֽڣ�����ʵ���ڴ�صĵ�ַ */
}GOS_MEM_BLOCK_T;

typedef struct
{
    BOOL                    bUseLock;
    UINT32                  ulBlockSize;    /* �ڴ���С */
    UINT32                  ulBlockNum;     /* �ڴ����� */

    HANDLE                  hMutex;

    UINT32                  ulBlockNodeSize;/* ���нڵ��С */
    UINT8                   *pucBlockAddr;  /* �ڴ���ַ��ָ��GOS_MEM_BLOCK_T���� */

    GOS_MEM_BLOCK_T         *pstFreeHead;   /* ���ж���ͷ */

    UINT8                   *pucStartAddr;  /* ���ڴ����ʼ��ַ */
    UINT8                   *pucEndAddr;    /* ���ڴ�ؽ�����ַ */

    UINT32                  ulUsedBlockNum; /* �Ѿ�������ڴ�� */
    UINT32                  ulMaxUsedBlockNum;

}GOS_MEM_POOL_INFO_T;

#define GOS_MEM_POOL_SIZE   32              /* ����Ӧ��Ϊ64 - 8*1024*1024  */
typedef struct
{
    BOOL                    bCanDynamicAlloc;
    BOOL                    bUseLock;

    UINT32                  ulDynaAllocSize;

    UINT32                  ulMemPoolNum;
    GOS_MEM_POOL_INFO_T     astMemPool[GOS_MEM_POOL_SIZE];

    UINT32                  ulAllocMemSize;
}GOS_MEM_POOLS_T;

HANDLE gos_mem_create_lock(VOID)
{
    return gos_mutex_init();
}

VOID gos_mem_delete_lock(HANDLE hMutex)
{
    gos_mutex_free(hMutex);
}

#define gos_mempool_lock(pstPool, ulTime)   \
    ((!pstPool->bUseLock) || gos_mutex(pstPool->hMutex))

#define gos_mempool_unlock(pstPool) \
    if (pstPool->bUseLock) {gos_unmutex(pstPool->hMutex);}

/*******************************************************************************
* ��������: gos_mempool_init
* ��    ��: ��ʼ���ڴ��
* ��    ��:
* ��������          ����/���   ����
* pstMemPool        IN          �ڴ����Ϣ�б�
* pNulMemPoolNum    IN          �ڴ����Ϣ�б����
* pfGetAllocUser    IN          ��ȡ�ڴ����������ID����
* bCanDynhamicAlloc IN          ����̬�����ڴ�
* ��������: �ڴ�ع�����
* ˵    ��:
*******************************************************************************/
HANDLE gos_mempool_init(GOS_MEM_POOL_T          *pstMemPool,
                        UINT32                  ulMemPoolNum,
                        BOOL                    bCanDynamicAlloc,
                        BOOL                    bUseLock)
{
    GOS_MEM_POOLS_T         *pstMemPools = NULL;
    GOS_MEM_POOL_INFO_T     *pstPool = NULL;
    GOS_MEM_BLOCK_T         *pstNode;
    UINT32                  i, j;

    pstMemPools = (GOS_MEM_POOLS_T*)GOS_ZALLOC(sizeof(GOS_MEM_POOLS_T));
    if (NULL == pstMemPools)
    {
        goto fail;
    }

    memset(pstMemPools, 0, sizeof(GOS_MEM_POOLS_T));
    pstMemPools->bCanDynamicAlloc = bCanDynamicAlloc;
    pstMemPools->bUseLock = bUseLock;
    pstMemPools->ulDynaAllocSize = 0;

    for (i=0; i<ulMemPoolNum; i++)
    {
        if (pstMemPool->ulBlockSize == 0 ||
            pstMemPool->ulBlockNum  == 0 )
        {
            pstMemPool ++;
            continue;
        }

        if (pstMemPools->ulMemPoolNum > 0)
        {
            if (pstMemPool->ulBlockSize <= pstMemPools->astMemPool[pstMemPools->ulMemPoolNum-1].ulBlockSize)
            {
                goto fail;
            }
        }

        if (pstMemPools->ulMemPoolNum >= GOS_MEM_POOL_SIZE)
        {
            goto fail;
        }

        pstMemPools->ulMemPoolNum ++;
        pstPool = &pstMemPools->astMemPool[pstMemPools->ulMemPoolNum-1];

        pstPool->ulBlockSize = pstMemPool->ulBlockSize;
        pstPool->ulBlockNum  = pstMemPool->ulBlockNum;
        pstPool->bUseLock    = pstMemPools->bUseLock;

        // ������
        if (bUseLock)
        {
            pstPool->hMutex = gos_mem_create_lock();
            if (INVALID_MUTEX == pstPool->hMutex)
            {
                goto fail;
            }
        }
        else
        {
            pstPool->hMutex = INVALID_MUTEX;
        }

        pstPool->ulBlockNodeSize = STRUCT_OFFSET(GOS_MEM_BLOCK_T, aucAddr) + pstPool->ulBlockSize;
        pstPool->pucBlockAddr = (UINT8*)GOS_ZALLOC(pstPool->ulBlockNodeSize*pstPool->ulBlockNum);
        if (NULL == pstPool->pucBlockAddr)
        {
            goto fail;
        }

        pstMemPools->ulAllocMemSize += pstPool->ulBlockNodeSize*pstPool->ulBlockNum;

        /* ��ʼ�����ж��� */
        pstPool->pstFreeHead = (GOS_MEM_BLOCK_T *)pstPool->pucBlockAddr;

        for (j=0; j<pstPool->ulBlockNum; j++)
        {
            pstNode = (GOS_MEM_BLOCK_T*)(pstPool->pucBlockAddr + pstPool->ulBlockNodeSize*j);

            pstNode->ulMagic = GOS_MEM_MAGIC;
            pstNode->ucUsed = FALSE;
            pstNode->ucDynamicAlloc = FALSE;
//          pstNode->usUserID = 0xffff;
            pstNode->ulMemSize = 0;
            if (j<(pstPool->ulBlockNum-1))
            {
                pstNode->pstNext = (GOS_MEM_BLOCK_T*)(pstPool->pucBlockAddr + pstPool->ulBlockNodeSize*(j+1));
            }
            else
            {
                pstNode->pstNext = NULL;
            }

            pstNode->ulBlockIndex = j;
            pstNode->pPool = pstPool;
        }

        pstPool->pucStartAddr = pstPool->pucBlockAddr+(UINT32)STRUCT_OFFSET(GOS_MEM_BLOCK_T, aucAddr);
        pstPool->pucEndAddr = pstPool->pucStartAddr+pstPool->ulBlockNodeSize*pstPool->ulBlockNum;

        pstPool->ulUsedBlockNum = 0;
        pstMemPool ++;
    }

    return (HANDLE)pstMemPools;

fail:
    gos_mempool_free((HANDLE)pstMemPools);

    return NULL;
}

UINT32 gos_mempool_get_alloc_size(HANDLE hMemPool)
{
    GOS_MEM_POOLS_T     *pstMemPools = (GOS_MEM_POOLS_T*)hMemPool;

    if (!pstMemPools)
    {
        return 0;
    }

    return pstMemPools->ulAllocMemSize;
}

VOID gos_mempool_set_dyna_alloc_size(HANDLE hMemPool, UINT32 ulDynaAllocSize)
{
    GOS_MEM_POOLS_T         *pstMemPools = (GOS_MEM_POOLS_T*)hMemPool;

    if (pstMemPools)
    {
        pstMemPools->ulDynaAllocSize = ulDynaAllocSize;
    }
}

VOID gos_mempool_free(HANDLE hMemPool)
{
    GOS_MEM_POOLS_T     *pstMemPools = (GOS_MEM_POOLS_T*)hMemPool;
    GOS_MEM_POOL_INFO_T *pstMemPool;
    UINT32              i;

    if (!pstMemPools)
    {
        return;
    }

    for (i=0; i<pstMemPools->ulMemPoolNum; i++)
    {
        pstMemPool = &pstMemPools->astMemPool[i];
        if (pstMemPool->pucBlockAddr)
        {
            gos_mutex_free(pstMemPool->hMutex);
            free(pstMemPool->pucBlockAddr);
        }
    }

    free(pstMemPools);
}

/*******************************************************************************
* ��������: gos_mem_get_usage
* ��    ��: ��ȡ�ڴ�ʹ�����
* ��    ��:
* ��������       ����/���       ����
* hMemPool       IN              �ڴ�ؾ��
* ��������:
* ˵    ��:
*******************************************************************************/
BOOL gos_mem_get_usage(HANDLE hMemPool, UINT32 ulBlockIndex,
                       UINT32 *pulUsedBlockNum, UINT32 *pulMaxUsedBlockNum)
{
    GOS_MEM_POOLS_T         *pstMemPools = (GOS_MEM_POOLS_T*)hMemPool;
    GOS_MEM_POOL_INFO_T     *pstPool;

    if (NULL == pstMemPools || ulBlockIndex >= pstMemPools->ulMemPoolNum)
    {
        return FALSE;
    }

    pstPool = &pstMemPools->astMemPool[ulBlockIndex];

    *pulUsedBlockNum = pstPool->ulUsedBlockNum;
    *pulMaxUsedBlockNum = pstPool->ulMaxUsedBlockNum;

    return TRUE;
}

/*******************************************************************************
* ��������: gos_mem_print
* ��    ��: ��ӡ�ڴ�ʹ�����
* ��    ��:
* ��������       ����/���       ����
* hMemPool       IN              �ڴ�ؾ��
* ��������:
* ˵    ��:
*******************************************************************************/
VOID gos_mem_print(HANDLE hMemPool, FILE *fp)
{
#ifdef MEM_DEBUG
    GOS_MEM_POOLS_T         *pstMemPools = (GOS_MEM_POOLS_T*)hMemPool;
    GOS_MEM_POOL_INFO_T     *pstPool;
    UINT32                  i;

    if (!fp)
    {
        fp = stdout;
    }

    for (i=0; i<GOS_MEM_POOL_SIZE; i++)
    {
        pstPool = &pstMemPools->astMemPool[i];
        if (pstPool->ulBlockSize == 0)
        {
            continue;
        }

        fprintf(fp, "\n mem pool: size = %u num = %u used = %u max used = %u",
                pstPool->ulBlockSize,
                pstPool->ulBlockNum,
                pstPool->ulUsedBlockNum,
                pstPool->ulMaxUsedBlockNum);

    }
#endif
}

/*******************************************************************************
* ��������: gos_mem_alloc
* ��    ��: �����ڴ��
* ��    ��:
* ��������       ����/���       ����
* hMemPool       IN              �ڴ�ؾ��
* ulMemSize      IN              ������ڴ��С
* ��������: �ڴ��ַ
* ˵    ��:
*******************************************************************************/
#ifdef MEM_DEBUG
VOID* gos_mem_alloc(HANDLE hMemPool, UINT32 ulMemSize)
{
    return gos_mem_alloc_ex(hMemPool, ulMemSize, NULL, 0);
}

VOID* gos_mem_alloc_ex(HANDLE hMemPool, UINT32 ulMemSize, CHAR *szFile, UINT32 ulLine)
#else
VOID* gos_mem_alloc(HANDLE hMemPool, UINT32 ulMemSize)
#endif
{
    GOS_MEM_POOLS_T         *pstMemPools = (GOS_MEM_POOLS_T*)hMemPool;
    GOS_MEM_POOL_INFO_T     *pstPool;
    GOS_MEM_BLOCK_T         *pstNode;
    VOID                    *pAddr;
    UINT32                  i;

    if (!pstMemPools)
    {
        return NULL;
    }

    if (ulMemSize <= pstMemPools->ulDynaAllocSize)
    {
        goto dyna_alloc;
    }

    /* ������̬�ڴ�� */
    for (i=0; i<pstMemPools->ulMemPoolNum; i++)
    {
        pstPool = &pstMemPools->astMemPool[i];
        if (pstPool->ulBlockSize < ulMemSize)
        {
            continue;
        }

        if (!gos_mempool_lock(pstPool, 1000))
        {
            continue;
        }

        /* �ж��Ƿ���ڿ����ڴ�� */
        pstNode = pstPool->pstFreeHead;
        if (pstNode == NULL)
        {
            gos_mempool_unlock(pstPool);
            continue;
        }

        pstPool->pstFreeHead = pstNode->pstNext;
        pstPool->ulUsedBlockNum ++;
        if (pstPool->ulMaxUsedBlockNum < pstPool->ulUsedBlockNum)
        {
            pstPool->ulMaxUsedBlockNum = pstPool->ulUsedBlockNum;
        }

        pstNode->ucUsed = TRUE;
        pstNode->ulMemSize = ulMemSize;
#ifdef MEM_DEBUG
        pstNode->szFile = szFile;
        pstNode->ulLine = ulLine;
#endif
        pAddr = pstNode->aucAddr;

        gos_mempool_unlock(pstPool);

        return pAddr;
    }

    /* ��̬���� */
dyna_alloc:
    if (pstMemPools->bCanDynamicAlloc)
    {
        pstNode = (GOS_MEM_BLOCK_T*)malloc(ulMemSize + STRUCT_OFFSET(GOS_MEM_BLOCK_T, aucAddr));
        if (NULL == pstNode)
        {
            return NULL;
        }

        pstNode->ulMagic = GOS_MEM_MAGIC;
        pstNode->ucUsed = TRUE;
        pstNode->ucDynamicAlloc = TRUE;
        pstNode->ulMemSize = ulMemSize;
        pstNode->pstNext = NULL;
        pstNode->ulBlockIndex = 0xffffffff;
        pstNode->pPool = NULL;
#ifdef MEM_DEBUG
        pstNode->szFile = szFile;
        pstNode->ulLine = ulLine;
#endif
        return pstNode->aucAddr;
    }

    return NULL;
}

#ifdef MEM_DEBUG
VOID* gos_mem_calloc(HANDLE hMemPool, UINT32 ulBlockNum, UINT32 ulBlockSize)
{
    return gos_mem_calloc_ex(hMemPool, ulBlockNum, ulBlockSize, NULL, 0);
}

VOID* gos_mem_calloc_ex(HANDLE hMemPool, UINT32 ulBlockNum, UINT32 ulBlockSize, CHAR *szFile, UINT32 ulLine)
#else
VOID* gos_mem_calloc(HANDLE hMemPool, UINT32 ulBlockNum, UINT32 ulBlockSize)
#endif
{
    VOID    *pMem;
    UINT64  ullMemSize = ulBlockNum*ulBlockSize;

    if (ullMemSize > 0xffffffff)
    {
        return NULL;
    }

#ifdef MEM_DEBUG
    pMem = gos_mem_alloc_ex(hMemPool, (UINT32)ullMemSize, szFile, ulLine);
#else
    pMem = gos_mem_alloc(hMemPool, (UINT32)ullMemSize);
#endif
    if (pMem)
    {
        memset(pMem, 0, (UINT32)ullMemSize);
    }

    return pMem;
}

VOID* gos_mem_zalloc(HANDLE hMemPool, UINT32 ulSize)
{
#ifdef MEM_DEBUG
    return gos_mem_calloc_ex(hMemPool, 1, ulSize, NULL, 0);
#else
    return gos_mem_calloc(hMemPool, 1, ulSize);
#endif
}

/*******************************************************************************
* ��������: gos_mem_realloc
* ��    ��: ���������ڴ��
* ��    ��:
* ��������       ����/���       ����
* hMemPool       IN              �ڴ�ؾ��
* ulMemSize      IN              ������ڴ��С
* ��������: �ڴ��ַ
* ˵    ��:
*******************************************************************************/
#ifdef MEM_DEBUG
VOID* gos_mem_realloc(HANDLE hMemPool, VOID *pMem, UINT32 ulMemSize)
{
    return gos_mem_realloc_ex(hMemPool, pMem, ulMemSize, NULL, 0);
}

VOID* gos_mem_realloc_ex(HANDLE hMemPool, VOID *pMem, UINT32 ulMemSize, CHAR *szFile, UINT32 ulLine)
#else
VOID* gos_mem_realloc(HANDLE hMemPool, VOID *pMem, UINT32 ulMemSize)
#endif
{
    GOS_MEM_POOL_INFO_T     *pstPool;
    GOS_MEM_BLOCK_T         *pstNode;
    VOID                    *pNewMem;

    if (pMem == NULL)
    {
#ifdef MEM_DEBUG
        return gos_mem_alloc_ex(hMemPool, ulMemSize, szFile, ulLine);
#else
        return gos_mem_alloc(hMemPool, ulMemSize);
#endif
    }

    pstNode = (GOS_MEM_BLOCK_T*)((UINT8*)pMem - (UINT32)STRUCT_OFFSET(GOS_MEM_BLOCK_T, aucAddr));
    if (pstNode->ulMagic != GOS_MEM_MAGIC)
    {
        return pMem;
    }

    if (pstNode->ulMemSize >= ulMemSize)
    {
        return pMem;
    }

    pstPool = (GOS_MEM_POOL_INFO_T*)pstNode->pPool;
    if (pstPool)
    {
        // �쳣
        if (pstNode->ulMemSize > pstPool->ulBlockSize)
        {
            return pMem;
        }

        // ��Ҫ��������
        if (ulMemSize <= pstPool->ulBlockSize)
        {
            pstNode->ulMemSize = ulMemSize;
            return pMem;
        }
    }

#ifdef MEM_DEBUG
    pNewMem = gos_mem_alloc_ex(hMemPool, ulMemSize, szFile, ulLine);
#else
    pNewMem = gos_mem_alloc(hMemPool, ulMemSize);
#endif
    if (!pNewMem)
    {
        return pMem;
    }

    memcpy(pNewMem, pMem, pstNode->ulMemSize);
    gos_mem_free(pMem);

    return pNewMem;
}

/*******************************************************************************
* ��������: gos_mem_free
* ��    ��: �ͷ��ڴ��
* ��    ��:
* ��������       ����/���       ����
* hMemPool       IN              �ڴ�ؾ��
* pAddr          IN              ��Ҫ�ͷŵ��ڴ��ַ
* ��������: �ڴ��ַ
* ˵    ��:
*******************************************************************************/
#ifdef MEM_DEBUG
VOID gos_mem_free(VOID *pAddr)
{
    gos_mem_free_ex(pAddr, NULL, 0);
}

VOID gos_mem_free_ex(VOID *pAddr, CHAR *szFile, UINT32 ulLine)
#else
VOID gos_mem_free(VOID *pAddr)
#endif
{
    GOS_MEM_POOL_INFO_T     *pstPool;
    GOS_MEM_BLOCK_T         *pstNode;
    UINT8                   *pucAddr = (UINT8*)pAddr;

    if (NULL == pucAddr)
    {
        return;
    }

    pstNode = (GOS_MEM_BLOCK_T*)(pucAddr - (UINT32)STRUCT_OFFSET(GOS_MEM_BLOCK_T, aucAddr));
    if (pstNode->ulMagic != GOS_MEM_MAGIC)
    {
        return;
    }

    if (pstNode->ucDynamicAlloc)
    {
        free(pstNode);
        return;
    }

    pstPool = (GOS_MEM_POOL_INFO_T*)pstNode->pPool;
    if (NULL == pstPool)
    {
        return;
    }

    if (pucAddr != pstPool->pucStartAddr + pstNode->ulBlockIndex*pstPool->ulBlockNodeSize)
    {
        return;
    }

    /* ���Լ��� */
    if (!gos_mempool_lock(pstPool, WAIT_FOREVER))
    {
        return;
    }

    pstNode->usUserID = 0xffff;

    if (pstPool->pstFreeHead)
    {
        pstNode->pstNext = pstPool->pstFreeHead;
    }
    else
    {
        pstNode->pstNext = NULL;
    }

    pstPool->pstFreeHead = pstNode;
    pstPool->ulUsedBlockNum --;

    pstNode->ucUsed = FALSE;

    gos_mempool_unlock(pstPool);

    return;
}

/*******************************************************************************
* ��������: gos_mem_print_usage
* ��    ��: ��ӡ�ڴ�ʹ�����
* ��    ��:
* ��������       ����/���       ����
* hMemPool       IN              �ڴ�ؾ��
* bPrintAll      IN              �Ƿ�ȫ����ӡ
* ��������: �ڴ��ַ
* ˵    ��:
*******************************************************************************/
VOID gos_mem_print_usage(HANDLE hMemPool, FILE *f, BOOL bPrintAll)
{
    GOS_MEM_POOLS_T         *pstMemPools = (GOS_MEM_POOLS_T*)hMemPool;
    GOS_MEM_POOL_INFO_T     *pstPool;
    UINT32                  i;

    if (!hMemPool)
    {
        return;
    }

    if (NULL == f)
    {
        f = stdout;
    }

    /* ������̬�ڴ�� */
    for (i=0; i<pstMemPools->ulMemPoolNum; i++)
    {
        pstPool = &pstMemPools->astMemPool[i];

        if (!gos_mempool_lock(pstPool, WAIT_FOREVER))
        {
            continue;
        }

        fprintf(f, "\n gos_mem_usage of %u: total=%u, used=%u", pstPool->ulBlockSize, pstPool->ulBlockNum, pstPool->ulUsedBlockNum);

#ifdef MEM_DEBUG
        for (j=0; j<pstPool->ulBlockNum; j++)
        {
            pstNode = (GOS_MEM_BLOCK_T*)(pstPool->pucBlockAddr + pstPool->ulBlockNodeSize*j);

            if (!pstNode->ucUsed)
            {
                continue;
            }

            if (pstNode->szFile)
            {
                fprintf(f, "\n %u line = %8u file = %s", pstNode->ulMemSize, pstNode->ulLine, pstNode->szFile);
            }
            else if (bPrintAll)
            {
                fprintf(f, "\n %u line = %8u", pstNode->ulMemSize, pstNode->ulLine);
            }
        }
#endif

        gos_mempool_unlock(pstPool);
    }

    fprintf(f, "\n");
}

#ifdef MEM_TEST

GOS_MEM_POOL_T  g_astMemPool[] = {
    { 64,               1024*48},       /* pool(0):  3M      */
    { 128,              1024*48},       /* pool(1):  6M      */
    { 256,              1024*32},       /* pool(2):  8M      */
    { 512,              1024*16},       /* pool(3):  8M      */
    { 1024,             1024*8 },       /* pool(4):  8M      */
    { 2*1024,           1024*4 },       /* pool(5):  8M      */
    { 4*1024,           1024*2 },       /* pool(6):  8M      */
    { 8*1024,           1024   },       /* pool(7):  8M      */
    { 16*1024,          512    },       /* pool(8):  8M      */
    { 32*1024,          256    },       /* pool(9):  8M      */
    { 64*1024,          128    },       /* pool(10): 8M      */
    { 128*1024,         32     },       /* pool(11): 4M      */
    { 256*1024,         16     },       /* pool(12): 4M      */
    { 512*1024,         8      },       /* pool(13): 4M      */
    { 1024*1024,        4      },       /* pool(14): 4M      */
    { 2048*1024,        2      },       /* pool(15): 4M      */
    { 4096*1024,        2      },       /* pool(16): 8M      */
};

#include <math.h>
BOOL gos_mem_test(VOID)
{
    HANDLE          hMemPool;
    UINT32          i, j;
    UINT32          ulTestNum = 1500*1024;
    INT32           *apucTmp;
    UINT32          aulBlockNum[32];
    UINT32          *aulNum;
    UINT32          ulMemSize = 2;
    UINT32          ulPrime = gos_get_prime(1024*1024*1024);
    UINT32          ulMaxPower = 2+10+10;
    FLOAT           fTmp;
    UINT32          ulCurrPrime = 1;
    UINT32          ulStartTick;
    UINT32          ulEndTick;

    apucTmp = malloc(ulTestNum*sizeof(INT32*));
    aulNum = malloc(ulTestNum*sizeof(UINT32));

    memset(apucTmp, 0, ulTestNum*sizeof(INT32*));
    memset(aulNum, 0, ulTestNum*sizeof(UINT32));

    memset(aulBlockNum, 0, sizeof(aulBlockNum));

    ulStartTick = gos_get_uptime_1ms();

    /* ���Ҫ����ϵͳ��malloc���ܣ�����Ҫ�����溯�������һ�������ĳ�TRUE��ͬʱ��ע��
       gos_mem_alloc�еĵ�һ��ѭ��(������̬�ڴ��) */
    hMemPool = gos_mempool_init(g_astMemPool, ARRAY_SIZE(g_astMemPool), TRUE, FALSE);
    if (NULL == hMemPool)
    {
        return FALSE;
    }

    for (i=0; i<ulTestNum; i++)
    {
        ulCurrPrime = ((ulCurrPrime*3) % ulPrime);

        fTmp = (FLOAT)0.5+ulMaxPower*(ulCurrPrime % 1024) / (FLOAT)1024;
        ulMemSize = 1 << ((UINT32)fTmp);

        for (j=ARRAY_SIZE(g_astMemPool)-1; j>=0; j--)
        {
            if (ulMemSize < g_astMemPool[j].ulBlockSize)
            {
                continue;
            }

            if (aulBlockNum[j] < g_astMemPool[j].ulBlockNum)
            {
                apucTmp[i] = (INT32)gos_mem_alloc(hMemPool, ulMemSize);
                aulBlockNum[j] ++;
            }

            break;
        }
    }

    gos_mem_print(hMemPool, NULL);

    for (i=0; i<ulTestNum; i++)
    {
        gos_mem_free((VOID*)apucTmp[i]);
    }

    ulEndTick = gos_get_uptime_1ms();

    printf("\n time = %d ", (INT32)(ulEndTick - ulStartTick));

    gos_mem_print(hMemPool, NULL);

    return TRUE;
}

BOOL gos_mem_test1(VOID)
{
    HANDLE          hMemPool;
    UINT32          ulTestNum = 1500*1024;
    INT32           *apucTmp;
    UINT32          aulBlockNum[32];
    UINT32          *aulNum;
    UINT32          ulMemSize = 2;
    UINT32          ulPrime = gos_get_prime(1024*1024*1024);
    UINT32          ulMaxPower = 2+10+10;
    UINT32          ulCurrPrime = 1;
    UINT32          ulStartTick;
    UINT32          ulEndTick;

    apucTmp = malloc(ulTestNum*sizeof(INT32*));
    aulNum = malloc(ulTestNum*sizeof(UINT32));

    memset(apucTmp, 0, ulTestNum*sizeof(INT32*));
    memset(aulNum, 0, ulTestNum*sizeof(UINT32));

    memset(aulBlockNum, 0, sizeof(aulBlockNum));

    ulStartTick = gos_get_uptime_1ms();

    /* ���Ҫ����ϵͳ��malloc���ܣ�����Ҫ�����溯�������һ�������ĳ�TRUE��ͬʱ��ע��
       gos_mem_alloc�еĵ�һ��ѭ��(������̬�ڴ��) */
    hMemPool = gos_mempool_init(g_astMemPool, ARRAY_SIZE(g_astMemPool),  TRUE, FALSE);
    if (NULL == hMemPool)
    {
        return FALSE;
    }

    apucTmp[0] = (INT32)gos_mem_alloc(hMemPool, 1500);
    apucTmp[1] = (INT32)gos_mem_alloc(hMemPool, 5000);

    sprintf((char*)apucTmp[0], "test1");
    sprintf((char*)apucTmp[1], "test2");

    gos_mem_print(hMemPool, NULL);

    apucTmp[0] = (INT32)gos_mem_realloc(hMemPool, (VOID*)apucTmp[0], 1700);
    apucTmp[1] = (INT32)gos_mem_realloc(hMemPool, (VOID*)apucTmp[1], 10000);

    gos_mem_print(hMemPool, NULL);

    gos_mem_free((VOID*)apucTmp[0]);
    gos_mem_free((VOID*)apucTmp[1]);

    ulEndTick = gos_get_uptime_1ms();

    printf("\n time = %d ", (INT32)(ulEndTick - ulStartTick));

    gos_mem_print(hMemPool, NULL);

    return TRUE;
}

int main()
{
    gos_mem_test1();

    return 0;
}

#endif
