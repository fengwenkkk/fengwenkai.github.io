#ifndef _GOS_HLIST_H
#define _GOS_HLIST_H

#ifdef __cplusplus
extern "C"{
#endif

typedef struct _GOS_HLIST_          GHLIST;
typedef struct _GOS_HLIST_NODE_     GHLISTNODE;

typedef VOID*   (GOS_HLIST_MEMALLOC_FUNC)(UINT32 ulMemSize);
typedef VOID    (GOS_HLIST_MEMFREE_FUNC)(VOID* pvData);

GHLIST*         gos_hlist_init     (UINT32                  ulMinSize,          /* 队列节点最小个数 */
                                    UINT32                  ulMaxSize,          /* 队列节点最大个数 */
                                    UINT32                  ulHashEntrySize,    /* Hash桶大小 */
                                    UINT32                  ulMaxDataLen,       /* 队列节点数据长度 */
                                    UINT32                  ulKeyFieldOffset,
                                    GOS_HASH_FUNC           pfHashFunc,
                                    GOS_HASH_KEY_COMP_FUNC  pfKeyCompFunc,
                                    GOS_HLIST_MEMALLOC_FUNC pfMemAllocFunc,    /* 队列节点malloc函数 */
                                    GOS_HLIST_MEMFREE_FUNC  pfMemFreeFunc);    /* 队列节点free函数 */
GHLIST*         gos_hlist_init_ex  (UINT32                  ulMinSize,          /* 队列节点最小个数 */
                                    UINT32                  ulMaxSize,          /* 队列节点最大个数 */
                                    UINT32                  ulHashEntrySize,    /* Hash桶大小 */
                                    UINT32                  ulMaxDataLen,       /* 队列节点数据长度 */
                                    UINT32                  ulKeyFieldOffset,
                                    GOS_HASH_FUNC           pfHashFunc,
                                    GOS_HASH_KEY_COMP_FUNC  pfKeyCompFunc,
                                    GOS_HLIST_MEMALLOC_FUNC pfMemAllocFunc,     /* 队列节点malloc函数 */
                                    GOS_HLIST_MEMFREE_FUNC  pfMemFreeFunc,
                                    UINT32                  ulListNum);

UINT32          gos_hlist_get_alloc_size(GHLIST* pList);
BOOL            gos_hlist_free          (GHLIST* pList);
INT32           gos_hlist_get_node_num  (GHLIST* pList);
INT32           gos_hlist_get_static_node_num(GHLIST* pList);

GHLISTNODE*     gos_hlist_add_ex        (GHLIST* pList, VOID* pData, UINT32 ulDataLen, UINT32 ulListIndex);
GHLISTNODE*     gos_hlist_add_int_ex    (GHLIST* pList, VOID* pData, UINT32 ulDataLen, UINT32 ulListIndex);

VOID*           gos_hlist_get           (GHLIST* pList, VOID* pKey);
VOID*           gos_hlist_get_int       (GHLIST* pList, UINT32 ulKey);
VOID*           gos_hlist_get_string    (GHLIST* pList, CHAR *szKey);

BOOL            gos_hlist_remove        (GHLIST* pList, VOID* pKey);
BOOL            gos_hlist_remove_int    (GHLIST* pList, UINT32 ulKey);
BOOL            gos_hlist_remove_string (GHLIST* pList, CHAR* szKey);
BOOL            gos_hlist_remove_all    (GHLIST* pList);

BOOL            gos_hlist_set_list_index(GHLIST* hHashQue, GHLISTNODE* hNode, UINT32 ulNewListIndex);
BOOL            gos_hlist_get_list_num  (GHLIST* hHashQue, UINT32 ulListIndex);

VOID*           gos_hlist_get_head_ex     (GHLIST *pList, UINT32 ulListIndex);      /* 获取队列头内容 */
GHLISTNODE*     gos_hlist_get_head_node_ex(GHLIST *pList, UINT32 ulListIndex);      /* 删除队列头 */
BOOL            gos_hlist_remove_head_ex  (GHLIST *pList, UINT32 ulListIndex);      /* 删除队列头 */

VOID*           gos_hlist_get_tail_ex     (GHLIST *pList, UINT32 ulListIndex);      /* 获取队列尾内容 */
GHLISTNODE*     gos_hlist_get_tail_node_ex(GHLIST *pList, UINT32 ulListIndex);      /* 删除队列尾 */
BOOL            gos_hlist_remove_tail_ex  (GHLIST *pList, UINT32 ulListIndex);      /* 删除队列尾 */

GHLISTNODE*     gos_hlist_get_node      (GHLIST* pList, VOID* pKey);
GHLISTNODE*     gos_hlist_get_next_node (GHLIST *pList, GHLISTNODE* pNode);
GHLISTNODE*     gos_hlist_get_prev_node (GHLIST *pList, GHLISTNODE* pNode);
VOID*           gos_hlist_get_node_data (GHLISTNODE* pNode);
UINT32          gos_hlist_get_node_list_index(GHLISTNODE* pNode);

VOID            gos_hlist_remove_node   (GHLIST *pList, GHLISTNODE *pNode);


#define         gos_hlist_add(pList, pData, ulDataLen)      gos_hlist_add_ex(pList, pData, ulDataLen, 0)
#define         gos_hlist_add_int(pList, pData, ulDataLen)  gos_hlist_add_int_ex(pList, pData, ulDataLen, 0)
#define         gos_hlist_get_head_node(pList)              gos_hlist_get_head_node_ex(pList, 0)
#define         gos_hlist_get_head(pList)                   gos_hlist_get_head_ex(pList, 0)
#define         gos_hlist_remove_head(pList)                gos_hlist_remove_head_ex(pList, 0)

#define         gos_hlist_get_tail_node(pList)              gos_hlist_get_tail_node_ex(pList, 0)
#define         gos_hlist_get_tail(pList)                   gos_hlist_get_tail_ex(pList, 0)
#define         gos_hlist_remove_tail(pList)                gos_hlist_remove_tail_ex(pList, 0)

#ifdef __cplusplus
}
#endif

#endif /* _GOS_HLIST_H */
/******************************* 头文件结束 **********************************/
