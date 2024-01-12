/******************************************************************************
模块名      : public
文件名      : gos_api.h
相关文件    :
实现功能    : GOS模块的接口
修改记录    :
------------------------------------------------------------------------------
日期        修改人      修改内容
2010/05/08  雷杰        创建
******************************************************************************/
#ifndef _GOS_TYPEDEF_H_
#define _GOS_TYPEDEF_H_

#if defined(_MSC_VER) && _MSC_VER >= 1500
#ifndef _VS2008_
#define _VS2008_
#endif
#endif

#ifdef _VS2008_
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

#endif

/* 32位操作系统 */
#undef  GOS_64BYTE_OS
#define GOS_32BYTE_OS

/* 工程类型预编译宏的合法性 */
#undef _OSLINUX_
#undef _OSWIN32_

#ifdef __BORLANDC__

#ifndef WIN32
#define WIN32
#endif

#define _BCB_

#if __BORLANDC__ >= 0x0620    // 2010
#define _UNICODE_
#endif

#define DISABLE_EMPTY_ARRAY

#endif

/* 底层 OS 宏开关 */
#if defined(WIN32) || defined(_WIN32)
    #define _OSWIN32_
#else
    #define _OSLINUX_
#endif

#ifdef _OSWIN32_
#define GOS_USE_SEM
#define GOS_USE_PTHREAD
#endif

/* 缺省都是用小端 */
#ifdef _OSWIN32_
#ifndef GOS_BIG_ENDIAN
#define GOS_LITTLE_ENDIAN
#endif
#else
#ifndef GOS_BIG_ENDIAN
#define GOS_LITTLE_ENDIAN
#endif
#endif

/* 根据不同平台包含不同的系统头文件 */
#ifdef _OSWIN32_
    #pragma warning (disable : 4100 4083 4018 4115 4127 4200 4214 4244 4505 4512 4663 4706 4710 4786 4996) /* rpcasync.h(45) : warning C4115: '_RPC_ASYNC_STATE' : named type definition in parentheses */

#ifdef _VS2008_
    #include <Winsock2.h>
#endif
    #include <windows.h>
    #include <stdio.h>
    #include <string.h>
    #include <fcntl.h>
    #include <stdarg.h>
    #include <stdlib.h>
    #include <sys/stat.h>
    #include <io.h>
    #include <time.h>
    #include <errno.h>
    #include <direct.h>
    #include <winbase.h>
    #include <math.h>
    #include <stdint.h>

    #pragma warning (default : 4115) /* 恢复警告 4115 */

    /*
     * 由于 VC 4 级告警太多, 定制几个告警级别:
     * 1) GOS_WARNING_LEVEL = 35: 编译警告级别 3.5, 关闭大量 4 级编译警告
     * 2) GOS_WARNING_LEVEL = 36: 编译警告级别 3.6, 关闭少量 4 级编译警告
     * 3) GOS_WARNING_LEVEL = 40: 编译警告级别 4.0, 打开所有 4 级编译警告
     */
    #define GOS_WARNING_LEVEL 40

    /* 定制编译警告级别, 必须放在 #include "*.h" 之后才生效 */

    #pragma warning (error: 4700 4701)   /* 把C4700警告设置为错误: local variable used without having been initialized */

    #if GOS_WARNING_LEVEL == 35 /* 告警级别为 3.5 级 */
        #pragma warning(disable : 4100) /* unreferenced formal parameter */
        #pragma warning(disable : 4102)
        #pragma warning(disable : 4127) /* conditional expression is constant */
        #pragma warning(disable : 4201) /* unreferenced label */
        #pragma warning(disable : 4206) /* nonstandard extension used : translation unit is empty */
        #pragma warning(disable : 4214) /* nonstandard extension used : bit field types other than int */
        #pragma warning(disable : 4505)
        #pragma warning(disable : 4996) /* To disable deprecation */
    #elif GOS_WARNING_LEVEL == 36 /* 告警级别为 3.6 级 */
        #pragma warning(disable : 4102 4206 4214)
    #elif GOS_WARNING_LEVEL == 40 /* 告警级别为 4.0 级 */
    #else /* 无效级别 */
        #pragma error Invalid GOS_WARNING_LEVEL
    #endif

#endif

#ifdef _OSLINUX_
    #include <time.h>
    #include <stdio.h>
    #include <string.h>
    #include <errno.h>
    #include <pthread.h>
    #include <signal.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <linux/udp.h>
    #include <sys/select.h>
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <stdlib.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <stdarg.h>
    #include <semaphore.h>
    #include <sys/sysinfo.h>
    #include <sys/times.h>
    #include <sys/ipc.h>
    #include <sys/msg.h>
    #include <sys/time.h>
    #include <dirent.h>
    #include <ctype.h>
    #include <termios.h>
    #include <math.h>
#endif /* _OSLINUX_ */

/*以下类型和宏在LINUX下未定义*/
#ifdef _OSLINUX_

    #ifndef INT
    #define INT int
    #endif

    #ifndef CHAR
    #define CHAR char
    #endif

    #ifndef SHORT
    #define SHORT short
    #endif

    #ifndef LONG
    #define LONG long
    #endif

    #ifndef BYTE
    #define BYTE unsigned char
    #endif

    #ifndef HANDLE
    #define HANDLE void *
    #endif

    #ifndef FLOAT
    #define FLOAT float
    #endif

    #ifndef DOUBLE
    #define DOUBLE double
    #endif

    #ifndef UINT8
    #define UINT8 unsigned char
    #endif

    #ifndef UINT16
    #define UINT16 unsigned short
    #endif

    #ifndef UINT32
    #define UINT32 unsigned int
    #endif

    #ifndef UINT64
    #define UINT64 unsigned long long
    #endif

    #ifndef INT8
    #define INT8 char
    #endif

    #ifndef INT16
    #define INT16 short
    #endif

    #ifndef INT32
    #define INT32 int
    #endif

    #ifndef INT64
    #define INT64 long long
    #endif

    #ifndef BOOL
    typedef int BOOL;
    #endif

    #ifndef VOID
    #define VOID void
    #endif

    #ifndef SOCKET
    #define SOCKET unsigned int
    #endif

    #define _mkdir    mkdir
    #define WINAPI
    #define DIR_SEPARATOR '/'

    typedef fd_set  FD_SET;
#endif /* _OSLINUX_ */

/*以下类型和宏在WINDOWS下未定义*/
#ifdef _OSWIN32_

    #ifndef CHAR
    #define CHAR char
    #endif

    typedef double DOUBLE;

    #ifndef INT64
    #define INT64 __int64
    #endif

    #ifndef UINT64
    #define UINT64 DWORD64
    #endif

    #ifndef UINT8
    #define UINT8 unsigned char
    #endif

    #ifndef UINT16
    #define UINT16 unsigned short
    #endif

    #ifndef UINT32
    #define UINT32 unsigned int
    #endif

    #ifndef INT8
    #define INT8 char
    #endif

    #ifndef INT16
    #define INT16 short
    #endif

    #ifndef INT32
    #define INT32 int
    #endif

    typedef int BOOL;

    #ifndef VOID
    #define VOID void
    #endif

    #ifndef SOCKET
    #define SOCKET unsigned int
    #endif

    typedef struct tm       LOCALTIME;
    #define snprintf        gos_snprintf  // _snprintf
    #define vsnprintf       _vsnprintf
    #define strcasecmp      stricmp

    #define DIR_SEPARATOR    '\\'
#endif /*_OSWIN32_ */

#ifdef TRUE
#undef TRUE
#define TRUE 1
#else
#define TRUE 1
#endif

#ifdef FALSE
#undef FALSE
#define FALSE 0
#else
#define FALSE 0
#endif

#ifdef _OSLINUX_
    #ifndef SH_COMPAT
    #define SH_COMPAT   0x0000
    #endif

    #ifndef SH_DENYRW
    #define SH_DENYRW   0x0010
    #endif

    #ifndef SH_DENYWR
    #define SH_DENYWR   0x0020
    #endif

    #ifndef SH_DENYRD
    #define SH_DENYRD   0x0030
    #endif

    #ifndef SH_DENYNONE
    #define SH_DENYNONE 0x0040
    #endif
#endif

/********************************************************************/
/* 类型定义 */

#define GOS_MAC_FMT     "%02X:%02X:%02X:%02X:%02X:%02X"
#define GOS_MAC_ARG(x)  ((UINT8*)(x))[0], ((UINT8*)(x))[1], ((UINT8*)(x))[2], \
                    ((UINT8*)(x))[3], ((UINT8*)(x))[4], ((UINT8*)(x))[5]

#define IP_FMT      "%u.%u.%u.%u"
#define IP_ARG(x)   ((UINT8*)(x))[0], ((UINT8*)(x))[1], ((UINT8*)(x))[2], ((UINT8*)(x))[3]

#define MASK_FMT    IP_FMT
#define MASK_ARG    IP_ARG

#define TIME_FMT    "%04d-%02d-%02d %02d:%02d:%02d"
#define TIME_ARG(t) (1900+(t)->tm_year), (1+(t)->tm_mon), (t)->tm_mday, (t)->tm_hour, (t)->tm_min, (t)->tm_sec

#ifndef SET_IP
#define SET_IP(ip,ip1,ip2,ip3,ip4)      \
{                                       \
    ((UINT8*)(ip))[0] = (UINT8)(ip1);   \
    ((UINT8*)(ip))[1] = (UINT8)(ip2);   \
    ((UINT8*)(ip))[2] = (UINT8)(ip3);   \
    ((UINT8*)(ip))[3] = (UINT8)(ip4);   \
}
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)   sizeof(x)/sizeof((x)[0])
#endif

#ifndef STRUCT_OFFSET
#define STRUCT_OFFSET(stru, member)     (UINT64)(&(((stru*)0)->member))
#endif

#define FIELD_LEN(stru, member)         sizeof(((stru*)0)->member)

#define GET_INT(ptr)            (*((UINT32*)(ptr)))
#define SET_INT(ptr, value)     (*((UINT32*)(ptr))) = (UINT32)(value)

#define GET_INT64(ptr)          (*((INT64*)(ptr)))
#define SET_INT64(ptr, value)   (*((INT64*)(ptr))) = (INT64)(value)

#define GET_UINT64(ptr)         (*((UINT64*)(ptr)))
#define SET_UINT64(ptr, value)  (*((UINT64*)(ptr))) = (UINT64)(value)

#define GET_SHORT(ptr)          (*((UINT16*)(ptr)))
#define SET_SHORT(ptr, value)   (*((UINT16*)(ptr))) = (UINT16)(value)

/* 字节序转换 */
#ifdef GOS_LITTLE_ENDIAN

#undef  HTONL
#define HTONL(a)        htonl(a)
#define HTONL_IT(a)     ((a) = HTONL((a)))

#undef  NTOHL
#define NTOHL(a)        ntohl(a)
#define NTOHL_IT(a)     ((a) = NTOHL((a)))

#undef  HTONS
#define HTONS(a)        htons(a)
#define HTONS_IT(a)     ((a) = HTONS((a)))

#undef  NTOHS
#define NTOHS(a)        ntohs(a)
#define NTOHS_IT(a)     ((a) = NTOHS((a)))

#undef  HTONLL
#define HTONLL(a)      ((((a) & (UINT64)0xff00000000000000LL) >> 56) |\
                        (((a) & (UINT64)0x00ff000000000000LL) >> 40) |\
                        (((a) & (UINT64)0x0000ff0000000000LL) >> 24) |\
                        (((a) & (UINT64)0x000000ff00000000LL) >> 8)  |\
                        (((a) & (UINT64)0x00000000ff000000LL) << 8)  |\
                        (((a) & (UINT64)0x0000000000ff0000LL) << 24) |\
                        (((a) & (UINT64)0x000000000000ff00LL) << 40) |\
                        (((a) & (UINT64)0x00000000000000ffLL) << 56) )
#define HTONLL_IT(a)    ((a) = HTONLL((a)))

#undef  NTOHLL
#define NTOHLL          HTONLL
#define NTOHLL_IT(a)    ((a) = NTOHLL((a)))

#else // #ifndef GOS_LITTLE_ENDIAN
#undef  HTONL
#define HTONL(a)        (a)
#define HTONL_IT(a)

#undef  HTONS
#define HTONS(a)        (a)
#define HTONS_IT(a)

#undef  NTOHL
#define NTOHL(a)        (a)
#define NTOHL_IT(a)

#undef  NTOHS
#define NTOHS(a)        (a)
#define NTOHS_IT(a)

#undef  HTONLL
#define HTONLL(a)       (a)
#define HTONLL_IT(a)

#undef  NTOHLL
#define NTOHLL(a)       (a)
#define NTOHLL_IT(a)
#endif  // #ifdef GOS_LITTLE_ENDIAN

#define GOS_TOL(bToNet, ulData)     \
{                                   \
    if (bToNet)                     \
        HTONL_IT(ulData);           \
    else                            \
        NTOHL_IT(ulData);           \
}

#define GOS_TOS(bToNet, usData)     \
{                                   \
    if (bToNet)                     \
        HTONS_IT(usData);           \
    else                            \
        NTOHS_IT(usData);           \
}

#define MOD(a, b)   ((a) % (b))

#ifndef MIN
#define MIN(a,b)    ((a)>(b)?(b):(a))
#endif

#ifndef MAX
#define MAX(a,b)    ((a)>(b)?(a):(b))
#endif

#ifndef ABS
#define ABS(x)      ((x)>0?(x):(-(x)))
#endif

#ifdef _OSWIN32_
#define GOS_THREAD_RET unsigned WINAPI
#else
#define GOS_THREAD_RET void *
#endif

#ifdef GOS_DEBUG
#define PRIVATE
#else
#define PRIVATE    static
#endif

#ifndef WAIT_FOREVER
#define WAIT_FOREVER    0xFFFFFFFF  /* Infinite timeout */
#endif

#ifndef NO_WAIT
#define NO_WAIT         (UINT32)0   /* don't wait */
#endif

#define OSTOKEN         UINT32
#define FHANDLE         INT

#define INVALID_MUTEX   NULL
#define INVALID_LOCK    NULL
#define INVALID_SEM     NULL

#define MAX_TASK_DATA_LEN   1024

typedef GOS_THREAD_RET THREAD_ENTRY(void *pData);

#define CLOSE_SOCKET(s)     \
{                           \
    if ((s)!=INVALID_SOCKET)\
    {                       \
        closesocket(s);     \
        (s)=INVALID_SOCKET; \
    }                       \
}

#define INVALID_THREAD_TOKEN    0

#ifdef _OSWIN32_
typedef UINT32 THREAD_TOKEN;
#else
typedef pthread_t THREAD_TOKEN;
#endif

/* 函数返回类型 */
typedef UINT32          GRET;

/* 返回值定义 */
#define G_OK            ((GRET)0)             /* 成功 */
#define G_FAIL          ((GRET)0xFFFFFFFF)    /* 失败 */

typedef struct
{
    UINT16              usYear;     /* 四位数 */
    UINT8               ucMonth;    /* 1-12 */
    UINT8               ucDay;      /* 1-31 */

    UINT8               ucHour;     /* 0-23 */
    UINT8               ucMinute;   /* 0-59 */
    UINT8               ucSecond;   /* 0-59 */
    UINT8               ucSn;
}GOS_DATETIME_T;

typedef struct
{
    UINT16              usYear;     /* 四位数  */
    UINT8               ucMonth;    /* 1-12 */
    UINT8               ucDay;      /* 1-31 */
}GOS_DATE_T;

typedef struct
{
    UINT8               ucHour;     /* 0-23 */
    UINT8               ucMinute;   /* 0-59 */
    UINT8               ucSecond;   /* 0-59 */
    UINT8               ucSn;
}GOS_TIME_T;

#ifdef _OSWIN32_
#define FORMAT_U64          "%I64u"
#define FORMAT_I64          "%I64d"
#define FORMAT_H64          "%I64X"
#else
#define FORMAT_U64          "%llu"
#define FORMAT_I64          "%lld"
#define FORMAT_H64          "%llx"
#endif

#define FMT_I64         FORMAT_I64

#define IS_NULL_STRING(x)   (!(x) || *((CHAR*)(x)) == '\0')

#define DOUBLE_MAX          1.7976931348623158e+308 /* max value */
#define DOUBLE_MIN          (DOUBLE)(-DOUBLE_MAX)   /* 2.2250738585072014e-308  min positive value */

#define FLOAT_MAX           3.402823466e+38F        /* max value */
#define FLOAT_MIN           (FLOAT)(-FLOAT_MAX)     /* 1.175494351e-38F */

#ifdef _OSWIN32_
#ifndef INT32_MAX
#define INT32_MAX           2147483647i32 /* maximum signed 32 bit value */
#define INT32_MIN           (-2147483647i32 - 1) /* minimum signed 32 bit value */
#endif

#ifndef INT64_MAX
#define INT64_MAX           9223372036854775807i64
#define INT64_MIN           (-9223372036854775807i64 - 1)
#endif
#else
#ifndef INT32_MAX
#define INT32_MAX           0x7fffffff
#define INT32_MIN           (-0x80000000L)
#endif

#ifndef INT64_MAX
#define INT64_MAX           0x7fffffffffffffffLL
#define INT64_MIN           (-0x8000000000000000LL)
#endif
#endif

#define FIND_MAP_KEY(map, key)      (map.find(key) != map.end())
#define VECTOR              std::vector
#define MAP                 std::map


#define gos_isspace(x)      (isascii(x) && isspace(x))

#pragma pack(push, 1)

typedef struct
{
    UINT8               aucIP[4];               /* IP  */
    UINT16              usPort;                 /* 端口 */
}IP_ADDR_T;

typedef struct
{
    UINT32              ulID;
    CHAR                *szValue;
}ID_VALUE_T;

typedef struct
{
    UINT8   aucIP[4];
    INT32   iDelay;
}PING_T;

#pragma pack(pop)

#endif /* _GOS_TYPEDEF_H_ */
/******************************* 头文件结束 **********************************/


