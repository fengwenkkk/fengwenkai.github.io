//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"

#include "MsgDialog.h"
#include "msg.h"
#include "ds_public.h"
#include "ClientUtil.h"
#include "Resource.h"

#include "DlgSetPredefinedSMS.h"
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
#pragma link "cxImageList"
#pragma resource "*.dfm"
TDialogSetPredefinedSMS *DialogSetPredefinedSMS;

//---------------------------------------------------------------------------
__fastcall TDialogSetPredefinedSMS::TDialogSetPredefinedSMS(TComponent* Owner)
    : TDialogForm(Owner)
{
    m_DictSDSType.Add(SDS_TYPE_NORMAL, "����");
    m_DictSDSType.Add(SDS_TYPE_STATUS, "״̬��Ϣ");
    m_DictSDSType.Add(SDS_TYPE_EMERG_STATUS, "����״̬��Ϣ");

    m_DictSDSType.Init(cbSDSType);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetPredefinedSMS::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetPredefinedSMS::Init()
{
    ModalResult = mrNone;
    m_bAdd = true;
    cbTX->Checked = true;
    cbFX->Checked = true;
    mmInfo->Text = "";

    Caption = "�½��ն�Ԥ�������Ϣ";
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetPredefinedSMS::Init(PREDEFINED_SMS_INFO_T &stInfo)
{
    ModalResult = mrNone;
    m_bAdd = false;

    cbTX->Checked = (stInfo.ulSMSTemplateType & SDS_TEMPLATE_TX);
    cbFX->Checked = (stInfo.ulSMSTemplateType & SDS_TEMPLATE_FX);
    m_DictSDSType.SetID(cbSDSType, stInfo.ulSDSType);
    mmInfo->Text = stInfo.acSMSInfo;
    m_stInfo.ulSMSID = stInfo.ulSMSID;

    Caption = "�����ն�Ԥ�������Ϣ";
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetPredefinedSMS::btnOKClick(TObject *Sender)
{
    ModalResult = mrNone;

    if (!cbTX->Checked && !cbFX->Checked)
    {
        InfoMsg("��ѡ��ģ�����ö���");
        cbTX->Focused();
        return;
    }

    if (!m_DictSDSType.GetID(cbSDSType, &m_stInfo.ulSDSType))
    {
        InfoMsg("��ѡ��ģ������");
        cbSDSType->Focused();
        return;
    }

    m_stInfo.ulSMSTemplateType = 0;
    if (cbTX->Checked)
    {
        m_stInfo.ulSMSTemplateType |= SDS_TEMPLATE_TX;
    }

    if (cbFX->Checked)
    {
        m_stInfo.ulSMSTemplateType |= SDS_TEMPLATE_FX;
    }

    UINT32  ulMaxLen = sizeof(m_stInfo.acSMSInfo) - 1;

    AnsiString  strTmp = mmInfo->Text.Trim();
    if (strTmp.IsEmpty())
    {
        InfoMsg("����Ϣ����Ϊ��!");
        mmInfo->Focused();
        return;
    }

    if (strTmp.Length() > (int)ulMaxLen)
    {
        InfoMsg("����Ϣ���ݳ�����󳤶ȣ�%u", ulMaxLen);
        mmInfo->Focused();
        return;
    }

    StrCpy(m_stInfo.acSMSInfo, strTmp);

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetPredefinedSMS::Get(PREDEFINED_SMS_INFO_T &stInfo)
{
    memcpy(&stInfo, &m_stInfo, sizeof(m_stInfo));
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetPredefinedSMS::FormShow(TObject *Sender)
{
    if (cbSDSType->ItemIndex < 0)
    {
        cbSDSType->SetFocus();
    }
    else
    {
        mmInfo->SetFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetPredefinedSMS::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------

