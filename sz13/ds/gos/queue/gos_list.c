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
    UINT32                  ulMinSize;          /* ���нڵ���С���� */
    UINT32                  ulMaxSize;          /* ���нڵ������� */
    UINT32                  ulDataLen;          /* ���нڵ����ݳ��� */

    GOS_LIST_SET_FUNC       pfSetFunc;
    GOS_LIST_ALLOC_FUNC     pfMemAllocFunc;
    GOS_LIST_FREE_FUNC      pfMemFreeFunc;
    GOS_LIST_FREE_DATA_FUNC pfFreeDataFunc;    /* ���нڵ������ͷź��� */
}GOS_LIST_OPTION_T;

typedef struct
{
    UINT32                  ulSize;         /* ��ǰ���и��� */

    GOS_LIST_OPTION_T       stOption;

    GOS_LIST_NODE_T         stFreeList;     /* ����������Ԫ */
    GOS_LIST_NODE_T         stBusyList;     /* æ������Ԫ */

    GOS_LIST_NODE_T         *pstNodes;
    UINT8                   *pucData;       /* ��̬����ʱ�����ݴ�ŵĵ�ַ */
    UINT8                   *pucDataEnd;

    UINT32                  ulAllocMemSize;
}GOS_LIST_T;

/*******************************************************************************
* ��������: list_free_node_func
* ��    ��: �ͷŽڵ���ڴ�ռ�
* ��    ��:
* ��������       ����/���       ����
* pstList        IN              ����ָ��
* pNode          IN              ���ͷŵĽڵ�ָ��
* bFreeNode      IN              �Ƿ���Ҫ�ͷŽڵ㱾��
* ��������: �ͷ��ڴ�ɹ�����TRUE���ͷ��ڴ�ʧ�ܷ���FALSE
* ˵    ��:
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

    /* ���ͷ�pData�ڲ���ָ�� */
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
* ��������: gos_list_cmp_int
* ��    ��: int���������ȽϺ���
* ��    ��:
* ��������       ����/���       ����
* pSrc           IN              ����1
* pDst           IN              ����2
* BOOL           OUT
* ��������: ��ͬ����TRUE����ͬ����FALSE
* ˵    ��:
*******************************************************************************/
BOOL gos_list_cmp_int(VOID* pvSrc, VOID *pvDst)
{
    return (*((INT32*)pvSrc) == *((INT32*)pvDst));
}

/*******************************************************************************
* ��������: gos_list_set_func
* ��    ��: ���ݸ��ƺ���
* ��    ��:
* ��������       ����/���       ����
* pSrc           IN              ��������Դ
* ulDataLen      IN              ����Դ����
* pDst           IN              ��������Ŀ��
* ��������:
* ˵    ��:
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
* ��������: list_init
* ��    ��: ���г�ʼ��
* ��    ��:
* ��������       ����/���       ����
* ��������: ���ض��о��
* ˵    ��:
*******************************************************************************/
GLIST* gos_list_init
(
    UINT32                  ulMinSize,          /* ���нڵ���С���� */
    UINT32                  ulMaxSize,          /* ���нڵ������� */
    UINT32                  ulDataLen,          /* ���нڵ����ݳ��� */
    GOS_LIST_SET_FUNC       pfSetFunc,          /* ���нڵ㸳ֵ���� */
    GOS_LIST_ALLOC_FUNC     pfMemAllocFunc,     /* ���нڵ�malloc���� */
    GOS_LIST_FREE_FUNC      pfMemFreeFunc,      /* ���нڵ�free���� */
    GOS_LIST_FREE_DATA_FUNC pfFreeDataFunc      /* ���нڵ������ͷź��� */
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

    /* ��̬����һ���ڴ�,���ڴ��GOS_LIST_T�ṹ�� */
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

    /* ����ȱʡֵ */
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

    /* �������еĹ̶����� */
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
            pstNode->pstNext = NULL;    /* ���һ��ָ��� */
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
* ��������: gos_list_clear
* ��    ��: �ͷŶ��������нڵ�
* ��    ��:
* ��������       ����/���       ����
* pstList        IN              ���о��
* ��������: �ɹ�����TRUE; ʧ�ܷ���FALSE
* ˵    ��:
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
* ��������: gos_list_free
* ��    ��: �ͷŶ���
* ��    ��:
* ��������       ����/���       ����
* pstList        IN              ���о��
* ��������: �ɹ�����TRUE; ʧ�ܷ���FALSE
* ˵    ��:
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
* ��������: gos_list_full
* ��    ��: �ж϶����Ƿ���
* ��    ��:
* ��������       ����/���       ����
* pstList        IN              ���о��
* ��������:
* ˵    ��:
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
* ��������: gos_list_empty
* ��    ��: �ж϶����Ƿ��
* ��    ��:
* ��������       ����/���       ����
* pstList        IN              ���о��
* ��������:
* ˵    ��:
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
* ��������: gos_list_add
* ��    ��: ������β������һ���ڵ�
* ��    ��:
* ��������       ����/���       ����
* pstList        IN              ���о��
* pcData         IN              ����������
* ulDataLen      IN              ���ݳ���
* ��������: ���ӳɹ�����TRUE;���ʧ�ܷ���FALSE
* ˵    ��:
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

    /* ��ѯ���ж��� */
    pstNode = pstList->stFreeList.pstNext;
    if (pstNode && ulDataLen <= pstList->stOption.ulDataLen)
    {
        bUseFreeList = TRUE;
    }

    /* ��ȡ/�������нڵ� */
    if (bUseFreeList)
    {
        pstNew = pstNode;
    }
    else
    {
        /* ����NODE�ڵ�����ڴ�ռ� */
        pstNew = (GOS_LIST_NODE_T*)pstList->stOption.pfMemAllocFunc(sizeof(GOS_LIST_NODE_T)+ulDataLen);
        if (!pstNew)
        {
            return NULL;
        }

        pstNew->pvData = (pstNew+1);
    }

    /* �������� */
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

    /* ά��free��busy���� */
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

        pstList->stBusyList.pstNext = pstNew;   /* stBusyList.pstNextΪ����β */
        pstList->stBusyList.pstPrev = pstNew;   /* stBusyList.pstPrevΪ����ͷ */
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
* ��������: gos_list_remove
* ��    ��: �Ӷ���ͷȡ��һ���ڵ�
* ��    ��:
* ��������       ����/���       ����
* pstList        IN              ���о��
* ��������: �ɹ�����TRUE; ʧ�ܷ���FALSE
* ˵    ��:
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

    /* ά��æ���� */
    if (NULL == pstNode->pstPrev || NULL == pstNode->pstNext)
    {
        return FALSE;
    }

    pstNode->pstPrev->pstNext = pstNode->pstNext;
    pstNode->pstNext->pstPrev = pstNode->pstPrev;

    /* �ж��Ƿ��Ǿ�̬����Ľڵ� */
    if ((UINT8*)pstNode->pvData >= pstList->pucData && (UINT8*)pstNode->pvData < pstList->pucDataEnd)
    {
        gos_list_free_node_func(pstList, pstNode, FALSE);

        /* ��ӵ��������� */
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
* ��������: gos_list_get_head
* ��    ��: �Ӷ���ͷȡ��һ���ڵ������
* ��    ��:
* ��������       ����/���       ����
* pstList        IN              ���о��
* ��������: �ɹ�����TRUE; ʧ�ܷ���FALSE
* ˵    ��:
*******************************************************************************/
VOID* gos_list_get_head(GLIST *pList)
{
    GOS_LIST_T          *pstList = (GOS_LIST_T*)pList;
    GOS_LIST_NODE_T     *pstNode = NULL;

    if (gos_list_empty(pstList))
    {
        return NULL;
    }

    /* ��ȡ����β */
    pstNode = pstList->stBusyList.pstPrev;
    if (NULL == pstNode)
    {
        return NULL;
    }

    return pstNode->pvData;
}

/*******************************************************************************
* ��������: gos_list_get_tail
* ��    ��: �Ӷ���βȡ��һ���ڵ������
* ��    ��:
* ��������       ����/���       ����
* pstList        IN              ���о��
* ��������: �ɹ�����TRUE; ʧ�ܷ���FALSE
* ˵    ��:
*******************************************************************************/
VOID* gos_list_get_tail(GLIST *pList)
{
    GOS_LIST_T          *pstList = (GOS_LIST_T*)pList;
    GOS_LIST_NODE_T     *pstNode   = NULL;

    if (gos_list_empty(pstList))
    {
        return NULL;
    }

    /* ��ȡ����β */
    pstNode = pstList->stBusyList.pstNext;
    if (NULL == pstNode)
    {
        return NULL;
    }

    return pstNode->pvData;
}

/*******************************************************************************
* ��������: gos_list_get_head_node
* ��    ��: �Ӷ���ͷȡ����һ���ڵ�
* ��    ��:
* ��������       ����/���       ����
* pstList        IN              ���о��
* ��������: ���ص�һ���ڵ�
* ˵    ��:
*******************************************************************************/
GLISTNODE* gos_list_get_head_node(GLIST *pList)
{
    GOS_LIST_T          *pstList = (GOS_LIST_T*)pList;

    if (gos_list_empty(pstList))
    {
        return NULL;
    }

    /* ��ȡ����β */
    return (GLISTNODE*)pstList->stBusyList.pstPrev;
}

/*******************************************************************************
* ��������: gos_list_get_next_node
* ��    ��: ��ȡ��һ���ڵ�
* ��    ��:
* ��������       ����/���       ����
* pstList        IN              ���о��
* ��������: ������һ���ڵ�
* ˵    ��:
*******************************************************************************/
GLISTNODE* gos_list_get_next_node(GLIST *pList, GLISTNODE* pNode)
{
    GOS_LIST_T          *pstList = (GOS_LIST_T*)pList;
    GOS_LIST_NODE_T     *pstNode = (GOS_LIST_NODE_T*)pNode;

    if (gos_list_empty(pstList))
    {
        return NULL;
    }

    /* ��ȡ����β */
    if (NULL == pstNode || pstNode == &pstList->stBusyList)
    {
        return NULL;
    }

    /* ���е����һ����ʵ�ǵ�һ���ڵ㣬���Ӧ����ǰȡ */
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
* ��������: gos_list_get_node
* ��    ��: �����±��ȡһ���ڵ�
* ��    ��:
* ��������       ����/���       ����
* pstList        IN              ���о��
* ulIndex        IN              �ڵ��±�
* ��������: ���ؽڵ�
* ˵    ��:
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
* ��������: gos_list_get_node_data
* ��    ��: ��ȡ�ڵ�����
* ��    ��:
* ��������       ����/���       ����
* pstNode        IN              �ڵ�
* ��������: ������һ���ڵ�
* ˵    ��:
*******************************************************************************/
VOID* gos_list_get_node_data(GLISTNODE* pstNode)
{
    /* ��ȡ����β */
    if (NULL == pstNode)
    {
        return NULL;
    }

    return ((GOS_LIST_NODE_T*)pstNode)->pvData;
}

/*******************************************************************************
* ��������: gos_list_get_node_data_len
* ��    ��: ��ȡ�ڵ����ݳ���
* ��    ��:
* ��������       ����/���       ����
* pstNode        IN              �ڵ�
* ��������: ������һ���ڵ�
* ˵    ��:
*******************************************************************************/
UINT32 gos_list_get_node_data_len(GLISTNODE* pstNode)
{
    /* ��ȡ����β */
    if (NULL == pstNode)
    {
        return 0;
    }

    return ((GOS_LIST_NODE_T*)pstNode)->ulDataLen;
}

