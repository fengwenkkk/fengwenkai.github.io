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

GLIST*          gos_list_init      (UINT32                  ulMinSize,          /* 队列节点最小个数 */
                                    UINT32                  ulMaxSize,          /* 队列节点最大个数 */
                                    UINT32                  ulDataLen,          /* 队列节点数据长度 */
                                    GOS_LIST_SET_FUNC       pfSetFunc,         /* 队列节点赋值函数 */
                                    GOS_LIST_ALLOC_FUNC     pfMemAllocFunc,    /* 队列节点malloc函数 */
                                    GOS_LIST_FREE_FUNC      pfMemFreeFunc,     /* 队列节点free函数 */
                                    GOS_LIST_FREE_DATA_FUNC pfFreeDataFunc);   /* 队列节点数据释放函数 */
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
/******************************* 头文件结束 **********************************/
