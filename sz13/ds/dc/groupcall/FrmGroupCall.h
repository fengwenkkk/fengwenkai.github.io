//---------------------------------------------------------------------------

#ifndef FrmGroupCallH
#define FrmGroupCallH
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
#include "cxGroupBox.hpp"
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

#include "MDIForm.h"
#include <ExtCtrls.hpp>
#include "AdvSmoothPanel.hpp"
#include "cxRadioGroup.hpp"
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
#include "dxZoomTrackBar.hpp"
#include "cxImage.hpp"
#include "dxGDIPlusClasses.hpp"
#include "cxClasses.hpp"
#include "dxSkinsForm.hpp"
#include "dxBevel.hpp"
#include "cxTextEdit.hpp"
#include "cxCheckBox.hpp"

//---------------------------------------------------------------------------
class TFormGroupCall : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbMain;
    TdxBevel *dxBevel1;
    TcxButton *btnClose;
    TcxRadioButton *rbGroupID;
    TcxRadioButton *rbTrainUnitID;
    TcxRadioButton *rbTrainID;
    TcxTextEdit *edtGroupID;
    TcxTextEdit *edtTrainUnitID;
    TcxTextEdit *edtTrainID;
    TcxCheckBox *cbEmergencyCall;
    TcxLabel *LabelStatus;
    TcxLabel *LabelStatusInfo;
    TcxLabel *cxLabel1;
    TcxButton *btnOK;
    TcxButton *btnPTT;
    void __fastcall btnPTTMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall btnPTTMouseLeave(TObject *Sender);
    void __fastcall btnPTTMouseEnter(TObject *Sender);
    void __fastcall btnPTTMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall btnCloseClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall btnOKClick(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall rbGroupClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);

private:    // User declarations
    virtual void __fastcall InitSkin();

    INT64       m_i64GroupID;
    bool        m_bEmergencyCall;

public:     // User declarations
    __fastcall TFormGroupCall(TComponent* Owner);

    INT64 __fastcall GetGroupID(){return m_i64GroupID;};
    void __fastcall Init(INT64 i64GroupID, AnsiString strName);
    void __fastcall Init();
    void __fastcall UpdateGroupCallStatus(EVENT_GROUP_STATUS_T &stEvent);
};

//---------------------------------------------------------------------------
extern PACKAGE TFormGroupCall *FormGroupCall;
//---------------------------------------------------------------------------
#endif
