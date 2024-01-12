//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"

#include "MsgDialog.h"
#include "msg.h"
#include "ds_public.h"
#include "Group.h"
#include "ClientUtil.h"
#include "Resource.h"

#include "DlgSetGroupInfo.h"
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
TDialogSetGroupInfo *DialogSetGroupInfo;

static ID_VALUE_T   s_astGroupFunc[] = {
                                {GROUP_FUNC_TRAIN,      "列车组"},
                                {GROUP_FUNC_STATION,    "车站组"},
                                {GROUP_FUNC_FIX,        "维护组"},
                                {GROUP_FUNC_MULTIFUNC,  "多职能组"},
                                {GROUP_FUNC_OTHER,      "其他"},
                                };

static Dict g_DictGroupFunc(s_astGroupFunc, ARRAY_SIZE(s_astGroupFunc));

//---------------------------------------------------------------------------
__fastcall TDialogSetGroupInfo::TDialogSetGroupInfo(TComponent* Owner)
    : TDialogForm(Owner)
{
    g_DictGroupFunc.Init(cbFuncType);
    g_DictGroupFunc.SetID(cbFuncType, GROUP_FUNC_OTHER);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetGroupInfo::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetGroupInfo::Init(INT64 i64GroupID, CHAR *szGroupName, UINT32 ulFuncType)
{
    ModalResult = mrNone;
    edtGroupID->Text = i64GroupID;
    edtOriGroupName->Text = Group::GetGroupName(i64GroupID);
    edtGroupName->Text = szGroupName;
    g_DictGroupFunc.SetID(cbFuncType, ulFuncType);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetGroupInfo::btnOKClick(TObject *Sender)
{
    UINT32  ulMaxLen = sizeof(((GROUP_INFO_T*)0)->acGroupName) - 1;

    m_strGroupName = edtGroupName->Text.Trim();
    if (m_strGroupName.Length() > (int)ulMaxLen)
    {
        InfoMsg("群组别名超出最大长度：%u", ulMaxLen);
        edtGroupName->Focused();
        ModalResult = mrNone;
        return;
    }

    if (!g_DictGroupFunc.GetID(cbFuncType, &m_ulFuncType))
    {
        InfoMsg("未选择群组分类");
        cbFuncType->Focused();
        ModalResult = mrNone;
        return;
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetGroupInfo::Get(CHAR *szGroupName, UINT32 &ulFuncType)
{
    ulFuncType = m_ulFuncType;
    StrCpy(szGroupName, m_strGroupName);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetGroupInfo::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------

