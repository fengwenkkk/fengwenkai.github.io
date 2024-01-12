#include "GPublic.h"
#include "GMsgPool.h"
#include "dtp_public.h"

typedef struct
{
    UINT32                      ulMsgID;
    GOS_UNIQUE_MSG_CMP_FUNC     *pfUniqueMsgCmp;
}UNIQUE_MSG_DESC_T;

BOOL        g_bExitWhenMsgQueueIsFull = FALSE;

static MSG_T* AllocMsg(UINT16 usTaskID, MSG_T *pstMsgHdr, VOID *pvMsgBody, UINT32 ulMsgBodyLen)
{
    MSG_T       *pstMsg;
    GMsgPool    *pMsgPool = GMsgPool::GetInstance();

    pstMsg = (MSG_T*)pMsgPool->Alloc(pstMsgHdr, pvMsgBody, ulMsgBodyLen);
    if (!pstMsg)
    {
        return NULL;
    }

    pstMsg->stReceiver.usTaskID = usTaskID;

    return pstMsg;
}

VOID GosFreeMsg(MSG_T *pstMsg)
{
    GMsgPool    *pMsgPool = GMsgPool::GetInstance();

    if (!pMsgPool)
    {
        return;
    }

    pMsgPool->Free(pstMsg);
}

static BOOL GosDefaultUniqueMsgCmp(VOID *pvSrc, VOID *pvDst)
{
    return TRUE;
}

GMsgQueue::GMsgQueue(UINT16 usProcID, UINT32 ulMaxMsgNodeNum)
{
    m_usProcID = usProcID;
    m_Mutex = new GMutex();

    if (ulMaxMsgNodeNum < 32)
    {
        ulMaxMsgNodeNum = 32;
    }

    m_ImpMsgList = new GList(ulMaxMsgNodeNum/2, ulMaxMsgNodeNum, sizeof(MSG_NODE_INFO_T));
    m_NorMsgList = new GList(1, 1, sizeof(MSG_NODE_INFO_T));

    m_UniqueMsgList = new GList(32, 0, sizeof(MSG_NODE_INFO_T));
    m_UniqueMsgDesc = new GIntHash(100, sizeof(UNIQUE_MSG_DESC_T));

    m_ulPrintTime = 0;
}

GList* GMsgQueue::GetMsgQue(MSG_T *pstMsgHdr)
{
    return m_ImpMsgList;
}

MSG_NODE_INFO_T* GMsgQueue::LocateMsgNodeInfo(GList *pMsgQueue, GOS_UNIQUE_MSG_CMP_FUNC *pfUniqueMsgDesc, MSG_T *pstMsgHdr, VOID *pvMsgBody, UINT32 ulMsgBodyLen)
{
    GLISTNODE   *pNode = LocateMsgNode(pMsgQueue, pfUniqueMsgDesc, pstMsgHdr, pvMsgBody, ulMsgBodyLen);

    if (!pNode)
    {
        return NULL;
    }

    return (MSG_NODE_INFO_T*)pMsgQueue->GetNodeData(pNode);
}

GLISTNODE* GMsgQueue::LocateMsgNode(GList *pMsgQueue, GOS_UNIQUE_MSG_CMP_FUNC *pfUniqueMsgDesc, MSG_T *pstMsgHdr, VOID *pvMsgBody, UINT32 ulMsgBodyLen)
{
    GLISTNODE   *pNode = pMsgQueue->GetHeadNode();

    while(pNode)
    {
        MSG_NODE_INFO_T *pstMsgNode = (MSG_NODE_INFO_T*)pMsgQueue->GetNodeData(pNode);

        if (pstMsgNode && pstMsgNode->pstMsg->usMsgID == pstMsgHdr->usMsgID &&
            memcmp(&pstMsgNode->pstMsg->stSender, &pstMsgHdr->stSender, sizeof(PID_T)) == 0)
        {
            if (pfUniqueMsgDesc(pstMsgNode->pstMsg->aucMsgData, pvMsgBody))
            {
                return pNode;
            }
        }

        pNode = pMsgQueue->GetNextNode(pNode);
    }

    return NULL;
}

BOOL GMsgQueue::MergeMsg(MSG_NODE_INFO_T *pstUniqueMsgNode, GOS_UNIQUE_MSG_CMP_FUNC *pfUniqueMsgDesc, MSG_T *pstMsgHdr, VOID *pvMsgBody, UINT32 ulMsgBodyLen)
{
    MSG_NODE_INFO_T     *pstMsgNode;

    pstMsgNode = LocateMsgNodeInfo(m_ImpMsgList, pfUniqueMsgDesc, pstMsgHdr, pvMsgBody, ulMsgBodyLen);
    if (!pstMsgNode)
    {
        pstMsgNode = LocateMsgNodeInfo(m_NorMsgList, pfUniqueMsgDesc, pstMsgHdr, pvMsgBody, ulMsgBodyLen);
        if (!pstMsgNode)
        {
            return TRUE;
        }
    }

    MSG_T   *pstNewMsg = AllocMsg(pstMsgNode->pstMsg->stReceiver.usTaskID, pstMsgHdr, pvMsgBody, ulMsgBodyLen);

    if (!pstNewMsg)
    {
        return FALSE;
    }

    GosFreeMsg(pstMsgNode->pstMsg);
    pstMsgNode->pstMsg = pstNewMsg;         // 更新数据
    pstUniqueMsgNode->pstMsg = pstNewMsg;

    return TRUE;
}

BOOL GMsgQueue::InitUniqueMsg(UINT16 usMsgID, GOS_UNIQUE_MSG_CMP_FUNC *pfUniqueMsgCmp)
{
    if (NULL == pfUniqueMsgCmp)
    {
        pfUniqueMsgCmp = GosDefaultUniqueMsgCmp;
    }

    UNIQUE_MSG_DESC_T       stMsgDesc = {0};

    stMsgDesc.ulMsgID = usMsgID;
    stMsgDesc.pfUniqueMsgCmp = pfUniqueMsgCmp;

    return (m_UniqueMsgDesc->Add(&stMsgDesc, sizeof(stMsgDesc)) != NULL);
}

BOOL GMsgQueue::AddMsg(UINT16 usTaskID, MSG_T *pstMsgHdr, VOID *pvMsgBody, UINT32 ulMsgBodyLen)
{
    MSG_T               *pstMsg;
    GList               *pMsgQueue;
    MSG_NODE_INFO_T     *pstMsgNode;

    if (!m_Mutex->Mutex())
    {
        return FALSE;
    }

    pMsgQueue = GetMsgQue(pstMsgHdr);
    if (!pMsgQueue)
    {
        m_Mutex->Unmutex();
        return FALSE;
    }

    // 判断是否为唯一性消息
    UNIQUE_MSG_DESC_T   *pstMsgDesc = (UNIQUE_MSG_DESC_T*)m_UniqueMsgDesc->Get(pstMsgHdr->usMsgID);

    if (pstMsgDesc)
    {
        pstMsgNode = LocateMsgNodeInfo(m_UniqueMsgList, pstMsgDesc->pfUniqueMsgCmp, pstMsgHdr, pvMsgBody, ulMsgBodyLen);
        if (pstMsgNode)
        {
            BOOL bRet = MergeMsg(pstMsgNode, pstMsgDesc->pfUniqueMsgCmp, pstMsgHdr, pvMsgBody, ulMsgBodyLen);

            m_Mutex->Unmutex();
            return bRet;
        }
    }

    if (pMsgQueue->IsFull())
    {
        GosLog(LOG_FATAL, "msg queue of proc %u is full, msg %u from %u,%u is discarded!", m_usProcID, pstMsgHdr->usMsgID,
            pstMsgHdr->stSender.usInstID, pstMsgHdr->stSender.usTaskID);

        PrintMsgStat(TRUE);
        PrintMsgQueue();

        if (g_bExitWhenMsgQueueIsFull)
        {
            GosLog(LOG_FATAL, "auto exist when msg queue of proc %u is full", m_usProcID);
        //  gos_sleep_1ms(10);
            exit(-1);
        }

        m_Mutex->Unmutex();

        return FALSE;
    }

    pstMsg = AllocMsg(usTaskID, pstMsgHdr, pvMsgBody, ulMsgBodyLen);
    if (!pstMsg)
    {
        GosLog(LOG_FAIL, "alloc msg failed when add msg to queue");
        m_Mutex->Unmutex();
        return FALSE;
    }

    MSG_NODE_INFO_T     stMsgNode = {0};

    stMsgNode.pstMsg = pstMsg;
    stMsgNode.ulMsgDeadline = gos_get_uptime() + pstMsg->ulTimeout;
    gos_gettimeofday(&stMsgNode.stTimeVal);
    if (!pMsgQueue->Add(&stMsgNode, sizeof(stMsgNode)))
    {
        GosLog(LOG_FAIL, "add msg to queue failed");
        GosFreeMsg(pstMsg);
        m_Mutex->Unmutex();
        return FALSE;
    }

    if (pstMsgDesc)
    {
        m_UniqueMsgList->Add(&stMsgNode, sizeof(stMsgNode));
    }

    PrintMsgStat(FALSE);

    m_Mutex->Unmutex();

    return TRUE;
}

BOOL GMsgQueue::AddMsg(UINT16 usTaskID, MSG_T *pstMsg)
{
    return AddMsg(usTaskID, pstMsg, pstMsg->aucMsgData, pstMsg->ulMsgLen);
}

VOID GMsgQueue::RemoveUniqueMsg(MSG_T *pstMsg)
{
    UNIQUE_MSG_DESC_T   *pstMsgDesc = (UNIQUE_MSG_DESC_T*)m_UniqueMsgDesc->Get(pstMsg->usMsgID);

    if (pstMsgDesc)
    {
        GLISTNODE *pNode = LocateMsgNode(m_UniqueMsgList, pstMsgDesc->pfUniqueMsgCmp, pstMsg, pstMsg->aucMsgData, pstMsg->ulMsgLen);

        if (pNode)
        {
            m_UniqueMsgList->RemoveNode(pNode);
        }
    }
}

MSG_T* GMsgQueue::GetMsg(GList *pMsgQueue)
{
    MSG_NODE_INFO_T     *pstMsgNode;
    MSG_T               *pstMsg = NULL;
    UINT32              ulCurrTime = gos_get_uptime();
    UINT32              ulMsgDeadline;

    if (!pMsgQueue)
    {
        return NULL;
    }

    while(1)
    {
        pstMsgNode = (MSG_NODE_INFO_T*)pMsgQueue->GetHead();
        if (!pstMsgNode)
        {
            return NULL;
        }

        pstMsg = pstMsgNode->pstMsg;
        ulMsgDeadline = pstMsgNode->ulMsgDeadline;
        pMsgQueue->Remove();
        RemoveUniqueMsg(pstMsg);

        if (pstMsg->ulTimeout == 0 || ulCurrTime < ulMsgDeadline)
        {
            return pstMsg;
        }
        else
        {
            GosFreeMsg(pstMsg);
        }
    }
}

MSG_T* GMsgQueue::GetMsg()
{
    MSG_T               *pstMsg = NULL;

    if (!m_Mutex->Mutex())
    {
        return NULL;
    }

    pstMsg = GetMsg(m_ImpMsgList);
    if (!pstMsg)
    {
        pstMsg = GetMsg(m_NorMsgList);
    }

    m_Mutex->Unmutex();

    return pstMsg;
}

VOID GMsgQueue::PrintMsgQueue()
{
    GList       *apList[] = {m_ImpMsgList, m_NorMsgList};

    for (UINT32 i=0; i<ARRAY_SIZE(apList); i++)
    {
        GList           *pList = apList[i];
        GLISTNODE       *pNode = pList->GetHeadNode();
        MSG_NODE_INFO_T *pstMsg;
        CHAR            acMsgInfo[256];
        UINT32          ulMsgLen;
        CHAR            acTime[32];

        while(pNode)
        {
            pstMsg = (MSG_NODE_INFO_T*)pList->GetNodeData(pNode);

            memset(acMsgInfo, 0, sizeof(acMsgInfo));
            ulMsgLen = pstMsg->pstMsg->ulMsgLen;
            if (ulMsgLen >= sizeof(acMsgInfo))
            {
                ulMsgLen = sizeof(acMsgInfo) - 1;
            }

            if (ulMsgLen > 0)
            {
                memcpy(acMsgInfo, pstMsg->pstMsg->aucMsgData, ulMsgLen);
                if (!isascii(acMsgInfo[0]))
                {
                    acMsgInfo[0] = '\0';
                }
            }

            gos_asctimeval(&pstMsg->stTimeVal, acTime);
            GosLog(LOG_DETAIL, "msg queue: id=0x%X, time=%s, info=%s", pstMsg->pstMsg->usMsgID, acTime, acMsgInfo);

            pNode = pList->GetNextNode(pNode);
        }
    }
}

VOID GMsgQueue::PrintMsgStat(BOOL bPrintNow)
{
    UINT32  ulCurrTime = gos_get_uptime();

    if (!bPrintNow && m_ulPrintTime > 0 &&
        ulCurrTime < (m_ulPrintTime+10))
    {
        return;
    }

    m_ulPrintTime = ulCurrTime;

    MAP<UINT32, UINT32>             mMsgStat;
    std::map<UINT32, UINT32>::iterator   it;

    MsgStat(mMsgStat);

    for (it=mMsgStat.begin(); it!=mMsgStat.end(); it++)
    {
        UINT32  ulMsgID = it->first;
        UINT32  ulMsgNum = it->second;

        if (ulMsgNum > 10)
        {
            GosLog(LOG_INFO, "msg of proc %u: msg id = %u, msg num = %u", m_usProcID, ulMsgID, ulMsgNum);
        }
    }
}

BOOL GMsgQueue::MsgStat(MAP<UINT32, UINT32> &mMsgStat)
{
    MSG_T               *pstMsg;
    GList               *apMsgQueue[] = {m_ImpMsgList, m_NorMsgList};
    MSG_NODE_INFO_T     *pstMsgNode;
    GLISTNODE           *pNode;

    mMsgStat.clear();

    for (UINT32 i=0; i<ARRAY_SIZE(apMsgQueue); i++)
    {
        pNode = (GLISTNODE*)apMsgQueue[i]->GetHeadNode();
        while(pNode)
        {
            pstMsgNode = (MSG_NODE_INFO_T*)apMsgQueue[i]->GetNodeData(pNode);
            pstMsg = pstMsgNode->pstMsg;
            if (FIND_MAP_KEY(mMsgStat, pstMsg->usMsgID))
            {
                mMsgStat[pstMsg->usMsgID] ++;
            }
            else
            {
                mMsgStat[pstMsg->usMsgID] = 1;
            }

            pNode = apMsgQueue[i]->GetNextNode(pNode);
        }
    }

    return TRUE;
}

#if 0
static BOOL _Msg1UniqueMsgCmp(VOID *pvSrc, VOID *pvDst)
{
    return *((UINT8*)pvSrc) == *((UINT8*)pvDst);
}

void test_GMsgQueue()
{
    GMsgQueue   *que = new GMsgQueue();

    que->InitUniqueMsg(1, _Msg1UniqueMsgCmp);
    que->InitUniqueMsg(2, _Msg1UniqueMsgCmp);

    MSG_T   astMsg[64];
    MSG_T   *pstMsg = astMsg;

    pstMsg->usMsgID = 1;
    pstMsg->aucMsgData[0] = 1;
    que->AddMsg(pstMsg);
    que->PrintMsgQueue();

    pstMsg++;
    pstMsg->usMsgID = 1;
    pstMsg->aucMsgData[0] = 1;
    que->AddMsg(pstMsg);
    que->PrintMsgQueue();

    que->GetMsg();
    que->PrintMsgQueue();

    que->GetMsg();
    que->PrintMsgQueue();

    delete que;
}
#endif
