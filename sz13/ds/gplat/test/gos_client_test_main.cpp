#include "g_include.h"
#include "GPublic.h"
#include "GCommon.h"


PLAT_CFG_T  g_stLocalCfg = {0};

INT32 main(VOID)
{
    memset(&g_stLocalCfg, 0, sizeof(g_stLocalCfg));

    g_stLocalCfg.usInstID = 1111;
    g_stLocalCfg.bMultiInst = FALSE;         /* 是否允许多实例运行，缺省否 */
    g_stLocalCfg.ulSubNetID = 1;
    g_stLocalCfg.ulMaxUniqueMsgNum = 0;  /* 消息查重最大缓存消息个数*/
    g_stLocalCfg.bServerMode = FALSE;
    g_stLocalCfg.stSelfAddr.usPort = 20000;
    g_stLocalCfg.ulPeerNum = 1;
    SET_IP(g_stLocalCfg.astPeerAddr[0].aucIP, 192,168,100,150);
    g_stLocalCfg.astPeerAddr[0].usPort = 30001;
    g_stLocalCfg.astPeerAddr[0].ucTCPMode = FALSE;

    g_stLocalCfg.ulLogLevel = LOG_INFO;
    g_stLocalCfg.bLogToStdout = TRUE;
    g_stLocalCfg.bLogToFile = FALSE;
    g_stLocalCfg.ulLogFileNum = 0;
    g_stLocalCfg.ulLogFileSize = 0;
    g_stLocalCfg.ulLogBufTime = 0;       /* 日志缓存时间，单位秒 */
    g_stLocalCfg.ulMaxSysLogNum = 0;        /* 最大日志缓存数 */

    if (!GosStart(&g_stLocalCfg))
    {
        return -1;
    }

    while(1)
    {
        gos_sleep(1);
    }


    return 0;
}
