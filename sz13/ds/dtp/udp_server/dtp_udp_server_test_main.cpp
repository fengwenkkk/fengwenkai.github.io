#include "g_include.h"
#include "DTPServer.h"

extern void dtp_server_test_main();

INT32 main(VOID)
{
    if (!LoadPlatConf(&g_stLocalCfg))
    {
        return -1;
    }

    g_ulSubNetID = g_stLocalCfg.ulSubNetID;

    if (!GosLoadPlatConf(&g_stLocalCfg,(CHAR*)"config/nms_server.conf"))
    {
        return -1;
    }

    GosRegisterMsgPreHandle(GosRecvMsgStatistic);

    if (!GosStart(&g_stLocalCfg))
    {
        return -1;
    }

    if (!InitDBEnv())
    {
        return -1;
    }

    if (!NMSServerInit())
    {
        return -1;
    }

    {
        GTransFile  *pTransFile = new GTransFile(g_stLocalCfg.stSelfAddr.usPort+1);

        pTransFile->Start();
    }

    GosRegisterTask(new TaskSys(1));
    GosRegisterTask(new TaskCfg(2));
    GosRegisterTask(new TaskTopo(3));
    GosRegisterTask(new TaskPerf(4));
    GosRegisterTask(new TaskSec(5));
    GosRegisterTask(new TaskPerfReport(6));
    GosRegisterTask(new TaskAlarm(7));
    GosRegisterTask(new TaskStatus(9));
    GosRegisterTask(new TaskLog(10));
    GosRegisterTask(new TaskFsm(11));

    if (g_ulSubNetID < MIN_REMOTE_SERVER_ID)
    {
        GosRegisterTask(new TaskVer(12));
        GosRegisterTask(new TaskEnMon(14));
        GosRegisterTask(new TaskPstnMon(15));
    }

    GosRegisterTask(new TaskSrm(16));

    while(1)
    {
        gos_sleep(1);
    }

    return 0;
}
