#ifndef DTP_COMMON_H
#define DTP_COMMON_H


#define DTPLog(ulLogLevel, ...)      { GosLog("DTP", ulLogLevel, ##__VA_ARGS__);}


/* �����߳�״̬ */
typedef enum
{
    THREAD_STATE_NULL,
    THREAD_STATE_WORK,
    THREAD_STATE_STOPPING,
    THREAD_STATE_STOP
}DTP_THREAD_STATUS_E;

#pragma pack(push, 1)

typedef struct
{
    BOOL                    bUsed;
    HANDLE                  hSem;

    UINT32                  ulDeadTime;     /* ��Ϣ��ʱʱ�� */

    UINT32                  ulMsgLen;       /* ��Ϣ�ܳ���(����Ϣͷ����Ϣ��) */
    UINT16                  usMsgType;      /* ��Ϣ���� */
    UINT16                  usResv;

    UINT32                  ulMsgSn;

    DTP_MSG_HDR_T           *pstRspMsg;
    UINT32                  ulMaxRspMsgLen;
    UINT32                  *pulRspMsgLen;
}DTP_SYNC_MSG_NODE_T;

// ��Ϣ����
typedef struct
{
    UINT16                  usMsgType;
    UINT16                  usResv;

    UINT32                  ulMsgLen;       /* ����ǹ̶���Ϣ����Ϊ��Ϣ�ĳ��ȣ�������Ϣͷ�� */

    BOOL                    bNeedAck;       /* �Ƿ���ҪӦ�� */
}DTP_CLIENT_MSG_DESC_T;

typedef struct
{
    BOOL                    bSendFlag;                          /* ���ͱ�ʶ */
    UINT32                  ulSendTime;                         /* �����ʱ��uptimeʱ�� */

    UINT32                  ulLastMsgHdrPos;                    /* ���һ����Ϣͷλ�� */
    UINT32                  ulLastMsgDataLen;                   /* ���һ����Ϣ�ܳ��� */
    UINT8                   ucLastMsgContentNum;                /* ���һ����ϢMsgContent���� */

    UINT32                  ulMsgLen;                           /* DTP_MSG_T��Ϣ�ܳ��� */
    UINT32                  ulMsgNum;                           /* DTP_MSG_T��Ϣ���� */
    UINT32                  ulMsgContentNum;                    /* MsgContent����(��ӦDTP��Ϣͷ�е�MsgContent��֮��) */

    UINT8                   *pucMsg;
}DTP_CLIENT_MSG_NODE_T;

typedef struct
{
    UINT8                   aucClientAddr[4];
    UINT16                  usClientPort;
    UINT16                  usClientID;

    UINT8                   aucServerAddr[4];
    UINT16                  usServerPort;
    UINT16                  usServerID;

    UINT32                  ulMsgDescNum;
    DTP_CLIENT_MSG_DESC_T   astMsgDesc[32];

    UINT32                  ulLinkHelloInterval;
    UINT32                  ulLinkHelloFailNum;     /* ��������helloӦ��ʧ�ܴ��� */

    HANDLE                  hLinkStatMutex;
    UINT32                  ulLinkHelloReqNum;      /* û���յ�Ӧ���hello��Ϣ */
    UINT32                  ulLastSendTime;         /* ���һ����Ϣ���͵�ʱ�� */
    UINT32                  ulLastRecvTime;         /* ���һ����Ϣ���յ�ʱ�� */

    UINT32                  ulSocketErrNum;
    UINT32                  ulMaxSocketErrNum;

    HANDLE                  hMsgQueueSem;
    HANDLE                  hMsgQueueMutex;
    HANDLE                  hMsgQueue;

    HANDLE                  hSyncMsgMutex;
    DTP_SYNC_MSG_NODE_T     astSyncMsg[16];

    UINT32                  ulMaxMsgBufSize;

    DTP_MSG_T               stRecvMsgBuf;
    INT32                   iRecvSize;

    HANDLE                  hStatusMutex;
    DTP_CLIENT_STATUS_E     enClientStatus;
    DTP_THREAD_STATUS_E     enRunStatus;
    BOOL                    bSendThreadRunning;

    SOCKET                  stSocket;

    HANDLE                  hSeqIDMutex;
    UINT32                  ulSeqID;

    UINT8                   ucMsgPri;
    UINT8                   ucSenderType;

    DTP_CLIENT_MSG_NODE_T   stMsgNode;
}DTP_CLIENT_INFO_T;

typedef enum
{
    DTP_SERVER_IDLE,
    DTP_SERVER_INIT,
    DTP_SERVER_WORK,
    DTP_SERVER_CLOSE,
}DTP_SERVER_STATUS_E;

#define DTP_MAX_MSGBUF_LEN          (DTP_MAX_MSG_LEN+sizeof(DTP_MSG_HDR_T)+64*1024)

typedef struct
{
    SOCKET                  stSocket;                           /* TCP ���� */
    UINT16                  usClientID;
    UINT8                   ucNeedClose;                        /* TCP ���� */

    SOCKADDR_IN             stClientSendAddr;
    UINT32                  ulConnectSetupTime;                 /* �ͻ������ӽ���ʱ�� */

    UINT16                  usClientRecvPort;                   /* UDP ���� */

    UINT8                   ucClientType;

    DTP_CLIENT_STATUS_E     enClientStatus;                     /* �ͻ�������״̬ */
    UINT32                  ulLastRecvTime;                     /* ���յ������һ�οͻ�����Ϣʱ�� */

    UINT32                  ulRecvOffset;                       /* TCP ���У���ǰ������ʵ����ʼλ�� */
    UINT32                  ulRecvSize;                         /* TCP ���У���ǰ�Ѿ�recv���ֽ��� */
    UINT8                   *pucRecvBuf;                        /* TCP ���У����ܻ����� */
}DTP_CONN_INFO_T;

typedef struct
{
    UINT16                  usMsgType;
    UINT16                  usResv;

    UINT32                  ulMsgLen;       /* ����ǹ̶���Ϣ����Ϊ��Ϣ�ĳ��ȣ�������Ϣͷ�� */

    UINT8                   ucPriority;
}DTP_SERVER_MSG_DESC_T;

typedef struct
{
    DTP_SERVER_STATUS_E     enStatus;
    BOOL                    bClose;

    UINT8                   ucCompressTypeNum;
    UINT8                   aucCompressType[8]; /* ֧�ֵ�ѹ��Э�� */

    UINT8                   aucAddr[4];         /* ������������ַ */
    UINT16                  usPort;             /* �����������˿� */
    UINT16                  usInstID;           /* ��������ʶ */

    SOCKET                  stSocket;           /* ������socket */

    UINT32                  ulReconnectTime;
    UINT32                  ulSendErrNum;
    UINT32                  ulRecvErrNum;       /* TCP only */
    UINT32                  ulMaxSendErrNum;
    UINT32                  ulMaxRecvErrNum;
    UINT32                  ulRecvTimeout;
    UINT32                  ulClientCheckInterval;  /* �������� */
    UINT32                  ulRecvCheckTime;        /* ������ʱ��û��Ӧ����رմ����� */

    HANDLE                  hMsgQueueSem;
    HANDLE                  hMsgQueueMutex; /* ������Ϣ������ */
    HANDLE                  hMsgQueue;      /* ������Ϣ���� */

    HANDLE                  hSyncMsgMutex;
    DTP_SYNC_MSG_NODE_T     astSyncMsg[16];

    DTP_MSG_T               stSendMsgBuf;

    HANDLE                  hConnInfo;
    DTP_CONN_INFO_T         astConnInfo[DTP_MAX_CONN_NUM];

    FDSET                   stFdSet;

    HANDLE                  hSeqIDMutex;
    UINT32                  ulSeqID;

    DTP_MSG_T               stRecvMsgBuf;    /* UDP only */
}DTP_SERVER_INFO_T;

typedef struct
{
    SOCKET                  stSocket;
    UINT16                  usClientID;

    DTP_MSG_T               *pstMsg;
}DTP_SERVER_MSG_NODE_T;

#pragma pack(pop)

#endif
