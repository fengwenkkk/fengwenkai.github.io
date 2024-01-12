/**
 * @file            mutex_utility.hpp
 * @brief           mutex 的自我实现
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-09-17 11:37:26
 *
 * @copyright       Copyright (c) 2021 GBCOM
 *
 */

#ifndef GOS_PUBLIC_GOS_UTILITY_DETAIL_MUTEX_UTILITY_HPP
#define GOS_PUBLIC_GOS_UTILITY_DETAIL_MUTEX_UTILITY_HPP

#include "g_lock.h"
#include "gos_api.h"

namespace gos
{
/// class Mutex
/// {
/// public:
///     Mutex(void)
///     {
///         ::InitializeCriticalSection(&m_CritSec);
///     }
///
/// public:
///     virtual ~Mutex(void)
///     {
///         ::DeleteCriticalSection(&m_CritSec);
///     }
///
///     int Lock()
///     {
///         ::EnterCriticalSection(&m_CritSec);
///         return 0;
///     }
///     int UnLock()
///     {
///         ::LeaveCriticalSection(&m_CritSec);
///         return 0;
///     }
///
/// protected:
///     CRITICAL_SECTION m_CritSec;
/// };

class mutex
{
public:
    mutex()
    {
        mutex_ = gos_mutex_init();
    }
    ~mutex()
    {
        gos_mutex_free(mutex_);
    }
    void lock()
    {
        gos_mutex(mutex_);
    }

    void unlock()
    {
        gos_unmutex(mutex_);
    }

private:
    mutex(const mutex&);
    mutex& operator=(const mutex&);
    HANDLE mutex_;
};

}  // namespace gos

/// 不要删除
/// #ifdef _OSWIN32_
/// #include "process.h"
/// #else
/// #include "semaphore.h"
/// #include <sys/types.h>
/// #include <sys/socket.h>
/// #endif

/// HANDLE gos_mutex_init(VOID)
/// {
/// #ifdef _OSWIN32_
///     CRITICAL_SECTION    *pstMutex = (CRITICAL_SECTION*)calloc(1, sizeof(CRITICAL_SECTION));
///
///     if (!pstMutex)
///     {
///         return NULL;
///     }
///
///     InitializeCriticalSection(pstMutex);
///
///     return (HANDLE)pstMutex;
/// #else
///     pthread_mutex_t     *pstMutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t));
///
///     if (!pstMutex)
///     {
///         return NULL;
///     }
///
///     pthread_mutex_init(pstMutex, NULL);
///
///     return (HANDLE)pstMutex;
///
/// #endif
/// }
///
/// VOID gos_mutex_free(HANDLE hMutex)
/// {
///     if (NULL == hMutex)
///     {
///         return;
///     }
///
/// #ifdef _OSWIN32_
///     DeleteCriticalSection((CRITICAL_SECTION*)hMutex);
/// #else
///     pthread_mutex_destroy((pthread_mutex_t*)hMutex);
/// #endif
///
///     free((void*)hMutex);
/// }
///
/// BOOL gos_mutex(HANDLE hMutex)
/// {
///     if (!hMutex)
///     {
///         return FALSE;
///     }
///
/// #ifdef _OSWIN32_
///     EnterCriticalSection((CRITICAL_SECTION*)hMutex);
/// #else
///     if (pthread_mutex_lock((pthread_mutex_t*)hMutex))
///     {
///         return FALSE;
///     }
/// #endif
///
///     return TRUE;
/// }
///
/// VOID gos_unmutex(HANDLE hMutex)
/// {
///     if (!hMutex)
///     {
///         return;
///     }
///
/// #ifdef _OSWIN32_
///     LeaveCriticalSection((CRITICAL_SECTION*)hMutex);
/// #else
///     pthread_mutex_unlock((pthread_mutex_t*)hMutex);
/// #endif
/// }

#endif  ///< GOS_PUBLIC_GOS_UTILITY_DETAIL_MUTEX_UTILITY_HPP
