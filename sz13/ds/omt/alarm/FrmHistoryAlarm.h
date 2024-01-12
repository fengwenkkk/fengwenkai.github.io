//---------------------------------------------------------------------------
#ifndef FrmHistoryAlarmH
#define FrmHistoryAlarmH
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
#include "cxCalendar.hpp"
#include "cxDateUtils.hpp"
#include "dxCore.hpp"
#include <Vcl.ComCtrls.hpp>

//---------------------------------------------------------------------------
class TFormHistoryAlarm : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbTrain;
    TcxTreeList *tlAlarm;
    TcxTreeListColumn *ColumnNeID;
    TcxTreeListColumn *ColumnAlarmLevel;
    TcxTreeListColumn *ColumnAlarmType;
    TcxTreeListColumn *ColumnAlarmRaiseTime;
    TcxTreeListColumn *ColumnAlarmInfo;
    TcxTreeListColumn *ColumnAlarmReason;
    TcxGroupBox *gbWorkPanel;
    TcxTreeListColumn *ColumnAlarmDealAdvise;
    TcxTreeListColumn *ColumnAlarmID;
    TcxButton *btnQuery;
    TLabel *Label3;
    TcxComboBox *cbDevType;
    TLabel *Label1;
    TcxComboBox *cbDev;
    TcxLabel *cxLabel1;
    TcxDateEdit *deStartTime;
    TcxLabel *cxLabel2;
    TcxDateEdit *deEndTime;
    TcxLabel *cxLabel3;
    TcxTextEdit *edtAlarmInfoKey;
    TcxTreeListColumn *ColumnAlarmClearTime;
    TcxTreeListColumn *ColumnAlarmClearInfo;
    TcxButton *btnPrev;
    TcxButton *btnNext;
    void __fastcall tlAlarmCustomDrawDataCell(TcxCustomTreeList *Sender, TcxCanvas *ACanvas,
          TcxTreeListEditCellViewInfo *AViewInfo, bool &ADone);
    void __fastcall ColumnAlarmRaiseTimeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall btnQueryClick(TObject *Sender);
    void __fastcall ColumnAlarmLevelGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnNeIDGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall cbDevTypePropertiesChange(TObject *Sender);
    void __fastcall ColumnAlarmClearTimeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnAlarmIDGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall btnNextClick(TObject *Sender);
    void __fastcall btnPrevClick(TObject *Sender);

private:    // User declarations
    UINT32      m_ulStartTime;
    UINT32      m_ulEndTime;
    virtual void __fastcall InitSkin();
    void __fastcall InitView();
    void __fastcall OnGetHistoryAlarmRsp(UINT32 ulSeqID, GJson &Json);
public:     // User declarations
    __fastcall TFormHistoryAlarm(TComponent* Owner);

    void __fastcall UpdateDevList();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormHistoryAlarm *FormHistoryAlarm;
//---------------------------------------------------------------------------
#endif
