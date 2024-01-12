#include "DISPublic.h"
#include "dtp_public.h"

#include "TaskSys.h"
#include "TaskATS.h"
#include "TaskAPP.h"
#include "TaskSA.h"
#include "TaskCfg.h"
#include "TaskDBSync.h"
#include "TaskOCC.h"
#include "ThreadOMA.h"
#include "GCommon.h"
#include "GTransFile.h"
#include "version.h"
#include "ThreadGPS.h"

#include <string>

#ifdef _OSWIN32_
#include <new.h>
#else
#include <new>
#endif

CHAR            *g_szVersion = (CHAR*)"v2.0.0b";
CHAR            *g_szConfigFile = (CHAR*)"dis.ini";

PLAT_CFG_T      g_stPlatCfg = {0};
PDBEnvBase      *g_PDBEnv = NULL;
PDB_CONN_PARAM_T    g_stPeerConnParam = {0};

BOOL InitDBEnv(UINT8 *pucClusterPeerAddr)
{
    UINT32              ulConNum = 6;
    UINT32              ulTimeout = 10;
    CHAR                acFile[512];
    GConfFile           *pConf;
    BOOL                bRet = FALSE;
    PDB_CONN_PARAM_T    stConnParam = {0};
    CHAR                *szTmp;
    UINT32              ulDBPort = 0;
    UINT32              ulODBCType = ODBC_NULL;
    CHAR                *szCfgFile = (CHAR*)g_szConfigFile;
    CHAR                *szPeerDBName = NULL;
    CHAR                *szPeerDBPwd = NULL;
    UINT32              ulDBType = STORE_NULL;

    sprintf(acFile, "%s/%s", gos_get_root_path(), szCfgFile);

    if (!gos_file_exist(acFile))
    {
        GosLog(LOG_FATAL, "file %s not exist", acFile);
        return FALSE;
    }

    pConf = new GConfFile(acFile);

    szTmp = pConf->Get("db_type");
    if (IS_NULL_STRING(szTmp))
    {
        GosLog(LOG_FATAL, "db_type does not exist in file %s", szCfgFile);
        return FALSE;
    }

    if (strcasecmp(szTmp, "null") == 0)
    {
        ulDBType = STORE_NULL;
    }
    else if (strcasecmp(szTmp, "file") == 0)
    {
        ulDBType = STORE_ON_FILE;
    }
    else if (strcasecmp(szTmp, "mysql") == 0)
    {
        ulDBType = STORE_ON_MYSQL;
    }
    else if (strcasecmp(szTmp, "mssql") == 0)
    {
        ulDBType = STORE_ON_SQLSERVER;
    }
    else if (strcasecmp(szTmp, "oracle") == 0)
    {
        ulDBType = STORE_ON_ORACLE;
    }
    else
    {
        GosLog(LOG_FATAL, "invalid db_type(%s) in file %s", szTmp, szCfgFile);
        return FALSE;
    }

    if (ulDBType == STORE_ON_FILE)
    {
        goto end;
    }

    szTmp = pConf->Get("db_server");
    if (!szTmp)
    {
        GosLog(LOG_FATAL, "db_server does not exist in file %s", szCfgFile);
        return FALSE;
    }

    strcpy(stConnParam.acServer, szTmp);

    szPeerDBName = pConf->Get("peer_db_name");
    szPeerDBPwd = pConf->Get("peer_db_password");

    stConnParam.bUseDSN = FALSE;

#ifdef _OSWIN32_
    if (pConf->Get("db_odbc"))
    {
        if (!pConf->Get("db_odbc", (INT32 *)&ulODBCType))
        {
            GosLog(LOG_FATAL, "invalid db_odbc in file %s", szCfgFile);

            return FALSE;
        }

        if (ulODBCType != ODBC_NULL &&
            ulODBCType != ODBC_DRIVER_CONNECT &&
            ulODBCType != ODBC_DSN_CONNECT )
        {
            GosLog(LOG_FATAL, "invalid db_odbc in file %s", szCfgFile);

            return FALSE;
        }

        if (ulODBCType == ODBC_DSN_CONNECT )
        {
            stConnParam.bUseDSN = TRUE;
        }
    }
#endif

    if (ulDBType != STORE_NULL &&
        ulDBType != STORE_ON_FILE )
    {
        if (ulDBType == STORE_ON_MYSQL ||
            ulDBType == STORE_ON_ORACLE)
        {
            if (!pConf->Get("db_port", (INT32 *)&ulDBPort) || ulDBPort > 0xffff)
            {
                GosLog(LOG_FATAL, "invalid db_port in file %s", szCfgFile);
                return FALSE;
            }
        }
        stConnParam.usPort = (UINT16)ulDBPort;

        szTmp = pConf->Get("db_name");
        if (!szTmp)
        {
            GosLog(LOG_FATAL, "db_name does not exist in file %s", szCfgFile);
            return FALSE;
        }
        else if (strlen(szTmp) >= sizeof(stConnParam.acDBName))
        {
            GosLog(LOG_FATAL, "db_name is too long in file %s", szCfgFile);
            return FALSE;
        }
        strcpy(stConnParam.acDBName, szTmp);

        szTmp = pConf->Get("db_user");
        if (!szTmp)
        {
            GosLog(LOG_FATAL, "db_user does not exist in file %s", szCfgFile);
            return FALSE;
        }
        else if (strlen(szTmp) >= sizeof(stConnParam.acUserName))
        {
            GosLog(LOG_FATAL, "db_user is too long in file %s", szCfgFile);
            return FALSE;
        }
        strcpy(stConnParam.acUserName, szTmp);

        szTmp = pConf->Get("db_password");
        if (!szTmp)
        {
            GosLog(LOG_FATAL, "db_password does not exist in file %s", szCfgFile);
            return FALSE;
        }
        else if (strlen(szTmp) >= sizeof(stConnParam.acPassword))
        {
            GosLog(LOG_FATAL, "db_password is too long in file %s", szCfgFile);
            return FALSE;
        }
        strcpy(stConnParam.acPassword, szTmp);

        szTmp = pConf->Get("character_set");
        if (!szTmp || szTmp[0] == '\0')
        {
            stConnParam.acCharacterSet[0] = '\0';
        }
        else if (strlen(szTmp) >= sizeof(stConnParam.acCharacterSet))
        {
            GosLog(LOG_FATAL, "character_set is too long in file %s", szCfgFile);
            return FALSE;
        }
        strcpy(stConnParam.acCharacterSet, szTmp);

        if (!pConf->Get("db_timeout", ulTimeout, &ulTimeout))
        {
            GosLog(LOG_FATAL, "invalid db_timeout in file %s", szCfgFile);
            return FALSE;
        }

        if (ulTimeout < 5)
        {
            ulTimeout = 5;
        }
    }

    /*初始化数据库环境 */
    if (ulDBType == STORE_NULL)
    {
        stConnParam.ulDBType = PDBT_NOSQL;
    }
    else if (ulDBType == STORE_ON_SQLSERVER)
    {
        stConnParam.ulDBType = PDBT_MSSQL;
    }
    else if(ulDBType == STORE_ON_MYSQL)
    {
        stConnParam.ulDBType = PDBT_MYSQL;
    }
    else if(ulDBType == STORE_ON_ORACLE)
    {
        stConnParam.ulDBType = PDBT_ORACLE;
    }
    else
    {
        GosLog(LOG_FATAL, "unknown db type(%u)", ulDBType);
        return FALSE;
    }

    if (ulDBType != STORE_NULL &&
        ulDBType != STORE_ON_FILE )
    {
        if (ulODBCType != ODBC_NULL)
        {
            stConnParam.ulDBType |= PDBT_ODBC;
        }
    }

    /*初始化数据库环境 */
    if (stConnParam.ulDBType == PDBT_NOSQL)
    {
        g_PDBEnv = new PDBNoSQLEnv();
    }
#ifdef HAVE_MYSQL
    else if (stConnParam.ulDBType == PDBT_MYSQL)
    {
        g_PDBEnv = new PDBMySQLEnv();
    }
#endif
#ifdef _OSWIN32_
    else if (stConnParam.ulDBType & PDBT_ODBC)
    {
        g_PDBEnv = new PDBODBCEnv();
    }
#endif
#ifdef HAVE_ORACLE
    else if (stConnParam.ulDBType == PDBT_ORACLE)
    {
        g_PDBEnv = new PDBOracleEnv();
    }
#endif

    if (g_PDBEnv == NULL)
    {
        GosLog(LOG_FATAL, "unknown db type(%u)", stConnParam.ulDBType);
        return FALSE;
    }

    GosLog(LOG_INFO, "db param: type=%u, server=%s, port=%u, dbname=%s, character-set=%s",
            stConnParam.ulDBType, stConnParam.acServer, stConnParam.usPort,
            stConnParam.acDBName, stConnParam.acCharacterSet);

    bRet = g_PDBEnv->InitEnv(&stConnParam,
                    PDB_PREFETCH_ROW_NUM,
                    ulTimeout,
                    ulConNum);

    if (PDB_SUCCESS != bRet)
    {
        GosLog(LOG_FATAL, "init db environment failed");
        return FALSE;
    }

    memcpy(&g_stPeerConnParam, &stConnParam, sizeof(g_stPeerConnParam));
    if (pucClusterPeerAddr &&
        GET_INT(pucClusterPeerAddr) != 0)
    {
        sprintf(g_stPeerConnParam.acServer, IP_FMT, IP_ARG(pucClusterPeerAddr));
        if (szPeerDBName)
        {
            strcpy(g_stPeerConnParam.acDBName, szPeerDBName);
        }

        if (szPeerDBPwd)
        {
            strcpy(g_stPeerConnParam.acPassword, szPeerDBPwd);
        }
    }

end:
    GosLog(LOG_FATAL, "init db environment successful");

    return TRUE;
}

#ifdef _OSWIN32_
static int NewHandler(size_t nSize)
#else
static void NewHandler()
#endif
{
    UINT32  ulNum = 4;
    VOID    *pvValue;

#ifdef _OSWIN32_
    GosLog(LOG_FATAL, "new %u bytes failed", nSize);
#endif

    for (UINT32 i=0; i<32; i++)
    {
        pvValue = malloc(ulNum);
        if (pvValue == NULL)
        {
            GosLog(LOG_FATAL, "malloc %u bytes failed", ulNum);
            getchar();
            break;
        }
        else
        {
            free(pvValue);
            ulNum *= 2;
        }
    }

#ifdef _OSWIN32_
    return 0;
#endif
}

static VOID SetNewHandler(VOID)
{
#ifdef _OSWIN32_
    _set_new_handler(NewHandler);
#else
    std::set_new_handler(NewHandler);
#endif
}

static VOID OnServerExit(VOID)
{
    GLogFactory::GetInstance()->FlushAll();
}

#ifdef _OSWIN32_
static BOOL WINAPI ProcExitHandler(DWORD dwCtrlType)
{
    if (dwCtrlType == CTRL_C_EVENT ||
        dwCtrlType == CTRL_BREAK_EVENT )
    {
        return TRUE;
    }

    GosLog(LOG_FATAL, "get proc exit ctrl %u", dwCtrlType);
    OnServerExit();

    return FALSE;
}
#else
static void ProcExitHandler(int sig)
{
    GosLog(LOG_FATAL, "get proc exit sig %d", sig);

    OnServerExit();
}
#endif

static VOID SetProcExitHandler(VOID)
{
#ifdef _OSWIN32_
    SetConsoleCtrlHandler(ProcExitHandler, TRUE);
#else
    signal(SIGPIPE, ProcExitHandler);
#endif
}

BOOL ServerInit()
{
    CHAR        acDir[260];

    sprintf(acDir, "%s/log", gos_get_root_path());
    if (!gos_file_exist(acDir))
    {
        if (!gos_create_dir(acDir))
        {
            GosLog(MODULE_CFG, LOG_FATAL, "create dir %s failed!", acDir);
            return FALSE;
        }
    }

    GosLog(LOG_FATAL, "Init successful!");

    return TRUE;
}

static GMutex       s_MsgPreHandleMutex;
static GLogger      *s_MsgStatLogger = NULL;

static MAP<UINT32, UINT64>      s_mMsgStat;
static MAP<std::string, UINT64> s_mJsonMsgStat;

VOID GosMsgPreHandle(MSG_T *pstMsgHdr, VOID *pvMsg, UINT32 ulMsgLen)
{
    CHAR    acMsgName[64];
    CHAR    *szMsg = (CHAR*)pvMsg;
    CHAR    *szKey = (CHAR*)"{\"MsgName\":\"";
    UINT32  ulKeyLen = strlen(szKey);
    UINT32  i, j;
    UINT32  *pulNum;

    if (!s_MsgPreHandleMutex.Mutex())
    {
        return;
    }

    // {"MsgName":"SAApplyIPHTalk","MsgSeqID":52
    pulNum = (UINT32*)&s_mMsgStat[pstMsgHdr->usMsgID];
    pulNum[1]++;

    if (pstMsgHdr->ulMsgLen <= ulKeyLen )
    {
        goto end;
    }

    for (i=0; i<ulKeyLen; i++)
    {
        if (!isascii(szMsg[i]))
        {
            goto end;
        }
    }

    for (i=0; (i+ulKeyLen)<pstMsgHdr->ulMsgLen; i++)
    {
        if (memcmp(szMsg+i, szKey, ulKeyLen) == 0)
        {
            szMsg += ulKeyLen+i;
            acMsgName[sizeof(acMsgName)-1] = '\0';
            for (j=0; j<sizeof(acMsgName)-1; j++)
            {
                if (szMsg[j] == '"')
                {
                    acMsgName[j] = '\0';
                    break;
                }

                acMsgName[j] = szMsg[j];
            }

            pulNum = (UINT32*)&s_mJsonMsgStat[acMsgName];
            pulNum[1]++;

            break;
        }
    }

end:
    s_MsgPreHandleMutex.Unmutex();
}

VOID PrintMsgStat()
{
    if (!s_MsgStatLogger)
    {
        return;
    }

    if (!s_MsgPreHandleMutex.Mutex())
    {
        return;
    }

    for (MAP<UINT32, UINT64>::iterator  it=s_mMsgStat.begin();
         it!=s_mMsgStat.end(); it++)
    {
        UINT32      *pulNum = (UINT32*)&it->second;

        s_MsgStatLogger->Log(LOG_INFO, "== msg stat: %X %10u %u", it->first, pulNum[1], pulNum[1]-pulNum[0]);
        pulNum[0] = pulNum[1];
    }

    for (MAP<std::string, UINT64>::iterator it=s_mJsonMsgStat.begin();
        it!=s_mJsonMsgStat.end(); it++)
    {
        char *szMsgName = (char*)it->first.c_str();
        UINT32      *pulNum = (UINT32*)&it->second;

        s_MsgStatLogger->Log(LOG_INFO, "== msg stat: %s %10u %u", szMsgName, pulNum[1], pulNum[1]-pulNum[0]);
        pulNum[0] = pulNum[1];
    }

    s_MsgPreHandleMutex.Unmutex();
}

VOID GosInitMsgStatLogger()
{
    s_MsgStatLogger = new GLogger(LOG_INFO);

    s_MsgStatLogger->SetLogToStdout(TRUE);
    s_MsgStatLogger->SetLogToFile(TRUE, 2, 1024, 0);
    s_MsgStatLogger->SetLogFilePostfix("ms");
}

int main(int argc, char **argv)
{
    if (gos_is_proc_running())
    {
        printf("current proc is already running\n");

        exit(-1);
    }

    BOOL    bLogToStdout = FALSE;

    for (int i=1; i<argc; i++)
    {
        if (strcmp(argv[i], "-l") == 0)
        {
            bLogToStdout = TRUE;
            break;
        }
    }

    gos_run_daemon(bLogToStdout);

    if (GosLoadPlatConf(&g_stPlatCfg, g_szConfigFile))
    {
        GosInitLog(&g_stPlatCfg.stLogCfg);
    }
    else
    {
        GosInitLog();
    }

    SetNewHandler();
    SetProcExitHandler();

    GosLog(LOG_FATAL, "dis version: %s", DIS_VERSION);

    if (!GosLoadPlatConf(&g_stPlatCfg, g_szConfigFile))
    {
        GosLog(LOG_FATAL, "load config file %s failed", g_szConfigFile);
        return -1;
    }

    if (!LoadLocalCfg())
    {
        GosLog(LOG_FATAL, "load config file %s failed", g_szConfigFile);
        return -1;
    }

    LOCAL_CFG_T     &stLocalCfg = GetLocalCfg();

    GosLog(LOG_INFO, "server addr " IP_FMT ":%u", IP_ARG(g_stPlatCfg.stSelfAddr.aucIP), g_stPlatCfg.stSelfAddr.usPort);

    if (g_stPlatCfg.bLogMsgStat)
    {
        GosInitMsgStatLogger();
        GosRegisterMsgPreHandle(GosMsgPreHandle);
    }

    if (!GosStart(&g_stPlatCfg))
    {
        return -1;
    }

    if (!InitDBEnv(stLocalCfg.aucClusterPeerAddr))
    {
        return -1;
    }

    if (!ServerInit())
    {
        return -1;
    }

    // 基本Task
    TaskCfg     *pTaskCfg = new TaskCfg(2);

    GosRegisterTask(pTaskCfg);

    while (pTaskCfg->GetTaskStatus() != TASK_STATUS_WORK)
    {
        gos_sleep_1ms(10);
    }

    GosRegisterTask(new TaskSys(1));

    if (stLocalCfg.ulClusterDefaultStatus == CLUSTER_STATUS_STAND_ALONE)
    {
        g_stPeerConnParam.ulDBType = PDBT_NOSQL;
    }

    GosRegisterTask(new TaskDBSync(8));

    GosRegisterTask(new TaskAPP(11));

    GosRegisterTask(new TaskATS(21));
    GosRegisterTask(new TaskSA(22));

    if (GET_INT(stLocalCfg.aucNOCCAddr) != 0)
    {
        GosRegisterTask(new TaskOCC(91));
    }

    if (GET_INT(stLocalCfg.aucOmcAddr) != 0)
    {
        ThreadOma   *pThreadOma = new ThreadOma(g_szConfigFile, g_szVersion, stLocalCfg.aucMdcAddr, stLocalCfg.aucOmcAddr, stLocalCfg.aucClusterMasterAddr);

        pThreadOma->Start();
    }

#ifdef HAS_TX_GPS   // 车载台是否支持GPS上报
    StartThreadGPS(g_usGPSRecvPort);
#endif

    while(1)
    {
        gos_sleep(10);
        if (g_stPlatCfg.bLogMsgStat)
        {
            PrintMsgStat();
        }
    }

//    return 0;
}
