//---------------------------------------------------------------------------

#ifndef eSDKH
#define eSDKH
//---------------------------------------------------------------------------

typedef VOID (__stdcall * eSDK_EventCallBack)(INT32 iEventType, VOID* pEventBuf, UINT32 uiBufSize, VOID* pUserData);


#define DC_PRIV_BREAK_OFF_CALL      1   // 强拆。
#define DC_PRIV_BREAK_IN_CALL       2   // 抢话。
#define DC_PRIV_DISCREET_LISTEN     4   // 缜密监听。
#define DC_PRIV_RECV_VIDEO          8   // 视频监控。
#define DC_PRIV_EXPORT_SDS          16  // 短消息导出。
#define DC_PRIV_ENV_LISTEN          32  // 环境侦听。
#define DC_PRIV_VIDEO_WALL          64  // 视频上墙。

#define DC_ROLE_ADMIN               0   // 管理员。
#define DC_ROLE_NORMAL_DC           1   // 普通调度员
#define DC_ROLE_UPPER_DC            2   // 上级调度员

typedef struct
{
    UINT32      ulPriviledge;   // 调度台特权
    UINT32      ulRole;         // 角色
    CHAR        acAlias[64];    // 调度台别名
}CURR_DC_INFO_T;

#define REC_TYPE_P2P_AUDIO      0   // 语音点呼
#define REC_TYPE_P2P_VIDEO      1   // 视频点呼
#define REC_TYPE_VIDEO_UPLOAD   2   // 视频回传
#define REC_TYPE_PTT            3   // 群组呼叫
#define REC_TYPE_IPH            50  // 乘客对讲

typedef struct
{
    UINT32      ulRecType;
    UINT32      ulCaller;
    UINT32      ulCallee;
    INT64       i64ResourceID;
    UINT32      ulStartTime;
    UINT32      ulEndTime;
    AnsiString  strHttpUrl;     // 下载录音录像的地址
    AnsiString  strRtspUrl;     // 查看录音录像的流媒体链接
}ESDK_REC_FILE_INFO_T;

typedef struct
{
    INT32       iEventType;
    UINT32      ulMsgLen;
    CHAR        acMsg[4];       // 需要动态分配实际内存
}ESDK_EVENT_T;

// 用户状态
#define USER_STATUS_UNKNOWN             4000    // 未知状态，
#define USER_STATUS_REGOK               4011    // 资源注册成功，
#define USER_STATUS_REGFAIL             4012    // 资源注册失败，
#define USER_STATUS_INVITING            4020    // 资源正在呼叫中
#define USER_STATUS_RINGING             4021    // 资源正在振铃音，
#define USER_STATUS_TXMTING             4022    // 资源正在通话或者转移呼叫中，
#define USER_STATUS_TXMTIDLE            4023    // 资源空闲
#define USER_STATUS_HALFVOICE_ACTIVATED 4040    // 半双工通话中
#define USER_STATUS_HALFVOICE_CLOSED    4041    // 半双工通话关闭

// 呼叫类型：
#define USER_CALL_TYPE_UNREG            0       // 未注册
#define USER_CALL_TYPE_REG              1       // 已注册
#define USER_CALL_TYPE_VOICEP2P         2       // 视频点呼
#define USER_CALL_TYPE_VIDEOP2P         3       // 语音点呼
#define USER_CALL_TYPE_UPLOADP2P        4       // 点呼上传
#define USER_CALL_TYPE_VIDEODISPATCH    5       // 视频分发
#define USER_CALL_TYPE_VIDEOONWALL      6       // 视频上墙
#define USER_CALL_TYPE_VOICEPTT         7       // 音频组呼
#define USER_CALL_TYPE_ENVLISTEN        8       // P2P环境侦听

// 视频格式
#define VIDEO_FMT_CIF       0
#define VIDEO_FMT_QCIF      1
#define VIDEO_FMT_D1        2
#define VIDEO_FMT_720P      3
#define VIDEO_FMT_1080P     4

#define CAMERA_TYPE_FRONT       0   // 前置摄像头
#define CAMERA_TYPE_BACK        1   // 后置摄像头

#define VIDEO_VOICE_OPEN        0   // 需要伴音
#define VIDEO_VOICE_MUTE        1   // 不需要伴音

#define VIDEO_CALL_CONFIRM_NONE 0   // 不需要用户确认
#define VIDEO_CALL_CONFIRM_NEED 1   // 需要用户确认


//<Content><UserID>7007</UserID><UserName></UserName><StatusType>16</StatusType><StatusValue>4011</StatusValue><AttachingGroup>0</AttachingGroup></Content>
typedef struct
{
    UINT32  ulUserNumber;           // 用户终端号码

    UINT32  ulStatusType;           // 状态类型（固定值）
    UINT32  ulStatus;               // 状态
}EVENT_USER_STATUS_T;

#define P2P_IND_STATUS_RECEIVED                 2002 // 指示调度台收到点呼请求 ，SDK会自动播放振铃音（上层应用需要激活界面 ，提醒用户接听或拒接）
#define P2P_IND_STATUS_ANSWERED                 2003 // 指示调度台（做主叫时）对端已接听 ，并已发送200 OK消息此时调度台开始建立媒体面RTP连接
#define P2P_IND_STATUS_EMERGENCY                2004 // 指示调度台收到紧急呼叫请求 ，SDK会自动接听（上层应用可以通过声光等方式来提醒调度员）
#define P2P_IND_STATUS_PROCEEDING               2005 // 指示调度台收到SIP 100、 SIP 101等
#define P2P_IND_STATUS_RINGING                  2006 // 指示调度台收到SIP 180 ，SDK会自动播放回铃音（上层应用需要激活界面 ，提醒用户可以取消呼叫）
#define P2P_IND_STATUS_ACKED                    2007 // 指示调度台（做被叫时）收到对端的ACK响应 ，并开始建立媒体面连接
#define P2P_IND_STATUS_RELEASED                 2008 // 指示对端拒绝接听
#define P2P_IND_STATUS_HANGUPED                 2009 // 指示对端已挂断
#define P2P_IND_STATUS_HANGUPED_ACTIVE          2010 // 指示已主动挂断
#define P2P_IND_STATUS_CANCELLED                2011 // 指示点呼请求已取消
#define P2P_IND_STATUS_PREMPTED                 2012 // 指示点呼已被取代
#define P2P_IND_STATUS_RELEASED_BUSY            2013 // 指示对端忙
#define P2P_IND_STATUS_BREAKIN_FAIL             2014 // 指示抢权失败
#define P2P_IND_STATUS_BREAKIN_SUCCESS          2015 // 指示抢权成功
#define P2P_IND_STATUS_PROHIBITED               2016 // 指示无权限
#define P2P_IND_STATUS_REMOTE_NOANSWER          2017 // 指示对端无应答
#define P2P_IND_STATUS_REMOTE_NOTFOUND          2018 // 指示对端未找到
#define P2P_IND_STATUS_LICENSE_LIMIT            2019 // 指示证书受限
#define P2P_IND_STATUS_MEDIAPLANE_ERROR         2020 // 指示音频流组织失败
#define P2P_IND_STATUS_MEDIAPLANE_MUTE          2021 // 指示音频被静音
#define P2P_IND_STATUS_MEDIAPLANE_UNMUTE        2022 // 指示音频被去静音
#define P2P_IND_STATUS_REMOTE_NOTSUPPORTED      2023 // 指示远端不支持要求的音频格式
#define P2P_IND_STATUS_REMOTE_NOTCONNECTED      2024 // 指示远端未注册
#define P2P_IND_STATUS_TRANSFER_SUCCESS         2025 // 指示语音转接成功
#define P2P_IND_STATUS_TRANSFER_FAIL            2026 // 指示语音转接失败
#define P2P_IND_STATUS_TRANSFER                 2027 // 指示语音转接
#define P2P_IND_STATUS_SIGNAL_ERROR             2028 // 指示SIP错误
#define P2P_IND_STATUS_EMERGENCY_BEGIN          2029 // 指示收到了远端终端的一路紧急呼叫
#define P2P_IND_STATUS_RELEASED_POWEROFF        2033 // 远程断电
#define P2P_IND_STATUS_PICKEDUP                 2040 // 指示被叫端已接听
#define P2P_IND_STATUS_HALFDUPLEX_BEGIN         2041 // 指示收到半双工点呼请求
#define P2P_IND_STATUS_TX_BEGIN                 2042 // 指示半双工点呼开始
#define P2P_IND_STATUS_TX_IDLE                  2043 // 指示半双工点呼空闲
#define P2P_IND_STATUS_TX_VOICE                 2044 // 指示半双工点呼音频
#define P2P_IND_STATUS_FC_ACCEPTED              2045 // 指示半双工点呼被接收
#define P2P_IND_STATUS_FC_REJECTED              2046 // 指示半双工点呼被拒绝
#define P2P_IND_STATUS_ENVIR_ANSWERED           2047 // 指示环境侦听被应答
#define P2P_IND_STATUS_CALLER_UNSUPPORT         2048 // 指示呼叫发起方不被支持
#define P2P_IND_STATUS_CALLEE_UNSUPPORT         2049 // 指示被呼叫方不被支持
#define P2P_IND_STATUS_CIPHER_NOT_AVAILABLE     2050 // 指示混杂警告
#define P2P_IND_STATUS_MISCELLANEOUS_WARNING    2051 // 指示对端无权限
#define P2P_IND_STATUS_REMOTE_PROHIBITED        2052 // 指示对端无权限
#define P2P_IND_STATUS_EMERGENCY_WAIT           2053 // 紧急语音呼叫正在等待接听
#define P2P_IND_STATUS_RELEASED_GENERAL_ERROR   2054 // 发生普通错误

// resource 资源状态类型
#define RESOURCE_STATUS_TYPE_REG_PROXY          15      // 资源注册状态指示
#define RESOURCE_STATUS_TYPE_USERDGNA           21      // 用户动态重组状态，
#define RESOURCE_STATUS_TYPE_VIDEODISPATCH      22      // 视频分发状态，
#define RESOURCE_STATUS_TYPE_MUTE               23      // 静音状态
#define RESOURCE_STATUS_TYPE_GRPPATCH           24      // 派接操作状态

// resource 资源状态
#define RESOURCE_STATUS_UNKNOWN                 4000    // 未知状态
#define RESOURCE_STATUS_ASSIGNED                4003    // 资源指派状态
#define RESOURCE_STATUS_DEASSIGNED              4004    // 资源去指派状态
#define RESOURCE_STATUS_LISTENING               4005    // 资源监听中状态
#define RESOURCE_STATUS_LISTENEND               4006    // 资源监听结束状态
#define RESOURCE_STATUS_REGOK                   4011    // 资源注册成功
#define RESOURCE_STATUS_REGFAIL                 4012    // 资源去注册
#define RESOURCE_STATUS_FORBIDEN                4013    // 资源未授权
#define RESOURCE_STATUS_PASSWORD_WRONG          4014    // 用户名或者密码错误
#define RESOURCE_STATUS_TEMP_UNAVAILABLE        4015    // 资源暂时不可用
#define RESOURCE_STATUS_RES_CONFILCT            4016    // 资源冲突
#define RESOURCE_STATUS_UNAUTHORIZED            4017    // 未鉴权
#define RESOURCE_STATUS_LICENSE_LIMIT           4018    // License受限
//#define RESOURCE_STATUS_PASSWORD_WRONG          4019    // 账号或密码错误
#define RESOURCE_STATUS_DGNAOK                  4024    // 资源动态重组成功
#define RESOURCE_STATUS_DGNAFAIL                4025    // 资源动态重组失败
#define RESOURCE_STATUS_DGNA_UEFAIL             4026    // 资源动态重组存在用户设备失败
#define RESOURCE_STATUS_GROUP_ACTIVATED         4027    // 资源群组有效
#define RESOURCE_STATUS_GROUP_DEACTIVATED       4028    // 资源群组无效
#define RESOURCE_STATUS_MUTE                    4029    // 执行静音
#define RESOURCE_STATUS_UNMUTE                  4030    // 取消静音
#define RESOURCE_STATUS_GRPPATCH_CREATEOK       4031    // 派接组创建成功
#define RESOURCE_STATUS_GRPPATCH_CREATEFAIL     4032    // 派接组创建失败
#define RESOURCE_STATUS_GRPPATCH_CANCELOK       4033    // 派接组删除成功
#define RESOURCE_STATUS_GRPPATCH_CANCELFAIL     4034    // 派接组删除失败
#define RESOURCE_STATUS_GRPPATCH_ADDOK          4035    // 派接组增加成员成功
#define RESOURCE_STATUS_GRPPATCH_ADDFAIL        4036    // 派接组增加成员失败
#define RESOURCE_STATUS_GRPPATCH_DELOK          4037    // 派接组删除成员成功
#define RESOURCE_STATUS_GRPPATCH_DELFAIL        4038    // 派接组删除成员失败
#define RESOURCE_STATUS_ACCOUNT_LOCKED          4060    // 账号被锁定

typedef struct
{
    INT64       i64ResourceID;
    CHAR        acResourceName[64];
    UINT32      ulStatusType;           // 状态类型（固定值）
    UINT32      ulStatusValue;          // 状态

    INT64       i64AttachingGroupID;    // 用户当前加入的组号
    CHAR        acSpeaker[64];          // 主讲人
    UINT32      ulCameraID;             // 被分发的资源ID
}EVENT_RESOURCE_STATUS_T;

typedef struct
{
    UINT32  ulP2PCallStatus;        // 点呼状态

    UINT32  ulCaller;               // 主叫
    UINT32  ulCallee;               // 被叫
    UINT32  ulCallID;               // 被叫
}EVENT_P2PCALL_STATUS_T;

// 组呼状态：
#define PTT_IND_STATUS_UNKNOWN              1000    // 保留。
#define PTT_IND_STATUS_RECEIVED             1001    // 组呼请求被接收。
#define PTT_IND_STATUS_REQUEST              1002    // 保留
#define PTT_IND_STATUS_ACCEPT               1003    // 组呼之后收到调度机200 OK消息，通知话权授予的状况为接收。
#define PTT_IND_STATUS_WAITING              1004    // 保留
#define PTT_IND_STATUS_REJECT               1005    // 组呼之后收到调度机200 OK消息，通知话权授予的状况为拒绝。
#define PTT_IND_STATUS_SNATCH               1006    // 组呼建立成功。
#define PTT_IND_STATUS_HANGUP_OK            1007    // 调度台用户离开组呼
#define PTT_IND_STATUS_CLOSE_OK             1008    // 组呼结束（收到调度机BYE消息）。
#define PTT_IND_STATUS_CANCEL_OK            1009    // 组呼建立失败（收到调度机CANCEL消息）。
#define PTT_IND_STATUS_PREMPTED             1010    // 组呼被替换。
#define PTT_IND_STATUS_TX_BEGIN             1011    // 组呼之后收到调度机INFO消息，通知话权变更。
#define PTT_IND_STATUS_TX_IDLE              1012    // 组呼之后收到调度机INFO消息，通知话权空闲。
#define PTT_IND_STATUS_MEDIAPLANE_ERROR     1013    // 组呼创建失败
#define PTT_IND_STATUS_EMERGENCY_BEGIN      1014    // 收到一路紧急组呼
#define PTT_IND_STATUS_EMERGENCY_UPDATE     1015    // 组呼转变为紧急组呼
#define PTT_IND_STATUS_LICENSE_LIMIT        1016    // 证书受限
#define PTT_IND_STATUS_SIGNAL_ERROR         1017    // SIP错误

typedef struct
{
    INT64   i64GroupID;             // 组ID
    BOOL    bIsEmergencyCall;       // 是否紧急呼叫
    BOOL    bClosed;
    UINT32  ulCallType;
    UINT32  ulGroupCallStatus;      // 组呼状态
    UINT32  ulSpeaker;              // 主讲人
    UINT32  ulLocalPort;            // 本地端口
    UINT32  ulRemotePort;           // 远端端口
    CHAR    acRemoteIP[64];         // 远端IP
    INT32   iSoundType;             // 音频格式。
    CHAR    acSpeakerName[64];      // 主讲人姓名
    UINT32  ulSpeakTime;
}EVENT_GROUP_STATUS_T;

typedef struct
{
    INT64       i64GroupID;
    UINT32      ulPatchIndex;
    CHAR        acGroupName[64];
    INT32       iDCUserID;  ///< 创建派接组的调度台ID
}EVENT_CREATE_PATCH_GROUP_T;


#define SDS_TYPE_TEXT_NORMAL    "0001"
#define SDS_TYPE_TEXT_SMS       "0004"

typedef struct
{
    UINT32      ulSDSType;
    CHAR        acContent[1024];    // 消息内容
    CHAR        acFrom[32];         // 发件人
//  CHAR        acTo[32];           // 收件人
    UINT32      ulMsgState;         // 短信状态, 0：在线 1：离线。
    INT64       i64EmergGroupID;    // 紧急状态的群组ID。
    UINT32      ulEmergUserID;      // 紧急状态的终端ID。
    INT64       i64GroupID;         // 群组ID。
    CHAR        acFile[256];
    CHAR        acSubject[128];     // 消息主题
    CHAR        acDirection[32];    // 接发短信标志。
    CHAR        acSendDate[32];     // 消息发送日期。
    CHAR        acSendTime[32];     // 消息发送时间。
    //StatusCode 短信的状态码（普通短信不适用）
}EVENT_SDS_REPORT_T;

#define SDS_SEND_SUCC                   0       // 发送成功
#define SDS_SEND_NO_ACK                 0xff    // 接收端没有ACK
#define SDS_SEND_FAIL_SERVICE_UNSUPORT  1       // 业务不支持，如对方不支持加密短信。
#define SDS_SEND_FAIL_VPN_LIMIT         2       // VPN权限非法
#define SDS_SEND_FAIL_TARGET_NOT_FOUND  3       // 对方号码不存在

typedef struct
{
    CHAR        acReceiver[32];
    CHAR        acMsgID[32];
    BOOL        bSendFlag;          // true
    UINT32      ulRetCode;
}EVENT_SDS_SEND_STATUS_T;


// 群组
#define GROUP_TYPE_ALLBROADCAST     0   // 全网广播组
#define GROUP_TYPE_GENERAL          1   // 普通组
#define GROUP_TYPE_AREABROADCAST    2   // 区域广播组
#define GROUP_TYPE_EXTERNAL         8   // 外部组
#define GROUP_TYPE_DYNAMIC          9   // 动态组
#define GROUP_TYPE_ALLTYPE          10  // 所有组

typedef struct
{
    INT64       i64GroupID;         // 群组ID
    CHAR        acGroupName[64];    // 名称
    UINT32      ulPriority;         // 优先级，取值范围为 1~15，值越小优先级越高
    UINT32      ulGroupType;        // 类型
    UINT32      ulGroupStatus;      // 订阅/加入状态
}DC_GROUP_INFO_T;

#define ESDK_MAX_PATCH_GROUP_MEMBER_NUM     20
#define ESDK_MAX_MULTI_GROUP_MEMBER_NUM     30

typedef struct
{
    INT64       i64GroupID;         // 群组ID
    CHAR        acGroupName[64];    // 名称
    UINT32      ulPriority;         // 优先级，取值范围为 1~15，值越小优先级越高
    UINT32      ulPatchIndex;
    UINT32      ulDCUserID;

    UINT32      ulMemberNum;
    INT64       ai64MemberGroupID[ESDK_MAX_PATCH_GROUP_MEMBER_NUM];  // SDK限制最多20个成员
}SDK_PATCH_GROUP_INFO_T;

typedef struct
{
    INT64       i64GroupID;         // 群组ID
    CHAR        acGroupName[64];    // 名称
    UINT32      ulDCUserID;

    UINT32      ulMemberNum;
    INT64       ai64MemberGroupID[ESDK_MAX_MULTI_GROUP_MEMBER_NUM];  // SDK限制最多30个成员
//    VECTOR<INT64>   vGroup;
}SDK_MULTI_GROUP_INFO_T;


#define P2PVIDEO_IND_STATUS_RECEIVED                3002    // 上层应用作为被叫，收到对端点呼请求（终端视频点呼）
#define P2PVIDEO_IND_STATUS_ANSWERED                3003    // 上层应用作为主叫，对端已接听呼叫
#define P2PVIDEO_IND_STATUS_HANGUPED                3008    // 实时视频已被对端挂断
#define P2PVIDEO_IND_STATUS_HANGUPED_ACTIVE         3009    // 实时视频被调度台主动挂断。
#define P2PVIDEO_IND_STATUS_CANCELLED               3010    // 实时视频请求被取消
#define P2PVIDEO_IND_STATUS_MONITOR                 3011    // 上层应用作为被叫，收到对端视频回传请求。(终端视频回传)
#define P2PVIDEO_IND_STATUS_RELEASED_BUSY           3013    // 对端忙
#define P2PVIDEO_IND_STATUS_REMOTE_NOTFOUND         3016    // 未找到对端资源。
#define P2PVIDEO_IND_STATUS_REMOTE_NOTCONNECTED     3021    // 对端未注册
#define P2PVIDEO_IND_STATUS_RELEASED_GENERAL_ERROR  3052    // 发生普通错误
#define P2PVIDEO_IND_STATUS_PICKEDUP                3040    // 本端已摘机，调度台接收终端视频。

typedef struct
{
    UINT32  ulStatus;               // 状态
    UINT32  ulCallee;               // 被叫
    UINT32  ulCaller;               // 主叫
    INT32   iCameraType;            // 摄像头类型
    UINT32  ulVideoFormat;          // 视频格式
    BOOL    bIsP2PVideoDial;        // 是否视频点呼
}EVENT_P2PVIDEO_STATUS_T;

#define GIS_REPORT_TYPE_PERIODIC    // 0：周期上报。
#define GIS_REPORT_TYPE_EMERGCALL   // 1：特定事件，即紧急呼叫。
#define GIS_REPORT_TYPE_EMERGOVER   // 2: 紧急事件结束
#define GIS_REPORT_TYPE_FALLDOWN    // 3: 倒地告警触发
#define GIS_REPORT_TYPE_FALLDOWN    // 4: 倒地告警清除
#define GIS_REPORT_TYPE_HISTORY     // 5: 历史位置下发

#define GPS_STATUS_NORMAL       // 0：代表GPS正常开启。
#define GPS_STATUS_STARFAIL     // 1：GPS搜星失败。
#define GPS_STATUS_ERROR        // 2：代表其它异常。
#define GPS_STATUS_UNREGISTER   // 3 : GPS未注册
#define GPS_STATUS_NORIGHT      // 4 : GPS上报权限关闭

typedef struct
{
    UINT32  ulUserID;       // 上报方
    UINT32  ulTime;         // 最后一次上报时间。
    DOUBLE  dAltitude;      // 高度。
    DOUBLE  dLatitude;      // 纬度。
    DOUBLE  dLongtitude;    // 经度。
    DOUBLE  dDirection;     // 方向, 单位是顺时针方向的度数.
    UINT32  ulReportType;   // 事件编码：

    UINT32  ulCellID;           // 小区ID
    UINT32  ulPLMN;
    UINT32  ulDeviceType;       // 设备类型
    UINT32  ulLocatMode;        // 定位方式.
    UINT32  ulReportStatus;     // 接收状态

    DOUBLE  dSpeed;             // 速度, 单位 m/s
}EVENT_GIS_REPORT_T;


#define USER_DISPATCHER      0      // 调度台用户
#define USER_FIXEDCAMERA     1      // 固定摄像头
#define USER_PSTNUSER        2      // PSTN用户
#define USER_TETRAUSER       3      // TETRA用户
#define USER_PLMNUSER        4      // PLMN移动用户
#define USER_EXTERNALPTT     5      // 外部PTT用户
#define USER_SDKUSER         6      // SDK网关内部用户
#define USER_APPUSER         7      // 公网APP终端用户
#define USER_UELOGINUSER     8      // 终端登录用户
#define USER_PTTUSER         9      // PTT用户
#define USER_ALLTYPEUSER     50     // 所有用户，不分类型
#define USER_OTHERUSER       255    // 其它未分类用户


typedef struct
{
    UINT32      ulUserID;           // 用户ID
    UINT32      ulUserCategory;     // 类型
    UINT32      ulUserPriority;     // 优先级，取值范围为 1~15，值越小优先级越高
    CHAR        acUserName[64];     // 名称
}DC_INFO_T;

#define DC_MUTE_ALL         0   // 全部静音
#define DC_UNMUTE_ALL       1   // 单独去静音
#define DC_MUTE_PTT         2   // 全部组呼静音
#define DC_UNMUTE_PTT       3   // 全部组呼去静音（恢复声音）
#define DC_MUTE_ONE         4   // 单独静音
#define DC_UNMUTE_ONE       5   // 单独去静音

// 录音呼叫类型
#define REC_TYPE_AUDIO      0   // 录音
#define REC_TYPE_VIDEO      1   // 录像

#define REC_CALL_TYPE_P2P       0   // 点呼
#define REC_CALL_TYPE_PTT       1   // 组呼
#define REC_CALL_TYPE_VIDEO     2   // 视频回传

// 静音呼叫类型
#define DC_MUTE_CALL_P2P    0   // 点呼
#define DC_MUTE_CALL_PTT    1   // 组呼
#define DC_MUTE_CALL_VIDEO  2   // 视频回传

// 呼叫状态
#define CALL_STATUS_CLOSE       0
#define CALL_STATUS_DIAL        1
#define CALL_STATUS_DIAL_FAIL   2

#define CALL_STATUS_RECVED      11
#define CALL_STATUS_RECV_SUCC   12
#define CALL_STATUS_RECV_FAIL   13
#define CALL_STATUS_PLAY        14
#define CALL_STATUS_PLAY_FAIL   15
#define CALL_STATUS_REJECT      16
#define CALL_STATUS_BUSY        17
#define CALL_STATUS_HANGUP      18
#define CALL_STATUS_NOANSWER    19

#define CALL_STATUS_TIMEOUT     80

#define CALL_STATUS_UNKNOWN     0xff

// 模块类型
#define MODULE_TYPE_DC                  7   //  RTP_MODULE， 媒体通道模块

// 模块状态
#define MODULE_STATE_KICK_OFF           15  // 退出调度台通知，因为有相同账号登录
#define MODULE_STATE_PWD_CHANGE         17  // 用户密码修改
#define MODULE_STATE_DISCONNECT         18  // 服务器未连接
#define MODULE_STATE_RECONNECT          19  // 服务器已连接
#define MODULE_STATE_USER_DELETE        22  // 用户被删除

#define MAX_REAL_SDS_TEXT_LEN       1000    // 实时短信最大长度
#define MAX_MM_SDS_TEXT_LEN         10240   // 彩信文本最大长度

typedef struct
{
    UINT32      ulModuleType;
    UINT32      ulModuleStatus;
}EVENT_MODULE_STATUS_T;

#define VWALL_STATUS_INIT       1       // 初始化状态，表示可用
#define VWALL_STATUS_BUSY       4022    // 已经占用
#define VWALL_STATUS_IDLE       4023    // 空闲

typedef struct
{
    UINT32      ulVideoChanID;  // 视频墙ID列表的视频通道号。
    UINT32      ulState;        // 视频墙ID状态：
    CHAR        acAlias[64];    // 视频墙别名。
}ESDK_VWALL_INFO_T;

BOOL eSDK_IsLogined();
BOOL eSDK_IsMRSLogined();
VOID eSDK_Logined();
VOID eSDK_Free(CHAR *str);

VOID eSDK_StartLocalService();
VOID eSDK_StopLocalService();
UINT32 eSDK_GetLocalServiceStatus();

BOOL eSDK_Init(eSDK_EventCallBack fEventCallBack);
VOID eSDK_Cleanup();
AnsiString eSDK_GetVer();
VOID eSDK_TriggerStatusReport(BOOL bEnable);

BOOL eSDK_GetSpeakerList(VECTOR<AnsiString> &vList);
BOOL eSDK_GetMicList(VECTOR<AnsiString> &vList);
BOOL eSDK_IsCameraValid();

BOOL eSDK_Login(CHAR* szDCUserID, CHAR* szPWD, UINT8* pucServerIP, UINT16 usServerSIPPort, UINT8* pucLocalIP);
BOOL eSDK_LoginMRS(UINT8* pucServerIP);
BOOL eSDK_Logout(CHAR* szDCUserID);

BOOL eSDK_CheckMDC();
BOOL eSDK_GetCurrDcUserInfo(CURR_DC_INFO_T &stDcInfo);

BOOL eSDK_GetDcGroups(VECTOR<DC_GROUP_INFO_T> &vDcGroup);
BOOL eSDK_GetDcGroups(UINT32 ulGroupType, VECTOR<DC_GROUP_INFO_T> &vDcGroup);
UINT32 eSDK_GetGroupCreator(INT64 i64GroupID);
BOOL eSDK_GetDcUsers(VECTOR<DC_INFO_T> &vDcUser);
BOOL eSDK_GetGroupMember(INT64 i64GroupID, VECTOR<UINT32> &vUser, VECTOR<INT64> &vGroup);

BOOL eSDK_GetPatchGroups(VectorMap<INT64, SDK_PATCH_GROUP_INFO_T> &vPatchGroup);
BOOL eSDK_GetPatchGroupMembers(INT64 i64PatchGroupID, VECTOR<INT64> &vMember);
BOOL eSDK_GetPatchGroupInfo(INT64 i64PatchGroupID, SDK_PATCH_GROUP_INFO_T& stInfo);

BOOL eSDK_SubGroup(INT64 i64GroupID);
BOOL eSDK_UnSubGroup(INT64 i64GroupID);
BOOL eSDK_SubJoinGroup(INT64 i64GroupID);
BOOL eSDK_JoinGroup(INT64 i64GroupID);
//BOOL eSDK_PTTDial(INT64 i64GroupID);
//BOOL eSDK_PTTRelease(INT64 i64GroupID);
BOOL eSDK_PTTHangup(INT64 i64GroupID);
BOOL eSDK_PTTEmergency(INT64 i64GroupID);
BOOL eSDK_GroupBreakoff(INT64 i64GroupID);

BOOL eSDK_P2PReject(UINT32 ulNumber);
BOOL eSDK_P2PBreakoff(UINT32 ulNumber);
BOOL eSDK_P2PTransfer(UINT32 ulCallingNumber, UINT32 ulObjectUserNumber);

BOOL eSDK_P2PHalfDuplexDial(UINT32 ulUserID);
BOOL eSDK_P2PHalfDuplexRelease(UINT32 ulUserID);

BOOL eSDK_CreatePatchGroup(CHAR *szGroupName);
BOOL eSDK_CancelPatchGroup(INT64 i64GroupID);
BOOL eSDK_AddPatchGroupMember(INT64 i64GroupID, CHAR *szGroupName, VECTOR<INT64> &vGroup);
BOOL eSDK_DelPatchGroupMember(INT64 i64GroupID, CHAR *szGroupName, VECTOR<INT64> &vGroup);
BOOL eSDK_ClearPatchGroupMember(INT64 i64GroupID, CHAR *szGroupName);

BOOL eSDK_GetDynamicGroupMember(INT64 i64GroupID, VECTOR<UINT32> &vUser, VECTOR<INT64> &vGroup);
BOOL eSDK_GetDynamicGroupMember(INT64 i64GroupID, VECTOR<UINT32> &vUser);
BOOL eSDK_CreateDynamicGroup(INT64 i64GroupID, VECTOR<INT64> &vGroupID, VECTOR<UINT32> &vUserID, UINT32 ulPriority, UINT32 ulMaxPeriod, CHAR *szGroupName);
BOOL eSDK_CreateDynamicGroup(INT64 i64GroupID, VECTOR<UINT32> &vUserID, UINT32 ulPriority, UINT32 ulMaxPeriod, CHAR *szGroupName);
BOOL eSDK_CreateDynamicGroup(INT64 i64GroupID, UINT32 ulPriority, UINT32 ulMaxPeriod, CHAR *szGroupName);
BOOL eSDK_CancelDynamicGroup(INT64 i64GroupID);
BOOL eSDK_AddDynamicGroupUser(INT64 i64GroupID, VECTOR<UINT32> &vUserID);
BOOL eSDK_DelDynamicGroupUser(INT64 i64GroupID, VECTOR<UINT32> &vUserID);
BOOL eSDK_DelDynamicGroupUser(INT64 i64GroupID, UINT32 ulUserID);
BOOL eSDK_SetDynamicGroupUser(INT64 i64GroupID, VECTOR<UINT32> &vAddUserID, VECTOR<UINT32> &vDelUserID);

BOOL eSDK_GetMultiGroup(VECTOR<SDK_MULTI_GROUP_INFO_T> &vGroupInfo);
BOOL eSDK_CreateMultiGroup(INT64 i64GroupID, CHAR *szGroupName, VECTOR<INT64> &vGroupID);
BOOL eSDK_CancelMultiGroup(INT64 i64GroupID);
BOOL eSDK_AddMultiGroupMember(INT64 i64GroupID, VECTOR<INT64> &vGroupID);
BOOL eSDK_DelMultiGroupMember(INT64 i64GroupID, VECTOR<INT64> &vGroupID);

BOOL eSDK_SendMsg(AnsiString strMsgInfo, UINT32 ulSDSID, VECTOR<UINT32> &vUserID, VECTOR<INT64> &vGroupID);
BOOL eSDK_SendMsg(AnsiString strMsgInfo, AnsiString strFile, UINT32 ulSDSID, VECTOR<UINT32> &vUserID, VECTOR<INT64> &vGroupID);

BOOL eSDK_SendRealMsg(AnsiString strMsgInfo, UINT32 ulSDSID, VECTOR<UINT32> &vUserID);
BOOL eSDK_SendRealMsg(AnsiString strMsgInfo, UINT32 ulSDSID, VECTOR<INT64> &vGroupID);
BOOL eSDK_ReplyRealMsg(VECTOR<UINT32> &vUserID);
BOOL eSDK_ReplyRealMsg(VECTOR<INT64> &vGroupID);
BOOL eSDK_ReplyRealMsg(UINT32 ulUserID);
BOOL eSDK_ReplyRealMsg(INT64 i64GroupID);

BOOL eSDK_StartGroupDiscreetListen(INT64 i64GroupID);
BOOL eSDK_StartUserDiscreetListen(UINT32 ulUserID);
BOOL eSDK_StopGroupDiscreetListen(INT64 i64GroupID);
BOOL eSDK_StopUserDiscreetListen(UINT32 ulUserID);

BOOL eSDK_StartEnvListen(UINT32 ulUserID);
BOOL eSDK_StopEnvListen(UINT32 ulUserID);

BOOL eSDK_VolumeMute(INT64 i64ResourceID, UINT32 ulCallType);
BOOL eSDK_VolumeUnMute(INT64 i64ResourceID, UINT32 ulCallType);
BOOL eSDK_VolumeControl(INT64 i64ResourceID, UINT32 ulVolumn);

BOOL eSDK_P2PVideoReject(UINT32 ulNumber);
BOOL eSDK_SetVideoPlayWindow(UINT32 ulNumber, HANDLE hPlayWnd);
BOOL eSDK_StartVideoDispatch(UINT32 ulSrcUserID, VECTOR<UINT32> vDstUserID, BOOL bOriginalVideoFormat);
BOOL eSDK_StopVideoDispatch(UINT32 ulSrcUserID, UINT32 ulDstUserID);

BOOL eSDK_GetRecFileList(UINT32 ulRecType, INT32 iCaller, INT32 iCallee, INT64 i64ResourceID,
                         UINT32 ulStartTime, UINT32 ulEndTime, VECTOR<ESDK_REC_FILE_INFO_T> &vFileList);

BOOL eSDK_GetVWallInfo(VECTOR<ESDK_VWALL_INFO_T> &vInfo);
BOOL eSDK_VWallStart(UINT32 ulUserID, UINT32 ulVideoChanID);
BOOL eSDK_VWallStop(UINT32 ulUserID, UINT32 ulVideoChanID);

BOOL eSDK_GISSub(VECTOR<UINT32> &vUserID);
BOOL eSDK_GISSub(UINT32 ulUserID);
BOOL eSDK_GISUnSub(VECTOR<UINT32> &vUserID);
BOOL eSDK_GISUnSub(UINT32 ulUserID);

BOOL eSDK_StartRecord(INT64 i64GroupID, UINT32 ulRecType, UINT32 ulRecCallType);
BOOL eSDK_StopRecord(INT64 i64GroupID, UINT32 ulRecType, UINT32 ulRecCallType);

BOOL ParseEventUserStatus(CHAR *szEvent, EVENT_USER_STATUS_T *pstEvent);
BOOL ParseEventP2PCallStatus(CHAR *szEvent, EVENT_P2PCALL_STATUS_T *pstEvent);
BOOL ParseEventGroupStatus(CHAR *szEvent, EVENT_GROUP_STATUS_T *pstEvent);
BOOL ParseEventResourceStatus(CHAR *szEvent, EVENT_RESOURCE_STATUS_T *pstEvent);
BOOL ParseEventCreatePatchGroup(CHAR *szEvent, EVENT_CREATE_PATCH_GROUP_T *pstEvent);
BOOL ParseEventSDSReport(CHAR *szEvent, EVENT_SDS_REPORT_T *pstEvent);
BOOL ParseEventSDSSendStatus(CHAR *szEvent, EVENT_SDS_SEND_STATUS_T *pstEvent);
BOOL ParseEventP2PVideoCallStatust(CHAR *szEvent, EVENT_P2PVIDEO_STATUS_T *pstEvent);
BOOL ParseEventModuleStatus(CHAR *szEvent, EVENT_MODULE_STATUS_T *pstEvent);
BOOL ParseEventGISReport(CHAR *szEvent, EVENT_GIS_REPORT_T *pstEvent);

AnsiString GetGroupCallStatusText(UINT32 ulStatus);

VOID SetPatchGroupID(CHAR *szGroupName, INT64 i64GroupID);
INT64 GetPatchGroupID(CHAR *szGroupName);
BOOL GetLocalIP(UINT8 *pucServerIP, UINT8 *pucLocalIP);

#endif

