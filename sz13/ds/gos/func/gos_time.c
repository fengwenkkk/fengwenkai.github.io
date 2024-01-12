#include "time.h"
#include "gos_api.h"
#include "gos_time.h"

/* unsigned long    g_ulRegCpu=0; */
INT64           g_i64RawCpu;
unsigned int    g_uiTickPer100Ns=0;
UINT64          g_uliLargeIntStart;
static UINT64   g_ui64CpuSpeed = 0;

/* 20071218 */
DOUBLE          g_d64TickPerNs=0;
UINT64          g_ui64TickPerUs=0;
unsigned int    g_uiSecondsFirst=0;
unsigned int    g_uiUsOffset=0;

/* 20070917 */
SYSTEMTIME     g_tLastSysTime;
SYSTEMTIME     g_tNowSysTime;

unsigned int   g_uiLastUs=0;
extern char g_XCardRootDir[1024];

UINT64 gos_time_get_cpu_speed();
//#define UTIL_TIMER_USE_SYSTEM_TIME

#if 0
static inline UINT64 _gos_time_get_cycle_count()
{
    unsigned int timehi, timelo;

    // Use the assembly instruction rdtsc, which gets the current
    // cycle count (since the process started) and puts it in edx:eax.
#ifdef _OSWIN32_
    __asm
    {
        rdtsc
        mov timehi, edx;
        mov timelo, eax;
    }
#endif
    return ((UINT64)timehi << 32) + (UINT64)timelo;
}
#else
#define _gos_time_get_cycle_count(x)    \
{   \
    unsigned int __timehi, __timelo;    \
    __asm   \
    {   \
        rdtsc   \
        mov __timehi, edx;    \
        mov __timelo, eax;    \
    }   \
    x = ((UINT64)__timehi << 32) + (UINT64)__timelo;    \
}
#endif

static UINT64 m_startcycle;
static UINT64 m_overhead;

VOID _gos_time_start(VOID)
{
#ifdef _OSWIN32_
    unsigned int __timehi, __timelo;
    __asm
    {
        rdtsc
        mov __timehi, edx;
        mov __timelo, eax;
    }
    m_startcycle = ((UINT64)__timehi << 32) + (UINT64)__timelo;
#endif
}

UINT64 _gos_time_stop(VOID)
{
#ifdef _OSWIN32_
    UINT64  ullCount;

    __asm
    {
        rdtsc
        mov   DWORD   PTR   ullCount,   eax
        mov   DWORD   PTR   [ullCount+4],   edx
    }

    return ullCount-m_startcycle-m_overhead;
#else
    return 0;
#endif
}

VOID _gos_time_init(VOID)
{
    m_overhead = 0;
    _gos_time_start();
    m_overhead = _gos_time_stop();
}

#ifdef _OSWIN32_
VOID CalRawCpuFreq( VOID )
{
    INT64 raw_freq = 0;
    INT64 t0,t1;
    INT64 freq  =0;            // Most current frequ. calculation
    INT64 freq2 =0;            // 2nd most current frequ. calc.
    INT64 freq3 =0;            // 3rd most current frequ. calc.
    INT64 total;               // Sum of previous three frequency calculations
    INT64 tries=0;             // Number of times a calculation has

    INT64 total_cycles=0, cycles;// Clock cycles elapsed during test
    INT64 stamp0=0, stamp1=0;    // Time Stamp Variable for beginning and end of test
    INT64 total_ticks=0, ticks;  // Microseconds elapsed during test
    INT64 count_freq;            //
    unsigned int __timehi, __timelo;
    int iPriority;
    HANDLE hThread = GetCurrentThread();

    if( !QueryPerformanceFrequency( (PLARGE_INTEGER)&count_freq ) )
    {
        //FuraPrintf( "ERROR: The installed hardware does not support a high-resolution performance counter\n" );
        return;
    }

    //   This do loop runs up to 20 times or
    //   until the average of the previous
    //   three calculated frequencies is
    //   within 1 MHz of each of the
    //   individual calculated frequencies.
    //   This resampling increases the
    //   accuracy of the results since
    //   outside factors could affect this
    //   calculation
    do
    {
        tries++;          // Increment number of times sampled on this call to cpuspeed
        freq3 = freq2;    // Shift frequencies back to make room for new frequency measurement
        freq2 = freq;     //

        QueryPerformanceCounter( (PLARGE_INTEGER)&t0 );
        t1 = t0;

        iPriority = GetThreadPriority(hThread);
        if ( iPriority != THREAD_PRIORITY_ERROR_RETURN )
        {
            SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);
        }

        // Loop until 50 ticks have
        // passed    since last read of hi-
        // res counter. This accounts for
        // overhead later.
        while( t1-t0 < 50 )
        {
            QueryPerformanceCounter( (PLARGE_INTEGER)&t1 );
            //_gos_time_get_cycle_count(stamp0);

            __asm
            {
                rdtsc
                mov __timehi, edx;
                mov __timelo, eax;
            }
            stamp0 = ((UINT64)__timehi << 32) + (UINT64)__timelo;
        }

        t0 = t1;        // Reset Initial Time

        //   Loop until 1000 ticks have
        //   passed    since last read of hi-
        //   res counter. This allows for
        //   elapsed time for sampling.
        while( t1-t0 < 1000 )
        {
            QueryPerformanceCounter( (PLARGE_INTEGER)&t1 );
            //_gos_time_get_cycle_count(stamp1);
            __asm
            {
                rdtsc
                mov __timehi, edx;
                mov __timelo, eax;
            }
            stamp1 = ((UINT64)__timehi << 32) + (UINT64)__timelo;
        }

        // Reset priority
        if ( iPriority != THREAD_PRIORITY_ERROR_RETURN )
        {
            SetThreadPriority(hThread, iPriority);
        }

        // Number of internal
        //   clock cycles is
        //   difference between
        //   two time stamp
        //   readings.
        cycles = stamp1 - stamp0;

        // Number of external ticks is
        //   difference between two
        //   hi-res counter reads.
        ticks = t1 - t0;

        // Note that some seemingly arbitrary mulitplies and
        //   divides are done below. This is to maintain a
        //   high level of precision without truncating the
        //   most significant data. According to what value
        //   ITERATIIONS is set to, these multiplies and
        //   divides might need to be shifted for optimal
        //   precision.

        // Convert ticks to hundred
        //   thousandths of a tick
        ticks = ticks * 100000;

        // Hundred Thousandths of a
        //   Ticks / ( 10 ticks/second )
        //   = microseconds (us)
        ticks = (ticks*10) /count_freq;

        total_ticks += ticks;
        total_cycles += cycles;

        if( ticks%count_freq > count_freq/2 )
        {
            // Round up if necessary
            ticks++;
        }

        // Cycles / us  = MHz
        freq = cycles/ticks;

        // Round up if necessary
        if( cycles%ticks > ticks/2 )
        {
            freq++;
        }

        // Total last three frequency
        //   calculations
        total = ( freq + freq2 + freq3 );
    } while ( (tries < 3 ) ||
               (tries < 20)&&
              ((abs((int)(3 * freq -total)) > 3 )||
               (abs((int)(3 * freq2-total)) > 3 )||
               (abs((int)(3 * freq3-total)) > 3 )));
                    // Compare last three calculations to
                    //   average of last three calculations.

    // Try one more significant digit.
    freq3 = ( total_cycles * 10 ) / total_ticks;
    freq2 = ( total_cycles * 100 ) / total_ticks;

    if ( freq2 - (freq3 * 10) >= 6 )
        freq3++;

    raw_freq = total_cycles / total_ticks;

    g_i64RawCpu = raw_freq;
}

UINT64 gos_time_get_cpu_speed()
{
    UINT64   start,   stop;
    UINT64   nCtr,   nFreq,   nCtrStop;

    QueryPerformanceFrequency((LARGE_INTEGER   *)&nFreq);

    __asm
    {
        rdtsc
        mov   DWORD   PTR   start,   eax
        mov   DWORD   PTR   [start+4],   edx
    }

    QueryPerformanceCounter((LARGE_INTEGER   *)&nCtrStop);

    nCtrStop   +=   nFreq;
    do
    {
        QueryPerformanceCounter((LARGE_INTEGER   *)&nCtr);
    }while   (nCtr   <   nCtrStop);

    __asm
    {
        rdtsc
        mov   DWORD   PTR   stop,   eax
        mov   DWORD   PTR   [stop+4],   edx
    }

    return   (stop-start);
}

VOID gos_time_get_local_time(VOID)
{
#ifdef UTIL_TIMER_USE_SYSTEM_TIME
    ::GetLocalTime(&g_tNowSysTime);
#endif
}
#endif      //_OSWIN32_

#ifndef _OSWIN32_
#define RESULT_FILE     "/root/cpuinfo_freq"
UINT64 _GetCpuFreq(VOID)
{
#ifndef __mips__
    char buf[512];
    int ret = 0;
    int fd;
    int sz;
    float fFreq = 0;
    char *szFreq;

    ret = gos_system("cat /proc/cpuinfo | grep GHz >> " RESULT_FILE);
    fd = open(RESULT_FILE, O_RDONLY);

    memset(buf, 0, sizeof (buf));
    // model name      : Intel(R) Xeon(R) CPU           X3450  @ 2.67GHz
    sz = read(fd, buf, sizeof (buf) - 1);
    close(fd);
    remove(RESULT_FILE);

    if (sz > 0)
    {
        szFreq = strchr(buf, '@');
        if (szFreq)
        {
            sscanf(szFreq+1, "%fGHz", &fFreq);
            return (UINT64)(fFreq*1000000000);
        }
    }

    ret = gos_system("cat /proc/cpuinfo | grep MHz >> " RESULT_FILE);
    fd = open(RESULT_FILE, O_RDONLY);

    memset(buf, 0, sizeof (buf));
    sz = read(fd, buf, sizeof (buf) - 1);
    close(fd);
    remove(RESULT_FILE);

    if (sz > 0)
    {
        szFreq = strchr(buf, ':');
        if (szFreq)
        {
            sscanf(szFreq+1, "%f", &fFreq);
            return (UINT64)(fFreq*1000000);
        }
    }

    return (UINT64)2000*1000000; // ȱʡΪ2G
#else

    UINT64      u64StartTick;
    UINT64      u64EndTick;

    gos_time_get_tick(&u64StartTick);
    sleep(1);
    gos_time_get_tick(&u64EndTick);

    return u64EndTick - u64StartTick;
#endif
}

#endif

VOID gos_time_init(VOID)
{
    static BOOL bInit = FALSE;

#ifdef UTIL_TIMER_USE_SYSTEM_TIME
    FILETIME       sFt;// to 100ns
    SYSTEMTIME     tCurTime;
#endif
    time_t tTime;

    if (bInit)
    {
        return;
    }

    _gos_time_init();

#ifdef _OSWIN32_
    g_ui64CpuSpeed = gos_time_get_cpu_speed();
#else
    g_ui64CpuSpeed = _GetCpuFreq();
#endif

    g_i64RawCpu=g_ui64CpuSpeed/(1000*1000);

#ifdef UTIL_TIMER_USE_SYSTEM_TIME
    ::GetLocalTime(&g_tNowSysTime);
    memset(&tCurTime,0,sizeof(tCurTime));
    ::GetLocalTime(&tCurTime);
    if (FALSE == SystemTimeToFileTime(&tCurTime,&sFt))
    {
        // Do nothing
    }
    memcpy(&g_uliLargeIntStart,&sFt,sizeof(g_uliLargeIntStart));

    /* 20070917 */
    memcpy(&g_tLastSysTime,&tCurTime,sizeof(g_tLastSysTime));
    g_uiLastUs=0;
#endif

    g_ui64TickPerUs  = g_i64RawCpu;
    g_d64TickPerNs   = (DOUBLE)(INT64)g_ui64CpuSpeed/(1000*1000*1000);
    g_uiTickPer100Ns = (unsigned int)(g_i64RawCpu/10);

    _gos_time_start();
    time( &tTime);
    g_uiSecondsFirst=(long)tTime;
    if (g_ui64TickPerUs > 0)
    {
        g_uiUsOffset=((unsigned int)((m_overhead+m_startcycle)/g_ui64TickPerUs))%1000000;
    }

    bInit = TRUE;

    return;
}

/* Get hour minute second us */
VOID gos_time_get_time_us(SYSTEMTIME *ptTime, UINT32 *pulUs)
{
#ifdef UTIL_TIMER_USE_SYSTEM_TIME
    UINT64 ui64NowTickInterval;

    if ((NULL == ptTime)||
        (NULL == pulUs))
    {
        return;
    }

    /* Mod at 20070917 */
    SYSTEMTIME     tCurTime;
    unsigned int   uiUsLeft;
    static unsigned int   uiTmpCount=0;
    static unsigned int   uiOffset=0;

    uiTmpCount++;

    ui64NowTickInterval = g_cTimer.Stop();

    uiUsLeft = (unsigned int)((ui64NowTickInterval/g_uiTickPer100Ns)/10)%1000000;

    *pulUs=uiUsLeft;

    memcpy(&tCurTime,&g_tNowSysTime,sizeof(tCurTime));

    tCurTime.wMilliseconds=g_tLastSysTime.wMilliseconds;

    if (0==memcmp(&tCurTime,&g_tLastSysTime,sizeof(g_tLastSysTime)))
    {
        if(*pulUs>g_uiLastUs)
        {
            if (*pulUs>(g_uiLastUs+uiOffset))
            {
                *pulUs=*pulUs-uiOffset;
            }
            g_uiLastUs=*pulUs;
        }
        /* <= */
        else
        {
            if (1000000 > (g_uiLastUs+5))
            {
                g_uiLastUs=g_uiLastUs+5-(uiTmpCount%3);
            }
            *pulUs=g_uiLastUs;
        }
    }
    else
    {
        memcpy(&g_tLastSysTime,&tCurTime,sizeof(g_tLastSysTime));
        if (*pulUs>500000)
        {
            *pulUs=*pulUs-500000;
            uiOffset=500000;
        }
        else if (*pulUs>300000)
        {
            *pulUs=*pulUs-300000;
            uiOffset=300000;
        }
        else
        {
            uiOffset=0;
        }
        g_uiLastUs=*pulUs;
    }
    memcpy(ptTime,&g_tLastSysTime,sizeof(g_tLastSysTime));
#endif

    return;
}

VOID gos_time_get_tick(UINT64 *pui64Tick)
{
#ifdef _OSWIN32_
    *pui64Tick = _gos_time_stop();
#else
    __asm__ __volatile__("rdtsc" : "=A" (*pui64Tick));
#endif
}

UINT32 gos_time_get_interval_ms(UINT64 ui64StartTick,UINT64 ui64EndTick)
{
    UINT64  ui64IntervalTicks;

    ui64IntervalTicks = ui64EndTick - ui64StartTick;

    return (UINT32)((ui64IntervalTicks/g_ui64TickPerUs)/1000);
}

UINT64 gos_time_get_interval_us(UINT64 ui64StartTick,UINT64 ui64EndTick)
{
    UINT64  ui64IntervalTicks;

    ui64IntervalTicks = ui64EndTick - ui64StartTick;

    return (UINT64)(ui64IntervalTicks/g_ui64TickPerUs);
}

UINT64 gos_time_get_interval_ns(UINT64 ui64StartTick,UINT64 ui64EndTick)
{
    UINT64  ui64IntervalTicks;

    ui64IntervalTicks = ui64EndTick - ui64StartTick;

    return (UINT64)((INT64)ui64IntervalTicks/g_d64TickPerNs);
}

VOID gos_time_get_time_from_tick(UINT64 *pui64Tick, SYSTEMTIME *ptTime, UINT32 *pulUs)
{
#ifdef UTIL_TIMER_USE_SYSTEM_TIME
    FILETIME         sFt;// to 100ns
    unsigned int     uiUsLeft;
    UINT64 ui64TimeInterval100Ns;
    ULARGE_INTEGER   uliLargeIntNow;

    if ((NULL == ptTime)||
        (NULL == pulUs)||
        (NULL == pui64Tick))
    {
        return;
    }

    ui64TimeInterval100Ns   = *pui64Tick/g_uiTickPer100Ns;
    uliLargeIntNow.QuadPart = g_uliLargeIntStart.QuadPart + ui64TimeInterval100Ns;

    memcpy(&sFt,&uliLargeIntNow,sizeof(sFt));

    uiUsLeft = (unsigned int)(ui64TimeInterval100Ns/10)%1000;

    if (FALSE == FileTimeToSystemTime(&sFt,ptTime))
    {
        // Do nothing
    }

    *pulUs  = ptTime->wMilliseconds*1000+uiUsLeft;
#endif

    return;
}

VOID gos_time_get_timeval(TIMEVAL *pTime)
{
    UINT64 ui64TimeIntervalUs = 0; // us
    UINT64 ui64NowTickInterval;

    if (NULL==pTime)
    {
        return;
    }

    ui64NowTickInterval = _gos_time_stop();
    if (g_ui64TickPerUs > 0)
    {
        ui64TimeIntervalUs = ui64NowTickInterval/g_ui64TickPerUs;
    }
    pTime->tv_sec =(long)g_uiSecondsFirst + (long)(ui64TimeIntervalUs/1000000);
    pTime->tv_usec=(long)(ui64TimeIntervalUs%1000000);

    return;
}

UINT64 gos_get_cpu_freq(VOID)
{
    return g_ui64CpuSpeed;
}

UINT64 gos_get_uptime_1ns(VOID)
{
#ifdef _OSWIN32_
    return (UINT64)((INT64)_gos_time_stop()/g_d64TickPerNs);
#else
    UINT64 u64Tick;

    gos_time_get_tick(&u64Tick);
    return u64Tick/g_d64TickPerNs;
#endif
}

INT64 gos_time_get_diff_tick(UINT32 ulTimeStartHigh, UINT32 ulTimeStartLow,
                             UINT32 ulTimeEndHigh, UINT32 ulTimeEndLow)
{
    return (((INT64)(ulTimeEndHigh-ulTimeStartHigh)) << 32) + ulTimeEndLow-ulTimeStartLow;
}

#ifdef GOS_TICK_TEST
static CHAR     g_acText[32];
static CHAR     g_aucMem1[32];
static CHAR     g_aucMem2[32];
static HANDLE   g_hMutex = NULL;

static GOS_TICK_INFO_T      g_stTickInfo = {0};

VOID tick_test_sprintf(UINT32 ulNum)
{
    sprintf(g_acText, "%30u", ulNum);
}

VOID tick_test_memset(VOID)
{
    memset(g_aucMem1, 0, sizeof(g_aucMem1));
}

VOID tick_test_memcpy(VOID)
{
    memcpy(g_aucMem1, g_aucMem2, sizeof(g_aucMem1));
}

VOID tick_test_mutex(VOID)
{
    gos_mutex(g_hMutex);
    gos_unmutex(g_hMutex);
}

VOID tick_test_main(UINT32 ulNum)
{
    tick_test_mutex();

    GOS_TICK_GET(g_stTickInfo, 3);

    if (MOD(ulNum, 2) == 0)
    {
        tick_test_memcpy();

        GOS_TICK_GET(g_stTickInfo, 0);
        return;
    }

    if (MOD(ulNum, 3) == 0)
    {
        tick_test_memset();

        GOS_TICK_GET(g_stTickInfo, 1);
        return;
    }

    tick_test_sprintf(ulNum);
    GOS_TICK_GET(g_stTickInfo, 2);

    return;
}

INT main(VOID)
{
    UINT32  i, j;
    UINT32  ulTestLoop = 1;
    UINT32  ulTestNum = 1000*1000;

    gos_time_init();

    g_hMutex = gos_mutex_init();

    for (j=0; j<ulTestLoop; j++)
    {
        GOS_TICK_INIT(g_stTickInfo, ulTestNum);
        for (i=0; i<ulTestNum; i++)
        {
            GOS_TICK_START(g_stTickInfo);

            tick_test_main(i);

            GOS_TICK_END(g_stTickInfo);
        }

        GOS_TICK_PRINT(g_stTickInfo);
    }

    return 0;
}
#endif

