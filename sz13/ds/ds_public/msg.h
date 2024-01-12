#ifndef MSG_H
#define MSG_H

#define EV_REMOTE_START                     EV_SERVICE_START    // 0x4000

#define EV_SYS_START                        (EV_REMOTE_START + 0x0100)
#define EV_CFG_START                        (EV_REMOTE_START + 0x0300)
#define EV_USER_START                       (EV_REMOTE_START + 0x0700)
#define EV_LOG_START                        (EV_REMOTE_START + 0x0800)
#define EV_CHECK_START                      (EV_REMOTE_START + 0x0900)

#define EV_RADIO_START                      (EV_REMOTE_START + 0x2000)  // 车载台/固定台相关
#define EV_TEST_START                       (EV_REMOTE_START + 0x0A00)   // TaskTest

#define EV_ESDK_MSG                         (EV_REMOTE_START + 0x3000)

/* Sys */
#define EV_UPLOAD_FILE_REQ                  (EV_SYS_START + 0)  // 文件上传
#define EV_DOWNLOAD_FILE_REQ                (EV_SYS_START + 1)  // 文件上传
#define EV_CONNECTION_CHANGE_MSG            (EV_SYS_START + 2)
#define EV_SET_DATACLEAR_CFG_REQ            (EV_SYS_START + 3)
#define EV_SERVER_TO_AGENT_ANSWER           (EV_SYS_START + 4)
#define EV_SYSTEM_ALARM_PARA_IND            (EV_SYS_START + 5)  //通知TaskAlarm接收数据
#define EV_DATA_CLEAR_REQ                   (EV_SYS_START + 6)
#define EV_SET_STATUS_REQ                   (EV_SYS_START + 7)
#define EV_STATUS_REFRESH_REQ               (EV_SYS_START + 8)  // 进入等待状态

#define EV_SYS_RESET_REQ                    (EV_SYS_START + 9)
#define EV_HARDWARE_RESET_REQ               (EV_SYS_START + 10)

#define EV_CHANGE_RUN_MODE_REQ              (EV_SYS_START + 11)

#define EV_DOWNLOAD_FILE_EX_REQ             (EV_SYS_START + 21) // 文件下载扩展接口

/* sys Rsp */
#define EV_UPLOAD_FILE_RSP                  EV_RSP(EV_UPLOAD_FILE_REQ)
#define EV_DOWNLOAD_FILE_RSP                EV_RSP(EV_DOWNLOAD_FILE_REQ)
#define EV_SET_DATACLEAR_CFG_RSP            EV_RSP(EV_SET_DATACLEAR_CFG_REQ)
#define EV_DATA_CLEAR_RSP                   EV_RSP(EV_DATA_CLEAR_REQ)

#define EV_SET_STATUS_RSP                   EV_RSP(EV_SET_STATUS_REQ)
#define EV_STATUS_REFRESH_RSP               EV_RSP(EV_STATUS_REFRESH_REQ)   // 进入等待状态

#define EV_SYS_RESET_RSP                    EV_RSP(EV_SYS_RESET_REQ)
#define EV_CHANGE_RUN_MODE_RSP              EV_RSP(EV_CHANGE_RUN_MODE_REQ)

#define EV_DOWNLOAD_FILE_EX_RSP             EV_RSP(EV_DOWNLOAD_FILE_EX_REQ)

/* Check */

/* RADIO */
#define EV_APP_DIS_REQ                      (EV_RADIO_START+0)      // APP消息
#define EV_SA_DIS_REQ                       (EV_RADIO_START+100)    // SA消息

#define EV_APP_DIS_RSP                      EV_RSP(EV_APP_DIS_REQ)
#define EV_SA_DIS_RSP                       EV_RSP(EV_SA_DIS_REQ)

/* Cfg */
#define EV_CLIENT_REQ                       (EV_CFG_START + 0)      // 客户端->服务器的通用消息
#define EV_DC_DIS_REQ                       (EV_CFG_START + 1)      // DC->DIS的通用消息
#define EV_DC_DIS_CFG_REQ                   (EV_CFG_START + 2)      // DC->DIS的配置数据
#define EV_DIS_DC_IND                       (EV_CFG_START + 3)      // DIS->DC的通知消息
#define EV_DC_DIS_OMA_REQ                   (EV_CFG_START + 4)      // DC->DIS的OMA数据

#define EV_APP_DIS_DC_REQ                   (EV_CFG_START + 20)     // DIS转发APP消息到DC
#define EV_SA_DIS_DC_REQ                    (EV_CFG_START + 21)     // DIS转发SA消息到DC

#define EV_DC_DIS_APP_REQ                   (EV_CFG_START + 31)     // DIS转发DC消息到APP
#define EV_DC_DIS_SA_REQ                    (EV_CFG_START + 32)     // DIS转发DC消息到SA

#define EV_UE_DIS_REQ                       (EV_CFG_START + 33)     // 从手持台(UE)发送到服务器(dis)的消息
#define EV_DC_DIS_UE_REQ                    (EV_CFG_START + 34)     // DIS转发DC消息到UE

#define EV_NOCC_REQ                         (EV_CFG_START + 41)     // NOCC到OCC的消息
#define EV_OCC_REQ                          (EV_CFG_START + 42)     // OCC到NOCC的消息

/* Cfg Rsp */
#define EV_CLIENT_RSP                       EV_RSP(EV_CLIENT_REQ)
#define EV_DC_DIS_RSP                       EV_RSP(EV_DC_DIS_REQ)
#define EV_DC_DIS_CFG_RSP                   EV_RSP(EV_DC_DIS_CFG_REQ)

#define EV_APP_DIS_DC_RSP                   EV_RSP(EV_APP_DIS_DC_REQ)
#define EV_SA_DIS_DC_RSP                    EV_RSP(EV_SA_DIS_DC_REQ)

#define EV_DC_DIS_APP_RSP                   EV_RSP(EV_DC_DIS_APP_REQ)
#define EV_DC_DIS_SA_RSP                    EV_RSP(EV_DC_DIS_SA_REQ)

#define EV_NOCC_RSP                         EV_RSP(EV_NOCC_REQ)
#define EV_OCC_RSP                          EV_RSP(EV_OCC_REQ)

/* User */

/* User Rsp */

//Log Req
#define EV_GET_OPER_LOG_REQ                 (EV_LOG_START + 0)
#define EV_DEL_OPER_LOG_REQ                 (EV_LOG_START + 1)
#define EV_ADD_OPER_LOG_REQ                 (EV_LOG_START + 5)

//Log Rsp
#define EV_GET_OPER_LOG_RSP                 EV_RSP(EV_GET_OPER_LOG_REQ)
#define EV_DEL_OPER_LOG_RSP                 EV_RSP(EV_DEL_OPER_LOG_REQ)

/* OMC */
#define EV_OMA_OMC_REQ                      (EV_OMC_START + 1)      // OMA->OMC的通用消息
#define EV_OMT_OMC_REQ                      (EV_OMC_START + 2)      // OMA->OMC的通用消息

#define EV_OMT_ADD_NE_REQ                   (EV_OMC_START + 10)     // 添加网元
#define EV_OMT_DEL_NE_REQ                   (EV_OMC_START + 11)     // 删除网元

#define EV_OMT_GET_NEINFO_REQ               (EV_OMC_START + 50)     // 获取网元信息

#define EV_OMT_GET_TXSTATUS_REQ             (EV_OMC_START + 70)     // 获取某个TX设备的状态数据（一段时间内）
#define EV_OMT_GET_ALLTXSTATUS_REQ          (EV_OMC_START + 71)     // 获取所有TX设备的某个时刻的状态数据
#define EV_OMT_SET_ALARM_LEVEL_REQ          (EV_OMC_START + 72)     //设置告警等级 tzl

#define EV_OMA_OMC_RSP                      EV_RSP(EV_OMA_OMC_REQ)
#define EV_OMT_OMC_RSP                      EV_RSP(EV_OMT_OMC_REQ)

#define EV_OMT_ADD_NE_RSP                   EV_RSP(EV_OMT_ADD_NE_REQ)
#define EV_OMT_DEL_NE_RSP                   EV_RSP(EV_OMT_DEL_NE_REQ)

#define EV_OMT_GET_NEINFO_RSP               EV_RSP(EV_OMT_GET_NEINFO_REQ)
#define EV_OMT_GET_TXSTATUS_RSP             EV_RSP(EV_OMT_GET_TXSTATUS_REQ)
#define EV_OMT_GET_ALLTXSTATUS_RSP          EV_RSP(EV_OMT_GET_ALLTXSTATUS_REQ)

// LMT
#define EV_LMT_START                        (EV_OMC_START + 0x800)  // LMT
#define EV_LMT_SHELL_CMD_REQ                (EV_LMT_START + 0)
#define EV_LMT_DOWNLOAD_FILE_REQ            (EV_LMT_START + 1)
#define EV_LMT_UPLOAD_FILE_REQ              (EV_LMT_START + 2)

#define EV_LMT_SHELL_CMD_RSP                EV_RSP(EV_LMT_SHELL_CMD_REQ)
#define EV_LMT_DOWNLOAD_FILE_RSP            EV_RSP(EV_LMT_DOWNLOAD_FILE_REQ)
#define EV_LMT_UPLOAD_FILE_RSP              EV_RSP(EV_LMT_UPLOAD_FILE_REQ)

#pragma pack(push, 4)

#define OPER_TYPE_ADMIN         1   // 超级管理员
#define OPER_TYPE_MANAGE        2   // 一般操作
#define OPER_TYPE_QUERY         3   // 查询操作

/****************************** SYS 消息结构定义 *************************************/
// 组订阅状态
#define GROUP_STATUS_UNKNOWN    0
#define GROUP_STATUS_TOJOIN     1   // 待自动加入
#define GROUP_STATUS_TOUNSUB    2   // 待自动去订阅
#define GROUP_STATUS_UNSUB      3   // 去订阅
#define GROUP_STATUS_SUB        4
#define GROUP_STATUS_JOIN       5
#define GROUP_STATUS_DELETE     6

// 呼叫类型
#define CALL_TYPE_UNKNOWN               0   //

#define CALL_TYPE_PTT                   1   // 组呼
#define CALL_TYPE_EMERGENCY_CALL        2   // 紧急呼叫
#define CALL_TYPE_TRAIN_BRD             3   // 列车广播
#define CALL_TYPE_ZONE_BRD              4   // 选区广播

#define CALL_TYPE_P2P                   11  // 语音点呼
#define CALL_TYPE_P2P_TRANSFER          12  // 语音点呼转接
#define CALL_TYPE_P2P_EMERGENCY_CALL    13  // 语音紧急点呼
#define CALL_TYPE_LRBUS_REQ_CALL        14  // LRBUS语音点呼(请呼）
#define CALL_TYPE_HALF_DUPLEX_P2P_CALL  16  // 半双工语音点呼

#define CALL_TYPE_VIDEO_P2P             21  // 视频点呼
#define CALL_TYPE_VIDEO_REALPLAY        22  // 视频浏览（调度台发起）
#define CALL_TYPE_VIDEO_RECV            23  // 视频接收（终端发起）

#define CALL_TYPE_VIDEO_DISPATCH        31  // 视频分发
#define CALL_TYPE_VIDEO_ONWALL          32  // 视频上墙
#define CALL_TYPE_VIDEO_PTT             33  // 视频组呼

#define CALL_TYPE_REQ_CALL              91  // 请呼
#define CALL_TYPE_IPH                   92  // 乘客对讲
#define CALL_TYPE_ENV_LISTEN            93  // 环境侦听
#define CALL_TYPE_DISCREET_LISTEN       94  // 缜密监听

#define TRAIN_REGISTER                  101 // 列车注册
#define TRAIN_SET_TRAINID               102 // 设置列车车次号

// 主备集群状态
#define CLUSTER_STATUS_STAND_ALONE      0
#define CLUSTER_STATUS_MASTER           1
#define CLUSTER_STATUS_SLAVE            2
#define CLUSTER_STATUS_CHECK_FAIL       9

#define TC_HEAD                         1  // 1表示车头，其他值为车尾

// upload file
typedef struct
{
    CHAR                    acFile[256];

    UINT64                  u64StartPos;
    UINT32                  ulIndex;
    UINT32                  ulDataLen;
    CHAR                    aucData[32*1024];
}UPLOAD_FILE_REQ_T;

typedef struct
{
    CHAR                    acFile[256];

    UINT8                   ucRet;
    UINT8                   aucResv[3];

    UINT32                  ulIndex;
}UPLOAD_FILE_RSP_T;

// download file
typedef struct
{
    CHAR                    acFile[256];
    UINT32                  ulIndex;
    UINT64                  u64StartPos;
}DOWNLOAD_FILE_REQ_T;

typedef struct
{
    CHAR                    acFile[256];
    UINT32                  ulIndex;
    UINT32                  ulMaxDataLen;
    UINT64                  u64StartPos;
}DOWNLOAD_FILE_EX_REQ_T;

typedef struct
{
    CHAR                    acFile[256];

    UINT8                   ucRet;
    UINT8                   ucIsEnd;
    UINT8                   aucResv[2];

    UINT32                  ulIndex;
    UINT32                  ulDataLen;
    CHAR                    aucData[32*1024];
}DOWNLOAD_FILE_RSP_T, DOWNLOAD_FILE_EX_RSP_T;

typedef struct
{
    BOOL                    bAutoClear;
    UINT32                  ulClearHour;
    UINT32                  ulClearMinu;
}DATA_CLEAR_CFG_T;

typedef struct
{
    DATA_CLEAR_CFG_T        stDataClearCfg;
}SET_DATACLEAR_CFG_REQ_T;

typedef struct
{
    UINT32                  ulRetCode;
}SET_DATACLEAR_CFG_RSP_T;

typedef struct
{
    UINT32                  ulSeqID;
}SERVER_TO_AGENT_ANSWER_T;

typedef struct
{
    UINT32                  ulDays;
}DEL_DATA_REQ_T;

typedef struct
{
    UINT32                  ulRetCode;
}DEL_DATA_RSP_T;

typedef struct
{
    UINT32                  ulStatus;
}STATUS_CHANGE_REQ_ST;

typedef struct
{
    UINT32                  ulStatus;
    BOOL                    bRet;
}STATUS_CHANGE_RSP_ST;

typedef struct
{
    BOOL                    bRet;
    CHAR                    acWorkDir[256];
    CHAR                    acProcName[256];
}GET_SYS_RESET_RSP;

/****************************** log 消息结构定义 *************************************/
typedef struct
{
    CHAR                    acName[64];
    UINT32                  ulStartTime;
    UINT32                  ulEndTime;
}GET_OPER_LOG_REQ_T;

typedef struct
{
    UINT32                  ulRetCode;
}GET_OPER_LOG_RSP_T;

typedef struct
{
    INT32                   idays;
}DEL_OPER_LOG_REQ_T;

typedef struct
{
    UINT32                  ulRetCode;
}DEL_OPER_LOG_RSP_T;

/****************************** cfg 消息结构定义 *************************************/
#define CFG_DATA_USER           "User"
#define CFG_DATA_GROUP          "Group"
#define CFG_DATA_SUBGROUP       "SubGroup"
#define CFG_DATA_TRAIN          "Train"
#define CFG_DATA_STATION        "Station"

// 终端硬件分类
#define TRAIN_UE                1       // 车载台
#define STATION_UE              2       // 固定台
#define SMALL_PORTAL_UE         3       // 小屏手持台
#define LARGE_PORTAL_UE         4       // 大屏手持台
#define DC_USER                 9       // 调度台

// 终端功能分类
#define UE_DC                   1       // 调度
#define UE_TRAIN                2       // 列车
#define UE_STATION              3       // 车站
#define UE_FIX                  4       // 维修
#define UE_MANAGE               5       // 管理
#define UE_OTHER                9       // 其它

#define INVALID_GROUP_ID        0
#define INVALID_USER_ID         0
#define INVALID_TRAIN_UNIT_ID   0
#define INVALID_STATION_ID      0
#define INVALID_DEPOT_ID        0
#define INVALID_BUSINESS_ID     0   ///< 无效业务ID
#define DEFAULT_LINE_ID         0
#define ALL_LINE_ID             0xffffffff
#define INVALID_LINE_ID         0xfffffffe

// 组分类
#define GROUP_FUNC_TRAIN        2   // 列车组
#define GROUP_FUNC_STATION      3   // 车站组
#define GROUP_FUNC_FIX          4   // 维护组
#define GROUP_FUNC_MULTIFUNC    6   // 多职能组
#define GROUP_FUNC_OTHER        9   // 其他

// 群组分类
#define STATIC_GROUP            10      // 静态组
#define TRAIN_GROUP             20      // 列车组
#define STATION_GROUP           30      // 车站组
#define PATCH_GROUP             40      // 派接组
#define DYNAMIC_GROUP           50      // 动态组
#define MULTI_GROUP             60      // 多选组
#define ZONE_BRD_GROUP          99      // 车站选区广播组
#define TRAIN_BRD_GROUP         99      // 列车广播临时组（1个或多个列车广播组）

#define MAX_DC_NUM                      32  // 最多32个调度台
#define MAX_DC_DYNAMIC_GRPUP_NUM        15

#define STATION_TYPE_NORMAL         1   // 普通车站
#define STATION_TYPE_DEPOT          2   // 停车段
#define STATION_TYPE_FIX            3   // 维修段
#define STATION_TYPE_TEST           4   // 测试线

#define DEPOT_ID_AUTO               0
#define DEPOT_ID_UNKNOWN            0
#define DEPOT_ID_MAINLINE           1   // 1为正线，车辆段的位置值必须大于1

#define TRAIN_STATUS_INFO_UNKNOW    0xff   //未知

#define TRAIN_POS_UNKNOWN           0   // 未知
#define TRAIN_POS_MAINLINE          1   // 正线， 对应STATION_TYPE_NORMAL
#define TRAIN_POS_DEPOT             2   // 停车场/车辆段, 对应STATION_TYPE_DEPOT
#define TRAIN_POS_FIX               3   // 维修段, 对应STATION_TYPE_FIX

#define TRAIN_POS_ALL               0xff

// 列车方向
#define TRAIN_DIRECTION_UNKNOWN     0   // 未知
#define TRAIN_DIRECTION_UPSIDE      1   // 上行
#define TRAIN_DIRECTION_DOWNSIDE    2   // 下行

// 是否在折返轨或存车轨
#define TRAIN_ON_TURNBACK_TRACK_NO      0   // 不在
#define TRAIN_ON_TURNBACK_TRACK_YES     1   // 在
#define TRAIN_ON_TURNBACK_TRACK_UNKNOWN 2   // 未知

// 是否进出正线
//#define INOUT_DEPOT_STATE_NULL          0   // 默认值为0
//#define INOUT_DEPOT_STATE_OUT           1   // 当列车从停车场/车辆段进入转换轨时值为1（保持5秒）
//#define INOUT_DEPOT_STATE_IN            2   // 当列车从转换轨进入停车场/车辆段时值为2（保持5秒）。

// 列车激活端
#define TRAIN_ATS_TC1                 0x01 // TC1端为当前的激活端
#define TRAIN_ATS_TC2                 0x02 // TC2端为当前的激活端
#define TRAIN_ATS_INVALID_TC          0xFF // 无效

// 是否在转换轨
//#define TRAIN_ON_TRANSFER_NO      0   // 不在
//#define TRAIN_ON_TRANSFER_YES     1   // 在

// 是否在站台
#define TRAIN_ON_PLATFORM_NO      0   // 不在
#define TRAIN_ON_PLATFORM_YES     1   // 在

// 是否失去真实位置
//#define TRAIN_LOST_REAL_LOCATION_NO    0 // 没有失去真实位置信息
//#define TRAIN_LOST_REAL_LOCATION_YES   1 // 失去真实位置信息

// <GetCfgDataReq Type="Station" />
typedef struct
{
    CHAR                    acReq[1024];
}GET_CFG_DATA_REQ_T;

// <GetCfgDataRsp Type="Station">  <Station ... /> ... </GetCfgDataRsp>
typedef struct
{
    CHAR                    acRsp[60*1024];
}GET_CFG_DATA_RSP_T;

typedef struct
{
    UINT32                  ulUserID;
    UINT32                  ulTime;
    CHAR                    acDCOperatorID[32];
    CHAR                    acLogInfo[1024];
}OPER_LOG_INFO_T;

/// 对应数据库中 groupcfg, 存储的静态组的组号
typedef struct
{
    INT64                   i64UpLineTrainGroupID;          // 上行列车通播组
    INT64                   i64DownLineTrainGroupID;        // 下行列车通播组
    INT64                   i64MainLineTrainGroupID;        // 正线列车通播组

    INT64                   i64UpLineTrainBrdGroupID;       // 上行列车广播组
    INT64                   i64DownLineTrainBrdGroupID;     // 下行列车广播组
    INT64                   i64MainLineTrainBrdGroupID;     // 正线列车广播组

    INT64                   i64AllStationGroupID;           // 所有车站通播组

    INT64                   i64StartTempDynamicGroupID;     // 临时创建的动态组组号区间的开始
    INT64                   i64EndTempDynamicGroupID;       // 临时创建的动态组组号区间的结束

    INT64                   i64DriverGroupID;               // 司机通话组
    INT64                   i64DepotDriverGroupID;          // 车辆段司机通话组
}GROUP_CFG_T;

/// 对应数据库中 dcuser 表， 存储 SDK 账号相关信息
typedef struct
{
    UINT32                  ulUserID;       ///< SDK 号码, 核心网定义
    CHAR                    acName[32];     ///< 该账号的名称，我们自己定义
    UINT32                  ulDCType;       ///< 调度台类型
    UINT32                  ulDepotID;      ///< 判定是哪个车辆段，（正线、非正线）

    UINT32                  ulStationNum;
    UINT32                  aulStationList[80];

    UINT32                  ulDevStatus;    ///< 本字段数据库中没有
    UINT32                  ulCallStatus;   ///< 从SDK事件上报信息中获取,  本字段数据库中没有
}DC_USER_INFO_T;

/// 对应数据库中 dcoperator 表， 存储调度员账号相关信息
typedef struct
{
    CHAR                    acOperatorID[32];   ///< 调度员账号，我们自己定义
    CHAR                    acName[32];         ///< 调度员名称
    CHAR                    acPassword[32];     ///< 密码
    UINT32                  ulDCType;           ///< 调度台类型
}DC_OPERATOR_INFO_T;

typedef struct
{
    CHAR                    acName[32];
    CHAR                    acPassword[32];
    UINT32                  ulOmtType;
}OMT_OPERATOR_INFO_T;

/// 对应数据库中 stationue, 保存固定台相关信息
typedef struct
{
    UINT32                  ulUserID;           ///< 固定台 ID
    CHAR                    acName[32];         ///< 固定台名称
    CHAR                    acLanAddr[16];      ///< Lan 口 IP 地址
    UINT32                  ulStationID;        ///< 车站ID
    INT64                   i64DefaultGroupID;  ///< 与调度台的通话组

    UINT32                  ulDevStatus;        ///< 本字段数据库表中没有
    UINT32                  ulCallStatus;       ///< 本字段数据库表中没有
}STATION_UE_INFO_T;

// 对应数据库中 section, 保存车轨的物理区段
typedef struct
{
    UINT32                  ulSectionID;        // 区段ID
    CHAR                    acSectionName[16];  // 区段名称
    UINT32                  ulStationID;        // 车站ID
    UINT32                  ulDirection;        // 方向
    INT32                   iIndexInStation;   // 车站内的相对位置
    BOOL                    bInPlatform;        // 是否在站台
    BOOL                    bInDepotTrack;      // 是否在正线存车轨
    INT32                   x1;
    INT32                   y1;
    INT32                   x2;
    INT32                   y2;
}SECTION_INFO_T;

/// 对应数据库中 trainue, 保存车载台的信息
typedef struct
{
    UINT32                  ulUserID;       ///< 车载台 ID
    CHAR                    acName[32];     ///< 车载台信息
    CHAR                    acLanAddr[16];  ///< Lan 口 IP 地址
    UINT32                  ulTrainUnitID;  ///< 装配列车的车体号

    UINT32                  ulDevStatus;    ///< 表中没有该字段
    UINT32                  ulCallStatus;   ///< 表中没有该字段
    UINT32                  ulCellID;       ///< 表中没有该字段
}TRAIN_UE_INFO_T;

/// 对应数据库表 portalue， 保存手持台信息
typedef struct
{
    UINT32                  ulUserID;       ///< 手台的核心网短号码
    CHAR                    acName[32];     ///< 手台名称
    UINT32                  ulType;         ///< 手台类型(大屏、小屏)
    UINT32                  ulFuncType;     ///< 功能分类
    UINT32                  ulStationID;    ///< 隶属车站 ID

    UINT32                  ulDevStatus;    ///< 表中没有该字段
    UINT32                  ulCallStatus;   ///< 表中没有该字段
    UINT32                  ulCellID;       ///< 表中没有该字段

    DOUBLE                  dLon;
    DOUBLE                  dLat;
}PORTAL_UE_INFO_T;

typedef struct
{
    INT64                   i64GroupID;         // 组ID
    CHAR                    acGroupName[64];
    UINT32                  ulGroupType;
    UINT32                  ulFuncType;
    UINT32                  ulPriority;
    UINT32                  ulDCUserID;         // 创建者（DC用户）
}GROUP_INFO_T;

/// 对应数据库表 depot
typedef struct
{
    UINT32                  ulDepotID;          // 索引，0开始, 车辆段 ID
    UINT32                  ulTrainDirection;   ///< 列车行进方向, 0: 未知， 1: 上行， 2： 下行
    CHAR                    acDepotName[32+1];  ///< 车辆段名称
}DEPOT_INFO_T;

typedef struct
{
    UINT32                  ulStationIndex; // 索引，0开始
    UINT32                  ulStationID;
    CHAR                    acStationName[32+1];
    UINT32                  ulStationType;
    UINT32                  ulDepotID;
    UINT32                  ulInterLockStationID;
    INT64                   i64StationGroupID;      // 车站内部通话组
}STATION_INFO_T;

typedef struct
{
    UINT32                  ulDestID;
    UINT32                  ulStationID;
    CHAR                    acDestName[64+1];
}DEST_INFO_T;

/// 对应数据库表 brdzoneinfo， 广播区域 ID
typedef struct
{
    INT64                   i64GroupID;         ///< 广播组 ID
    UINT32                  ulStationID;        ///< 车站 ID
    UINT32                  ulZoneID;           ///< 区域 ID
    CHAR                    acZoneName[32+1];   ///< 区域名称
}BRD_ZONE_INFO_T;

#define TRAIN_TYPE_NORMAL           1   // 普通车
#define TRAIN_TYPE_ENGINE           2   // 工程车
#define TRAIN_TYPE_OPERATE          3   // 作业车

#define TRAIN_STATUS_ONLINE         0   // 正常
#define TRAIN_STATUS_OFFLINE        1   // 离线
#define TRAIN_STATUS_SILENT         2   // 静默（正常）

#define UE_STATUS_ONLINE            0   // 正常
#define UE_STATUS_OFFLINE           1   // 离线

// APP上报给调度台的信息
typedef struct
{
    UINT32      ulTrainUnitID;
    UINT32      ulTC;               // 1：车头 other:车尾
    UINT32      ulUserID;
    CHAR        acTrainID[16];
    INT64       i64CurrentGroupID;
    UINT32      ulStationID;
    UINT32      ulCoverStatus;  // COVER_STATUS_OPEN

    BOOL        bIsIPHOnline;
    UINT32      ulIPHStatus;

    BOOL        bIsPAOnline;
    UINT32      ulPAStatus;
    BOOL        bIsMasterPA;    // 主备状态

    UINT32      ulDriverID;
    UINT32      ulCellID;
    UINT32      ulCallType;
    INT64       i64CallID;
    UINT32      ulCurrentTime;
    UINT32      ulUptime;
    CHAR        acSerialNumber[64];
    CHAR        acAPPVer[64];
}APP_TRAIN_INFO_T;

typedef struct
{
    UINT8       aucIPHAddr[4];
    UINT32      ulMasterStatus;
    UINT32      ulIPHStatus;
    UINT16      ulIPHRecvAudioPort;
}IPH_AUDIO_NET_INFO_T;

typedef struct
{
    UINT32                  ulTrainUnitID;          // 车组号
    UINT32                  ulTrainType;            // 普通车、工程车
    CHAR                    acTrainName[16];
    INT64                   i64DefaultGroupID;
}TRAIN_T;

typedef struct
{
    UINT32                  ulUptime;               // 获取记录的uptime
    UINT32                  ulTimeDiff;             // 获取记录的时间与当前时间的差
    UINT32                  ulTrainUnitID;          // 车组号(车体号)
    UINT32                  ulTrainType;            // 普通车、工程车
    CHAR                    acTrainName[16];
    CHAR                    acTrainServiceNumber[16];   // 服务号
    CHAR                    acTrainSerialNumber[16];    // 序列号
    CHAR                    acTrainID[16];          // 车次号
    UINT32                  ulDriverID;             // 司机号
    UINT32                  ulComeTime;             // 进站时间（站区之间切换时间）
    UINT32                  ulParkTime;             // 到站时间
    UINT32                  ulLeaveTime;            // 离站时间
    CHAR                    acDestID[8];
    UINT32                  ulRollingStockNum;      // 几节编组
    UINT32                  ulDirection;            // 上下行方向
    UINT32                  ulDriveMode;            // 驾驶模式
    UINT32                  ulStationID;            // 当前车站
    UINT32                  ulUpsideStationID;      // 列车上行侧车站的编号(刚经过的车站，车辆段时为车辆段编号，列车停在车站时，同车站号）
    UINT32                  ulDownsideStationID;    // 列车下行侧车站的编号(将到达的车站，车辆段时为车辆段编号，列车停在车站时，同车站号）
    UINT32                  ulTurnbackTrackFlag;
    UINT32                  ulDepotTrackFlag;       // 是否在存车轨标识
    UINT32                  ulPlatformID;           // 不在站台时为0
    UINT32                  ulTransferTrackFlag;

    UINT32                  ulInOutDepot;           // 默认值为0，从停车场/车辆段进入转换轨时值为1；
                                                    // 当列车从转换轨进入停车场/车辆段时值为2（保持5秒）
    UINT32                  ulLostRealLocation;
    CHAR                    acTrackName[16];

    UINT16                  usSectionID;            // 列车所在物理区段ID
    UINT8                   ucIndexInSection;       // 同一物理区段内列车的次序, 取值从0开始
    INT32                   iTrainOrder;            // 列车相对车站的位置

    INT32                   iTrainStationPos;       // 列车相对车站的状态
    INT32                   iTrainStationPosIndex;  // 列车相对车站的位置相同时，其先后顺序，进站为-2，-1，在车站为0，出站为1，2

    INT64                   i64DefaultGroupID;      // 列车缺省通话组

    BOOL                    bRegister;              // 注册、注销状态
    UINT32                  ulRegisterDepotID;      // 注册的位置（正线/车辆段/停车场）

    DOUBLE                  dLon;                   // 经度
    DOUBLE                  dLat;                   // 纬度

    UINT32                  ulAppReportTime;        // APP上报消息时DIS记录下来的uptime
    UINT32                  ulTrainStatus;
    IPH_AUDIO_NET_INFO_T    stIPHAudioNetInfo;
    APP_TRAIN_INFO_T        astAppTrainInfo[2];
}TRAIN_INFO_T;

typedef struct
{
    UINT32              ulTrainUnitID;
    BOOL                bRegister;
    UINT32              ulRegisterDepotID;
    BOOL                bAutoRegister;
    BOOL                bRegisterOver;
    UINT32              ulTime;
    UINT32              ulDCNum;    // 当前待通知注册/注销的DC
    UINT32              aulDCUser[MAX_DC_NUM];
}REGISTER_INFO_T;

typedef struct
{
    BOOL        bSynchronization;
    UINT32      ulNtpPort;
    CHAR        acIPAddr[32];
    UINT32      ulPeriod;
    UINT32      ulServerTime;
    BOOL        bNtpServerValid;
}NTP_INFO_T;

typedef struct
{
    UINT8       aucNTPAddr[4];
    UINT16      usNTPPort;
    UINT32      ulNTPSyncPeriod;
}MRS_NTP_CFG;

typedef struct
{
    UINT32      ulTime;
    UINT32      ulTrainUserID;
    UINT32      ulTrainUnitID;
    UINT32      ulCellID;
    UINT32      ulStationID;
    UINT32      ulUpsideStationID;
    UINT32      ulDownsideStationID;
    UINT32      ulTurnbackTrackFlag;
    UINT32      ulTransferTrackFlag;
    UINT32      ulDirection;
}CELL_INFO_T;

//---------------------------- UE -------------------------------------------------
/// 对应数据库中的表 appcfg
typedef struct
{
    BOOL        bPTTAllow;                  ///< 车载台 PTT 按钮的受限情况
    UINT32      ulBroadcastTimeout;         ///< 广播超时时间
    UINT32      ulBroadcastVolume;          ///< 广播音量
    UINT32      ulHangoffTime;              ///< 呼叫超时自动挂断时间

    CHAR        acNTPServerAddr[16];        ///< NTP 服务端IP地址
    UINT32      ulNTPServerPort;            ///< NTP 服务端端口
    UINT32      ulNTPSyncPeriod;            ///< NTP 的同步周期，单位秒

    UINT32      ulFXAutoRebootPeriod;       ///< 固定台重启间隔（秒）
    UINT32      ulFXAutoRebootTime;         ///< 重启时刻，如1表示1点钟
}APP_BASIC_CFG_T;

// 预定义短信设备类型
#define SDS_TEMPLATE_TX             1   // 车载台
#define SDS_TEMPLATE_FX             2   // 固定台
#define SDS_TEMPLATE_ALL            3

/// 对应数据库 predefinedsms 表， 预定义短消息
typedef struct
{
    UINT32      ulSMSID;                // 预定义短信 ID
    UINT32      ulSDSType;              // 预定义短信类型
    UINT32      ulSMSTemplateType;      // 预定义短信设备类型
    CHAR        acSMSInfo[128];         // 预定义信息内容
}PREDEFINED_SMS_INFO_T;

// 预定义短信类型
#define SDS_TYPE_NULL                   0
#define SDS_TYPE_FORMAL_DISPATCH        1   // 书面命令
#define SDS_TYPE_ORAL_DISPATCH          2   // 口头命令
#define SDS_TYPE_OPERATE                3   // 运营信息发布
#define SDS_TYPE_STATUS                 7   // 状态短信
#define SDS_TYPE_EMERG_STATUS           8   // 紧急状态短信
#define SDS_TYPE_NORMAL                 9   // 普通消息
#define SDS_TYPE_SMS                    11  // 彩信

/**
 * @brief           对应数据库表 sdstemplate, 短信模板
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-16 11:09:22
 * @note
 */
typedef struct
{
    UINT32      ulTemplateID;               // 模板ID
    UINT32      ulSDSType;                  // 消息类型
    CHAR        acTitle[64];                // 模板标题
    CHAR        acSDSInfo[4000];            // 模板内容（包含模板关键字）
}SDS_TEMPLATE_T;

typedef struct
{
    UINT32      ulFormalDispatchSDSIndex;   // 书面命令工单编号
    UINT32      ulOralDispatchSDSDay;       // 口头命令日期
    UINT32      ulOralDispatchSDSIndex;     // 口头命令工单编号

    UINT32      ulOperateSDSDay;            // 运营信息日期
    UINT32      ulOperateSDSIndex;          // 运营信息工单编号
}SDS_INDEX_T;

typedef struct
{
    UINT32              ulSDSID;            // 唯一编号
    UINT32              ulSendUserID;       // 发送者
    CHAR                acSendUserName[32]; // 发送者名称
    UINT32              ulSendTime;         // 发送时间
    UINT32              ulSDSIndex;         // 命令（1-100）或运营信息编号
    UINT32              ulSDSType;          // 消息类型
    CHAR                acFile[256];        // 附件
    CHAR                acSDSInfo[4000];    // 消息内容
    BOOL                bNeedReply;         // 是否需要回执
    BOOL                bSendFlag;          // 发送状态：TRUE：已经发送
    UINT32              ulRecvUserNum;      // 接收者个数
    UINT32              ulRecvSuccUserNum;  // 成功接收者的个数
    UINT32              ulReplyUserNum;     // 确认的个数

    UINT32              ulRecvDCUserID;     // （调度台收到后）调度台ID
}SDS_INFO_T;

typedef struct
{
    UINT32              ulSDSID;            // SDS唯一编号
    UINT32              ulUserID;           // 接收者
    UINT32              ulRecvTime;         // 接收时间
    UINT32              ulSendResult;       // 发送状态：1成功
    UINT32              ulReplyTime;        // 回执时间
}SDS_REPLY_T;

#define CDR_TYPE_P2P        1
#define CDR_TYPE_PTT        10

typedef struct
{
    UINT32          ulDCUserID;
    UINT32          ulTime;
    UINT32          ulCallType;
    INT64           i64GroupID;
    UINT32          ulCaller;
    UINT32          ulCallee;
    UINT32          ulStatus;
}CDR_INFO_T;

typedef struct
{
    INT64           i64GroupID;
    UINT32          ulGroupStatus;
    BOOL            bNeedSave;
}GROUP_STATUS_T;

typedef struct
{
    UINT32          ulSeqID;
    UINT32          ulCallType;
    UINT32          ulCaller;
    UINT32          ulCallee;
    UINT32          ulTime;
    UINT32          ulStatus;
}P2P_CALL_INFO_T, P2PVIDEO_CALL_INFO_T;

typedef struct
{
    UINT32          ulSeqID;
    UINT32          ulDCUserID;
    INT64           i64GroupID;
    UINT32          ulTime;
    UINT32          ulSpeaker;
    UINT32          ulCallType;
}GROUP_CALL_INFO_T;

typedef struct
{
    UINT32          ulReqTime;
    UINT32          ulReqCallType;
    UINT32          ulReqUserID;        // 列车车载台号码或固定台号码
    UINT32          ulCaller;           // 发起方的列车车体号或车站ID

    UINT32          ulDCType;           // 被叫类型，缺省DC_TYPE_ALL

    UINT32          ulDCUserID;
    UINT32          ulAcceptTime;
}REQ_CALL_INFO_T;

/// 对应数据库表 recbrd ，预录音广播信息
typedef struct
{
    UINT32              ulRecID;        // 录音ID
    CHAR                acRecInfo[512]; // 录音内容
}REC_BRD_CFG_T;

typedef struct
{
    UINT32              ulBrdSeqID;        // 广播序号
    UINT32              ulTime;
    UINT32              ulRecID;
    CHAR                acRecInfo[512]; // 录音内容
    UINT32              ulBrdLoopTime;
    UINT32              ulDCUserID;
    CHAR                acTrainList[1024];
}REC_BRD_INFO_T;

typedef struct
{
    UINT32              ulBrdSeqID;
    UINT32              ulTrainUnitID;
    UINT32              ulReplyTime;
    INT32               iResult;
}REC_BRD_REPLY_T;

#define REC_BRD_RESULT_NULL     (-1)
#define REC_BRD_RESULT_FAIL     0
#define REC_BRD_RESULT_SUCC     1

typedef struct
{
    UINT32              ulBrdSeqID;
    UINT32              ulStartTime;
    UINT32              ulEndTime;
    UINT32              ulDCUserID;
    CHAR                acTrainList[1024];
}REAL_BRD_INFO_T;

// IPH对讲状态
//#define IPH_STATUS_NONE         0
#define IPH_STATUS_WAIT         1
#define IPH_STATUS_TALK         3
//#define IPH_STATUS_PREVIEW      4
#define IPH_STATUS_OVER         5
#define IPH_STATUS_WAIT_ACK     6
#define IPH_STATUS_ACK_ERR      0xff

typedef struct
{
    UINT32      ulApplyUptime;
    UINT32      ulApplyTime;
    UINT32      ulTrainUnitID;
    CHAR        acTrainID[16];
    UINT32      ulCarriageID;
    UINT32      ulIPHDevDoorID;
    UINT32      ulIPHDevID;
    UINT32      ulAcceptDCUserID;
    UINT32      ulAcceptTime;
    UINT32      ulGetACKTime;
    UINT32      ulStopTime;
    UINT32      ulStopUptime;
    UINT32      ulStatus;
    CHAR        acVideoURL[64];
    UINT8       aucIPHDevIP[4];
}IPH_TALK_INFO_T;

typedef struct
{
    UINT32      ulDCUserID;
    INT64       i64GroupID;
}SUB_GROUP_T;

typedef struct
{
    UINT32      ulDCUserID;
    UINT32      ulUserID;
}SUB_USER_T;

// 调度台信息
typedef struct
{
    UINT32      ulDevID;
    CHAR        acName[32];
    CHAR        acIPAddr[32];
    CHAR        acMacAddr[32];
    CHAR        acGateway[32];
}DC_DEV_INFO_T;

typedef struct
{
    DC_DEV_INFO_T   stDevInfo;

    BOOL        bOnline;
    CHAR        acVersion[32];
    UINT32      ulCpuUsage;
    UINT32      ulMemUsage;
    UINT32      ulDiskUsage;
    UINT32      ulSysUptime;
}DC_OMA_INFO_T;

// 录音录像用户
#define REC_OPER_TYPE_ADMIN     1   // 超级用户
#define REC_OPER_TYPE_NORMAL    2   // 普通用户
#define REC_OPER_TYPE_QUERY     3   // 查询用户

typedef struct
{
    CHAR                    acOperatorID[32];
    CHAR                    acName[32];
    CHAR                    acPassword[32];
    UINT32                  ulOperType;
}REC_OPER_INFO_T;

#define REC_CALL_TYPE_ALL               0

#define REC_CALL_TYPE_P2P_AUDIO         1   // 个呼
#define REC_CALL_TYPE_P2P_VIDEO         2   // 视频个呼
#define REC_CALL_TYPE_VIDEO_UPLOAD      3   // 视频上传

#define REC_CALL_TYPE_PTT_AUDIO         11  // 组呼
#define REC_CALL_TYPE_PTT_VIDEO         12  // 视频组呼

#define REC_CALL_TYPE_IPH               50  //
#define REC_CALL_TYPE_IPH_VIDEO         51  // 包括音视频
#define REC_CALL_TYPE_IPH_AUDIO         52  // 只包含音频

#define REC_CALL_TYPE_REAL_BRD          60  // 实时广播

// 录音录像记录
typedef struct
{
    CHAR        acFile[128];
    CHAR        acFileSuffix[8];
    UINT32      ulTime;      // 录音文件产生时间
    DOUBLE      dDuration;   // 录音文件时间长度（单位秒）
    UINT32      ulFileSize;  // 录音文件大小（单位字节）
    UINT32      ulFileType;  // 文件类型
    UINT32      ulCallType;  // 呼叫类型

    UINT32      ulTranUnitID;
    UINT32      ulCaller;    // 主叫
    UINT32      ulCallee;    // 被叫
    UINT32      ulDCUserID;
    INT64       i64GroupID;
}REC_FILE_INFO_T;

// GPS 信息
typedef struct
{
    UINT32                      ulDevID;    // 设备唯一标识（就是UserID）
    UINT32                      ulTime;     // 标准时间
    DOUBLE                      dLon;       // 经度
    DOUBLE                      dLat;       // 纬度
    FLOAT                       fHeight;    // 高度
    FLOAT                       fAzimuth;   // 方位角
    FLOAT                       fSpeed;     // 速度

    UINT8                       ucEastLon;  // 是否东经
    UINT8                       ucNorthLat; // 是否北纬
}GPS_INFO_T;

// 多选组信息
typedef struct
{
    UINT32      ulDCUserID;         // 创建者
    INT64       i64GroupID;         // 群组ID
    CHAR        acGroupName[64];    // 名称
    CHAR        acGroupMember[1024];// 组成员，最多30个组
}MULTI_GROUP_INFO_T;

// OCC广播类型
#define OCC_PA_TYPE_NORMAL         1    // 普通列车广播
#define OCC_PA_TYPE_MAINLINE       2    // 正线列车广播
#define OCC_PA_TYPE_UPLINE         3    // 上行列车广播
#define OCC_PA_TYPE_DOWNLINE       4    // 下行列车广播

// OCC实时广播类型
#define OCC_REAL_PA_TYPE_NONE            0  // 无
#define OCC_REAL_PA_TYPE_PCM             1  // 数字语音流
#define OCC_REAL_PA_TYPE_TRUNK           2  // 集群模式

#pragma pack(pop)

#endif

