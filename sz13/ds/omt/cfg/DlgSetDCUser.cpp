//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"

#include "MsgDialog.h"
#include "msg.h"
#include "ds_public.h"
#include "ClientUtil.h"
#include "Resource.h"

#include "DlgSetDCUser.h"
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
#pragma resource "*.dfm"
TDialogSetDCUser *DialogSetDCUser;


ID_VALUE_T  s_astDCType[] = {
#if 0       
       //{DC_TYPE_ALL,         "ȫ���ܵ���̨"},
                                {DC_TYPE_TRAIN,     "�г�����̨"},
//                              {DC_TYPE_PASSENGER, "�˿͵���̨"},
                                {DC_TYPE_DEPOT,     "�����ε���̨"},
                                {DC_TYPE_FIX,       "ά�޵���̨"},
                                {DC_TYPE_ENV,       "���ص���̨"},
                                {DC_TYPE_POWER,     "��������̨"},
//                              {DC_TYPE_REC,       "¼��¼���ն�"},
                                {DC_TYPE_MANAGER,   "ά������̨"},
                                };
#endif
                                {DC_TYPE_TRAIN,     "�г�����̨"},
                                {DC_TYPE_SHUNT,     "��������̨"},
                                {DC_TYPE_PASSENGER, "�˿͵���̨"},
                                {DC_TYPE_DEPOT,     "�γ�����̨"},
                                {DC_TYPE_ENV,       "�豸����̨"},
                                {DC_TYPE_MANAGER,   "ά������̨"},
                                {DC_TYPE_MASTER,    "�ܿص���̨"},
                            }��
                            
Dict    g_DictDCType(s_astDCType, ARRAY_SIZE(s_astDCType));

VOID GetDCType(UINT32 ulDCType, Dict *pDict)
{
    pDict->Clear();

    for (UINT32 i=0; i<ARRAY_SIZE(s_astDCType); i++)
    {
        if ((s_astDCType[i].ulID & ulDCType) == s_astDCType[i].ulID)
        {
            pDict->Add(&s_astDCType[i]);
        }
    }
}


AnsiString GetDCTypeText(UINT32 ulDCType)
{
    if (ulDCType == DC_TYPE_ALL)
    {
        return "ȫ���ܵ���̨";
    }

    AnsiString  strText = "";

    for (UINT32 i=0; i<ARRAY_SIZE(s_astDCType); i++)
    {
        if ((s_astDCType[i].ulID & ulDCType) == s_astDCType[i].ulID)
        {
            if (strText.IsEmpty())
            {
                strText = s_astDCType[i].szValue;
            }
            else
            {
                strText += AnsiString(",") + AnsiString(s_astDCType[i].szValue);
            }
        }
    }

    return strText;
}

//---------------------------------------------------------------------------
__fastcall TDialogSetDCUser::TDialogSetDCUser(TComponent* Owner)
    : TDialogForm(Owner)
{
    clbDCType->Clear();
    for (UINT32 i=0; i<ARRAY_SIZE(s_astDCType); i++)
    {
        TcxCheckListBoxItem     *Item = clbDCType->Items->Add();

        Item->Text = s_astDCType[i].szValue;
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDCUser::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDCUser::Init()
{
    ModalResult = mrNone;
    m_bAdd = true;
    edtUserID->Text = "";
    edtUserName->Text = "";
    edtUserID->Enabled = true;

    Caption = "���ӵ���̨�û�";

    for (INT32 i=0; i<clbDCType->Items->Count; i++)
    {
        TcxCheckListBoxItem     *Item = clbDCType->Items->Items[i];

        Item->Checked = false;
    }
}
void __fastcall TDialogSetDCUser::Init(UINT32 ulUserID, AnsiString strUserName, UINT32 ulDCType)
{
    ModalResult = mrNone;
    m_bAdd = false;
    edtUserID->Text = ulUserID;
    edtUserName->Text = strUserName;
    edtUserID->Enabled = false;

    Caption = "���õ���̨�û�";
//  clbDCType->SetFocus();

    bool    bAll = (ulDCType == DC_TYPE_ALL);

    clbDCType->Clear();
    for (UINT32 i=0; i<ARRAY_SIZE(s_astDCType); i++)
    {
        TcxCheckListBoxItem     *Item = clbDCType->Items->Add();

        Item->Text = s_astDCType[i].szValue;

        Item->Checked = false;
        if (i == 0)
        {
            Item->Checked = bAll;
        }
        else if (ulDCType & s_astDCType[i].ulID)
        {
            Item->Checked = true;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDCUser::btnOKClick(TObject *Sender)
{
    INT32       iUserID = edtUserID->Text.ToIntDef(-1);

    if (iUserID <= 0)
    {
        InfoMsg("�û�����Ƿ�");
        edtUserID->Focused();
        ModalResult = mrNone;
        return;
    }

    m_ulUserID = iUserID;

    UINT32  ulMaxLen = sizeof(((DC_USER_INFO_T*)0)->acName) - 1;

    m_strUserName = edtUserName->Text.Trim();
    if ((UINT32)m_strUserName.Length() > ulMaxLen)
    {
        InfoMsg("�û����Ƴ�����󳤶ȣ�%u", ulMaxLen);
        edtUserName->Focused();
        ModalResult = mrNone;
        return;
    }

    m_ulDCType = 0;
    for (INT32 i=0; i<clbDCType->Items->Count; i++)
    {
        TcxCheckListBoxItem     *Item = clbDCType->Items->Items[i];

        if (Item->Checked)
        {
            m_ulDCType |= s_astDCType[i].ulID;
        }
    }

    if (m_ulDCType == 0)
    {
        InfoMsg("δѡ��ɹ���ĵ���̨");
        clbDCType->Focused();
        ModalResult = mrNone;
        return;
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDCUser::Get(UINT32 &ulUserID, AnsiString &strUserName, UINT32 &ulDCType)
{
    ulUserID = m_ulUserID;

    if (m_strUserName.IsEmpty())
    {
        strUserName = m_ulUserID;
    }
    else
    {
        strUserName = m_strUserName;
    }

    ulDCType = m_ulDCType;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDCUser::clbDCTypeClickCheck(TObject *Sender, int AIndex,
          TcxCheckBoxState APrevState, TcxCheckBoxState ANewState)
{
    if (AIndex == 0)
    {
        if (ANewState == cbsChecked)
        {
            for (INT32 i=1; i<clbDCType->Items->Count; i++)
            {
                TcxCheckListBoxItem     *Item = clbDCType->Items->Items[i];

                Item->Checked = false;
            }
        }
    }
    else
    {
        if (ANewState == cbsChecked)
        {
            clbDCType->Items->Items[0]->Checked = false;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDCUser::FormShow(TObject *Sender)
{
    if (m_bAdd)
    {
        edtUserID->SetFocus();
    }
    else
    {
        clbDCType->SetFocus();
    }
}
//---------------------------------------------------------------------------

void __fastcall TDialogSetDCUser::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------

