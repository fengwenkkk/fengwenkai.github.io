//---------------------------------------------------------------------------

#ifndef FrmTopoH
#define FrmTopoH
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
#include "cxClasses.hpp"

//---------------------------------------------------------------------------
class TFormTopo : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbMain;
    TcxGroupBox *gbCtrl;
    TcxButton *btnRefresh;
    TTimer *TimerCheck;
    TcxImageList *ImageListDevStatus;
    TChart *chtTopo;
    TcxGroupBox *gbChart;
    TTeeGDIPlus *TeeGDIPlus1;
    TPointSeries *SeriesTXAlarm;
    TPointSeries *SeriesTXOffline;
    TPointSeries *SeriesTXOnline;
    TPointSeries *SeriesFXOnline;
    TPointSeries *SeriesFXOffline;
    TPointSeries *SeriesFXAlarm;
    TPointSeries *SeriesDCOnline;
    TPointSeries *SeriesDCOffline;
    TPointSeries *SeriesDCAlarm;
    TPointSeries *SeriesDISOnline;
    TPointSeries *SeriesDISOffline;
    TPointSeries *SeriesDISAlarm;
    TdxBarManager *BarManager;
    TdxBarButton *DevInfo;
    TdxBarButton *pmiRestart;
    void __fastcall btnRefreshClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall TimerCheckTimer(TObject *Sender);
    void __fastcall OnGetTxStatusRsp(UINT8 *pucMsg, UINT32 ulMsgLen);
    void __fastcall SeriesDISOnlineClick(TChartSeries *Sender, int ValueIndex, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall SeriesFXOfflineClick(TChartSeries *Sender, int ValueIndex, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall DevInfoClick(TObject *Sender);
private:    // User declarations
    virtual void __fastcall InitSkin(){};

//  void __fastcall OnGetTxStatusRsp(UINT8 *pucMsg, UINT32 ulMsgLen);

public:     // User declarations

    __fastcall TFormTopo(TComponent* Owner);

    void __fastcall InitDevList();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormTopo *FormTopo;
//---------------------------------------------------------------------------
#endif
