#ifndef G_THREAD_H
#define G_THREAD_H

#define THREAD_STATUS_NULL      0
#define THREAD_STATUS_RUN       1

class GThread
{
public:
    GThread(VOID *pPara, UINT32 ulStackSize=65536);
    virtual ~GThread();

    VOID *GetThreadPara() const
    {
        return m_pThreadPara;
    }

    THREAD_TOKEN GetThreadID() const
    {
        return m_ThreadID;
    }

    UINT32 GetThreadStatus() const
    {
        return m_ulThreadStatus;
    }

    VOID Start();
    VOID Stop();

    virtual GOS_THREAD_RET ThreadEntry(void* pPara)=0;

private:
    VOID            *m_pThreadPara;
    THREAD_TOKEN    m_ThreadID;
    UINT32          m_ulThreadStatus;
};

#endif
