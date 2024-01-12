#ifndef UserH
#define UserH

#include "eSDK.h"

extern IntSet          g_sSubUser;
extern IntSet          g_sSubVideoUser;

namespace GIS
{

VOID InitGISInfo(UINT32 ulUserID, DOUBLE dLon, DOUBLE dLat);
VOID UpdateGISInfo(EVENT_GIS_REPORT_T &stEvent);
AnsiString GetGISText(DOUBLE dLongtitude, DOUBLE dLatitude);
AnsiString GetGISText(UINT32 ulUserID);
}

namespace User
{
AnsiString GetUserDisplayName(UINT32 ulUserID, AnsiString strName);

BOOL IsUserExist(UINT32 ulUserID);
BOOL IsUserOnline(UINT32 ulStatus);
CHAR* GetUserName(UINT32 ulUserID);
AnsiString GetUserDisplayName(UINT32 ulUserID);

BOOL GetUserInfo(UINT32 ulUserID, CHAR *szUserName, UINT32 *pulUserType, UINT32 *pulFuncType,
                 UINT32 *pulStatus, UINT32 *pulCellID, UINT32 *pulCallStatus);
BOOL UpdateUserStatus(EVENT_USER_STATUS_T &stEvent);

AnsiString GetUserAlias(UINT32 ulUserID);
AnsiString GetUserAlias(AnsiString strUserID);

}

namespace StationUE
{

VOID InitStationUE(VectorMap<UINT32, STATION_UE_INFO_T> &mInfo);
STATION_UE_INFO_T* GetStationUE(UINT32 ulUserID);
STATION_UE_INFO_T* GetStationUEByStationID(UINT32 ulStationID);
STATION_UE_INFO_T* GetStationUEByGroupID(INT64 i64GroupID);
VECTOR<STATION_UE_INFO_T*>& GetStationUE();
VOID GetNormalStationUE(VECTOR<UINT32> &vUserID);

}

namespace PortalUE
{

VOID InitPortalUE(VectorMap<UINT32, PORTAL_UE_INFO_T> &mInfo);
VOID GetPortalUE(UINT32 ulStationID, VECTOR<PORTAL_UE_INFO_T*> &vUser);
VECTOR<PORTAL_UE_INFO_T*>& GetPortalUE();
VOID GetPortalUE(VECTOR<PORTAL_UE_INFO_T*> &vUser);
PORTAL_UE_INFO_T* GetPortalUE(UINT32 ulUserID);
}

namespace TrainUE
{

VOID InitTrainUE(VectorMap<UINT32, TRAIN_UE_INFO_T> &mInfo);
VECTOR<TRAIN_UE_INFO_T*>& GetTrainUE();
VOID GetNormalTrainUE(VECTOR<UINT32> &vUserID);
TRAIN_UE_INFO_T* GetTrainUE(UINT32 ulUserID);
VOID GetTrainUserIDOfStation(UINT32 ulStationID, VECTOR<UINT32> &vUser);

}

namespace DCUser
{

VOID InitDCUser(VECTOR<DC_USER_INFO_T> &vUser);
UINT32 GetDCUserNum();
DC_USER_INFO_T* GetDCUser(UINT32 ulUserID);
VOID GetDCUser(VECTOR<DC_USER_INFO_T*> &vUser);
VOID GetTrainDCUser(VECTOR<UINT32> &vUserID);
VOID GetDepotDCUser(VECTOR<UINT32> &vUserID);
BOOL IsStationInDCManageRange(UINT32 ulStationID);

}

namespace DCOperator
{

VOID InitDCOperator(VECTOR<DC_OPERATOR_INFO_T> &vInfo);
VOID GetDCOperator(VECTOR<DC_OPERATOR_INFO_T> &vInfo);
DC_OPERATOR_INFO_T* GetDCOperator(AnsiString strDCOperatorID);
AnsiString GetDCOperatorName(AnsiString strDCOperatorID);

}


#endif
