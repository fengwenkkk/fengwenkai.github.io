#include "ClientInclude.h"
#include "Station.h"

static VectorMap<UINT32, STATION_INFO_T>    g_vStationInfo;
static VectorMap<UINT32, DEPOT_INFO_T>      g_vDepotInfo;

Dict                                        g_DictDepotID;

namespace Depot
{

void InitDepot(VECTOR<DEPOT_INFO_T> &vDepotInfo)
{
    DEPOT_INFO_T    stMainLine = {0};

    g_vDepotInfo.Clear();
    g_DictDepotID.Clear();

    for (UINT32 i=0; i<vDepotInfo.size(); i++)
    {
        g_vDepotInfo.Add(vDepotInfo[i].ulDepotID, vDepotInfo[i]);

        g_DictDepotID.Add(vDepotInfo[i].ulDepotID, vDepotInfo[i].acDepotName);
    }
}

VECTOR<DEPOT_INFO_T*>& GetDepotInfo()
{
    return g_vDepotInfo.GetValueList();
}

DEPOT_INFO_T* GetDepotInfo(UINT32 ulDepotID)
{
    return g_vDepotInfo.GetValue(ulDepotID);
}

CHAR* GetDepotName(UINT32 ulDepotID)
{
    if (ulDepotID == DEPOT_ID_MAINLINE)
    {
        return "正线";
    }

    DEPOT_INFO_T    *pstInfo = g_vDepotInfo.GetValue(ulDepotID);

    if (!pstInfo)
    {
        return "";
    }

    return pstInfo->acDepotName;
}

CHAR* GetDepotName(TRAIN_INFO_T *pstTrain)
{
    UINT32          ulDepotID = Station::GetDepotID(pstTrain);

    if (ulDepotID == DEPOT_ID_MAINLINE)
    {
        return "正线";
    }

    DEPOT_INFO_T    *pstInfo = g_vDepotInfo.GetValue(ulDepotID);

    if (!pstInfo)
    {
        return "";
    }

    return pstInfo->acDepotName;
}

// 与当前登录时选择的Depot类型是否相同
BOOL IsSameDepot(UINT32 ulDepotID)
{
    // 调度台登录时，需要选择指定的段场
    if (ManageOneDepot())
    {
        return ulDepotID == APP::GetDepotID();
    }

    // 当调度台非行调、乘客调、段场调时为DEPOT_ID_UNKNOWN
    if (APP::GetDepotID() == DEPOT_ID_UNKNOWN)
    {
        return TRUE;
    }

    // 调度台登录时不需要选择段场，段场合并处理
    if (ulDepotID == DEPOT_ID_MAINLINE &&
        APP::GetDepotID() == DEPOT_ID_MAINLINE)
    {
        return TRUE;
    }
    else if (ulDepotID != DEPOT_ID_MAINLINE &&
             APP::GetDepotID() != DEPOT_ID_MAINLINE)
    {
        return TRUE;
    }

    return FALSE;
}
}

namespace Section
{
static VECTOR<SECTION_INFO_T>   g_vSectionInfo;

void InitSection(VECTOR<SECTION_INFO_T> &vInfo)
{
    g_vSectionInfo.clear();
    g_vSectionInfo.assign(vInfo.begin(), vInfo.end());
}

void GetDepotSection(UINT32 ulDepotID, VECTOR<SECTION_INFO_T*> &vInfo)
{
    vInfo.clear();

    VECTOR<STATION_INFO_T*>     vDepotStation;

    Station::GetDepotStationInfo(vDepotStation);

    for (UINT32 i=0; i<g_vSectionInfo.size(); i++)
    {
        for (UINT32 j=0; j<vDepotStation.size(); j++)
        {
            if (vDepotStation[j]->ulDepotID == ulDepotID &&
                vDepotStation[j]->ulStationID == g_vSectionInfo[i].ulStationID)
            {
                vInfo.push_back(&g_vSectionInfo[i]);
                break;
            }
        }
    }
}

SECTION_INFO_T* GetSection(AnsiString strSectionName)
{
    for (UINT32 i=0; i<g_vSectionInfo.size(); i++)
    {
        if (strcmp(g_vSectionInfo[i].acSectionName, strSectionName.c_str()) == 0)
        {
            return &g_vSectionInfo[i];
        }
    }

    return NULL;
}

SECTION_INFO_T* GetSection(UINT32 ulSectionID)
{
    for (UINT32 i=0; i<g_vSectionInfo.size(); i++)
    {
        if (g_vSectionInfo[i].ulSectionID == ulSectionID)
        {
            return &g_vSectionInfo[i];
        }
    }

    return NULL;
}

void GetSection(UINT32 ulStationID, UINT32 ulDirection, BOOL bInDepotTrack, VECTOR<SECTION_INFO_T*> &vInfo)
{
    bool    bIsDepotStation = Station::IsDepotStation(ulStationID);
    bool    bMatch;

    vInfo.clear();
    for (UINT32 i=0; i<g_vSectionInfo.size(); i++)
    {
        bMatch = false;
        if (g_vSectionInfo[i].ulStationID == ulStationID)
        {
            if (bIsDepotStation)
            {
                if (g_vSectionInfo[i].x1 >= 0)
                {
                    bMatch = true;
                }
            }
            else if (g_vSectionInfo[i].ulDirection == ulDirection &&
                     g_vSectionInfo[i].bInDepotTrack == bInDepotTrack )
            {
                bMatch = true;
            }
        }

        if (bMatch)
        {
            vInfo.push_back(&g_vSectionInfo[i]);
        }
    }
}
};

namespace Station
{
void InitDictStation(Dict &DictStation)
{
    VECTOR<STATION_INFO_T*>      vInfo;

    GetStationInfo(vInfo);

    DictStation.Clear();
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        DictStation.Add(vInfo[i]->ulStationID, vInfo[i]->acStationName);
    }
}

void InitStation(VECTOR<STATION_INFO_T> &vStationInfo)
{
    g_vStationInfo.Clear();

    for (UINT32 i=0; i<vStationInfo.size(); i++)
    {
        g_vStationInfo.Add(vStationInfo[i].ulStationID, vStationInfo[i]);
    }

    //InitDictStation();
}

UINT32 GetNormalStationNum()
{
    UINT32  ulNum = 0;

    for (UINT32 i=0; i<g_vStationInfo.Size(); i++)
    {
        if (g_vStationInfo[i]->ulStationType == STATION_TYPE_NORMAL)
        {
            ulNum++;
        }
    }

    return ulNum;
}

void GetNormalStation(VECTOR<STATION_INFO_T*> &vInfo)
{
    for (UINT32 i=0; i<g_vStationInfo.Size(); i++)
    {
        if (g_vStationInfo[i]->ulStationType == STATION_TYPE_NORMAL)
        {
            vInfo.push_back(g_vStationInfo[i]);
        }
    }
}

void GetStationInfo(VECTOR<STATION_INFO_T*> &vStationInfo)
{
    g_vStationInfo.GetValueList(vStationInfo);
}

VECTOR<STATION_INFO_T*>& GetStationInfo()
{
    return g_vStationInfo.GetValueList();
}

STATION_INFO_T* GetStationInfo(UINT32 ulStationID)
{
    return g_vStationInfo.GetValue(ulStationID);
}

STATION_INFO_T* GetStationByGroupID(INT64 i64GroupID)
{
    STATION_UE_INFO_T   *pstUE = StationUE::GetStationUEByGroupID(i64GroupID);

    if (!pstUE)
    {
        return NULL;
    }

    return GetStationInfo(pstUE->ulStationID);
}

void GetDepotStationInfo(VECTOR<STATION_INFO_T*> &vInfo)
{
    vInfo.clear();
    for (UINT32 i=0; i<g_vStationInfo.Size(); i++)
    {
        if (STATION_TYPE_NORMAL != g_vStationInfo[i]->ulStationType)
        {
            vInfo.push_back(g_vStationInfo[i]);
        }
    }
}

int GetStationIndex(UINT32 ulStationID)
{
    STATION_INFO_T  *pstStation = GetStationInfo(ulStationID);

    if (!pstStation)
    {
        return -1;
    }

    return pstStation->ulStationIndex;
}

AnsiString GetStationName(UINT32 ulStationID)
{
    STATION_INFO_T  *pstStation = GetStationInfo(ulStationID);

    if (!pstStation)
    {
        return "";
    }

    return pstStation->acStationName;
}

void GetInterLockStation(VECTOR<STATION_INFO_T*> &vInfo)
{
    vInfo.clear();
    for (UINT32 i=0; i<g_vStationInfo.Size(); i++)
    {
        if (g_vStationInfo[i]->ulInterLockStationID == g_vStationInfo[i]->ulStationID)
        {
            vInfo.push_back(g_vStationInfo[i]);
        }
    }
}

void GetInterLockStation(UINT32 ulInterLockStationID, VECTOR<STATION_INFO_T*> &vInfo)
{
    vInfo.clear();
    for (UINT32 i=0; i<g_vStationInfo.Size(); i++)
    {
        if (ulInterLockStationID == g_vStationInfo[i]->ulInterLockStationID)
        {
            vInfo.push_back(g_vStationInfo[i]);
        }
    }
}

INT64 GetStationGroupID(UINT32 ulStationID)
{
    STATION_UE_INFO_T   *pstInfo = StationUE::GetStationUEByStationID(ulStationID);

    if (!pstInfo)
    {
        return INVALID_GROUP_ID;
    }

    return pstInfo->i64DefaultGroupID;
}

// 列车是否在调度台所在的车辆段/停车场
BOOL IsTrainInDepot(TRAIN_INFO_T *pstTrain)
{
    if (!pstTrain)
    {
        return TRUE;
    }

    UINT32  ulRegisterDepotID = pstTrain->ulRegisterDepotID;

    /*
    if (pstTrain->ulRegisterDepotID == APP::GetDepotID())
    {
        return TRUE;
    }

    if (pstTrain->ulRegisterDepotID == DEPOT_ID_AUTO)
    {
        STATION_INFO_T  *pstStation = Station::GetStationInfo(pstTrain->ulStationID);

        if (pstStation &&
            pstStation->ulDepotID == APP::GetDepotID())
        {
            return TRUE;
        }
    }

    return FALSE;
     */

    if (ulRegisterDepotID == DEPOT_ID_AUTO)
    {
        STATION_INFO_T  *pstStation = Station::GetStationInfo(pstTrain->ulStationID);

        if (!pstStation)
        {
            return FALSE;
        }

        ulRegisterDepotID = pstStation->ulDepotID;
    }

    if (Depot::IsSameDepot(ulRegisterDepotID) )
    {
        return TRUE;
    }

    return FALSE;
}

UINT32 GetDepotID(TRAIN_INFO_T *pstTrain)
{
    if (!pstTrain)
    {
        return DEPOT_ID_UNKNOWN;
    }

    if (pstTrain->ulRegisterDepotID != DEPOT_ID_UNKNOWN)
    {
        return pstTrain->ulRegisterDepotID;
    }

    STATION_INFO_T    *pstStation = Station::GetStationInfo(pstTrain->ulStationID);

    if (!pstStation)
    {
        return DEPOT_ID_UNKNOWN;
    }

    return pstStation->ulDepotID;
}

/**
 * @brief           获取包括本车站临近的两个车站的ID
 * @param           ulStationID    [in] 获取到的车站ID
 * @return          std::vector<UINT32> 返回的车站ID
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
std::vector<UINT32> GetAdjacentStationID(UINT32 ulStationID)
{
    std::vector<UINT32> vecStationID;
    if (ulStationID == 0)
    {
        for (UINT32 index = ulStationID; index < ulStationID + 1; ++index)
        {
            STATION_INFO_T *pStation = g_vStationInfo.GetValue(index);
            if (pStation)
            {
//                GosLog(LOG_DETAIL, "Get StationID(%u) info", pStation->ulStationID);
                vecStationID.push_back(pStation->ulStationID);
            }
        }
    }
    else
    {
        for (UINT32 index = ulStationID - 1; index < ulStationID + 2; ++index)
        {
            STATION_INFO_T *pStation = g_vStationInfo.GetValue(index);
            if (pStation)
            {
//                GosLog(LOG_DETAIL, "Get StationID(%u) info", pStation->ulStationID);
                vecStationID.push_back(pStation->ulStationID);
            }
        }
    }

    return vecStationID;
}

bool IsDepotStation(UINT32 ulStationID)
{
    VECTOR<STATION_INFO_T*>     vDepotStation;

    Station::GetDepotStationInfo(vDepotStation);

    for (UINT32 i=0; i<vDepotStation.size(); i++)
    {
        if (vDepotStation[i]->ulStationID == ulStationID)
        {
            return true;
        }
    }

    return false;
}

}  // namespace Station


namespace DestInfo
{
static std::vector<DEST_INFO_T>     g_vDestInfo;

void InitDestInfo(std::vector<DEST_INFO_T> &vDestInfo)
{
    g_vDestInfo.swap(vDestInfo);
}

UINT32 GetDestStationID(AnsiString strDestID)
{
    for (UINT32 i=0; i<g_vDestInfo.size(); i++)
    {
        if (strDestID == g_vDestInfo[i].ulDestID)
        {
            return g_vDestInfo[i].ulStationID;
        }
    }

    return INVALID_STATION_ID;
}

// 判断DestID是否与DepotID相匹配
bool IsValidDestID(UINT32 ulDepotID, AnsiString strDestID)
{
    UINT32  ulStationID = GetDestStationID(strDestID);

    if (ulStationID == INVALID_STATION_ID)
    {
        return false;
    }

    STATION_INFO_T  *pstStation = Station::GetStationInfo(ulStationID);

    if (!pstStation)
    {
        return false;
    }

    return pstStation->ulDepotID == ulDepotID;
}

void InitDictDest(UINT32 ulStationID, Dict &DictDest)
{
    DictDest.Clear();

    char        acName[128];

    for (UINT32 i=0; i<g_vDestInfo.size(); i++)
    {
        if (ulStationID == g_vDestInfo[i].ulStationID)
        {
            sprintf(acName, "%u - %s", g_vDestInfo[i].ulDestID, g_vDestInfo[i].acDestName);
            DictDest.Add(g_vDestInfo[i].ulDestID, acName);
        }
    }
}

void InitDictDest(Dict &DictDest)
{
    DictDest.Clear();

    char        acName[128];

    for (UINT32 i=0; i<g_vDestInfo.size(); i++)
    {
        sprintf(acName, "%u - %s", g_vDestInfo[i].ulDestID, g_vDestInfo[i].acDestName);
        DictDest.Add(g_vDestInfo[i].ulDestID, acName);
    }
}

}
