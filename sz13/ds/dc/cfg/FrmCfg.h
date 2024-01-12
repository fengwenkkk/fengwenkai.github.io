//---------------------------------------------------------------------------

#ifndef FrmCfgH
#define FrmCfgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "cxControls.hpp"
#include "cxCustomData.hpp"
#include "cxDropDownEdit.hpp"
#include "cxGraphics.hpp"
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
#include "cxContainer.hpp"
#include "cxEdit.hpp"
#include "cxGroupBox.hpp"

#include "MDIForm.h"
#include <ExtCtrls.hpp>
#include "cxButtons.hpp"
#include "cxPC.hpp"
#include "dxSkinscxPCPainter.hpp"
#include <Menus.hpp>
#include "cxRadioGroup.hpp"
#include "cxCheckBox.hpp"
#include "cxImageComboBox.hpp"
#include <ImgList.hpp>
#include "cxMemo.hpp"
#include "cxLabel.hpp"
#include "cxTrackBar.hpp"
#include "cxMaskEdit.hpp"
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
#include "dxBarBuiltInMenu.hpp"
#include "cxClasses.hpp"
#include "dxBar.hpp"
//---------------------------------------------------------------------------
class TFormCfg : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbLeft;
    TcxButton *btnDCUser;
    TPanel *PanelForm;
    TcxGroupBox *gbMain;
    TcxButton *btnStation;
    TcxButton *btnStationUE;
    TcxButton *btnPortalUE;
    TcxButton *btnBrdZone;
    TcxButton *btnTrain;
    TcxButton *btnTrainUE;
    TTimer *TimerInit;
    TcxButton *btnPredefinedSMS;
    TcxButton *btnDCOperator;
    TcxButton *btnRecBrdCfg;
    TcxButton *btnDepot;
    TcxButton *btnGroupCfg;
    void __fastcall btnDCUserClick(TObject *Sender);
    void __fastcall btnStationClick(TObject *Sender);
    void __fastcall btnStationUEClick(TObject *Sender);
    void __fastcall TimerInitTimer(TObject *Sender);
    void __fastcall btnPortalUEClick(TObject *Sender);
    void __fastcall btnBrdZoneClick(TObject *Sender);
    void __fastcall btnTrainClick(TObject *Sender);
    void __fastcall btnTrainUEClick(TObject *Sender);
    void __fastcall btnPredefinedSMSClick(TObject *Sender);
    void __fastcall btnDCOperatorClick(TObject *Sender);
    void __fastcall btnRecBrdCfgClick(TObject *Sender);
    void __fastcall btnDepotClick(TObject *Sender);
    void __fastcall btnGroupCfgClick(TObject *Sender);

private:    // User declarations
    virtual void __fastcall InitSkin();

    void __fastcall ShowForm(TForm *Form);

public:     // User declarations
    __fastcall TFormCfg(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormCfg *FormCfg;

//---------------------------------------------------------------------------
#endif
