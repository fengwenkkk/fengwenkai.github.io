//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
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
#include "cxSplitter.hpp"
#include "cxLabel.hpp"
#include "dxBarBuiltInMenu.hpp"
#include "AdvGrid.hpp"
#include "AdvObj.hpp"
#include "AdvUtil.hpp"
#include "BaseGrid.hpp"
#include <Vcl.Grids.hpp>
#include "cxButtonEdit.hpp"
#include <Vcl.Dialogs.hpp>
//---------------------------------------------------------------------------

typedef struct
{
    UINT8       aucLocalAddr[4]; //localIp=192.168.3.123
    UINT8       aucServerAddr[4];

    CHAR        acLocalAddr[32];
    CHAR        acServerAddr[32];

    UINT16      usServerPort;
    CHAR        acUser[32];
    CHAR        acPwd[32];
}DC_CFG_T;


class TFormMain : public TSubForm
{
__published:    // IDE-managed Components
    TdxSkinController *dxSkinController1;
    TFileOpenDialog *OpenDialog;
    TcxPageControl *pcMain;
    TcxTabSheet *cxTabSheet1;
    TcxTabSheet *cxTabSheet2;
    TcxTabSheet *cxTabSheet3;
    TPanel *Panel1;
    TcxLabel *cxLabel1;
    TcxTextEdit *edtCallee;
    TcxButton *cxButton1;
    TcxLabel *cxLabel2;
    TcxTextEdit *cxTextEdit1;
    TcxTabSheet *tsLogin;
    TcxButton *btnLogin;
    TcxButton *btnLogout;
    TTimer *TimerCheck;
    TcxLabel *LabelMDCStatus;
    void __fastcall TimerCheckTimer(TObject *Sender);
    void __fastcall btnLoginClick(TObject *Sender);

private:    // User declarations

public:     // User declarations
    __fastcall TFormMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif
