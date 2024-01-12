//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include "g_include.h"
#include "GLog.h"
#include "RemoteMsg.h"
#include "msg.h"
#include "ds_public.h"
#include "ats.h"
#include "main.h"
#include "ClientInit.h"
#include "ClientUtil.h"

//---------------------------------------------------------------------------
USEFORM("main.cpp", FormMain);
//---------------------------------------------------------------------------
BOOL    g_bTestMode = FALSE;
BOOL    g_bNeedLogin = TRUE;
UINT32  g_ulCurrDCUserID = INVALID_USER_ID;

void ClientConnectionChange(bool bConnected)
{
}

void LoadSkinCfg()
{
}

//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    g_strSkinName = "Lilian";

    // ��ʾ�����̵�Ȩ��
    EnablePriv();

    if (!InitClientEnv()/* ||
        !InitRemoteMsg() */)
    {
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
    return 0;
}
//---------------------------------------------------------------------------
