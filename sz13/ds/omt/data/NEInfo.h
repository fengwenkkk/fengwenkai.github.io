//---------------------------------------------------------------------------

#ifndef NEInfoH
#define NEInfoH
//---------------------------------------------------------------------------
#endif

namespace NEBasic
{
void InitNEBasicInfo(VectorMap<AnsiString, NE_BASIC_INFO_T> &mNEBasicInfo);
void GetNEBasicInfo(VectorMap<AnsiString, NE_BASIC_INFO_T>  &mNEBasicInfo);

// DevonlineCount
void InitDevOnlineCount(std::map<UINT32, UINT32> &mDevOnlineCount);
void GetDevOnlineCount(std::map<UINT32, UINT32> &mDevOnlineCount);
}
