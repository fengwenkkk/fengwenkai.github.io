//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"
#include "MsgDialog.h"
#include "GCommon.h"
#include "MDIForm.h"
#include "MakeUtil.h"
#include "main.h"

//---------------------------------------------------------------------------
USEFORM("main.cpp", FormMain);
//---------------------------------------------------------------------------
extern AnsiString g_strSkinName;

extern bool g_bAgentMode;

//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    if (gos_is_proc_running())
    {
        InfoMsg("软件已经运行");

        if (FormMsgDialog)
        {
            delete FormMsgDialog;
            FormMsgDialog = NULL;
        }

        Application->Terminate();

        return -1;
    }

    g_strSkinName = "MoneyTwins";

    AnsiString  strExeName = ParamStr(0).LowerCase();
    char        *szTmp = gos_right_strchr(strExeName.c_str(), '\\');

    if (szTmp && strstr(szTmp, "agent") != 0)
    {
        g_bAgentMode = true;
    }

    if (!LoadCfg())
    {
        InfoMsg("加载配置数据失败");

        Application->Initialize();
        Application->Run();

        return -1;
    }

    try
    {
        Application->Initialize();
        Application->MainFormOnTaskBar = false;
        Application->CreateForm(__classid(TFormMain), &FormMain);
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

//  SaveLocalCfg();

    return 0;
}
//---------------------------------------------------------------------------
