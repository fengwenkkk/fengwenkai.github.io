//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include "g_include.h"

#include "RemoteMsg.h"
#include "msg.h"
#include "ds_public.h"
#include "FrmATS.h"
#include "ClientInit.h"
#include "ClientUtil.h"

//---------------------------------------------------------------------------
USEFORM("FrmATS.cpp", FormATS);
USEFORM("..\..\dc\plat\MDIForm.cpp", SubForm);
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

BOOL GetClientCfg()
{
    CHAR    acFile[256];
    CHAR    *szValue;
    INT32   iValue;

    sprintf(acFile, "%s/%s", gos_get_root_path(), "dc.ini");

    if (!gos_file_exist(acFile))
    {
        GosLog(LOG_ERROR, "open file %s failed!", acFile);
        return FALSE;
    }

    GConfFile   Conf(acFile);

    if (!Conf.Get("Sys", "user_name", g_stLocalCfg.acDCUserName, sizeof(g_stLocalCfg.acDCUserName)))
    {
        GosLog(LOG_ERROR, "get user_name of Sys failed");
        return FALSE;
    }

    szValue = Conf.Get("Sys", "dis_ip");
    if (!gos_get_ip_port(szValue, g_stLocalCfg.aucDISAddr, &g_stLocalCfg.usDISPort))
    {
        GosLog(LOG_ERROR, "get dis_ip of Sys failed");
        return FALSE;
    }

    if (g_stLocalCfg.usDISPort == 0)
    {
        g_stLocalCfg.usDISPort = GOS_DEFAULT_SERVER_PORT;
    }

    g_stLocalCfg.bTCPMode = FALSE;
    szValue = Conf.Get("Sys", "tcp_mode");
    if (!szValue || strcasecmp(szValue, "true") == 0)
    {
        g_stLocalCfg.bTCPMode = TRUE;
    }

    return TRUE;
}

//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    g_strSkinName = "Lilian";

    // 提示本进程的权限
    EnablePriv();

    if (!GetClientCfg() ||
        !InitClientEnv() ||
        !InitRemoteMsg() )
    {
        return -1;
    }

    try
    {
        Application->Initialize();
        Application->MainFormOnTaskBar = false;
        Application->CreateForm(__classid(TFormATS), &FormATS);
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
