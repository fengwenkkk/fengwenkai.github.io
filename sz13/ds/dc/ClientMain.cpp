//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "MDIForm.h"

#include "main.h"
#include "FrmLogin.h"
#include "FrmInit.h"
#include "FrmMediaPlayer.h"
#include "LocalCfg.h"
#include "AudioDev.h"

#include <Vcl.Styles.hpp>
#include <Vcl.Themes.hpp>

//---------------------------------------------------------------------------

extern bool InitNativeServicePath();

//---------------------------------------------------------------------------
bool LoginDIS()
{
    Application->ShowMainForm = false;

    Application->CreateForm(__classid(TFormLogin), &FormLogin);
    FormLogin->BringToFront();
    FormLogin->ShowModal();

    return APP::IsDCLogin();
}

void InitFail(AnsiString strErrInfo)
{
    if (FormInit)
    {
        delete FormInit;
        FormInit = NULL;
    }

    if (FormLogin)
    {
        delete FormLogin;
        FormLogin = NULL;
    }

    if (!strErrInfo.IsEmpty())
    {
        ErrMsg(strErrInfo);
    }

    if (FormMsgDialog)
    {
        delete FormMsgDialog;
        FormMsgDialog = NULL;
    }

    FreeFormResource();

    Application->Initialize();
    Application->Run();

    CloseApp();
}

/**
 * @brief           在显示登录界面之前，从数据库中加载一些信息
 * @param           strErrorMsg    [out]    如果函数出错，该参数返回错误信息
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-15 14:16:51
 * @note
 */
BOOL LoadDataBeforeLogin(AnsiString& strErrorMsg)
{
    /// 加载数据库中表 dcoperator 中的所有信息, 即加载SDK账号的新消息
    if (!LoadDCUserInfo() &&  // 加载两次
        !LoadDCUserInfo())
    {
        GosLog(LOG_FATAL, "Load dc user data failed");
        strErrorMsg = "加载调度台信息失败";

        return FALSE;
    }

    /// 加载数据库中表 dcoperator 中的所有信息, 即加载调度员账号的新消息
    if (!LoadDCOperatorInfo())
    {
        GosLog(LOG_FATAL, "Load dc operator data failed");
        strErrorMsg = "加载调度台信息失败";

        return FALSE;
    }

    /// 加载数据库表 depot 中的信息，即加载车辆段信息
    if (!LoadDepotInfo())
    {
        GosLog(LOG_FATAL, "Load depot data failed");
        strErrorMsg = "加载车辆段信息失败";

        return FALSE;
    }

    // DIS全局配置
    if (!LoadAPPCfg())
    {
        GosLog(LOG_FATAL, "Load app cfg failed");
        strErrorMsg = "加载业务配置信息失败";

        return FALSE;
    }

    return TRUE;
}

//---------------------------------------------------------------------------
WINAPI ClientMain()
{
    for (int i=1; i<=ParamCount(); i++)
    {
        AnsiString strParam = ParamStr(i).LowerCase();

        if (strParam == "-t")
        {
            LocalCfg::GetInstance().SetTestMode(true);
        }
        else if (strParam == "-auto_login")
        {
            LocalCfg::GetInstance().SetAutoLoginDIS(true);
        }
    }

    if (gos_is_proc_running())
    {
        InitFail("软件已经启动");
        return -1;
    }

    SetDefaultSkinName("Office2010Blue");

    // 提升本进程的权限
    EnablePriv();

    gos_init_svcenv();

    GosLog(LOG_INFO, "dc ver: %s", LocalCfg::GetInstance().GetDCVersion().c_str());
 //   gos::PrintCurrentVersion(LocalCfg::GetInstance().GetAppTitle().c_str(), LocalCfg::GetInstance().GetDCVersion().c_str());

    if (!InitClientEnv())
    {
        GosLog(LOG_ERROR, "init client env failed!");

        InitFail("初始化失败");

        return 0;
    }

    /// 初始化与dis通信的模块
    if (!InitRemoteMsg())
    {
        GosLog(LOG_FATAL, "init remote msg failed");

        if (!AskMsg("连接到调度服务器失败, \n是否降级运行?"))
        {
            InitFail("");
            return 0;
        }

        APP::AllowDISOffline(true);
    }

    if (!InitMRSRemoteMsg())
    {
        GosLog(LOG_FATAL, "init mrs remote msg failed");
        InitFail("连接到录音录像服务器失败!");
        return 0;
    }

    AnsiString strErrorMsg;
    /// 在显示登录界面之前，从数据库中加载一些信息
    if(!LoadDataBeforeLogin(strErrorMsg))
    {
        InitFail(strErrorMsg);
        return 0;
    }

    GosLog(LOG_INFO, "login to dis server start");

    /// 进入登录界面
    if (!LoginDIS())
    {
        InitFail("");

        return 0;
    }

    /// 验证本地配置文件中的 SDK 登陆账号是不是在数据库表(dcuser)中
    if (!DCUser::GetDCUser(APP::GetSDKLoginShortNum()))
    {
        strErrorMsg.printf("获取当前调度台%u信息失败", APP::GetSDKLoginShortNum());
        InitFail(strErrorMsg);

        return 0;
    }

    /// 显示进度条窗口
    FormInit = new TFormInit(Application);
    FormInit->Show();
    FormInit->Invalidate();
    FormInit->Repaint();

   // eSDK_StopLocalService();
    eSDK_StartLocalService();

    if (!InitNativeServicePath())
    {
        InitFail("获取本地通信服务信息失败！");

        return 0;
    }

    /// 加载数据库中的静态数据
    GosLog(LOG_INFO, "start load cfg data from dis");
    if (!LoadCfgData())
    {
        InitFail("加载数据失败！");

        return 0;
    }

    if (LocalCfg::GetInstance().NeedLogin() &&
        !eSDK_IsLogined())
    {
        GosLog(LOG_INFO, "login to MDC");
        SetInitInfoEx("正在登录集群服务器");
        if (!APP::Login(NULL))
        {
            GosLog(LOG_ERROR, "login to MDC failed");
            InitFail("登录集群服务器失败！");
            return 0;
        }

        if (!InitAudioDevList())
        {
            GosLog(LOG_ERROR, "load speaker and mic info failed");
            InitFail("加载音频资源信息失败！");
            return 0;
        }

        GosLog(LOG_INFO, "eSDK ver is %s", eSDK_GetVer().c_str());
    }

    try
    {
        Application->Initialize();

        Application->MainFormOnTaskBar = false;

        SetInitInfo("系统界面");
        GosLog(LOG_INFO, "create form main");

        TForm **pMainForm = &(Application->MainForm);

        Application->CreateForm(__classid(TFormMain), &FormMain);
        GetTreeListHeaderStyle();  // 目的是创建窗体
      //  Application->CreateForm(__classid(TFormResource), &FormResource);
        Application->CreateForm(__classid(TFormMediaPlayer), &FormMediaPlayer);

        *pMainForm = FormMain;

        APP::SetEventHandle(FormMain->Handle);

        Application->Run();
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    catch (...)
    {
        try
        {
            throw Exception("");
        }
        catch (Exception &exception)
        {
            Application->ShowException(&exception);
        }
    }

    return 0;
}
