#include "RecPublic.h"

#include "TaskSys.h"
#include "TaskCfg.h"
#include "TaskCheck.h"
#include "ThreadOMA.h"
#include "GCommon.h"

#include "GTransFile.h"

#include <string>

#ifdef _OSWIN32_
#include <new.h>
#else
#include <new>
#endif

CHAR            *g_szVersion = (CHAR*)"v2.0.2b";
CHAR            *g_szConfigFile = (CHAR*)"mrs.ini";

PLAT_CFG_T      g_stLocalCfg = {0};
PDBEnvBase      *g_PDBEnv = NULL;
UINT32          g_ulDBType = STORE_NULL;

REC_CFG_T       g_stIniCfg = {0};
MRS_NTP_CFG     g_stNTPCfg = {0};
UINT32          g_ulDCUserID = 0;

BOOL            g_bLogMsgStat = FALSE;

UINT8           g_aucOmcAddr[4] = {0};

UINT32          g_ulClearDataHour = 2;          // 数据清理时间
UINT32          g_ulResvDataDays = 366;         // 数据(库）保留天数
UINT32          g_ulBackupFileNum = 366;        // 数据备份文件个数

INT32           g_iAutoCloseHour = -1;

UINT8           g_aucNTPAddr[4] = {127,0,0,1};
UINT16          g_usNTPPort = 123;
UINT32          g_ulNTPSyncPeriod = 60;

//BOOL            g_bUseFFMpegCmd = TRUE;         // 音视频文件转换采样操作系统的ffmpeg命令实现
CHAR            g_acFFMpegExe[256] = "ffmpeg";

CHAR            g_acOriAudioFormat[16] = "amr"; // 原始音频格式
CHAR            g_acAudioFormat[16] = "mp3";    // 存储音频格式

CHAR            g_acVideoFormat[16]    = "mp4";    // 存储视频格式
//CHAR            g_acMp4Format[16]      = "mp4";    // 存储视频格式
CHAR            g_acOriVideoFormat[16] = "h264";// 原始视频格式

BOOL            g_bReserveOriFile = TRUE;       // 保留原始文件

UINT16          g_usTransFilePort = 50221;
UINT16          g_usDCTransServerPort = 50021;

UINT32          g_ulClusterDefaultStatus = CLUSTER_STATUS_STAND_ALONE;
UINT8           g_aucClusterPeerAddr[4] = {0};
UINT8           g_aucClusterMasterAddr[4] = {0};

extern UINT32          g_ulLowerUDPPort;
extern UINT32          g_ulUpperUDPPort;
extern BOOL     g_bExitWhenMsgQueueIsFull;      // 来自gplat， 通常不暴露出来


BOOL LoadConfig(PLAT_CFG_T *pstCfg)
{
    CHAR        acFile[512];
    GConfFile   *pConf;
    BOOL        bRet = FALSE;
    CHAR        *szValue = NULL;
    UINT32      ulMinPort;
    UINT32      ulMaxPort;

#ifdef _OSWIN32_
    const CHAR  *szFFMpegCmd = "ffmpeg.exe";
#else
    const CHAR  *szFFMpegCmd = "ffmpeg";
#endif

    DOUBLE      dDelay;
    UINT16      usSeq;
    UINT32      ulTTL;

    sprintf(acFile, "%s/%s", gos_get_root_path(), g_szConfigFile);

    if (!gos_file_exist(acFile))
    {
        GosLog(LOG_ERROR, "file %s does not exist", acFile);
        return FALSE;
    }

    pConf = new GConfFile(acFile);

    if (!pConf->Get("user", g_stIniCfg.acUser, sizeof(g_stIniCfg.acUser)))
    {
        GosLog(LOG_ERROR, "get user of cfg failed");
        goto end;
    }

    if (!gos_atou(g_stIniCfg.acUser, &g_ulDCUserID))
    {
        GosLog(LOG_ERROR, "invalid user of cfg failed");
        goto end;
    }

    if (!pConf->Get("pwd", g_stIniCfg.acPwd, sizeof(g_stIniCfg.acPwd)))
    {
        GosLog(LOG_ERROR, "get pwd of cfg failed");
        goto end;
    }

    GosLog(LOG_INFO, "login user:%s, pwd:%s", g_stIniCfg.acUser, g_stIniCfg.acPwd);

    szValue = pConf->Get("server_addr");
    if (!gos_get_ip_port(szValue, g_stIniCfg.aucServerAddr, &g_stIniCfg.usServerPort))
    {
        GosLog(LOG_ERROR, "get server_addr of cfg failed");
        goto end;
    }

    GosLog(LOG_INFO, "sip server IP:IP_FMT, port:%u", IP_ARG(g_stIniCfg.aucServerAddr), g_stIniCfg.usServerPort);

    if (g_stIniCfg.usServerPort == 0)
    {
        g_stIniCfg.usServerPort = 5060;
    }

    ///ntp_cfg
    szValue = pConf->Get("ntp_addr");
    if (!gos_get_ip_port(szValue, g_stNTPCfg.aucNTPAddr, &g_stNTPCfg.usNTPPort))
    {
        GosLog(LOG_ERROR, "get ntp_addr of cfg failed");
        goto end;
    }

    if (g_stNTPCfg.usNTPPort == 0)
    {
        g_stNTPCfg.usNTPPort = 123;
    }

    szValue = pConf->Get("ntp_SyncPeriod");
    if (szValue)
    {
        if (!gos_atou(szValue, &g_stNTPCfg.ulNTPSyncPeriod))
        {
            g_stNTPCfg.ulNTPSyncPeriod = 30;
        }

        if (g_stNTPCfg.ulNTPSyncPeriod < 5 || g_stNTPCfg.ulNTPSyncPeriod > 60)
        {
            g_stNTPCfg.ulNTPSyncPeriod = 30;
        }
    }

    szValue = pConf->Get("local_addr");
    if (szValue)
    {
        if (!gos_get_ip(szValue, g_stIniCfg.aucLocalAddr))
        {
            GosLog(LOG_ERROR, "get local_addr of cfg failed");
            goto end;
        }

        if (!gos_is_local_ip(g_stIniCfg.aucLocalAddr))
        {
            GosLog(LOG_ERROR, "local_addr(%s) is not exist", szValue);
            goto end;
        }
    }
    else if (!gos_ping(g_stIniCfg.aucServerAddr, 32, 200, &dDelay, &usSeq, &ulTTL, g_stIniCfg.aucLocalAddr) &&
             !gos_ping(g_stIniCfg.aucServerAddr, 32, 1000, &dDelay, &usSeq, &ulTTL, g_stIniCfg.aucLocalAddr) )
    {
        GosLog(LOG_ERROR, "get local addr failed, server addr is " IP_FMT, IP_ARG(g_stIniCfg.aucServerAddr));
        goto end;
    }

    sprintf(g_stIniCfg.acLocalAddr, IP_FMT, IP_ARG(g_stIniCfg.aucLocalAddr));
    sprintf(g_stIniCfg.acServerAddr, IP_FMT, IP_ARG(g_stIniCfg.aucServerAddr));

    szValue = pConf->Get("exit_when_msg_is_full");
    if (szValue)
    {
        if (strcasecmp(szValue, "true") == 0 ||
            strcmp(szValue, "1") == 0)
        {
            g_bExitWhenMsgQueueIsFull = TRUE;
        }
    }

    szValue = pConf->Get("msg_max_wait_time");
    if (szValue)
    {
        if (!gos_atou(szValue, &pstCfg->ulMsgMaxWaitTime))
        {
            pstCfg->ulMsgMaxWaitTime = 0;
        }

        if (pstCfg->ulMsgMaxWaitTime > 0 && pstCfg->ulMsgMaxWaitTime < 60)
        {
            pstCfg->ulMsgMaxWaitTime = 60;
        }
    }

    szValue = pConf->Get("max_msg_node_num");
    if (szValue)
    {
        if (!gos_atou(szValue, &pstCfg->ulMaxMsgNodeNum))
        {
            pstCfg->ulMaxMsgNodeNum = 0;
        }
    }

    // rec_path
    if (!pConf->Get("rec_path", g_stIniCfg.acRecRootPath, sizeof(g_stIniCfg.acRecRootPath)))
    {
        GosLog(LOG_ERROR, "invalid rec_path in %s", g_szConfigFile);
        goto end;
    }

    if (g_stIniCfg.acRecRootPath[0] == '\0')
    {
        sprintf(g_stIniCfg.acRecRootPath, "%s/rec", gos_get_root_path());
    }

    szValue = pConf->Get("rec_idle_time");
    if (szValue)
    {
        if (!gos_atou(szValue, &g_stIniCfg.ulMaxRecFileIdleTime))
        {
            GosLog(LOG_ERROR, "invalid rec_idle_time in %s", g_szConfigFile);
        }
    }

    if (g_stIniCfg.ulMaxRecFileIdleTime <= 15)
    {
        g_stIniCfg.ulMaxRecFileIdleTime = 15;
    }
    else if (g_stIniCfg.ulMaxRecFileIdleTime > 600)
    {
        g_stIniCfg.ulMaxRecFileIdleTime = 600;
    }

    //缺省保存7天
    g_stIniCfg.ulTempSaveDays = 7;
    szValue = pConf->Get("temp_file_save_time");
    if (szValue)
    {
        if (!gos_atou(szValue, &g_stIniCfg.ulTempSaveDays))
        {
            GosLog(LOG_ERROR, "invalid temp_file_save_time in %s", g_szConfigFile);
        }
    }

    if (g_stIniCfg.ulTempSaveDays <= 3)
    {
        g_stIniCfg.ulTempSaveDays = 3;
    }
    else if (g_stIniCfg.ulTempSaveDays > 15)
    {
        g_stIniCfg.ulTempSaveDays = 15;
    }

    //缺省保存30天
    g_stIniCfg.ulOriSaveDays = 30;
    szValue = pConf->Get("ori_file_save_time");
    if (szValue)
    {
        if (!gos_atou(szValue, &g_stIniCfg.ulOriSaveDays))
        {
            GosLog(LOG_ERROR, "invalid ori_file_save_time in %s", g_szConfigFile);
        }
    }

    if (g_stIniCfg.ulOriSaveDays <= 30)
    {
        g_stIniCfg.ulOriSaveDays = 30;
    }
    else if (g_stIniCfg.ulOriSaveDays > 90)
    {
        g_stIniCfg.ulOriSaveDays = 90;
    }

    //缺省保存一年
    g_stIniCfg.ulRecSaveDays = 365;
    szValue = pConf->Get("rec_file_save_time");
    if (szValue)
    {
        if (!gos_atou(szValue, &g_stIniCfg.ulRecSaveDays))
        {
            GosLog(LOG_ERROR, "invalid rec_file_save_time in %s", g_szConfigFile);
        }
    }

    if (g_stIniCfg.ulOriSaveDays <= 365)
    {
        g_stIniCfg.ulOriSaveDays = 365;
    }
    else if (g_stIniCfg.ulOriSaveDays > 365*3)
    {
        g_stIniCfg.ulOriSaveDays = 365*3;
    }

    // omc
    szValue = pConf->Get("omc_addr");
    if (!szValue)
    {
        GosLog(LOG_ERROR, "no omc_addr in %s", g_szConfigFile);
        memset(g_aucOmcAddr, 0, sizeof(g_aucOmcAddr));
        //goto end;
    }
    else if (!gos_get_ip(szValue, g_aucOmcAddr))
    {
        GosLog(LOG_ERROR, "invalid omc_addr in %s", g_szConfigFile);
        goto end;
    }

    szValue = pConf->Get("clear_data_hour");
    if (szValue)
    {
        gos_atou(szValue, &g_ulClearDataHour);
        if (g_ulClearDataHour >= 24)
        {
            g_ulClearDataHour = 2;
        }
    }

    szValue = pConf->Get("data_resv_day");
    if (szValue)
    {
        gos_atou(szValue, &g_ulResvDataDays);
        if (g_ulResvDataDays < 30)
        {
            g_ulResvDataDays = 30;
        }
        else if (g_ulResvDataDays > 3*366)
        {
            g_ulResvDataDays = 3*366;
        }
    }
    else
    {
        g_ulResvDataDays = 366;
    }

    szValue = pConf->Get("backup_file_num");
    if (szValue)
    {
        gos_atou(szValue, &g_ulBackupFileNum);
        if (g_ulBackupFileNum < 30)
        {
            g_ulBackupFileNum = 30;
        }
        else if (g_ulBackupFileNum > 3*366)
        {
            g_ulBackupFileNum = 3*366;
        }
    }
    else
    {
        g_ulBackupFileNum = 366;
    }

    g_iAutoCloseHour = -1;
    szValue = pConf->Get("auto_close_hour");
    if (szValue)
    {
        if (!gos_atoi(szValue, &g_iAutoCloseHour))
        {
            g_iAutoCloseHour = -1;
        }
        else if (g_iAutoCloseHour >= 24)
        {
            g_iAutoCloseHour = -1;
        }
    }

    g_bLogMsgStat = FALSE;
    szValue = pConf->Get("log_msg_stat");
    if (szValue)
    {
        if (strcasecmp(szValue, "true") == 0)
        {
            g_bLogMsgStat = TRUE;
        }
    }

    /*g_bUseFFMpegCmd = TRUE;
    szValue = pConf->Get("use_ffmpeg_cmd");
    if (szValue)
    {
        if (strcasecmp(szValue, "false") == 0 ||
            strcmp(szValue, "0") == 0)
        {
            g_bUseFFMpegCmd = FALSE;
        }
    }*/

    szValue = pConf->Get("ffmpeg_path");
    if (szValue)
    {
        sprintf(g_acFFMpegExe, "%s/%s", szValue, szFFMpegCmd);
        gos_format_path(g_acFFMpegExe);
        if (!gos_file_exist(g_acFFMpegExe))
        {
            GosLog(LOG_FATAL, "%s is not exist", g_acFFMpegExe);
            goto end;
        }
    }
    else
    {
        sprintf(g_acFFMpegExe, szFFMpegCmd);
    }

    //trans_file_port
    szValue = pConf->Get("trans_file_port");
    if (!szValue)
    {
        GosLog(LOG_INFO, "no trans_file_port in %s", g_szConfigFile);
        g_stIniCfg.usTransFilePort = g_usTransFilePort;
    }
    else if (!gos_get_short(szValue, &g_stIniCfg.usTransFilePort))
    {
        GosLog(LOG_ERROR, "invalid trans_file_port in %s", g_szConfigFile);
        g_stIniCfg.usTransFilePort = g_usTransFilePort;
        //goto end;
    }

    GosLog(LOG_INFO, "trans_file_port is %u", g_stIniCfg.usTransFilePort);

    //dc_trans_server_port
    szValue = pConf->Get("dc_trans_server_port");
    if (!szValue)
    {
        GosLog(LOG_INFO, "no dc_trans_server_port in %s", g_szConfigFile);
        g_stIniCfg.usDCTransServerPort = g_usDCTransServerPort;
    }
    else if (!gos_get_short(szValue, &g_stIniCfg.usTransFilePort))
    {
        GosLog(LOG_ERROR, "invalid dc_trans_server_port in %s", g_szConfigFile);
        g_stIniCfg.usDCTransServerPort = g_usDCTransServerPort;
        //goto end;
    }

    GosLog(LOG_INFO, "dc_trans_server_port is %u", g_stIniCfg.usDCTransServerPort);

    szValue = pConf->Get("Min_Port");
    if (szValue)
    {
        if (!gos_atou(szValue, &ulMinPort))
        {
            g_ulLowerUDPPort = 51000;
        }
        else if (ulMinPort < 51000 || ulMinPort > 55000)
        {
            g_ulLowerUDPPort = 51000;
        }
        else
        {
            g_ulLowerUDPPort = ulMinPort;
        }
    }

    GosLog(LOG_INFO, "min_port is %u", g_ulLowerUDPPort);

    szValue = pConf->Get("Max_Port");
    if (szValue)
    {
        if (!gos_atou(szValue, &ulMaxPort))
        {
            g_ulUpperUDPPort = 56999;
        }
        else if (ulMaxPort < 56000 || ulMaxPort > 56999)
        {
            g_ulUpperUDPPort = 56999;
        }
        else
        {
            g_ulUpperUDPPort = ulMaxPort;
        }
    }

    GosLog(LOG_INFO, "max_port is %u", g_ulUpperUDPPort);

#ifdef _OSWIN32_
    sprintf(g_acVideoFormat, "mp4");
#else
    //sprintf(g_acVideoFormat, "mkv");
    sprintf(g_acVideoFormat, "mp4");
#endif

    szValue = pConf->Get("video_format");
    if (szValue)
    {
        if (strcmp(szValue, "mp4") == 0 ||
            strcmp(szValue, "avi") == 0 ||
            strcmp(szValue, "mkv") == 0 )
        {
            strcpy(g_acVideoFormat, szValue);
        }
        else
        {
            GosLog(LOG_FATAL, "video_fmt %s is not supported", szValue);
            goto end;
        }
    }

    szValue = pConf->Get("audio_format");
    if (szValue)
    {
        if (strcmp(szValue, "mp3") == 0 ||
            strcmp(szValue, "amr") == 0 ||
            strcmp(szValue, "wav") == 0 )
        {
            strcpy(g_acAudioFormat, szValue);
        }
        else
        {
            GosLog(LOG_FATAL, "audio_fmt %s is not supported", szValue);
            goto end;
        }
    }

    bRet = TRUE;

end:
    delete pConf;
    return bRet;
}

BOOL LoadClusterConfig(CHAR *szCfgFile)
{
    CHAR        acFile[512];
    GConfFile   *pConf;
    BOOL        bRet = FALSE;
    CHAR        *szValue = NULL;

    sprintf(acFile, "%s/%s", gos_get_root_path(), szCfgFile);

    if (!gos_file_exist(acFile))
    {
        GosLog(LOG_ERROR, "file %s does not exist", acFile);
        return FALSE;
    }

    pConf = new GConfFile(acFile);

    g_ulClusterDefaultStatus = CLUSTER_STATUS_STAND_ALONE;
    SET_IP(g_aucClusterMasterAddr, 127, 0, 0, 1);
    SET_IP(g_aucClusterPeerAddr, 127, 0, 0, 1);

    if (!pConf->HasGroupKey("Cluster"))
    {
        delete pConf;
        return TRUE;
    }

    if (!pConf->Get("Cluster", "default_status", CLUSTER_STATUS_STAND_ALONE, &g_ulClusterDefaultStatus))
    {
        GosLog(LOG_ERROR, "invalid default_status in [Cluster] of %s", szCfgFile);
        goto end;
    }

    if (g_ulClusterDefaultStatus == CLUSTER_STATUS_STAND_ALONE)
    {
        GosLog(LOG_INFO, "running under stand alone mode");

        bRet = TRUE;
        goto end;
    }

    szValue = pConf->Get("Cluster", "peer_addr");
    if (szValue)
    {
        if (!gos_get_ip(szValue, g_aucClusterPeerAddr))
        {
            GosLog(LOG_ERROR, "invalid peer_addr in [Cluster] of %s", szCfgFile);
            goto end;
        }
    }

    szValue = pConf->Get("Cluster", "master_addr");
    if (!szValue)
    {
        GosLog(LOG_ERROR, "no master_addr in [Cluster] of %s", szCfgFile);
        goto end;
    }

    if (!gos_get_ip(szValue, g_aucClusterMasterAddr))
    {
        GosLog(LOG_ERROR, "invalid master_addr in [Cluster] of %s", szCfgFile);
        goto end;
    }

    bRet = TRUE;

end:
    delete pConf;
    return bRet;
}

BOOL InitDBEnv(VOID)
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

    sprintf(acFile, "%s/%s", gos_get_root_path(), szCfgFile);

    if (!gos_file_exist(acFile))
    {
        GosLog(LOG_FATAL, "file %s not exist", acFile);
        return FALSE;
    }

    GosLog(LOG_INFO, "read db config file %s", acFile);

    pConf = new GConfFile(acFile);

    szTmp = pConf->Get("db_type");
    if (IS_NULL_STRING(szTmp))
    {
        GosLog(LOG_FATAL, "db_type does not exist in file %s", szCfgFile);
        return FALSE;
    }

    if (strcasecmp(szTmp, "null") == 0)
    {
        g_ulDBType = STORE_NULL;
    }
    else if (strcasecmp(szTmp, "file") == 0)
    {
        g_ulDBType = STORE_ON_FILE;
    }
    else if (strcasecmp(szTmp, "mysql") == 0)
    {
        g_ulDBType = STORE_ON_MYSQL;
    }
    else if (strcasecmp(szTmp, "mssql") == 0)
    {
        g_ulDBType = STORE_ON_SQLSERVER;
    }
    else if (strcasecmp(szTmp, "oracle") == 0)
    {
        g_ulDBType = STORE_ON_ORACLE;
    }
    else
    {
        GosLog(LOG_FATAL, "invalid db_type(%s) in file %s", szTmp, szCfgFile);
        return FALSE;
    }

    if (g_ulDBType == STORE_ON_FILE)
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

    if (g_ulDBType != STORE_NULL &&
        g_ulDBType != STORE_ON_FILE )
    {
        if (g_ulDBType == STORE_ON_MYSQL ||
            g_ulDBType == STORE_ON_ORACLE)
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
    if (g_ulDBType == STORE_NULL)
    {
        stConnParam.ulDBType = PDBT_NOSQL;
    }
    else if (g_ulDBType == STORE_ON_SQLSERVER)
    {
        stConnParam.ulDBType = PDBT_MSSQL;
    }
    else if(g_ulDBType == STORE_ON_MYSQL)
    {
        stConnParam.ulDBType = PDBT_MYSQL;
    }
    else if(g_ulDBType == STORE_ON_ORACLE)
    {
        stConnParam.ulDBType = PDBT_ORACLE;
    }
    else
    {
        GosLog(LOG_FATAL, "unknown db type(%u)", g_ulDBType);
        return FALSE;
    }

    if (g_ulDBType != STORE_NULL &&
        g_ulDBType != STORE_ON_FILE )
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

    OnServerExit();

    return FALSE;
}
#else
static void ProcExitHandler(int sig)
{
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

    gos_format_path(g_stIniCfg.acRecRootPath);
    if (!gos_file_exist(g_stIniCfg.acRecRootPath))
    {
        if (!gos_create_dir(g_stIniCfg.acRecRootPath))
        {
            GosLog(MODULE_CFG, LOG_FATAL, "create dir %s failed!", g_stIniCfg.acRecRootPath);
            return FALSE;
        }
    }

    sprintf(g_stIniCfg.acRecTmpPath, "%s/tmp", g_stIniCfg.acRecRootPath);
    gos_format_path(g_stIniCfg.acRecTmpPath);
    if (!gos_file_exist(g_stIniCfg.acRecTmpPath))
    {
        if (!gos_create_dir(g_stIniCfg.acRecTmpPath))
        {
            GosLog(MODULE_CFG, LOG_FATAL, "create dir %s failed!", g_stIniCfg.acRecTmpPath);
            return FALSE;
        }
    }

    sprintf(g_stIniCfg.acRecOriPath, "%s/ori", g_stIniCfg.acRecRootPath);
    gos_format_path(g_stIniCfg.acRecOriPath);
    if (!gos_file_exist(g_stIniCfg.acRecOriPath))
    {
        if (!gos_create_dir(g_stIniCfg.acRecOriPath))
        {
            GosLog(MODULE_CFG, LOG_FATAL, "create dir %s failed!", g_stIniCfg.acRecOriPath);
            return FALSE;
        }
    }

    GosLog(LOG_FATAL, "Init successful!");

    return TRUE;
}

#if 0
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

VOID InitMsgStatLogger()
{
    CHAR        acPrefix[256];

    gos_get_proc_name(acPrefix, FALSE);
    strcat(acPrefix, "_ms");

    s_MsgStatLogger = new GLogger(LOG_INFO);
    s_MsgStatLogger->SetLogToStdout(TRUE);
    s_MsgStatLogger->SetLogToFile(TRUE, 2, 1024, 0);
    s_MsgStatLogger->SetLogFilePrefix(acPrefix);
}
#endif

extern void test_mix_file();

extern UINT32 GetFreeUDPPort();

#if 0 // def REC_QRY_SRV
int main(int argc, char **argv)
#else
int gos_main(int argc, char **argv)
#endif
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

    if (GosLoadPlatConf(&g_stLocalCfg, g_szConfigFile, DEFAULT_REC_TCP_PORT))
    {
        GosInitLog(&g_stLocalCfg.stLogCfg);
    }
    else
    {
        GosInitLog();
    }

    SetNewHandler();
    SetProcExitHandler();

    GosLog(LOG_FATAL, "mrs version: %s", g_szVersion);

    if (!LoadConfig(&g_stLocalCfg))
    {
        GosLog(LOG_FATAL, "load plat config file %s failed", g_szConfigFile);
        return -1;
    }

    if (!GosLoadPlatConf(&g_stLocalCfg, g_szConfigFile, DEFAULT_REC_TCP_PORT))
    {
        GosLog(LOG_FATAL, "load config file %s failed", g_szConfigFile);
        return -1;
    }

    GosLog(LOG_INFO, "server addr " IP_FMT ":%u", IP_ARG(g_stLocalCfg.stSelfAddr.aucIP), g_stLocalCfg.stSelfAddr.usPort);

/*  if (g_bLogMsgStat)
    {
        InitMsgStatLogger();
        GosRegisterMsgPreHandle(GosMsgPreHandle);
    }*/

    if (!GosStart(&g_stLocalCfg))
    {
        return -1;
    }

    if (!LoadClusterConfig(g_szConfigFile))
    {
        GosLog(LOG_FAIL, "load cluster config failed");
        return FALSE;
    }

    if (!InitDBEnv())
    {
        return -1;
    }

    if (!ServerInit())
    {
        return -1;
    }

    GosRegisterTask(new TaskSys(1));
    GosRegisterTask(new TaskCfg(2));
    GosRegisterTask(new TaskCheck(4));

    if (GET_INT(g_aucOmcAddr) != 0)
    {
        GLogFactory::GetInstance()->GetDefaultLogger()->SetLogModule(MODULE_OMA, (CHAR*)"OMA");

        ThreadOma   *pTheradOma = new ThreadOma(g_szConfigFile, g_szVersion, g_stIniCfg.aucServerAddr, g_aucOmcAddr, g_aucClusterMasterAddr);

        pTheradOma->Start();
    }

    GTransFile  *pTransFile = new GTransFile(g_usTransFilePort, TRUE);

    pTransFile->Start();

//  rec_main();
#ifdef REC_QRY_SRV
    while(1)
    {
        gos_sleep(10);

/*      if (g_bLogMsgStat)
        {
            PrintMsgStat();
        } */
    }
#endif

    return 0;
}
