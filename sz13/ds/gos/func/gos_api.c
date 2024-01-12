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
    if (ReleaseSemaphore (hSem, dwCount, NULL) == 0) /* �Էǻ����ź����Ĵ��� */
    {
        if (ERROR_INVALID_HANDLE == GetLastError ()) /* ����������ǻ����ź��� */
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
 * @brief          ��ȡϵͳ�ϵ�ʱ��
 * @return         UINT32����ϵͳ�ϵ������ʱ�� �뼶
 * @author         zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @parʾ��:
 * @code
 *
 * @endcode
 * @note
 *    struct sysinfo{
 *      long uptime;             //���������ھ�����ʱ��?
 *      unsigned long loads[3];
 *      unsigned long totalram;  //�ܵĿ��õ��ڴ��С??
 *      unsigned long freeram;   //��δ��ʹ�õ��ڴ��С??
 *      unsigned long sharedram; //����Ĵ洢���Ĵ�С
 *      unsigned long bufferram; //����Ĵ洢���Ĵ�С?
 *      unsigned long totalswap; //��������С?
 *      unsigned long freeswap;  //�����õĽ�������С?
 *      unsigned short procs;    //��ǰ������Ŀ??
 *      unsigned long totalhigh; //�ܵĸ��ڴ��С?
 *      unsigned long freehigh; //���õĸ��ڴ��С
 *      unsigned int mem_unit;  //���ֽ�Ϊ��λ���ڴ��С??
 *      char _f[20-2*sizeof(long)-sizeof(int)];
 *   };
 *   ԭ�����ӣ�https://blog.csdn.net/qq_33706673/article/details/78187559
 *
 */
UINT32 gos_get_sys_uptime(VOID)
{
#ifdef _OSWIN32_
    return GetTickCount()/1000;
#else
    struct sysinfo info;

    //�ɹ�����0 ���󷵻� -1
    if( 0 == sysinfo( &info ) )
    {
        return (unsigned long)info.uptime;
    }

    return 0;
#endif
}

/*
CLOCK_REALTIME:ϵͳʵʱʱ��,��ϵͳʵʱʱ��ı���ı�,����UTC1970-1-1 0:0:0��ʼ��ʱ,�м�ʱ�����ϵͳʱ�䱻�û��ĳ�����,���Ӧ��ʱ����Ӧ�ı�
CLOCK_MONOTONIC:��ϵͳ������һ����ʼ��ʱ,����ϵͳʱ�䱻�û��ı��Ӱ��
CLOCK_PROCESS_CPUTIME_ID:�����̵���ǰ����ϵͳCPU���ѵ�ʱ��
CLOCK_THREAD_CPUTIME_ID:���̵߳���ǰ����ϵͳCPU���ѵ�ʱ��
*/

/**
 * @brief          ��ȡϵͳ�ϵ�ʱ�� ���뼶
 * @return         UINT32  ���غ��뼶��ϵͳ�ϵ��ʱ��
 * @author         zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @parʾ��:
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
    INT64           ims;        /* ����(=1000΢��)*/
    INT32           nsec;       /* ����*/

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
 * @brief           ��ȡ΢�뾫�ȵ�ʱ�䣬����ϵͳ��ͬ�����ز�ͬ���
 * @details         WIN32ϵͳ�»�ȡϵͳ���������ĺ����� * 1000��
 *                  Linuxϵͳ�»�ȡ��Epoch(1970��1��1�� 00:00:00)��΢����
 * @return          UINT64  ��ȡ���ĺ�����
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
 * @brief          ��ȡ������е����ڵ�ʱ�� �뼶
 * @return         UINT32
 * @author         zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @parʾ��:
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
 * @brief          ��ȡϵͳ���е����ڵ�ʱ�� ���뼶
 * @return         UINT32
 * @author         zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par ʾ��:
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
    INT64           ims;        /* ����(=1000΢��)*/
    INT32           nsec;       /* ����*/

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
 * @brief           ��ȡ���뾫�ȵ�ʱ��, ����ϵͳ��ͬ�����ز�ͬ���
 * @details         WIN32ϵͳ�»�ȡ�������������ĺ�������
 *                  Linuxϵͳ�»�ȡ��Epoch(1970��1��1�� 00:00:00)�ĺ�����
 * @return          UINT64  ��ȡ���ĺ�����
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
 * @brief           ���߳��������ɺ���
 * @param           dwTimeout    [in]
 * @return          VOID
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-02-10 11:30:50
 * @note
 * lijiancong(2022-02-10): Windows API: Sleep ������Ϊʱ����Ƭ��ԭ�����0~18ms���ӳ١�
 *                         ������ 5000ms ��ʵ��ʱ��������п��ܹ�ȥ��5015ms
 * �ο�����:
 * [Sleep function (synchapi.h)](https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-sleep)
 * [���WINDOWS SLEEP��������](https://blog.liboliu.com/a/78)
 * [Windows����sleep���ȵĲ��ԣ��������΢��](https://blog.csdn.net/liuhengxiao/article/details/99641539)
 */
VOID gos_sleep_1ms(UINT32 ulTimeout)
{
#ifdef _OSWIN32_
    timeBeginPeriod(1);  ///< ���þ���Ϊ1����
    Sleep(ulTimeout);
    timeEndPeriod(1);    ///< ������������
#else
    /* ms��tick */
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
 * @brief           �� pstTime ��ָ��Ľṹת��Ϊ1970���������
 * @param           pstTime [in]
 * @return          UINT32
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @parʾ��:
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
 * @brief          ��pulCurrTime����ʱ��ת��Ϊ��ʵ�����ʱ���﷽ʽ
 * @param          pulCurrTime [in] ������� ��Ҫת����ʱ�� �뼶
 * @param          pstTime [out] ������� ʱ��ṹ��
 * @return         BOOL�ɹ� ������TRUE ʧ�� �� ����FALSE
 * @author         zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par ʾ��:
 * @code
 *
 * @endcode
 * @note
 *1�� gmtime()����ת�����ֵ��UTCʱ��û�о���ʱ���任
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
 * @brief          �� pulCurrTime ת��Ϊ����ʱ���任��ı���ʱ��ṹ
 * @param          pulCurrTime [in]  ������� ��Ҫת����ʱ�� �뼶
 * @param          pstTime [out]     ������� ת�����ʱ��ṹ
 * @return         BOOL
 * @author         zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par ʾ��:
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
 * @brief           ��ʱ���ת���ɵ�������ʱ���
 * @param           ulTime    [in]
 * @return          UINT32
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par ʾ��:
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
UINT32 gos_get_dayweek(UINT16 usYear, UINT8 ucMonth, UINT8 ucDay)   //��ķ����ɭ��ʽ
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
 * @brief           �����߳�
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par ʾ��:
 * @code
 *
 * @endcode
 * @note
 * 1.�����̴߳�����
 */
#ifdef GOS_USE_PTHREAD

/**
 * @brief
 * @param           pfThreadEntry    [in] ����ָ�룬�̵߳Ĺ�������
 * @param           dwStacksize      [in]
 * @param           pPara            [in]
 * @return          THREAD_TOKEN
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par ʾ��:
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
    pthread_attr_setdetachstate(&stAttr, PTHREAD_CREATE_DETACHED);  // ��������̬������̬��
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
 * @brief           �����ػ�����
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par ʾ��:
 * @code
 *
 * @endcode
 * @note
 * 1.fork�ӽ���, �������˳�, ���й������ӽ����н�����ʽ�������˿����ն�; ����
 * 2.�ӽ��̴����»Ự, setsid����, ʹ�ӽ�����ȫ��������, �������; ����
 * 3.�ı䵱ǰĿ¼Ϊ��Ŀ¼, chdir()����, ��ֹռ�ÿ�ж�ص��ļ�ϵͳ, Ҳ���Ի�������·���� Ϊ����ǿ����Ľ�׳��; �Ǳ���
 * 4.�����ļ�Ȩ������, umask()����, ��ֹ�̳е��ļ����������־ܾ�ĳЩȨ��, �����ػ����������; �Ǳ���
 * 5. �ر��ļ�������, �̳еĴ��ļ������õ�, �˷�ϵͳ��Դ, �޷�ж��, close(0), close(1), close(2); �Ǳ���
 * 6.ִ�к��Ĺ���
 *
 * lijiancong: ��ϸʾ���� ��UNIX �����߼���̡� 3edit�� ��13���ػ����� 13.3С�� Page 374
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
        exit(0);        //�Ǹ����̣�����������
    }
    else if(pid< 0)
    {
        exit(1);        //forkʧ�ܣ��˳�
    }

    //�ǵ�һ�ӽ��̣���̨����ִ��
    setsid();           //��һ�ӽ��̳�Ϊ�µĻỰ�鳤�ͽ����鳤

    //��������ն˷���
    if (pid = fork())
    {
        exit(0);        //�ǵ�һ�ӽ��̣�������һ�ӽ���
    }

    else if(pid< 0)
    {
        exit(1);        //forkʧ�ܣ��˳�
    }

    //�ǵڶ��ӽ��̣�����
    //�ڶ��ӽ��̲����ǻỰ�鳤
    if (!bLogToStdout)
    {
        cnt = sysconf(_SC_OPEN_MAX);
        for (i=0; i<cnt; ++i)  //�رմ򿪵��ļ�������
        {
            close(i);
        }
    }

    //chdir("/tmp");      //�ı乤��Ŀ¼��/tmp

    umask(0);           //�����ļ�������ģ
    return;
}
#endif

/**
 * @brief           �ж��ļ���Ŀ¼�Ƿ����
 * @param           szFile    [in] ��Ҫ�жϵ��ļ���
 * @return          BOOL      �ļ����ڷ��� true����֮���� false��
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par ʾ��:       �ж� /home/tmp �� �Ƿ���sa.log
 * @code
 * gos_file_exist��"/home/tmp/sa.log"��
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

    /* 0 == F_OK, �ж��ļ�/Ŀ¼�Ƿ���� */
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
 * @brief           ����Ŀ¼
 * @param           szDir    [in] ����·��
 * @return          BOOL    ����Ŀ¼�ɹ����� true����֮���� false��
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par ʾ��:       �� home Ŀ¼�´��� tmp Ŀ¼
 * @code
 * gos_create_dir("/home/tmp")
 * @endcode
 * @note
 * 1. ���ܵݹ鴴��������Ŀ¼�����ڣ���ʧ��.
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

    /* ����Ŀ¼�����Ŀ¼������ */
    memcpy(szTempDir, szDir, ulLen);
    if (!IS_DIR_SPLIT(szTempDir[ulLen-1]))
    {
        szTempDir[ulLen] = '/';
        ulLen ++;   /* ulLenΪ��Ŀ¼���������ַ������� */
    }
    szTempDir[ulLen] = '\0';

    szFirstDir = szCurrDir;

#ifdef _OSWIN32_
    /* �жϿ�ͷ�������ַ��Ƿ�����X:\ */
    if (ulLen < 3 || szDir[1] != ':' || !IS_DIR_SPLIT(szDir[2]))
    {
        goto err;
    }
    ulStartPos = 3;
    szFirstDir[0] = szDir[0];
    szFirstDir[1] = ':';
    szFirstDir[2] = DIR_SEPARATOR;
#else
    /* �жϵ�һ���ַ��Ƿ���Ŀ¼�ָ��' */
    if (!IS_DIR_SPLIT(szDir[0]))
    {
        goto err;
    }
    ulStartPos = 1;
    szFirstDir[0] = DIR_SEPARATOR;
#endif

    /* ��ȡ��һ��Ŀ¼ */
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

    /* ������һ��Ŀ¼ */
    if (!gos_file_exist(szFirstDir))
    {
        if (!gos_create_dir_ex(szFirstDir, dwMode))
        {
            goto err;
        }
    }

    /* ѭ��������Ŀ¼ */
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

    /* ���һ���ж�Ŀ¼�Ƿ���� */
    if (!gos_file_exist(szTempDir)) //Ҳ��Ӧ����szCurrDir����
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
 * @brief           �ݹ鴴��Ŀ¼
 * @param           szDir    [in] ��Ҫ������Ŀ¼
 * @return          BOOL     ����Ŀ¼�ɹ����� true����֮����false��
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par ʾ��:       �� home Ŀ¼�´��� tmp Ŀ¼
 * @code
 * gos_create_redir("/home/tmp")
 * @endcode
 * @note
 * 1.�ɵݹ鴴������home Ŀ¼�����ڣ����ȴ��� hoem Ŀ¼��Ȼ���ٴ��� tmp Ŀ¼
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
 * @brief           ɾ���ļ�
 * @param           szFile    [in] ��Ҫɾ�����ļ�
 * @return          BOOL    ɾ���ɹ����� true����֮���� false��
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par ʾ��:       ɾ�� sa.log
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
 * @brief           ɾ������Ŀ¼
 * @param           szDir    [in] ��Ҫɾ����Ŀ¼
 * @return          BOOL    ɾ���ɹ�����true����֮���� false��
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par ʾ��:       ɾ�� tmp Ŀ¼
 * @code
 * gos_delete_dir_full("/home/tmp")
 * @endcode
 * @note
 * 1. ��ɾ�����Ŀ¼�µ��ļ�����Ŀ¼�������ɾ����Ŀ¼��
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
 * @brief           ɾ��Ŀ¼�µ������ļ���Ŀ¼
 * @param           szDir    [in] ��Ҫɾ����Ŀ¼
 * @return          BOOL    ɾ���ɹ�����true����֮���� false��
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par ʾ��:       ɾ�� tmp Ŀ¼
 * @code
 * gos_delete_dir("/home/tmp")
 * @endcode
 * @note
 * 1. ɾ����Ŀ¼�µ���Ŀ¼���ļ���ɾ��֮�󣬸�Ŀ¼�Ա�����
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
 * @brief           ��ȡĿ¼��Ϣ
 * @param           szDir    [in]  ��Ҫ��ȡ��Ϣ��Ŀ¼
 * @param           pulFileNum  [out] ����Ŀ¼�µ��ļ���Ŀ
 * @return          BOOL ��ȡĿ¼��Ϣ�ɹ�����true����֮���� false��
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par ʾ��:       ��ȡ tmp Ŀ¼����Ϣ
 * @code
 * gos_get_dir_info("/home/tmp")
 * @endcode
 * @note
 * 1.����Ŀ¼������Ŀ¼�����ݹ��ѯĳ����Ŀ¼�µ��ļ���Ŀ�����ص��Ǹ�Ŀ¼�µ������ļ�������
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
 * @brief           �������ļ�
 * @param           szOldFile    [in] �ɵ��ļ���
 * @param           szNewFile    [out] ������֮����ļ���
 * @return          BOOL �������ɹ����� true����֮���� false��
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par ʾ��:       �� old.log ������ new.log
 * @code
 * gos_rename_file("/hmoe/tmp/old.log","/home/tmp/new.log")
 * @endcode
 * @note
 * 1.�ļ��������Ǿ���·��
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
 * @brief           �����ļ�
 * @param           szOldFile    [in]  �������ļ�
 * @param           szNewFile    [out] ����֮����ļ�
 * @return          BOOL ���Ƴɹ�����true����֮���� false��
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par ʾ��:       �� sa.log ������Ϊ sacpy.log
 * @code
 * gos_copy_file("/home/tmp/sa.log","/home/tmp/sacpy.log")
 * @endcode
 * @note
 * 1.һ��Ҫ�Ǿ���·��
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

    /* �ٰ� SOCKET ����Ϊ BLOCK ģʽ */
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
 * @brief           ���� tcp server
 * @param           pucServerAddr   [in]
 * @param           usPort          [in]
 * @return          SOCKET
 * @retval          INVALID_SOCKET ����ʧ��
 * @retval          SOCKET ���ӳɹ��ҷ��� SOCKET ������
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
 * @brief           ��һ���Ѿ����ӵ� tcp SOCKET �ϻ�ȡ��Ϣ
 * @param           stSock          [in]    �Ѿ����ӵ� tcp SOCKET
 * @param           pMsgBuf         [out]   ָ����ջ�������ָ��
 * @param           ulMsgBufLen     [in]    ������Ϣ����
 * @param           ulTimeout       [in]    ��ʱʱ�䣬��λ�Ǻ���
 * @return          INT32
 * @retval          -1  stSock �Ƿ�����մ���
 * @retval          0   ulMsgBufLen Ϊ��ʱ����
 * @retval          int ���ؽ��յ����ݵĳ���
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

    /* ͬ�����õĳ�ʱʱ��ض�����0���첽���ò���Ҫrecv */
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
������          : gos_lock_init
����            : ������
�㷨ʵ��        : ��
����ȫ�ֱ���    : ��
�������˵��    : ��
����ֵ˵��      : �����´������ľ�������ΪNULL����ʾ����ʧ��
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
������          : gos_lock_free
����            : ɾ����
�㷨ʵ��        :
����ȫ�ֱ���    :
�������˵��    :
����ֵ˵��      :
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
������          : gos_lock
����            : ��������
�㷨ʵ��        :
����ȫ�ֱ���    :
�������˵��    :
����ֵ˵��      :
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
������          : gos_unlock
����            : ��������
�㷨ʵ��        :
����ȫ�ֱ���    :
�������˵��    :
����ֵ˵��      :
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
 * @brief           ִ�д�������������������ΪNULL���鱾���Ƿ��������д�����������������ڲ����ط���
 * @param           szCmd    [in]   ��Ҫִ�е�����
 * @return          VOID
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @par ʾ��:
 * @code
 * gos_system("pause"); ///< ִ����������ͣ����
 * @endcode
 * @note
 */
VOID gos_system(const CHAR *szCmd)
{
    INT32 iRet = system(szCmd);

    (void)iRet; // < ��ֹ linux �¾�����Ϊ����
}

/**
 * @brief           ִ��ϵͳ������������
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
    UINT32      ulSecondBetween1900And1970 = 0x83AA7E80; // 1900/1/1 �� 1970/1/1 ������
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
