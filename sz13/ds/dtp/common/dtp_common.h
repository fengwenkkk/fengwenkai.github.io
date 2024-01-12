#ifndef DTP_COMMON_H
#define DTP_COMMON_H


#define DTPLog(ulLogLevel, ...)      { GosLog("DTP", ulLogLevel, ##__VA_ARGS__);}


/* 侦听线程状态 */
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

    UINT32                  ulDeadTime;     /* 消息超时时间 */

    UINT32                  ulMsgLen;       /* 消息总长度(含消息头及消息体) */
    UINT16                  usMsgType;      /* 消息类型 */
    UINT16                  usResv;

    UINT32                  ulMsgSn;

    DTP_MSG_HDR_T           *pstRspMsg;
    UINT32                  ulMaxRspMsgLen;
    UINT32                  *pulRspMsgLen;
}DTP_SYNC_MSG_NODE_T;

// 消息描述
typedef struct
{
    UINT16                  usMsgType;
    UINT16                  usResv;

    UINT32                  ulMsgLen;       /* 如果是固定消息，则为消息的长度（包括消息头） */

    BOOL                    bNeedAck;       /* 是否需要应答 */
}DTP_CLIENT_MSG_DESC_T;

typedef struct
{
    BOOL                    bSendFlag;                          /* 发送标识 */
    UINT32                  ulSendTime;                         /* 入队列时的uptime时间 */

    UINT32                  ulLastMsgHdrPos;                    /* 最后一个消息头位置 */
    UINT32                  ulLastMsgDataLen;                   /* 最后一个消息总长度 */
    UINT8                   ucLastMsgContentNum;                /* 最后一个消息MsgContent个数 */

    UINT32                  ulMsgLen;                           /* DTP_MSG_T消息总长度 */
    UINT32                  ulMsgNum;                           /* DTP_MSG_T消息个数 */
    UINT32                  ulMsgContentNum;                    /* MsgContent个数(对应DTP消息头中的MsgContent数之和) */

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
    UINT32                  ulLinkHelloFailNum;     /* 最大允许的hello应答失败次数 */

    HANDLE                  hLinkStatMutex;
    UINT32                  ulLinkHelloReqNum;      /* 没有收到应答的hello消息 */
    UINT32                  ulLastSendTime;         /* 最后一次消息发送的时间 */
    UINT32                  ulLastRecvTime;         /* 最后一次消息接收的时间 */

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
    SOCKET                  stSocket;                           /* TCP 特有 */
    UINT16                  usClientID;
    UINT8                   ucNeedClose;                        /* TCP 特有 */

    SOCKADDR_IN             stClientSendAddr;
    UINT32                  ulConnectSetupTime;                 /* 客户端连接建立时间 */

    UINT16                  usClientRecvPort;                   /* UDP 特有 */

    UINT8                   ucClientType;

    DTP_CLIENT_STATUS_E     enClientStatus;                     /* 客户端连接状态 */
    UINT32                  ulLastRecvTime;                     /* 接收到的最后一次客户端消息时间 */

    UINT32                  ulRecvOffset;                       /* TCP 特有，当前缓冲区实际起始位置 */
    UINT32                  ulRecvSize;                         /* TCP 特有，当前已经recv的字节数 */
    UINT8                   *pucRecvBuf;                        /* TCP 特有，接受缓冲区 */
}DTP_CONN_INFO_T;

typedef struct
{
    UINT16                  usMsgType;
    UINT16                  usResv;

    UINT32                  ulMsgLen;       /* 如果是固定消息，则为消息的长度（包括消息头） */

    UINT8                   ucPriority;
}DTP_SERVER_MSG_DESC_T;

typedef struct
{
    DTP_SERVER_STATUS_E     enStatus;
    BOOL                    bClose;

    UINT8                   ucCompressTypeNum;
    UINT8                   aucCompressType[8]; /* 支持的压缩协议 */

    UINT8                   aucAddr[4];         /* 服务器侦听地址 */
    UINT16                  usPort;             /* 服务器侦听端口 */
    UINT16                  usInstID;           /* 服务器标识 */

    SOCKET                  stSocket;           /* 服务器socket */

    UINT32                  ulReconnectTime;
    UINT32                  ulSendErrNum;
    UINT32                  ulRecvErrNum;       /* TCP only */
    UINT32                  ulMaxSendErrNum;
    UINT32                  ulMaxRecvErrNum;
    UINT32                  ulRecvTimeout;
    UINT32                  ulClientCheckInterval;  /* 保活监测间隔 */
    UINT32                  ulRecvCheckTime;        /* 超过此时间没有应答，则关闭此连接 */

    HANDLE                  hMsgQueueSem;
    HANDLE                  hMsgQueueMutex; /* 控制消息队列锁 */
    HANDLE                  hMsgQueue;      /* 控制消息队列 */

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
