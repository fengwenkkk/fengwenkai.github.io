#include "gos_api.h"
#include "gos_func.h"
#include <stdlib.h>

#ifdef _OSWIN32_
#include <process.h>
#include <shellapi.h>
#else
#include "semaphore.h"
#include <sys/types.h>
#include <sys/socket.h>
#endif

BOOL gos_init_socket(VOID)
{
    static BOOL bInited = FALSE;
#ifdef _OSWIN32_

    UINT16      wVersionRequested;
    WSADATA     wsaData;

    if (bInited)
    {
        return TRUE;
    }

    wVersionRequested = MAKEWORD (2, 2);
    if (WSAStartup (wVersionRequested, &wsaData) != 0)
    {
        return FALSE;
    }

    /* Confirm that the WinSock DLL supports 2.2.*/
    if (LOBYTE (wsaData.wVersion) != 2 ||
        HIBYTE (wsaData.wVersion) != 2 )
    {
        WSACleanup ();
        return FALSE;
    }
#else
    struct sigaction    sa;

    if (bInited)
    {
        return TRUE;
    }

    sa.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sa, 0);
#endif

    bInited = TRUE;

    return TRUE;
}

HANDLE gos_mutex_init(VOID)
{
#ifdef _OSWIN32_
    CRITICAL_SECTION    *pstMutex = (CRITICAL_SECTION*)calloc(1, sizeof(CRITICAL_SECTION));

    if (!pstMutex)
    {
        return NULL;
    }

    InitializeCriticalSection(pstMutex);

    return (HANDLE)pstMutex;
#else
    pthread_mutex_t     *pstMutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t));

    if (!pstMutex)
    {
        return NULL;
    }

    pthread_mutex_init(pstMutex, NULL);

    return (HANDLE)pstMutex;

#endif
}

VOID gos_mutex_free(HANDLE hMutex)
{
    if (NULL == hMutex)
    {
        return;
    }

#ifdef _OSWIN32_
    DeleteCriticalSection((CRITICAL_SECTION*)hMutex);
#else
    pthread_mutex_destroy((pthread_mutex_t*)hMutex);
#endif

    free((void*)hMutex);
}

BOOL gos_mutex(HANDLE hMutex)
{
    if (!hMutex)
    {
        return FALSE;
    }

#ifdef _OSWIN32_
    EnterCriticalSection((CRITICAL_SECTION*)hMutex);
#else
    if (pthread_mutex_lock((pthread_mutex_t*)hMutex))
    {
        return FALSE;
    }
#endif

    return TRUE;
}

VOID gos_unmutex(HANDLE hMutex)
{
    if (!hMutex)
    {
        return;
    }

#ifdef _OSWIN32_
    LeaveCriticalSection((CRITICAL_SECTION*)hMutex);
#else
    pthread_mutex_unlock((pthread_mutex_t*)hMutex);
#endif
}

#ifdef GOS_USE_SEM
HANDLE gos_sem_init(VOID)
{
    UINT32 dwInitVal = 0;

#ifdef _OSWIN32_
    UINT32 dwMaxVal = 1;
    HANDLE hSem = INVALID_SEM;

    hSem = CreateSemaphore (NULL, dwInitVal, dwMaxVal, NULL);
    return hSem;
#else
    int ret;
    sem_t *sem = (sem_t*)calloc(1, sizeof(sem_t));

    if(sem == NULL)
    {
        return NULL;
    }

    ret = sem_init(sem, 0, dwInitVal);
    if (ret != 0)
    {
        free(sem);
        return NULL;
    }
    else
    {
        return (HANDLE)sem;
    }
#endif
}

BOOL gos_sem_free(HANDLE hSem)
{
#ifdef _OSWIN32_
    return CloseHandle(hSem);
#else
    if (sem_destroy ((sem_t*)hSem) == -1)
    {
        free((sem_t*)hSem);
        return FALSE;
    }
    else
    {
        free((sem_t*)hSem);
        return TRUE;
    }
#endif
}

BOOL gos_sem_p(HANDLE hSem, UINT32 dwTimeout)
{
#ifdef _OSWIN32_
    UINT32    dwRet;

    dwRet = WaitForSingleObject(hSem, dwTimeout);
    if (dwRet == WAIT_OBJECT_0)
    {
        return TRUE;
    }

    if (dwRet == WAIT_TIMEOUT)
    {
        return FALSE;
    }

    return FALSE;
#else
    struct timespec timeout;

    if (dwTimeout == WAIT_FOREVER)
    {
        if(0 == sem_wait((sem_t*)hSem))
        {
            return TRUE;
        }
    }
    else
    {
        if (dwTimeout == NO_WAIT)
        {
            timeout.tv_sec = 0;
            timeout.tv_nsec = 0;
        }
        else
        {
            clock_gettime(CLOCK_REALTIME, &timeout);
            timeout.tv_sec += dwTimeout/1000;
            timeout.tv_nsec += (dwTimeout%1000)*1000000;
            while (timeout.tv_nsec >= 1000000000)
            {
                timeout.tv_sec ++;
                timeout.tv_nsec -= 1000000000;
            }
        }

        if (0 == sem_timedwait((sem_t*)hSem, &timeout))
        {
            return TRUE;
        }
        else
        {
            if (errno == ETIMEDOUT)
            {
                return FALSE;
            }
        }
    }

    return FALSE;
#endif
}

BOOL gos_sem_v(HANDLE hSem)
{
    UINT32 dwCount = 1;
#ifdef _OSWIN32_
    if (ReleaseSemaphore (hSem, dwCount, NULL) == 0) /* 对非互斥信号量的处理 */
    {
        if (ERROR_INVALID_HANDLE == GetLastError ()) /* 如果出错，则是互斥信号量 */
        {
            if (0 == ReleaseMutex (hSem))
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }

        return FALSE;
    }

    return TRUE;
#else
    int i;

    for(i=0; i<dwCount; i++)
    {
        if(sem_post((sem_t*)hSem) != 0)
        {
            return FALSE;
        }
    }

    return TRUE;
#endif
}
#endif

/**
 * @brief          获取系统上电时间
 * @return         UINT32返回系统上电至今的时间 秒级
 * @author         zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par示例:
 * @code
 *
 * @endcode
 * @note
 *    struct sysinfo{
 *      long uptime;             //启动到现在经过的时间?
 *      unsigned long loads[3];
 *      unsigned long totalram;  //总的可用的内存大小??
 *      unsigned long freeram;   //还未被使用的内存大小??
 *      unsigned long sharedram; //共享的存储器的大小
 *      unsigned long bufferram; //共享的存储器的大小?
 *      unsigned long totalswap; //交换区大小?
 *      unsigned long freeswap;  //还可用的交换区大小?
 *      unsigned short procs;    //当前进程数目??
 *      unsigned long totalhigh; //总的高内存大小?
 *      unsigned long freehigh; //可用的高内存大小
 *      unsigned int mem_unit;  //以字节为单位的内存大小??
 *      char _f[20-2*sizeof(long)-sizeof(int)];
 *   };
 *   原文链接：https://blog.csdn.net/qq_33706673/article/details/78187559
 *
 */
UINT32 gos_get_sys_uptime(VOID)
{
#ifdef _OSWIN32_
    return GetTickCount()/1000;
#else
    struct sysinfo info;

    //成功返回0 错误返回 -1
    if( 0 == sysinfo( &info ) )
    {
        return (unsigned long)info.uptime;
    }

    return 0;
#endif
}

/*
CLOCK_REALTIME:系统实时时间,随系统实时时间改变而改变,即从UTC1970-1-1 0:0:0开始计时,中间时刻如果系统时间被用户改成其他,则对应的时间相应改变
CLOCK_MONOTONIC:从系统启动这一刻起开始计时,不受系统时间被用户改变的影响
CLOCK_PROCESS_CPUTIME_ID:本进程到当前代码系统CPU花费的时间
CLOCK_THREAD_CPUTIME_ID:本线程到当前代码系统CPU花费的时间
*/

/**
 * @brief          获取系统上电时间 毫秒级
 * @return         UINT32  返回毫秒级的系统上电的时间
 * @author         zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par示例:
 * @code
 *
 * @endcode
 * @note
 */
UINT32 gos_get_sys_uptime_1ms(VOID)
{
#ifdef _OSWIN32_
    return GetTickCount();
#else
    struct timespec stCurrTime;
    INT64           sec;
    INT64           ims;        /* 毫秒(=1000微秒)*/
    INT32           nsec;       /* 纳秒*/

    if (clock_gettime(CLOCK_MONOTONIC, &stCurrTime) < 0)
    {
        return 0;
    }

    sec  = stCurrTime.tv_sec;
    nsec = stCurrTime.tv_nsec;

    ims = (sec * 1000) + (nsec / 1000000);
    if (ims < 0)
    {
        return 0;
    }

    return (UINT32)ims;
#endif
}

/**
 * @brief           获取微秒精度的时间，根据系统不同，返回不同结果
 * @details         WIN32系统下获取系统运行以来的毫秒数 * 1000，
 *                  Linux系统下获取自Epoch(1970年1月1日 00:00:00)的微秒数
 * @return          UINT64  获取到的毫秒数
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
INT64 gos_get_sys_uptime_1us(VOID)
{
#ifdef _OSWIN32_
    return (INT64)GetTickCount()*1000;
#else
    struct timespec stCurrTime;
    INT64           usec;

    if (clock_gettime(CLOCK_MONOTONIC, &stCurrTime) < 0)
    {
        return 0;
    }

    usec = ((INT64)stCurrTime.tv_sec * 1000000) + (stCurrTime.tv_nsec / 1000);
    if (usec < 0)
    {
        return 0;
    }

    return usec;
#endif
}

/**
 * @brief          获取软件运行到现在的时间 秒级
 * @return         UINT32
 * @author         zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par示例:
 * @code
 *
 * @endcode
 * @note
 */
UINT32 gos_get_uptime(VOID)
{
#ifdef _OSWIN32_
    static UINT32  ulStartTicks = 0;

    if (0 == ulStartTicks)
    {
        ulStartTicks = GetTickCount();
        return 0;
    }

    return (UINT32) ((GetTickCount() - ulStartTicks) / 1000);

#else
    static unsigned long ulStartTime = 0;
    struct sysinfo info;

    if (0 == ulStartTime)
    {
        if( 0 == sysinfo( &info ) )
        {
            ulStartTime = (unsigned long)info.uptime;
        }

        return 0;
    }

    if( 0 == sysinfo( &info ) )
    {
        return (UINT32)(info.uptime - ulStartTime);
    }
    else
    {
        return 0;
    }
#endif
}

/**
 * @brief          获取系统运行到现在的时间 毫秒级
 * @return         UINT32
 * @author         zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *
 * @endcode
 * @note
 */
UINT32 gos_get_uptime_1ms(VOID)
{
#ifdef _OSWIN32_
    static UINT32  ulStartTicks = 0;

    if (0 == ulStartTicks)
    {
        ulStartTicks = GetTickCount();
        return 0;
    }

    return (UINT32)(GetTickCount() - ulStartTicks);

#else
    static BOOL bInit = FALSE;
    static struct timespec stStartTime = {0};
    struct timespec stCurrTime;
    INT64           sec;
    INT64           ims;        /* 毫秒(=1000微秒)*/
    INT32           nsec;       /* 纳秒*/

    if (!bInit)
    {
        if (clock_gettime(CLOCK_MONOTONIC, &stStartTime) < 0)
        {
            return 0;
        }

        bInit = TRUE;

        return 0;
    }

    if (clock_gettime(CLOCK_MONOTONIC, &stCurrTime) < 0)
    {
        return 0;
    }

    sec  = stCurrTime.tv_sec - stStartTime.tv_sec;
    nsec = stCurrTime.tv_nsec - stStartTime.tv_nsec;

    ims = (sec * 1000) + (nsec / 1000000);
    if (ims < 0)
    {
        return 0;
    }

    return (UINT32)ims;
#endif
}

INT64 gos_get_uptime_1us(VOID)
{
    static INT64    i64StartTicks = -1;

    if (i64StartTicks < 0)
    {
        i64StartTicks = gos_get_sys_uptime_1us();
    }

    return gos_get_sys_uptime_1us() - i64StartTicks;
}


/**
 * @brief           获取毫秒精度的时间, 根据系统不同，返回不同结果
 * @details         WIN32系统下获取程序运行以来的毫秒数，
 *                  Linux系统下获取自Epoch(1970年1月1日 00:00:00)的毫秒数
 * @return          UINT64  获取到的毫秒数
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
UINT64 gos_get_tick(VOID)
{
#ifdef _OSWIN32_
    return GetTickCount();
#else
    struct timespec stCurrTime;

    clock_gettime(CLOCK_MONOTONIC, &stCurrTime);
    return ((UINT64)stCurrTime.tv_sec * 1000) + (stCurrTime.tv_nsec / 1000000);
#endif
}

VOID gos_sleep(UINT32 ulSecond)
{
#ifdef _OSWIN32_
    Sleep(ulSecond*1000);
#else
    sleep(ulSecond);
#endif
}

/**
 * @brief           本线程休眠若干毫秒
 * @param           dwTimeout    [in]
 * @return          VOID
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-02-10 11:30:50
 * @note
 * lijiancong(2022-02-10): Windows API: Sleep 函数因为时间切片的原因会有0~18ms的延迟。
 *                         如休眠 5000ms 在实际时间的流逝中可能过去了5015ms
 * 参考资料:
 * [Sleep function (synchapi.h)](https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-sleep)
 * [解决WINDOWS SLEEP精度问题](https://blog.liboliu.com/a/78)
 * [Windows几种sleep精度的测试，结果基于微秒](https://blog.csdn.net/liuhengxiao/article/details/99641539)
 */
VOID gos_sleep_1ms(UINT32 ulTimeout)
{
#ifdef _OSWIN32_
    timeBeginPeriod(1);  ///< 设置精度为1毫秒
    Sleep(ulTimeout);
    timeEndPeriod(1);    ///< 结束精度设置
#else
    /* ms到tick */
    struct timespec req, rem;

    req.tv_sec = ulTimeout/1000;
    req.tv_nsec = (ulTimeout%1000)*1000000;
    while (req.tv_nsec >= 1000000000)
    {
        req.tv_sec ++;
        req.tv_nsec -= 1000000000;
    }

    while (EINTR == nanosleep(&req, &rem))
    {
        req = rem;
        //nanosleep (&req, &rem);
    }
#endif
}

VOID gos_gettimeofday(TIMEVAL *tp)
{
#ifdef _OSWIN32_
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;

    GetLocalTime(&wtm);
    tm.tm_year  = wtm.wYear - 1900;
    tm.tm_mon   = wtm.wMonth - 1;
    tm.tm_mday  = wtm.wDay;
    tm.tm_hour  = wtm.wHour;
    tm.tm_min   = wtm.wMinute;
    tm.tm_sec   = wtm.wSecond;
    tm.tm_isdst= -1;
    clock = mktime(&tm);
    tp->tv_sec = clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
#else
    gettimeofday(tp, NULL);
#endif
}

/**
 * @brief           把 pstTime 所指向的结构转换为1970至今的秒数
 * @param           pstTime [in]
 * @return          UINT32
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par示例:
 * @code
 *
 * @endcode
 * @note
 */
UINT32 gos_mktime(GOS_DATETIME_T *pstTime)
{
    struct tm       stLocalTime = {0};

    if (pstTime->usYear < 1970 ||
        pstTime->usYear > 2038 ||
        pstTime->ucMonth > 12 ||
        pstTime->ucDay > 31 ||
        pstTime->ucHour > 23 ||
        pstTime->ucMinute > 59 ||
        pstTime->ucSecond > 59)
    {
        return 0;
    }

    stLocalTime.tm_sec  = pstTime->ucSecond;
    stLocalTime.tm_min  = pstTime->ucMinute;
    stLocalTime.tm_hour = pstTime->ucHour;
    stLocalTime.tm_mday = pstTime->ucDay;
    stLocalTime.tm_mon  = pstTime->ucMonth-1;
    stLocalTime.tm_year = pstTime->usYear-1900;

    return mktime(&stLocalTime);
}

/**
 * @brief          将pulCurrTime整数时间转换为真实世界的时间表达方式
 * @param          pulCurrTime [in] 输入参数 需要转换的时间 秒级
 * @param          pstTime [out] 输出参数 时间结构体
 * @return         BOOL成功 ：返回TRUE 失败 ： 返回FALSE
 * @author         zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *
 * @endcode
 * @note
 *1、 gmtime()函数转换后的值的UTC时间没有经过时区变换
 */
BOOL gos_get_abstime(UINT32 *pulCurrTime, GOS_DATETIME_T *pstTime)
{
    struct tm   *pstGMTime = gmtime((time_t*)pulCurrTime);

    if (!pstGMTime)
    {
        return FALSE;
    }

    pstTime->usYear   = (UINT16)(pstGMTime->tm_year + 1900);
    pstTime->ucMonth  = (UINT8)(pstGMTime->tm_mon + 1);
    pstTime->ucDay    = (UINT8)pstGMTime->tm_mday;
    pstTime->ucHour   = (UINT8)pstGMTime->tm_hour;
    pstTime->ucMinute = (UINT8)pstGMTime->tm_min;
    pstTime->ucSecond = (UINT8)pstGMTime->tm_sec;
    pstTime->ucSn     = 0;

    return TRUE;
}

INT32 gos_get_timezone(VOID)
{
    time_t          ulTime = 24*3600;
    struct tm       stTime;

#ifdef WIN32
    memcpy(&stTime, localtime(&ulTime), sizeof(stTime));
#else
    localtime_r(&ulTime, &stTime);
#endif

    return (INT32)((stTime.tm_mday-1)*24*3600 + stTime.tm_hour*3600 - ulTime);
}

#ifndef GOS_SPEED_OPTIMIZE

/**
 * @brief          将 pulCurrTime 转换为经过时区变换后的本地时间结构
 * @param          pulCurrTime [in]  输入参数 需要转换的时间 秒级
 * @param          pstTime [out]     输出参数 转换后的时间结构
 * @return         BOOL
 * @author         zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *
 * @endcode
 * @note
 */
BOOL gos_get_localtime(UINT32 *pulCurrTime, GOS_DATETIME_T *pstTime)
{
    time_t          stCurrTime;
#ifdef _OSWIN32_
    struct tm       *pstLocalTime;
#else
    struct tm       stLocalTime;
#endif

    if (NULL == pulCurrTime)
    {
        stCurrTime = time(NULL);
    }
    else
    {
        stCurrTime = (time_t)(*pulCurrTime);
    }

#ifdef _OSWIN32_
    pstLocalTime = localtime(&stCurrTime);
    if (!pstLocalTime)
    {
        return FALSE;
    }

    pstTime->usYear   = (UINT16)(pstLocalTime->tm_year + 1900);
    pstTime->ucMonth  = (UINT8)(pstLocalTime->tm_mon + 1);
    pstTime->ucDay    = (UINT8)pstLocalTime->tm_mday;
    pstTime->ucHour   = (UINT8)pstLocalTime->tm_hour;
    pstTime->ucMinute = (UINT8)pstLocalTime->tm_min;
    pstTime->ucSecond = (UINT8)pstLocalTime->tm_sec;
    pstTime->ucSn     = 0;
#else
    localtime_r (&stCurrTime, &stLocalTime);
    pstTime->usYear   = stLocalTime.tm_year + 1900;
    pstTime->ucMonth  = stLocalTime.tm_mon + 1;
    pstTime->ucDay    = stLocalTime.tm_mday;
    pstTime->ucHour   = stLocalTime.tm_hour;
    pstTime->ucMinute = stLocalTime.tm_min;
    pstTime->ucSecond = stLocalTime.tm_sec;
    pstTime->ucSn     = 0;
#endif

    return TRUE;
}
#else
BOOL gos_get_localtime(UINT32 *pulCurrTime, GOS_DATETIME_T *pstTime)
{
    static UINT32           bInit = FALSE;
    static UINT32           ulSecPerDay = 24*3600;
    static GOS_DATETIME_T   astDayInfo[(2038-1970)*365];
    static GOS_DATETIME_T   astSecInfo[24*3600];
    UINT32                  ulDay;
    UINT32                  ulSec;
    UINT32                  ulCurrTime;
    static INT32            iTimeZone = 0x7fffffff;
    UINT32                  i;

    if (!bInit)
    {
        time_t      ulStartTime = 0;
        struct tm   stTime;
        time_t      ulTime;

        bInit = TRUE;

        iTimeZone = gos_get_timezone();

        memset(astDayInfo, 0, sizeof(astDayInfo));
        for (i=0; i<ARRAY_SIZE(astDayInfo); i++)
        {
#ifdef WIN32
            memcpy(&stTime, localtime(&ulStartTime), sizeof(stTime));
#else
            localtime_r(&ulStartTime, &stTime);
#endif
            astDayInfo[i].usYear = stTime.tm_year+1900;
            astDayInfo[i].ucMonth = stTime.tm_mon+1;
            astDayInfo[i].ucDay = stTime.tm_mday;
            ulStartTime += ulSecPerDay;
        }

        memset(astSecInfo, 0, sizeof(astSecInfo));
        for (i=0; i<ARRAY_SIZE(astSecInfo); i++)
        {
            ulTime = 24*3600 + i;
            memcpy(&stTime, gmtime((time_t*)&ulTime), sizeof(stTime));
            astSecInfo[i].ucHour   = stTime.tm_hour;
            astSecInfo[i].ucMinute = stTime.tm_min;
            astSecInfo[i].ucSecond = stTime.tm_sec;
        }
    }

    if (NULL == pulCurrTime)
    {
        ulCurrTime = (UINT32)time(NULL) + iTimeZone;
    }
    else
    {
        ulCurrTime =  pulCurrTime[0] + iTimeZone;
    }

    ulDay = ulCurrTime / ulSecPerDay;
    ulSec = ulCurrTime % ulSecPerDay;

    if (ulDay >= ARRAY_SIZE(astSecInfo))
    {
        return FALSE;
    }

    SET_INT(pstTime, GET_INT(&astDayInfo[ulDay]));
    SET_INT(&pstTime->ucHour, GET_INT(&astSecInfo[ulSec].ucHour));

    return TRUE;
}
#endif

/**
 * @brief           把时间戳转换成当天零点的时间戳
 * @param           ulTime    [in]
 * @return          UINT32
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 * UINT32 i = gos_get_day_from_time(time(NULL));
 * std::cout << gos::get_time_string(i);
 * @endcode
 * @note
 */
UINT32 gos_get_day_from_time(UINT32 ulTime)
{
    GOS_DATETIME_T  stTime;

    gos_get_localtime(&ulTime, &stTime);
    stTime.ucHour = 0;
    stTime.ucMinute = 0;
    stTime.ucSecond = 0;

    return gos_mktime(&stTime);
}

BOOL gos_get_leapyear(UINT32 ulYear)
{
    if (ulYear%400==0 || (ulYear%4==0 && ulYear%100!=0))
    {
        return TRUE;
    }

    return FALSE;
}

UINT32 gos_get_daycount(UINT32 ulYear, UINT32 ulMonth)
{
    switch(ulMonth)
    {
        case 1:
            return 31;
        case 3:
            return 31;
        case 5:
            return 31;
        case 7:
            return 31;
        case 8:
            return 31;
        case 10:
            return 31;
        case 12:
            return 31;
        case 4:
            return 30;
        case 6:
            return 30;
        case 9:
            return 30;
        case 11:
            return 30;
        case 2:
            return (28 + (gos_get_leapyear(ulYear)?1:0));
        default:
            return 0;
    }
}

//---------------------------------------------------------------------------
UINT32 gos_get_dayweek(UINT16 usYear, UINT8 ucMonth, UINT8 ucDay)   //基姆拉尔森公式
{
    if (ucMonth == 1 || ucMonth == 2)
    {
        usYear --;
        ucMonth += 12;
    }

    return      (ucDay + 2*ucMonth + 3*(ucMonth+1)/5 +
                 usYear + usYear/4 - usYear/100 + usYear/400)%7
                + 1;
}

typedef struct
{
    //YYYY-MM-DD
    UINT32      aulTime[3];
}GOS_ASCTIME_DAY_T;

typedef struct
{
    //HH:MI:SS
    UINT32      aulTime[3];
}GOS_ASCTIME_SEC_T;

#ifdef GOS_SPEED_OPTIMIZE
INT32 gos_asctime_ex(UINT32 ulCurrTime, CHAR *szTime, BOOL bHaveSec)
{
    static UINT32               bInit = FALSE;
    static UINT32               ulSecPerDay = 24*3600;
    static GOS_ASCTIME_DAY_T    astDayInfo[(2038-1970)*365];
    static GOS_ASCTIME_SEC_T    astSecInfo[24*3600];
    static CHAR                 aacYear[2038-1970][8];
    static CHAR                 aacMonth[12][4];
    static CHAR                 aacDay[31][4];
    static CHAR                 aacHour[24][4];
    static CHAR                 aacMinOrSec[60][4];
    UINT32                      ulDay;
    UINT32                      ulSec;
    static INT32                iTimeZone = 0x7fffffff;
    CHAR                        *pcTime;
    UINT32                      i;

    if (!bInit)
    {
        time_t      ulStartTime = 0;
        struct tm   stTime;

        bInit = TRUE;

        iTimeZone = gos_get_timezone();

        for (i=0; i<2038-1970; i++)
        {
            sprintf(aacYear[i], "%04d", 1970+i);
        }

        for (i=0; i<12; i++)
        {
            sprintf(aacMonth[i], "-%02d", i+1);
        }

        for (i=0; i<31; i++)
        {
            sprintf(aacDay[i], "-%02d", i+1);
        }

        for (i=0; i<24; i++)
        {
            sprintf(aacHour[i], " %02d", i);
        }

        for (i=0; i<60; i++)
        {
            sprintf(aacMinOrSec[i], ":%02d", i);
        }

        memset(astDayInfo, 0, sizeof(astDayInfo));
        for (i=0; i<ARRAY_SIZE(astDayInfo); i++)
        {
#ifdef WIN32
            memcpy(&stTime, localtime(&ulStartTime), sizeof(stTime));
#else
            localtime_r(&ulStartTime, &stTime);
#endif
            pcTime = (CHAR*)astDayInfo[i].aulTime;
            SET_INT(pcTime, GET_INT(aacYear[stTime.tm_year-70]));   // YYYY
            SET_INT(pcTime+4, GET_INT(aacMonth[stTime.tm_mon]));    // -MM
            SET_INT(pcTime+7, GET_INT(aacDay[stTime.tm_mday-1]));   // -DD

            ulStartTime += ulSecPerDay;
        }

        memset(astSecInfo, 0, sizeof(astSecInfo));
        for (i=0; i<ARRAY_SIZE(astSecInfo); i++)
        {
            memcpy(&stTime, gmtime((time_t*)&i), sizeof(stTime));

            pcTime = (CHAR*)astSecInfo[i].aulTime;
            SET_INT(pcTime, GET_INT(aacHour[stTime.tm_hour]));       // ' HH'
            SET_INT(pcTime+3, GET_INT(aacMinOrSec[stTime.tm_min]));    // ':MI'
            SET_INT(pcTime+6, GET_INT(aacMinOrSec[stTime.tm_sec]));    // ':SS'
        }
    }

    ulCurrTime += iTimeZone;

    ulDay = ulCurrTime / ulSecPerDay;
    ulSec = ulCurrTime % ulSecPerDay;

    if (ulDay >= ARRAY_SIZE(astSecInfo))
    {
        return 0;
    }

    pcTime = (CHAR*)&astDayInfo[ulDay].aulTime;
    SET_INT(szTime, GET_INT(pcTime));
    SET_INT(szTime+4, GET_INT(pcTime+4));
    SET_INT(szTime+8, GET_INT(pcTime+8));
    szTime += 10;   // YYYY-MM-DD

    pcTime = (CHAR*)&astSecInfo[ulSec].aulTime;
    SET_INT(szTime, GET_INT(pcTime));
    SET_INT(szTime+4, GET_INT(pcTime+4));

    if (bHaveSec)
    {
        SET_INT(szTime+8, GET_INT(pcTime+8)); // ' HH:MI:SS'
        szTime[9] = '\0';
        return 19;
    }
    else
    {
        szTime[6] = '\0';
        return 16;
    }
}

INT32 gos_asctime(UINT32 ulCurrTime, CHAR *szTime)
{
    return gos_asctime_ex(ulCurrTime, szTime, TRUE);
}
#else
INT32 gos_asctime(UINT32 ulCurrTime, CHAR *szTime)
{
    gos_get_text_time_ex(&ulCurrTime, NULL, szTime);

    return strlen(szTime);
}
#endif

INT32 gos_asctimeval(TIMEVAL *pstTimeVal, CHAR *szTime)
{
    INT32   iLen = gos_asctime(pstTimeVal->tv_sec, szTime);

    iLen += sprintf(szTime+iLen, ".%03d", pstTimeVal->tv_usec/1000);

    return iLen;
}

UINT32 gos_text_time_to_int(const CHAR *szTime)
{
    GOS_DATETIME_T  stTime;

    if (!szTime || !gos_parse_time(szTime, &stTime))
    {
        return 0xffffffff;
    }

    return gos_mktime(&stTime);
}

UINT32 gos_compact_text_time_to_int(const CHAR *szTime)
{
    GOS_DATETIME_T  stTime;

    if (!gos_parse_compact_time(szTime, &stTime))
    {
        return 0xffffffff;
    }

    return gos_mktime(&stTime);
}

#define INVALID_THREAD_TOKEN    0

/**
 * @brief           创建线程
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par 示例:
 * @code
 *
 * @endcode
 * @note
 * 1.进入线程处理函数
 */
#ifdef GOS_USE_PTHREAD

/**
 * @brief
 * @param           pfThreadEntry    [in] 函数指针，线程的工作函数
 * @param           dwStacksize      [in]
 * @param           pPara            [in]
 * @return          THREAD_TOKEN
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par 示例:
 * @code
 *
 * @endcode
 * @note
 */
THREAD_TOKEN gos_create_thread(THREAD_ENTRY *pfThreadEntry, UINT32 dwStacksize, VOID *pPara)
{
    THREAD_TOKEN    ThreadToken = 0;
#ifdef _OSWIN32_
    UINT32          ulRet = 0;
#endif

#ifdef _OSWIN32_
    ulRet = _beginthreadex((void*)NULL,
                            dwStacksize,
                            pfThreadEntry,
                            pPara,
                            0,
                            (unsigned int*)&ThreadToken);
    if (ulRet != 0)
    {
        return ThreadToken;
    }
    else
    {
        return INVALID_THREAD_TOKEN;
    }
#else
    pthread_attr_t      stAttr;

    pthread_attr_init(&stAttr);
    pthread_attr_setdetachstate(&stAttr, PTHREAD_CREATE_DETACHED);  // 设置游离态（分离态）
    if (0 == pthread_create ((pthread_t *)&ThreadToken,
                             &stAttr,
                             (void*)pfThreadEntry,
                             (void*)pPara))
    {
        return ThreadToken;
    }
    else
    {
        return INVALID_THREAD_TOKEN;
    }
#endif
}

BOOL gos_close_thread(THREAD_TOKEN ThreadToken)
{
#ifdef _OSWIN32_
    return TRUE;
//    ExitThread(0);

#else
    return TRUE;
#endif
}
#endif

/**
 * @brief           运行守护进程
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par 示例:
 * @code
 *
 * @endcode
 * @note
 * 1.fork子进程, 父进程退出, 所有工作在子进程中进行形式上脱离了控制终端; 必须
 * 2.子进程创建新会话, setsid函数, 使子进程完全独立出来, 脱离控制; 必须
 * 3.改变当前目录为根目录, chdir()函数, 防止占用可卸载的文件系统, 也可以换成其它路径， 为了增强程序的健壮性; 非必须
 * 4.重设文件权限掩码, umask()函数, 防止继承的文件创建屏蔽字拒绝某些权限, 增加守护进程灵活性; 非必须
 * 5. 关闭文件描述符, 继承的打开文件不会用到, 浪费系统资源, 无法卸载, close(0), close(1), close(2); 非必须
 * 6.执行核心工作
 *
 * lijiancong: 详细示例见 《UNIX 环境高级编程》 3edit， 第13章守护进程 13.3小节 Page 374
 */
#ifdef _OSWIN32_
VOID gos_run_daemon(BOOL bLogToStdout)
{
}
#else
VOID gos_run_daemon(BOOL bLogToStdout)
{
    pid_t pid;
    int i;
    int cnt;

    if (pid = fork())
    {
        exit(0);        //是父进程，结束父进程
    }
    else if(pid< 0)
    {
        exit(1);        //fork失败，退出
    }

    //是第一子进程，后台继续执行
    setsid();           //第一子进程成为新的会话组长和进程组长

    //并与控制终端分离
    if (pid = fork())
    {
        exit(0);        //是第一子进程，结束第一子进程
    }

    else if(pid< 0)
    {
        exit(1);        //fork失败，退出
    }

    //是第二子进程，继续
    //第二子进程不再是会话组长
    if (!bLogToStdout)
    {
        cnt = sysconf(_SC_OPEN_MAX);
        for (i=0; i<cnt; ++i)  //关闭打开的文件描述符
        {
            close(i);
        }
    }

    //chdir("/tmp");      //改变工作目录到/tmp

    umask(0);           //重设文件创建掩模
    return;
}
#endif

/**
 * @brief           判断文件或目录是否存在
 * @param           szFile    [in] 所要判断的文件名
 * @return          BOOL      文件存在返回 true，反之返回 false。
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par 示例:       判断 /home/tmp 下 是否有sa.log
 * @code
 * gos_file_exist（"/home/tmp/sa.log"）
 * @endcode
 * @note
 */
BOOL gos_file_exist(const CHAR* szFile)
{
    INT32   iRet;

    if (!szFile || *szFile == '\0')
    {
        return FALSE;
    }

    /* 0 == F_OK, 判断文件/目录是否存在 */
#if defined (_OSWIN32_)
    iRet = _access(szFile, 0);
#else
    iRet = access(szFile, 0);
#endif
    if (iRet != 0)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL gos_create_dir_ex(const CHAR* szDir, UINT32 dwMode)
{
#ifdef _OSWIN32_
    return CreateDirectory(szDir, NULL);
#else

    INT32   iRet;

    iRet = mkdir(szDir, dwMode);
    if (iRet != 0)
    {
        return FALSE;
    }

    return TRUE;
#endif
}

/**
 * @brief           创建目录
 * @param           szDir    [in] 绝对路径
 * @return          BOOL    创建目录成功返回 true，反之返回 false。
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par 示例:       在 home 目录下创建 tmp 目录
 * @code
 * gos_create_dir("/home/tmp")
 * @endcode
 * @note
 * 1. 不能递归创建，若根目录不存在，则失败.
 */
BOOL gos_create_dir(const CHAR* szDir)
{
#ifdef _OSWIN32_
    UINT32  dwMode = 0;
#else
    UINT32  dwMode = S_IRWXU | S_IRWXG | S_IRWXO;
#endif

    return gos_create_dir_ex(szDir, dwMode);
}

BOOL gos_create_redir_ex(const CHAR* szDir, UINT32 dwMode)
{
    CHAR    *szTempDir = NULL;
    CHAR    *szCurrDir = NULL;
    UINT32  ulLen;
    CHAR    *szFirstDir;
    UINT32  i;
    UINT32  ulStartPos = 0;
    UINT32  ulCurrLen = 0;

#define IS_DIR_SPLIT(c) ((c) == '/' || (c) == '\\')

    if (szDir == NULL)
    {
        goto err;
    }

    ulLen = strlen(szDir);
    if (ulLen == 0)
    {
        goto err;
    }

    szTempDir = (CHAR*)malloc(2*(ulLen+2));
    if (NULL == szTempDir)
    {
        goto err;
    }
    szCurrDir = szTempDir + (ulLen+2);

    /* 复制目录并添加目录结束符 */
    memcpy(szTempDir, szDir, ulLen);
    if (!IS_DIR_SPLIT(szTempDir[ulLen-1]))
    {
        szTempDir[ulLen] = '/';
        ulLen ++;   /* ulLen为有目录结束符的字符串长度 */
    }
    szTempDir[ulLen] = '\0';

    szFirstDir = szCurrDir;

#ifdef _OSWIN32_
    /* 判断开头的三个字符是否形如X:\ */
    if (ulLen < 3 || szDir[1] != ':' || !IS_DIR_SPLIT(szDir[2]))
    {
        goto err;
    }
    ulStartPos = 3;
    szFirstDir[0] = szDir[0];
    szFirstDir[1] = ':';
    szFirstDir[2] = DIR_SEPARATOR;
#else
    /* 判断第一个字符是否是目录分割符' */
    if (!IS_DIR_SPLIT(szDir[0]))
    {
        goto err;
    }
    ulStartPos = 1;
    szFirstDir[0] = DIR_SEPARATOR;
#endif

    /* 获取第一个目录 */
    for (i=ulStartPos; ;i++)
    {
        if (szDir[i] == '\0' || IS_DIR_SPLIT(szDir[i]))
        {
            ulCurrLen = i+1;
            szFirstDir[ulCurrLen-1] = '\0';

            break;
        }

        if (i == ulLen)
        {
            goto err;
        }
        szFirstDir[i] = szDir[i];
    }

    /* 创建第一个目录 */
    if (!gos_file_exist(szFirstDir))
    {
        if (!gos_create_dir_ex(szFirstDir, dwMode))
        {
            goto err;
        }
    }

    /* 循环创建子目录 */
    while(TRUE)
    {
        if (ulCurrLen >= ulLen)
        {
            break;
        }

        szCurrDir[ulCurrLen-1] = DIR_SEPARATOR;

        for(i=ulCurrLen; ;i++)
        {
            if (IS_DIR_SPLIT(szTempDir[i]))
            {
                ulCurrLen = i+1;
                szCurrDir[ulCurrLen-1] = '\0';
                break;
            }

            szCurrDir[i] = szTempDir[i];
        }

        if (!gos_file_exist(szCurrDir))
        {
            if (!gos_create_dir_ex(szCurrDir, dwMode))
            {
                goto err;
            }
        }
    }

    /* 最后一次判断目录是否存在 */
    if (!gos_file_exist(szTempDir)) //也许应该用szCurrDir更好
    {
        goto err;
    }

    if (szTempDir)
    {
        free(szTempDir);
    }

    return TRUE;

err:
    if (szTempDir)
    {
        free(szTempDir);
    }

    return FALSE;
}

/**
 * @brief           递归创建目录
 * @param           szDir    [in] 所要创建的目录
 * @return          BOOL     创建目录成功返回 true，反之返回false。
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par 示例:       在 home 目录下创建 tmp 目录
 * @code
 * gos_create_redir("/home/tmp")
 * @endcode
 * @note
 * 1.可递归创建，若home 目录不存在，则先创建 hoem 目录，然后再创建 tmp 目录
 */
BOOL gos_create_redir(const CHAR* szDir)
{
#ifdef _OSWIN32_
    UINT32  dwMode = 0;
#else
    UINT32  dwMode = S_IRWXU | S_IRWXG | S_IRWXO;   // FILE_ACCESS_MODE_ALL
#endif

    return gos_create_redir_ex(szDir, dwMode);
}

/**
 * @brief           删除文件
 * @param           szFile    [in] 所要删除的文件
 * @return          BOOL    删除成功返回 true，反之返回 false。
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par 示例:       删除 sa.log
 * @code
 * gos_delete_file("/home/tmp/sa.log")
 * @endcode
 * @note
 */
BOOL gos_delete_file(const CHAR* szFile)
{
#ifdef _OSWIN32_
    if (0 == DeleteFile(szFile))
    {
        UINT32  ulFileAttr = GetFileAttributes(szFile);

        ulFileAttr &= ~FILE_ATTRIBUTE_READONLY;
        SetFileAttributes(szFile, ulFileAttr);

        DeleteFile(szFile);
    }

    return !gos_file_exist(szFile);
#else
    if (0 == remove(szFile))
    {
        return TRUE;
    }

    return FALSE;
#endif
}

/**
 * @brief           删除整个目录
 * @param           szDir    [in] 所要删除的目录
 * @return          BOOL    删除成功返回true，反之返回 false。
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par 示例:       删除 tmp 目录
 * @code
 * gos_delete_dir_full("/home/tmp")
 * @endcode
 * @note
 * 1. 先删除这个目录下的文件和子目录，最后再删除该目录。
 */
BOOL gos_delete_dir_full(const CHAR *szDir)
{
    CHAR    acFile[1024];
    BOOL    bIsDirFile;
    HANDLE  hDir = NULL;

    hDir = gos_open_dir(szDir);
    if (!hDir)
    {
        return FALSE;
    }

    while(gos_get_next_file(hDir, acFile, &bIsDirFile))
    {
        if (bIsDirFile)
        {
            gos_delete_dir_full(acFile);
        }
        else
        {
            gos_delete_file(acFile);
        }
    }

    gos_close_dir(hDir);

#ifdef _OSWIN32_
    RemoveDirectory(szDir);
    return !gos_file_exist(szDir);
#else

    if (0 == remove(szDir))
    {
        return TRUE;
    }

    return FALSE;
#endif
}

/**
 * @brief           删除目录下的所有文件或目录
 * @param           szDir    [in] 所要删除的目录
 * @return          BOOL    删除成功返回true，反之返回 false。
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par 示例:       删除 tmp 目录
 * @code
 * gos_delete_dir("/home/tmp")
 * @endcode
 * @note
 * 1. 删除该目录下的子目录和文件，删除之后，该目录仍保留。
 */
BOOL gos_delete_dir(const CHAR *szDir)
{
    CHAR    acFile[1024];
    BOOL    bIsDirFile;
    HANDLE  hDir = NULL;

    hDir = gos_open_dir(szDir);
    if (!hDir)
    {
        return FALSE;
    }

    while(gos_get_next_file(hDir, acFile, &bIsDirFile))
    {
        if (bIsDirFile)
        {
            gos_delete_dir_full(acFile);
        }
        else
        {
            gos_delete_file(acFile);
        }
    }

    gos_close_dir(hDir);

    return TRUE;
}

/**
 * @brief           获取目录信息
 * @param           szDir    [in]  所要获取信息的目录
 * @param           pulFileNum  [out] 返回目录下的文件数目
 * @return          BOOL 获取目录信息成功返回true，反之返回 false。
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par 示例:       获取 tmp 目录的信息
 * @code
 * gos_get_dir_info("/home/tmp")
 * @endcode
 * @note
 * 1.若该目录还有子目录，则会递归查询某个子目录下的文件数目，返回的是该目录下的所有文件个数。
 */
BOOL gos_get_dir_info(const CHAR *szDir, UINT32 *pulFileNum)
{
    CHAR    acFile[256];
    BOOL    bIsDirFile;
    HANDLE  hDir = NULL;
    UINT32  ulFileNum = 0;

    *pulFileNum = 0;

    hDir = gos_open_dir(szDir);
    if (!hDir)
    {
        return FALSE;
    }

    while(gos_get_next_file(hDir, acFile, &bIsDirFile))
    {
        if (bIsDirFile)
        {
            if (!gos_get_dir_info(acFile, &ulFileNum))
            {
                return FALSE;
            }

            pulFileNum[0] += ulFileNum;
        }
        else
        {
            pulFileNum[0] ++;
        }
    }

    gos_close_dir(hDir);

    return TRUE;
}

/**
 * @brief           重命名文件
 * @param           szOldFile    [in] 旧的文件名
 * @param           szNewFile    [out] 重命名之后的文件名
 * @return          BOOL 重命名成功返回 true，反之返回 false。
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par 示例:       将 old.log 重命名 new.log
 * @code
 * gos_rename_file("/hmoe/tmp/old.log","/home/tmp/new.log")
 * @endcode
 * @note
 * 1.文件名必须是绝对路径
 */
BOOL gos_rename_file(const CHAR* szOldFile, const CHAR* szNewFile)
{
#ifdef _OSWIN32_
    INT32   iRet;
    SHFILEOPSTRUCT   op = {0};

    if (!gos_file_exist(szOldFile))
    {
        return FALSE;
    }

    gos_delete_file(szNewFile);
    iRet = rename(szOldFile, szNewFile);
    if (iRet == 0)
    {
        return TRUE;
    }

    op.pFrom  = szOldFile;
    op.pTo    = szNewFile;
    op.fFlags = FOF_SILENT;
    op.wFunc  = FO_RENAME;
    if (0 == SHFileOperation(&op))
    {
        return TRUE;
    }

    return FALSE;
#else
    INT32   iRet;
    CHAR    acCmd[1024];

    if (!gos_file_exist(szOldFile))
    {
        return FALSE;
    }

    gos_delete_file(szNewFile);
    iRet = rename(szOldFile, szNewFile);
    if (iRet == 0)
    {
        return TRUE;
    }

    memset(acCmd, 0, sizeof(acCmd));
    snprintf(acCmd, sizeof(acCmd)-1, "mv %s %s", szOldFile, szNewFile);
    gos_system(acCmd);

    if (gos_file_exist(szOldFile))
    {
        return FALSE;
    }

    return TRUE;
#endif
}

/**
 * @brief           复制文件
 * @param           szOldFile    [in]  被复制文件
 * @param           szNewFile    [out] 复制之后的文件
 * @return          BOOL 复制成功返回true，反之返回 false。
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par 示例:       将 sa.log ，复制为 sacpy.log
 * @code
 * gos_copy_file("/home/tmp/sa.log","/home/tmp/sacpy.log")
 * @endcode
 * @note
 * 1.一定要是绝对路径
 */
BOOL gos_copy_file(const CHAR* szOldFile, const CHAR* szNewFile)
{
    CHAR    acFileDir[256];

#ifndef _OSWIN32_
    FILE    *fpRead;
    FILE    *fpWrite;
    BOOL    bRet = TRUE;
    UINT8   aucBuf[4096];
    UINT32  ulReadSize;
    UINT32  ulWriteSize;
#endif

    gos_get_file_path(szNewFile, acFileDir);
    if (acFileDir[0] && !gos_file_exist(acFileDir))
    {
        if (!gos_create_redir(acFileDir))
        {
            return FALSE;
        }

        gos_set_dir_writable(acFileDir, TRUE);
    }

#ifdef _OSWIN32_
    return CopyFileA(szOldFile, szNewFile, FALSE);
#else
    if (strcmp(szOldFile, szNewFile) == 0)
    {
        return TRUE;
    }

    fpRead = fopen(szOldFile, "rb");
    if (!fpRead)
    {
        return FALSE;
    }

    fpWrite = fopen(szNewFile, "wb");
    if (!fpWrite)
    {
        fclose(fpRead);
        return FALSE;
    }

    while(!feof(fpRead))
    {
        ulReadSize = fread(aucBuf, 1, sizeof(aucBuf), fpRead);
        if (ulReadSize < sizeof(aucBuf))
        {
            if (0 == ulReadSize)
            {
                break;
            }

            if (ferror(fpRead) || !feof(fpRead))
            {
                bRet = FALSE;
                break;
            }
        }

        ulWriteSize = fwrite(aucBuf, 1, ulReadSize, fpWrite);
        if (ulWriteSize != ulReadSize)
        {
            bRet = FALSE;
            break;
        }
    }

    fclose(fpRead);
    fclose(fpWrite);

    return bRet;

#endif
}

VOID gos_init_socket_addr(SOCKADDR_IN *pstSockAddr, UINT8 *pucAddr, UINT16 usPort)
{
    memset(pstSockAddr, 0, sizeof(SOCKADDR_IN));

    pstSockAddr->sin_family = PF_INET;
    pstSockAddr->sin_port   = htons(usPort);
    memcpy(&pstSockAddr->sin_addr.s_addr, pucAddr, 4);
}

BOOL gos_get_socket_addr(SOCKET stSock, UINT8 *pucAddr, UINT16 *pusPort)
{
    SOCKADDR_IN     stSockAddr;
    INT32           iRet;
    INT32           iLen = sizeof(SOCKADDR_IN);

    memset(&stSockAddr, 0, sizeof(stSockAddr));
    iRet = getsockname(stSock, (SOCKADDR*)&stSockAddr, (socklen_t*)&iLen);
    if (0 != iRet)
    {
        return FALSE;
    }

    memcpy(pucAddr, &stSockAddr.sin_addr.s_addr, 4);
    if (pusPort)
    {
        *pusPort = stSockAddr.sin_port;
    }

    return TRUE;
}

INT32 gos_get_socket_err_code(VOID)
{
    INT32 iErrCode;

#ifdef _OSWIN32_
    iErrCode = WSAGetLastError();
#else
    iErrCode = errno;
#endif

    return iErrCode;
}

const CHAR* gos_get_socket_err(VOID)
{
    INT32 iErrCode;

#ifdef _OSWIN32_
    iErrCode = WSAGetLastError();
#else
    iErrCode = errno;
#endif

    switch (iErrCode)
    {
    case SEDESTADDRREQ:
        return "Socket error: Destination address required!";

    case SEPROTOTYPE:
        return "Socket error: Protocol wrong type for socket!";

    case SENOPROTOOPT:
        return "Socket error: Protocol not available!";

    case SEPROTONOSUPPORT:
        return "Socket error: Protocol not supported!";

    case SESOCKTNOSUPPORT:
        return "Socket error: Socket type not supported!";

    case SEOPNOTSUPP:
        return "Socket error: Operation not supported on socket!";

    case SEPFNOSUPPORT:
        return "Socket error: Protocol family not supported!";

    case SEAFNOSUPPORT:
        return "Socket error: Addr family not supported!";

    case SEADDRINUSE:
        return "Socket error: Port already in use!";

    case SEADDRNOTAVAIL:
        return "Socket error: Can't assign requested address!";

    case SENOTSOCK:
        return "Socket error: Socket operation on non-socket!";

    case SENETUNREACH:
        return "Socket error: Network is unreachable!";

    case SENETRESET:
        return "Socket error: Network dropped connection on reset!";

    case SECONNABORTED:
        return "Socket error: Software caused connection abort!";

    case SECONNRESET:
        return "Socket error: Connection reset by peer!";

    case SENOBUFS:
        return "Socket error: No buffer space available!";

    case SEISCONN:
        return "Socket error: Socket is already connected!";

    case SENOTCONN:
        return "Socket error: Socket is not connected!";

    case SETOOMANYREFS:
        return "Socket error: Too many references: can't splice!";

    case SESHUTDOWN:
        return "Socket error: Can't send after socket shutdown!";

    case SETIMEDOUT:
        return "Socket error: Connection timed out!";

    case SECONNREFUSED:
        return "Socket error: Connection refused!";

    case SENETDOWN:
        return "Socket error: Network is down!";

    case SELOOP:
        return "Socket error: Too many levels of symbolic links!";

    case SEHOSTUNREACH:
        return "Socket error: No route to host!";

    case SEHOSTDOWN:
        return "Socket error: Peer is down!";

    case SEINPROGRESS:
        return "Socket error: Operation now in progress!";

    case SEALREADY:
        return "Socket error: Operation already in progress!";

    case SEWOULDBLOCK:
        return "Socket error: Operation would block!";

    case SENOTINITIALISED:
        return "Socket error: Not initialized!";

    case SEFAULT:
        return "Socket error: Bad ip address!";

    case SEINVAL:
        return "Socket error: Invalid parameter!";

    default:
        return "Socket error: Unknown error!";
    }
}

INT32 gos_connect(SOCKET tSock, SOCKADDR *ptSockAddr, INT32 iAddrLen, UINT32 ulTimeout)
{
    struct timeval  tTimeout;
    u_long          ulNonBlock;
    fd_set          tWriteFds;
    SOCKADDR        tPeerSockAddr;
    INT             iLen;
    INT             iResult;
    INT             iRetVal = 0;
    INT             iSockErr;
    INT             iErrno;

    if (0 == ulTimeout)
    {
        return connect(tSock, ptSockAddr, iAddrLen);
    }

    iSockErr = 0;
    ulNonBlock = 1;
#ifdef WIN32
    //WSAAsynSelect(tSock, NULL, NULL, 0);
#endif
    if ((ioctlsocket (tSock, FIONBIO, &ulNonBlock)) == SOCKET_ERROR)
    {
        //return SOCKET_ERROR;
    }

    if (connect (tSock, ptSockAddr, iAddrLen) == SOCKET_ERROR)
    {
        iErrno = errno;
        /*
         * when socket is set to non-blocking mode, connect() will return
         * with WSAEWOULDBLOCK if a connection cannot be setup immediately.
         */

#ifdef _OSWIN32_
        if ((iErrno == SEINPROGRESS) ||
             (iErrno == SETIMEDOUT) ||
             (iErrno == SENOTINITIALISED) ||
             (iErrno == SEWOULDBLOCK) ||
             (iErrno == 2))
#else
        if ((iErrno == SEINPROGRESS) ||
             (iErrno == SETIMEDOUT) ||
             (iErrno == SENOTINITIALISED))
#endif
        {
            /*
             * We do synchronous polling on the write side of this socket
             * using select() with the given <timeVal> timeout value until
             * the connection gets established.
             */
            FD_ZERO (&tWriteFds);
            FD_SET (tSock, &tWriteFds); /* poll write-side of <sock> */
            tTimeout.tv_sec = ulTimeout/1000;
            tTimeout.tv_usec = 1000*MOD(ulTimeout, 1000);
            iResult = select (FD_SETSIZE,
                              (fd_set *) NULL,
                              &tWriteFds,
                              (fd_set *) NULL,
                              &tTimeout
                             );
            if (iResult == SOCKET_ERROR)
            {
                iSockErr = GetSockErr ();
                iRetVal = SOCKET_ERROR;
            }
            else if (iResult == 0) /* time out */
            {
                iSockErr = SETIMEDOUT;
                iRetVal = SOCKET_ERROR;
            }
            else /* if (iResult == SOCKET_ERROR) */
            {
                /*
                 * select() is successful, see if our socket is actually
                 * selected for writing.
                 */
                if (FD_ISSET (tSock, &tWriteFds))
                {
                    /*
                     * connection attempt has completed.  we now see if
                     * it was a successful attempt by trying to get the
                     * remote peer's address.  If getpeername() succeeds
                     * we have a connection, otherwise the connect() has
                     * failed.
                     */
                    iLen = sizeof (tPeerSockAddr);
                    if (getpeername (tSock, &tPeerSockAddr, (socklen_t*)&iLen) != SOCKET_ERROR)
                    {
                        iRetVal = 0;
                    }
                    else
                    {
                        iSockErr = GetSockErr ();
                        iRetVal = SOCKET_ERROR;
                    }
                }
                else
                {
                    iSockErr = GetSockErr ();
                    iRetVal = SOCKET_ERROR;
                }
            }
        }
        else
        {
            iSockErr = GetSockErr ();
            iRetVal = SOCKET_ERROR;
        }
    }
    else
    {
        iRetVal = 0;
    }

    /* 再把 SOCKET 设置为 BLOCK 模式 */
    ulNonBlock = 0;
    if ((ioctlsocket (tSock, FIONBIO, &ulNonBlock)) == SOCKET_ERROR)
    {
        iSockErr = GetSockErr ();
        iRetVal = SOCKET_ERROR;
    }

    if (iSockErr != 0)
    {
        SetSockErr (iSockErr);
    }

    return (iRetVal);
}

BOOL gos_tcp_send(SOCKET stSocket, VOID *pstReqMsg, UINT32 ulReqMsgLen)
{
    INT32   iLeftLen = ulReqMsgLen;
    INT32   iSendLen;
    CHAR    *szData = (CHAR*)pstReqMsg;

    if (stSocket == INVALID_SOCKET)
    {
        return FALSE;
    }

    while(iLeftLen > 0)
    {
        iSendLen = send(stSocket, szData, iLeftLen, 0);
        if (iSendLen < 0)
        {
            return FALSE;
        }

        szData += iSendLen;
        iLeftLen -= iSendLen;
    }

    return TRUE;
}

VOID gos_close_socket(SOCKET *pSocket)
{
    if (pSocket)
    {
        CLOSE_SOCKET(*pSocket);
    }
}


/**
 * @brief           连接 tcp server
 * @param           pucServerAddr   [in]
 * @param           usPort          [in]
 * @return          SOCKET
 * @retval          INVALID_SOCKET 连接失败
 * @retval          SOCKET 连接成功且返回 SOCKET 描述符
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-05 14:04:33
 * @note
 */
SOCKET gos_connect_tcp_server(UINT8 *pucServerAddr, UINT16 usPort)
{
    SOCKADDR_IN         stServerAddr = {0};
    UINT32              ulTimeout = 0;
    UINT32              ulFlag = 1;
    INT32               iNoDelay = 1;
    SOCKET              stSocket;

    gos_init_socket();

    stServerAddr.sin_family = PF_INET;
    stServerAddr.sin_port   = htons(usPort);
    memcpy(&stServerAddr.sin_addr.s_addr, pucServerAddr, 4);

    stSocket = socket (PF_INET, SOCK_STREAM, 0);
    if (stSocket == INVALID_SOCKET)
    {
        return INVALID_SOCKET;
    }

    if (setsockopt(stSocket, SOL_SOCKET, SO_REUSEADDR, (CHAR*)&ulFlag, sizeof(ulFlag)) == SOCKET_ERROR)
    {
        CLOSE_SOCKET(stSocket);

        return INVALID_SOCKET;
    }

    if (gos_connect(stSocket, (SOCKADDR *)&stServerAddr, sizeof (SOCKADDR), ulTimeout) == SOCKET_ERROR)
    {
        CLOSE_SOCKET(stSocket);

        return INVALID_SOCKET;
    }

    setsockopt(stSocket, IPPROTO_TCP, TCP_NODELAY, (CHAR*)&iNoDelay, sizeof(iNoDelay));

    return stSocket;
}

BOOL gos_send_tcp_msg(UINT8 *pucServerAddr, UINT16 usPort, VOID *pvMsg, UINT32 ulMsgLen)
{
    SOCKET          stSocket = gos_connect_tcp_server(pucServerAddr, usPort);
    BOOL            bRet;

    if (stSocket == INVALID_SOCKET)
    {
        return FALSE;
    }

    bRet = gos_tcp_send(stSocket, pvMsg, ulMsgLen);

    CLOSE_SOCKET(stSocket);

    return bRet;
}

/**
 * @brief           从一个已经连接的 tcp SOCKET 上获取消息
 * @param           stSock          [in]    已经连接的 tcp SOCKET
 * @param           pMsgBuf         [out]   指向接收缓冲区的指针
 * @param           ulMsgBufLen     [in]    接收消息长度
 * @param           ulTimeout       [in]    超时时间，单位是毫秒
 * @return          INT32
 * @retval          -1  stSock 非法或接收错误
 * @retval          0   ulMsgBufLen 为零时返回
 * @retval          int 返回接收到数据的长度
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-05 13:22:29
 * @note
 */
INT32 gos_recv_tcp_msg(SOCKET stSock, VOID *pMsgBuf, UINT32 ulMsgBufLen, UINT32 ulTimeout)
{
    FD_SET  fds;
    TIMEVAL stTimeout = {0};
    INT32   iRet;
    INT32   iRecvLen;
    INT32   iTotalRecvLen = 0;
    CHAR    *szMsgBuf = (CHAR*)pMsgBuf;
    INT32   iTimeout;
    UINT32  ulStartTime = gos_get_uptime_1ms();
    UINT32  ulTimePassed;

    if (stSock == INVALID_SOCKET)
    {
        return -1;
    }

    if (0 == ulMsgBufLen)
    {
        return 0;
    }

    while(1)
    {
        ulTimePassed = gos_get_uptime_1ms() - ulStartTime;
        iTimeout = ulTimeout - ulTimePassed;

        if (iTimeout <= 0)
        {
            return iTotalRecvLen;
        }

        stTimeout.tv_sec = iTimeout/1000;
        stTimeout.tv_usec = 1000*MOD(iTimeout,1000);
        FD_ZERO(&fds);
        FD_SET(stSock, &fds);

        iRet = select(stSock + 1, &fds, NULL, NULL, &stTimeout);
        if (iRet == 0)
        {
            return iTotalRecvLen;
        }

        if (iRet < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            return -1;
        }

        if (FD_ISSET(stSock, &fds))
        {
            iRecvLen = recv(stSock, szMsgBuf + iTotalRecvLen, ulMsgBufLen - iTotalRecvLen, 0);
            if (iRecvLen < 0)
            {
                return -1;
            }
            else if (iRecvLen == 0)
            {
                if (iTotalRecvLen == 0)
                {
                    return -1;
                }

                return iTotalRecvLen;
            }
            else
            {
                iTotalRecvLen += iRecvLen;
                if (ulMsgBufLen == (UINT32)iTotalRecvLen)
                {
                    return iTotalRecvLen;
                }
            }
        }
    }
}

BOOL gos_udp_send(SOCKET stSocket, VOID *pstReqMsg, UINT32 ulReqMsgLen, SOCKADDR_IN *pstAddr)
{
    INT32   iSendLen;
    CHAR    *szData = (CHAR*)pstReqMsg;

    if (stSocket == INVALID_SOCKET)
    {
        return FALSE;
    }

    if (!pstAddr)
    {
        iSendLen = send(stSocket, szData, ulReqMsgLen, 0);
    }
    else
    {
        iSendLen = sendto(stSocket, szData, ulReqMsgLen, 0, (SOCKADDR *)pstAddr, sizeof(SOCKADDR));
    }

    return iSendLen == (INT32)ulReqMsgLen;
}

SOCKET gos_create_udp_socket(VOID)
{
    SOCKET              stSocket;

    gos_init_socket();

    stSocket = socket (AF_INET, SOCK_DGRAM, 0);
    if (stSocket == INVALID_SOCKET)
    {
        return INVALID_SOCKET;
    }

    return stSocket;
}

SOCKET gos_create_udp_server_socket(UINT8 *pucAddr, UINT16 usPort)
{
    SOCKET              stSocket;
    SOCKADDR_IN         stLocalAddr = {0};
    UINT32              ulFlag = 1;

    gos_init_socket();

    stSocket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (stSocket == INVALID_SOCKET)
    {
        return INVALID_SOCKET;
    }

    memset(&stLocalAddr, 0, sizeof(stLocalAddr));
    stLocalAddr.sin_family = PF_INET;
    stLocalAddr.sin_port   = htons(usPort);
    if (pucAddr)
    {
        memcpy(&stLocalAddr.sin_addr.s_addr, pucAddr, 4);
    }

    if (setsockopt(stSocket, SOL_SOCKET, SO_REUSEADDR, (CHAR*)&ulFlag, sizeof(ulFlag)) == SOCKET_ERROR)
    {
        CLOSE_SOCKET(stSocket);

        return INVALID_SOCKET;
    }

    if (bind (stSocket, (SOCKADDR *) &stLocalAddr, sizeof (SOCKADDR)) == SOCKET_ERROR)
    {
        CLOSE_SOCKET (stSocket);

        return INVALID_SOCKET;
    }

    return stSocket;
}

INT32 gos_recv_udp_msg(SOCKET stSock, VOID *pMsgBuf, UINT32 ulMsgBufLen, UINT32 ulTimeout, SOCKADDR_IN *pstAddr)
{
    FD_SET          fds;
    TIMEVAL         stTimeout = {0};
    INT32           iRet;
    INT32           iRcvLen;
    socklen_t       iLen = sizeof(SOCKADDR);
    UINT32          ulStartTime = gos_get_uptime_1ms();
    UINT32          ulPassedTime;
    INT32           iLeftTime;

    /* 同步调用的超时时间必定大于0，异步调用不需要recv */
    if (ulTimeout == 0)
    {
        iRcvLen = recv(stSock, (char*)pMsgBuf, ulMsgBufLen, 0);
        return iRcvLen;
    }

again:
    ulPassedTime = gos_get_uptime_1ms() - ulStartTime;
    iLeftTime = ulTimeout - ulPassedTime;
    if (iLeftTime <= 0)
    {
        return 0;
    }

    stTimeout.tv_sec = iLeftTime/1000;
    stTimeout.tv_usec = 1000*(iLeftTime%1000);
    FD_ZERO(&fds);
    FD_SET(stSock, &fds);

    iRet = select(stSock + 1, &fds, NULL, NULL, &stTimeout);
    if (iRet == 0)
    {
        return 0;
    }

    if (iRet < 0)
    {
        if (errno == EINTR)
        {
            goto again;
        }

        return -1;
    }

    if (FD_ISSET(stSock, &fds))
    {
        if (!pstAddr)
        {
            iRcvLen = recv(stSock, (char*)pMsgBuf, ulMsgBufLen, 0);
        }
        else
        {
            iRcvLen = recvfrom(stSock, (char*)pMsgBuf, ulMsgBufLen, 0, (SOCKADDR *)pstAddr, &iLen);
        }

        FD_CLR(stSock, &fds);

        return iRcvLen;
    }

    FD_CLR(stSock, &fds);
    return 0;
}

#ifdef GOS_USE_SEM
/*====================================================================
函数名          : gos_lock_init
功能            : 创建锁
算法实现        : 无
引用全局变量    : 无
输入参数说明    : 无
返回值说明      : 返回新创建锁的句柄，如果为NULL，表示创建失败
====================================================================*/
HANDLE gos_lock_init(VOID)
{
#ifdef _OSWIN32_
    return CreateMutex(NULL, FALSE, NULL);
#else
    sem_t   *pstSem = (sem_t*)malloc(sizeof(sem_t));

    if (pstSem == NULL)
    {
        return NULL;
    }

    if (sem_init(pstSem, 0, 1) != 0)
    {
        free(pstSem);
        return NULL;
    }

    return (HANDLE)pstSem;
#endif
}

/*====================================================================
函数名          : gos_lock_free
功能            : 删除锁
算法实现        :
引用全局变量    :
输入参数说明    :
返回值说明      :
====================================================================*/
BOOL gos_lock_free(HANDLE hLock)
{
    if (!hLock)
    {
        return FALSE;
    }

#ifdef _OSWIN32_
    return CloseHandle(hLock);
#else
    if (sem_destroy((sem_t*)hLock) == -1)
    {
        free(hLock);
        return FALSE;
    }
    else
    {
        free(hLock);
        return TRUE;
    }
#endif
}

/*====================================================================
函数名          : gos_lock
功能            : 加锁操作
算法实现        :
引用全局变量    :
输入参数说明    :
返回值说明      :
====================================================================*/
BOOL gos_lock(HANDLE hLock, UINT32 ulTimeout)
{
#ifdef _OSWIN32_
    UINT32    ulRet;

    if (hLock == NULL)
    {
        return FALSE;
    }

    ulRet = WaitForSingleObject(hLock, ulTimeout);
    if (ulRet == WAIT_OBJECT_0)
    {
        return TRUE;
    }

    if (ulRet == WAIT_TIMEOUT)
    {
        return FALSE;
    }

    return FALSE;
#else
    struct timespec timeout;

    if (hLock == NULL)
    {
        return FALSE;
    }

    if (ulTimeout == WAIT_FOREVER)
    {
        if(0 == sem_wait((sem_t*)hLock))
        {
            return TRUE;
        }
    }
    else
    {
        if (ulTimeout == NO_WAIT)
        {
            timeout.tv_sec = 0;
            timeout.tv_nsec = 0;
        }
        else
        {
            clock_gettime(CLOCK_REALTIME, &timeout);
            timeout.tv_sec += ulTimeout/1000;
            timeout.tv_nsec += (ulTimeout%1000)*1000000;
            while (timeout.tv_nsec >= 1000000000)
            {
                timeout.tv_sec ++;
                timeout.tv_nsec -= 1000000000;
            }
        }

        if (0 == sem_timedwait((sem_t*)hLock, &timeout))
        {
            return TRUE;
        }
        else
        {
            if (errno == ETIMEDOUT)
            {
                return FALSE;
            }
        }
    }

    return FALSE;
#endif
}

/*====================================================================
函数名          : gos_unlock
功能            : 解锁操作
算法实现        :
引用全局变量    :
输入参数说明    :
返回值说明      :
====================================================================*/
BOOL gos_unlock(HANDLE hLock)
{
    if (hLock == NULL)
    {
        return FALSE;
    }

#ifdef _OSWIN32_
    if (0 == ReleaseMutex(hLock))
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
#else
    if (sem_post((sem_t*)hLock) != 0)
    {
        return FALSE;
    }

    return TRUE;
#endif
}
#endif

VOID* gos_malloc(UINT32 ulSize)
{
    return malloc(ulSize);
}

VOID gos_free(VOID* pMem)
{
    if (pMem)
    {
        free(pMem);
    }
}

#ifdef _OSWIN32_
typedef BOOL (WINAPI * EXIT_WINDOWS_FUNC)(UINT uFlags, DWORD dwReserved);
#endif

/**
 * @brief           执行传入命令行命令，如果传入为NULL则检查本地是否有命令行处理器，如果存在则内部返回非零
 * @param           szCmd    [in]   需要执行的命令
 * @return          VOID
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @par 示例:
 * @code
 * gos_system("pause"); ///< 执行命令行暂停命令
 * @endcode
 * @note
 */
VOID gos_system(const CHAR *szCmd)
{
    INT32 iRet = system(szCmd);

    (void)iRet; // < 防止 linux 下警告视为错误
}

/**
 * @brief           执行系统立刻重启命令
 * @return          VOID
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
VOID gos_reboot(VOID)
{
#ifdef _OSWIN32_
    static HINSTANCE            hInst = NULL;
    static EXIT_WINDOWS_FUNC    pfExitWindowsExFunc = NULL;

    if (NULL == hInst)
    {
        hInst = LoadLibrary("user32.dll");
    }

    if (hInst && NULL == pfExitWindowsExFunc)
    {
        pfExitWindowsExFunc = (EXIT_WINDOWS_FUNC) GetProcAddress(hInst, "ExitWindowsEx");
    }

    if (pfExitWindowsExFunc)
    {
        pfExitWindowsExFunc(EWX_FORCE | EWX_REBOOT, 0);
    }

    gos_sleep(1);
    gos_system("shutdown -r -t 0");
#else
    gos_system("reboot");
#endif
}

INT32 gos_get_ntp_time(UINT8 *pucAddr, UINT16 usPort)
{
    SOCKET      stSocket;
    SOCKADDR_IN stAddr;
    UINT32      ulSecondBetween1900And1970 = 0x83AA7E80; // 1900/1/1 至 1970/1/1 的秒数
    UINT32      ulNtpPacketLen  = 48;
    CHAR        acNtpMsg[256];
    UINT32      ulTime;
    UINT32      ulTimeout = 2000;
    INT32       iRecvLen;

    gos_init_socket();

    memset(&stAddr, 0, sizeof(stAddr));
    stAddr.sin_family = AF_INET;
    memcpy(&stAddr.sin_addr.s_addr, pucAddr, 4);
    stAddr.sin_port = htons(usPort);

    stSocket = socket (AF_INET, SOCK_DGRAM, 0);
    if (stSocket == INVALID_SOCKET)
    {
        return -1;
    }

    memset(acNtpMsg, 0, ulNtpPacketLen);
    acNtpMsg[0] = 0x1b;

    if (!gos_udp_send(stSocket, acNtpMsg, ulNtpPacketLen, &stAddr))
    {
        closesocket(stSocket);
        return -1;
    }

    iRecvLen = gos_recv_udp_msg(stSocket, acNtpMsg, sizeof(acNtpMsg), ulTimeout, NULL);
    closesocket(stSocket);

    if (iRecvLen < 44)
    {
        return -1;
    }

    memcpy(&ulTime, acNtpMsg + 40, 4);

    ulTime = ntohl(ulTime) - ulSecondBetween1900And1970;

    return ulTime;
}
