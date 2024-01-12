#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "main.h"
#include "FrmStation.h"
#include "FrmGroup.h"
#include "FrmUser.h"
#include "DlgRealBrd.h"
#include "DlgRecBrd.h"
#include "FrmGroupCallWin.h"
#include "FrmLatestSDS.h"
#include "DlgSetTrainID.h"
#include "DlgSetTrainGroup.h"

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
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
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
#pragma link "cxImageComboBox"
#pragma link "cxPC"
#pragma link "dxSkinscxPCPainter"
#pragma link "cxProgressBar"
#pragma link "cxClasses"
#pragma link "cxImageList"
#pragma link "dxBarBuiltInMenu"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxTrackBar"
#pragma link "dxCameraControl"
#pragma link "AdvSmoothToggleButton"
#pragma link "cxCheckBox"
#pragma link "dxToggleSwitch"
#pragma link "cxLabel"
#pragma link "AdvSmoothPanel"
#pragma link "cxScrollBox"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma resource "*.dfm"
TFormStation *FormStation;

void __fastcall TFormStation::InitWorkPanel()
{
    TcxButton   *btns[] = { btnGroupCall,
                            btnRealBrd,
                            btnRecBrd,
                            btnPTT
                          };

    gbWorkPanel->Width = 150;

    for (UINT32 i=0; i<ARRAY_SIZE(btns); i++)
    {
        btns[i]->TabOrder = i;
        btns[i]->Top = 240+95*i-95*2;
        btns[i]->Width = g_iMidToolbarButtonWidth;
        btns[i]->Height = g_iMidToolbarButtonHeight;
        btns[i]->Left = (btns[i]->Parent->Width - btns[i]->Width)/2;
    }

    btnPTT->Width = 120;
    btnPTT->Height = 90;
    btnPTT->Top = btnPTT->Parent->Height - btnPTT->Height - 60;
    btnPTT->Left = (btnPTT->Parent->Width - btnPTT->Width)/2;
}

void __fastcall TFormStation::InitSkin()
{
}
//---------------------------------------------------------------------------
__fastcall TFormStation::TFormStation(TComponent* Owner)
    : TSubForm(Owner)
{
    InitStation(tlMain);
    InitWorkPanel();

    if (IsSimpleSMSMode())
    {
        pmiStationSDS->Caption = "短信";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormStation::InitStation(TcxTreeList *tl)
{
    TcxTreeListNode         *Node;
    VECTOR<STATION_INFO_T*> vStation;
    STATION_UE_INFO_T       *pstUE;

    Station::GetNormalStation(vStation);

    tl->BeginUpdate();
    tl->Clear();

    for (UINT32 i=0; i<vStation.size(); i++)
    {
        pstUE = StationUE::GetStationUEByStationID(vStation[i]->ulStationID);

        Node = tl->Add();
        Node->Values[0] = vStation[i]->ulStationID;
        Node->Values[1] = vStation[i]->acStationName;
        Node->Values[2] = vStation[i]->ulInterLockStationID;
        Node->Values[3] = "";
        Node->Values[4] = UE_STATUS_ONLINE;
        Node->Values[5] = "";
        Node->Values[6] = "";

        if (pstUE)
        {
            Node->Values[3] = pstUE->i64DefaultGroupID;
            Node->Values[5] = pstUE->ulUserID;
        }
    }

    tl->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormStation::FormCreate(TObject *Sender)
{
    SetImage(pmiStationP2PCall, IMG_P2P_CALL);
    SetImage(pmiStationSDS, IMG_SDS);
    SetImage(pmiCallStation, IMG_PTT_DIAL);
    SetImage(pmiInterLockStationPTTDial, IMG_PTT_DIAL);
    SetImage(pmiInterLockStationBrd, IMG_BRD);
}
//---------------------------------------------------------------------------
UINT32 __fastcall TFormStation::GetSelectStation()
{
    if (tlMain->SelectionCount == 0)
    {
        return INVALID_STATION_ID;
    }

    TcxTreeListNode     *Node = tlMain->Selections[0];

    if (Node->Values[0].IsNull())
    {
        return INVALID_STATION_ID;
    }

    return (UINT32)Node->Values[0];
}
//---------------------------------------------------------------------------
void __fastcall TFormStation::btnRealBrdClick(TObject *Sender)
{
    UINT32      ulStationID = GetSelectStation();

    StationRealBrd(ulStationID);
}
//---------------------------------------------------------------------------
void __fastcall TFormStation::btnRecBrdClick(TObject *Sender)
{
    UINT32  ulStationID = GetSelectStation();

    StationRecBrd(ulStationID);
}
//---------------------------------------------------------------------------
void __fastcall TFormStation::ColumnInterLockStationIDGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulValue = (UINT32)ANode->Values[2];

    Value = Station::GetStationName(ulValue);
}
//---------------------------------------------------------------------------
void __fastcall TFormStation::btnGroupCallClick(TObject *Sender)
{
    UINT32              ulStationID = GetSelectStation();
    STATION_UE_INFO_T   *pstUE = StationUE::GetStationUEByStationID(ulStationID);

    if (!pstUE)
    {
        return;
    }

    if (!Group::SubJoinGroup(pstUE->i64DefaultGroupID))
    {
        InfoMsg("加入组失败!");
        return;
    }

    Group::UpdateGroupSubStatus(pstUE->i64DefaultGroupID, GROUP_STATUS_JOIN, FALSE);

    if (IsShowCallWin())
    {
        ShowGroupCallWin(pstUE->i64DefaultGroupID);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormStation::btnP2PCallClick(TObject *Sender)
{
    UINT32              ulStationID = GetSelectStation();
    STATION_UE_INFO_T   *pstUE = StationUE::GetStationUEByStationID(ulStationID);

    if (!pstUE)
    {
        return;
    }

    FormUser->Dial(pstUE->ulUserID);
}
//---------------------------------------------------------------------------
void __fastcall TFormStation::btnEmergencyCallClick(TObject *Sender)
{
/*
    UINT32              ulStationID = GetSelectStation();
    STATION_UE_INFO_T   *pstUE = GetStationUEByStationID(ulStationID);

    if (!pstUE)
    {
        return;
    }

    if (!SubJoinGroup(pstUE->i64DefaultGroupID))
    {
        InfoMsg("加入组失败!");
        return;
    }

    if (!eSDK_PTTEmergency(pstUE->i64DefaultGroupID))
    {
        InfoMsg("呼叫失败!");
        return;
    }

    ShowGroupCallWin(pstUE->i64DefaultGroupID, pstUE->acName);         */
}
//---------------------------------------------------------------------------
void __fastcall TFormStation::pmiStationSDSClick(TObject *Sender)
{
    VECTOR<UINT32>      vUserID;
    UINT32              ulStationID = GetSelectStation();
    STATION_UE_INFO_T   *pstUE = StationUE::GetStationUEByStationID(ulStationID);

    if (!pstUE)
    {
        return;
    }

    vUserID.push_back(pstUE->ulUserID);

    FormLatestSDS->NewSDS(vUserID);
}
//---------------------------------------------------------------------------
void __fastcall TFormStation::FormShow(TObject *Sender)
{
    btnPTT->Font->Size = UICfg::GetButtonPTTFontSize();
    btnPTT->Font->Name = UICfg::GetButtonPTTFontName();
}
//---------------------------------------------------------------------------
void __fastcall TFormStation::btnPTTMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    OnPTTDial();
}
//---------------------------------------------------------------------------
void __fastcall TFormStation::btnPTTMouseEnter(TObject *Sender)
{
    btnPTT->LookAndFeel->SkinName = UICfg::GetPTTSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormStation::btnPTTMouseLeave(TObject *Sender)
{
    btnPTT->LookAndFeel->SkinName = GetDefaultSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormStation::btnPTTMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    OnPTTRelease();
}
//---------------------------------------------------------------------------

void __fastcall TFormStation::pmiShowCallWinClick(TObject *Sender)
{
    BOOL    bShow = pmiShowCallWin->Down;

    SetShowCallWin(bShow);

    SaveLocalConfInfo();

    btnPTT->Visible = !bShow;
}
//---------------------------------------------------------------------------

void __fastcall TFormStation::pmStationPopup(TObject *Sender)
{
    pmiShowCallWin->Down = IsShowCallWin();
}
//---------------------------------------------------------------------------
void __fastcall TFormStation::pmiInterLockStationPTTDialClick(TObject *Sender)
{
    UINT32                  ulStationID = GetSelectStation();
    VECTOR<TRAIN_INFO_T*>   vTrain;
    VECTOR<UINT32>          vUser;
    UINT32                  ulUserID1;
    UINT32                  ulUserID2;

    Train::GetTrainOfInterLockStation(ulStationID, vTrain);
    for (UINT32 i=0; i<vTrain.size(); i++)
    {
        Train::GetTrainUserID(vTrain[i]->ulStationID, &ulUserID1, &ulUserID2);
        if (ulUserID1 != INVALID_USER_ID)
        {
            vUser.push_back(ulUserID1);
        }

        if (ulUserID2 != INVALID_USER_ID)
        {
            vUser.push_back(ulUserID2);
        }
    }

    INT64   i64GroupID = DynamicGroup::InitMultiUserGroup(vUser);

    if (i64GroupID == INVALID_GROUP_ID)
    {
        ErrMsg("动态重组失败");
        return;
    }

    m_i64CurrGroupID = i64GroupID;
    UpdateMainGroupCall(i64GroupID);

    if (IsShowCallWin())
    {
        ShowGroupCallWin(i64GroupID);
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormStation::tlMainCustomDrawDataCell(TcxCustomTreeList *Sender,
          TcxCanvas *ACanvas, TcxTreeListEditCellViewInfo *AViewInfo,
          bool &ADone)
{
    if (AViewInfo->Node &&
        !AViewInfo->Node->Values[4].IsNull() &&
        AViewInfo->Column->ItemIndex == 4)
    {
        UINT32  ulStatus = AViewInfo->Node->Values[4];

        if (ulStatus == UE_STATUS_OFFLINE)
        {
            ACanvas->Canvas->Brush->Color = UICfg::GetOfflineDevColor();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormStation::ColumnStationUEStatusGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[4].IsNull())
    {
        return;
    }

    UINT32  ulStatus = ANode->Values[4];

    if (ulStatus == UE_STATUS_ONLINE)
    {
        Value = "";
    }
    else if (ulStatus == UE_STATUS_OFFLINE)
    {
        Value = "离线";
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormStation::OnPTTDial()
{
    if (m_i64CurrGroupID == INVALID_GROUP_ID)
    {
        UINT32              ulStationID = GetSelectStation();
        STATION_UE_INFO_T   *pstUE = StationUE::GetStationUEByStationID(ulStationID);

        m_i64CurrGroupID = INVALID_GROUP_ID;

        if (!pstUE)
        {
            return;
        }

        m_i64CurrGroupID = pstUE->i64DefaultGroupID;
    }

    if (PTT::Dial(m_i64CurrGroupID))
    {
        btnPTT->LookAndFeel->SkinName = UICfg::GetPTTDownSkinName();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormStation::OnPTTRelease()
{
    PTT::Release();
    btnPTT->LookAndFeel->SkinName = UICfg::GetPTTSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormStation::CloseGroupCall(INT64 i64GroupID)
{
    if (m_i64CurrGroupID == i64GroupID)
    {
        // TODO 是否要释放动态创建的组

        m_i64CurrGroupID = INVALID_GROUP_ID;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormStation::pmiStationGroupMemberClick(TObject *Sender)
{
    UINT32              ulStationID = GetSelectStation();
    STATION_UE_INFO_T   *pstUE = StationUE::GetStationUEByStationID(ulStationID);

    if (!pstUE)
    {
        return;
    }

    if (pstUE->i64DefaultGroupID != INVALID_GROUP_ID)
    {
        FormGroup->SetGroupMember(pstUE->i64DefaultGroupID);
    }
}
//---------------------------------------------------------------------------

