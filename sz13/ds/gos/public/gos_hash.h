#ifndef _GOS_HASH_H
#define _GOS_HASH_H

#ifndef __cplusplus
#define GOS_HASH_FOREACH(pHash, pNode, pNextNode)\
    pNextNode =NULL; \
    for (pNode = gos_hash_get_first(pHash);\
         pNode && ((pNextNode = gos_hash_get_next(pHash, pNode)) || TRUE); \
         pNode=pNextNode)
#else
#define GOS_HASH_FOREACH(pHash, pNode)\
    pNode = gos_hash_get_first(pHash);\
    for (GHASHNODE*  ___pNextNode___=NULL; \
         pNode && ((___pNextNode___ = gos_hash_get_next(pHash, pNode)) || TRUE); \
         pNode=___pNextNode___)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GOS_HASH_      GHASH;
typedef struct _GOS_HASHNODE_  GHASHNODE;

typedef UINT32  (*GOS_HASH_FUNC)(VOID* pKey);
typedef BOOL    (*GOS_HASH_KEY_COMP_FUNC)(VOID* pSrcKey, VOID *pDestKey);
typedef VOID*   (*GOS_HASH_ALLOC_FUNC)(UINT32 ulLen);
typedef VOID    (*GOS_HASH_FREE_FUNC)(VOID* pData);
typedef VOID    (*GOS_HASH_FOREACH_FUNC)(VOID* pKey, VOID *pValue, VOID *pData);
typedef BOOL    (*GOS_HASH_FOREACH_REMOVE_FUNC)(VOID* pKey, VOID *pValue, VOID *pData);

UINT32      gos_hash_byte       (VOID* pKey);
UINT32      gos_hash_byte3      (VOID* pKey);
UINT32      gos_hash_byte5      (VOID* pKey);
UINT32      gos_hash_byte6      (VOID* pKey);
UINT32      gos_hash_byte7      (VOID* pKey);
UINT32      gos_hash_short      (VOID *pKey);
UINT32      gos_hash_int        (VOID *pKey);
UINT32      gos_hash_int64      (VOID *pKey);
UINT32      gos_hash_ptr        (VOID *pKey);
UINT32      gos_hash_string     (VOID *pKey);
UINT32      gos_hash_string_bkcr(VOID *pKey);
UINT32      gos_hash_string_js  (VOID *pKey);
UINT32      gos_hash_string_rs  (VOID *pKey);
BOOL        gos_hash_key_cmp_byte   (VOID* pSrcKey, VOID *pDestKey);
BOOL        gos_hash_key_cmp_byte3  (VOID* pSrcKey, VOID *pDestKey);
BOOL        gos_hash_key_cmp_byte5  (VOID* pSrcKey, VOID *pDestKey);
BOOL        gos_hash_key_cmp_byte6  (VOID* pSrcKey, VOID *pDestKey);
BOOL        gos_hash_key_cmp_byte7  (VOID* pSrcKey, VOID *pDestKey);
BOOL        gos_hash_key_cmp_short  (VOID* pSrcKey, VOID *pDestKey);
BOOL        gos_hash_key_cmp_int    (VOID* pSrcKey, VOID *pDestKey);
BOOL        gos_hash_key_cmp_int64  (VOID* pSrcKey, VOID *pDestKey);
BOOL        gos_hash_key_cmp_ptr    (VOID* pSrcKey, VOID *pDestKey);
BOOL        gos_hash_key_cmp_string (VOID* pSrcKey, VOID *pDestKey);

GHASH*      gos_hash_init       (UINT32                 ulMinNodeNum,
                                 UINT32                 ulMaxNodeNum,
                                 UINT32                 ulHashEntrySize,
                                 UINT32                 ulMaxDataLen,
                                 UINT32                 ulKeyFieldOffset,
                                 GOS_HASH_FUNC          pfHashFunc,
                                 GOS_HASH_KEY_COMP_FUNC pfKeyCompFunc,
                                 GOS_HASH_ALLOC_FUNC    pfAllocNodeFunc,
                                 GOS_HASH_FREE_FUNC     pfFreeNodeFunc);
UINT32      gos_hash_get_alloc_size (GHASH *hHashQue);
BOOL        gos_hash_set_size       (GHASH* hHashQue, UINT32 ulMaxNodeNum);
BOOL        gos_hash_set_entry_size (GHASH* hHashQue, UINT32 ulEntrySize);
VOID*       gos_hash_add            (GHASH* hHashQue, VOID* Data, UINT32 ulDataLen);
VOID*       gos_hash_add_int        (GHASH* hHashQue, VOID* pData, UINT32 ulDataLen);
VOID*       gos_hash_add_int64      (GHASH* hHashQue, VOID* pData, UINT32 ulDataLen);
VOID*       gos_hash_get            (GHASH* hHashQue, VOID* pKey);
VOID*       gos_hash_get_int        (GHASH* hHashQue, UINT32 ulKey);
VOID*       gos_hash_get_int64      (GHASH* hHashQue, UINT64 u64Key);
VOID*       gos_hash_get_string     (GHASH* hHashQue, CHAR *szKey);
BOOL        gos_hash_remove         (GHASH* hHashQue, VOID* pKey);
BOOL        gos_hash_remove_int     (GHASH* hHashQue, UINT32 ulKey);
BOOL        gos_hash_remove_int64   (GHASH* hHashQue, UINT64 u64Key);
BOOL        gos_hash_remove_string  (GHASH* hHashQue, CHAR* szKey);
BOOL        gos_hash_remove_all     (GHASH* hHashQue);
BOOL        gos_hash_free           (GHASH* hHashQue);
INT32       gos_hash_get_node_num   (GHASH* hHashQue);
INT32       gos_hash_get_static_node_num(GHASH* hHashQue);

BOOL        gos_hash_foreach        (GHASH* hHashQue, GOS_HASH_FOREACH_FUNC pfFunc, VOID *pData);
BOOL        gos_hash_foreach_remove (GHASH* hHashQue, GOS_HASH_FOREACH_REMOVE_FUNC pfFunc, VOID *pData);
GHASHNODE*  gos_hash_get_first      (GHASH* hHashQue);
GHASHNODE*  gos_hash_get_next       (GHASH* hHashQue, GHASHNODE* pNode);
VOID*       gos_hash_get_node_data  (GHASHNODE* pNode);
UINT32      gos_hash_get_node_data_len(GHASHNODE* pNode);
VOID        gos_hash_remove_node    (GHASH* hHashQueQue, GHASHNODE* pNode);
GHASHNODE*  gos_hash_next           (GHASH* hHashQue, GHASHNODE* pNode, GHASHNODE **ppNext);

#ifdef __cplusplus
}
#endif

#endif /* _GOS_HASH_H */
/******************************* 头文件结束 **********************************/
