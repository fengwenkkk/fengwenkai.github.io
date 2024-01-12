#include "g_include.h"

GList::GList(UINT32 ulMinSize, UINT32 ulMaxSize, UINT32 ulDataLen,
             GOS_LIST_SET_FUNC pfSetFunc, GOS_LIST_ALLOC_FUNC pfMemAllocFunc,
             GOS_LIST_FREE_FUNC pfMemFreeFunc, GOS_LIST_FREE_DATA_FUNC pfFreeDataFunc)

{
    m_pList = gos_list_init(ulMinSize, ulMaxSize, ulDataLen, pfSetFunc, pfMemAllocFunc, pfMemFreeFunc, pfFreeDataFunc);
}

GList::GList(UINT32 ulDataLen)
{
    m_pList = gos_list_init(0, 0, ulDataLen, NULL, gos_malloc, gos_free, NULL);
}

GList::~GList()
{
    gos_list_free(m_pList);
}

VOID GList::Clear()
{
    gos_list_clear(m_pList);
}

GLISTNODE* GList::Add(VOID *pvData, UINT32 ulDataLen)
{
    return gos_list_add(m_pList, pvData, ulDataLen);
}

BOOL GList::Remove()
{
    return gos_list_remove(m_pList);
}

VOID* GList::GetHead()
{
    return gos_list_get_head(m_pList);
}

VOID* GList::GetTail()
{
    return gos_list_get_tail(m_pList);
}

BOOL GList::IsEmpty()
{
    return gos_list_is_empty(m_pList);
}

BOOL GList::IsFull()
{
    return gos_list_is_full(m_pList);
}

GLISTNODE* GList::GetHeadNode()
{
    return gos_list_get_head_node(m_pList);
}

GLISTNODE* GList::GetNextNode(GLISTNODE* pstNode)
{
    return gos_list_get_next_node(m_pList, pstNode);
}

GLISTNODE* GList::GetNode(UINT32 ulIndex)
{
    return gos_list_get_node(m_pList, ulIndex);
}

BOOL GList::RemoveNode(GLISTNODE *pstNode)
{
    return gos_list_remove_node(m_pList, pstNode);
}

VOID* GList::GetNodeData(GLISTNODE* pstNode)
{
    return gos_list_get_node_data(pstNode);
}

UINT32 GList::GetNodeDataLen(GLISTNODE* pstNode)
{
    return gos_list_get_node_data_len(pstNode);
}

UINT32 GList::GetUsedNum()
{
    return gos_list_get_node_num(m_pList);
}

UINT32 GList::GetMinNum()
{
    return gos_list_get_min_node_num(m_pList);
}

UINT32 GList::GetMaxNum()
{
    return gos_list_get_max_node_num(m_pList);
}
