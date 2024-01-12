#ifndef G_COMMON_H
#define G_COMMON_H

/* �궨�� */
#define GOS_DEFAULT_SERVER_PORT             63001       /* ������ȱʡ�����˿� */
#define GOS_DEFAULT_SELF_ADDR               "0.0.0.0"

#define MAX_FILE_NAME                       256

#define EV_RSP_START                        0x8000
#define EV_RSP(x)                           ((x)|0x8000)

#define EV_SYSTEM_START                     0x0000      /* ϵͳ��Ϣ */
#define EV_TIMER_START                      0x1000      /* ��ʱ����Ϣ */
#define EV_OMC_START                        0x3000      /* OMC��Ϣ */
#define EV_SERVICE_START                    0x4000      /* ҵ�����Ϣ */
#define EV_LOCAL_START                      0x7000      /* ҵ����ڲ���Ϣ */

#define EV_TASK_INIT_REQ                    (EV_SYSTEM_START+0)
#define EV_TASK_INIT_RSP                    EV_RSP(EV_TASK_INIT_REQ)

#define EV_TASK_CHECK_REQ                   (EV_SYSTEM_START+10)
#define EV_TASK_CHECK_RSP                   EV_RSP(EV_TASK_CHECK_REQ)

#define EV_CLIENT_CONNECT_SERVER_IND        (EV_SYSTEM_START+20)    /* �ͻ������ӷ����� */
#define EV_CLIENT_DISCONNECT_SERVER_IND     (EV_SYSTEM_START+21)    /* �ͻ��˶Ͽ������� */

#define EV_SERVER_ACCEPT_CLIENT_IND         (EV_SYSTEM_START+22)    /* ���������տͻ������� */
#define EV_SERVER_DISCONNECT_CLIENT_IND     (EV_SYSTEM_START+23)    /* �������Ͽ��ͻ��� */

//SYSLOG
#define EV_SYSLOG_START                     (EV_SYSTEM_START+100)
#define EV_SYSLOG_INIT_LOG_IND              (EV_SYSLOG_START+1)     /* GSysLog��ʼ�� */
#define EV_SYSLOG_GET_LOG_REQ               (EV_SYSLOG_START+2)     /* ��ȡlog��Ϣ */
#define EV_SYSLOG_GET_LOG_CFG_REQ           (EV_SYSLOG_START+3)     /* ��ȡlog��ӡ���� */
#define EV_SYSLOG_SET_LOG_CFG_REQ           (EV_SYSLOG_START+4)     /* ����log��ӡ���� */

#define EV_SYSLOG_GET_LOG_RSP               EV_RSP(EV_SYSLOG_GET_LOG_REQ)
#define EV_SYSLOG_GET_LOG_CFG_RSP           EV_RSP(EV_SYSLOG_GET_LOG_CFG_REQ)
#define EV_SYSLOG_SET_LOG_CFG_RSP           EV_RSP(EV_SYSLOG_SET_LOG_CFG_REQ)

/* Task״̬ */
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

/* �ṹ���� */
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
    UINT16              usPort;             /* �˿� */
    UINT8               ucTCPMode;
}PEER_ADDR_T;

typedef struct
{
    UINT16              usInstID;           /* ����ƽ̨��ʶ */

    BOOL                bMultiInst;         /* �Ƿ������ʵ�����У�ȱʡ�� */

    UINT32              ulMaxMsgNodeNum;
    UINT32              ulMaxUniqueMsgNum;  /* ��Ϣ������󻺴���Ϣ����*/
    UINT32              ulMsgMaxWaitTime;   /* socket�첽��Ϣ���ȴ�ʱ�� */

    BOOL                bExitWhenMsgQueueIsFull;    // ��Ϣ���������Ժ��Զ��˳�
    BOOL                bLogMsgStat;                    // �Ƿ�������Ϣ��ӡ
    BOOL                bServerMode;
    IP_ADDR_T           stSelfAddr;         /* ����������ַ */

    UINT32              ulPeerNum;
    PEER_ADDR_T         astPeerAddr[256];   /* �Զ˵�ַ */

    LOG_CFG_T           stLogCfg;
    UINT32              ulMaxSysLogNum;     /* �����־������ */
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
