#ifndef ThreadOmaH
#define ThreadOmaH

#include "omc.h"
#include "oid.h"

/*
// 设备类型
#define OMC_DEV_TYPE_HX         5   // 车载台
#define OMC_DEV_TYPE_FX         6   // 固定台
#define OMC_DEV_TYPE_DC         7   // 调度台
#define OMC_DEV_TYPE_DIS        8   // 调度服务器

// 设备类型
#define DIS_MODEL_NAME          "DIS-S2000"
#define DC_MODEL_NAME           "DIS-C2000"

// 告警等级
#define ALARM_LEVEL_CRITICAL    1
#define ALARM_LEVEL_MAIN        2
#define ALARM_LEVEL_MINOR       3
#define ALARM_LEVEL_NORMAL      4
#define ALARM_LEVEL_UNKNOWN     5

// 告警类型
#define ALARM_TYPE_COMM         1   // 通信
#define ALARM_TYPE_ENV          2   // 环境
#define ALARM_TYPE_DEV          3   // 设备
#define ALARM_TYPE_FAIL         4   // 处理失败
#define ALARM_TYPE_QOS          5   // QOS
#define ALARM_TYPE_OTHER        6

// 告警状态
#define ALARM_STATUS_RAISE      1   // 告警产生
#define ALARM_STATUS_CLEAR      2   // 告警清除

// 告警码
typedef enum
{
    REBOOT_ALARM,
    CPU_USAGE_ALARM,
    MEM_USAGE_ALARM,
    DISK_USAGE_ALARM,

    SWITCH_TO_MASTER_ALARM,
    SWITCH_TO_SLAVE_ALARM,

    NTP_ALARM,
    ATS_ALARM,
    LTE_ALARM,

    ALARM_NUM
}OMA_ALARM_CODE_E;

typedef struct
{
    UINT32      astOID[32];
    UINT32      ulOIDSize;          // OID个数
    UINT32      ulAlarmSn;
//  CHAR        acAlarmNeName[32];  // MAC地址
    UINT32      ulAlarmLevel;
    UINT32      ulAlarmType;
    BOOL        bIsNotifyAlarm;
    UINT32      ulAlarmReasonId;
    CHAR        acAlarmReason[32];
    UINT32      ulAlarmRaiseTime;
    UINT32      ulAlarmStatus;
    CHAR        acAlarmTitle[64];
    CHAR        acAlarmInfo[64];
}OMA_ALARM_INFO_T;
*/

typedef struct
{
//  BOOL        bReboot;

    INT32       iCpuUsage;
    INT32       iMemUsage;
    INT32       iDiskUsage;

    BOOL        bIsMasterStatus;

    BOOL        bNTPValid;
    BOOL        bATSValid;
    BOOL        bLTEValid;
}DIS_ALARM_DATA_T;

typedef struct
{
    INT32       iCpuUsageUpperThreshold;
    INT32       iCpuUsageLowerThreshold;

    INT32       iMemUsageUpperThreshold;
    INT32       iMemUsageLowerThreshold;

    INT32       iDiskUsageUpperThreshold;
    INT32       iDiskUsageLowerThreshold;

    UINT32      ulCheckPeriod;
    UINT32      ulCheckFailNum;

    UINT32      ulSnmpVer;
    UINT32      ulRecvTimeout;

    UINT8       aucOmaAddr[4];
    UINT8       aucOmaMac[6];

    UINT8       aucGateway[4];

    UINT16      usSnmpPort;
    UINT16      usTrapPort;

    CHAR        acGetCommunity[32];
    CHAR        acSetCommunity[32];
    CHAR        acTrapCommunity[32];

    CHAR        acSoftwareVer[64];
    CHAR        acHardwareVer[64];
    UINT32      ulTrainUnitID;
    UINT32      ulTCID;
}OMA_CFG_T;

class ThreadOma: public GThread
{
public :
    ThreadOma(UINT32 ulDevType, BOOL bTestMode=FALSE);

    BOOL Init(CHAR *szOmaMac, UINT16 usSnmpPort, UINT32 ulDevNumber);
    VOID Free();
    BOOL IsOnline(){return m_bSendDiscovery;};

    VOID AlarmTest(INT32 iAlarmTest){m_iAlarmTest = iAlarmTest;};

    VOID SendPackageStatus(BOOL bIsStopSend){m_bStopSend = bIsStopSend;};

    virtual GOS_THREAD_RET ThreadEntry(void* pPara);

private:
    UINT32              m_ulDevType;
    BOOL                m_bTestMode;

    SNMP_SESSION_T      m_stSnmpSession;

    BOOL                m_InitOmaCfg;
    OMA_CFG_T           g_stOmaCfg;

    SOCKET              m_stSock;

    INT32               m_iAlarmTest;

    UINT32              g_ulOmaInfoNum;
    DIS_ALARM_DATA_T    g_astAlarmData[16];
    BOOL                g_abAlarmStatus[ALARM_NUM];
    OMA_ALARM_INFO_T    g_astAlarmInfo[ALARM_NUM];

    UINT32              m_ulOmcHeartBeatPeriod;
    UINT32              m_ulHeartBeatFailNum;
    UINT32              m_ulMaxHeartBeatFailNum;
    UINT32              m_ulLastSendHeartBeatTime;
    UINT32              m_ulLastRecvHeartBeatTime;
    BOOL                m_bSendDiscovery;
    BOOL                m_bStopSend;
    CHAR                m_acDevName[32];
    BOOL m_bRunning;

    BOOL SendDiscoveryTrap();
    BOOL SendHeartBeatTrap();
    BOOL SendAlarmTrap(OMA_ALARM_INFO_T *pstAlarmInfo);
    BOOL SendGetSysInfoRsp(UINT32 ulReqID);
    BOOL SendGetTXStatusRsp(UINT32 ulReqID);
    BOOL SendGetLteStatusRsp(UINT32 ulReqID);
    BOOL SendGetDISStatusRsp(UINT32 ulReqID);

    VOID InitAlarmInfo();
    BOOL InitOmaCfg(CHAR *szOmaMac, UINT16 usSnmpPort, UINT32 ulDevNumber);
    BOOL InitOmaAddr();
    BOOL CheckAlarm();

    VOID InitSnmpSession(UINT8 *pucAgengIP, UINT16 usAgentPort);

    VOID HandleGetMacReq();
    VOID HandleSetManageStateReq();
    BOOL HandleGetHeartBeatRsp();
    VOID HandleSnmpReq();

    VOID AlarmTest();
    VOID AlarmRaise(UINT32 ulAlarmCode);
    VOID AlarmClear(UINT32 ulAlarmCode);


};

#endif
