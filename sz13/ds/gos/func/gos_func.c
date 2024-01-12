/******************************************************************************
* 功    能: 公共函数实现
* 修改历史:
******************************************************************************/

/******************************* 包含文件声明 ********************************/
#include "gos_api.h"
#include "gos_func.h"
#include "gos_file.h"
#include "assert.h"
#include <stdint.h>

#ifndef _OSWIN32_
#include <sys/statvfs.h>
#include <net/if.h>
#include <libgen.h>
#else
#endif

/******************************* 局部宏定义 **********************************/

/******************************* 全局变量定义/初始化 *************************/

/******************************* 局部常数和类型定义 **************************/
typedef struct
{
    HANDLE      hFile;
    CHAR        acDir[512];
}_GOS_DIR_HANDLE_T;

/******************************* 局部函数原型声明 ****************************/

/******************************* 函数实现 ************************************/
/*
* @name               : gos_snprintf
* @function           : 对字符串进行操作，把szFormat的内容写进szDst中（写入iBufLen-1个字符）
* @class              : NONE
* @param              : CHAR *szDst,                    [out]   // 写入对象（出参）
                        const INT32 iBufLen,            [in]    // 入参长度（入参）
                                                                // 重要！iBufLen不能为4!
                        const CHAR * const szFormat,    [in]    // 格式化字符串
                        ...                             [in]    // 变参列表
* @globalparam        : NONE
* @classparam         : NONE
* @database           : NONE
* @return             : 失败返回零，成功返回写入szDst的字符串的长度（不包含\0）
* @author             : Leijie
* @createtime         : 2019-03-29 15:29:50
* @note               : 1. 设计思路：为了让该函数可以在一个重叠区域进行操作,
                                     把格式化字符串(szFormat)拷贝出一个备份szTempFormat，
                                     再用szTempFormat对szDst进行赋值操作。
                        2. 在对szTempFormat、szDst进行操作前都对其进行置零操作。
                        3. 注意事项：1) szDst、szFormat不可为空指针
                                     2) 0 < iBufLen < RSIZE_MAX
                                     3) 重要！不能iBufLen不能为sizeof(CHAR), 以避免有人传入错误的长度
* @example :
                  CHAR acString[1024] = {0};
            以前：       sprintf(acString, "It's a example! ");
            现在：gos_snprintf(acString, sizeof(acString), "It's a example! ");
*/
#ifdef _OSWIN32_
INT32 gos_snprintf(CHAR *szDst, UINT32 ulBufLen, const CHAR *szFormat, ...)
{
    va_list         vaLog;
    INT32           iLen = 0;

    /* 判断入参的有效性 */
    if (!szDst || !szFormat || ulBufLen == 0)
    {
        return 0;
    }

    memset(szDst, 0, ulBufLen);

    /* 利用格式化字符串对临时变量(szTempFormat)进行拷贝操作 */
    va_start(vaLog, szFormat);
    iLen = vsnprintf(szDst, ulBufLen-1, szFormat, vaLog);
    va_end(vaLog);

    /* 利用临时变量对szDst进行写入操作 */
    if (iLen <= 0)
    {
        iLen = strlen(szDst);
    }

    return iLen;
}
#endif

/**
 * @brief           判断CPU的大小端
 * @return          BOOL 返回TRUE->小端   返回FALSE->大端
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *  std::cout << "本地为: " << gos_estimate_cpu_endian() ? "大端" : "小端";
 * @endcode
 * @note
  */
BOOL gos_estimate_cpu_endian()
{
    UINT32  ulValue = 1;
    UINT8   *pucValue = (UINT8*)&ulValue;

    if (*pucValue == 1)
    {
        return TRUE;
    }

    return FALSE;
}

/*************************************************************************
* 函数名称: get_current_time
* 功    能: 获取从1970年0点整开始到现在的秒数
* 参    数:
* 参数名称          输入/输出               描述
* 函数返回: 秒数
* 说    明: 内部调用time()函数
*************************************************************************/
UINT32 gos_get_current_time( VOID )
{
    UINT32 ulCurrTime = (UINT32)time(NULL);

    return ulCurrTime;
}

/*************************************************************************
* 函数名称: gos_get_text_time
* 功    能: 获取从1970年0点整开始的秒数
* 参    数:
* 参数名称          输入/输出                       描述
  pulCurrTime       IN                      时间，如果为NULL，则为当前时间
* 函数返回:
* 说    明: YYYY-MM-DD hh24:mm:ss格式的字符串
struct tm
{
    INT32 tm_sec;     seconds after the minute: 0-61
    INT32 tm_min;     minutes after the hour: 0-59
    INT32 tm_hour;    hours since midnight: 0-23
    INT32 tm_mday;    day of the month: 1-31
    INT32 tm_mon;     months since January: 0-11
    INT32 tm_year;    years since 1900
    INT32 tm_wday;    days since Sunday: 0-6
    INT32 tm_yday;    days since January 1: 0-365
    INT32 tm_isdst;   Daylight Saving Time flag
};
*************************************************************************/
VOID gos_get_text_time_ex(UINT32 *pulTime, const CHAR *szFormat, CHAR *szTime)
{
    GOS_DATETIME_T      stLocalTime = {0};
    UINT32              ulTime = gos_get_current_time();

    if (pulTime == NULL)
    {
        pulTime = &ulTime;
    }

    if (!gos_get_localtime(pulTime, &stLocalTime))
    {
        return;
    }

    if (szFormat == NULL || *szFormat == '\0')
    {
        szFormat = "%04d-%02d-%02d %02d:%02d:%02d";
    }

    sprintf(szTime, szFormat,
            stLocalTime.usYear,
            stLocalTime.ucMonth,
            stLocalTime.ucDay,
            stLocalTime.ucHour,
            stLocalTime.ucMinute,
            stLocalTime.ucSecond);
}

/**
 * @brief          获取pulCurrTime时间的格式化文本
 * @param?          pulCurrTime [in]
 *                  输入参数：当pulCurrTime = NULL时表示获取当前时间的格式化文本否则
 *                           当pulCurrTime ！= NULL 时表示获取pulCurrTime指定的时间的格式化文本
 * @return         CHAR*
 *                 返回static 修饰的数组—该数组保存pulCurrTime时间的格式化文本： YYYY-MM-DD hh24:mm:ss格式的字符串
 * @author         zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par示例:
 * @code
 *
 * @endcode
 * @note
 */
CHAR* gos_get_text_time(UINT32 *pulCurrTime)
{
    static CHAR         acTextTime[64];

    gos_get_text_time_ex(pulCurrTime, NULL, acTextTime);

    return acTextTime;
}

/*************************************************************************
 * 函数名称: gos_get_rand
 * 功    能: 产生随机数
 * 参    数:
 * 参数名称          输入/输出               描述
 * 函数返回: 随机数
 * 说    明:
 *************************************************************************/

/**
 * @brief          产生一个随机数
 * @return         UINT32 返回产生的随机数
 * @author         zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par?示例:
 * @code
 *
 * @endcode
 * @note
 *  需要调用srand()产生随机数种子
 */
UINT32 gos_get_rand(VOID)
{
    UINT32  ulValue = 0;
    static BOOL bInited = FALSE;

    if (!bInited)
    {
        srand(time(NULL));
        bInited = TRUE;
    }

    ulValue = (rand() << 16) + rand();

    return ulValue;
}

/**
 * @brief          产生一个指定范围的随机数
 * @param          ulMin   [in]  输入参数 范围下界
 * @param          ulMax   [in]  输入参数 范围上界
 * @return         UINT32  返回一个 指定范围的随机数
 * @author         zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par示例:
 * @code
 *
 * @endcode
 * @note
 * 1、当ulMax < ilMain 时返回 0
 * 2、
 */
UINT32 gos_get_range_rand(UINT32 ulMin, UINT32 ulMax)
{
    static BOOL bInit = FALSE;
    UINT32  ulValue = 0;

    if (ulMax < ulMin)
    {
        return ulValue;
    }

    if (!bInit)
    {
        srand(time(NULL));
        bInit = TRUE;
    }

    ulValue = ulMin + rand()%(ulMax - ulMin +1);

    return ulValue;
}

UINT8 gos_hex_to_byte(UINT8 ucHexChar)
{
    if (ucHexChar >= '0' && ucHexChar <= '9')   return ucHexChar - '0';
    if (ucHexChar >= 'a' && ucHexChar <= 'f')   return ucHexChar - 'a' + 0x0a;
    if (ucHexChar >= 'A' && ucHexChar <= 'F')   return ucHexChar - 'A' + 0x0A;

    return 0xff;
}

BOOL gos_hexstr_to_uint(const CHAR *szHex, UINT32 *pulValue)
{
    UINT64      u64Value = 0;
    UINT64      u64Max = 0xffffffff;
    UINT32      ulValue;

    if (!szHex)
    {
        return FALSE;
    }

    if (szHex[0] == '0' && (szHex[1] == 'x' || szHex[1] == 'X'))
    {
        szHex += 2;
    }

    while(*szHex != '\0')
    {
        ulValue = gos_hex_to_byte(*szHex++);
        if (ulValue > 0x0f)
        {
            return FALSE;
        }

        u64Value = (u64Value<<4) | ulValue;

        if (u64Value > u64Max)
        {
            return FALSE;
        }
    }

    *pulValue = (UINT32)u64Value;

    return TRUE;
}

BOOL gos_hexstr_to_byte(const CHAR *szHex, UINT8* pucByte)
{
    UINT32  i;
    UINT32  ulLen = strlen(szHex);
    UINT8   ucLow;
    UINT8   ucHigh;

    if ((ulLen & 1) != 0)
    {
        return FALSE;
    }

    ulLen >>= 1;
    for (i=0; i<ulLen; i++)
    {
        ucLow = gos_hex_to_byte(*szHex++);
        if (ucLow > 0x0f)
        {
            return FALSE;
        }

        ucHigh = gos_hex_to_byte(*szHex++);
        if (ucHigh > 0x0f)
        {
            return FALSE;
        }

        pucByte[i] = (ucLow << 4) + ucHigh;
    }

    return TRUE;
}

VOID gos_byte_to_hexstr(UINT8* pucByte, UINT32 ulLen, CHAR *szHex)
{
    UINT32  i;

    *szHex = '\0';
    for (i=0; i<ulLen; i++)
    {
        sprintf(szHex, "%02X", *pucByte++);
        szHex += 2;
    }
}

/**
* @brief          获取系统实时时间
* @return         UINT32返回值 系统实时时间的秒数
* @author         zhengwenjie(zhengwenjie@gbcom.com.cn)
* @par示例:
* @code
*
* @endcode
* @note
*    * struct timespec
*   {
*       time_t tv_sec; //秒
*       long tv_nsec; //纳秒
*   };
*
*   CLOCK_REALTIME:系统实时时间,随系统实时时间改变而改变,即从UTC1970-1-1 0:0:0开始计时,中间时刻如果系统时间被用户该成其他,则对应的时间相应改变
*   CLOCK_MONOTONIC:从系统启动这一刻起开始计时,不受系统时间被用户改变的影响
*   CLOCK_PROCESS_CPUTIME_ID:本进程到当前代码系统CPU花费的时间
*   CLOCK_THREAD_CPUTIME_ID:本线程到当前代码系统CPU花费的时间
 */
UINT32 gos_get_gmtime(VOID)
{
#ifdef _OSWIN32_
    time_t      stCurrTime = time(NULL);
    struct tm   *pstGMTime = gmtime(&stCurrTime);

    return (UINT32)mktime(pstGMTime);

#else
    struct timespec     stTime;

    clock_gettime(CLOCK_REALTIME, &stTime);

    return (UINT32)stTime.tv_sec;
#endif
}

/**
 * @brief          设置系统时间 秒级
 * @param          ulSecond[in] 输入参数 需要设置的时间的秒数
 * @return         VOID
 * @author         zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par示例:
 * @code
 *
 * @endcode
 * @note
 */
VOID gos_set_gmtime(UINT32 ulSecond)
{
#ifdef _OSWIN32_
    SYSTEMTIME  stSysTime = {0};
    struct tm   stLocalTime;
    time_t      stTime = ulSecond;

    memcpy(&stLocalTime, localtime(&stTime), sizeof(stLocalTime));

    stSysTime.wYear   = (UINT16)(stLocalTime.tm_year + 1900);
    stSysTime.wMonth  = (UINT16)(stLocalTime.tm_mon + 1);
    stSysTime.wDay    = (UINT16)(stLocalTime.tm_mday);
    stSysTime.wHour   = (UINT16)(stLocalTime.tm_hour);
    stSysTime.wMinute = (UINT16)(stLocalTime.tm_min);
    stSysTime.wSecond = (UINT16)(stLocalTime.tm_sec);

    SetLocalTime(&stSysTime);
#else
    struct timespec stNewTime = {0};

    stNewTime.tv_sec  = ulSecond;
    stNewTime.tv_nsec = 0;

    clock_settime (CLOCK_REALTIME, &stNewTime);
#endif
}

BOOL gos_set_localtime(UINT32 ulSecond)
{
#ifdef _OSWIN32_

    SYSTEMTIME  stSysTime = {0};
    struct tm   stLocalTime;
    time_t      stTime = ulSecond;

    memcpy(&stLocalTime, localtime(&stTime), sizeof(stLocalTime));

    stSysTime.wYear   = (UINT16)(stLocalTime.tm_year + 1900);
    stSysTime.wMonth  = (UINT16)(stLocalTime.tm_mon + 1);
    stSysTime.wDay    = (UINT16)(stLocalTime.tm_mday);
    stSysTime.wHour   = (UINT16)(stLocalTime.tm_hour);
    stSysTime.wMinute = (UINT16)(stLocalTime.tm_min);
    stSysTime.wSecond = (UINT16)(stLocalTime.tm_sec);

    return SetLocalTime(&stSysTime);
#else
    struct timespec stNewTime = {0};

    stNewTime.tv_sec  = ulSecond;
    stNewTime.tv_nsec = 0;

    clock_settime(CLOCK_REALTIME, &stNewTime);

    //system("clock -w");
    gos_system("hwclock -w");

    return TRUE;
#endif
}

/*******************************************************************************
* 函数名称: gos_get_prime
* 功    能: 获取指定范围的素数
* 参    数:
* 参数名称          输入/输出   描述
* ulStart           IN          指定返回的素数应该大于的值
* 函数返回: 大于指定值的素数，0表示失败
* 说    明: 应该在系统初始化时，先调用本函数一次，以先进行初始化
*******************************************************************************/
#ifdef GOS_SPEED_OPTIMIZE
UINT32 gos_get_prime(UINT32 ulStart)
{
    const  UINT32   ulMaxPrimeNum = 1024*64;    /* 建议不修改本值 */
    static UINT32   *apulPrime = NULL;
    static UINT32   ulPrimeNum = 0;
    UINT32          i, j;
    BOOL            bPrime;

    /* 初始化 */
    if (ulPrimeNum == 0)
    {
        apulPrime = (UINT32*)malloc(ulMaxPrimeNum*sizeof(UINT32));
        if (NULL == apulPrime)
        {
            return 0;
        }

        memset(apulPrime, 0, ulMaxPrimeNum*sizeof(UINT32));
        apulPrime[ulPrimeNum++] = 2;
        apulPrime[ulPrimeNum++] = 3;

        while(1)
        {
            for (i=apulPrime[ulPrimeNum-1]+2; i<=0xfffffffe; i+=2)
            {
                bPrime = TRUE;
                for (j=0; j<ulPrimeNum; j++)
                {
                    if (apulPrime[j]*2 > i ||
                        apulPrime[j]*apulPrime[j] > i ||
                        apulPrime[j] >= 65536 )
                    {
                        break;
                    }

                    if ((i % apulPrime[j]) == 0)
                    {
                        bPrime = FALSE;
                        break;
                    }
                }

                if (bPrime)
                {
                    apulPrime[ulPrimeNum++] = i;
                    break;
                }
            }

            if (ulPrimeNum >= ulMaxPrimeNum)
            {
                break;
            }
        }

        //printf("\n ulPrimeNum = %u, max prime = %u \n", ulPrimeNum, apulPrime[ulPrimeNum-1]);
    }

    /* 静态表中查询 */
    if (ulStart <= apulPrime[ulPrimeNum-1])
    {
        for (i=0; i<ulPrimeNum; i++)
        {
            if (apulPrime[i] >= ulStart)
            {
                return apulPrime[i];
            }
        }

        return apulPrime[ulPrimeNum-1];
    }

    /* 动态计算 */
    for (i=(ulStart/2)*2+1; i<=0xfffffffe; i+=2)
    {
        bPrime = TRUE;
        for (j=0; j<ulPrimeNum; j++)
        {
            if (apulPrime[j]*2 > i ||
                apulPrime[j]*apulPrime[j] > i ||
                apulPrime[j] >= 65536 )
            {
                break;
            }

            if ((i % apulPrime[j]) == 0)
            {
                bPrime = FALSE;
                break;
            }
        }

        if (bPrime)
        {
            return i;
        }
    }

    return 0;
}
#else
UINT32 gos_get_prime(UINT32 ulStart)
{
    UINT32      i;
    UINT32      j;
    BOOL        bPrime;
    UINT32      ulMax = (UINT32)sqrt(ulStart);

    for (i=(ulStart/2)*2+1; i<=0xfffffffe; i+=2)
    {
        ulMax = (UINT32)sqrt(i);

        bPrime = TRUE;
        for (j=2; j<=ulMax; j++)
        {
            if (MOD(i, j) == 0)
            {
                bPrime = FALSE;
                break;
            }
        }

        if (bPrime)
        {
            return i;
        }
    }

    return 0;
}
#endif

BOOL gos_get_file_time(const CHAR* szFileName, UINT32 *pulCreateTime, UINT32 *pulModifyTime)
{
#ifdef _OSWIN32_

#if 0//def _BCB_
    struct stat  stStat;
#else
    struct _stat stStat;
#endif

    *pulCreateTime = 0;
    *pulModifyTime = 0;

    if (_stat(szFileName, &stStat) != 0)
    {
        return FALSE;
    }

    *pulCreateTime = stStat.st_ctime;
    *pulModifyTime = stStat.st_mtime;

    return TRUE;
#else
    INT32       fd = open(szFileName, O_RDONLY);
    struct stat stStat;

    *pulCreateTime = 0;
    *pulModifyTime = 0;

    if (fd == -1)
    {
        return FALSE;
    }

    //获取文件的大小
    if (fstat(fd, &stStat) < 0)
    {
        close(fd);
        return FALSE;
    }

    close(fd);

    *pulCreateTime = stStat.st_ctime;
    *pulModifyTime = stStat.st_mtime;

    return TRUE;
#endif
}

INT32 gos_get_file_size(const CHAR* szFileName)
{
#ifdef _OSWIN32_

#if 0//def _BCB_
    struct stat  stStat;
#else
    struct _stat stStat;
#endif

    if (_stat(szFileName, &stStat) != 0)
    {
        return -1;
    }

    return stStat.st_size;

#else
    INT32       fd = open(szFileName, O_RDONLY);
    struct stat sbuf;

    if (fd == -1)
    {
        return -1;
    }

    //获取文件的大小
    if (fstat(fd, &sbuf) < 0)
    {
        close(fd);
        return -1;
    }

    close(fd);
    return (INT32)sbuf.st_size;
#endif
}

BOOL gos_save_file(const CHAR *szFile, VOID *pData, UINT32 ulLen)
{
    HANDLE  hFile;
    INT32   iWriteLen = 0;

    if (!pData)
    {
        return FALSE;
    }

    hFile = gos_fopen(szFile, "wb");
    if (!hFile)
    {
        return FALSE;
    }

    if (ulLen > 0)
    {
        iWriteLen = gos_fwrite(hFile, pData, ulLen);
    }
    gos_fclose(hFile);

    return iWriteLen == (INT32)ulLen;
}

BOOL gos_save_string_to_file(const CHAR *szFile, const CHAR *szText)
{
    return gos_save_file(szFile, (VOID*)szText, strlen(szText));
}

CHAR* gos_load_string_from_file(const CHAR *szFile)
{
    HANDLE  hFile;
    CHAR    *szOut;
    INT32   iReadLen;
    INT32   iLen = gos_get_file_size(szFile);

    if (iLen < 0)
    {
        return NULL;
    }

    if (iLen == 0)
    {
        return strdup("");
    }

    hFile = gos_fopen(szFile, "rb");
    if (!hFile)
    {
        return NULL;
    }

    szOut = (CHAR*)calloc(1, iLen+1);
    if (!szOut)
    {
        gos_fclose(hFile);
        return NULL;
    }

    iReadLen = gos_fread(hFile, (VOID*)szOut, iLen);
    gos_fclose(hFile);
    if (iReadLen != iLen)
    {
        free(szOut);
        return NULL;
    }

    return szOut;
}

VOID gos_close_file_hander(HANDLE hFile)
{
    if (hFile)
    {
#ifdef _OSWIN32_
        FindClose(hFile);
#else
        closedir((DIR*)hFile);
#endif
    }
}

HANDLE gos_find_first_file(const CHAR *szDir, CHAR *szFile, BOOL *pbIsDir)
{
    BOOL        bRet = TRUE;
    CHAR        *szCurrFile;
    HANDLE      hFile;

#ifdef _OSWIN32_
    WIN32_FIND_DATA     stFindData;
    HANDLE              hfind;
    CHAR                acSearch[1024] = {0};
#else
    DIR                 *dp;
    struct dirent       *dirp;
    struct stat         statbuf;
#endif

    if (szDir == NULL || szFile == NULL || pbIsDir == NULL)
    {
        return NULL;
    }

    szFile[0] = '\0';
    *pbIsDir = FALSE;

#ifdef _OSWIN32_
    sprintf(acSearch, "%s\\*.*", szDir);
    hfind = FindFirstFile(acSearch, &stFindData);
    if(hfind == INVALID_HANDLE_VALUE)
    {
        return NULL;
    }
#else
    dp = opendir(szDir);
    if (NULL == dp)
    {
        return NULL;
    }

    dirp = readdir(dp);
    if (NULL == dirp)
    {
        closedir(dp);
        return NULL;
    }

#endif

    do
    {
#ifdef _OSWIN32_
        szCurrFile = stFindData.cFileName;
#else
        szCurrFile = dirp->d_name;
#endif

        if (strcmp(szCurrFile, ".")  == 0 ||
            strcmp(szCurrFile, "..") == 0 )
        {
            continue;
        }

        sprintf(szFile, "%s%c%s", szDir, DIR_SEPARATOR, szCurrFile);

#ifdef _OSWIN32_
        if((stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
#else
        if (lstat(szFile, &statbuf) < 0)
        {
            bRet = FALSE;
            break;
        }

        if (S_ISDIR(statbuf.st_mode)) // dir
#endif
        {
            *pbIsDir = TRUE;

            break;
        }
        else
        {
            *pbIsDir = FALSE;

            break;
        }
#ifdef _OSWIN32_
    }while(FindNextFile(hfind, &stFindData));
#else
    }while((dirp = readdir(dp)) != NULL);
#endif

#ifdef _OSWIN32_
    hFile = hfind;
#else
    hFile = (HANDLE)dp;
#endif

    if (*szFile == '\0')
    {
        bRet = FALSE;
    }

    if (!bRet)
    {
#ifdef _OSWIN32_
        FindClose(hfind);
#else
        closedir(dp);
#endif

        return NULL;
    }
    else
    {
        return hFile;
    }
}

BOOL gos_find_next_file(HANDLE hFile, const CHAR *szDir, CHAR *szFile, BOOL *pbIsDir)
{
    CHAR        acFile[512];
    BOOL        bRet = TRUE;
    CHAR        *szCurrFile;

#ifdef _OSWIN32_
    WIN32_FIND_DATA     stFindData;
    HANDLE              hfind;
#else
    DIR                 *dp;
    struct dirent       *dirp;
    struct stat         statbuf;
#endif

    if (szDir == NULL || szFile == NULL || pbIsDir == NULL)
    {
        return FALSE;
    }

    szFile[0] = '\0';
    *pbIsDir = FALSE;

#ifdef _OSWIN32_
    hfind = hFile;
#else
    dp = (DIR*)hFile;
#endif

#ifdef _OSWIN32_
    while(FindNextFile(hfind, &stFindData))
#else
    while((dirp = readdir(dp)) != NULL)
#endif
    {
#ifdef _OSWIN32_
        szCurrFile = stFindData.cFileName;
#else
        szCurrFile = dirp->d_name;
#endif

        if (strcmp(szCurrFile, ".")  == 0 ||
            strcmp(szCurrFile, "..") == 0 )
        {
            continue;
        }

        sprintf(acFile, "%s%c%s", szDir, DIR_SEPARATOR, szCurrFile);

#ifdef _OSWIN32_
        if((stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
#else
        if (lstat(acFile, &statbuf) < 0)
        {
            bRet = FALSE;
            break;
        }

        if (S_ISDIR(statbuf.st_mode)) // dir
#endif
        {
            strcpy(szFile, acFile);
            *pbIsDir = TRUE;

            break;
        }
        else
        {
            strcpy(szFile, acFile);
            *pbIsDir = FALSE;

            break;
        }
    }

    if (*szFile == '\0')
    {
        bRet = FALSE;
    }

    return bRet;
}

HANDLE gos_open_dir(const CHAR *szDir)
{
    _GOS_DIR_HANDLE_T   *pstHandle = NULL;

    if (NULL == szDir || *szDir == '\0')
    {
        return NULL;
    }

    if (!gos_file_exist(szDir))
    {
        return NULL;
    }

    pstHandle = (_GOS_DIR_HANDLE_T*)GOS_ZALLOC(sizeof(_GOS_DIR_HANDLE_T));
    if (!pstHandle)
    {
        return NULL;
    }

    strncpy(pstHandle->acDir, szDir, sizeof(pstHandle->acDir)-1);

    return (HANDLE)pstHandle;
}

VOID gos_close_dir(HANDLE hDir)
{
    _GOS_DIR_HANDLE_T   *pstHandle = (_GOS_DIR_HANDLE_T*)hDir;

    if (pstHandle)
    {
        if (pstHandle->hFile)
        {
#ifdef _OSWIN32_
            FindClose(pstHandle->hFile);
#else
            closedir((DIR*)pstHandle->hFile);
#endif
        }

        free(hDir);
    }
}

BOOL gos_get_next_file(HANDLE hDir, CHAR *szFile, BOOL *pbIsDir)
{
    _GOS_DIR_HANDLE_T   *pstHandle = (_GOS_DIR_HANDLE_T*)hDir;

    if (!hDir)
    {
        return FALSE;
    }

    if (!pstHandle->hFile)
    {
        pstHandle->hFile = gos_find_first_file(pstHandle->acDir, szFile, pbIsDir);
        if (!pstHandle->hFile)
        {
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
    else
    {
        return gos_find_next_file(pstHandle->hFile, pstHandle->acDir, szFile, pbIsDir);
    }
}

/**
 * @brief           格式化文件路径
 * @param           szFile    [in] 文件名
 * @return          CHAR*      返回格式化之后的路径
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par 示例:
 * @code
 *
 * @endcode
 * @note
 * 1. format \\ ->/
 * 2. remove /./  \\.\\
 * 3. replace /../  \\..\\
 */
CHAR* gos_format_path(CHAR *szFile)
{
    CHAR    *szTmp;
    CHAR    *szStart;
    CHAR    acToken[32];
    BOOL    bExist;

    // format \\ ->/
    for (szTmp=szFile; *szTmp; szTmp++)
    {
        if (*szTmp == '\\' || *szTmp == '/')
        {
            *szTmp = DIR_SEPARATOR;
        }
    }

    /* remove /./  \\.\\ */
#ifdef _OSWIN32_
    strcpy(acToken, "\\.\\");
#else
    strcpy(acToken, "/./");
#endif

    while(1)
    {
        szTmp = strstr(szFile, acToken);
        if (!szTmp)
        {
            break;
        }

        memmove(szTmp, szTmp+2, strlen(szTmp+2)+1);
    }

    while(1)
    {
        szTmp = strstr(szFile, "\\\\");
        if (!szTmp)
        {
            break;
        }

        memmove(szTmp, szTmp+1, strlen(szTmp+1)+1);
    }

    while(1)
    {
        szTmp = strstr(szFile, "//");
        if (!szTmp)
        {
            break;
        }

        memmove(szTmp, szTmp+1, strlen(szTmp+1)+1);
    }

    /* replace /../  \\..\\ */
#ifdef _OSWIN32_
    strcpy(acToken, "\\..\\");
#else
    strcpy(acToken, "/../");
#endif

    while(1)
    {
        szTmp = strstr(szFile, acToken);
        if (!szTmp)
        {
            break;
        }

        // szTmp is (/xxx)/../yyy
        bExist = FALSE;
        for (szStart=szTmp-1; szStart>=szFile; szStart--)
        {
            // szStart is /xxx/../yyy
            if (*szStart == DIR_SEPARATOR)
            {
                memmove(szStart, szTmp+3, strlen(szTmp+3)+1);
                bExist = TRUE;
                break;
            }
        }

        if (!bExist)
        {
            break;
        }
    }

    return szFile;
}

/**
 * @brief           获取文件路径
 * @param           szFile    [in] 文件名
 * @param           szPath    [out] 返回文件目录
 * @return          VOID
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @par 示例:       获取 sa.log 的路径
 * @code
 *
 * @endcode
 * @note
 */
VOID gos_get_file_path(const CHAR *szFile, CHAR *szPath)
{
    CHAR    *szEnd;

    if (IS_NULL_STRING(szFile) || !szPath)
    {
        return;
    }

    if (szPath != szFile)
    {
        strcpy(szPath, szFile);
    }

    szEnd = szPath + strlen(szPath);
    for (; szEnd>szPath; szEnd--)
    {
        if (*szEnd == '/' || *szEnd == '\\')
        {
            *szEnd = '\0';
            break;
        }
    }

    gos_format_path(szPath);
}

VOID gos_get_file_rawname(const CHAR *szFile, CHAR *szRawName)
{
    CHAR    *szEnd;

    if (IS_NULL_STRING(szFile) || !szRawName)
    {
        return;
    }

    szEnd = (CHAR*)szFile + strlen(szFile);
    for (; szEnd>szFile; szEnd--)
    {
        if (*szEnd == '/' || *szEnd == '\\')
        {
            strcpy(szRawName, szEnd+1);
            return;
        }
    }

    strcpy(szRawName, szFile);
}

BOOL gos_set_file_executable(const CHAR *szFile)
{
#ifdef _OSWIN32_
    return TRUE;
#else
    struct stat     stStat;

    UINT32          ulMode = S_ISUID|S_ISGID|S_IXUSR|S_IXUSR|S_IXGRP|S_IXOTH;
    INT32           iMode;

    if (stat(szFile, &stStat) < 0)
    {
        return FALSE;
    }

    if ((stStat.st_mode & ulMode) == ulMode)
    {
        return TRUE;
    }

    iMode = (stStat.st_mode & 0x7777) | ulMode;

    if (chmod(szFile, iMode) < 0)
    {
        return FALSE;
    }

    return TRUE;
#endif
}

BOOL gos_set_file_writable(const CHAR *szFile)
{
#ifdef _OSWIN32_
    DWORD   dwAttrs = GetFileAttributes(szFile);
    DWORD   dwWritable = (DWORD)~FILE_ATTRIBUTE_READONLY;

    if (dwAttrs & FILE_ATTRIBUTE_READONLY)
    {
        dwAttrs = dwAttrs & dwWritable;

        return SetFileAttributes(szFile, dwAttrs);
    }

    return TRUE;
#else
    struct stat     stStat;
    UINT32          ulMode = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH;
    INT32           iMode;

    if (stat(szFile, &stStat) < 0)
    {
        return FALSE;
    }

    if ((stStat.st_mode & ulMode) == ulMode)
    {
        return TRUE;
    }

    iMode = (stStat.st_mode & 0x7777) | ulMode;

    if (chmod(szFile, iMode) < 0)
    {
        return FALSE;
    }

    return TRUE;
#endif
}

BOOL gos_set_file_readable(const CHAR *szFile)
{
#ifdef _OSWIN32_
    return TRUE;
#else
    struct stat     stStat;
    UINT32          ulMode = S_IRUSR|S_IRGRP|S_IROTH;
    INT32           iMode;

    if (stat(szFile, &stStat) < 0)
    {
        return FALSE;
    }

    if ((stStat.st_mode & ulMode) == ulMode)
    {
        return TRUE;
    }

    iMode = (stStat.st_mode & 0x7777) | ulMode;
    if (chmod(szFile, iMode) < 0)
    {
        return FALSE;
    }

    return TRUE;
#endif
}

BOOL gos_set_dir_executable(const CHAR *szDir, BOOL bRecursive)
{
    HANDLE  hDir = gos_open_dir(szDir);
    CHAR    acFile[260];
    BOOL    bIsDir;
    BOOL    bRet = TRUE;

    if (!hDir)
    {
        return FALSE;
    }

    while(gos_get_next_file(hDir, acFile, &bIsDir))
    {
        if (bIsDir)
        {
            if (bRecursive && !gos_set_dir_executable(acFile, TRUE))
            {
                goto end;
            }
        }
        else
        {
            if (!gos_set_file_executable(acFile))
            {
                goto end;
            }
        }
    }

    bRet = TRUE;

end:
    gos_close_dir(hDir);

    return bRet;
}

BOOL gos_set_dir_writable(const CHAR *szDir, BOOL bRecursive)
{
    HANDLE  hDir = gos_open_dir(szDir);
    CHAR    acFile[260];
    BOOL    bIsDir;
    BOOL    bRet = TRUE;

    if (!hDir)
    {
        return FALSE;
    }

    while(gos_get_next_file(hDir, acFile, &bIsDir))
    {
        if (bIsDir)
        {
            if (bRecursive && !gos_set_dir_writable(acFile, TRUE))
            {
                goto end;
            }
        }
        else
        {
            if (!gos_set_file_writable(acFile))
            {
                goto end;
            }
        }
    }

    bRet = TRUE;

end:
    gos_close_dir(hDir);

    return bRet;
}

BOOL gos_set_dir_readable(const CHAR *szDir, BOOL bRecursive)
{
    HANDLE  hDir = gos_open_dir(szDir);
    CHAR    acFile[260];
    BOOL    bIsDir;
    BOOL    bRet = TRUE;

    if (!hDir)
    {
        return FALSE;
    }

    while(gos_get_next_file(hDir, acFile, &bIsDir))
    {
        if (bIsDir)
        {
            if (bRecursive && !gos_set_dir_readable(acFile, TRUE))
            {
                goto end;
            }
        }
        else
        {
            if (!gos_set_file_readable(acFile))
            {
                goto end;
            }
        }
    }

    bRet = TRUE;

end:
    gos_close_dir(hDir);

    return bRet;
}

UINT64 gos_htonll(UINT64 u64Data)
{
    return  ((u64Data & 0xff00000000000000LL) >> 56) |
            ((u64Data & 0x00ff000000000000LL) >> 40) |
            ((u64Data & 0x0000ff0000000000LL) >> 24) |
            ((u64Data & 0x000000ff00000000LL) >> 8)  |
            ((u64Data & 0x00000000ff000000LL) << 8)  |
            ((u64Data & 0x0000000000ff0000LL) << 24) |
            ((u64Data & 0x000000000000ff00LL) << 40) |
            ((u64Data & 0x00000000000000ffLL) << 56) ;
}

UINT64 gos_ntohll(UINT64 u64Data)
{
    return gos_htonll(u64Data);
}

CHAR* gos_right_strchr(const CHAR *szText, CHAR cChar)
{
    CHAR *szLast;

    if (!szText)
    {
        return NULL;
    }

    szLast = (CHAR*)szText + strlen(szText);
    for (; szLast>=szText; szLast--)
    {
        if (*szLast == cChar)
        {
            return szLast;
        }
    }

    return NULL;
}

CHAR* gos_right_strstr(const CHAR *szText, const CHAR *szSub)
{
    CHAR    *szLast;
    UINT32  ulSubLen;

    if (!szText || !szSub)
    {
        return NULL;
    }

    ulSubLen = strlen(szSub);
    if (ulSubLen == 0)
    {
        return (CHAR*)szText;
    }

    szLast = (CHAR*)szText + strlen(szText) - ulSubLen;
    for (; szLast>=szText; szLast--)
    {
        if (memcmp(szLast, szSub, ulSubLen) == 0)
        {
            return szLast;
        }
    }

    return NULL;
}

INT32 gos_find_str(const CHAR *szText, const CHAR *szSub)
{
    CHAR    *szPos;

    if (!szText || !szSub)
    {
        return -1;
    }

    szPos = strstr(szText, szSub);
    if (!szPos)
    {
        return -1;
    }

    return (INT32)(szPos - szText);
}

INT32 gos_right_find_str(const CHAR *szText, const CHAR *szSub)
{
    CHAR    *szPos;

    if (!szText || !szSub)
    {
        return -1;
    }

    //如果子串是空，返回字符串长度
    if (*szSub == '\0')
    {
        return strlen(szText);
    }

    szPos = gos_right_strstr(szText, szSub);

    if (!szPos)
    {
        return -1;
    }

    return (INT32)(szPos - szText);
}

VOID gos_str_to_lower(CHAR *szText)
{
    for(; *szText; szText++)
    {
        *szText = (CHAR)tolower(*szText);
    }
}

VOID gos_str_to_upper(CHAR *szText)
{
    for(; *szText; szText++)
    {
        *szText = (CHAR)toupper(*szText);
    }
}

/**
 * @brief           查找一个字符串中最后一个 '\' 或 '/' 字符后的字符串，如果没有找到则返回源字符串
 * @param           szFileName    [in]
 * @return          CHAR*
 * @date            2021-09-16 14:08:56
 * @note
 */
CHAR* gos_get_simple_file_name(const CHAR *szFileName)
{
    CHAR *szLast;

    if (!szFileName)
    {
        return NULL;
    }

    szLast = (CHAR*)szFileName + strlen(szFileName);
    for (; szLast>=szFileName; szLast--)
    {
        if (*szLast == '\\' || *szLast == '/')
        {
            return szLast+1;
        }
    }

    return (CHAR*)szFileName;
}

BOOL gos_atoi(const CHAR *szData, INT32 *piValue)
{
    BOOL    bRet = TRUE;
    BOOL    bReverse = FALSE;
    INT32   iTmp;
    UINT64  i64Value = 0;
    CHAR    *szStart = NULL;
    CHAR    *szEnd = NULL;

    if (IS_NULL_STRING(szData) || !piValue)
    {
        return FALSE;
    }

    for (szStart=(CHAR*)(szData); *szStart != '\0'; szStart++)
    {
        if (!gos_isspace(*szStart))
        {
            break;
        }
    }

    if (*szStart == '\0')
    {
        *piValue = 0;
        return TRUE;
    }

    if (*szStart == '+' && szStart[1] >='0' && szStart[1] <='9')
    {
        szStart ++;
    }
    else
    {
        if (*szStart == '-' && szStart[1] >='0' && szStart[1] <='9')
        {
            bReverse = TRUE;
            szStart ++;
        }
    }

    szEnd = szStart + strlen(szStart)-1;
    for(; szEnd >= szStart; szEnd--)
    {
        if (!gos_isspace(*szEnd))
        {
            break;
        }
    }

    for (; szStart<=szEnd; szStart++)
    {
        iTmp = *szStart - '0';
        if (iTmp < 0 || iTmp > 9)
        {
            bRet = FALSE;
            break;
        }

        i64Value = (i64Value*10) + iTmp;

        if (i64Value > 0xffffffff)
        {
            bRet = FALSE;
            break;
        }
    }

    if (bReverse)
    {
        *piValue = -(INT32)i64Value;
    }
    else
    {
        *piValue = (INT32)i64Value;
    }

    return bRet;
}

BOOL gos_atou(const CHAR *szData, UINT32 *pulValue)
{
    BOOL    bRet = TRUE;
    INT32   iTmp;
    UINT64  i64Value = 0;
    CHAR    *szStart = NULL;
    CHAR    *szEnd = NULL;

    if (IS_NULL_STRING(szData) || !pulValue)
    {
        return FALSE;
    }

    for (szStart=(CHAR*)szData; *szStart!='\0'; szStart++)
    {
        if (!gos_isspace(*szStart))
        {
            break;
        }
    }

    if (*szStart == '\0')
    {
        *pulValue = 0;
        return TRUE;
    }

    if (*szStart == '+' && szStart[1] >='0' && szStart[1] <='9')
    {
        szStart ++;
    }

    szEnd = szStart + strlen(szStart)-1;
    for(; szEnd >= szStart; szEnd--)
    {
        if (!gos_isspace(*szEnd))
        {
            break;
        }
    }

    for (; szStart<=szEnd; szStart++)
    {
        iTmp = *szStart - '0';
        if (iTmp < 0 || iTmp > 9)
        {
            bRet = FALSE;
            break;
        }

        i64Value = (i64Value*10) + iTmp;

        if (i64Value > 0xffffffff)
        {
            bRet = FALSE;
            break;
        }
    }

    *pulValue = (UINT64)i64Value;

    return bRet;
}

/**
 * @brief           转换字符串到 INT64
 * @param           szData          [in]
 * @param           pi64Value       [out]
 * @return          BOOL    处理成功或失败
 *  @retval          TRUE    转换成功
 *  @retval          FALSE   转换失败
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-12-01 11:20:17
 * @note
 * @warning
 * 用例不通过:
 * 输入: "-9223372036854775808" (LLONG_MIN)
 * 函数返回 FALSE， *pi64Value == -922337203685477580
 */
BOOL gos_atoi64(const CHAR *szData, INT64 *pi64Value)
{
    BOOL    bRet = TRUE;
    BOOL    bReverse = FALSE;
    INT32   iTmp;
    UINT64  u64Value = 0;
    CHAR    *szStart = NULL;
    CHAR    *szEnd = NULL;
    UINT64  u64MaxValue = INT64_MAX;

    if (IS_NULL_STRING(szData) || !pi64Value)
    {
        return FALSE;
    }

    for (szStart=(CHAR*)szData; *szStart!='\0'; szStart++)
    {
        if (!gos_isspace(*szStart))
        {
            break;
        }
    }

    if (*szStart == '\0')
    {
        *pi64Value = 0;
        return TRUE;
    }

    if (*szStart == '+' && szStart[1] >='0' && szStart[1] <='9')
    {
        szStart ++;
    }
    else
    {
        if (*szStart == '-' && szStart[1] >='0' && szStart[1] <='9')
        {
            bReverse = TRUE;
            szStart ++;
        }
    }

    szEnd = szStart + strlen(szStart)-1;
    for(; szEnd >= szStart; szEnd--)
    {
        if (!gos_isspace(*szEnd))
        {
            break;
        }
    }

    for (; szStart<=szEnd; szStart++)
    {
        iTmp = *szStart - '0';
        if (iTmp < 0 || iTmp > 9)
        {
            bRet = FALSE;
            break;
        }

        if (u64Value > (u64MaxValue-iTmp)/10)
        {
            bRet = FALSE;
            break;
        }

        u64Value = (u64Value*10) + iTmp;
    }

    if (bReverse)
    {
        pi64Value[0] = 0;
        pi64Value[0] -= u64Value;
    }
    else
    {
        pi64Value[0] = u64Value;
    }

    return bRet;
}

BOOL gos_atou64(const CHAR *szData, UINT64 *pu64Value)
{
    BOOL    bRet = TRUE;
    INT32   iTmp;
    UINT64  u64Value = 0;
    CHAR    *szStart = NULL;
    CHAR    *szEnd = NULL;
    UINT64  u64Max = (UINT64)(-1);

    if (IS_NULL_STRING(szData) || !pu64Value)
    {
        return FALSE;
    }

    for (szStart=(CHAR*)szData; *szStart!='\0'; szStart++)
    {
        if (!gos_isspace(*szStart))
        {
            break;
        }
    }

    if (*szStart == '\0')
    {
        *pu64Value = 0;
        return TRUE;
    }

    szEnd = szStart + strlen(szStart)-1;
    for(; szEnd >= szStart; szEnd--)
    {
        if (!gos_isspace(*szEnd))
        {
            break;
        }
    }

    for (; szStart<=szEnd; szStart++)
    {
        iTmp = *szStart - '0';
        if (iTmp < 0 || iTmp > 9)
        {
            bRet = FALSE;
            break;
        }

        if (u64Value > (u64Max-iTmp)/10)
        {
            bRet = FALSE;
            break;
        }

        u64Value = (u64Value*10) + iTmp;
    }

    *pu64Value = u64Value;

    return bRet;
}

BOOL gos_atof(const CHAR *szData, DOUBLE *pdValue)
{
    BOOL    bRet = TRUE;
    INT32   iTmp;
    UINT32  ulPointNum = 0;
    CHAR    *szStart = NULL;
    CHAR    *szEnd = NULL;
    CHAR    *szPoint = NULL;

    if (IS_NULL_STRING(szData) || !pdValue)
    {
        return FALSE;
    }

    for (szStart=(CHAR*)szData; *szStart!='\0'; szStart++)
    {
        if (!gos_isspace(*szStart))
        {
            break;
        }
    }

    if (*szStart == '\0')
    {
        *pdValue = 0;
        return TRUE;
    }

    if (*szStart == '+' && szStart[1] >='0' && szStart[1] <='9')
    {
        szStart ++;
    }
    else
    {
        if (*szStart == '-' && szStart[1] >='0' && szStart[1] <='9')
        {
            szStart ++;
        }
    }

    szEnd = szStart + strlen(szStart)-1;
    for(; szEnd >= szStart; szEnd--)
    {
        if (!gos_isspace(*szEnd))
        {
            break;
        }
    }

    for (szPoint=szStart; szPoint<=szEnd; szPoint++)
    {
        if (*szPoint == '.')
        {
            ulPointNum ++;
            if (ulPointNum > 1)
            {
                bRet = FALSE;
                break;
            }
        }
        else
        {
            iTmp = *szPoint - '0';
            if (iTmp < 0 || iTmp > 9)
            {
                bRet = FALSE;
                break;
            }
        }
    }

    *pdValue = atof(szData);

    return bRet;
}

#ifdef GOS_SPEED_OPTIMIZE
INT32 gos_itoa(UINT32 ulDigit, CHAR *szDigit)
{
    static BOOL         bInit = FALSE;
    static UINT32       aulLen[100000];
    static UINT64       au64Value[100000];
    static UINT64       au64Value2[100000];
    UINT32              ulDigitLow,ulDigitHigh;

    if (!bInit)
    {
        UINT32  i;

        bInit = TRUE;

        memset(au64Value, 0, sizeof(au64Value));
        memset(au64Value2, 0, sizeof(au64Value2));

        for (i=0; i<ARRAY_SIZE(au64Value); i++)
        {
            aulLen[i] = sprintf((CHAR*)&au64Value[i], "%u", i);

            sprintf((CHAR*)&au64Value2[i], "%05u", i);
        }
    }

    ulDigitLow = MOD(ulDigit, 100000);
    ulDigitHigh = ulDigit/100000;

    if (ulDigitHigh > 0)
    {
        SET_INT64(szDigit, au64Value[ulDigitHigh]);
        szDigit += aulLen[ulDigitHigh];

        memcpy(szDigit, &au64Value2[ulDigitLow], 6);

        return aulLen[ulDigitHigh]+5;
    }
    else
    {
        memcpy(szDigit, &au64Value[ulDigitLow], aulLen[ulDigitLow]+1);

        return aulLen[ulDigitLow];
    }
}
#else
INT32 gos_itoa(UINT32 ulDigit, CHAR *szDigit)
{
    return sprintf(szDigit, "%u", ulDigit);
}
#endif

#ifdef GOS_SPEED_OPTIMIZE
INT32 gos_i64toa(UINT64 u64Digit, CHAR *szDigit)
{
    static BOOL         bInit = FALSE;
    static UINT32       aulLen[100000];
    static UINT64       au64Value[100000];
    static UINT64       au64Value2[100000];
    UINT32              ulDigit0,ulDigit1,ulDigit2,ulDigit3;
    UINT32              ulLen;

    if (!bInit)
    {
        UINT32  i;

        bInit = TRUE;

        for (i=0; i<ARRAY_SIZE(au64Value); i++)
        {
            aulLen[i] = sprintf((CHAR*)&au64Value[i], "%u", i);

            sprintf((CHAR*)&au64Value2[i], "%05u", i);
        }
    }

    ulDigit0 = (UINT32)MOD(u64Digit, 100000);
    u64Digit /= 100000;

    ulDigit1 = (UINT32)MOD(u64Digit, 100000);
    u64Digit /= 100000;

    ulDigit2 = (UINT32)MOD(u64Digit, 100000);
    ulDigit3 = (UINT32)(u64Digit/100000);

    if (ulDigit3 > 0)
    {
        SET_INT64(szDigit, au64Value[ulDigit3]);
        ulLen = aulLen[ulDigit3];
        szDigit += ulLen;

        SET_INT64(szDigit, au64Value2[ulDigit2]);
        szDigit += 5;
        SET_INT64(szDigit, au64Value2[ulDigit1]);
        szDigit += 5;
        SET_INT64(szDigit, au64Value2[ulDigit0]);

        return ulLen+15;
    }
    else if (ulDigit2 > 0)
    {
        SET_INT64(szDigit, au64Value[ulDigit2]);
        ulLen = aulLen[ulDigit2];
        szDigit += ulLen;

        SET_INT64(szDigit, au64Value2[ulDigit1]);
        szDigit += 5;
        SET_INT64(szDigit, au64Value2[ulDigit0]);

        return ulLen+10;
    }
    else if (ulDigit1 > 0)
    {
        SET_INT64(szDigit, au64Value[ulDigit1]);
        ulLen = aulLen[ulDigit1];
        szDigit += ulLen;

        SET_INT64(szDigit, au64Value2[ulDigit0]);

        return ulLen+5;
    }
    else
    {
        SET_INT64(szDigit, au64Value[ulDigit0]);

        return aulLen[ulDigit0];
    }
}
#else
INT32 gos_i64toa(UINT64 u64Digit, CHAR *szDigit)
{
    return sprintf(szDigit, FORMAT_U64, u64Digit);
}
#endif

/**
 * @brief           判断源字符串是否具有某个前缀
 * @param           szText    [in]  源字符串
 * @param           szLike    [in]  需要查找的前缀
 * @return          BOOL
 * @author          leijie(leijie@gbcom.com.cn)
 * @date            2021-11-17 09:24:08
 * @note
 * lijiancong: 推荐改名为 gos_str_prefix
 */
BOOL gos_str_like(const CHAR *szText, const CHAR *szLike)
{
    if (NULL == szText || NULL == szLike)
    {
        return FALSE;
    }

    for (; ; szText++, szLike++)
    {
        if (*szLike == '\0')
        {
            return TRUE;
        }

        if (*szText != *szLike)
        {
            return FALSE;
        }
    }
}

BOOL gos_str_end_like(const CHAR *szText, const CHAR *szLike)
{
    CHAR    *szEnd;

    if (NULL == szText || NULL == szLike)
    {
        return FALSE;
    }

    szEnd = (CHAR*)szText + strlen(szText) - strlen(szLike);
    if (szEnd < szText)
    {
        return FALSE;
    }

    return strcmp(szEnd, szLike) == 0;
}

/*******************************************************************************
* 函数名称: gos_trim_string
* 功    能: 去除字符串的前后空格
* 参    数:
* 参数名称          输入/输出       描述
  pstNode           IN              节点
  pstAttr           IN              节点属性
* 函数返回:
*
* 说    明:
*******************************************************************************/
CHAR* gos_trim_string(CHAR *szText)
{
    CHAR    *szEnd;
    UINT32  ulLen;

    if (NULL == szText )
    {
        return NULL;
    }

    for (; *szText!='\0'; szText++)
    {
        if (!gos_isspace(*szText))
        {
            break;
        }
    }

    ulLen = strlen(szText);

    for (szEnd=szText+ulLen-1; szEnd>=szText; szEnd--)
    {
        if (!gos_isspace(*szEnd))
        {
            break;
        }

        *szEnd = '\0';
    }

    return szText;
}

VOID gos_right_trim_string(CHAR *szText)
{
    CHAR    *szEnd;
    UINT32  ulLen;

    if (NULL == szText )
    {
        return;
    }

    ulLen = strlen(szText);
    for (szEnd=szText+ulLen-1; szEnd>=szText; szEnd--)
    {
        if (!gos_isspace(*szEnd))
        {
            break;
        }

        *szEnd = '\0';
    }
}

CHAR* gos_left_trim_string(CHAR *szText)
{
    if (NULL == szText )
    {
        return NULL;
    }

    for (; *szText!='\0'; szText++)
    {
        if (!gos_isspace(*szText))
        {
            break;
        }
    }

    return szText;
}

INT32 gos_right_memcmp(VOID* pMem1, VOID *pMem2, UINT32 ulLen)
{
    UINT8   *pucMem1 = (UINT8*)pMem1;
    UINT8   *pucMem2 = (UINT8*)pMem2;

    for ( ;ulLen; ulLen--)
    {
        if (*pucMem1 > *pucMem2)
        {
            return 1;
        }
        else if (*pucMem1-- < *pucMem2--)
        {
            return -1;
        }
    }

    return 0;
}

BOOL gos_get_tmp_file_name(const CHAR *szTmpDir, const CHAR *szPrefix, CHAR *szTmpFile)
{
#ifndef _OSWIN32_
    CHAR    acPrefix[4];
    UINT32  i;
#endif

    if (!szTmpDir || !szTmpFile)
    {
        return FALSE;
    }

    *szTmpFile = '\0';
#ifdef _OSWIN32_
    GetTempFileName(szTmpDir, szPrefix, 0, szTmpFile);

    return TRUE;
#else

    if (szPrefix)
    {
        strncpy(acPrefix, szPrefix, 3);
        acPrefix[3] = '\0';
    }
    else
    {
        acPrefix[0] = '\0';
    }

    for (i=0; i<1024; i++)
    {
        sprintf(szTmpFile, "%s/%s%08X.tmp", szTmpDir, acPrefix, gos_get_rand());
        if (!gos_file_exist(szTmpFile))
        {
            return TRUE;
        }
    }

    return FALSE;
#endif
}

UINT32 gos_get_thread_id(VOID)
{
    UINT32  ulID = 0;

#ifdef _OSWIN32_
    ulID = GetCurrentThreadId ();
#else
    ulID = pthread_self ();
#endif

    return ulID;
}

INT32 gos_time_cmp(GOS_DATETIME_T *pstTime1, GOS_DATETIME_T *pstTime2)
{
    UINT32  ulTime1 = gos_mktime(pstTime1);
    UINT32  ulTime2 = gos_mktime(pstTime2);

    return (INT32)(ulTime1-ulTime2);
}

BOOL gos_parse_time(const CHAR *szTime, GOS_DATETIME_T *pstTime)
{
    INT32   aiTemp[6] = {-1,-1,-1,-1,-1,-1};
    CHAR    acTime[20] = {0};
    UINT32  ulLen = strlen(szTime);
    UINT32  ulTimeLen = 19;//strlen("YYYY-MM-DD HH:MI:SS");
    UINT32  ulTimeLenDay = 10;

    if (!szTime)
    {
        return FALSE;
    }

    if (ulLen == ulTimeLenDay)
    {
        ulLen = sprintf(acTime, "%s %s", szTime, "00:00:00");
    }
    else if (ulLen == ulTimeLen)
    {
        strcpy(acTime, szTime);
    }
    else
    {
        return FALSE;
    }

    if (ulLen != ulTimeLen ||
        acTime[4] != '-' ||
        acTime[7] != '-' ||
        acTime[10] != ' ' ||
        acTime[13] != ':' ||
        acTime[16] != ':' )
    {
        return FALSE;
    }

    sscanf(acTime, "%d-%d-%d %d:%d:%d", &aiTemp[0],&aiTemp[1],&aiTemp[2],&aiTemp[3],&aiTemp[4],&aiTemp[5]);

    if (aiTemp[0] < 0 ||
        aiTemp[1] < 1 || aiTemp[1] > 12 ||
        aiTemp[2] < 1 || aiTemp[2] > 31 ||
        aiTemp[3] < 0 || aiTemp[3] > 23 ||
        aiTemp[4] < 0 || aiTemp[4] > 59 ||
        aiTemp[5] < 0 || aiTemp[5] > 59 )
    {
        return FALSE;
    }

    pstTime->usYear = aiTemp[0];
    pstTime->ucMonth = aiTemp[1];
    pstTime->ucDay = aiTemp[2];
    pstTime->ucHour = aiTemp[3];
    pstTime->ucMinute = aiTemp[4];
    pstTime->ucSecond = aiTemp[5];

    return TRUE;
}

BOOL gos_parse_compact_time(const CHAR *szTime, GOS_DATETIME_T *pstTime)
{
    CHAR    acTime[20] = {0};
    UINT32  ulLen = strlen(szTime);
    UINT32  ulTimeLen = 14;//strlen("YYYYMMDDHHMISS");
    UINT32  ulTimeLenDay = 8;
    INT64   i64Time;

    if (ulLen == ulTimeLenDay)
    {
        ulLen = sprintf(acTime, "%s%s", szTime, "000000");
    }
    else if (ulLen == ulTimeLen)
    {
        strcpy(acTime, szTime);
    }
    else
    {
        return FALSE;
    }

    if (!gos_atoi64(acTime, &i64Time))
    {
        return FALSE;
    }

    // YYYYMMDDHHMISS
    pstTime->ucSecond = MOD(i64Time, 100);
    i64Time /= 100;
    pstTime->ucMinute = MOD(i64Time, 100);
    i64Time /= 100;
    pstTime->ucHour = MOD(i64Time, 100);
    i64Time /= 100;
    pstTime->ucDay = MOD(i64Time, 100);
    i64Time /= 100;
    pstTime->ucMonth = MOD(i64Time, 100);
    i64Time /= 100;
    pstTime->usYear = i64Time;

    return TRUE;
}

/*******************************************************************************
* 函数名称: gos_string_to_bcd
* 功    能: 字符串转换成BCD码
* 函数类型:
* 参    数:
* 参数名称          类型                    输入/输出       描述
* 函数返回:  无
* 说    明:  无
*******************************************************************************/
BOOL gos_string_to_bcd(const CHAR *szString, UINT8 *pucBCD, UINT32 ulBCDLen)
{
    UINT32      i;
    UINT8       ucValue1;
    UINT8       ucValue2;

    if (NULL == pucBCD || NULL == szString)
    {
        return FALSE;
    }

    for (i=0; i<ulBCDLen; i++)
    {
        if (*szString == '\0')
        {
            break;
        }

        ucValue1 = HEX_CHAR_TO_INT(*szString);
        if (ucValue1 > 0x0f)
        {
            memset(pucBCD-i, 0xff, ulBCDLen);
            return FALSE;
        }

        if (*(++szString) == '\0')
        {
            i++;
            *pucBCD++ = 0xf0 | ucValue1;
            break;
        }

        ucValue2 = HEX_CHAR_TO_INT(*szString);
        if (ucValue2 > 0x0f)
        {
            memset(pucBCD-i, 0xff, ulBCDLen);
            return FALSE;
        }

        szString++;
        *pucBCD++ = (ucValue2<<4) + ucValue1;
    }

    if (i < ulBCDLen)
    {
        memset(pucBCD, 0xff, ulBCDLen-i);
    }

    return TRUE;
}

BOOL gos_string_to_bcd_string(const CHAR *szIn, CHAR *szOut)
{
    UINT32      ulNum;
    UINT32      i;

    if (!szIn)
    {
        return FALSE;
    }

    ulNum = strlen(szIn);
    for (i=0; i<ulNum/2; i++)
    {
        if (!isdigit(szIn[0]) || !isdigit(szIn[1]))
        {
            return FALSE;
        }

        *szOut++ = szIn[1];
        *szOut++ = szIn[0];

        szIn+=2;
    }

    if ((ulNum%2) == 1)
    {
        if (!isdigit(szIn[0]))
        {
            return FALSE;
        }

        *szOut++ = 'F';
        *szOut++ = *szIn;
    }

    *szOut = '\0';

    return TRUE;
}

/*******************************************************************************
* 函数名称: gos_bcd_to_string
* 功    能: BCD码转换成字符串
* 函数类型:
* 参    数:
* 参数名称          类型                    输入/输出       描述
* 函数返回:  无
* 说    明:  无
*******************************************************************************/
BOOL gos_bcd_to_string(UINT8 *pucBCD, UINT32 ulBCDLen, CHAR *szString)
{
    UINT32      i;
    UINT8       ucBCD1;
    UINT8       ucBCD2;

    if (NULL == pucBCD || NULL == szString)
    {
        return FALSE;
    }

    for (i=0; i<ulBCDLen; i++)
    {
        ucBCD1 = pucBCD[i] & 0x0f;
        ucBCD2 = pucBCD[i] >> 4;
        if (ucBCD2 == 0x0f)
        {
            if (ucBCD1 != 0x0f)
            {
                *szString++ = BCD_TO_CHAR(ucBCD1);
            }
            break;
        }

        *szString++ = BCD_TO_CHAR(ucBCD1);
        *szString++ = BCD_TO_CHAR(ucBCD2);
    }

    *szString = '\0';

    return TRUE;
}

/*******************************************************************************
* 函数名称: gos_bcd_to_digit_string
* 功    能: BCD码转换成字符串(不包含F)
* 函数类型:
* 参    数:
* 参数名称          类型                    输入/输出       描述
* 函数返回:  无
* 说    明:  无
*******************************************************************************/
INT32 gos_bcd_to_digit_string(UINT8 *pucBCD, UINT32 ulBCDLen, CHAR *szString)
{
    static BOOL bInit = FALSE;
    static CHAR aacBcd[256][4];
    UINT32      i, j;
    CHAR        *szBcd;
    CHAR        acEnd[4] = {'f','f',0,0};

    if (!bInit)
    {
        bInit = TRUE;
        memset(aacBcd, 0, sizeof(aacBcd));

        // 0x00 - 0x99
        for (i=0; i<=9; i++)
        {
            for (j=0; j<=9; j++)
            {
                aacBcd[(i<<4)|j][0] = j+'0';
                aacBcd[(i<<4)|j][1] = i+'0';
            }
        }

        // f0 - f9
        i = 0x0f;
        for (j=0; j<=9; j++)
        {
            aacBcd[(i<<4)|j][0] = j+'0';
        }

        // ff
        aacBcd[0xff][0] = 'f';
        aacBcd[0xff][1] = 'f';
    }

    j = 0;
    for (i=0; i<ulBCDLen; i++)
    {
        szBcd = &aacBcd[*pucBCD++][0];
        if (GET_INT(szBcd) == 0)
        {
            return -1;
        }

        if (GET_INT(szBcd) == GET_INT(acEnd))
        {
            break;
        }

        if (szBcd[1] == '\0')
        {
            SET_SHORT(szString, GET_SHORT(szBcd));
            return j+1;
        }

        SET_SHORT(szString, GET_SHORT(szBcd));
        j += 2;
        szString += 2;
    }

    *szString = '\0';

    return j;
}

/*******************************************************************************
* 函数名称: gos_digot_to_bcd
* 功    能: 数字字符串转换成BCD码
* 函数类型:
* 参    数:
* 参数名称          类型                    输入/输出       描述
* 函数返回:  无
* 说    明:  无
*******************************************************************************/
BOOL gos_digit_to_bcd(const CHAR *szString, UINT8 *pucBCD, UINT32 ulBCDLen)
{
    UINT32      i;
    UINT8       ucValue1;
    UINT8       ucValue2;

    if (NULL == pucBCD || NULL == szString)
    {
        return FALSE;
    }

    for (i=0; i<ulBCDLen; i++)
    {
        if (*szString == '\0')
        {
            break;
        }

        ucValue1 = DIGIT_CHAR_TO_INT(*szString);
        if (ucValue1 > 9)
        {
            memset(pucBCD-i, 0xff, ulBCDLen);
            return FALSE;
        }

        if (*(++szString) == '\0')
        {
            i++;
            *pucBCD++ = 0xf0 | ucValue1;
            break;
        }

        ucValue2 = DIGIT_CHAR_TO_INT(*szString);
        if (ucValue2 > 9)
        {
            memset(pucBCD-i, 0xff, ulBCDLen);
            return FALSE;
        }

        szString++;
        *pucBCD++ = (ucValue2<<4) + ucValue1;
    }

    if (i < ulBCDLen)
    {
        memset(pucBCD, 0xff, ulBCDLen-i);
    }

    return TRUE;
}

BOOL gos_get_ip(const CHAR *szIP, UINT8* pucIP)
{
    INT32   i;
    INT32   aiTmp[4] = {-1, -1, -1, -1};

    if (!szIP)
    {
        return FALSE;
    }

    sscanf(szIP, IP_FMT, &aiTmp[0],&aiTmp[1],&aiTmp[2],&aiTmp[3]);

    for (i=0; i<4; i++)
    {
        if (aiTmp[i] > 0xff || aiTmp[i] < 0)
        {
            memset(pucIP, 0, 4);
            return FALSE;
        }

        pucIP[i] = (UINT8)aiTmp[i];
    }

    return TRUE;
}

BOOL gos_get_short(const CHAR *szPort, UINT16 *pusValue)
{
    INT32   iPort;

    if (!gos_atoi(szPort, &iPort))
    {
        return FALSE;
    }

    if (iPort < 0 || iPort > 0xffff)
    {
        return FALSE;
    }

    *pusValue = (UINT16)iPort;

    return TRUE;
}

BOOL gos_get_ip_port(const CHAR *szAddr, UINT8* pucIP, UINT16 *pusPort)
{
    CHAR    acAddr[32];
    CHAR    *szPort;

    if (!szAddr)
    {
        return FALSE;
    }

    *pusPort = 0;

    memset(acAddr, 0, sizeof(acAddr));
    strncpy(acAddr, szAddr, sizeof(acAddr)-1);

    szPort = strchr(acAddr, ':');
    if (szPort)
    {
        *szPort++ = '\0';
        if (!gos_get_short(szPort, pusPort))
        {
            return FALSE;
        }
    }

    return gos_get_ip(acAddr, pucIP);
}

BOOL gos_get_mac(const CHAR *szMac, UINT8* pucMac)
{
    INT32   i;
    INT32   aiTmp[6] = {-1, -1, -1, -1, -1, -1};

    sscanf(szMac, GOS_MAC_FMT, &aiTmp[0],&aiTmp[1],&aiTmp[2],&aiTmp[3],&aiTmp[4],&aiTmp[5]);

    for (i=0; i<6; i++)
    {
        if (aiTmp[i] > 0xff || aiTmp[i] < 0)
        {
            memset(pucMac, 0, 6);
            return FALSE;
        }

        pucMac[i] = (UINT8)aiTmp[i];
    }

    return TRUE;
}

VOID gos_get_curr_path(CHAR *szCurrPath, INT32 iLen)
{
    if( getcwd( szCurrPath, iLen) == NULL )
    {
        szCurrPath[0] = '\0';
    }
}

/**
 * @brief           获取当前工作目录绝对路径
 * @return          CHAR* 返回获取到的路径
 * @author          tuzhenglin(tuzhenglin@gbcom.com.cn)
 * @note
 */
CHAR* gos_get_root_path(VOID)
{
    static BOOL     bGetPath = FALSE;
    static CHAR     acPath[512];
    INT32           iPathLen = 0;

    if (!bGetPath)
    {
        memset(acPath, 0, sizeof(acPath));

#ifdef _OSWIN32_
        if (GetModuleFileName(NULL, acPath, sizeof(acPath) - 1) == 0)
#else
        iPathLen = readlink("/proc/self/exe", acPath, sizeof(acPath) - 1);
        if (iPathLen > 0 && iPathLen < (sizeof(acPath) - 1))
        {
            dirname(acPath);
        }
        else
        {
            if (getcwd(acPath, sizeof(acPath) - 1) == NULL)
            {
                iPathLen = -1;
            }
            else
            {
                iPathLen = strlen(acPath);
            }
        }

        if (iPathLen <= 0)
#endif
        {
            acPath[0] = '\0';
        }

        iPathLen = strlen(acPath);
#ifdef _OSWIN32_
        // 去除可执行文件名后缀
        --iPathLen;
        for ( ; iPathLen>0; iPathLen--)
        {
            if (acPath[iPathLen] == '\\' ||
                acPath[iPathLen] == '/' )
            {
                acPath[iPathLen] = '\0';
                break;
            }
        }
#endif

        gos_format_path(acPath);

        bGetPath = TRUE;
    }

    return acPath;
}

UINT32 gos_get_pid()
{
#ifdef _OSWIN32_
    return GetCurrentProcessId();
#else
    return getpid();
#endif
}

BOOL gos_get_proc_full_name(CHAR *szProcName)
{
#ifdef _OSWIN32_
    CHAR    acPath[256];

    if (GetModuleFileName(NULL, acPath, sizeof(acPath)-1) == 0)
    {
        return FALSE;
    }

    strcpy(szProcName, acPath);

    return TRUE;
#else
    pid_t       pid;
    CHAR        acLine[1024];
    CHAR        acCmd[256];
    CHAR        *szLine;
    CHAR        *szFileName=NULL;
    CHAR        *szPID = acLine;
    CHAR        *szCmd = NULL;
    CHAR        *szTmp;
    FILE        *fp;
    CHAR        acPID[32];
    UINT32      ulLen = 0;

    if (!szProcName)
    {
        return FALSE;
    }

    pid = getpid();
    if (0 == pid)
    {
        return FALSE;
    }

    sprintf(acPID, "%d", pid);

#ifdef _BUSYBOX_
    sprintf(acCmd, "ps 2>/dev/null");
#else
    sprintf(acCmd, "ps -q %d -ef 2>/dev/null", pid);
#endif

    fp = popen(acCmd, "r");
    if ((FILE *)0 == fp)
    {
        return FALSE;
    }

    /*
    PID TTY          TIME CMD
    2364 ?        00:00:00 gnome-session
    */

    /* read the header line */
    if (NULL == fgets(acLine, sizeof(acLine), fp))
    {
        pclose(fp);
        return FALSE;
    }

    /* figure out where the command name is from the column headings.
    * (BSD-ish machines put the COMMAND in the 5th column, while SysV
    * seems to put CMD or COMMAND in the 4th column.)
    */
    for (szLine = acLine; ; szLine = NULL)
    {
        if (NULL == (szFileName = strtok(szLine, " \t\n")))
        {
            pclose(fp);
            return FALSE;
        }

        if (0 == strcmp("COMMAND", szFileName) || 0 == strcmp("CMD", szFileName))
        { /*  we found the COMMAND column */
            szCmd = szFileName;
            break;
        }
    }

    if (!szPID || !szCmd)
    {
        pclose(fp);
        return FALSE;
    }

    /* read the ps(1) output line */
    if (NULL == fgets(acLine, sizeof(acLine), fp))
    {
        pclose(fp);
        return FALSE;
    }

    while(1)
    {
        if (NULL == fgets(acLine, sizeof(acLine), fp))
        {
            pclose(fp);
            return FALSE;
        }

        // 11017 root      2488 S    ./sa
#ifdef _BUSYBOX_
        szPID = gos_left_trim_string(acLine);
        ulLen = 0;
        for (szTmp=szPID; *szTmp; szTmp++, ulLen++)
        {
            if (*szTmp == ' ' || *szTmp == '\t' || *szTmp == '\n')
            {
                break;
            }
        }

        if (ulLen != strlen(acPID) ||
            strncmp(szPID, acPID, ulLen) != 0)
        {
            continue;
        }
#endif

        /* grab the "word" underneath the command heading... */
        if (NULL == (szFileName = strtok(szCmd, " \t\n")))
        {
            pclose(fp);
            return FALSE;
        }

        pclose(fp);

        for (szTmp=szFileName+strlen(szFileName)-1; szTmp>=szFileName; szTmp--)
        {
            if (*szTmp == ' ')
            {
                *szTmp = '\0';
            }
        }

        strcpy(szProcName, szFileName);

        return TRUE;
    }
#endif
}

BOOL gos_get_proc_name(CHAR *szProcName, BOOL bResvPostfix)
{
#ifdef _OSWIN32_
    CHAR    acPath[256];
    INT32   iLen;

    if (GetModuleFileName(NULL, acPath, sizeof(acPath)-1) == 0)
    {
        return FALSE;
    }

    iLen = strlen(acPath);
    --iLen;
    for ( ; iLen>0; iLen--)
    {
        if (acPath[iLen] == '\\' ||
            acPath[iLen] == '/' )
        {
            break;
        }
    }

    if (iLen <= 0)
    {
        return FALSE;
    }

    strcpy(szProcName, &acPath[iLen+1]);
    if (!bResvPostfix)
    {
        iLen = strlen(szProcName);
        --iLen;
        for ( ; iLen>0; iLen--)
        {
            if (szProcName[iLen] == '.')
            {
                szProcName[iLen] = '\0';
                break;
            }
        }
    }

    return TRUE;
#else
    pid_t       pid;
    CHAR        acLine[1024];
    CHAR        acCmd[256];
    CHAR        *szLine;
    CHAR        *szFileName=NULL;
    CHAR        *szPID = acLine;
    CHAR        *szCmd = NULL;
    CHAR        *szTmp;
    FILE        *fp;
    CHAR        acPID[32];
    UINT32      ulLen = 0;

    if (!szProcName)
    {
        return FALSE;
    }

    pid = getpid();
    if (0 == pid)
    {
        return FALSE;
    }

    sprintf(acPID, "%d", pid);

#ifdef _BUSYBOX_
    sprintf(acCmd, "ps 2>/dev/null");
#else
    sprintf(acCmd, "ps -p %d 2>/dev/null", pid);
#endif

    fp = popen(acCmd, "r");
    if ((FILE *)0 == fp)
    {
        return FALSE;
    }

    /*
    PID TTY          TIME CMD
    2364 ?        00:00:00 gnome-session
    */

    /* read the header line */
    if (NULL == fgets(acLine, sizeof(acLine), fp))
    {
        pclose(fp);
        return FALSE;
    }

    /* figure out where the command name is from the column headings.
    * (BSD-ish machines put the COMMAND in the 5th column, while SysV
    * seems to put CMD or COMMAND in the 4th column.)
    */
    for (szLine = acLine; ; szLine = NULL)
    {
        if (NULL == (szFileName = strtok(szLine, " \t\n")))
        {
            pclose(fp);
            return FALSE;
        }

        if (0 == strcmp("COMMAND", szFileName) || 0 == strcmp("CMD", szFileName))
        { /*  we found the COMMAND column */
            szCmd = szFileName;
            break;
        }
    }

    if (!szPID || !szCmd)
    {
        pclose(fp);
        return FALSE;
    }

    /* read the ps(1) output line */
    while(1)
    {
        if (NULL == fgets(acLine, sizeof(acLine), fp))
        {
            pclose(fp);
            return FALSE;
        }

#ifdef _BUSYBOX_
        // 11017 root      2488 S    ./sa
        szPID = gos_left_trim_string(acLine);
        ulLen = 0;
        for (szTmp=szPID; *szTmp; szTmp++, ulLen++)
        {
            if (*szTmp == ' ' || *szTmp == '\t' || *szTmp == '\n')
            {
                break;
            }
        }

        if (ulLen != strlen(acPID) ||
            strncmp(szPID, acPID, ulLen) != 0)
        {
            continue;
        }
#endif

        /* grab the "word" underneath the command heading... */
        if (NULL == (szFileName = strtok(szCmd, " \t\n")))
        {
            pclose(fp);
            return FALSE;
        }

        pclose(fp);

        for (szTmp=szFileName+strlen(szFileName)-1; szTmp>=szFileName; szTmp--)
        {
            if (*szTmp == ' ')
            {
                *szTmp = '\0';
            }
            else if (*szTmp == '/' )
            {
                szFileName = szTmp+1;
                break;
            }
        }

        strcpy(szProcName, szFileName);

        return TRUE;
    }
#endif
}

CHAR* gos_get_file_postfix(const CHAR *szFile)
{
    CHAR    *szTmp;

    if (!szFile)
    {
        return (CHAR*)"";
    }

    szTmp = (CHAR*)szFile + strlen(szFile);
    for (; szTmp>=szFile; szTmp--)
    {
        if (*szTmp == '.')
        {
            return szTmp+1;
        }
    }

    return (CHAR*)"";
}

#ifdef _OSWIN32_

#define SystemBasicInformation          0
#define SystemPerformanceInformation    2
#define SystemTimeInformation           3

#define Li2Double(x)   ((DOUBLE)((x).HighPart)*4.294967296E9 + (DOUBLE)((x).LowPart))

typedef struct
{
    DWORD       dwUnknown1;
    ULONG       uKeMaximumIncrement;
    ULONG       uPageSize;
    ULONG       uMmNumberOfPhysicalPages;
    ULONG       uMmLowestPhysicalPage;
    ULONG       uMmHighestPhysicalPage;
    ULONG       uAllocationGranularity;
    PVOID       pLowestUserAddress;
    PVOID       pMmHighestUserAddress;
    ULONG       uKeActiveProcessors;
    BYTE        bKeNumberProcessors;
    BYTE        bUnknown2;
    WORD        wUnknown3;
}SYSTEM_BASIC_INFORMATION;

typedef struct
{
    LARGE_INTEGER   liIdleTime;
    DWORD           dwSpare[76];
}SYSTEM_PERFORMANCE_INFORMATION;

typedef struct
{
    LARGE_INTEGER   liKeBootTime;
    LARGE_INTEGER   liKeSystemTime;
    LARGE_INTEGER   liExpTimeZoneBias;
    ULONG           uCurrentTimeZoneId;
    DWORD           dwReserved;
}SYSTEM_TIME_INFORMATION;

typedef LONG (__stdcall *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);
typedef BOOL (__stdcall *GET_SYSTEM_TIMES)(LPFILETIME,LPFILETIME,LPFILETIME);

#endif

#ifdef _OSWIN32_
INT64 SubstractFileTime(FILETIME PreTime, FILETIME LastTime)
{
    UINT64 P = ((UINT64)PreTime.dwHighDateTime << 32) | PreTime.dwLowDateTime ;
    UINT64 L = ((UINT64)LastTime.dwHighDateTime << 32) | LastTime.dwLowDateTime ;

    return (INT64)(L - P);
}
#endif

INT32 gos_get_cpu_usage (VOID)
{
#ifdef _OSWIN32_

    static GET_SYSTEM_TIMES pfGetSystemTimes = NULL;
    INT32   iUsage;
    INT64   i64Idle;
    INT64   i64Total;

    FILETIME preidleTime, prekernelTime, preuserTime;
    FILETIME idleTime, kernelTime, userTime;

    if (!pfGetSystemTimes)
    {
        pfGetSystemTimes = (GET_SYSTEM_TIMES)GetProcAddress(GetModuleHandle("kernel32"), "GetSystemTimes");
        if (!pfGetSystemTimes)
        {
            return 0;
        }
    }

    pfGetSystemTimes(&preidleTime, &prekernelTime, &preuserTime);

    gos_sleep_1ms(100);

    pfGetSystemTimes(&idleTime, &kernelTime, &userTime);

    i64Idle = SubstractFileTime(preidleTime, idleTime);
    i64Total = SubstractFileTime(prekernelTime, kernelTime) + SubstractFileTime(preuserTime, userTime);

    if(0 == i64Total)
    {
        return 0;
    }

    iUsage = 100*(i64Total - i64Idle)/i64Total;//（总的时间-空闲时间）/总的时间=占用cpu的时间就是使用率

    return iUsage;

#else
    FILE            *fp = NULL;
    static BOOL     bFirst = TRUE;
    static CHAR     acBuf[512];
    UINT64          au64Frame[7];
    UINT64          au64Curr[7];
    static UINT64   au64Prev[7];
    INT32           iUsage = 0;
    UINT64          u64Total = 0;
    UINT32          i;

    fp = fopen("/proc/stat", "r");

    if (NULL == fp)
    {
        return 0;
    }

    if (!fgets(acBuf, sizeof(acBuf), fp))
    {
        fclose(fp);
        return 0;
    }

    /* 替换为串行化文件操作*/
    fclose(fp);

    sscanf(acBuf, "cpu %Lu %Lu %Lu %Lu %Lu %Lu %Lu",
        &au64Curr[0],&au64Curr[1],&au64Curr[2],&au64Curr[3],
        &au64Curr[4],&au64Curr[5],&au64Curr[6]);

    if (bFirst)
    {
        bFirst = FALSE;
        memset(au64Prev, 0, sizeof(au64Prev));
    }
    else
    {
        for (i=0; i<7; i++)
        {
            au64Frame[i] = au64Curr[i] - au64Prev[i];
            u64Total += au64Frame[i];
        }

        if (u64Total < 1)
        {
            u64Total = 1;
        }

        //iUsage = (INT32)((frme.u + frme.s + frme.n)*100/tot_frme);
        iUsage = (INT32)((au64Frame[0]+au64Frame[1]+au64Frame[2])*100/u64Total);
    }

    memcpy(au64Prev, au64Curr, sizeof(au64Prev));

    return iUsage;
#endif
}

#ifdef _OSWIN32_
#if _MSC_VER < 1400
#ifndef _BCB_
typedef struct _MEMORYSTATUSEX
{
    DWORD dwLength;
    DWORD dwMemoryLoad;
    DWORDLONG ullTotalPhys;
    DWORDLONG ullAvailPhys;
    DWORDLONG ullTotalPageFile;
    DWORDLONG ullAvailPageFile;
    DWORDLONG ullTotalVirtual;
    DWORDLONG ullAvailVirtual;
    DWORDLONG ullAvailExtendedVirtual;
} MEMORYSTATUSEX, *LPMEMORYSTATUSEX;
#endif
#endif

typedef BOOL (WINAPI * Fun_GlobalMemoryStatusEx)(LPMEMORYSTATUSEX lpBuffer);
#endif
/*************************************************************************
* 函数名称: gos_get_mem_usage
* 功    能: 获取内存统计
* 参    数:
* 参数名称          输入/输出               描述
* 函数返回: 秒数
* 说    明:
  struct sysinfo
  {
    long uptime;                     Seconds since boot
    unsigned long loads[3];          1, 5, and 15 minute load averages
    unsigned long totalram;          Total usable main memory size
    unsigned long freeram;           Available memory size
    unsigned long sharedram;         Amount of shared memory
    unsigned long bufferram;         Memory used by buffers
    unsigned long totalswap;         Total swap space size
    unsigned long freeswap;          swap space still available
    unsigned short procs;            Number of current processes
    unsigned long totalhigh;         Total high memory size
    unsigned long freehigh;          Available high memory size
    unsigned int mem_unit;           Memory unit size in bytes
    char _f[20-2*sizeof(long)-sizeof(int)];  Padding: libc5 uses this..
  };
*************************************************************************/
VOID gos_get_mem_usage(INT64 *pi64TotalMem, INT64 *pi64FreeMem)
{
#ifdef _OSWIN32_
    MEMORYSTATUSEX      stMeminfo;
    static Fun_GlobalMemoryStatusEx     GlobalMemoryStatusEx_ = NULL;
    static HINSTANCE hinstLib = NULL;

    if (NULL == hinstLib)
    {
        hinstLib = LoadLibrary("kernel32.dll");
        if (hinstLib != NULL)
        {
            GlobalMemoryStatusEx_ = (Fun_GlobalMemoryStatusEx) GetProcAddress(hinstLib, "GlobalMemoryStatusEx");
        }
    }

    if(NULL == GlobalMemoryStatusEx_)
    {
        *pi64TotalMem = 0;
        *pi64FreeMem  = 0;
    }
    else
    {
        memset(&stMeminfo, 0, sizeof(stMeminfo));
        stMeminfo.dwLength = sizeof(stMeminfo);

        GlobalMemoryStatusEx_(&stMeminfo);

        *pi64TotalMem = stMeminfo.ullTotalPhys;
        *pi64FreeMem  = stMeminfo.ullAvailPhys;
    }
#else
    typedef struct MEMPACKED         //定义一个mem occupy的结构体
    {
        CHAR   acMemTotal[20];      //定义一个char类型的数组名name有20个元素
        UINT32 ulMemTotal;
        CHAR   acMemFree[20];
        UINT32 ulMemFree;
        CHAR   acBuffers[20];
        UINT32 ulBuffers;
        CHAR   acCached[20];
        UINT32 ulCached;
    }MEM_OCCUPY;

    FILE *pstFile = NULL;
    CHAR buff[256];
    CHAR* szRet;
    MEM_OCCUPY stMemOccupy;

    memset(&stMemOccupy, 0, sizeof(stMemOccupy));

    pstFile = popen("cat /proc/meminfo", "r");
    if (pstFile == NULL)
    {
        return;
    }

    /*
MemTotal:        7889108 kB
MemFree:         6306324 kB
MemAvailable:    6489416 kB
Buffers:            2260 kB
Cached:           399572 kB
*/

    szRet = fgets(buff, sizeof(buff), pstFile);
    if (!szRet)
    {
        goto end;
    }
    sscanf(buff, "%s %u ", stMemOccupy.acMemTotal, &stMemOccupy.ulMemTotal);

    szRet = fgets(buff, sizeof(buff), pstFile);
    if (!szRet)
    {
        goto end;
    }
    sscanf(buff, "%s %u ", stMemOccupy.acMemFree, &stMemOccupy.ulMemFree);

    szRet = fgets(buff, sizeof(buff), pstFile);
    if (strstr(szRet, "MemAvailable"))
    {
        szRet = fgets(buff, sizeof(buff), pstFile);
    }

    if (!szRet)
    {
        goto end;
    }
    sscanf(buff, "%s %u ", stMemOccupy.acBuffers, &stMemOccupy.ulBuffers);

    szRet = fgets(buff, sizeof(buff), pstFile);
    if (!szRet)
    {
        goto end;
    }
    sscanf(buff, "%s %u ", stMemOccupy.acCached, &stMemOccupy.ulCached);

end:
    pclose(pstFile);     //关闭文件fd

    *pi64TotalMem = stMemOccupy.ulMemTotal;
    *pi64FreeMem  = stMemOccupy.ulBuffers + stMemOccupy.ulCached + stMemOccupy.ulMemFree;
#endif
}

/*************************************************************************
* 函数名称: gos_get_disk_usage
* 功    能: 获取磁盘统计
* 参    数:
* 参数名称          输入/输出               描述
* 函数返回: 秒数
* 说    明:
  struct statvfs
  {
    unsigned long  f_bsize;    file system block size
    unsigned long  f_frsize;   fragment size
    fsblkcnt_t     f_blocks;   size of fs in f_frsize units
    fsblkcnt_t     f_bfree;    free blocks
    fsblkcnt_t     f_bavail;   free blocks for non-root
    fsfilcnt_t     f_files;    inodes
    fsfilcnt_t     f_ffree;    free inodes
    fsfilcnt_t     f_favail;   free inodes for non-root
    unsigned long  f_fsid;     file system ID
    unsigned long  f_flag;     mount flags
    unsigned long  f_namemax;  maximum filename length
};
*************************************************************************/
VOID gos_get_disk_usage_ex(CHAR *szPath, INT64 *pi64TotalSize, INT64 *pi64FreeSize)
{
#ifdef _OSWIN32_
    ULARGE_INTEGER  liFreeBytesAvailable = {0};
    ULARGE_INTEGER  liTotalNumberOfBytes = {0};
    ULARGE_INTEGER  liTotalNumberOfFreeBytes = {0};
    BOOL            bRet;

    bRet = GetDiskFreeSpaceEx(szPath,
                              &liFreeBytesAvailable,
                              &liTotalNumberOfBytes,
                              &liTotalNumberOfFreeBytes);
    if (bRet)
    {
        *pi64TotalSize = liTotalNumberOfBytes.QuadPart;
        *pi64FreeSize = liTotalNumberOfFreeBytes.QuadPart;
    }

#else
    struct statvfs info;

    if( 0 == statvfs(szPath, &info))
    {
        *pi64TotalSize = info.f_bsize;
        *pi64FreeSize  = info.f_bsize;

        *pi64TotalSize *= info.f_blocks;
        *pi64FreeSize  *= info.f_bfree;
    }

#endif
}

VOID gos_get_disk_usage(INT64 *pi64TotalSize, INT64 *pi64FreeSize)
{
#ifdef _OSWIN32_
    CHAR    *szPath = gos_get_root_path();
#else
    CHAR    *szPath = (CHAR*)"/";
#endif

    gos_get_disk_usage_ex(szPath, pi64TotalSize, pi64FreeSize);
}

BOOL gos_get_disk_model(const CHAR *szDev, CHAR *szModel, UINT32 ulMaxModelLen)
{
#ifdef _OSWIN32_

    strcpy(szModel, "(unknow)");

    return TRUE;
#else
    // cmd: hdparm -i /dev/sda1 |grep Model
    // result: Model=GB0250EAFJF  , FwRev=HPGB  , SerialNo=9SF1SK2L
    CHAR    acBuf[256];
    CHAR    acCmd[256];
    CHAR    *szTmpFile = (CHAR*)"/tmp/disk_model.tmp";
    CHAR    *szLine = NULL;
    UINT32  i;
    FILE    *fp;

    sprintf(acCmd, "hdparm -i %s |grep Model >%s", szDev, szTmpFile);
    gos_system(acCmd);

    fp = fopen(szTmpFile, "r+t");
    if (!fp)
    {
        strcpy(szModel, "(unknow)");
        return FALSE;
    }

    while(!feof(fp))
    {
        memset(acBuf, 0, sizeof(acBuf));
        if (fgets(acBuf, sizeof(acBuf), fp) == 0)
        {
            break;
        }

        if (acBuf[sizeof(acBuf)-1] != 0)
        {
            continue;
        }

        szLine = strstr(acBuf, "Model=");
        if (szLine == NULL)
        {
            continue;
        }

        szLine += strlen("Model=");
        szLine = gos_left_trim_string(szLine);
        for (i=0; i<ulMaxModelLen; i++)
        {
            if (gos_isspace(szLine[i]))
            {
                break;
            }

            szModel[i] = szLine[i];
        }

        szModel[i] = '\0';
        fclose(fp);
        return TRUE;
    }

    fclose(fp);

    return FALSE;
#endif
}

#ifdef _OSWIN32_

#define MAX_ADAPTER_NAME_LENGTH         256
#define MAX_ADAPTER_DESCRIPTION_LENGTH  128
#define MAX_ADAPTER_ADDRESS_LENGTH      8

typedef struct
{
    char String[4 * 4];
} IP_ADDRESS_STRING, *PIP_ADDRESS_STRING, IP_MASK_STRING, *PIP_MASK_STRING;

typedef struct _IP_ADDR_STRING
{
    struct _IP_ADDR_STRING* Next;
    IP_ADDRESS_STRING IpAddress;
    IP_MASK_STRING IpMask;
    DWORD Context;
} IP_ADDR_STRING, *PIP_ADDR_STRING;

typedef struct _IP_ADAPTER_INFO
{
    struct _IP_ADAPTER_INFO* Next;
    DWORD ComboIndex;
    char AdapterName[MAX_ADAPTER_NAME_LENGTH + 4];
    char Description[MAX_ADAPTER_DESCRIPTION_LENGTH + 4];
    UINT AddressLength;
    BYTE Address[MAX_ADAPTER_ADDRESS_LENGTH];
    DWORD Index;
    UINT Type;
    UINT DhcpEnabled;
    PIP_ADDR_STRING CurrentIpAddress;
    IP_ADDR_STRING IpAddressList;
    IP_ADDR_STRING GatewayList;
    IP_ADDR_STRING DhcpServer;
    BOOL HaveWins;
    IP_ADDR_STRING PrimaryWinsServer;
    IP_ADDR_STRING SecondaryWinsServer;
    time_t LeaseObtained;
    time_t LeaseExpires;
} IP_ADAPTER_INFO, *PIP_ADAPTER_INFO;

typedef DWORD (__stdcall *PGAINFO)(PIP_ADAPTER_INFO,PULONG);
#endif

BOOL gos_get_adapter_info(GOS_ADAPTER_INFO_T *pstAdapterInfo, UINT32 ulMaxNum, UINT32 *pulNum)
{
#ifdef _OSWIN32_
    UINT32              ulError;
    PIP_ADAPTER_INFO    pAdapterInfo = NULL;
    PIP_ADAPTER_INFO    pCurrAdapterInfo = NULL;
    UINT32              ulLen = 0;
    static HMODULE      hInst = NULL;
    static PGAINFO      pGAInfo = NULL;
    IP_ADDR_STRING      *pstIpAddr;

    if (!hInst)
    {
        hInst =  LoadLibrary("iphlpapi.dll");
        if(!hInst)
        {
            return FALSE;
        }
    }

    if (!pGAInfo)
    {
        pGAInfo = (PGAINFO)GetProcAddress(hInst,"GetAdaptersInfo");
        if (!pGAInfo)
        {
            return FALSE;
        }
    }

    gos_init_socket();

    //获取本机所有网卡信息
    ulError = pGAInfo(pAdapterInfo, (PULONG)&ulLen);
    if (ulLen <= 0)
    {
        return FALSE;
    }

    pAdapterInfo = (PIP_ADAPTER_INFO)malloc(ulLen);
    if (NULL == pAdapterInfo)
    {
        return FALSE;
    }

    ulError = pGAInfo(pAdapterInfo, (PULONG)&ulLen);
    if (0 != ulError || ulLen <= 0)
    {
        free(pAdapterInfo);
        return FALSE;
    }

    for (pCurrAdapterInfo = pAdapterInfo;
         pCurrAdapterInfo != NULL;
         pCurrAdapterInfo = pCurrAdapterInfo->Next)
    {
        memset(pstAdapterInfo, 0, sizeof(GOS_ADAPTER_INFO_T));

//      pstAdapterInfo->ulIndex = pCurrAdapterInfo->Index;
        strncpy(pstAdapterInfo->acName, pCurrAdapterInfo->AdapterName, sizeof(pstAdapterInfo->acName)-1);
        strncpy(pstAdapterInfo->acDesc, pCurrAdapterInfo->Description, sizeof(pstAdapterInfo->acDesc)-1);

        for(pstIpAddr=&pCurrAdapterInfo->IpAddressList; pstIpAddr; pstIpAddr=pstIpAddr->Next)
        {
            gos_get_ip(&pstIpAddr->IpAddress.String[0], pstAdapterInfo->aucAddr);
            memcpy(pstAdapterInfo->aucMac, pCurrAdapterInfo->Address, sizeof(pstAdapterInfo->aucMac));

            pstAdapterInfo++;
            (*pulNum) ++;
            if ((*pulNum) >= ulMaxNum)
            {
                goto end;
            }

            memcpy(pstAdapterInfo, pstAdapterInfo-1, sizeof(GOS_ADAPTER_INFO_T));
        }
    }

end:
    free(pAdapterInfo);

    return TRUE;
#else
    UINT32          i;
    SOCKET          stSock;
    struct ifconf   stConf;
    CHAR            acBuf[512];
    struct ifreq    *ifreq;
    UINT8           *pucTmpIP;
    UINT8           aucLocalIP[4] = {127,0,0,1};

    //初始化ifconf
    stConf.ifc_len = sizeof(acBuf);
    stConf.ifc_buf = acBuf;

    if((stSock = socket(AF_INET, SOCK_DGRAM, 0))<0)
    {
        return FALSE;
    }

    ioctl(stSock, SIOCGIFCONF, &stConf);    //获取所有接口信息

    //接下来一个一个的获取IP地址
    ifreq = (struct ifreq*)acBuf;
    for(i=(stConf.ifc_len/sizeof(struct ifreq)); i>0; i--, ifreq++)
    {
        pucTmpIP = (UINT8*)&(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr.s_addr);
        if (memcmp(pucTmpIP, aucLocalIP, 4) == 0)  // 排除127.0.0.1，继续下一个
        {
            continue;
        }

        /* IP */
        memcpy(pstAdapterInfo->aucAddr, pucTmpIP, 4);

        /* Name */
        strncpy(pstAdapterInfo->acName, ifreq->ifr_name, sizeof(pstAdapterInfo->acName)-1);
        pstAdapterInfo->acName[sizeof(pstAdapterInfo->acName)-1] = '\0';

        // if status
        if (ioctl(stSock, SIOCGIFFLAGS, (char *)ifreq) == 0)
        {
            if (ifreq->ifr_flags & IFF_UP)
            {
                pstAdapterInfo->ucIsUp = TRUE;
            }
            else
            {
                pstAdapterInfo->ucIsUp = FALSE;
            }
        }

        // MAC addr
        if (ioctl(stSock, SIOCGIFHWADDR, (char *)ifreq) == 0)
        {
            memcpy(pstAdapterInfo->aucMac, ifreq->ifr_hwaddr.sa_data, sizeof(pstAdapterInfo->aucMac));
        }

        pstAdapterInfo++;
        (*pulNum) ++;
        if ((*pulNum) >= ulMaxNum)
        {
            break;
        }
    }

    close(stSock);

    return TRUE;
#endif
}

BOOL gos_get_apapter_status(UINT8 *pucMacAddr, UINT8 *pucIsAdapterUp)
{
#ifdef _OSWIN32_
    //CHAR    acAdapterName[256];
    CHAR    *szBuf = NULL;
    UINT32  ulLen;
    CHAR    acCmd[256];
    CHAR    acTmpFile[512];
    CHAR    *szLine = NULL;
    CHAR    acMac[32];
    FILE    *fp = NULL;
    BOOL    bRet = FALSE;

    sprintf(acMac, "%02X-%02X-%02X-%02X-%02X-%02X", GOS_MAC_ARG(pucMacAddr));
    sprintf(acTmpFile, "%s/if_status.tmp", gos_get_root_path());
    sprintf(acCmd, "ipconfig /all >%s", acTmpFile);
    gos_system(acCmd);

    fp = fopen(acTmpFile, "rb");
    if (!fp)
    {
        return FALSE;
    }

    fseek(fp, 0, SEEK_END);
    ulLen = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    szBuf = (char*)malloc(ulLen+1);
    if (!szBuf)
    {
        goto end;
    }

    ulLen = fread(szBuf, 1, ulLen, fp);
    szBuf[ulLen] = '\0';
    szLine = strstr(szBuf, acMac);
    if (!szLine)
    {
        goto end;
    }

    bRet = TRUE;
    *pucIsAdapterUp = TRUE;
    while(szLine >= szBuf)
    {
        if (gos_str_like(szLine, "Media disconnected"))
        {
            *pucIsAdapterUp = FALSE;
            goto end;
        }

        if (gos_str_like(szLine, "Ethernet adapter"))
        {
            goto end;
        }

        szLine--;
    }

end:
    if (fp)
    {
        fclose(fp);
    }

    if (szBuf)
    {
        free(szBuf);
    }

    return bRet;
#else

    UINT32          i;
    SOCKET          stSock;
    struct ifconf   stConf;
    CHAR            acBuf[512];
    struct ifreq    *ifreq;
    UINT8           *pucTmpIP;
    UINT8           aucLocalIP[4] = {127,0,0,1};

    //初始化ifconf
    stConf.ifc_len = sizeof(acBuf);
    stConf.ifc_buf = acBuf;

    if((stSock = socket(AF_INET, SOCK_DGRAM, 0))<0)
    {
        return FALSE;
    }

    ioctl(stSock, SIOCGIFCONF, &stConf);    //获取所有接口信息

    //接下来一个一个的获取IP地址
    ifreq = (struct ifreq*)acBuf;
    for(i=(stConf.ifc_len/sizeof(struct ifreq)); i>0; i--, ifreq++)
    {
        pucTmpIP = (UINT8*)&(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr.s_addr);
        if (memcmp(pucTmpIP, aucLocalIP, 4) == 0)  // 排除127.0.0.1，继续下一个
        {
            continue;
        }

        // MAC addr
        if (ioctl(stSock, SIOCGIFHWADDR, (char *)ifreq) == 0)
        {
            if (memcmp(pucMacAddr, ifreq->ifr_hwaddr.sa_data, 6) == 0)
            {
                // if status
                if (ioctl(stSock, SIOCGIFFLAGS, (char *)ifreq) == 0)
                {
                    if (ifreq->ifr_flags & IFF_UP)
                    {
                        *pucIsAdapterUp = TRUE;
                    }
                    else
                    {
                        *pucIsAdapterUp = FALSE;
                    }

                    close(stSock);
                    return TRUE;
                }

                break;
            }
        }
    }

    close(stSock);
    return FALSE;

#endif
}

BOOL gos_is_local_ip(UINT8 *pucAddr)
{
    GOS_ADAPTER_INFO_T  astAdapterInfo[16];
    UINT32              ulAddrNum = 0;
    UINT32              i;

    if (!gos_get_adapter_info(astAdapterInfo, ARRAY_SIZE(astAdapterInfo), &ulAddrNum))
    {
        return FALSE;
    }

    for (i=0; i<ulAddrNum; i++)
    {
        if (memcmp(pucAddr, astAdapterInfo[i].aucAddr, 4) == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}

#ifdef _OSWIN32_
typedef struct _PROCESS_MEMORY_COUNTERS
{
  DWORD cb; // 结构体大小
  DWORD PageFaultCount; // 缺页计数
  SIZE_T PeakWorkingSetSize; // in byte
  SIZE_T WorkingSetSize;
  SIZE_T QuotaPeakPagedPoolUsage;
  SIZE_T QuotaPagedPoolUsage; // in byte
  SIZE_T QuotaPeakNonPagedPoolUsage;
  SIZE_T QuotaNonPagedPoolUsage; // in byte
  SIZE_T PagefileUsage; // in byte
  SIZE_T PeakPagefileUsage;

} PROCESS_MEMORY_COUNTERS,
 *PPROCESS_MEMORY_COUNTERS;


typedef BOOL (__stdcall *PGPROCMEMINFO)(HANDLE,PPROCESS_MEMORY_COUNTERS,DWORD);

#endif

BOOL gos_get_proc_mem_usage(UINT32 *pulMemSize)
{
#ifdef _OSWIN32_
    //return FALSE;
    static HMODULE          hInst = NULL;
    static PGPROCMEMINFO    pGetProcMemInfo = NULL;

    PROCESS_MEMORY_COUNTERS pmc;
    HANDLE                  hProcess;

    if (!hInst)
    {
        hInst =  LoadLibrary("psapi.dll");
        if(!hInst)
        {
            return FALSE;
        }
    }

    if (!pGetProcMemInfo)
    {
        pGetProcMemInfo = (PGPROCMEMINFO)GetProcAddress(hInst, "GetProcessMemoryInfo");
        if (!pGetProcMemInfo)
        {
            return FALSE;
        }
    }

    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, GetCurrentProcessId());
    if (!hProcess)
    {
        return FALSE;
    }

    if (!pGetProcMemInfo(hProcess, &pmc, sizeof(pmc)))
    {
        return FALSE;
    }

    *pulMemSize = pmc.PagefileUsage;
    return TRUE;
#else
    // proc/pid/status :: VmSize:    12312 kB
    CHAR    acBuf[512];
    CHAR    acCmd[128];
    CHAR    *szLine = NULL;
    FILE    *fp;
    INT32   iMemSize = 0;

    sprintf(acCmd, "cat /proc/%d/status |grep VmSize 2>/dev/null", getpid());
    fp = popen(acCmd, "r");
    if (!fp)
    {
        return FALSE;
    }

    while(!feof(fp))
    {
        memset(acBuf, 0, sizeof(acBuf));
        if (fgets(acBuf, sizeof(acBuf), fp) == 0)
        {
            break;
        }

        if (acBuf[sizeof(acBuf)-1] != 0)
        {
            continue;
        }

        szLine = strstr(acBuf, "VmSize:");
        if (szLine == NULL)
        {
            continue;
        }

        szLine += strlen("VmSize:");
        sscanf(szLine, "%d kB", &iMemSize);

        *pulMemSize = ((UINT32)iMemSize)*1024;

        pclose(fp);
        return TRUE;
    }

    pclose(fp);
    return FALSE;
#endif
}

VOID gos_parse_string(CHAR *szIn, CHAR **pszOut, UINT32 *pulOutNum)
{
    if (!pszOut || !pulOutNum)
    {
        return;
    }

    pulOutNum[0] = 0;

    *pszOut++ = szIn;

    while(1)
    {
        if (*szIn == '\0')
        {
            pulOutNum[0] ++;
            break;
        }

        if (gos_isspace(*szIn))
        {
            *szIn++ = '\0';
            szIn = gos_left_trim_string(szIn);
            *pszOut++ = szIn;

            pulOutNum[0] ++;
        }
        else
        {
            szIn++;
        }
    }
}

VOID gos_encrypt_byte(UINT8 *pucByte)
{
    pucByte[0] *= 171;
    pucByte[0] -= 57;
}

VOID gos_decrypt_byte(UINT8 *pucByte)
{
    pucByte[0] += 57;
    pucByte[0] *= 3;
}

VOID gos_encrypt_data(UINT8 *pucData, UINT32 ulDataLen)
{
    UINT32  i;

    for (i=0; i<ulDataLen; i++)
    {
        gos_encrypt_byte(pucData+i);
    }
}

VOID gos_decrypt_data(UINT8 *pucData, UINT32 ulDataLen)
{
    UINT32  i;

    for (i=0; i<ulDataLen; i++)
    {
        gos_decrypt_byte(pucData+i);
    }
}

static const UINT8 g_aucBase64EncodeMap[64] =
{
    'A','B','C','D','E','F','G','H',
    'I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X',
    'Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n',
    'o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3',
    '4','5','6','7','8','9','+','/'
};

static const UINT8 g_aucBase64DecodeMap[128] =
{
    127,127,127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127,127,127,
    127,127,127,62, 127,127,127,63, 52, 53,
    54, 55, 56, 57, 58, 59, 60, 61, 127,127,
    127,64, 127,127,127,0,  1,  2,  3,  4,
    5,  6,  7,  8,  9,  10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 127,127,127,127,127,127,26, 27, 28,
    29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
    39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
    49, 50, 51, 127,127,127,127,127
};

/**
 * @brief           base64 编码
 * @param           src         [in]        源字符串
 * @param           iSlen       [in]        源字符串长度
 * @param           dst         [out]       目标字符串
 * @param           iDlen       [in/out]    作为入参用于判断源字符串
 * @return          BOOL
 * @author          leijie(leijie@gbcom.com.cn)
 * @date            2021-09-01 11:19:02
 * @note
 *
 * @par 示例:
 * @code
void base64_test()
{
    CHAR acSrc[32] = "123456";
    CHAR acDst[32] = {0};
    INT32 iDstLen = 0;
    INT32 iSrcLen = 0;

    iDstLen = sizeof(acDst);
    gos_base64_encode((UINT8 *)acSrc, strlen(acSrc), (UINT8 *)acDst, &iDstLen);

    memset(acSrc, 0, sizeof(acSrc));
    iSrcLen = sizeof(acSrc);
    gos_base64_decode((UINT8 *)acDst, iDstLen, (UINT8 *)acSrc, &iSrcLen);

    assert(strcmp(acSrc, acDst) == 0);
}
 * @endcode
 */
BOOL gos_base64_encode(const UINT8 * src, INT32 iSlen, UINT8 * dst, INT32 *iDlen)
{
    int i, n;
    int c1,c2,c3;
    UINT8 * p;

    if (iSlen == 0)
    {
        return TRUE;
    }

    n = (iSlen << 3) / 6;
    switch( (iSlen << 3) - (n * 6))
    {
        case 2: n += 3; break;
        case 3: n += 2; break;
        default:break;
    }
    //int x = *iDlen;
    if (*iDlen < n + 1)
    {
        *iDlen = n + 1;
        return FALSE;//ERR_BASE64_BUFFER_TOO_SMALL;
    }

    n = (iSlen / 3) * 3;

    for(i = 0, p = dst; i< n; i += 3)
    {
        c1 = *src++;
        c2 = *src++;
        c3 = *src++;

        *p++ = g_aucBase64EncodeMap[(c1 >> 2) & 0x3F];
        *p++ = g_aucBase64EncodeMap[(((c1 & 3) << 4) + (c2 >> 4)) & 0x3F];
        *p++ = g_aucBase64EncodeMap[(((c2 & 15) << 2) + (c3 >> 6)) & 0x3F];
        *p++ = g_aucBase64EncodeMap[c3 & 0x3F];
    }

    if (i < iSlen)
    {
        c1 = *src++;
        c2 = ((i+1) < iSlen) ? *src++ : 0;

        *p++ = g_aucBase64EncodeMap[(c1 >> 2) & 0x3F];
        *p++ = g_aucBase64EncodeMap[(((c1 & 3) << 4) + (c2 >> 4)) & 0x3F];

        if ((i+1) < iSlen)
        {
            *p++ = g_aucBase64EncodeMap[((c2 & 15) << 2) & 0x3F];
        }
        else
        {
            *p++ = '=';
        }
        *p++ = '=';
    }

    *iDlen = (INT32)(p-dst);
    *p = 0;

    return TRUE;
}

BOOL gos_base64_decode(const UINT8 * src, INT32 iSlen, UINT8 * dst, INT32 *iDlen)
{
    int i, j, n;
    unsigned long x;
    unsigned char *p;

    for( i = j = n = 0; i< iSlen; i++)
    {
        if ((iSlen - i) >= 2 && *(src+i) == '\r' && *(src+i+1) == '\n')
            continue;

        if (*(src+i) == '\n')
            continue;

        if (*(src+i) == '=' && ++j > 2)
            return FALSE;//ERR_BASE64_INVALID_CHARACTER;

        if (*(src+i) > 127 || g_aucBase64DecodeMap[*(src+i)] == 127)
            return FALSE;//ERR_BASE64_INVALID_CHARACTER;

        if (g_aucBase64DecodeMap[*(src+i)] < 64 && j != 0)
            return FALSE;//ERR_BASE64_INVALID_CHARACTER;

        n++;
    }

    if (n == 0)
        return 0;

    n = (n*6+7) >> 3;

    if (*iDlen < n)
    {
        *iDlen = n;
        return FALSE;//ERR_BASE64_BUFFER_TOO_SMALL;
    }

    for (j =3, n = x = 0, p= dst; i> 0; i--, src++)
    {
        if (*src == '\r' || *src == '\n')
            continue;

        j -= (g_aucBase64DecodeMap[*src] == 64);
        x = (x << 6) | (g_aucBase64DecodeMap[*src] & 0x3F);

        if (++n == 4)
        {
            n = 0;
            if (j > 0) *p++ = (unsigned char)(x >> 16);
            if (j > 1) *p++ = (unsigned char)(x >> 8);
            if (j > 2) *p++ = (unsigned char)(x);

        }
    }

    *iDlen = (INT32)(p - dst);

    return TRUE;
}

BOOL gos_merge_file(const CHAR* szTmpFile, const CHAR* szNewFile)
{
    FILE    *fpRead;
    FILE    *fpWrite;
    BOOL    bRet = TRUE;
    UINT8   aucBuf[4096];
    UINT32  ulReadSize;
    UINT32  ulWriteSize;
    CHAR    acNewTmpFile[256];

    if (!gos_file_exist(szTmpFile))
    {
        return FALSE;
    }

    if (!gos_file_exist(szNewFile))
    {
        return gos_copy_file(szTmpFile, szNewFile);
    }

    sprintf(acNewTmpFile, "%s.tmp", szNewFile);
    gos_copy_file(szNewFile, acNewTmpFile);

    fpRead = fopen(szTmpFile, "rb");
    if (!fpRead)
    {
        return FALSE;
    }

    fpWrite = fopen(acNewTmpFile, "a+b");
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

    if (bRet)
    {
        bRet = gos_rename_file(acNewTmpFile, szNewFile);
    }
    else
    {
        gos_delete_file(acNewTmpFile);
    }

    return bRet;
}

UINT16 gos_ansi_crc16(UINT8 *pucData, UINT32 ulLen)
{
    UINT16  usCRC16 = (UINT16)0x8005; //0x18005;    // X16+X15+X2+1
    UINT32  i, j;
    UINT16  usRet = 0;

    for (j=0; j<ulLen; j++)
    {
        usRet ^= (pucData[j]<<8);

        for (i=0; i<8; i++)
        {
            if (usRet & 0x8000)
            {
                usRet <<= 1;
                usRet ^= usCRC16;
            }
            else
            {
                usRet <<= 1;
            }
        }
    }

    return usRet;
}

BOOL gos_is_local_addr(UINT8 *pucAddr)
{
#ifdef _OSWIN32_
    CHAR    acCmd[256] = {0};
    CHAR    acFile[256] = {0};
    FILE    *fp = NULL;
    CHAR    *szInfo = NULL;
    CHAR    *szStart = NULL;
    INT32   iSize = 0;
    CHAR    acKey[64] = {0};

    if (!gos_is_local_ip(pucAddr))
    {
        return FALSE;
    }

    // 以太网适配器 有线:
    //   IPv4 地址 . . . . . . . . . . . . : 192.0.1.120
    gos_delete_file(acFile);
    sprintf(acFile, "%s/ip.info", gos_get_root_path());
    sprintf(acCmd, "ipconfig >%s", acFile);
    gos_system(acCmd);

    fp = fopen(acFile, "rb");
    if (!fp)
    {
        return FALSE;
    }

    fseek(fp, 0, SEEK_END);
    iSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (iSize <= 10)
    {
        fclose(fp);
        return FALSE;
    }

    szInfo = (CHAR*)calloc(1, iSize + 2);
    if (!szInfo)
    {
        fclose(fp);
        return FALSE;
    }

    fread(szInfo, 1, iSize, fp);
    fclose(fp);

    sprintf(acKey, IP_FMT, IP_ARG(pucAddr));
    szStart = strstr(szInfo, acKey);
    free(szInfo);

    return szStart != NULL;
#else
    return gos_is_local_ip(pucAddr);
#endif
}