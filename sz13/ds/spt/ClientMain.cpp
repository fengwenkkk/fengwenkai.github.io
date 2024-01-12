//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "MDIForm.h"

#include "main.h"
#include "FrmLogin.h"
#include "FrmInit.h"

//---------------------------------------------------------------------------
#include <Vcl.Styles.hpp>
#include <Vcl.Themes.hpp>

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
USEFORM("sys\FrmLog.cpp", FormLog);
USEFORM("sys\FrmLocalCfg.cpp", FormLocalCfg);
USEFORM("sys\FrmInit.cpp", FormInit);
USEFORM("sys\DlgSetSkin.cpp", DialogSetSkin);
USEFORM("sys\DlgSetOperator.cpp", DialogSetOperator);
USEFORM("sys\FrmLogin.cpp", FormLogin);
USEFORM("train\DlgSetTrainID.cpp", DialogSetTrainID);
USEFORM("train\DlgSetTrainGroup.cpp", DialogSetTrainGroup);
USEFORM("train\DlgSetTrainDepotID.cpp", DialogSetTrainDepotID);
USEFORM("sys\FrmQuery.cpp", FormQuery);
USEFORM("sds\FrmInbox.cpp", FormInbox);
USEFORM("sds\DlgSetSDSTemplate.cpp", DialogSetSDSTemplate);
USEFORM("sds\DlgNewSDS.cpp", DialogNewSDS);
USEFORM("rec\FrmReqCallInfo.cpp", FormReqCallInfo);
USEFORM("rec\FrmRecManage.cpp", FormRecManage);
USEFORM("sds\FrmLatestSDS.cpp", FormLatestSDS);
USEFORM("sys\DlgSelectUser.cpp", DialogSelectUser);
USEFORM("sds\FrmSDSTemplate.cpp", FormSDSTemplate);
USEFORM("sds\FrmSDSManage.cpp", FormSDSManage);
USEFORM("sds\FrmSDS.cpp", FormSDS);
USEFORM("train\FrmTrainList.cpp", FormTrainList);
USEFORM("train\FrmTrain.cpp", FormTrain);
USEFORM("train\FrmStation.cpp", FormStation);
USEFORM("groupcall\DlgNewPatchGroup.cpp", DialogNewPatchGroup);
USEFORM("groupcall\DlgNewDynamicGroup.cpp", DialogNewDynamicGroup);
USEFORM("groupcall\DlgDynamicGroupMember.cpp", DialogDynamicGroupMember);
USEFORM("FrmTest.cpp", FormTest);
USEFORM("call\FrmUser.cpp", FormUser);
USEFORM("call\FrmP2PCallWin.cpp", FormP2PCallWin);
USEFORM("call\FrmCallIn.cpp", FormCallIn);
USEFORM("call\DlgVideoCallCfg.cpp", DialogVideoCallCfg);
USEFORM("call\DlgSubUser.cpp", DialogSubUser);
USEFORM("call\FrmVideoCall.cpp", FormVideoCall);
USEFORM("call\FrmVideoWin.cpp", FormVideoWin);
USEFORM("rec\FrmEmergencyCallInfo.cpp", FormEmergencyCallInfo);
USEFORM("rec\FrmCDR.cpp", FormCDR);
USEFORM("main.cpp", FormMain);
USEFORM("iph\FrmIPHWin.cpp", FormIPHWin);
USEFORM("rec\FrmGroupCallInfo.cpp", FormGroupCallInfo);
USEFORM("rec\FrmRecBrdInfo.cpp", FormRecBrdInfo);
USEFORM("rec\FrmRealBrdInfo.cpp", FormRealBrdInfo);
USEFORM("rec\FrmP2PCallInfo.cpp", FormP2PCallInfo);
USEFORM("rec\FrmLocalRecManage.cpp", FormLocalRecManage);
USEFORM("rec\FrmIPHCallInfo.cpp", FormIPHCallInfo);
USEFORM("groupcall\DlgStaticGroupMember.cpp", DialogStaticGroupMember);
USEFORM("groupcall\DlgSetGroupInfo.cpp", DialogSetGroupInfo);
USEFORM("groupcall\DlgRecBrd.cpp", DialogRecBrd);
USEFORM("groupcall\DlgRealBrd.cpp", DialogRealBrd);
USEFORM("groupcall\DlgPatchGroupMember.cpp", DialogPatchGroupMember);
USEFORM("groupcall\DlgSubGroup.cpp", DialogSubGroup);
USEFORM("iph\FrmIPH.cpp", FormIPH);
USEFORM("groupcall\FrmGroupCallWin.cpp", FormGroupCallWin);
USEFORM("groupcall\FrmGroupCallPanel.cpp", FormGroupCallPanel);
USEFORM("groupcall\FrmGroupCall.cpp", FormGroupCall);
USEFORM("groupcall\FrmGroup.cpp", FormGroup);
//---------------------------------------------------------------------------
extern BOOL g_bNeedLogin;
extern BOOL g_bTestMode;

extern bool ESDKLogin();
extern bool GetNativeServicePath();
extern BOOL LoadDepotInfo();
extern BOOL LoadDCOperatorInfo();

//---------------------------------------------------------------------------
bool Login()
{
    Application->ShowMainForm = false;

//  FormLogin = new TFormLogin(Application);
//    FormLogin->ShowModal();
    Application->CreateForm(__classid(TFormLogin), &FormLogin);
    FormLogin->ShowModal();

    if (!g_strDCOperatorName.IsEmpty())
    {
//      Application->ShowMainForm = true;
        return true;
    }
    else
    {
        return false;
    }
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

 /* Application->Initialize();
    Application->Run();  */
    CloseApp();
}

//---------------------------------------------------------------------------
WINAPI ClientMain()
{

 // Application->Initialize();

    for (int i=1; i<=ParamCount(); i++)
    {
        String strParam = ParamStr(i).LowerCase();

        if (strParam == "-t")
        {
            g_bTestMode = true;
        }
    }

    //g_bTestMode = true;

    if (gos_is_proc_running())
    {
        InitFail("软件已经启动");
        return -1;
    }

    // 提示本进程的权限
    SetDefaultSkinName("Office2010Blue");
//  SetTreeListSkinName("MoneyTwins");
    EnablePriv();

    gos_init_svcenv();

    if (!InitClientEnv())
    {
        GosLog(LOG_ERROR, "init client env failed!");
       //   ErrMsg("初始化失败,系统退出!");

        InitFail("初始化失败");

        return 0;
    }

    if (!InitRemoteMsg())
    {
        GosLog(LOG_FATAL, "init remote msg failed");
        InitFail("连接到服务器失败!");
        return 0;
    }

    if (!g_pRemoteMsg->IsConnected())
    {
        GosLog(LOG_FATAL, "connect to server failed");
        InitFail("连接到服务器失败!");
        return 0;
    }

    GosLog(LOG_INFO, "login to server");

    if (!Login())
    {
        InitFail("");

        return 0;
    }

    FormInit = new TFormInit(Application);
    FormInit->Show();
    FormInit->Invalidate();
    FormInit->Repaint();

    GosLog(LOG_INFO, "load cfg data from dis");

    ///if (!LoadCfgData())
    ///{
    ///    InitFail("加载数据失败！");

    ///    return 0;
    ///}

    try
    {
        Application->Initialize();
        //TStyleManager::TrySetStyle("Amethyst Kamri");

        Application->MainFormOnTaskBar = false;

        SetInitInfo("系统界面");
        GosLog(LOG_INFO, "create form main");

        TForm **pMainForm = &(Application->MainForm);

        Application->CreateForm(__classid(TFormMain), &FormMain);

        /*
        if (g_bNeedLogin && !eSDK_IsLogined())
        {
            GosLog(LOG_INFO, "login to MDC");
            SetInitInfoEx("正在登录集群服务器");
            if (!ESDKLogin())
            {
                GosLog(LOG_ERROR, "login to MDC failed");
                InitFail("登录集群服务器失败！");
                return 0;
            }
        } */

        Application->CreateForm(__classid(TFormResource), &FormResource);

        *pMainForm = FormMain;

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
