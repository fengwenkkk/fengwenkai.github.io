//---------------------------------------------------------------------------
#ifndef FrmTrainListH
#define FrmTrainListH
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
#include "cxMaskEdit.hpp"
#include "cxRadioGroup.hpp"
#include "cxImage.hpp"
#include "dxGDIPlusClasses.hpp"

//---------------------------------------------------------------------------
class TFormTrainList : public TSubForm
{
__published:    // IDE-managed Components
    TdxBarPopupMenu *pmTrain;
    TcxGroupBox *gbTrain;
    TcxTreeList *tlTrain;
    TcxTreeListColumn *ColumnTrainID;
    TcxTreeListColumn *ColumnTrainUnitID;
    TcxTreeListColumn *ColumnDirection;
    TcxTreeListColumn *ColumnStation;
    TcxTreeListColumn *ColumnCurrGroupID;
    TTimer *TimerGetATS;
    TdxBarManager *BarManager;
    TdxBarButton *pmiStationPrivateCall;
    TdxBarButton *pmiRealBrd;
    TdxBarButton *pmiSetTrainPos;
    TdxBarButton *pmiSetTrainDepotID;
    TdxBarSeparator *dxBarSeparator1;
    TdxBarSeparator *dxBarSeparator4;
    TdxBarButton *pmiTrainSDS;
    TdxBarSeparator *dxBarSeparator6;
    TdxBarButton *pmiPTTDialTrain;
    TdxBarSeparator *dxBarSeparator2;
    TTimer *TimerInit;
    TcxTreeListColumn *ColumnDepotID;
    TcxTreeListColumn *ColumnCallStatus;
    TcxTreeListColumn *ColumnCallNumber;
    TdxBarButton *pmiRecBrd;
    TcxButton *btnRealBrd;
    TcxButton *btnGroupCall;
    TcxButton *btnRecBrd;
    TcxButton *btnSetTrainPos;
    TcxGroupBox *gbWorkPanel;
    TcxGroupBox *gbSelectTrain;
    TcxRadioButton *rbSingleTrain;
    TcxRadioButton *rbMultiTrain;
    TcxRadioButton *rbUpLineTrain;
    TcxRadioButton *rbDownLineTrain;
    TcxRadioButton *rbMainLineTrain;
    TdxBarButton *pmiShowAllTrain;
    TcxTreeListColumn *ColumnDevStatus;
    TcxTreeListColumn *ColumnDriverID;
    TcxButton *btnPTT;
    TcxTreeListColumn *ColumnCoverStatus;
    TdxBarButton *pmiTrainInfo;
    TdxBarButton *pmiShowCallWin;
    TcxButton *btnCallDriver;
    TdxBarButton *pmiTrainGroupMember;
    TdxBarSeparator *dxBarSeparator3;
    TcxTreeListColumn *ColumnGPSInfo;
    TcxTreeListColumn *ColumnDriveMode;
    TcxTreeListColumn *ColumnSection;
    TcxTreeListColumn *ColumnATSStatus;
    TcxImageList *ImageListTrain;
    void __fastcall pmiTrainSDSClick(TObject *Sender);
    void __fastcall pmiRealBrdClick(TObject *Sender);
    void __fastcall tlTrainCustomDrawDataCell(TcxCustomTreeList *Sender, TcxCanvas *ACanvas,
          TcxTreeListEditCellViewInfo *AViewInfo, bool &ADone);
    void __fastcall tlTrainNodeCheckChanged(TcxCustomTreeList *Sender, TcxTreeListNode *ANode,
          TcxCheckBoxState AState);
    void __fastcall btnRealBrdClick(TObject *Sender);
    void __fastcall ColumnStationGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall TimerGetATSTimer(TObject *Sender);
    void __fastcall btnSDSClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall ColumnCurrGroupIDGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall pmiRecBrdClick(TObject *Sender);
    void __fastcall pmiSetTrainPosClick(TObject *Sender);
    void __fastcall btnGroupCallClick(TObject *Sender);
    void __fastcall btnRecBrdClick(TObject *Sender);
    void __fastcall rbSelectTrainClick(TObject *Sender);
    void __fastcall tlTrainClick(TObject *Sender);
    void __fastcall pmiShowAllTrainClick(TObject *Sender);
    void __fastcall btnSetTrainPosClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall btnPTTMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall btnPTTMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall ColumnDevStatusGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall btnPTTMouseEnter(TObject *Sender);
    void __fastcall btnPTTMouseLeave(TObject *Sender);
    void __fastcall ColumnCallNumberGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall pmiSetTrainDepotIDClick(TObject *Sender);
    void __fastcall pmiPTTDialTrainClick(TObject *Sender);
    void __fastcall pmiShowCallWinClick(TObject *Sender);
    void __fastcall pmTrainPopup(TObject *Sender);
    void __fastcall btnCallDriverClick(TObject *Sender);
    void __fastcall pmiTrainGroupMemberClick(TObject *Sender);
    void __fastcall pmiTrainInfoClick(TObject *Sender);
    void __fastcall ColumnTrainUnitIDGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnDriveModeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnSectionGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnDirectionGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnATSStatusGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);

private:    // User declarations
    UINT32      m_ulCheckNodeNum;
    INT64       m_i64CurrGroupID;
    bool        m_bIsPatchGroup;
    UINT32      m_ulLastDISATSTimeDiff;  // DIS返回有变化的ATS数据的相对时间
    UINT32      m_ulDCGetATSUptime;     // DC获取ATS数据的相对时间（收到应答后重置，每次发送更新，非0不更新）

    void __fastcall InitWorkPanel();
    virtual void __fastcall InitSkin();

    void __fastcall InitTrain();
    void __fastcall UpdateTrainInfo();
    void __fastcall ShowTrain(TRAIN_INFO_T *pstTrainInfo);
    void __fastcall ShowDepotTrain(UINT32 ulIndex, TRAIN_INFO_T *pstTrainInfo);
    void __fastcall UpdateCheckNodeNum();
    void __fastcall GetSelectTrain(VECTOR<UINT32> &vTrain);
    void __fastcall OnDCLoadTrainInfoRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall InterLockStationPTTDial(UINT32 ulStationID);
    UINT32 __fastcall GetSelectTrain();
    INT64 __fastcall GetSelectTrainGroupID();
    void __fastcall CheckATSStatus();

public:     // User declarations
    __fastcall TFormTrainList(TComponent* Owner);

    void __fastcall OnPTTDial();
    void __fastcall OnPTTRelease();

    void __fastcall NewGroupCall(INT64 i64GroupID);
    void __fastcall CloseGroupCall(INT64 i64GroupID);
    void __fastcall UpdateATSStatus();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormTrainList *FormTrainList;
//---------------------------------------------------------------------------
#endif
