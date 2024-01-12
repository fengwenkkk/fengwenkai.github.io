//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmLogin.h"
#include "FrmInit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxGraphics"

#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxEdit"
#pragma link "cxTextEdit"
#pragma link "cxMaskEdit"
#pragma link "dxSkinsForm"
#pragma link "cxLabel"
#pragma link "cxDropDownEdit"
#pragma link "AdvSmoothLabel"
#pragma link "AdvSmoothPanel"
#pragma link "cxImage"
#pragma link "cxProgressBar"
#pragma link "dxGDIPlusClasses"
#pragma link "cxClasses"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinOffice2010Silver"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxCustomData"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxInplaceContainer"
#pragma link "cxStyles"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "trayicon"
#pragma resource "*.dfm"

TFormLogin *FormLogin;

static bool     g_bLogined = false;
UINT32  g_ulOperType = OPER_TYPE_MANAGE;

extern BOOL InitRemoteMsg();

extern "C" VOID gos_encrypt_byte(UINT8 *pucByte);
extern "C" VOID gos_decrypt_byte(UINT8 *pucByte);

extern CHAR g_acLocalCfgFile[1024];

/*
DC_OPERATOR_INFO_T* GetDCOperatorInfo(AnsiString strDCOperatorID)
{
    for (UINT32 i=0; i<g_vDCOperatorInfo.size(); i++)
    {
        if (strDCOperatorID == g_vDCOperatorInfo[i].acOperatorID)
        {
            return &g_vDCOperatorInfo[i];
        }
    }

    return NULL;
}    */

AnsiString GetDCOperatorName(AnsiString strDCOperatorID)
{
    return strDCOperatorID;
}

void SaveLoginInfo(CHAR *szName)
{
    CHAR        *szFile = g_acLocalCfgFile;

    if (!gos_file_exist(szFile))
    {
        return;
    }

    GConfFile   Conf(szFile);

    if (g_bDCLoginMode)
    {
        Conf.Set("Sys", "user_name", szName);
    }

    Conf.Save();
}

VOID DCLogout()
{
    // ��¼ʧ���������Ҫ�ǳ�
    if (g_strDCOperatorID.IsEmpty())
    {
        return;
    }

    GJsonParam  ReqJsonParam;

    ReqJsonParam.Add("User", g_strDCOperatorID.c_str());

    SendServer(EV_CLIENT_REQ, MSG_REC_Logout, ReqJsonParam);
}

AnsiString DCOperatorLogin(CHAR *szUser, CHAR *szPwd)
{
    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("User", szUser);
    ReqJsonParam.Add("Pwd", szPwd);

    if (!SyncSendServer(EV_CLIENT_REQ, MSG_REC_Login, ReqJsonParam, RspJson))
    {
        return "��¼ʧ�ܣ�ϵͳ����";
    }

    char    *szResult = RspJson.GetValue("Result");

    if (strcmp(szResult, "Succ") == 0)
    {
        if (!RspJson.GetValue("OperType", &g_ulOperType))
        {
            return "��¼ʧ�ܣ�ϵͳ����";;
        }

        return "";
    }

    char    *szErrInfo = RspJson.GetValue("ErrInfo");
    if (!szErrInfo)
    {
        return "��¼ʧ�ܣ�ϵͳ����";
    }
    else
    {
        char    acErrInfo[1024];

        sprintf(acErrInfo, "��¼ʧ�ܣ�%s��", szErrInfo);

        return acErrInfo;
    }
}

VOID DCOperatorLogout()
{
    // ��¼ʧ���������Ҫ�ǳ�
    if (g_strDCOperatorID.IsEmpty())
    {
        return;
    }

    GJsonParam  ReqJsonParam;

    ReqJsonParam.Add("User", g_strDCOperatorID);

    SendServer(EV_CLIENT_REQ, MSG_REC_Logout, ReqJsonParam);
}

AnsiString ClientLogin(CHAR *szUser, CHAR *szPwd)
{
    return DCOperatorLogin(szUser, szPwd);
}

VOID ClientLogout()
{
    GosLog(LOG_INFO, "client logout");
    if (g_bDCLoginMode)
    {
        DCLogout();
    }
    else
    {
        DCOperatorLogout();
    }
}

//---------------------------------------------------------------------
__fastcall TFormLogin::TFormLogin(TComponent* AOwner)
    : TForm(AOwner)
{
    LabelTitle->Caption = "�����е���14�������߼�Ⱥ¼��¼���ն�";

    this->Width = PanelLogin->Width;
    this->Height = PanelLogin->Height;
    PanelLogin->Left = 0;
    PanelLogin->Top  = 0;

    Init();
//    bInited = TRUE;
    ModalResult = mrNone;

    edtDCOperatorID->Text = "";
    edtDCOperatorPwd->Text = "";
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::InitSkin()
{

}
void __fastcall TFormLogin::SetInitMode()
{
    btnLogin->SetFocus();

/*    pbInit->Visible = true;
    pbInit->Properties->Max = g_ulInitNum;
    pbInit->Position = 0;
    pbInit->Properties->Text = "���ڳ�ʼ��"; */
}
//---------------------------------------------------------------------------
void _fastcall TFormLogin::Init()
{
    edtDCOperatorID->Text = g_strDCOperatorID;
}
//---------------------------------------------------------------------
bool __fastcall TFormLogin::DCOperatorLoginCheck()
{
    CHAR        acUserInfoPath[256];
    AnsiString  strDCOperatorID = edtDCOperatorID->Text.Trim();
    AnsiString  strDCOperatorPwd = edtDCOperatorPwd->Text.Trim();

    if (strDCOperatorID.Length() == 0)
    {
        ErrMsg("�������û�!");
        edtDCOperatorID->SetFocus();
        ModalResult = mrNone;

        return false;
    }

    if (strDCOperatorPwd.Length() == 0)
    {
        ErrMsg("����������!");
        edtDCOperatorPwd->SetFocus();
        ModalResult = mrNone;

        return false;
    }

    g_strDCOperatorID = "";

    SaveLoginInfo(strDCOperatorID.c_str());

    AnsiString strErrInfo = DCOperatorLogin(strDCOperatorID.c_str(), strDCOperatorPwd.c_str());

    if (!strErrInfo.IsEmpty())
    {
        ErrMsg(strErrInfo);
        return false;
    }

    g_strDCOperatorID = strDCOperatorID;
    g_strDCOperatorName = GetDCOperatorName(g_strDCOperatorID);

    return true;
}

void __fastcall TFormLogin::btnLoginClick(TObject *Sender)
{
    bool    bRet = false;

    bRet = DCOperatorLoginCheck();
    if (bRet)
    {
        SetInitMode();
        ModalResult = mrOk;
    }
    else
    {
        g_strDCOperatorID = "";
        ModalResult = mrNone;
    }

    g_bLogined = bRet;
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::btnExitClick(TObject *Sender)
{
    ModalResult = mrCancel;
    g_strDCOperatorID = "";
}

void __fastcall TFormLogin::FormShow(TObject *Sender)
{
    InitSkin();
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::edtDCUserIDKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == 13)
    {
        btnLoginClick(Sender);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::LoginReady()
{
    btnLogin->Enabled = false;

    if (edtDCOperatorID->Text.IsEmpty() ||
        edtDCOperatorPwd->Text.IsEmpty() )
    {
        return;
    }

    btnLogin->Enabled = true;
}
void __fastcall TFormLogin::edtPwdPropertiesChange(TObject *Sender)
{
    LoginReady();
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::edtDCOperatorIDPropertiesChange(TObject *Sender)
{
    LoginReady();
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::FormClose(TObject *Sender, TCloseAction &Action)
{
/*  Action = caHide;
    ModalResult = mrCancel;
    g_stLocalCfg.acDCUserName[0] = '\0';  */
}
//---------------------------------------------------------------------------

