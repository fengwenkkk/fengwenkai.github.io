//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "MDIForm.h"

#include "main.h"
#include "FrmInit.h"
#include "FrmLogin.h"

//---------------------------------------------------------------------------
#include <Vcl.Styles.hpp>
#include <Vcl.Themes.hpp>

extern void FreeFormResource();

//---------------------------------------------------------------------------
USEFORM("sys\FrmLocalCfg.cpp", FormLocalCfg);
USEFORM("sys\FrmInit.cpp", FormInit);
USEFORM("sys\FrmLogin.cpp", FormLogin);
USEFORM("main.cpp", FormMain);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
bool Login()
{
    Application->ShowMainForm = false;

    FormLogin = new TFormLogin(Application);
    FormLogin->ShowModal();

    if (g_stLocalCfg.acUserName[0] != '\0')
    {
        Application->ShowMainForm = true;
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

//---------------------------------------------------------------------------
WINAPI ClientMain()
{
    if (gos_is_proc_running())
    {
        InitFail("软件已经启动");
        return -1;
    }

    // 提示本进程的权限
    GetDefaultSkinName() = "Office2010Blue";
    EnablePriv();

    gos_init_svcenv();

    if (!InitClientEnv())
    {
        GosLog(LOG_ERROR, "init client env failed!");

        InitFail("");

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
        GosLog(LOG_FATAL, "connect to omc failed");
        InitFail("连接到服务器失败!");
        return 0;
    }

    GosLog(LOG_INFO, "login to omc server");
    if (!Login())
    {
        Application->Initialize();
        Application->Run();
        CloseApp();

        return 0;
    }

    //FormLogin->ShowModal();
    FormInit = new TFormInit(Application);
    FormInit->Show();
    FormInit->Invalidate();
    FormInit->Repaint();

    GosLog(LOG_INFO, "load cfg data from omc");

    if (!LoadCfgData())
    {
        InitFail("加载数据失败!");

        return 0;
    }

    CHAR        acTmpDir[256];

    sprintf(acTmpDir, "%s/tmp", gos_get_root_path());
    if (!gos_file_exist(acTmpDir))
    {
        gos_create_dir(acTmpDir);
    }
    else
    {
        gos_delete_dir(acTmpDir);
    }

    try
    {
        Application->Initialize();

        Application->MainFormOnTaskBar = false;

        SetInitInfo("系统界面");
        GosLog(LOG_INFO, "create form main");
		Application->CreateForm(__classid(TFormMain), &FormMain);
		GetTreeListHeaderStyle();  // 目的是创建窗体
//        Application->CreateForm(__classid(TFormResource), &FormResource);
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
