//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"

#include "DlgSelectVideoChan.h"
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
TDialogSelectVideoChan *DialogSelectVideoChan;

//---------------------------------------------------------------------------
__fastcall TDialogSelectVideoChan::TDialogSelectVideoChan(TComponent* Owner)
    : TDialogForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TDialogSelectVideoChan::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSelectVideoChan::Init(UINT32 ulUserID, VECTOR<ESDK_VWALL_INFO_T> &vInfo)
{
    ModalResult = mrNone;
    SetEditable(edtUserID, true);

    edtUserID->Text = ulUserID;
    cbVideoChanID->Properties->Items->Clear();

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        if (vInfo[i].ulState == VWALL_STATUS_INIT)
        {
            cbVideoChanID->Properties->Items->Add(vInfo[i].ulVideoChanID);
        }
    }

    cbVideoChanID->ItemIndex = -1;

    btnOK->Enabled = (cbVideoChanID->Properties->Items->Count > 0);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSelectVideoChan::btnOKClick(TObject *Sender)
{
    ModalResult = mrNone;

    if (!ToInt(cbVideoChanID->Text, &m_ulVideoChanID))
    {
        InfoMsg("视频窗口编号非法");
        cbVideoChanID->Focused();
        return;
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSelectVideoChan::Get(UINT32 &ulVideoChanID)
{
    m_ulVideoChanID = ulVideoChanID;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSelectVideoChan::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------


