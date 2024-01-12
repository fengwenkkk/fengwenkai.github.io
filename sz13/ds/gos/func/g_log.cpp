#include "g_include.h"

static const CHAR  *g_szLoggerImp = "imp";
static const CHAR  *g_szLoggerMsg = "msg";

BOOL GosInitLog()
{
    Logger *pLogger = GLogFactory::GetInstance()->GetDefaultLogger();

    pLogger->SetLogToStdout(TRUE);
    pLogger->SetLogToFile(TRUE, 100, 4 * 1024 * 1024, 0);
    pLogger->SetLogLevel(LOG_INFO);

    return TRUE;
}

BOOL GosInitLog(LOG_CFG_T *pstLogCfg)
{
    Logger *pLogger = GLogFactory::GetInstance()->GetDefaultLogger();

    pLogger->SetLogToStdout(pstLogCfg->bLogToStdout);
    pLogger->SetLogToFile(pstLogCfg->bLogToFile, pstLogCfg->ulLogFileNum, pstLogCfg->ulLogFileSize,
                          pstLogCfg->ulLogBufTime);
    pLogger->SetLogLevel(pstLogCfg->ulLogLevel);
    pLogger->SetImpLogLevel(pstLogCfg->ulImpLogLevel);
    pLogger->SetPrintColor(pstLogCfg->bPrintColor);
    pLogger->SetLogDevInfo(pstLogCfg->bLogDevInfo);

    pLogger->SetLogModule(MODULE_DTP, (CHAR *)MODULE_DTP_NAME);

    return TRUE;
}

VOID gos_log(const CHAR *szFile, const CHAR *szFunc, UINT32 ulLine,
             const CHAR *szLogModule, UINT32 ulLogLevel, const CHAR *szFormat, ...)
{
    va_list vaLog;

    va_start(vaLog, szFormat);
    GLogFactory::GetInstance()->GetDefaultLogger()->Log(szFile, szFunc, ulLine, szLogModule, ulLogLevel, szFormat, vaLog);

    Logger  *pLogger = GLogFactory::GetInstance()->GetLogger(g_szLoggerImp);
    if (pLogger)
    {
        va_end(vaLog);
        va_start(vaLog, szFormat);
        pLogger->Log(szFile, szFunc, ulLine, szLogModule, ulLogLevel, szFormat, vaLog);
    }

    va_end(vaLog);
}

VOID gos_log(const CHAR *szFile, const CHAR *szFunc, UINT32 ulLine,
             UINT32 ulLogModule, UINT32 ulLogLevel, const CHAR *szFormat, ...)
{
    va_list vaLog;

    va_start(vaLog, szFormat);
    GLogFactory::GetInstance()->GetDefaultLogger()->Log(szFile, szFunc, ulLine, ulLogModule, ulLogLevel, szFormat, vaLog);

    Logger  *pLogger = GLogFactory::GetInstance()->GetLogger(g_szLoggerImp);
    if (pLogger)
    {
        va_end(vaLog);
        va_start(vaLog, szFormat);
        pLogger->Log(szFile, szFunc, ulLine, ulLogModule, ulLogLevel, szFormat, vaLog);
    }

    va_end(vaLog);
}

VOID gos_log(const CHAR *szFile, const CHAR *szFunc, UINT32 ulLine,
             UINT32 ulLogLevel, const CHAR *szFormat, ...)
{
    va_list vaLog;

    va_start(vaLog, szFormat);
    GLogFactory::GetInstance()->GetDefaultLogger()->Log(szFile, szFunc, ulLine, "", ulLogLevel, szFormat, vaLog);

    Logger  *pLogger = GLogFactory::GetInstance()->GetLogger(g_szLoggerImp);
    if (pLogger)
    {
        va_end(vaLog);
        va_start(vaLog, szFormat);

        pLogger->Log(szFile, szFunc, ulLine, "", ulLogLevel, szFormat, vaLog);
    }

    va_end(vaLog);
}

VOID GosInitImpLogger(UINT32 ulLogLevel)
{
    CHAR        acPrefix[256];
    LOG_CFG_T   stLogCfg = {0};

    gos_get_proc_name(acPrefix, FALSE);

    GLogFactory::GetInstance()->GetDefaultLogger()->GetLogCfg(stLogCfg);

    if (ulLogLevel <= stLogCfg.ulLogLevel)
    {
        return;
    }

    Logger *pMsgLogger = GLogFactory::GetInstance()->GetLogger(g_szLoggerImp);

    if (!pMsgLogger)
    {
        pMsgLogger = GLogFactory::GetInstance()->AddLogger(g_szLoggerImp, LOG_DETAIL, FALSE, stLogCfg.ulLogFileNum, stLogCfg.ulLogFileSize);
    }

    pMsgLogger->SetLogCfg(stLogCfg);
    pMsgLogger->SetLogLevel(ulLogLevel);
    pMsgLogger->SetLogToStdout(FALSE);
    pMsgLogger->SetLogFilePostfix(g_szLoggerImp);

    return;
}

VOID GosInitMsgLogger()
{
    CHAR        acPrefix[256];
    LOG_CFG_T   stLogCfg = {0};

    gos_get_proc_name(acPrefix, FALSE);

    GLogFactory::GetInstance()->GetDefaultLogger()->GetLogCfg(stLogCfg);

    Logger *pMsgLogger = GLogFactory::GetInstance()->GetLogger(g_szLoggerMsg);

    if (!pMsgLogger)
    {
        pMsgLogger = GLogFactory::GetInstance()->AddLogger(g_szLoggerMsg, LOG_DETAIL, FALSE, stLogCfg.ulLogFileNum, stLogCfg.ulLogFileSize);
    }

    pMsgLogger->SetLogCfg(stLogCfg);
    pMsgLogger->SetLogToStdout(FALSE);
    pMsgLogger->SetImpLogLevel(LOG_NULL);
    pMsgLogger->SetLogFilePostfix(g_szLoggerMsg);

    return;
}

VOID gos_msg_log(const CHAR *szFile, const CHAR *szFunc, UINT32 ulLine,
                 const CHAR *szLogModule, UINT32 ulLogLevel, const CHAR *szFormat, ...)
{
    va_list vaLog;
    Logger  *pLogger = GLogFactory::GetInstance()->GetLogger(g_szLoggerMsg);

    va_start(vaLog, szFormat);

    if (pLogger)
    {
        pLogger->Log(szFile, szFunc, ulLine, szLogModule, ulLogLevel, szFormat, vaLog);

        va_end(vaLog);
        va_start(vaLog, szFormat);
    }

    GLogFactory::GetInstance()->GetDefaultLogger()->Log(szFile, szFunc, ulLine, szLogModule, ulLogLevel, szFormat, vaLog);

    va_end(vaLog);
}
