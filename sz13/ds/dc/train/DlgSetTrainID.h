//---------------------------------------------------------------------------

#ifndef DlgSetTrainIDH
#define DlgSetTrainIDH
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
#include "dxBevel.hpp"
#include "cxRadioGroup.hpp"

typedef struct
{
    UINT32              ulTrainUnitID;
    UINT32              ulDepotID;
    CHAR                acTrainID[16];
    CHAR                acDestID[8];
    UINT32              ulDirection;
    UINT32              ulStationID;
    UINT32              ulDriverID;
}SET_TRAIN_POS_T;

//---------------------------------------------------------------------------
class TDialogSetTrainID : public TDialogForm
{
__published:    // IDE-managed Components
    TPanel *PanelCfgCtrl;
    TcxButton *btnOK;
    TcxButton *btnCancel;
    TdxBevel *dxBevel1;
    TcxLabel *cxLabel6;
    TcxTextEdit *edtTrainID;
    TcxLabel *cxLabel1;
    TcxComboBox *cbDestID;
    TcxLabel *cxLabel2;
    TcxRadioButton *rbUp;
    TcxRadioButton *rbDown;
    TcxTextEdit *edtDestID;
    TcxLabel *cxLabel3;
    TcxComboBox *cbStationID;
    TcxLabel *cxLabel4;
    TcxComboBox *cbDepotID;
    TcxLabel *cxLabel5;
    TcxTextEdit *edtDriverID;
    TcxLabel *cxLabel7;
    TcxTextEdit *edtTrainUnitID;
    void __fastcall btnCancelClick(TObject *Sender);
    void __fastcall btnOKClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall cbTrainPosPropertiesChange(TObject *Sender);

private:    // User declarations
    UINT32      m_ulDepotID;
    CHAR        m_acTrainID[16];
    CHAR        m_acDestID[3+1];
    UINT32      m_ulDirection;
    UINT32      m_ulStationID;
    UINT32      m_ulDriverID;
    Dict        m_DictStation;
    Dict        m_DictDest;

    virtual void __fastcall InitSkin(){};

public:     // User declarations
    __fastcall TDialogSetTrainID(TComponent* Owner);
    void __fastcall Init(TRAIN_INFO_T *pstTrain, SET_TRAIN_POS_T *pstPos);
    void __fastcall Get(SET_TRAIN_POS_T *pstPos);
};
//---------------------------------------------------------------------------
extern PACKAGE TDialogSetTrainID *DialogSetTrainID;
extern BOOL SetTrainPos(TRAIN_INFO_T *pstTrain, SET_TRAIN_POS_T *pstPos=NULL);

//---------------------------------------------------------------------------
#endif
