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

AnsiString DCLogin(AnsiString strUser, AnsiString strPwd, UINT32 ulDCType)
{
    GJsonParam  ReqJsonParam;
    GJson       RspJson;
    std::string strEncodePassword = gos::base64::EncodeString(strPwd.c_str());

    ReqJsonParam.Add("User", strUser);
    ReqJsonParam.Add("Pwd", strEncodePassword);
    ReqJsonParam.Add("DCType", ulDCType);

    bool    bRet = false;

    if (APP::AllowDISOffline() )
    {
        bRet = true;
    }
    else
    {
        if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCOperatorLogin, ReqJsonParam, RspJson))
        {
            return "µÇÂ¼Ê§°Ü£¨ÏµÍ³´íÎó£©";
        }

        char *szResult = RspJson.GetValue("Result");

        if (szResult &&
            strcmp(szResult, "Succ") == 0)
        {
            bRet = true;
        }
    }

    if (bRet)
    {
        APP::SetDCAccount(strUser, strPwd, ulDCType);

        return "";
    }

    char *szErrInfo = RspJson.GetValue("ErrInfo");

    if (!szErrInfo)
    {
        return "µÇÂ¼Ê§°Ü£¨ÏµÍ³´íÎó£©";
    }
    else
    {
        char acErrInfo[1024];

        sprintf(acErrInfo, "µÇÂ¼Ê§°Ü£¨%s£©", szErrInfo);

        return acErrInfo;
    }
}

VOID DCLogout()
{
    // µÇÂ¼Ê§°ÜÇé¿ö²»ÐèÒªµÇ³ö
    if (!APP::IsDCLogin())
    {
        return;
    }

    GJsonParam ReqJsonParam;

    ReqJsonParam.Add("User", APP::GetDCLoginAccount());

    SendServer(EV_DC_DIS_CFG_REQ, MSG_DCOperatorLogout, ReqJsonParam);
}

//---------------------------------------------------------------------
__fastcall TFormLogin::TFormLogin(TComponent* AOwner)
    : TForm(AOwner)
{
    this->Width = PanelLogin->Width;
    this->Height = PanelLogin->Height;
    PanelLogin->Left = 0;
    PanelLogin->Top  = 0;

    VECTOR<DEPOT_INFO_T*>   &vDepotInfo = Depot::GetDepotInfo();

    for (UINT32 i=0; i<vDepotInfo.size(); i++)
    {
        if (vDepotInfo[i]->ulDepotID != DEPOT_ID_MAINLINE)
        {
            m_DictDepotID.Add(vDepotInfo[i]->ulDepotID, vDepotInfo[i]->acDepotName);
        }
    }

    m_DictDepotID.Init(cbDepotID);

    ModalResult = mrNone;

    AnsiString  strDCOperatorID = "";
    AnsiString  strDCOperatorPwd = "";

    if (LocalCfg::GetInstance().GetAutoLoginDIS())
    {
        strDCOperatorID  = LoadConfInfo("Param", "LoginUser");
        strDCOperatorPwd = LoadConfInfo("Param", "LoginPwd");
    }

    edtDCOperatorID->Text  = strDCOperatorID;
    edtDCOperatorPwd->Text = strDCOperatorPwd;

    cbDCTypePropertiesChange(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::InitSkin()
{
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::SetInitMode()
{
    btnLogin->SetFocus();
}
//---------------------------------------------------------------------
bool __fastcall TFormLogin::DCLoginCheck(bool bShowErrMsg)
{
    CHAR        acUserInfoPath[256];
    AnsiString  strDCOperatorID = edtDCOperatorID->Text.Trim();
    AnsiString  strDCOperatorPwd = edtDCOperatorPwd->Text.Trim();

    if (strDCOperatorID.Length() == 0)
    {
        ErrMsg("ÇëÊäÈëÓÃ»§!");
        edtDCOperatorID->SetFocus();
        ModalResult = mrNone;

        return false;
    }

    if (strDCOperatorPwd.Length() == 0)
    {
        ErrMsg("ÇëÊäÈëÃÜÂë!");
        edtDCOperatorPwd->SetFocus();
        ModalResult = mrNone;

        return false;
    }

    UINT32  ulDCType = 0;
    UINT32  ulDepotID = DEPOT_ID_UNKNOWN;

    if (!m_DictDCType.GetID(&ulDCType))
    {
        return false;
    }

    APP::SetDCType(ulDCType);

    if (APP::HasDCType(DC_TYPE_TRAIN) ||
        APP::HasDCType(DC_TYPE_PASSENGER))
    {
        ulDepotID = DEPOT_ID_MAINLINE;
    }
    else if (APP::HasDCType(DC_TYPE_FIX) ||
             APP::HasDCType(DC_TYPE_DEPOT))
    {
        if (!m_DictDepotID.GetID(&ulDepotID))
        {
            return false;
        }
    }

    APP::SetDepotID(ulDepotID);

    LocalCfg::GetInstance().SaveLoginInfo(strDCOperatorID.c_str(), ulDCType, ulDepotID);

    AnsiString strErrInfo = DCLogin(strDCOperatorID, strDCOperatorPwd, ulDCType);

    if (!strErrInfo.IsEmpty())
    {
        ErrMsg(strErrInfo);
        return false;
    }

    SaveConfInfo("Param", "LoginUser", strDCOperatorID);
    SaveConfInfo("Param", "LoginPwd", strDCOperatorPwd);

    return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::btnLoginClick(TObject *Sender)
{
    Login(true);
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::Login(bool bShowErrMsg)
{
    bool bRet;

    bRet = DCLoginCheck(bShowErrMsg);
    if (bRet)
    {
        SetInitMode();
        ModalResult = mrOk;

        /// µÇÂ¼³É¹¦
        APP::SetDCLoginStatus(APP::STATUS_LOGIN);
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
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::FormShow(TObject *Sender)
{
    InitSkin();
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::edtDCOperatorPwdKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == 13)
    {
        btnLoginClick(Sender);
        //Login(false);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::LoginReady()
{
    btnLogin->Enabled = false;

    if (edtDCOperatorID->Text.IsEmpty() ||
        edtDCOperatorPwd->Text.IsEmpty())
    {
        return;
    }

    if (cbDCType->ItemIndex < 0)
    {
        return;
    }

    UINT32  ulDCType;

    if (!m_DictDCType.GetID(&ulDCType) )
    {
        return;
    }

    if (ulDCType == DC_TYPE_FIX ||
        ulDCType == DC_TYPE_DEPOT )
    {
        if (cbDepotID->ItemIndex < 0)
        {
            return;
        }
    }

    btnLogin->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::cbDCTypePropertiesChange(TObject *Sender)
{
    LoginReady();

    UINT32  ulDCType;

    cbDepotID->ItemIndex = -1;

    if (!m_DictDCType.GetID(&ulDCType) )
    {
        btnLogin->Top = 275;
        btnExit->Top = btnLogin->Top;

        return;
    }

    if (ulDCType == DC_TYPE_FIX ||
        ulDCType == DC_TYPE_DEPOT )
    {
        if (ManageOneDepot())
        {
            cbDepotID->Visible = true;
        }
        else
        {
            cbDepotID->Visible = false;
            cbDepotID->ItemIndex = 0;
        }
    }
    else if (ulDCType == DC_TYPE_TRAIN ||
             ulDCType == DC_TYPE_PASSENGER)
    {
        cbDepotID->Visible = false;
        m_DictDepotID.SetID(DEPOT_ID_MAINLINE);
    }
    else
    {
        cbDepotID->Visible = false;
        m_DictDepotID.SetID(DEPOT_ID_MAINLINE);
    }

    lbDepot->Visible = cbDepotID->Visible;
    if (cbDepotID->Visible)
    {
        btnLogin->Top = 315;
    }
    else
    {
        btnLogin->Top = 275;
    }

    btnExit->Top = btnLogin->Top;
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::edtPwdPropertiesChange(TObject *Sender)
{
    LoginReady();
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::edtDCOperatorIDPropertiesChange(TObject *Sender)
{
    DC_OPERATOR_INFO_T  *pstInfo;

    cbDCType->Clear();
    LoginReady();

    pstInfo = DCOperator::GetDCOperator(edtDCOperatorID->Text);
    if (!pstInfo)
    {
        return;
    }

    InitDCTypeDict(pstInfo->ulDCType, m_DictDCType);
    m_DictDCType.Init(cbDCType);
    m_DictDCType.SetID(APP::GetDCType());

    if (cbDCType->Properties->Items->Count == 1)
    {
        cbDCType->ItemIndex = 0;
    }

    LoginReady();
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::edtDCOperatorIDKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == 13)
    {
        if (!edtDCOperatorID->Text.IsEmpty())
        {
            edtDCOperatorPwd->SetFocus();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::TimerAutoLoginTimer(TObject *Sender)
{
    if (!TimerAutoLogin->Enabled)
    {
        return;
    }

    TimerAutoLogin->Enabled = false;

    if (!LocalCfg::GetInstance().GetAutoLoginDIS())
    {
        return;
    }

    if (!edtDCOperatorID->Text.IsEmpty() &&
        !edtDCOperatorPwd->Text.IsEmpty() &&
        cbDCType->ItemIndex >= 0)
    {
        btnLoginClick(this);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormLogin::cbDepotIDPropertiesEditValueChanged(TObject *Sender)
{
    LoginReady();
}
//---------------------------------------------------------------------------

