//---------------------------------------------------------------------------

#pragma hdrstop

#include "ClientInclude.h"
#include "Station.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

static VectorMap<UINT32,   STATION_INFO_T>           s_mStationInfo;

namespace Station
{
void InitStationInfo(VectorMap<UINT32, STATION_INFO_T> &mStationInfo)
{
    s_mStationInfo.Clear();
    s_mStationInfo.AddAll(mStationInfo);
}

void GetStationInfo(VectorMap<UINT32, STATION_INFO_T> &mStationInfo)
{
    mStationInfo.Clear();
    mStationInfo.AddAll(s_mStationInfo);
}

}
