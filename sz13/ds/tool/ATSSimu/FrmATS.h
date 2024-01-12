//---------------------------------------------------------------------------

#ifndef FrmATSH
#define FrmATSH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "cxButtons.hpp"
#include "cxGraphics.hpp"
#include "cxLookAndFeelPainters.hpp"
#include "cxLookAndFeels.hpp"
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
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include "cxContainer.hpp"
#include "cxControls.hpp"
#include "cxDropDownEdit.hpp"
#include "cxEdit.hpp"
#include "cxMaskEdit.hpp"
#include "cxTextEdit.hpp"
#include "cxCustomData.hpp"
#include "cxInplaceContainer.hpp"
#include "cxStyles.hpp"
#include "cxTL.hpp"
#include "cxTLdxBarBuiltInMenu.hpp"
#include "dxSkinsForm.hpp"
#include "cxPC.hpp"
#include "cxPCdxBarPopupMenu.hpp"
#include "dxSkinscxPCPainter.hpp"
#include "cxGroupBox.hpp"
#include "MDIForm.h"
#include "cxClasses.hpp"
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
#include "cxCheckBox.hpp"
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxDrawTextUtils.hpp"
#include "dxBkgnd.hpp"
#include "dxPrnDev.hpp"
#include "dxPrnPg.hpp"
#include "dxPSCompsProvider.hpp"
#include "dxPSCore.hpp"
#include "dxPScxEditorProducers.hpp"
#include "dxPScxExtEditorProducers.hpp"
#include "dxPScxPageControlProducer.hpp"
#include "dxPSEdgePatterns.hpp"
#include "dxPSEngn.hpp"
#include "dxPSFillPatterns.hpp"
#include "dxPSGlbl.hpp"
#include "dxPSPDFExport.hpp"
#include "dxPSPDFExportCore.hpp"
#include "dxPSPrVwAdv.hpp"
#include "dxPSPrVwRibbon.hpp"
#include "dxPSPrVwStd.hpp"
#include "dxPSUtl.hpp"
#include "dxWrap.hpp"
#include "cxRadioGroup.hpp"
#include "AdvGrid.hpp"
#include "AdvObj.hpp"
#include "AdvUtil.hpp"
#include "BaseGrid.hpp"
#include "cxButtonEdit.hpp"
#include "cxLabel.hpp"
#include "dxBarBuiltInMenu.hpp"
#include <Vcl.Grids.hpp>
#include <Vcl.Dialogs.hpp>
#include "cxSplitter.hpp"
#include "cxSpinEdit.hpp"
#include "cxTLData.hpp"
//---------------------------------------------------------------------------

class TFormATS : public TSubForm
{
__published:    // IDE-managed Components
    TdxSkinController *dxSkinController1;
    TPanel *Panel1;
    TcxTreeList *tlTrain;
    TcxTreeListColumn *ColumnTrainUnitID;
    TcxTreeListColumn *ColumnServiceNumber;
    TcxTreeListColumn *ColumnOrderNumber;
    TcxTreeListColumn *ColumnUpsideStation;
    TcxTreeListColumn *ColumnStation;
    TcxTreeListColumn *ColumnDownsideStation;
    TcxTreeListColumn *ColumnDirection;
    TcxTreeListColumn *ColumnDest;
    TcxTreeListColumn *ColumnTurnbackTrackFlag;
    TcxTreeListColumn *ColumnLostPos;
    TcxTreeListColumn *ColumnPlatformID;
    TcxTreeListColumn *ColumnInOutDepot;
    TcxTreeListColumn *ColumnTransferTrackFlag;
    TPanel *Panel2;
    TPanel *Panel3;
    TcxButton *btnDepart;
    TcxButton *btnTransferTrack;
    TcxButton *btnNextStation;
    TcxButton *btnTurnBack;
    TcxButton *btnRefresh;
    TcxCheckBox *cbTransferTrack;
    TcxCheckBox *cbTurnbackTrack;
    TcxTextEdit *edtCurrStation;
    TcxTextEdit *edtDestID;
    TcxTextEdit *edtServiceNumber;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TLabel *Label4;
    TLabel *Label5;
    TLabel *Label6;
    TcxTextEdit *edtTrainID;
    TcxTextEdit *edtTrainUnitID;
    TTimer *TimerRefresh;
    TcxButton *btnDeposit;
    TcxButton *btnLeaveStation;
    TcxButton *btnEnterStation;
    TcxButton *btnLeaveTransferTrack;
    TcxTextEdit *edtOrderNumber;
    TLabel *Label7;
    TcxTreeListColumn *ColumnSectionID;
    TcxTreeListColumn *ColumnIndexInSection;
    TcxButton *btnInit;
    TcxRadioButton *rbUpSide;
    TcxRadioButton *rbDownSide;
    TLabel *Label8;
    TLabel *Label9;
    TcxTextEdit *edtIndexInSection;
    TcxComboBox *cbSection;
    TLabel *Label10;
    TcxPageControl *pcMain;
    TcxTabSheet *tsATSFile;
    TPanel *Panel5;
    TLabel *Label12;
    TcxButton *btnLoadFile;
    TcxLabel *LabelInfo;
    TcxButtonEdit *edtFile;
    TcxTabSheet *tsATSData;
    TPanel *PanelLeft;
    TcxCheckBox *cbPlayRec;
    TFileOpenDialog *OpenDialog;
    TcxCheckBox *cbFilterRec;
    TcxCheckBox *cbShowHex;
    TcxSplitter *cxSplitter1;
    TcxTreeList *tlATS;
    TcxTreeListColumn *Column0;
    TcxTreeListColumn *Column1;
    TcxTreeListColumn *Column2;
    TcxTreeListColumn *Column3;
    TcxTreeListColumn *Column10;
    TcxTreeListColumn *Column4;
    TcxTreeListColumn *Column5;
    TcxTreeListColumn *Column7;
    TcxTreeListColumn *Column6;
    TcxTreeListColumn *Column8;
    TcxTreeListColumn *Column9;
    TcxTreeListColumn *Column11;
    TcxTreeListColumn *Column13;
    TcxTreeListColumn *Column16;
    TcxTreeListColumn *Column14;
    TcxTreeListColumn *Column12;
    TcxTreeListColumn *Column15;
    TcxTreeListColumn *tlATSColumn18;
    TcxTreeListColumn *tlATSColumn19;
    TTimer *TimerSaveATSRec;
    TcxSpinEdit *edtPlayPeriod;
    TcxTreeList *tlRec;
    TcxTreeListColumn *cxTreeList1Column1;
    TcxTreeListColumn *cxTreeList1Column2;
    TcxTreeListColumn *cxTreeList1Column3;
    TcxTreeListColumn *cxTreeList1Column4;
    TcxTreeListColumn *cxTreeList1Column5;
    TcxTreeListColumn *cxTreeList1Column6;
    TcxTreeListColumn *cxTreeList1Column7;
    TcxTreeListColumn *cxTreeList1Column8;
    TcxTreeListColumn *cxTreeList1Column9;
    TcxTreeListColumn *cxTreeList1Column10;
    TcxGroupBox *gbMain;
    TcxButton *btnSelectAll;
    TcxButton *btnUnSelectAll;
    TcxTreeListColumn *tlATSColumn20;
    void __fastcall btnRefreshClick(TObject *Sender);
    void __fastcall btnNextStationClick(TObject *Sender);
    void __fastcall tlTrainFocusedNodeChanged(TcxCustomTreeList *Sender, TcxTreeListNode *APrevFocusedNode,
          TcxTreeListNode *AFocusedNode);
    void __fastcall btnDepartClick(TObject *Sender);
    void __fastcall btnTransferTrackClick(TObject *Sender);
    void __fastcall btnTurnBackClick(TObject *Sender);
    void __fastcall btnDepositClick(TObject *Sender);
    void __fastcall TimerRefreshTimer(TObject *Sender);
    void __fastcall ColumnStationGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnDirectionGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnTransferTrackFlagGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnTurnbackTrackFlagGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall btnInitClick(TObject *Sender);
    void __fastcall btnLeaveStationClick(TObject *Sender);
    void __fastcall btnEnterStationClick(TObject *Sender);
    void __fastcall btnLeaveTransferTrackClick(TObject *Sender);
    void __fastcall ColumnSectionIDGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall cbTurnbackTrackPropertiesEditValueChanged(TObject *Sender);
    void __fastcall edtCurrStationPropertiesEditValueChanged(TObject *Sender);
    void __fastcall rbUpSideClick(TObject *Sender);
    void __fastcall PlayATSRec();
    void __fastcall edtFilePropertiesButtonClick(TObject *Sender, int AButtonIndex);
    void __fastcall btnLoadFileClick(TObject *Sender);
    void __fastcall ShowATSRec(ATS_REC_T *pstRec, int iIndex);
    void __fastcall ShowATSInfo(ATS_REC_T *pstRec);
    void __fastcall tlATSGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall cbShowHexPropertiesChange(TObject *Sender);
    void __fastcall TimerSaveATSRecTimer(TObject *Sender);
    void __fastcall cbPlayRecPropertiesEditValueChanged(TObject *Sender);
    void __fastcall edtPlayPeriodPropertiesEditValueChanged(TObject *Sender);
    void __fastcall Check();
    void __fastcall tlRecFocusedNodeChanged(TcxCustomTreeList *Sender, TcxTreeListNode *APrevFocusedNode,
          TcxTreeListNode *AFocusedNode);
    void __fastcall tlRecClick(TObject *Sender);
    void __fastcall btnSelectAllClick(TObject *Sender);
    void __fastcall btnUnSelectAllClick(TObject *Sender);
    void __fastcall tlATSGetCellHint(TcxCustomTreeList *Sender, TObject *ACell, UnicodeString &AText,
          bool &ANeedShow);

private:    // User declarations
    int     m_iPlayPeriod;
    virtual void __fastcall InitSkin();
    void __fastcall SaveATSInfo();
    void __fastcall ShowTrainInfo();
    void __fastcall UpdateTrainInfo();
    void __fastcall SaveTrainInfoToFile();
public:     // User declarations
    __fastcall TFormATS(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormATS *FormATS;
//---------------------------------------------------------------------------
#endif
