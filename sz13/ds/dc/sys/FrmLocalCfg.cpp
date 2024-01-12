//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmLocalCfg.h"
#include "main.h"
#include "FrmPredefinedSMSCfg.h"
#include "FrmSDSTemplate.h"
#include "AudioDev.h"
#include "AudioVolume.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxCustomData"
#pragma link "cxDropDownEdit"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxGroupBox"
#pragma link "cxInplaceContainer"
#pragma link "cxLabel"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxMemo"
#pragma link "cxStyles"
#pragma link "cxTextEdit"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "dxSkinBlue"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "MDIForm"
#pragma link "dxBar"
#pragma link "cxProgressBar"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxCalendar"
#pragma link "cxDateUtils"
#pragma link "cxMaskEdit"
#pragma link "dxCore"
#pragma link "cxPC"
#pragma link "cxSpinEdit"
#pragma link "cxTimeEdit"
#pragma link "dxBarBuiltInMenu"
#pragma link "cxClasses"
#pragma link "cxLocalization"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxScrollBox"
#pragma link "cxCheckBox"
#pragma link "cxRadioGroup"
#pragma resource "*.dfm"
TFormLocalCfg *FormLocalCfg;

static UINT32 GetVolume(Variant vValue)
{
    if (vValue.IsNull())
    {
        return 100;
    }

    int iValue = vValue;

    if (iValue < 0)
    {
        return 0;
    }

    if (iValue > 100)
    {
        return 100;
    }

    return iValue;
}

static INT32 GetVolumeGain(Variant vValue)
{
    if (vValue.IsNull())
    {
        return 0;
    }

    int iValue = vValue;

    if (iValue < -20)
    {
        return -20;
    }

    if (iValue > 20)
    {
        return 20;
    }

    return iValue;
}

void __fastcall TFormLocalCfg::Init(bool bInit)
{
    VECTOR<AnsiString>  vList;
    UINT32              i;
    AnsiString          strMainSpeaker;
    AnsiString          strMainMic;
    AnsiString          strSlaveSpeaker;
    AnsiString          strSlaveMic;
    AnsiString          strMonSpeaker;

    GetAudioDev(strMainSpeaker, strMainMic, strSlaveSpeaker, strSlaveMic, strMonSpeaker);

    NTP_CFG_T       stNTPCfg;

    GetNTPCfg(stNTPCfg);

    edtDISAddr->Text = GetTextIP(LocalCfg::GetInstance().GetDisIP().aucIP);
    edtMDCAddr->Text = GetTextIP(LocalCfg::GetInstance().GetMdcIP().aucIP);
    edtOMCAddr->Text = GetTextIP(LocalCfg::GetInstance().GetOmcIP().aucIP);
    edtATSAddr1->Text = GetATSAddr1();
    edtATSAddr2->Text = GetATSAddr2();
    edtNTPAddr->Text = GetTextIP(stNTPCfg.aucNTPAddr);
    edtNTPSyncPeriod->Text = stNTPCfg.ulNTPSyncPeriod;

    GetSpeakerList(vList);

    cbMainSpeaker->Properties->Items->Clear();
    cbSlaveSpeaker->Properties->Items->Clear();
    cbMonSpeaker->Properties->Items->Clear();

    for (i=0; i<vList.size(); i++)
    {
        cbMainSpeaker->Properties->Items->Add(vList[i]);
        cbSlaveSpeaker->Properties->Items->Add(vList[i]);
        cbMonSpeaker->Properties->Items->Add(vList[i]);
    }

    if (vList.size() == 1)
    {
        SetMainSpeaker(vList[0]);
        SetSlaveSpeaker(vList[0]);
        SetMonSpeaker(vList[0]);
    }

    cbMainSpeaker->ItemIndex = cbMainSpeaker->Properties->Items->IndexOf(strMainSpeaker);
    cbSlaveSpeaker->ItemIndex = cbSlaveSpeaker->Properties->Items->IndexOf(strSlaveSpeaker);
    cbMonSpeaker->ItemIndex = cbMonSpeaker->Properties->Items->IndexOf(strMonSpeaker);

    if (cbMonSpeaker->ItemIndex >= 0)
    {
        SetMonSpeaker(GetMonSpeaker());
    }

    GetMicList(vList);

    cbMainMic->Properties->Items->Clear();
    cbSlaveMic->Properties->Items->Clear();
    for (i=0; i<vList.size(); i++)
    {
        cbMainMic->Properties->Items->Add(vList[i]);
        cbSlaveMic->Properties->Items->Add(vList[i]);
    }

    if (vList.size() == 1)
    {
        SetMainMic(vList[0]);
        SetSlaveMic(vList[0]);
    }

    cbMainMic->ItemIndex = cbMainMic->Properties->Items->IndexOf(strMainMic);
    cbSlaveMic->ItemIndex = cbSlaveMic->Properties->Items->IndexOf(strSlaveMic);

    edtMainSpeakerVolume->Value  = GetMainSpeakerVolume();
    edtMainMicVolume->Value      = GetMainMicVolume();
    edtSlaveSpeakerVolume->Value = GetSlaveSpeakerVolume();
    edtSlaveMicVolume->Value     = GetSlaveMicVolume();
    edtMonSpeakerVolume->Value   = GetMonVolume();
    edtRealBrdVolume->Value      = GetRealBrdVolume();
    edtVoiceAlarmVolume->Value   = GetVoiceAlarmVolume();
    edtIPHMicVolumeRate->Value   = GetIPHMicVolumeRate();

    cbShowCallWin->Checked = IsShowCallWin();
    cbOriginalVideoFormat->Checked = IsOriginalVideoFormat();
    rbIPHVideoTCPMode->Checked = IsIPHVideoOnTcpMode();
    rbIPHVideoUDPMode->Checked = !rbIPHVideoTCPMode->Checked;
    cbIPHRecMergeMode->Checked = IsIPHRecMergeMode();

    if (!cbSlaveMic->Text.IsEmpty())
    {
        SetMicVolume(cbSlaveMic->Text, GetSlaveMicVolume());
    }

    if (!cbMainMic->Text.IsEmpty())
    {
        SetMicVolume(cbMainMic->Text, GetMainMicVolume());
    }

    edtUser->Text = APP::GetDCLoginAccount();
    edtOldPwd->Text = "";
    edtNewPwd->Text = "";
    edtNewPwd2->Text = "";

    edtOldPwd->Style->Font->Name = "Tahoma";
    edtNewPwd->Style->Font->Name = "Tahoma";
    edtNewPwd2->Style->Font->Name = "Tahoma";
}
//---------------------------------------------------------------------------
__fastcall TFormLocalCfg::TFormLocalCfg(TComponent* Owner)
    : TSubForm(Owner)
{
    Init(true);

    pcMain->ActivePageIndex = 0;

    tsSetPwd->TabVisible = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::FormCreate(TObject *Sender)
{
    SetImage(btnRefresh,     IMG_REFRESH);
    SetImage(btnSetBasic,    IMG_SET);
    SetImage(btnCancelBasic, IMG_CANCEL);

    SetImage(btnRefreshSpeaker,IMG_REFRESH);
    SetImage(btnSetAudio,    IMG_SET);
    SetImage(btnCancelAudio, IMG_CANCEL);

    SetImage(btnDefaultParam,IMG_REFRESH);
    SetImage(btnSetParam,    IMG_SET);
    SetImage(btnCancelParam, IMG_CANCEL);

    SetImage(btnSetPwd,    IMG_SET);
    SetImage(btnCancelPwd, IMG_CANCEL);
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::btnSetAudioClick(TObject *Sender)
{
    if (cbMainSpeaker->ItemIndex < 0)
    {
        InfoMsg("请选择主扬声器");
        cbMainSpeaker->SetFocus();
        return;
    }

    if (cbMainMic->ItemIndex < 0)
    {
        InfoMsg("请选择主麦克风");
        cbMainMic->SetFocus();
        return;
    }

    if (cbSlaveSpeaker->ItemIndex < 0)
    {
        InfoMsg("请选择副扬声器");
        cbSlaveSpeaker->SetFocus();
        return;
    }

    if (cbSlaveMic->ItemIndex < 0)
    {
        InfoMsg("请选择副麦克风");
        cbSlaveMic->SetFocus();
        return;
    }

    if (cbMonSpeaker->ItemIndex < 0)
    {
        InfoMsg("请选择监听扬声器");
        cbMonSpeaker->SetFocus();
        return;
    }

    SetMainSpeaker(cbMainSpeaker->Text);
    SetMainMic(cbMainMic->Text);
    SetSlaveSpeaker(cbSlaveSpeaker->Text);
    SetSlaveMic(cbSlaveMic->Text);
    SetMonSpeaker(cbMonSpeaker->Text);

    SetMainSpeakerVolume    (GetVolume(edtMainSpeakerVolume->Value));
    SetMainMicVolume        (GetVolume(edtMainMicVolume->Value));
    SetSlaveSpeakerVolume   (GetVolume(edtSlaveSpeakerVolume->Value));
    SetSlaveMicVolume       (GetVolume(edtSlaveMicVolume->Value));
    SetMonVolume            (GetVolume(edtMonSpeakerVolume->Value));
    SetRealBrdVolume        (GetVolume(edtRealBrdVolume->Value));
    SetVoiceAlarmVolume     (GetVolume(edtVoiceAlarmVolume->Value));
    SetIPHMicVolumeRate     (GetVolumeGain(edtIPHMicVolumeRate->Value));

    SaveAudioDevCfg();

    SetMicVolume(cbMainMic->Text, GetMainMicVolume());
    if (cbMainMic->Text != cbSlaveMic->Text)
    {
        SetMicVolume(cbSlaveMic->Text, GetSlaveMicVolume());
    }

    SetAudioVolume(cbMainSpeaker->Text, GetMainSpeakerVolume());
    if (cbMainSpeaker->Text != cbSlaveSpeaker->Text)
    {
        SetAudioVolume(cbSlaveSpeaker->Text, GetSlaveSpeakerVolume());
    }

    if (cbMainSpeaker->Text != cbMonSpeaker->Text)
    {
        SetAudioVolume(cbMonSpeaker->Text, GetMonVolume());
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::btnRefreshClick(TObject *Sender)
{
    Init();
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::pcMainPageChanging(TObject *Sender, TcxTabSheet *NewPage,
          bool &AllowChange)
{
    if (NewPage == tsSMS)
    {
        FormPredefinedSMSCfg->Parent = NewPage;
        FormPredefinedSMSCfg->BorderStyle = bsNone;
        FormPredefinedSMSCfg->Align = alClient;

        FormPredefinedSMSCfg->gbMain->PanelStyle->Active = true;

        FormPredefinedSMSCfg->Visible = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::gbBasicCfgMainResize(TObject *Sender)
{
    int  iLeft = (gbBasicCfg->Parent->Width - gbBasicCfg->Width)/2;
    int  iTop  = (gbBasicCfg->Parent->Height - gbBasicCfg->Height)/2;

    gbBasicCfg->Left = iLeft<0?0:iLeft;
    gbBasicCfg->Top  = iTop<0?0:iTop;
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::gbAudioCfgMainResize(TObject *Sender)
{
    int  iLeft = (gbAudioCfg->Parent->Width - gbAudioCfg->Width)/2;
    int  iTop  = (gbAudioCfg->Parent->Height - gbAudioCfg->Height)/2;

    gbAudioCfg->Left = iLeft<0?0:iLeft;
    gbAudioCfg->Top  = iTop<0?0:iTop;
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::btnRefreshSpeakerClick(TObject *Sender)
{
    InitAudioDevList();
    Init();
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::btnCancelAudioClick(TObject *Sender)
{
    btnRefreshSpeakerClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::btnDefaultParamClick(TObject *Sender)
{
    cbShowCallWin->Checked = true;
    cbOriginalVideoFormat->Checked = false;
    rbIPHVideoTCPMode->Checked;
    cbIPHRecMergeMode->Checked = true;

    btnSetParamClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::btnSetParamClick(TObject *Sender)
{
    SetShowCallWin(cbShowCallWin->Checked);
    SetOriginalVideoFormat(cbOriginalVideoFormat->Checked);
//    SetIPHVideoOnTcpMode(rbIPHVideoTCPMode->Checked);
//    SetIPHRecMergeMode(cbIPHRecMergeMode->Checked);

    SaveLocalConfInfo();
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::btnCancelParamClick(TObject *Sender)
{
    Init();
}
//---------------------------------------------------------------------------

void __fastcall TFormLocalCfg::btnCancelBasicClick(TObject *Sender)
{
    Init();
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::gbServiceCfgMainResize(TObject *Sender)
{
    int  iLeft = (gbServiceCfg->Parent->Width - gbServiceCfg->Width)/2;
    int  iTop  = (gbServiceCfg->Parent->Height - gbServiceCfg->Height)/2;

    gbServiceCfg->Left = iLeft<0?0:iLeft;
    gbServiceCfg->Top  = iTop<0?0:iTop;
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::btnSetPwdClick(TObject *Sender)
{
    AnsiString  strUser = edtUser->Text.Trim();
    AnsiString  strOldPwd = edtOldPwd->Text.Trim();
    AnsiString  strNewPwd = edtNewPwd->Text.Trim();
    AnsiString  strNewPwd2 = edtNewPwd2->Text.Trim();

    if (strOldPwd.IsEmpty())
    {
        InfoMsg("请输入旧密码");
        edtOldPwd->SetFocus();
        return;
    }

    if (strNewPwd.IsEmpty())
    {
        InfoMsg("请输入新密码");
        edtNewPwd->SetFocus();
        return;
    }

    if (strNewPwd.Length() < 8)
    {
        InfoMsg("密码长度至少为8位");
        edtNewPwd->SetFocus();
        return;
    }

    if (strNewPwd2 != strNewPwd)
    {
        InfoMsg("两次输入的新密码不同");
        edtNewPwd2->SetFocus();
        return;
    }

    GJsonParam  ReqJsonParam;

    ReqJsonParam.Add("User", strUser);
    ReqJsonParam.Add("OldPwd", strOldPwd);
    ReqJsonParam.Add("NewPwd", strNewPwd);

    if (!SyncSendServer(EV_DC_DIS_CFG_REQ, MSG_DCSetPwd, ReqJsonParam))
    {
        ErrMsg("操作失败");
        return;
    }

    InfoMsg("操作成功");
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::gbSetPwdResize(TObject *Sender)
{
    int  iLeft = (gbSetPwd->Parent->Width - gbSetPwd->Width)/2;
    int  iTop  = (gbSetPwd->Parent->Height - gbSetPwd->Height)/2;

    gbSetPwd->Left = iLeft<0?0:iLeft;
    gbSetPwd->Top  = iTop<0?0:iTop;
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalCfg::btnCancelPwdClick(TObject *Sender)
{
    edtNewPwd->Text = "";
    edtNewPwd2->Text = "";
}
//---------------------------------------------------------------------------

