#include "ClientInclude.h"
#include "AudioDev.h"
#include "eSDK.h"

IntSet      g_sSubUser;
IntSet      g_sSubVideoUser;

static VECTOR<DC_USER_INFO_T>               g_vDCUserInfo;
static VECTOR<DC_OPERATOR_INFO_T>           g_vDCOperatorInfo;

static VectorMap<UINT32, STATION_UE_INFO_T> g_mStationUEInfo;
static VectorMap<UINT32, TRAIN_UE_INFO_T>   g_mTrainUEInfo;
static VectorMap<UINT32, PORTAL_UE_INFO_T>  g_mPortalUEInfo;

static VectorMap<UINT32, EVENT_GIS_REPORT_T> g_mGISInfo;

namespace GIS
{
EVENT_GIS_REPORT_T* GetGISInfo(UINT32 ulUserID)
{
    return g_mGISInfo.GetValue(ulUserID);
}

VOID InitGISInfo(UINT32 ulUserID, DOUBLE dLon, DOUBLE dLat)
{
    EVENT_GIS_REPORT_T      stInfo = {0};

    stInfo.ulUserID = ulUserID;
    stInfo.dLongtitude = dLon;
    stInfo.dLatitude = dLat;

    g_mGISInfo.Add(ulUserID, stInfo);
}

VOID UpdateGISInfo(EVENT_GIS_REPORT_T &stEvent)
{
    int     iLatitude = 1000000*stEvent.dLatitude;
    int     iLongtitude = 1000000*stEvent.dLongtitude;

    if (iLatitude == 0 &&
        iLongtitude == 0)
    {
        return;
    }

    g_mGISInfo.Add(stEvent.ulUserID, stEvent);

    SaveGISInfo(stEvent.ulUserID, stEvent.dLongtitude, stEvent.dLatitude);
}

AnsiString GetGISText(DOUBLE dLongtitude, DOUBLE dLatitude)
{
    AnsiString  strInfo;
    int         iLatitude = 1000000*dLatitude;
    int         iLongtitude = 1000000*dLongtitude;

    if (iLatitude == 0 &&
        iLongtitude == 0)
    {
        return "";
    }

    strInfo.printf("经度%.6f 纬度%.6f", dLongtitude, dLatitude);

    return strInfo;
}

AnsiString GetGISText(UINT32 ulUserID)
{
    EVENT_GIS_REPORT_T  *pstInfo = GetGISInfo(ulUserID);

    if (!pstInfo)
    {
        return "";
    }

    return GetGISText(pstInfo->dLongtitude, pstInfo->dLatitude);
}
}

namespace User
{

AnsiString GetUserDisplayName(UINT32 ulUserID, AnsiString strName)
{
    if (strName.IsEmpty())
    {
        return ulUserID;
    }

    if (strName != ulUserID)
    {
        strName = AnsiString(ulUserID) + AnsiString("(") + strName + AnsiString(")");
    }

    return strName;
}

BOOL UpdateUserStatus(EVENT_USER_STATUS_T &stEvent)
{
    UINT32  ulUserID = stEvent.ulUserNumber;

    DC_USER_INFO_T  *pstDCUserInfo = DCUser::GetDCUser(ulUserID);
    if (pstDCUserInfo)
    {
        pstDCUserInfo->ulCallStatus = stEvent.ulStatus;
    }
    else if (g_mTrainUEInfo.HasKey(ulUserID))
    {
        TRAIN_UE_INFO_T *pstInfo = g_mTrainUEInfo.GetValue(ulUserID);

        pstInfo->ulCallStatus = stEvent.ulStatus;
    }
    else if (g_mStationUEInfo.HasKey(ulUserID))
    {
        STATION_UE_INFO_T   *pstInfo = g_mStationUEInfo.GetValue(ulUserID);

        pstInfo->ulCallStatus = stEvent.ulStatus;
    }
    else if (g_mPortalUEInfo.HasKey(ulUserID))
    {
        PORTAL_UE_INFO_T    *pstInfo = g_mPortalUEInfo.GetValue(ulUserID);

        pstInfo->ulCallStatus = stEvent.ulStatus;
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

CHAR* GetUserName(UINT32 ulUserID)
{
    DC_USER_INFO_T  *pstDCUserInfo = DCUser::GetDCUser(ulUserID);

    if (pstDCUserInfo)
    {
        return pstDCUserInfo->acName;
    }

    if (g_mStationUEInfo.HasKey(ulUserID))
    {
        STATION_UE_INFO_T   *pstInfo = g_mStationUEInfo.GetValue(ulUserID);

        return pstInfo->acName;
    }
    else if (g_mTrainUEInfo.HasKey(ulUserID))
    {
        TRAIN_UE_INFO_T *pstInfo = g_mTrainUEInfo.GetValue(ulUserID);

        return pstInfo->acName;
    }
    else if (g_mPortalUEInfo.HasKey(ulUserID))
    {
        PORTAL_UE_INFO_T    *pstInfo = g_mPortalUEInfo.GetValue(ulUserID);

        return pstInfo->acName;
    }

    return "";
}

AnsiString GetUserDisplayName(UINT32 ulUserID)
{
    AnsiString  strName = GetUserName(ulUserID);

    return GetUserDisplayName(ulUserID, strName);
}

BOOL GetUserInfo(UINT32 ulUserID, CHAR *szUserName, UINT32 *pulUserType, UINT32 *pulFuncType,
                 UINT32 *pulStatus, UINT32 *pulCellID, UINT32 *pulCallStatus)
{
    DC_USER_INFO_T  *pstDCUserInfo = DCUser::GetDCUser(ulUserID);

    if (pstDCUserInfo)
    {
        *pulUserType = DC_USER;
        *pulStatus = pstDCUserInfo->ulDevStatus;
        strcpy(szUserName, pstDCUserInfo->acName);
        *pulFuncType = UE_MANAGE;
        *pulCellID = 0;
        *pulCallStatus = pstDCUserInfo->ulCallStatus;
    }
    else if (g_mStationUEInfo.HasKey(ulUserID))
    {
        STATION_UE_INFO_T   *pstInfo = g_mStationUEInfo.GetValue(ulUserID);

        *pulUserType = STATION_UE;
        *pulStatus = pstInfo->ulDevStatus;
        strcpy(szUserName, pstInfo->acName);
        *pulFuncType = UE_STATION;
        *pulCellID = 0;
        *pulCallStatus = pstInfo->ulCallStatus;
    }
    else if (g_mTrainUEInfo.HasKey(ulUserID))
    {
        TRAIN_UE_INFO_T *pstInfo = g_mTrainUEInfo.GetValue(ulUserID);

        *pulUserType = TRAIN_UE;
        *pulStatus = pstInfo->ulDevStatus;
        strcpy(szUserName, pstInfo->acName);
        *pulFuncType = UE_TRAIN;
        *pulCellID = pstInfo->ulCellID;
        *pulCallStatus = pstInfo->ulCallStatus;
    }
    else if (g_mPortalUEInfo.HasKey(ulUserID))
    {
        PORTAL_UE_INFO_T    *pstInfo = g_mPortalUEInfo.GetValue(ulUserID);

        *pulUserType = pstInfo->ulType;
        *pulStatus = pstInfo->ulDevStatus;
        strcpy(szUserName, pstInfo->acName);

        *pulFuncType = pstInfo->ulFuncType;
        *pulCellID = pstInfo->ulCellID;
        *pulCallStatus = pstInfo->ulCallStatus;
    }
    else
    {
        return FALSE;
    }

    if (IsUserOnline(*pulCallStatus))
    {
        *pulStatus = UE_STATUS_OFFLINE;
    }
    else
    {
        *pulStatus = UE_STATUS_ONLINE;
    }

    return TRUE;
}

BOOL IsUserExist(UINT32 ulUserID)
{
    if (DCUser::GetDCUser(ulUserID))
    {
        return TRUE;
    }
    else if (g_mStationUEInfo.HasKey(ulUserID))
    {
        return TRUE;
    }
    else if (g_mTrainUEInfo.HasKey(ulUserID))
    {
        return TRUE;
    }
    else if (g_mPortalUEInfo.HasKey(ulUserID))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL IsUserOnline(UINT32 ulStatus)
{
    if (ulStatus == USER_STATUS_UNKNOWN ||
        ulStatus == USER_STATUS_REGFAIL )
    {
        return FALSE;
    }

    return TRUE;
}

AnsiString GetUserAlias(UINT32 ulUserID)
{
    AnsiString  strName = GetUserName(ulUserID);

    if (strName.IsEmpty())
    {
        if (ulUserID == INVALID_USER_ID)
        {
            return "";
        }

        return ulUserID;
    }

    return strName;
}

AnsiString GetUserAlias(AnsiString strUserID)
{
    UINT32      ulUserID = strUserID.ToIntDef(INVALID_USER_ID);

    if (ulUserID == INVALID_USER_ID)
    {
        return "";
    }

    return GetUserAlias(ulUserID);
}

} // namespace User

namespace StationUE
{

VOID InitStationUE(VectorMap<UINT32, STATION_UE_INFO_T> &mInfo)
{
    g_mStationUEInfo.Clear();
    g_mStationUEInfo.AddAll(mInfo);
}

STATION_UE_INFO_T* GetStationUEByStationID(UINT32 ulStationID)
{
    for (UINT32 i=0; i<g_mStationUEInfo.Size(); i++)
    {
        STATION_UE_INFO_T   *pstInfo = g_mStationUEInfo.GetValueByIndex(i);

        if (pstInfo->ulStationID == ulStationID)
        {
            return pstInfo;
        }
    }

    return NULL;
}

STATION_UE_INFO_T* GetStationUEByGroupID(INT64 i64GroupID)
{
    for (UINT32 i=0; i<g_mStationUEInfo.Size(); i++)
    {
        STATION_UE_INFO_T   *pstInfo = g_mStationUEInfo.GetValueByIndex(i);

        if (pstInfo->i64DefaultGroupID == i64GroupID)
        {
            return pstInfo;
        }
    }

    return NULL;
}

VECTOR<STATION_UE_INFO_T*>& GetStationUE()
{
    return g_mStationUEInfo.GetValueList();
}

STATION_UE_INFO_T* GetStationUE(UINT32 ulUserID)
{
    return g_mStationUEInfo.GetValue(ulUserID);
}

VOID GetNormalStationUE(VECTOR<UINT32> &vUserID)
{
    STATION_INFO_T    *pstStation;

    vUserID.clear();
    for (UINT32 i=0; i<g_mStationUEInfo.Size(); i++)
    {
        pstStation = Station::GetStationInfo(g_mStationUEInfo[i]->ulStationID);

        if (pstStation && pstStation->ulStationType == STATION_TYPE_NORMAL)
        {
            vUserID.push_back(g_mStationUEInfo[i]->ulUserID);
        }
    }
}

} // namespace StationUE

namespace TrainUE
{
VOID InitTrainUE(VectorMap<UINT32, TRAIN_UE_INFO_T> &mInfo)
{
    g_mTrainUEInfo.Clear();
    g_mTrainUEInfo.AddAll(mInfo);
}

VOID GetNormalTrainUE(VECTOR<UINT32> &vUserID)
{
    TRAIN_INFO_T    *pstTrain;

    vUserID.clear();
    for (UINT32 i=0; i<g_mTrainUEInfo.Size(); i++)
    {
        pstTrain = Train::GetTrainInfo(g_mTrainUEInfo[i]->ulTrainUnitID);

        if (pstTrain && pstTrain->ulTrainType == TRAIN_TYPE_NORMAL)
        {
            vUserID.push_back(g_mTrainUEInfo[i]->ulUserID);
        }
    }
}

TRAIN_UE_INFO_T* GetTrainUE(UINT32 ulUserID)
{
    if (!g_mTrainUEInfo.HasKey(ulUserID))
    {
        return NULL;
    }

    return g_mTrainUEInfo.GetValue(ulUserID);
}

VECTOR<TRAIN_UE_INFO_T*>& GetTrainUE()
{
    return g_mTrainUEInfo.GetValueList();
}

VOID GetTrainUserIDOfStation(UINT32 ulStationID, VECTOR<UINT32> &vUser)
{
    VECTOR<TRAIN_INFO_T*>   vTrainInfo;
    UINT32                  ulUserID1;
    UINT32                  ulUserID2;

    vUser.clear();
    Train::GetTrainOfStation(ulStationID, vTrainInfo);
    for (UINT32 i=0; i<vTrainInfo.size(); i++)
    {
        Train::GetTrainUserID(vTrainInfo[i]->ulTrainUnitID, &ulUserID1, &ulUserID2);

        if (ulUserID1 != INVALID_USER_ID)
        {
            vUser.push_back(ulUserID1);
        }

        if (ulUserID2 != INVALID_USER_ID)
        {
            vUser.push_back(ulUserID2);
        }
    }
}

} // namespace TrainUE

namespace PortalUE
{
VOID InitPortalUE(VectorMap<UINT32, PORTAL_UE_INFO_T> &mInfo)
{
    g_mPortalUEInfo.Clear();
    g_mPortalUEInfo.AddAll(mInfo);
}

VOID GetPortalUE(UINT32 ulStationID, VECTOR<PORTAL_UE_INFO_T*> &vUser)
{
    vUser.clear();
    for (UINT32 i=0; i<g_mPortalUEInfo.Size(); i++)
    {
        PORTAL_UE_INFO_T    *pstInfo = g_mPortalUEInfo.GetValueByIndex(i);

        if (pstInfo->ulStationID == ulStationID)
        {
            vUser.push_back(pstInfo);
        }
    }
}

VECTOR<PORTAL_UE_INFO_T*>& GetPortalUE()
{
    return g_mPortalUEInfo.GetValueList();
}

VOID GetPortalUE(VECTOR<PORTAL_UE_INFO_T*> &vUser)
{
    g_mPortalUEInfo.GetValueList(vUser);
}

PORTAL_UE_INFO_T* GetPortalUE(UINT32 ulUserID)
{
    return g_mPortalUEInfo.GetValue(ulUserID);
}

} // namespace PortalUE

namespace DCUser
{
VOID InitDCUser(VECTOR<DC_USER_INFO_T> &vUser)
{
    g_vDCUserInfo.swap(vUser);
}

UINT32 GetDCUserNum()
{
    return g_vDCUserInfo.size();
}

DC_USER_INFO_T* GetDCUser(UINT32 ulUserID)
{
    for (UINT32 i=0; i<g_vDCUserInfo.size(); i++)
    {
        if (g_vDCUserInfo[i].ulUserID == ulUserID)
        {
            return &g_vDCUserInfo[i];
        }
    }

    return NULL;
}

VOID GetDCUser(VECTOR<DC_USER_INFO_T*> &vUser)
{
    vUser.clear();

    for (UINT32 i=0; i<g_vDCUserInfo.size(); i++)
    {
        vUser.push_back(&g_vDCUserInfo[i]);
    }
}

// 行调调度台
VOID GetTrainDCUser(VECTOR<UINT32> &vUserID)
{
    vUserID.clear();
    for (UINT32 i=0; i<g_vDCUserInfo.size(); i++)
    {
        if (g_vDCUserInfo[i].ulDCType & DC_TYPE_TRAIN)
        {
            vUserID.push_back(g_vDCUserInfo[i].ulUserID);
        }
    }
}

// 车辆段调度台
VOID GetDepotDCUser(VECTOR<UINT32> &vUserID)
{
    vUserID.clear();
    for (UINT32 i=0; i<g_vDCUserInfo.size(); i++)
    {
        if (g_vDCUserInfo[i].ulDCType & DC_TYPE_DEPOT ||
            g_vDCUserInfo[i].ulDCType & DC_TYPE_FIX )
        {
            vUserID.push_back(g_vDCUserInfo[i].ulUserID);
        }
    }
}

BOOL IsStationInDCManageRange(UINT32 ulStationID)
{
    for (UINT32 i=0; i<g_vDCUserInfo.size(); i++)
    {
        if (g_vDCUserInfo[i].ulUserID == APP::GetCurrDCUserID())
        {
            if (g_vDCUserInfo[i].ulStationNum == 0)
            {
                return TRUE;
            }

            for (UINT32 j=0; j<g_vDCUserInfo[i].ulStationNum; j++)
            {
                if (g_vDCUserInfo[i].aulStationList[j] == ulStationID)
                {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}
} // namespace DCUser

namespace DCOperator
{
VOID InitDCOperator(VECTOR<DC_OPERATOR_INFO_T> &vInfo)
{
    g_vDCOperatorInfo.swap(vInfo);
}

VOID GetDCOperator(VECTOR<DC_OPERATOR_INFO_T> &vInfo)
{
    vInfo.clear();
    vInfo.assign(g_vDCOperatorInfo.begin(), g_vDCOperatorInfo.end());
}

DC_OPERATOR_INFO_T* GetDCOperator(AnsiString strDCOperatorID)
{
    for (UINT32 i=0; i<g_vDCOperatorInfo.size(); i++)
    {
        if (strDCOperatorID == g_vDCOperatorInfo[i].acOperatorID)
        {
            return &g_vDCOperatorInfo[i];
        }
    }

    return NULL;
}

AnsiString GetDCOperatorName(AnsiString strDCOperatorID)
{
    DC_OPERATOR_INFO_T  *pstInfo = GetDCOperator(strDCOperatorID);

    if (!pstInfo)
    {
        return strDCOperatorID;
    }

    return pstInfo->acName;
}

} // namespace DCOperator
