//---------------------------------------------------------------------------

#ifndef DynamicGroupH
#define DynamicGroupH

#include "cxTL.hpp"

#include "Msg.h"
#include "eSDK.h"

#define GROUP_NAME_TRAIN            "Train"         // 列车缺省通话组
#define GROUP_NAME_DC_BRD_TRAINS    "广播组"
#define GROUP_NAME_REQ_CALL         "请呼组"
#define GROUP_NAME_MULTI_USER       "多选组"

namespace DynamicGroup
{

BOOL LoadDynamicGroup();
VECTOR<DC_GROUP_INFO_T*>& GetDynamicGroup();
DC_GROUP_INFO_T* GetDynamicGroup(INT64 i64GroupID);
UINT32 GetDynamicGroupSubStatus(INT64 i64GroupID);
VOID UpdateDynamicGroupStatus(INT64 i64GroupID, UINT32 ulGroupStatus);
INT64 CreateDynamicGroup(CHAR *szGroupNamePrefix, VECTOR<UINT32> &vUserID);
BOOL DeleteDynamicGroup(INT64 i64GroupID);
BOOL GetDynamicGroupTime(CHAR *szGroupName, UINT32 *pulCreateTime);
void CheckDefaultTrainGroup();
void CheckDynamicGroup(CHAR *szGroupType);
VOID InitDefaultDynamicGroup();
BOOL CreateDepotTrainGroup(TRAIN_INFO_T *pstTrain);
BOOL CreateDCTrainBrdGroup(INT64 i64GroupID);
BOOL CreateTrainMulticastGroup(INT64 i64GroupID);
BOOL CreateStationMulticastGroup(INT64 i64GroupID);
BOOL CreateTempDynamicGroup(INT64 i64GroupID, CHAR *szGroupName, VECTOR<UINT32> &vUser);

INT64 InitMultiUserGroup(VECTOR<UINT32> &vUserID);

INT64 GetReqCallGroupID();
VOID SetReqCallGroupID(INT64 i64GroupID);

INT64 GetBrdGroupID();
VOID SetBrdGroupID(INT64 i64GroupID);

}  // namespace DynamicGroup

//---------------------------------------------------------------------------
#endif
