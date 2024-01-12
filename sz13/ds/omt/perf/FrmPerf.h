//---------------------------------------------------------------------------

#ifndef FrmPerfH
#define FrmPerfH
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
#include "cxLabel.hpp"
#include "cxLookAndFeelPainters.hpp"
#include "cxLookAndFeels.hpp"
#include "cxMemo.hpp"
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
#include "cxProgressBar.hpp"
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
#include <Vcl.Dialogs.hpp>
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxImageComboBox.hpp"
#include "cxImageList.hpp"
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include "cxCheckBox.hpp"
#include "cxCalendar.hpp"
#include "cxDateUtils.hpp"
#include "cxMaskEdit.hpp"
#include "dxCore.hpp"
#include <Vcl.ComCtrls.hpp>
#include "cxSplitter.hpp"
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include "cxPC.hpp"
#include "dxBarBuiltInMenu.hpp"

//---------------------------------------------------------------------------

typedef struct
{
    UINT32              ulTime;
    UINT32              ulWanTxRate;
    UINT32              ulWanRxRate;
    UINT32              ulLan1TxRate;
    UINT32              ulLan1RxRate;
    UINT32              ulLan2TxRate;
    UINT32              ulLan2RxRate;
    UINT32              ulLan3TxRate;
    UINT32              ulLan3RxRate;
    UINT32              ulLan4TxRate;
    UINT32              ulLan4RxRate;
}TRAFFIC_RATE_STATISTICS_T;

class TFormPerf : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbMain;
    TcxTreeList *tlMain;
    TcxTreeListColumn *ColumnNeID;
    TcxGroupBox *gbCtrl;
    TcxButton *btnRefresh;
    TOpenDialog *OpenDialog;
    TTimer *TimerCheck;
    TcxTreeListColumn *ColumnCpuUsage;
    TcxImageList *ImageListDevStatus;
	TcxComboBox *cbDevName;
    TcxLabel *cxLabel1;
    TcxDateEdit *deStartTime;
    TcxCheckBox *cbLastTime;
    TcxTreeListColumn *ColumnTime;
    TcxTreeListColumn *ColumnMemUsage;
    TcxTreeListColumn *ColumnDiskUsage;
    TcxTreeListColumn *ColumnPCI;
    TcxTreeListColumn *ColumnRSRP;
    TcxTreeListColumn *ColumnRSRQ;
    TcxTreeListColumn *ColumnRSSI;
    TcxTreeListColumn *ColumnSINR;
    TcxSplitter *Splitter;
    TChart *chtLte;
    TLineSeries *SeriesRSRQ;
    TLineSeries *SeriesRSRP;
    TLineSeries *SeriesRSSI;
    TLineSeries *SeriesSINR;
    TcxGroupBox *gbChart;
    TTeeGDIPlus *TeeGDIPlus1;
    TcxComboBox *cbDevType;
    TChart *chtBasic;
    TLineSeries *SeriesMem;
    TLineSeries *SeriesCpu;
    TLineSeries *SeriesDisk;
    TLineSeries *SeriesOnline;
    TcxTreeListColumn *ColumnOnline;
    TcxTreeListColumn *ColumnClusterStatus;
    TcxTreeListColumn *ColumnDevType;
    TChart *chtTraffic;
    TLineSeries *SeriesWanTx;
    TLineSeries *SeriesWanRx;
    TLineSeries *SeriesLan1Tx;
    TLineSeries *SeriesLan1Rx;
    TLineSeries *SeriesLan2Tx;
    TLineSeries *SeriesLan2Rx;
    TLineSeries *SeriesLan3Tx;
    TLineSeries *SeriesLan3Rx;
    TLineSeries *SeriesLan4Tx;
    TLineSeries *SeriesLan4Rx;
    TcxTreeListColumn *ColumnWANTx;
    TcxTreeListColumn *ColumnWANRx;
    TcxTreeListColumn *ColumnLAN1Tx;
    TcxTreeListColumn *ColumnLAN1Rx;
    TcxTreeListColumn *ColumnLAN2Tx;
    TcxTreeListColumn *ColumnLAN2Rx;
    TcxTreeListColumn *ColumnLAN3Tx;
    TcxTreeListColumn *ColumnLAN3Rx;
    TcxTreeListColumn *ColumnLAN4Tx;
    TcxTreeListColumn *ColumnLAN4Rx;
    TcxTreeListColumn *ColumnLogsave;
    TcxTreeListColumn *ColumnPackage;
    TcxTreeListColumn *ColumnModel;
	TcxLabel *lbDevType;
	TcxLabel *lbDevName;
	TcxLabel *lbReal;
	TcxPageControl *pcMain;
	TcxTabSheet *tsAll;
	TcxTabSheet *tsBasicStatus;
	TcxTabSheet *tsLteStatus;
	TcxTabSheet *tsTrafficStatus;
    void __fastcall btnRefreshClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall ColumnNeIDGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnTimeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall cbDevTypePropertiesChange(TObject *Sender);
    void __fastcall cbLastTimePropertiesChange(TObject *Sender);
    void __fastcall pcMainChange(TObject *Sender);
    void __fastcall ColumnDevTypeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnOnlineGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
private:    // User declarations
    virtual void __fastcall InitSkin(){};

    void __fastcall OnGetTxStatusRsp(UINT8 *pucMsg, UINT32 ulMsgLen);
    void __fastcall OnGetAllTxStatusRsp(UINT8 *pucMsg, UINT32 ulMsgLen);
	void __fastcall OnGetAllDevStatusRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnGetDevStatusRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnGetLteStatusRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnGetTrafficStatusRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall ChangePage();
	void __fastcall InitTrafficStatusView(VECTOR<TRAFFIC_RATE_STATISTICS_T> &vRate, CHAR *szNeID, UINT32 ulDevType);
public:     // User declarations

    __fastcall TFormPerf(TComponent* Owner);

    void __fastcall UpdateDevList();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormPerf *FormPerf;
//---------------------------------------------------------------------------
#endif
