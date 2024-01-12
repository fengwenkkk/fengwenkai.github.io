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
#pragma resource "*.dfm"

TFormLogin *FormLogin;

UINT32  g_ulInitIndex = 0;
UINT32  g_ulInitNum = 30;
UINT32  g_ulOperType = OPER_TYPE_MANAGE;

extern BOOL InitRemoteMsg();

extern "C" VOID gos_encrypt_byte(UINT8 *pucByte);
extern "C" VOID gos_decrypt_byte(UINT8 *pucByte);

extern CHAR g_acLocalCfgFile[1024];

bool GetMaxInitNum()
{
    return LoadConfInfo("conf", "max_init_item", (INT32*)&g_ulInitNum);
}
//---------------------------------------------------------------------------
void SaveMaxInitNum()
{
    SaveConfInfo("conf", "max_init_item", g_ulInitNum);
}
//---------------------------------------------------------------------------
void SetInitOver()
{
    if (FormLogin)
    {
        FormLogin->SetInitProgress(g_ulInitIndex, g_ulInitIndex, "初始化成功");
    }

    SaveConfInfo("conf", "max_init_item", g_ulInitIndex);
}
//---------------------------------------------------------------------------
void SetInitInfoEx(AnsiString strInitInfo)
{
    if (FormLogin)
    {
        g_ulInitIndex++;
        if (g_ulInitNum < g_ulInitIndex)
        {
            g_ulInitNum = g_ulInitIndex;
        }

        FormLogin->SetInitProgress(g_ulInitIndex, g_ulInitNum, strInitInfo);
    }
}
//---------------------------------------------------------------------------
void SetInitInfo(AnsiString strInitInfo)
{
    SetInitInfoEx("正在初始化" + strInitInfo);
}
//---------------------------------------------------------------------------
void SaveLoginInfo(CHAR *szName, CHAR *szPwd)
{
    CHAR        acTmpPwd[128];
    CHAR        *szFile = g_acLocalCfgFile;

    if (!gos_file_exist(szFile))
    {
        return;
    }

    EncryptString(szPwd, acTmpPwd);

    GConfFile   Conf(szFile);

    Conf.Set("Sys", "user_name", szName);
    Conf.Set("Sys", "user_pwd", acTmpPwd);

    Conf.Save();
}
//---------------------------------------------------------------------------
AnsiString ClientLogin(CHAR *szUser, CHAR *szPwd)
{
	GJsonParam  ReqJsonParam;
	GJson       RspJson;

    ReqJsonParam.Add("User", szUser);
    ReqJsonParam.Add("Pwd", szPwd);

    if (!SyncSendServer(EV_OMT_OMC_REQ, MSG_OMT_Login, ReqJsonParam, RspJson))
	{
		return "登录失败 (系统错误)";
	}

	char    *szResult = RspJson.GetValue("Result");

    if (strcmp(szResult, "Succ") == 0)
    {
		if (!RspJson.GetValue("OperType", &g_ulOperType))
        {
            return "登录失败（系统错误）";
        }

        return "";
    }

    char    *szErrInfo = RspJson.GetValue("ErrInfo");
    if (!szErrInfo)
    {
        return "登录失败（系统错误）";
    }
    else
    {
        char    acErrInfo[1024];

        sprintf(acErrInfo, "登录失败（%s）", szErrInfo);

        return acErrInfo;
	}

    return "";
}
//---------------------------------------------------------------------------
VOID ClientLogout()
{
    // 登录失败情况不需要登出
    if (g_stLocalCfg.acUserName[0] == '\0')
    {
        return;
    }

    GJsonParam  ReqJsonParam;

    ReqJsonParam.Add("User", g_stLocalCfg.acUserName);

    SendServer(EV_OMT_OMC_REQ, MSG_OMT_Logout, ReqJsonParam);
}
//---------------------------------------------------------------------
__fastcall TFormLogin::TFormLogin(TComponent* AOwner)
    : TForm(AOwner)
{
    this->Width = PanelLogin->Width;
    this->Height = PanelLogin->Height;
    PanelLogin->Left = 0;
    PanelLogin->Top  = 0;

    Init();
    edtUser->Text = "";
    edtPwd->Text = "";
    ModalResult = mrNone;
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::InitSkin()
{

}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::SetInitMode()
{
    btnLogin->SetFocus();

    pbInit->Visible = true;
    pbInit->Properties->Max = g_ulInitNum;
    pbInit->Position = 0;
    pbInit->Properties->Text = "正在初始化";
}
//---------------------------------------------------------------------------
VOID _fastcall TFormLogin::Init()
{
//  edtUser->Text = g_stLocalCfg.acUserName;
//  edtPwd->Text = g_stLocalCfg.acUserPwd;

    edtUserPropertiesChange(this);
}
//---------------------------------------------------------------------
bool __fastcall TFormLogin::LoginCheck()
{
    AnsiString  strUser = edtUser->Text.Trim();
    AnsiString  strPwd  = edtPwd->Text.Trim();

    if (strUser.Length() == 0)
    {
        ErrMsg("请输入用户名!");
        g_stLocalCfg.acUserName[0] = '\0';
        edtUser->SetFocus();
        ModalResult = mrNone;

        return false;
    }

    if (strUser.Length() >= (int)sizeof(g_stLocalCfg.acUserName))
    {
        ErrMsg("用户名长度超出范围!");
        g_stLocalCfg.acUserName[0] = '\0';
        edtUser->SetFocus();
        ModalResult = mrNone;

        return false;
    }

    if (strPwd.Length() == 0)
    {
        ErrMsg("请输入密码!");
        g_stLocalCfg.acUserPwd[0] = '\0';
        edtPwd->SetFocus();
        ModalResult = mrNone;

        return false;
    }

    if (strPwd.Length() >= (int)sizeof(g_stLocalCfg.acUserPwd))
    {
        ErrMsg("密码长度超出范围!");
        g_stLocalCfg.acUserPwd[0] = '\0';

        edtPwd->SetFocus();
        ModalResult = mrNone;

        return false;
    }
    strcpy(g_stLocalCfg.acUserName, strUser.c_str());
    strcpy(g_stLocalCfg.acUserPwd, strPwd.c_str());

    SaveLoginInfo(g_stLocalCfg.acUserName, g_stLocalCfg.acUserPwd);

    AnsiString  strErrInfo =  ClientLogin(g_stLocalCfg.acUserName, g_stLocalCfg.acUserPwd);
    if (!strErrInfo.IsEmpty())
    {
        g_stLocalCfg.acUserName[0] = '\0';
        ErrMsg(strErrInfo);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::btnLoginClick(TObject *Sender)
{

    if (LoginCheck())
    {
        SetInitMode();
        ModalResult = mrOk;

/*      if (!bLogin)
        {
            bLogin = TRUE;
        }
  */
        //Init();
    }
    else
    {
        ModalResult = mrNone;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::btnExitClick(TObject *Sender)
{
    ModalResult = mrCancel;
    g_stLocalCfg.acUserName[0] = '\0';
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::SetInitProgress(UINT32 ulPos, UINT32 ulMaxPos, AnsiString strInitInfo)
{
    pbInit->Properties->Max = ulMaxPos;
    pbInit->Position = ulPos;

    pbInit->Properties->Text = strInitInfo;
    pbInit->Invalidate();
    pbInit->Update();
    PanelLogin->Invalidate();
    PanelLogin->Update();
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::InitProgress()
{
    ModalResult = mrNone;

    Width = this->Width;
    Height = this->Height;

    pbInit->Visible = true;

    lbUser->Visible = false;
    edtUser->Visible = false;
    lbPwd->Visible = false;
    edtPwd->Visible = false;
    btnLogin->Visible = false;
    btnExit->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::FormShow(TObject *Sender)
{
    InitSkin();
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::edtUserKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == 13)
    {
        btnLoginClick(Sender);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::edtPwdKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == 13)
    {
        btnLoginClick(Sender);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::edtUserPropertiesChange(TObject *Sender)
{
    LoginReady();
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::LoginReady()
{
    btnLogin->Enabled = false;

    if (edtUser->Text.IsEmpty() ||
        edtPwd->Text.IsEmpty() )
    {
        return;
    }

    btnLogin->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::edtPwdPropertiesChange(TObject *Sender)
{
    LoginReady();
}
