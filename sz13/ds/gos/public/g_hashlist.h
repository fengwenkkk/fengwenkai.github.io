#ifndef G_HASHLIST_H
#define G_HASHLIST_H

class GHashList
{
public:
    GHashList(UINT32 ulMinNodeNum, UINT32 ulMaxNodeNum, UINT32 ulHashEntrySize, UINT32 ulMaxDataLen,
        UINT32 ulKeyFieldOffset, GOS_HASH_FUNC pfHashFunc, GOS_HASH_KEY_COMP_FUNC pfKeyCompFunc,
        GOS_HASH_ALLOC_FUNC pfAllocFunc=gos_malloc, GOS_HASH_FREE_FUNC pfFreeFunc=gos_free);

    GHashList(UINT32 ulHashEntrySize, UINT32 ulMaxDataLen,
              GOS_HASH_FUNC pfHashFunc, GOS_HASH_KEY_COMP_FUNC pfKeyCompFunc);

    ~GHashList();

    VOID*       Add(VOID* pvData, UINT32 ulDataLen);
    VOID*       Get(VOID* pvKey);

    BOOL        Remove(VOID* pvKey);
    BOOL        Clear();

    INT32       GetUsedNum();

    GHASHNODE*  GetFirstNode();
    GHASHNODE*  GetLastNode();
    GHASHNODE*  GetNextNode(GHASHNODE *pNode);
    GHASHNODE*  GetPrevNode(GHASHNODE *pNode);

    VOID*       GetNodeData(GHASHNODE *pNode);
    UINT32      GetNodeDataLen(GHASHNODE *pNode);
    VOID        RemoveNode(GHASHNODE* pNode);
    VOID        RemoveFirstNode();

    VOID*       Data(UINT32 ulIndex, UINT32 *pulDataLen=NULL);

protected:
    GHASH       *m_pHash;

    UINT32      m_ulIndex;
    GHASHNODE   *m_pCurrNode;

private:
    VOID*       GetValueByIndex(UINT32 ulIndex, UINT32 *pulDataLen=NULL);
};

class GStringHashList : public GHashList
{
public:
    GStringHashList(UINT32 ulMinNodeNum, UINT32 ulMaxNodeNum, UINT32 ulHashEntrySize, UINT32 ulMaxDataLen,
                UINT32 ulKeyFieldOffset, GOS_HASH_ALLOC_FUNC pfAllocFunc=gos_malloc, GOS_HASH_FREE_FUNC pfFreeFunc=gos_free);

    GStringHashList(UINT32 ulHashEntrySize, UINT32 ulMaxDataLen);

    VOID*       Get(CHAR *szKey);
    BOOL        Remove(CHAR *szKey);
};

class GIntHashList : public GHashList
{
public:
    GIntHashList(UINT32 ulMinNodeNum, UINT32 ulMaxNodeNum, UINT32 ulHashEntrySize, UINT32 ulMaxDataLen,
                UINT32 ulKeyFieldOffset, GOS_HASH_ALLOC_FUNC pfAllocFunc=gos_malloc, GOS_HASH_FREE_FUNC pfFreeFunc=gos_free);

    GIntHashList(UINT32 ulHashEntrySize, UINT32 ulMaxDataLen);

    VOID*       Get(UINT32  ulKey);
    BOOL        Remove(UINT32  ulKey);
};

#endif
