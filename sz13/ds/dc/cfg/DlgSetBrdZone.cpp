//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"

#include "MsgDialog.h"
#include "msg.h"
#include "ClientUtil.h"
#include "Resource.h"
#include "Station.h"

#include "DlgSetBrdZone.h"
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
TDialogSetBrdZone *DialogSetBrdZone;

//---------------------------------------------------------------------------
__fastcall TDialogSetBrdZone::TDialogSetBrdZone(TComponent* Owner)
    : TDialogForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetBrdZone::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
bool __fastcall TDialogSetBrdZone::Init()
{
    ModalResult = mrNone;
    m_bAdd = true;

    m_stInfo.i64GroupID = INVALID_GROUP_ID;

    Station::InitDictStation(m_DictStation);   // 非NOCC功能
    m_DictStation.Init(cbStationID);
    cbStationID->ItemIndex = -1;
    edtZoneID->Text = "";
    edtName->Text = "";
    edtGroupID->Text = "";

    SetEditable(cbStationID, true);
    SetEditable(edtZoneID, true);

    Caption = "增加站区";

    return true;
}

void __fastcall TDialogSetBrdZone::Init(BRD_ZONE_INFO_T &stInfo)
{
    ModalResult = mrNone;
    m_bAdd = false;

    memcpy(&m_stInfo, &stInfo, sizeof(m_stInfo));

    Station::InitDictStation(m_DictStation);   // 非NOCC功能
    m_DictStation.Init(cbStationID);
    m_DictStation.SetID(cbStationID, stInfo.ulStationID);
    edtZoneID->Text = stInfo.ulZoneID;
    edtName->Text = stInfo.acZoneName;
    edtGroupID->Text = stInfo.i64GroupID;

    SetEditable(cbStationID, false);
    SetEditable(edtZoneID, false);

    Caption = "设置站区";
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetBrdZone::btnOKClick(TObject *Sender)
{
    ModalResult = mrNone;

    UINT32  ulMaxLen = sizeof(m_stInfo.acZoneName)-1;

    if (!m_DictStation.GetID(cbStationID, &m_stInfo.ulStationID))
    {
        InfoMsg("车站编号非法");
        cbStationID->Focused();
        return;
    }

    if (!ToInt(edtZoneID->Text, &m_stInfo.ulZoneID) ||
        m_stInfo.ulZoneID == 0 ||
        m_stInfo.ulZoneID > 0xff)
    {
        InfoMsg("站区编号非法");
        edtZoneID->Focused();
        return;
    }

    AnsiString  strTmp = edtName->Text.Trim();
    if (strTmp.IsEmpty())
    {
        InfoMsg("请输入站区名称");
        edtName->Focused();
        return;
    }

    if (strTmp.Length() > (int)ulMaxLen)
    {
        InfoMsg("站区名称超出最大长度：%u", ulMaxLen);
        edtName->Focused();
        return;
    }

    StrCpy(m_stInfo.acZoneName, strTmp);

    if (!ToInt64(edtGroupID->Text, &m_stInfo.i64GroupID) ||
        m_stInfo.i64GroupID <= 0 )
    {
        InfoMsg("站区广播组非法");
        edtGroupID->Focused();
        return;
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetBrdZone::Get(BRD_ZONE_INFO_T &stInfo)
{
    memcpy(&stInfo, &m_stInfo, sizeof(stInfo));
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetBrdZone::FormShow(TObject *Sender)
{
    if (m_bAdd)
    {
        cbStationID->SetFocus();
    }
    else
    {
        edtName->SetFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetBrdZone::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------

