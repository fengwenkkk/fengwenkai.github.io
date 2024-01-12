//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"
#include "MsgDialog.h"
#include "msg.h"
#include "ds_public.h"
#include "ClientUtil.h"
#include "Resource.h"
#include "omc.h"
#include "ClientInit.h"
//#include "RemoteMsg.h"

#include "DlgSetOmtOperator.h"
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
TDialogSetOmtOperator *DialogSetOmtOperator;

static ID_VALUE_T s_astOperatorType[] ={
								{OPER_TYPE_ADMIN,   "�����û�"},
								{OPER_TYPE_MANAGE,  "�����û�"},
								{OPER_TYPE_QUERY,   "��ѯ�û�"},
                                };

static Dict g_DictOperatorType(s_astOperatorType, ARRAY_SIZE(s_astOperatorType));

//---------------------------------------------------------------------------
__fastcall TDialogSetOmtOperator::TDialogSetOmtOperator(TComponent* Owner)
    : TDialogForm(Owner)
{
    cbOperatorType->Clear();
    for (UINT32 i=0; i<ARRAY_SIZE(s_astOperatorType); i++)
    {
         cbOperatorType->Properties->Items->AddObject(s_astOperatorType[i].szValue, (TObject*)i+1);
    }
}

//---------------------------------------------------------------------------
void __fastcall TDialogSetOmtOperator::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetOmtOperator::Init()
{
    ModalResult = mrNone;
    m_bAdd = true;
	edtUserName->Text = "";
	edtUserName->Enabled = true;
    edtPwd->Text      = "";
    edtPwd->Properties->PasswordChar = '\0';
	cbOperatorType->ItemIndex = 1;
    Caption = "���Ӳ����û�";

}
//--------------------------------------------------------------------------
void __fastcall TDialogSetOmtOperator::Init(OMT_OPERATOR_INFO_T &stInfo)
{
    ModalResult = mrNone;
    m_bAdd = false;
	edtUserName->Text = stInfo.acName;
	edtUserName->Enabled = false;
	edtPwd->Text = stInfo.acPassword;
	g_DictOperatorType.SetID(cbOperatorType, stInfo.ulOmtType);
	Caption = "���ò����û�";

    if (strcmp(g_stLocalCfg.acUserName, "super") !=0 )
    {
        edtPwd->Properties->PasswordChar = '��';
    }
    else
    {
        edtPwd->Properties->PasswordChar = '\0';;
    }

    bool    bAll = (strcmp(stInfo.acName, "super") == 0);

}
//---------------------------------------------------------------------------
void __fastcall TDialogSetOmtOperator::btnOKClick(TObject *Sender)
{
    AnsiString strTmp = edtUserName->Text.Trim();
    if (strTmp.Length() < 4)
    {
        InfoMsg("�û��������ȹ���\n(�������������ַ�)");
		edtUserName->Focused();
        ModalResult = mrNone;
        return;
    }

    if (strTmp.Length() >= (int)sizeof(m_stInfo.acName))
    {
        InfoMsg("�û��������ȹ���");
        edtUserName->Focused();
        ModalResult = mrNone;
        return;
    }

    StrCpy(m_stInfo.acName, strTmp);

    strTmp = edtPwd->Text.Trim();
    //if (strTmp != m_stInfo.acOperatorID)
    {
        if (strTmp.Length() < 4)
        {
            InfoMsg("�û����벻�ܵ���4λ");
            edtPwd->Focused();
            ModalResult = mrNone;
            return;
        }

        if (strTmp.Length() >= (int)sizeof(m_stInfo.acPassword))
        {
            InfoMsg("�û����볤�ȹ���");
            edtPwd->Focused();
            ModalResult = mrNone;
            return;
        }
    }

    StrCpy(m_stInfo.acPassword, strTmp);

    m_stInfo.ulOmtType = 0;

    m_stInfo.ulOmtType =  cbOperatorType->ItemIndex +1;

    if (strcmp(m_stInfo.acName, "super") == 0)
    {
		m_stInfo.ulOmtType = OPER_TYPE_ADMIN;
    }
    else if (m_stInfo.ulOmtType == 0)
    {
        InfoMsg("��ѡ�������û�����");
        ModalResult = mrNone;
        return;
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetOmtOperator::Get(OMT_OPERATOR_INFO_T &stInfo)
{
    memcpy(&stInfo, &m_stInfo, sizeof(stInfo));
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetOmtOperator::clbOmtTypeClickCheck(TObject *Sender, int AIndex,
          TcxCheckBoxState APrevState, TcxCheckBoxState ANewState)
{
 /* if (AIndex == 0)
    {
        if (ANewState == cbsChecked)
        {
            for (UINT32 i=1; i<clbDCType->Items->Count; i++)
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
    } */
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetOmtOperator::FormShow(TObject *Sender)
{
	if (m_bAdd)
	{
		edtUserName->SetFocus();
	}
	else
	{
		edtPwd->SetFocus();
	}
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetOmtOperator::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------
//void __fastcall TDialogSetOmtOperator::edtUserIDPropertiesChange(TObject *Sender)
//{
//    edtPwd->Text = edtUserName->Text;
//}
//---------------------------------------------------------------------------




