#ifndef MSG_H
#define MSG_H

#define EV_REMOTE_START                     EV_SERVICE_START    // 0x4000

#define EV_SYS_START                        (EV_REMOTE_START + 0x0100)
#define EV_CFG_START                        (EV_REMOTE_START + 0x0300)
#define EV_USER_START                       (EV_REMOTE_START + 0x0700)
#define EV_LOG_START                        (EV_REMOTE_START + 0x0800)
#define EV_CHECK_START                      (EV_REMOTE_START + 0x0900)

#define EV_RADIO_START                      (EV_REMOTE_START + 0x2000)  // ����̨/�̶�̨���
#define EV_TEST_START                       (EV_REMOTE_START + 0x0A00)   // TaskTest

#define EV_ESDK_MSG                         (EV_REMOTE_START + 0x3000)

/* Sys */
#define EV_UPLOAD_FILE_REQ                  (EV_SYS_START + 0)  // �ļ��ϴ�
#define EV_DOWNLOAD_FILE_REQ                (EV_SYS_START + 1)  // �ļ��ϴ�
#define EV_CONNECTION_CHANGE_MSG            (EV_SYS_START + 2)
#define EV_SET_DATACLEAR_CFG_REQ            (EV_SYS_START + 3)
#define EV_SERVER_TO_AGENT_ANSWER           (EV_SYS_START + 4)
#define EV_SYSTEM_ALARM_PARA_IND            (EV_SYS_START + 5)  //֪ͨTaskAlarm��������
#define EV_DATA_CLEAR_REQ                   (EV_SYS_START + 6)
#define EV_SET_STATUS_REQ                   (EV_SYS_START + 7)
#define EV_STATUS_REFRESH_REQ               (EV_SYS_START + 8)  // ����ȴ�״̬

#define EV_SYS_RESET_REQ                    (EV_SYS_START + 9)
#define EV_HARDWARE_RESET_REQ               (EV_SYS_START + 10)

#define EV_CHANGE_RUN_MODE_REQ              (EV_SYS_START + 11)

#define EV_DOWNLOAD_FILE_EX_REQ             (EV_SYS_START + 21) // �ļ�������չ�ӿ�

/* sys Rsp */
#define EV_UPLOAD_FILE_RSP                  EV_RSP(EV_UPLOAD_FILE_REQ)
#define EV_DOWNLOAD_FILE_RSP                EV_RSP(EV_DOWNLOAD_FILE_REQ)
#define EV_SET_DATACLEAR_CFG_RSP            EV_RSP(EV_SET_DATACLEAR_CFG_REQ)
#define EV_DATA_CLEAR_RSP                   EV_RSP(EV_DATA_CLEAR_REQ)

#define EV_SET_STATUS_RSP                   EV_RSP(EV_SET_STATUS_REQ)
#define EV_STATUS_REFRESH_RSP               EV_RSP(EV_STATUS_REFRESH_REQ)   // ����ȴ�״̬

#define EV_SYS_RESET_RSP                    EV_RSP(EV_SYS_RESET_REQ)
#define EV_CHANGE_RUN_MODE_RSP              EV_RSP(EV_CHANGE_RUN_MODE_REQ)

#define EV_DOWNLOAD_FILE_EX_RSP             EV_RSP(EV_DOWNLOAD_FILE_EX_REQ)

/* Check */

/* RADIO */
#define EV_APP_DIS_REQ                      (EV_RADIO_START+0)      // APP��Ϣ
#define EV_SA_DIS_REQ                       (EV_RADIO_START+100)    // SA��Ϣ

#define EV_APP_DIS_RSP                      EV_RSP(EV_APP_DIS_REQ)
#define EV_SA_DIS_RSP                       EV_RSP(EV_SA_DIS_REQ)

/* Cfg */
#define EV_CLIENT_REQ                       (EV_CFG_START + 0)      // �ͻ���->��������ͨ����Ϣ
#define EV_DC_DIS_REQ                       (EV_CFG_START + 1)      // DC->DIS��ͨ����Ϣ
#define EV_DC_DIS_CFG_REQ                   (EV_CFG_START + 2)      // DC->DIS����������
#define EV_DIS_DC_IND                       (EV_CFG_START + 3)      // DIS->DC��֪ͨ��Ϣ
#define EV_DC_DIS_OMA_REQ                   (EV_CFG_START + 4)      // DC->DIS��OMA����

#define EV_APP_DIS_DC_REQ                   (EV_CFG_START + 20)     // DISת��APP��Ϣ��DC
#define EV_SA_DIS_DC_REQ                    (EV_CFG_START + 21)     // DISת��SA��Ϣ��DC

#define EV_DC_DIS_APP_REQ                   (EV_CFG_START + 31)     // DISת��DC��Ϣ��APP
#define EV_DC_DIS_SA_REQ                    (EV_CFG_START + 32)     // DISת��DC��Ϣ��SA

#define EV_UE_DIS_REQ                       (EV_CFG_START + 33)     // ���ֳ�̨(UE)���͵�������(dis)����Ϣ
#define EV_DC_DIS_UE_REQ                    (EV_CFG_START + 34)     // DISת��DC��Ϣ��UE

#define EV_NOCC_REQ                         (EV_CFG_START + 41)     // NOCC��OCC����Ϣ
#define EV_OCC_REQ                          (EV_CFG_START + 42)     // OCC��NOCC����Ϣ

/* Cfg Rsp */
#define EV_CLIENT_RSP                       EV_RSP(EV_CLIENT_REQ)
#define EV_DC_DIS_RSP                       EV_RSP(EV_DC_DIS_REQ)
#define EV_DC_DIS_CFG_RSP                   EV_RSP(EV_DC_DIS_CFG_REQ)

#define EV_APP_DIS_DC_RSP                   EV_RSP(EV_APP_DIS_DC_REQ)
#define EV_SA_DIS_DC_RSP                    EV_RSP(EV_SA_DIS_DC_REQ)

#define EV_DC_DIS_APP_RSP                   EV_RSP(EV_DC_DIS_APP_REQ)
#define EV_DC_DIS_SA_RSP                    EV_RSP(EV_DC_DIS_SA_REQ)

#define EV_NOCC_RSP                         EV_RSP(EV_NOCC_REQ)
#define EV_OCC_RSP                          EV_RSP(EV_OCC_REQ)

/* User */

/* User Rsp */

//Log Req
#define EV_GET_OPER_LOG_REQ                 (EV_LOG_START + 0)
#define EV_DEL_OPER_LOG_REQ                 (EV_LOG_START + 1)
#define EV_ADD_OPER_LOG_REQ                 (EV_LOG_START + 5)

//Log Rsp
#define EV_GET_OPER_LOG_RSP                 EV_RSP(EV_GET_OPER_LOG_REQ)
#define EV_DEL_OPER_LOG_RSP                 EV_RSP(EV_DEL_OPER_LOG_REQ)

/* OMC */
#define EV_OMA_OMC_REQ                      (EV_OMC_START + 1)      // OMA->OMC��ͨ����Ϣ
#define EV_OMT_OMC_REQ                      (EV_OMC_START + 2)      // OMA->OMC��ͨ����Ϣ

#define EV_OMT_ADD_NE_REQ                   (EV_OMC_START + 10)     // �����Ԫ
#define EV_OMT_DEL_NE_REQ                   (EV_OMC_START + 11)     // ɾ����Ԫ

#define EV_OMT_GET_NEINFO_REQ               (EV_OMC_START + 50)     // ��ȡ��Ԫ��Ϣ

#define EV_OMT_GET_TXSTATUS_REQ             (EV_OMC_START + 70)     // ��ȡĳ��TX�豸��״̬���ݣ�һ��ʱ���ڣ�
#define EV_OMT_GET_ALLTXSTATUS_REQ          (EV_OMC_START + 71)     // ��ȡ����TX�豸��ĳ��ʱ�̵�״̬����
#define EV_OMT_SET_ALARM_LEVEL_REQ          (EV_OMC_START + 72)     //���ø澯�ȼ� tzl

#define EV_OMA_OMC_RSP                      EV_RSP(EV_OMA_OMC_REQ)
#define EV_OMT_OMC_RSP                      EV_RSP(EV_OMT_OMC_REQ)

#define EV_OMT_ADD_NE_RSP                   EV_RSP(EV_OMT_ADD_NE_REQ)
#define EV_OMT_DEL_NE_RSP                   EV_RSP(EV_OMT_DEL_NE_REQ)

#define EV_OMT_GET_NEINFO_RSP               EV_RSP(EV_OMT_GET_NEINFO_REQ)
#define EV_OMT_GET_TXSTATUS_RSP             EV_RSP(EV_OMT_GET_TXSTATUS_REQ)
#define EV_OMT_GET_ALLTXSTATUS_RSP          EV_RSP(EV_OMT_GET_ALLTXSTATUS_REQ)

// LMT
#define EV_LMT_START                        (EV_OMC_START + 0x800)  // LMT
#define EV_LMT_SHELL_CMD_REQ                (EV_LMT_START + 0)
#define EV_LMT_DOWNLOAD_FILE_REQ            (EV_LMT_START + 1)
#define EV_LMT_UPLOAD_FILE_REQ              (EV_LMT_START + 2)

#define EV_LMT_SHELL_CMD_RSP                EV_RSP(EV_LMT_SHELL_CMD_REQ)
#define EV_LMT_DOWNLOAD_FILE_RSP            EV_RSP(EV_LMT_DOWNLOAD_FILE_REQ)
#define EV_LMT_UPLOAD_FILE_RSP              EV_RSP(EV_LMT_UPLOAD_FILE_REQ)

#pragma pack(push, 4)

#define OPER_TYPE_ADMIN         1   // ��������Ա
#define OPER_TYPE_MANAGE        2   // һ�����
#define OPER_TYPE_QUERY         3   // ��ѯ����

/****************************** SYS ��Ϣ�ṹ���� *************************************/
// �鶩��״̬
#define GROUP_STATUS_UNKNOWN    0
#define GROUP_STATUS_TOJOIN     1   // ���Զ�����
#define GROUP_STATUS_TOUNSUB    2   // ���Զ�ȥ����
#define GROUP_STATUS_UNSUB      3   // ȥ����
#define GROUP_STATUS_SUB        4
#define GROUP_STATUS_JOIN       5
#define GROUP_STATUS_DELETE     6

// ��������
#define CALL_TYPE_UNKNOWN               0   //

#define CALL_TYPE_PTT                   1   // ���
#define CALL_TYPE_EMERGENCY_CALL        2   // ��������
#define CALL_TYPE_TRAIN_BRD             3   // �г��㲥
#define CALL_TYPE_ZONE_BRD              4   // ѡ���㲥

#define CALL_TYPE_P2P                   11  // �������
#define CALL_TYPE_P2P_TRANSFER          12  // �������ת��
#define CALL_TYPE_P2P_EMERGENCY_CALL    13  // �����������
#define CALL_TYPE_LRBUS_REQ_CALL        14  // LRBUS�������(�����
#define CALL_TYPE_HALF_DUPLEX_P2P_CALL  16  // ��˫���������

#define CALL_TYPE_VIDEO_P2P             21  // ��Ƶ���
#define CALL_TYPE_VIDEO_REALPLAY        22  // ��Ƶ���������̨����
#define CALL_TYPE_VIDEO_RECV            23  // ��Ƶ���գ��ն˷���

#define CALL_TYPE_VIDEO_DISPATCH        31  // ��Ƶ�ַ�
#define CALL_TYPE_VIDEO_ONWALL          32  // ��Ƶ��ǽ
#define CALL_TYPE_VIDEO_PTT             33  // ��Ƶ���

#define CALL_TYPE_REQ_CALL              91  // ���
#define CALL_TYPE_IPH                   92  // �˿ͶԽ�
#define CALL_TYPE_ENV_LISTEN            93  // ��������
#define CALL_TYPE_DISCREET_LISTEN       94  // ���ܼ���

#define TRAIN_REGISTER                  101 // �г�ע��
#define TRAIN_SET_TRAINID               102 // �����г����κ�

// ������Ⱥ״̬
#define CLUSTER_STATUS_STAND_ALONE      0
#define CLUSTER_STATUS_MASTER           1
#define CLUSTER_STATUS_SLAVE            2
#define CLUSTER_STATUS_CHECK_FAIL       9

#define TC_HEAD                         1  // 1��ʾ��ͷ������ֵΪ��β

// upload file
typedef struct
{
    CHAR                    acFile[256];

    UINT64                  u64StartPos;
    UINT32                  ulIndex;
    UINT32                  ulDataLen;
    CHAR                    aucData[32*1024];
}UPLOAD_FILE_REQ_T;

typedef struct
{
    CHAR                    acFile[256];

    UINT8                   ucRet;
    UINT8                   aucResv[3];

    UINT32                  ulIndex;
}UPLOAD_FILE_RSP_T;

// download file
typedef struct
{
    CHAR                    acFile[256];
    UINT32                  ulIndex;
    UINT64                  u64StartPos;
}DOWNLOAD_FILE_REQ_T;

typedef struct
{
    CHAR                    acFile[256];
    UINT32                  ulIndex;
    UINT32                  ulMaxDataLen;
    UINT64                  u64StartPos;
}DOWNLOAD_FILE_EX_REQ_T;

typedef struct
{
    CHAR                    acFile[256];

    UINT8                   ucRet;
    UINT8                   ucIsEnd;
    UINT8                   aucResv[2];

    UINT32                  ulIndex;
    UINT32                  ulDataLen;
    CHAR                    aucData[32*1024];
}DOWNLOAD_FILE_RSP_T, DOWNLOAD_FILE_EX_RSP_T;

typedef struct
{
    BOOL                    bAutoClear;
    UINT32                  ulClearHour;
    UINT32                  ulClearMinu;
}DATA_CLEAR_CFG_T;

typedef struct
{
    DATA_CLEAR_CFG_T        stDataClearCfg;
}SET_DATACLEAR_CFG_REQ_T;

typedef struct
{
    UINT32                  ulRetCode;
}SET_DATACLEAR_CFG_RSP_T;

typedef struct
{
    UINT32                  ulSeqID;
}SERVER_TO_AGENT_ANSWER_T;

typedef struct
{
    UINT32                  ulDays;
}DEL_DATA_REQ_T;

typedef struct
{
    UINT32                  ulRetCode;
}DEL_DATA_RSP_T;

typedef struct
{
    UINT32                  ulStatus;
}STATUS_CHANGE_REQ_ST;

typedef struct
{
    UINT32                  ulStatus;
    BOOL                    bRet;
}STATUS_CHANGE_RSP_ST;

typedef struct
{
    BOOL                    bRet;
    CHAR                    acWorkDir[256];
    CHAR                    acProcName[256];
}GET_SYS_RESET_RSP;

/****************************** log ��Ϣ�ṹ���� *************************************/
typedef struct
{
    CHAR                    acName[64];
    UINT32                  ulStartTime;
    UINT32                  ulEndTime;
}GET_OPER_LOG_REQ_T;

typedef struct
{
    UINT32                  ulRetCode;
}GET_OPER_LOG_RSP_T;

typedef struct
{
    INT32                   idays;
}DEL_OPER_LOG_REQ_T;

typedef struct
{
    UINT32                  ulRetCode;
}DEL_OPER_LOG_RSP_T;

/****************************** cfg ��Ϣ�ṹ���� *************************************/
#define CFG_DATA_USER           "User"
#define CFG_DATA_GROUP          "Group"
#define CFG_DATA_SUBGROUP       "SubGroup"
#define CFG_DATA_TRAIN          "Train"
#define CFG_DATA_STATION        "Station"

// �ն�Ӳ������
#define TRAIN_UE                1       // ����̨
#define STATION_UE              2       // �̶�̨
#define SMALL_PORTAL_UE         3       // С���ֳ�̨
#define LARGE_PORTAL_UE         4       // �����ֳ�̨
#define DC_USER                 9       // ����̨

// �ն˹��ܷ���
#define UE_DC                   1       // ����
#define UE_TRAIN                2       // �г�
#define UE_STATION              3       // ��վ
#define UE_FIX                  4       // ά��
#define UE_MANAGE               5       // ����
#define UE_OTHER                9       // ����

#define INVALID_GROUP_ID        0
#define INVALID_USER_ID         0
#define INVALID_TRAIN_UNIT_ID   0
#define INVALID_STATION_ID      0
#define INVALID_DEPOT_ID        0
#define INVALID_BUSINESS_ID     0   ///< ��Чҵ��ID
#define DEFAULT_LINE_ID         0
#define ALL_LINE_ID             0xffffffff
#define INVALID_LINE_ID         0xfffffffe

// �����
#define GROUP_FUNC_TRAIN        2   // �г���
#define GROUP_FUNC_STATION      3   // ��վ��
#define GROUP_FUNC_FIX          4   // ά����
#define GROUP_FUNC_MULTIFUNC    6   // ��ְ����
#define GROUP_FUNC_OTHER        9   // ����

// Ⱥ�����
#define STATIC_GROUP            10      // ��̬��
#define TRAIN_GROUP             20      // �г���
#define STATION_GROUP           30      // ��վ��
#define PATCH_GROUP             40      // �ɽ���
#define DYNAMIC_GROUP           50      // ��̬��
#define MULTI_GROUP             60      // ��ѡ��
#define ZONE_BRD_GROUP          99      // ��վѡ���㲥��
#define TRAIN_BRD_GROUP         99      // �г��㲥��ʱ�飨1�������г��㲥�飩

#define MAX_DC_NUM                      32  // ���32������̨
#define MAX_DC_DYNAMIC_GRPUP_NUM        15

#define STATION_TYPE_NORMAL         1   // ��ͨ��վ
#define STATION_TYPE_DEPOT          2   // ͣ����
#define STATION_TYPE_FIX            3   // ά�޶�
#define STATION_TYPE_TEST           4   // ������

#define DEPOT_ID_AUTO               0
#define DEPOT_ID_UNKNOWN            0
#define DEPOT_ID_MAINLINE           1   // 1Ϊ���ߣ������ε�λ��ֵ�������1

#define TRAIN_STATUS_INFO_UNKNOW    0xff   //δ֪

#define TRAIN_POS_UNKNOWN           0   // δ֪
#define TRAIN_POS_MAINLINE          1   // ���ߣ� ��ӦSTATION_TYPE_NORMAL
#define TRAIN_POS_DEPOT             2   // ͣ����/������, ��ӦSTATION_TYPE_DEPOT
#define TRAIN_POS_FIX               3   // ά�޶�, ��ӦSTATION_TYPE_FIX

#define TRAIN_POS_ALL               0xff

// �г�����
#define TRAIN_DIRECTION_UNKNOWN     0   // δ֪
#define TRAIN_DIRECTION_UPSIDE      1   // ����
#define TRAIN_DIRECTION_DOWNSIDE    2   // ����

// �Ƿ����۷����泵��
#define TRAIN_ON_TURNBACK_TRACK_NO      0   // ����
#define TRAIN_ON_TURNBACK_TRACK_YES     1   // ��
#define TRAIN_ON_TURNBACK_TRACK_UNKNOWN 2   // δ֪

// �Ƿ��������
//#define INOUT_DEPOT_STATE_NULL          0   // Ĭ��ֵΪ0
//#define INOUT_DEPOT_STATE_OUT           1   // ���г���ͣ����/�����ν���ת����ʱֵΪ1������5�룩
//#define INOUT_DEPOT_STATE_IN            2   // ���г���ת�������ͣ����/������ʱֵΪ2������5�룩��

// �г������
#define TRAIN_ATS_TC1                 0x01 // TC1��Ϊ��ǰ�ļ����
#define TRAIN_ATS_TC2                 0x02 // TC2��Ϊ��ǰ�ļ����
#define TRAIN_ATS_INVALID_TC          0xFF // ��Ч

// �Ƿ���ת����
//#define TRAIN_ON_TRANSFER_NO      0   // ����
//#define TRAIN_ON_TRANSFER_YES     1   // ��

// �Ƿ���վ̨
#define TRAIN_ON_PLATFORM_NO      0   // ����
#define TRAIN_ON_PLATFORM_YES     1   // ��

// �Ƿ�ʧȥ��ʵλ��
//#define TRAIN_LOST_REAL_LOCATION_NO    0 // û��ʧȥ��ʵλ����Ϣ
//#define TRAIN_LOST_REAL_LOCATION_YES   1 // ʧȥ��ʵλ����Ϣ

// <GetCfgDataReq Type="Station" />
typedef struct
{
    CHAR                    acReq[1024];
}GET_CFG_DATA_REQ_T;

// <GetCfgDataRsp Type="Station">  <Station ... /> ... </GetCfgDataRsp>
typedef struct
{
    CHAR                    acRsp[60*1024];
}GET_CFG_DATA_RSP_T;

typedef struct
{
    UINT32                  ulUserID;
    UINT32                  ulTime;
    CHAR                    acDCOperatorID[32];
    CHAR                    acLogInfo[1024];
}OPER_LOG_INFO_T;

/// ��Ӧ���ݿ��� groupcfg, �洢�ľ�̬������
typedef struct
{
    INT64                   i64UpLineTrainGroupID;          // �����г�ͨ����
    INT64                   i64DownLineTrainGroupID;        // �����г�ͨ����
    INT64                   i64MainLineTrainGroupID;        // �����г�ͨ����

    INT64                   i64UpLineTrainBrdGroupID;       // �����г��㲥��
    INT64                   i64DownLineTrainBrdGroupID;     // �����г��㲥��
    INT64                   i64MainLineTrainBrdGroupID;     // �����г��㲥��

    INT64                   i64AllStationGroupID;           // ���г�վͨ����

    INT64                   i64StartTempDynamicGroupID;     // ��ʱ�����Ķ�̬���������Ŀ�ʼ
    INT64                   i64EndTempDynamicGroupID;       // ��ʱ�����Ķ�̬���������Ľ���

    INT64                   i64DriverGroupID;               // ˾��ͨ����
    INT64                   i64DepotDriverGroupID;          // ������˾��ͨ����
}GROUP_CFG_T;

/// ��Ӧ���ݿ��� dcuser �� �洢 SDK �˺������Ϣ
typedef struct
{
    UINT32                  ulUserID;       ///< SDK ����, ����������
    CHAR                    acName[32];     ///< ���˺ŵ����ƣ������Լ�����
    UINT32                  ulDCType;       ///< ����̨����
    UINT32                  ulDepotID;      ///< �ж����ĸ������Σ������ߡ������ߣ�

    UINT32                  ulStationNum;
    UINT32                  aulStationList[80];

    UINT32                  ulDevStatus;    ///< ���ֶ����ݿ���û��
    UINT32                  ulCallStatus;   ///< ��SDK�¼��ϱ���Ϣ�л�ȡ,  ���ֶ����ݿ���û��
}DC_USER_INFO_T;

/// ��Ӧ���ݿ��� dcoperator �� �洢����Ա�˺������Ϣ
typedef struct
{
    CHAR                    acOperatorID[32];   ///< ����Ա�˺ţ������Լ�����
    CHAR                    acName[32];         ///< ����Ա����
    CHAR                    acPassword[32];     ///< ����
    UINT32                  ulDCType;           ///< ����̨����
}DC_OPERATOR_INFO_T;

typedef struct
{
    CHAR                    acName[32];
    CHAR                    acPassword[32];
    UINT32                  ulOmtType;
}OMT_OPERATOR_INFO_T;

/// ��Ӧ���ݿ��� stationue, ����̶�̨�����Ϣ
typedef struct
{
    UINT32                  ulUserID;           ///< �̶�̨ ID
    CHAR                    acName[32];         ///< �̶�̨����
    CHAR                    acLanAddr[16];      ///< Lan �� IP ��ַ
    UINT32                  ulStationID;        ///< ��վID
    INT64                   i64DefaultGroupID;  ///< �����̨��ͨ����

    UINT32                  ulDevStatus;        ///< ���ֶ����ݿ����û��
    UINT32                  ulCallStatus;       ///< ���ֶ����ݿ����û��
}STATION_UE_INFO_T;

// ��Ӧ���ݿ��� section, ���泵�����������
typedef struct
{
    UINT32                  ulSectionID;        // ����ID
    CHAR                    acSectionName[16];  // ��������
    UINT32                  ulStationID;        // ��վID
    UINT32                  ulDirection;        // ����
    INT32                   iIndexInStation;   // ��վ�ڵ����λ��
    BOOL                    bInPlatform;        // �Ƿ���վ̨
    BOOL                    bInDepotTrack;      // �Ƿ������ߴ泵��
    INT32                   x1;
    INT32                   y1;
    INT32                   x2;
    INT32                   y2;
}SECTION_INFO_T;

/// ��Ӧ���ݿ��� trainue, ���泵��̨����Ϣ
typedef struct
{
    UINT32                  ulUserID;       ///< ����̨ ID
    CHAR                    acName[32];     ///< ����̨��Ϣ
    CHAR                    acLanAddr[16];  ///< Lan �� IP ��ַ
    UINT32                  ulTrainUnitID;  ///< װ���г��ĳ����

    UINT32                  ulDevStatus;    ///< ����û�и��ֶ�
    UINT32                  ulCallStatus;   ///< ����û�и��ֶ�
    UINT32                  ulCellID;       ///< ����û�и��ֶ�
}TRAIN_UE_INFO_T;

/// ��Ӧ���ݿ�� portalue�� �����ֳ�̨��Ϣ
typedef struct
{
    UINT32                  ulUserID;       ///< ��̨�ĺ������̺���
    CHAR                    acName[32];     ///< ��̨����
    UINT32                  ulType;         ///< ��̨����(������С��)
    UINT32                  ulFuncType;     ///< ���ܷ���
    UINT32                  ulStationID;    ///< ������վ ID

    UINT32                  ulDevStatus;    ///< ����û�и��ֶ�
    UINT32                  ulCallStatus;   ///< ����û�и��ֶ�
    UINT32                  ulCellID;       ///< ����û�и��ֶ�

    DOUBLE                  dLon;
    DOUBLE                  dLat;
}PORTAL_UE_INFO_T;

typedef struct
{
    INT64                   i64GroupID;         // ��ID
    CHAR                    acGroupName[64];
    UINT32                  ulGroupType;
    UINT32                  ulFuncType;
    UINT32                  ulPriority;
    UINT32                  ulDCUserID;         // �����ߣ�DC�û���
}GROUP_INFO_T;

/// ��Ӧ���ݿ�� depot
typedef struct
{
    UINT32                  ulDepotID;          // ������0��ʼ, ������ ID
    UINT32                  ulTrainDirection;   ///< �г��н�����, 0: δ֪�� 1: ���У� 2�� ����
    CHAR                    acDepotName[32+1];  ///< ����������
}DEPOT_INFO_T;

typedef struct
{
    UINT32                  ulStationIndex; // ������0��ʼ
    UINT32                  ulStationID;
    CHAR                    acStationName[32+1];
    UINT32                  ulStationType;
    UINT32                  ulDepotID;
    UINT32                  ulInterLockStationID;
    INT64                   i64StationGroupID;      // ��վ�ڲ�ͨ����
}STATION_INFO_T;

typedef struct
{
    UINT32                  ulDestID;
    UINT32                  ulStationID;
    CHAR                    acDestName[64+1];
}DEST_INFO_T;

/// ��Ӧ���ݿ�� brdzoneinfo�� �㲥���� ID
typedef struct
{
    INT64                   i64GroupID;         ///< �㲥�� ID
    UINT32                  ulStationID;        ///< ��վ ID
    UINT32                  ulZoneID;           ///< ���� ID
    CHAR                    acZoneName[32+1];   ///< ��������
}BRD_ZONE_INFO_T;

#define TRAIN_TYPE_NORMAL           1   // ��ͨ��
#define TRAIN_TYPE_ENGINE           2   // ���̳�
#define TRAIN_TYPE_OPERATE          3   // ��ҵ��

#define TRAIN_STATUS_ONLINE         0   // ����
#define TRAIN_STATUS_OFFLINE        1   // ����
#define TRAIN_STATUS_SILENT         2   // ��Ĭ��������

#define UE_STATUS_ONLINE            0   // ����
#define UE_STATUS_OFFLINE           1   // ����

// APP�ϱ�������̨����Ϣ
typedef struct
{
    UINT32      ulTrainUnitID;
    UINT32      ulTC;               // 1����ͷ other:��β
    UINT32      ulUserID;
    CHAR        acTrainID[16];
    INT64       i64CurrentGroupID;
    UINT32      ulStationID;
    UINT32      ulCoverStatus;  // COVER_STATUS_OPEN

    BOOL        bIsIPHOnline;
    UINT32      ulIPHStatus;

    BOOL        bIsPAOnline;
    UINT32      ulPAStatus;
    BOOL        bIsMasterPA;    // ����״̬

    UINT32      ulDriverID;
    UINT32      ulCellID;
    UINT32      ulCallType;
    INT64       i64CallID;
    UINT32      ulCurrentTime;
    UINT32      ulUptime;
    CHAR        acSerialNumber[64];
    CHAR        acAPPVer[64];
}APP_TRAIN_INFO_T;

typedef struct
{
    UINT8       aucIPHAddr[4];
    UINT32      ulMasterStatus;
    UINT32      ulIPHStatus;
    UINT16      ulIPHRecvAudioPort;
}IPH_AUDIO_NET_INFO_T;

typedef struct
{
    UINT32                  ulTrainUnitID;          // �����
    UINT32                  ulTrainType;            // ��ͨ�������̳�
    CHAR                    acTrainName[16];
    INT64                   i64DefaultGroupID;
}TRAIN_T;

typedef struct
{
    UINT32                  ulUptime;               // ��ȡ��¼��uptime
    UINT32                  ulTimeDiff;             // ��ȡ��¼��ʱ���뵱ǰʱ��Ĳ�
    UINT32                  ulTrainUnitID;          // �����(�����)
    UINT32                  ulTrainType;            // ��ͨ�������̳�
    CHAR                    acTrainName[16];
    CHAR                    acTrainServiceNumber[16];   // �����
    CHAR                    acTrainSerialNumber[16];    // ���к�
    CHAR                    acTrainID[16];          // ���κ�
    UINT32                  ulDriverID;             // ˾����
    UINT32                  ulComeTime;             // ��վʱ�䣨վ��֮���л�ʱ�䣩
    UINT32                  ulParkTime;             // ��վʱ��
    UINT32                  ulLeaveTime;            // ��վʱ��
    CHAR                    acDestID[8];
    UINT32                  ulRollingStockNum;      // ���ڱ���
    UINT32                  ulDirection;            // �����з���
    UINT32                  ulDriveMode;            // ��ʻģʽ
    UINT32                  ulStationID;            // ��ǰ��վ
    UINT32                  ulUpsideStationID;      // �г����в೵վ�ı��(�վ����ĳ�վ��������ʱΪ�����α�ţ��г�ͣ�ڳ�վʱ��ͬ��վ�ţ�
    UINT32                  ulDownsideStationID;    // �г����в೵վ�ı��(������ĳ�վ��������ʱΪ�����α�ţ��г�ͣ�ڳ�վʱ��ͬ��վ�ţ�
    UINT32                  ulTurnbackTrackFlag;
    UINT32                  ulDepotTrackFlag;       // �Ƿ��ڴ泵���ʶ
    UINT32                  ulPlatformID;           // ����վ̨ʱΪ0
    UINT32                  ulTransferTrackFlag;

    UINT32                  ulInOutDepot;           // Ĭ��ֵΪ0����ͣ����/�����ν���ת����ʱֵΪ1��
                                                    // ���г���ת�������ͣ����/������ʱֵΪ2������5�룩
    UINT32                  ulLostRealLocation;
    CHAR                    acTrackName[16];

    UINT16                  usSectionID;            // �г�������������ID
    UINT8                   ucIndexInSection;       // ͬһ�����������г��Ĵ���, ȡֵ��0��ʼ
    INT32                   iTrainOrder;            // �г���Գ�վ��λ��

    INT32                   iTrainStationPos;       // �г���Գ�վ��״̬
    INT32                   iTrainStationPosIndex;  // �г���Գ�վ��λ����ͬʱ�����Ⱥ�˳�򣬽�վΪ-2��-1���ڳ�վΪ0����վΪ1��2

    INT64                   i64DefaultGroupID;      // �г�ȱʡͨ����

    BOOL                    bRegister;              // ע�ᡢע��״̬
    UINT32                  ulRegisterDepotID;      // ע���λ�ã�����/������/ͣ������

    DOUBLE                  dLon;                   // ����
    DOUBLE                  dLat;                   // γ��

    UINT32                  ulAppReportTime;        // APP�ϱ���ϢʱDIS��¼������uptime
    UINT32                  ulTrainStatus;
    IPH_AUDIO_NET_INFO_T    stIPHAudioNetInfo;
    APP_TRAIN_INFO_T        astAppTrainInfo[2];
}TRAIN_INFO_T;

typedef struct
{
    UINT32              ulTrainUnitID;
    BOOL                bRegister;
    UINT32              ulRegisterDepotID;
    BOOL                bAutoRegister;
    BOOL                bRegisterOver;
    UINT32              ulTime;
    UINT32              ulDCNum;    // ��ǰ��֪ͨע��/ע����DC
    UINT32              aulDCUser[MAX_DC_NUM];
}REGISTER_INFO_T;

typedef struct
{
    BOOL        bSynchronization;
    UINT32      ulNtpPort;
    CHAR        acIPAddr[32];
    UINT32      ulPeriod;
    UINT32      ulServerTime;
    BOOL        bNtpServerValid;
}NTP_INFO_T;

typedef struct
{
    UINT8       aucNTPAddr[4];
    UINT16      usNTPPort;
    UINT32      ulNTPSyncPeriod;
}MRS_NTP_CFG;

typedef struct
{
    UINT32      ulTime;
    UINT32      ulTrainUserID;
    UINT32      ulTrainUnitID;
    UINT32      ulCellID;
    UINT32      ulStationID;
    UINT32      ulUpsideStationID;
    UINT32      ulDownsideStationID;
    UINT32      ulTurnbackTrackFlag;
    UINT32      ulTransferTrackFlag;
    UINT32      ulDirection;
}CELL_INFO_T;

//---------------------------- UE -------------------------------------------------
/// ��Ӧ���ݿ��еı� appcfg
typedef struct
{
    BOOL        bPTTAllow;                  ///< ����̨ PTT ��ť���������
    UINT32      ulBroadcastTimeout;         ///< �㲥��ʱʱ��
    UINT32      ulBroadcastVolume;          ///< �㲥����
    UINT32      ulHangoffTime;              ///< ���г�ʱ�Զ��Ҷ�ʱ��

    CHAR        acNTPServerAddr[16];        ///< NTP �����IP��ַ
    UINT32      ulNTPServerPort;            ///< NTP ����˶˿�
    UINT32      ulNTPSyncPeriod;            ///< NTP ��ͬ�����ڣ���λ��

    UINT32      ulFXAutoRebootPeriod;       ///< �̶�̨����������룩
    UINT32      ulFXAutoRebootTime;         ///< ����ʱ�̣���1��ʾ1����
}APP_BASIC_CFG_T;

// Ԥ��������豸����
#define SDS_TEMPLATE_TX             1   // ����̨
#define SDS_TEMPLATE_FX             2   // �̶�̨
#define SDS_TEMPLATE_ALL            3

/// ��Ӧ���ݿ� predefinedsms �� Ԥ�������Ϣ
typedef struct
{
    UINT32      ulSMSID;                // Ԥ������� ID
    UINT32      ulSDSType;              // Ԥ�����������
    UINT32      ulSMSTemplateType;      // Ԥ��������豸����
    CHAR        acSMSInfo[128];         // Ԥ������Ϣ����
}PREDEFINED_SMS_INFO_T;

// Ԥ�����������
#define SDS_TYPE_NULL                   0
#define SDS_TYPE_FORMAL_DISPATCH        1   // ��������
#define SDS_TYPE_ORAL_DISPATCH          2   // ��ͷ����
#define SDS_TYPE_OPERATE                3   // ��Ӫ��Ϣ����
#define SDS_TYPE_STATUS                 7   // ״̬����
#define SDS_TYPE_EMERG_STATUS           8   // ����״̬����
#define SDS_TYPE_NORMAL                 9   // ��ͨ��Ϣ
#define SDS_TYPE_SMS                    11  // ����

/**
 * @brief           ��Ӧ���ݿ�� sdstemplate, ����ģ��
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-16 11:09:22
 * @note
 */
typedef struct
{
    UINT32      ulTemplateID;               // ģ��ID
    UINT32      ulSDSType;                  // ��Ϣ����
    CHAR        acTitle[64];                // ģ�����
    CHAR        acSDSInfo[4000];            // ģ�����ݣ�����ģ��ؼ��֣�
}SDS_TEMPLATE_T;

typedef struct
{
    UINT32      ulFormalDispatchSDSIndex;   // ������������
    UINT32      ulOralDispatchSDSDay;       // ��ͷ��������
    UINT32      ulOralDispatchSDSIndex;     // ��ͷ��������

    UINT32      ulOperateSDSDay;            // ��Ӫ��Ϣ����
    UINT32      ulOperateSDSIndex;          // ��Ӫ��Ϣ�������
}SDS_INDEX_T;

typedef struct
{
    UINT32              ulSDSID;            // Ψһ���
    UINT32              ulSendUserID;       // ������
    CHAR                acSendUserName[32]; // ����������
    UINT32              ulSendTime;         // ����ʱ��
    UINT32              ulSDSIndex;         // ���1-100������Ӫ��Ϣ���
    UINT32              ulSDSType;          // ��Ϣ����
    CHAR                acFile[256];        // ����
    CHAR                acSDSInfo[4000];    // ��Ϣ����
    BOOL                bNeedReply;         // �Ƿ���Ҫ��ִ
    BOOL                bSendFlag;          // ����״̬��TRUE���Ѿ�����
    UINT32              ulRecvUserNum;      // �����߸���
    UINT32              ulRecvSuccUserNum;  // �ɹ������ߵĸ���
    UINT32              ulReplyUserNum;     // ȷ�ϵĸ���

    UINT32              ulRecvDCUserID;     // ������̨�յ��󣩵���̨ID
}SDS_INFO_T;

typedef struct
{
    UINT32              ulSDSID;            // SDSΨһ���
    UINT32              ulUserID;           // ������
    UINT32              ulRecvTime;         // ����ʱ��
    UINT32              ulSendResult;       // ����״̬��1�ɹ�
    UINT32              ulReplyTime;        // ��ִʱ��
}SDS_REPLY_T;

#define CDR_TYPE_P2P        1
#define CDR_TYPE_PTT        10

typedef struct
{
    UINT32          ulDCUserID;
    UINT32          ulTime;
    UINT32          ulCallType;
    INT64           i64GroupID;
    UINT32          ulCaller;
    UINT32          ulCallee;
    UINT32          ulStatus;
}CDR_INFO_T;

typedef struct
{
    INT64           i64GroupID;
    UINT32          ulGroupStatus;
    BOOL            bNeedSave;
}GROUP_STATUS_T;

typedef struct
{
    UINT32          ulSeqID;
    UINT32          ulCallType;
    UINT32          ulCaller;
    UINT32          ulCallee;
    UINT32          ulTime;
    UINT32          ulStatus;
}P2P_CALL_INFO_T, P2PVIDEO_CALL_INFO_T;

typedef struct
{
    UINT32          ulSeqID;
    UINT32          ulDCUserID;
    INT64           i64GroupID;
    UINT32          ulTime;
    UINT32          ulSpeaker;
    UINT32          ulCallType;
}GROUP_CALL_INFO_T;

typedef struct
{
    UINT32          ulReqTime;
    UINT32          ulReqCallType;
    UINT32          ulReqUserID;        // �г�����̨�����̶�̨����
    UINT32          ulCaller;           // ���𷽵��г�����Ż�վID

    UINT32          ulDCType;           // �������ͣ�ȱʡDC_TYPE_ALL

    UINT32          ulDCUserID;
    UINT32          ulAcceptTime;
}REQ_CALL_INFO_T;

/// ��Ӧ���ݿ�� recbrd ��Ԥ¼���㲥��Ϣ
typedef struct
{
    UINT32              ulRecID;        // ¼��ID
    CHAR                acRecInfo[512]; // ¼������
}REC_BRD_CFG_T;

typedef struct
{
    UINT32              ulBrdSeqID;        // �㲥���
    UINT32              ulTime;
    UINT32              ulRecID;
    CHAR                acRecInfo[512]; // ¼������
    UINT32              ulBrdLoopTime;
    UINT32              ulDCUserID;
    CHAR                acTrainList[1024];
}REC_BRD_INFO_T;

typedef struct
{
    UINT32              ulBrdSeqID;
    UINT32              ulTrainUnitID;
    UINT32              ulReplyTime;
    INT32               iResult;
}REC_BRD_REPLY_T;

#define REC_BRD_RESULT_NULL     (-1)
#define REC_BRD_RESULT_FAIL     0
#define REC_BRD_RESULT_SUCC     1

typedef struct
{
    UINT32              ulBrdSeqID;
    UINT32              ulStartTime;
    UINT32              ulEndTime;
    UINT32              ulDCUserID;
    CHAR                acTrainList[1024];
}REAL_BRD_INFO_T;

// IPH�Խ�״̬
//#define IPH_STATUS_NONE         0
#define IPH_STATUS_WAIT         1
#define IPH_STATUS_TALK         3
//#define IPH_STATUS_PREVIEW      4
#define IPH_STATUS_OVER         5
#define IPH_STATUS_WAIT_ACK     6
#define IPH_STATUS_ACK_ERR      0xff

typedef struct
{
    UINT32      ulApplyUptime;
    UINT32      ulApplyTime;
    UINT32      ulTrainUnitID;
    CHAR        acTrainID[16];
    UINT32      ulCarriageID;
    UINT32      ulIPHDevDoorID;
    UINT32      ulIPHDevID;
    UINT32      ulAcceptDCUserID;
    UINT32      ulAcceptTime;
    UINT32      ulGetACKTime;
    UINT32      ulStopTime;
    UINT32      ulStopUptime;
    UINT32      ulStatus;
    CHAR        acVideoURL[64];
    UINT8       aucIPHDevIP[4];
}IPH_TALK_INFO_T;

typedef struct
{
    UINT32      ulDCUserID;
    INT64       i64GroupID;
}SUB_GROUP_T;

typedef struct
{
    UINT32      ulDCUserID;
    UINT32      ulUserID;
}SUB_USER_T;

// ����̨��Ϣ
typedef struct
{
    UINT32      ulDevID;
    CHAR        acName[32];
    CHAR        acIPAddr[32];
    CHAR        acMacAddr[32];
    CHAR        acGateway[32];
}DC_DEV_INFO_T;

typedef struct
{
    DC_DEV_INFO_T   stDevInfo;

    BOOL        bOnline;
    CHAR        acVersion[32];
    UINT32      ulCpuUsage;
    UINT32      ulMemUsage;
    UINT32      ulDiskUsage;
    UINT32      ulSysUptime;
}DC_OMA_INFO_T;

// ¼��¼���û�
#define REC_OPER_TYPE_ADMIN     1   // �����û�
#define REC_OPER_TYPE_NORMAL    2   // ��ͨ�û�
#define REC_OPER_TYPE_QUERY     3   // ��ѯ�û�

typedef struct
{
    CHAR                    acOperatorID[32];
    CHAR                    acName[32];
    CHAR                    acPassword[32];
    UINT32                  ulOperType;
}REC_OPER_INFO_T;

#define REC_CALL_TYPE_ALL               0

#define REC_CALL_TYPE_P2P_AUDIO         1   // ����
#define REC_CALL_TYPE_P2P_VIDEO         2   // ��Ƶ����
#define REC_CALL_TYPE_VIDEO_UPLOAD      3   // ��Ƶ�ϴ�

#define REC_CALL_TYPE_PTT_AUDIO         11  // ���
#define REC_CALL_TYPE_PTT_VIDEO         12  // ��Ƶ���

#define REC_CALL_TYPE_IPH               50  //
#define REC_CALL_TYPE_IPH_VIDEO         51  // ��������Ƶ
#define REC_CALL_TYPE_IPH_AUDIO         52  // ֻ������Ƶ

#define REC_CALL_TYPE_REAL_BRD          60  // ʵʱ�㲥

// ¼��¼���¼
typedef struct
{
    CHAR        acFile[128];
    CHAR        acFileSuffix[8];
    UINT32      ulTime;      // ¼���ļ�����ʱ��
    DOUBLE      dDuration;   // ¼���ļ�ʱ�䳤�ȣ���λ�룩
    UINT32      ulFileSize;  // ¼���ļ���С����λ�ֽڣ�
    UINT32      ulFileType;  // �ļ�����
    UINT32      ulCallType;  // ��������

    UINT32      ulTranUnitID;
    UINT32      ulCaller;    // ����
    UINT32      ulCallee;    // ����
    UINT32      ulDCUserID;
    INT64       i64GroupID;
}REC_FILE_INFO_T;

// GPS ��Ϣ
typedef struct
{
    UINT32                      ulDevID;    // �豸Ψһ��ʶ������UserID��
    UINT32                      ulTime;     // ��׼ʱ��
    DOUBLE                      dLon;       // ����
    DOUBLE                      dLat;       // γ��
    FLOAT                       fHeight;    // �߶�
    FLOAT                       fAzimuth;   // ��λ��
    FLOAT                       fSpeed;     // �ٶ�

    UINT8                       ucEastLon;  // �Ƿ񶫾�
    UINT8                       ucNorthLat; // �Ƿ�γ
}GPS_INFO_T;

// ��ѡ����Ϣ
typedef struct
{
    UINT32      ulDCUserID;         // ������
    INT64       i64GroupID;         // Ⱥ��ID
    CHAR        acGroupName[64];    // ����
    CHAR        acGroupMember[1024];// ���Ա�����30����
}MULTI_GROUP_INFO_T;

// OCC�㲥����
#define OCC_PA_TYPE_NORMAL         1    // ��ͨ�г��㲥
#define OCC_PA_TYPE_MAINLINE       2    // �����г��㲥
#define OCC_PA_TYPE_UPLINE         3    // �����г��㲥
#define OCC_PA_TYPE_DOWNLINE       4    // �����г��㲥

// OCCʵʱ�㲥����
#define OCC_REAL_PA_TYPE_NONE            0  // ��
#define OCC_REAL_PA_TYPE_PCM             1  // ����������
#define OCC_REAL_PA_TYPE_TRUNK           2  // ��Ⱥģʽ

#pragma pack(pop)

#endif

