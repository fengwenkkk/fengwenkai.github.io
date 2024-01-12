#include "g_include.h"
#include "g_hash.h"

GHash::GHash(UINT32 ulMinNodeNum, UINT32 ulMaxNodeNum, UINT32 ulHashEntrySize, UINT32 ulMaxDataLen,
             UINT32 ulKeyFieldOffset, GOS_HASH_FUNC pfHashFunc, GOS_HASH_KEY_COMP_FUNC pfKeyCompFunc,
             GOS_HASH_ALLOC_FUNC pfAllocFunc, GOS_HASH_FREE_FUNC pfFreeFunc)

{
    m_pHash = gos_hash_init(ulMinNodeNum, ulMaxNodeNum, ulHashEntrySize, ulMaxDataLen, ulKeyFieldOffset, pfHashFunc, pfKeyCompFunc, pfAllocFunc, pfFreeFunc);
    m_ulIndex = 0;
    m_pCurrNode = NULL;
}

GHash::GHash(UINT32 ulHashEntrySize, UINT32 ulMaxDataLen,
          GOS_HASH_FUNC pfHashFunc, GOS_HASH_KEY_COMP_FUNC pfKeyCompFunc)
{
    m_pHash = gos_hash_init(0, 0, ulHashEntrySize, ulMaxDataLen, 0, pfHashFunc, pfKeyCompFunc, gos_malloc, gos_free);
    m_ulIndex = 0;
    m_pCurrNode = NULL;
}

GHash::~GHash()
{
    gos_hash_free(m_pHash);
}

VOID* GHash::Add(VOID* pvData, UINT32 ulDataLen)
{
    m_ulIndex = 0;
    m_pCurrNode = NULL;
    return gos_hash_add(m_pHash, pvData, ulDataLen);
}

VOID* GHash::Get(VOID* pvKey)
{
    return gos_hash_get(m_pHash, pvKey);
}

BOOL GHash::Remove(VOID* pvKey)
{
    m_ulIndex = 0;
    m_pCurrNode = NULL;
    return gos_hash_remove(m_pHash, pvKey);
}

BOOL GHash::Clear()
{
    m_ulIndex = 0;
    m_pCurrNode = NULL;
    return gos_hash_remove_all(m_pHash);
}

INT32 GHash::GetUsedNum()
{
    return gos_hash_get_node_num(m_pHash);
}

GHASHNODE* GHash::GetFirstNode()
{
    return gos_hash_get_first(m_pHash);
}

GHASHNODE* GHash::GetNextNode(GHASHNODE* pNode)
{
    return gos_hash_get_next(m_pHash, pNode);
}

VOID* GHash::GetNodeData(GHASHNODE* pNode)
{
    return gos_hash_get_node_data(pNode);
}

UINT32 GHash::GetNodeDataLen(GHASHNODE* pNode)
{
    return gos_hash_get_node_data_len(pNode);
}

VOID GHash::RemoveNode(GHASHNODE* pNode)
{
    m_ulIndex = 0;
    m_pCurrNode = NULL;
    gos_hash_remove_node(m_pHash, pNode);
}

/* GStringHash */
GStringHash::GStringHash(UINT32 ulMinNodeNum, UINT32 ulMaxNodeNum, UINT32 ulHashEntrySize, UINT32 ulMaxDataLen,
                         UINT32 ulKeyFieldOffset, GOS_HASH_ALLOC_FUNC pfAllocFunc, GOS_HASH_FREE_FUNC pfFreeFunc):
            GHash(ulMinNodeNum, ulMaxNodeNum, ulHashEntrySize, ulMaxDataLen, ulKeyFieldOffset, gos_hash_string, gos_hash_key_cmp_string, pfAllocFunc, pfFreeFunc)
{
}

GStringHash::GStringHash(UINT32 ulHashEntrySize, UINT32 ulMaxDataLen):
            GHash(0, 0, ulHashEntrySize, ulMaxDataLen, 0, gos_hash_string, gos_hash_key_cmp_string, gos_malloc, gos_free)
{
}

VOID* GStringHash::Get(CHAR *szKey)
{
    return gos_hash_get_string(m_pHash, szKey);
}

BOOL GStringHash::Remove(CHAR *szKey)
{
    m_ulIndex = 0;
    m_pCurrNode = NULL;
    return gos_hash_remove_string(m_pHash, szKey);
}

GIntHash::GIntHash(UINT32 ulMinNodeNum, UINT32 ulMaxNodeNum, UINT32 ulHashEntrySize, UINT32 ulMaxDataLen,
                         UINT32 ulKeyFieldOffset, GOS_HASH_ALLOC_FUNC pfAllocFunc, GOS_HASH_FREE_FUNC pfFreeFunc):
            GHash(ulMinNodeNum, ulMaxNodeNum, ulHashEntrySize, ulMaxDataLen, ulKeyFieldOffset, gos_hash_string, gos_hash_key_cmp_string, pfAllocFunc, pfFreeFunc)
{
}
GIntHash::GIntHash(UINT32 ulHashEntrySize, UINT32 ulMaxDataLen):
            GHash(0, 0, ulHashEntrySize, ulMaxDataLen, 0, gos_hash_int, gos_hash_key_cmp_int, gos_malloc, gos_free)
{
}

VOID* GIntHash::Get(UINT32 ulKey)
{
    return gos_hash_get_int(m_pHash, ulKey);
}

BOOL GIntHash::Remove(UINT32 ulkey)
{
    m_ulIndex = 0;
    m_pCurrNode = NULL;
    return gos_hash_remove_int(m_pHash, ulkey);
}

#if 0

BOOL utest_GHash()
{
    GHash   *pHash = new GHash(3, 8, gos_hash_int, gos_hash_key_cmp_int);
    UINT32  aulData[100][2];
    UINT32  i;
    GHASHNODE *pNode;
    GHASHNODE *pNextNode;
    UINT32  *pulData;
    UINT32  ulNum;
    BOOL    bRet = FALSE;
    UINT64  u64Time;

    gos_time_init();

    for (i=0; i<100; i++)
    {
        aulData[i][0] = i;
        aulData[i][1] = gos_get_rand();
    }

    for (i=0; i<100; i++)
    {
        pHash->Add(aulData[i], 8);
    }

    ulNum = pHash->GetUsedNum();

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

    pNode = pHash->GetFirstNode();
    while(pNode)
    {
        pulData = (UINT32*)pHash->GetNodeData(pNode);
        pNextNode = pHash->GetNextNode(pNode);

        pHash->RemoveNode(pNode);

        pNode = pNextNode;
    }

    ulNum = pHash->GetUsedNum();

    bRet = TRUE;

end:
    delete pHash;

    return bRet;
}

#endif
