#ifndef ThreadOmaH
#define ThreadOmaH

#include "g_include.h"
#include "omc.h"

typedef struct
{
    INT32       iCpuUsage;
    INT32       iMemUsage;
    INT32       iDiskUsage;

    BOOL        bIsMasterStatus;

    BOOL        bNTPValid;
    BOOL        bATSValid;
    BOOL        bLTEValid;
}ALARM_DATA_T;

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
    CHAR        acDevName[64];
}OMA_CFG_T;

class ThreadOma: public GThread
{
public :
    ThreadOma(const CHAR *szConfigFile, const CHAR *szVersion, UINT8 *pucMdcAddr, UINT8 *pucOmcAddr, UINT8 *pucMasterAddr);

    BOOL Init();
    VOID Free();
    virtual GOS_THREAD_RET ThreadEntry(void* pPara);

private:
    BOOL m_bRunning;

    CHAR                *m_szConfigFile;
    CHAR                *m_szVersion;
    UINT8               m_aucMdcAddr[4];
    UINT8               m_aucOmcAddr[4];
    UINT8               m_aucMasterAddr[4];

    OMA_CFG_T           m_stOmaCfg;

    SOCKET              m_stOmaSocket;

    UINT32              m_ulOmaInfoNum;     // µ±Ç°¸æ¾¯¼ì²â²âÊÔ
    ALARM_DATA_T        m_astAlarmData[16];
    OMA_ALARM_INFO_T    m_astAlarmInfo[ALARM_NUM];

    UINT32              m_ulOmcHeartBeatPeriod;
    UINT32              m_ulMaxHeartBeatFailNum;
    UINT32              m_ulLastSendHeartBeatTime;
    UINT32              m_ulLastRecvHeartBeatTime;
    BOOL                m_bSendDiscovery;
    BOOL                m_bSendRebootAlarm;

    VOID CheckStatus();

    BOOL SendDiscoveryTrap();
    BOOL SendHeartBeatTrap();
    BOOL SendAlarmTrap(OMA_ALARM_INFO_T *pstAlarmInfo);
    BOOL SendGetSysInfoRsp(VOID *pvSession, UINT32 ulReqID);
    BOOL SendGetSysStatusRsp(VOID *pvSession, UINT32 ulReqID);

    BOOL InitOmaCfg();
    VOID InitAlarmInfo();
    BOOL InitOmaSocket();
    BOOL InitOmaAddr();
    BOOL CheckAlarm();

    VOID HandleSnmpMsg();

    VOID AlarmRaise(UINT32 ulAlarmCode);
    VOID AlarmClear(UINT32 ulAlarmCode);
};

#endif
