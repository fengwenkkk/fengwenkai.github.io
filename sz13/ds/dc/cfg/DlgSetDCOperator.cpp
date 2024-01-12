//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"

#include "DlgSetDCOperator.h"
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
#pragma link "dxSkinGlassOceans"
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
#pragma resource "*.dfm"
TDialogSetDCOperator *DialogSetDCOperator;

//---------------------------------------------------------------------------
__fastcall TDialogSetDCOperator::TDialogSetDCOperator(TComponent* Owner)
    : TDialogForm(Owner)
{
    VECTOR<ID_VALUE_T>  vDCType;

    GetDCType(vDCType);

    clbDCType->Clear();
    for (UINT32 i=0; i<vDCType.size(); i++)
    {
        TcxCheckListBoxItem     *Item = clbDCType->Items->Add();

        Item->Text = vDCType[i].szValue;
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDCOperator::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDCOperator::Init()
{
    ModalResult = mrNone;
    m_bAdd = true;
    edtUserID->Text = "";
    edtUserName->Text = "";
    edtPwd->Properties->PasswordChar = '\0';

    SetEditable(edtUserID, true);
    SetEditable(clbDCType, true);

    Caption = "增加调度员";

    for (int i=0; i<clbDCType->Items->Count; i++)
    {
        TcxCheckListBoxItem     *Item = clbDCType->Items->Items[i];

        Item->Checked = false;
    }
}

void __fastcall TDialogSetDCOperator::Init(DC_OPERATOR_INFO_T &stInfo)
{
    ModalResult = mrNone;
    m_bAdd = false;
    edtUserID->Text = stInfo.acOperatorID;
    edtUserName->Text = stInfo.acName;
    edtPwd->Text = stInfo.acPassword;
    SetEditable(edtUserID, false);

    Caption = "设置调度员";

    if (APP::GetDCLoginAccount() != "admin")
    {
        edtPwd->Properties->PasswordChar = '●';
    }
    else
    {
        edtPwd->Properties->PasswordChar = '\0';
    }

    bool    bAll = (strcmp(stInfo.acOperatorID, "admin") == 0);
    VECTOR<ID_VALUE_T>  vDCType;

    GetDCType(vDCType);

    clbDCType->Clear();
    for (UINT32 i=0; i<vDCType.size(); i++)
    {
        TcxCheckListBoxItem     *Item = clbDCType->Items->Add();

        Item->Text = vDCType[i].szValue;
        Item->Checked = false;

        if (bAll || (stInfo.ulDCType & vDCType[i].ulID) )
        {
            Item->Checked = true;
        }
    }

    SetEditable(clbDCType, !bAll);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDCOperator::btnOKClick(TObject *Sender)
{
    AnsiString  strTmp = edtUserID->Text.Trim();

    if (strTmp.Length() < 3)
    {
        InfoMsg("调度员账号长度不能低于3位");
        edtUserID->Focused();
        ModalResult = mrNone;
        return;
    }

    if (strTmp.Length() >= (int)sizeof(m_stInfo.acOperatorID))
    {
        InfoMsg("调度员账号长度过长");
        edtUserID->Focused();
        ModalResult = mrNone;
        return;
    }

    StrCpy(m_stInfo.acOperatorID, strTmp);

    strTmp = edtUserName->Text.Trim();
    if (strTmp.Length() < 4)
    {
        InfoMsg("调度员姓名长度过短\n(最少两个中文字符)");
        edtUserName->Focused();
        ModalResult = mrNone;
        return;
    }

    if (strTmp.Length() >= (int)sizeof(m_stInfo.acName))
    {
        InfoMsg("调度员姓名长度过长");
        edtUserName->Focused();
        ModalResult = mrNone;
        return;
    }

    StrCpy(m_stInfo.acName, strTmp);

    strTmp = edtPwd->Text.Trim();
    if (strTmp != m_stInfo.acOperatorID)
    {
        if (strTmp.Length() < 4)
        {
            InfoMsg("调度员密码不能低于4位");
            edtPwd->Focused();
            ModalResult = mrNone;
            return;
        }

        if (strTmp.Length() >= (int)sizeof(m_stInfo.acPassword))
        {
            InfoMsg("调度员密码长度过长");
            edtPwd->Focused();
            ModalResult = mrNone;
            return;
        }
    }

    StrCpy(m_stInfo.acPassword, strTmp);

    VECTOR<ID_VALUE_T>  vDCType;

    GetDCType(vDCType);

    m_stInfo.ulDCType = 0;
    for (int i=0; i<clbDCType->Items->Count; i++)
    {
        TcxCheckListBoxItem     *Item = clbDCType->Items->Items[i];

        if (Item->Checked)
        {
            m_stInfo.ulDCType |= vDCType[i].ulID;
        }
    }

    if (strcmp(m_stInfo.acOperatorID, "admin") == 0)
    {
        m_stInfo.ulDCType = DC_TYPE_ALL;
    }
    else if (m_stInfo.ulDCType == 0)
    {
        InfoMsg("请选择管理范围");
        clbDCType->Focused();
        ModalResult = mrNone;
        return;
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDCOperator::Get(DC_OPERATOR_INFO_T &stInfo)
{
    memcpy(&stInfo, &m_stInfo, sizeof(stInfo));
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDCOperator::FormShow(TObject *Sender)
{
    if (m_bAdd)
    {
        edtUserID->SetFocus();
    }
    else
    {
        edtUserName->SetFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDCOperator::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDCOperator::edtUserIDPropertiesChange(TObject *Sender)
{
    edtPwd->Text = edtUserID->Text;
}
//---------------------------------------------------------------------------

