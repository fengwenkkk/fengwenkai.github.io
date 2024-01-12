#ifndef TIMER_H
#define TIMER_H

#define TIMER_PARA_PTR      0
#define TIMER_PARA_INT      1

typedef struct
{
    BOOL            bLoopTimer;

    UINT16          usTaskID;
    UINT16          usTimerID;

    UINT32          ulTimeout;

    UINT32          ulParaType;
    TIMER_PARA_U    unPara;

    UINT32          ulStartTime;
}TIMER_INFO_T;

class GTimer :public GThread
{
public:
    static GTimer* GetInstance();

    BOOL SetTimer(UINT16 usTskID, UINT16 usTimerID, UINT32 ulTimeout, UINT32 ulPara);
    BOOL SetTimer(UINT16 usTskID, UINT16 usTimerID, UINT32 ulTimeout, VOID *pvPara=NULL);

    BOOL SetLoopTimer(UINT16 usTskID, UINT16 usTimerID, UINT32 ulTimeout, UINT32 ulPara);
    BOOL SetLoopTimer(UINT16 usTskID, UINT16 usTimerID, UINT32 ulTimeout, VOID *pvPara=NULL);

    BOOL ClearTimer(UINT16 usTaskID, UINT16 usTimerID);

    VOID CheckTimer();

    GOS_THREAD_RET ThreadEntry(void* pPara);

private:
    GLIST       *m_hTimerInfo;
    GMutex      *m_Mutex;

    GTimer();

    BOOL SetTimer(BOOL bLoopTimer, UINT16 usTskID, UINT16 usTimerID, UINT32 ulTimeout, UINT32 ulParaType, TIMER_PARA_U unPara);
    VOID DispatchTimerMsg(TIMER_INFO_T *pstTimerInfo);
};

#endif
