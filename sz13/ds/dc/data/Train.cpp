#include "vcl.h"
#include "ClientInclude.h"
#include "Train.h"

static VectorMap<UINT32, TRAIN_INFO_T>  g_vTrainInfo;

namespace Train
{
UINT32  g_ulATSDataTimeDiff = 0;

UINT32 GetATSDataTimeDiff()
{
    return g_ulATSDataTimeDiff;
}

AnsiString GetTrainStationPosText(INT32 iTrainStationPos)
{
    if (iTrainStationPos == TRAIN_PARK)     return "停靠车站";
    if (iTrainStationPos == TRAIN_COME)     return "即将入站";
    if (iTrainStationPos == TRAIN_LEAVE)    return "已经出站";

    return "";
}

void InitTrainInfo(VECTOR<TRAIN_INFO_T> &vTrainInfo, UINT32 ulATSDataTimeDiff)
{
    g_vTrainInfo.Clear();

    for (UINT32 i=0; i<vTrainInfo.size(); i++)
    {
        g_vTrainInfo.Add(vTrainInfo[i].ulTrainUnitID, vTrainInfo[i]);
    }

    g_ulATSDataTimeDiff = ulATSDataTimeDiff;
}

void GetTrainInfo(VECTOR<TRAIN_INFO_T*> &vTrainInfo)
{
    g_vTrainInfo.GetValueList(vTrainInfo);
}

void GetTrainInfo(VECTOR<TRAIN_INFO_T> &vTrainInfo)
{
    vTrainInfo.clear();

    for (UINT32 i=0; i<g_vTrainInfo.Size(); i++)
    {
        TRAIN_INFO_T    *pstTrain = g_vTrainInfo.GetValueByIndex(i);

        vTrainInfo.push_back(*pstTrain);
    }
}

VECTOR<TRAIN_INFO_T*>& GetTrainInfo()
{
    return g_vTrainInfo.GetValueList();
}

void GetTrainOfStation(UINT32 ulStationID, VECTOR<TRAIN_INFO_T*> &vTrainInfo)
{
    vTrainInfo.clear();

    for (UINT32 i=0; i<g_vTrainInfo.Size(); i++)
    {
        if (ulStationID == g_vTrainInfo[i]->ulStationID)
        {
            vTrainInfo.push_back(g_vTrainInfo[i]);
        }
    }
}

void GetTrainOfInterLockStation(UINT32 ulInterLockStationID, VECTOR<TRAIN_INFO_T*> &vInfo)
{
    VECTOR<STATION_INFO_T*>     vStation;
    VECTOR<TRAIN_INFO_T*>       vTrain;

    Station::GetInterLockStation(ulInterLockStationID, vStation);
    vInfo.clear();

    for (UINT32 i=0; i<vStation.size(); i++)
    {
        GetTrainOfStation(vStation[i]->ulStationID, vTrain);

        for (UINT32 j=0; j<vTrain.size(); j++)
        {
            vInfo.push_back(vTrain[j]);
        }
    }
}

TRAIN_INFO_T* GetTrainInfo(UINT32 ulTrainUnitID)
{
    return g_vTrainInfo.GetValue(ulTrainUnitID);
}

AnsiString GetTrainUnitIDText(UINT32 ulTrainUnitID)
{
    TRAIN_INFO_T    *pstTrain = GetTrainInfo(ulTrainUnitID);

    return GetTrainUnitIDText(pstTrain);
}

AnsiString GetTrainUnitIDText(TRAIN_INFO_T *pstTrain)
{
    if (!pstTrain)
    {
        return "";
    }

    if (pstTrain->acTrainName[0] == '\0')
    {
        return pstTrain->ulTrainUnitID;
    }

    return pstTrain->acTrainName;
}

// 运营列车
VOID GetNoramlTrainInfo(VECTOR<TRAIN_INFO_T*> &vTrainInfo)
{
    vTrainInfo.clear();

    for (UINT32 i=0; i<g_vTrainInfo.Size(); i++)
    {
        if (g_vTrainInfo[i]->ulTrainType == TRAIN_TYPE_NORMAL)
        {
            vTrainInfo.push_back(g_vTrainInfo[i]);
        }
    }
}

// 车辆段/包括工程车
VOID GetDepotTrainInfo(UINT32 ulDepotID, VECTOR<TRAIN_INFO_T*> &vTrainInfo)
{
    vTrainInfo.clear();

    for (UINT32 i=0; i<g_vTrainInfo.Size(); i++)
    {
        TRAIN_INFO_T    *pstTrain = g_vTrainInfo[i];

        if (Station::GetDepotID(pstTrain) == ulDepotID)
        {
            vTrainInfo.push_back(pstTrain);
        }
    }
}

UINT32 GetNormalTrainNum()
{
    UINT32  ulNum = 0;

    for (UINT32 i=0; i<g_vTrainInfo.Size(); i++)
    {
        if (g_vTrainInfo[i]->ulTrainType == TRAIN_TYPE_NORMAL)
        {
            ulNum++;
        }
    }

    return ulNum;
}

BOOL HaveTrainAlarm(UINT32 ulUnitID)
{
    // TODO
    return FALSE;
}

VOID GetTrainUserID(UINT32 ulUnitID, UINT32 *pulUserID1, UINT32 *pulUserID2)
{
    *pulUserID1 = INVALID_USER_ID;
    *pulUserID2 = INVALID_USER_ID;

    VECTOR<TRAIN_UE_INFO_T*> &vTrainUE = TrainUE::GetTrainUE();

    for (UINT32 i=0; i<vTrainUE.size(); i++)
    {
        TRAIN_UE_INFO_T    *pstInfo = vTrainUE[i];

        if (pstInfo->ulTrainUnitID == ulUnitID)
        {
            if (*pulUserID1 == INVALID_USER_ID)
            {
                *pulUserID1 = pstInfo->ulUserID;
            }
            else if (*pulUserID2 == INVALID_USER_ID)
            {
                *pulUserID2 = pstInfo->ulUserID;
                break;
            }
        }
    }
}

VOID GetTrainUserID(UINT32 ulUnitID, VECTOR<UINT32> &vUserID)
{
    vUserID.clear();

    VECTOR<TRAIN_UE_INFO_T*> &vTrainUE = TrainUE::GetTrainUE();

    for (UINT32 i=0; i<vTrainUE.size(); i++)
    {
        TRAIN_UE_INFO_T    *pstInfo = vTrainUE[i];

        if (pstInfo->ulTrainUnitID == ulUnitID)
        {
            vUserID.push_back(pstInfo->ulUserID);
        }
    }
}

// 列车当前正工作的群组  // TODO 是否要根据列车上报的组为准
INT64 GetTrainGroupID(TRAIN_INFO_T *pstTrain)
{
    if (!pstTrain)
    {
        return INVALID_GROUP_ID;
    }

    return pstTrain->i64DefaultGroupID;
}

INT64 GetTrainGroupID(UINT32 ulTrainUnitID)
{
    TRAIN_INFO_T    *pstTrain = GetTrainInfo(ulTrainUnitID);

    return GetTrainGroupID(pstTrain);
}

TRAIN_INFO_T* GetTrainInfo(INT64 i64GroupID)
{
    for (UINT32 i=0; i<g_vTrainInfo.Size(); i++)
    {
        if (g_vTrainInfo[i]->i64DefaultGroupID == i64GroupID)
        {
            return g_vTrainInfo[i];
        }
    }

    return NULL;
}

TRAIN_INFO_T* GetTrainInfoByTrainID(CHAR *szTrainID)
{
    for (UINT32 i=0; i<g_vTrainInfo.Size(); i++)
    {
        if (strcmp(g_vTrainInfo[i]->acTrainID, szTrainID) == 0)
        {
            return g_vTrainInfo[i];
        }
    }

    return NULL;
}

UINT32 GetTrainUnitID(INT64 i64GroupID)
{
    TRAIN_INFO_T    *pstTrain = GetTrainInfo(i64GroupID);

    if (!pstTrain)
    {
        return INVALID_TRAIN_UNIT_ID;
    }

    return pstTrain->ulTrainUnitID;
}

VOID ClearTrainBroadcastMember(INT64 i64GroupID)
{
    VECTOR<UINT32>  vCurrUser;
    VECTOR<INT64>   vCurrGroup;

    if (!eSDK_GetGroupMember(i64GroupID, vCurrUser, vCurrGroup))
    {
        return;
    }

    if (vCurrUser.size() > 0)
    {
        eSDK_DelDynamicGroupUser(i64GroupID, vCurrUser);
    }
}

BOOL InitTrainBroadcastMember(INT64 i64GroupID, VECTOR<UINT32> &vUser)
{
    VECTOR<UINT32>  vCurrUser;
    VECTOR<INT64>   vCurrGroup;
    VECTOR<UINT32>  vNewUser;

    if (vUser.size() == 0)
    {
        return FALSE;
    }

    vNewUser.assign(vUser.begin(), vUser.end());

    if (!eSDK_GetGroupMember(i64GroupID, vCurrUser, vCurrGroup))
    {
        return FALSE;
    }

    for (int i=vCurrUser.size()-1; i>=0; i--)
    {
        if (vCurrUser[i] == APP::GetCurrDCUserID())
        {
            vCurrUser.erase(vCurrUser.begin() + i);
            continue;
        }

        for (UINT32 j=0; j<vNewUser.size(); j++)
        {
            if (vCurrUser[i] == vNewUser[j])
            {
                vCurrUser.erase(vCurrUser.begin() + i);
                vNewUser.erase(vNewUser.begin() + j);
                break;
            }
        }
    }

    if (vCurrUser.size() > 0 || vNewUser.size() > 0)
    {
        if (!eSDK_SetDynamicGroupUser(i64GroupID, vNewUser, vCurrUser))
        {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL GetTrainRegisterState(UINT32 ulTrainUnitID)
{
    TRAIN_INFO_T    *pstTrain = GetTrainInfo(ulTrainUnitID);

    if (!pstTrain)
    {
        return FALSE;
    }

    return pstTrain->bRegister;
}

BOOL GetTrainRegisterInfo(UINT32 ulTrainUnitID, BOOL *pbRegister, UINT32 *pulDepotID)
{
    TRAIN_INFO_T    *pstTrain = GetTrainInfo(ulTrainUnitID);

    if (!pstTrain)
    {
        return FALSE;
    }

    *pbRegister = pstTrain->bRegister;
    *pulDepotID = Station::GetDepotID(pstTrain);

    return TRUE;
}

BOOL SetTrainRegisterInfo(UINT32 ulTrainUnitID, BOOL bRegister, UINT32 ulDepotID)
{
    TRAIN_INFO_T    *pstTrain = GetTrainInfo(ulTrainUnitID);

    if (!pstTrain)
    {
        return FALSE;
    }

    pstTrain->bRegister = bRegister;
    pstTrain->ulRegisterDepotID = ulDepotID;

    // 自动加入或退出组
    Group::CheckTrainGroup();

    return TRUE;
}

// 需要根据各线路实际生成规则生成
AnsiString GetTrainID(TRAIN_INFO_T *pstTrain)
{
    return pstTrain->acTrainID;//+2;
}

BOOL IsTrainGroup(INT64 i64GroupID)
{
    TRAIN_INFO_T    *pstTrain = GetTrainInfo(i64GroupID);

    return (pstTrain != NULL);
}

AnsiString GetTrainAlias(UINT32 ulTrainUnitID, CHAR *szTrainID)
{
    return AnsiString("列车") + AnsiString(ulTrainUnitID);
}

AnsiString GetTrainAlias(TRAIN_INFO_T *pstTrain)
{
    return GetTrainAlias(pstTrain->ulTrainUnitID, pstTrain->acTrainID);
}

UINT32 GetTrainDepotID(UINT32 ulTrainUnitID)
{
    TRAIN_INFO_T    *pstTrain = GetTrainInfo(ulTrainUnitID);

    if (!pstTrain)
    {
        return DEPOT_ID_UNKNOWN;
    }

    STATION_INFO_T  *pstStation = Station::GetStationInfo(pstTrain->ulStationID);

    if (!pstStation)
    {
        return DEPOT_ID_UNKNOWN;
    }

    return pstStation->ulDepotID;
}

} // namespace Train
