#ifndef GDisplayManagerH
#define GDisplayManagerH

#include "displaymanager.h"

class GDisplayManager : public DisplayManager
{
public:
    GDisplayManager(void);
    virtual ~GDisplayManager(void);

    //֪ͨ�����״̬�����Ϣ
    virtual void notifyGroupStatus(GrpDspInfo * grpInfo);

    //֪ͨ�û���Ⱥ����Դ��״̬�����Ϣ
    virtual void notifyResourceStatus(ResourceStatusIndicator * resourceStatus);

    //֪ͨ��Ե���е�״̬�����Ϣ
    virtual void notifyP2pcallStatus(P2pcallStatusIndicator * callStatus);

    //֪ͨ��Ƶ���е�״̬�����Ϣ��������Ƶ�ش�������ҪYCʵ��
    virtual void notifyP2pvideocallStatus(P2pvideocallStatusIndicator * callStatus);

    virtual void notifyUserStatus(ResourceStatusIndicator * resourceStatus);

    virtual void notifyUserSdsStatus(SdsMessageIndicator * sdsmsg);
    virtual void notifyModuleStatus(DCModuleStatusIndicator * moduleStatus);

    //��������Ϣ�ϱ���GUI
    virtual void notifySMS(SdsMessageIndicator * sdsmsg);

    //֪ͨ���ö���ı����Ϣ�� ����Ϊһ����ָ̬�룬 ʹ������Ҫdowncast Ϊ��Ӧ�ı������
    virtual void notifyProvisionChange(ProvChangeIndicator* );

    //֪ͨ���ö����ȫͬ���� ����Ϊһ����ָ̬�룬 ʹ������Ҫdowncast Ϊ��Ӧ�ı������
    virtual void notifyProvisionAllResync(ProvChangeIndicator* ) {;}

    //֪ͨ״̬�����ȫͬ���� ����Ϊһ����ָ̬�룬 ʹ������Ҫdowncast Ϊ��Ӧ�ı������
    virtual void notifyStatusAllResync(ResourceStatusIndicator* ) {;}

protected:

};

#endif

