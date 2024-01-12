#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "UICfg.h"
#include "FrmGroupCall.h"
#include "FrmGroup.h"
#include "FrmCallIn.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxGroupBox"
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
#pragma link "AdvSmoothPanel"
#pragma link "cxRadioGroup"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "dxZoomTrackBar"
#pragma link "cxImage"
#pragma link "dxGDIPlusClasses"
#pragma link "cxClasses"
#pragma link "dxSkinsForm"
#pragma link "dxBevel"
#pragma link "cxTextEdit"
#pragma link "cxCheckBox"
#pragma resource "*.dfm"
TFormGroupCall *FormGroupCall;

void __fastcall TFormGroupCall::InitSkin()
{
    btnPTT->SpeedButtonOptions->Transparent = false;
    btnPTT->SpeedButtonOptions->CanBeFocused = false;
    btnPTT->LookAndFeel->SkinName = UICfg::GetPTTSkinName();
}
//---------------------------------------------------------------------------
__fastcall TFormGroupCall::TFormGroupCall(TComponent* Owner)
    : TSubForm(Owner, true)
{
    m_i64GroupID = INVALID_GROUP_ID;
    m_bEmergencyCall = false;

    Init();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCall::Init()
{
    gbMain->Enabled = true;
    btnOK->Enabled = true;
    btnClose->Enabled = false;
    btnPTT->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCall::Init(INT64 i64GroupID, AnsiString strName)
{

}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCall::btnPTTMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    if (m_bEmergencyCall)
    {
        if (!eSDK_PTTEmergency(m_i64GroupID))
        {
            LabelStatus->Caption = "提示：";
            LabelStatusInfo->Caption = "呼叫失败";
            return;
        }
    }
    else if (!PTT::Dial(m_i64GroupID))
    {
        LabelStatus->Caption = "提示：";
        LabelStatusInfo->Caption = "呼叫失败";
        return;
    }

    LabelStatus->Caption = "话权人：";
    btnPTT->LookAndFeel->SkinName = UICfg::GetPTTDownSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCall::btnPTTMouseLeave(TObject *Sender)
{
    btnPTT->LookAndFeel->SkinName = GetDefaultSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCall::btnPTTMouseEnter(TObject *Sender)
{
    btnPTT->LookAndFeel->SkinName = UICfg::GetPTTSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCall::btnPTTMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    PTT::Release();
    btnPTT->LookAndFeel->SkinName = UICfg::GetPTTSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCall::FormClose(TObject *Sender, TCloseAction &Action)
{
    Action = caHide;
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCall::btnOKClick(TObject *Sender)
{
    m_i64GroupID = INVALID_GROUP_ID;
    m_bEmergencyCall = cbEmergencyCall->Checked;

    if (rbGroupID->Checked)
    {
        if (!ToInt64(edtGroupID->Text, &m_i64GroupID) ||
            m_i64GroupID <= 0)
        {
            InfoMsg("组编号非法");
            return;
        }
    }
    else if (rbTrainID->Checked)
    {
        AnsiString      strTrainID = edtTrainID->Text.Trim();
        TRAIN_INFO_T    *pstTrain = Train::GetTrainInfoByTrainID(strTrainID.c_str());

        if (!pstTrain)
        {
            InfoMsg("未知的车次号");
            return;
        }

        // TODO : 根据车次号呼叫是否在车辆段要用车辆段组
        m_i64GroupID = Train::GetTrainGroupID(pstTrain);
    }
    else if (rbTrainUnitID->Checked)
    {
        UINT32  ulTrainUnitID;

        if (!ToInt(edtTrainUnitID->Text, &ulTrainUnitID))
        {
            InfoMsg("车体号非法");
            return;
        }

        TRAIN_INFO_T    *pstTrain = Train::GetTrainInfo(ulTrainUnitID);

        if (!pstTrain)
        {
            InfoMsg("未知的车体号");
            return;
        }

        // TODO : 根据车次号呼叫是否在车辆段要用车辆段组
        m_i64GroupID = Train::GetTrainGroupID(pstTrain);
    }

    if (m_i64GroupID == INVALID_GROUP_ID)
    {
        InfoMsg("列车未注册");
        return;
    }

    btnOK->Enabled = false;
    btnClose->Enabled = true;
    btnPTT->Enabled = true;
//    btnPTT->SpeedButtonOptions->Transparent = false;
    gbMain->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCall::btnCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCall::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnClose, IMG_CLOSE);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCall::UpdateGroupCallStatus(EVENT_GROUP_STATUS_T &stEvent)
{
    if (stEvent.i64GroupID != m_i64GroupID)
    {
        return;
    }

    LabelStatusInfo->Caption = User::GetUserAlias(stEvent.ulSpeaker);
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCall::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    CanClose = true;

    Init();
    if (!btnClose->Enabled)
    {
        return;
    }

    // 强制拆除组呼
    eSDK_GroupBreakoff(m_i64GroupID);

    // 如果组呼包含车载台、固定台，发送通知消息
    FormCallIn->StopGroupCall(m_i64GroupID);

    m_i64GroupID = INVALID_GROUP_ID;
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCall::rbGroupClick(TObject *Sender)
{
    edtGroupID->Enabled = rbGroupID->Checked;
    edtTrainUnitID->Enabled = rbTrainUnitID->Checked;
    edtTrainID->Enabled = rbTrainID->Checked;

    if (edtGroupID->Enabled)
    {
        edtGroupID->SetFocus();
    }

    if (edtTrainUnitID->Enabled)
    {
        edtTrainUnitID->SetFocus();
    }

    if (edtTrainID->Enabled)
    {
        edtTrainID->SetFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormGroupCall::FormShow(TObject *Sender)
{
    btnPTT->Font->Size = UICfg::GetButtonPTTFontSize();
    btnPTT->Font->Name = UICfg::GetButtonPTTFontName();

    rbGroupClick(this);
}
//---------------------------------------------------------------------------

