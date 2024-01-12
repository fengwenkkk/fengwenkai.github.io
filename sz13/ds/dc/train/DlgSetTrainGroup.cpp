//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "DlgSetTrainGroup.h"
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
#pragma link "dxBevel"
#pragma resource "*.dfm"
TDialogSetTrainGroup *DialogSetTrainGroup;

BOOL SetTrainPos(TRAIN_INFO_T *pstTrain, UINT32 ulTrainPos)
{
    DialogSetTrainGroup->Init(pstTrain->acTrainID, ulTrainPos);
    if (DialogSetTrainGroup->ShowModal() != mrOk)
    {
        return FALSE;
    }

    DialogSetTrainGroup->Get(&ulTrainPos);

    GJsonParam   Param;

    Param.Add("TrainUnitID", pstTrain->ulTrainUnitID);
    Param.Add("TrainPos", ulTrainPos);

    if (!SyncSendServer(EV_DC_DIS_APP_REQ, MSG_DCSetTrainGroup, Param))
    {
        InfoMsg("设置列车归属区域失败");
        return FALSE;
    }
    else
    {
        InfoMsg("设置列车归属区域成功");
        return TRUE;
    }
}
//---------------------------------------------------------------------------
__fastcall TDialogSetTrainGroup::TDialogSetTrainGroup(TComponent* Owner)
    : TDialogForm(Owner)
{
    g_DictDepotID.Init(cbTrainPos);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainGroup::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainGroup::Init(AnsiString strTrainID, UINT32 ulTrainPos)
{
    edtTrainID->Text = strTrainID;

    g_DictDepotID.SetID(cbTrainPos, ulTrainPos);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainGroup::Get(UINT32 *pulTrainPos)
{
    *pulTrainPos = m_ulTrainPos;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainGroup::btnOKClick(TObject *Sender)
{
    m_ulTrainPos = 0xffffffff;
    if (!g_DictDepotID.GetID(cbTrainPos, &m_ulTrainPos))
    {
        ErrMsg("请选择列车位置");
        cbTrainPos->Focused();
        ModalResult = mrNone;
        return;
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainGroup::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------

