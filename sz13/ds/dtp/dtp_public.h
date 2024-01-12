#ifndef _DTP_PUBLIC_H
#define _DTP_PUBLIC_H

#define DTP_MAX_CONN_NUM                        1024

#define DTP_DEFAULT_SERVER_PORT                 ((UINT16)27000)

#define DTP_MSG_PRI_HIGH                        1
#define DTP_MSG_PRI_LOW                         2

/* 压缩算法 */
#define DTP_COMPRESS_TYPE_NONE                  0x00    /* 不压缩 */
#define DTP_COMPRESS_TYPE_QUICKLZ               0x80    /* QuickLZ压缩 */

// 加密算法
#define DTP_ENCRYPT_TYPE_NONE                   0x00    /* 不加密 */
#define DTP_ENCRYPT_TYPE_GOS                    0x01    /* GOS内置线性加密/解密算法 */

/* 协议标准事件定义 */
/********************************************** SDTP自定义消息 *******************************/
#define DTP_EV_LINK_AUTH_REQ                    (UINT16)0x0001  /* 鉴权请求 */
#define DTP_EV_LINK_AUTH_RSP                    (UINT16)0x8001  /* 鉴权应答 */

#define DTP_EV_LINK_RELEASE_REQ                 (UINT16)0x0002  /* 连接释放请求 */
#define DTP_EV_LINK_RELEASE_RSP                 (UINT16)0x8002  /* 连接释放应答 */

#define DTP_EV_LINK_CHECK_REQ                   (UINT16)0x0003  /* 链路检测请求 */
#define DTP_EV_LINK_CHECK_RSP                   (UINT16)0x8003  /* 链路检测应答 */

#define DTP_MAX_MSG_LEN                         (62*1024)

/* 客户端状态 */
typedef enum
{
    DTP_CLIENT_IDLE,            /* 未连接 */
    DTP_CLIENT_CONNECTED,       /* 连接正常 */
    DTP_CLIENT_LINKAUTHED,      /* 连接鉴权通过 */
    DTP_CLIENT_SUSPEND,
}DTP_CLIENT_STATUS_E;

#define DTP_CLIENT_LINKSETUP        DTP_CLIENT_LINKAUTHED
#define DTP_CLIENT_DISCONNECTED     DTP_CLIENT_IDLE    /* 连接关闭 */

#define DTP_INVALID_INST_ID         0xffff

#define DTP_MSG_MAGIC               0x47424D50          // magic字段，对应字符串"GBMP", GB Msg Protocol
#define DTP_MSG_VER                 0x0100              // 版本号

#pragma pack(push)
#pragma pack(1)

typedef struct
{
    UINT32      ulMsgID;
    UINT64      u64MsgCount;
    UINT64      u64MsgSize;
}DTP_MSG_STAT_T;

/* 客户端配置 */
typedef struct
{
    UINT8       aucServerAddr[4];                       /* 服务器IP */
    UINT16      usServerPort;                           /* 服务器端口 */
    UINT16      usServerID;                             /* 服务器标识 */

    UINT32      ulLinkHelloInterval;                    /* 链路检查时间间隔 */
    UINT32      ulLinkHelloFailNum;                     /* 最大链路检查失败次数 */
    UINT32      ulMaxSocketErrNum;                      /* 最大socket异常次数（超过该次数则拆链）*/
    BOOL        bAllowSyncMsg;                          /* 是否支持同步消息 */
    UINT8       ucMsgPri;
}DTP_CLIENT_CFG_T;

/************************** 消息头 *********************************************/
#define DTP_MAX_PKG_MSG_NUM             256     /* 消息体中的事件数量 */

/************************** ver nego *********************************************/
#define DTP_VER_NEGO_OK                 0       /* 版本协商通过 */
#define DTP_VER_NEGO_VERTOOHIGH         1       /* 版本过高 */
#define DTP_VER_NEGO_VERTOOLOW          2       /* 版本过低 */

/************************** link auth *********************************************/
#define DTP_LINK_AUTH_OK                0       /* 鉴权通过 */
#define DTP_LINK_AUTH_LOGINED           1       /* 已经登录(同AUTH_OK) */
#define DTP_LINK_AUTH_INVALID_LOGINID   2       /* Login ID不存在 */
#define DTP_LINK_AUTH_CIPHER_FAILED     3       /* 加密结果出错 */

/************************** link release *********************************************/
#define DTP_LINK_RELEASE_NORMAL         1       /* 用户正常释放 */
#define DTP_LINK_RELEASE_DATAERROR      2       /* 数据类型错误 */
#define DTP_LINK_RELEASE_OVERLOAD       3       /* 超出机器处理能力 */

#define DTP_LINK_RELEASE_OK             0       /* 释放成功 */
#define DTP_LINK_RELEASE_FAIL           1       /* 释放失败 */

/************************** compress nego *********************************************/
#ifdef DTP_VER_20
#define DTP_COMPRESS_NEGO_OK            0       /* 成功 */
#define DTP_COMPRESS_NEGO_FAIL          1       /* 失败 */
#define DTP_COMPRESS_NEGO_NULL          2       /* 不压缩 */
#endif

#define MSG_PRI_IMP                     1
#define MSG_PRI_NOR                     0

/************************** 消息头 ***************************************************/

typedef struct
{
    UINT32                  ulMsgMagic;
    UINT16                  usMsgVer;
    UINT16                  usMsgType;      /* 消息类型 */

    UINT32                  ulMsgLen;       /* 消息总长度(含消息头及消息体) */
    UINT32                  ulSeqID;        /* 交互的流水号，顺序累加，步长为1，循环使用（一个交互的一对请求和应答消息的流水号必须相同） */

    UINT16                  usSenderModule;
    UINT16                  usReceiverModule;

    UINT8                   ucCompType;     /* 数据压缩算法 */
    UINT8                   ucEncryptType;  /* 数据加密算法 */
    UINT16                  usTimeout;      /* 消息有效时间，单位秒 */

    UINT16                  usCRC;          /* 校验码，对消息体数据每2个字节转换为16位整数相加，如有最后一个字节转化为8位整数相加 */
    UINT8                   ucMsgPri;
    UINT8                   ucSenderType;   /* 发送者类型 */

    UINT8                   aucResv[4];     /* 全填0 */
}DTP_MSG_HDR_T;

typedef struct
{
    DTP_MSG_HDR_T           stMsgHdr;
    UINT8                   aucMsgBody[DTP_MAX_MSG_LEN];
}DTP_MSG_T;

/************************** link auth *********************************************/
typedef struct
{
    DTP_MSG_HDR_T           stMsgHdr;
}DTP_TCP_LINK_AUTH_REQ_T;

typedef struct
{
    DTP_MSG_HDR_T           stMsgHdr;

    UINT8                   aucClientAddr[4];       // 客户端侦听地址
    UINT16                  usClientPort;           // 客户端侦听端口
}DTP_UDP_LINK_AUTH_REQ_T;

typedef struct
{
    DTP_MSG_HDR_T           stMsgHdr;

    UINT8                   ucResult;
    UINT8                   ucResv;

    UINT8                   aucClientAddr[4];       // 客户端发送地址
    UINT16                  usClientPort;           // 客户端发送端口

    UINT16                  usServerID;
    UINT16                  usClientID;

}DTP_LINK_AUTH_RSP_T;

/************************** link check *********************************************/
typedef struct
{
    DTP_MSG_HDR_T           stMsgHdr;
}DTP_LINK_CHECK_REQ_T;

typedef struct
{
    DTP_MSG_HDR_T           stMsgHdr;
}DTP_LINK_CHECK_RSP_T;

/************************** link release *********************************************/
typedef struct
{
    DTP_MSG_HDR_T           stMsgHdr;
}DTP_LINK_RELEASE_REQ_T;

typedef struct
{
    DTP_MSG_HDR_T           stMsgHdr;
    UINT8                   ucResult;
}DTP_LINK_RELEASE_RSP_T;

/************************** compress nego *********************************************/
typedef struct
{
    DTP_MSG_HDR_T           stMsgHdr;
    UINT8                   ucCompressFlag;
}DTP_COMPRESS_NEGO_REQ_T;

typedef struct
{
    DTP_MSG_HDR_T           stMsgHdr;
    UINT8                   ucResult;
}DTP_COMPRESS_NEGO_RSP_T;

/************************** 多消息同时发送消息体 *********************************************/
typedef struct
{
    VOID                    *pvMsg;
    UINT32                  ulMsgLen;
}DTP_MSG_INFO_T;

typedef struct
{
    UINT32                  ulMsgNum;
    DTP_MSG_INFO_T          astMsgInfo[1024];
}DTP_MSG_LIST_T;

/************************** 其它结构 *********************************************/
// 消息队列池
typedef struct
{
    UINT32                  ulBlockSize;    /* 内存块大小 */
    UINT32                  ulBlockNum;     /* 内存块个数 */
}DTP_MSG_POOL_T;

#pragma pack(pop)

typedef VOID (*DTP_CLIENT_MSG_HANDLER)(HANDLE hClient, DTP_MSG_T * pstMsg);
typedef VOID (*DTP_SERVER_MSG_HANDLER)(HANDLE hServer, UINT16 usClientID, DTP_MSG_T * pstMsg);

/************************** 函数 *********************************************/


#endif

