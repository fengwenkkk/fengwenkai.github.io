//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"

#include "MsgDialog.h"
#include "msg.h"
#include "ds_public.h"
#include "ClientUtil.h"
#include "Resource.h"

#include "DlgSetNe.h"
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
#pragma resource "*.dfm"
TDialogSetNe *DialogSetNe;


extern Dict     g_DictDevType;
extern Dict     g_DictAlarmLevel;

//---------------------------------------------------------------------------
__fastcall TDialogSetNe::TDialogSetNe(TComponent* Owner)
    : TDialogForm(Owner)
{
    g_DictDevType.Init(cbDevType);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetNe::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetNe::Init()
{
    m_ulDevType = OMC_DEV_TYPE_TX;
    m_strDevName = "";
    m_strMAC = "";

    ModalResult = mrNone;
    m_bAdd = true;
    edtDevName->Text = "";
    edtMAC->Text = "";
    cbDevType->Enabled = true;
    edtMAC->Enabled = true;

    g_DictDevType.SetID(cbDevType, OMC_DEV_TYPE_TX);

    Caption = "增加设备";
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetNe::Init(UINT32 ulDevType, AnsiString strDevName, AnsiString strMAC)
{
    m_ulDevType = ulDevType;
    m_strDevName = strDevName;
    m_strMAC = strMAC;

    ModalResult = mrNone;
    m_bAdd = false;

    edtDevName->Text = m_strDevName;
    g_DictDevType.SetID(cbDevType, m_ulDevType);
    cbDevType->Enabled = false;

    edtMAC->Text = m_strMAC;
    edtMAC->Enabled = false;
    Caption = "修改设备信息";
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetNe::btnOKClick(TObject *Sender)
{
    ModalResult = mrNone;

    AnsiString  strTmp = edtDevName->Text.Trim();

    if (strTmp.Length() == 0 )
    {
        InfoMsg("设备名称不能为空");
        edtDevName->Focused();
        return;
    }

    if (strTmp.Length() >= 32 )
    {
        InfoMsg("设备名称长度最大%u", 32-1);
        edtDevName->Focused();
        return;
    }

    m_strDevName = strTmp;

    if (!g_DictDevType.GetID(cbDevType, &m_ulDevType))
    {
        InfoMsg("设备类型非法");
        cbDevType->Focused();
        return;
    }

    strTmp = edtMAC->Text.Trim();
    UINT8   aucMAC[6];

    if (!gos_get_mac(strTmp.c_str(), aucMAC))
    {
        InfoMsg("MAC地址非法");
        edtMAC->Focused();
        return;
    }

    m_strMAC = GetTextMAC(aucMAC);

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetNe::Get(UINT32 &ulDevType, AnsiString &strDevName,
                                    AnsiString &strMAC)
{
    ulDevType = m_ulDevType;
    strDevName = m_strDevName;
    strMAC = m_strMAC;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetNe::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------

