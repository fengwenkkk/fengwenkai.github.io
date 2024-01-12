#include "g_include.h"

/* GMutex */
GMutex::GMutex()
{
    m_hMutex = gos_mutex_init();
}

GMutex::~GMutex()
{
    gos_mutex_free(m_hMutex);
    m_hMutex = NULL;
}

BOOL GMutex::Mutex()
{
    return gos_mutex(m_hMutex);
}

VOID GMutex::Unmutex()
{
    gos_unmutex(m_hMutex);
}

GAutoLock::GAutoLock(GMutex &Mutex):m_Mutex(Mutex)
{
    m_Mutex.Mutex();
}

GAutoLock::~GAutoLock(VOID)
{
    m_Mutex.Unmutex();
}

#ifdef GOS_USE_SEM
/* GLock */
GLock::GLock()
{
    m_hLock = gos_lock_init();
}

GLock::~GLock()
{
    gos_lock_free(m_hLock);
}

BOOL GLock::Lock(UINT32 ulTimeout)
{
    return gos_lock(m_hLock, ulTimeout);
}

BOOL GLock::Unlock()
{
    return gos_unlock(m_hLock);
}

/* GSem */
GSem::GSem()
{
    m_hSem = gos_sem_init();
}

GSem::~GSem()
{
    gos_sem_free(m_hSem);
}

BOOL GSem::P(UINT32 ulTimeout)
{
    return gos_sem_p(m_hSem, ulTimeout);
}

BOOL GSem::V()
{
    return gos_sem_v(m_hSem);
}
#endif
