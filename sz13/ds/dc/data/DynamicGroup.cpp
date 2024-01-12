#include "vcl.h"
#include "ClientInclude.h"
#include "DynamicGroup.h"
#include "Station.h"
#include "Train.h"

/*
上行、下行、正线通话组、广播组采用动态组方式，需要动态维护成员，或车载台维护其扫描功能
其它动态组采用动态创建、动态删除方式
启动时从MDC加载所有动态组
周期性检测动态组，删除过期的通话组
动态组分类：
0：不删除的动态组，
1：需要删除的动态组
动态组名称XXXXX_0/1_time
*/

static VectorMap<INT64, DC_GROUP_INFO_T>    g_mDynamicGroup;

static INT64    g_i64ReqCallGroupID = INVALID_GROUP_ID;
static INT64    g_i64BrdGroupID = INVALID_GROUP_ID;
static INT64    g_i64MultiUserGroupID = INVALID_GROUP_ID;

namespace DynamicGroup
{

BOOL LoadDynamicGroup()
{
    VECTOR<DC_GROUP_INFO_T>     vGroup;

    if (!eSDK_GetDcGroups(GROUP_TYPE_DYNAMIC, vGroup))
    {
        return FALSE;
    }

    g_mDynamicGroup.Clear();
    for (UINT32 i=0; i<vGroup.size(); i++)
    {
        g_mDynamicGroup.Add(vGroup[i].i64GroupID, vGroup[i]);

        GROUP_INFO_T    stGroup = {0};

        stGroup.ulPriority = vGroup[i].ulPriority;
        stGroup.ulDCUserID = eSDK_GetGroupCreator(vGroup[i].i64GroupID);
        stGroup.i64GroupID = vGroup[i].i64GroupID;
        stGroup.ulGroupType = DYNAMIC_GROUP;
        strcpy(stGroup.acGroupName, vGroup[i].acGroupName);

        GosLog(LOG_INFO, "dynamic group: "FORMAT_I64 " %s", stGroup.i64GroupID, stGroup.acGroupName);

        Group::AddGroup(&stGroup);
    }

    return TRUE;
}

VOID AddDynamicGroup(GROUP_INFO_T &stGroup)
{
    DC_GROUP_INFO_T     stDCGroup = {0};

    stDCGroup.i64GroupID = stGroup.i64GroupID;
    strcpy(stDCGroup.acGroupName, stGroup.acGroupName);
    stDCGroup.ulPriority = stGroup.ulPriority;
    stDCGroup.ulGroupType = GROUP_TYPE_DYNAMIC;

    g_mDynamicGroup.Add(stGroup.i64GroupID, stDCGroup);
}

DC_GROUP_INFO_T* GetDynamicGroup(INT64 i64GroupID)
{
    return g_mDynamicGroup.GetValue(i64GroupID);
}

UINT32 GetDynamicGroupSubStatus(INT64 i64GroupID)
{
    DC_GROUP_INFO_T     *pstGroup = GetDynamicGroup(i64GroupID);

    if (!pstGroup)
    {
        return GROUP_STATUS_UNKNOWN;
    }

    return pstGroup->ulGroupStatus;
}

INT64 GetDynamicGroupByName(CHAR *szGroupNamePrefix)
{
    for (UINT32 i=0; i<g_mDynamicGroup.Size(); i++)
    {
        DC_GROUP_INFO_T     *pstGroup = g_mDynamicGroup.GetValueByIndex(i);

        if (gos_str_like(pstGroup->acGroupName, szGroupNamePrefix))
        {
            return pstGroup->i64GroupID;
        }
    }

    return INVALID_GROUP_ID;
}

VOID UpdateDynamicGroupStatus(INT64 i64GroupID, UINT32 ulGroupStatus)
{
    DC_GROUP_INFO_T     *pstGroup = GetDynamicGroup(i64GroupID);

    if (!pstGroup)
    {
        return;
    }

    pstGroup->ulGroupStatus = ulGroupStatus;
}

VECTOR<DC_GROUP_INFO_T*>& GetDynamicGroup()
{
    return g_mDynamicGroup.GetValueList();
}

INT64 GenDynamicGroupID()
{
    GROUP_CFG_T     *pstGroupCfg = GroupCfg::GetGroupCfg();
    INT64   i64Num = pstGroupCfg->i64EndTempDynamicGroupID - pstGroupCfg->i64StartTempDynamicGroupID + 1;
    BOOL    abExist[100];
    UINT32  ulMaxNum = ARRAY_SIZE(abExist);
    INT64   i64Index;

    if (i64Num <= 0)
    {
        return INVALID_GROUP_ID;
    }

    if (i64Num > ulMaxNum)
    {
        i64Num = ulMaxNum;
    }

    memset(abExist, 0, sizeof(abExist));

    for (UINT32 i=0; i<g_mDynamicGroup.Size(); i++)
    {
        i64Index = g_mDynamicGroup[i]->i64GroupID - pstGroupCfg->i64StartTempDynamicGroupID;
        if (i64Index >= 0 && i64Index < ulMaxNum)
        {
            abExist[(int)i64Index] = TRUE;
        }
    }

    UINT32  ulRand = gos_get_rand();
    UINT32  j;

    for (UINT32 i=0; i<i64Num; i++)
    {
        j = MOD(ulRand+i, i64Num);
        if (!abExist[j])
        {
            return pstGroupCfg->i64StartTempDynamicGroupID+j;
        }
    }

    return INVALID_GROUP_ID;
}

VOID GetDynamicGroupName(CHAR *szGroupNamePrefix, CHAR *szGroupName)
{
    UINT32  ulCurrTime = gos_get_current_time();

    sprintf(szGroupName, "%s_%u_%u", szGroupNamePrefix, APP::GetCurrDCUserID(), ulCurrTime);
}

// 临时创建的动态组，用后会被删除
INT64 CreateDynamicGroup(CHAR *szGroupNamePrefix, VECTOR<UINT32> &vUserID)
{
    GROUP_INFO_T    stGroupInfo = {0};
    UINT32          ulMaxPeriod = g_ulDynamicGroupMaxPeriod;

    stGroupInfo.ulPriority = g_ulDynamicGroupPriority;
    stGroupInfo.ulDCUserID = APP::GetCurrDCUserID();
    stGroupInfo.i64GroupID = GenDynamicGroupID();
    stGroupInfo.ulGroupType = DYNAMIC_GROUP;

    if (stGroupInfo.i64GroupID == INVALID_GROUP_ID)
    {
        return INVALID_GROUP_ID;
    }

    GetDynamicGroupName(szGroupNamePrefix, stGroupInfo.acGroupName);

    if (!eSDK_CreateDynamicGroup(stGroupInfo.i64GroupID, vUserID, stGroupInfo.ulPriority, ulMaxPeriod, stGroupInfo.acGroupName))
    {
        return INVALID_GROUP_ID;
    }

    Group::AddGroup(&stGroupInfo);
    DynamicGroup::AddDynamicGroup(stGroupInfo);

    Group::SubJoinGroup(stGroupInfo.i64GroupID);
    Group::UpdateGroupSubStatus(stGroupInfo.i64GroupID, GROUP_STATUS_TOJOIN, TRUE);

    return stGroupInfo.i64GroupID;
}

BOOL DeleteDynamicGroup(INT64 i64GroupID)
{
    if (!eSDK_CancelDynamicGroup(i64GroupID))
    {
        return FALSE;
    }

    Group::DelGroup(i64GroupID);
    g_mDynamicGroup.Del(i64GroupID);

    GosLog(LOG_INFO, "delete dynamic group "FORMAT_I64, i64GroupID);

    return TRUE;
}

BOOL CreateTempDynamicGroup(INT64 i64GroupID, CHAR *szGroupName, VECTOR<UINT32> &vUser)
{
    GROUP_INFO_T    stGroup = {0};
    UINT32          ulMaxPeriod = g_ulDynamicGroupMaxPeriod;
    BOOL            bRet;

    stGroup.ulPriority = g_ulDynamicGroupPriority;
    stGroup.ulDCUserID = APP::GetCurrDCUserID();
    stGroup.i64GroupID = i64GroupID;
    stGroup.ulGroupType = DYNAMIC_GROUP;
    sprintf(stGroup.acGroupName, szGroupName);

    bRet = eSDK_CreateDynamicGroup(stGroup.i64GroupID, vUser, stGroup.ulPriority, ulMaxPeriod, stGroup.acGroupName);
    if (!bRet)
    {
        return FALSE;
    }

    Group::AddGroup(&stGroup);
    DynamicGroup::AddDynamicGroup(stGroup);

    Group::SubJoinGroup(stGroup.i64GroupID);

    return TRUE;
}

// DCBrdTrain_XXX_TIME （XXX为创建者）
BOOL GetDynamicGroupInfo(CHAR *szGroupName, CHAR *szGroupType, UINT32 &ulDCUserID, UINT32 &ulCreateTime)
{
    CHAR    acGroupName[64];
    CHAR    *szTime;
    CHAR    *szDCUserID;
    UINT32  ulLen = strlen(szGroupType);

    memset(acGroupName, 0, sizeof(acGroupName));
    strncpy(acGroupName, szGroupName, sizeof(acGroupName)-1);
    if (!gos_str_like(szGroupName, szGroupType))
    {
        return FALSE;
    }

    szDCUserID = acGroupName + ulLen;
    if (*szDCUserID != '_')
    {
        return FALSE;
    }

    szDCUserID++;
    szTime = strchr(szDCUserID, '_');
    if (!szTime)
    {
        return FALSE;
    }

    *szTime++ = '\0';
    if (!gos_atou(szDCUserID, &ulDCUserID) ||
        !gos_atou(szTime, &ulCreateTime))
    {
        return FALSE;
    }

    return TRUE;
}

// 删除过期的动态组
void CheckDynamicGroup(CHAR *szGroupType)
{
    DC_GROUP_INFO_T             *pstGroup;
    UINT32                      ulDCUserID;
    UINT32                      ulCreateTime;
    UINT32                      ulCurrTime = gos_get_current_time();
    UINT32                      ulPeriod;
    UINT32                      ulMaxPeriod = 120;

    for (UINT32 i=0; i<g_mDynamicGroup.Size(); i++)
    {
        pstGroup = g_mDynamicGroup[i];
        if (pstGroup->i64GroupID == g_i64BrdGroupID ||
            pstGroup->i64GroupID == g_i64ReqCallGroupID )
        {
            continue;
        }

        if (!GetDynamicGroupInfo(pstGroup->acGroupName, szGroupType, ulDCUserID, ulCreateTime) ||
            ulDCUserID != APP::GetCurrDCUserID())
        {
            continue;
        }

        ulPeriod = ulCurrTime - ulCreateTime;
        if (ulPeriod > ulMaxPeriod)
        {
            DeleteDynamicGroup(pstGroup->i64GroupID);
        }
    }
}

VOID InitDefaultDynamicGroup()
{
    VECTOR<UINT32>  vUser;
    INT64           i64GroupID;
    CHAR            acGroupNamePrefix[64];

    if (g_i64BrdGroupID == INVALID_GROUP_ID)
    {
        sprintf(acGroupNamePrefix, "%s_%u_", GROUP_NAME_DC_BRD_TRAINS, APP::GetCurrDCUserID());
        g_i64BrdGroupID = GetDynamicGroupByName(acGroupNamePrefix);
        if (g_i64BrdGroupID != INVALID_GROUP_ID)
        {
            GosLog(LOG_INFO, "default brd group is "FORMAT_I64, g_i64BrdGroupID);
        }
        else
        {
            g_i64BrdGroupID = CreateDynamicGroup(GROUP_NAME_DC_BRD_TRAINS, vUser);
            if (g_i64BrdGroupID != INVALID_GROUP_ID)
            {
                GosLog(LOG_INFO, "create default brd group "FORMAT_I64" succ", g_i64BrdGroupID);
            }
        }
    }

    if (g_i64ReqCallGroupID == INVALID_GROUP_ID)
    {
        sprintf(acGroupNamePrefix, "%s_%u_", GROUP_NAME_REQ_CALL, APP::GetCurrDCUserID());
        g_i64ReqCallGroupID = GetDynamicGroupByName(acGroupNamePrefix);
        if (g_i64ReqCallGroupID != INVALID_GROUP_ID)
        {
            GosLog(LOG_INFO, "default req group is "FORMAT_I64, g_i64ReqCallGroupID);
        }
        else
        {
            g_i64ReqCallGroupID = CreateDynamicGroup(GROUP_NAME_REQ_CALL, vUser);
            if (g_i64ReqCallGroupID != INVALID_GROUP_ID)
            {
                GosLog(LOG_INFO, "create default req call group "FORMAT_I64" succ", g_i64ReqCallGroupID);
            }
        }
    }

    if (g_i64MultiUserGroupID == INVALID_GROUP_ID)
    {
        sprintf(acGroupNamePrefix, "%s_%u_", GROUP_NAME_MULTI_USER, APP::GetCurrDCUserID());
        g_i64MultiUserGroupID = GetDynamicGroupByName(acGroupNamePrefix);
        if (g_i64MultiUserGroupID != INVALID_GROUP_ID)
        {
            GosLog(LOG_INFO, "default multi user group is "FORMAT_I64, g_i64MultiUserGroupID);
        }
        else
        {
            g_i64MultiUserGroupID = CreateDynamicGroup(GROUP_NAME_MULTI_USER, vUser);
            if (g_i64MultiUserGroupID != INVALID_GROUP_ID)
            {
                GosLog(LOG_INFO, "create default multi user group "FORMAT_I64" succ", g_i64MultiUserGroupID);
            }
        }
    }
}

INT64 InitMultiUserGroup(VECTOR<UINT32> &vUserID)
{
    CHAR            *szGroupPrefix = GROUP_NAME_MULTI_USER;
    INT64           i64GroupID = g_i64MultiUserGroupID;
    VECTOR<UINT32>  vCurrUser;
    VECTOR<INT64>   vCurrGroup;

    if (i64GroupID == INVALID_GROUP_ID ||
        !GetDynamicGroup(i64GroupID))
    {
        i64GroupID = CreateDynamicGroup(szGroupPrefix, vUserID);
        if (i64GroupID == INVALID_GROUP_ID)
        {
            return INVALID_GROUP_ID;
        }
        else
        {
            g_i64MultiUserGroupID = i64GroupID;
            return i64GroupID;
        }
    }

    if (!eSDK_GetGroupMember(i64GroupID, vCurrUser, vCurrGroup))
    {
        GosLog(LOG_ERROR, "get group member of " FORMAT_I64 " failed", i64GroupID);
        return INVALID_GROUP_ID;
    }

    Group::SubJoinGroup(i64GroupID);

    RemoveCommonMember(vCurrUser, vUserID);
    RemoveMember(vCurrUser, APP::GetCurrDCUserID());
    if (vCurrUser.size() == 0 && vUserID.size() == 0)
    {
        return i64GroupID;
    }

    if (!eSDK_SetDynamicGroupUser(i64GroupID, vUserID, vCurrUser))
    {
        GosLog(LOG_ERROR, "set group member of " FORMAT_I64 " failed", i64GroupID);
        return INVALID_GROUP_ID;
    }

    return i64GroupID;
}

INT64 GetReqCallGroupID()
{
    return g_i64ReqCallGroupID;
}

VOID SetReqCallGroupID(INT64 i64GroupID)
{
    g_i64ReqCallGroupID = i64GroupID;
}

INT64 GetBrdGroupID()
{
    return g_i64BrdGroupID;
}

VOID SetBrdGroupID(INT64 i64GroupID)
{
    g_i64BrdGroupID = i64GroupID;
}

} // namespace DynamicGroup
