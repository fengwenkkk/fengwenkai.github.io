//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"

#include "DlgSetDepot.h"
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
TDialogSetDepot *DialogSetDepot;

//---------------------------------------------------------------------------
__fastcall TDialogSetDepot::TDialogSetDepot(TComponent* Owner)
    : TDialogForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDepot::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDepot::Init()
{
    ModalResult = mrNone;
    m_bAdd = true;
    SetEditable(edtID, true);

    edtID->Text = "";
    edtName->Text = "";

    memset(&m_stInfo, 0, sizeof(m_stInfo));
    m_stInfo.ulTrainDirection = TRAIN_DIRECTION_UNKNOWN;

    Caption = "增加车辆段/停车场";
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDepot::Init(DEPOT_INFO_T *pstInfo)
{
    memcpy(&m_stInfo, pstInfo, sizeof(m_stInfo));
    m_stInfo.ulTrainDirection = TRAIN_DIRECTION_UNKNOWN;

    ModalResult = mrNone;
    m_bAdd = false;
    SetEditable(edtID, false);

    edtID->Text = m_stInfo.ulDepotID;
    edtName->Text = m_stInfo.acDepotName;

    Caption = "设置车辆段/停车场";
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDepot::btnOKClick(TObject *Sender)
{
    ModalResult = mrNone;

    if (!ToInt(edtID->Text, &m_stInfo.ulDepotID))
    {
        InfoMsg("编号非法");
        edtID->Focused();
        return;
    }

    if (m_stInfo.ulDepotID == DEPOT_ID_MAINLINE)
    {
        InfoMsg("编号不能为", DEPOT_ID_MAINLINE);
        edtID->Focused();
        return;
    }

    UINT32  ulMaxLen = sizeof(((DEPOT_INFO_T*)0)->acDepotName) - 1;

    AnsiString  strTmp = edtName->Text.Trim();
    if (strTmp.Length() > (int)ulMaxLen)
    {
        InfoMsg("名称超出最大长度：%u", ulMaxLen);
        edtName->Focused();
        return;
    }

    StrCpy(m_stInfo.acDepotName, strTmp);
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDepot::Get(DEPOT_INFO_T &stInfo)
{
    memcpy(&stInfo, &m_stInfo, sizeof(stInfo));
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDepot::FormShow(TObject *Sender)
{
    if (m_bAdd)
    {
        edtID->SetFocus();
    }
    else
    {
        edtName->SetFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDepot::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------

