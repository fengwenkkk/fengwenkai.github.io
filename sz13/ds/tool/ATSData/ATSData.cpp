//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include "ClientInclude.h"

#include "RemoteMsg.h"
#include "msg.h"
#include "ds_public.h"
#include "ats.h"
#include "FrmATSData.h"
#include "ClientInit.h"
#include "ClientUtil.h"

//---------------------------------------------------------------------------
USEFORM("FrmATSData.cpp", FormATSData);
//---------------------------------------------------------------------------
BOOL    g_bTestMode = FALSE;
BOOL    g_bNeedLogin = TRUE;
UINT32  g_ulCurrDCUserID = INVALID_USER_ID;

LOG_CFG_T   g_stLogCfg = {0};

void ClientConnectionChange(bool bConnected)
{
}

void LoadSkinCfg()
{
}

BOOL GetLocalCfg()
{
    CHAR    acFile[256];
    CHAR    *szValue;
    INT32   iValue;

    sprintf(acFile, "%s/%s", gos_get_root_path(), "dc.ini");

    if (!gos_file_exist(acFile))
    {
        ErrMsg("本地配置文件[%s]不存在!", acFile);
        return FALSE;
    }

    GConfFile   Conf(acFile);

    // log
    if (!Conf.Get("log", "log_level", LOG_INFO, &iValue))
    {
        GosLog(LOG_ERROR, "get log_level of Log failed");
        return FALSE;
    }

    if (iValue > LOG_FATAL)
    {
        iValue = LOG_FATAL;
    }
    else if (iValue < LOG_NULL)
    {
        iValue = LOG_NULL;
    }

    g_stLogCfg.bLogToStdout = FALSE;
    g_stLogCfg.bLogToFile = TRUE;
    g_stLogCfg.ulLogLevel = iValue;

    // log_file_num
    if (!Conf.Get("log", "log_file_num", 90, &iValue))
    {
        GosLog(LOG_ERROR, "get log_file_num of Log failed");
        return FALSE;
    }

    if (iValue < 2)
    {
        iValue = 2;
    }
    else if (iValue > 100)
    {
        iValue = 100;
    }

    g_stLogCfg.ulLogFileNum = iValue;

    // log_file_size
    if (!Conf.Get("log", "log_file_size", 4096, &iValue))
    {
        GosLog(LOG_ERROR, "get log_file_size of Log failed");
        return FALSE;
    }

    if (iValue < 1024)
    {
        iValue = 1024;
    }
    else if (iValue > 102400)
    {
        iValue = 102400;
    }

    g_stLogCfg.ulLogFileSize = 1024*iValue;


    return TRUE;
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

    if (!GetLocalCfg())
    {
        GosLog(LOG_FATAL, "Get local Cfg failed!");

        return FALSE;
    }

    GosInitLog(&g_stLogCfg);

    LoadSkinCfg();
    InitSystemFont();
    gos_init_svcenv();

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
    pLogger->SetLogToFile(g_stLogCfg.ulLogFileNum>0, g_stLogCfg.ulLogFileNum, g_stLogCfg.ulLogFileSize, 0);
    pLogger->SetLogLevel(g_stLogCfg.ulLogLevel);
    pLogger->SetLogModule(MODULE_DTP, MODULE_DTP_NAME);

    return TRUE;
}

//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	SetDefaultSkinName("Lilian");

    // 提示本进程的权限
    EnablePriv();

    if (!InitClientEnv())
    {
        return -1;
    }

    try
    {
        Application->Initialize();
        Application->MainFormOnTaskBar = false;
        Application->CreateForm(__classid(TFormATSData), &FormATSData);
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
