//#include "logger.h"
//#include "dc/dc_module_error.h"
#include "ace/ACE.h"

#include "appcontext.h"

#include "RECDisplayMnager.h"
#include "dcstate.h"
#include "operationmanager.h"
//#include "RECGisReportManager.h"

//#include "sdk_helper.h"
//#include "provision_helper.h"
//#include "provisionmanager.h"

#include <iostream>
#include <fstream>
//#include "OperationProxy.h"
#include "connectionControl.h"

#include "ace/Init_ACE.h"
#include "ace/OS_NS_unistd.h"

#include "RecPublic.h"
#include "RecFileManager.h"

using namespace std;

const bool bypassMedia=true;

BOOL    g_bDCLogined = FALSE;

extern int gos_main(int argc, char **argv);

extern res_status_value_t GetLoginStatus();

BOOL IsCleanFileTime()
{
    static UINT32 ulLastTime = 0;
    UINT32 ulCurrTime = gos_get_uptime();

    if (ulLastTime == 0 ||
        ((ulCurrTime-ulLastTime) > 30*24*60*60 )) //30天==2592000秒
    {
        ulLastTime = ulCurrTime;

        return TRUE;
    }

    return FALSE;
}

BOOL App_Login()
{
    ResourceInfo    resInfo(g_ulDCUserID);
    CHAR            acServerPort[32];

    sprintf(acServerPort, "%u", g_stIniCfg.usServerPort);

    GosLog(LOG_INFO, "mrs login start!");

    resInfo.setUserName(g_stIniCfg.acUser);
    resInfo.setPassWord(g_stIniCfg.acPwd);
    resInfo.setServerIp(g_stIniCfg.acServerAddr);
    resInfo.setLocalIp(g_stIniCfg.acLocalAddr);
    resInfo.setSipPort(acServerPort);

    GosLog(LOG_INFO, "setUserName(%s), setPassWord(%s), setServerIp(%s), setLocalIp(%s), setSipPort(%s)", g_stIniCfg.acUser, g_stIniCfg.acPwd, g_stIniCfg.acServerAddr, g_stIniCfg.acLocalAddr, acServerPort);

    // 应用程序调用operationmanager来注册
    OperationManager *om = OperationManager_T::instance();

    //SDK_Helper::setLogonType(SPECIAL_PROXY);
//    GosLog(LOG_INFO, "om->invokeOp start");
    Op_Return_t ret = om->invokeOp(&resInfo, DC_LOGIN );

    GosLog(LOG_INFO, "om->invokeOp(&resInfo, DC_LOGIN ) return is %u", ret);

    if (OP_OK_ACCEPTED != ret)
    {
        GosLog(LOG_ERROR, "login failed");
        return FALSE;
    }

    //res_status_value_t res = TDHelper::getinstance()->waitLoginStatus();
    res_status_value_t res = GetLoginStatus();

    GosLog(LOG_INFO, "mrs login end, res is %u", res);
    if (res == STATUS_REGOK)
    {
        GosLog(LOG_INFO, "login success");
        return TRUE;
    }
    else
    {
        GosLog(LOG_ERROR, "login fail with %d", res);
        return FALSE;
    }
}

BOOL App_Logout()
{
    ResourceInfo    resInfo(g_ulDCUserID);
    CHAR            acServerPort[32];

    sprintf(acServerPort, "%u", g_stIniCfg.usServerPort);

    resInfo.setUserName(g_stIniCfg.acUser);
    resInfo.setPassWord(g_stIniCfg.acPwd);
    resInfo.setServerIp(g_stIniCfg.acServerAddr);
    resInfo.setLocalIp(g_stIniCfg.acLocalAddr);
    resInfo.setSipPort(acServerPort);

    // 应用程序调用operationmanager来注册
    OperationManager *om = OperationManager_T::instance();

    Op_Return_t ret = om->invokeOp(&resInfo, DC_LOGOUT);

    if (OP_OK_ACCEPTED != ret)
    {
        GosLog(LOG_ERROR, "logout fail");
        return FALSE;
    }
    else
    {
        GosLog(LOG_INFO, "logout success");
        return TRUE;
    }
}

//int ACE_TMAIN(int argc, char **argv)
int main(int argc, char **argv)
{
    if (gos_main(argc, argv) < 0)
    {
        return -1;
    }

    // init sdk
    ApplicationContext * ac = ApplicationContext::getInstance();

    // set parameters
    ac->setWorkMode(APPMODE_LOGGING_SERVER);
    ac->setMix(1);
    ac->setBypassBuildMedia(bypassMedia);
    ac->setMemMgrMode(MEM_SDK_MGR);
    //ac->setNATIndicator(true);
    RECDisplayMnager* dm = new RECDisplayMnager();
    ac->setDisplayManager((DisplayManager*)dm);

/*  RECGisReportManager* gis=new RECGisReportManager();
    ac->setReportManager((GISReportManager*)gis);
*/

#if 1
    RecLicenseControl   *licCtr = new RecLicenseControl;
    RecSdpControl       *recSdpC = new RecSdpControl;

    ac->setLicenseControl(licCtr);
    ac->setConnectionControl(recSdpC);
#endif

//  ubp::platform::trace::UBPTrace_T::instance()->Init("RECS");

    // init
    int nRet = ac->initApp();

    if (0 != nRet)
    {
        ac->exitApp();
        GosLog(LOG_ERROR, "init app failed");
        return -1;
    }

    //-----------------------------------------------------
    DcState     *dc = ac->getConsoleState();
    CHAR        acServerPort[32];

    sprintf(acServerPort, "%u", g_stIniCfg.usServerPort);

    /*
    dc->setDCID(g_ulDCUserID);
    dc->setServerIP(g_stIniCfg.acServerAddr);
    dc->setSipPort(acServerPort);
    dc->setUser(g_stIniCfg.acUser);

    dc->setPwd(g_stIniCfg.acPwd);
    dc->setLocalIP(g_stIniCfg.acLocalAddr);

    */

        //dc->setSipLocalPort("4001");

    dc->set_login_info(g_ulDCUserID, g_stIniCfg.acPwd, g_stIniCfg.acServerAddr, g_stIniCfg.usServerPort, g_stIniCfg.acLocalAddr, "en_us");

    std::string str = g_stIniCfg.acServerAddr;

    // login
    if (!App_Login())
    {
        ac->exitApp();
        return -1;
    }

    g_bDCLogined = TRUE;

    ac->startRunning(str);

    gos_sleep(2);
    ac->triggerStatusReport(true);

    RecFileManager::GetInstance().Init(g_stIniCfg.acRecOriPath, g_stIniCfg.acRecTmpPath, g_stIniCfg.ulTempSaveDays, g_stIniCfg.ulOriSaveDays, g_stIniCfg.ulRecSaveDays);

    //  doBusiness(stdin);
    while(1)
    {
        if (!g_bDCLogined)
        {
            g_bDCLogined = App_Login();
        }

        if (IsCleanFileTime())
        {
            //Init(CHAR *szDirOri, CHAR *szDirTemp, UINT32 ulTempSaveDays, UINT32 ulOriSaveMonth)
            
            RecFileManager::GetInstance().Start();
            
        }

        gos_sleep(1);
    }

//end:
    // 停止运行调度台
    ac->stopRunning();
    if (!App_Logout())
    {
        return -1;
    }

    gos_sleep(3);
    ac->exitApp();

    return 0;
}

