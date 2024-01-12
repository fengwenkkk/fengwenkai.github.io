#ifndef eSDKDataH
#define eSDKDataH

namespace eSDKData
{

AnsiString GetSDSTypeText(UINT32 ulSDSType);
BOOL IsRealMsg(UINT32 ulSDSType);
BOOL IsMMMsg(UINT32 ulSDSType);  // ÊÇ·ñ²ÊÐÅ
AnsiString GetUserCallStatus(UINT32 ulStatus);

}  // namespace eSDKData

#endif
