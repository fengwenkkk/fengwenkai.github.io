#ifndef GDisplayManagerH
#define GDisplayManagerH

#include "displaymanager.h"

class GDisplayManager : public DisplayManager
{
public:
    GDisplayManager(void);
    virtual ~GDisplayManager(void);

    //通知组呼的状态变更信息
    virtual void notifyGroupStatus(GrpDspInfo * grpInfo);

    //通知用户和群组资源的状态变更信息
    virtual void notifyResourceStatus(ResourceStatusIndicator * resourceStatus);

    //通知点对点呼叫的状态变更信息
    virtual void notifyP2pcallStatus(P2pcallStatusIndicator * callStatus);

    //通知视频呼叫的状态变更信息（包括视频回传），需要YC实现
    virtual void notifyP2pvideocallStatus(P2pvideocallStatusIndicator * callStatus);

    virtual void notifyUserStatus(ResourceStatusIndicator * resourceStatus);

    virtual void notifyUserSdsStatus(SdsMessageIndicator * sdsmsg);
    virtual void notifyModuleStatus(DCModuleStatusIndicator * moduleStatus);

    //短数据信息上报给GUI
    virtual void notifySMS(SdsMessageIndicator * sdsmsg);

    //通知配置对象的变更信息， 参数为一个多态指针， 使用者需要downcast 为对应的变更子类
    virtual void notifyProvisionChange(ProvChangeIndicator* );

    //通知配置对象的全同步， 参数为一个多态指针， 使用者需要downcast 为对应的变更子类
    virtual void notifyProvisionAllResync(ProvChangeIndicator* ) {;}

    //通知状态对象的全同步， 参数为一个多态指针， 使用者需要downcast 为对应的变更子类
    virtual void notifyStatusAllResync(ResourceStatusIndicator* ) {;}

protected:

};

#endif

