//---------------------------------------------------------------------------

#ifndef FrmHalfDuplexP2PCallWinH
#define FrmHalfDuplexP2PCallWinH
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

struct GROUP_CALL_CTRL_T;

//---------------------------------------------------------------------------
class TFormHalfDuplexP2PCallWin : public TSubForm
{
__published:    // IDE-managed Components
    TcxButton *btnPTT;
    TcxLabel *LabelUserID;
    TcxGroupBox *gbMain;
    TdxBevel *dxBevel1;
    TcxLabel *Label1;
    TcxLabel *cxLabel2;
    TcxLabel *LabelStatus;
    TcxButton *btnClose;
    TcxLabel *cxLabel3;
    TcxLabel *LabelUserName;
    TcxLabel *cxLabel1;
    TcxLabel *LabelSpeaker;
    void __fastcall btnPTTMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall btnPTTMouseLeave(TObject *Sender);
    void __fastcall btnPTTMouseEnter(TObject *Sender);
    void __fastcall btnPTTMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall btnHangupClick(TObject *Sender);
    void __fastcall FormHide(TObject *Sender);

private:    // User declarations
    virtual void __fastcall InitSkin();

    UINT32      m_ulUserID;
    int         m_iCurrVolume;

    UINT32      m_ulIdleTime;

public:     // User declarations
    __fastcall TFormHalfDuplexP2PCallWin(TComponent* Owner);

    void __fastcall Init(UINT32 ulUserID);
    void __fastcall UpdateP2PCallStatus(EVENT_P2PCALL_STATUS_T &stEvent);
    void __fastcall OnPTTDial();
    void __fastcall OnPTTRelease();
    void __fastcall CloseP2PCall(UINT32 ulUserID);
//  void __fastcall SetParent(TWinControl *Parent);
};

//---------------------------------------------------------------------------
extern PACKAGE TFormHalfDuplexP2PCallWin *FormHalfDuplexP2PCallWin;
//---------------------------------------------------------------------------
#endif
