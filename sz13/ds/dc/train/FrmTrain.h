//---------------------------------------------------------------------------
#ifndef FrmTrainH
#define FrmTrainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "cxButtons.hpp"
#include "cxContainer.hpp"
#include "cxControls.hpp"
#include "cxCustomData.hpp"
#include "cxDropDownEdit.hpp"
#include "cxEdit.hpp"
#include "cxGraphics.hpp"
#include "cxGroupBox.hpp"
#include "cxInplaceContainer.hpp"
#include "cxLookAndFeelPainters.hpp"
#include "cxLookAndFeels.hpp"
#include "cxStyles.hpp"
#include "cxTextEdit.hpp"
#include "cxTL.hpp"
#include "cxTLdxBarBuiltInMenu.hpp"
#include "dxSkinBlack.hpp"
#include "dxSkinBlue.hpp"
#include "dxSkinCaramel.hpp"
#include "dxSkinCoffee.hpp"
#include "dxSkinDarkRoom.hpp"
#include "dxSkinDarkSide.hpp"
#include "dxSkinFoggy.hpp"
#include "dxSkinGlassOceans.hpp"
#include "dxSkiniMaginary.hpp"
#include "dxSkinLilian.hpp"
#include "dxSkinLiquidSky.hpp"
#include "dxSkinLondonLiquidSky.hpp"
#include "dxSkinMcSkin.hpp"
#include "dxSkinMoneyTwins.hpp"
#include "dxSkinOffice2007Black.hpp"
#include "dxSkinOffice2007Blue.hpp"
#include "dxSkinOffice2007Green.hpp"
#include "dxSkinOffice2007Pink.hpp"
#include "dxSkinOffice2007Silver.hpp"
#include "dxSkinOffice2010Black.hpp"
#include "dxSkinOffice2010Blue.hpp"
#include "dxSkinOffice2010Silver.hpp"
#include "dxSkinPumpkin.hpp"
#include "dxSkinsCore.hpp"
#include "dxSkinsDefaultPainters.hpp"
#include "dxSkinSeven.hpp"
#include "dxSkinSharp.hpp"
#include "dxSkinSilver.hpp"
#include "dxSkinSpringTime.hpp"
#include "dxSkinStardust.hpp"
#include "dxSkinSummer2008.hpp"
#include "dxSkinValentine.hpp"
#include "dxSkinXmas2008Blue.hpp"
#include "MDIForm.h"
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include "dxBar.hpp"
#include "dxSkinsdxBarPainter.hpp"
#include "cxImageComboBox.hpp"
#include "cxPC.hpp"
#include "dxSkinscxPCPainter.hpp"
#include <ImgList.hpp>
#include "cxProgressBar.hpp"
#include "cxClasses.hpp"
#include "cxImageList.hpp"
#include "dxBarBuiltInMenu.hpp"
#include "dxSkinBlueprint.hpp"
#include "dxSkinDevExpressDarkStyle.hpp"
#include "dxSkinDevExpressStyle.hpp"
#include "dxSkinHighContrast.hpp"
#include "dxSkinMetropolis.hpp"
#include "dxSkinMetropolisDark.hpp"
#include "dxSkinOffice2013DarkGray.hpp"
#include "dxSkinOffice2013LightGray.hpp"
#include "dxSkinOffice2013White.hpp"
#include "dxSkinOffice2016Colorful.hpp"
#include "dxSkinOffice2016Dark.hpp"
#include "dxSkinSevenClassic.hpp"
#include "dxSkinSharpPlus.hpp"
#include "dxSkinTheAsphaltWorld.hpp"
#include "dxSkinTheBezier.hpp"
#include "dxSkinVisualStudio2013Blue.hpp"
#include "dxSkinVisualStudio2013Dark.hpp"
#include "dxSkinVisualStudio2013Light.hpp"
#include "dxSkinVS2010.hpp"
#include "dxSkinWhiteprint.hpp"
#include <System.ImageList.hpp>
#include "cxTrackBar.hpp"
#include <Vcl.Buttons.hpp>
#include "cxCheckBox.hpp"
#include "dxToggleSwitch.hpp"
#include "cxLabel.hpp"
#include "AdvSmoothPanel.hpp"
#include "cxScrollBox.hpp"
#include "PlanDraw.hpp"
#include "Planner.hpp"
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxRadioGroup.hpp"

//---------------------------------------------------------------------------
class TFormTrain : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbMain;
    TdxBarPopupMenu *pmTrain;
    TcxImageList *ImageListTrain;
    TdxBarManager *BarManager;
    TdxBarButton *pmiStationPrivateCall;
    TdxBarButton *pmiRealBrd;
    TdxBarButton *pmiBrdMainLine;
    TdxBarButton *pmiBrdUpLine;
    TdxBarButton *pmiBrdDownLine;
    TdxBarButton *pmiSetTrainPos;
    TdxBarButton *pmiSetTrainID;
    TdxBarSeparator *dxBarSeparator1;
    TdxBarSeparator *dxBarSeparator4;
    TdxBarPopupMenu *pmStation;
    TdxBarButton *pmiStationP2PCall;
    TdxBarButton *pmiStationSDS;
    TdxBarSeparator *dxBarSeparator5;
    TdxBarButton *pmiTrainSDS;
    TAdvSmoothPanel *PanelTransferTrack;
    TcxButton *btnDepotTrainUp;
    TcxScrollBox *ScrollBoxMain;
    TdxBarSeparator *dxBarSeparator3;
    TdxBarButton *pmiInterLockStationPTTDial;
    TdxBarButton *pmiStationBrd;
    TcxGroupBox *gbMap;
    TdxBarSeparator *dxBarSeparator6;
    TdxBarButton *pmiPTTDialTrain;
    TdxBarSeparator *dxBarSeparator2;
    TTimer *TimerInit;
    TAdvSmoothPanel *PanelTurnbackTrack;
    TAdvSmoothPanel *AdvSmoothPanel2;
    TAdvSmoothPanel *AdvSmoothPanel1;
    TdxBarButton *pmiRecBrd;
    TdxBarSeparator *dxBarSeparator7;
    TdxBarButton *pmiInterLockStationBrd;
    TcxButton *btnGroupCall;
    TcxButton *btnDepotTrainDown;
    TcxButton *btnRealBrd;
    TcxButton *btnRecBrd;
    TcxButton *btnSetTrainPos;
    TcxGroupBox *gbWorkPanel;
    TcxGroupBox *gbSelectTrain;
    TcxRadioButton *rbMultiTrain;
    TcxRadioButton *rbUpLineTrain;
    TcxRadioButton *rbDownLineTrain;
    TcxRadioButton *rbMainLineTrain;
    TcxRadioButton *rbSingleTrain;
    TcxButton *btnSetTrainID;
    TcxButton *btnPTT;
    TdxBarButton *pmiShowCallWin;
    TdxBarButton *pmiStationPTTDial;
    TdxBarButton *pmiStationTrainPTTDial;
    TdxBarSeparator *dxBarSeparator8;
    TdxBarPopupMenu *pmMain;
    TcxButton *btnCallDriver;
    TdxBarButton *pmiShowTrainUnitID;
    TdxBarButton *pmiTrainGroupMember;
    TdxBarButton *pmiStationGroupMember;
    TcxButton *btnTrainUp;
    TcxButton *btnTrainDown;
    TcxPageControl *pcDepot;
    TcxGroupBox *gbDepotMap;
    TcxPageControl *pcMainLine;
    TcxTabSheet *tsMainLine;
    TcxTabSheet *tsDepotTrack;
    void __fastcall pmiTrainSDSClick(TObject *Sender);
    void __fastcall pmiRealBrdClick(TObject *Sender);
    void __fastcall pmiBrdMainLineClick(TObject *Sender);
    void __fastcall pmiBrdUpLineClick(TObject *Sender);
    void __fastcall pmiBrdDownLineClick(TObject *Sender);
    void __fastcall pmiStationPrivateCallClick(TObject *Sender);
    void __fastcall pmiStationSDSClick(TObject *Sender);
    void __fastcall btnTrainMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall btnStationMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall pmiInterLockStationPTTDialClick(TObject *Sender);
    void __fastcall pmiStationBrdClick(TObject *Sender);
    void __fastcall TimerInitTimer(TObject *Sender);
    void __fastcall btnTrainClick(TObject *Sender);
    void __fastcall btnStationClick(TObject *Sender);
    void __fastcall gbMapDragOver(TObject *Sender, TObject *Source, int X, int Y, TDragState State,
          bool &Accept);
    void __fastcall pmiRecBrdClick(TObject *Sender);
    void __fastcall pmiSetTrainPosClick(TObject *Sender);
    void __fastcall pmiSetTrainIDClick(TObject *Sender);
    void __fastcall pmiInterLockStationBrdClick(TObject *Sender);
    void __fastcall btnRealBrdClick(TObject *Sender);
    void __fastcall btnRecBrdClick(TObject *Sender);
    void __fastcall rbSelectTrainClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall btnGroupCallClick(TObject *Sender);
    void __fastcall btnPTTMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall btnPTTMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall btnPTTMouseLeave(TObject *Sender);
    void __fastcall btnPTTMouseEnter(TObject *Sender);
    void __fastcall pmiPTTDialTrainClick(TObject *Sender);
    void __fastcall pmiStationTrainPTTDialClick(TObject *Sender);
    void __fastcall pmiShowCallWinClick(TObject *Sender);
    void __fastcall pmTrainPopup(TObject *Sender);
    void __fastcall btnCallDriverClick(TObject *Sender);
    void __fastcall pmiShowTrainUnitIDClick(TObject *Sender);
    void __fastcall pmiTrainGroupMemberClick(TObject *Sender);
    void __fastcall pmiStationGroupMemberClick(TObject *Sender);
    void __fastcall pcDepotChange(TObject *Sender);
    void __fastcall gbMainResize(TObject *Sender);
    void __fastcall pcMainLineChange(TObject *Sender);


private:    // User declarations
    bool        m_bInited;
    UINT32      m_ulCheckNodeNum;
    INT64       m_i64CurrGroupID;
    INT64       m_i64GroupID;
    bool        m_bIsPatchGroup;

    virtual void __fastcall InitSkin();

    void __fastcall InitWorkPanel();
    void __fastcall InitMap();
    void __fastcall InitTrack();
//    void __fastcall ShowTrain(TRAIN_INFO_T *pstTrainInfo);
//    void __fastcall ShowDepotTrain(TRAIN_INFO_T *pstTrainInfo, UINT32 &ulUnknownPosIndex);
//    void __fastcall InitStation(STATION_INFO_T *pstStation, bool bIsLast);
    void __fastcall btnTrainMouseEnter(TObject *Sender);
    void __fastcall btnTrainMouseLeave(TObject *Sender);
    void __fastcall btnStationMouseEnter(TObject *Sender);
    void __fastcall btnStationMouseLeave(TObject *Sender);
    void __fastcall UpdateCheckNodeNum();
//    void __fastcall OnDCLoadTrainInfoRsp(UINT32 ulSeqID, GJson &Json);
    BOOL __fastcall IsTrainVisible(UINT32 ulTrainUnitID);
    void __fastcall StationTrainPTTDial(UINT32 ulStationID, bool bInterLockStationPTTDial);

    UINT32 __fastcall GetCurrStationID();
    UINT32 __fastcall GetCurrTrainUnitID();
    UINT32 __fastcall GetCurrDepotID();
    void __fastcall GetSelectTrain(VECTOR<UINT32> &vTrain);
    INT64 __fastcall GetSelectGroupID();
    void __fastcall InitDepotMap(TcxTabSheet *tsDepot);
//    void __fastcall InitSection(TcxGroupBox *gb, SECTION_INFO_T *pstSection, int iMinX, int iMinY, int iRangeX, int iRangeY);
//    bool __fastcall GetDepotTrainPos(TRAIN_INFO_T *pstTrain, int &x, int &y);

public:     // User declarations
    __fastcall TFormTrain(TComponent* Owner);

    void __fastcall InitStationMap();
    void __fastcall InitTrainMap();
    void __fastcall InitDepotTrainMap();
    void __fastcall NewEmergencyCall(INT64 i64GroupID, UINT32 ulSpeaker);
    void __fastcall OnPTTDial();
    void __fastcall OnPTTRelease();
    void __fastcall CloseGroupCall(INT64 i64GroupID);
    void __fastcall ShowMainLine();
    void __fastcall ShowDepot();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormTrain *FormTrain;
//---------------------------------------------------------------------------
#endif
