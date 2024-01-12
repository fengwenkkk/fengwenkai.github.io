//---------------------------------------------------------------------------

#ifndef TrainH
#define TrainH

#define TRAIN_PARK              0   // ͣ���ڳ�վ
#define TRAIN_COME              (-1)// ����ʻ�복վ
#define TRAIN_LEAVE             1   // �Ѿ�ʻ����վ

#define SINGLE_TRAIN                0
#define MULTI_TRAIN                 1
#define UPLINE_TRAIN                2
#define DOWNLINE_TRAIN              3
#define MAINLINE_TRAIN              4
#define INTERLOCK_STATION_TRAIN     5
#define STATION_TRAIN               6

typedef struct
{
    BOOL    bEmergencyCall;     // ��������
    BOOL    bRequestCall;       // �������
    BOOL    bPassengerCall;     // �˿ͶԽ�
    BOOL    bBroadcast;         // �г��㲥

}TRAIN_STATE_T;

#define TRAIN_EVENT_ONLINE                  1   // ����
#define TRAIN_EVENT_OFFLINE                 2   // ����
#define TRAIN_EVENT_AUTO_CHECK              3   // �г��Լ�״̬
#define TRAIN_EVENT_POS_CHANGED             5   // �г�λ�ñ仯���������ߣ������з���

#define TRAIN_EVENT_REGISTER                11  // ע��
#define TRAIN_EVENT_UNREGISTER              12  // ע��
#define TRAIN_EVENT_AUTO_CHANGE_GROUP       13  // �Զ�ת��
#define TRAIN_EVENT_MANU_CHANGE_GROUP       14  // �ֶ�ת��
#define TRAIN_EVENT_TRAINID_CHANGED         15  // ���κű仯
#define TRAIN_EVENT_MANU_SET_TRAINID        16  // �ֹ����ó��κţ�

#define TRAIN_EVENT_EMERGENCY_CALL          20  // ��������
#define TRAIN_EVENT_REQUEST_CALL            21  // �������
#define TRAIN_EVENT_IPH                     22  // �˿ͶԽ�

typedef struct
{
    UINT32      ulTime;
    UINT32      ulTrainUnitID;
    UINT32      ulEventID;
    CHAR        acEvent[64];
}TRAIN_EVENT_T;

namespace Train
{
UINT32 GetATSDataTimeDiff();
UINT32 GetMaxStationID();
UINT32 GetMinStationID();
UINT16 GetNextStationID(TRAIN_INFO_T *pstTrainInfo);

void InitTrainInfo(VECTOR<TRAIN_INFO_T> &vTrainInfo, UINT32 ulATSDataTimeDiff);
void GetTrainInfo(VECTOR<TRAIN_INFO_T*> &vTrainInfo);
void GetTrainInfo(VECTOR<TRAIN_INFO_T> &vTrainInfo);
VECTOR<TRAIN_INFO_T*>& GetTrainInfo();
void GetTrainOfStation(UINT32 ulStationID, VECTOR<TRAIN_INFO_T*> &vTrainInfo);
void GetTrainOfInterLockStation(UINT32 ulInterLockStationID, VECTOR<TRAIN_INFO_T*> &vInfo);
TRAIN_INFO_T* GetTrainInfo(UINT32 ulTrainUnitID);
TRAIN_INFO_T* GetTrainInfo(INT64 i64GroupID);
AnsiString GetTrainUnitIDText(UINT32 ulTrainUnitID);
AnsiString GetTrainUnitIDText(TRAIN_INFO_T *pstTrain);

VOID GetNoramlTrainInfo(VECTOR<TRAIN_INFO_T*> &vTrainInfo);
VOID GetDepotTrainInfo(UINT32 ulDepotID, VECTOR<TRAIN_INFO_T*> &vTrainInfo);
UINT32 GetNormalTrainNum();
TRAIN_INFO_T* GetTrainInfoByTrainID(CHAR *szTrainID);
UINT32 GetTrainUnitID(INT64 i64GroupID);
INT64 GetTrainGroupID(TRAIN_INFO_T *pstTrain);
INT64 GetTrainGroupID(UINT32 ulTrainUnitID);
AnsiString GetTrainStationPosText(INT32 iTrainStationPos);

BOOL HaveTrainAlarm(UINT32 ulTrainUnitID);
VOID GetTrainUserID(UINT32 ulTrainUnitID, UINT32 *pulUserID1, UINT32 *pulUserID2);
VOID GetTrainUserID(UINT32 ulTrainUnitID, VECTOR<UINT32> &vUserID);

VOID ClearTrainBroadcastMember(INT64 i64GroupID);
BOOL InitTrainBroadcastMember(INT64 i64GroupID, VECTOR<UINT32> &vUser);

BOOL GetTrainRegisterInfo(UINT32 ulTrainUnitID, BOOL *pbRegister, UINT32 *pulDepotID);
BOOL SetTrainRegisterInfo(UINT32 ulTrainUnitID, BOOL bRegister, UINT32 ulDepotID);

BOOL GetTrainRegisterState(UINT32 ulTrainUnitID);
BOOL GetGroupRegisterState(INT64 i64GroupID);

AnsiString GetTrainID(TRAIN_INFO_T *pstTrain);
AnsiString GetTrainAlias(UINT32 ulTrainUnitID, CHAR *szTrainID=NULL);
AnsiString GetTrainAlias(TRAIN_INFO_T *pstTrain);

BOOL IsTrainGroup(INT64 i64GroupID);
UINT32 GetTrainDepotID(UINT32 ulTrainUnitID);

}  // namespace Train

//---------------------------------------------------------------------------
#endif
