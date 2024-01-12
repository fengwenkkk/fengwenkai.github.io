#include "g_include.h"
#include "Mutex.h"

Mutex::Mutex(void)
{
    ::InitializeCriticalSection(&m_CritSec);
}

Mutex::~Mutex(void)
{
   ::DeleteCriticalSection(&m_CritSec);
}
int   Mutex::Lock()
{
     ::EnterCriticalSection(&m_CritSec);
     return 0;
}
int   Mutex::UnLock()
{
    ::LeaveCriticalSection(&m_CritSec);
    return 0;
}

AutoLock::AutoLock(Mutex& mutex):m_Mutex(mutex)
{
     m_Mutex.Lock();
}
AutoLock::~AutoLock(void)
{
     m_Mutex.UnLock();
}

