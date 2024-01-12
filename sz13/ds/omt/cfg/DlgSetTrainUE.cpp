//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"

#include "MsgDialog.h"
#include "msg.h"
#include "ds_public.h"
#include "ClientUtil.h"
#include "Resource.h"
#include "Train.h"

#include "DlgSetTrainUE.h"
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
TDialogSetTrainUE *DialogSetTrainUE;

static Dict  g_DictTrain;

static void InitDictTrain()
{
    VECTOR<TRAIN_INFO_T*>      vInfo;

    GetTrainInfo(vInfo);

    g_DictTrain.Clear();
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        ID_VALUE_T      stIDValue;

        AnsiString      strValue = vInfo[i]->ulTrainUnitID;

        stIDValue.ulID = vInfo[i]->ulTrainUnitID;
        stIDValue.szValue = strValue.c_str();
        g_DictTrain.Add(&stIDValue);
    }
}

//---------------------------------------------------------------------------
__fastcall TDialogSetTrainUE::TDialogSetTrainUE(TComponent* Owner)
    : TDialogForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainUE::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainUE::Init()
{
    ModalResult = mrNone;
    m_bAdd = true;
    edtID->Enabled = true;

    InitDictTrain();
    g_DictTrain.Init(cbTrain);

    edtID->Text = "";
    edtName->Text = "";
    edtIPAddr->Text = "";
    cbTrain->ItemIndex = -1;
    //edtDefaultGroup->Text = "";

    Caption = "增加车载台";
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainUE::Init(TRAIN_UE_INFO_T &stInfo)
{
    ModalResult = mrNone;
    m_bAdd = false;
    edtID->Enabled = false;

    InitDictTrain();
    g_DictTrain.Init(cbTrain);

    edtID->Text = stInfo.ulUserID;
    edtName->Text = stInfo.acName;
    edtIPAddr->Text = stInfo.acLanAddr;
    g_DictTrain.SetID(stInfo.ulTrainUnitID);

    Caption = "设置车载台";
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainUE::btnOKClick(TObject *Sender)
{
    ModalResult = mrNone;

    if (!ToInt(edtID->Text, &m_stInfo.ulUserID))
    {
        InfoMsg("终端号码非法");
        edtID->Focused();
        return;
    }

    UINT32  ulMaxLen = sizeof(((TRAIN_UE_INFO_T*)0)->acName) - 1;

    AnsiString  strTmp = edtName->Text.Trim();
    if (strTmp.Length() > ulMaxLen)
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

    if (!g_DictTrain.GetID(&m_stInfo.ulTrainUnitID))
    {
        InfoMsg("车体号非法");
        cbTrain->Focused();
        return;
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainUE::Get(TRAIN_UE_INFO_T &stInfo)
{
    memcpy(&stInfo, &m_stInfo, sizeof(stInfo));
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainUE::FormShow(TObject *Sender)
{
    if (m_bAdd)
    {
        edtID->SetFocus();
    }
    else
    {
        edtIPAddr->SetFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainUE::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------

