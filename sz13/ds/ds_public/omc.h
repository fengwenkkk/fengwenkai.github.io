#ifndef OMC_H
#define OMC_H

#include "snmp_nms.h"

// �豸����
#define OMC_DEV_TYPE_ALL        0xff// ȫ��
#define OMC_DEV_TYPE_TAU        3   // TAU
#define OMC_DEV_TYPE_TX         5   // ����̨
#define OMC_DEV_TYPE_FX         6   // �̶�̨
#define OMC_DEV_TYPE_DC         7   // ����̨
#define OMC_DEV_TYPE_DIS        8   // ���ȷ�����
#define OMC_DEV_TYPE_REC        9   // ¼��¼�������
#define OMC_DEV_TYPE_MASTER_DIS 10  // ���ȷ�����(����)
#define OMC_DEV_TYPE_SLAVE_DIS  11  // ���ȷ�����(����)
#define OMC_DEV_TYPE_OMC        12  // ���ܷ�����
#define OMC_DEV_TYPE_SPT        13  // ¼��¼���ն�

#define OMC_DEV_TYPE_EOMC       30  // eOMC��������
#define OMC_DEV_TYPE_ZTE        35  // ZTE��������

#define OMC_DEV_TYPE_NORTH      40  // ��������

// �豸����
#define OMC_MODEL_NAME_TX       "H3000-TX"      // ����̨�ͺ�
#define OMC_MODEL_NAME_FX       "H3000-FX"      // �̶�̨�ͺ�
#define OMC_MODEL_NAME_DC       "DIS-C3000"     // ����̨�ͺ�
#define OMC_MODEL_NAME_DIS      "DIS-S3000"     // ���ȷ������ͺ�
#define OMC_MODEL_NAME_REC      "REC-S3000"     // ¼��¼��������ͺ�
#define OMC_MODEL_NAME_SPT      "SPT-S3000"     // ¼��¼���ն��ͺ�

#define OMC_MODEL_NAME_EOMC     "eOMC"          // eOMC�ͺ�
#define OMC_MODEL_NAME_ZTE      "ZTE"           // ZTE�ͺ�

#define OMC_NE_ID_EOMC          "eOMC"          // eOMC
#define OMC_NE_ID_ZTE           "ZTE"           // ZTE

// �澯�ȼ�
#define ALARM_LEVEL_UNKNOWN     0xff
#define ALARM_LEVEL_ALL         0xfff
#define ALARM_LEVEL_CRITICAL    1   // �����澯
#define ALARM_LEVEL_MAIN        2   // ��Ҫ�澯
#define ALARM_LEVEL_MINOR       3   // ��Ҫ�澯
#define ALARM_LEVEL_NORMAL      4   // ��ͨ�澯
#define ALARM_LEVEL_CLEAR       6
#define ALARM_LEVEL_EVENT       0

// �澯����
#define ALARM_TYPE_COMM         1   // ͨ��
#define ALARM_TYPE_ENV          2   // ����
#define ALARM_TYPE_DEV          3   // �豸
#define ALARM_TYPE_FAIL         4   // ����ʧ��
#define ALARM_TYPE_QOS          5   // QOS
#define ALARM_TYPE_OTHER        6

// �澯״̬
#define ALARM_STATUS_UNKNOWN    0   // �澯״̬δ֪
#define ALARM_STATUS_RAISE      1   // �澯����
#define ALARM_STATUS_CLEAR      2   // �澯���
#define ALARM_STATUS_IGNORE     3   // �澯���� add by tzl

// �澯�����ʽ
#define ALARM_CLEAR_AUTO        "�Զ����"  // �Զ����
#define ALARM_CLEAR_OFFLINE     "�������"  // �������

// �豸�ļ�����
#define DEV_FILE_REC            1   // ¼���ļ�
#define DEV_FILE_LOG            2   // ��־�ļ�
#define DEV_FILE_OPER_LOG       3   // ������־
// �豸λ��
#define OMC_DEV_AREA_CENTER     11  // ��������
#define OMC_DEV_AREA_DEPOT      12  // ������
#define OMC_DEV_AREA_REPAIR     13  // ���޶�
#define OMC_DEV_AREA_STATION    14  // ��վ
#define OMC_DEV_AREA_TRAIN      15  // �г�
#define OMC_DEV_AREA_MASTER_DIS 16  // DIS ��
#define OMC_DEV_AREA_SLAVE_DIS  17  // DIS ��
#define OMC_DEV_AREA_ROOM       18

#define IS_TABLE_OID(a, b)    (memcmp((a), (b), sizeof(b)) == 0)

#define IS_OID(a, b)      ((memcmp(a, &(b), sizeof(b)) == 0) || (memcmp(a, &(b), (sizeof(b)- sizeof(int))) == 0))

#define NE_ID_LEN               32

#define TRANS_FILE_PORT         50221 //  �ļ�����˿�

#define SW_TYPE_TX              1   // ����̨�����汾
#define SW_TYPE_FX              2   // �̶�̨�����汾

typedef struct
{
    CHAR        acNeID[NE_ID_LEN];
}NEID_T;

// �澯��
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

    EMEGENCY_CALL_ALARM,
    EMERGENCY_BRACE_ALARM,
    SELF_CHECK_FAIL_ALARM,
    COVER_OPENED_ALARM,
    IF_STATUS_ALARM,
    DEV_TEMP_ALARM,

    IPH_LINK_FAIL_ALARM,
    PA_LINK_FAIL_ALARM,

   ALARM_NUM
}OMA_ALARM_CODE_E;


/* OMC ��Ϣ */
#define MSG_OMA_OnlineReq               "OMA_OnlineReq"         // ��������

#define MSG_OMA_AlarmRaiseReq           "OMA_AlarmRaiseReq"     // �澯����
#define MSG_OMA_AlarmClearReq           "OMA_AlarmClearReq"     // �澯�������

#define MSG_OMC_GetNeInfoReq            "OMC_GetNeInfoReq "     // ��ȡ�豸������Ϣ/״̬
#define MSG_OMC_GetNeInfoRsp            "OMC_GetNeInfoRsp "     // ��ȡ�豸������Ϣ/״̬
#define MSG_OMC_KickOutOMT              "OMC_KickOutOMT"        // OMC->OMT

/* OMT ��Ϣ */
#define MSG_OMT_Login                   "OMT_Login"
#define MSG_OMT_Logout                  "OMT_Logout"
/*���OMT�û�������Ϣ*/
#define MSG_OMT_AddNewUser              "OMT_AddNewUser"
#define MSG_OMT_DeleteUser              "OMT_DeleteUser"
#define MSG_OMT_GetUserInfo             "OMT_GetUserInfo"
#define MSG_OMT_AlterUserInfo           "OMT_AlterUserInfo"

#define MSG_OMT_GetNeBasicInfo          "OMT_GetNeBasicInfo"
#define MSG_OMT_GetNeInfo               "OMT_GetNeInfo"
#define MSG_OMT_GetClusterStatus        "OMT_GetClusterStatus"

#define MSG_OMT_AddNe                   "OMT_AddNe"  //����豸
#define MSG_OMT_SetNe                   "OMT_SetNe"  // �����豸
#define MSG_OMT_DelNe                   "OMT_DelNe"  // ɾ���豸
#define MSG_OMT_RebootNe                "OMT_RebootNe"  //�����豸

#define MSG_OMT_GetActiveAlarm          "OMT_GetActiveAlarm"  // ��ȡ��Ծ�澯
#define MSG_OMT_ClearActiveAlarm        "OMT_ClearActiveAlarm" // ������Ծ�澯

#define MSG_OMT_GetHistoryAlarm         "OMT_GetHistoryAlarm"   // ��ȡ��ʷ�澯
#define MSG_OMT_GetAlarmStat            "OMT_GetAlarmStat"      // ��ȡ�澯״̬
#define MSG_OMT_GetAlarmLevelCfg        "OMT_GetAlarmLevelCfg"  // ��ȡ�澯�ȼ�
#define MSG_OMT_SetAlarmLevelCfg        "OMT_SetAlarmLevelCfg"  // ���ø澯�ȼ�
#define MSG_OMT_AddAlarmLevelCfg        "OMT_AddAlarmLevelCfg"  // ��Ӹ澯�ȼ�����
#define MSG_OMT_DelAlarmLevelCfg        "OMT_DelAlarmLevelCfg"  // ɾ���澯�ȼ�����

#define MSG_OMT_GetAllDevStatus         "OMT_GetAllDevStatus"  // ��ȡ�����豸�Ļ���״̬��Ϣ
#define MSG_OMT_GetDevStatus            "OMT_GetDevStatus"     // ��ȡָ���豸�Ļ���״̬��Ϣ
#define MSG_OMT_GetLteStatus            "OMT_GetLteStatus"     // ��ȡ Lte ״̬��Ϣ
#define MSG_OMT_GetTrafficStatus        "OMT_GetTrafficStatus" // ��ȡ���� Tx/Rx ״̬��Ϣ

#define MSG_OMT_GetDevOperLog           "OMT_GetDevOperLog"   // ��ȡ�豸������־
#define MSG_OMT_GetDevFileList          "OMT_GetDevFileList" // ��ȡ�豸�ļ��б�
#define MSG_OMT_GetDevFile              "OMT_GetDevFile"     // ��ȡ�豸���ļ�

#define MSG_OMT_GetCfg                  "OMT_GetCfg"     //��ȡ����

#define MSG_OMT_GetSwInfo               "OMT_GetSwInfo"
#define MSG_OMT_AddSwInfo               "OMT_AddSwInfo"
#define MSG_OMT_DelSwInfo               "OMT_DelSwInfo"

#define MSG_OMT_GetSwUpdateInfo         "OMT_GetSwUpdateInfo"
#define MSG_OMT_UpdateSw                "OMT_UpdateSw"

#define MSG_OMT_UploadDevFile           "OMT_UploadDevFile"

// ��վ����
#define MSG_OMT_GetStationInfo          "OMT_GetStationInfo"    // ��ȡ��վ��Ϣ
#define MSG_OMT_AddStationInfo          "OMT_AddStationInfo"    // ��ӳ�վ��Ϣ
#define MSG_OMT_SetStationInfo          "OMT_SetStationInfo"    // �޸ĳ�վ��Ϣ
#define MSG_OMT_DelStationInfo          "OMT_DelStationInfo"    // ɾ����վ��Ϣ

#define MSG_OMT_LoadOperLogInfo         "OMT_LoadOperLogInfo"   // ��ȡomt�Ĳ�����־

#ifndef SNMP_OID
#define SNMP_OID unsigned int
#endif

#pragma pack(push, 4)

typedef struct
{
    SNMP_OID            astOID[SNMP_MAX_OID_SIZE];
    UINT32              ulOIDNum;   // ����
}OID_T;

typedef struct
{
    UINT32              ulOIDSize;      // OID����
    SNMP_OID            astOID[32];

    UINT32              ulDataType;  // ��������
    UINT32              ulDataLen;  //  ���ݳ���
    CHAR                *szValue;
}SNMP_DATA_T;

typedef struct
{
    UINT8               ucGetVer;
    UINT8               ucMaxRepetitions;
    UINT32              ulGetType;
    UINT32              ulSeqID;
    CHAR                acGetCommunity[32];

    VECTOR<OID_T>       vOID;
}SNMP_GET_REQ_T;

typedef struct
{
    UINT8               *pucBuf;
    UINT32              ulMaxLen;
    UINT32              ulLen;
}SNMP_BUF_T;

// ��ȡ���� snmp_response ����
typedef struct
{
    UINT8               ucGetVer;
    UINT32              ulGetType;
    UINT32              ulSeqID;
    CHAR                acGetCommunity[32];

    IP_ADDR_T           stClientAddr;
    VECTOR<SNMP_DATA_T> vData;
}SNMP_GET_RSP_T;

typedef struct
{
    UINT8               ucSetVer;
    UINT32              ulSetType;
    UINT32              ulSeqID;
    CHAR                acSetCommunity[32];

    VECTOR<SNMP_DATA_T> vData;
}SNMP_SET_REQ_T;

typedef struct
{
    CHAR                acUser[32];
    CHAR                acPwd[32];

    UINT32              ulUserType;
}OMT_USER_INFO_T;

typedef struct
{
    UINT32              ulTime;
    CHAR                acUserID[32];
    CHAR                acLogInfo[1024];
}OMT_OPER_LOG_INFO_T;

typedef struct
{
    CHAR                acNeID[NE_ID_LEN];         // Ŀǰ��MAC��ַ����
    CHAR                acDevName[32];
}OMA_ONLINE_INFO_T;

typedef struct
{
    CHAR                acNeID[NE_ID_LEN];  // ��Ԫ��ʶ
    UINT32              ulAlarmID;          // �澯��
    UINT32              ulAlarmRaiseTime;   // �澯����ʱ��

    UINT32              ulAlarmLevel;       // �澯�ȼ�
    UINT32              ulAlarmType;        // �澯���
    UINT32              ulAlarmStatus;      // �澯״̬
    UINT32              ulPrevAlarmStatus;  // ��һ��ʱ�̵ĸ澯״̬

    BOOL                bIsNotifyAlarm;     // �Ƿ����

    CHAR                acAlarmTitle[64];   // �澯����
    CHAR                acAlarmInfo[256];   // �澯��Ϣ
    UINT32              ulAlarmReasonID;    // �澯��
    CHAR                acAlarmReason[256]; // �澯ԭ��

    UINT32              ulAlarmClearTime;       // �澯���ʱ��
    CHAR                acAlarmClearInfo[64];   // �澯���˵����Ϣ
}ALARM_INFO_T;

typedef struct
{
    UINT32              astOID[32];
    UINT32              ulOIDSize;          // OID����

    ALARM_INFO_T        stAlarmInfo;
}OMA_ALARM_INFO_T;

typedef struct
{
    UINT32              ulTime;                 ///< ��ǲ�ѯ��¼���ڵ�������ʱ���
    UINT32              ulCriticalAlarmNum;     ///< �����澯������
    UINT32              ulMainAlarmNum;         ///< ��Ҫ�澯������
    UINT32              ulMinorAlarmNum;        ///< ��Ҫ�澯������
    UINT32              ulNormalAlarmNum;       ///< ��ͨ�澯������
}OMC_ALARM_STAT_T;

typedef struct
{
    UINT32              ulAlarmLevel;
    UINT32              ulNum;
}OMC_ALARM_COUNT_T;

typedef struct
{
    UINT32              ulDevType;
    UINT32              ulAlarmID;
    UINT32              ulAlarmLevel;
    BOOL                bIsIgnore;
}ALARM_LEVEL_CFG_T;  //CFG

typedef struct
{
    UINT32              ulNum;
    OMC_ALARM_STAT_T    astAlarmTrend[90];
}OMC_ALARM_TREND_T;

typedef struct
{
    CHAR                acNeID[NE_ID_LEN];  // ��ԪID��Ŀǰ��MAC��ַ����
    CHAR                acDevMac[20];       // ��ԪMAC��ַ
    CHAR                acDevIp[20];        // ��ԪIP��ַ

    IP_ADDR_T           stClientAddr;       // �豸��SNMP get/set��ַ�Ͷ˿�

    UINT32              ulDevType;          // ��Ԫ���
    UINT32              ulDevTC;            // ��ԪTC��(����̨����)
    CHAR                acDevName[32];      // ��Ԫ����
    CHAR                acDevAlias[32];     // ��Ԫ����

    CHAR                acSoftwareVer[64];  // ����汾(����̨�͹̶�̨Ϊ�ն�APP�汾)
    CHAR                acHardwareVer[64];  // �����汾
    CHAR                acModelName[64];    // ��Ԫ���ͺ�
    CHAR                acRadioVer[64];     // �ŵ����汾(����̨�͹̶�̨����)
    CHAR                acAndroidVer[64];   // ��׿�汾(����̨�͹̶�̨����)
    CHAR                acSIMIMSI[32];      // IMSI����(TAU����)
    UINT32              ulFirstOnlineTime;  // ��һ������uptime
    UINT32              ulLastOnlineTime;   // �������ʱ�� ������յ���Ԫ��������ʱ�����
    INT32               iOnlineSec;         // ��Ԫ����ʱ��
}NE_BASIC_INFO_T;

typedef struct
{
    UINT32              ulUptime;
    UINT8               aucNetmask[4];
    UINT8               aucGateway[4];
}NE_INFO_T;

typedef struct
{
    UINT32              ulCpuUsage;
    UINT32              ulMemUsage;
    UINT32              ulDiskUsage;
    UINT32              ulDiskLogsaveUsage;     // ��־����ʹ����(TAU����)
    UINT32              ulDiskPackageUsage;     // ץ������ʹ����(TAU����)
    UINT32              ulDiskModuleUsage;      // ģ�����ʹ����(TAU����)
    INT32               iDevTemp;              // �豸�¶�(����̨�͹̶�̨����)
}OMA_RES_STATUS_T;

typedef struct
{
    BOOL                bOnline;
    OMA_RES_STATUS_T    stResStatus;

    BOOL                bATSStatus;         // 0 δ���� 1 ����
    BOOL                bNTPStatus;
    BOOL                bLTEStatus;

    UINT32              ulClusterStatus;
}OMA_DIS_STATUS_T;

typedef struct
{
    UINT32              ulTime;
    CHAR                acNeID[NE_ID_LEN];         // Ŀǰ��MAC��ַ����

    OMA_DIS_STATUS_T     stStatus;
}OMA_DIS_STATUS_REC_T;

typedef struct
{
    BOOL                bOnline;
    OMA_RES_STATUS_T    stResStatus;

    BOOL                bATSStatus;         // 0 δ���� 1 ����
    BOOL                bNTPStatus;
    BOOL                bLTEStatus;

    UINT32              ulClusterStatus;
}OMA_REC_STATUS_T;

typedef struct
{
    UINT32              ulTime;
    CHAR                acNeID[NE_ID_LEN];         // Ŀǰ��MAC��ַ����

    OMA_REC_STATUS_T     stStatus;
}OMA_REC_STATUS_REC_T;

typedef struct
{
    BOOL                bOnline;
    OMA_RES_STATUS_T    stResStatus;

    BOOL                bATSStatus; // 0 δ���� 1 ����
    BOOL                bNTPStatus;
    BOOL                bLTEStatus;
}OMA_DC_STATUS_T;

typedef struct
{
    UINT32              ulTime;
    CHAR                acNeID[NE_ID_LEN];         // Ŀǰ��MAC��ַ����

    OMA_DC_STATUS_T     stStatus;
}OMA_DC_STATUS_REC_T;
// TX ״̬
typedef struct
{
    UINT32              ulIsOnline;             // ����״̬��1������ 0������
    UINT32              ulGatewayReachable;     // ������ͨ״̬, 1����ͨ  0����ͨ
    UINT32              ulMDCRegStatus;         // ע�ᵽԭװ���ȷ�����״̬ 0 δע�� 1 ע��
    UINT32              ulOMCLinkStatus;        // ���ܷ���������״̬ 0 δ���� 1 ����
    UINT32              ulDISLinkStatus;        // �������ȷ���������״̬ 0 δ���� 1 ����
    UINT32              ulSelfCheckStatus;      // �Լ�״̬ 0 �쳣 1 ����
    UINT32              ulDevCoverStatus;       // �豸��״̬ 1���� 0���ر�
    UINT32              ulDriveMode;            // ��ʻģʽ 1�����˼�ʻ 0�����˼�ʻ
    UINT32              ulEmergencyCallStatus;  // ��������״̬ 1���� 0����
    UINT32              ulEmergencyBraceStatus; // ����ͣ��״̬ 1���� 0����
    UINT32              ulIPHLinkStatus;        // IPH����״̬ 1 ���� 0 �쳣
    UINT32              ulPALinkStatus;         // PA ����״̬ 1 ���� 0 �쳣
}OMA_TX_RUN_STATUS_T;

typedef struct
{
    UINT32              ulStationID;
    UINT32              ulPCI;
    INT32               iRSRP;
    INT32               iRSRQ;
    INT32               iSINR;
    INT32               iRSSI;
}OMA_LTE_STATUS_T;

typedef struct
{
    UINT64  u64WanTxBytes;
    UINT64  u64WanRxBytes;
    UINT64  u64Lan1TxBytes;
    UINT64  u64Lan1RxBytes;
    UINT64  u64Lan2TxBytes;
    UINT64  u64Lan2RxBytes;
    UINT64  u64Lan3TxBytes;
    UINT64  u64Lan3RxBytes;
    UINT64  u64Lan4TxBytes;
    UINT64  u64Lan4RxBytes;
}OMA_TRAFFIC_STATUS_T;  // �˿�����ͳ��

typedef struct
{
    BOOL                bOnline;
    OMA_RES_STATUS_T    stResStatus;
    OMA_LTE_STATUS_T    stLteStatus;

    UINT32              ulOperLogIndex;     // ��ǰ��ȡ��Index
    BOOL                bGetOperLogIndex;   // ��ǰ��OperLogIndex�Ƿ��Ѿ���ȡ

    UINT32              ulFileIndex;
    BOOL                bGetFileIndex;

    UINT32              ulRunStatus;        // ����״̬��ÿһλ��ӦOMA_TX_RUN_STATUS_T�е�һ����Ա
}OMA_TX_STATUS_T;

typedef struct
{
    BOOL                    bOnline;
    OMA_RES_STATUS_T        stResStatus;
    OMA_LTE_STATUS_T        stLteStatus;
    OMA_TRAFFIC_STATUS_T    stTrafficStatus;
    UINT32                  ulRunStatus;
}OMA_TAU_STATUS_T;

typedef struct
{
    UINT32              ulTime;
    CHAR                acNeID[NE_ID_LEN];         // Ŀǰ��MAC��ַ����

    OMA_TX_STATUS_T     stStatus;
}OMA_TX_STATUS_REC_T;

typedef struct
{
    UINT32              ulTime;
    CHAR                acNeID[NE_ID_LEN];         // Ŀǰ��MAC��ַ����

    OMA_TAU_STATUS_T     stStatus;
}OMA_TAU_STATUS_REC_T;
// FX״̬
typedef struct
{
    UINT32              ulIsOnline;
    UINT32              ulGatewayReachable;     // ������ͨ״̬, 1����ͨ  0����ͨ
    UINT32              ulMDCRegStatus;         // ע�ᵽԭװ���ȷ�����״̬ 0 δע�� 1 ע��
    UINT32              ulOMCLinkStatus;        // ���ܷ���������״̬ 0 δ���� 1 ����
    UINT32              ulDISLinkStatus;        // �������ȷ���������״̬ 0 δ���� 1 ����
    UINT32              ulSelfCheckStatus;      // �Լ�״̬ 0 �쳣 1 ����
    UINT32              ulEmergencyCallStatus;  // ��������״̬ 1���� 0����
    UINT32              ulPALinkStatus;         // PA ����״̬  1 ���� 0 �쳣
}OMA_FX_RUN_STATUS_T;

// TAU״̬
typedef struct
{
    UINT32      ulIsOnline;
}OMA_TAU_RUN_STATUS_T;

typedef struct
{
    BOOL                bOnline;
    OMA_RES_STATUS_T    stResStatus;
    OMA_LTE_STATUS_T    stLteStatus;

    UINT32              ulOperLogIndex;
    BOOL                bGetOperLogIndex;

    UINT32              ulFileIndex;
    BOOL                bGetFileIndex;

    UINT32              ulRunStatus;
}OMA_FX_STATUS_T;

typedef struct
{
    UINT32              ulTime;
    CHAR                acNeID[NE_ID_LEN];         // Ŀǰ��MAC��ַ����

    OMA_FX_STATUS_T     stStatus;
}OMA_FX_STATUS_REC_T;

typedef union
{
    OMA_TX_STATUS_T         stTxStatus;
    OMA_FX_STATUS_T         stFxStatus;
    OMA_DIS_STATUS_T        stDisStatus;
    OMA_DC_STATUS_T         stDcStatus;
    OMA_TAU_STATUS_T        stTauStatus;
    OMA_REC_STATUS_T        stRecStatus;
}OMA_DEV_STATUS_U;

typedef struct
{
    NE_BASIC_INFO_T         stNeBasicInfo;
    NE_INFO_T               stNeInfo;

    UINT32                  ulTime;
    BOOL                    bIsOnLine;
    OMA_DEV_STATUS_U        unDevStatus;
}OMA_DEV_INFO_T;

typedef struct
{
    UINT32                  ulTime;
    CHAR                    acNeID[NE_ID_LEN];
    CHAR                    acLogInfo[256];
}OMA_OPERLOG_T;

typedef struct
{
    CHAR                    acNeID[NE_ID_LEN];
    UINT32                  ulFileTime;
    UINT32                  ulFileType;
    CHAR                    acFileName[256];
    CHAR                    acFileSize[32];
    BOOL                    bFileExist;
}OMA_FILE_INFO_T;

// ��Ϣ
typedef struct
{
    UINT32                  ulStartTime;
    UINT32                  ulEndTime;
    CHAR                    acNeID[NE_ID_LEN];
}OMT_GET_TXSTATUS_REQ_T;

typedef struct
{
    UINT32                  ulNum;
    OMA_TX_STATUS_REC_T     astRec[32*1024/sizeof(OMA_TX_STATUS_REC_T)];
}OMT_GET_TXSTATUS_RSP_T;

typedef struct
{
    UINT32                  ulTime;
}OMT_GET_ALLTXSTATUS_REQ_T;

typedef struct
{
    UINT32                  ulNum;
    OMA_TX_STATUS_REC_T     astRec[32*1024/sizeof(OMA_TX_STATUS_REC_T)];
}OMT_GET_ALLTXSTATUS_RSP_T;

typedef struct
{
    CHAR                    acSwVer[64];
    CHAR                    acSwFile[256];
    UINT32                  ulSwTime;       // �汾����ʱ��
    UINT32                  ulSwType;       // �汾����
}OMC_SW_INFO_T;

#define OMC_SW_UPDATE_OVER      0   // ����
#define OMC_SW_UPDATE_DOING     1   // ������

typedef struct
{
    CHAR                    acNeID[NE_ID_LEN];
    CHAR                    acNewSwVer[64];
    BOOL                    bSetFlag;               // �Ƿ��Ѿ����óɹ�
}OMC_SW_UPDATE_INFO_T;

#pragma pack(pop)

#endif

