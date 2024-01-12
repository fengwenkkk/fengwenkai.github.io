#include "vcl.h"
#include "ClientInclude.h"
#include "Group.h"
#include "Station.h"
#include "Train.h"
#include "AudioDev.h"

Int64Set        g_sSubGroup;            // 关注组
Int64Set        g_sSubVideoGroup;       // 视频关注组

GROUP_CFG_T     g_stGroupCfg = {0};

static VectorMap<INT64, GROUP_INFO_T>       g_mGroupInfo;
static VectorMap<INT64, GROUP_STATUS_T>     g_sAutoJoinGroup;       // 自动订阅组
static std::map<INT64, EVENT_GROUP_STATUS_T> g_mGroupCallStatus;

static VECTOR<BRD_ZONE_INFO_T>              g_vBrdZoneInfo;

static VectorMap<INT64, GROUP_INFO_T>       g_mGroupMarkInfo;       // 群组标注信息
static MAP<INT64, UINT32>                   g_mGroupIdleInfo;       // 群组话权空闲开始时间, 用于保证对此类群组实施Join操作

static MAP<INT64, AnsiString>               g_mJoinedGroup;         // 已经加入的组

static INT64    g_i64MainGroupID = INVALID_GROUP_ID;   // 当前正在通话的群组
static UINT32   g_ulKeepOutGroupTime = 15;  // 组呼结束15秒内不自动加入组

namespace GroupCfg
{
VOID InitGroupCfg(GROUP_CFG_T &stCfg)
{
    memcpy(&g_stGroupCfg, &stCfg, sizeof(g_stGroupCfg));
}

GROUP_CFG_T* GetGroupCfg()
{
    return &g_stGroupCfg;
}

BOOL IsLineBrdGroup(INT64 i64GroupID)
{
    if (i64GroupID == g_stGroupCfg.i64UpLineTrainBrdGroupID ||
        i64GroupID == g_stGroupCfg.i64DownLineTrainBrdGroupID ||
        i64GroupID == g_stGroupCfg.i64MainLineTrainBrdGroupID )
    {
        return TRUE;
    }

    return FALSE;
}

BOOL IsMulticastGroup(INT64 i64GroupID)
{
    if (i64GroupID == g_stGroupCfg.i64UpLineTrainGroupID   ||
        i64GroupID == g_stGroupCfg.i64DownLineTrainGroupID ||
        i64GroupID == g_stGroupCfg.i64MainLineTrainGroupID ||
        i64GroupID == g_stGroupCfg.i64AllStationGroupID )
    {
        return TRUE;
    }

    return FALSE;
}

}

namespace Group
{

BOOL SubJoinGroup(INT64 i64GroupID)
{
    if (!eSDK_SubJoinGroup(i64GroupID))
    {
        return FALSE;
    }

    AnsiString  strSpeaker = GetESDKMonSpeaker();

    if (g_mJoinedGroup.find(i64GroupID) == g_mJoinedGroup.end() ||
        g_mJoinedGroup[i64GroupID] != strSpeaker)
    {
        g_mJoinedGroup[i64GroupID] = strSpeaker;

        AssignGroupSpeaker(i64GroupID, strSpeaker);
    }

    if (g_mGroupIdleInfo.find(i64GroupID) != g_mGroupIdleInfo.end() )
    {
        g_mGroupIdleInfo[i64GroupID] = 0xffffffff; // 不需要继续加入
    }

    return TRUE;
}

BOOL ExitGroup(INT64 i64GroupID)
{
    if (!eSDK_UnSubGroup(i64GroupID))
    {
        return FALSE;
    }

    g_mJoinedGroup.erase(i64GroupID);

    return TRUE;
}

VOID AutoExitGroup(INT64 i64GroupID)
{
    if (Station::GetStationByGroupID(i64GroupID))
    {
        GosLog(LOG_INFO, "auto exit group" FORMAT_I64, i64GroupID);
        ExitGroup(i64GroupID);
    }
}

VOID GroupCallOver(INT64 i64GroupID)
{
    g_mGroupIdleInfo[i64GroupID] = gos_get_uptime();
}

// 其他调度台正在接听车载台/固定台时，本调度台自动退出
VOID AutoExitGroup(INT64 i64GroupID, UINT32 ulCurrSpeaker)
{
    if (Train::GetTrainInfo(i64GroupID) ||
        Station::GetStationByGroupID(i64GroupID) )
    {
        if (ulCurrSpeaker != APP::GetCurrDCUserID() &&
            DCUser::GetDCUser(ulCurrSpeaker))
        {
            GosLog(LOG_INFO, "auto exit group " FORMAT_I64 " when another dc is speaking", i64GroupID);
            ExitGroup(i64GroupID);
        }
    }
}

VOID CheckAutoJoinGroup()
{
    UINT32  ulCurrTime = gos_get_uptime();
    MAP<INT64, UINT32>::iterator    it = g_mGroupIdleInfo.begin();
    VECTOR<INT64>                   vGroup;

    for (; it!=g_mGroupIdleInfo.end(); it++)
    {
        INT64   i64GroupID = it->first;
        UINT32  ulIdleTime = it->second;

        if (ulIdleTime == 0xffffffff ||
            !Train::GetTrainInfo(i64GroupID) )
        {
            continue;
        }

        if ((ulIdleTime + g_ulKeepOutGroupTime) < ulCurrTime)
        {
            GosLog(LOG_INFO, "auto rejoin group " FORMAT_I64, i64GroupID);
            vGroup.push_back(i64GroupID);
        }
    }

    // SubJoinGroup可能会对g_mGroupIdleInfo进行插入操作，如果放在上面的循环可能导致iterator++错误
    for (UINT32 i=0; i<vGroup.size(); i++)
    {
        SubJoinGroup(vGroup[i]);
    }
}

VOID GetGroupType(std::vector<UINT32> &vGroupType)
{
    vGroupType.clear();
    vGroupType.push_back(TRAIN_GROUP);
    vGroupType.push_back(STATIC_GROUP);
    vGroupType.push_back(STATION_GROUP);
}

AnsiString GetGroupTypeName(UINT32 ulGroupType)
{
    if (ulGroupType == TRAIN_GROUP)     return "列车组";
    if (ulGroupType == STATIC_GROUP)    return "静态组";
    if (ulGroupType == STATION_GROUP)   return "车站组";

    return "";
}

BOOL LoadStaticGroup()
{
    VECTOR<DC_GROUP_INFO_T>     vGroup;
    VECTOR<GROUP_INFO_T>        vStaticGroup;

    if (!eSDK_GetDcGroups(GROUP_TYPE_GENERAL, vGroup))
    {
        return FALSE;
    }

    for (UINT32 i=0; i<vGroup.size(); i++)
    {
        GROUP_INFO_T    stGroup = {0};

        stGroup.ulPriority = vGroup[i].ulPriority;
        stGroup.ulDCUserID = INVALID_USER_ID;
        stGroup.i64GroupID = vGroup[i].i64GroupID;
        stGroup.ulGroupType = STATIC_GROUP;
        sprintf(stGroup.acGroupName, vGroup[i].acGroupName);

        if (Train::GetTrainInfo(stGroup.i64GroupID))
        {
            stGroup.ulGroupType = TRAIN_GROUP;
        }
        else if (StationUE::GetStationUEByGroupID(stGroup.i64GroupID))
        {
            stGroup.ulGroupType = STATION_GROUP;
        }

        GosLog(LOG_INFO, "static group: "FORMAT_I64 " %s", stGroup.i64GroupID, stGroup.acGroupName);

        vStaticGroup.push_back(stGroup);
    }

    InitGroup(vStaticGroup);

    return TRUE;
}

// 初始化静态组
VOID InitGroup(VECTOR<GROUP_INFO_T> &vGroup)
{
    // 移除多余的组
    Int64Set       mDelGroup;
    VECTOR<INT64>  vDelGroup;

    for (UINT32 i=0; i<g_mGroupInfo.Size(); i++)
    {
        if (g_mGroupInfo[i]->ulGroupType == STATIC_GROUP)
        {
            mDelGroup.Add(g_mGroupInfo[i]->i64GroupID);
        }
    }

    for (INT32 i=vGroup.size()-1; i>=0; i--)
    {
        if (g_mGroupInfo.GetValue(vGroup[i].i64GroupID))
        {
            mDelGroup.Del(vGroup[i].i64GroupID);
            vGroup.erase(vGroup.begin()+i);
        }
    }

    mDelGroup.GetList(vDelGroup);
    for (UINT32 i=0; i<vDelGroup.size(); i++)
    {
        UpdateGroupSubStatus(vDelGroup[i], GROUP_STATUS_UNSUB, FALSE);
        g_mGroupInfo.Del(vDelGroup[i]);
    }

    for (UINT32 i=0; i<vGroup.size(); i++)
    {
        AddGroup(&vGroup[i]);
    }
}

VOID UpdateMultiGroup(VECTOR<SDK_MULTI_GROUP_INFO_T> &vGroup)
{
    // 移除多余的组
    Int64Set       mDelGroup;
    VECTOR<INT64>  vDelGroup;

    // 先把多选组取出到待删除集合中
    for (UINT32 i=0; i<g_mGroupInfo.Size(); i++)
    {
        if (g_mGroupInfo[i]->ulGroupType == MULTI_GROUP)
        {
            mDelGroup.Add(g_mGroupInfo[i]->i64GroupID);
        }
    }

    // 如果不存在，则插入，否则，从待删除集合中去除该成员
    for (UINT32 i=0; i<vGroup.size(); i++)
    {
        if (g_mGroupInfo.GetValue(vGroup[i].i64GroupID))
        {
            mDelGroup.Del(vGroup[i].i64GroupID);
        }
        else
        {
            GROUP_INFO_T        stInfo = {0};

            stInfo.i64GroupID = vGroup[i].i64GroupID;
            strcpy(stInfo.acGroupName, vGroup[i].acGroupName);
            stInfo.ulGroupType = MULTI_GROUP;
            stInfo.ulFuncType = GROUP_FUNC_OTHER;
            stInfo.ulPriority = 0;
            stInfo.ulDCUserID = vGroup[i].ulDCUserID;

            if (stInfo.acGroupName[0] == '\0')
            {
                sprintf(stInfo.acGroupName, FORMAT_I64, stInfo.i64GroupID);
            }

            AddGroup(&stInfo);
        }
    }

    mDelGroup.GetList(vDelGroup);
    for (UINT32 i=0; i<vDelGroup.size(); i++)
    {
        UpdateGroupSubStatus(vDelGroup[i], GROUP_STATUS_UNSUB, FALSE);
        g_mGroupInfo.Del(vDelGroup[i]);
    }
}

VOID AddGroup(GROUP_INFO_T *pstGroup)
{
    g_mGroupInfo.Add(pstGroup->i64GroupID, *pstGroup);

    bool bAssign = true;

    // 多选组、派接组（随车通信也是派接组）不需要assign
    if (pstGroup->ulGroupType == PATCH_GROUP ||
        pstGroup->ulGroupType == MULTI_GROUP  )
    {
        bAssign = false;
    }
    else if (pstGroup->ulGroupType == DYNAMIC_GROUP ||
             pstGroup->ulGroupType == TRAIN_BRD_GROUP)
    {
        if (pstGroup->ulDCUserID != APP::GetCurrDCUserID())
        {
            bAssign = false;
        }
    }

    if (bAssign)
    {
        AssignGroupSpeaker(pstGroup->i64GroupID, GetESDKMonSpeaker());//GetMonSpeaker());
        AssignGroupMic(pstGroup->i64GroupID, GetESDKMainMic());
    }
}

VOID DelGroup(INT64 i64GroupID)
{
    g_sAutoJoinGroup.Del(i64GroupID);

    g_mGroupInfo.Del(i64GroupID);
}

GROUP_INFO_T* GetGroup(INT64 i64GroupID)
{
    return g_mGroupInfo.GetValue(i64GroupID);
}

AnsiString GetGroupName(INT64 i64GroupID)
{
    GROUP_INFO_T    *pstGroup = GetGroup(i64GroupID);

    if (!pstGroup)
    {
        return i64GroupID;
    }

    return GetGroupName(pstGroup);
}

AnsiString GetGroupName(GROUP_INFO_T *pstGroup)
{
    if (!pstGroup)
    {
        return "";
    }

    CHAR    acGroupName[128];
    CHAR    *szName = strchr(pstGroup->acGroupName, '_');

    if (!szName)
    {
        return pstGroup->acGroupName;
    }

    memset(acGroupName, 0, sizeof(acGroupName));
    memcpy(acGroupName, pstGroup->acGroupName, (size_t)(szName - pstGroup->acGroupName));
    szName++;

    if (strcmp(acGroupName, GROUP_NAME_TRAIN) == 0)
    {
        sprintf(acGroupName, "列车%s通话组", szName);
    }
    else if (strcmp(acGroupName, GROUP_NAME_DC_BRD_TRAINS) == 0)
    {
        sprintf(acGroupName, "列车广播组");
    }
    else if (strcmp(acGroupName, GROUP_NAME_REQ_CALL) == 0)
    {
        sprintf(acGroupName, "请呼动态组");
    }
    else
    {
        return pstGroup->acGroupName;
    }

    return acGroupName;
}

AnsiString GetGroupDisplayName(INT64 i64GroupID)
{
    AnsiString  strName = GetGroupName(i64GroupID);

    if (strName != i64GroupID)
    {
        strName += AnsiString("(") + AnsiString(i64GroupID) + AnsiString(")");
    }

    return strName;
}

VOID GetAllGroup(VECTOR<GROUP_INFO_T*> &vGroup)
{
    g_mGroupInfo.GetValueList(vGroup);
}

BOOL IsVisibleGroup(GROUP_INFO_T *pstGroup)
{
    if (pstGroup->ulGroupType == ZONE_BRD_GROUP ||
        pstGroup->ulGroupType == TRAIN_BRD_GROUP )
    {
        return FALSE;
    }

    if (pstGroup->i64GroupID == g_stGroupCfg.i64UpLineTrainBrdGroupID ||
        pstGroup->i64GroupID == g_stGroupCfg.i64DownLineTrainBrdGroupID ||
        pstGroup->i64GroupID == g_stGroupCfg.i64MainLineTrainBrdGroupID )
    {
        return FALSE;
    }

    return TRUE;
}

BOOL IsPatchGroup(UINT32 ulGroupType)
{
    if (ulGroupType == PATCH_GROUP)
    {
        return TRUE;
    }

    return FALSE;
}

VOID GetVisibleGroup(VECTOR<GROUP_INFO_T*> &vGroup)
{
    VECTOR<GROUP_INFO_T*> vAllGroup;

    vGroup.clear();
    GetAllGroup(vAllGroup);

    for (UINT32 i=0; i<vAllGroup.size(); i++)
    {
        if (IsVisibleGroup(vAllGroup[i]))
        {
            vGroup.push_back(vAllGroup[i]);
        }
    }
}

VOID GetGroup(UINT32 ulGroupType, VECTOR<GROUP_INFO_T*> &vGroup)
{
    VECTOR<GROUP_INFO_T*> vAllGroup;

    vGroup.clear();
    GetAllGroup(vAllGroup);

    for (UINT32 i=0; i<vAllGroup.size(); i++)
    {
        if (ulGroupType == vAllGroup[i]->ulGroupType)
        {
            vGroup.push_back(vAllGroup[i]);
        }
    }
}

VOID GetGroup(UINT32 ulGroupType, VECTOR<INT64> &vGroupID)
{
    VECTOR<GROUP_INFO_T*> vAllGroup;

    GetAllGroup(vAllGroup);
    vGroupID.clear();

    for (UINT32 i=0; i<vAllGroup.size(); i++)
    {
        if (ulGroupType == vAllGroup[i]->ulGroupType)
        {
            vGroupID.push_back(vAllGroup[i]->i64GroupID);
        }
    }
}

BOOL GetGroupMember(INT64 i64GroupID, VECTOR<UINT32> &vUser, VECTOR<INT64> &vGroup)
{
    vUser.clear();
    vGroup.clear();

    GROUP_INFO_T    *pstGroup = Group::GetGroup(i64GroupID);

    if (!pstGroup)
    {
        return FALSE;
    }

    if (pstGroup->ulGroupType == PATCH_GROUP)
    {
        if (!eSDK_GetPatchGroupMembers(i64GroupID, vGroup))
        {
            return FALSE;
        }
    }
    else if (pstGroup->ulGroupType == DYNAMIC_GROUP)
    {
        if (!eSDK_GetDynamicGroupMember(i64GroupID, vUser, vGroup))
        {
            return FALSE;
        }
    }
    else if (pstGroup->ulGroupType == MULTI_GROUP)
    {
        if (!MultiGroup::GetMultiGroupMember(i64GroupID, vGroup))
        {
            return FALSE;
        }
    }
    else
    {
        if (!eSDK_GetGroupMember(i64GroupID, vUser, vGroup))
        {
            return FALSE;
        }
    }

    return TRUE;
}

VOID InitAutoJoinGroup(VECTOR<GROUP_STATUS_T> &vStatus)
{
    g_sAutoJoinGroup.Clear();

    for (UINT32 i=0; i<vStatus.size(); i++)
    {
        g_sAutoJoinGroup.Add(vStatus[i].i64GroupID, vStatus[i]);
    }
}

VOID AddAutoJoinGroup(INT64 i64GroupID, BOOL bNeedSave)
{
    GROUP_STATUS_T  *pstGroup = GetAutoJoinGroup(i64GroupID);

    if (pstGroup)
    {
        if (pstGroup->ulGroupStatus == GROUP_STATUS_JOIN)
        {
            return;
        }

        pstGroup->ulGroupStatus = GROUP_STATUS_TOJOIN;
        pstGroup->bNeedSave = bNeedSave;
    }
    else
    {
        GROUP_STATUS_T  stStatus = {0};

        stStatus.i64GroupID = i64GroupID;
        stStatus.ulGroupStatus = GROUP_STATUS_TOJOIN;
        stStatus.bNeedSave = bNeedSave;

        g_sAutoJoinGroup.Add(i64GroupID, stStatus);
    }
}

GROUP_STATUS_T* GetAutoJoinGroup(INT64 i64GroupID)
{
    return g_sAutoJoinGroup.GetValue(i64GroupID);
}

VOID DelAutoJoinGroup(INT64 i64GroupID)
{
    g_sAutoJoinGroup.Del(i64GroupID);
}

UINT32 GetGroupSubStatus(INT64 i64GroupID)
{
    GROUP_STATUS_T  *pstStatus = g_sAutoJoinGroup.GetValue(i64GroupID);

    if (!pstStatus)
    {
        return GROUP_STATUS_UNKNOWN;
    }

    return pstStatus->ulGroupStatus;
}

VOID UpdateGroupSubStatus(INT64 i64GroupID, UINT32 ulGroupStatus, BOOL bNeedSave)
{
    GROUP_STATUS_T  *pstStatus = g_sAutoJoinGroup.GetValue(i64GroupID);

    if (!pstStatus)
    {
        if (ulGroupStatus == GROUP_STATUS_TOJOIN )
        {
            AddAutoJoinGroup(i64GroupID, bNeedSave);
        }
        else if (ulGroupStatus == GROUP_STATUS_JOIN ||
                 ulGroupStatus == GROUP_STATUS_SUB   )
        {
            GROUP_STATUS_T  stStatus = {0};

            stStatus.i64GroupID = i64GroupID;
            stStatus.ulGroupStatus = ulGroupStatus;
            stStatus.bNeedSave = bNeedSave;

            g_sAutoJoinGroup.Add(i64GroupID, stStatus);
        }
        else
        {
            return;
        }
    }
    else
    {
        pstStatus->ulGroupStatus = ulGroupStatus;
        pstStatus->bNeedSave = bNeedSave;
    }
}

VOID CommitGroupSubStatus(INT64 i64GroupID, UINT32 ulGroupStatus)
{
    GROUP_STATUS_T  *pstStatus = g_sAutoJoinGroup.GetValue(i64GroupID);

    if (!pstStatus || pstStatus->ulGroupStatus != ulGroupStatus)
    {
        return;
    }

    if (ulGroupStatus == GROUP_STATUS_UNSUB ||
        ulGroupStatus == GROUP_STATUS_DELETE )
    {
        if (ExitGroup(i64GroupID))
        {
            GosLog(LOG_INFO, "group "FORMAT_I64" unsubscribed ", i64GroupID);
            g_sAutoJoinGroup.Del(i64GroupID);
        }
        else
        {
            GosLog(LOG_INFO, "group "FORMAT_I64" unsubscribed failed", i64GroupID);
        }
    }
    else
    {
        pstStatus->ulGroupStatus = ulGroupStatus;
        pstStatus->bNeedSave = FALSE;
    }
}

VOID GetAutoJoinGroup(VECTOR<INT64> &vGroup)
{
    VECTOR<INT64>   &vKey = g_sAutoJoinGroup.GetKeyList();

    vGroup.clear();
    vGroup.assign(vKey.begin(), vKey.end());
}

UINT32 GetGroupCallType(INT64 i64GroupID)
{
    if (g_mGroupCallStatus.find(i64GroupID) == g_mGroupCallStatus.end())
    {
        return CALL_TYPE_UNKNOWN;
    }
    else
    {
        return g_mGroupCallStatus[i64GroupID].ulCallType;
    }
}

VOID NewEmergencyGroupCall(EVENT_GROUP_STATUS_T &stStatus)
{
    g_mGroupCallStatus[stStatus.i64GroupID] = stStatus;
}

VOID CloseEmergencyGroupCall(INT64 i64GroupID)
{
    if (g_mGroupCallStatus.find(i64GroupID) != g_mGroupCallStatus.end())
    {
        g_mGroupCallStatus[i64GroupID].bClosed = TRUE;
    }
}

VOID UpdateGroupCallStatus(EVENT_GROUP_STATUS_T &stStatus)
{
    g_mGroupCallStatus[stStatus.i64GroupID] = stStatus;
}

EVENT_GROUP_STATUS_T* GetGroupCallStatus(INT64 i64GroupID)
{
    if (g_mGroupCallStatus.find(i64GroupID) == g_mGroupCallStatus.end())
    {
        return NULL;
    }

    return &g_mGroupCallStatus[i64GroupID];
}

BOOL IsTrainBroadcastGroup(INT64 i64GroupID)
{
    GROUP_INFO_T    *pstGroup = GetGroup(i64GroupID);

    if (!pstGroup)
    {
        return FALSE;
    }

    if (pstGroup->ulGroupType == TRAIN_BRD_GROUP)
    {
        return TRUE;
    }

    return FALSE;
}

VOID InitBrdZoneInfo(VECTOR<BRD_ZONE_INFO_T> &vInfo)
{
    g_vBrdZoneInfo.swap(vInfo);
}

VECTOR<BRD_ZONE_INFO_T>& GetBrdZoneInfo()
{
    return g_vBrdZoneInfo;
}

VOID GetBrdZoneInfo(UINT32 ulStationID, VECTOR<BRD_ZONE_INFO_T> &vInfo)
{
    vInfo.clear();
    for (UINT32 i=0; i<g_vBrdZoneInfo.size(); i++)
    {
        if (g_vBrdZoneInfo[i].ulStationID == ulStationID)
        {
            vInfo.push_back(g_vBrdZoneInfo[i]);
        }
    }
}

//---------------------------------------------------------------------------
/**
 * @brief           检查并加入列车组
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
void CheckTrainGroup()
{
    // 如果列车不属于当前调度台管辖范围，则自动退出该列车通话组，否则自动加入、显示该通话组
    VECTOR<TRAIN_INFO_T*>   &vTrainInfo = Train::GetTrainInfo();

    for (UINT32 i=0; i<vTrainInfo.size(); i++)
    {
        TRAIN_INFO_T    *pstTrain = vTrainInfo[i];

        if (Station::IsTrainInDepot(pstTrain) &&
            DCUser::IsStationInDCManageRange(pstTrain->ulStationID))
        {
            SubGroup(pstTrain->i64DefaultGroupID);
        }
        else
        {
            UnSubGroup(pstTrain->i64DefaultGroupID);
        }
    }

    if (!APP::HasDCType(DC_TYPE_DEPOT) &&
        !APP::HasDCType(DC_TYPE_FIX) )
    {
        SubGroup(g_stGroupCfg.i64UpLineTrainGroupID);
        SubGroup(g_stGroupCfg.i64DownLineTrainGroupID);
        SubGroup(g_stGroupCfg.i64MainLineTrainGroupID);
        SubGroup(g_stGroupCfg.i64UpLineTrainBrdGroupID);
        SubGroup(g_stGroupCfg.i64DownLineTrainBrdGroupID);
        SubGroup(g_stGroupCfg.i64MainLineTrainBrdGroupID);
        SubGroup(g_stGroupCfg.i64AllStationGroupID);
    }
}
//---------------------------------------------------------------------------
/**
 * @brief           检查并加入车站组
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
void CheckStationGroup()
{
    if (APP::HasDCType(DC_TYPE_DEPOT) ||
        APP::HasDCType(DC_TYPE_FIX) )
    {
        return;
    }

    VECTOR<STATION_UE_INFO_T*>  &vUEInfo = StationUE::GetStationUE();

    for (UINT32 i=0; i<vUEInfo.size(); i++)
    {
        STATION_UE_INFO_T   *pstUEInfo = vUEInfo[i];
        STATION_INFO_T      *pstStation = Station::GetStationInfo(pstUEInfo->ulStationID);

        if (pstStation && pstStation->ulStationType == STATION_TYPE_NORMAL)
        {
            SubGroup(pstUEInfo->i64DefaultGroupID);
        }
    }

    SubGroup(g_stGroupCfg.i64AllStationGroupID);
}
//---------------------------------------------------------------------------
void SubGroup(INT64 i64GroupID)
{
    UINT32 ulSubStatus = GetGroupSubStatus(i64GroupID);

    if (!GetGroup(i64GroupID) &&
        !DynamicGroup::GetDynamicGroup(i64GroupID))
    {
        return;
    }

    if (ulSubStatus == GROUP_STATUS_TOJOIN ||
        ulSubStatus == GROUP_STATUS_JOIN )
    {
        return;
    }

    if (g_mGroupIdleInfo.find(i64GroupID) != g_mGroupIdleInfo.end())
    {
        UINT32  ulIdleTime = g_mGroupIdleInfo[i64GroupID];

        if (ulIdleTime != 0xffffffff &&
            (ulIdleTime + g_ulKeepOutGroupTime) > gos_get_uptime())
        {
            return;
        }
    }

    /// 这句话打印太多，暂时屏蔽
    /// GosLog(LOG_DETAIL, "group " FORMAT_I64 " will be auto subJoin", i64GroupID);
    UpdateGroupSubStatus(i64GroupID, GROUP_STATUS_TOJOIN, FALSE);
}
//---------------------------------------------------------------------------
void UnSubGroup(INT64 i64GroupID)
{
    UINT32 ulSubStatus = GetGroupSubStatus(i64GroupID);

    if (!GetGroup(i64GroupID) &&
        !DynamicGroup::GetDynamicGroup(i64GroupID))
    {
        return;
    }

    if (ulSubStatus == GROUP_STATUS_TOUNSUB ||
        ulSubStatus == GROUP_STATUS_UNSUB)
    {
        return;
    }

    UpdateGroupSubStatus(i64GroupID, GROUP_STATUS_TOUNSUB, FALSE);
}

VOID InitGroupMarkInfo(VECTOR<GROUP_INFO_T> &vGroup)
{
    g_mGroupMarkInfo.Clear();
    for (UINT32 i=0; i<vGroup.size(); i++)
    {
        g_mGroupMarkInfo.Add(vGroup[i].i64GroupID, vGroup[i]);
    }
}

GROUP_INFO_T* GetGroupMarkInfo(INT64 i64GroupID)
{
    return g_mGroupMarkInfo.GetValue(i64GroupID);
}

UINT32 GetGroupFuncType(INT64 i64GroupID)
{
    GROUP_INFO_T    *pstGroup = g_mGroupMarkInfo.GetValue(i64GroupID);

    if (pstGroup)
    {
        return pstGroup->ulFuncType;
    }

    return GROUP_FUNC_OTHER;
}

INT64 GetMainGroupID()
{
    return g_i64MainGroupID;
}

VOID SetMainGroupID(INT64 i64GroupID)
{
    g_i64MainGroupID = i64GroupID;
}

VOID SetDefaultGroupSpeaker()
{
    MAP<INT64, AnsiString>::iterator it = g_mJoinedGroup.begin();

    for (; it!=g_mJoinedGroup.end();it++)
    {
        if (g_i64MainGroupID != it->first)
        {
            AssignGroupSpeaker(it->first, GetESDKMonSpeaker());
        }
    }
}

VOID SetDefaultGroupMic()
{
    MAP<INT64, AnsiString>::iterator it = g_mJoinedGroup.begin();

    for (; it!=g_mJoinedGroup.end();it++)
    {
        if (g_i64MainGroupID != it->first)
        {
            AssignGroupMic(it->first, GetESDKMainMic());
        }
    }
}

} // namespace Group
