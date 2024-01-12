#ifndef G_LOGGER_H
#define G_LOGGER_H

/* 打印级别 */
#define LOG_NULL            0
#define LOG_DETAIL          1
#define LOG_INFO            2
#define LOG_WARN            3
#define LOG_ERROR           4
#define LOG_FAIL            5
#define LOG_FATAL           6

#define LOG_ALL             0xf0
#define LOG_DEFAULT         0xff

#define MODULE_NULL         0xffff
#define MODULE_PLAT         0xff00
#define MODULE_DTP          0xff01
#define MODULE_SYS_LOG      0xff02
#define MODULE_PDB          0xff03

#define MODULE_PLAT_NAME    "PLAT"
#define MODULE_DTP_NAME     "DTP"
#define MODULE_SYSLOG_NAME  "SYSLOG"
#define MODULE_PDB_NAME     "PDB"

#define DEFAULT_REMOTE_LOGGER_PORT      27300

typedef struct
{
    UINT32  ulLogLevel;
    UINT32  ulImpLogLevel;  // 重点日志打印级别，缺省为LOG_INFO，当<=ulLogLevel时，重点日志不记录
    BOOL    bLogToStdout;
    BOOL    bPrintColor;
    BOOL    bLogDevInfo;
    BOOL    bLogToFile;
    UINT32  ulLogFileNum;
    UINT32  ulLogFileSize;
    UINT32  ulLogBufTime;   /* 日志缓存时间，单位秒 */
}LOG_CFG_T;

class Logger
{
public :
    virtual VOID Log(const CHAR *szFile, const CHAR *szFunc, UINT32 ulLine,
                     UINT32 ulLogModule, UINT32 ulLogLevel, const CHAR *szFormat, va_list vaLog)=0;
    virtual VOID Log(const CHAR *szFile, const CHAR *szFunc, UINT32 ulLine,
                     const CHAR *szLogModule, UINT32 ulLogLevel, const CHAR *szFormat, va_list vaLog)=0;
    virtual VOID Log(UINT32 ulLogLevel, const CHAR *szFormat, va_list vaLog)=0;
    virtual VOID Log(UINT32 ulLogLevel, const CHAR *szFormat, ...)=0;
    virtual VOID Log(UINT32 ulLogModule, UINT32 ulLogLevel, const CHAR *szFormat, ...)=0;
    virtual VOID Log(const CHAR *szLogModule, UINT32 ulLogLevel, const CHAR *szFormat, ...)=0;
    virtual VOID Log(UINT32 ulLogLevel, const std::string &strLog)=0;

    virtual VOID SetLogModule(UINT32 ulLogModule, const CHAR *szLogModule)=0;
    virtual VOID SetLogLevel(UINT32 ulLogLevel)=0;
    virtual VOID SetImpLogLevel(UINT32 ulLogLevel)=0;
    virtual VOID SetAllModuleLogLevel(UINT32 ulLogLevel)=0;
    virtual VOID SetLogLevel(UINT32 ulLogModule, UINT32 ulLogLevel)=0;
    virtual BOOL SetLogLevel(const CHAR *szLogModule, UINT32 ulLogLevel)=0;

    virtual VOID SetLogToStdout(BOOL bEnable)=0;
    virtual VOID SetPrintColor(BOOL bEnable)=0;
    virtual VOID SetLogDevInfo(BOOL bEnable)=0;
    virtual VOID SetLogToFile(BOOL bEnable, UINT32 ulMaxFileNum=0, UINT32 ulMaxFileSize=1024*1024, UINT32 ulMaxBufTime=300)=0;
    virtual BOOL SetLogFilePath(CHAR *szLogFilePath)=0;

    virtual VOID GetLogCfg(LOG_CFG_T &stCfg)=0;
    virtual VOID SetLogCfg(LOG_CFG_T &stCfg)=0;

    virtual VOID SetLogFilePostfix(const CHAR *szLogFilePostfix)=0;
    virtual VOID Flush(VOID)=0;

    virtual VECTOR<UINT32> GetModuleList()=0;
    virtual VOID GetModule(UINT32 ulModuleID, CHAR *szModuleName, UINT32 &ulModuleLevel)=0;
};

typedef struct
{
    UINT32      ulLogLevel;
    CHAR        acLogModule[32];
}LOG_MODULE_T;

class GLogger:public Logger
{
public :
    GLogger(UINT32 ulLowLogLevel=LOG_INFO, const CHAR *szSubFolderName=NULL);

    virtual VOID Log(const CHAR *szFile, const CHAR *szFunc, UINT32 ulLine,
                     UINT32 ulLogModule, UINT32 ulLogLevel, const CHAR *szFormat, va_list vaLog);
    virtual VOID Log(const CHAR *szFile, const CHAR *szFunc, UINT32 ulLine,
                     const CHAR *szLogModule, UINT32 ulLogLevel, const CHAR *szFormat, va_list vaLog);
    virtual VOID Log(UINT32 ulLogLevel, const CHAR *szFormat, va_list vaLog);
    virtual VOID Log(UINT32 ulLogLevel, const CHAR *szFormat, ...);
    virtual VOID Log(UINT32 ulLogModule, UINT32 ulLogLevel, const CHAR *szFormat, ...);

    virtual VOID Log(const CHAR *szLogModule, UINT32 ulLogLevel, const CHAR *szFormat, ...);

    virtual VOID Log(UINT32 ulLogLevel, const std::string &strLog);

    virtual VOID SetLogModule(UINT32 ulLogModule, const CHAR *szLogModule);
    virtual VOID SetLogLevel(UINT32 ulLogLevel);
    virtual VOID SetImpLogLevel(UINT32 ulLogLevel);
    virtual VOID SetAllModuleLogLevel(UINT32 ulLogLevel);
    virtual VOID SetLogLevel(UINT32 ulLogModule, UINT32 ulLogLevel);
    virtual BOOL SetLogLevel(const CHAR *szLogModule, UINT32 ulLogLevel);

    virtual VOID SetLogToStdout(BOOL bEnable);
    virtual VOID SetPrintColor(BOOL bEnable);
    virtual VOID SetLogDevInfo(BOOL bEnable);
    virtual VOID SetLogToFile(BOOL bEnable, UINT32 ulMaxFileNum=0, UINT32 ulMaxFileSize=1024*1024, UINT32 ulMaxBufTime=300);
    virtual BOOL SetLogFilePath(CHAR *szLogFilePath);

    virtual VOID GetLogCfg(LOG_CFG_T &stCfg);
    virtual VOID SetLogCfg(LOG_CFG_T &stCfg);

    virtual VOID SetLogFilePostfix(const CHAR *szLogFilePostfix);

    virtual VOID Flush(VOID);

    virtual VECTOR<UINT32> GetModuleList();
    virtual VOID GetModule(UINT32 ulModuleID, CHAR *szModuleName, UINT32 &ulModuleLevel);

private:
    LOG_CFG_T               m_stCfg;
    //  UINT32                  m_ulLowLogLevel;

    GMutex                  m_Mutex;
    GMutex                  *m_pLogFileMutex;
    UINT32                  m_ulCurrLogFileSize;
    FILE                    *m_fpLog;

    CHAR                    m_acLoggerName[256];
    CHAR                    m_acLogFilePrefix[256];
    CHAR                    m_acLogFilePath[256];

    UINT32                  m_ulLastFlushTime;

    RawIntMap<LOG_MODULE_T> *m_mLogModule;

    VOID RenameLogFile();
    VOID LogToFile(CHAR *szLog, UINT32 ulLogLen);

    VOID _Log(const CHAR *szLogModule, UINT32 LogLevel, const CHAR *szFormat, va_list vaLog);
};

class GLogFactory
{
public:
    static GLogFactory* GetInstance();
    Logger* GetDefaultLogger();

    Logger* AddLogger(const CHAR *szLoggerName, UINT32 ulLogLevel, BOOL bLogStdout, UINT32 ulLogFileNum, UINT32 ulLogFileSize);
    Logger* GetLogger(const CHAR *szLoggerName);

    /// 刷新所有 logger
    VOID FlushAll();
    ~GLogFactory();

private:
    GLogger     *m_pLogger; ///< 默认 logger

    GMutex      m_Mutex;
    std::map<std::string, GLogger*>  m_mLogger;  ///< 添加的 logger

    GLogFactory();
};

#endif
