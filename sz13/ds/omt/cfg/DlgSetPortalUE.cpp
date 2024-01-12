//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"

#include "MsgDialog.h"
#include "msg.h"
#include "ats.h"
#include "ds_public.h"
#include "ClientUtil.h"
#include "Resource.h"
#include "Station.h"

#include "DlgSetPortalUE.h"
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
TDialogSetPortalUE *DialogSetPortalUE;

ID_VALUE_T  s_astPortalUEType[] = { {SMALL_PORTAL_UE,   "小屏手持台"},
                                    {LARGE_PORTAL_UE,   "大屏手持台"}
                                    };

ID_VALUE_T  s_astUserFuncType[] = { {UE_TRAIN,      "列车"},
                                        {UE_STATION,    "车站"},
                                        {UE_FIX,        "维修"},
                                        {UE_MANAGE,     "管理"},
                                        {UE_OTHER,      "其他"},
                                      };

Dict    g_DictPortalUEType(s_astPortalUEType, ARRAY_SIZE(s_astPortalUEType));
Dict    g_DictUserFuncType(s_astUserFuncType, ARRAY_SIZE(s_astUserFuncType));

AnsiString GetPortalUETypeText(UINT32 ulType)
{
    char    *szValue = g_DictPortalUEType.GetValue(ulType);

    if (!szValue)
    {
        return "";
    }

    return szValue;
}

AnsiString GetUserFuncTypeText(UINT32 ulType)
{
    char    *szValue = g_DictUserFuncType.GetValue(ulType);

    if (!szValue)
    {
        return "";
    }

    return szValue;
}

//---------------------------------------------------------------------------
__fastcall TDialogSetPortalUE::TDialogSetPortalUE(TComponent* Owner)
    : TDialogForm(Owner)
{
    g_DictPortalUEType.Init(cbType);
    g_DictUserFuncType.Init(cbFuncType);
    g_DictStation.Init(cbStation);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetPortalUE::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetPortalUE::Init()
{
    ModalResult = mrNone;
    m_bAdd = true;
    edtID->Enabled = true;

    g_DictStation.Init(cbStation);

    edtID->Text = "";
    edtName->Text = "";
    cbType->ItemIndex = 0;
    cbFuncType->ItemIndex = -1;
    cbStation->ItemIndex = -1;

    Caption = "增加手持台";
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetPortalUE::Init(PORTAL_UE_INFO_T &stInfo)
{
    ModalResult = mrNone;
    m_bAdd = false;
    edtID->Enabled = false;

    g_DictStation.Init(cbStation);

    edtID->Text = stInfo.ulUserID;
    edtName->Text = stInfo.acName;
    g_DictPortalUEType.SetID(stInfo.ulType);
    g_DictUserFuncType.SetID(stInfo.ulFuncType);
    g_DictStation.SetID(stInfo.ulStationID);

    Caption = "设置手持台";
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetPortalUE::btnOKClick(TObject *Sender)
{
    ModalResult = mrNone;

    if (!ToInt(edtID->Text, &m_stInfo.ulUserID))
    {
        InfoMsg("终端号码非法");
        edtID->Focused();
        return;
    }

    UINT32  ulMaxLen = sizeof(((PORTAL_UE_INFO_T*)0)->acName) - 1;

    AnsiString  strTmp = edtName->Text.Trim();
    if (strTmp.Length() > ulMaxLen)
    {
        InfoMsg("终端名称超出最大长度：%u", ulMaxLen);
        edtName->Focused();
        return;
    }

    StrCpy(m_stInfo.acName, strTmp);

    if (!g_DictPortalUEType.GetID(&m_stInfo.ulType))
    {
        InfoMsg("终端类型非法");
        cbType->Focused();
        return;
    }

    if (!g_DictUserFuncType.GetID(&m_stInfo.ulFuncType))
    {
        InfoMsg("功能分类非法");
        cbFuncType->Focused();
        return;
    }

    m_stInfo.ulStationID = INVALID_STATION_ID;
    if (m_stInfo.ulFuncType == UE_STATION)
    {
        if (!g_DictStation.GetID(&m_stInfo.ulStationID))
        {
            InfoMsg("所属车站非法");
            cbStation->Focused();
            return;
        }
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetPortalUE::Get(PORTAL_UE_INFO_T &stInfo)
{
    memcpy(&stInfo, &m_stInfo, sizeof(stInfo));
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetPortalUE::FormShow(TObject *Sender)
{
    if (m_bAdd)
    {
        edtID->SetFocus();
    }
    else
    {
        cbStation->SetFocus();
    }
}
//---------------------------------------------------------------------------

void __fastcall TDialogSetPortalUE::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------

