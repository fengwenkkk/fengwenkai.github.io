//---------------------------------------------------------------------------
#ifndef FrmStationH
#define FrmStationH
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

//---------------------------------------------------------------------------
class TFormStation : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbTrain;
    TcxTreeList *tlMain;
    TcxTreeListColumn *ColumnInterLockStationID;
    TcxTreeListColumn *ColumnStationID;
    TcxTreeListColumn *ColumnStationName;
    TcxTreeListColumn *ColumnStationUE;
    TdxBarManager *BarManager;
    TdxBarButton *pmiStationPrivateCall;
    TdxBarSeparator *dxBarSeparator1;
    TdxBarSeparator *dxBarSeparator4;
    TdxBarPopupMenu *pmStation;
    TdxBarButton *pmiStationP2PCall;
    TdxBarButton *pmiStationSDS;
    TdxBarSeparator *dxBarSeparator5;
    TdxBarSeparator *dxBarSeparator3;
    TdxBarButton *pmiInterLockStationPTTDial;
    TdxBarButton *pmiInterLockStationBrd;
    TdxBarSeparator *dxBarSeparator6;
    TdxBarSeparator *dxBarSeparator2;
    TdxBarSeparator *dxBarSeparator7;
    TdxBarButton *pmiInterLockBrd;
    TcxButton *btnRealBrd;
    TcxButton *btnGroupCall;
    TcxButton *btnRecBrd;
    TcxGroupBox *gbWorkPanel;
    TcxTreeListColumn *ColumnStationUEStatus;
    TcxTreeListColumn *ColumnStationUEID;
    TcxTreeListColumn *ColumnStationUECallStatus;
    TcxButton *btnPTT;
    TdxBarSeparator *dxBarSeparator8;
    TdxBarButton *pmiCallStation;
    TcxTreeListColumn *ColumnStationGroup;
    TdxBarSeparator *dxBarSeparator9;
    TdxBarButton *pmiShowCallWin;
    TdxBarButton *pmiStationGroupMember;
    TdxBarSeparator *dxBarSeparator10;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall btnRealBrdClick(TObject *Sender);
    void __fastcall btnRecBrdClick(TObject *Sender);
    void __fastcall ColumnInterLockStationIDGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall btnGroupCallClick(TObject *Sender);
    void __fastcall btnP2PCallClick(TObject *Sender);
    void __fastcall btnEmergencyCallClick(TObject *Sender);
    void __fastcall pmiStationSDSClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall btnPTTMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall btnPTTMouseEnter(TObject *Sender);
    void __fastcall btnPTTMouseLeave(TObject *Sender);
    void __fastcall btnPTTMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall pmiShowCallWinClick(TObject *Sender);
    void __fastcall pmStationPopup(TObject *Sender);
    void __fastcall pmiInterLockStationPTTDialClick(TObject *Sender);
    void __fastcall tlMainCustomDrawDataCell(TcxCustomTreeList *Sender, TcxCanvas *ACanvas,
          TcxTreeListEditCellViewInfo *AViewInfo, bool &ADone);
    void __fastcall ColumnStationUEStatusGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall pmiStationGroupMemberClick(TObject *Sender);

private:    // User declarations
    INT64       m_i64CurrGroupID;

    void __fastcall InitWorkPanel();
    virtual void __fastcall InitSkin();

    void __fastcall InitStation(TcxTreeList *tl);
    UINT32 __fastcall GetSelectStation();
    void __fastcall InterLockStationBrd(UINT32 ulStationID);

public:     // User declarations
    __fastcall TFormStation(TComponent* Owner);

    void __fastcall NewEmergencyCall(INT64 i64GroupID, UINT32 ulSpeaker);
    void __fastcall OnPTTDial();
    void __fastcall OnPTTRelease();
    void __fastcall CloseGroupCall(INT64 i64GroupID);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormStation *FormStation;
//---------------------------------------------------------------------------
#endif
