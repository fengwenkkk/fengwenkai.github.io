//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"

#include "MsgDialog.h"
#include "msg.h"
#include "ds_public.h"
#include "ClientUtil.h"
#include "Resource.h"
#include "Station.h"

#include "DlgSetStationUE.h"
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
TDialogSetStationUE *DialogSetStationUE;

//---------------------------------------------------------------------------
__fastcall TDialogSetStationUE::TDialogSetStationUE(TComponent* Owner)
    : TDialogForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetStationUE::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetStationUE::Init()
{
    memset(&m_stInfo, 0, sizeof(m_stInfo));

    ModalResult = mrNone;
    m_bAdd = true;
    SetEditable(edtID, true);
    Station::InitDictStation(m_DictStation);
    m_DictStation.Init(cbStation);

    edtID->Text = "";
    edtName->Text = "";
    edtIPAddr->Text = "";
    cbStation->ItemIndex = -1;
    edtDefaultGroup->Text = "";

    Caption = "增加固定台";
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetStationUE::Init(STATION_UE_INFO_T &stInfo)
{
    ModalResult = mrNone;
    m_bAdd = false;

    memcpy(&m_stInfo, &stInfo, sizeof(stInfo));
    SetEditable(edtID, false);

    Station::InitDictStation(m_DictStation);
    m_DictStation.Init(cbStation);

    edtID->Text = stInfo.ulUserID;
    edtName->Text = stInfo.acName;
    edtIPAddr->Text = stInfo.acLanAddr;
    m_DictStation.SetID(cbStation, stInfo.ulStationID);
    edtDefaultGroup->Text = stInfo.i64DefaultGroupID;

    Caption = "设置固定台";
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetStationUE::btnOKClick(TObject *Sender)
{
    ModalResult = mrNone;

    if (!ToInt(edtID->Text, &m_stInfo.ulUserID))
    {
        InfoMsg("终端号码非法");
        edtID->Focused();
        return;
    }

    if (m_bAdd && StationUE::GetStationUE(m_stInfo.ulUserID))
    {
        InfoMsg("终端号码已经存在");
        edtID->Focused();
        return;
    }

    UINT32  ulMaxLen = sizeof(((STATION_UE_INFO_T*)0)->acName) - 1;

    AnsiString  strTmp = edtName->Text.Trim();
    if (strTmp.Length() > (int)ulMaxLen)
    {
        InfoMsg("终端名称超出最大长度：%u", ulMaxLen);
        edtName->Focused();
        return;
    }

    StrCpy(m_stInfo.acName, strTmp);

    UINT8   aucIPAddr[4];

    strTmp = edtIPAddr->Text.Trim();
    if (!GetIP(strTmp, aucIPAddr))
    {
        InfoMsg("主机地址非法");
        edtIPAddr->Focused();
        return;
    }

    StrCpy(m_stInfo.acLanAddr, strTmp);

    if (!m_DictStation.GetID(cbStation, &m_stInfo.ulStationID))
    {
        InfoMsg("所属车站非法");
        cbStation->Focused();
        return;
    }

    strTmp = edtDefaultGroup->Text.Trim();
    if (!gos_atoi64(strTmp.c_str(), &m_stInfo.i64DefaultGroupID))
    {
        InfoMsg("通话组非法");
        edtDefaultGroup->Focused();
        return;
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetStationUE::Get(STATION_UE_INFO_T &stInfo)
{
    memcpy(&stInfo, &m_stInfo, sizeof(stInfo));
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetStationUE::FormShow(TObject *Sender)
{
    if (m_bAdd)
    {
        edtID->SetFocus();
    }
    else
    {
        edtDefaultGroup->SetFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetStationUE::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------

