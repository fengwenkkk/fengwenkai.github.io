//---------------------------------------------------------------------------

#ifndef DlgSetTrainUEH
#define DlgSetTrainUEH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "cxButtons.hpp"
#include "cxContainer.hpp"
#include "cxControls.hpp"
#include "cxEdit.hpp"
#include "cxGraphics.hpp"
#include "cxLabel.hpp"
#include "cxLookAndFeelPainters.hpp"
#include "cxLookAndFeels.hpp"
#include "cxTrackBar.hpp"
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
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include "cxTextEdit.hpp"

#include "Dialog.h"
#include "cxCheckBox.hpp"
#include "cxDropDownEdit.hpp"
#include "cxMaskEdit.hpp"
#include "cxButtonEdit.hpp"
#include "cxCustomData.hpp"
#include "cxInplaceContainer.hpp"
#include "cxStyles.hpp"
#include "cxTL.hpp"
#include "cxTLdxBarBuiltInMenu.hpp"
#include "cxMemo.hpp"
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
#include "cxCheckListBox.hpp"
#include "cxCustomListBox.hpp"
#include "dxBevel.hpp"
//---------------------------------------------------------------------------
class TDialogSetTrainUE : public TDialogForm
{
__published:    // IDE-managed Components
    TcxButton *btnOK;
    TcxButton *btnCancel;
    TcxLabel *cxLabel6;
    TcxLabel *cxLabel5;
    TcxTextEdit *edtID;
    TcxLabel *cxLabel1;
    TcxTextEdit *edtName;
    TdxBevel *dxBevel1;
    TcxTextEdit *edtIPAddr;
    TcxComboBox *cbTrain;
    TcxLabel *label4;
    void __fastcall btnCancelClick(TObject *Sender);
    void __fastcall btnOKClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
private:    // User declarations
    bool    m_bAdd;
    TRAIN_UE_INFO_T m_stInfo;

    virtual void __fastcall InitSkin(){};
public:     // User declarations
    __fastcall TDialogSetTrainUE(TComponent* Owner);
    void __fastcall Init();
    void __fastcall Init(TRAIN_UE_INFO_T &stInfo);
    void __fastcall Get(TRAIN_UE_INFO_T &stInfo);
};
//---------------------------------------------------------------------------
extern PACKAGE TDialogSetTrainUE *DialogSetTrainUE;
//---------------------------------------------------------------------------
#endif
