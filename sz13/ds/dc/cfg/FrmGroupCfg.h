//---------------------------------------------------------------------------

#ifndef FrmGroupCfgH
#define FrmGroupCfgH
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
#include "cxCalendar.hpp"
#include "cxDateUtils.hpp"
#include "cxMaskEdit.hpp"
#include "dxCore.hpp"
#include <Vcl.ComCtrls.hpp>
#include "cxPC.hpp"
#include "cxSpinEdit.hpp"
#include "cxTimeEdit.hpp"
#include "dxBarBuiltInMenu.hpp"
#include "cxClasses.hpp"
#include "cxLocalization.hpp"
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxScrollBox.hpp"
#include "cxCheckBox.hpp"
#include "cxRadioGroup.hpp"
//---------------------------------------------------------------------------
class TFormGroupCfg : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbCfg;
    TcxButton *btnCancel;
    TcxButton *btnSet;
    TcxLabel *cxLabel10;
    TcxLabel *cxLabel5;
    TcxLabel *cxLabel6;
    TcxLabel *cxLabel7;
    TcxLabel *cxLabel8;
    TcxLabel *cxLabel9;
    TcxTextEdit *edtMainLineTrainGroupID;
    TcxTextEdit *edtUpLineTrainBrdGroupID;
    TcxTextEdit *edtUpLineTrainGroupID;
    TcxTextEdit *edtDownLineTrainGroupID;
    TcxTextEdit *edtMainLineTrainBrdGroupID;
    TcxTextEdit *edtDownLineTrainBrdGroupID;
    TcxLabel *cxLabel25;
    TcxTextEdit *edtAllStationGroupID;
    TcxLabel *cxLabel1;
    TcxTextEdit *edtDriverGroupID;
    TcxLabel *cxLabel2;
    TcxTextEdit *edtDepotDriverGroupID;
    TcxLabel *cxLabel3;
    TcxTextEdit *edtStartTempDynamicGroupID;
    TcxLabel *cxLabel4;
    TcxTextEdit *edtEndTempDynamicGroupID;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall btnCancelClick(TObject *Sender);
    void __fastcall btnSetClick(TObject *Sender);
    void __fastcall MainPanelResize(TObject *Sender);

private:    // User declarations
    virtual void __fastcall InitSkin(){};
    void __fastcall Init();

public:     // User declarations
    __fastcall TFormGroupCfg(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormGroupCfg *FormGroupCfg;
//---------------------------------------------------------------------------
#endif
