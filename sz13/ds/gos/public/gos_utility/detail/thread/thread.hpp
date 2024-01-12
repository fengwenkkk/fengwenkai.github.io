/**
 * @file            thread.hpp
 * @brief           Ïß³ÌÀà
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2022-02-19 15:51:35
 * @copyright       Copyright (c) 2021-2022 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_THREAD_THREAD_HPP__
#define DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_THREAD_THREAD_HPP__

class Thread
{
public:
    Thread(void)
    {
        m_threadHandle = NULL;
        m_runnerReady = NULL;
        m_bExit = FALSE;
    }

public:
    virtual ~Thread(void);

    virtual int Start()
    {
        Stop();

        DWORD threadId = 0;

        m_bExit = FALSE;
        m_runnerReady = CreateEvent(NULL, TRUE, FALSE, NULL);

        m_threadHandle = (HANDLE)::_beginthreadex(NULL, 0, Run, (LPVOID)this, 0, (unsigned int*)&threadId);

        if (m_threadHandle != NULL)
        {
            WaitForSingleObject(m_runnerReady, INFINITE);
        }

        CloseHandle(m_runnerReady);
        m_runnerReady = NULL;

        return 0;
    }
    virtual int Stop()
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

protected:
    virtual int Run() = 0;

private:
    static unsigned int WINAPI Run(void* lpParameter)
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

    volatile BOOL m_bExit;
    HANDLE m_threadHandle;
    HANDLE m_runnerReady;
};

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_THREAD_THREAD_HPP__