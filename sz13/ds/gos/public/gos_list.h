#ifndef _GOS_LIST_H
#define _GOS_LIST_H

#ifdef __cplusplus
extern "C"{
#endif

typedef struct _GOS_LIST        GLIST;
typedef struct _GOS_LISTNODE    GLISTNODE;

typedef BOOL    (*GOS_LIST_SET_FUNC)(VOID* pvSrc, UINT32 ulLen, VOID *pvDst);
typedef VOID*   (*GOS_LIST_ALLOC_FUNC)(UINT32 ulMemSize);
typedef VOID    (*GOS_LIST_FREE_FUNC)(VOID* pvData);
typedef BOOL    (*GOS_LIST_FREE_DATA_FUNC)(VOID* pvData);

GLIST*          gos_list_init      (UINT32                  ulMinSize,          /* ���нڵ���С���� */
                                    UINT32                  ulMaxSize,          /* ���нڵ������� */
                                    UINT32                  ulDataLen,          /* ���нڵ����ݳ��� */
                                    GOS_LIST_SET_FUNC       pfSetFunc,         /* ���нڵ㸳ֵ���� */
                                    GOS_LIST_ALLOC_FUNC     pfMemAllocFunc,    /* ���нڵ�malloc���� */
                                    GOS_LIST_FREE_FUNC      pfMemFreeFunc,     /* ���нڵ�free���� */
                                    GOS_LIST_FREE_DATA_FUNC pfFreeDataFunc);   /* ���нڵ������ͷź��� */
UINT32          gos_list_get_alloc_size(GLIST *pList);
VOID            gos_list_clear      (GLIST *pList);
VOID            gos_list_free       (GLIST *pstList);
GLISTNODE*      gos_list_add        (GLIST *pstList, VOID *pvData, UINT32 ulDataLen);
BOOL            gos_list_remove     (GLIST *pstList);
VOID*           gos_list_get_head   (GLIST *pList);
VOID*           gos_list_get_tail   (GLIST *pList);
BOOL            gos_list_is_empty   (GLIST *pList);
BOOL            gos_list_is_full    (GLIST *pList);

BOOL            gos_list_remove_node   (GLIST *pList, GLISTNODE *pNode);
GLISTNODE*      gos_list_get_head_node (GLIST *pList);
GLISTNODE*      gos_list_get_next_node (GLIST *pList, GLISTNODE* pstNode);
GLISTNODE*      gos_list_get_node      (GLIST *pList, UINT32 ulIndex);
VOID*           gos_list_get_node_data (GLISTNODE* pstNode);
UINT32          gos_list_get_node_data_len(GLISTNODE* pstNode);
UINT32          gos_list_get_node_num     (GLIST *pList);
UINT32          gos_list_get_min_node_num (GLIST *pList);
UINT32          gos_list_get_max_node_num (GLIST *pList);

#ifdef __cplusplus
}
#endif

#endif /* _GOS_LIST_H */
/******************************* ͷ�ļ����� **********************************/
