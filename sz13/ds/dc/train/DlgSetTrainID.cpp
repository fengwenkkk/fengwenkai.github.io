//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "DlgSetTrainID.h"
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
#pragma link "cxRadioGroup"
#pragma resource "*.dfm"
TDialogSetTrainID *DialogSetTrainID;

BOOL SetTrainPos(TRAIN_INFO_T *pstTrain, SET_TRAIN_POS_T *pstPos)
{
    UINT32          ulDepotID;
    CHAR            acTrainID[16] = {0};
    UINT32          ulDirection;
    CHAR            acDestID[16] = {0};
    UINT32          ulStationID;
    UINT32          ulDriverID;

    if (!pstTrain)
    {
        InfoMsg("δ֪�г�");
        return FALSE;
    }

    DialogSetTrainID->Init(pstTrain, pstPos);
    if (DialogSetTrainID->ShowModal() != mrOk)
    {
        return FALSE;
    }

    SET_TRAIN_POS_T     stPos = {0};

    DialogSetTrainID->Get(&stPos);

    GJsonParam   Param;

    Param.Add("TrainUnitID", pstTrain->ulTrainUnitID);
    Param.Add("DepotID", stPos.ulDepotID);
    Param.Add("TrainID", stPos.acTrainID);
    Param.Add("Direction", stPos.ulDirection);
    Param.Add("DestID", stPos.acDestID);
    Param.Add("StationID", stPos.ulStationID);
    Param.Add("DriverID", stPos.ulDriverID);

    if (!SyncSendServer(EV_DC_DIS_APP_REQ, MSG_DCSetTrainPos, Param))
    {
        InfoMsg("�����г�������Ϣʧ��");
        return FALSE;
    }
    else
    {
        InfoMsg("�����г�������Ϣ�ɹ�");
        return TRUE;
    }
}

//---------------------------------------------------------------------------
__fastcall TDialogSetTrainID::TDialogSetTrainID(TComponent* Owner)
    : TDialogForm(Owner)
{
    g_DictDepotID.Init(cbDepotID);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainID::btnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainID::Init(TRAIN_INFO_T *pstTrain, SET_TRAIN_POS_T *pstPos)
{
    Station::InitDictStation(m_DictStation);
    DestInfo::InitDictDest(/*pstTrain->ulStationID, */m_DictDest);

    m_DictDest.Init(cbDestID);
    m_DictStation.Init(cbStationID);

    m_ulDepotID= 0xffffffff;
    m_acTrainID[0] = '\0';
    m_acDestID[0] = '\0';
    m_ulDirection = TRAIN_DIRECTION_UPSIDE;
    m_ulStationID = INVALID_STATION_ID;
    m_ulDriverID = INVALID_USER_ID;

    UINT32          ulDestID = INVALID_STATION_ID;

    edtTrainUnitID->Text = pstTrain->ulTrainUnitID;
    g_DictDepotID.SetID(cbDepotID, Station::GetDepotID(pstTrain));

    cbTrainPosPropertiesChange(this);
    if (Station::GetDepotID(pstTrain) == DEPOT_ID_MAINLINE)
    {
        gos_atou(pstTrain->acDestID, &ulDestID);
    }

    edtTrainID->Text = pstTrain->acTrainID;
    if (pstTrain->ulDirection == TRAIN_DIRECTION_UPSIDE)
    {
        rbUp->Checked = true;
        rbDown->Checked = false;
    }
    else
    {
        rbUp->Checked = false;
        rbDown->Checked = true;
    }

    edtDestID->Text = pstTrain->acDestID;
    m_DictDest.SetID(cbDestID, ulDestID);
    m_DictStation.SetID(cbStationID, pstTrain->ulStationID);
    if (pstTrain->ulDriverID == INVALID_USER_ID)
    {
        edtDriverID->Text = "";
    }
    else
    {
        edtDriverID->Text = pstTrain->ulDriverID;
    }

    if (pstPos)
    {
        ulDestID = INVALID_STATION_ID;

        edtTrainUnitID->Text = pstTrain->ulTrainUnitID;
        g_DictDepotID.SetID(cbDepotID, pstPos->ulDepotID);

        cbTrainPosPropertiesChange(this);
        if (pstPos->ulDepotID == DEPOT_ID_MAINLINE)
        {
            gos_atou(pstPos->acDestID, &ulDestID);
        }

        edtTrainID->Text = pstPos->acTrainID;
        if (pstPos->ulDirection == TRAIN_DIRECTION_UPSIDE)
        {
            rbUp->Checked = true;
            rbDown->Checked = false;
        }
        else
        {
            rbUp->Checked = false;
            rbDown->Checked = true;
        }

        edtDestID->Text = pstPos->acDestID;
        m_DictStation.SetID(cbDestID, ulDestID);
        m_DictStation.SetID(cbStationID, pstPos->ulStationID);

        if (pstPos->ulDriverID == INVALID_USER_ID)
        {
            edtDriverID->Text = "";
        }
        else
        {
            edtDriverID->Text = pstPos->ulDriverID;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainID::btnOKClick(TObject *Sender)
{
    m_ulDepotID = 0xffffffff;
    if (!g_DictDepotID.GetID(cbDepotID, &m_ulDepotID))
    {
        ErrMsg("��ѡ��ע��λ��");
        cbDepotID->Focused();
        ModalResult = mrNone;
        return;
    }

    AnsiString strValue = edtTrainID->Text.Trim();

    if (strValue.IsEmpty())
    {
        ErrMsg("���κŲ���Ϊ��");
        edtTrainID->Focused();
        ModalResult = mrNone;
        return;
    }

    if (strValue.Length() >= (int)sizeof(m_acTrainID))
    {
        ErrMsg("���κų��ȳ�����Χ");
        edtTrainID->Focused();
        ModalResult = mrNone;
        return;
    }

    StrCpy(m_acTrainID, strValue);

    if (!m_DictStation.GetID(cbStationID, &m_ulStationID))
    {
        ErrMsg("��ѡ��ǰλ��");
        cbStationID->Focused();
        ModalResult = mrNone;
        return;
    }

    if (cbDestID->Visible)
    {
        UINT32  ulDestID;

        if (!m_DictDest.GetID(cbDestID, &ulDestID))
        {
            ErrMsg("��ѡ��Ŀ�ĵ�");
            cbDestID->Focused();
            ModalResult = mrNone;
            return;
        }

        sprintf(m_acDestID, "%u", ulDestID);
    }
    else
    {
        strValue = edtDestID->Text.Trim();
        if (strValue.Length() >= (int)sizeof(m_acDestID))
        {
            ErrMsg("Ŀ�ĵس��ȳ�����Χ");
            edtDestID->Focused();
            ModalResult = mrNone;
            return;
        }

        if (!DestInfo::IsValidDestID(m_ulDepotID, strValue))
        {
            ErrMsg("Ŀ�ĵطǷ�");
            edtDestID->Focused();
            ModalResult = mrNone;
            return;
        }

        StrCpy(m_acDestID, strValue);
    }

    if (rbUp->Checked)
    {
        m_ulDirection = TRAIN_DIRECTION_UPSIDE;
    }
    else
    {
        m_ulDirection = TRAIN_DIRECTION_DOWNSIDE;
    }

    strValue = edtDriverID->Text.Trim();
    if (strValue.IsEmpty())
    {
        m_ulDriverID = INVALID_USER_ID;
    }
    else
    {
        if (!ToInt(strValue, &m_ulDriverID))
        {
            ErrMsg("˾������Ƿ�");
            edtDriverID->Focused();
            ModalResult = mrNone;
            return;
        }
    }

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainID::Get(SET_TRAIN_POS_T *pstPos)
{
    pstPos->ulDepotID = m_ulDepotID;
    strcpy(pstPos->acTrainID, m_acTrainID);
    pstPos->ulDirection = m_ulDirection;
    strcpy(pstPos->acDestID, m_acDestID);
    pstPos->ulStationID = m_ulStationID;
    pstPos->ulDriverID = m_ulDriverID;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainID::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetTrainID::cbTrainPosPropertiesChange(TObject *Sender)
{
    if (!g_DictDepotID.GetID(cbDepotID, &m_ulDepotID))
    {
        return;
    }

  //  if (m_ulDepotID != DEPOT_ID_MAINLINE)
    {
        cbDestID->Visible = true;
        edtDestID->Visible = false;
    }
    /*else
    {
        cbDestID->Visible = false;
        edtDestID->Visible = true;
    } */
}
//---------------------------------------------------------------------------

