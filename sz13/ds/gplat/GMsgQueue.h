#ifndef MSG_QUEUE_H
#define MSG_QUEUE_H

typedef struct
{
    MSG_T           *pstMsg;
    UINT32          ulMsgDeadline;  // 消息过期时间,单位秒
    TIMEVAL         stTimeVal;
}MSG_NODE_INFO_T;

class GMsgQueue
{
public:
    GMsgQueue(UINT16 usProcID, UINT32 ulMaxMsgNodeNum);

    BOOL            InitUniqueMsg(UINT16 usMsgID, GOS_UNIQUE_MSG_CMP_FUNC *pfUniqueMsgCmp=NULL);

    BOOL            AddMsg(UINT16 usTaskID, MSG_T *pstMsgHdr, VOID *pvMsgBody, UINT32 ulMsgBodyLen);
    BOOL            AddMsg(UINT16 usTaskID, MSG_T *pstMsg);
    MSG_T*          GetMsg();

    VOID            PrintMsgStat(BOOL bPrintNow);
    VOID            PrintMsgQueue();
    BOOL            MsgStat(MAP<UINT32, UINT32> &vMsgStat);

private:
    GLISTNODE*       LocateMsgNode(GList *pMsgQueue, GOS_UNIQUE_MSG_CMP_FUNC *pfUniqueMsgDesc, MSG_T *pstMsgHdr, VOID *pvMsgBody, UINT32 ulMsgBodyLen);
    MSG_NODE_INFO_T* LocateMsgNodeInfo(GList *pMsgQueue, GOS_UNIQUE_MSG_CMP_FUNC *pfUniqueMsgDesc, MSG_T *pstMsgHdr, VOID *pvMsgBody, UINT32 ulMsgBodyLen);
    BOOL             MergeMsg(MSG_NODE_INFO_T *pstMsgNode, GOS_UNIQUE_MSG_CMP_FUNC *pfUniqueMsgCmp, MSG_T *pstMsgHdr, VOID *pvMsgBody, UINT32 ulMsgBodyLen);
    VOID             RemoveUniqueMsg(MSG_T *pstMsg);
    GList*           GetMsgQue(MSG_T *pstMsgHdr);
    MSG_T*           GetMsg(GList *pMsgQueue);

    UINT16          m_usProcID;
    GList           *m_ImpMsgList;
    GList           *m_NorMsgList;
    GMutex          *m_Mutex;
    GList           *m_UniqueMsgList;   // 唯一消息队列
    UINT32          m_ulUniqueMsgSeqID;
    UINT32          m_ulPrintTime;

    GIntHash        *m_UniqueMsgDesc;
};

#endif
