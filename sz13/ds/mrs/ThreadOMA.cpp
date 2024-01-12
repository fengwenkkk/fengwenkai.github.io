#include "g_include.h"
#include "ds_public.h"
#include "msg.h"
#include "omc.h"
#include "oid.h"

#include "ThreadOMA.h"
#include "snmp_nms.h"
#include "snmp_def.h"

#define OmaLog(ulLogLevel, ...)      { GosLog("OMA", ulLogLevel, ##__VA_ARGS__); }

extern BOOL IsNTPValid();

BOOL IsLocalAddr(UINT8 *pucAddr)
{
    UINT8   aucLoopAddr[] = {127,0,0,1};

    if (memcmp(pucAddr, aucLoopAddr, sizeof(aucLoopAddr)) == 0)
    {
        return TRUE;
    }

#ifdef _OSWIN32_
    if (!gos_is_local_ip(pucAddr))
    {
        return FALSE;
    }

    CHAR    acCmd[256];
    CHAR    acFile[256];
    FILE    *fp;
    CHAR    *szInfo = NULL;
    CHAR    *szStart;
    INT32   iSize;
    CHAR    acKey[64];

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

BOOL ThreadOma::InitOmaSocket()
{
    UINT8   aucLocalAddr[4] = {0,0,0,0};

    CLOSE_SOCKET(m_stOmaSocket);
    m_stOmaSocket = gos_create_udp_server_socket(aucLocalAddr, m_stOmaCfg.usSnmpPort);
    if (m_stOmaSocket == INVALID_SOCKET)
    {
        OmaLog(LOG_ERROR, "init snmp socket failed");
        return FALSE;
    }

    OmaLog(LOG_INFO, "init snmp socket succ");

    return TRUE;
}

BOOL ThreadOma::SendDiscoveryTrap()
{
    SNMP_VAR_T              *pstVarList = NULL;
    INT32                   iRet = -1;
    CHAR                    acModelName[32] = {OMC_MODEL_NAME_REC};
    UINT32                  ulDevType = OMC_DEV_TYPE_REC;
    UINT32                  ulDefaultInt = 0;
    UINT32                  ulDefaultNum = 2000;
    CHAR                    acDefaultStr[8]={" "};

#ifdef __BORLANDC__
    strcpy(acModelName, OMC_MODEL_NAME_DC);
    ulDevType = OMC_DEV_TYPE_DC;
#endif

    SNMP_INIT_TRAP_VAR(pstVarList, oid_DiscoveryTrap);

    SNMP_ADD_TRAP_VAR(pstVarList, oid_sysMacAddress,    TYPE_MAC,   m_stOmaCfg.aucOmaMac);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_sysIpaddress,     TYPE_IP,    m_stOmaCfg.aucOmaAddr);

    SNMP_ADD_TRAP_VAR(pstVarList, oid_sysManageState,   TYPE_INT,   ulDefaultInt);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_modelId,          TYPE_INT,   ulDefaultInt);

    SNMP_ADD_TRAP_VAR(pstVarList, oid_currentVersion,   TYPE_CHAR,  acDefaultStr);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_softwareVersion,  TYPE_CHAR,  m_stOmaCfg.acSoftwareVer);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_hardwareVersion,  TYPE_CHAR,  m_stOmaCfg.acHardwareVer);

    SNMP_ADD_TRAP_VAR(pstVarList, oid_modelName,        TYPE_CHAR,  acModelName);

    SNMP_ADD_TRAP_VAR(pstVarList, oid_hardwareType,     TYPE_INT,  ulDefaultInt);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_boardVersion,     TYPE_INT,  ulDefaultInt);

    SNMP_ADD_TRAP_VAR(pstVarList, oid_sysModel,         TYPE_CHAR,  acDefaultStr);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_devType,          TYPE_INT,   ulDevType);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_sysName,          TYPE_CHAR,  acDefaultStr);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_sysHotId,         TYPE_CHAR,  m_stOmaCfg.acDevName);

    SNMP_ADD_TRAP_VAR(pstVarList, oid_sendOfflineConfigFile,    TYPE_CHAR,  acDefaultStr);

    SNMP_ADD_TRAP_VAR(pstVarList, oid_sysMaxStaNum,     TYPE_INT,  ulDefaultNum);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_memorySize,       TYPE_INT,  ulDefaultInt);

    iRet = snmp_send_trap(m_stOmaSocket, m_aucOmcAddr, m_stOmaCfg.usTrapPort, m_stOmaCfg.acTrapCommunity,
                          m_stOmaCfg.ulRecvTimeout, SNMP_MSG_TRAP2, pstVarList);
    if (-1 == iRet)
    {
        OmaLog(LOG_ERROR, "send discovery trap failed");
        return FALSE;
    }

    OmaLog(LOG_INFO, "send discovery trap succ");
    return TRUE;
}

BOOL ThreadOma::SendHeartBeatTrap()
{
    SNMP_VAR_T              *pstVarList = NULL;
//    UINT32                  ulDefaultInt = 0;
    INT32                   iRet = 0;

    SNMP_INIT_TRAP_VAR(pstVarList, oid_HeartBeatTrap);

    SNMP_ADD_TRAP_VAR(pstVarList, oid_sysMacAddress,        TYPE_MAC,   m_stOmaCfg.aucOmaMac);
/*
    SNMP_ADD_TRAP_VAR(pstVarList, oid_channelUsing,         TYPE_LONG,  ulDefaultInt);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_transmitPower,        TYPE_LONG,  ulDefaultInt);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_wifi5ChannelUsing,    TYPE_LONG,  ulDefaultInt);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_wifi5TransmitPower,   TYPE_LONG,  ulDefaultInt);
*/
    iRet = snmp_send_trap(m_stOmaSocket, m_aucOmcAddr, m_stOmaCfg.usTrapPort, m_stOmaCfg.acTrapCommunity,
                          m_stOmaCfg.ulRecvTimeout, SNMP_MSG_INFORM, pstVarList);
    if (-1 == iRet)
    {
        OmaLog(LOG_ERROR, "send heart beat trap failed");
        return FALSE;
    }

    OmaLog(LOG_DETAIL, "send heart beat trap succ");
    return TRUE;
}

VOID ThreadOma::InitAlarmInfo()
{
    OMA_ALARM_INFO_T    *pstInfo;

    memset(m_astAlarmInfo, 0, sizeof(m_astAlarmInfo));

    pstInfo = &m_astAlarmInfo[REBOOT_ALARM];
    memcpy(pstInfo->astOID, oid_RebootTrap, sizeof(oid_RebootTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_RebootTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_Reboot;
    pstInfo->stAlarmInfo.bIsNotifyAlarm = TRUE;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_CRITICAL;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_DEV;
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "设备重启");

    pstInfo = &m_astAlarmInfo[CPU_USAGE_ALARM];
    memcpy(pstInfo->astOID, oid_CpuUsageTooHighTrap, sizeof(oid_CpuUsageTooHighTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_CpuUsageTooHighTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_CpuUsageTooHigh;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_DEV;
    strcpy(pstInfo->stAlarmInfo.acAlarmReason, DevCpuUsageTooHighAlarmInfo);
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "CPU使用率过高");

    pstInfo = &m_astAlarmInfo[MEM_USAGE_ALARM];
    memcpy(pstInfo->astOID, oid_MemUsageTooHighTrap, sizeof(oid_MemUsageTooHighTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_MemUsageTooHighTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_MemUsageTooHigh;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_DEV;
    strcpy(pstInfo->stAlarmInfo.acAlarmReason, DevMemUsageTooHighAlarmInfo);
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "内存使用率过高");

    pstInfo = &m_astAlarmInfo[DISK_USAGE_ALARM];
    memcpy(pstInfo->astOID, oid_DiskUsageTooHighTrap, sizeof(oid_DiskUsageTooHighTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_DiskUsageTooHighTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_DiskUsageTooHigh;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_DEV;
    strcpy(pstInfo->stAlarmInfo.acAlarmReason, DiskUsageTooHighAlarmInfo);
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "硬盘使用率过高");

    pstInfo = &m_astAlarmInfo[SWITCH_TO_MASTER_ALARM];
    memcpy(pstInfo->astOID, oid_SwitchToMasterTrap, sizeof(oid_SwitchToMasterTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_SwitchToMasterTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_SwitchToMaster;
    pstInfo->stAlarmInfo.bIsNotifyAlarm = TRUE;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_DEV;
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "切换到主用状态");

    pstInfo = &m_astAlarmInfo[SWITCH_TO_SLAVE_ALARM];
    memcpy(pstInfo->astOID, oid_SwitchToSlaveTrap, sizeof(oid_SwitchToSlaveTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_SwitchToSlaveTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_SwitchToSlave;
    pstInfo->stAlarmInfo.bIsNotifyAlarm = TRUE;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_DEV;
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "切换到备用状态");

    pstInfo = &m_astAlarmInfo[NTP_ALARM];
    memcpy(pstInfo->astOID, oid_SyncTimeFailTrap, sizeof(oid_SyncTimeFailTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_SyncTimeFailTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_SyncTimeFail;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_COMM;
    strcpy(pstInfo->stAlarmInfo.acAlarmReason, DevSyncTimeFailAlarmInfo);
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "时钟同步失败");

    pstInfo = &m_astAlarmInfo[ATS_ALARM];
    memcpy(pstInfo->astOID, oid_GetATSDataFailTrap, sizeof(oid_GetATSDataFailTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_GetATSDataFailTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_GetATSDataFail;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_COMM;
    strcpy(pstInfo->stAlarmInfo.acAlarmReason, DevGetATSDataFailAlarmInfo);
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "获取ATS数据失败");

    pstInfo = &m_astAlarmInfo[LTE_ALARM];
    memcpy(pstInfo->astOID, oid_LinkToLteFailTrap, sizeof(oid_LinkToLteFailTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_LinkToLteFailTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_LinkToLteFail;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_COMM;
    strcpy(pstInfo->stAlarmInfo.acAlarmReason, DevLinkToLteFailAlarmInfo);
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "集群服务器不可达");

    for (UINT32 i=0; i<ARRAY_SIZE(m_astAlarmInfo); i++)
    {
        pstInfo = &m_astAlarmInfo[i];
        if (pstInfo->stAlarmInfo.bIsNotifyAlarm)
        {
            pstInfo->stAlarmInfo.ulPrevAlarmStatus = ALARM_STATUS_CLEAR;
        }
        else
        {
            pstInfo->stAlarmInfo.ulPrevAlarmStatus = ALARM_STATUS_UNKNOWN;
        }
    }
}

BOOL ThreadOma::SendAlarmTrap(OMA_ALARM_INFO_T *pstAlarmInfo)
{
    SNMP_VAR_T      *pstVarList = NULL;
    INT32           iRet = -1;
    const CHAR      *szAlarmStatus = pstAlarmInfo->stAlarmInfo.ulAlarmStatus == ALARM_STATUS_RAISE? "raise":"clear";

    if (snmp_varlist_add_var(&pstVarList, SNMP_TRAP, OID_LENGTH(SNMP_TRAP), snmp_get_asn1_type(TYPE_OID),
        (UINT8*)pstAlarmInfo->astOID, pstAlarmInfo->ulOIDSize*sizeof(SNMP_OID)) < 0)
    {
        return FALSE;
    }

    // OMC收到后，如果为0时则采用OMC本地时间
    pstAlarmInfo->stAlarmInfo.ulAlarmRaiseTime = 0;

    if (pstAlarmInfo->stAlarmInfo.acAlarmReason[0] == '\0')
    {
        strcpy(pstAlarmInfo->stAlarmInfo.acAlarmReason, " ");
    }

    if (pstAlarmInfo->stAlarmInfo.acAlarmTitle[0] == '\0')
    {
        strcpy(pstAlarmInfo->stAlarmInfo.acAlarmTitle, " ");
    }

    if (pstAlarmInfo->stAlarmInfo.acAlarmInfo[0] == '\0')
    {
        strcpy(pstAlarmInfo->stAlarmInfo.acAlarmInfo, " ");
    }

    SNMP_ADD_TRAP_VAR(pstVarList, oid_AlarmSn,       TYPE_INT,  pstAlarmInfo->stAlarmInfo.ulAlarmID);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_AlarmNeName,   TYPE_MAC,  m_stOmaCfg.aucOmaMac);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_AlarmLevel,    TYPE_INT,  pstAlarmInfo->stAlarmInfo.ulAlarmLevel);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_AlarmType,     TYPE_INT,  pstAlarmInfo->stAlarmInfo.ulAlarmType);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_AlarmReasonID, TYPE_INT,  pstAlarmInfo->stAlarmInfo.ulAlarmReasonID);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_AlarmReason,   TYPE_CHAR, pstAlarmInfo->stAlarmInfo.acAlarmReason);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_AlarmRaiseTime,TYPE_TIME, pstAlarmInfo->stAlarmInfo.ulAlarmRaiseTime);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_AlarmStatus,   TYPE_INT,  pstAlarmInfo->stAlarmInfo.ulAlarmStatus);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_AlarmTitle,    TYPE_CHAR, pstAlarmInfo->stAlarmInfo.acAlarmTitle);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_AlarmInfo,     TYPE_CHAR, pstAlarmInfo->stAlarmInfo.acAlarmInfo);

    iRet = snmp_send_trap(m_stOmaSocket, m_aucOmcAddr, m_stOmaCfg.usTrapPort, m_stOmaCfg.acTrapCommunity,
                          m_stOmaCfg.ulRecvTimeout, SNMP_MSG_TRAP2, pstVarList);
    if (-1 == iRet)
    {
        OmaLog(LOG_ERROR, "send alarm %s trap of %u failed", szAlarmStatus, pstAlarmInfo->stAlarmInfo.ulAlarmID);
        return FALSE;
    }

    OmaLog(LOG_INFO, "send alarm %s trap of %u succ", szAlarmStatus, pstAlarmInfo->stAlarmInfo.ulAlarmID);
    return TRUE;
}

ThreadOma::ThreadOma(const CHAR *szConfigFile, const CHAR *szVersion, UINT8 *pucMdcAddr, UINT8 *pucOmcAddr, UINT8 *pucMasterAddr):GThread(NULL)
{
    m_szConfigFile = szConfigFile?strdup(szConfigFile):(CHAR*)"";
    m_szVersion = szVersion?strdup(szVersion):(CHAR*)"";
    memcpy(m_aucMdcAddr, pucMdcAddr, sizeof(m_aucMdcAddr));
    memcpy(m_aucOmcAddr, pucOmcAddr, sizeof(m_aucOmcAddr));

    if (pucMasterAddr == NULL)
    {
        memset(m_aucMasterAddr, 0, sizeof(m_aucMasterAddr));
    }
    else
    {
        memcpy(m_aucMasterAddr, pucMasterAddr, sizeof(m_aucMasterAddr));
    }

    memset(&m_stOmaCfg, 0, sizeof(m_stOmaCfg));
    m_stOmaSocket = INVALID_SOCKET;
    m_ulOmaInfoNum = 0;     // 当前告警检测测试

    memset(m_astAlarmData, 0, sizeof(m_astAlarmData));
    memset(m_astAlarmInfo, 0, sizeof(m_astAlarmInfo));

    m_ulOmcHeartBeatPeriod = 30;
    m_ulMaxHeartBeatFailNum = 6;
    m_ulLastSendHeartBeatTime = 0;
    m_ulLastRecvHeartBeatTime = 0;
    m_bSendDiscovery = FALSE;

    m_bRunning = FALSE;
}

BOOL ThreadOma::InitOmaAddr()
{
    UINT8   aucOmaAddr[4] = {0xff,0xff,0xff,0xff};

    DOUBLE  dDelay;
    UINT16  usSeq;
    UINT32  ulTTL;

    if (IsLocalAddr(m_aucOmcAddr))
    {
        memcpy(aucOmaAddr, m_aucOmcAddr, sizeof(m_aucOmcAddr));
    }
    else if (!gos_ping(m_aucOmcAddr, 32, 2000, &dDelay, &usSeq, &ulTTL, aucOmaAddr))
    {
        OmaLog(LOG_ERROR, "get oma addr failed, omc addr is " IP_FMT, IP_ARG(m_aucOmcAddr));
        memset(m_stOmaCfg.aucOmaAddr, 0, sizeof(m_stOmaCfg.aucOmaAddr));
        return FALSE;
    }

    if (memcmp(aucOmaAddr, m_stOmaCfg.aucOmaAddr, sizeof(aucOmaAddr)) == 0)
    {
        return TRUE;
    }

    OmaLog(LOG_INFO, "oma addr is " IP_FMT, IP_ARG(aucOmaAddr));
    memcpy(m_stOmaCfg.aucOmaAddr, aucOmaAddr, sizeof(aucOmaAddr));
    m_bSendDiscovery = FALSE;

    memcpy(m_stOmaCfg.aucGateway, m_stOmaCfg.aucOmaAddr, sizeof(m_stOmaCfg.aucGateway));
    m_stOmaCfg.aucGateway[3] = 254; // TODO 目前写死

    return TRUE;
}

BOOL ThreadOma::Init()
{
    if (!InitOmaCfg())
    {
        OmaLog(LOG_ERROR, "init oma cfg failed");
        return FALSE;
    }

    if (!InitOmaAddr())
    {
        OmaLog(LOG_ERROR, "init oma addr failed");
        return FALSE;
    }

    if (m_stOmaSocket == INVALID_SOCKET)
    {
        if (!InitOmaSocket())
        {
            return FALSE;
        }
    }

    InitAlarmInfo();

    return TRUE;
}

BOOL ThreadOma::InitOmaCfg()
{
    static BOOL bInit = FALSE;

    if (bInit)
    {
        return TRUE;
    }

    CHAR        acCfgFile[256];

    sprintf(acCfgFile, "%s/%s", gos_get_root_path(), m_szConfigFile);

    if (!gos_file_exist(acCfgFile))
    {
        OmaLog(LOG_ERROR, "file %s does not exist", acCfgFile);
        return FALSE;
    }

    GConfFile   *pConf = new GConfFile(acCfgFile);

    CHAR *szValue = pConf->Get("Oma", "oma_mac");

    if (!szValue)
    {
        OmaLog(LOG_ERROR, "no oma_mac in %s", m_szConfigFile);
        goto fail;
    }

    if (!gos_get_mac(szValue, m_stOmaCfg.aucOmaMac))
    {
        OmaLog(LOG_ERROR, "invalid oma_mac in %s", m_szConfigFile);
        goto fail;
    }

    pConf->Get("Oma", "cpu_alarm_upper", 60, &m_stOmaCfg.iCpuUsageUpperThreshold);
    pConf->Get("Oma", "cpu_alarm_lower", 30, &m_stOmaCfg.iCpuUsageLowerThreshold);

    if (m_stOmaCfg.iCpuUsageUpperThreshold <= m_stOmaCfg.iCpuUsageLowerThreshold ||
        m_stOmaCfg.iCpuUsageUpperThreshold >= 100 ||
        m_stOmaCfg.iCpuUsageLowerThreshold == 0)
    {
        OmaLog(LOG_ERROR, "invalid cpu alarm value in %s", m_szConfigFile);
        goto fail;
    }

    pConf->Get("Oma", "mem_alarm_upper", 60, &m_stOmaCfg.iMemUsageUpperThreshold);
    pConf->Get("Oma", "mem_alarm_lower", 40, &m_stOmaCfg.iMemUsageLowerThreshold);

    if (m_stOmaCfg.iMemUsageUpperThreshold <= m_stOmaCfg.iMemUsageLowerThreshold ||
        m_stOmaCfg.iMemUsageUpperThreshold >= 100 ||
        m_stOmaCfg.iMemUsageLowerThreshold == 0)
    {
        OmaLog(LOG_ERROR, "invalid mem alarm value in %s", m_szConfigFile);
        goto fail;
    }

    pConf->Get("Oma", "disk_alarm_upper", 80, &m_stOmaCfg.iDiskUsageUpperThreshold);
    pConf->Get("Oma", "disk_alarm_lower", 50, &m_stOmaCfg.iDiskUsageLowerThreshold);

    if (m_stOmaCfg.iDiskUsageUpperThreshold <= m_stOmaCfg.iDiskUsageLowerThreshold ||
        m_stOmaCfg.iDiskUsageUpperThreshold >= 100 ||
        m_stOmaCfg.iDiskUsageLowerThreshold == 0)
    {
        OmaLog(LOG_ERROR, "invalid disk alarm value in %s", m_szConfigFile);
        goto fail;
    }

    pConf->Get("Oma", "alarm_check_period", 15, &m_stOmaCfg.ulCheckPeriod);
    if (m_stOmaCfg.ulCheckPeriod < 5)
    {
        m_stOmaCfg.ulCheckPeriod = 5;
    }
    else if (m_stOmaCfg.ulCheckPeriod > 60)
    {
        m_stOmaCfg.ulCheckPeriod = 60;
    }

    pConf->Get("Oma", "alarm_check_fail_num", 10, &m_stOmaCfg.ulCheckFailNum);
    if (m_stOmaCfg.ulCheckFailNum < 5)
    {
        m_stOmaCfg.ulCheckFailNum = 5;
    }
    else if (m_stOmaCfg.ulCheckFailNum > ARRAY_SIZE(m_astAlarmData))
    {
        m_stOmaCfg.ulCheckFailNum = ARRAY_SIZE(m_astAlarmData);
    }

    pConf->Get("Oma", "snmp_ver", SNMP_V2, &m_stOmaCfg.ulSnmpVer);
    if (m_stOmaCfg.ulSnmpVer != SNMP_V1 &&
        m_stOmaCfg.ulSnmpVer != SNMP_V2 )
    {
        m_stOmaCfg.ulSnmpVer = SNMP_V2;
    }

    pConf->Get("Oma", "recv_timeout", 5, &m_stOmaCfg.ulRecvTimeout);
    if (m_stOmaCfg.ulRecvTimeout < 2)
    {
        m_stOmaCfg.ulRecvTimeout = 2;
    }
    else if (m_stOmaCfg.ulRecvTimeout > 10)
    {
        m_stOmaCfg.ulRecvTimeout = 10;
    }

    pConf->Get("Oma", "snmp_port", SNMP_PORT, &m_stOmaCfg.usSnmpPort);
    if (m_stOmaCfg.usSnmpPort == 0)
    {
        m_stOmaCfg.usSnmpPort = SNMP_PORT;
    }

    pConf->Get("Oma", "snmp_trap_port", SNMP_TRAP_PORT, &m_stOmaCfg.usTrapPort);
    if (m_stOmaCfg.usTrapPort == 0)
    {
        m_stOmaCfg.usTrapPort = SNMP_TRAP_PORT;
    }

    if (!pConf->Get("Oma", "snmp_get_community", m_stOmaCfg.acGetCommunity, sizeof(m_stOmaCfg.acGetCommunity)) ||
        m_stOmaCfg.acGetCommunity[0] == '\0')
    {
        strcpy(m_stOmaCfg.acGetCommunity, "public");
    }

    if (!pConf->Get("Oma", "snmp_set_community", m_stOmaCfg.acSetCommunity, sizeof(m_stOmaCfg.acSetCommunity)) ||
        m_stOmaCfg.acSetCommunity[0] == '\0')
    {
        strcpy(m_stOmaCfg.acSetCommunity, "private");
    }

    if (!pConf->Get("Oma", "snmp_trap_community", m_stOmaCfg.acTrapCommunity, sizeof(m_stOmaCfg.acTrapCommunity)) ||
        m_stOmaCfg.acTrapCommunity[0] == '\0')
    {
        strcpy(m_stOmaCfg.acTrapCommunity, "public");
    }

    if (!pConf->Get("Oma", "hardware_ver", m_stOmaCfg.acHardwareVer, sizeof(m_stOmaCfg.acHardwareVer)) ||
        m_stOmaCfg.acHardwareVer[0] == '\0')
    {
        strcpy(m_stOmaCfg.acHardwareVer, " ");
    }

    if (!pConf->Get("Oma", "dev_name", m_stOmaCfg.acDevName, sizeof(m_stOmaCfg.acDevName)) ||
        m_stOmaCfg.acDevName[0] == '\0')
    {
        strcpy(m_stOmaCfg.acDevName, " ");
    }

    strcpy(m_stOmaCfg.acSoftwareVer, m_szVersion);

    delete pConf;

    bInit = TRUE;

    OmaLog(LOG_INFO, "oma cfg of cpu_alarm_upper is %d and cpu_alarm_lower is %d", m_stOmaCfg.iCpuUsageUpperThreshold, m_stOmaCfg.iCpuUsageLowerThreshold);
    OmaLog(LOG_INFO, "oma cfg of mem_alarm_upper is %d and mem_alarm_lower is %d", m_stOmaCfg.iMemUsageUpperThreshold, m_stOmaCfg.iMemUsageLowerThreshold);
    OmaLog(LOG_INFO, "oma cfg of disk_alarm_upper is %d and disk_alarm_lower is %d", m_stOmaCfg.iDiskUsageUpperThreshold, m_stOmaCfg.iDiskUsageLowerThreshold);

    OmaLog(LOG_INFO, "oma cfg of alarm_check_period is %u", m_stOmaCfg.ulCheckPeriod);
    OmaLog(LOG_INFO, "oma cfg of alarm_check_fail_num is %u", m_stOmaCfg.ulCheckFailNum);

    OmaLog(LOG_INFO, "oma cfg of snmp_ver is %s", m_stOmaCfg.ulSnmpVer==SNMP_V2?"V2":"V1");
    OmaLog(LOG_INFO, "oma cfg of recv_timeout is %u", m_stOmaCfg.ulRecvTimeout);

    OmaLog(LOG_INFO, "omc addr is " IP_FMT, IP_ARG(m_aucOmcAddr));
    OmaLog(LOG_INFO, "oma cfg of oma_mac is " MAC_FMT, MAC_ARG(m_stOmaCfg.aucOmaMac));

    OmaLog(LOG_INFO, "oma cfg of snmp_port is %u", m_stOmaCfg.usSnmpPort);
    OmaLog(LOG_INFO, "oma cfg of snmp_trap_port is %u", m_stOmaCfg.usTrapPort);

    OmaLog(LOG_INFO, "oma cfg of snmp_get_community is %s", m_stOmaCfg.acGetCommunity);
    OmaLog(LOG_INFO, "oma cfg of snmp_set_community is %s", m_stOmaCfg.acSetCommunity);
    OmaLog(LOG_INFO, "oma cfg of snmp_trap_community is %s", m_stOmaCfg.acTrapCommunity);

    m_stOmaCfg.ulRecvTimeout *= 1000;   // 转换成毫秒

    return TRUE;

fail:
    delete pConf;

    return FALSE;
}

static int snmp_parse_get_req(unsigned char ucGetVer, unsigned char *pucRspData, unsigned int ulRspDataLen,
                              unsigned int *pulGetType, unsigned int *pulReqSeq, SNMP_OID_T *pstOID,
                              unsigned int ulMaxOIDNum, unsigned int *pulOIDNum)
{
    unsigned int    ulPDULen;
    unsigned int    ulSnmpPDULen;
    unsigned int    ulParasLen;
    unsigned int    ulPara1Len;
    unsigned char   ucOIDSize;
    unsigned char   ucTmp;
    unsigned char   *pucRsp = pucRspData;

    *pulOIDNum = 0;
    if ((*pucRspData++) != SNMP_ID_SEQUENCE)
    {
        return -1;
    }

    if (pucRspData[0] > 0x80)
    {
        ucTmp = pucRspData[0] - 0x80;
        pucRspData ++;

        ulPDULen = snmp_get_int_value(pucRspData, ucTmp);
        pucRspData += ucTmp;

        if (ulRspDataLen != (1+1+ucTmp+ulPDULen))
        {
            return -1;
        }
    }
    else
    {
        ulPDULen = pucRspData[0];
        pucRspData ++;

        if (ulRspDataLen != (1+1+ulPDULen))
        {
            return -1;
        }
    }

    //ver check
    if (pucRspData[0] != ASN_INTEGER ||
        pucRspData[1] != 1 ||
        pucRspData[2] != ucGetVer)
    {
        return -1;
    }
    pucRspData += (1+1+1);

    //community
    if (pucRspData[0] != ASN_OCTET_STR ||
        pucRspData[1] == 0)
    {
        return -1;
    }
    pucRspData += (1+1+pucRspData[1]);

    //snmp pdu type : get response
    *pulGetType = *pucRspData++;
    if (*pulGetType != SNMP_MSG_GET &&
        *pulGetType != SNMP_MSG_GETNEXT &&
        *pulGetType != SNMP_MSG_GETBULK )
    {
        return -1;
    }

    //snmp pdu len
    if (pucRspData[0] > 0x80)
    {
        ucTmp = pucRspData[0] - 0x80;
        pucRspData ++;

        ulSnmpPDULen = snmp_get_int_value(pucRspData, ucTmp);
        pucRspData += ucTmp;
    }
    else
    {
        ulSnmpPDULen = pucRspData[0];
        pucRspData ++;
    }

    //req id
    if (pucRspData[0] != ASN_INTEGER ||
        pucRspData[1] == 0)
    {
        OmaLog(LOG_ERROR, "snmp_parse_get_req: check reqid failed, pucRspData[0]:%u,pucRspData[1]:%u.",
            pucRspData[0], pucRspData[1]);
        return -1;
    }

    ucTmp = pucRspData[1];
    pucRspData += 2;

    *pulReqSeq = snmp_get_int_value(pucRspData, ucTmp);
    pucRspData += ucTmp;

    //error status
    if (pucRspData[0] != ASN_INTEGER ||
        pucRspData[1] != 1)
    {
        return -1;
    }

    pucRspData += 2;

    if ((*pucRspData++) != SNMP_ERR_NOERROR)
    {
        return -2;
    }

    //error index
    pucRspData += (1+1+1);

    //paras
    if ((*pucRspData++) != SNMP_ID_SEQUENCE)
    {
        return -1;
    }

    if (pucRspData[0] > 0x80)
    {
        ucTmp = pucRspData[0] - 0x80;
        pucRspData ++;

        ulParasLen = snmp_get_int_value(pucRspData, ucTmp);
        pucRspData += ucTmp;
    }
    else
    {
        ulParasLen = pucRspData[0];
        pucRspData ++;
    }

    while(1)
    {
        // para
        if ((*pucRspData++) != SNMP_ID_SEQUENCE)
        {
            return -1;
        }

        if (pucRspData[0] > 0x80)
        {
            ucTmp = pucRspData[0] - 0x80;
            pucRspData ++;

            ulPara1Len = snmp_get_int_value(pucRspData, ucTmp);
            pucRspData += ucTmp;
        }
        else
        {
            ulPara1Len = pucRspData[0];
            pucRspData ++;
        }

        //oid type
        if (pucRspData[0] != ASN_OBJECT_ID ||
            pucRspData[1] == 0)
        {
            return -1;
        }

        ucOIDSize = pucRspData[1];
        pucRspData += 2;

        //check oid
        if (snmp_parse_oid_var(pucRspData, ucOIDSize, pstOID->astOID, &pstOID->ulOIDSize) < 0)
        {
            return -1;
        }

        pucRspData += ucOIDSize;
        if (*pucRspData++ != ASN_NULL)
        {
            return -1;
        }

        if (*pucRspData++ != 0)
        {
            return -1;
        }

        (*pulOIDNum)++;

        if (pucRspData >= pucRsp+ulRspDataLen ||
            (*pulOIDNum) >= ulMaxOIDNum)
        {
            break;
        }

        pstOID++;
    }

    return 0;
}

BOOL ThreadOma::SendGetSysInfoRsp(VOID *pvSession, UINT32 ulReqID)
{
    SNMP_SESSION_T  *pstSnmpSession = (SNMP_SESSION_T*)pvSession;
    SNMP_VAR_T      *pstVarList = NULL;
    INT32           iRet = -1;
    UINT32          ulUptime = gos_get_sys_uptime_1ms()/10; // 10ms
    INT32           iIndex = m_ulOmaInfoNum - 1;
    UINT32          ulClusterStatus = CLUSTER_STATUS_MASTER;

    if (iIndex < 0)
    {
        return TRUE;
    }

#ifndef __BORLANDC__
    if (IsLocalAddr(m_aucMasterAddr))
    {
        ulClusterStatus = CLUSTER_STATUS_MASTER;
    }
    else
    {
        ulClusterStatus = CLUSTER_STATUS_SLAVE;
    }
#endif

    SNMP_ADD_SCALAR_VAR(pstVarList, oid_sysUptime,      TYPE_TIME,  ulUptime);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_currentVersion, TYPE_CHAR,  m_szVersion);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_CpuUsage,       TYPE_INT,   m_astAlarmData[iIndex].iCpuUsage);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_MemUsage,       TYPE_INT,   m_astAlarmData[iIndex].iMemUsage);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_DiskUsage,      TYPE_INT,   m_astAlarmData[iIndex].iDiskUsage);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_sysHardwareVer, TYPE_CHAR,  m_stOmaCfg.acHardwareVer);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_sysSoftwareVer, TYPE_CHAR,  m_stOmaCfg.acSoftwareVer);
#ifndef __BORLANDC__
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_ClusterStatus,  TYPE_INT,   ulClusterStatus);
#endif

    iRet = snmp_send_get_rsp_ex(pstSnmpSession, m_stOmaSocket, ulReqID, pstVarList);
    if (-1 == iRet)
    {
        OmaLog(LOG_ERROR, "send GetSysInfoRsp failed");
        return FALSE;
    }

    OmaLog(LOG_DETAIL, "send GetSysInfoRsp succ");
    return TRUE;
}

BOOL ThreadOma::SendGetSysStatusRsp(VOID *pvSession, UINT32 ulReqID)
{
    SNMP_SESSION_T  *pstSnmpSession = (SNMP_SESSION_T*)pvSession;
    SNMP_VAR_T      *pstVarList = NULL;
    INT32           iRet = -1;
    BOOL            bNTPValid = IsNTPValid();

    SNMP_ADD_SCALAR_VAR(pstVarList, oid_NTPStatus,      TYPE_INT,   bNTPValid);

    iRet = snmp_send_get_rsp_ex(pstSnmpSession, m_stOmaSocket, ulReqID, pstVarList);
    if (-1 == iRet)
    {
        OmaLog(LOG_ERROR, "send GetSysStatusRsp failed");
        return FALSE;
    }

    OmaLog(LOG_DETAIL, "send GetSysStatusRsp succ");
    return TRUE;
}

VOID ThreadOma::HandleSnmpMsg()
{
    UINT32          ulOIDNum;
    UINT32          ulReqID = 0;
    UINT8           aucRecvBuf[4096];
    SOCKADDR_IN     stPeerAddr = {0};
    INT32           iRecvSize;
    UINT32          ulStartTime = gos_get_uptime();
    SNMP_SESSION_T  *pstSnmpSession = NULL;
    SNMP_OID_T      astOID[64] = {0};
    SNMP_OID        *pstOID = astOID[0].astOID;
    UINT32          ulSnmpType = 0xffffffff;
    UINT8           aucNetMask[] = {255,255,255,0};
    CHAR            acOID[256];
    UINT32          ulLen = 0;
    UINT32          i;

recv:
    memset(&astOID, 0, sizeof(astOID));
    ulSnmpType = 0xffffffff;

    iRecvSize = gos_recv_udp_msg(m_stOmaSocket, aucRecvBuf, sizeof(aucRecvBuf), 1000, &stPeerAddr);
    if (iRecvSize < 0)
    {
        OmaLog(LOG_ERROR, "recv snmp msg failed: %s", gos_get_socket_err());
        gos_sleep_1ms(10);

        InitOmaSocket();

        return;
    }

    if (iRecvSize == 0)
    {
        goto end;
    }

    pstSnmpSession = snmp_init_session((UINT8*)&stPeerAddr.sin_addr, ntohs(stPeerAddr.sin_port),
                                       m_stOmaCfg.acGetCommunity, m_stOmaCfg.acSetCommunity,
                                       m_stOmaCfg.ulSnmpVer, m_stOmaCfg.ulSnmpVer, m_stOmaCfg.ulRecvTimeout);

    if (snmp_parse_get_req(m_stOmaCfg.ulSnmpVer, aucRecvBuf, iRecvSize, &ulSnmpType,
                           &ulReqID, astOID, ARRAY_SIZE(astOID), &ulOIDNum) == 0)
    {
        if (ulOIDNum == 0)
        {
            goto end;
        }

        ulLen = 0;
        for (i=0; i<astOID[0].ulOIDSize; i++)
        {
            ulLen += sprintf(acOID+ulLen, ".%u", astOID[0].astOID[i]);
        }

        if (strcmp(".1.3.6.1.4.1.28723.60.2.4.3.1.0", acOID) == 0 ||
            gos_str_like(acOID, (CHAR*)".1.3.6.1.4.1.28723.70.2.1") )
        {
            goto end;
        }

        OmaLog(LOG_DETAIL, "recv snmp msg type %u, oid is %s", ulSnmpType, acOID);
    }
    else if (snmp_parse_set_scalar_req(m_stOmaCfg.ulSnmpVer, aucRecvBuf, iRecvSize, &ulReqID, pstOID, &astOID[0].ulOIDSize) == 0)
    {
        if (memcmp(oid_sysManageState, pstOID, sizeof(oid_sysManageState)) == 0)
        {
            UINT32          ulManageState = 1;

            snmp_send_get_scalar_rsp_ex(pstSnmpSession,  m_stOmaSocket, astOID[0].astOID, astOID[0].ulOIDSize, ulReqID, ASN_INTEGER, sizeof(ulManageState), &ulManageState);

            OmaLog(LOG_INFO, "send set manage state rsp to omc");

            m_bSendDiscovery = TRUE;
        }

        goto end;
    }
    else if (snmp_parse_get_scalar_rsp(m_stOmaCfg.ulSnmpVer, aucRecvBuf, iRecvSize, &ulReqID, pstOID, &astOID[0].ulOIDSize) == 0)
    {
        SNMP_OID    oid_TimeStamp[] = {SNMP_TIMESTAMP_OID};

        if (memcmp(oid_TimeStamp, pstOID, sizeof(oid_TimeStamp)) == 0)
        {
            m_ulLastRecvHeartBeatTime = gos_get_uptime();

            OmaLog(LOG_DETAIL, "get heartbeat rsp from omc");
        }

        goto end;
    }

    if (ulSnmpType == SNMP_MSG_GET)
    {
        if (memcmp(oid_sysUptime, astOID[0].astOID, sizeof(oid_sysUptime)) == 0)// && ulOIDNum == 7)
        {
            SendGetSysInfoRsp((VOID*)pstSnmpSession, ulReqID);
        }
        else if (memcmp(oid_sysMacAddress, astOID[0].astOID, sizeof(oid_sysMacAddress)) == 0)
        {
            snmp_send_get_scalar_rsp_ex(pstSnmpSession, m_stOmaSocket, astOID[0].astOID, astOID[0].ulOIDSize, ulReqID,
                                        ASN_OCTET_STR, sizeof(m_stOmaCfg.aucOmaMac), m_stOmaCfg.aucOmaMac);

            OmaLog(LOG_INFO, "send get mac rsp to omc");
        }
        else if (memcmp(oid_ATSStatus, astOID[0].astOID, sizeof(oid_ATSStatus)) == 0)
        {
            SendGetSysStatusRsp((VOID*)pstSnmpSession, ulReqID);

            OmaLog(LOG_INFO, "send get sys status rsp to omc");
        }
    }
    else if (ulSnmpType == SNMP_MSG_GETNEXT)
    {
         if (memcmp(oid_ipAddress, astOID[0].astOID, sizeof(oid_ipAddress)) == 0)
        {
            if (astOID[0].ulOIDSize == OID_LENGTH(oid_ipAddress))
            {
                snmp_send_get_scalar_rsp_ex(pstSnmpSession, m_stOmaSocket, astOID[0].astOID, astOID[0].ulOIDSize+1,
                                            ulReqID, snmp_get_asn1_type(TYPE_IP),
                                            sizeof(m_stOmaCfg.aucOmaAddr), m_stOmaCfg.aucOmaAddr);
                OmaLog(LOG_INFO, "send get ip addr rsp to omc");
            }
            else
            {
                memset(astOID[0].astOID, 0, sizeof(astOID[0].astOID));
                memcpy(astOID[0].astOID, oid_ipNetMask, sizeof(oid_ipNetMask));
                snmp_send_get_scalar_rsp_ex(pstSnmpSession, m_stOmaSocket, astOID[0].astOID,
                                            OID_LENGTH(oid_ipNetMask)+1, ulReqID, snmp_get_asn1_type(TYPE_IP),
                                            sizeof(aucNetMask), aucNetMask);
            }
        }
        else if (memcmp(oid_ipNetMask, astOID[0].astOID, sizeof(oid_ipNetMask)) == 0)
        {
            if (astOID[0].ulOIDSize == OID_LENGTH(oid_ipNetMask))
            {
                snmp_send_get_scalar_rsp_ex(pstSnmpSession, m_stOmaSocket, astOID[0].astOID, astOID[0].ulOIDSize+1,
                                            ulReqID, snmp_get_asn1_type(TYPE_IP),
                                            sizeof(aucNetMask), aucNetMask);
                OmaLog(LOG_INFO, "send get netmask rsp to omc");
            }
            else
            {
                memset(astOID[0].astOID, 0, sizeof(astOID[0].astOID));
                memcpy(astOID[0].astOID, oid_ipGatewayAddress, sizeof(oid_ipGatewayAddress));
                snmp_send_get_scalar_rsp_ex(pstSnmpSession, m_stOmaSocket, astOID[0].astOID,
                                            OID_LENGTH(oid_ipGatewayAddress)+1, ulReqID, snmp_get_asn1_type(TYPE_IP),
                                            sizeof(m_stOmaCfg.aucGateway), m_stOmaCfg.aucGateway);
            }
        }
        else if (memcmp(oid_ipGatewayAddress, astOID[0].astOID, sizeof(oid_ipGatewayAddress)) == 0)
        {
            if (astOID[0].ulOIDSize == OID_LENGTH(oid_ipGatewayAddress))
            {
                snmp_send_get_scalar_rsp_ex(pstSnmpSession, m_stOmaSocket, astOID[0].astOID, astOID[0].ulOIDSize+1,
                                            ulReqID, snmp_get_asn1_type(TYPE_IP),
                                            sizeof(m_stOmaCfg.aucGateway), m_stOmaCfg.aucGateway);
                OmaLog(LOG_INFO, "send get gateway rsp to omc");
            }
            else
            {
                UINT32      ulUptime = gos_get_sys_uptime_1ms()/10;

                memset(astOID[0].astOID, 0, sizeof(astOID[0].astOID));
                memcpy(astOID[0].astOID, oid_sysUptime, sizeof(oid_sysUptime));
                snmp_send_get_scalar_rsp_ex(pstSnmpSession, m_stOmaSocket, astOID[0].astOID,
                                            OID_LENGTH(oid_sysUptime)+1, ulReqID, snmp_get_asn1_type(TYPE_INT),
                                            sizeof(INT32), &ulUptime);
            }
        }
    }

    // 根据收到OMC的GET操作时间，更新最后收到OMC获取信息的时间
    m_ulLastRecvHeartBeatTime = gos_get_uptime();

end:
    if (pstSnmpSession)
    {
        snmp_free_session(pstSnmpSession);
        pstSnmpSession = NULL;
    }

    if (!m_bRunning)
    {
        return;
    }

    if ((gos_get_uptime() - ulStartTime) <= 5)
    {
        goto recv;
    }
}

VOID ThreadOma::AlarmRaise(UINT32 ulAlarmCode)
{
    ALARM_INFO_T        *pstAlarmInfo;

    if (ulAlarmCode >= ARRAY_SIZE(m_astAlarmInfo) )
    {
        return;
    }

    pstAlarmInfo = &m_astAlarmInfo[ulAlarmCode].stAlarmInfo;

    pstAlarmInfo->ulAlarmStatus = ALARM_STATUS_RAISE;
    if (pstAlarmInfo->ulAlarmStatus == pstAlarmInfo->ulPrevAlarmStatus)
    {
        return;
    }

    if (SendAlarmTrap(&m_astAlarmInfo[ulAlarmCode]))
    {
        pstAlarmInfo->ulPrevAlarmStatus = pstAlarmInfo->ulAlarmStatus;
    }
}

VOID ThreadOma::AlarmClear(UINT32 ulAlarmCode)
{
    ALARM_INFO_T        *pstAlarmInfo;

    if (ulAlarmCode >= ARRAY_SIZE(m_astAlarmInfo) )
    {
        return;
    }

    pstAlarmInfo = &m_astAlarmInfo[ulAlarmCode].stAlarmInfo;

    pstAlarmInfo->ulAlarmStatus = ALARM_STATUS_CLEAR;
    if (pstAlarmInfo->ulAlarmStatus == pstAlarmInfo->ulPrevAlarmStatus)
    {
        return;
    }

    if (SendAlarmTrap(&m_astAlarmInfo[ulAlarmCode]))
    {
        pstAlarmInfo->ulPrevAlarmStatus = pstAlarmInfo->ulAlarmStatus;
    }
}

VOID ThreadOma::Free()
{
    m_bRunning = FALSE;
}

BOOL ThreadOma::CheckAlarm()
{
    ALARM_DATA_T        stAlarmData = {0};
    INT64               i64Total;
    INT64               i64Free;

    // check reboot
    CHAR    acRebootIndFile[256];

    sprintf(acRebootIndFile, "%s/reboot.ind", gos_get_root_path());
    if (gos_file_exist(acRebootIndFile))
    {
        AlarmRaise(REBOOT_ALARM);
        gos_sleep_1ms(100);
        AlarmClear(REBOOT_ALARM);

        gos_delete_file(acRebootIndFile);
    }

    // cpu
    stAlarmData.iCpuUsage = gos_get_cpu_usage();

    // mem
    gos_get_mem_usage(&i64Total, &i64Free);

    if (i64Free <= 0)
    {
        return FALSE;
    }

    stAlarmData.iMemUsage = 100 - (INT32)(100*i64Free/i64Total);

    // disk
    gos_get_disk_usage(&i64Total, &i64Free);

    if (i64Free <= 0)
    {
        return FALSE;
    }

    stAlarmData.iDiskUsage = 100 - (INT32)(100*i64Free/i64Total);
    stAlarmData.bNTPValid = IsNTPValid();
#ifndef __BORLANDC__
    if (IsLocalAddr(m_aucMasterAddr))
    {
        stAlarmData.bIsMasterStatus = TRUE;
    }
    else
    {
        stAlarmData.bIsMasterStatus = FALSE;
    }
#else
    stAlarmData.bIsMasterStatus = FALSE;
#endif

    if (m_ulOmaInfoNum >= m_stOmaCfg.ulCheckFailNum)
    {
        m_ulOmaInfoNum = m_stOmaCfg.ulCheckFailNum;
        memmove(&m_astAlarmData[0], &m_astAlarmData[1], sizeof(m_astAlarmData[0])*(m_ulOmaInfoNum-1));
        memcpy(&m_astAlarmData[m_ulOmaInfoNum-1], &stAlarmData, sizeof(stAlarmData));
    }
    else
    {
        memcpy(&m_astAlarmData[m_ulOmaInfoNum++], &stAlarmData, sizeof(stAlarmData));
    }

    if (m_ulOmaInfoNum < m_stOmaCfg.ulCheckFailNum)
    {
        return FALSE;
    }

    // check
    UINT32  i;
    INT32   aiAlarmStatus[ALARM_NUM];

    memset(aiAlarmStatus, 0, sizeof(aiAlarmStatus));

    for (i=0; i<m_ulOmaInfoNum; i++)
    {
        if (m_astAlarmData[i].iCpuUsage > m_stOmaCfg.iCpuUsageUpperThreshold)
        {
            aiAlarmStatus[CPU_USAGE_ALARM] ++;
        }
        else if (m_astAlarmData[i].iCpuUsage < m_stOmaCfg.iCpuUsageLowerThreshold)
        {
            aiAlarmStatus[CPU_USAGE_ALARM] --;
        }

        if (m_astAlarmData[i].iMemUsage > m_stOmaCfg.iMemUsageUpperThreshold)
        {
            aiAlarmStatus[MEM_USAGE_ALARM] ++;
        }
        else if (m_astAlarmData[i].iMemUsage < m_stOmaCfg.iMemUsageLowerThreshold)
        {
            aiAlarmStatus[MEM_USAGE_ALARM] --;
        }

        if (m_astAlarmData[i].iDiskUsage > m_stOmaCfg.iDiskUsageUpperThreshold)
        {
            aiAlarmStatus[DISK_USAGE_ALARM] ++;
        }
        else if (m_astAlarmData[i].iDiskUsage < m_stOmaCfg.iDiskUsageLowerThreshold)
        {
            aiAlarmStatus[DISK_USAGE_ALARM] --;
        }

        aiAlarmStatus[NTP_ALARM] += m_astAlarmData[i].bNTPValid?-1:1;
    }

    // check switch to master/slave status
    UINT32      ulSum = 0;

    if (!m_astAlarmData[0].bIsMasterStatus)
    {
        for (i=1; i<m_ulOmaInfoNum; i++)
        {
            if (m_astAlarmData[1].bIsMasterStatus)
            {
                ulSum ++;
            }
        }

        if (ulSum == (m_ulOmaInfoNum-1))
        {
            AlarmRaise(SWITCH_TO_MASTER_ALARM);
            gos_sleep_1ms(100);
            AlarmClear(SWITCH_TO_MASTER_ALARM);
        }
    }
    else
    {
        for (i=1; i<m_ulOmaInfoNum; i++)
        {
            if (!m_astAlarmData[1].bIsMasterStatus)
            {
                ulSum ++;
            }
        }

        if (ulSum == (m_ulOmaInfoNum-1))
        {
            AlarmRaise(SWITCH_TO_SLAVE_ALARM);
            gos_sleep_1ms(100);
            AlarmClear(SWITCH_TO_SLAVE_ALARM);
        }
    }

    // check other alarm
    for (i=0; i<ARRAY_SIZE(aiAlarmStatus); i++)
    {
#if 0
        static BOOL bRaise = 1;

        static UINT32      ulAlarmCode = 0;

        if (bRaise)
        {
            AlarmRaise(i);
        }
        else
        {
            AlarmClear(i);
        }

        continue;
#endif

        if (i == SWITCH_TO_MASTER_ALARM ||
            i == SWITCH_TO_SLAVE_ALARM  )
        {
            continue;
        }

        if (aiAlarmStatus[i] == (INT32)m_ulOmaInfoNum)
        {
            AlarmRaise(i);
        }
        else if (aiAlarmStatus[i] == (INT32)(0 - m_ulOmaInfoNum))
        {
            AlarmClear(i);
        }
    }

    return TRUE;
}

VOID ThreadOma::CheckStatus()
{
    InitOmaAddr();

    if (!m_bSendDiscovery)
    {
        SendDiscoveryTrap();
    }

    HandleSnmpMsg();

    if (m_bSendDiscovery)
    {
        if ((gos_get_uptime() - m_ulLastSendHeartBeatTime) > m_ulOmcHeartBeatPeriod)
        {
            SendHeartBeatTrap();

            m_ulLastSendHeartBeatTime = gos_get_uptime();
            if (m_ulLastRecvHeartBeatTime == 0)
            {
                m_ulLastRecvHeartBeatTime = m_ulLastSendHeartBeatTime;
            }

            if ((m_ulLastSendHeartBeatTime - m_ulLastRecvHeartBeatTime) > m_ulMaxHeartBeatFailNum*m_ulOmcHeartBeatPeriod)
            {
                m_bSendDiscovery = FALSE;
                m_ulLastSendHeartBeatTime = 0;
                m_ulLastRecvHeartBeatTime = 0;
            }
        }
    }

    CheckAlarm();
}

GOS_THREAD_RET ThreadOma::ThreadEntry(void* pPara)
{
    OmaLog(LOG_INFO, "thread oma start running");

    if (!Init())
    {
        gos_sleep(1);
    }

    OmaLog(LOG_INFO, "thread oma init succ");

    m_bRunning = TRUE;
    while(m_bRunning)
    {
        CheckStatus();

        gos_sleep_1ms(1);
    }

    OmaLog(LOG_INFO, "thread oma stopped");

    return 0;
}
