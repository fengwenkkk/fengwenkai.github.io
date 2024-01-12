#pragma once

class  Thread
{
public:
    Thread(void);

public:
    virtual ~Thread(void);

    virtual int Start();
    virtual int Stop();

protected:
    virtual int Run() = 0;

private:
    static unsigned int  WINAPI Run(void* lpParameter);

    volatile  BOOL m_bExit;
    HANDLE         m_threadHandle;
    HANDLE         m_runnerReady;
};
