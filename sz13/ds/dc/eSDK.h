//---------------------------------------------------------------------------

#ifndef eSDKH
#define eSDKH
//---------------------------------------------------------------------------

typedef VOID (__stdcall * eSDK_EventCallBack)(INT32 iEventType, VOID* pEventBuf, UINT32 uiBufSize, VOID* pUserData);


#define DC_PRIV_BREAK_OFF_CALL      1   // ǿ��
#define DC_PRIV_BREAK_IN_CALL       2   // ������
#define DC_PRIV_DISCREET_LISTEN     4   // ���ܼ�����
#define DC_PRIV_RECV_VIDEO          8   // ��Ƶ��ء�
#define DC_PRIV_EXPORT_SDS          16  // ����Ϣ������
#define DC_PRIV_ENV_LISTEN          32  // ����������
#define DC_PRIV_VIDEO_WALL          64  // ��Ƶ��ǽ��

#define DC_ROLE_ADMIN               0   // ����Ա��
#define DC_ROLE_NORMAL_DC           1   // ��ͨ����Ա
#define DC_ROLE_UPPER_DC            2   // �ϼ�����Ա

typedef struct
{
    UINT32      ulPriviledge;   // ����̨��Ȩ
    UINT32      ulRole;         // ��ɫ
    CHAR        acAlias[64];    // ����̨����
}CURR_DC_INFO_T;

#define REC_TYPE_P2P_AUDIO      0   // �������
#define REC_TYPE_P2P_VIDEO      1   // ��Ƶ���
#define REC_TYPE_VIDEO_UPLOAD   2   // ��Ƶ�ش�
#define REC_TYPE_PTT            3   // Ⱥ�����
#define REC_TYPE_IPH            50  // �˿ͶԽ�

typedef struct
{
    UINT32      ulRecType;
    UINT32      ulCaller;
    UINT32      ulCallee;
    INT64       i64ResourceID;
    UINT32      ulStartTime;
    UINT32      ulEndTime;
    AnsiString  strHttpUrl;     // ����¼��¼��ĵ�ַ
    AnsiString  strRtspUrl;     // �鿴¼��¼�����ý������
}ESDK_REC_FILE_INFO_T;

typedef struct
{
    INT32       iEventType;
    UINT32      ulMsgLen;
    CHAR        acMsg[4];       // ��Ҫ��̬����ʵ���ڴ�
}ESDK_EVENT_T;

// �û�״̬
#define USER_STATUS_UNKNOWN             4000    // δ֪״̬��
#define USER_STATUS_REGOK               4011    // ��Դע��ɹ���
#define USER_STATUS_REGFAIL             4012    // ��Դע��ʧ�ܣ�
#define USER_STATUS_INVITING            4020    // ��Դ���ں�����
#define USER_STATUS_RINGING             4021    // ��Դ������������
#define USER_STATUS_TXMTING             4022    // ��Դ����ͨ������ת�ƺ����У�
#define USER_STATUS_TXMTIDLE            4023    // ��Դ����
#define USER_STATUS_HALFVOICE_ACTIVATED 4040    // ��˫��ͨ����
#define USER_STATUS_HALFVOICE_CLOSED    4041    // ��˫��ͨ���ر�

// �������ͣ�
#define USER_CALL_TYPE_UNREG            0       // δע��
#define USER_CALL_TYPE_REG              1       // ��ע��
#define USER_CALL_TYPE_VOICEP2P         2       // ��Ƶ���
#define USER_CALL_TYPE_VIDEOP2P         3       // �������
#define USER_CALL_TYPE_UPLOADP2P        4       // ����ϴ�
#define USER_CALL_TYPE_VIDEODISPATCH    5       // ��Ƶ�ַ�
#define USER_CALL_TYPE_VIDEOONWALL      6       // ��Ƶ��ǽ
#define USER_CALL_TYPE_VOICEPTT         7       // ��Ƶ���
#define USER_CALL_TYPE_ENVLISTEN        8       // P2P��������

// ��Ƶ��ʽ
#define VIDEO_FMT_CIF       0
#define VIDEO_FMT_QCIF      1
#define VIDEO_FMT_D1        2
#define VIDEO_FMT_720P      3
#define VIDEO_FMT_1080P     4

#define CAMERA_TYPE_FRONT       0   // ǰ������ͷ
#define CAMERA_TYPE_BACK        1   // ��������ͷ

#define VIDEO_VOICE_OPEN        0   // ��Ҫ����
#define VIDEO_VOICE_MUTE        1   // ����Ҫ����

#define VIDEO_CALL_CONFIRM_NONE 0   // ����Ҫ�û�ȷ��
#define VIDEO_CALL_CONFIRM_NEED 1   // ��Ҫ�û�ȷ��


//<Content><UserID>7007</UserID><UserName></UserName><StatusType>16</StatusType><StatusValue>4011</StatusValue><AttachingGroup>0</AttachingGroup></Content>
typedef struct
{
    UINT32  ulUserNumber;           // �û��ն˺���

    UINT32  ulStatusType;           // ״̬���ͣ��̶�ֵ��
    UINT32  ulStatus;               // ״̬
}EVENT_USER_STATUS_T;

#define P2P_IND_STATUS_RECEIVED                 2002 // ָʾ����̨�յ�������� ��SDK���Զ��������������ϲ�Ӧ����Ҫ������� �������û�������ܽӣ�
#define P2P_IND_STATUS_ANSWERED                 2003 // ָʾ����̨��������ʱ���Զ��ѽ��� �����ѷ���200 OK��Ϣ��ʱ����̨��ʼ����ý����RTP����
#define P2P_IND_STATUS_EMERGENCY                2004 // ָʾ����̨�յ������������� ��SDK���Զ��������ϲ�Ӧ�ÿ���ͨ������ȷ�ʽ�����ѵ���Ա��
#define P2P_IND_STATUS_PROCEEDING               2005 // ָʾ����̨�յ�SIP 100�� SIP 101��
#define P2P_IND_STATUS_RINGING                  2006 // ָʾ����̨�յ�SIP 180 ��SDK���Զ����Ż��������ϲ�Ӧ����Ҫ������� �������û�����ȡ�����У�
#define P2P_IND_STATUS_ACKED                    2007 // ָʾ����̨��������ʱ���յ��Զ˵�ACK��Ӧ ������ʼ����ý��������
#define P2P_IND_STATUS_RELEASED                 2008 // ָʾ�Զ˾ܾ�����
#define P2P_IND_STATUS_HANGUPED                 2009 // ָʾ�Զ��ѹҶ�
#define P2P_IND_STATUS_HANGUPED_ACTIVE          2010 // ָʾ�������Ҷ�
#define P2P_IND_STATUS_CANCELLED                2011 // ָʾ���������ȡ��
#define P2P_IND_STATUS_PREMPTED                 2012 // ָʾ����ѱ�ȡ��
#define P2P_IND_STATUS_RELEASED_BUSY            2013 // ָʾ�Զ�æ
#define P2P_IND_STATUS_BREAKIN_FAIL             2014 // ָʾ��Ȩʧ��
#define P2P_IND_STATUS_BREAKIN_SUCCESS          2015 // ָʾ��Ȩ�ɹ�
#define P2P_IND_STATUS_PROHIBITED               2016 // ָʾ��Ȩ��
#define P2P_IND_STATUS_REMOTE_NOANSWER          2017 // ָʾ�Զ���Ӧ��
#define P2P_IND_STATUS_REMOTE_NOTFOUND          2018 // ָʾ�Զ�δ�ҵ�
#define P2P_IND_STATUS_LICENSE_LIMIT            2019 // ָʾ֤������
#define P2P_IND_STATUS_MEDIAPLANE_ERROR         2020 // ָʾ��Ƶ����֯ʧ��
#define P2P_IND_STATUS_MEDIAPLANE_MUTE          2021 // ָʾ��Ƶ������
#define P2P_IND_STATUS_MEDIAPLANE_UNMUTE        2022 // ָʾ��Ƶ��ȥ����
#define P2P_IND_STATUS_REMOTE_NOTSUPPORTED      2023 // ָʾԶ�˲�֧��Ҫ�����Ƶ��ʽ
#define P2P_IND_STATUS_REMOTE_NOTCONNECTED      2024 // ָʾԶ��δע��
#define P2P_IND_STATUS_TRANSFER_SUCCESS         2025 // ָʾ����ת�ӳɹ�
#define P2P_IND_STATUS_TRANSFER_FAIL            2026 // ָʾ����ת��ʧ��
#define P2P_IND_STATUS_TRANSFER                 2027 // ָʾ����ת��
#define P2P_IND_STATUS_SIGNAL_ERROR             2028 // ָʾSIP����
#define P2P_IND_STATUS_EMERGENCY_BEGIN          2029 // ָʾ�յ���Զ���ն˵�һ·��������
#define P2P_IND_STATUS_RELEASED_POWEROFF        2033 // Զ�̶ϵ�
#define P2P_IND_STATUS_PICKEDUP                 2040 // ָʾ���ж��ѽ���
#define P2P_IND_STATUS_HALFDUPLEX_BEGIN         2041 // ָʾ�յ���˫���������
#define P2P_IND_STATUS_TX_BEGIN                 2042 // ָʾ��˫�������ʼ
#define P2P_IND_STATUS_TX_IDLE                  2043 // ָʾ��˫���������
#define P2P_IND_STATUS_TX_VOICE                 2044 // ָʾ��˫�������Ƶ
#define P2P_IND_STATUS_FC_ACCEPTED              2045 // ָʾ��˫�����������
#define P2P_IND_STATUS_FC_REJECTED              2046 // ָʾ��˫��������ܾ�
#define P2P_IND_STATUS_ENVIR_ANSWERED           2047 // ָʾ����������Ӧ��
#define P2P_IND_STATUS_CALLER_UNSUPPORT         2048 // ָʾ���з��𷽲���֧��
#define P2P_IND_STATUS_CALLEE_UNSUPPORT         2049 // ָʾ�����з�����֧��
#define P2P_IND_STATUS_CIPHER_NOT_AVAILABLE     2050 // ָʾ���Ӿ���
#define P2P_IND_STATUS_MISCELLANEOUS_WARNING    2051 // ָʾ�Զ���Ȩ��
#define P2P_IND_STATUS_REMOTE_PROHIBITED        2052 // ָʾ�Զ���Ȩ��
#define P2P_IND_STATUS_EMERGENCY_WAIT           2053 // ���������������ڵȴ�����
#define P2P_IND_STATUS_RELEASED_GENERAL_ERROR   2054 // ������ͨ����

// resource ��Դ״̬����
#define RESOURCE_STATUS_TYPE_REG_PROXY          15      // ��Դע��״ָ̬ʾ
#define RESOURCE_STATUS_TYPE_USERDGNA           21      // �û���̬����״̬��
#define RESOURCE_STATUS_TYPE_VIDEODISPATCH      22      // ��Ƶ�ַ�״̬��
#define RESOURCE_STATUS_TYPE_MUTE               23      // ����״̬
#define RESOURCE_STATUS_TYPE_GRPPATCH           24      // �ɽӲ���״̬

// resource ��Դ״̬
#define RESOURCE_STATUS_UNKNOWN                 4000    // δ֪״̬
#define RESOURCE_STATUS_ASSIGNED                4003    // ��Դָ��״̬
#define RESOURCE_STATUS_DEASSIGNED              4004    // ��Դȥָ��״̬
#define RESOURCE_STATUS_LISTENING               4005    // ��Դ������״̬
#define RESOURCE_STATUS_LISTENEND               4006    // ��Դ��������״̬
#define RESOURCE_STATUS_REGOK                   4011    // ��Դע��ɹ�
#define RESOURCE_STATUS_REGFAIL                 4012    // ��Դȥע��
#define RESOURCE_STATUS_FORBIDEN                4013    // ��Դδ��Ȩ
#define RESOURCE_STATUS_PASSWORD_WRONG          4014    // �û��������������
#define RESOURCE_STATUS_TEMP_UNAVAILABLE        4015    // ��Դ��ʱ������
#define RESOURCE_STATUS_RES_CONFILCT            4016    // ��Դ��ͻ
#define RESOURCE_STATUS_UNAUTHORIZED            4017    // δ��Ȩ
#define RESOURCE_STATUS_LICENSE_LIMIT           4018    // License����
//#define RESOURCE_STATUS_PASSWORD_WRONG          4019    // �˺Ż��������
#define RESOURCE_STATUS_DGNAOK                  4024    // ��Դ��̬����ɹ�
#define RESOURCE_STATUS_DGNAFAIL                4025    // ��Դ��̬����ʧ��
#define RESOURCE_STATUS_DGNA_UEFAIL             4026    // ��Դ��̬��������û��豸ʧ��
#define RESOURCE_STATUS_GROUP_ACTIVATED         4027    // ��ԴȺ����Ч
#define RESOURCE_STATUS_GROUP_DEACTIVATED       4028    // ��ԴȺ����Ч
#define RESOURCE_STATUS_MUTE                    4029    // ִ�о���
#define RESOURCE_STATUS_UNMUTE                  4030    // ȡ������
#define RESOURCE_STATUS_GRPPATCH_CREATEOK       4031    // �ɽ��鴴���ɹ�
#define RESOURCE_STATUS_GRPPATCH_CREATEFAIL     4032    // �ɽ��鴴��ʧ��
#define RESOURCE_STATUS_GRPPATCH_CANCELOK       4033    // �ɽ���ɾ���ɹ�
#define RESOURCE_STATUS_GRPPATCH_CANCELFAIL     4034    // �ɽ���ɾ��ʧ��
#define RESOURCE_STATUS_GRPPATCH_ADDOK          4035    // �ɽ������ӳ�Ա�ɹ�
#define RESOURCE_STATUS_GRPPATCH_ADDFAIL        4036    // �ɽ������ӳ�Աʧ��
#define RESOURCE_STATUS_GRPPATCH_DELOK          4037    // �ɽ���ɾ����Ա�ɹ�
#define RESOURCE_STATUS_GRPPATCH_DELFAIL        4038    // �ɽ���ɾ����Աʧ��
#define RESOURCE_STATUS_ACCOUNT_LOCKED          4060    // �˺ű�����

typedef struct
{
    INT64       i64ResourceID;
    CHAR        acResourceName[64];
    UINT32      ulStatusType;           // ״̬���ͣ��̶�ֵ��
    UINT32      ulStatusValue;          // ״̬

    INT64       i64AttachingGroupID;    // �û���ǰ��������
    CHAR        acSpeaker[64];          // ������
    UINT32      ulCameraID;             // ���ַ�����ԴID
}EVENT_RESOURCE_STATUS_T;

typedef struct
{
    UINT32  ulP2PCallStatus;        // ���״̬

    UINT32  ulCaller;               // ����
    UINT32  ulCallee;               // ����
    UINT32  ulCallID;               // ����
}EVENT_P2PCALL_STATUS_T;

// ���״̬��
#define PTT_IND_STATUS_UNKNOWN              1000    // ������
#define PTT_IND_STATUS_RECEIVED             1001    // ������󱻽��ա�
#define PTT_IND_STATUS_REQUEST              1002    // ����
#define PTT_IND_STATUS_ACCEPT               1003    // ���֮���յ����Ȼ�200 OK��Ϣ��֪ͨ��Ȩ�����״��Ϊ���ա�
#define PTT_IND_STATUS_WAITING              1004    // ����
#define PTT_IND_STATUS_REJECT               1005    // ���֮���յ����Ȼ�200 OK��Ϣ��֪ͨ��Ȩ�����״��Ϊ�ܾ���
#define PTT_IND_STATUS_SNATCH               1006    // ��������ɹ���
#define PTT_IND_STATUS_HANGUP_OK            1007    // ����̨�û��뿪���
#define PTT_IND_STATUS_CLOSE_OK             1008    // ����������յ����Ȼ�BYE��Ϣ����
#define PTT_IND_STATUS_CANCEL_OK            1009    // �������ʧ�ܣ��յ����Ȼ�CANCEL��Ϣ����
#define PTT_IND_STATUS_PREMPTED             1010    // ������滻��
#define PTT_IND_STATUS_TX_BEGIN             1011    // ���֮���յ����Ȼ�INFO��Ϣ��֪ͨ��Ȩ�����
#define PTT_IND_STATUS_TX_IDLE              1012    // ���֮���յ����Ȼ�INFO��Ϣ��֪ͨ��Ȩ���С�
#define PTT_IND_STATUS_MEDIAPLANE_ERROR     1013    // �������ʧ��
#define PTT_IND_STATUS_EMERGENCY_BEGIN      1014    // �յ�һ·�������
#define PTT_IND_STATUS_EMERGENCY_UPDATE     1015    // ���ת��Ϊ�������
#define PTT_IND_STATUS_LICENSE_LIMIT        1016    // ֤������
#define PTT_IND_STATUS_SIGNAL_ERROR         1017    // SIP����

typedef struct
{
    INT64   i64GroupID;             // ��ID
    BOOL    bIsEmergencyCall;       // �Ƿ��������
    BOOL    bClosed;
    UINT32  ulCallType;
    UINT32  ulGroupCallStatus;      // ���״̬
    UINT32  ulSpeaker;              // ������
    UINT32  ulLocalPort;            // ���ض˿�
    UINT32  ulRemotePort;           // Զ�˶˿�
    CHAR    acRemoteIP[64];         // Զ��IP
    INT32   iSoundType;             // ��Ƶ��ʽ��
    CHAR    acSpeakerName[64];      // ����������
    UINT32  ulSpeakTime;
}EVENT_GROUP_STATUS_T;

typedef struct
{
    INT64       i64GroupID;
    UINT32      ulPatchIndex;
    CHAR        acGroupName[64];
    INT32       iDCUserID;  ///< �����ɽ���ĵ���̨ID
}EVENT_CREATE_PATCH_GROUP_T;


#define SDS_TYPE_TEXT_NORMAL    "0001"
#define SDS_TYPE_TEXT_SMS       "0004"

typedef struct
{
    UINT32      ulSDSType;
    CHAR        acContent[1024];    // ��Ϣ����
    CHAR        acFrom[32];         // ������
//  CHAR        acTo[32];           // �ռ���
    UINT32      ulMsgState;         // ����״̬, 0������ 1�����ߡ�
    INT64       i64EmergGroupID;    // ����״̬��Ⱥ��ID��
    UINT32      ulEmergUserID;      // ����״̬���ն�ID��
    INT64       i64GroupID;         // Ⱥ��ID��
    CHAR        acFile[256];
    CHAR        acSubject[128];     // ��Ϣ����
    CHAR        acDirection[32];    // �ӷ����ű�־��
    CHAR        acSendDate[32];     // ��Ϣ�������ڡ�
    CHAR        acSendTime[32];     // ��Ϣ����ʱ�䡣
    //StatusCode ���ŵ�״̬�루��ͨ���Ų����ã�
}EVENT_SDS_REPORT_T;

#define SDS_SEND_SUCC                   0       // ���ͳɹ�
#define SDS_SEND_NO_ACK                 0xff    // ���ն�û��ACK
#define SDS_SEND_FAIL_SERVICE_UNSUPORT  1       // ҵ��֧�֣���Է���֧�ּ��ܶ��š�
#define SDS_SEND_FAIL_VPN_LIMIT         2       // VPNȨ�޷Ƿ�
#define SDS_SEND_FAIL_TARGET_NOT_FOUND  3       // �Է����벻����

typedef struct
{
    CHAR        acReceiver[32];
    CHAR        acMsgID[32];
    BOOL        bSendFlag;          // true
    UINT32      ulRetCode;
}EVENT_SDS_SEND_STATUS_T;


// Ⱥ��
#define GROUP_TYPE_ALLBROADCAST     0   // ȫ���㲥��
#define GROUP_TYPE_GENERAL          1   // ��ͨ��
#define GROUP_TYPE_AREABROADCAST    2   // ����㲥��
#define GROUP_TYPE_EXTERNAL         8   // �ⲿ��
#define GROUP_TYPE_DYNAMIC          9   // ��̬��
#define GROUP_TYPE_ALLTYPE          10  // ������

typedef struct
{
    INT64       i64GroupID;         // Ⱥ��ID
    CHAR        acGroupName[64];    // ����
    UINT32      ulPriority;         // ���ȼ���ȡֵ��ΧΪ 1~15��ֵԽС���ȼ�Խ��
    UINT32      ulGroupType;        // ����
    UINT32      ulGroupStatus;      // ����/����״̬
}DC_GROUP_INFO_T;

#define ESDK_MAX_PATCH_GROUP_MEMBER_NUM     20
#define ESDK_MAX_MULTI_GROUP_MEMBER_NUM     30

typedef struct
{
    INT64       i64GroupID;         // Ⱥ��ID
    CHAR        acGroupName[64];    // ����
    UINT32      ulPriority;         // ���ȼ���ȡֵ��ΧΪ 1~15��ֵԽС���ȼ�Խ��
    UINT32      ulPatchIndex;
    UINT32      ulDCUserID;

    UINT32      ulMemberNum;
    INT64       ai64MemberGroupID[ESDK_MAX_PATCH_GROUP_MEMBER_NUM];  // SDK�������20����Ա
}SDK_PATCH_GROUP_INFO_T;

typedef struct
{
    INT64       i64GroupID;         // Ⱥ��ID
    CHAR        acGroupName[64];    // ����
    UINT32      ulDCUserID;

    UINT32      ulMemberNum;
    INT64       ai64MemberGroupID[ESDK_MAX_MULTI_GROUP_MEMBER_NUM];  // SDK�������30����Ա
//    VECTOR<INT64>   vGroup;
}SDK_MULTI_GROUP_INFO_T;


#define P2PVIDEO_IND_STATUS_RECEIVED                3002    // �ϲ�Ӧ����Ϊ���У��յ��Զ˵�������ն���Ƶ�����
#define P2PVIDEO_IND_STATUS_ANSWERED                3003    // �ϲ�Ӧ����Ϊ���У��Զ��ѽ�������
#define P2PVIDEO_IND_STATUS_HANGUPED                3008    // ʵʱ��Ƶ�ѱ��Զ˹Ҷ�
#define P2PVIDEO_IND_STATUS_HANGUPED_ACTIVE         3009    // ʵʱ��Ƶ������̨�����Ҷϡ�
#define P2PVIDEO_IND_STATUS_CANCELLED               3010    // ʵʱ��Ƶ����ȡ��
#define P2PVIDEO_IND_STATUS_MONITOR                 3011    // �ϲ�Ӧ����Ϊ���У��յ��Զ���Ƶ�ش�����(�ն���Ƶ�ش�)
#define P2PVIDEO_IND_STATUS_RELEASED_BUSY           3013    // �Զ�æ
#define P2PVIDEO_IND_STATUS_REMOTE_NOTFOUND         3016    // δ�ҵ��Զ���Դ��
#define P2PVIDEO_IND_STATUS_REMOTE_NOTCONNECTED     3021    // �Զ�δע��
#define P2PVIDEO_IND_STATUS_RELEASED_GENERAL_ERROR  3052    // ������ͨ����
#define P2PVIDEO_IND_STATUS_PICKEDUP                3040    // ������ժ��������̨�����ն���Ƶ��

typedef struct
{
    UINT32  ulStatus;               // ״̬
    UINT32  ulCallee;               // ����
    UINT32  ulCaller;               // ����
    INT32   iCameraType;            // ����ͷ����
    UINT32  ulVideoFormat;          // ��Ƶ��ʽ
    BOOL    bIsP2PVideoDial;        // �Ƿ���Ƶ���
}EVENT_P2PVIDEO_STATUS_T;

#define GIS_REPORT_TYPE_PERIODIC    // 0�������ϱ���
#define GIS_REPORT_TYPE_EMERGCALL   // 1���ض��¼������������С�
#define GIS_REPORT_TYPE_EMERGOVER   // 2: �����¼�����
#define GIS_REPORT_TYPE_FALLDOWN    // 3: ���ظ澯����
#define GIS_REPORT_TYPE_FALLDOWN    // 4: ���ظ澯���
#define GIS_REPORT_TYPE_HISTORY     // 5: ��ʷλ���·�

#define GPS_STATUS_NORMAL       // 0������GPS����������
#define GPS_STATUS_STARFAIL     // 1��GPS����ʧ�ܡ�
#define GPS_STATUS_ERROR        // 2�����������쳣��
#define GPS_STATUS_UNREGISTER   // 3 : GPSδע��
#define GPS_STATUS_NORIGHT      // 4 : GPS�ϱ�Ȩ�޹ر�

typedef struct
{
    UINT32  ulUserID;       // �ϱ���
    UINT32  ulTime;         // ���һ���ϱ�ʱ�䡣
    DOUBLE  dAltitude;      // �߶ȡ�
    DOUBLE  dLatitude;      // γ�ȡ�
    DOUBLE  dLongtitude;    // ���ȡ�
    DOUBLE  dDirection;     // ����, ��λ��˳ʱ�뷽��Ķ���.
    UINT32  ulReportType;   // �¼����룺

    UINT32  ulCellID;           // С��ID
    UINT32  ulPLMN;
    UINT32  ulDeviceType;       // �豸����
    UINT32  ulLocatMode;        // ��λ��ʽ.
    UINT32  ulReportStatus;     // ����״̬

    DOUBLE  dSpeed;             // �ٶ�, ��λ m/s
}EVENT_GIS_REPORT_T;


#define USER_DISPATCHER      0      // ����̨�û�
#define USER_FIXEDCAMERA     1      // �̶�����ͷ
#define USER_PSTNUSER        2      // PSTN�û�
#define USER_TETRAUSER       3      // TETRA�û�
#define USER_PLMNUSER        4      // PLMN�ƶ��û�
#define USER_EXTERNALPTT     5      // �ⲿPTT�û�
#define USER_SDKUSER         6      // SDK�����ڲ��û�
#define USER_APPUSER         7      // ����APP�ն��û�
#define USER_UELOGINUSER     8      // �ն˵�¼�û�
#define USER_PTTUSER         9      // PTT�û�
#define USER_ALLTYPEUSER     50     // �����û�����������
#define USER_OTHERUSER       255    // ����δ�����û�


typedef struct
{
    UINT32      ulUserID;           // �û�ID
    UINT32      ulUserCategory;     // ����
    UINT32      ulUserPriority;     // ���ȼ���ȡֵ��ΧΪ 1~15��ֵԽС���ȼ�Խ��
    CHAR        acUserName[64];     // ����
}DC_INFO_T;

#define DC_MUTE_ALL         0   // ȫ������
#define DC_UNMUTE_ALL       1   // ����ȥ����
#define DC_MUTE_PTT         2   // ȫ���������
#define DC_UNMUTE_PTT       3   // ȫ�����ȥ�������ָ�������
#define DC_MUTE_ONE         4   // ��������
#define DC_UNMUTE_ONE       5   // ����ȥ����

// ¼����������
#define REC_TYPE_AUDIO      0   // ¼��
#define REC_TYPE_VIDEO      1   // ¼��

#define REC_CALL_TYPE_P2P       0   // ���
#define REC_CALL_TYPE_PTT       1   // ���
#define REC_CALL_TYPE_VIDEO     2   // ��Ƶ�ش�

// ������������
#define DC_MUTE_CALL_P2P    0   // ���
#define DC_MUTE_CALL_PTT    1   // ���
#define DC_MUTE_CALL_VIDEO  2   // ��Ƶ�ش�

// ����״̬
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

// ģ������
#define MODULE_TYPE_DC                  7   //  RTP_MODULE�� ý��ͨ��ģ��

// ģ��״̬
#define MODULE_STATE_KICK_OFF           15  // �˳�����̨֪ͨ����Ϊ����ͬ�˺ŵ�¼
#define MODULE_STATE_PWD_CHANGE         17  // �û������޸�
#define MODULE_STATE_DISCONNECT         18  // ������δ����
#define MODULE_STATE_RECONNECT          19  // ������������
#define MODULE_STATE_USER_DELETE        22  // �û���ɾ��

#define MAX_REAL_SDS_TEXT_LEN       1000    // ʵʱ������󳤶�
#define MAX_MM_SDS_TEXT_LEN         10240   // �����ı���󳤶�

typedef struct
{
    UINT32      ulModuleType;
    UINT32      ulModuleStatus;
}EVENT_MODULE_STATUS_T;

#define VWALL_STATUS_INIT       1       // ��ʼ��״̬����ʾ����
#define VWALL_STATUS_BUSY       4022    // �Ѿ�ռ��
#define VWALL_STATUS_IDLE       4023    // ����

typedef struct
{
    UINT32      ulVideoChanID;  // ��ƵǽID�б����Ƶͨ���š�
    UINT32      ulState;        // ��ƵǽID״̬��
    CHAR        acAlias[64];    // ��Ƶǽ������
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

