#ifndef G_COMMON_H
#define G_COMMON_H

/* 宏定义 */
#define GOS_DEFAULT_SERVER_PORT             63001       /* 服务器缺省侦听端口 */
#define GOS_DEFAULT_SELF_ADDR               "0.0.0.0"

#define MAX_FILE_NAME                       256

#define EV_RSP_START                        0x8000
#define EV_RSP(x)                           ((x)|0x8000)

#define EV_SYSTEM_START                     0x0000      /* 系统消息 */
#define EV_TIMER_START                      0x1000      /* 定时器消息 */
#define EV_OMC_START                        0x3000      /* OMC消息 */
#define EV_SERVICE_START                    0x4000      /* 业务层消息 */
#define EV_LOCAL_START                      0x7000      /* 业务层内部消息 */

#define EV_TASK_INIT_REQ                    (EV_SYSTEM_START+0)
#define EV_TASK_INIT_RSP                    EV_RSP(EV_TASK_INIT_REQ)

#define EV_TASK_CHECK_REQ                   (EV_SYSTEM_START+10)
#define EV_TASK_CHECK_RSP                   EV_RSP(EV_TASK_CHECK_REQ)

#define EV_CLIENT_CONNECT_SERVER_IND        (EV_SYSTEM_START+20)    /* 客户端连接服务器 */
#define EV_CLIENT_DISCONNECT_SERVER_IND     (EV_SYSTEM_START+21)    /* 客户端断开服务器 */

#define EV_SERVER_ACCEPT_CLIENT_IND         (EV_SYSTEM_START+22)    /* 服务器接收客户端连接 */
#define EV_SERVER_DISCONNECT_CLIENT_IND     (EV_SYSTEM_START+23)    /* 服务器断开客户端 */

//SYSLOG
#define EV_SYSLOG_START                     (EV_SYSTEM_START+100)
#define EV_SYSLOG_INIT_LOG_IND              (EV_SYSLOG_START+1)     /* GSysLog初始化 */
#define EV_SYSLOG_GET_LOG_REQ               (EV_SYSLOG_START+2)     /* 获取log信息 */
#define EV_SYSLOG_GET_LOG_CFG_REQ           (EV_SYSLOG_START+3)     /* 获取log打印配置 */
#define EV_SYSLOG_SET_LOG_CFG_REQ           (EV_SYSLOG_START+4)     /* 设置log打印配置 */

#define EV_SYSLOG_GET_LOG_RSP               EV_RSP(EV_SYSLOG_GET_LOG_REQ)
#define EV_SYSLOG_GET_LOG_CFG_RSP           EV_RSP(EV_SYSLOG_GET_LOG_CFG_REQ)
#define EV_SYSLOG_SET_LOG_CFG_RSP           EV_RSP(EV_SYSLOG_SET_LOG_CFG_REQ)

/* Task状态 */
#define TASK_STATUS_IDLE            0
#define TASK_STATUS_INIT            1
#define TASK_STATUS_WORK            2
#define TASK_STATUS_STANDBY         3
#define TASK_STATUS_REFRESHED       0xfffe

#define INVALID_TASK_STATUS         0xffff

#define DEFAULT_LOG_LEVEL           LOG_INFO   // LOG_INFO
#define DEFAULT_LOG_TO_STDOUT       0
#define DEFAULT_LOG_TO_FILE         1
#define DEFAULT_LOG_FILE_NUM        2
#define DEFAULT_LOG_FILE_SIZE       4096
#define DEFAULT_LOG_BUFF_TIME       0
#define DEFAULT_LOCAL_BUFF_SIZE     0   //128000
#define DEFAULT_REMOTE_BUFF_SIZE    0   //128000

typedef enum
{
    TIMER0=0,
    TIMER1,
    TIMER2,
    TIMER3,
    TIMER4,
    TIMER5,
    TIMER6,
    TIMER7,
    TIMER8,
    TIMER9,
    TIMER10,
    TIMER11,
    TIMER12,
    TIMER13,
    TIMER14,
    TIMER15,
    TIMER16,
    TIMER17,
    TIMER18,
    TIMER19,

    TIMER_NUM
}TIMER_E;

#define GET_TIMER_EV(TIMER)     (EV_TIMER_START+TIMER)

typedef enum
{
    EV_TIMER0=GET_TIMER_EV(TIMER0),
    EV_TIMER1,
    EV_TIMER2,
    EV_TIMER3,
    EV_TIMER4,
    EV_TIMER5,
    EV_TIMER6,
    EV_TIMER7,
    EV_TIMER8,
    EV_TIMER9,
    EV_TIMER10,
    EV_TIMER11,
    EV_TIMER12,
    EV_TIMER13,
    EV_TIMER14,
    EV_TIMER15,
    EV_TIMER16,
    EV_TIMER17,
    EV_TIMER18,
    EV_TIMER19,
}EV_TIMER_E;

#define INVALID_PROC_ID     0xffff
#define INVALID_TASK_ID     0xffff

#define LOCAL_INST_ID       0x0000
#define INVALID_INST_ID     0xffff

/* 结构定义 */
#pragma pack(push, 4)
typedef struct
{
    UINT16              usInstID;
    UINT16              usTaskID;
}PID_T;

typedef union
{
    VOID                *pvPara;
    UINT32              ulPara;
}TIMER_PARA_U;

typedef struct
{
    UINT32              ulParaType;
    TIMER_PARA_U        unPara;
}TIMER_MSG_T;

typedef struct
{
    PID_T               stSender;
    PID_T               stReceiver;

    UINT16              usMsgID;
    UINT8               ucMsgPri;
    UINT8               ucResv1;

    UINT32              ulSeqID;
    UINT32              ulTimeout;

    UINT32              ulMsgLen;
}MSG_HDR_T;

typedef struct MSG_T
{
    PID_T               stSender;
    PID_T               stReceiver;

    UINT16              usMsgID;
    UINT8               ucMsgPri;
    UINT8               ucSenderType;

    UINT32              ulSeqID;
    UINT32              ulTimeout;

    UINT32              ulMsgLen;
    UINT8               aucMsgData[4];
}MSG_T, *PMSG_T;

typedef BOOL (GOS_UNIQUE_MSG_CMP_FUNC)(VOID *pvSrc, VOID *pvDst);

#define MSG_HDR_LEN     STRUCT_OFFSET(MSG_T, aucMsgData)

typedef struct
{
    MSG_HDR_T           stMsgHdr;
    UINT16              usServerID;
}CLIENT_CONNECT_SERVER_T, CLIENT_DISCONNECT_SERVER_T;

typedef struct
{
    MSG_HDR_T           stMsgHdr;
    UINT16              usClientID;
}SERVER_ACCEPT_CLIENT_T, SERVER_DISCONNECT_CLIENT_T;

typedef struct
{
    UINT8               aucIP[4];           /* IP  */
    UINT16              usPort;             /* 端口 */
    UINT8               ucTCPMode;
}PEER_ADDR_T;

typedef struct
{
    UINT16              usInstID;           /* 本地平台标识 */

    BOOL                bMultiInst;         /* 是否允许多实例运行，缺省否 */

    UINT32              ulMaxMsgNodeNum;
    UINT32              ulMaxUniqueMsgNum;  /* 消息查重最大缓存消息个数*/
    UINT32              ulMsgMaxWaitTime;   /* socket异步消息最大等待时间 */

    BOOL                bExitWhenMsgQueueIsFull;    // 消息队列满了以后，自动退出
    BOOL                bLogMsgStat;                    // 是否启动消息打印
    BOOL                bServerMode;
    IP_ADDR_T           stSelfAddr;         /* 本地侦听地址 */

    UINT32              ulPeerNum;
    PEER_ADDR_T         astPeerAddr[256];   /* 对端地址 */

    LOG_CFG_T           stLogCfg;
    UINT32              ulMaxSysLogNum;     /* 最大日志缓存数 */
}PLAT_CFG_T;

#define MAX_LOG_MODULE_LEN      15

typedef struct
{
    UINT32                  ulTime;
    UINT32                  ulLogLevel;
    CHAR                    acLogModule[MAX_LOG_MODULE_LEN+1];
    CHAR                    acLog[1024];
}SYSLOG_INFO_T;

typedef struct
{
    UINT32                  ulStartTime;
    UINT32                  ulEndTime;
    UINT32                  ulLogLevel;
    CHAR                    acLogModule[MAX_LOG_MODULE_LEN+1];
}GET_SYSLOG_REQ_T;

typedef struct
{
    UINT32                  ulLogNum;
    UINT32                  ulLogDataLen;
    UINT8                   aucLogData[64*1024];
}GET_SYSLOG_RSP_T;

typedef struct
{
    CHAR                    acLogModule[MAX_LOG_MODULE_LEN+1];
    UINT32                  ulLogLevel;
}SYSLOG_CFG_T;

typedef struct
{
    UINT32                  ulLogModuleNum;
    SYSLOG_CFG_T            astLogCfg[256];
}GET_SYSLOG_CFG_RSP_T;

typedef struct
{
    UINT32                  ulLogModuleNum;
    SYSLOG_CFG_T            astLogCfg[256];
}SET_SYSLOG_CFG_REQ_T;

typedef struct
{
    BOOL                    bRet;
}SET_SYSLOG_CFG_RSP_T;

typedef struct
{
    CHAR                    acTaskName[32];

    UINT16                  usTaskID;
    UINT16                  usMsgID;

    PID_T                   stSenderPID;
    UINT32                  ulMsgStartTime;         // uptime
    UINT32                  ulMsgEndTime;           // uptime
}GOS_TASK_STATUS_T;

#pragma pack(pop)

#endif
