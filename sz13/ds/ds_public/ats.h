#ifndef ATS_H
#define ATS_H

// 本接口适配北交控ATS

#define ATS_REC_MAGIC               "ATS_TCT"   // 北交控

#define ATS_SERVER_PORT             2100

#define ATS_FRAME_HEAD              0xEFEF
#define ATS_FRAME_TAIL              0xFDFD

#define ATS_MAX_DATA_LEN            1024

#define ATS_VER                     0x01

#define ATS_MSGID_HEARTBEAT         0x01    // 心跳消息
#define ATS_MSGID_ATSINFO           0x05    // 列车位置信息

#define ATS_MAX_TRAIN_NUM           120

#pragma pack(push, 1)

typedef struct
{
    UINT16  usFrameHead;    // 帧头，固定为0xEF,0xEF
    UINT8   ucFrameCount;   // 总帧数
    UINT8   ucFrameIndex;   // 当前帧序号，从1开始
    UINT16  usDataLen;      // 表示Data域的长度
}ATS_FRAME_HDR_T;

typedef struct
{
    UINT16      usYear;
    UINT8       ucMonth;
    UINT8       ucDay;
    UINT8       ucHour;
    UINT8       ucMin;
    UINT8       ucSec;
}ATS_TIME_T;

typedef struct
{
    UINT16      usMsgLen;   // 消息长度，不包括长度本身，<=65533
    ATS_TIME_T  stMsgTime;  // 消息发送时间(UTC)
    UINT16      usLineID;   // 线路号
    UINT8       ucResv[18]; // 预留
    UINT8       ucMsgID;    // 消息ID, 用于区分信息包的类型
    UINT8       ucVer;      // 版本号，本协议为0x01
}ATS_MSG_HDR_T;

// 列车激活端
#define ATS_TC1                 0x01 // TC1端为当前的激活端
#define ATS_TC2                 0x02 // TC2端为当前的激活端
#define ATS_INVALID_TC          0xFF // 无效

// 列车方向
#define ATS_DIRECTION_UP        0   // 上行
#define ATS_DIRECTION_DOWN      1   // 下行
#define ATS_DIRECTION_UNKNOWN   2   // 未知

// 是否在折返轨或存车轨
#define ATS_ON_TURNBACK_TRACK_NO      0   // 不在
#define ATS_ON_TURNBACK_TRACK_YES     1   // 在

// 出入库标识
#define ATS_INOUT_DEPOT_STATE_UNKNOW    0xff  // 未知
#define ATS_INOUT_DEPOT_STATE_OUT       0     // 列车从停车场/车辆段进入正线
#define ATS_INOUT_DEPOT_STATE_IN        1     // 列车从正线进入停车场/车辆段

// 是否在转换轨
#define ATS_ON_TRANSFER_NO          0   // 不在
#define ATS_ON_TRANSFER_YES         1   // 在

// 是否在站台
#define ATS_ON_PLATFORM_NO          0   // 不在
#define ATS_ON_PLATFORM_YES         1   // 在

// 是否失去真实位置
#define ATS_LOST_REAL_LOCATION_NO   0 // 没有失去真实位置信息
#define ATS_LOST_REAL_LOCATION_YES  1 // 失去真实位置信息

// 紧急刹车
#define ATS_EB_NO                   0x55    // 无紧急制动
#define ATS_EB_ON                   0xAA    // 紧急制动

// 驾驶模式
#define ATS_DRIVE_MODE_AM           1   // 有人的自动驾驶
#define ATS_DRIVE_MODE_CM           2   // 人工驾驶
#define ATS_DRIVE_MODE_RM           3   // 限速25的人工驾驶
#define ATS_DRIVE_MODE_EUM          4   // 切除信号驾驶
#define ATS_DRIVE_MODE_FAM          5   // 全自动驾驶
#define ATS_DRIVE_MODE_CAM          6   // 限速25的全自动驾驶
#define ATS_DRIVE_MODE_RRM          7   // 限速25的全自动驾驶, rrm的功能比cam更少
#define ATS_DRIVE_MODE_NULL         0xff

typedef struct
{
    //UINT8   ucTrainNum;     // 列车数目
    //  Repeat begin
    UINT8       ucTrainUnitIDLen;
    CHAR        acTrainUnitID[4];       // 车组号，固定编组列车的车组号不变，取值范围0001—9999
    UINT8       ucServiceNumberLen;
    CHAR        acServiceNumber[3];     // 服务号，以列车出车辆段/停车场顺序编号，在一天的服务中保持不变，取值范围001—999
    UINT8       ucOrderNumberLen;
    CHAR        acOrderNumber[4];       // 车次号，随着列车的折返和上下行的变化而变化，上行为偶数，下行为奇数，取值范围01—99
    UINT8       ucDestIDLen;
    CHAR        acDestID[3];            // 目的地码，取值范围001—999
    UINT16      usRtuID;                // 所在集中站站号
    UINT16      usStationID;            // 所在车站的编号
    UINT16      usUpsideStationID;      // 上行侧车站的编号, 在停车场/车辆段时，取值为停车场/车辆段编号；列车在车站时为车站ID
    UINT16      usDownsideStationID;    // 下行侧车站的编号, 在停车场/车辆段时，取值为停车场/车辆段编号；列车在车站时为车站ID
    UINT8       ucActiveTCID;           // 列车的激活端
    UINT8       ucDirection;            // 运行方向, 0：上行 1：下行 2：未知
    UINT8       ucInOutDepot;           // 出入库标识, 0：从停车场/车辆段进入正线 1：从正线进入停车场/车辆段
    UINT8       ucOnTransferTrackFlag;  // 0：车头压出即认为不在转换轨 1：车头压入即认为在转换轨；
    UINT8       ucOnTurnBackTrackFlag;  // 0：列车不在折返轨或存车轨 1：列车在折返轨或存车轨
    UINT8       ucIsOnPlatform;         // 0：列车不在车站站台 1：列车在车站站台
    UINT8       ucLostRealLocation;     // 0：列车没有失去真实位置 1：列车失去真实位置, 丢失位置后，其他信息保持上一帧的状态
    UINT16      usLogicSectionID;       // 列车所在逻辑区段ID 以逻辑区段定位列车位置
    UINT16      usPhysicSectionID;      // 列车所在物理区段ID
    UINT8       ucIndexInSection;       // 同一物理区段内列车的次序, 取值从0开始
    UINT8       ucCompartmentNum;       // 编组数量
    UINT16      usDriverNumber;         // 驾驶员编号
    UINT8       ucTimeoutStatus;        // 0：无区间停车超时； 1：有区间停车超时；
    INT32       iOtpTime;               // 早晚点时间（单位：秒，-215- +215 ，正数表示列车晚点秒数，负数表示列车早点秒数）
    ATS_TIME_T  stArriveTime;           // 列车到达时间(本地时区)
    ATS_TIME_T  stDepartTime;           // 列车离开时间(本地时区)
    UINT8       ucDriveMode;            // 驾驶模式：
    UINT8       ucIsOpenDoor;           // 车门状态：
    UINT8       ucIsStopped;            // 停车状态：
    UINT8       ucControlMode;          // 运行控制级别：
    UINT8       ucARFlag;               // 列车换端过程中，原尾端ATP与ZC建立通讯后到折返流程完成之前的状态
    UINT8       ucEBFlag;               // 无紧急制动：0x55；紧急制动：0xAA；其它：非法。
    UINT8       ucRate;                 // 满载率（百分比，例如50，表示50%）
    UINT8       ucSpeed;                // 速度（KM/H）
    UINT8       ucStopDeviation;        // 停车过标欠标标志：0x55=过标或者正值，0xAA=欠标或者负值0xFF=默认值，未发生停站行为其他无效
    UINT8       ucSnowStatus;           // 雨雪模式：0-默认1-雨雪模式
    UINT8       ucFireStatus;           // 火灾报警：0-默认1-发生车辆火灾
    UINT8       aucResv[5];
}ATS_INFO_T;

typedef struct
{
    ATS_MSG_HDR_T   stMsgHdr;
    UINT8           ucResv[8];
}ATS_MSG_HEARTBEAT_T;

typedef struct
{
    ATS_MSG_HDR_T   stMsgHdr;
    UINT8           ucTrainNum;
    ATS_INFO_T      astATSInfo[ATS_MAX_TRAIN_NUM];
}ATS_MSG_ATSINFO_T;

typedef struct
{
    CHAR        acMagic[8];     // "ATS_XXX"
    UINT32      ulTime;
    UINT8       aucAddr[4];
    ATS_MSG_ATSINFO_T   stRecData;      // 实际的长度是变长的
}ATS_REC_T;

#pragma pack(pop)

#endif
