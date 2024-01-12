/******************************* 包含文件声明 ********************************/
#include "gos_api.h"
#include "gos_func.h"
#include "gos_hash.h"
#include "gos_hashlist.h"

typedef struct GOS_HASH_NODE_T
{
    struct GOS_HASH_NODE_T      *pstPrev;
    struct GOS_HASH_NODE_T      *pstNext;

    VOID                        *pData;
    UINT32                      ulCurrDataLen;
    UINT32                      ulMaxDataLen;

    UINT32                      ulHashValue;

    struct GOS_HASH_NODE_T      *pstPrevLink;
    struct GOS_HASH_NODE_T      *pstNextLink;
}GOS_HASH_NODE_T;

typedef struct
{
    GOS_HASH_NODE_T             *pstTail;
    GOS_HASH_NODE_T             *pstHead;
}GOS_HASH_DUMMY_NODE_T;

typedef struct GOS_HASH_ENTRY_T
{
    GOS_HASH_DUMMY_NODE_T       stNodeDummy;        /* 节点队列哑元 */
}GOS_HASH_ENTRY_T;

typedef struct
{
    GOS_HASH_NODE_T             *pstTail;
    GOS_HASH_NODE_T             *pstHead;
}GOS_HASH_LIST_DUMMY_NODE_T;

typedef struct
{
    UINT32                      ulHashEntrySize;    /* hash桶大小 */
    GOS_HASH_NODE_T             stFreeDummy;        /* 静态队列哑元 */
    GOS_HASH_NODE_T             *pstStaticNodeStart;
    GOS_HASH_NODE_T             *pstStaticNodeEnd;

    GOS_HASH_ENTRY_T            *pstHashEntry;      /* hash桶，必须是连续地址 */

    GOS_HASH_LIST_DUMMY_NODE_T  stListDummy;

    UINT32                      ulCurrNodeNum;      /* 当前节点数 */
    UINT32                      ulMinNodeNum;       /* 允许的最小节点数（静态节点数） */
    UINT32                      ulMaxNodeNum;       /* 允许的最大节点数 */
    UINT32                      ulStaticNodeDataLen;/* 静态节点数据长度 */

    UINT32                      ulKeyFieldOffset;   /* 主键在数据记录中的偏移量（主键字段必须是连续的） */

    GOS_HASH_FUNC               pfHashFunc;         /* Hash函数 */
    GOS_HASH_KEY_COMP_FUNC      pfKeyCompFunc;      /* 主键比较函数 */

    GOS_HASH_ALLOC_FUNC         pfAllocFunc;        /* 申请节点的函数 */
    GOS_HASH_FREE_FUNC          pfFreeFunc;         /* 释放节点的函数 */

    UINT32                      ulAllocMemSize;
}GOS_HASH_QUE_T;

/******************************************************************************
* 函数名称: gos_hashlist_init
* 功    能: 初始化HASH队列
* 参    数:
* 参数名称          输入/输出   描述
* ulMinNodeNum          IN          最小队列节点数
* ulMaxNodeNum          IN          最大队列节点数
* ulHashEntrySize       IN          hash桶大下
* ulStaticNodeDataLen   IN          静态分配节点数据的最大长度
* ulKeyFieldOffset      IN          主键的偏移量
* pfHashFunc            IN          Hash函数
* pfKeyCompFunc         IN          主键比较函数
* pfAllocFunc           IN          节点申请函数
* pfFreeFunc            IN          节点释放函数
* 函数返回:
* 说    明:
******************************************************************************/
GHASH* gos_hashlist_init
(
    UINT32                      ulMinNodeNum,
    UINT32                      ulMaxNodeNum,
    UINT32                      ulHashEntrySize,
    UINT32                      ulStaticNodeDataLen,
    UINT32                      ulKeyFieldOffset,
    GOS_HASH_FUNC               pfHashFunc,
    GOS_HASH_KEY_COMP_FUNC      pfKeyCompFunc,
    GOS_HASH_ALLOC_FUNC         pfAllocFunc,
    GOS_HASH_FREE_FUNC          pfFreeFunc
)
{
    GOS_HASH_QUE_T              *pstHashQue;
    UINT32                      ulMemSize;
    UINT8                       *pucData;
    GOS_HASH_NODE_T             *pstNode;
    GOS_HASH_NODE_T             *pstPrevNode;
    UINT32                      i;

    if (0 == ulMaxNodeNum)
    {
        ulMaxNodeNum = 0xffffffff;
    }

    if (ulMinNodeNum > ulMaxNodeNum)
    {
        ulMinNodeNum = ulMaxNodeNum;
    }

    if (0 == ulHashEntrySize ||
        NULL== pfAllocFunc || NULL == pfFreeFunc)
    {
        return NULL;
    }

    ulHashEntrySize = gos_get_prime(ulHashEntrySize);

    pstHashQue = (GOS_HASH_QUE_T*)GOS_MALLOC(sizeof(GOS_HASH_QUE_T));
    if (NULL == pstHashQue)
    {
        return NULL;
    }

    memset(pstHashQue, 0, sizeof(GOS_HASH_QUE_T));

    pstHashQue->ulMinNodeNum     = ulMinNodeNum;
    pstHashQue->ulMaxNodeNum     = ulMaxNodeNum;
    pstHashQue->ulHashEntrySize  = ulHashEntrySize;
    pstHashQue->ulStaticNodeDataLen = ulStaticNodeDataLen;
    pstHashQue->ulKeyFieldOffset = ulKeyFieldOffset;
    pstHashQue->pfHashFunc       = pfHashFunc;
    pstHashQue->pfKeyCompFunc    = pfKeyCompFunc;
    pstHashQue->pfAllocFunc      = pfAllocFunc;
    pstHashQue->pfFreeFunc       = pfFreeFunc;

    ulMemSize = sizeof(GOS_HASH_ENTRY_T)*pstHashQue->ulHashEntrySize;
    if (ulStaticNodeDataLen > 0 && ulMinNodeNum > 0)
    {
        ulMemSize += (sizeof(GOS_HASH_NODE_T)+ulStaticNodeDataLen)*ulMinNodeNum;
    }

    pstHashQue->pstHashEntry = (GOS_HASH_ENTRY_T*)GOS_ZALLOC(ulMemSize);
    if (NULL == pstHashQue->pstHashEntry)
    {
        GOS_FREE(pstHashQue);
        return NULL;
    }

    pstHashQue->ulAllocMemSize += ulMemSize;

    memset(&pstHashQue->stFreeDummy, 0, sizeof(pstHashQue->stFreeDummy));

    if (ulStaticNodeDataLen > 0 && ulMinNodeNum > 0)
    {
        pstHashQue->pstStaticNodeStart = (GOS_HASH_NODE_T *)&pstHashQue->pstHashEntry[pstHashQue->ulHashEntrySize];
        pstHashQue->pstStaticNodeEnd = pstHashQue->pstStaticNodeStart+ulMinNodeNum-1;
        pstHashQue->stFreeDummy.pstNext = pstHashQue->pstStaticNodeStart;

        pucData = (UINT8*)(pstHashQue->pstStaticNodeStart + ulMinNodeNum);
        pstNode = pstHashQue->pstStaticNodeStart;
        pstPrevNode = &pstHashQue->stFreeDummy;
        for (i=0; i<ulMinNodeNum; i++)
        {
            pstNode->pData = (VOID*)pucData;
            pstNode->pstPrev = pstPrevNode;
            pstPrevNode->pstNext = pstNode;

            pucData += ulStaticNodeDataLen;
            pstPrevNode = pstNode;
            pstNode++;
        }

        pstHashQue->stFreeDummy.pstPrev = pstPrevNode;
        pstPrevNode->pstNext = &pstHashQue->stFreeDummy;
    }

    for (i=0; i<pstHashQue->ulHashEntrySize; i++)
    {
        pstHashQue->pstHashEntry[i].stNodeDummy.pstHead = NULL;//(GOS_HASH_NODE_T *)&pstHashQue->pstHashEntry[i].stNodeDummy;
        pstHashQue->pstHashEntry[i].stNodeDummy.pstTail = NULL;//(GOS_HASH_NODE_T *)&pstHashQue->pstHashEntry[i].stNodeDummy;
    }

    pstHashQue->stListDummy.pstTail = NULL;
    pstHashQue->stListDummy.pstHead = NULL;

    return (GHASH*)pstHashQue;
}

UINT32 gos_hashlist_get_alloc_size(GHASH *hHashQue)
{
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;

    if (!pstHashQue)
    {
        return 0;
    }

    return pstHashQue->ulAllocMemSize;
}

/******************************************************************************
* 函数名称: gos_hash_get_entry_index
* 功    能: 计算HASH值
* 参    数:
* 参数名称          输入/输出               描述

* 函数返回:
* 说    明:
******************************************************************************/
#define gos_hash_get_entry_index(pstHashQue, pKey)   (pstHashQue->pfHashFunc(pKey) % pstHashQue->ulHashEntrySize)

/******************************************************************************
* 函数名称: gos_hashlist_alloc_node
* 功    能: 申请一个空闲的hash节点
* 参    数:
* 参数名称          输入/输出               描述

* 函数返回:
* 说    明:
******************************************************************************/
GOS_HASH_NODE_T *gos_hashlist_alloc_node(GOS_HASH_QUE_T* pstHashQue, VOID *pData, UINT32 ulDataLen)
{
    GOS_HASH_NODE_T     *pstHashNode;

    if (!pstHashQue)
    {
        return NULL;
    }

    if (pstHashQue->ulCurrNodeNum >= pstHashQue->ulMaxNodeNum)
    {
        return NULL;
    }

    if (pstHashQue->stFreeDummy.pstNext &&
        pstHashQue->stFreeDummy.pstNext != &pstHashQue->stFreeDummy &&
        ulDataLen <= pstHashQue->ulStaticNodeDataLen)
    {
        pstHashNode = pstHashQue->stFreeDummy.pstNext;
        memcpy(pstHashNode->pData, pData, ulDataLen);
        pstHashNode->ulCurrDataLen = ulDataLen;
        pstHashNode->ulMaxDataLen = pstHashQue->ulStaticNodeDataLen;

        pstHashQue->stFreeDummy.pstNext = pstHashNode->pstNext;
        pstHashQue->stFreeDummy.pstNext->pstPrev = &pstHashQue->stFreeDummy;

        pstHashQue->ulCurrNodeNum++;

        return pstHashNode;
    }
    else
    {
        pstHashNode = (GOS_HASH_NODE_T*)pstHashQue->pfAllocFunc(sizeof(GOS_HASH_NODE_T)+ulDataLen);
        if (!pstHashNode)
        {
            return NULL;
        }

        pstHashNode->pData = (pstHashNode+1);
        memcpy(pstHashNode->pData, pData, ulDataLen);
        pstHashNode->ulCurrDataLen = ulDataLen;
        pstHashNode->ulMaxDataLen = ulDataLen;

        pstHashQue->ulCurrNodeNum++;

        return pstHashNode;
    }
}

/******************************************************************************
* 函数名称: gos_hashlist_free_node
* 功    能: 释放hash节点
* 参    数:
* 参数名称          输入/输出               描述

* 函数返回:
* 说    明:
******************************************************************************/
VOID gos_hashlist_free_node(GOS_HASH_QUE_T *pstHashQue, GOS_HASH_NODE_T *pstHashNode)
{
    if (!pstHashQue || !pstHashNode)
    {
        return;
    }

    if (pstHashNode >= pstHashQue->pstStaticNodeStart &&
        pstHashNode <= pstHashQue->pstStaticNodeEnd )
    {
        pstHashQue->stFreeDummy.pstNext->pstPrev = pstHashNode;
        pstHashNode->pstNext = pstHashQue->stFreeDummy.pstNext;

        pstHashQue->stFreeDummy.pstNext = pstHashNode;
        pstHashNode->pstPrev = &pstHashQue->stFreeDummy;
    }
    else
    {
        pstHashQue->pfFreeFunc(pstHashNode);
    }

    pstHashQue->ulCurrNodeNum--;
}

/******************************************************************************
* 函数名称: _gos_hashlist_add
* 功    能: 在HASH队列中增加一个节点
* 参    数:
* 参数名称          输入/输出               描述

  * 函数返回:
  * 说    明:
******************************************************************************/
VOID* _gos_hashlist_add(GOS_HASH_QUE_T* pstHashQue, UINT32 ulHashValue, VOID* pData, UINT32 ulDataLen)
{
    GOS_HASH_ENTRY_T    *pstHashEntry;
    GOS_HASH_NODE_T     *pstHashNode;
    CHAR                *pSrcKey;
    CHAR                *pDstKey;

    if (!pstHashQue)
    {
        return NULL;
    }

    if (pstHashQue->ulCurrNodeNum >= pstHashQue->ulMaxNodeNum)
    {
        return NULL;
    }

    if (ulHashValue >= pstHashQue->ulHashEntrySize)
    {
        return NULL;
    }

    pstHashEntry = &pstHashQue->pstHashEntry[ulHashValue];
    pstHashNode = pstHashEntry->stNodeDummy.pstHead;

    /* 先判断是否存在 */
    while(pstHashNode && pstHashNode != (GOS_HASH_NODE_T *)&pstHashEntry->stNodeDummy && pstHashNode->pData)
    {
        pSrcKey = ((CHAR*)pstHashNode->pData) + pstHashQue->ulKeyFieldOffset;
        pDstKey = ((CHAR*)pData) + pstHashQue->ulKeyFieldOffset;
        if (pstHashQue->pfKeyCompFunc(pSrcKey, pDstKey))
        {
            if (ulDataLen <= pstHashNode->ulCurrDataLen ||
               (ulDataLen <= pstHashNode->ulMaxDataLen &&
                pstHashNode >= pstHashQue->pstStaticNodeStart &&
                pstHashNode <= pstHashQue->pstStaticNodeEnd ) )
            {
                memcpy(pstHashNode->pData, pData, ulDataLen);
                pstHashNode->ulCurrDataLen = ulDataLen;
                return pstHashNode->pData;
            }

            /* 释放该节点 */
            gos_hashlist_remove_node((GHASH*)pstHashQue, (GHASHNODE*)pstHashNode);
            break;
        }

        pstHashNode = pstHashNode->pstNext;
    }

    /* 获取一个空闲的节点 */
    pstHashNode = gos_hashlist_alloc_node(pstHashQue, pData, ulDataLen);
    if (NULL == pstHashNode)
    {
        return NULL;
    }

    /* 维护HashEntry */
    pstHashNode->pstPrev = pstHashEntry->stNodeDummy.pstTail;
    pstHashNode->pstNext = NULL;

    if (pstHashEntry->stNodeDummy.pstTail)
    {
        pstHashEntry->stNodeDummy.pstTail->pstNext = pstHashNode;
    }

    pstHashEntry->stNodeDummy.pstTail = pstHashNode;
    if (!pstHashEntry->stNodeDummy.pstHead)
    {
        pstHashEntry->stNodeDummy.pstHead = pstHashNode;
    }

    /* 维护List */
    pstHashNode->pstPrevLink = pstHashQue->stListDummy.pstTail;
    pstHashNode->pstNextLink = NULL;

    if (pstHashQue->stListDummy.pstTail)
    {
        pstHashQue->stListDummy.pstTail->pstNextLink = pstHashNode;
    }

    pstHashQue->stListDummy.pstTail = pstHashNode;
    if (!pstHashQue->stListDummy.pstHead)
    {
        pstHashQue->stListDummy.pstHead = pstHashNode;
    }

    pstHashNode->ulHashValue = ulHashValue;

    return pstHashNode->pData;
}

/******************************************************************************
* 函数名称: gos_hashlist_add
* 功    能: 在HASH队列中增加一个节点
* 参    数:
* 参数名称          输入/输出               描述

* 函数返回:
* 说    明:
******************************************************************************/
VOID* gos_hashlist_add(GHASH* hHashQue, VOID* pData, UINT32 ulDataLen)
{
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;
    UINT32              ulHashValue;

    if (!pstHashQue)
    {
        return NULL;
    }

    ulHashValue = gos_hash_get_entry_index(pstHashQue, ((UINT8*)pData)+pstHashQue->ulKeyFieldOffset);

    return _gos_hashlist_add(pstHashQue, ulHashValue, pData, ulDataLen);
}

VOID* gos_hashlist_add_int(GHASH* hHashQue, VOID* pData, UINT32 ulDataLen)
{
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;
    UINT32              ulHashValue;

    if (!pstHashQue)
    {
        return NULL;
    }

    ulHashValue = GET_INT(((UINT8*)pData)+pstHashQue->ulKeyFieldOffset);
    ulHashValue = ulHashValue % pstHashQue->ulHashEntrySize;

    return _gos_hashlist_add(pstHashQue, ulHashValue, pData, ulDataLen);
}

VOID* gos_hashlist_add_int64(GHASH* hHashQue, VOID* pData, UINT32 ulDataLen)
{
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;
    UINT32              ulHashValue;
    UINT64              u64Key;

    if (!pstHashQue)
    {
        return NULL;
    }

    u64Key = GET_INT64(((UINT8*)pData)+pstHashQue->ulKeyFieldOffset);
    ulHashValue = ((u64Key>>32) + (UINT32)u64Key) % pstHashQue->ulHashEntrySize;

    return _gos_hashlist_add(pstHashQue, ulHashValue, pData, ulDataLen);
}

/******************************************************************************
* 函数名称: gos_hashlist_get_node
* 功    能: 在HASH队列中查找一个节点
* 参    数:
* 参数名称          输入/输出               描述

* 函数返回:
* 说    明:
******************************************************************************/
GOS_HASH_NODE_T* gos_hashlist_get_node(GHASH* hHashQue, VOID* pKey)
{
    UINT32              ulHashValue;
    GOS_HASH_ENTRY_T    *pstHashEntry;
    GOS_HASH_NODE_T     *pstHashNode;
    UINT8*              pSrcKey;
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;

    if (!pstHashQue)
    {
        return NULL;
    }

    if (pstHashQue->pfHashFunc == gos_hash_int)
    {
        ulHashValue = GET_INT(pKey) % pstHashQue->ulHashEntrySize;
    }
    else
    {
        ulHashValue = gos_hash_get_entry_index(pstHashQue, pKey);
    }

    if (ulHashValue >= pstHashQue->ulHashEntrySize)
    {
        return NULL;
    }

    pstHashEntry = &pstHashQue->pstHashEntry[ulHashValue];
    pstHashNode = pstHashEntry->stNodeDummy.pstHead;

    while(pstHashNode && pstHashNode != (GOS_HASH_NODE_T *)&pstHashEntry->stNodeDummy && pstHashNode->pData)
    {
        pSrcKey = ((UINT8*)pstHashNode->pData) + pstHashQue->ulKeyFieldOffset;
        if (pstHashQue->pfKeyCompFunc(pSrcKey, pKey))
        {
            return pstHashNode;
        }

        pstHashNode = pstHashNode->pstNext;
    }

    return NULL;
}

VOID *gos_hashlist_get(GHASH* hHashQue, VOID* pKey)
{
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;
    GOS_HASH_NODE_T     *pstHashNode;

    if (!pstHashQue)
    {
        return NULL;
    }

    if (pstHashQue->pfHashFunc == gos_hash_int)
    {
        return gos_hashlist_get_int(hHashQue, GET_INT(pKey));
    }

    pstHashNode = (GOS_HASH_NODE_T*)gos_hashlist_get_node(hHashQue, pKey);
    if (NULL == pstHashNode)
    {
        return NULL;
    }
    else
    {
        return pstHashNode->pData;
    }
}

VOID *gos_hashlist_get_int(GHASH* hHashQue, UINT32 ulKey)
{
    UINT32              ulHashValue;
    GOS_HASH_ENTRY_T    *pstHashEntry;
    GOS_HASH_NODE_T     *pstHashNode;
    UINT32              *pulSrc;
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;

    if (!pstHashQue)
    {
        return NULL;
    }

    ulHashValue = ulKey % pstHashQue->ulHashEntrySize;
    pstHashEntry = pstHashQue->pstHashEntry+ulHashValue;
    pstHashNode = pstHashEntry->stNodeDummy.pstHead;

    for (; pstHashNode && pstHashNode != (GOS_HASH_NODE_T *)&pstHashEntry->stNodeDummy; pstHashNode=pstHashNode->pstNext)
    {
        pulSrc = (UINT32*)(((UINT8*)pstHashNode->pData) + pstHashQue->ulKeyFieldOffset);

        if (*pulSrc == ulKey)
        {
            return pstHashNode->pData;
        }
    }

    return NULL;
}

VOID *gos_hashlist_get_int64(GHASH* hHashQue, UINT64 u64Key)
{
    UINT32              ulHashValue;
    GOS_HASH_ENTRY_T    *pstHashEntry;
    GOS_HASH_NODE_T     *pstHashNode;
    UINT64              *pu64Src;
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;

    if (!pstHashQue)
    {
        return NULL;
    }

    ulHashValue = ((u64Key>>32) + (UINT64)u64Key) % pstHashQue->ulHashEntrySize;
    pstHashEntry = pstHashQue->pstHashEntry+ulHashValue;
    pstHashNode = pstHashEntry->stNodeDummy.pstHead;

    for (; pstHashNode && pstHashNode != (GOS_HASH_NODE_T *)&pstHashEntry->stNodeDummy; pstHashNode=pstHashNode->pstNext)
    {
        pu64Src = (UINT64*)(((UINT8*)pstHashNode->pData) + pstHashQue->ulKeyFieldOffset);

        if (*pu64Src == u64Key)
        {
            return pstHashNode->pData;
        }
    }

    return NULL;
}

VOID *gos_hashlist_get_string(GHASH* hHashQue, CHAR *szKey)
{
    UINT32              ulHashValue;
    GOS_HASH_ENTRY_T    *pstHashEntry;
    GOS_HASH_NODE_T     *pstHashNode;
    CHAR                *szSrcKey;
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;

    if (!pstHashQue || !szKey)
    {
        return NULL;
    }

    ulHashValue = gos_hash_get_entry_index(pstHashQue, szKey);
    if (ulHashValue >= pstHashQue->ulHashEntrySize)
    {
        return NULL;
    }

    pstHashEntry = pstHashQue->pstHashEntry+ulHashValue;
    pstHashNode = pstHashEntry->stNodeDummy.pstHead;
    for (; pstHashNode && pstHashNode != (GOS_HASH_NODE_T *)&pstHashEntry->stNodeDummy; pstHashNode=pstHashNode->pstNext)
    {
        szSrcKey = ((CHAR*)pstHashNode->pData) + pstHashQue->ulKeyFieldOffset;
        if (pstHashQue->pfKeyCompFunc)
        {
            if (pstHashQue->pfKeyCompFunc(szSrcKey, szKey))
            {
                return pstHashNode->pData;
            }
        }
        else
        {
            if (strcmp(szSrcKey, szKey) == 0)
            {
                return pstHashNode->pData;
            }
        }
    }

    return NULL;
}

/******************************************************************************
* 函数名称: gos_hashlist_remove_node
* 功    能: 在HASH队列中删除一个节点
* 参    数:
* 参数名称          输入/输出               描述

* 函数返回:
* 说    明:
******************************************************************************/
VOID _gos_hashlist_remove_node(GOS_HASH_QUE_T* pstHashQue, GOS_HASH_ENTRY_T *pstHashEntry, GOS_HASH_NODE_T* pstHashNode)
{
    if (!pstHashQue || !pstHashNode || !pstHashEntry)
    {
        return;
    }

    /* 维护HasnEntry */
    if (pstHashNode->pstPrev != NULL)
    {
        pstHashNode->pstPrev->pstNext = pstHashNode->pstNext;
    }

    if (pstHashNode->pstNext != NULL)
    {
        pstHashNode->pstNext->pstPrev = pstHashNode->pstPrev;
    }

    if (pstHashEntry->stNodeDummy.pstTail == pstHashNode)
    {
        pstHashEntry->stNodeDummy.pstTail = pstHashNode->pstPrev;
    }

    if (pstHashEntry->stNodeDummy.pstHead == pstHashNode)
    {
        pstHashEntry->stNodeDummy.pstHead = pstHashNode->pstNext;
    }

    if (pstHashEntry->stNodeDummy.pstHead == NULL)
    {
        pstHashEntry->stNodeDummy.pstTail = NULL;
    }

    /* 维护List */
    if (pstHashNode->pstPrevLink != NULL)
    {
        pstHashNode->pstPrevLink->pstNextLink = pstHashNode->pstNextLink;
    }

    if (pstHashNode->pstNextLink != NULL)
    {
        pstHashNode->pstNextLink->pstPrevLink = pstHashNode->pstPrevLink;
    }

    if (pstHashQue->stListDummy.pstTail == pstHashNode)
    {
        pstHashQue->stListDummy.pstTail = pstHashNode->pstPrevLink;
    }

    if (pstHashQue->stListDummy.pstHead == pstHashNode)
    {
        pstHashQue->stListDummy.pstHead = pstHashNode->pstNextLink;
    }

    if (pstHashQue->stListDummy.pstHead == NULL)
    {
        pstHashQue->stListDummy.pstTail = NULL;
    }

    gos_hashlist_free_node(pstHashQue, pstHashNode);
}

/******************************************************************************
* 函数名称: gos_hashlist_remove_node
* 功    能: 在HASH队列中删除一个节点
* 参    数:
* 参数名称          输入/输出               描述

  * 函数返回:
  * 说    明:
******************************************************************************/
VOID gos_hashlist_remove_node(GHASH* hHashQue, GHASHNODE* hNode)
{
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;
    GOS_HASH_NODE_T     *pstHashNode = (GOS_HASH_NODE_T*)hNode;
    GOS_HASH_ENTRY_T    *pstHashEntry;

    if (!pstHashQue || !pstHashNode)
    {
        return;
    }

    pstHashEntry = &pstHashQue->pstHashEntry[pstHashNode->ulHashValue];

    _gos_hashlist_remove_node(pstHashQue, pstHashEntry, (GOS_HASH_NODE_T*)hNode);
}

VOID gos_hashlist_remove_first(GHASH* hHashQue)
{
    GHASHNODE   *hNode = gos_hashlist_get_first(hHashQue);

    gos_hashlist_remove_node(hHashQue, hNode);
}

/******************************************************************************
* 函数名称: gos_hashlist_remove
* 功    能: 在HASH队列中删除一个节点
* 参    数:
* 参数名称          输入/输出               描述
  ulKeyLen          IN                      如果是字符串，长度为字符串长度加1
* 函数返回:
* 说    明:
******************************************************************************/
BOOL _gos_hashlist_remove(GOS_HASH_QUE_T* pstHashQue, UINT32 ulHashValue, VOID* pKey)
{
    GOS_HASH_ENTRY_T    *pstHashEntry;
    GOS_HASH_NODE_T     *pstHashNode;
    UINT8*              pSrcKey;

    if (!pstHashQue)
    {
        return FALSE;
    }

    if (ulHashValue >= pstHashQue->ulHashEntrySize)
    {
        return FALSE;
    }

    pstHashEntry = &pstHashQue->pstHashEntry[ulHashValue];
    pstHashNode = pstHashEntry->stNodeDummy.pstHead;

    while(pstHashNode && pstHashNode != (GOS_HASH_NODE_T *)&pstHashEntry->stNodeDummy && pstHashNode->pData)
    {
        pSrcKey = ((UINT8*)pstHashNode->pData) + pstHashQue->ulKeyFieldOffset;
        if (pstHashQue->pfKeyCompFunc(pSrcKey, pKey))
        {
            _gos_hashlist_remove_node(pstHashQue, pstHashEntry, pstHashNode);
            return TRUE;
        }

        pstHashNode = pstHashNode->pstNext;
    }

    return FALSE;
}

/******************************************************************************
* 函数名称: gos_hashlist_remove
* 功    能: 在HASH队列中删除一个节点
* 参    数:
* 参数名称          输入/输出               描述
  ulKeyLen          IN                      如果是字符串，长度为字符串长度加1
* 函数返回:
* 说    明:
******************************************************************************/
BOOL gos_hashlist_remove(GHASH* hHashQue, VOID* pKey)
{
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;
    UINT32              ulHashValue;

    if (!pstHashQue)
    {
        return FALSE;
    }

    if (pstHashQue->pfHashFunc == gos_hash_int)
    {
        ulHashValue = GET_INT(pKey) % pstHashQue->ulHashEntrySize;
    }
    else
    {
        ulHashValue = gos_hash_get_entry_index(pstHashQue, pKey);
    }

    return _gos_hashlist_remove(pstHashQue, ulHashValue, pKey);
}

BOOL gos_hashlist_remove_string(GHASH* hHashQue, CHAR *szKey)
{
    if (NULL == szKey)
    {
        return FALSE;
    }

    return gos_hashlist_remove(hHashQue, szKey);
}

BOOL gos_hashlist_remove_int(GHASH* hHashQue, UINT32 ulKey)
{
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;
    UINT32              ulHashValue;

    if (!pstHashQue)
    {
        return FALSE;
    }

    ulHashValue = ulKey % pstHashQue->ulHashEntrySize;

    return _gos_hashlist_remove(pstHashQue, ulHashValue, &ulKey);
}

BOOL gos_hashlist_remove_int64(GHASH* hHashQue, UINT64 u64Key)
{
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;
    UINT32              ulHashValue;

    if (!pstHashQue)
    {
        return FALSE;
    }

    ulHashValue = ((u64Key>>32) + (UINT64)u64Key) % pstHashQue->ulHashEntrySize;

    return _gos_hashlist_remove(pstHashQue, ulHashValue, &u64Key);
}

/******************************************************************************
* 函数名称: gos_hashlist_remove_all
* 功    能: 释放所有Hash队列节点
* 参    数:
* 参数名称          输入/输出               描述
* 函数返回:
* 说    明:
******************************************************************************/
BOOL gos_hashlist_remove_all(GHASH* hHashQue)
{
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;
    UINT32              i;
    GOS_HASH_ENTRY_T    *pstHashEntry;
    GOS_HASH_NODE_T     *pstNode;
    GOS_HASH_NODE_T     *pstNextNode;

    if (!pstHashQue)
    {
        return FALSE;
    }

    if (pstHashQue->ulCurrNodeNum == 0)
    {
        return TRUE;
    }

    for (i=0; i<pstHashQue->ulHashEntrySize; i++)
    {
        pstHashEntry = &pstHashQue->pstHashEntry[i];

        pstNode = pstHashEntry->stNodeDummy.pstHead;
        while(pstNode && pstNode != (GOS_HASH_NODE_T *)&pstHashEntry->stNodeDummy)
        {
            pstNextNode = pstNode->pstNext;
            _gos_hashlist_remove_node(pstHashQue, pstHashEntry, pstNode);
            pstNode = pstNextNode;
        }
    }

    return TRUE;
}

/******************************************************************************
* 函数名称: gos_hashlist_free
* 功    能: 释放Hash队列
* 参    数:
* 参数名称          输入/输出               描述
* 函数返回:
* 说    明:
******************************************************************************/
BOOL gos_hashlist_free(GHASH* hHashQue)
{
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;

    if (!pstHashQue)
    {
        return FALSE;
    }

    gos_hashlist_remove_all(hHashQue);

    GOS_FREE(pstHashQue->pstHashEntry);
    GOS_FREE(pstHashQue);

    return TRUE;
}

/******************************************************************************
* 函数名称: gos_hashlist_get_node_num
* 功    能: 获取队列节点数
* 参    数:
* 参数名称          输入/输出               描述
* 函数返回:
* 说    明:
******************************************************************************/
INT32 gos_hashlist_get_node_num(GHASH* hHashQue)
{
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;

    if (!pstHashQue)
    {
        return -1;
    }

    return pstHashQue->ulCurrNodeNum;
}

INT32 gos_hashlist_get_static_node_num(GHASH* hHashQue)
{
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;

    if (!pstHashQue)
    {
        return -1;
    }

    return pstHashQue->ulMinNodeNum;
}

/******************************************************************************
* 函数名称: gos_hashlist_foreach
* 功    能: 遍历符合条件的Hash队列节点
* 参    数:
* 参数名称          输入/输出               描述
* 函数返回:
* 说    明:
******************************************************************************/
BOOL gos_hashlist_foreach(GHASH* hHashQue, GOS_HASH_FOREACH_FUNC pfFunc, VOID *pData)
{
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;
    GOS_HASH_NODE_T     *pstNode;
    GHASHNODE           *pNode;
    GHASHNODE           *pNextNode;

    if (!pstHashQue)
    {
        return FALSE;
    }

    pNode = gos_hashlist_get_first(hHashQue);
    while(pNode)
    {
        pNextNode = gos_hashlist_get_next(hHashQue, pNode);
        pstNode = (GOS_HASH_NODE_T*)pNode;

        pfFunc(((UINT8*)pstNode->pData) + pstHashQue->ulKeyFieldOffset, pstNode->pData, pData);

        pNode = pNextNode;
    }

    return TRUE;
}

/******************************************************************************
* 函数名称: gos_hashlist_foreach_remove
* 功    能: 遍历所有Hash队列节点
* 参    数:
* 参数名称          输入/输出               描述
* 函数返回:
* 说    明:
******************************************************************************/
BOOL gos_hashlist_foreach_remove(GHASH* hHashQue, GOS_HASH_FOREACH_REMOVE_FUNC pfFunc, VOID *pData)
{
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;
    GOS_HASH_NODE_T     *pstNode;
    GHASHNODE           *pNode;
    GHASHNODE           *pNextNode;

    if (!pstHashQue)
    {
        return FALSE;
    }

    pNode = gos_hashlist_get_first(hHashQue);
    while(pNode)
    {
        pNextNode = gos_hashlist_get_next(hHashQue, pNode);
        pstNode = (GOS_HASH_NODE_T*)pNode;

        if (pfFunc(((UINT8*)pstNode->pData) + pstHashQue->ulKeyFieldOffset, pstNode->pData, pData))
        {
            gos_hashlist_remove_node(hHashQue, pNode);
        }

        pNode = pNextNode;
    }

    return TRUE;
}

/******************************************************************************
* 函数名称: gos_hashlist_get_first
* 功    能: 获取第一个节点
* 参    数:
* 参数名称          输入/输出               描述
* 函数返回:
* 说    明:
******************************************************************************/
GHASHNODE* gos_hashlist_get_first(GHASH* hHashQue)
{
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;

    if (!pstHashQue)
    {
        return NULL;
    }

    if (pstHashQue->ulCurrNodeNum == 0)
    {
        return NULL;
    }

    return (GHASHNODE*)pstHashQue->stListDummy.pstHead;
}

/******************************************************************************
* 函数名称: gos_hashlist_get_last
* 功    能: 获取最后一个节点
* 参    数:
* 参数名称          输入/输出               描述
* 函数返回:
* 说    明:
******************************************************************************/
GHASHNODE* gos_hashlist_get_last(GHASH* hHashQue)
{
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;

    if (!pstHashQue)
    {
        return NULL;
    }

    if (pstHashQue->ulCurrNodeNum == 0)
    {
        return NULL;
    }

    return (GHASHNODE*)pstHashQue->stListDummy.pstTail;
}

/******************************************************************************
* 函数名称: gos_hashlist_get_next
* 功    能: 获取下一个节点
* 参    数:
* 参数名称          输入/输出               描述
* 函数返回:
* 说    明:
******************************************************************************/
GHASHNODE* gos_hashlist_get_next(GHASH* hHashQue, GHASHNODE* hNode)
{
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;
    GOS_HASH_NODE_T     *pstNode = (GOS_HASH_NODE_T*)hNode;

    if (!pstHashQue || !pstNode)
    {
        return NULL;
    }

    if (pstHashQue->ulCurrNodeNum == 0)
    {
        return NULL;
    }

    return (GHASHNODE*)pstNode->pstNextLink;
}

/******************************************************************************
* 函数名称: gos_hashlist_get_prev
* 功    能: 获取前一个节点
* 参    数:
* 参数名称          输入/输出               描述
* 函数返回:
* 说    明:
******************************************************************************/
GHASHNODE* gos_hashlist_get_prev(GHASH* hHashQue, GHASHNODE* hNode)
{
    GOS_HASH_QUE_T      *pstHashQue = (GOS_HASH_QUE_T*)hHashQue;
    GOS_HASH_NODE_T     *pstNode = (GOS_HASH_NODE_T*)hNode;

    if (!pstHashQue || !pstNode)
    {
        return NULL;
    }

    if (pstHashQue->ulCurrNodeNum == 0)
    {
        return NULL;
    }

    return (GHASHNODE*)pstNode->pstPrevLink;
}

/******************************************************************************
* 函数名称: gos_hashlist_get_node_data
* 功    能: 获取节点内容
* 参    数:
* 参数名称          输入/输出               描述
* 函数返回:
* 说    明:
******************************************************************************/
VOID* gos_hashlist_get_node_data(GHASHNODE* hNode)
{
    GOS_HASH_NODE_T     *pstNode = (GOS_HASH_NODE_T*)hNode;

    if (!pstNode)
    {
        return NULL;
    }
    else
    {
        return pstNode->pData;
    }
}

UINT32 gos_hashlist_get_node_data_len(GHASHNODE* hNode)
{
    GOS_HASH_NODE_T     *pstNode = (GOS_HASH_NODE_T*)hNode;

    if (!pstNode)
    {
        return 0;
    }
    else
    {
        return pstNode->ulCurrDataLen;
    }
}

GHASHNODE* gos_hashlist_next(GHASH* hHash, GHASHNODE* hNode, GHASHNODE **ppNext)
{
    GHASHNODE *hNext = gos_hashlist_get_next(hHash, hNode);

    if (ppNext)
    {
        *ppNext = hNext;
    }

    return hNext;
}

GHASHNODE* gos_hashlist_prev(GHASH* hHash, GHASHNODE* hNode, GHASHNODE **ppPrev)
{
    GHASHNODE *hPrev = gos_hashlist_get_prev(hHash, hNode);

    if (ppPrev)
    {
        *ppPrev = hPrev;
    }

    return hPrev;
}
