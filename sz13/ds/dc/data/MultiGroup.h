#ifndef MultiGroupH
#define MultiGroupH

namespace MultiGroup
{
BOOL IsMultiGroupLoaded();
BOOL LoadMultiGroup(BOOL bForce=FALSE);
BOOL CreateMultiGroup(INT64 i64GroupID, CHAR *szGroupName, VECTOR<INT64> &vGroupID);
BOOL CancelMultiGroup(INT64 i64GroupID);
BOOL SetMultiGroupMember(INT64 i64GroupID, VECTOR<INT64> &vAddGroupID, VECTOR<INT64> &vDelGroupID);
BOOL GetMultiGroupMember(INT64 i64GroupID, VECTOR<INT64> &vGroup);

VOID InitAllMultiGroup(VECTOR<MULTI_GROUP_INFO_T> &vInfo);
BOOL GetBelongingMultiGroupInfo(INT64 i64GroupID, INT64 &i64MultiGroupID, UINT32 &ulDCUserID);


}  // namespace PatchGroup

#endif
