//---------------------------------------------------------------------------

#ifndef GroupH
#define GroupH

#include "cxTL.hpp"

#include "Msg.h"
#include "eSDK.h"

extern Int64Set         g_sSubGroup;
extern Int64Set         g_sSubVideoGroup;

namespace GroupCfg
{
VOID InitGroupCfg(GROUP_CFG_T &vInfo);
GROUP_CFG_T* GetGroupCfg();
BOOL IsLineBrdGroup(INT64 i64GroupID);
BOOL IsMulticastGroup(INT64 i64GroupID);
};

namespace Group
{

BOOL SubJoinGroup(INT64 i64GroupID);
BOOL ExitGroup(INT64 i64GroupID);

VOID GroupCallOver(INT64 i64GroupID);
VOID AutoExitGroup(INT64 i64GroupID);
VOID AutoExitGroup(INT64 i64GroupID, UINT32 ulCurrSpeaker);
VOID AutoJoinGroup(INT64 i64GroupID);
VOID CheckAutoJoinGroup();

BOOL LoadStaticGroup();
VOID InitGroup(VECTOR<GROUP_INFO_T> &vGroup);
VOID AddGroup(GROUP_INFO_T *pstGroup);
VOID DelGroup(INT64 i64GroupID);

VOID UpdateMultiGroup(VECTOR<SDK_MULTI_GROUP_INFO_T> &vGroup);

GROUP_INFO_T* GetGroup(INT64 i64GroupID);
AnsiString GetGroupName(INT64 i64GroupID);
AnsiString GetGroupName(GROUP_INFO_T *pstGroup);
AnsiString GetGroupDisplayName(INT64 i64GroupID);

BOOL IsVisibleGroup(GROUP_INFO_T *pstGroup);
BOOL IsPatchGroup(UINT32 ulGroupType);

VOID GetAllGroup(VECTOR<GROUP_INFO_T*> &vGroup);
VOID GetVisibleGroup(VECTOR<GROUP_INFO_T*> &vGroup);    // 不包括列车广播组、选区广播组
VOID GetGroup(UINT32 ulGroupType, VECTOR<GROUP_INFO_T*> &vGroup);
VOID GetGroup(UINT32 ulGroupType, VECTOR<INT64> &vGroupID);

BOOL GetGroupMember(INT64 i64GroupID, VECTOR<UINT32> &vUser, VECTOR<INT64> &vGroup);

VOID GetGroupType(std::vector<UINT32> &vGroupType);
AnsiString GetGroupTypeName(UINT32 ulGroupType);

UINT32 GetGroupSubStatus(INT64 i64GroupID);
VOID UpdateGroupSubStatus(INT64 i64GroupID, UINT32 ulGroupStatus, BOOL bNeedSave);
VOID CommitGroupSubStatus(INT64 i64GroupID, UINT32 ulGroupStatus);

VOID InitAutoJoinGroup();
VOID InitAutoJoinGroup(VECTOR<GROUP_STATUS_T> &vStatus);
VOID AddAutoJoinGroup(INT64 i64GroupID, BOOL bNeedSave);
VOID GetAutoJoinGroup(VECTOR<INT64> &vGroup);
GROUP_STATUS_T* GetAutoJoinGroup(INT64 i64GroupID);
VOID DelAutoJoinGroup(INT64 i64GroupID);

UINT32 GetGroupCallType(INT64 i64GroupID);
VOID NewEmergencyGroupCall(EVENT_GROUP_STATUS_T &stStatus);
VOID CloseEmergencyGroupCall(INT64 i64GroupID);
VOID UpdateGroupCallStatus(EVENT_GROUP_STATUS_T &stStatus);
EVENT_GROUP_STATUS_T* GetGroupCallStatus(INT64 i64GroupID);

BOOL IsTrainBroadcastGroup(INT64 i64GroupID);

VOID InitBrdZoneInfo(VECTOR<BRD_ZONE_INFO_T> &vInfo);
VOID GetBrdZoneInfo(UINT32 ulStationID, VECTOR<BRD_ZONE_INFO_T> &vInfo);
VECTOR<BRD_ZONE_INFO_T>& GetBrdZoneInfo();

INT64 AllocMultiTrainBrdGroupID();

void CheckTrainGroup();
void CheckStationGroup();
void SubGroup(INT64 i64GroupID);
void UnSubGroup(INT64 i64GroupID);

VOID InitGroupMarkInfo(VECTOR<GROUP_INFO_T> &vGroup);
GROUP_INFO_T* GetGroupMarkInfo(INT64 i64GroupID);
UINT32 GetGroupFuncType(INT64 i64GroupID);

INT64 GetMainGroupID();
VOID SetMainGroupID(INT64 i64GroupID);

VOID SetDefaultGroupSpeaker();
VOID SetDefaultGroupMic();
}  // namespace Group

//---------------------------------------------------------------------------
#endif
