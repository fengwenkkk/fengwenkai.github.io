#ifndef PatchGroupH
#define PatchGroupH

namespace PatchGroup
{
VOID SetMinTmpPatchGroupNum(UINT32 ulNum);
BOOL CreateTmpPatchGroup();
BOOL CancelPatchGroup(INT64 i64PatchGroupID);
BOOL CancelTmpPatchGroup(INT64 i64PatchGroupID);
INT64 AllocTmpPatchGroup(VECTOR<INT64> &vGroupID);
VOID CheckPatchGroup();
BOOL IsTmpPatchGroup(CHAR *szGroupName);
BOOL LoadPatchGroup(BOOL bForce=FALSE);
BOOL IsPatchGroupLoaded();

VOID LoadAllPatchGroupMember();
BOOL GetBelongingPatchGroupInfo(INT64 i64GroupID, INT64 &i64PatchGroupID, UINT32 &ulDCuserID);

}  // namespace PatchGroup

#endif
