//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "DlgSetTrainDepotID.h"
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
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxImageComboBox"
#pragma resource "*.dfm"
TDialogSetTrainDepotID *DialogSetTrainDepotID;

//---------------------------------------------------------------------------
__fastcall TDialogSetTrainDepotID::TDialogSetTrainDepotID(TComponent* Owner)
    : TDialogForm(Owner)
{
    cbDestDepotID->Clear();
    if (APP::GetDepotID() == DEPOT_ID_MAINLINE)
    {
        cbDestDepotID->Enabled = true;

        cbDestDepotID->Properties->Items->Add("正线");
    }
    else
    {
        cbDestDepotID->Enabled = false;

        VECTOR<DEPOT_INFO_T*>   &vDepotInfo = Depot::GetDepotInfo();

        for (UINT32 i=0; i<vDepotInfo.size(); i++)
        {
            m_DictDepotID.Add(vDepotInfo[i]->ulDepotID, vDepotInfo[i]->acDepotName);
        }

        m_DictDepotID.Init(cbDestDepotID);
    }

    cbDest->Clear();
    cbDest->Properties->Items->Add("");

    Station::InitDictStation(m_DictStation);
    m_DictStation.Init(cbDest, FALSE);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainDepotID::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainDepotID::Init(VECTOR<UINT32> &vTrain)
{
    ModalResult = mrNone;
    m_acDestID[0] = '\0';
    m_ulDestDepotID = DEPOT_ID_MAINLINE;

    TcxTreeListNode     *Node;

    tlMain->BeginUpdate();
    tlMain->Clear();

    for (UINT32 i=0; i<vTrain.size(); i++)
    {
        Node = tlMain->Add();

        Node->Values[0] = i+1;
        Node->Values[1] = vTrain[i];
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainDepotID::Get(UINT32 *pulDestDepotID, CHAR *szDestID)
{
    *pulDestDepotID = m_ulDestDepotID;
    strcpy(szDestID, m_acDestID);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainDepotID::btnOKClick(TObject *Sender)
{
    UINT32      ulStationID = 0;

    m_acDestID[0] = '\0';
    if (m_DictStation.GetID(cbDest, &ulStationID))
    {
        sprintf(m_acDestID, "%02u", ulStationID);
    }

    if (!m_DictDepotID.GetID(&m_ulDestDepotID))
    {
        InfoMsg("请选择目的区域");
        return;
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainDepotID::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------

