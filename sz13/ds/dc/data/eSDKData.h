#ifndef eSDKDataH
#define eSDKDataH

namespace eSDKData
{

AnsiString GetSDSTypeText(UINT32 ulSDSType);
BOOL IsRealMsg(UINT32 ulSDSType);
BOOL IsMMMsg(UINT32 ulSDSType);  // �Ƿ����
AnsiString GetUserCallStatus(UINT32 ulStatus);

}  // namespace eSDKData

#endif
