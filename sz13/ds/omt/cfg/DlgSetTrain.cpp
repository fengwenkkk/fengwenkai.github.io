//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"

#include "MsgDialog.h"
#include "msg.h"
#include "ds_public.h"
#include "ClientUtil.h"
#include "Resource.h"

#include "DlgSetTrain.h"
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
TDialogSetTrain *DialogSetTrain;

ID_VALUE_T  s_astTrainType[] = {    {TRAIN_TYPE_NORMAL, "普通车"},
                                    {TRAIN_TYPE_ENGINE, "工程车"},
                                };

Dict    g_DictTrainType(s_astTrainType, ARRAY_SIZE(s_astTrainType));

AnsiString GetTrainTypeText(UINT32 ulTrainType)
{
    return g_DictTrainType.GetValue(ulTrainType);
}

//---------------------------------------------------------------------------
__fastcall TDialogSetTrain::TDialogSetTrain(TComponent* Owner)
    : TDialogForm(Owner)
{
    g_DictTrainType.Init(cbType);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrain::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrain::Init()
{
    ModalResult = mrNone;
    m_bAdd = true;
    edtID->Text = "";
    edtDefaultGroupID->Text = "";
    edtID->Enabled = true;

    g_DictTrainType.SetID(TRAIN_TYPE_NORMAL);

    Caption = "增加列车";
}
void __fastcall TDialogSetTrain::Init(TRAIN_INFO_T &stInfo)
{
    ModalResult = mrNone;
    m_bAdd = false;
    edtID->Text = stInfo.ulTrainUnitID;
    edtDefaultGroupID->Text = stInfo.i64DefaultGroupID;
    edtID->Enabled = false;

    g_DictTrainType.SetID(stInfo.ulTrainType);

    Caption = "设置列车";
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrain::btnOKClick(TObject *Sender)
{
    ModalResult = mrNone;

    if (!ToInt(edtID->Text, &m_stInfo.ulTrainUnitID) ||
        m_stInfo.ulTrainUnitID == 0 || m_stInfo.ulTrainUnitID > 0xffff)
    {
        InfoMsg("车体号非法");
        edtID->Focused();
        return;
    }

    if (!g_DictTrainType.GetID(&m_stInfo.ulTrainType))
    {
        InfoMsg("列车类型非法");
        cbType->Focused();
        return;
    }

    if (!ToInt64(edtDefaultGroupID->Text, &m_stInfo.i64DefaultGroupID) ||
        m_stInfo.i64DefaultGroupID <= 0 )
    {
        InfoMsg("通话组非法");
        edtDefaultGroupID->Focused();
        return;
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrain::Get(TRAIN_INFO_T &stInfo)
{
    memcpy(&stInfo, &m_stInfo, sizeof(m_stInfo));
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrain::FormShow(TObject *Sender)
{
    if (m_bAdd)
    {
        edtID->SetFocus();
    }
    else
    {
        cbType->SetFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrain::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------

