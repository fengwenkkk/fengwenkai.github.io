//---------------------------------------------------------------------------

#ifndef MsgDialogH
#define MsgDialogH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
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
#include <Menus.hpp>

#include "MDIForm.h"
#include "Dialog.h"
#include "cxContainer.hpp"
#include "cxControls.hpp"
#include "cxEdit.hpp"
#include "cxImage.hpp"
#include "dxGDIPlusClasses.hpp"
#include "cxLabel.hpp"
#include "cxGroupBox.hpp"
#include "dxSkinsForm.hpp"
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

typedef enum
{
    INFO_MSG,
    WARN_MSG,
    ERROR_MSG,
    ASK_MSG,
}MSG_TYPE_E;

//---------------------------------------------------------------------------
class TFormMsgDialog : public TSubForm
{
__published:    // IDE-managed Components
    TTimer *TimerExit;
    TcxButton *btnOK;
    TcxButton *btnYes;
    TcxButton *btnNo;
    TPanel *Panel1;
    TcxImage *ImageInfo;
    TcxImage *ImageAsk;
    TcxImage *ImageError;
    TcxImage *ImageWarn;
    TLabel *LabelMsg1;
    TcxLabel *LabelMsg;
    TcxGroupBox *gbMain;
    TdxSkinController *dxSkinController1;
    void __fastcall gbYesClick(TObject *Sender);
    void __fastcall TimerExitTimer(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall btnYesClick(TObject *Sender);
    void __fastcall btnOKClick(TObject *Sender);
    void __fastcall btnNoClick(TObject *Sender);
    void __fastcall FormActivate(TObject *Sender);

private:    // User declarations
    int m_iHintTimer;

public:     // User declarations
    __fastcall TFormMsgDialog(TComponent* Owner);

    virtual void __fastcall InitSkin(){};

    void __fastcall SetMsg(MSG_TYPE_E enType, AnsiString strMsg, DWORD dwWaitTime);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMsgDialog *FormMsgDialog;

void InfoMsg(const char* szFormat, ...);
void InfoMsg(AnsiString strMsg);
void WarnMsg(const char* szFormat, ...);
void WarnMsg(AnsiString strMsg);
void ErrMsg(const char* szFormat, ...);
void ErrMsg(AnsiString strMsg);
bool AskMsg(const char* szFormat, ...);
bool AskMsg(AnsiString strMsg);
bool AskMsg(UINT32 ulWaitTime, AnsiString strMsg);

//---------------------------------------------------------------------------
#endif
