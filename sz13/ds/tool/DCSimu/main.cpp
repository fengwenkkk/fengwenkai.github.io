//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"

#include "msg.h"
#include "ds_public.h"
#include "MsgDialog.h"
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxGraphics"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "dxSkinLilian"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxDropDownEdit"
#pragma link "cxEdit"
#pragma link "cxMaskEdit"
#pragma link "cxTextEdit"
#pragma link "cxCustomData"
#pragma link "cxInplaceContainer"
#pragma link "cxStyles"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "dxSkinsForm"
#pragma link "cxPC"
#pragma link "dxSkinscxPCPainter"
#pragma link "cxGroupBox"
#pragma link "cxClasses"
#pragma link "cxCheckBox"
#pragma link "cxSplitter"
#pragma link "cxLabel"
#pragma link "dxBarBuiltInMenu"
#pragma link "AdvGrid"
#pragma link "AdvObj"
#pragma link "AdvUtil"
#pragma link "BaseGrid"
#pragma link "cxButtonEdit"
#pragma resource "*.dfm"

TFormMain *FormMain;

#pragma pack(push, 1)

#pragma pack(pop)

BOOL                    g_bShowTitle = TRUE;

ApplicationContext      *g_AppContext = NULL;


DC_CFG_T                g_stDCCfg = {0};

UINT32                  g_ulDCUserID = 0;

extern BOOL    g_bMDCConnected;

BOOL GetClientCfg()
{
    CHAR    acFile[256];
    CHAR    *szValue;
    INT32   iValue;

    sprintf(acFile, "%s/%s", gos_get_root_path(), "dc_test.ini");

    if (!gos_file_exist(acFile))
    {
        GosLog(LOG_ERROR, "open file %s failed!", acFile);
        return FALSE;
    }

    GConfFile   Conf(acFile);

    if (!Conf.Get("user", g_stDCCfg.acUser, sizeof(g_stDCCfg.acUser)))
    {
        GosLog(LOG_ERROR, "get user of cfg failed");
        return FALSE;
    }

    if (!gos_atou(g_stDCCfg.acUser, &g_ulDCUserID))
    {
        GosLog(LOG_ERROR, "invalid user of cfg failed");
        return FALSE;
    }

    if (!Conf.Get("pwd", g_stDCCfg.acPwd, sizeof(g_stDCCfg.acPwd)))
    {
        GosLog(LOG_ERROR, "get pwd of cfg failed");
        return FALSE;
    }

    szValue = Conf.Get("server_addr");
    if (!gos_get_ip_port(szValue, g_stDCCfg.aucServerAddr, &g_stDCCfg.usServerPort))
    {
        GosLog(LOG_ERROR, "get server_addr of cfg failed");
        return FALSE;
    }

    if (g_stDCCfg.usServerPort == 0)
    {
        g_stDCCfg.usServerPort = 5060;
    }

    DOUBLE  dDelay;
    UINT16  usSeq;
    UINT32  ulTTL;

    if (!gos_ping(g_stDCCfg.aucServerAddr, 32, 2000, &dDelay, &usSeq, &ulTTL, g_stDCCfg.aucLocalAddr))
    {
        GosLog(LOG_ERROR, "get local addr failed, server addr is " IP_FMT, IP_ARG(g_stDCCfg.aucServerAddr));
        return FALSE;
    }

    sprintf(g_stDCCfg.acLocalAddr, IP_FMT, IP_ARG(g_stDCCfg.aucLocalAddr));
    sprintf(g_stDCCfg.acServerAddr, IP_FMT, IP_ARG(g_stDCCfg.aucServerAddr));

    return TRUE;
}

BOOL InitAppContext()
{
    g_AppContext = ApplicationContext::getInstance();

    if (!g_AppContext)
    {
        return FALSE;
    }

    g_AppContext->setWorkMode(APPMODE_DISPATCH_CONSOLE);
    g_AppContext->setBypassBuildMedia(bypassMedia);
    g_AppContext->setMemMgrMode(MEM_SDK_MGR);

    GDisplayManager* dm = new GDisplayManager();
    g_AppContext->setDisplayManager((DisplayManager*)dm);

    ubp::platform::trace::UBPTrace_T::instance()->Init("DC");

    int iRet = g_AppContext->initApp();

    if (iRet != 0)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL Login(UINT32 ulUserID, DC_CFG_T *pstCfg)
{
    DcState* dc = ac->getConsoleState();

    dc->setDCID(ulDCUserID);
    dc->setServerIP(pstCfg->acServerAddr);
    dc->setSipPort(pstCfg->usServerPort);
    dc->setUser(pstCfg->acUser);
    dc->setPwd(pstCfg->acPwd);
    dc->setLocalIP(pstCfg->acLocalAddr);

    ResourceInfo    *DCInfo = new ResourceInfo(ulUserID);

    DCInfo->setUserName(pstCfg->acUser);
    DCInfo->setPassWord(pstCfg->acPwd);
    DCInfo->setServerIp(pstCfg->acServerAddr);
    DCInfo->setLocalIp(pstCfg->acLocalAddr);
    DCInfo->setSipPort(pstCfg->usServerPort);

    /* 应用程序调用operationmanager来注册 */
    OperationManager * om = OperationManager_T::instance();

    //SDK_Helper::setLogonType(SPECIAL_PROXY);
    Op_Return_t iRet = om->invokeOp(DCInfo, DC_LOGIN);
    if (iRet != OP_OK_ACCEPTED)
    {
        return FALSE;
    }

    res_status_value_t iStatus = TDHelper::getinstance()->waitLoginStatus();
    if (iStatus != STATUS_REGOK)
    {
        return FALSE;
    }

    ac->startRunning(g_stDCCfg.acServerAddr);

    gos_sleep(2);
    ac->triggerStatusReport(true);

    return TRUE;
}


//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TSubForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerCheckTimer(TObject *Sender)
{
    LabelMDCStatus->Caption = g_bMDCConnected?"MDC状态：正常":"MDC状态：断开";
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnLoginClick(TObject *Sender)
{
    if (!Login())
    {
        InfoMsg("登录失败");
        return;
    }
}
//---------------------------------------------------------------------------

