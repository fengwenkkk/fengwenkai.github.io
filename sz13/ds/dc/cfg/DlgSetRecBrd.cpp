//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"

#include "DlgSetRecBrd.h"
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
TDialogSetRecBrd *DialogSetRecBrd;


extern BOOL IsRecBrdIDExist(UINT32 ulRecID);

//---------------------------------------------------------------------------
__fastcall TDialogSetRecBrd::TDialogSetRecBrd(TComponent* Owner)
    : TDialogForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetRecBrd::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetRecBrd::Init()
{
    memset(&m_stInfo, 0, sizeof(m_stInfo));
    ModalResult = mrNone;
    m_bAdd = true;
    SetEditable(edtRecID, true);

    edtRecID->Text = "";
    mmInfo->Text = "";

    Caption = "ÐÂ½¨Ô¤Â¼Òô¹ã²¥";
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetRecBrd::Init(REC_BRD_CFG_T &stInfo)
{
    ModalResult = mrNone;
    m_bAdd = false;

    memcpy(&m_stInfo, &stInfo, sizeof(stInfo));
    SetEditable(edtRecID, false);
    edtRecID->Text = stInfo.ulRecID;
    mmInfo->Text = stInfo.acRecInfo;

    Caption = "ÉèÖÃÔ¤Â¼Òô¹ã²¥°å";
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetRecBrd::btnOKClick(TObject *Sender)
{
    ModalResult = mrNone;

    AnsiString  strTmp;

    if (m_bAdd)
    {
        strTmp = edtRecID->Text.Trim();
        if (strTmp.IsEmpty())
        {
            InfoMsg("Ô¤Â¼Òô¹ã²¥±àºÅÎª¿Õ!");
            edtRecID->Focused();
            return;
        }

        if (!ToInt(strTmp, &m_stInfo.ulRecID))
        {
            InfoMsg("Ô¤Â¼Òô¹ã²¥±àºÅ·Ç·¨!");
            edtRecID->Focused();
            return;
        }

        if (IsRecBrdIDExist(m_stInfo.ulRecID))
        {
            InfoMsg("Ô¤Â¼Òô¹ã²¥±àºÅÒÑ¾­´æÔÚ!");
            edtRecID->Focused();
            return;
        }
    }

    UINT32  ulMaxLen = sizeof(m_stInfo.acRecInfo) - 1;

    strTmp = mmInfo->Text.Trim();
    if (strTmp.IsEmpty())
    {
        InfoMsg("Ô¤Â¼Òô¹ã²¥ÄÚÈÝÎª¿Õ!");
        mmInfo->Focused();
        return;
    }

    if (strTmp.Length() > (int)ulMaxLen)
    {
        InfoMsg("Ô¤Â¼Òô¹ã²¥ÄÚÈÝ³¬³ö×î´ó³¤¶È£º%u", ulMaxLen);
        mmInfo->Focused();
        return;
    }

    StrCpy(m_stInfo.acRecInfo, strTmp);

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetRecBrd::Get(REC_BRD_CFG_T &stInfo)
{
    memcpy(&stInfo, &m_stInfo, sizeof(m_stInfo));
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetRecBrd::FormShow(TObject *Sender)
{
    if (m_bAdd)
    {
        edtRecID->SetFocus();
    }
    else
    {
        mmInfo->SetFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetRecBrd::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------

