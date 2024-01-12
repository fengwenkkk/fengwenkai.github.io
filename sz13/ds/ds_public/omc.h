#ifndef OMC_H
#define OMC_H

#include "snmp_nms.h"

// 设备类型
#define OMC_DEV_TYPE_ALL        0xff// 全部
#define OMC_DEV_TYPE_TAU        3   // TAU
#define OMC_DEV_TYPE_TX         5   // 车载台
#define OMC_DEV_TYPE_FX         6   // 固定台
#define OMC_DEV_TYPE_DC         7   // 调度台
#define OMC_DEV_TYPE_DIS        8   // 调度服务器
#define OMC_DEV_TYPE_REC        9   // 录音录像服务器
#define OMC_DEV_TYPE_MASTER_DIS 10  // 调度服务器(主用)
#define OMC_DEV_TYPE_SLAVE_DIS  11  // 调度服务器(备用)
#define OMC_DEV_TYPE_OMC        12  // 网管服务器
#define OMC_DEV_TYPE_SPT        13  // 录音录像终端

#define OMC_DEV_TYPE_EOMC       30  // eOMC南向网管
#define OMC_DEV_TYPE_ZTE        35  // ZTE南向网管

#define OMC_DEV_TYPE_NORTH      40  // 北向网管

// 设备类型
#define OMC_MODEL_NAME_TX       "H3000-TX"      // 车载台型号
#define OMC_MODEL_NAME_FX       "H3000-FX"      // 固定台型号
#define OMC_MODEL_NAME_DC       "DIS-C3000"     // 调度台型号
#define OMC_MODEL_NAME_DIS      "DIS-S3000"     // 调度服务器型号
#define OMC_MODEL_NAME_REC      "REC-S3000"     // 录音录像服务器型号
#define OMC_MODEL_NAME_SPT      "SPT-S3000"     // 录音录像终端型号

#define OMC_MODEL_NAME_EOMC     "eOMC"          // eOMC型号
#define OMC_MODEL_NAME_ZTE      "ZTE"           // ZTE型号

#define OMC_NE_ID_EOMC          "eOMC"          // eOMC
#define OMC_NE_ID_ZTE           "ZTE"           // ZTE

// 告警等级
#define ALARM_LEVEL_UNKNOWN     0xff
#define ALARM_LEVEL_ALL         0xfff
#define ALARM_LEVEL_CRITICAL    1   // 紧急告警
#define ALARM_LEVEL_MAIN        2   // 重要告警
#define ALARM_LEVEL_MINOR       3   // 次要告警
#define ALARM_LEVEL_NORMAL      4   // 普通告警
#define ALARM_LEVEL_CLEAR       6
#define ALARM_LEVEL_EVENT       0

// 告警类型
#define ALARM_TYPE_COMM         1   // 通信
#define ALARM_TYPE_ENV          2   // 环境
#define ALARM_TYPE_DEV          3   // 设备
#define ALARM_TYPE_FAIL         4   // 处理失败
#define ALARM_TYPE_QOS          5   // QOS
#define ALARM_TYPE_OTHER        6

// 告警状态
#define ALARM_STATUS_UNKNOWN    0   // 告警状态未知
#define ALARM_STATUS_RAISE      1   // 告警产生
#define ALARM_STATUS_CLEAR      2   // 告警清除
#define ALARM_STATUS_IGNORE     3   // 告警忽略 add by tzl

// 告警清除方式
#define ALARM_CLEAR_AUTO        "自动清除"  // 自动清除
#define ALARM_CLEAR_OFFLINE     "离线清除"  // 离线清除

// 设备文件类型
#define DEV_FILE_REC            1   // 录音文件
#define DEV_FILE_LOG            2   // 日志文件
#define DEV_FILE_OPER_LOG       3   // 操作日志
// 设备位置
#define OMC_DEV_AREA_CENTER     11  // 控制中心
#define OMC_DEV_AREA_DEPOT      12  // 车辆段
#define OMC_DEV_AREA_REPAIR     13  // 定修段
#define OMC_DEV_AREA_STATION    14  // 车站
#define OMC_DEV_AREA_TRAIN      15  // 列车
#define OMC_DEV_AREA_MASTER_DIS 16  // DIS 主
#define OMC_DEV_AREA_SLAVE_DIS  17  // DIS 备
#define OMC_DEV_AREA_ROOM       18

#define IS_TABLE_OID(a, b)    (memcmp((a), (b), sizeof(b)) == 0)

#define IS_OID(a, b)      ((memcmp(a, &(b), sizeof(b)) == 0) || (memcmp(a, &(b), (sizeof(b)- sizeof(int))) == 0))

#define NE_ID_LEN               32

#define TRANS_FILE_PORT         50221 //  文件传输端口

#define SW_TYPE_TX              1   // 车载台主机版本
#define SW_TYPE_FX              2   // 固定台主机版本

typedef struct
{
    CHAR        acNeID[NE_ID_LEN];
}NEID_T;

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


/* OMC 消息 */
#define MSG_OMA_OnlineReq               "OMA_OnlineReq"         // 上线请求

#define MSG_OMA_AlarmRaiseReq           "OMA_AlarmRaiseReq"     // 告警请求
#define MSG_OMA_AlarmClearReq           "OMA_AlarmClearReq"     // 告警清除请求

#define MSG_OMC_GetNeInfoReq            "OMC_GetNeInfoReq "     // 获取设备基本信息/状态
#define MSG_OMC_GetNeInfoRsp            "OMC_GetNeInfoRsp "     // 获取设备基本信息/状态
#define MSG_OMC_KickOutOMT              "OMC_KickOutOMT"        // OMC->OMT

/* OMT 消息 */
#define MSG_OMT_Login                   "OMT_Login"
#define MSG_OMT_Logout                  "OMT_Logout"
/*添加OMT用户操作消息*/
#define MSG_OMT_AddNewUser              "OMT_AddNewUser"
#define MSG_OMT_DeleteUser              "OMT_DeleteUser"
#define MSG_OMT_GetUserInfo             "OMT_GetUserInfo"
#define MSG_OMT_AlterUserInfo           "OMT_AlterUserInfo"

#define MSG_OMT_GetNeBasicInfo          "OMT_GetNeBasicInfo"
#define MSG_OMT_GetNeInfo               "OMT_GetNeInfo"
#define MSG_OMT_GetClusterStatus        "OMT_GetClusterStatus"

#define MSG_OMT_AddNe                   "OMT_AddNe"  //添加设备
#define MSG_OMT_SetNe                   "OMT_SetNe"  // 设置设备
#define MSG_OMT_DelNe                   "OMT_DelNe"  // 删除设备
#define MSG_OMT_RebootNe                "OMT_RebootNe"  //重启设备

#define MSG_OMT_GetActiveAlarm          "OMT_GetActiveAlarm"  // 获取活跃告警
#define MSG_OMT_ClearActiveAlarm        "OMT_ClearActiveAlarm" // 消除活跃告警

#define MSG_OMT_GetHistoryAlarm         "OMT_GetHistoryAlarm"   // 获取历史告警
#define MSG_OMT_GetAlarmStat            "OMT_GetAlarmStat"      // 获取告警状态
#define MSG_OMT_GetAlarmLevelCfg        "OMT_GetAlarmLevelCfg"  // 获取告警等级
#define MSG_OMT_SetAlarmLevelCfg        "OMT_SetAlarmLevelCfg"  // 设置告警等级
#define MSG_OMT_AddAlarmLevelCfg        "OMT_AddAlarmLevelCfg"  // 添加告警等级配置
#define MSG_OMT_DelAlarmLevelCfg        "OMT_DelAlarmLevelCfg"  // 删除告警等级配置

#define MSG_OMT_GetAllDevStatus         "OMT_GetAllDevStatus"  // 获取所有设备的基础状态信息
#define MSG_OMT_GetDevStatus            "OMT_GetDevStatus"     // 获取指定设备的基础状态信息
#define MSG_OMT_GetLteStatus            "OMT_GetLteStatus"     // 获取 Lte 状态信息
#define MSG_OMT_GetTrafficStatus        "OMT_GetTrafficStatus" // 获取流量 Tx/Rx 状态信息

#define MSG_OMT_GetDevOperLog           "OMT_GetDevOperLog"   // 获取设备操作日志
#define MSG_OMT_GetDevFileList          "OMT_GetDevFileList" // 获取设备文件列表
#define MSG_OMT_GetDevFile              "OMT_GetDevFile"     // 获取设备的文件

#define MSG_OMT_GetCfg                  "OMT_GetCfg"     //获取配置

#define MSG_OMT_GetSwInfo               "OMT_GetSwInfo"
#define MSG_OMT_AddSwInfo               "OMT_AddSwInfo"
#define MSG_OMT_DelSwInfo               "OMT_DelSwInfo"

#define MSG_OMT_GetSwUpdateInfo         "OMT_GetSwUpdateInfo"
#define MSG_OMT_UpdateSw                "OMT_UpdateSw"

#define MSG_OMT_UploadDevFile           "OMT_UploadDevFile"

// 车站管理
#define MSG_OMT_GetStationInfo          "OMT_GetStationInfo"    // 获取车站信息
#define MSG_OMT_AddStationInfo          "OMT_AddStationInfo"    // 添加车站信息
#define MSG_OMT_SetStationInfo          "OMT_SetStationInfo"    // 修改车站信息
#define MSG_OMT_DelStationInfo          "OMT_DelStationInfo"    // 删除车站信息

#define MSG_OMT_LoadOperLogInfo         "OMT_LoadOperLogInfo"   // 获取omt的操作日志

#ifndef SNMP_OID
#define SNMP_OID unsigned int
#endif

#pragma pack(push, 4)

typedef struct
{
    SNMP_OID            astOID[SNMP_MAX_OID_SIZE];
    UINT32              ulOIDNum;   // 个数
}OID_T;

typedef struct
{
    UINT32              ulOIDSize;      // OID个数
    SNMP_OID            astOID[32];

    UINT32              ulDataType;  // 数据类型
    UINT32              ulDataLen;  //  数据长度
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

// 获取到的 snmp_response 报文
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
    CHAR                acNeID[NE_ID_LEN];         // 目前由MAC地址生成
    CHAR                acDevName[32];
}OMA_ONLINE_INFO_T;

typedef struct
{
    CHAR                acNeID[NE_ID_LEN];  // 网元标识
    UINT32              ulAlarmID;          // 告警码
    UINT32              ulAlarmRaiseTime;   // 告警产生时间

    UINT32              ulAlarmLevel;       // 告警等级
    UINT32              ulAlarmType;        // 告警类别
    UINT32              ulAlarmStatus;      // 告警状态
    UINT32              ulPrevAlarmStatus;  // 上一个时刻的告警状态

    BOOL                bIsNotifyAlarm;     // 是否忽略

    CHAR                acAlarmTitle[64];   // 告警标题
    CHAR                acAlarmInfo[256];   // 告警信息
    UINT32              ulAlarmReasonID;    // 告警码
    CHAR                acAlarmReason[256]; // 告警原因

    UINT32              ulAlarmClearTime;       // 告警清除时间
    CHAR                acAlarmClearInfo[64];   // 告警清除说明信息
}ALARM_INFO_T;

typedef struct
{
    UINT32              astOID[32];
    UINT32              ulOIDSize;          // OID个数

    ALARM_INFO_T        stAlarmInfo;
}OMA_ALARM_INFO_T;

typedef struct
{
    UINT32              ulTime;                 ///< 标记查询记录所在的天的零点时间戳
    UINT32              ulCriticalAlarmNum;     ///< 紧急告警的数量
    UINT32              ulMainAlarmNum;         ///< 重要告警的数量
    UINT32              ulMinorAlarmNum;        ///< 次要告警的数量
    UINT32              ulNormalAlarmNum;       ///< 普通告警的数量
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
    CHAR                acNeID[NE_ID_LEN];  // 网元ID，目前由MAC地址生成
    CHAR                acDevMac[20];       // 网元MAC地址
    CHAR                acDevIp[20];        // 网元IP地址

    IP_ADDR_T           stClientAddr;       // 设备的SNMP get/set地址和端口

    UINT32              ulDevType;          // 网元类别
    UINT32              ulDevTC;            // 网元TC端(车载台特有)
    CHAR                acDevName[32];      // 网元名称
    CHAR                acDevAlias[32];     // 网元别名

    CHAR                acSoftwareVer[64];  // 软件版本(车载台和固定台为终端APP版本)
    CHAR                acHardwareVer[64];  // 主机版本
    CHAR                acModelName[64];    // 网元的型号
    CHAR                acRadioVer[64];     // 信道机版本(车载台和固定台特有)
    CHAR                acAndroidVer[64];   // 安卓版本(车载台和固定台特有)
    CHAR                acSIMIMSI[32];      // IMSI卡号(TAU特有)
    UINT32              ulFirstOnlineTime;  // 第一次上线uptime
    UINT32              ulLastOnlineTime;   // 最后在线时间 由最后收到网元的心跳包时间更新
    INT32               iOnlineSec;         // 网元在线时长
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
    UINT32              ulDiskLogsaveUsage;     // 日志分区使用率(TAU特有)
    UINT32              ulDiskPackageUsage;     // 抓包分区使用率(TAU特有)
    UINT32              ulDiskModuleUsage;      // 模块分区使用率(TAU特有)
    INT32               iDevTemp;              // 设备温度(车载台和固定台特有)
}OMA_RES_STATUS_T;

typedef struct
{
    BOOL                bOnline;
    OMA_RES_STATUS_T    stResStatus;

    BOOL                bATSStatus;         // 0 未连接 1 连接
    BOOL                bNTPStatus;
    BOOL                bLTEStatus;

    UINT32              ulClusterStatus;
}OMA_DIS_STATUS_T;

typedef struct
{
    UINT32              ulTime;
    CHAR                acNeID[NE_ID_LEN];         // 目前由MAC地址生成

    OMA_DIS_STATUS_T     stStatus;
}OMA_DIS_STATUS_REC_T;

typedef struct
{
    BOOL                bOnline;
    OMA_RES_STATUS_T    stResStatus;

    BOOL                bATSStatus;         // 0 未连接 1 连接
    BOOL                bNTPStatus;
    BOOL                bLTEStatus;

    UINT32              ulClusterStatus;
}OMA_REC_STATUS_T;

typedef struct
{
    UINT32              ulTime;
    CHAR                acNeID[NE_ID_LEN];         // 目前由MAC地址生成

    OMA_REC_STATUS_T     stStatus;
}OMA_REC_STATUS_REC_T;

typedef struct
{
    BOOL                bOnline;
    OMA_RES_STATUS_T    stResStatus;

    BOOL                bATSStatus; // 0 未连接 1 连接
    BOOL                bNTPStatus;
    BOOL                bLTEStatus;
}OMA_DC_STATUS_T;

typedef struct
{
    UINT32              ulTime;
    CHAR                acNeID[NE_ID_LEN];         // 目前由MAC地址生成

    OMA_DC_STATUS_T     stStatus;
}OMA_DC_STATUS_REC_T;
// TX 状态
typedef struct
{
    UINT32              ulIsOnline;             // 在线状态，1：在线 0：离线
    UINT32              ulGatewayReachable;     // 网关连通状态, 1：连通  0：不通
    UINT32              ulMDCRegStatus;         // 注册到原装调度服务器状态 0 未注册 1 注册
    UINT32              ulOMCLinkStatus;        // 网管服务器连接状态 0 未连接 1 连接
    UINT32              ulDISLinkStatus;        // 二开调度服务器连接状态 0 未连接 1 连接
    UINT32              ulSelfCheckStatus;      // 自检状态 0 异常 1 正常
    UINT32              ulDevCoverStatus;       // 设备盖状态 1：打开 0：关闭
    UINT32              ulDriveMode;            // 驾驶模式 1：有人驾驶 0：无人驾驶
    UINT32              ulEmergencyCallStatus;  // 紧急呼叫状态 1：是 0：否
    UINT32              ulEmergencyBraceStatus; // 紧急停车状态 1：是 0：否
    UINT32              ulIPHLinkStatus;        // IPH连接状态 1 正常 0 异常
    UINT32              ulPALinkStatus;         // PA 连接状态 1 正常 0 异常
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
}OMA_TRAFFIC_STATUS_T;  // 端口流量统计

typedef struct
{
    BOOL                bOnline;
    OMA_RES_STATUS_T    stResStatus;
    OMA_LTE_STATUS_T    stLteStatus;

    UINT32              ulOperLogIndex;     // 当前获取的Index
    BOOL                bGetOperLogIndex;   // 当前的OperLogIndex是否已经获取

    UINT32              ulFileIndex;
    BOOL                bGetFileIndex;

    UINT32              ulRunStatus;        // 运行状态，每一位对应OMA_TX_RUN_STATUS_T中的一个成员
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
    CHAR                acNeID[NE_ID_LEN];         // 目前由MAC地址生成

    OMA_TX_STATUS_T     stStatus;
}OMA_TX_STATUS_REC_T;

typedef struct
{
    UINT32              ulTime;
    CHAR                acNeID[NE_ID_LEN];         // 目前由MAC地址生成

    OMA_TAU_STATUS_T     stStatus;
}OMA_TAU_STATUS_REC_T;
// FX状态
typedef struct
{
    UINT32              ulIsOnline;
    UINT32              ulGatewayReachable;     // 网关连通状态, 1：连通  0：不通
    UINT32              ulMDCRegStatus;         // 注册到原装调度服务器状态 0 未注册 1 注册
    UINT32              ulOMCLinkStatus;        // 网管服务器连接状态 0 未连接 1 连接
    UINT32              ulDISLinkStatus;        // 二开调度服务器连接状态 0 未连接 1 连接
    UINT32              ulSelfCheckStatus;      // 自检状态 0 异常 1 正常
    UINT32              ulEmergencyCallStatus;  // 紧急呼叫状态 1：是 0：否
    UINT32              ulPALinkStatus;         // PA 连接状态  1 正常 0 异常
}OMA_FX_RUN_STATUS_T;

// TAU状态
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
    CHAR                acNeID[NE_ID_LEN];         // 目前由MAC地址生成

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

// 消息
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
    UINT32                  ulSwTime;       // 版本发布时间
    UINT32                  ulSwType;       // 版本类型
}OMC_SW_INFO_T;

#define OMC_SW_UPDATE_OVER      0   // 结束
#define OMC_SW_UPDATE_DOING     1   // 进行中

typedef struct
{
    CHAR                    acNeID[NE_ID_LEN];
    CHAR                    acNewSwVer[64];
    BOOL                    bSetFlag;               // 是否已经设置成功
}OMC_SW_UPDATE_INFO_T;

#pragma pack(pop)

#endif

