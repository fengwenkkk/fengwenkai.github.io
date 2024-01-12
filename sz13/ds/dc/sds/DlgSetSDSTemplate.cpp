//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "DlgSetSDSTemplate.h"
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
#pragma link "cxRichEdit"
#pragma resource "*.dfm"
TDialogSetSDSTemplate *DialogSetSDSTemplate;

//---------------------------------------------------------------------------
__fastcall TDialogSetSDSTemplate::TDialogSetSDSTemplate(TComponent* Owner)
    : TDialogForm(Owner)
{
    m_DictSDSType = GetDictSDSTemplateType();

    m_DictSDSType->Init(cbType);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetSDSTemplate::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetSDSTemplate::Init()
{
    ModalResult = mrNone;
    m_bAdd = true;

    cbType->ItemIndex = -1;
    edtTitle->Text = "";
    reCmd->Text = "";

    Caption = "增加调度命令模板";
    if (IsSimpleSMSMode())
    {
        Caption = "增加短信模板";
    }
}
void __fastcall TDialogSetSDSTemplate::Init(SDS_TEMPLATE_T &stInfo)
{
    ModalResult = mrNone;
    m_bAdd = false;

    memcpy(&m_stInfo, &stInfo, sizeof(stInfo));
    m_DictSDSType->SetID(stInfo.ulSDSType);
    edtTitle->Text = stInfo.acTitle;
    reCmd->Text = stInfo.acSDSInfo;

    Caption = "设置调度命令模板";
    if (IsSimpleSMSMode())
    {
        Caption = "设置短信模板";
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetSDSTemplate::btnOKClick(TObject *Sender)
{
    ModalResult = mrNone;

    if (!m_DictSDSType->GetID(cbType, &m_stInfo.ulSDSType))
    {
        InfoMsg("模板类型非法");
        cbType->Focused();
        return;
    }

    UINT32      ulMaxLen = sizeof(m_stInfo.acTitle)-1;
    AnsiString  strTmp = edtTitle->Text.Trim();

    if (strTmp.Length() == 0)
    {
        InfoMsg("模板标题不能为空");
        edtTitle->Focused();
        return;
    }

    if (strTmp.Length() > (int)ulMaxLen)
    {
        InfoMsg("模板标题超出最大长度：%u", ulMaxLen);
        edtTitle->Focused();
        return;
    }

    StrCpy(m_stInfo.acTitle, strTmp);

    ulMaxLen = sizeof(m_stInfo.acSDSInfo)-1;
    strTmp = reCmd->Text.Trim();

    if (strTmp.Length() == 0)
    {
        InfoMsg("模板内容不能为空");
        reCmd->Focused();
        return;
    }

    if (strTmp.Length() > (int)ulMaxLen)
    {
        InfoMsg("模板内容超出最大长度：%u", ulMaxLen);
        reCmd->Focused();
        return;
    }

    StrCpy(m_stInfo.acSDSInfo, strTmp);

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetSDSTemplate::Get(SDS_TEMPLATE_T &stInfo)
{
    memcpy(&stInfo, &m_stInfo, sizeof(m_stInfo));
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetSDSTemplate::FormShow(TObject *Sender)
{
    cbType->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetSDSTemplate::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------

