#ifndef _DTP_PUBLIC_H
#define _DTP_PUBLIC_H

#define DTP_MAX_CONN_NUM                        1024

#define DTP_DEFAULT_SERVER_PORT                 ((UINT16)27000)

#define DTP_MSG_PRI_HIGH                        1
#define DTP_MSG_PRI_LOW                         2

/* ѹ���㷨 */
#define DTP_COMPRESS_TYPE_NONE                  0x00    /* ��ѹ�� */
#define DTP_COMPRESS_TYPE_QUICKLZ               0x80    /* QuickLZѹ�� */

// �����㷨
#define DTP_ENCRYPT_TYPE_NONE                   0x00    /* ������ */
#define DTP_ENCRYPT_TYPE_GOS                    0x01    /* GOS�������Լ���/�����㷨 */

/* Э���׼�¼����� */
/********************************************** SDTP�Զ�����Ϣ *******************************/
#define DTP_EV_LINK_AUTH_REQ                    (UINT16)0x0001  /* ��Ȩ���� */
#define DTP_EV_LINK_AUTH_RSP                    (UINT16)0x8001  /* ��ȨӦ�� */

#define DTP_EV_LINK_RELEASE_REQ                 (UINT16)0x0002  /* �����ͷ����� */
#define DTP_EV_LINK_RELEASE_RSP                 (UINT16)0x8002  /* �����ͷ�Ӧ�� */

#define DTP_EV_LINK_CHECK_REQ                   (UINT16)0x0003  /* ��·������� */
#define DTP_EV_LINK_CHECK_RSP                   (UINT16)0x8003  /* ��·���Ӧ�� */

#define DTP_MAX_MSG_LEN                         (62*1024)

/* �ͻ���״̬ */
typedef enum
{
    DTP_CLIENT_IDLE,            /* δ���� */
    DTP_CLIENT_CONNECTED,       /* �������� */
    DTP_CLIENT_LINKAUTHED,      /* ���Ӽ�Ȩͨ�� */
    DTP_CLIENT_SUSPEND,
}DTP_CLIENT_STATUS_E;

#define DTP_CLIENT_LINKSETUP        DTP_CLIENT_LINKAUTHED
#define DTP_CLIENT_DISCONNECTED     DTP_CLIENT_IDLE    /* ���ӹر� */

#define DTP_INVALID_INST_ID         0xffff

#define DTP_MSG_MAGIC               0x47424D50          // magic�ֶΣ���Ӧ�ַ���"GBMP", GB Msg Protocol
#define DTP_MSG_VER                 0x0100              // �汾��

#pragma pack(push)
#pragma pack(1)

typedef struct
{
    UINT32      ulMsgID;
    UINT64      u64MsgCount;
    UINT64      u64MsgSize;
}DTP_MSG_STAT_T;

/* �ͻ������� */
typedef struct
{
    UINT8       aucServerAddr[4];                       /* ������IP */
    UINT16      usServerPort;                           /* �������˿� */
    UINT16      usServerID;                             /* ��������ʶ */

    UINT32      ulLinkHelloInterval;                    /* ��·���ʱ���� */
    UINT32      ulLinkHelloFailNum;                     /* �����·���ʧ�ܴ��� */
    UINT32      ulMaxSocketErrNum;                      /* ���socket�쳣�����������ô����������*/
    BOOL        bAllowSyncMsg;                          /* �Ƿ�֧��ͬ����Ϣ */
    UINT8       ucMsgPri;
}DTP_CLIENT_CFG_T;

/************************** ��Ϣͷ *********************************************/
#define DTP_MAX_PKG_MSG_NUM             256     /* ��Ϣ���е��¼����� */

/************************** ver nego *********************************************/
#define DTP_VER_NEGO_OK                 0       /* �汾Э��ͨ�� */
#define DTP_VER_NEGO_VERTOOHIGH         1       /* �汾���� */
#define DTP_VER_NEGO_VERTOOLOW          2       /* �汾���� */

/************************** link auth *********************************************/
#define DTP_LINK_AUTH_OK                0       /* ��Ȩͨ�� */
#define DTP_LINK_AUTH_LOGINED           1       /* �Ѿ���¼(ͬAUTH_OK) */
#define DTP_LINK_AUTH_INVALID_LOGINID   2       /* Login ID������ */
#define DTP_LINK_AUTH_CIPHER_FAILED     3       /* ���ܽ������ */

/************************** link release *********************************************/
#define DTP_LINK_RELEASE_NORMAL         1       /* �û������ͷ� */
#define DTP_LINK_RELEASE_DATAERROR      2       /* �������ʹ��� */
#define DTP_LINK_RELEASE_OVERLOAD       3       /* ���������������� */

#define DTP_LINK_RELEASE_OK             0       /* �ͷųɹ� */
#define DTP_LINK_RELEASE_FAIL           1       /* �ͷ�ʧ�� */

/************************** compress nego *********************************************/
#ifdef DTP_VER_20
#define DTP_COMPRESS_NEGO_OK            0       /* �ɹ� */
#define DTP_COMPRESS_NEGO_FAIL          1       /* ʧ�� */
#define DTP_COMPRESS_NEGO_NULL          2       /* ��ѹ�� */
#endif

#define MSG_PRI_IMP                     1
#define MSG_PRI_NOR                     0

/************************** ��Ϣͷ ***************************************************/

typedef struct
{
    UINT32                  ulMsgMagic;
    UINT16                  usMsgVer;
    UINT16                  usMsgType;      /* ��Ϣ���� */

    UINT32                  ulMsgLen;       /* ��Ϣ�ܳ���(����Ϣͷ����Ϣ��) */
    UINT32                  ulSeqID;        /* ��������ˮ�ţ�˳���ۼӣ�����Ϊ1��ѭ��ʹ�ã�һ��������һ�������Ӧ����Ϣ����ˮ�ű�����ͬ�� */

    UINT16                  usSenderModule;
    UINT16                  usReceiverModule;

    UINT8                   ucCompType;     /* ����ѹ���㷨 */
    UINT8                   ucEncryptType;  /* ���ݼ����㷨 */
    UINT16                  usTimeout;      /* ��Ϣ��Чʱ�䣬��λ�� */

    UINT16                  usCRC;          /* У���룬����Ϣ������ÿ2���ֽ�ת��Ϊ16λ������ӣ��������һ���ֽ�ת��Ϊ8λ������� */
    UINT8                   ucMsgPri;
    UINT8                   ucSenderType;   /* ���������� */

    UINT8                   aucResv[4];     /* ȫ��0 */
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

    UINT8                   aucClientAddr[4];       // �ͻ���������ַ
    UINT16                  usClientPort;           // �ͻ��������˿�
}DTP_UDP_LINK_AUTH_REQ_T;

typedef struct
{
    DTP_MSG_HDR_T           stMsgHdr;

    UINT8                   ucResult;
    UINT8                   ucResv;

    UINT8                   aucClientAddr[4];       // �ͻ��˷��͵�ַ
    UINT16                  usClientPort;           // �ͻ��˷��Ͷ˿�

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

/************************** ����Ϣͬʱ������Ϣ�� *********************************************/
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

/************************** �����ṹ *********************************************/
// ��Ϣ���г�
typedef struct
{
    UINT32                  ulBlockSize;    /* �ڴ���С */
    UINT32                  ulBlockNum;     /* �ڴ����� */
}DTP_MSG_POOL_T;

#pragma pack(pop)

typedef VOID (*DTP_CLIENT_MSG_HANDLER)(HANDLE hClient, DTP_MSG_T * pstMsg);
typedef VOID (*DTP_SERVER_MSG_HANDLER)(HANDLE hServer, UINT16 usClientID, DTP_MSG_T * pstMsg);

/************************** ���� *********************************************/


#endif

