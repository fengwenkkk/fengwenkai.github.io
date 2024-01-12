#ifndef G_LIST_H
#define G_LIST_H

class GList
{
public:
    GList(UINT32 ulMinSize, UINT32 ulMaxSize, UINT32 ulDataLen,
          GOS_LIST_SET_FUNC pfSetFunc=NULL, GOS_LIST_ALLOC_FUNC pfMemAllocFunc=gos_malloc,
          GOS_LIST_FREE_FUNC pfMemFreeFunc=gos_free, GOS_LIST_FREE_DATA_FUNC pfFreeDataFunc=NULL);

    GList(UINT32 ulDataLen);

    ~GList();

    VOID            Clear();
    GLISTNODE*      Add(VOID *pvData, UINT32 ulDataLen);
    BOOL            Remove();
    VOID*           GetHead();
    VOID*           GetTail();
    BOOL            IsEmpty();
    BOOL            IsFull();

    GLISTNODE*      GetHeadNode();
    GLISTNODE*      GetNextNode(GLISTNODE* pstNode);
    GLISTNODE*      GetNode(UINT32 ulIndex);
    BOOL            RemoveNode(GLISTNODE *pstNode);

    VOID*           GetNodeData (GLISTNODE* pstNode);
    UINT32          GetNodeDataLen(GLISTNODE* pstNode);

    UINT32          GetUsedNum();
    UINT32          GetMinNum();
    UINT32          GetMaxNum();

private:
    GLIST           *m_pList;
};

#endif
