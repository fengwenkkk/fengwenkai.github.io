//---------------------------------------------------------------------------

#ifndef DlgSetSkinH
#define DlgSetSkinH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "cxControls.hpp"
#include "cxGraphics.hpp"
#include "cxLookAndFeelPainters.hpp"
#include "cxLookAndFeels.hpp"
#include "cxSplitter.hpp"
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
#include "cxCustomData.hpp"
#include "cxInplaceContainer.hpp"
#include "cxStyles.hpp"
#include "cxTextEdit.hpp"
#include "cxTL.hpp"
#include "cxTLdxBarBuiltInMenu.hpp"
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
#include <Grids.hpp>
#include <ImgList.hpp>
#include "cxClasses.hpp"
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
#include <Graphics.hpp>
#include <Grids.hpp>
#include <ImgList.hpp>
#include "cxCustomData.hpp"
#include "cxInplaceContainer.hpp"
#include "cxStyles.hpp"
#include "cxTextEdit.hpp"
#include "cxTL.hpp"
#include "cxTLdxBarBuiltInMenu.hpp"
#include "cxCheckBox.hpp"
#include "cxCheckGroup.hpp"
#include "cxDropDownEdit.hpp"
#include "cxEdit.hpp"
#include "cxVGrid.hpp"

#include "MDIForm.h"
#include <Menus.hpp>
#include "dxBar.hpp"
#include "cxRadioGroup.hpp"
#include "cxMaskEdit.hpp"
#include "cxContainer.hpp"
#include "cxGroupBox.hpp"
#include "cxEditRepositoryItems.hpp"
#include "cxExtEditRepositoryItems.hpp"
#include "cxCheckComboBox.hpp"
#include "cxLabel.hpp"
#include "cxSpinEdit.hpp"
#include "cxPC.hpp"
#include "dxSkinscxPCPainter.hpp"
#include "cxButtons.hpp"
#include <Dialogs.hpp>
#include "cxFontNameComboBox.hpp"
#include "dxLayoutContainer.hpp"
#include "dxLayoutControl.hpp"
#include "dxLayoutControlAdapters.hpp"

#include "Dialog.h"
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
#include "dxBevel.hpp"
//---------------------------------------------------------------------------
class TDialogSetSkin : public TDialogForm
{
__published:    // IDE-managed Components
    TPanel *PanelCfgCtrl;
    TcxButton *btnOK;
    TcxButton *btnCancel;
    TcxLabel *cxLabel29;
    TcxComboBox *cbSkin;
    TcxLabel *cxLabel1;
    TcxComboBox *cbCtrl;
    TdxBevel *dxBevel1;
    void __fastcall btnOKClick(TObject *Sender);
    void __fastcall btnCancelClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall cbCtrlPropertiesChange(TObject *Sender);
    void __fastcall FormActivate(TObject *Sender);
    void __fastcall cbSkinPropertiesChange(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);

private:    // User declarations
    std::map<AnsiString, AnsiString>    m_mSkin;
    void __fastcall SetSystemSkin();

public:     // User declarations
    __fastcall TDialogSetSkin(TComponent* Owner);

    virtual void __fastcall InitSkin();
    void __fastcall Init();
};

//---------------------------------------------------------------------------
extern PACKAGE TDialogSetSkin *DialogSetSkin;
//---------------------------------------------------------------------------
#endif
