//---------------------------------------------------------------------------

#ifndef FrmQueryH
#define FrmQueryH
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
//#include "cxPCdxBarPopupMenu.hpp"
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
class TFormQuery : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbLeft;
    TcxButton *btnCDR;
    TPanel *PanelForm;
    TcxGroupBox *gbMain;
    TcxButton *btnSDS;
    TcxButton *btnRec;
    TcxButton *btnLog;
    TTimer *TimerInit;
    TcxButton *btnRecBrd;
    TcxButton *btnIPH;
    TcxButton *btnReqCall;
    void __fastcall btnCDRClick(TObject *Sender);
    void __fastcall btnSDSClick(TObject *Sender);
    void __fastcall btnRecClick(TObject *Sender);
    void __fastcall TimerInitTimer(TObject *Sender);
    void __fastcall btnLogClick(TObject *Sender);
    void __fastcall btnRecBrdClick(TObject *Sender);
    void __fastcall btnIPHClick(TObject *Sender);
    void __fastcall btnReqCallClick(TObject *Sender);


private:    // User declarations
    virtual void __fastcall InitSkin();

    void __fastcall InitWorkPanel();
    void __fastcall ShowForm(TForm *Form);

public:     // User declarations
    __fastcall TFormQuery(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormQuery *FormQuery;

//---------------------------------------------------------------------------
#endif
