#include "sdk/include/license_control.h"

//第三方可以进行license控制，此demo简单处理，对所以场景都认为license充足

class RecLicenseControl :public LicenseControl {
  virtual bool is_license_accept( ServInfo *Sinfo )
  {
    return true;
  }
private:
};

 //第三方进行端口分配，为保证不与LTE系统内部端口冲突，建议使用51000-56999之间的端口

//获取语音、视频的端口
class RecSdpControl
  :public ConnectionControl
{
  virtual bool get_Connection_info( ServInfo *Sinfo,ConnInfo *Cinfo );
private:
};
