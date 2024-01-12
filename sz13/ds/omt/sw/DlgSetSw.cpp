//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"

#include "MsgDialog.h"
#include "msg.h"
#include "ds_public.h"
#include "ClientUtil.h"
#include "Resource.h"

#include "DlgSetSw.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxLabel"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxTrackBar"
#pragma link "dxSkinBlue"
#pragma link "dxSkinDarkRoom"
#pragma link "dxSkinDarkSide"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinLilian"
#pragma link "dxSkinMcSkin"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "cxTextEdit"
#pragma link "cxCheckBox"
#pragma link "cxDropDownEdit"
#pragma link "cxMaskEdit"
#pragma link "cxButtonEdit"
#pragma link "cxCustomData"
#pragma link "cxInplaceContainer"
#pragma link "cxStyles"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "cxMemo"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxCheckListBox"
#pragma link "cxCustomListBox"
#pragma link "dxBevel"
#pragma link "cxImageList"
#pragma link "cxProgressBar"
#pragma resource "*.dfm"
TDialogSetSw *DialogSetSw;

ID_VALUE_T  s_astSwType[] = {   {SW_TYPE_TX,    "车载台主机"},
                                {SW_TYPE_FX,    "固定台主机"},
                                };

Dict    g_DictSwType(s_astSwType, ARRAY_SIZE(s_astSwType));

extern UINT16       g_usFtpPort;
extern CHAR         g_acFtpVerDir[256];
extern CHAR         g_acFtpUser[32];
extern CHAR         g_acFtpPwd[32];

extern BOOL TransFile(UINT8 *pucServerAddr, UINT16 usServerPort, CHAR *szFile, CHAR *szNewFile, UINT32 ulMaxDataLen, BOOL *pbCancel);

AnsiString GetSwTypeText(UINT32 ulSwType)
{
    for (UINT32 i=0; i<ARRAY_SIZE(s_astSwType); i++)
    {
        if (s_astSwType[i].ulID == ulSwType)
        {
            return s_astSwType[i].szValue;
        }
    }

    return "";
}

//---------------------------------------------------------------------------
__fastcall TDialogSetSw::TDialogSetSw(TComponent* Owner)
    : TDialogForm(Owner)
{
    g_DictSwType.Init(cbType);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetSw::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetSw::Init()
{
    ModalResult = mrNone;
    edtVer->Text = "";
    edtFile->Text = "";
    cbType->ItemIndex = -1;
    cbUpload->Position = 0;

    btnOK->Enabled = false;

    m_bUploaded = false;
    Caption = "增加软件版本";
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetSw::btnOKClick(TObject *Sender)
{
    ModalResult = mrNone;

    AnsiString  strTmp = edtVer->Text.Trim();
    if (strTmp.IsEmpty())
    {
        InfoMsg("请输入版本名称");
        edtVer->Focused();
        return;
    }

    if (strTmp.Length() >= (int)sizeof(m_stInfo.acSwVer))
    {
        InfoMsg("版本名称超出最大长度：%u", sizeof(m_stInfo.acSwVer)-1);
        edtVer->Focused();
        return;
    }

    StrCpy(m_stInfo.acSwVer, strTmp);

    /*
    strTmp = edtFile->Text.Trim();
    if (strTmp.IsEmpty())
    {
        InfoMsg("请输入版本文件");
        edtFile->Focused();
        return;
    }

    if (!FileExists(strTmp))
    {
        InfoMsg("版本文件不存在");
        edtFile->Focused();
        return;
    }   */

    if (!g_DictSwType.GetID(cbType, &m_stInfo.ulSwType))
    {
        InfoMsg("版本类型非法");
        cbType->Focused();
        return;
    }

    if (!m_bUploaded)
    {
        InfoMsg("请先上传版本文件");
        btnUpload->Focused();
        return;
    }

    strTmp = edtFile->Text.Trim();
    gos_get_file_rawname(strTmp.c_str(), m_stInfo.acSwFile);
    m_stInfo.ulSwTime = gos_get_current_time();

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetSw::Get(OMC_SW_INFO_T &stInfo)
{
    memcpy(&stInfo, &m_stInfo, sizeof(stInfo));
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetSw::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetSw::btnUploadClick(TObject *Sender)
{
    AnsiString  strFile = edtFile->Text.Trim();

    btnOK->Enabled = false;

    if (strFile.IsEmpty())
    {
        InfoMsg("请输入版本文件");
        edtFile->Focused();
        return;
    }

    if (!FileExists(strFile))
    {
        InfoMsg("版本文件不存在");
        edtFile->Focused();
        return;
    }

    UINT8   *pucServerAddr = g_stLocalCfg.aucOMCAddr;
    UINT16  usServerPort = TRANS_FILE_PORT;
    CHAR    *szLocalFile = strFile.c_str();
    CHAR    acServerFile[256];
    UINT32  ulMaxDataLen = 32*1024;
    BOOL    bCancel = FALSE;

    gos_get_file_rawname(szLocalFile, acServerFile);

    BOOL bRet = gos_ftp_put(g_acFtpUser, g_acFtpPwd, g_stLocalCfg.aucOMCAddr,
                       g_usFtpPort, acServerFile, szLocalFile);

//  BOOL bRet = TransFile(pucServerAddr, usServerPort, szFile, acNewFile, ulMaxDataLen, &bCancel);
 //
    if (!bRet)
    {
        ErrMsg("文件上传失败");
    }
    else
    {
        InfoMsg("文件上传成功");
    }

    m_bUploaded = bRet;
    btnOK->Enabled = bRet;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetSw::edtFilePropertiesButtonClick(TObject *Sender, int AButtonIndex)
{
    if (!OpenDialog->Execute())
    {
        return;
    }

    edtFile->Text = OpenDialog->FileName;

    char    acVer[256];

    StrCpy(acVer, edtFile->Text);
    gos_format_path(acVer);

    char    *szTmp = gos_right_strchr(acVer, '.');

    if (szTmp)
    {
        *szTmp = '\0';
    }

    szTmp = gos_right_strchr(acVer, '\\');
    if (szTmp)
    {
        szTmp++;
    }
    else
    {
        szTmp = acVer;
    }

    edtVer->Text = szTmp;
}
//---------------------------------------------------------------------------

void __fastcall TDialogSetSw::edtFilePropertiesEditValueChanged(TObject *Sender)
{
    btnOK->Enabled = false;
    m_bUploaded = false;
}
//---------------------------------------------------------------------------

