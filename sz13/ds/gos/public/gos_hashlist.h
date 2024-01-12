#ifndef _GOS_HASHLIST_H
#define _GOS_HASHLIST_H

#include "gos_hash.h"

#ifndef __cplusplus
#define GOS_HASHLIST_FOREACH(pHash, pNode, pNextNode)\
    pNextNode =NULL; \
    for (pNode = gos_hashlist_get_first(pHash);\
         pNode && ((pNextNode = gos_hashlist_get_next(pHash, pNode)) || TRUE); \
         pNode=pNextNode)
#else
#define GOS_HASHLIST_FOREACH(pHash, pNode)\
    pNode = gos_hashlist_get_first(pHash);\
    for (GHASHNODE*  ___pNextNode___=NULL; \
         pNode && ((___pNextNode___ = gos_hashlist_get_next(pHash, pNode)) || TRUE); \
         pNode=___pNextNode___)
#endif

#ifdef __cplusplus
extern "C" {
#endif

GHASH*      gos_hashlist_init   (UINT32                 ulMinNodeNum,
                                 UINT32                 ulMaxNodeNum,
                                 UINT32                 ulHashEntrySize,
                                 UINT32                 ulMaxDataLen,
                                 UINT32                 ulKeyFieldOffset,
                                 GOS_HASH_FUNC          pfHashFunc,
                                 GOS_HASH_KEY_COMP_FUNC pfKeyCompFunc,
                                 GOS_HASH_ALLOC_FUNC    pfAllocNodeFunc,
                                 GOS_HASH_FREE_FUNC     pfFreeNodeFunc);
UINT32      gos_hashlist_get_alloc_size (GHASH *hHashQue);
VOID*       gos_hashlist_add            (GHASH* hHashQue, VOID* Data, UINT32 ulDataLen);
VOID*       gos_hashlist_add_int        (GHASH* hHashQue, VOID* pData, UINT32 ulDataLen);
VOID*       gos_hashlist_add_int64      (GHASH* hHashQue, VOID* pData, UINT32 ulDataLen);
VOID*       gos_hashlist_get            (GHASH* hHashQue, VOID* pKey);
VOID*       gos_hashlist_get_int        (GHASH* hHashQue, UINT32 ulKey);
VOID*       gos_hashlist_get_int64      (GHASH* hHashQue, UINT64 u64Key);
VOID*       gos_hashlist_get_string     (GHASH* hHashQue, CHAR *szKey);
BOOL        gos_hashlist_remove         (GHASH* hHashQue, VOID* pKey);
BOOL        gos_hashlist_remove_int     (GHASH* hHashQue, UINT32 ulKey);
BOOL        gos_hashlist_remove_int64   (GHASH* hHashQue, UINT64 u64Key);
BOOL        gos_hashlist_remove_string  (GHASH* hHashQue, CHAR* szKey);
BOOL        gos_hashlist_remove_all     (GHASH* hHashQue);
BOOL        gos_hashlist_free           (GHASH* hHashQue);
INT32       gos_hashlist_get_node_num   (GHASH* hHashQue);
INT32       gos_hashlist_get_static_node_num(GHASH* hHashQue);

BOOL        gos_hashlist_foreach        (GHASH* hHashQue, GOS_HASH_FOREACH_FUNC pfFunc, VOID *pData);
BOOL        gos_hashlist_foreach_remove (GHASH* hHashQue, GOS_HASH_FOREACH_REMOVE_FUNC pfFunc, VOID *pData);
GHASHNODE*  gos_hashlist_get_first      (GHASH* hHashQue);
GHASHNODE*  gos_hashlist_get_last       (GHASH* hHashQue);
GHASHNODE*  gos_hashlist_get_next       (GHASH* hHashQue, GHASHNODE* pNode);
GHASHNODE*  gos_hashlist_get_prev       (GHASH* hHashQue, GHASHNODE* pNode);
VOID*       gos_hashlist_get_node_data  (GHASHNODE* pNode);
UINT32      gos_hashlist_get_node_data_len(GHASHNODE* pNode);
VOID        gos_hashlist_remove_node    (GHASH* hHashQueQue, GHASHNODE* pNode);
VOID        gos_hashlist_remove_first   (GHASH* hHashQue);
GHASHNODE*  gos_hashlist_next           (GHASH* hHashQue, GHASHNODE* pNode, GHASHNODE **ppNext);
GHASHNODE*  gos_hashlist_prev           (GHASH* hHashQue, GHASHNODE* pNode, GHASHNODE **ppNext);

#ifdef __cplusplus
}
#endif

#endif /* _GOS_HASHLIST_H */
/******************************* 头文件结束 **********************************/
