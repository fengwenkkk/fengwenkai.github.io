//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "DlgSetStation.h"
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
TDialogSetStation *DialogSetStation;

static Dict g_DictInterLockStation;

//---------------------------------------------------------------------------
__fastcall TDialogSetStation::TDialogSetStation(TComponent* Owner)
    : TDialogForm(Owner)
{
    g_DictStationType.Init(cbType);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetStation::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetStation::InitInterLockStation()
{
    VECTOR<STATION_INFO_T*>     vInfo;

    Station::GetInterLockStation(vInfo);

    g_DictInterLockStation.Clear();
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        g_DictInterLockStation.Add(vInfo[i]->ulStationID, vInfo[i]->acStationName);
    }

    g_DictInterLockStation.Init(cbInterLockStationID);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetStation::Init()
{
    memset(&m_stInfo, 0, sizeof(m_stInfo));

    ModalResult = mrNone;
    m_bAdd = true;
    edtID->Text = "";
    edtName->Text = "";
    SetEditable(edtID, true);
    cbInterLockStation->Checked = false;

    g_DictStationType.SetID(STATION_TYPE_NORMAL);
    InitInterLockStation();

    cbInterLockStationPropertiesChange(this);
    edtStationGroupID->Text = "";

    Caption = "增加车站";
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetStation::Init(STATION_INFO_T *pstInfo)
{
    ModalResult = mrNone;
    m_bAdd = false;
    edtID->Text = pstInfo->ulStationID;
    edtName->Text = pstInfo->acStationName;
    SetEditable(edtID, false);

    g_DictStationType.SetID(pstInfo->ulStationType);
    InitInterLockStation();

    if (pstInfo->ulStationID == pstInfo->ulInterLockStationID)
    {
        cbInterLockStation->Checked = true;
    }
    else
    {
        cbInterLockStation->Checked = false;
    }

    cbInterLockStationPropertiesChange(this);
    g_DictInterLockStation.SetID(pstInfo->ulInterLockStationID);
    edtStationGroupID->Text = pstInfo->i64StationGroupID;

    Caption = "设置车站";
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetStation::btnOKClick(TObject *Sender)
{
    ModalResult = mrNone;

    if (!ToInt(edtID->Text, &m_stInfo.ulStationID) ||
        m_stInfo.ulStationID == 0 || m_stInfo.ulStationID > 0xffff)
    {
        InfoMsg("车站编号非法");
        edtID->Focused();
        return;
    }

    if (m_bAdd && Station::GetStationInfo(m_stInfo.ulStationID))
    {
        InfoMsg("车站已经存在");
        edtID->Focused();
        return;
    }

    AnsiString  strTmp = edtName->Text.Trim();
    if (strTmp.IsEmpty())
    {
        InfoMsg("请输入车站名称");
        edtName->Focused();
        return;
    }

    if (strTmp.Length() >= (int)sizeof(m_stInfo.acStationName))
    {
        InfoMsg("车站名称超出最大长度：%u", 31);
        edtName->Focused();
        return;
    }

    StrCpy(m_stInfo.acStationName, strTmp);

    if (!g_DictStationType.GetID(&m_stInfo.ulStationType))
    {
        InfoMsg("车站类型非法");
        cbType->Focused();
        return;
    }

    if (cbInterLockStation->Checked)
    {
        m_stInfo.ulInterLockStationID = m_stInfo.ulStationID;
    }
    else
    {
        if (!g_DictInterLockStation.GetID(&m_stInfo.ulInterLockStationID))
        {
            InfoMsg("集中站非法");
            cbType->Focused();
            return;
        }
    }

    if (!ToInt64(edtStationGroupID->Text, &m_stInfo.i64StationGroupID) ||
        m_stInfo.i64StationGroupID <= 0 )
    {
        InfoMsg("车站组非法");
        edtStationGroupID->Focused();
        return;
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetStation::Get(STATION_INFO_T &stInfo)
{
    memcpy(&stInfo, &m_stInfo, sizeof(stInfo));
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetStation::FormShow(TObject *Sender)
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
void __fastcall TDialogSetStation::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetStation::cbInterLockStationPropertiesChange(TObject *Sender)
{
    cbInterLockStationID->ItemIndex = -1;

    if (cbInterLockStation->Checked)
    {
        SetEditable(cbInterLockStationID, false);
    }
    else
    {
        SetEditable(cbInterLockStationID, true);
    }
}
//---------------------------------------------------------------------------

