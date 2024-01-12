#ifndef GOS_FUNC_G_LOG_H__
#define GOS_FUNC_G_LOG_H__


BOOL GosInitLog();
BOOL GosInitLog(LOG_CFG_T *pstLogCfg);

#define GosLog(x,...)       { gos_log(__FILE__, __FUNCTION__, __LINE__, x,##__VA_ARGS__);}
#define GosMsgLog(x,...)    { gos_msg_log(__FILE__, __FUNCTION__, __LINE__, x,##__VA_ARGS__);}

VOID gos_log(const CHAR *szFile, const CHAR *szFunc, UINT32 ulLine, const CHAR *szLogModule, UINT32 ulLogLevel, const CHAR *szFormat, ...);
VOID gos_log(const CHAR *szFile, const CHAR *szFunc, UINT32 ulLine, UINT32 ulLogModule, UINT32 ulLogLevel, const CHAR *szFormat, ...);
VOID gos_log(const CHAR *szFile, const CHAR *szFunc, UINT32 ulLine, UINT32 ulLogLevel, const CHAR *szFormat, ...);

VOID GosInitImpLogger(UINT32 ulLogLevel);
VOID GosInitMsgLogger();

VOID gos_msg_log(const CHAR *szFile, const CHAR *szFunc, UINT32 ulLine, const CHAR *szLogModule, UINT32 ulLogLevel, const CHAR *szFormat, ...);

#endif  ///< GOS_FUNC_G_LOG_H__
