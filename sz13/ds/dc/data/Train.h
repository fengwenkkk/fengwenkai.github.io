//---------------------------------------------------------------------------

#ifndef TrainH
#define TrainH

#define TRAIN_PARK              0   // 停靠在车站
#define TRAIN_COME              (-1)// 即将驶入车站
#define TRAIN_LEAVE             1   // 已经驶出车站

#define SINGLE_TRAIN                0
#define MULTI_TRAIN                 1
#define UPLINE_TRAIN                2
#define DOWNLINE_TRAIN              3
#define MAINLINE_TRAIN              4
#define INTERLOCK_STATION_TRAIN     5
#define STATION_TRAIN               6

typedef struct
{
    BOOL    bEmergencyCall;     // 紧急呼叫
    BOOL    bRequestCall;       // 请求呼叫
    BOOL    bPassengerCall;     // 乘客对讲
    BOOL    bBroadcast;         // 列车广播

}TRAIN_STATE_T;

#define TRAIN_EVENT_ONLINE                  1   // 上线
#define TRAIN_EVENT_OFFLINE                 2   // 离线
#define TRAIN_EVENT_AUTO_CHECK              3   // 列车自检状态
#define TRAIN_EVENT_POS_CHANGED             5   // 列车位置变化（出入正线，上下行方向）

#define TRAIN_EVENT_REGISTER                11  // 注册
#define TRAIN_EVENT_UNREGISTER              12  // 注销
#define TRAIN_EVENT_AUTO_CHANGE_GROUP       13  // 自动转组
#define TRAIN_EVENT_MANU_CHANGE_GROUP       14  // 手动转组
#define TRAIN_EVENT_TRAINID_CHANGED         15  // 车次号变化
#define TRAIN_EVENT_MANU_SET_TRAINID        16  // 手工设置车次号）

#define TRAIN_EVENT_EMERGENCY_CALL          20  // 紧急呼叫
#define TRAIN_EVENT_REQUEST_CALL            21  // 请求呼叫
#define TRAIN_EVENT_IPH                     22  // 乘客对讲

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
