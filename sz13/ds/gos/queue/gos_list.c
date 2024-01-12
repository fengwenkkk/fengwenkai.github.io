#include "gos_api.h"
#include "gos_func.h"
#include "gos_list.h"

typedef struct GOS_LIST_NODE_T
{
    struct GOS_LIST_NODE_T  *pstPrev;
    struct GOS_LIST_NODE_T  *pstNext;
    VOID                    *pvData;

    UINT32                  ulDataLen;
}GOS_LIST_NODE_T;

typedef BOOL (GOS_LIST_CMP_FUNC)(VOID* pvSrc, VOID *pvDst);

typedef struct
{
    UINT32                  ulMinSize;          /* 队列节点最小个数 */
    UINT32                  ulMaxSize;          /* 队列节点最大个数 */
    UINT32                  ulDataLen;          /* 队列节点数据长度 */

    GOS_LIST_SET_FUNC       pfSetFunc;
    GOS_LIST_ALLOC_FUNC     pfMemAllocFunc;
    GOS_LIST_FREE_FUNC      pfMemFreeFunc;
    GOS_LIST_FREE_DATA_FUNC pfFreeDataFunc;    /* 队列节点数据释放函数 */
}GOS_LIST_OPTION_T;

typedef struct
{
    UINT32                  ulSize;         /* 当前队列个数 */

    GOS_LIST_OPTION_T       stOption;

    GOS_LIST_NODE_T         stFreeList;     /* 空闲链表哑元 */
    GOS_LIST_NODE_T         stBusyList;     /* 忙链表哑元 */

    GOS_LIST_NODE_T         *pstNodes;
    UINT8                   *pucData;       /* 静态分配时，数据存放的地址 */
    UINT8                   *pucDataEnd;

    UINT32                  ulAllocMemSize;
}GOS_LIST_T;

/*******************************************************************************
* 函数名称: list_free_node_func
* 功    能: 释放节点的内存空间
* 参    数:
* 参数名称       输入/输出       描述
* pstList        IN              队列指针
* pNode          IN              想释放的节点指针
* bFreeNode      IN              是否需要释放节点本身
* 函数返回: 释放内存成功返回TRUE，释放内存失败返回FALSE
* 说    明:
*******************************************************************************/
BOOL gos_list_free_node_func(GOS_LIST_T *pstList, GOS_LIST_NODE_T* pNode, BOOL bFreeNode)
{
    if (!pstList)
    {
        return FALSE;
    }

    if (!pNode)
    {
        return FALSE;
    }

    /* 先释放pData内部的指针 */
    if (pstList->stOption.pfFreeDataFunc)
    {
        pstList->stOption.pfFreeDataFunc(pNode->pvData);
    }

    if (bFreeNode)
    {
        pstList->stOption.pfMemFreeFunc(pNode);
    }

    return TRUE;
}

/*******************************************************************************
* 函数名称: gos_list_cmp_int
* 功    能: int类型主键比较函数
* 参    数:
* 参数名称       输入/输出       描述
* pSrc           IN              主键1
* pDst           IN              主键2
* BOOL           OUT
* 函数返回: 相同返回TRUE，不同返回FALSE
* 说    明:
*******************************************************************************/
BOOL gos_list_cmp_int(VOID* pvSrc, VOID *pvDst)
{
    return (*((INT32*)pvSrc) == *((INT32*)pvDst));
}

/*******************************************************************************
* 函数名称: gos_list_set_func
* 功    能: 数据复制函数
* 参    数:
* 参数名称       输入/输出       描述
* pSrc           IN              队列数据源
* ulDataLen      IN              数据源长度
* pDst           IN              队列数据目的
* 函数返回:
* 说    明:
*******************************************************************************/
#define gos_list_set_func(pvSrc, ulDataLen, pvDst) \
if (!ulDataLen)     \
{                   \
    pvDst = pvSrc;  \
}                   \
else                \
{                   \
    memcpy(pvDst, pvSrc, ulDataLen);\
}

/*******************************************************************************
* 函数名称: list_init
* 功    能: 队列初始化
* 参    数:
* 参数名称       输入/输出       描述
* 函数返回: 返回队列句柄
* 说    明:
*******************************************************************************/
GLIST* gos_list_init
(
    UINT32                  ulMinSize,          /* 队列节点最小个数 */
    UINT32                  ulMaxSize,          /* 队列节点最大个数 */
    UINT32                  ulDataLen,          /* 队列节点数据长度 */
    GOS_LIST_SET_FUNC       pfSetFunc,          /* 队列节点赋值函数 */
    GOS_LIST_ALLOC_FUNC     pfMemAllocFunc,     /* 队列节点malloc函数 */
    GOS_LIST_FREE_FUNC      pfMemFreeFunc,      /* 队列节点free函数 */
    GOS_LIST_FREE_DATA_FUNC pfFreeDataFunc      /* 队列节点数据释放函数 */
)
{
    GOS_LIST_T          *pstList;
    GOS_LIST_NODE_T     *pstNode;
    UINT32              i;
    UINT32              ulSize;

    if (0 == ulDataLen )
    {
        return NULL;
    }

    /* 动态分配一块内存,用于存放GOS_LIST_T结构体 */
    pstList = (GOS_LIST_T*)GOS_ZALLOC(sizeof(GOS_LIST_T));
    if (NULL == pstList)
    {
        return NULL;
    }

    pstList->stOption.ulMinSize = ulMinSize;
    pstList->stOption.ulMaxSize = ulMaxSize;
    pstList->stOption.ulDataLen = ulDataLen;
    pstList->stOption.pfSetFunc = pfSetFunc;
    pstList->stOption.pfMemAllocFunc = pfMemAllocFunc;
    pstList->stOption.pfMemFreeFunc  = pfMemFreeFunc;
    pstList->stOption.pfFreeDataFunc = pfFreeDataFunc;

    /* 设置缺省值 */
    if (NULL == pstList->stOption.pfMemAllocFunc)
    {
        pstList->stOption.pfMemAllocFunc = gos_malloc;
    }

    if (NULL == pstList->stOption.pfMemFreeFunc)
    {
        pstList->stOption.pfMemFreeFunc = gos_free;
    }

    if (0 == pstList->stOption.ulMaxSize)
    {
        pstList->stOption.ulMaxSize = (UINT32)-1;
    }

    if (pstList->stOption.ulMinSize > pstList->stOption.ulMaxSize)
    {
        pstList->stOption.ulMinSize = pstList->stOption.ulMaxSize;
    }

    /* 创建队列的固定部分 */
    if (pstList->stOption.ulMinSize > 0)
    {
        ulSize = (sizeof(GOS_LIST_NODE_T)+pstList->stOption.ulDataLen)*pstList->stOption.ulMinSize;
        pstList->pstNodes = (GOS_LIST_NODE_T*)GOS_ZALLOC(ulSize);
        if (NULL == pstList->pstNodes)
        {
            GOS_FREE(pstList);
            return NULL;
        }

        pstList->ulAllocMemSize += ulSize;

        pstList->pucData = (UINT8*)(pstList->pstNodes + pstList->stOption.ulMinSize);
        pstList->pucDataEnd = pstList->pucData + pstList->stOption.ulDataLen*pstList->stOption.ulMinSize;
    }

    pstList->stFreeList.pstNext = pstList->pstNodes;

    pstNode = pstList->pstNodes;
    for (i=0; i<pstList->stOption.ulMinSize; i++)
    {
        if ((INT32)i < (INT32)(pstList->stOption.ulMinSize-1))
        {
            pstNode->pstNext = (pstNode+1);
        }
        else
        {
            pstNode->pstNext = NULL;    /* 最后一个指向空 */
        }

        pstNode->pvData = pstList->pucData + pstList->stOption.ulDataLen*i;

        pstNode ++;
    }

    pstList->stBusyList.pstPrev = NULL;
    pstList->stBusyList.pstNext = NULL;

    return (GLIST*)pstList;
}

UINT32 gos_list_get_alloc_size(GLIST *pList)
{
    GOS_LIST_T     *pstList = (GOS_LIST_T*)pList;

    if (!pstList)
    {
        return 0;
    }

    return pstList->ulAllocMemSize;
}

/*******************************************************************************
* 函数名称: gos_list_clear
* 功    能: 释放队列中所有节点
* 参    数:
* 参数名称       输入/输出       描述
* pstList        IN              队列句柄
* 函数返回: 成功返回TRUE; 失败返回FALSE
* 说    明:
*******************************************************************************/
VOID gos_list_clear(GLIST* pList)
{
    GOS_LIST_T *pstList = (GOS_LIST_T*)pList;

    if (!pstList)
    {
        return;
    }

    while(gos_list_remove(pList));
}

/*******************************************************************************
* 函数名称: gos_list_free
* 功    能: 释放队列
* 参    数:
* 参数名称       输入/输出       描述
* pstList        IN              队列句柄
* 函数返回: 成功返回TRUE; 失败返回FALSE
* 说    明:
*******************************************************************************/
VOID gos_list_free(GLIST* pList)
{
    GOS_LIST_T *pstList = (GOS_LIST_T*)pList;

    if (!pstList)
    {
        return;
    }

    while(gos_list_remove(pList));

    GOS_FREE(pstList->pstNodes);

    GOS_FREE(pList);
}

/*******************************************************************************
* 函数名称: gos_list_full
* 功    能: 判断队列是否满
* 参    数:
* 参数名称       输入/输出       描述
* pstList        IN              队列句柄
* 函数返回:
* 说    明:
*******************************************************************************/
BOOL gos_list_full(GOS_LIST_T *pstList)
{
    if (0 == pstList->stOption.ulMaxSize)
    {
        return FALSE;
    }

    if (pstList->ulSize >= pstList->stOption.ulMaxSize)
    {
        if (pstList->ulSize > pstList->stOption.ulMaxSize)
        {
            pstList->ulSize = pstList->stOption.ulMaxSize;
        }

        return TRUE;
    }

    return FALSE;
}

/*******************************************************************************
* 函数名称: gos_list_empty
* 功    能: 判断队列是否空
* 参    数:
* 参数名称       输入/输出       描述
* pstList        IN              队列句柄
* 函数返回:
* 说    明:
*******************************************************************************/
BOOL gos_list_empty(GOS_LIST_T *pstList)
{
    if (NULL == pstList->stBusyList.pstNext)
    {
        if (pstList->ulSize > 0)
        {
            pstList->ulSize = 0;
        }

        return TRUE;
    }

    if (pstList->ulSize == 0)
    {
        if (pstList->stBusyList.pstNext)
        {
            pstList->stBusyList.pstNext = NULL;
        }

        return TRUE;
    }

    return FALSE;
}

BOOL gos_list_is_empty(GLIST *pList)
{
    return gos_list_empty((GOS_LIST_T *)pList);
}

BOOL gos_list_is_full(GLIST *pList)
{
    return gos_list_full((GOS_LIST_T *)pList);
}

UINT32 gos_list_get_node_num(GLIST *pList)
{
    GOS_LIST_T *pstList = (GOS_LIST_T*)pList;

    if (!pstList)
    {
        return 0;
    }

    return pstList->ulSize;
}

UINT32 gos_list_get_min_node_num(GLIST *pList)
{
    GOS_LIST_T *pstList = (GOS_LIST_T*)pList;

    if (!pstList)
    {
        return 0;
    }

    return pstList->stOption.ulMinSize;
}

UINT32 gos_list_get_max_node_num(GLIST *pList)
{
    GOS_LIST_T *pstList = (GOS_LIST_T*)pList;

    if (!pstList)
    {
        return 0;
    }

    return pstList->stOption.ulMaxSize;
}

/*******************************************************************************
* 函数名称: gos_list_add
* 功    能: 往队列尾部增加一个节点
* 参    数:
* 参数名称       输入/输出       描述
* pstList        IN              队列句柄
* pcData         IN              新增的数据
* ulDataLen      IN              数据长度
* 函数返回: 增加成功返回TRUE;添加失败返回FALSE
* 说    明:
*******************************************************************************/
GLISTNODE* gos_list_add(GLIST *pList, VOID *pvData, UINT32 ulDataLen)
{
    GOS_LIST_T          *pstList = (GOS_LIST_T*)pList;
    GOS_LIST_NODE_T     *pstNew     = NULL;
    GOS_LIST_NODE_T     *pstNode    = NULL;
    BOOL                bUseFreeList = FALSE;

    if (!pList || gos_list_full(pstList))
    {
        return NULL;
    }

    /* 查询空闲队列 */
    pstNode = pstList->stFreeList.pstNext;
    if (pstNode && ulDataLen <= pstList->stOption.ulDataLen)
    {
        bUseFreeList = TRUE;
    }

    /* 获取/创建队列节点 */
    if (bUseFreeList)
    {
        pstNew = pstNode;
    }
    else
    {
        /* 给该NODE节点分配内存空间 */
        pstNew = (GOS_LIST_NODE_T*)pstList->stOption.pfMemAllocFunc(sizeof(GOS_LIST_NODE_T)+ulDataLen);
        if (!pstNew)
        {
            return NULL;
        }

        pstNew->pvData = (pstNew+1);
    }

    /* 复制数据 */
    if (pstList->stOption.pfSetFunc)
    {
        if (!pstList->stOption.pfSetFunc(pvData, ulDataLen, pstNew->pvData))
        {
            if (!bUseFreeList)
            {
                //GOS_FREE(pstNew);
                pstList->stOption.pfMemFreeFunc(pstNew);
            }

            return NULL;
        }
    }
    else
    {
        gos_list_set_func(pvData, ulDataLen, pstNew->pvData)
    }

    pstNew->ulDataLen = ulDataLen;

    /* 维护free、busy链表 */
    if (bUseFreeList)
    {
        pstList->stFreeList.pstNext = pstNode->pstNext;
        if (pstNode->pstNext)
        {
            pstNode->pstNext->pstPrev = &pstList->stFreeList;
        }
    }

    if (NULL == pstList->stBusyList.pstNext)
    {
        pstNew->pstPrev = &pstList->stBusyList;
        pstNew->pstNext = &pstList->stBusyList;

        pstList->stBusyList.pstNext = pstNew;   /* stBusyList.pstNext为链表尾 */
        pstList->stBusyList.pstPrev = pstNew;   /* stBusyList.pstPrev为链表头 */
    }
    else
    {
        pstNew->pstPrev = &pstList->stBusyList;
        pstNew->pstNext = pstList->stBusyList.pstNext;

        pstList->stBusyList.pstNext->pstPrev = pstNew;
        pstList->stBusyList.pstNext = pstNew;
    }

    pstList->ulSize ++;

    return (GLISTNODE*)pstNew;
}

/*******************************************************************************
* 函数名称: gos_list_remove
* 功    能: 从队列头取出一个节点
* 参    数:
* 参数名称       输入/输出       描述
* pstList        IN              队列句柄
* 函数返回: 成功返回TRUE; 失败返回FALSE
* 说    明:
*******************************************************************************/
BOOL gos_list_remove(GLIST *pList)
{
    GOS_LIST_T          *pstList = (GOS_LIST_T*)pList;

    if (!pstList)
    {
        return FALSE;
    }

    return gos_list_remove_node(pList, (GLISTNODE*)pstList->stBusyList.pstPrev);
}

BOOL gos_list_remove_node(GLIST *pList, GLISTNODE *pNode)
{
    GOS_LIST_T          *pstList = (GOS_LIST_T*)pList;
    GOS_LIST_NODE_T     *pstNode = (GOS_LIST_NODE_T*)pNode;

    if (gos_list_empty(pstList) || !pstNode)
    {
        return FALSE;
    }

    /* 维护忙链表 */
    if (NULL == pstNode->pstPrev || NULL == pstNode->pstNext)
    {
        return FALSE;
    }

    pstNode->pstPrev->pstNext = pstNode->pstNext;
    pstNode->pstNext->pstPrev = pstNode->pstPrev;

    /* 判断是否是静态分配的节点 */
    if ((UINT8*)pstNode->pvData >= pstList->pucData && (UINT8*)pstNode->pvData < pstList->pucDataEnd)
    {
        gos_list_free_node_func(pstList, pstNode, FALSE);

        /* 添加到空闲链表 */
        pstNode->pstNext = pstList->stFreeList.pstNext;
        pstNode->pstPrev = NULL;

        pstList->stFreeList.pstNext = pstNode;
    }
    else
    {
        gos_list_free_node_func(pstList, pstNode, TRUE);
    }

    pstList->ulSize --;
    if (0 == pstList->ulSize)
    {
        pstList->stBusyList.pstNext = NULL;
        pstList->stBusyList.pstPrev = NULL;
    }

    return TRUE;
}

/*******************************************************************************
* 函数名称: gos_list_get_head
* 功    能: 从队列头取出一个节点的内容
* 参    数:
* 参数名称       输入/输出       描述
* pstList        IN              队列句柄
* 函数返回: 成功返回TRUE; 失败返回FALSE
* 说    明:
*******************************************************************************/
VOID* gos_list_get_head(GLIST *pList)
{
    GOS_LIST_T          *pstList = (GOS_LIST_T*)pList;
    GOS_LIST_NODE_T     *pstNode = NULL;

    if (gos_list_empty(pstList))
    {
        return NULL;
    }

    /* 获取队列尾 */
    pstNode = pstList->stBusyList.pstPrev;
    if (NULL == pstNode)
    {
        return NULL;
    }

    return pstNode->pvData;
}

/*******************************************************************************
* 函数名称: gos_list_get_tail
* 功    能: 从队列尾取出一个节点的内容
* 参    数:
* 参数名称       输入/输出       描述
* pstList        IN              队列句柄
* 函数返回: 成功返回TRUE; 失败返回FALSE
* 说    明:
*******************************************************************************/
VOID* gos_list_get_tail(GLIST *pList)
{
    GOS_LIST_T          *pstList = (GOS_LIST_T*)pList;
    GOS_LIST_NODE_T     *pstNode   = NULL;

    if (gos_list_empty(pstList))
    {
        return NULL;
    }

    /* 获取队列尾 */
    pstNode = pstList->stBusyList.pstNext;
    if (NULL == pstNode)
    {
        return NULL;
    }

    return pstNode->pvData;
}

/*******************************************************************************
* 函数名称: gos_list_get_head_node
* 功    能: 从队列头取出第一个节点
* 参    数:
* 参数名称       输入/输出       描述
* pstList        IN              队列句柄
* 函数返回: 返回第一个节点
* 说    明:
*******************************************************************************/
GLISTNODE* gos_list_get_head_node(GLIST *pList)
{
    GOS_LIST_T          *pstList = (GOS_LIST_T*)pList;

    if (gos_list_empty(pstList))
    {
        return NULL;
    }

    /* 获取队列尾 */
    return (GLISTNODE*)pstList->stBusyList.pstPrev;
}

/*******************************************************************************
* 函数名称: gos_list_get_next_node
* 功    能: 获取下一个节点
* 参    数:
* 参数名称       输入/输出       描述
* pstList        IN              队列句柄
* 函数返回: 返回下一个节点
* 说    明:
*******************************************************************************/
GLISTNODE* gos_list_get_next_node(GLIST *pList, GLISTNODE* pNode)
{
    GOS_LIST_T          *pstList = (GOS_LIST_T*)pList;
    GOS_LIST_NODE_T     *pstNode = (GOS_LIST_NODE_T*)pNode;

    if (gos_list_empty(pstList))
    {
        return NULL;
    }

    /* 获取队列尾 */
    if (NULL == pstNode || pstNode == &pstList->stBusyList)
    {
        return NULL;
    }

    /* 队列的最后一个其实是第一个节点，因此应该往前取 */
    pstNode = pstNode->pstPrev;
    if (pstNode == &pstList->stBusyList)
    {
        return NULL;
    }
    else
    {
        return (GLISTNODE*)pstNode;
    }
}

/*******************************************************************************
* 函数名称: gos_list_get_node
* 功    能: 根据下标获取一个节点
* 参    数:
* 参数名称       输入/输出       描述
* pstList        IN              队列句柄
* ulIndex        IN              节点下标
* 函数返回: 返回节点
* 说    明:
*******************************************************************************/
GLISTNODE* gos_list_get_node(GLIST *pList, UINT32 ulIndex)
{
    GLISTNODE           *pNode;
    UINT32              i;

    if (ulIndex >= gos_list_get_node_num(pList))
    {
        return NULL;
    }

    pNode = gos_list_get_head_node(pList);

    for (i=0; i<ulIndex; i++)
    {
        pNode = gos_list_get_next_node(pList, pNode);
    }

    return pNode;
}

/*******************************************************************************
* 函数名称: gos_list_get_node_data
* 功    能: 获取节点内容
* 参    数:
* 参数名称       输入/输出       描述
* pstNode        IN              节点
* 函数返回: 返回下一个节点
* 说    明:
*******************************************************************************/
VOID* gos_list_get_node_data(GLISTNODE* pstNode)
{
    /* 获取队列尾 */
    if (NULL == pstNode)
    {
        return NULL;
    }

    return ((GOS_LIST_NODE_T*)pstNode)->pvData;
}

/*******************************************************************************
* 函数名称: gos_list_get_node_data_len
* 功    能: 获取节点内容长度
* 参    数:
* 参数名称       输入/输出       描述
* pstNode        IN              节点
* 函数返回: 返回下一个节点
* 说    明:
*******************************************************************************/
UINT32 gos_list_get_node_data_len(GLISTNODE* pstNode)
{
    /* 获取队列尾 */
    if (NULL == pstNode)
    {
        return 0;
    }

    return ((GOS_LIST_NODE_T*)pstNode)->ulDataLen;
}

