#ifndef _GOS_TIME_H
#define _GOS_TIME_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _OSWIN32_
#else
#pragma pack(push)
#pragma pack(1)
typedef struct _SYSTEMTIME
{
    UINT16 wYear;
    UINT16 wMonth;
    UINT16 wDayOfWeek;
    UINT16 wDay;
    UINT16 wHour;
    UINT16 wMinute;
    UINT16 wSecond;
    UINT16 wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;
#pragma pack(pop)
#endif

typedef struct
{
    UINT32      ulTickHigh;
    UINT32      ulTickLow;
}GOS_TICK_T;

VOID     gos_time_init(VOID);
VOID     gos_time_get_time_us(SYSTEMTIME *ptTime, UINT32 *pulUs);
VOID     gos_time_get_tick(UINT64 *pui64Tick);
VOID     gos_time_get_time_from_tick(UINT64 *pui64Tick, SYSTEMTIME *ptTime, UINT32 *pulUs);
UINT32   gos_time_get_interval_ms(UINT64 ui64StartTick, UINT64 ui64EndTick);
UINT64   gos_time_get_interval_us(UINT64 ui64StartTick,UINT64 ui64EndTick);
UINT64   gos_time_get_interval_ns(UINT64 ui64StartTick,UINT64 ui64EndTick);
VOID     gos_time_get_local_time(VOID);
UINT64   gos_get_uptime_1ns(VOID);
UINT64   gos_get_cpu_freq(VOID);

#define MAX_TICK_NUM        32
typedef struct
{
    CHAR            acTickName[128];
    UINT32          ulPrintNum;

    UINT64          u64TestNum;
    UINT64          u64StartTick;
    UINT64          u64CurrTick;

    UINT64          u64TickTotal;

    UINT64          au64TestNum[MAX_TICK_NUM];
    UINT64          au64Tick[MAX_TICK_NUM];
}GOS_TICK_INFO_T;

#define GOS_TICK_INIT(t, print_num)         \
{                                           \
    memset(&t, 0, sizeof(t));               \
    t.ulPrintNum = print_num;               \
}

#define GOS_TICK_START(t)                   \
{                                           \
    t.u64TestNum++;                         \
    gos_time_get_tick(&t.u64StartTick);     \
}

#define GOS_TICK_GET(t, index)              \
if (index < MAX_TICK_NUM)                   \
{                                           \
    t.au64TestNum[index]++;                 \
    gos_time_get_tick(&t.u64CurrTick);      \
    t.au64Tick[index] += t.u64CurrTick - t.u64StartTick;\
}

#define GOS_TICK_END(t)                     \
{                                           \
    gos_time_get_tick(&t.u64CurrTick);      \
    t.u64TickTotal += t.u64CurrTick-t.u64StartTick;\
}

#define GOS_TICK_PRINT(t)       \
if (MOD(t.u64TestNum, t.ulPrintNum) == 0)\
{\
    UINT32 __i__;   \
    if (t.u64TickTotal > 0) \
        printf("\n %s: TestNum=%u TickCount=" FORMAT_U64 " Time=%f", t.acTickName, t.ulPrintNum, t.u64TickTotal, (INT64)gos_time_get_interval_ns(0, t.u64TickTotal)/(DOUBLE)(1000000));\
    for (__i__=0; __i__<MAX_TICK_NUM; __i__++)\
    {\
        if (t.au64TestNum[__i__])\
        {\
            printf("\n %s: Tick[%u]: Num=" FORMAT_U64 "(%.2f%%) Tick=" FORMAT_U64 "(%.2f%%) Tick/Num=%f", \
                    t.acTickName, __i__, \
                    t.au64TestNum[__i__], (DOUBLE)(INT64)t.au64TestNum[__i__]*100/(INT64)t.u64TestNum,\
                    t.au64Tick[__i__], (DOUBLE)(INT64)t.au64Tick[__i__]*100/(INT64)t.u64TickTotal,\
                    (DOUBLE)(INT64)t.au64Tick[__i__]/(INT64)t.au64TestNum[__i__]);\
        }\
    }\
}

#ifdef __cplusplus
}
#endif

#endif
