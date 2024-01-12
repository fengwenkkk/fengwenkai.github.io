//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include "g_include.h"
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
extern UINT8            g_aucMdcAddr[4];
extern UINT8            g_aucOmcAddr[4];

extern CHAR            *g_szConfigFile;
extern UINT32           g_ulOmaNum;
extern AnsiString       g_strSkinName;

void ClientConnectionChange(bool bConnected)
{
}

void LoadSkinCfg()
{
}

BOOL GetConfValue(CHAR *szConfFile, CHAR *szGroup, const CHAR *szKey, AnsiString &strValue)
{
    CHAR            acFile[256];

    sprintf(acFile, "%s/%s", gos_get_root_path(), szConfFile);

    GConfFile       Conf(szConfFile);
    CHAR            *szValue = Conf.Get(szGroup, szKey);

    strValue = "";
    if (!szValue)
    {
        return FALSE;
    }

    strValue = szValue;
    return TRUE;
}

BOOL GetConfValue(CHAR *szConfFile, CHAR *szGroup, const CHAR *szKey, UINT32 &ulValue)
{
    AnsiString  strValue;

    if (!GetConfValue(szConfFile, szGroup, szKey, strValue))
    {
        return FALSE;
    }

    return gos_atou(strValue.c_str(), &ulValue);
}

BOOL InitClientEnv()
{
    /* dev express时间格式设置 */
    TcxFormatController     *FormatController = cxFormatController();

    FormatController->UseDelphiDateTimeFormats = true;

    /* 全局设置 */
    if (dxFormatSettings)
    {
        dxFormatSettings->DateSeparator = '-';
        dxFormatSettings->ShortDateFormat = "yyyy-MM-dd";
        //dxFormatSettings->ShortTimeFormat = "hh:mm:ss";
        //dxFormatSettings->LongTimeFormat = "HH:mm:ss";
    }

    gos_init_socket();

	GosInitLog();

    /*
    if (!GetLocalCfg())
    {
        GosLog(LOG_FATAL, "Get local Cfg failed!");

        return FALSE;
    }    */

    UINT32  ulLogLevel = LOG_INFO;
    AnsiString  strValue = "";

	GetConfValue(g_szConfigFile, "Log", "log_level", ulLogLevel);

    GetConfValue(g_szConfigFile, "Sys", "mdc_ip", strValue);
    if (!gos_get_ip(strValue.c_str(), g_aucMdcAddr))
    {
        GosLog(LOG_ERROR, "get mdc_addr failed");
        return FALSE;
    }

    GetConfValue(g_szConfigFile, "Sys", "omc_ip", strValue);
    if (!gos_get_ip(strValue.c_str(), g_aucOmcAddr)  )
    {
        GosLog(LOG_ERROR, "get omc_addr failed");
        return FALSE;
    }

    /* 日志 */
    GLogFactory     *pLogFactory = GLogFactory::GetInstance();

    if (!pLogFactory)
    {
        GosLog(LOG_FATAL, "Get LogFactory failed!");

        return FALSE;
	}

	Logger *pLogger = pLogFactory->GetDefaultLogger();
	if (!pLogger)
    {
        GosLog(LOG_FATAL, "Get Logger failed!");

        return FALSE;
    }

    pLogger->SetLogToStdout(FALSE);
    pLogger->SetLogToFile(TRUE, 4, 4096, 0);
	pLogger->SetLogLevel(ulLogLevel);
    pLogger->SetLogModule(MODULE_DTP, MODULE_DTP_NAME);

	LoadSkinCfg();
	InitSystemFont();
    gos_init_svcenv();

	return TRUE;
}

extern VOID SetTreeListParam(int iFontSize, int iNodeHeight);

//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	g_strSkinName = "Lilian";

    SetTreeListParam(11, 22);

    // 提示本进程的权限
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
