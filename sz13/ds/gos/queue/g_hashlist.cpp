#include "g_include.h"
#include "g_hash.h"
#include "g_hashlist.h"

GHashList::GHashList(UINT32 ulMinNodeNum, UINT32 ulMaxNodeNum, UINT32 ulHashEntrySize, UINT32 ulMaxDataLen,
             UINT32 ulKeyFieldOffset, GOS_HASH_FUNC pfHashFunc, GOS_HASH_KEY_COMP_FUNC pfKeyCompFunc,
             GOS_HASH_ALLOC_FUNC pfAllocFunc, GOS_HASH_FREE_FUNC pfFreeFunc)

{
    m_pHash = gos_hashlist_init(ulMinNodeNum, ulMaxNodeNum, ulHashEntrySize, ulMaxDataLen, ulKeyFieldOffset, pfHashFunc, pfKeyCompFunc, pfAllocFunc, pfFreeFunc);
    m_ulIndex = 0;
    m_pCurrNode = NULL;
}

GHashList::GHashList(UINT32 ulHashEntrySize, UINT32 ulMaxDataLen,
          GOS_HASH_FUNC pfHashFunc, GOS_HASH_KEY_COMP_FUNC pfKeyCompFunc)
{
    m_pHash = gos_hashlist_init(0, 0, ulHashEntrySize, ulMaxDataLen, 0, pfHashFunc, pfKeyCompFunc, gos_malloc, gos_free);
    m_ulIndex = 0;
    m_pCurrNode = NULL;
}

GHashList::~GHashList()
{
    gos_hashlist_free(m_pHash);
}

VOID* GHashList::Add(VOID* pvData, UINT32 ulDataLen)
{
    m_ulIndex = 0;
    m_pCurrNode = NULL;
    return gos_hashlist_add(m_pHash, pvData, ulDataLen);
}

VOID* GHashList::Get(VOID* pvKey)
{
    return gos_hashlist_get(m_pHash, pvKey);
}

BOOL GHashList::Remove(VOID* pvKey)
{
    m_ulIndex = 0;
    m_pCurrNode = NULL;
    return gos_hashlist_remove(m_pHash, pvKey);
}

BOOL GHashList::Clear()
{
    m_ulIndex = 0;
    m_pCurrNode = NULL;
    return gos_hashlist_remove_all(m_pHash);
}

INT32 GHashList::GetUsedNum()
{
    return gos_hashlist_get_node_num(m_pHash);
}

GHASHNODE* GHashList::GetFirstNode()
{
    return gos_hashlist_get_first(m_pHash);
}

GHASHNODE* GHashList::GetLastNode()
{
    return gos_hashlist_get_last(m_pHash);
}

GHASHNODE* GHashList::GetNextNode(GHASHNODE* pNode)
{
    return gos_hashlist_get_next(m_pHash, pNode);
}

GHASHNODE* GHashList::GetPrevNode(GHASHNODE* pNode)
{
    return gos_hashlist_get_prev(m_pHash, pNode);
}

VOID* GHashList::GetNodeData(GHASHNODE* pNode)
{
    return gos_hashlist_get_node_data(pNode);
}

UINT32 GHashList::GetNodeDataLen(GHASHNODE* pNode)
{
    return gos_hashlist_get_node_data_len(pNode);
}

VOID GHashList::RemoveNode(GHASHNODE* pNode)
{
    m_ulIndex = 0;
    m_pCurrNode = NULL;
    gos_hashlist_remove_node(m_pHash, pNode);
}

VOID GHashList::RemoveFirstNode()
{
    GHASHNODE *pNode = GetFirstNode();

    RemoveNode(pNode);
}

VOID* GHashList::GetValueByIndex(UINT32 ulIndex, UINT32 *pulDataLen)
{
    GHASHNODE   *pNode;

    if ((INT32)ulIndex >= GetUsedNum())
    {
        return NULL;
    }

    pNode = gos_hashlist_get_first(m_pHash);
    for (UINT32 i=0; i<ulIndex; i++)
    {
        pNode = gos_hashlist_get_next(m_pHash, pNode);
    }

    m_ulIndex = ulIndex;
    m_pCurrNode = pNode;

    if (pulDataLen)
    {
        *pulDataLen = gos_hashlist_get_node_data_len(pNode);
    }

    return gos_hashlist_get_node_data(pNode);
}

VOID* GHashList::Data(UINT32 ulIndex, UINT32 *pulDataLen)
{
    if (ulIndex == 0)
    {
        return GetValueByIndex(0, pulDataLen);
    }

    if (ulIndex != (m_ulIndex+1) ||
        m_pCurrNode == NULL)
    {
        return GetValueByIndex(ulIndex, pulDataLen);
    }

    m_ulIndex = ulIndex;
    m_pCurrNode = gos_hashlist_get_next(m_pHash, m_pCurrNode);

    if (pulDataLen)
    {
        *pulDataLen = gos_hashlist_get_node_data_len(m_pCurrNode);
    }

    return gos_hashlist_get_node_data(m_pCurrNode);
}

/* GStringHash */
GStringHashList::GStringHashList(UINT32 ulMinNodeNum, UINT32 ulMaxNodeNum, UINT32 ulHashEntrySize, UINT32 ulMaxDataLen,
                         UINT32 ulKeyFieldOffset, GOS_HASH_ALLOC_FUNC pfAllocFunc, GOS_HASH_FREE_FUNC pfFreeFunc):
            GHashList(ulMinNodeNum, ulMaxNodeNum, ulHashEntrySize, ulMaxDataLen, ulKeyFieldOffset, gos_hash_string, gos_hash_key_cmp_string, pfAllocFunc, pfFreeFunc)
{
}

GStringHashList::GStringHashList(UINT32 ulHashEntrySize, UINT32 ulMaxDataLen):
            GHashList(0, 0, ulHashEntrySize, ulMaxDataLen, 0, gos_hash_string, gos_hash_key_cmp_string, gos_malloc, gos_free)
{
}

VOID* GStringHashList::Get(CHAR *szKey)
{
    return gos_hashlist_get_string(m_pHash, szKey);
}

BOOL GStringHashList::Remove(CHAR *szKey)
{
    m_ulIndex = 0;
    m_pCurrNode = NULL;
    return gos_hashlist_remove_string(m_pHash, szKey);
}

GIntHashList::GIntHashList(UINT32 ulMinNodeNum, UINT32 ulMaxNodeNum, UINT32 ulHashEntrySize, UINT32 ulMaxDataLen,
                         UINT32 ulKeyFieldOffset, GOS_HASH_ALLOC_FUNC pfAllocFunc, GOS_HASH_FREE_FUNC pfFreeFunc):
            GHashList(ulMinNodeNum, ulMaxNodeNum, ulHashEntrySize, ulMaxDataLen, ulKeyFieldOffset, gos_hash_string, gos_hash_key_cmp_string, pfAllocFunc, pfFreeFunc)
{
}
GIntHashList::GIntHashList(UINT32 ulHashEntrySize, UINT32 ulMaxDataLen):
            GHashList(0, 0, ulHashEntrySize, ulMaxDataLen, 0, gos_hash_int, gos_hash_key_cmp_int, gos_malloc, gos_free)
{
}

VOID* GIntHashList::Get(UINT32 ulKey)
{
    return gos_hashlist_get_int(m_pHash, ulKey);
}

BOOL GIntHashList::Remove(UINT32 ulkey)
{
    m_ulIndex = 0;
    m_pCurrNode = NULL;
    return gos_hashlist_remove_int(m_pHash, ulkey);
}

#if 0

BOOL utest_GHashList()
{
    GHashList   *pHash = new GHashList(50,200, 3, 8, 0, gos_hash_int, gos_hash_key_cmp_int);
    UINT32  aulData[100][3];
    UINT32  i;
    GHASHNODE *pNode;
    GHASHNODE *pNextNode;
    UINT32  *pulData;
    UINT32  ulNum;
    UINT32  ulLen;
    BOOL    bRet = FALSE;
    UINT64  u64Time;

    gos_time_init();

    for (i=0; i<100; i++)
    {
        aulData[i][0] = i;
        aulData[i][1] = gos_get_rand();
        aulData[i][2] = gos_get_rand();
    }

    // insert
    for (i=0; i<100; i++)
    {
        pHash->Add(aulData[i], 8);
    }

    ulNum = pHash->GetUsedNum();

    // get
    u64Time = gos_get_uptime_1ns();

    pNode = pHash->GetFirstNode();
    i = 0;
    while(pNode)
    {
        pulData = (UINT32*)pHash->GetNodeData(pNode);

        if (pulData[0] != aulData[i][0] ||
            pulData[1] != aulData[i][1] )
        {
            goto end;
        }

        pNode = pHash->GetNextNode(pNode);
        i++;
    }

    u64Time = gos_get_uptime_1ns() - u64Time;

    printf(FORMAT_U64"\n", u64Time);

    ulNum = pHash->GetUsedNum();

    // get by index
    u64Time = gos_get_uptime_1ns();

    for (i=0; i<ulNum; i++)
    {
        pulData = (UINT32*)pHash->Data(i, &ulLen);

        if (!pulData ||
            pulData[0] != aulData[i][0] ||
            pulData[1] != aulData[i][1] )
        {
            goto end;
        }
    }

    u64Time = gos_get_uptime_1ns() - u64Time;
    printf(FORMAT_U64"\n", u64Time);

    // remove
    pNode = pHash->GetFirstNode();
    while(pNode)
    {
        pulData = (UINT32*)pHash->GetNodeData(pNode);
        pNextNode = pHash->GetNextNode(pNode);

        pHash->RemoveNode(pNode);

        pNode = pNextNode;
    }

    ulNum = pHash->GetUsedNum();

    // insert and update
    for (i=0; i<100; i++)
    {
        pHash->Add(aulData[i], 6);
    }

    for (i=0; i<100; i++)
    {
        pHash->Add(aulData[i], 12);
    }

    ulNum = pHash->GetUsedNum();
    for (i=0; i<ulNum; i++)
    {
        pulData = (UINT32*)pHash->Data(i, &ulLen);

        if (!pulData ||
            pulData[0] != aulData[i][0] ||
            pulData[1] != aulData[i][1] ||
            pulData[2] != aulData[i][2] )
        {
            goto end;
        }
    }

    bRet = TRUE;

end:
    delete pHash;

    return bRet;
}

#endif
