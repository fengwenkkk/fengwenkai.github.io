//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include "g_include.h"

#include "RemoteMsg.h"
#include "msg.h"
#include "ds_public.h"
#include "ats.h"
#include "FrmATS.h"
#include "ClientInit.h"
#include "ClientUtil.h"
#include "LocalCfg.h"
#include "station.h"
//---------------------------------------------------------------------------
USEFORM("FrmATS.cpp", FormATS);
//---------------------------------------------------------------------------
BOOL    g_bTestMode = FALSE;
BOOL    g_bNeedLogin = TRUE;
BOOL    g_bShowTitle = TRUE;
UINT32  g_ulCurrDCUserID = INVALID_USER_ID;

typedef struct
{
    CHAR        acDCUserName[32];
    UINT8       aucDISAddr[4];
    UINT16      usDISPort;
    BOOL        bTCPMode;

}ATSLocalCfg;

ATSLocalCfg     g_stATSLocalCfg = {0};

void ClientConnectionChange(bool bConnected)
{
}

void LoadSkinCfg()
{
}

BOOL InitRemoteMsg()
{
    static BOOL bConnect = FALSE;

    if (bConnect)
    {
        return TRUE;
    }

    /* 初始化远程消息模块 */
    g_pRemoteMsg = RemoteMsg::New(g_stATSLocalCfg.aucDISAddr,
                                  g_stATSLocalCfg.usDISPort,
                                  true);
    if (!g_pRemoteMsg)
    {
        GosLog(LOG_FATAL, "Get Remote Msg failed!");

        return FALSE;
    }

    g_pRemoteMsg->SetClientType(DIS_CLIENT_DC);
    g_pRemoteMsg->SetClientConnectionChange(ClientConnectionChange);

    for (int i=0; i<100; i++)
    {
        if (g_pRemoteMsg->IsConnected())
        {
            break;
        }

        gos_sleep_1ms(100);
    }

    if (!g_pRemoteMsg->IsConnected())
    {
        GosLog(LOG_FATAL, "Connect to server failed!");

        return FALSE;
    }

    bConnect = TRUE;

    return TRUE;
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

    if (!Conf.Get("Sys", "dc_number", &g_ulCurrDCUserID))
    {
        GosLog(LOG_ERROR, "get dc_number of Sys failed");
        return FALSE;
    }

    szValue = Conf.Get("Sys", "dis_ip");
    if (!gos_get_ip_port(szValue, g_stATSLocalCfg.aucDISAddr, &g_stATSLocalCfg.usDISPort))
    {
        GosLog(LOG_ERROR, "get dis_ip of Sys failed");
        return FALSE;
    }

    if (g_stATSLocalCfg.usDISPort == 0)
    {
        g_stATSLocalCfg.usDISPort = GOS_DEFAULT_SERVER_PORT;
    }

    g_stATSLocalCfg.bTCPMode = FALSE;
    szValue = Conf.Get("Sys", "tcp_mode");
    if (!szValue || strcasecmp(szValue, "true") == 0)
    {
        g_stATSLocalCfg.bTCPMode = TRUE;
    }

    return TRUE;
}
//-------------------------------------------------------------------
BOOL InitClientEnvATS()
{
    /* dev express时间格式设置 */
    TcxFormatController     *FormatController = cxFormatController();

    FormatController->UseDelphiDateTimeFormats = true;

    /* 全局设置 */
    if (dxFormatSettings)
    {
        dxFormatSettings->DateSeparator = '-';
        dxFormatSettings->ShortDateFormat = "yyyy-MM-dd";
    }

    gos_init_socket();

    /// 使用默认参数初始化日志模块
    GosInitLog();

    LoadSkinCfg();
    InitSystemFont();
    gos_init_svcenv();

    return TRUE;
}

extern VOID SetTreeListParam(int iFontSize, int iNodeHeight);

//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    SetDefaultSkinName("Lilian");
    SetDefaultFontName("Arial");

    SetTreeListParam(11, 24);

    // 提示本进程的权限
    EnablePriv();

    if (!GetClientCfg() ||
        !InitClientEnvATS() ||
        !InitRemoteMsg()
        )
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
