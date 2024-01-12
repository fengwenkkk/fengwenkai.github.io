//---------------------------------------------------------------------------

#pragma hdrstop

#include "ClientInclude.h"
#include "NEInfo.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

static VectorMap<AnsiString, NE_BASIC_INFO_T>        s_mNEBasicInfo;
static std::map<UINT32, UINT32>                      s_mDevOnlineCount;
namespace NEBasic
{

void InitNEBasicInfo(VectorMap<AnsiString, NE_BASIC_INFO_T> &mNEBasicInfo)
{
   s_mNEBasicInfo.Clear();
   s_mNEBasicInfo.AddAll(mNEBasicInfo);
}

void GetNEBasicInfo(VectorMap<AnsiString, NE_BASIC_INFO_T>  &mNEBasicInfo)
{
   mNEBasicInfo.Clear();
   mNEBasicInfo.AddAll(s_mNEBasicInfo);
}

void InitDevOnlineCount(std::map<UINT32, UINT32> &mDevOnlineCount)
{
    s_mDevOnlineCount.clear();
    s_mDevOnlineCount = mDevOnlineCount;
}

void GetDevOnlineCount(std::map<UINT32, UINT32> &mDevOnlineCount)
{
   mDevOnlineCount.clear();
   mDevOnlineCount = s_mDevOnlineCount;
}
}
