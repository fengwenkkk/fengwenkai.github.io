#include "g_include.h"
#include "g_thread.h"
#ifdef _OSWIN32_
#include "process.h"
#else
#include "semaphore.h"
#endif

static GOS_THREAD_RET __ThreadEntry(void* pPara)
{
    GThread     *pThread = (GThread*)pPara;
    VOID        *pThreadPara = pThread->GetThreadPara();

    while(pThread->GetThreadStatus() != THREAD_STATUS_RUN)
    {
        gos_sleep_1ms(1);
    }

#ifdef _OSWIN32_
    return pThread->ThreadEntry(pThreadPara);
#else
    return (void*)pThread->ThreadEntry(pThreadPara);
#endif
}

GThread::GThread(VOID *pPara, UINT32 ulStackSize)
{
    m_pThreadPara = pPara;

    m_ulThreadStatus = THREAD_STATUS_NULL;

#ifdef _OSWIN32_
    CreateThread(NULL,
                 ulStackSize,
                 (LPTHREAD_START_ROUTINE)__ThreadEntry,
                 this,
                 0,
                 (unsigned long*)&m_ThreadID);
#else
    pthread_attr_t      stAttr;

    pthread_attr_init(&stAttr);
    pthread_attr_setdetachstate(&stAttr, PTHREAD_CREATE_DETACHED);  // …Ë÷√”Œ¿ÎÃ¨£®∑÷¿ÎÃ¨£©
    pthread_create ((pthread_t *)&m_ThreadID,
                     &stAttr,
                     __ThreadEntry,
                     (void*)this);

#endif
}

GThread::~GThread()
{
    gos_close_thread(m_ThreadID);
}

VOID GThread::Start()
{
    m_ulThreadStatus = THREAD_STATUS_RUN;
}

VOID GThread::Stop()
{
    m_ulThreadStatus = THREAD_STATUS_NULL;
}
