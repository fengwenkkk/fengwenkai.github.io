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
TDialogSetDCUser *DialogSetDCUser;

static UINT32 GetStationID(String strStationName)
{
    VECTOR<STATION_INFO_T*>     vStation;

    Station::GetStationInfo(vStation);

    for (UINT32 i=0; i<vStation.size(); i++)
    {
        if (strStationName == vStation[i]->acStationName)
        {
            return vStation[i]->ulStationID;
        }
    }

    return INVALID_STATION_ID;
}
//---------------------------------------------------------------------------
__fastcall TDialogSetDCUser::TDialogSetDCUser(TComponent* Owner)
    : TDialogForm(Owner)
{
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
    SetEditable(edtUserID, true);

    cbAllStation->Checked = true;

    cbAllStationPropertiesChange(this);

    Caption = "增加调度台";

    VECTOR<STATION_INFO_T*>     vStation;

    Station::GetNormalStation(vStation);

    clbStationList->Clear();
    for (UINT32 i=0; i<vStation.size(); i++)
    {
        TcxCheckListBoxItem     *Item = clbStationList->Items->Add();

        Item->Text = vStation[i]->acStationName;
        Item->Checked = false;
    }
}

static bool HasStation(UINT32 *pulStationList, UINT32 ulStationNum, UINT32 ulStationID)
{
    for (UINT32 i=0; i<ulStationNum; i++)
    {
        if (pulStationList[i] == ulStationID)
        {
            return true;
        }
    }

    return false;
}

void __fastcall TDialogSetDCUser::Init(DC_USER_INFO_T &stInfo)
{
    ModalResult = mrNone;
    m_bAdd = false;
    memcpy(&m_stInfo, &stInfo, sizeof(stInfo));

    edtUserID->Text = stInfo.ulUserID;
    edtUserName->Text = stInfo.acName;
    SetEditable(edtUserID, false);

    cbAllStationPropertiesChange(this);

    Caption = "设置调度台";

    VECTOR<STATION_INFO_T*>     vStation;

    Station::GetNormalStation(vStation);

    clbStationList->Clear();
    for (UINT32 i=0; i<vStation.size(); i++)
    {
        TcxCheckListBoxItem     *Item = clbStationList->Items->Add();

        Item->Text = vStation[i]->acStationName;

        Item->Checked = false;
        if (stInfo.ulStationNum == 0 ||
            HasStation(stInfo.aulStationList, stInfo.ulStationNum, vStation[i]->ulStationID))
        {
            Item->Checked = true;
        }
    }

    if (stInfo.ulStationNum == 0)
    {
        cbAllStation->Checked = true;
    }
    else
    {
        cbAllStation->Checked = false;
    }

    cbAllStationPropertiesChange(this);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDCUser::btnOKClick(TObject *Sender)
{
    if (!ToInt(edtUserID->Text, &m_stInfo.ulUserID))
    {
        InfoMsg("调度台号码非法");
        edtUserID->Focused();
        ModalResult = mrNone;
        return;
    }

    UINT32  ulMaxLen = sizeof(m_stInfo.acName) - 1;

    AnsiString  strUserName = edtUserName->Text.Trim();
    if (strUserName.IsEmpty())
    {
        InfoMsg("调度台名称不能为空");
        edtUserName->Focused();
        ModalResult = mrNone;
        return;
    }
    else if (strUserName.Length() > (int)ulMaxLen)
    {
        InfoMsg("调度台名称超出最大长度：%u", ulMaxLen);
        edtUserName->Focused();
        ModalResult = mrNone;
        return;
    }

    m_stInfo.ulDCType = 0xffff;
    StrCpy(m_stInfo.acName, strUserName);

    UINT32      ulStationNum = 0;

    for (int i=0; i<clbStationList->Items->Count; i++)
    {
        TcxCheckListBoxItem     *Item = clbStationList->Items->Items[i];

        if (Item->Checked)
        {
            m_stInfo.aulStationList[ulStationNum++] = GetStationID(Item->Text);
        }
    }

    m_stInfo.ulStationNum = ulStationNum;

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDCUser::Get(DC_USER_INFO_T &stInfo)
{
    memcpy(&stInfo, &m_stInfo, sizeof(stInfo));
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
        cbAllStation->SetFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDCUser::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetDCUser::cbAllStationPropertiesChange(TObject *Sender)
{
    SetEditable(clbStationList, !cbAllStation->Checked);
}
//---------------------------------------------------------------------------

