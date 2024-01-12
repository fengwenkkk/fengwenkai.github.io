#include "g_include.h"

#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
#endif

typedef enum
{
    GOS_LOG_COLOR_ORIGIN,
    GOS_LOG_COLOR_WHITE,
    GOS_LOG_COLOR_BLUE,
    GOS_LOG_COLOR_GREEN,
    GOS_LOG_COLOR_YELLOW,
    GOS_LOG_COLOR_RED,
}GOS_LOG_COLOR_E;

#ifdef _WIN32
enum window_color
{
    //ORIGIN_COLOR = FOREGROUND_INTENSITY,                                                       ///< 原色
    WINDOWS_LOG_COLOR_ORIGIN = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    WINDOWS_LOG_COLOR_WHITE  = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,  ///< 白色
    WINDOWS_LOG_COLOR_BLUE   = FOREGROUND_INTENSITY | FOREGROUND_BLUE,                                       ///< 蓝色
    WINDOWS_LOG_COLOR_GREEN  = FOREGROUND_INTENSITY | FOREGROUND_GREEN,                                     ///< 绿色
    WINDOWS_LOG_COLOR_YELLOW = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,                   ///< 黄色
    WINDOWS_LOG_COLOR_RED    = /*FOREGROUND_INTENSITY | */FOREGROUND_RED,                                         ///< 红色
};

inline WORD GetWindowLogColor(GOS_LOG_COLOR_E enColor)
{
    if (enColor == GOS_LOG_COLOR_ORIGIN)    return WINDOWS_LOG_COLOR_ORIGIN;
    if (enColor == GOS_LOG_COLOR_WHITE)     return WINDOWS_LOG_COLOR_WHITE;
    if (enColor == GOS_LOG_COLOR_BLUE)      return WINDOWS_LOG_COLOR_BLUE;
    if (enColor == GOS_LOG_COLOR_GREEN)     return WINDOWS_LOG_COLOR_GREEN;
    if (enColor == GOS_LOG_COLOR_YELLOW)    return WINDOWS_LOG_COLOR_YELLOW;
    if (enColor == GOS_LOG_COLOR_RED)       return WINDOWS_LOG_COLOR_RED;

    return WINDOWS_LOG_COLOR_ORIGIN;
}
#else
inline const char* GetLinuxLogColor(GOS_LOG_COLOR_E enColor)
{
    if (enColor == GOS_LOG_COLOR_ORIGIN)    return "\e[0m";
    if (enColor == GOS_LOG_COLOR_WHITE)     return "\e[1;37m";
    if (enColor == GOS_LOG_COLOR_BLUE)      return "\e[1;34m";
    if (enColor == GOS_LOG_COLOR_GREEN)     return "\e[1;32m";
    if (enColor == GOS_LOG_COLOR_YELLOW)    return "\e[1;33m";
    if (enColor == GOS_LOG_COLOR_RED)       return "\e[1;31m";

    return "\e[0m";
}
#endif

/// 颜色输出函数
static inline void GosPrintColor(GOS_LOG_COLOR_E enColor, const char* szLog)
{
#ifdef _WIN32
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GetWindowLogColor(enColor));
    printf("%s", szLog);
#ifdef USE_WHITE_COLOR_IN_LOG_TEXT
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GetWindowLogColor(GOS_LOG_COLOR_WHITE));
#else
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GetWindowLogColor(GOS_LOG_COLOR_ORIGIN));
#endif
#else
    printf("%s%s%s", GetLinuxLogColor(enColor), szLog, GetLinuxLogColor(GOS_LOG_COLOR_ORIGIN));
#endif
}

/**
 * @brief           获取日志等级字符串
 * @param           ulLogLevel    [in]
 * @return          CHAR*
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-16 15:47:34
 * @note
 * 这里在返回的字符串后面添加空格是为了对齐， 也是为了获取颜色时的比较字符串
 */
static CHAR* GetLogLevel(UINT32 ulLogLevel)
{
    if (ulLogLevel == LOG_DETAIL)
    {
        return (CHAR*)"DETAIL";
    }
    else if (ulLogLevel == LOG_INFO)
    {
        return (CHAR*)"INFO  ";
    }
    else if (ulLogLevel == LOG_WARN)
    {
        return (CHAR*)"WARN  ";
    }
    else if (ulLogLevel == LOG_ERROR)
    {
        return (CHAR*)"ERROR ";
    }
    else if (ulLogLevel == LOG_FAIL)
    {
        return (CHAR*)"FAIL  ";
    }
    else if (ulLogLevel == LOG_FATAL)
    {
        return (CHAR*)"FATAL ";
    }

    return (CHAR*)"";
}

GLogFactory::GLogFactory()
{
    m_pLogger = new GLogger(LOG_DETAIL);
}

GLogFactory::~GLogFactory()
{
    delete m_pLogger;

    std::map<std::string, GLogger*>::iterator it = m_mLogger.begin();
    for(; it != m_mLogger.end(); ++it)
    {
        delete it->second;
    }
}

GLogFactory* GLogFactory::GetInstance()
{
    static GLogFactory* pLogFactory = NULL;

    if (!pLogFactory)
    {
        pLogFactory = new GLogFactory();
    }

    return pLogFactory;
}

Logger* GLogFactory::GetDefaultLogger()
{
    return m_pLogger;
}

Logger* GLogFactory::AddLogger(const CHAR *szLoggerName, UINT32 ulLogLevel, BOOL bLogStdout, UINT32 ulLogFileNum, UINT32 ulLogFileSize)
{
    m_Mutex.Mutex();

    GLogger *pLogger = new GLogger(ulLogLevel, szLoggerName);

    pLogger->SetLogToStdout(bLogStdout);
    if (ulLogFileNum > 0)
    {
        pLogger->SetLogToFile(TRUE, ulLogFileNum, ulLogFileSize, 0);
    }

    pLogger->SetLogFilePostfix(szLoggerName);

    m_mLogger[szLoggerName] = pLogger;

    m_Mutex.Unmutex();

    return pLogger;
}

VOID GLogFactory::FlushAll()
{
    m_pLogger->Flush();
    std::map<std::string, GLogger*>::iterator it = m_mLogger.begin();
    for(; it != m_mLogger.end(); ++it)
    {
        it->second->Flush();
    }
}

Logger* GLogFactory::GetLogger(const CHAR *szLoggerName)
{
    m_Mutex.Mutex();

    Logger *pLogger = NULL;

    if (m_mLogger.find(szLoggerName) != m_mLogger.end())
    {
        pLogger = m_mLogger[szLoggerName];
    }

    m_Mutex.Unmutex();

    return pLogger;
}

/* GLogger */
GLogger::GLogger(UINT32 ulLowLogLevel, const CHAR *szSubFolderName)
{
    m_mLogModule = new RawIntMap<LOG_MODULE_T>(16);

    m_pLogFileMutex = new GMutex();

    SetLogModule(MODULE_PLAT, MODULE_PLAT_NAME);

    //m_ulLowLogLevel = ulLowLogLevel;
    memset(&m_stCfg, 0, sizeof(m_stCfg));
    m_stCfg.ulLogLevel = ulLowLogLevel;//LOG_ERROR;
    m_stCfg.bLogToStdout = FALSE;
    m_stCfg.bPrintColor = FALSE;
    m_stCfg.bLogDevInfo = FALSE;
    m_stCfg.bLogToFile = TRUE;

    m_stCfg.ulLogFileNum = 2;
    m_stCfg.ulLogFileSize = 1024*1024;

    m_ulCurrLogFileSize = 0;
    m_fpLog = NULL;

    m_ulLastFlushTime = 0;
    m_stCfg.ulLogBufTime = 0;//5*60;

    m_acLoggerName[0] = '\0';
    m_acLogFilePrefix[0] = '\0';
    if (!szSubFolderName || *szSubFolderName == '\0')
    {
        sprintf(m_acLogFilePath, "%s/log", gos_get_root_path());
    }
    else
    {
        sprintf(m_acLogFilePath, "%s/log/%s", gos_get_root_path(), szSubFolderName);
    }
    gos_get_proc_name(m_acLoggerName, FALSE);
    strcpy(m_acLogFilePrefix, m_acLoggerName);

    // 缺省不打开，以免正式环境下影响性能
    SetPrintColor(FALSE);
}

VOID GLogger::GetLogCfg(LOG_CFG_T &stCfg)
{
    memcpy(&stCfg, &m_stCfg, sizeof(stCfg));
}

VOID GLogger::SetLogCfg(LOG_CFG_T &stCfg)
{
    memcpy(&m_stCfg, &stCfg, sizeof(stCfg));
}

VOID GLogger::SetLogFilePostfix(const CHAR *szLogFilePostfix)
{
    memset(m_acLogFilePrefix, 0, sizeof(m_acLogFilePrefix));
    snprintf(m_acLogFilePrefix, sizeof(m_acLogFilePrefix)-1, "%s_%s", m_acLoggerName, szLogFilePostfix);
}

VOID GLogger::RenameLogFile()
{
    CHAR    acFile[1024];
    CHAR    acNextFile[512+16];

    // 9->10 8->9 ... 1->2 , last x.log -> x_1.log
    for (INT32 i=m_stCfg.ulLogFileNum-1; i>=0; i--)
    {
        if (i > 0)
        {
            sprintf(acFile, "%s/%s_%02d.log", m_acLogFilePath, m_acLogFilePrefix, i);
        }
        else
        {
            sprintf(acFile, "%s/%s.log", m_acLogFilePath, m_acLogFilePrefix);
        }

        if (!gos_file_exist(acFile))
        {
            continue;
        }

        sprintf(acNextFile, "%s/%s_%02d.log", m_acLogFilePath, m_acLogFilePrefix, i+1);
        gos_rename_file(acFile, acNextFile);
    }
}

VOID GLogger::LogToFile(CHAR *szLog, UINT32 ulLogLen)
{
    if (!m_pLogFileMutex->Mutex())
    {
        return;
    }

    if (m_ulCurrLogFileSize > m_stCfg.ulLogFileSize)
    {
        fclose(m_fpLog);
        m_fpLog = NULL;
        m_ulCurrLogFileSize = 0;
    }

    if (!m_fpLog)
    {
        CHAR    acCurrLogFile[512+4];

        if (!gos_file_exist(m_acLogFilePath))
        {
            gos_create_redir(m_acLogFilePath);
        }

        sprintf(acCurrLogFile, "%s/%s.log", m_acLogFilePath, m_acLogFilePrefix);
        gos_format_path(acCurrLogFile);

        INT32   iFileSize = gos_get_file_size(acCurrLogFile);

        if (iFileSize > 0 && (UINT32)iFileSize < m_stCfg.ulLogFileSize)
        {
            m_fpLog = fopen(acCurrLogFile, "a+t");
            if (m_fpLog)
            {
                CHAR    acTime[32];

                gos_get_text_time_ex(NULL, NULL, acTime);

                fprintf(m_fpLog, "\n\n =============== new log %s ===============\n", acTime);

                m_ulCurrLogFileSize = iFileSize;
            }
        }
        else
        {
            RenameLogFile();
            m_fpLog = fopen(acCurrLogFile, "w+t");
        }

        m_ulLastFlushTime = gos_get_uptime();
    }

    if (m_fpLog)
    {
        m_ulCurrLogFileSize += fprintf(m_fpLog, "%s\n", szLog);
#ifdef WIN32
        m_ulCurrLogFileSize++;
#endif

        UINT32  ulCurrTime = gos_get_uptime();

        if ((UINT32)(ulCurrTime - m_ulLastFlushTime) >= m_stCfg.ulLogBufTime)
        {
            fflush(m_fpLog);
            m_ulLastFlushTime = ulCurrTime;
        }
    }

    m_pLogFileMutex->Unmutex();
}

VOID GLogger::Flush()
{
    if (!m_pLogFileMutex->Mutex())
    {
        return;
    }

    if (m_fpLog)
    {
        fflush(m_fpLog);
    }

    m_pLogFileMutex->Unmutex();
}

VOID GLogger::Log(UINT32 ulLogLevel, const CHAR *szFormat, va_list vaLog)
{
    _Log((CHAR*)"", ulLogLevel, szFormat, vaLog);
}

VOID GLogger::Log(UINT32 ulLogLevel, const std::string &strLog)
{
    Log(ulLogLevel, strLog.c_str());
}

VOID GLogger::Log(UINT32 ulLogLevel, const CHAR *szFormat, ...)
{
    va_list vaLog;

    if (ulLogLevel == LOG_NULL || ulLogLevel < m_stCfg.ulLogLevel)
    {
        return;
    }

    va_start(vaLog, szFormat);

    _Log((CHAR*)"", ulLogLevel, szFormat, vaLog);

    va_end(vaLog);
}

VOID GLogger::Log(UINT32 ulLogModule, UINT32 ulLogLevel, const CHAR *szFormat, ...)
{
    va_list         vaLog;
    CHAR            *szLogModule = (CHAR*)"";
    LOG_MODULE_T    *pLogModule = NULL;

    if (ulLogLevel == LOG_NULL || ulLogLevel < m_stCfg.ulLogLevel)
    {
        return;
    }

    pLogModule = m_mLogModule->GetValue(ulLogModule);
    if (!pLogModule)
    {
        szLogModule = (CHAR*)"";
    }
    else
    {
        if (pLogModule->ulLogLevel != LOG_DEFAULT &&
            ulLogLevel < pLogModule->ulLogLevel)
        {
            return;
        }

        szLogModule = pLogModule->acLogModule;
    }

    va_start(vaLog, szFormat);

    _Log(szLogModule, ulLogLevel, (CHAR*)szFormat, vaLog);

    va_end(vaLog);
}

VOID GLogger::Log(const CHAR *szFile, const CHAR *szFunc, UINT32 ulLine,
                  UINT32 ulLogModule, UINT32 ulLogLevel, const CHAR *szFormat, va_list vaLog)
{
    CHAR            *szLogModule = (CHAR*)"";
    LOG_MODULE_T    *pLogModule = NULL;

    if (ulLogLevel == LOG_NULL || ulLogLevel < m_stCfg.ulLogLevel)
    {
        return;
    }

    pLogModule = m_mLogModule->GetValue(ulLogModule);
    if (!pLogModule)
    {
        szLogModule = (CHAR*)"";
    }
    else
    {
        if (pLogModule->ulLogLevel != LOG_DEFAULT &&
            ulLogLevel < pLogModule->ulLogLevel)
        {
            return;
        }

        szLogModule = pLogModule->acLogModule;
    }

    Log(szFile, szFunc, ulLine, szLogModule, ulLogLevel, (CHAR*)szFormat, vaLog);
}

VOID GLogger::Log(const CHAR *szLogModule, UINT32 ulLogLevel, const CHAR *szFormat, ...)
{
    va_list vaLog;

    if (ulLogLevel == LOG_NULL || ulLogLevel < m_stCfg.ulLogLevel)
    {
        return;
    }

    if (IS_NULL_STRING(szLogModule))
    {
        szLogModule = (CHAR*)"";
    }

    va_start(vaLog, szFormat);

    _Log(szLogModule, ulLogLevel, szFormat, vaLog);

    va_end(vaLog);
}

VOID GLogger::SetLogModule(UINT32 ulLogModule, const CHAR *szLogModule)
{
    LOG_MODULE_T    *pLogModule = NULL;

    pLogModule = m_mLogModule->GetValue(ulLogModule);
    if (pLogModule)
    {
        return;
    }

    pLogModule = (LOG_MODULE_T*)calloc(1, sizeof(LOG_MODULE_T));
    if (!pLogModule)
    {
        return;
    }

    pLogModule->ulLogLevel = LOG_DEFAULT;
    strncpy(pLogModule->acLogModule, szLogModule, sizeof(pLogModule->acLogModule)-1);

    m_mLogModule->Add(ulLogModule, pLogModule);
}

VOID GLogger::SetLogLevel(UINT32 ulLogLevel)
{
    if (ulLogLevel > LOG_FATAL)
    {
        ulLogLevel = LOG_FATAL;
    }

    m_stCfg.ulLogLevel = ulLogLevel;
}

VOID GLogger::SetImpLogLevel(UINT32 ulLogLevel)
{
    m_stCfg.ulImpLogLevel = ulLogLevel;
}

VOID GLogger::SetAllModuleLogLevel(UINT32 ulLogLevel)
{
    if (ulLogLevel == LOG_DEFAULT)
    {
        ulLogLevel = m_stCfg.ulLogLevel;
    }
    else
    {
        if (ulLogLevel > LOG_FATAL)
        {
            return;
        }

        m_stCfg.ulLogLevel = ulLogLevel;
    }

    std::vector<UINT32> vLogModule;

    m_mLogModule->GetKeyList(vLogModule);

    for (UINT32 i=0; i<vLogModule.size(); i++)
    {
        LOG_MODULE_T    *pLogModule = m_mLogModule->GetValue(vLogModule[i]);

        if(pLogModule)
        {
            pLogModule->ulLogLevel = ulLogLevel;
        }
    }
}

VOID GLogger::SetLogLevel(UINT32 ulLogModule, UINT32 ulLogLevel)
{
    LOG_MODULE_T    *pLogModule = NULL;

    if (ulLogLevel > LOG_DEFAULT)
    {
        return;
    }

    pLogModule = m_mLogModule->GetValue(ulLogModule);
    if (pLogModule)
    {
        pLogModule->ulLogLevel = ulLogLevel;
    }
}

BOOL GLogger::SetLogLevel(const CHAR *szLogModule, UINT32 ulLogLevel)
{
    LOG_MODULE_T        *pLogModule = NULL;
    std::vector<UINT32> vLogModule;
    BOOL                bRet = FALSE;

    if (ulLogLevel > LOG_DEFAULT)
    {
        return bRet;
    }

    m_mLogModule->GetKeyList(vLogModule);

    for (UINT32 i=0; i<vLogModule.size(); i++)
    {
        pLogModule = m_mLogModule->GetValue(vLogModule[i]);

        if(pLogModule && (strcmp(pLogModule->acLogModule, szLogModule) == 0))
        {
            pLogModule->ulLogLevel = ulLogLevel;
            bRet = TRUE;
            break;
        }
    }

    return bRet;
}

VOID GLogger::SetLogToStdout(BOOL bEnable)
{
    m_stCfg.bLogToStdout = bEnable;
}

VOID GLogger::SetPrintColor(BOOL bEnable)
{
    m_stCfg.bPrintColor = bEnable;
}

VOID GLogger::SetLogDevInfo(BOOL bEnable)
{
    m_stCfg.bLogDevInfo = bEnable;
}

BOOL GLogger::SetLogFilePath(CHAR *szLogFilePath)
{
    if (!m_pLogFileMutex->Mutex())
    {
        return FALSE;
    }

    if (strlen(szLogFilePath) >= sizeof(m_acLogFilePath))
    {
        m_pLogFileMutex->Unmutex();
        return FALSE;
    }

#ifdef _OSWIN32_
    if (strcasecmp(m_acLogFilePath, szLogFilePath) == 0)
    {
        m_pLogFileMutex->Unmutex();
        return TRUE;
    }
#else
    if (strcmp(m_acLogFilePath, szLogFilePath) == 0)
    {
        m_pLogFileMutex->Unmutex();
        return TRUE;
    }
#endif

    fclose(m_fpLog);
    m_fpLog = NULL;
    m_ulCurrLogFileSize = 0;
    m_ulLastFlushTime = 0;

    strcpy(m_acLogFilePath, szLogFilePath);
    m_pLogFileMutex->Unmutex();

    return TRUE;
}

VOID GLogger::SetLogToFile(BOOL bEnable, UINT32 ulMaxFileNum, UINT32 ulMaxFileSize, UINT32 ulMaxBufTime)
{
    if (ulMaxFileNum < 2)
    {
        ulMaxFileNum = 2;
    }

    if (ulMaxFileSize < 1024*1024)
    {
        ulMaxFileSize = 1024*1024;
    }

    m_stCfg.bLogToFile = bEnable;
    m_stCfg.ulLogFileNum = ulMaxFileNum;
    m_stCfg.ulLogFileSize = ulMaxFileSize;
    m_stCfg.ulLogBufTime = ulMaxBufTime;
}

VECTOR<UINT32> GLogger::GetModuleList()
{
    std::vector<UINT32> vLogModule;

    m_mLogModule->GetKeyList(vLogModule);

    return vLogModule;
}

VOID GLogger::GetModule(UINT32 ulModuleID, CHAR *szModuleName, UINT32 &ulModuleLevel)
{
    LOG_MODULE_T    *pLogModule = m_mLogModule->GetValue(ulModuleID);

    if(pLogModule)
    {
        strcpy(szModuleName, pLogModule->acLogModule);
        ulModuleLevel = pLogModule->ulLogLevel;
    }
}

static inline GOS_LOG_COLOR_E GetLogColor(UINT32 ulLogLevel)
{
//#pragma warning(disable : 4482)
    if (ulLogLevel == LOG_DETAIL)   return GOS_LOG_COLOR_BLUE;
    if (ulLogLevel == LOG_INFO)     return GOS_LOG_COLOR_GREEN;
    if (ulLogLevel == LOG_WARN)     return GOS_LOG_COLOR_YELLOW;
    if (ulLogLevel == LOG_ERROR)    return GOS_LOG_COLOR_RED;
    if (ulLogLevel == LOG_FAIL)     return GOS_LOG_COLOR_RED;
    if (ulLogLevel == LOG_FATAL)    return GOS_LOG_COLOR_RED;

    return GOS_LOG_COLOR_ORIGIN;
}

VOID GLogger::_Log(const CHAR *szLogModule, UINT32 ulLogLevel, const CHAR *szFormat, va_list vaLog)
{
    Log(NULL, NULL, 0, szLogModule, ulLogLevel, szFormat, vaLog);
}

VOID GLogger::Log(const CHAR *szFile, const CHAR *szFunc, UINT32 ulLine,
                   const CHAR *szLogModule, UINT32 ulLogLevel, const CHAR *szFormat, va_list vaLog)
{
    UINT32  ulLen;
    UINT32  ulLogLevelPos;
    INT32   iDataLen;
    CHAR    acTime[32] = {0};
    CHAR    acLog[4096] = {0};
    CHAR    *szLog;
    CHAR    *szFuncName;
    TIMEVAL stTimeVal;
    CHAR    *szLogLevel = GetLogLevel(ulLogLevel);

    //if (ulLogLevel < m_ulLowLogLevel)
    if (ulLogLevel < m_stCfg.ulLogLevel)
    {
        return;
    }

    if (!m_stCfg.bLogDevInfo)
    {
        szFile = NULL;
    }

    szFile = gos_get_simple_file_name(szFile);
    szFuncName = gos_right_strstr(szFunc, "::");
    if (!szFuncName)
    {
        szFuncName = (CHAR*)szFunc;
    }
    else
    {
        szFuncName += strlen("::");
    }

    gos_gettimeofday(&stTimeVal);
    gos_asctimeval(&stTimeVal, acTime);
    if (IS_NULL_STRING(szLogModule))
    {
        szLogModule = m_acLoggerName;
    }

    ulLen = sprintf(acLog, "%s [%s", acTime, szLogLevel);
    ulLogLevelPos = ulLen;
    if (szFile && szFunc)
    {
        ulLen += sprintf(acLog+ulLen, "] [%s] [%s %s %u] : ", szLogModule, szFile, szFuncName, ulLine);
    }
    else
    {
        ulLen += sprintf(acLog+ulLen, "] [%s] : ", szLogModule);
    }

    szLog = acLog+ulLen;
    iDataLen = vsnprintf(szLog, sizeof(acLog)-2-ulLen, szFormat, vaLog);
    // vsnprintf 在 valog内容超出buf大小时，返回负数
    if (iDataLen <= 0)
    {
        iDataLen = strlen(szLog);
    }

    ulLen += iDataLen;
    if (ulLen >= sizeof(acLog))
    {
        ulLen = sizeof(acLog)-1;
    }

    /* log */
    if (m_stCfg.bLogToStdout)
    {
        m_Mutex.Mutex();

        GOS_LOG_COLOR_E enPrintColor = GetLogColor(ulLogLevel);

        if (!m_stCfg.bPrintColor || enPrintColor == GOS_LOG_COLOR_ORIGIN)
        {
            printf("%s\n", acLog);
        }
        else
        {
            printf("%s [", acTime + strlen("YYYY-MM-DD "));                      ///< 打印时间
            GosPrintColor(enPrintColor, szLogLevel);  ///< 打印日志等级
            printf("%s\n", acLog+ulLogLevelPos);
        }

        m_Mutex.Unmutex();
    }

    if (m_stCfg.bLogToFile)
    {
        LogToFile(acLog, ulLen);
    }
}

#ifndef _WIN32
#pragma GCC diagnostic pop
#endif