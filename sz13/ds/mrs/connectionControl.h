#include "sdk/include/license_control.h"

//���������Խ���license���ƣ���demo�򵥴��������Գ�������Ϊlicense����

class RecLicenseControl :public LicenseControl {
  virtual bool is_license_accept( ServInfo *Sinfo )
  {
    return true;
  }
private:
};

 //���������ж˿ڷ��䣬Ϊ��֤����LTEϵͳ�ڲ��˿ڳ�ͻ������ʹ��51000-56999֮��Ķ˿�

//��ȡ��������Ƶ�Ķ˿�
class RecSdpControl
  :public ConnectionControl
{
  virtual bool get_Connection_info( ServInfo *Sinfo,ConnInfo *Cinfo );
private:
};
