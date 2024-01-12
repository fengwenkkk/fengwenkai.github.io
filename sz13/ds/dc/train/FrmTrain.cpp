#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "TrainMap.h"
#include "ClientInclude.h"
#include "main.h"
#include "Train.h"
#include "FrmTrain.h"
#include "FrmGroup.h"
#include "FrmUser.h"
#include "DlgRealBrd.h"
#include "DlgRecBrd.h"
#include "FrmTrainList.h"
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
#pragma link "cxRadioGroup"
#pragma resource "*.dfm"
TFormTrain *FormTrain;

static bool m_bShowStationID = false;
static BOOL m_bShowTrainUnitID = TRUE;


void __fastcall TFormTrain::InitWorkPanel()
{
    TcxButton   *btns[] = { btnGroupCall,
                            btnRealBrd,
                            btnRecBrd,
                            btnSetTrainPos,
//                            btnCallDriver,
                            btnPTT,
                          };

    gbWorkPanel->Width = 150;

    for (UINT32 i=0; i<ARRAY_SIZE(btns); i++)
    {
        btns[i]->TabOrder = i;
        btns[i]->Top = 240+95*i;
        btns[i]->Width = g_iMidToolbarButtonWidth;
        btns[i]->Height = g_iMidToolbarButtonHeight;
        btns[i]->Left = (btns[i]->Parent->Width - btns[i]->Width)/2;
    }

    btnPTT->Width = 120;
    btnPTT->Height = 90;
    btnPTT->Top = btnPTT->Parent->Height - btnPTT->Height - 60;
    btnPTT->Left = (btnPTT->Parent->Width - btnPTT->Width)/2;

    gbSelectTrain->Left = (gbSelectTrain->Parent->Width - gbSelectTrain->Width)/2;
}

void __fastcall TFormTrain::InitSkin()
{
    ScrollBoxMain->LookAndFeel->SkinName = GetDefaultSkinName();
}
//---------------------------------------------------------------------------
__fastcall TFormTrain::TFormTrain(TComponent* Owner)
    : TSubForm(Owner)
{
    m_bInited = false;

    pmiShowCallWin->Down = IsShowCallWin();
    m_ulCheckNodeNum = 0;
    m_i64GroupID = INVALID_GROUP_ID;
    m_bIsPatchGroup = false;

    gbMap->Left = 0;
    gbMap->Top = 0;

    if (!LoadConfInfo("conf", "show_train_unit_id", &m_bShowTrainUnitID))
    {
        m_bShowTrainUnitID = TRUE;
    }

    pmiShowTrainUnitID->Down = m_bShowTrainUnitID;

    InitWorkPanel();

    if (IsSimpleSMSMode())
    {
        pmiStationSDS->Caption = "短信";
        pmiTrainSDS->Caption = "短信";
    }

    if (APP::HasDCType(DC_TYPE_MASTER) ||
        APP::HasDCType(DC_TYPE_SHUNT) ||
        APP::HasDCType(DC_TYPE_DEPOT) ||
        APP::HasDCType(DC_TYPE_FIX) )
    {
        VECTOR<DEPOT_INFO_T*>   &vDepotInfo = Depot::GetDepotInfo();

        if (vDepotInfo.size() == 0)
        {
            pcDepot->Visible = false;
        }

        for (UINT32 i=0; i<vDepotInfo.size(); i++)
        {
            DEPOT_INFO_T    *pstDepot = vDepotInfo[i];

            if (pstDepot->ulDepotID == DEPOT_ID_MAINLINE)
            {
                continue;
            }

            TcxTabSheet     *tsDepot = new TcxTabSheet(pcDepot);

            tsDepot->Caption = pstDepot->acDepotName;
            tsDepot->Tag = pstDepot->ulDepotID;

            tsDepot->Parent = pcDepot;
//            m_mDepotMap[pstDepot->ulDepotID] = tsDepot;
        }
    }

    TrainMapCtrl::Init(m_bShowTrainUnitID, m_bShowStationID,
                       gbMap, gbDepotMap,
                       PanelTransferTrack, PanelTurnbackTrack,
                       pmTrain, pmStation,
                       btnTrainUp,
                       btnTrainDown,
                       btnDepotTrainUp,
                       btnDepotTrainDown,
                       btnTrainMouseDown,
                       btnTrainMouseEnter,
                       btnTrainMouseLeave,
                       btnTrainClick,
                       btnStationMouseDown,
                       btnStationMouseEnter,
                       btnStationMouseLeave,
                       btnStationClick,
                       gbMapDragOver
                       );
}

//---------------------------------------------------------------------------
void __fastcall TFormTrain::InitTrack()
{
    UINT32  ulStationNum = Station::GetNormalStationNum();

    TrainMapCtrl::InitTrack(ulStationNum);

    /*
    PanelTransferTrack->Visible = true;
    PanelTurnbackTrack->Visible = true;

    UINT32  ulStationNum = Station::GetNormalStationNum();
    int     iColNum;

    iColNum = (gbMap->Parent->Width - PanelTransferTrack->Width - PanelTurnbackTrack->Width - 4) / TrainMap::GetColWidth();
    if (iColNum > TrainMap::GetMaxColNum())
    {
        iColNum = TrainMap::GetMaxColNum();
    }

    if (ulStationNum == 0)
    {
        return;
    }

    if (iColNum <= 0)
    {
        iColNum = 1;
    }

    if (ulStationNum <= 4*6 &&
        iColNum > 4)
    {
        iColNum = 4;
        TrainMap::SetColNum(iColNum);
    }

    int iRowNum = (ulStationNum+iColNum-1)/iColNum;
    int iLineHeight = 2;
    int i;
    int iTopMargin = -1;
    TAdvSmoothPanel  *Panel;

    gbMap->Align = alNone;

    gbMap->Width = iColNum*TrainMap::GetColWidth() + 4 + PanelTurnbackTrack->Width;
    gbMap->Left = (gbMap->Parent->Width - gbMap->Width + PanelTransferTrack->Width)/2;

    int iRowHeight = (gbMap->Parent->Height-10)/iRowNum;

    TrainMap::SetRowHeight(iRowHeight);

    gbMap->Height = iRowNum*TrainMap::GetRowHeight();

    int x = 0;
    int y = (TrainMap::GetRowHeight() - iLineHeight)/2;

    while ((int)m_vTrackLine.size() < iRowNum)
    {
        Panel = new TAdvSmoothPanel(gbMap);
        Panel->Parent = gbMap;
        Panel->OnDragOver = gbMapDragOver;
        Panel->SendToBack();
        m_vTrackLine.push_back(Panel);
    }

    for (i=iRowNum; i<(int)m_vTrackLine.size(); i++)
    {
        m_vTrackLine[i]->Visible = false;
    }

    for (i=0; i<iRowNum; i++)
    {
        Panel = m_vTrackLine[i];
        Panel->Visible = true;

        Panel->Height = iLineHeight;
        if (i < (iRowNum-1))
        {
            Panel->Width = TrainMap::GetColWidth()*iColNum;
        }
        else
        {
            int iLastColNum = MOD(ulStationNum, iColNum);

            if (iLastColNum == 0)
            {
                iLastColNum = iColNum;
            }

            Panel->Width = TrainMap::GetColWidth()*iLastColNum;
            PanelTurnbackTrack->Left = x + Panel->Width;// - PanelTurnbackTrack->Width/2;
            PanelTurnbackTrack->Top = y +  TrainMap::GetRowHeight()*i + iLineHeight/2 - PanelTurnbackTrack->Height/2;

//            gbMap->Height = iRowNum*TrainMap::GetRowHeight();
//            iTopMargin = (gbMap->Parent->Height - gbMap->Height)/2;
            if (iTopMargin < 2)
            {
                iTopMargin = 2;
            }
        }

        Panel->Left = x;
        Panel->Top = y + TrainMap::GetRowHeight()*i;
    }

    iTopMargin = (gbMap->Parent->Height - gbMap->Height)/2;
    gbMap->Top = iTopMargin;

    PanelTransferTrack->Parent = gbMap->Parent;
    PanelTransferTrack->Left = gbMap->Left - PanelTransferTrack->Width;
    PanelTransferTrack->Top = gbMap->Top + y + iLineHeight/2 - PanelTransferTrack->Height/2;

    gbMap->Visible = true;  */
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::InitMap()
{
    if (TimerInit->Enabled)
    {
        return;
    }

    InitDepotTrainMap();

    if (APP::HasDCType(DC_TYPE_TRAIN)    ||
        APP::HasDCType(DC_TYPE_PASSENGER)||
        APP::HasDCType(DC_TYPE_SHUNT)    ||
        APP::HasDCType(DC_TYPE_MASTER))
    {
        InitTrack();

        InitStationMap();

        InitTrainMap();
    }

    rbSelectTrainClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::InitStationMap()
{
    VECTOR<STATION_INFO_T*>     vStationInfo;

    Station::GetNormalStation(vStationInfo);

    for (UINT32 i=0; i<vStationInfo.size(); i++)
    {
        STATION_INFO_T  *pstStation = vStationInfo[i];
        bool            bIsLast = ((1+i) == vStationInfo.size());

        TrainMapCtrl::InitStation(pstStation, bIsLast);
    }
}

void __fastcall TFormTrain::InitDepotTrainMap()
{
    if (!APP::HasDCType(DC_TYPE_DEPOT) &&
        !APP::HasDCType(DC_TYPE_FIX)   &&
        !APP::HasDCType(DC_TYPE_SHUNT) &&
        !APP::HasDCType(DC_TYPE_MASTER))
    {
        return;
    }

    if (!pcDepot->ActivePage)
    {
        return;
    }

    UINT32  ulDepotID = (UINT32)pcDepot->ActivePage->Tag;

    TrainMapCtrl::InitDepotTrainMap(ulDepotID);

    /*
    UINT32  ulTrainUnitID;
    UINT32  ulDepotID = (UINT32)pcDepot->ActivePage->Tag;
    VECTOR<TRAIN_INFO_T*>   vTrainInfo;

    if (ulDepotID == DEPOT_ID_UNKNOWN)
    {
        return;
    }

    Train::GetDepotTrainInfo(ulDepotID, vTrainInfo);

    std::map<UINT32, TcxButton*>::iterator it;
    TRAIN_INFO_T        *pstTrainInfo;
    STATION_INFO_T      *pstStation;

    // 搜索不存在的，并隐藏
    for(it=m_mTrainCtrlMap.begin();it!=m_mTrainCtrlMap.end(); it++)
    {
        ulTrainUnitID = it->first;
        pstTrainInfo = Train::GetTrainInfo(ulTrainUnitID);

        if (Station::GetDepotID(pstTrainInfo) != ulDepotID)
        {
            if (it->second->Parent == gbDepotMap)
            {
                it->second->Left = -1000;
            }
        }
    }

    UINT32  ulUnknownPosIndex = 0;

    for (UINT32 i=0; i<vTrainInfo.size(); i++)
    {
        ShowDepotTrain(vTrainInfo[i], ulUnknownPosIndex);
    }  */
}

/*
void __fastcall TFormTrain::ShowDepotTrain(TRAIN_INFO_T *pstTrainInfo, UINT32 &ulUnknownPosIndex)
{
    TcxButton   *Train;
    AnsiString  strName = Train::GetTrainID(pstTrainInfo);

    if (m_bShowTrainUnitID)
    {
        strName = Train::GetTrainUnitIDText(pstTrainInfo);
    }

    Train = m_mTrainCtrlMap[pstTrainInfo->ulTrainUnitID];
    if (!Train)
    {
        Train = new TcxButton(gbDepotMap);

        Train->LookAndFeel->SkinName = GetDefaultSkinName();
        m_mTrainCtrlMap[pstTrainInfo->ulTrainUnitID] = Train;
        Train->Width = TrainMap::GetTrainWidth() - TrainMap::GetTrainMargin();
        Train->Layout = Buttons::blGlyphTop;
        Train->Caption = strName;
        Train->Font->Size = 9;
        Train->Font->Name = "宋体";

        Train->PopupMenu = pmTrain;
    }
    else
    {
//      Train = m_mTrainCtrlMap[pstTrainInfo->ulTrainUnitID];
        Train->Caption = strName;
    }

    Train->Height = TrainMap::GetDepotTrainHeight();
    Train->Parent = gbDepotMap;
    Train->SpeedButtonOptions->Transparent = true;
    Train->SpeedButtonOptions->CanBeFocused = false;
    Train->SpeedButtonOptions->AllowAllUp = true;

    if (pstTrainInfo->ulDirection == TRAIN_DIRECTION_UPSIDE)
    {
        Train->Glyph = btnDepotTrainUp->Glyph;
    }
    else if (pstTrainInfo->ulDirection == TRAIN_DIRECTION_DOWNSIDE)
    {
        Train->Glyph = btnDepotTrainDown->Glyph;
    }

    Train->Visible = IsTrainVisible(pstTrainInfo->ulTrainUnitID);
    Train->OptionsImage->Layout = blGlyphTop;

    int x,y;

    if (!GetDepotTrainPos(pstTrainInfo, x, y))
    {
        x = 20;
        y = 10 + ulUnknownPosIndex*(TrainMap::GetDepotTrainHeight()+5);

        ulUnknownPosIndex++;
    }

    Train->Left = x;
    Train->Top  = y;

    Train->Tag = pstTrainInfo->ulTrainUnitID;
    Train->OnMouseDown = btnTrainMouseDown;
    Train->OnMouseEnter = btnTrainMouseEnter;
    Train->OnMouseLeave = btnTrainMouseLeave;
    Train->OnClick = btnTrainClick;
    Train->OnDragOver = gbMapDragOver;

    Train->OptionsImage->Margin = -1;
    Train->OptionsImage->Spacing = -2;
}
*/

void __fastcall TFormTrain::InitTrainMap()
{
    bool        bShowDepotTrackTrain = false;

    if (pcMainLine->Visible &&
        pcMainLine->ActivePage == tsDepotTrack)
    {
        bShowDepotTrackTrain = true;
    }

    TrainMapCtrl::InitTrainMap(m_bShowTrainUnitID, bShowDepotTrackTrain);
   /* UINT32  ulTrainUnitID;
    VECTOR<TRAIN_INFO_T*>   vTrainInfo;

    Train::GetTrainInfo(vTrainInfo);

    std::map<UINT32, TcxButton*>::iterator it;
    TRAIN_INFO_T        *pstTrainInfo;
    STATION_INFO_T      *pstStation;

    // 搜索不存在的，并隐藏
    for(it=m_mTrainCtrlMap.begin();it!=m_mTrainCtrlMap.end(); it++)
    {
        ulTrainUnitID = it->first;
        pstTrainInfo = Train::GetTrainInfo(ulTrainUnitID);
        pstStation = NULL;
        if (pstTrainInfo)
        {
            pstStation = Station::GetStationInfo(pstTrainInfo->ulStationID);
        }

        if (!pstStation || pstStation->ulStationType != STATION_TYPE_NORMAL)
        {
            if (it->second->Parent == gbMap)
            {
                it->second->Left = -1000;
            }
        }
    }

    // 计算列车位置
    TRAIN_POS_INFO_T    *pstTrainPos;
    UINT32              ulTrainPosKey;

    // 显示正线运营列车
    VectorMap<UINT32, TRAIN_POS_INFO_T>     mTrainPosInfo;          // 正线运营列车
    VectorMap<UINT32, TRAIN_POS_INFO_T>     mDepotTrainPosInfo;     // 正线停车线列车

    for (int i=vTrainInfo.size()-1; i>=0; i--)
    {
        pstTrainInfo = vTrainInfo[i];
        pstStation = Station::GetStationInfo(pstTrainInfo->ulStationID);
        if (!pstStation ||
            pstStation->ulStationType != STATION_TYPE_NORMAL  )
        {
            vTrainInfo.erase(vTrainInfo.begin()+i);
            continue;
        }

        ulTrainPosKey = (pstTrainInfo->ulDirection<<16) | pstTrainInfo->ulStationID;
        if (!pstTrainInfo->ulDepotTrackFlag)
        {
            pstTrainPos = mTrainPosInfo.GetValue(ulTrainPosKey);
            if (!pstTrainPos)
            {
                TRAIN_POS_INFO_T    stTrainPos = {0};

                pstTrainPos = mTrainPosInfo.Add(ulTrainPosKey, stTrainPos);
            }
        }
        else
        {
            pstTrainPos = mDepotTrainPosInfo.GetValue(ulTrainPosKey);
            if (!pstTrainPos)
            {
                TRAIN_POS_INFO_T    stTrainPos = {0};

                pstTrainPos = mDepotTrainPosInfo.Add(ulTrainPosKey, stTrainPos);
            }
        }

        if (pstTrainPos->ulTrainNum == ARRAY_SIZE(pstTrainPos->apstTrainInfo))
        {
            continue;
        }

  //      InitTrainPos(pstTrainInfo);   // 本方法废弃
        pstTrainPos->apstTrainInfo[pstTrainPos->ulTrainNum] = pstTrainInfo;
//        pstTrainPos->aiTrainPos[pstTrainPos->ulTrainNum] = pstTrainPos->ulTrainNum;
        pstTrainPos->aiTrainOrder[pstTrainPos->ulTrainNum] = pstTrainInfo->iTrainOrder;
        pstTrainPos->ulTrainNum++;
    }

    for (UINT32 i=0; i<mTrainPosInfo.Size(); i++)
    {
        pstTrainPos = mTrainPosInfo[i];

        if (pstTrainPos->ulTrainNum == 0)
        {
            continue;
        }

        SetTrainPosIndex(pstTrainPos);
    }

    for (UINT32 i=0; i<mDepotTrainPosInfo.Size(); i++)
    {
        pstTrainPos = mDepotTrainPosInfo[i];

        if (pstTrainPos->ulTrainNum == 0)
        {
            continue;
        }

        SetTrainPosIndex(pstTrainPos);
    }

    for (UINT32 i=0; i<vTrainInfo.size(); i++)
    {
        ShowTrain(vTrainInfo[i]);
    }     */
}

#if 0
void __fastcall TFormTrain::ShowTrain(TRAIN_INFO_T *pstTrainInfo)
{
    int         iStationIndex = Station::GetStationIndex(pstTrainInfo->ulStationID);
    int         x = 0;
    int         y = 0;
    UINT32      ulRowIndex;
    UINT32      ulColIndex;
    TcxButton   *Train;
    AnsiString  strName = Train::GetTrainID(pstTrainInfo);
    bool        bShowDepotTrackTrain = false;
    bool        bVisible = true;

    if (pcMainLine->Visible &&
        pcMainLine->ActivePage == tsDepotTrack)
    {
        bShowDepotTrackTrain = true;
    }

    if (pstTrainInfo->ulDepotTrackFlag && !bShowDepotTrackTrain)
    {
        bVisible = false;
    }
    else if (!pstTrainInfo->ulDepotTrackFlag && bShowDepotTrackTrain)
    {
        bVisible = false;
    }

    if (m_bShowTrainUnitID)
    {
//        strName = pstTrainInfo->ulTrainUnitID;
        strName = Train::GetTrainUnitIDText(pstTrainInfo);
    }

    if (iStationIndex < 0)
    {
        return;
    }

    Train = m_mTrainCtrlMap[pstTrainInfo->ulTrainUnitID];
   // if (m_mTrainCtrlMap.find(pstTrainInfo->ulTrainUnitID) == m_mTrainCtrlMap.end())
    if (!Train)
    {
        Train = new TcxButton(gbMap);

        Train->LookAndFeel->SkinName = GetDefaultSkinName();
        m_mTrainCtrlMap[pstTrainInfo->ulTrainUnitID] = Train;
        Train->Width = TrainMap::GetTrainWidth() - TrainMap::GetTrainMargin();
        Train->Layout = Buttons::blGlyphTop;
        Train->Caption = strName;
        Train->Font->Size = 9;

        Train->PopupMenu = pmTrain;
    }
    else
    {
        Train->Caption = strName;
    }

    Train->Height = TrainMap::GetTrainHeight();
    Train->Parent = gbMap;
    Train->SpeedButtonOptions->AllowAllUp = true;
    Train->SpeedButtonOptions->Transparent = true;
    Train->SpeedButtonOptions->CanBeFocused = false;

    TrainMap::GetStationPos(gbMap->Width, iStationIndex, x, y, &ulRowIndex);

    if (pstTrainInfo->ulDirection == TRAIN_DIRECTION_UPSIDE)
    {
        if (pstTrainInfo->ulDepotTrackFlag)
        {
            Train->Glyph = btnDepotTrainUp->Glyph;
        }
        else
        {
            Train->Glyph = btnTrainUp->Glyph;
        }

    }
    else if (pstTrainInfo->ulDirection == TRAIN_DIRECTION_DOWNSIDE)
    {
        if (pstTrainInfo->ulDepotTrackFlag)
        {
            Train->Glyph = btnDepotTrainDown->Glyph;
        }
        else
        {
            Train->Glyph = btnTrainDown->Glyph;
        }
    }

    Train->Visible = bVisible && IsTrainVisible(pstTrainInfo->ulTrainUnitID);

    int     iPos = 0;

    if (pstTrainInfo->ulDirection == TRAIN_DIRECTION_UPSIDE)
    {
        if (g_bReverseTrainPos)
        {
            Train->Top = y + TrainMap::GetStationHeight() + 2;
            Train->OptionsImage->Layout = blGlyphTop;

         /*   if (pstTrainInfo->ulDepotTrackFlag)
            {
                Train->Top += Train->Height;
            }    */
        }
        else
        {
            Train->Top = y - 2 - Train->Height;
            Train->OptionsImage->Layout = blGlyphBottom;

            /*
            if (pstTrainInfo->ulDepotTrackFlag)
            {
                Train->Top -= Train->Height;
            } */
        }

        iPos = pstTrainInfo->iTrainStationPosIndex;// - 2;
    }
    else if (pstTrainInfo->ulDirection == TRAIN_DIRECTION_DOWNSIDE)
    {
        if (g_bReverseTrainPos)
        {
            Train->Top = y - 2 - Train->Height;
            Train->OptionsImage->Layout = blGlyphBottom;

  /*          if (pstTrainInfo->ulDepotTrackFlag)
            {
                Train->Top -= Train->Height;
            } */
        }
        else
        {
            Train->Top = y + TrainMap::GetStationHeight() + 2;
            Train->OptionsImage->Layout = blGlyphTop;

           /* if (pstTrainInfo->ulDepotTrackFlag)
            {
                Train->Top += Train->Height;
            }    */
        }

        iPos = -pstTrainInfo->iTrainStationPosIndex;// - 2;
    }
    else
    {
        return;
    }

//    iPos *= Train->Width/2;
    iPos *= TrainMap::GetTrainWidth()/2;

    Train->Left = x + (TrainMap::GetColWidth() - Train->Width)/2 + iPos + TrainMap::GetTrainMargin()/2;

    Train->Tag = pstTrainInfo->ulTrainUnitID;
    Train->OnMouseDown = btnTrainMouseDown;
    Train->OnMouseEnter = btnTrainMouseEnter;
    Train->OnMouseLeave = btnTrainMouseLeave;
    Train->OnClick = btnTrainClick;
    Train->OnDragOver = gbMapDragOver;

    Train->OptionsImage->Margin = -1;
    Train->OptionsImage->Spacing = -2;

    bool bManageable = DCUser::IsStationInDCManageRange(pstTrainInfo->ulStationID);

    Train->Enabled = bManageable;
    if (!bManageable)
    {
        Train->Down = false;
    }
}
#endif

/*
void __fastcall TFormTrain::InitStation(STATION_INFO_T *pstStation, bool bIsLast)
{
    TcxButton   *Station = NULL;

    if (m_mStationCtrlMap.find(pstStation->ulStationIndex) == m_mStationCtrlMap.end())
    {
        AnsiString  strName;

        if (m_bShowStationID)
        {
            strName = AnsiString(pstStation->ulStationID) + AnsiString(pstStation->acStationName);
        }
        else
        {
            strName = AnsiString(pstStation->acStationName);
        }

        Station = new TcxButton(gbMap);

        m_mStationCtrlMap[pstStation->ulStationIndex] = Station;

        Station->Parent = gbMap;
        Station->ParentFont = false;
        Station->Font->Size = 11;
        Station->Caption = strName;
        Station->PopupMenu = pmStation;

        Station->OnMouseEnter = btnStationMouseEnter;
        Station->OnMouseDown = btnStationMouseDown;
        Station->OnMouseLeave = btnStationMouseLeave;                                                              Station->OnClick = btnStationClick;
        Station->OnDragOver = gbMapDragOver;

        Station->Tag = pstStation->ulStationID;

        if (pstStation->ulInterLockStationID == pstStation->ulStationID)
        {
            Station->LookAndFeel->SkinName = UICfg::GetInterLockStationSkinName();
        }
        else
        {
            Station->LookAndFeel->SkinName = UICfg::GetStationSkinName();
        }
    }
    else
    {
        Station = m_mStationCtrlMap[pstStation->ulStationIndex];
    }

    Station->SpeedButtonOptions->AllowAllUp = true;
    Station->SpeedButtonOptions->CanBeFocused = false;

    int x;
    int y;

    TrainMap::GetStationPos(gbMap->Width, pstStation->ulStationIndex, x, y, NULL);

    Station->Left = x + (TrainMap::GetColWidth() - TrainMap::GetStationWidth() - TrainMap::GetStationMargin())/2;
    Station->Top = y;
    Station->Width = TrainMap::GetStationWidth();
    Station->Height = TrainMap::GetStationHeight();

    bool bManageable = DCUser::IsStationInDCManageRange(pstStation->ulStationID);

    Station->Enabled = bManageable;
    if (!bManageable)
    {
        Station->Down = false;
    }

    Station->SpeedButtonOptions->Transparent = false;
}
*/

void __fastcall TFormTrain::pmiTrainSDSClick(TObject *Sender)
{
    UINT32  ulCurrTrainUnitID = GetCurrTrainUnitID();

    if (ulCurrTrainUnitID == INVALID_TRAIN_UNIT_ID)
    {
        return;
    }

    VECTOR<UINT32> vUserID;

    Train::GetTrainUserID(ulCurrTrainUnitID, vUserID);

    if (vUserID.size() == 0)
    {
        return;
    }

    FormLatestSDS->NewSDS(vUserID);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::pmiRealBrdClick(TObject *Sender)
{
    UINT32  ulCurrTrainUnitID = GetCurrTrainUnitID();

    if (ulCurrTrainUnitID == INVALID_TRAIN_UNIT_ID)
    {
        return;
    }

    TrainRealBrd(ulCurrTrainUnitID);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::pmiBrdMainLineClick(TObject *Sender)
{
    MainLineTrainRealBrd();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::pmiBrdUpLineClick(TObject *Sender)
{
    UpLineTrainRealBrd();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::pmiBrdDownLineClick(TObject *Sender)
{
    DownLineTrainRealBrd();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::btnTrainMouseEnter(TObject *Sender)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::btnTrainMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    btnTrainMouseLeave(Sender);

    if (Button == mbRight)
    {
        TcxButton *btn = (TcxButton*)Sender;

        btn->Down = true;

        btnTrainDown->Down = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::btnTrainMouseLeave(TObject *Sender)
{
    TcxButton   *btn = (TcxButton*)Sender;

    btn->LookAndFeel->SkinName = GetDefaultSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::btnStationMouseEnter(TObject *Sender)
{
    TcxButton   *btn = (TcxButton*)Sender;

    if (!btn->Down)
    {
        btn->LookAndFeel->SkinName = UICfg::GetActiveStationSkinName();
    }
    else
    {
        btn->LookAndFeel->SkinName = GetDefaultSkinName();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::btnStationMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    btnStationMouseLeave(Sender);

    if (Button == mbRight)
    {
        TcxButton *btn = (TcxButton*)Sender;

        btn->Down = true;

        btnTrainUp->Down = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::btnStationMouseLeave(TObject *Sender)
{
    TcxButton   *btn = (TcxButton*)Sender;
 /*   UINT32      ulStationID = btn->Tag;

    STATION_INFO_T  *pstStation = Station::GetStationInfo(ulStationID);

    if (pstStation && pstStation->ulInterLockStationID == pstStation->ulStationID &&
        !btn->Down)
    {
        btn->LookAndFeel->SkinName = UICfg::GetInterLockStationSkinName();
    }
    else  */
    {
        btn->LookAndFeel->SkinName = UICfg::GetStationSkinName();
    }
}
//---------------------------------------------------------------------------
/*BOOL __fastcall TFormTrain::IsTrainVisible(UINT32 ulTrainUnitID)
{
    TRAIN_INFO_T    *pstTrain = Train::GetTrainInfo(ulTrainUnitID);

    if (!pstTrain)
    {
        return FALSE;
    }

    return TRUE;
} */
//---------------------------------------------------------------------------
/*void __fastcall TFormTrain::OnDCLoadTrainInfoRsp(UINT32 ulSeqID, GJson &Json)
{
    CHAR            *szCfgData = Json.GetValue("CfgData");
    GJson           CfgDataJson;

    if (!CfgDataJson.Parse(szCfgData))
    {
        return;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = CfgDataJson.GetSubJson();
    TRAIN_INFO_T    stInfo = {0};
    TRAIN_INFO_T    *pstTrain;
    VECTOR<TRAIN_INFO_T>    vInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!ParseTrainInfo(pSubJson, stInfo))
        {
            GosLog(LOG_ERROR, "parse train info failed on %s", pSubJson->GetText());
            return;
        }

        vInfo.push_back(stInfo);
    }

    Train::InitTrainInfo(vInfo);

    Group::CheckTrainGroup();
    Group::CheckStationGroup();

    InitMap();
}    */
//---------------------------------------------------------------------------
UINT32 __fastcall TFormTrain::GetCurrStationID()
{
    return TrainMapCtrl::GetCurrStationID();
    /*
    std::map<UINT32, TcxButton*>::iterator it;
    TcxButton           *btn;
    UINT32              ulStationID = INVALID_STATION_ID;

    for(it=m_mStationCtrlMap.begin(); it!=m_mStationCtrlMap.end(); it++)
    {
        btn = it->second;
        if (btn->SpeedButtonOptions->Down)
        {
            if (ulStationID == INVALID_STATION_ID)
            {
                ulStationID = btn->Tag;
            }
            else
            {
                return INVALID_STATION_ID;   // 表示有多站被选中
            }
        }
    }

    return ulStationID;     */
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::pmiStationPrivateCallClick(TObject *Sender)
{
    UINT32  ulCurrStationID = GetCurrStationID();

    if (ulCurrStationID == INVALID_STATION_ID)
    {
        return;
    }

    STATION_UE_INFO_T   *pstInfo = StationUE::GetStationUEByStationID(ulCurrStationID);

    if (!pstInfo)
    {
        return;
    }

    FormUser->Dial(pstInfo->ulUserID);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::pmiStationSDSClick(TObject *Sender)
{
    UINT32  ulCurrStationID = GetCurrStationID();

    if (ulCurrStationID == INVALID_STATION_ID)
    {
        return;
    }

    STATION_UE_INFO_T   *pstInfo = StationUE::GetStationUEByStationID(ulCurrStationID);

    if (!pstInfo)
    {
        return;
    }

    FormLatestSDS->NewSDS(pstInfo->ulUserID);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::FormCreate(TObject *Sender)
{
    SetImage(pmiStationP2PCall, IMG_P2P_CALL);
    SetImage(pmiStationSDS, IMG_SDS);
    SetImage(pmiStationTrainPTTDial, IMG_PTT_DIAL);
    SetImage(pmiInterLockStationPTTDial, IMG_PTT_DIAL);
    SetImage(pmiStationPTTDial, IMG_PTT_DIAL);

    SetImage(pmiPTTDialTrain, IMG_PTT_DIAL);
    SetImage(pmiTrainSDS, IMG_SDS);
    SetImage(pmiSetTrainPos, IMG_SWITCH);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::pmiInterLockStationPTTDialClick(TObject *Sender)
{
    UINT32  ulCurrStationID = GetCurrStationID();

    StationTrainPTTDial(ulCurrStationID, true);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::pmiInterLockStationBrdClick(TObject *Sender)
{
    UINT32  ulCurrStationID = GetCurrStationID();

    InterLockStationRealBrd(ulCurrStationID);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::pmiStationBrdClick(TObject *Sender)
{
    UINT32  ulCurrStationID = GetCurrStationID();

    StationRealBrd(ulCurrStationID);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::StationTrainPTTDial(UINT32 ulStationID, bool bInterLockStationPTTDial)
{
    if (ulStationID == INVALID_STATION_ID)
    {
        return;
    }

    VECTOR<TRAIN_INFO_T*>   vTrain;
    VECTOR<UINT32>          vUser;
    UINT32                  ulUserID1;
    UINT32                  ulUserID2;

    if (bInterLockStationPTTDial)
    {
        Train::GetTrainOfInterLockStation(ulStationID, vTrain);
    }
    else
    {
        Train::GetTrainOfStation(ulStationID, vTrain);
    }

    for (UINT32 i=0; i<vTrain.size(); i++)
    {
        Train::GetTrainUserID(vTrain[i]->ulTrainUnitID, &ulUserID1, &ulUserID2);
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

    m_i64GroupID = i64GroupID;
    UpdateMainGroupCall(i64GroupID);

    if (IsShowCallWin())
    {
        ShowGroupCallWin(i64GroupID);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::TimerInitTimer(TObject *Sender)
{
    if (!TimerInit->Enabled)
    {
        return;
    }

    TimerInit->Enabled = false;
    m_bInited = true;

    gbMainResize(this);
    FormGroupCallWin->Parent = FormMain;

    rbMultiTrain->Checked = true;
    rbSingleTrain->Checked = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::btnTrainClick(TObject *Sender)
{
    TcxButton   *btn = (TcxButton*)Sender;

    if (!btn->Down)
    {
        btn->LookAndFeel->SkinName = UICfg::GetActiveTrainSkinName();
    }
    else
    {
        btn->LookAndFeel->SkinName = GetDefaultSkinName();
    }

    if (!rbSingleTrain->Checked)
    {
        return;
    }

    // 保证车站不被选中
    btnTrainDown->SpeedButtonOptions->Down = true;

    UINT32  ulCurrTrainUnitID = GetCurrTrainUnitID();

    if (ulCurrTrainUnitID == INVALID_TRAIN_UNIT_ID)
    {
        return;
    }

    TRAIN_INFO_T    *pstTrain = Train::GetTrainInfo(ulCurrTrainUnitID);

    if (!pstTrain)
    {
        return;
    }

    INT64   i64GroupID = pstTrain->i64DefaultGroupID;

    if (i64GroupID == INVALID_GROUP_ID)
    {
        return;
    }

    m_i64GroupID = i64GroupID;
    UpdateMainGroupCall(m_i64GroupID);

    if (!IsShowCallWin())
    {
        return;
    }

    int iLeft = btn->Left + btn->Width + 5;
    int iTop = btn->Top + btn->Height + 5;

    AnsiString  strTrainAlias = Train::GetTrainAlias(ulCurrTrainUnitID, pstTrain->acTrainID);

    ShowGroupCallWin(iLeft, iTop, i64GroupID, strTrainAlias);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::btnStationClick(TObject *Sender)
{
    TcxButton   *btn = (TcxButton*)Sender;

    if (!btn->Down)
    {
        btn->LookAndFeel->SkinName = UICfg::GetActiveStationSkinName();
    }
    else
    {
        btn->LookAndFeel->SkinName = GetDefaultSkinName();
    }

    if (!rbSingleTrain->Checked)
    {
        return;
    }

    // 保证列车不被选中
    btnTrainUp->SpeedButtonOptions->Down = true;

    UINT32  ulCurrStationID = btn->Tag;

    if (ulCurrStationID == INVALID_STATION_ID)
    {
        return;
    }

    STATION_UE_INFO_T   *pstUE = StationUE::GetStationUEByStationID(ulCurrStationID);

    if (!pstUE)
    {
        return;
    }

    INT64   i64GroupID = pstUE->i64DefaultGroupID;

    if (i64GroupID == INVALID_GROUP_ID)
    {
        return;
    }

    m_i64GroupID = i64GroupID;
    UpdateMainGroupCall(m_i64GroupID);

    if (!IsShowCallWin())
    {
        return;
    }

    int iLeft = btn->Left + btn->Width + 5;
    int iTop = btn->Top + btn->Height + 5;

    ShowGroupCallWin(iLeft, iTop, i64GroupID);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::gbMapDragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept)
{
    Accept = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::pmiRecBrdClick(TObject *Sender)
{
    UINT32  ulCurrTrainUnitID = GetCurrTrainUnitID();

    if (ulCurrTrainUnitID == INVALID_TRAIN_UNIT_ID)
    {
        return;
    }

    TrainRecBrd(ulCurrTrainUnitID);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::pmiSetTrainPosClick(TObject *Sender)
{
    UINT32  ulTrainPos;
    UINT32  ulCurrTrainUnitID = GetCurrTrainUnitID();

    if (ulCurrTrainUnitID == INVALID_TRAIN_UNIT_ID)
    {
        return;
    }

    TRAIN_INFO_T    *pstTrain = Train::GetTrainInfo(ulCurrTrainUnitID);

    if (!pstTrain)
    {
        InfoMsg("未知列车");
        return;
    }

    SetTrainPos(pstTrain);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::pmiSetTrainIDClick(TObject *Sender)
{
    CHAR            acTrainID[16] = {0};
    UINT32          ulDirection;
    UINT32          ulDestID = 0;
    UINT32          ulCurrTrainUnitID = GetCurrTrainUnitID();

    if (ulCurrTrainUnitID == INVALID_TRAIN_UNIT_ID)
    {
        return;
    }

    TRAIN_INFO_T    *pstTrain = Train::GetTrainInfo(ulCurrTrainUnitID);

    if (!pstTrain)
    {
        InfoMsg("未知列车");
        return;
    }

    SetTrainPos(pstTrain);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::btnRealBrdClick(TObject *Sender)
{
    if (rbSingleTrain->Checked)
    {
        pmiRealBrdClick(this);
    }
    else if (rbMultiTrain->Checked)
    {
        VECTOR<UINT32>  vTrain;

        GetSelectTrain(vTrain);

        if (vTrain.size() == 0)
        {
            InfoMsg("未选择列车");
            return;
        }

        TrainRealBrd(vTrain);
    }
    else if (rbUpLineTrain->Checked)
    {
        UpLineTrainRealBrd();
    }
    else if (rbDownLineTrain->Checked)
    {
        DownLineTrainRealBrd();
    }
    else if (rbMainLineTrain->Checked)
    {
        MainLineTrainRealBrd();
    }
}
//---------------------------------------------------------------------------
UINT32 __fastcall TFormTrain::GetCurrDepotID()
{
    if (!gbDepotMap->Visible)
    {
        return DEPOT_ID_MAINLINE;
    }

    if (!pcDepot->ActivePage)
    {
        return DEPOT_ID_UNKNOWN;
    }

    UINT32  ulDepotID = pcDepot->ActivePage->Tag;

    return ulDepotID;
}
//---------------------------------------------------------------------------
UINT32 __fastcall TFormTrain::GetCurrTrainUnitID()
{
    UINT32  ulDepotID = GetCurrDepotID();

    return TrainMapCtrl::GetCurrTrainUnitID(ulDepotID);

    /*std::map<UINT32, TcxButton*>::iterator it;
    TcxButton           *btn;
    UINT32              ulTrainUnitID = INVALID_TRAIN_UNIT_ID;
    UINT32              ulDepotID = GetCurrDepotID();

    for(it=m_mTrainCtrlMap.begin(); it!=m_mTrainCtrlMap.end(); it++)
    {
        btn = it->second;
        if (btn->SpeedButtonOptions->Down)
        {
            if (Train::GetTrainDepotID(it->first) == ulDepotID)
            {
                if (ulTrainUnitID == INVALID_TRAIN_UNIT_ID)
                {
                    ulTrainUnitID = it->first;
                }
                else
                {
                    return INVALID_TRAIN_UNIT_ID;   // 表示有多车被选中
                }
            }
        }
    }

    return ulTrainUnitID;     */
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::GetSelectTrain(VECTOR<UINT32> &vTrain)
{
    UINT32  ulDepotID = GetCurrDepotID();

    TrainMapCtrl::GetSelectTrain(ulDepotID, vTrain);

    /*std::map<UINT32, TcxButton*>::iterator it;
    TcxButton           *btn;
    UINT32              ulTrainUnitID;
    UINT32              ulDepotID = GetCurrDepotID();

    for(it=m_mTrainCtrlMap.begin(); it!=m_mTrainCtrlMap.end(); it++)
    {
        btn = it->second;
        if (!btn->SpeedButtonOptions->Down)
        {
            continue;
        }

        if (Train::GetTrainDepotID(it->first) == ulDepotID)
        {
            ulTrainUnitID = it->first;
            vTrain.push_back(ulTrainUnitID);
        }
    } */
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::btnRecBrdClick(TObject *Sender)
{
    if (rbSingleTrain->Checked)
    {
        pmiRecBrdClick(this);
    }
    else if (rbMultiTrain->Checked)
    {
        VECTOR<UINT32>  vTrain;

        GetSelectTrain(vTrain);

        if (vTrain.size() == 0)
        {
            InfoMsg("未选择列车");
            return;
        }

        TrainRecBrd(vTrain);
    }
    else if (rbUpLineTrain->Checked)
    {
        LineRecBrd(UPLINE_TRAIN);
    }
    else if (rbDownLineTrain->Checked)
    {
        LineRecBrd(DOWNLINE_TRAIN);
    }
    else if (rbMainLineTrain->Checked)
    {
        LineRecBrd(MAINLINE_TRAIN);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::rbSelectTrainClick(TObject *Sender)
{
    int         iGroupIndex = 10000;
    std::map<UINT32, TcxButton*>::iterator it;
    TcxButton   *SelectTrain = NULL;
    TcxButton   *SelectStation = NULL;
    TcxButton   *SelectDepotTrain1 = NULL;
    TcxButton   *SelectDepotTrain2 = NULL;
    int         iGroupIndexTrain = 1000;
    int         iGroupIndexStation = 2000;
    int         iGroupIndexDepotTrain1 = 5000;
    int         iGroupIndexDepotTrain2 = 6000;
    UINT32      ulFirstDepotID = DEPOT_ID_UNKNOWN;

    VECTOR<DEPOT_INFO_T*>   &vDepot = Depot::GetDepotInfo();

    for (UINT32 j=0; j<vDepot.size(); j++)
    {
        if (vDepot[j]->ulDepotID != DEPOT_ID_MAINLINE)
        {
            ulFirstDepotID = vDepot[j]->ulDepotID;
            break;
        }
    }

    btnTrainUp->SpeedButtonOptions->GroupIndex = iGroupIndexTrain;
    btnTrainDown->SpeedButtonOptions->GroupIndex = iGroupIndexStation;
    btnDepotTrainUp->SpeedButtonOptions->GroupIndex = iGroupIndexDepotTrain1;
    btnDepotTrainDown->SpeedButtonOptions->GroupIndex = iGroupIndexDepotTrain2;

    VECTOR<TcxButton *>  vTrain;

    TrainMapCtrl::GetTrain(vTrain);

   // for(it=m_mTrainCtrlMap.begin();it!=m_mTrainCtrlMap.end();it++)
    for(UINT32 i=0; i<vTrain.size(); i++)
    {
        TcxButton   *Train = vTrain[i];
        UINT32      ulTrainUnitID = Train->Tag;
        TRAIN_INFO_T    *pstTrain = Train::GetTrainInfo(ulTrainUnitID);
        STATION_INFO_T  *pstStation = NULL;

        if (pstTrain)
        {
            pstStation = Station::GetStationInfo(pstTrain->ulStationID);
        }

        if (rbSingleTrain->Checked)
        {
            if (!pstTrain || !pstStation)
            {
                Train->Down = false;
            }
            else
            {
                if (pstStation->ulStationType == STATION_TYPE_NORMAL)
                {
                    // btnTrainUp 、 btnTrainDown的GroupIndex不能相同
                    Train->SpeedButtonOptions->GroupIndex = iGroupIndexTrain;
                    if (Train->Down)
                    {
                        SelectTrain = Train;
                    }
                }
                else
                {
                    if (pstStation->ulDepotID == ulFirstDepotID)
                    {
                        Train->SpeedButtonOptions->GroupIndex = iGroupIndexDepotTrain1;
                        if (Train->Down)
                        {
                            SelectDepotTrain1 = Train;
                        }
                    }
                    else
                    {
                        Train->SpeedButtonOptions->GroupIndex = iGroupIndexDepotTrain2;
                        if (Train->Down)
                        {
                            SelectDepotTrain2 = Train;
                        }
                    }
                }
            }
        }
        else if (rbMultiTrain->Checked)
        {
            Train->SpeedButtonOptions->GroupIndex = iGroupIndex++;
        }
        else
        {
            Train->SpeedButtonOptions->GroupIndex = 0;
        }
    }

    VECTOR<TcxButton*>  vStation;

    TrainMapCtrl::GetStation(vStation);

    for(UINT32 i=0; i<vStation.size(); i++)
    {
        TcxButton   *Station = vStation[i];

        if (rbSingleTrain->Checked)
        {
            Station->SpeedButtonOptions->GroupIndex = iGroupIndexStation;// btnTrainDown->SpeedButtonOptions->GroupIndex;
            if (Station->Down)
            {
                SelectStation = Station;
            }
        }
        else if (rbMultiTrain->Checked)
        {
            Station->SpeedButtonOptions->GroupIndex = iGroupIndex++;
        }
        else
        {
            Station->SpeedButtonOptions->GroupIndex = 0;
        }
    }

    GROUP_CFG_T     *pstGroupCfg = GroupCfg::GetGroupCfg();

    if (rbSingleTrain->Checked)
    {
        if (!SelectTrain)
        {
            btnTrainUp->Down = true;
        }

        if (!SelectStation)
        {
            btnTrainDown->Down = true;
        }

        if (!SelectDepotTrain1)
        {
            btnDepotTrainUp->Down = true;
        }

        if (!SelectDepotTrain2)
        {
            btnDepotTrainDown->Down = true;
        }

        if (SelectTrain && SelectStation &&
            SelectDepotTrain1 && SelectDepotTrain2)
        {
            btnTrainDown->Down = true;
        }
    }
    else if (rbUpLineTrain->Checked)
    {
        m_i64GroupID = pstGroupCfg->i64UpLineTrainGroupID;
    }
    else if (rbDownLineTrain->Checked)
    {
        m_i64GroupID = pstGroupCfg->i64DownLineTrainGroupID;
    }
    else if (rbMainLineTrain->Checked)
    {
        m_i64GroupID = pstGroupCfg->i64MainLineTrainGroupID;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::FormShow(TObject *Sender)
{
    btnPTT->Font->Size = UICfg::GetButtonPTTFontSize();
    btnPTT->Font->Name = UICfg::GetButtonPTTFontName();
}
//---------------------------------------------------------------------------
INT64 __fastcall TFormTrain::GetSelectGroupID()
{
    INT64   i64GroupID = INVALID_GROUP_ID;
    GROUP_CFG_T *pstGroupCfg = GroupCfg::GetGroupCfg();

    if (rbSingleTrain->Checked)
    {
        UINT32  ulTrainUnitID = GetCurrTrainUnitID();
        UINT32  ulStationID = GetCurrStationID();

        if (ulTrainUnitID != INVALID_TRAIN_UNIT_ID)
        {
            i64GroupID = Train::GetTrainGroupID(ulTrainUnitID);
            if (i64GroupID == INVALID_GROUP_ID)
            {
                GosLog(LOG_ERROR, "unknown train %u", ulTrainUnitID);
                return INVALID_GROUP_ID;
            }
        }
        else if (ulStationID != INVALID_STATION_ID)
        {
            i64GroupID = Station::GetStationGroupID(ulStationID);
            if (i64GroupID == INVALID_GROUP_ID)
            {
                GosLog(LOG_ERROR, "unknown station %u", ulStationID);
                return INVALID_GROUP_ID;
            }
        }
        else
        {
//          InfoMsg("请选择列车或车站");
            return INVALID_GROUP_ID;
        }
    }
    else if (rbUpLineTrain->Checked)
    {
        return pstGroupCfg->i64UpLineTrainGroupID;
    }
    else if (rbDownLineTrain->Checked)
    {
        return pstGroupCfg->i64DownLineTrainGroupID;
    }
    else if (rbMainLineTrain->Checked)
    {
        return pstGroupCfg->i64MainLineTrainGroupID;
    }
    else if (rbMultiTrain->Checked)
    {
//        std::map<UINT32, TcxButton*>::iterator it;
        TRAIN_INFO_T        *pstTrainInfo;
        STATION_INFO_T      *pstStation;
        TcxButton           *btn;
        UINT32              ulTrainUnitID;
        UINT32              ulStationID;
        STATION_UE_INFO_T   *pstUE;
        VECTOR<INT64>       vGroupID;
        VECTOR<TcxButton*>  vTrain;
        VECTOR<TcxButton*>  vStation;

        TrainMapCtrl::GetTrain(vTrain);
        for (UINT32 i=0; i<vStation.size(); i++)
//        for(it=m_mTrainCtrlMap.begin(); it!=m_mTrainCtrlMap.end(); it++)
        {
            btn = vTrain[i];//it->second;
            if (!btn->SpeedButtonOptions->Down)
            {
                continue;
            }

            ulTrainUnitID = btn->Tag; //it->first;
            TRAIN_INFO_T    *pstTrain = Train::GetTrainInfo(ulTrainUnitID);

            if (pstTrain)
            {
                vGroupID.push_back(pstTrain->i64DefaultGroupID);
            }
        }

        TrainMapCtrl::GetStation(vStation);
//        for(it=m_mStationCtrlMap.begin(); it!=m_mStationCtrlMap.end(); it++)
        for (UINT32 i=0; i<vStation.size(); i++)
        {
            btn = vStation[i];//it->second;
            if (!btn->SpeedButtonOptions->Down)
            {
                continue;
            }

            ulStationID = btn->Tag;
            pstUE = StationUE::GetStationUEByStationID(ulStationID);
            if (pstUE)
            {
                vGroupID.push_back(pstUE->i64DefaultGroupID);
            }
        }

        if (vGroupID.size() == 0)
        {
            return INVALID_GROUP_ID;
        }
        else if (vGroupID.size() == 1)
        {
            i64GroupID = vGroupID[0];
        }
        else
        {
            m_bIsPatchGroup = false;
            i64GroupID = PatchGroup::AllocTmpPatchGroup(vGroupID);
            if (i64GroupID == INVALID_GROUP_ID)
            {
                ErrMsg("派接组资源已达上限，创建失败");
                return INVALID_GROUP_ID;
            }

            m_bIsPatchGroup = true;
        }
    }

    return i64GroupID;
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::btnGroupCallClick(TObject *Sender)
{
    INT64 i64GroupID = GetSelectGroupID();

    if (i64GroupID == INVALID_GROUP_ID)
    {
        return;
    }

    UpdateMainGroupCall(i64GroupID);

    if (IsShowCallWin())
    {
        if (m_bIsPatchGroup)
        {
            ShowGroupCallWin(i64GroupID, "多选组呼", m_bIsPatchGroup);
        }
        else
        {
            ShowGroupCallWin(i64GroupID);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::btnPTTMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    OnPTTDial();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::btnPTTMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
    OnPTTRelease();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::btnPTTMouseEnter(TObject *Sender)
{
    btnPTT->LookAndFeel->SkinName = UICfg::GetPTTSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::btnPTTMouseLeave(TObject *Sender)
{
    btnPTT->LookAndFeel->SkinName = GetDefaultSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::pmiPTTDialTrainClick(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::pmiStationTrainPTTDialClick(TObject *Sender)
{
    UINT32  ulCurrStationID = GetCurrStationID();

    StationTrainPTTDial(ulCurrStationID, false);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::pmiShowCallWinClick(TObject *Sender)
{
    BOOL bShow = pmiShowCallWin->Down;

    SetShowCallWin(bShow);

    SaveLocalConfInfo();

    btnPTT->Visible = !bShow;
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::pmTrainPopup(TObject *Sender)
{
    pmiShowCallWin->Down = IsShowCallWin();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::btnCallDriverClick(TObject *Sender)
{
    int iLeft = gbMap->Parent->Width - FormGroupCallWin->Width - 3;
    int iTop = btnCallDriver->Top - (FormGroupCallWin->Height - btnCallDriver->Height)/2;
    GROUP_CFG_T     *pstGroupCfg = GroupCfg::GetGroupCfg();

    INT64   i64GroupID = pstGroupCfg->i64DriverGroupID;

    if (APP::GetDepotID() != DEPOT_ID_MAINLINE)
    {
        i64GroupID = pstGroupCfg->i64DepotDriverGroupID;
    }

    if (i64GroupID == INVALID_GROUP_ID)
    {
        InfoMsg("未设置司机组");
        return;
    }

    ShowGroupCallWin(iLeft, iTop, i64GroupID);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::pmiShowTrainUnitIDClick(TObject *Sender)
{
    m_bShowTrainUnitID = pmiShowTrainUnitID->Down;
    SaveConfInfo("conf", "show_train_unit_id", m_bShowTrainUnitID);

    InitTrainMap();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::OnPTTDial()
{
    if (m_i64CurrGroupID == INVALID_GROUP_ID ||
        !m_bIsPatchGroup)
    {
        m_i64CurrGroupID = GetSelectGroupID();
        if (m_i64CurrGroupID == INVALID_GROUP_ID)
        {
            return;
        }
    }

    if (PTT::Dial(m_i64CurrGroupID))
    {
        btnPTT->LookAndFeel->SkinName = UICfg::GetPTTDownSkinName();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::OnPTTRelease()
{
    PTT::Release();
    btnPTT->LookAndFeel->SkinName = UICfg::GetPTTSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::CloseGroupCall(INT64 i64GroupID)
{
    if (m_i64CurrGroupID == i64GroupID)
    {
        if (m_bIsPatchGroup)
        {
            PatchGroup::CancelPatchGroup(m_i64CurrGroupID);
            m_bIsPatchGroup = false;
        }

        m_i64CurrGroupID = INVALID_GROUP_ID;
    }
}
void __fastcall TFormTrain::pmiTrainGroupMemberClick(TObject *Sender)
{
    UINT32  ulCurrTrainUnitID = GetCurrTrainUnitID();

    if (ulCurrTrainUnitID == INVALID_TRAIN_UNIT_ID)
    {
        return;
    }

    INT64   i64GroupID = Train::GetTrainGroupID(ulCurrTrainUnitID);

    if (i64GroupID != INVALID_GROUP_ID)
    {
        FormGroup->SetGroupMember(i64GroupID);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::pmiStationGroupMemberClick(TObject *Sender)
{
    UINT32  ulCurrStationID = GetCurrStationID();

    if (ulCurrStationID == INVALID_STATION_ID)
    {
        return;
    }

    STATION_UE_INFO_T   *pstInfo = StationUE::GetStationUEByStationID(ulCurrStationID);

    if (!pstInfo)
    {
        return;
    }

    if (pstInfo->i64DefaultGroupID != INVALID_GROUP_ID)
    {
        FormGroup->SetGroupMember(pstInfo->i64DefaultGroupID);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::ShowMainLine()
{
    gbDepotMap->Visible = false;
    pcDepot->Visible = false;

    if (APP::HasDCType(DC_TYPE_MASTER) ||
        APP::HasDCType(DC_TYPE_SHUNT) )
    {
        pcMainLine->Visible = true;
    }
    else
    {
        pcMainLine->Visible = false;
        pcMainLine->ActivePage = tsMainLine;
    }

    gbMap->Visible = true;
    PanelTransferTrack->Visible = true;
    gbMap->BringToFront();

    rbUpLineTrain->Enabled = true;
    rbDownLineTrain->Enabled = true;
    rbMainLineTrain->Enabled = true;

    InitTrainMap();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::InitDepotMap(TcxTabSheet *tsDepot)
{
    TrainMapCtrl::InitDepotMap(tsDepot);

    /*
//  gbDepotMap->Parent = tsDepot;
    SECTION_INFO_T  *pstSection;
    UINT32          ulDepotID = (UINT32)tsDepot->Tag;
    VECTOR<SECTION_INFO_T*>     vSection;

    if (ulDepotID == DEPOT_ID_UNKNOWN)
    {
        return;
    }

    Section::GetDepotSection(ulDepotID, vSection);

    std::map<UINT32, TAdvSmoothPanel*>::iterator it = m_mDepotSectionMap.begin();

    for (; it!=m_mDepotSectionMap.end(); it++)
    {
        it->second->Visible = false;

        TcxLabel    *Label = (TcxLabel*)it->second->Tag;

        Label->Visible = false;
    }

    int     iMinX = 10000;
    int     iMaxX = -1;
    int     iMinY = 10000;
    int     iMaxY = -1;

    for (UINT32 i=0; i<vSection.size(); i++)
    {
        pstSection = vSection[i];
        if (pstSection->x1 < 0)
        {
            continue;
        }

        iMinX = MIN(pstSection->x1, iMinX);
        iMaxX = MAX(pstSection->x1, iMaxX);
        iMinY = MIN(pstSection->y1, iMinY);
        iMaxY = MAX(pstSection->y1, iMaxY);

        iMinX = MIN(pstSection->x2, iMinX);
        iMaxX = MAX(pstSection->x2, iMaxX);
        iMinY = MIN(pstSection->y2, iMinY);
        iMaxY = MAX(pstSection->y2, iMaxY);
    }

    int     iRangeX = MAX(100, iMaxX - iMinX);
    int     iRangeY = MAX(100, iMaxY - iMinY);

    gbDepotMap->Width  = gbDepotMap->Parent->Width-2;
    gbDepotMap->Height = gbDepotMap->Parent->Height-6;
    gbDepotMap->Top = 4;
    gbDepotMap->Left = 0;

    for (UINT32 i=0; i<vSection.size(); i++)
    {
        pstSection = vSection[i];
        if (pstSection->x1 < 0)
        {
            continue;
        }

        InitSection(gbDepotMap, pstSection, iMinX, iMinY, iRangeX, iRangeY);
    }

    return;

    int     iDeltaX = gbDepotMap->Parent->Width  - gbDepotMap->Width;
    int     iDeltaY = gbDepotMap->Parent->Height - gbDepotMap->Height;

    if (iDeltaX > 0)
    {
        gbDepotMap->Left += iDeltaX/2;
    }
    else
    {
        gbDepotMap->Left = 0;
    }

    if (iDeltaY > 0)
    {
        gbDepotMap->Top += iDeltaY/2;
    }
    else
    {
        gbDepotMap->Top = 0;
    }   */
}
//---------------------------------------------------------------------------
/*void __fastcall TFormTrain::InitSection(TcxGroupBox *gb, SECTION_INFO_T *pstSection, int iMinX, int iMinY, int iRangeX, int iRangeY)
{
    TAdvSmoothPanel *Panel;
    TcxLabel        *Label;
    int             iLineHeight = 2;
    int             iMarginX = 0;
    int             iMarginY = TrainMap::GetDepotTrainHeight();
    int             iMarginY2 = 5;
    double          dRateX = 1;
    double          dRateY = ((double)gb->Height - iMarginY - iMarginY2)/iRangeY;

    if (dRateY > 10)
    {
        dRateY = 10;
    }

    iMarginX = (gb->Width - dRateX*iRangeX)/2;

    if (m_mDepotSectionMap.find(pstSection->ulSectionID) == m_mDepotSectionMap.end())
    {
        Panel = new TAdvSmoothPanel(gb);
        m_mDepotSectionMap[pstSection->ulSectionID] = Panel;

        Label = new TcxLabel(gb);
        Label->Caption = pstSection->acSectionName;
        Label->Transparent = true;
        Label->Style->TextColor = clGray;
        Label->Parent = gb;

        Panel->Tag = (int)Label;
    }
    else
    {
        Panel = m_mDepotSectionMap[pstSection->ulSectionID];
        Panel->Visible = true;

        Label = (TcxLabel*)Panel->Tag;
        Label->Visible = true;
    }

    Panel->Parent = gb;
    Panel->SendToBack();

    Panel->Left = iMarginX + dRateX*(pstSection->x1 - iMinX);
    Panel->Top  = iMarginY + dRateY*(pstSection->y1 - iMinY);

    if (pstSection->y1 == pstSection->y2)
    {
        Panel->Height = iLineHeight;
        Panel->Width = dRateX*(pstSection->x2 - pstSection->x1);

        Label->Left = Panel->Left + 5;
        Label->Top  = Panel->Top - 20;
    }
    else
    {
        Panel->Height = dRateY*(pstSection->y2 - pstSection->y1);
        Panel->Width = iLineHeight;

        Label->Left = Panel->Left - 20;
        Label->Top  = Panel->Top + 5;
    }
}
//---------------------------------------------------------------------------
bool __fastcall TFormTrain::GetDepotTrainPos(TRAIN_INFO_T *pstTrain, int &x, int &y)
{
    if (m_mDepotSectionMap.find(pstTrain->usSectionID) == m_mDepotSectionMap.end())
    {
        return false;
    }

    TAdvSmoothPanel *Panel = m_mDepotSectionMap[pstTrain->usSectionID];

    x = Panel->Left + (Panel->Width - TrainMap::GetTrainWidth())/2;
    y = Panel->Top - TrainMap::GetDepotTrainHeight();

    return true;
} */
//---------------------------------------------------------------------------
void __fastcall TFormTrain::ShowDepot()
{
    pcMainLine->Visible = false;
    pcDepot->Visible = true;
    gbMap->Visible = false;
    PanelTransferTrack->Visible = false;
    gbDepotMap->BringToFront();

    rbUpLineTrain->Enabled = false;
    rbDownLineTrain->Enabled = false;
    rbMainLineTrain->Enabled = false;

    if (!rbSingleTrain->Checked &&
        !rbMultiTrain->Checked )
    {
        rbSingleTrain->Checked = true;
    }

    TcxTabSheet *tsDepot = pcDepot->ActivePage;

    if (!tsDepot)
    {
        return;
    }

    static TcxTabSheet *tsLastDepot = NULL;
    static int          iDepotMapWidth = -1;
    static int          iDepotMapHeight = -1;

    if (pcDepot->PageCount > 0)
    {
        if (tsDepot != tsLastDepot ||
            gbDepotMap->Width != iDepotMapWidth ||
            gbDepotMap->Height != iDepotMapHeight )
        {
            tsLastDepot = tsDepot;
            iDepotMapWidth = gbDepotMap->Width;
            iDepotMapHeight = gbDepotMap->Height;

            InitDepotMap(tsDepot);
        }
    }

    gbDepotMap->Visible = true;

    InitDepotTrainMap();

    rbSelectTrainClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::pcDepotChange(TObject *Sender)
{
    if (!m_bInited)
    {
        return;
    }

    ShowDepot();
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::gbMainResize(TObject *Sender)
{
    if (m_bInited)
    {
        InitMap();

        if (gbDepotMap->Visible)
        {
            ShowDepot();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormTrain::pcMainLineChange(TObject *Sender)
{
    if (!m_bInited)
    {
        return;
    }

    ShowMainLine();
}
//---------------------------------------------------------------------------

