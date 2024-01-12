#ifndef PIS_H
#define PIS_H

// PIS消息
#define EV_PIS_GET_TRAIN_INFO           0x0301  // 查询列车信息请求、应答
#define EV_PIS_SELFCHECK_STATUS         0x0302  // 车载台自检状态上报请求、应答

#define EV_PIS_HEART_BEAT               0x0401  // 系统心跳（状态检测）请求、应答
#define EV_IPH_TALK_APPLY               0x0402  // 申请对讲请求、应答
#define EV_IPH_TALK_APPLY_ACK           0x0403  // 对讲确认应答
#define EV_IPH_AUDIO_UPLOAD             0x0404  // IPH音频上传
#define EV_IPH_AUDIO_DOWNLOAD           0x0405  // 调度台音频下发
#define EV_IPH_STOP_TALK                0x0406  // 对讲结束通知
#define EV_IPH_PAUSE_TALK               0x0407  // 通话挂起请求/应答
#define EV_IPH_RENEW_TALK               0x0408  // 通话挂起恢复请求/应答
#define EV_IPH_DEV_RESET                0x0409  // IPH设备复位请求/应答

#define EV_PA_REC_CFG                   0x0501  // 获取预录音广播编码表请求/应答
#define EV_PA_GET_SYS_STATUS            0x0502  // 获取PA系统状态请求/应答；认为是心跳
#define EV_PA_REC_BRD                   0x0503  // 预录音广播请求/应答

#define EV_PA_START_REAL_BRD            0x0601  // 人工广播请求/应答
#define EV_PA_PUSH_AUDIO_DATA           0x0602  // 人工广播语音下发
#define EV_PA_STOP_REAL_BRD             0x0603  // 人工广播结束请求/应答

#pragma pack(push, 1)

// 消息头
#define PIS_MSG_MAGIC           0xfe01  // PIS协议头

#define PA_AND_PIS_HEART_BEAT   3


typedef struct
{
    UINT16      usMsgMagic;
    UINT16      usMsgType;  // 命令字
    UINT16      usMsgLen;   // 字长
}PIS_MSG_HDR_T, PA_MSG_HDR_T;

#define MAX_IPH_DEV_NUM             64  // IPH请求最大数量

//车载台自检状态
#define APP_SELF_CHECK_STATUS_SUCC      0
#define APP_SELF_CHECK_STATUS_FAILED    1
#define APP_SELF_CHECK_STATUS_UNKNOWN   (-1)

//IPH检测ACK最大时长-秒级
#define IPH_CHECK_ACK_MAX_TIME      3

//IPH连接状态
#define PIS_LINK_STATUS_OFFLINE     0   // 离线
#define PIS_LINK_STATUS_ONLINE      1   // 在线

// 单个IPH对讲应答结果数据
#define IPH_APPLY_ACCEPT            1   // 允许对讲
#define IPH_APPLY_REJECT            2   // 拒绝对讲
#define IPH_APPLY_WAIT              3   // 排队等待中

//IPH系统运行状态
#define IPH_STATUS_NORMAL           0   // 正常
#define IPH_STATUS_ABNORMAL         1   // 异常

//IPH一键复位
#define IPH_TALK_RESET              0xff // 在IPH结束报文中报警器车厢号以及报警器ID填入0xff时认为是一键复位请求/应答

//对讲挂起处理结果
#define IPH_PAUSE_SUCC              0    // 正常挂起， 其他值异常

//IPH设备复位
#define IPH_DEV_RESET_SUCC          0    // IPH设备复位成功

//盖状态
#define COVER_STATUS_OPEN           1   // 打开
#define COVER_STATUS_CLOSE          0   // 关闭

//驾驶模式
#define DRIVE_MODE_MANUAL           1   // 有人驾驶
#define DRIVE_MODE_AUTO             0   // 无人驾驶

//PA系统状态
#define PA_STATUS_NA                0   // 系统不可用
#define PA_STATUS_IDLE              1   // 空闲
#define PA_STATUS_WORK              2   // 广播中

#define PA_SLAVE_STATUS             0   // 备用
#define PA_MASTER_STATUS            1   // 主用
#define PA_STATUS_UNKNOW            0xff   // 未知

//PA预录音广播应答状态
#define PA_REC_BRD_FAIL             0   // 预录音广播失败
#define PA_REC_BRD_SUCC             1   // 预录音广播成功
#define PA_REC_BRD_BUSY             2   // 广播设备忙

//PA全车广播
#define PA_REAL_BRD_ALL_CARRIAGE    0xff // 全车广播， 1~8：单车厢广播
#define PA_REAL_BRD_MIN_CARRIAGE_ID    1
#define PA_REAL_BRD_MAX_CARRIAGE_ID    8

// PCM数据配置
#define SAMPLE_RATE_8000            8000 // 8K采样频率
#define SAMPLE_RATE_16000           16000 // 16K采样频率

#define SAMPLE_BITS_8               8    // 8bit采样位数
#define SAMPLE_BITS_16              16   // 16bit采样位数

// 查询列车信息应答
typedef struct
{
    UINT8       ucCoverStatus;  // 1：打开 0：关闭
    UINT8       ucDriverMode;   // 1：打开 0：关闭
}PIS_GET_TRAIN_INFO_RSP_T;

// 车载台自检状态上报请求
typedef struct
{
    UINT8       ucCheckStatus;  // 自检状态：0：正常 其他：异常
}PIS_SELFCHECK_STATUS_REQ_T;

typedef struct
{
    CHAR       ucStatus;       // 工作状态，0：正常，其他：异常
}IPH_GET_SYS_STATUS_RSP_T;

// 单个IPH对讲申请请求数据
typedef struct
{
    UINT16      usTrainUnitID;         // 车组号（车体号）
    UINT8       ucCarriageID;          // 车厢号
    UINT8       ucIPHDevID;            // 报警器ID
    UINT16      usIPHDevDoorID;        // 报警器对应的车门号
    UINT8       aucIPHRecvAudioAddr[4]; // IPH服务器接收音频的地址
    UINT16      usIPHRecvAudioPort;    // IPH服务器接收音频的地址
    CHAR        acVideoUrl[84];        // 摄像机视频URL
    CHAR        acCCTVVideoUrl[84];    // CCTV摄像机视频URL, 预留填0
}IPH_TALK_APPLY_REQ_INFO_T;

typedef struct
{
    UINT8                       ucNum;              // IPH报警器个数
    IPH_TALK_APPLY_REQ_INFO_T   astApplyReq[MAX_IPH_DEV_NUM];   // IPH对讲请求数据
}IPH_TALK_APPLY_REQ_T;

typedef struct
{
    UINT16      usTrainUnitID;          // 车组号（车体号）
    UINT8       ucCarriageID;           // 车厢号
    UINT8       ucIPHDevID;             // 报警器ID
    UINT16      usIPHDevDoorID;         // 报警器对应的车门号
    UINT8       ucApplyResult;          // 申请对讲结果 1:允许对讲 2:拒绝对讲 3:排队等待中
    UINT8       aucAudioRecvAddr[4];    // IPH上传对讲音频的地址
    UINT16      usAudioRecvPort;        // IPH上传对讲音频的端口
}IPH_TALK_APPLY_RSP_INFO_T;

// IPH对讲申请应答
typedef struct
{
    UINT8                       ucNum;              // IPH报警器个数
    IPH_TALK_APPLY_RSP_INFO_T   astApplyRsp[MAX_IPH_DEV_NUM];   // IPH对讲请求应答数据
}IPH_TALK_APPLY_RSP_T;

// IPH 请求接听确认
typedef struct
{
    UINT16      usTrainUnitID;  // 车组号（车体号）
    UINT8       ucCarriageID;   // 车厢号
    UINT8       ucIPHDevID;     // 报警器ID
    UINT16      usIPHDevDoorID; // 报警器对应的车门
}IPH_TALK_APPLY_ACK_T;

// 结束对讲
typedef struct
{
    UINT16      usTrainUnitID;  // 车组号（车体号）
    UINT8       ucCarriageID;   // 车厢号 0xff：表示一键复位
    UINT8       ucIPHDevID;     // 报警器ID 0xff: 表示一键复位
}IPH_TALK_STOP_REQ_T, IPH_TALK_STOP_RSP_T;

// 对讲挂起请求
typedef struct
{
    UINT16      usTrainUnitID;  // 车组号（车体号）
    UINT8       ucCarriageID;
    UINT8       ucIPHDevID;
}IPH_TALK_PAUSE_REQ_T;

// 对讲挂起应答
typedef struct
{
    UINT16      usTrainUnitID;  // 车组号（车体号）
    UINT8       ucCarriageID;
    UINT8       ucIPHDevID;
    UINT8       ucIPHPauseResult; // 0:挂起成功正常， 其他值：挂起失败
}IPH_TALK_PAUSE_RSP_T;

//IPH复位请求
typedef struct
{
    UINT8       ucIPHResetStatus; // 0:正常， 其他值：异常
}IPH_DEV_RESET_REQ_T;

//IPH复位应答
typedef struct
{
    UINT8       ucIPHResetACK; // 0:成功， 其他值：异常
}IPH_DEV_RESET_RSP_T;

typedef struct
{
    UINT32      ulBrdSeqID;     // 广播序号
    UINT8       ucCarriageID;   // 0xff：全车广播， 1~8：单车厢广播
}PA_REAL_BRD_REQ_T;

typedef struct
{
    UINT32      ulBrdSeqID;         // 广播序号
    UINT16      usPARecvAudioPort;  // PA接收语音数据的端口
    UINT8       ucBrdResult;        // 人工广播结果  0：失败 1：成功
}PA_REAL_BRD_RSP_T;

// 预录音广播请求
typedef struct
{
    UINT32      ulRecID;        // 预录音广播编号
    UINT32      ulBrdSeqID;     // 广播序号
    UINT8       ucBrdLoopTime;  // 广播循环次数 缺省为1
}PA_REC_BRD_REQ_T;

typedef struct
{
    UINT32      ulRecID;        // 预录音广播编号
    UINT32      ulBrdSeqID;     // 广播序号
    UINT8       ucRet;          // 0：失败 1：成功 2: 系统忙（正在广播中）
}PA_REC_BRD_RSP_T;

typedef struct
{
    UINT8       ucIsMaster;     // 主备状态，0：备用 1：主用
    UINT8       ucStatus;       // 工作状态，0：系统不可用，1：空闲中，2：正在广播中
}PA_GET_SYS_STATUS_RSP_T;



typedef struct
{
    UINT16      usTrainUnitID;   // 车组号（车体号）
    UINT8       ucCarriageID;   // 车厢号
    UINT8       ucIPHDevID;     // 报警器ID

    UINT32      ulPCMPkgIndex;  // PCM数据包序号，第一个数据包为0，后续递增
    UINT32      ulPCMPkgTime;   // PCM数据包偏移时间，相对第一个PCM数据包的相对偏移时间，单位ms
    UINT16      usSampleRate;   // 采样频率（Hz）   8000/16000/44100等
    UINT8       ucChanNum;      // 采样声道数   1：单声道  2：双声道
    UINT8       ucSampleBits;   // 采样量化位数 8：8位（1字节） 16：16位（2字节）
    UINT16      usPCMDataLen;   // 数据长度(=usSampleRate*ucChanNum*ucSampleBits/8)
    UINT8       aucPCMData[1];  // PCM音频数据
}IPH_PCM_DATA_T;

typedef struct
{
    UINT32      ulBrdSeqID;      // 广播序号
    UINT32      ulPCMPkgIndex;   // PCM数据包序号 第一个数据包为0 后续递增
    UINT16      usSampleRate;    // 采样频率，8000/16000 优先使用8000
    UINT8       ucChanNum;       // 采样声道数   1：单声道  2：双声道
    UINT8       ucSampleBits;    // 采样量化位数 8：8位（1字节） 16：16位（2字节）
    UINT16      usPCMDataLen;    // 数据长度  不大于16384(=usSampleRate*ucChanNum*ucSampleBits/8)
    UINT8       aucPCMData[1];   // PCM音频数据
}PA_PCM_DATA_T;

// 结束人工广播
typedef struct
{
    UINT32      ulBrdSeqID;      // 广播序号
}PA_STOP_REAL_BRD_REQ_T, PA_STOP_REAL_BRD_RSP_T;

#pragma pack(pop)

#endif

