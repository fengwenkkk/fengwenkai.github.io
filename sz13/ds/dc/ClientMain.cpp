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
 * @brief           ����ʾ��¼����֮ǰ�������ݿ��м���һЩ��Ϣ
 * @param           strErrorMsg    [out]    ������������ò������ش�����Ϣ
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-15 14:16:51
 * @note
 */
BOOL LoadDataBeforeLogin(AnsiString& strErrorMsg)
{
    /// �������ݿ��б� dcoperator �е�������Ϣ, ������SDK�˺ŵ�����Ϣ
    if (!LoadDCUserInfo() &&  // ��������
        !LoadDCUserInfo())
    {
        GosLog(LOG_FATAL, "Load dc user data failed");
        strErrorMsg = "���ص���̨��Ϣʧ��";

        return FALSE;
    }

    /// �������ݿ��б� dcoperator �е�������Ϣ, �����ص���Ա�˺ŵ�����Ϣ
    if (!LoadDCOperatorInfo())
    {
        GosLog(LOG_FATAL, "Load dc operator data failed");
        strErrorMsg = "���ص���̨��Ϣʧ��";

        return FALSE;
    }

    /// �������ݿ�� depot �е���Ϣ�������س�������Ϣ
    if (!LoadDepotInfo())
    {
        GosLog(LOG_FATAL, "Load depot data failed");
        strErrorMsg = "���س�������Ϣʧ��";

        return FALSE;
    }

    // DISȫ������
    if (!LoadAPPCfg())
    {
        GosLog(LOG_FATAL, "Load app cfg failed");
        strErrorMsg = "����ҵ��������Ϣʧ��";

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
        InitFail("����Ѿ�����");
        return -1;
    }

    SetDefaultSkinName("Office2010Blue");

    // ���������̵�Ȩ��
    EnablePriv();

    gos_init_svcenv();

    GosLog(LOG_INFO, "dc ver: %s", LocalCfg::GetInstance().GetDCVersion().c_str());
 //   gos::PrintCurrentVersion(LocalCfg::GetInstance().GetAppTitle().c_str(), LocalCfg::GetInstance().GetDCVersion().c_str());

    if (!InitClientEnv())
    {
        GosLog(LOG_ERROR, "init client env failed!");

        InitFail("��ʼ��ʧ��");

        return 0;
    }

    /// ��ʼ����disͨ�ŵ�ģ��
    if (!InitRemoteMsg())
    {
        GosLog(LOG_FATAL, "init remote msg failed");

        if (!AskMsg("���ӵ����ȷ�����ʧ��, \n�Ƿ񽵼�����?"))
        {
            InitFail("");
            return 0;
        }

        APP::AllowDISOffline(true);
    }

    if (!InitMRSRemoteMsg())
    {
        GosLog(LOG_FATAL, "init mrs remote msg failed");
        InitFail("���ӵ�¼��¼�������ʧ��!");
        return 0;
    }

    AnsiString strErrorMsg;
    /// ����ʾ��¼����֮ǰ�������ݿ��м���һЩ��Ϣ
    if(!LoadDataBeforeLogin(strErrorMsg))
    {
        InitFail(strErrorMsg);
        return 0;
    }

    GosLog(LOG_INFO, "login to dis server start");

    /// �����¼����
    if (!LoginDIS())
    {
        InitFail("");

        return 0;
    }

    /// ��֤���������ļ��е� SDK ��½�˺��ǲ��������ݿ��(dcuser)��
    if (!DCUser::GetDCUser(APP::GetSDKLoginShortNum()))
    {
        strErrorMsg.printf("��ȡ��ǰ����̨%u��Ϣʧ��", APP::GetSDKLoginShortNum());
        InitFail(strErrorMsg);

        return 0;
    }

    /// ��ʾ����������
    FormInit = new TFormInit(Application);
    FormInit->Show();
    FormInit->Invalidate();
    FormInit->Repaint();

   // eSDK_StopLocalService();
    eSDK_StartLocalService();

    if (!InitNativeServicePath())
    {
        InitFail("��ȡ����ͨ�ŷ�����Ϣʧ�ܣ�");

        return 0;
    }

    /// �������ݿ��еľ�̬����
    GosLog(LOG_INFO, "start load cfg data from dis");
    if (!LoadCfgData())
    {
        InitFail("��������ʧ�ܣ�");

        return 0;
    }

    if (LocalCfg::GetInstance().NeedLogin() &&
        !eSDK_IsLogined())
    {
        GosLog(LOG_INFO, "login to MDC");
        SetInitInfoEx("���ڵ�¼��Ⱥ������");
        if (!APP::Login(NULL))
        {
            GosLog(LOG_ERROR, "login to MDC failed");
            InitFail("��¼��Ⱥ������ʧ�ܣ�");
            return 0;
        }

        if (!InitAudioDevList())
        {
            GosLog(LOG_ERROR, "load speaker and mic info failed");
            InitFail("������Ƶ��Դ��Ϣʧ�ܣ�");
            return 0;
        }

        GosLog(LOG_INFO, "eSDK ver is %s", eSDK_GetVer().c_str());
    }

    try
    {
        Application->Initialize();

        Application->MainFormOnTaskBar = false;

        SetInitInfo("ϵͳ����");
        GosLog(LOG_INFO, "create form main");

        TForm **pMainForm = &(Application->MainForm);

        Application->CreateForm(__classid(TFormMain), &FormMain);
        GetTreeListHeaderStyle();  // Ŀ���Ǵ�������
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
