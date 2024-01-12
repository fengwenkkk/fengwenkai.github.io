#include "g_include.h"
#include "ds_public.h"
#include "msg.h"
#include "omc.h"
#include "oid.h"
#include "ThreadOma.h"
#include "snmp_nms.h"
#include "snmp_def.h"

extern CHAR             *g_szConfigFile;
extern CHAR             *g_szVersion;
extern BOOL             g_bATSValid;
extern BOOL             g_bNTPValid;
extern BOOL             g_bMDCValid;
extern UINT8            g_aucMdcAddr[4];
extern UINT8            g_aucOmcAddr[4];
BOOL                    g_bSendPackage;
LOG_CFG_T       g_stLogCfg = {0};
#ifndef __BORLANDC__
extern CLUSTER_CFG_T    g_stClusterCfg;
extern BOOL             IsLocalAddr(UINT8 *pucAddr);
#endif

extern UINT32   ulAlarmRaise;
extern UINT32   ulAlarmClear;
extern GMutex  g_MutexAlarmRaise;
extern GMutex  g_MutexAlarmClear;

int SnmpUDPSend(SNMP_SESSION_T *pstSession,
                void *pstReqMsg, unsigned int ulReqMsgLen)
{
    int             iRet = 0;
    SOCKADDR_IN     stSockAddr = {0};
    SOCKADDR        *pstAddr = (SOCKADDR *)&stSockAddr;
    int             iAddrLen = sizeof(SOCKADDR);
    int             iRcvSize;
    unsigned int    i = 0;
    unsigned int    ulRecvTimeout = pstSession->ulTimeout; // 10
    unsigned int    ulSendTime = 1;

    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port   = htons(pstSession->usAgentPort);
    memcpy(&stSockAddr.sin_addr.s_addr, pstSession->aucAgentIP, 4);

    iRet = sendto(pstSession->stSock, (const char*)pstReqMsg, ulReqMsgLen, 0, pstAddr, iAddrLen);
    if (iRet < 0)
    {
        snmp_log(LOG_ERROR, "snmp_udp_send: sendto() failed");
        return -1;
    }

    return 0;
}

int SnmpSendGetScalarRsp(SNMP_SESSION_T *pstSession, SNMP_OID *pOID, unsigned int ulOIDSize,
                         unsigned int ulReqID, unsigned char ucDataType, unsigned int ulDataLen, void *pData)
{
    int             iRet;
    SNMP_GET_SCALAR_RSP_PDU_T  stPDU;
    unsigned int    ulRecvMsgLen;
    unsigned char   *pBuf = NULL;
    unsigned int    ulBufLen = 64*1024;
    unsigned int    ulReqPDULen;
    unsigned char   *pucReqPDU;

    if (!pstSession)
    {
        snmp_log(LOG_ERROR, "snmp_send_get_scalar_rsp: pstSession is null");
        return -1;
    }

    pBuf = (unsigned char *)malloc(ulBufLen);
    if (NULL == pBuf)
    {
        snmp_log(LOG_ERROR, "snmp_send_get_scalar_rsp: malloc failed");
        return -1;
    }

    memset(&stPDU, 0, sizeof(stPDU));
    iRet = snmp_init_get_rsp_pdu(pstSession->acReadCommunity, (UINT8)pstSession->ulGetVer, pOID, ulOIDSize,
                                 ulReqID, (unsigned char *)pData, ucDataType, ulDataLen, &stPDU);
    if (iRet < 0)
    {
        snmp_log(LOG_ERROR, "snmp_send_get_scalar_rsp: snmp_init_set_pdu failed");
        goto fail;
    }

    pucReqPDU = pBuf;
    iRet = snmp_create_get_rsp_pdu(&stPDU, pucReqPDU, ulBufLen, &ulReqPDULen);
    if (iRet < 0)
    {
        snmp_log(LOG_ERROR, "snmp_send_get_scalar_rsp: snmp_create_set_pdu failed");
        goto fail;
    }

    if (snmp_init_socket() < 0)
    {
        snmp_log(LOG_ERROR, "snmp_send_get_scalar_rsp: snmp_init_socket failed");
        goto fail;
    }

    iRet = SnmpUDPSend(pstSession, pucReqPDU, ulReqPDULen);
    if (iRet < 0)
    {
        snmp_log(LOG_ERROR, "snmp_send_get_scalar_rsp: snmp_udp_send failed");
        goto fail;
    }

    free(pBuf);
    snmp_free_get_scalar_rsp_pdu(&stPDU);

    return 0;

fail:
    if (pBuf)
    {
        free(pBuf);
    }

    snmp_free_get_scalar_rsp_pdu(&stPDU);

    return -1;
}

int SnmpSendGetRsp(SNMP_SESSION_T *pstSession, unsigned int ulReqID, SNMP_VAR_T *pstVarList)
{
    int                 iRet;
    UINT32              ulRecvMsgLen = 0;
    SNMP_GET_RSP_PDU_T  stPDU = {0};
    unsigned char       *pBuf = NULL;
    unsigned int        ulBufLen = 64*1024;
    unsigned int        ulReqPDULen;
    unsigned char       *pucReqPDU;

    pBuf = (unsigned char *)malloc(ulBufLen);
    if (NULL == pBuf)
    {
        snmp_log(LOG_ERROR, "snmp_send_get_rsp: malloc failed");
        return -1;
    }

    memset(&stPDU, 0, sizeof(stPDU));
    stPDU.pstVarList = pstVarList;
    iRet = snmp_init_batch_get_rsp_pdu(pstSession->ulGetVer, pstSession->acReadCommunity, ulReqID, &stPDU);
    if (iRet < 0)
    {
        snmp_log(LOG_ERROR, "snmp_send_get_rsp: snmp_init_set_pdu failed");
        goto fail;
    }

    pucReqPDU = pBuf;
    iRet = snmp_create_batch_get_rsp_pdu(&stPDU, pucReqPDU, ulBufLen, &ulReqPDULen);
    if (iRet < 0)
    {
        snmp_log(LOG_ERROR, "snmp_send_get_rsp: snmp_create_trap2_pdu failed");
        goto fail;
    }

    iRet = SnmpUDPSend(pstSession, pucReqPDU, ulReqPDULen);
    if (iRet < 0)
    {
        snmp_log(LOG_ERROR, "snmp_send_get_rsp: snmp_udp_send() failed");
        goto fail;
    }

    free(pBuf);
    snmp_free_get_rsp_pdu(&stPDU);

    return 0;

fail:
    if (pBuf)
    {
        free(pBuf);
    }

    snmp_free_get_rsp_pdu(&stPDU);

    return -1;
}

VOID ThreadOma::InitSnmpSession(UINT8 *pucAgengIP, UINT16 usAgentPort)
{
    memcpy(m_stSnmpSession.aucAgentIP, pucAgengIP, sizeof(m_stSnmpSession.aucAgentIP));
    m_stSnmpSession.usAgentPort = usAgentPort;
    m_stSnmpSession.ulGetVer = g_stOmaCfg.ulSnmpVer;
    m_stSnmpSession.ulSetVer = g_stOmaCfg.ulSnmpVer;
    m_stSnmpSession.ulTimeout = g_stOmaCfg.ulRecvTimeout;

    strcpy(m_stSnmpSession.acReadCommunity, g_stOmaCfg.acGetCommunity);
    strcpy(m_stSnmpSession.acWriteCommunity, g_stOmaCfg.acSetCommunity);

    m_stSnmpSession.stSock = m_stSock;
}

BOOL ThreadOma::SendDiscoveryTrap()
{
    SNMP_VAR_T              *pstVarList = NULL;
    INT32                   iRet = -1;
    CHAR                    acModelName[32] = "";
    UINT32                  ulDefaultInt = 0;
    UINT32                  ulDefaultNum = 2000;
    CHAR                    acDefaultStr[8]={" "};
    CHAR                    acDevName[32] = {0};
    if (m_ulDevType == OMC_DEV_TYPE_DIS)
    {
        strcpy(acModelName, OMC_MODEL_NAME_DIS);
    }
    else if (m_ulDevType == OMC_DEV_TYPE_DC)
    {
        strcpy(acModelName, OMC_MODEL_NAME_DC);
    }
    else if (m_ulDevType == OMC_DEV_TYPE_TX)
    {
        strcpy(acModelName, OMC_MODEL_NAME_TX);
    }
    else if (m_ulDevType == OMC_DEV_TYPE_FX)
    {
        strcpy(acModelName, OMC_MODEL_NAME_FX);
    }

    if (g_stOmaCfg.ulTrainUnitID%2 == 0)
    {
        sprintf(acDevName, "%u-TC1", g_stOmaCfg.ulTrainUnitID+1400);
    }
    else if (g_stOmaCfg.ulTrainUnitID % 2 == 1)
    {
        sprintf(acDevName, "%u-TC2", g_stOmaCfg.ulTrainUnitID+1400);
    }

    SNMP_INIT_TRAP_VAR(pstVarList, oid_DiscoveryTrap);

    SNMP_ADD_TRAP_VAR(pstVarList, oid_sysMacAddress,    TYPE_MAC,   g_stOmaCfg.aucOmaMac);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_sysIpaddress,     TYPE_IP,    g_stOmaCfg.aucOmaAddr);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_softwareVersion,  TYPE_CHAR,  g_stOmaCfg.acSoftwareVer);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_hardwareVersion,  TYPE_CHAR,  g_stOmaCfg.acHardwareVer);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_modelName,        TYPE_CHAR,  acModelName);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_devType,          TYPE_INT,   m_ulDevType);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_sysHotId,         TYPE_CHAR,  acDevName);
//  SNMP_ADD_TRAP_VAR(pstVarList, oid_sysManageState,   TYPE_INT,   ulDefaultInt);
//  SNMP_ADD_TRAP_VAR(pstVarList, oid_modelId,          TYPE_INT,   ulDefaultInt);
//  SNMP_ADD_TRAP_VAR(pstVarList, oid_currentVersion,   TYPE_CHAR,  acDefaultStr);
//  SNMP_ADD_TRAP_VAR(pstVarList, oid_hardwareType,     TYPE_INT,  ulDefaultInt);
//  SNMP_ADD_TRAP_VAR(pstVarList, oid_boardVersion,     TYPE_INT,  ulDefaultInt);
//  SNMP_ADD_TRAP_VAR(pstVarList, oid_sysModel,         TYPE_CHAR,  acDefaultStr);
//  SNMP_ADD_TRAP_VAR(pstVarList, oid_sysName,          TYPE_CHAR,  acDefaultStr);
//  SNMP_ADD_TRAP_VAR(pstVarList, oid_sysHotId,         TYPE_CHAR,  acDefaultStr);
//  SNMP_ADD_TRAP_VAR(pstVarList, oid_sendOfflineConfigFile,    TYPE_CHAR,  acDefaultStr);
//  SNMP_ADD_TRAP_VAR(pstVarList, oid_sysMaxStaNum,     TYPE_INT,  ulDefaultNum);
//  SNMP_ADD_TRAP_VAR(pstVarList, oid_memorySize,       TYPE_INT,  ulDefaultInt);

//  stSnmpSession.usAgentPort = g_usOmcTrapPort;
//  strcpy(stSnmpSession.acCommunity, g_acOmcTrapCommunity);
//  stSnmpSession.ulTimeout = 5;
    iRet = snmp_send_trap(m_stSock, g_aucOmcAddr, g_stOmaCfg.usTrapPort, g_stOmaCfg.acTrapCommunity,
                          g_stOmaCfg.ulRecvTimeout, SNMP_MSG_TRAP2, pstVarList);
    if (-1 == iRet)
    {
        GosLog(MODULE_OMA, LOG_ERROR, "send discovery trap failed");
        return FALSE;
    }

    GosLog(LOG_INFO, "Dev("GOS_MAC_FMT") send discovery trap succ", GOS_MAC_ARG(g_stOmaCfg.aucOmaMac));
    return TRUE;
}

BOOL ThreadOma::SendHeartBeatTrap()
{
    SNMP_VAR_T              *pstVarList = NULL;
    UINT32                  ulDefaultInt = 0;
    INT32                   iRet = 0;

    SNMP_INIT_TRAP_VAR(pstVarList, oid_HeartBeatTrap);

    SNMP_ADD_TRAP_VAR(pstVarList, oid_sysMacAddress,        TYPE_MAC,   g_stOmaCfg.aucOmaMac);

    iRet = snmp_send_trap(m_stSock, g_aucOmcAddr, g_stOmaCfg.usTrapPort, g_stOmaCfg.acTrapCommunity,
                          g_stOmaCfg.ulRecvTimeout, SNMP_MSG_INFORM, pstVarList);
    if (-1 == iRet)
    {
        GosLog(LOG_ERROR, "send heart beat trap failed");
        return FALSE;
    }

    GosLog(LOG_DETAIL, "send heart beat trap succ");
    return TRUE;
}

VOID ThreadOma::InitAlarmInfo()
{
    OMA_ALARM_INFO_T    *pstInfo;

    memset(g_astAlarmInfo, 0, sizeof(g_astAlarmInfo));

    pstInfo = &g_astAlarmInfo[REBOOT_ALARM];
    memcpy(pstInfo->astOID, oid_RebootTrap, sizeof(oid_RebootTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_RebootTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_Reboot;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_CRITICAL;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_DEV;
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "设备重启");
    pstInfo->stAlarmInfo.bIsNotifyAlarm = TRUE;

    pstInfo = &g_astAlarmInfo[CPU_USAGE_ALARM];
    memcpy(pstInfo->astOID, oid_CpuUsageTooHighTrap, sizeof(oid_CpuUsageTooHighTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_CpuUsageTooHighTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_CpuUsageTooHigh;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_DEV;
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "CPU使用率过高");

    pstInfo = &g_astAlarmInfo[MEM_USAGE_ALARM];
    memcpy(pstInfo->astOID, oid_MemUsageTooHighTrap, sizeof(oid_MemUsageTooHighTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_MemUsageTooHighTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_MemUsageTooHigh;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_DEV;
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "内存使用率过高");

    pstInfo = &g_astAlarmInfo[DISK_USAGE_ALARM];
    memcpy(pstInfo->astOID, oid_DiskUsageTooHighTrap, sizeof(oid_DiskUsageTooHighTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_DiskUsageTooHighTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_DiskUsageTooHigh;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_DEV;
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "硬盘使用率过高");

    pstInfo = &g_astAlarmInfo[SWITCH_TO_MASTER_ALARM];
    memcpy(pstInfo->astOID, oid_SwitchToMasterTrap, sizeof(oid_SwitchToMasterTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_SwitchToMasterTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_SwitchToMaster;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_DEV;
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "切换到主用状态");
    pstInfo->stAlarmInfo.bIsNotifyAlarm = TRUE;

    pstInfo = &g_astAlarmInfo[SWITCH_TO_SLAVE_ALARM];
    memcpy(pstInfo->astOID, oid_SwitchToSlaveTrap, sizeof(oid_SwitchToSlaveTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_SwitchToSlaveTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_SwitchToSlave;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_DEV;
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "切换到备用状态");
    pstInfo->stAlarmInfo.bIsNotifyAlarm = TRUE;

    pstInfo = &g_astAlarmInfo[NTP_ALARM];
    memcpy(pstInfo->astOID, oid_SyncTimeFailTrap, sizeof(oid_SyncTimeFailTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_SyncTimeFailTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_SyncTimeFail;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_COMM;
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "时钟同步失败");

    pstInfo = &g_astAlarmInfo[ATS_ALARM];
    memcpy(pstInfo->astOID, oid_GetATSDataFailTrap, sizeof(oid_GetATSDataFailTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_GetATSDataFailTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_GetATSDataFail;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_COMM;
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "获取ATS数据失败");

    pstInfo = &g_astAlarmInfo[LTE_ALARM];
    memcpy(pstInfo->astOID, oid_LinkToLteFailTrap, sizeof(oid_LinkToLteFailTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_LinkToLteFailTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_LinkToLteFail;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_COMM;
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "集群服务器不可达");

    pstInfo = &g_astAlarmInfo[SELF_CHECK_FAIL_ALARM];
    memcpy(pstInfo->astOID, oid_SelfCheckFailTrap, sizeof(oid_SelfCheckFailTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_SelfCheckFailTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_SelfCheckFailed;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_COMM;
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "设备自检失败");

    pstInfo = &g_astAlarmInfo[EMEGENCY_CALL_ALARM];
    memcpy(pstInfo->astOID, oid_EmergencyCallTrap, sizeof(oid_EmergencyCallTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_EmergencyCallTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_EmergencyCall;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_COMM;
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "设备发起紧急呼叫");

    pstInfo = &g_astAlarmInfo[EMERGENCY_BRACE_ALARM];
    memcpy(pstInfo->astOID, oid_EmergencyBraceTrap, sizeof(oid_EmergencyBraceTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_EmergencyBraceTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_EmergencyBrace;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_COMM;
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "紧急停车");

    pstInfo = &g_astAlarmInfo[DEV_TEMP_ALARM];
    memcpy(pstInfo->astOID, oid_DevTempTrap, sizeof(oid_DevTempTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_DevTempTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_DevTempTooHigh;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_COMM;
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "设备温度过高");

    pstInfo = &g_astAlarmInfo[IPH_LINK_FAIL_ALARM];
    memcpy(pstInfo->astOID, oid_IPHLinkFailTrap, sizeof(oid_IPHLinkFailTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_IPHLinkFailTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_PISLinkFail;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_COMM;
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "设备与 PIS 的连接失败");

    pstInfo = &g_astAlarmInfo[PA_LINK_FAIL_ALARM];
    memcpy(pstInfo->astOID, oid_PALinkFailTrap, sizeof(oid_PALinkFailTrap));
    pstInfo->ulOIDSize = ARRAY_SIZE(oid_PALinkFailTrap);
    pstInfo->stAlarmInfo.ulAlarmID = SNMP_ALARM_CODE_PALinkFail;
    pstInfo->stAlarmInfo.ulAlarmLevel = ALARM_LEVEL_MAIN;
    pstInfo->stAlarmInfo.ulAlarmType = ALARM_TYPE_COMM;
    strcpy(pstInfo->stAlarmInfo.acAlarmTitle, "设备与 PA 的连接失败");
}

BOOL ThreadOma::SendAlarmTrap(OMA_ALARM_INFO_T *pstAlarmInfo)
{
    SNMP_VAR_T      *pstVarList = NULL;
    INT32           iRet = -1;

    if (m_bStopSend)
    {
        return TRUE;
    }

    if (snmp_varlist_add_var(&pstVarList, SNMP_TRAP, OID_LENGTH(SNMP_TRAP), snmp_get_asn1_type(TYPE_OID),
        (UINT8*)pstAlarmInfo->astOID, pstAlarmInfo->ulOIDSize*sizeof(SNMP_OID)) < 0)
    {
        return FALSE;
    }

    pstAlarmInfo->stAlarmInfo.ulAlarmRaiseTime = gos_get_current_time();

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
    SNMP_ADD_TRAP_VAR(pstVarList, oid_AlarmNeName,   TYPE_MAC,  g_stOmaCfg.aucOmaMac);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_AlarmLevel,    TYPE_INT,  pstAlarmInfo->stAlarmInfo.ulAlarmLevel);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_AlarmType,     TYPE_INT,  pstAlarmInfo->stAlarmInfo.ulAlarmType);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_AlarmReasonID, TYPE_INT,  pstAlarmInfo->stAlarmInfo.ulAlarmReasonID);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_AlarmReason,   TYPE_CHAR, pstAlarmInfo->stAlarmInfo.acAlarmReason);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_AlarmRaiseTime,TYPE_TIME, pstAlarmInfo->stAlarmInfo.ulAlarmRaiseTime);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_AlarmStatus,   TYPE_INT,  pstAlarmInfo->stAlarmInfo.ulAlarmStatus);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_AlarmTitle,    TYPE_CHAR, pstAlarmInfo->stAlarmInfo.acAlarmTitle);
    SNMP_ADD_TRAP_VAR(pstVarList, oid_AlarmInfo,     TYPE_CHAR, pstAlarmInfo->stAlarmInfo.acAlarmInfo);

    iRet = snmp_send_trap(m_stSock, g_aucOmcAddr, g_stOmaCfg.usTrapPort, g_stOmaCfg.acTrapCommunity,
                          g_stOmaCfg.ulRecvTimeout, SNMP_MSG_TRAP2, pstVarList);
    if (-1 == iRet)
    {
        GosLog(MODULE_OMA, LOG_ERROR, "send alarm trap failed");
        return FALSE;
    }

    if (pstAlarmInfo->stAlarmInfo.ulAlarmStatus == ALARM_STATUS_RAISE)
    {
        g_MutexAlarmRaise.Mutex();
        ++ulAlarmRaise;
        g_MutexAlarmRaise.Unmutex();
    }
    else
    {
      g_MutexAlarmClear.Mutex();
      ++ulAlarmClear;
      g_MutexAlarmClear.Unmutex();
    }
    GosLog(LOG_INFO, "Dev("GOS_MAC_FMT") send alarm(%d) trap succ", GOS_MAC_ARG(g_stOmaCfg.aucOmaMac), pstAlarmInfo->stAlarmInfo.ulAlarmID);
    return TRUE;
}

ThreadOma::ThreadOma(UINT32 ulDevType, BOOL bTestMode):GThread(NULL)
{
    m_ulDevType = ulDevType;
    m_bTestMode = bTestMode;

    memset(&g_stOmaCfg, 0, sizeof(g_stOmaCfg));
    memset(g_abAlarmStatus, 0, sizeof(g_abAlarmStatus));
    memset(&m_stSnmpSession, 0, sizeof(m_stSnmpSession));
    m_bRunning = FALSE;

    m_InitOmaCfg = FALSE;
    m_iAlarmTest = -1;

    memset(&g_stOmaCfg, 0, sizeof(g_stOmaCfg));

    m_stSock = INVALID_SOCKET;
    g_ulOmaInfoNum = 0;
    memset(g_astAlarmData, 0, sizeof(g_astAlarmData));
    memset(g_abAlarmStatus, 0, sizeof(g_abAlarmStatus));
    memset(g_astAlarmInfo, 0, sizeof(g_astAlarmInfo));

    m_ulOmcHeartBeatPeriod = 30;
    m_ulHeartBeatFailNum = 0;
    m_ulMaxHeartBeatFailNum = 3;
    m_ulLastSendHeartBeatTime = 0;
    m_ulLastRecvHeartBeatTime = 0;
    m_bSendDiscovery = FALSE;
    m_bStopSend = FALSE;

}

BOOL ThreadOma::InitOmaAddr()
{
    if (GET_INT(g_stOmaCfg.aucOmaAddr) == 0)
    {
        DOUBLE  dDelay;
        UINT16  usSeq;
        UINT32  ulTTL;

        if (!gos_ping(g_aucOmcAddr, 32, 2000, &dDelay, &usSeq, &ulTTL, g_stOmaCfg.aucOmaAddr))
        {
            GosLog(LOG_ERROR, "get oma addr to failed");
            return FALSE;
        }

        GosLog(LOG_INFO, "oma addr is "IP_FMT, IP_ARG(g_stOmaCfg.aucOmaAddr));
    }

    return TRUE;
}

BOOL ThreadOma::Init(CHAR *szOmaMac, UINT16 usSnmpPort, UINT32 ulDevNumber)
{
    if (!InitOmaCfg(szOmaMac, usSnmpPort, ulDevNumber))
    {
        GosLog(LOG_ERROR, "init oma cfg failed");
        return FALSE;
    }

    if (!InitOmaAddr())
    {
        GosLog(LOG_ERROR, "init oma addr failed");
        return FALSE;
    }

    if (m_stSock == INVALID_SOCKET)
    {
        UINT8   aucLocalAddr[4] = {0,0,0,0};

        m_stSock = gos_create_udp_server_socket(aucLocalAddr, g_stOmaCfg.usSnmpPort);
        if (m_stSock == INVALID_SOCKET)
        {
            GosLog(LOG_ERROR, "init snmp socket failed");
            return FALSE;
        }
    }

    InitAlarmInfo();

    return TRUE;
}

BOOL ThreadOma::InitOmaCfg(CHAR *szOmaMac, UINT16 usSnmpPort, UINT32 ulDevNumber)
{
    //BOOL  bInit = FALSE;
    CHAR    *szValue;
    INT32   iValue;

    if (m_InitOmaCfg)
    {
        return TRUE;
    }

    CHAR        acCfgFile[256];

    sprintf(acCfgFile, "%s/%s", gos_get_root_path(), g_szConfigFile);

    if (!gos_file_exist(acCfgFile))
    {
        GosLog(LOG_ERROR, "file %s does not exist", acCfgFile);
        return FALSE;
    }

    GConfFile   *pConf = new GConfFile(acCfgFile);

    if (szOmaMac)
    {
        szValue = szOmaMac;
    }
    else
    {
        szValue = pConf->Get("Oma", "oma_mac");
        if (!szValue)
        {
            GosLog(LOG_ERROR, "no oma_mac in %s", g_szConfigFile);
            goto fail;
        }
    }

    if (!gos_get_mac(szValue, g_stOmaCfg.aucOmaMac))
    {
        GosLog(LOG_ERROR, "invalid oma_mac in %s", g_szConfigFile);
        goto fail;
    }

    pConf->Get("Log", "log_level", LOG_INFO, &iValue);
    g_stLogCfg.bLogToStdout = FALSE;
    g_stLogCfg.bLogToFile = TRUE;
    g_stLogCfg.ulLogLevel = iValue;


    pConf->Get("Oma", "cpu_alarm_upper", 60, &g_stOmaCfg.iCpuUsageUpperThreshold);
    pConf->Get("Oma", "cpu_alarm_lower", 30, &g_stOmaCfg.iCpuUsageLowerThreshold);

    if (g_stOmaCfg.iCpuUsageUpperThreshold <= g_stOmaCfg.iCpuUsageLowerThreshold ||
        g_stOmaCfg.iCpuUsageUpperThreshold >= 100 ||
        g_stOmaCfg.iCpuUsageLowerThreshold == 0)
    {
        GosLog(LOG_ERROR, "invalid cpu alarm value in %s", g_szConfigFile);
        goto fail;
    }

    pConf->Get("Oma", "mem_alarm_upper", 60, &g_stOmaCfg.iMemUsageUpperThreshold);
    pConf->Get("Oma", "mem_alarm_lower", 40, &g_stOmaCfg.iMemUsageLowerThreshold);

    if (g_stOmaCfg.iMemUsageUpperThreshold <= g_stOmaCfg.iMemUsageLowerThreshold ||
        g_stOmaCfg.iMemUsageUpperThreshold >= 100 ||
        g_stOmaCfg.iMemUsageLowerThreshold == 0)
    {
        GosLog(LOG_ERROR, "invalid mem alarm value in %s", g_szConfigFile);
        goto fail;
    }

    pConf->Get("Oma", "disk_alarm_upper", 80, &g_stOmaCfg.iDiskUsageUpperThreshold);
    pConf->Get("Oma", "disk_alarm_lower", 50, &g_stOmaCfg.iDiskUsageLowerThreshold);

    if (g_stOmaCfg.iDiskUsageUpperThreshold <= g_stOmaCfg.iDiskUsageLowerThreshold ||
        g_stOmaCfg.iDiskUsageUpperThreshold >= 100 ||
        g_stOmaCfg.iDiskUsageLowerThreshold == 0)
    {
        GosLog(LOG_ERROR, "invalid disk alarm value in %s", g_szConfigFile);
        goto fail;
    }

    pConf->Get("Oma", "alarm_check_period", 15, &g_stOmaCfg.ulCheckPeriod);
    if (g_stOmaCfg.ulCheckPeriod < 5)
    {
        g_stOmaCfg.ulCheckPeriod = 5;
    }
    else if (g_stOmaCfg.ulCheckPeriod > 60)
    {
        g_stOmaCfg.ulCheckPeriod = 60;
    }

    pConf->Get("Oma", "alarm_check_fail_num", 10, &g_stOmaCfg.ulCheckFailNum);
    if (g_stOmaCfg.ulCheckFailNum < 5)
    {
        g_stOmaCfg.ulCheckFailNum = 5;
    }
    else if (g_stOmaCfg.ulCheckFailNum > ARRAY_SIZE(g_astAlarmData))
    {
        g_stOmaCfg.ulCheckFailNum = ARRAY_SIZE(g_astAlarmData);
    }

    pConf->Get("Oma", "snmp_ver", SNMP_V2, &g_stOmaCfg.ulSnmpVer);
    if (g_stOmaCfg.ulSnmpVer != SNMP_V1 &&
        g_stOmaCfg.ulSnmpVer != SNMP_V2 )
    {
        g_stOmaCfg.ulSnmpVer = SNMP_V2;
    }

    pConf->Get("Oma", "recv_timeout", 5, &g_stOmaCfg.ulRecvTimeout);
    if (g_stOmaCfg.ulRecvTimeout < 2)
    {
        g_stOmaCfg.ulRecvTimeout = 2;
    }
    else if (g_stOmaCfg.ulRecvTimeout > 10)
    {
        g_stOmaCfg.ulRecvTimeout = 10;
    }

    if (usSnmpPort == 0)
    {
        pConf->Get("Oma", "snmp_port", SNMP_PORT, &g_stOmaCfg.usSnmpPort);
        if (g_stOmaCfg.usSnmpPort == 0)
        {
            g_stOmaCfg.usSnmpPort = SNMP_PORT;
        }
    }
    else
    {
        g_stOmaCfg.usSnmpPort = usSnmpPort;
    }

    pConf->Get("Oma", "snmp_trap_port", SNMP_TRAP_PORT, &g_stOmaCfg.usTrapPort);
    if (g_stOmaCfg.usTrapPort == 0)
    {
        g_stOmaCfg.usTrapPort = SNMP_TRAP_PORT;
    }

    if (!pConf->Get("Oma", "snmp_get_community", g_stOmaCfg.acGetCommunity, sizeof(g_stOmaCfg.acGetCommunity)) ||
        g_stOmaCfg.acGetCommunity[0] == '\0')
    {
        strcpy(g_stOmaCfg.acGetCommunity, "public");
    }

    if (!pConf->Get("Oma", "snmp_set_community", g_stOmaCfg.acSetCommunity, sizeof(g_stOmaCfg.acSetCommunity)) ||
        g_stOmaCfg.acSetCommunity[0] == '\0')
    {
        strcpy(g_stOmaCfg.acSetCommunity, "private");
    }

    if (!pConf->Get("Oma", "snmp_trap_community", g_stOmaCfg.acTrapCommunity, sizeof(g_stOmaCfg.acTrapCommunity)) ||
        g_stOmaCfg.acTrapCommunity[0] == '\0')
    {
        strcpy(g_stOmaCfg.acTrapCommunity, "public");
    }

    if (!pConf->Get("Oma", "hardware_ver", g_stOmaCfg.acHardwareVer, sizeof(g_stOmaCfg.acHardwareVer)) ||
        g_stOmaCfg.acHardwareVer[0] == '\0')
    {
        strcpy(g_stOmaCfg.acHardwareVer, " ");
    }

    strcpy(g_stOmaCfg.acSoftwareVer, g_szVersion);

    delete pConf;
    g_stOmaCfg.ulTrainUnitID = ulDevNumber;
    g_stOmaCfg.ulTCID = ulDevNumber;
    m_InitOmaCfg = TRUE;

    memcpy(g_stOmaCfg.aucGateway, g_aucMdcAddr, sizeof(g_stOmaCfg.aucGateway));
    g_stOmaCfg.aucGateway[3] = 254; // TODO 目前写死

    GosLog(LOG_INFO, "oma cfg of cpu_alarm_upper is %d and cpu_alarm_lower is %d", g_stOmaCfg.iCpuUsageUpperThreshold, g_stOmaCfg.iCpuUsageLowerThreshold);
    GosLog(LOG_INFO, "oma cfg of mem_alarm_upper is %d and mem_alarm_lower is %d", g_stOmaCfg.iMemUsageUpperThreshold, g_stOmaCfg.iMemUsageLowerThreshold);
    GosLog(LOG_INFO, "oma cfg of disk_alarm_upper is %d and disk_alarm_lower is %d", g_stOmaCfg.iDiskUsageUpperThreshold, g_stOmaCfg.iDiskUsageLowerThreshold);

    GosLog(LOG_INFO, "oma cfg of alarm_check_period is %u", g_stOmaCfg.ulCheckPeriod);
    GosLog(LOG_INFO, "oma cfg of alarm_check_fail_num is %u", g_stOmaCfg.ulCheckFailNum);

    GosLog(LOG_INFO, "oma cfg of snmp_ver is %s", g_stOmaCfg.ulSnmpVer==SNMP_V2?"V2":"V1");
    GosLog(LOG_INFO, "oma cfg of recv_timeout is %u", g_stOmaCfg.ulRecvTimeout);

    GosLog(LOG_INFO, "omc addr is "IP_FMT, IP_ARG(g_aucOmcAddr));
//  GosLog(LOG_INFO, "oma cfg of oma_addr is "IP_FMT, IP_ARG(g_stOmaCfg.aucOmaAddr));
    GosLog(LOG_INFO, "oma cfg of oma_mac is "MAC_FMT, MAC_ARG(g_stOmaCfg.aucOmaMac));

    GosLog(LOG_INFO, "oma cfg of snmp_port is %u", g_stOmaCfg.usSnmpPort);
    GosLog(LOG_INFO, "oma cfg of snmp_trap_port is %u", g_stOmaCfg.usTrapPort);

    GosLog(LOG_INFO, "oma cfg of snmp_get_community is %s", g_stOmaCfg.acGetCommunity);
    GosLog(LOG_INFO, "oma cfg of snmp_set_community is %s", g_stOmaCfg.acSetCommunity);
    GosLog(LOG_INFO, "oma cfg of snmp_trap_community is %s", g_stOmaCfg.acTrapCommunity);

    g_stOmaCfg.ulRecvTimeout *= 1000;   // 转换成毫秒

    return TRUE;

fail:
    delete pConf;

    return FALSE;
}

VOID ThreadOma::HandleGetMacReq()
{
    SNMP_OID    astOID[64];
    UINT32      ulOIDSize = ARRAY_SIZE(astOID);
    UINT32      ulReqID = 0;
    UINT8           aucRecvBuf[4096];
    SOCKADDR_IN     stPeerAddr;
    INT32           iRecvSize;
    UINT32          ulStartTime = gos_get_uptime();

recv:
    iRecvSize = gos_recv_udp_msg(m_stSock, aucRecvBuf, sizeof(aucRecvBuf), g_stOmaCfg.ulRecvTimeout, &stPeerAddr);

    if (iRecvSize > 0 &&
        snmp_parse_get_scalar_req(g_stOmaCfg.ulSnmpVer, aucRecvBuf, iRecvSize, &ulReqID, astOID, &ulOIDSize) == 0 &&
        memcmp(oid_sysMacAddress, astOID, sizeof(oid_sysMacAddress)) == 0)
    {
        UINT16  usPort = ntohs(stPeerAddr.sin_port);

        InitSnmpSession((UINT8*)&stPeerAddr.sin_addr, usPort);

//        m_stSnmpSession.stSock = m_stSock;
        SnmpSendGetScalarRsp(&m_stSnmpSession, astOID, ulOIDSize, ulReqID, ASN_OCTET_STR,
                                 sizeof(g_stOmaCfg.aucOmaMac), g_stOmaCfg.aucOmaMac);

//        m_stSnmpSession.stSock = INVALID_SOCKET;
//        snmp_free_session(pstSnmpSession);

        GosLog(LOG_INFO, "send get mac rsp to omc");

     //   g_bSendDiscovery = TRUE;

        HandleSetManageStateReq();

        return;
    }
    else
    {
        if ((gos_get_uptime() - ulStartTime) <= 5)
        {
            goto recv;
        }

        return;
    }
}

VOID ThreadOma::HandleSetManageStateReq()
{
    SNMP_OID    astOID[64];
    UINT32      ulOIDSize = ARRAY_SIZE(astOID);
    UINT32      ulReqID = 0;
    UINT8           aucRecvBuf[4096];
    SOCKADDR_IN     stPeerAddr;
    INT32           iRecvSize;
    UINT32          ulStartTime = gos_get_uptime();
    UINT32          ulManageState = 1;

recv:
    iRecvSize = gos_recv_udp_msg(m_stSock, aucRecvBuf, sizeof(aucRecvBuf), g_stOmaCfg.ulRecvTimeout, &stPeerAddr);

    if (iRecvSize <= 0)
    {
        goto end;
    }

    if (snmp_parse_set_scalar_req(g_stOmaCfg.ulSnmpVer, aucRecvBuf, iRecvSize, &ulReqID, astOID, &ulOIDSize) < 0)
    {
        goto end;
    }

    printf("OID = ");
    for (UINT32 i=0; i<ulOIDSize; i++)
    {
        printf("%d,", astOID[i]);
    }

    printf("\n");

    if (memcmp(oid_sysManageState, astOID, sizeof(oid_sysManageState)) == 0)
    {
        UINT16  usPort = ntohs(stPeerAddr.sin_port);

        InitSnmpSession((UINT8*)&stPeerAddr.sin_addr, usPort);

        SnmpSendGetScalarRsp(&m_stSnmpSession, astOID, ulOIDSize, ulReqID, ASN_INTEGER, sizeof(ulManageState), &ulManageState);

    //    snmp_free_session(pstSnmpSession);

        GosLog(LOG_INFO, "send set manage state rsp to omc");

        m_bSendDiscovery = TRUE;

        return;
    }

end:
    if ((gos_get_uptime() - ulStartTime) <= 5)
    {
        goto recv;
    }
}

BOOL ThreadOma::HandleGetHeartBeatRsp()
{
    SNMP_OID    astOID[64];
    UINT32      ulOIDSize = ARRAY_SIZE(astOID);
    UINT32      ulReqID = snmp_get_last_req_id();
    UINT32      ulRspID = 0;
    UINT8           aucRecvBuf[4096];
    SOCKADDR_IN     stPeerAddr;
    INT32           iRecvSize;
    UINT32          ulStartTime = gos_get_uptime();
//  SNMP_SESSION_T  *pstSnmpSession = NULL;

    return TRUE;

recv:
    iRecvSize = gos_recv_udp_msg(m_stSock, aucRecvBuf, sizeof(aucRecvBuf), g_stOmaCfg.ulRecvTimeout, &stPeerAddr);

    if (iRecvSize > 0 &&
        snmp_parse_get_scalar_rsp(g_stOmaCfg.ulSnmpVer, aucRecvBuf, iRecvSize, &ulRspID, astOID, &ulOIDSize) == 0 &&
        ulReqID == ulRspID)
        //memcmp(oid_sysMacAddress, astOID, sizeof(oid_sysMacAddress)) == 0)
    {
        GosLog(LOG_DETAIL, "get heart beat rsp");

        return TRUE;
    }
    else
    {
        if ((gos_get_uptime() - ulStartTime) <= 5)
        {
            goto recv;
        }

        GosLog(LOG_ERROR, "get heart beat rsp failed");

        return FALSE;
    }
}

int snmp_parse_get_req(unsigned char ucGetVer, unsigned char *pucRspData, unsigned int ulRspDataLen,
                       unsigned int *pulGetType, unsigned int *pulReqSeq, SNMP_OID_T *pstOID, unsigned int ulMaxOIDNum, unsigned int *pulOIDNum)
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

    /*if ((*pucRspData++) != SNMP_MSG_GET)
    {
        return -1;
    }*/

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
        GosLog(LOG_ERROR, "snmp_parse_get_req: pucRspData[0]:%u,pucRspData[1]:%u.",
            pucRspData[0],
            pucRspData[1]);
        GosLog(LOG_ERROR, "snmp_parse_get_req: check reqid failed");
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

BOOL ThreadOma::SendGetSysInfoRsp(UINT32 ulReqID)
{
//    SNMP_SESSION_T  *pstSnmpSession = (SNMP_SESSION_T*)pvSession;
    SNMP_VAR_T      *pstVarList = NULL;
    INT32           iRet = -1;
    UINT32          ulUptime = gos_get_sys_uptime_1ms()/10; // 10ms
    INT32           iIndex = g_ulOmaInfoNum - 1;
    UINT32          ulClusterStatus = CLUSTER_STATUS_MASTER;
    CHAR            acAndroidVer[32] = {"Android20211207"};
    CHAR            acRadioVer[32] = {"EV75120211207"};
    INT32           iTemp = MOD(gos_get_rand(), 30)+20;
    UINT32          ulTC = g_stOmaCfg.ulTCID%2+1;
    if (iIndex < 0)
    {
        return TRUE;
    }

    if (m_bStopSend)
    {
        return TRUE;
    }
#ifndef __BORLANDC__
    if (IsLocalAddr(g_stClusterCfg.aucMasterAddr))
    {
        ulClusterStatus = CLUSTER_STATUS_MASTER;
    }
    else
    {
        ulClusterStatus = CLUSTER_STATUS_SLAVE;
    }
#endif

    SNMP_ADD_SCALAR_VAR(pstVarList, oid_sysUptime,      TYPE_TIME,  ulUptime);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_currentVersion, TYPE_CHAR,  g_szVersion);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_CpuUsage,       TYPE_INT,   g_astAlarmData[iIndex].iCpuUsage);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_MemUsage,       TYPE_INT,   g_astAlarmData[iIndex].iMemUsage);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_DiskUsage,      TYPE_INT,   g_astAlarmData[iIndex].iDiskUsage);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_sysHardwareVer, TYPE_CHAR,  g_stOmaCfg.acHardwareVer);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_sysSoftwareVer, TYPE_CHAR,  g_stOmaCfg.acSoftwareVer);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_sysAndroidVer,  TYPE_CHAR,  acAndroidVer);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_sysRadioVer,    TYPE_CHAR,  acRadioVer);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_CpuTemp,        TYPE_INT,   iTemp);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_TCID,           TYPE_INT,   ulTC);


    if (m_ulDevType == OMC_DEV_TYPE_DIS)
    {
        SNMP_ADD_SCALAR_VAR(pstVarList, oid_ClusterStatus,  TYPE_INT,   ulClusterStatus);
    }

    iRet = SnmpSendGetRsp(&m_stSnmpSession, ulReqID, pstVarList);
    if (-1 == iRet)
    {
        GosLog(MODULE_OMA, LOG_ERROR, "send GetSysInfoRspfailed");
        return FALSE;
    }

    GosLog(MODULE_OMA, LOG_DETAIL, "send GetSysInfoRsp succ");
    return TRUE;
}

BOOL ThreadOma::SendGetDISStatusRsp(UINT32 ulReqID)
{
    SNMP_VAR_T      *pstVarList = NULL;
    INT32           iRet = -1;

    if (m_bStopSend)
    {
        return TRUE;
    }
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_ATSStatus,      TYPE_INT,   g_bATSValid);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_NTPStatus,      TYPE_INT,   g_bNTPValid);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_LTEStatus,      TYPE_INT,   g_bMDCValid);

    iRet = SnmpSendGetRsp(&m_stSnmpSession, ulReqID, pstVarList);
    if (-1 == iRet)
    {
        GosLog(LOG_ERROR, "send GetSysStatusRsp failed");
        return FALSE;
    }

    GosLog(LOG_DETAIL, "send GetSysStatusRsp succ");
    return TRUE;
}

BOOL GetRandBool()
{
    return 1 & gos_get_rand();
}

BOOL ThreadOma::SendGetTXStatusRsp(UINT32 ulReqID)
{
    SNMP_VAR_T      *pstVarList = NULL;
    INT32           iRet = -1;
    UINT8           aucNTPAddr[] = {192,0,1,120};

    UINT32  ulGatewayReachable     = GetRandBool();
    UINT32  ulMDCRegStatus         = GetRandBool();
    UINT32  ulOMCLinkStatus        = GetRandBool();
    UINT32  ulDISLinkStatus        = GetRandBool();
    UINT32  ulSelfCheckStatus      = GetRandBool();
    UINT32  ulDevCoverStatus       = GetRandBool();
    UINT32  ulDriveMode            = GetRandBool();
    UINT32  ulEmergencyCallStatus  = GetRandBool();
    UINT32  ulEmergencyBraceStatus = GetRandBool();
    UINT32  ulIPHLinkStatus        = GetRandBool();
    UINT32  ulPALinkStatus         = GetRandBool();
    UINT32  ulNTPPort = 123;
    UINT32  ulNTPSyncTime = 30;

    if (m_bStopSend)
    {
        return TRUE;
    }
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_GatewayReachable    , TYPE_INT, ulGatewayReachable    );
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_MDCRegStatus        , TYPE_INT, ulMDCRegStatus        );
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_OMCLinkStatus       , TYPE_INT, ulOMCLinkStatus       );
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_DISLinkStatus       , TYPE_INT, ulDISLinkStatus       );
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_SelfCheckStatus     , TYPE_INT, ulSelfCheckStatus     );
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_DevCoverStatus      , TYPE_INT, ulDevCoverStatus      );
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_DriveMode           , TYPE_INT, ulDriveMode           );
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_EmergencyCallStatus , TYPE_INT, ulEmergencyCallStatus );
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_EmergencyBraceStatus, TYPE_INT, ulEmergencyBraceStatus);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_PISLinkStatus       , TYPE_INT, ulIPHLinkStatus       );
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_PALinkStatus        , TYPE_INT, ulPALinkStatus        );

//  SNMP_ADD_SCALAR_VAR(pstVarList, oid_NTPAddr,            TYPE_IP,    aucNTPAddr);
//  SNMP_ADD_SCALAR_VAR(pstVarList, oid_NTPPort,            TYPE_INT,   ulNTPPort);
//  SNMP_ADD_SCALAR_VAR(pstVarList, oid_NTPSyncTime,        TYPE_INT,   ulNTPSyncTime);

    iRet = SnmpSendGetRsp(&m_stSnmpSession, ulReqID, pstVarList);
    if (-1 == iRet)
    {
        GosLog(LOG_ERROR, "send GetTXStatusRsp failed");
        return FALSE;
    }

    GosLog(LOG_DETAIL, "send GetTXStatusRsp succ");
    return TRUE;
}

BOOL ThreadOma::SendGetLteStatusRsp(UINT32 ulReqID)
{
    SNMP_VAR_T      *pstVarList = NULL;
    INT32           iRet = -1;
    CHAR            acPCI[32];
    INT32           iRSRP = MOD(gos_get_rand(), 30) - 113;
    INT32           iRSRQ = MOD(gos_get_rand(), 16) - 19;
    INT32           iSINR = MOD(gos_get_rand(), 30);
    INT32           iRSSI = MOD(gos_get_rand(), 30) - 100;
	INT32           iStationID =  MOD(gos_get_rand(), 34);

    if (m_bStopSend)
    {
        return TRUE;
    }
    sprintf(acPCI, "%u", MOD(gos_get_rand(),200)+1);

	// sz14 现场 StaionID 区间是在 11-33 之间 99111 为其它
	if (iStationID < 11)
	{
		iStationID = 99111;
	}

    SNMP_ADD_SCALAR_VAR(pstVarList, oid_LteRSSI,    TYPE_INT,   iRSSI);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_LteRSRP,    TYPE_INT,   iRSRP);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_LteRSRQ,    TYPE_INT,   iRSRQ);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_LteSINR,    TYPE_INT,   iSINR);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_LtePCI,     TYPE_CHAR,  acPCI);
    SNMP_ADD_SCALAR_VAR(pstVarList, oid_StationID,  TYPE_INT,   iStationID);
    iRet = SnmpSendGetRsp(&m_stSnmpSession, ulReqID, pstVarList);
    if (-1 == iRet)
    {
        GosLog(LOG_ERROR, "send SendGetLteStatusRsp failed");
        return FALSE;
    }

    GosLog(LOG_DETAIL, "send SendGetLteStatusRsp succ");
    return TRUE;
}

VOID ThreadOma::HandleSnmpReq()
{
    UINT32          ulOIDNum;
    UINT32          ulReqID = 0;
    UINT8           aucRecvBuf[4096];
    SOCKADDR_IN     stPeerAddr = {0};
    INT32           iRecvSize;
    UINT32          ulStartTime = gos_get_uptime();
    SNMP_OID_T      astOID[16] = {0};
    SNMP_OID        *pstOID = astOID[0].astOID;
    UINT32          ulSnmpType = 0xffffffff;
//    UINT32          ulTime = 0;
    UINT8           aucNetMask[] = {255,255,255,0};
    INT32           iCpuUsage = 0;
    CHAR            acOID[256];
    UINT32          ulLen = 0;
    UINT32          i;

recv:
    memset(&astOID, 0, sizeof(astOID));
    ulSnmpType = 0xffffffff;

    iRecvSize = gos_recv_udp_msg(m_stSock, aucRecvBuf, sizeof(aucRecvBuf), 1000, &stPeerAddr);
    if (iRecvSize <= 0)
    {
        goto end;
    }

    m_ulLastRecvHeartBeatTime = gos_get_uptime();
    InitSnmpSession((UINT8*)&stPeerAddr.sin_addr, ntohs(stPeerAddr.sin_port));

    if (snmp_parse_get_req(g_stOmaCfg.ulSnmpVer, aucRecvBuf, iRecvSize, &ulSnmpType, &ulReqID, astOID, ARRAY_SIZE(astOID), &ulOIDNum) == 0)
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

        if (//strcmp(".1.3.6.1.4.1.28723.60.2.4.3.1.0", acOID) == 0 ||
            gos_str_like(acOID, (CHAR*)".1.3.6.1.4.1.28723.70.2.1") )
        {
            goto end;
        }

        GosLog(LOG_DETAIL, "recv snmp msg type %u, oid is %s", ulSnmpType, acOID);
    }
    else if (snmp_parse_set_scalar_req(g_stOmaCfg.ulSnmpVer, aucRecvBuf, iRecvSize, &ulReqID, pstOID, &astOID[0].ulOIDSize) == 0)
    {
        if (memcmp(oid_sysManageState, pstOID, sizeof(oid_sysManageState)) == 0)
        {
            UINT32          ulManageState = 1;

            SnmpSendGetScalarRsp(&m_stSnmpSession, astOID[0].astOID, astOID[0].ulOIDSize, ulReqID, ASN_INTEGER, sizeof(ulManageState), &ulManageState);

            GosLog(LOG_INFO, "send set manage state rsp to omc");

            m_bSendDiscovery = TRUE;
        }

        goto end;
    }
    else if (snmp_parse_get_scalar_rsp(g_stOmaCfg.ulSnmpVer, aucRecvBuf, iRecvSize, &ulReqID, pstOID, &astOID[0].ulOIDSize) == 0)
    {
        SNMP_OID    oid_TimeStamp[] = {SNMP_TIMESTAMP_OID};

        if (memcmp(oid_TimeStamp, pstOID, sizeof(oid_TimeStamp)) == 0)
        {
            m_ulLastRecvHeartBeatTime = gos_get_uptime();

            GosLog(LOG_DETAIL, "get heartbeat rsp from omc");
        }

        goto end;
    }

    if (ulSnmpType == SNMP_MSG_GET)
    {
        if (memcmp(oid_sysUptime, astOID[0].astOID, sizeof(oid_sysUptime)) == 0)// && ulOIDNum == 7)
        {
            SendGetSysInfoRsp(ulReqID);
        }
        else if (memcmp(oid_sysMacAddress, astOID[0].astOID, sizeof(oid_sysMacAddress)) == 0)
        {
            SnmpSendGetScalarRsp(&m_stSnmpSession, astOID[0].astOID, astOID[0].ulOIDSize, ulReqID, ASN_OCTET_STR,
                                     sizeof(g_stOmaCfg.aucOmaMac), g_stOmaCfg.aucOmaMac);

            GosLog(LOG_INFO, "send get mac rsp to omc");
        }
        else if (memcmp(oid_ATSStatus, astOID[0].astOID, sizeof(oid_ATSStatus)) == 0)
        {
            SendGetDISStatusRsp(ulReqID);
        }
        else if (memcmp(oid_GatewayReachable, astOID[0].astOID, sizeof(oid_GatewayReachable)) == 0)
        {
            SendGetTXStatusRsp(ulReqID);
        }
        else if (memcmp(oid_LteRSSI, astOID[0].astOID, sizeof(oid_LteRSSI)) == 0)
        {
            SendGetLteStatusRsp(ulReqID);
        }
    }
    else if (ulSnmpType == SNMP_MSG_GETNEXT)
    {
         if (memcmp(oid_ipAddress, astOID[0].astOID, sizeof(oid_ipAddress)) == 0)
        {
            if (astOID[0].ulOIDSize == OID_LENGTH(oid_ipAddress))
            {
                SnmpSendGetScalarRsp(&m_stSnmpSession, astOID[0].astOID, astOID[0].ulOIDSize+1, ulReqID, snmp_get_asn1_type(TYPE_IP),
                                         sizeof(g_stOmaCfg.aucOmaAddr), g_stOmaCfg.aucOmaAddr);
                GosLog(LOG_INFO, "send get ip addr rsp to omc");
            }
            else
            {
                memset(astOID[0].astOID, 0, sizeof(astOID[0].astOID));
                memcpy(astOID[0].astOID, oid_ipNetMask, sizeof(oid_ipNetMask));
                SnmpSendGetScalarRsp(&m_stSnmpSession, astOID[0].astOID, OID_LENGTH(oid_ipNetMask)+1, ulReqID, snmp_get_asn1_type(TYPE_IP),
                                         sizeof(aucNetMask), aucNetMask);
               // GosLog(LOG_INFO, "send get ip addr rsp to omc");
            }
        }
        else if (memcmp(oid_ipNetMask, astOID[0].astOID, sizeof(oid_ipNetMask)) == 0)
        {
            if (astOID[0].ulOIDSize == OID_LENGTH(oid_ipNetMask))
            {
                SnmpSendGetScalarRsp(&m_stSnmpSession, astOID[0].astOID, astOID[0].ulOIDSize+1, ulReqID, snmp_get_asn1_type(TYPE_IP),
                                         sizeof(aucNetMask), aucNetMask);
                GosLog(LOG_INFO, "send get netmask rsp to omc");
            }
            else
            {
                memset(astOID[0].astOID, 0, sizeof(astOID[0].astOID));
                memcpy(astOID[0].astOID, oid_ipGatewayAddress, sizeof(oid_ipGatewayAddress));
                SnmpSendGetScalarRsp(&m_stSnmpSession, astOID[0].astOID, OID_LENGTH(oid_ipGatewayAddress)+1, ulReqID, snmp_get_asn1_type(TYPE_IP),
                                         sizeof(g_stOmaCfg.aucGateway), g_stOmaCfg.aucGateway);
            }
        }
        else if (memcmp(oid_ipGatewayAddress, astOID[0].astOID, sizeof(oid_ipGatewayAddress)) == 0)
        {
            if (astOID[0].ulOIDSize == OID_LENGTH(oid_ipNetMask))
            {
                SnmpSendGetScalarRsp(&m_stSnmpSession, astOID[0].astOID, astOID[0].ulOIDSize+1, ulReqID, snmp_get_asn1_type(TYPE_IP),
                                        sizeof(g_stOmaCfg.aucGateway), g_stOmaCfg.aucGateway);
                GosLog(LOG_INFO, "send get gateway rsp to omc");
            }
            else
            {
                memset(astOID[0].astOID, 0, sizeof(astOID[0].astOID));
                memcpy(astOID[0].astOID, oid_CpuUsage, sizeof(oid_CpuUsage));
                SnmpSendGetScalarRsp(&m_stSnmpSession, astOID[0].astOID, OID_LENGTH(oid_CpuUsage)+1, ulReqID, snmp_get_asn1_type(TYPE_INT),
                                         sizeof(INT32), &iCpuUsage);
            }
        }
    }

end:
 /*   if (pstSnmpSession)
    {
        snmp_free_session(pstSnmpSession);
        pstSnmpSession = NULL;
    }   */

    if (!m_bRunning)
    {
        return;
    }

    if ((gos_get_uptime() - ulStartTime) <= 10)
    {
        goto recv;
    }
}

VOID ThreadOma::AlarmRaise(UINT32 ulAlarmCode)
{
    // 过滤不同设备能够产生的告警
    if (ulAlarmCode >= ARRAY_SIZE(g_astAlarmInfo) ||
        ulAlarmCode == SWITCH_TO_MASTER_ALARM     ||
        ulAlarmCode == SWITCH_TO_SLAVE_ALARM      ||
        ulAlarmCode == NTP_ALARM                  ||
        ulAlarmCode == ATS_ALARM                  ||
        ulAlarmCode == LTE_ALARM                  ||
        ulAlarmCode == COVER_OPENED_ALARM         ||
        ulAlarmCode == IF_STATUS_ALARM)
    {
        return;
    }

    g_astAlarmInfo[ulAlarmCode].stAlarmInfo.ulAlarmStatus = ALARM_STATUS_RAISE;
    SendAlarmTrap(&g_astAlarmInfo[ulAlarmCode]);
}

VOID ThreadOma::AlarmClear(UINT32 ulAlarmCode)
{
    if (ulAlarmCode >= ARRAY_SIZE(g_astAlarmInfo) ||
        ulAlarmCode == SWITCH_TO_MASTER_ALARM     ||
        ulAlarmCode == SWITCH_TO_SLAVE_ALARM      ||
        ulAlarmCode == NTP_ALARM                  ||
        ulAlarmCode == ATS_ALARM                  ||
        ulAlarmCode == LTE_ALARM                  ||
        ulAlarmCode == COVER_OPENED_ALARM         ||
        ulAlarmCode == IF_STATUS_ALARM)
    {
        return;
    }

    g_astAlarmInfo[ulAlarmCode].stAlarmInfo.ulAlarmStatus = ALARM_STATUS_CLEAR;
    SendAlarmTrap(&g_astAlarmInfo[ulAlarmCode]);
}

VOID ThreadOma::Free()
{
    m_bRunning = FALSE;
}

BOOL ThreadOma::CheckAlarm()
{
    DIS_ALARM_DATA_T    stAlarmData = {0};
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

    stAlarmData.bNTPValid = g_bNTPValid;
    stAlarmData.bATSValid = g_bATSValid;
    stAlarmData.bLTEValid = g_bMDCValid;

#ifndef __BORLANDC__
    if (IsLocalAddr(g_stClusterCfg.aucMasterAddr))
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

    if (g_ulOmaInfoNum >= g_stOmaCfg.ulCheckFailNum)
    {
        g_ulOmaInfoNum = g_stOmaCfg.ulCheckFailNum;
        memmove(&g_astAlarmData[0], &g_astAlarmData[1], sizeof(g_astAlarmData[0])*(g_ulOmaInfoNum-1));
        memcpy(&g_astAlarmData[g_ulOmaInfoNum-1], &stAlarmData, sizeof(stAlarmData));
    }
    else
    {
        memcpy(&g_astAlarmData[g_ulOmaInfoNum++], &stAlarmData, sizeof(stAlarmData));
    }

    if (g_ulOmaInfoNum < g_stOmaCfg.ulCheckFailNum)
    {
        return FALSE;
    }

    // check
    UINT32  i;
    INT32   aiAlarmStatus[ALARM_NUM];

    memset(aiAlarmStatus, 0, sizeof(aiAlarmStatus));

    for (i=0; i<g_ulOmaInfoNum; i++)
    {
        if (g_astAlarmData[i].iCpuUsage > g_stOmaCfg.iCpuUsageUpperThreshold)
        {
            aiAlarmStatus[CPU_USAGE_ALARM] ++;
        }
        else if (g_astAlarmData[i].iCpuUsage < g_stOmaCfg.iCpuUsageLowerThreshold)
        {
            aiAlarmStatus[CPU_USAGE_ALARM] --;
        }

        if (g_astAlarmData[i].iMemUsage > g_stOmaCfg.iMemUsageUpperThreshold)
        {
            aiAlarmStatus[MEM_USAGE_ALARM] ++;
        }
        else if (g_astAlarmData[i].iMemUsage < g_stOmaCfg.iMemUsageLowerThreshold)
        {
            aiAlarmStatus[MEM_USAGE_ALARM] --;
        }

        if (g_astAlarmData[i].iDiskUsage > g_stOmaCfg.iDiskUsageUpperThreshold)
        {
            aiAlarmStatus[DISK_USAGE_ALARM] ++;
        }
        else if (g_astAlarmData[i].iDiskUsage < g_stOmaCfg.iDiskUsageLowerThreshold)
        {
            aiAlarmStatus[DISK_USAGE_ALARM] --;
        }

        aiAlarmStatus[NTP_ALARM] += g_astAlarmData[i].bNTPValid?-1:1;
        aiAlarmStatus[ATS_ALARM] += g_astAlarmData[i].bATSValid?-1:1;
        aiAlarmStatus[LTE_ALARM] += g_astAlarmData[i].bLTEValid?-1:1;
    }

    // check switch to master/slave status
    UINT32      ulSum = 0;

    if (!g_astAlarmData[0].bIsMasterStatus)
    {
        for (i=1; i<g_ulOmaInfoNum; i++)
        {
            if (g_astAlarmData[1].bIsMasterStatus)
            {
                ulSum ++;
            }
        }

        if (ulSum == (g_ulOmaInfoNum-1))
        {
            AlarmRaise(SWITCH_TO_MASTER_ALARM);
            gos_sleep_1ms(100);
            AlarmClear(SWITCH_TO_MASTER_ALARM);
        }
    }
    else
    {
        for (i=1; i<g_ulOmaInfoNum; i++)
        {
            if (!g_astAlarmData[1].bIsMasterStatus)
            {
                ulSum ++;
            }
        }

        if (ulSum == (g_ulOmaInfoNum-1))
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
            gos_sleep_1ms(1);
        }
        else
        {
            AlarmClear(i);
            gos_sleep_1ms(1);
        }

        continue;
#endif

        if (i == SWITCH_TO_MASTER_ALARM ||
            i == SWITCH_TO_SLAVE_ALARM  )
        {
            continue;
        }

        if (aiAlarmStatus[i] == (INT32)g_ulOmaInfoNum &&
            !g_abAlarmStatus[i])
        {
            AlarmRaise(i);
        }
        else if (aiAlarmStatus[i] == (INT32)(0 - g_ulOmaInfoNum) &&
                 g_abAlarmStatus[i])
        {
            AlarmClear(i);
        }
    }

    return TRUE;
}

VOID ThreadOma::AlarmTest()
{
    if (m_iAlarmTest < 0)
    {
        return;
    }

    // check other alarm
    for (UINT32 i=0; i<ALARM_NUM; i++)
    {
        if (m_iAlarmTest > 0)
        {
            AlarmRaise(i);
			// 若告警是可忽略的告警，则在告警产生之后100ms就发送告警清除
			/*
			if (g_astAlarmInfo[i].stAlarmInfo.bIsNotifyAlarm)
			{
				gos_sleep_1ms(100);
				AlarmClear(i);
			}
            */
        }
        else
        {
            AlarmClear(i);
        }
    }

    m_iAlarmTest = -1;
}

GOS_THREAD_RET ThreadOma::ThreadEntry(void* pPara)
{
    GosLog(LOG_INFO, "thread oma start running");

    m_bRunning = TRUE;

    while(m_bRunning)
    {
        AlarmTest();

        if (!m_bSendDiscovery)
        {
            SendDiscoveryTrap();
        }

        HandleSnmpReq();

        if (m_bSendDiscovery)
        {
            if ((gos_get_uptime() - m_ulLastSendHeartBeatTime) > m_ulOmcHeartBeatPeriod)
            {
               SendHeartBeatTrap();
               m_ulLastSendHeartBeatTime = gos_get_uptime();

               // omc 不回心跳包，所以根据最后一次omc GET 时间判断
               if (m_ulLastRecvHeartBeatTime == 0)
               {
                    m_ulLastRecvHeartBeatTime =  m_ulLastSendHeartBeatTime;
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

        gos_sleep_1ms(1);
    }

    GosLog(LOG_INFO, "thread oma stopped");
    CLOSE_SOCKET(m_stSock);

    return 0;
}

