#ifndef ATS_H
#define ATS_H

// ���ӿ����䱱����ATS

#define ATS_REC_MAGIC               "ATS_TCT"   // ������

#define ATS_SERVER_PORT             2100

#define ATS_FRAME_HEAD              0xEFEF
#define ATS_FRAME_TAIL              0xFDFD

#define ATS_MAX_DATA_LEN            1024

#define ATS_VER                     0x01

#define ATS_MSGID_HEARTBEAT         0x01    // ������Ϣ
#define ATS_MSGID_ATSINFO           0x05    // �г�λ����Ϣ

#define ATS_MAX_TRAIN_NUM           120

#pragma pack(push, 1)

typedef struct
{
    UINT16  usFrameHead;    // ֡ͷ���̶�Ϊ0xEF,0xEF
    UINT8   ucFrameCount;   // ��֡��
    UINT8   ucFrameIndex;   // ��ǰ֡��ţ���1��ʼ
    UINT16  usDataLen;      // ��ʾData��ĳ���
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
    UINT16      usMsgLen;   // ��Ϣ���ȣ����������ȱ���<=65533
    ATS_TIME_T  stMsgTime;  // ��Ϣ����ʱ��(UTC)
    UINT16      usLineID;   // ��·��
    UINT8       ucResv[18]; // Ԥ��
    UINT8       ucMsgID;    // ��ϢID, ����������Ϣ��������
    UINT8       ucVer;      // �汾�ţ���Э��Ϊ0x01
}ATS_MSG_HDR_T;

// �г������
#define ATS_TC1                 0x01 // TC1��Ϊ��ǰ�ļ����
#define ATS_TC2                 0x02 // TC2��Ϊ��ǰ�ļ����
#define ATS_INVALID_TC          0xFF // ��Ч

// �г�����
#define ATS_DIRECTION_UP        0   // ����
#define ATS_DIRECTION_DOWN      1   // ����
#define ATS_DIRECTION_UNKNOWN   2   // δ֪

// �Ƿ����۷����泵��
#define ATS_ON_TURNBACK_TRACK_NO      0   // ����
#define ATS_ON_TURNBACK_TRACK_YES     1   // ��

// ������ʶ
#define ATS_INOUT_DEPOT_STATE_UNKNOW    0xff  // δ֪
#define ATS_INOUT_DEPOT_STATE_OUT       0     // �г���ͣ����/�����ν�������
#define ATS_INOUT_DEPOT_STATE_IN        1     // �г������߽���ͣ����/������

// �Ƿ���ת����
#define ATS_ON_TRANSFER_NO          0   // ����
#define ATS_ON_TRANSFER_YES         1   // ��

// �Ƿ���վ̨
#define ATS_ON_PLATFORM_NO          0   // ����
#define ATS_ON_PLATFORM_YES         1   // ��

// �Ƿ�ʧȥ��ʵλ��
#define ATS_LOST_REAL_LOCATION_NO   0 // û��ʧȥ��ʵλ����Ϣ
#define ATS_LOST_REAL_LOCATION_YES  1 // ʧȥ��ʵλ����Ϣ

// ����ɲ��
#define ATS_EB_NO                   0x55    // �޽����ƶ�
#define ATS_EB_ON                   0xAA    // �����ƶ�

// ��ʻģʽ
#define ATS_DRIVE_MODE_AM           1   // ���˵��Զ���ʻ
#define ATS_DRIVE_MODE_CM           2   // �˹���ʻ
#define ATS_DRIVE_MODE_RM           3   // ����25���˹���ʻ
#define ATS_DRIVE_MODE_EUM          4   // �г��źż�ʻ
#define ATS_DRIVE_MODE_FAM          5   // ȫ�Զ���ʻ
#define ATS_DRIVE_MODE_CAM          6   // ����25��ȫ�Զ���ʻ
#define ATS_DRIVE_MODE_RRM          7   // ����25��ȫ�Զ���ʻ, rrm�Ĺ��ܱ�cam����
#define ATS_DRIVE_MODE_NULL         0xff

typedef struct
{
    //UINT8   ucTrainNum;     // �г���Ŀ
    //  Repeat begin
    UINT8       ucTrainUnitIDLen;
    CHAR        acTrainUnitID[4];       // ����ţ��̶������г��ĳ���Ų��䣬ȡֵ��Χ0001��9999
    UINT8       ucServiceNumberLen;
    CHAR        acServiceNumber[3];     // ����ţ����г���������/ͣ����˳���ţ���һ��ķ����б��ֲ��䣬ȡֵ��Χ001��999
    UINT8       ucOrderNumberLen;
    CHAR        acOrderNumber[4];       // ���κţ������г����۷��������еı仯���仯������Ϊż��������Ϊ������ȡֵ��Χ01��99
    UINT8       ucDestIDLen;
    CHAR        acDestID[3];            // Ŀ�ĵ��룬ȡֵ��Χ001��999
    UINT16      usRtuID;                // ���ڼ���վվ��
    UINT16      usStationID;            // ���ڳ�վ�ı��
    UINT16      usUpsideStationID;      // ���в೵վ�ı��, ��ͣ����/������ʱ��ȡֵΪͣ����/�����α�ţ��г��ڳ�վʱΪ��վID
    UINT16      usDownsideStationID;    // ���в೵վ�ı��, ��ͣ����/������ʱ��ȡֵΪͣ����/�����α�ţ��г��ڳ�վʱΪ��վID
    UINT8       ucActiveTCID;           // �г��ļ����
    UINT8       ucDirection;            // ���з���, 0������ 1������ 2��δ֪
    UINT8       ucInOutDepot;           // ������ʶ, 0����ͣ����/�����ν������� 1�������߽���ͣ����/������
    UINT8       ucOnTransferTrackFlag;  // 0����ͷѹ������Ϊ����ת���� 1����ͷѹ�뼴��Ϊ��ת���죻
    UINT8       ucOnTurnBackTrackFlag;  // 0���г������۷����泵�� 1���г����۷����泵��
    UINT8       ucIsOnPlatform;         // 0���г����ڳ�վվ̨ 1���г��ڳ�վվ̨
    UINT8       ucLostRealLocation;     // 0���г�û��ʧȥ��ʵλ�� 1���г�ʧȥ��ʵλ��, ��ʧλ�ú�������Ϣ������һ֡��״̬
    UINT16      usLogicSectionID;       // �г������߼�����ID ���߼����ζ�λ�г�λ��
    UINT16      usPhysicSectionID;      // �г�������������ID
    UINT8       ucIndexInSection;       // ͬһ�����������г��Ĵ���, ȡֵ��0��ʼ
    UINT8       ucCompartmentNum;       // ��������
    UINT16      usDriverNumber;         // ��ʻԱ���
    UINT8       ucTimeoutStatus;        // 0��������ͣ����ʱ�� 1��������ͣ����ʱ��
    INT32       iOtpTime;               // �����ʱ�䣨��λ���룬-215- +215 ��������ʾ�г����������������ʾ�г����������
    ATS_TIME_T  stArriveTime;           // �г�����ʱ��(����ʱ��)
    ATS_TIME_T  stDepartTime;           // �г��뿪ʱ��(����ʱ��)
    UINT8       ucDriveMode;            // ��ʻģʽ��
    UINT8       ucIsOpenDoor;           // ����״̬��
    UINT8       ucIsStopped;            // ͣ��״̬��
    UINT8       ucControlMode;          // ���п��Ƽ���
    UINT8       ucARFlag;               // �г����˹����У�ԭβ��ATP��ZC����ͨѶ���۷��������֮ǰ��״̬
    UINT8       ucEBFlag;               // �޽����ƶ���0x55�������ƶ���0xAA���������Ƿ���
    UINT8       ucRate;                 // �����ʣ��ٷֱȣ�����50����ʾ50%��
    UINT8       ucSpeed;                // �ٶȣ�KM/H��
    UINT8       ucStopDeviation;        // ͣ������Ƿ���־��0x55=���������ֵ��0xAA=Ƿ����߸�ֵ0xFF=Ĭ��ֵ��δ����ͣվ��Ϊ������Ч
    UINT8       ucSnowStatus;           // ��ѩģʽ��0-Ĭ��1-��ѩģʽ
    UINT8       ucFireStatus;           // ���ֱ�����0-Ĭ��1-������������
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
    ATS_MSG_ATSINFO_T   stRecData;      // ʵ�ʵĳ����Ǳ䳤��
}ATS_REC_T;

#pragma pack(pop)

#endif
