//---------------------------------------------------------------------------

#ifndef StationH
#define StationH

#include "ClientInit.h"
#include "ClientInclude.h"

//extern Dict  g_DictStation;
extern Dict  g_DictDepotID;

namespace Depot
{
void InitDepot(VECTOR<DEPOT_INFO_T> &vDepotInfo);
VECTOR<DEPOT_INFO_T*>& GetDepotInfo();
DEPOT_INFO_T* GetDepotInfo(UINT32 ulDepotID);
CHAR* GetDepotName(UINT32 ulDepotID);
CHAR* GetDepotName(TRAIN_INFO_T *pstTrain);
BOOL IsSameDepot(UINT32 ulDepotID);
}

namespace Section
{
void InitSection(VECTOR<SECTION_INFO_T> &vInfo);
void GetDepotSection(UINT32 ulDepotID, VECTOR<SECTION_INFO_T*> &vInfo);
SECTION_INFO_T* GetSection(AnsiString strSectionName);
SECTION_INFO_T* GetSection(UINT32 ulSectionID);
void GetSection(UINT32 ulStationID, UINT32 ulDirection, BOOL bInDepotTrack, VECTOR<SECTION_INFO_T*> &vInfo);
}

namespace Station
{
INT64 GetStationGroupID(UINT32 ulStationID);
void InitDictStation(Dict &dict);
void InitStation(VECTOR<STATION_INFO_T> &vStationInf);
UINT32 GetNormalStationNum();
void GetNormalStation(VECTOR<STATION_INFO_T*> &vInfo);
void GetStationInfo(VECTOR<STATION_INFO_T*> &vStationInfo);
VECTOR<STATION_INFO_T*>& GetStationInfo();
STATION_INFO_T* GetStationInfo(UINT32 ulStationID);
STATION_INFO_T* GetStationByGroupID(INT64 i64GroupID);
int GetStationIndex(UINT32 ulStationID);
AnsiString GetStationName(UINT32 ulStationID);
void GetDepotStationInfo(VECTOR<STATION_INFO_T*> &vStationInfo); //获取车辆段车站
void GetInterLockStation(VECTOR<STATION_INFO_T*> &vInfo);
void GetInterLockStation(UINT32 ulInterLockStationID, VECTOR<STATION_INFO_T*> &vInfo);

BOOL IsTrainInDepot(TRAIN_INFO_T *pstTrainInfo);
UINT32 GetDepotID(TRAIN_INFO_T *pstTrainInfo);

std::vector<UINT32> GetAdjacentStationID(UINT32 ulStationID);
bool IsDepotStation(UINT32 ulStationID);
}  // namespace Station

namespace DestInfo
{
void InitDestInfo(std::vector<DEST_INFO_T> &vDestInfo);
UINT32 GetDestStationID(AnsiString strDestID);
bool IsValidDestID(UINT32 ulDepotID, AnsiString strDestID);
void InitDictDest(UINT32 ulStationID, Dict &DictDest);
void InitDictDest(Dict &DictDest);
}

//---------------------------------------------------------------------------
#endif
