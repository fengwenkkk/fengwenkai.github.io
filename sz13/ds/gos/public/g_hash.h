#ifndef G_HASH_H
#define G_HASH_H

class GHash
{
public:
    GHash(UINT32 ulMinNodeNum, UINT32 ulMaxNodeNum, UINT32 ulHashEntrySize, UINT32 ulMaxDataLen,
        UINT32 ulKeyFieldOffset, GOS_HASH_FUNC pfHashFunc, GOS_HASH_KEY_COMP_FUNC pfKeyCompFunc,
        GOS_HASH_ALLOC_FUNC pfAllocFunc=gos_malloc, GOS_HASH_FREE_FUNC pfFreeFunc=gos_free);

    GHash(UINT32 ulHashEntrySize, UINT32 ulMaxDataLen,
          GOS_HASH_FUNC pfHashFunc, GOS_HASH_KEY_COMP_FUNC pfKeyCompFunc);

    ~GHash();

    VOID*       Add(VOID* pvData, UINT32 ulDataLen);
    VOID*       Get(VOID* pvKey);

    BOOL        Remove(VOID* pvKey);
    BOOL        Clear();

    INT32       GetUsedNum();

    GHASHNODE*  GetFirstNode();
    GHASHNODE*  GetNextNode(GHASHNODE *pNode);

    VOID*       GetNodeData(GHASHNODE *pNode);
    UINT32      GetNodeDataLen(GHASHNODE *pNode);
    VOID        RemoveNode(GHASHNODE* pNode);

protected:
    GHASH       *m_pHash;

    UINT32      m_ulIndex;
    GHASHNODE   *m_pCurrNode;
};

class GStringHash : public GHash
{
public:
    GStringHash(UINT32 ulMinNodeNum, UINT32 ulMaxNodeNum, UINT32 ulHashEntrySize, UINT32 ulMaxDataLen,
                UINT32 ulKeyFieldOffset, GOS_HASH_ALLOC_FUNC pfAllocFunc=gos_malloc, GOS_HASH_FREE_FUNC pfFreeFunc=gos_free);

    GStringHash(UINT32 ulHashEntrySize, UINT32 ulMaxDataLen);

    VOID*       Get(CHAR *szKey);
    BOOL        Remove(CHAR *szKey);
};

class GIntHash : public GHash
{
public:
    GIntHash(UINT32 ulMinNodeNum, UINT32 ulMaxNodeNum, UINT32 ulHashEntrySize, UINT32 ulMaxDataLen,
                UINT32 ulKeyFieldOffset, GOS_HASH_ALLOC_FUNC pfAllocFunc=gos_malloc, GOS_HASH_FREE_FUNC pfFreeFunc=gos_free);

    GIntHash(UINT32 ulHashEntrySize, UINT32 ulMaxDataLen);

    VOID*       Get(UINT32  ulKey);
    BOOL        Remove(UINT32  ulKey);
};

#endif
