//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MsgDialog.h"
#include "Resource.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxGraphics"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxEdit"
#pragma link "cxImage"
#pragma link "dxGDIPlusClasses"
#pragma link "cxLabel"
#pragma link "cxGroupBox"
#pragma link "dxSkinsForm"
#pragma link "cxClasses"

#pragma resource "*.dfm"

TFormMsgDialog *FormMsgDialog = NULL;

extern INT32               g_ColorFont;

//---------------------------------------------------------------------------
bool InitMsgDialog(MSG_TYPE_E enMsgType, AnsiString strMsg, UINT32 ulTimeout=30)
{
    if (!FormMsgDialog)
    {
        FormMsgDialog = new TFormMsgDialog(Application);
    }

    if (FormMsgDialog->Visible)
    {
        delete FormMsgDialog;

        FormMsgDialog = new TFormMsgDialog(Application);
    }

    FormMsgDialog->SetMsg(enMsgType, strMsg, ulTimeout);

    if (FormMsgDialog->Visible)
    {
        FormMsgDialog->Visible = false;
    }

    bool bRet = (mrOk == FormMsgDialog->ShowModal());

    return bRet;
}

//---------------------------------------------------------------------------
void InfoMsg(const char* szFormat, ...)
{
    if (NULL == szFormat)
        return;

    static CHAR    acMsg[2000];
    va_list vaMsg;

    va_start(vaMsg, szFormat);
    vsprintf(acMsg, szFormat, vaMsg);
    va_end(vaMsg);

    InfoMsg(AnsiString(acMsg));
}
//---------------------------------------------------------------------------
void InfoMsg(AnsiString strMsg)
{
    InitMsgDialog(INFO_MSG, strMsg, 30);
}
//---------------------------------------------------------------------------
void WarnMsg(const char* szFormat, ...)
{
    if (NULL == szFormat)
        return;

    static CHAR    acMsg[2000];
    va_list vaMsg;

    va_start(vaMsg, szFormat);
    vsprintf(acMsg, szFormat, vaMsg);
    va_end(vaMsg);

    WarnMsg(AnsiString(acMsg));
}
//---------------------------------------------------------------------------
void WarnMsg(AnsiString strMsg)
{
    InitMsgDialog(WARN_MSG, strMsg, 30);
}
//---------------------------------------------------------------------------
void ErrMsg(const char* szFormat, ...)
{
    if (NULL == szFormat)
        return;

    static CHAR    acMsg[2000];
    va_list vaMsg;

    va_start(vaMsg, szFormat);
    vsprintf(acMsg, szFormat, vaMsg);
    va_end(vaMsg);

    ErrMsg(AnsiString(acMsg));
}
//---------------------------------------------------------------------------
void ErrMsg(AnsiString strMsg)
{
    InitMsgDialog(ERROR_MSG, strMsg, 30);
}
//---------------------------------------------------------------------------
bool AskMsg(const char* szFormat, ...)
{
    if (NULL == szFormat)
        return false;

    static CHAR    acMsg[2000];
    va_list vaMsg;

    va_start(vaMsg, szFormat);
    vsprintf(acMsg, szFormat, vaMsg);
    va_end(vaMsg);

    return AskMsg(AnsiString(acMsg));
}
//---------------------------------------------------------------------------
bool AskMsg(AnsiString strMsg)
{
    return AskMsg(30, strMsg);
}
//---------------------------------------------------------------------------
bool AskMsg(UINT32 ulWaitTime, AnsiString strMsg)
{
    return InitMsgDialog(ASK_MSG, strMsg, ulWaitTime);
}
//---------------------------------------------------------------------------
__fastcall TFormMsgDialog::TFormMsgDialog(TComponent* Owner)
    : TSubForm(Owner)
{
    m_iHintTimer = 15*1000;
}

//---------------------------------------------------------------------------
void __fastcall TFormMsgDialog::FormCreate(TObject *Sender)
{
    int iLeft = 12;
    int iTop = 12;

    ImageInfo->Left = iLeft;
    ImageWarn->Left = iLeft;
    ImageError->Left = iLeft;
    ImageAsk->Left = iLeft;

    ImageInfo->Top = iTop;
    ImageWarn->Top = iTop;
    ImageError->Top = iTop;
    ImageAsk->Top = iTop;

    LabelMsg->Caption = "";

    SetImage(btnOK, IMG_OK);
    SetImage(btnYes, IMG_OK);
    SetImage(btnNo, IMG_CANCEL);
}
//---------------------------------------------------------------------------
void __fastcall TFormMsgDialog::SetMsg(MSG_TYPE_E enType, AnsiString strMsg, DWORD dwWaitTime)
{
    ImageInfo->Visible = false;
    ImageWarn->Visible = false;
    ImageError->Visible = false;
    ImageAsk->Visible = false;

    btnOK->Visible = true;
    btnYes->Visible = false;
    btnNo->Visible = false;

    TimerExit->Enabled = false;
    TimerExit->Interval = m_iHintTimer;

    switch(enType)
    {
        case WARN_MSG:
            Caption = "警告";
            ImageWarn->Visible = true;
            break;

        case ERROR_MSG:
            Caption = "错误";
            ImageError->Visible = true;
            break;

        case ASK_MSG:
            Caption = "选择";
            ImageAsk->Visible = true;
            btnOK->Visible = false;
            btnYes->Visible = true;
            btnNo->Visible = true;
            TimerExit->Interval = dwWaitTime * 1000;
            break;

        case INFO_MSG:
        default:
            Caption = "提示";
            ImageInfo->Visible = true;
            break;
    }

    TimerExit->Enabled = true;
    LabelMsg->Caption = strMsg;

    LabelMsg->Hint = strMsg;
}
//---------------------------------------------------------------------------
void __fastcall TFormMsgDialog::gbYesClick(TObject *Sender)
{
    ModalResult = mrOk;
    Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMsgDialog::TimerExitTimer(TObject *Sender)
{
    btnNoClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TFormMsgDialog::btnYesClick(TObject *Sender)
{
    ModalResult = mrOk;
    Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMsgDialog::btnOKClick(TObject *Sender)
{
    ModalResult = mrOk;
    Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMsgDialog::btnNoClick(TObject *Sender)
{
    ModalResult = mrCancel;
    Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMsgDialog::FormActivate(TObject *Sender)
{
    ::SetWindowPos(Handle,HWND_TOPMOST,Left,Top,Width,Height,SWP_NOMOVE);
}
//---------------------------------------------------------------------------

