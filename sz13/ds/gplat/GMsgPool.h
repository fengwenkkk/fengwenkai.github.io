#ifndef MSG_POOL_H
#define MSG_POOL_H

class GMsgPool
{
public:
    static GMsgPool* GetInstance();

    MSG_T*  Alloc(MSG_T *pstMsgHdr, VOID *pvMsgBody, UINT32 ulMsgBodyLen);
    MSG_T*  Alloc(MSG_T *pstMsg);
    VOID    Free(MSG_T* pstMsg);

    BOOL    GetMsgStat(UINT32 *pulMsgNum, UINT64 *pu64MsgSize);

private:
    GMutex      *m_Mutex;
    HANDLE      m_MemPool;
    UINT32      m_ulMsgNum;
    UINT64      m_u64MsgSize;

    GMsgPool();
    ~GMsgPool();

    VOID    StatMsg(UINT32 ulMsgLen, BOOL bAlloc);
};

#endif
