#include "g_include.h"
#include <process.h>
#include <Windows.h>
#include "Thread.h"

Thread::Thread(void)
{
    m_threadHandle = NULL;
    m_runnerReady = NULL;
    m_bExit = FALSE;
}

Thread::~Thread(void)
{
}

int Thread::Start()
{
    Stop();

    DWORD threadId = 0;

    m_bExit = FALSE;
    m_runnerReady = CreateEvent(NULL, TRUE, FALSE, NULL);

    m_threadHandle = (HANDLE)::_beginthreadex(
        NULL,
        0,
        Run,
        (LPVOID)this,
        0,
        (unsigned int*)&threadId);


    if (m_threadHandle != NULL)
    {
        WaitForSingleObject(m_runnerReady, INFINITE);
    }

    CloseHandle(m_runnerReady);
    m_runnerReady = NULL;

    return 0;
}

int Thread::Stop()
{
    m_bExit = TRUE;
    if (m_threadHandle)
    {
        // quit runner and thread.
        ::WaitForSingleObject(m_threadHandle, INFINITE);
        ::CloseHandle(m_threadHandle);
        m_threadHandle = NULL;
    }
    return 0;
}

unsigned int WINAPI Thread::Run(void* lpParameter)
{
    Thread* pThread = (Thread*)lpParameter;

    SetEvent(pThread->m_runnerReady);

    while (!pThread->m_bExit)
    {
        int nRet = pThread->Run();

        if (nRet < 0)
        {
            break;
        }

        if (nRet > 0)
        {
            continue;
        }

        Sleep(1);
    }

    _endthreadex(0);
    return 0;
}
