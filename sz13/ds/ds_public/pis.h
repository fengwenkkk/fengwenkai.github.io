#ifndef PIS_H
#define PIS_H

// PIS��Ϣ
#define EV_PIS_GET_TRAIN_INFO           0x0301  // ��ѯ�г���Ϣ����Ӧ��
#define EV_PIS_SELFCHECK_STATUS         0x0302  // ����̨�Լ�״̬�ϱ�����Ӧ��

#define EV_PIS_HEART_BEAT               0x0401  // ϵͳ������״̬��⣩����Ӧ��
#define EV_IPH_TALK_APPLY               0x0402  // ����Խ�����Ӧ��
#define EV_IPH_TALK_APPLY_ACK           0x0403  // �Խ�ȷ��Ӧ��
#define EV_IPH_AUDIO_UPLOAD             0x0404  // IPH��Ƶ�ϴ�
#define EV_IPH_AUDIO_DOWNLOAD           0x0405  // ����̨��Ƶ�·�
#define EV_IPH_STOP_TALK                0x0406  // �Խ�����֪ͨ
#define EV_IPH_PAUSE_TALK               0x0407  // ͨ����������/Ӧ��
#define EV_IPH_RENEW_TALK               0x0408  // ͨ������ָ�����/Ӧ��
#define EV_IPH_DEV_RESET                0x0409  // IPH�豸��λ����/Ӧ��

#define EV_PA_REC_CFG                   0x0501  // ��ȡԤ¼���㲥���������/Ӧ��
#define EV_PA_GET_SYS_STATUS            0x0502  // ��ȡPAϵͳ״̬����/Ӧ����Ϊ������
#define EV_PA_REC_BRD                   0x0503  // Ԥ¼���㲥����/Ӧ��

#define EV_PA_START_REAL_BRD            0x0601  // �˹��㲥����/Ӧ��
#define EV_PA_PUSH_AUDIO_DATA           0x0602  // �˹��㲥�����·�
#define EV_PA_STOP_REAL_BRD             0x0603  // �˹��㲥��������/Ӧ��

#pragma pack(push, 1)

// ��Ϣͷ
#define PIS_MSG_MAGIC           0xfe01  // PISЭ��ͷ

#define PA_AND_PIS_HEART_BEAT   3


typedef struct
{
    UINT16      usMsgMagic;
    UINT16      usMsgType;  // ������
    UINT16      usMsgLen;   // �ֳ�
}PIS_MSG_HDR_T, PA_MSG_HDR_T;

#define MAX_IPH_DEV_NUM             64  // IPH�����������

//����̨�Լ�״̬
#define APP_SELF_CHECK_STATUS_SUCC      0
#define APP_SELF_CHECK_STATUS_FAILED    1
#define APP_SELF_CHECK_STATUS_UNKNOWN   (-1)

//IPH���ACK���ʱ��-�뼶
#define IPH_CHECK_ACK_MAX_TIME      3

//IPH����״̬
#define PIS_LINK_STATUS_OFFLINE     0   // ����
#define PIS_LINK_STATUS_ONLINE      1   // ����

// ����IPH�Խ�Ӧ��������
#define IPH_APPLY_ACCEPT            1   // ����Խ�
#define IPH_APPLY_REJECT            2   // �ܾ��Խ�
#define IPH_APPLY_WAIT              3   // �Ŷӵȴ���

//IPHϵͳ����״̬
#define IPH_STATUS_NORMAL           0   // ����
#define IPH_STATUS_ABNORMAL         1   // �쳣

//IPHһ����λ
#define IPH_TALK_RESET              0xff // ��IPH���������б�����������Լ�������ID����0xffʱ��Ϊ��һ����λ����/Ӧ��

//�Խ���������
#define IPH_PAUSE_SUCC              0    // �������� ����ֵ�쳣

//IPH�豸��λ
#define IPH_DEV_RESET_SUCC          0    // IPH�豸��λ�ɹ�

//��״̬
#define COVER_STATUS_OPEN           1   // ��
#define COVER_STATUS_CLOSE          0   // �ر�

//��ʻģʽ
#define DRIVE_MODE_MANUAL           1   // ���˼�ʻ
#define DRIVE_MODE_AUTO             0   // ���˼�ʻ

//PAϵͳ״̬
#define PA_STATUS_NA                0   // ϵͳ������
#define PA_STATUS_IDLE              1   // ����
#define PA_STATUS_WORK              2   // �㲥��

#define PA_SLAVE_STATUS             0   // ����
#define PA_MASTER_STATUS            1   // ����
#define PA_STATUS_UNKNOW            0xff   // δ֪

//PAԤ¼���㲥Ӧ��״̬
#define PA_REC_BRD_FAIL             0   // Ԥ¼���㲥ʧ��
#define PA_REC_BRD_SUCC             1   // Ԥ¼���㲥�ɹ�
#define PA_REC_BRD_BUSY             2   // �㲥�豸æ

//PAȫ���㲥
#define PA_REAL_BRD_ALL_CARRIAGE    0xff // ȫ���㲥�� 1~8��������㲥
#define PA_REAL_BRD_MIN_CARRIAGE_ID    1
#define PA_REAL_BRD_MAX_CARRIAGE_ID    8

// PCM��������
#define SAMPLE_RATE_8000            8000 // 8K����Ƶ��
#define SAMPLE_RATE_16000           16000 // 16K����Ƶ��

#define SAMPLE_BITS_8               8    // 8bit����λ��
#define SAMPLE_BITS_16              16   // 16bit����λ��

// ��ѯ�г���ϢӦ��
typedef struct
{
    UINT8       ucCoverStatus;  // 1���� 0���ر�
    UINT8       ucDriverMode;   // 1���� 0���ر�
}PIS_GET_TRAIN_INFO_RSP_T;

// ����̨�Լ�״̬�ϱ�����
typedef struct
{
    UINT8       ucCheckStatus;  // �Լ�״̬��0������ �������쳣
}PIS_SELFCHECK_STATUS_REQ_T;

typedef struct
{
    CHAR       ucStatus;       // ����״̬��0���������������쳣
}IPH_GET_SYS_STATUS_RSP_T;

// ����IPH�Խ�������������
typedef struct
{
    UINT16      usTrainUnitID;         // ����ţ�����ţ�
    UINT8       ucCarriageID;          // �����
    UINT8       ucIPHDevID;            // ������ID
    UINT16      usIPHDevDoorID;        // ��������Ӧ�ĳ��ź�
    UINT8       aucIPHRecvAudioAddr[4]; // IPH������������Ƶ�ĵ�ַ
    UINT16      usIPHRecvAudioPort;    // IPH������������Ƶ�ĵ�ַ
    CHAR        acVideoUrl[84];        // �������ƵURL
    CHAR        acCCTVVideoUrl[84];    // CCTV�������ƵURL, Ԥ����0
}IPH_TALK_APPLY_REQ_INFO_T;

typedef struct
{
    UINT8                       ucNum;              // IPH����������
    IPH_TALK_APPLY_REQ_INFO_T   astApplyReq[MAX_IPH_DEV_NUM];   // IPH�Խ���������
}IPH_TALK_APPLY_REQ_T;

typedef struct
{
    UINT16      usTrainUnitID;          // ����ţ�����ţ�
    UINT8       ucCarriageID;           // �����
    UINT8       ucIPHDevID;             // ������ID
    UINT16      usIPHDevDoorID;         // ��������Ӧ�ĳ��ź�
    UINT8       ucApplyResult;          // ����Խ���� 1:����Խ� 2:�ܾ��Խ� 3:�Ŷӵȴ���
    UINT8       aucAudioRecvAddr[4];    // IPH�ϴ��Խ���Ƶ�ĵ�ַ
    UINT16      usAudioRecvPort;        // IPH�ϴ��Խ���Ƶ�Ķ˿�
}IPH_TALK_APPLY_RSP_INFO_T;

// IPH�Խ�����Ӧ��
typedef struct
{
    UINT8                       ucNum;              // IPH����������
    IPH_TALK_APPLY_RSP_INFO_T   astApplyRsp[MAX_IPH_DEV_NUM];   // IPH�Խ�����Ӧ������
}IPH_TALK_APPLY_RSP_T;

// IPH �������ȷ��
typedef struct
{
    UINT16      usTrainUnitID;  // ����ţ�����ţ�
    UINT8       ucCarriageID;   // �����
    UINT8       ucIPHDevID;     // ������ID
    UINT16      usIPHDevDoorID; // ��������Ӧ�ĳ���
}IPH_TALK_APPLY_ACK_T;

// �����Խ�
typedef struct
{
    UINT16      usTrainUnitID;  // ����ţ�����ţ�
    UINT8       ucCarriageID;   // ����� 0xff����ʾһ����λ
    UINT8       ucIPHDevID;     // ������ID 0xff: ��ʾһ����λ
}IPH_TALK_STOP_REQ_T, IPH_TALK_STOP_RSP_T;

// �Խ���������
typedef struct
{
    UINT16      usTrainUnitID;  // ����ţ�����ţ�
    UINT8       ucCarriageID;
    UINT8       ucIPHDevID;
}IPH_TALK_PAUSE_REQ_T;

// �Խ�����Ӧ��
typedef struct
{
    UINT16      usTrainUnitID;  // ����ţ�����ţ�
    UINT8       ucCarriageID;
    UINT8       ucIPHDevID;
    UINT8       ucIPHPauseResult; // 0:����ɹ������� ����ֵ������ʧ��
}IPH_TALK_PAUSE_RSP_T;

//IPH��λ����
typedef struct
{
    UINT8       ucIPHResetStatus; // 0:������ ����ֵ���쳣
}IPH_DEV_RESET_REQ_T;

//IPH��λӦ��
typedef struct
{
    UINT8       ucIPHResetACK; // 0:�ɹ��� ����ֵ���쳣
}IPH_DEV_RESET_RSP_T;

typedef struct
{
    UINT32      ulBrdSeqID;     // �㲥���
    UINT8       ucCarriageID;   // 0xff��ȫ���㲥�� 1~8��������㲥
}PA_REAL_BRD_REQ_T;

typedef struct
{
    UINT32      ulBrdSeqID;         // �㲥���
    UINT16      usPARecvAudioPort;  // PA�����������ݵĶ˿�
    UINT8       ucBrdResult;        // �˹��㲥���  0��ʧ�� 1���ɹ�
}PA_REAL_BRD_RSP_T;

// Ԥ¼���㲥����
typedef struct
{
    UINT32      ulRecID;        // Ԥ¼���㲥���
    UINT32      ulBrdSeqID;     // �㲥���
    UINT8       ucBrdLoopTime;  // �㲥ѭ������ ȱʡΪ1
}PA_REC_BRD_REQ_T;

typedef struct
{
    UINT32      ulRecID;        // Ԥ¼���㲥���
    UINT32      ulBrdSeqID;     // �㲥���
    UINT8       ucRet;          // 0��ʧ�� 1���ɹ� 2: ϵͳæ�����ڹ㲥�У�
}PA_REC_BRD_RSP_T;

typedef struct
{
    UINT8       ucIsMaster;     // ����״̬��0������ 1������
    UINT8       ucStatus;       // ����״̬��0��ϵͳ�����ã�1�������У�2�����ڹ㲥��
}PA_GET_SYS_STATUS_RSP_T;



typedef struct
{
    UINT16      usTrainUnitID;   // ����ţ�����ţ�
    UINT8       ucCarriageID;   // �����
    UINT8       ucIPHDevID;     // ������ID

    UINT32      ulPCMPkgIndex;  // PCM���ݰ���ţ���һ�����ݰ�Ϊ0����������
    UINT32      ulPCMPkgTime;   // PCM���ݰ�ƫ��ʱ�䣬��Ե�һ��PCM���ݰ������ƫ��ʱ�䣬��λms
    UINT16      usSampleRate;   // ����Ƶ�ʣ�Hz��   8000/16000/44100��
    UINT8       ucChanNum;      // ����������   1��������  2��˫����
    UINT8       ucSampleBits;   // ��������λ�� 8��8λ��1�ֽڣ� 16��16λ��2�ֽڣ�
    UINT16      usPCMDataLen;   // ���ݳ���(=usSampleRate*ucChanNum*ucSampleBits/8)
    UINT8       aucPCMData[1];  // PCM��Ƶ����
}IPH_PCM_DATA_T;

typedef struct
{
    UINT32      ulBrdSeqID;      // �㲥���
    UINT32      ulPCMPkgIndex;   // PCM���ݰ���� ��һ�����ݰ�Ϊ0 ��������
    UINT16      usSampleRate;    // ����Ƶ�ʣ�8000/16000 ����ʹ��8000
    UINT8       ucChanNum;       // ����������   1��������  2��˫����
    UINT8       ucSampleBits;    // ��������λ�� 8��8λ��1�ֽڣ� 16��16λ��2�ֽڣ�
    UINT16      usPCMDataLen;    // ���ݳ���  ������16384(=usSampleRate*ucChanNum*ucSampleBits/8)
    UINT8       aucPCMData[1];   // PCM��Ƶ����
}PA_PCM_DATA_T;

// �����˹��㲥
typedef struct
{
    UINT32      ulBrdSeqID;      // �㲥���
}PA_STOP_REAL_BRD_REQ_T, PA_STOP_REAL_BRD_RSP_T;

#pragma pack(pop)

#endif

