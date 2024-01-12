#ifndef GOS_PROC_H
#define GOS_PROC_H

#define PROC_STATUS_RUNNING     0
#define PROC_STATUS_SUSPEND     1
#define PROC_STATUS_RESUME      2

class GProc: public GThread
{
public:
    static GProc *GetProc(UINT16 usProcID);

    virtual GOS_THREAD_RET ThreadEntry(void* pPara);

    UINT16 GetProcID(){return m_usProcID;}

    BOOL AddMsg(UINT16 usTaskID, MSG_T *pstMsgHdr, VOID *pvMsgBody, UINT32 ulMsgBodyLen);
    BOOL AddMsg(UINT16 usTaskID, MSG_T *pstMsg);

    VOID Resume();
    VOID Suspend();
    UINT16 GetTaskID();
    BOOL InitUniqueMsg(UINT16 usMsgID, GOS_UNIQUE_MSG_CMP_FUNC *pfUniqueMsgCmp=NULL);

private:
    UINT16          m_usProcID;
    HANDLE          m_hSem;
    UINT32          m_ulStatus;
    UINT16          m_usTaskID;

    GMsgQueue       *m_MsgQueue;

    GProc(UINT16 usProcID, UINT32 ulMaxMsgNodeNum, VOID *pPara, UINT32 ulStackSize=65536);
    BOOL Schedule();
};

#endif
