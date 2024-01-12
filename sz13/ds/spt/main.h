//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "cxButtons.hpp"
#include "cxControls.hpp"
#include "cxCustomData.hpp"
#include "cxDropDownEdit.hpp"
#include "cxGraphics.hpp"
#include "cxInplaceContainer.hpp"
#include "cxLookAndFeelPainters.hpp"
#include "cxLookAndFeels.hpp"
#include "cxPC.hpp"
#include "cxPCdxBarPopupMenu.hpp"
#include "cxStyles.hpp"
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
#include "dxSkinscxPCPainter.hpp"
#include "dxSkinsDefaultPainters.hpp"
#include "dxSkinsdxStatusBarPainter.hpp"
#include "dxSkinSeven.hpp"
#include "dxSkinSharp.hpp"
#include "dxSkinSilver.hpp"
#include "dxSkinSpringTime.hpp"
#include "dxSkinStardust.hpp"
#include "dxSkinSummer2008.hpp"
#include "dxSkinValentine.hpp"
#include "dxSkinXmas2008Blue.hpp"
#include "dxStatusBar.hpp"
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <ImgList.hpp>
#include "dxSkinsForm.hpp"
#include "cxRadioGroup.hpp"
#include "cxContainer.hpp"
#include "cxEdit.hpp"
#include "cxLabel.hpp"
#include "cxGroupBox.hpp"
#include <Buttons.hpp>
#include "cxCheckBox.hpp"
#include "cxButtonEdit.hpp"
#include "cxMaskEdit.hpp"
#include "cxTextEdit.hpp"
#include "cxVGrid.hpp"
#include "dxBar.hpp"
#include "dxSkinsdxBarPainter.hpp"
#include "cxMemo.hpp"
//#include "RzBmpBtn.hpp"
#include "cxImage.hpp"
#include "dxGDIPlusClasses.hpp"
//#include "RzLabel.hpp"
#include "cxCheckComboBox.hpp"
#include "dximctrl.hpp"
#include "cxTrackBar.hpp"
#include "cxHeader.hpp"
#include "cxTreeView.hpp"
#include <ComCtrls.hpp>
#include "MDIForm.h"
#include "cxImageComboBox.hpp"
#include "AdvSmoothLabel.hpp"
#include "cxClasses.hpp"
#include "cxImageList.hpp"
#include "dxBarBuiltInMenu.hpp"
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
#include <System.ImageList.hpp>
#include "cxSplitter.hpp"
#include "cxLocalization.hpp"
#include "dxBevel.hpp"
#include "AdvSmoothPanel.hpp"
#include "cxHint.hpp"
#include "dxCustomHint.hpp"
#include "dxScreenTip.hpp"
#include "AdvPicture.hpp"
#include "AdvSmoothSlider.hpp"
#include <Vcl.Graphics.hpp>
#include "trayicon.h"

//---------------------------------------------------------------------------
class TFormMain : public TSubForm
{
__published:    // IDE-managed Components
    TcxImageList *ImageListStatus;
    TdxBarManager *MainBarManager;
    TdxSkinController *dxSkinController1;
    TcxImageList *ImageListBarManager;
    TPanel *PanelBottom;
    TTimer *TimerCheck;
    TdxBarLargeButton *pmPTTDial;
    TdxBarLargeButton *pmStartRealPlay;
    TdxBarLargeButton *pmStopRealPlay;
    TdxBarLargeButton *pmRecvVideoPlay;
    TdxBarLargeButton *pmP2PDial;
    TPanel *PanelMain;
    TdxBar *MainToolbar;
    TAdvSmoothPanel *PanelWork;
    TTimer *TimerExit;
    TAdvSmoothPanel *PanelStatusBar;
    TImageList *ImageListStatusBar;
    TTimer *TimerInit;
    TTimer *TimerInitAllForm;
    TAdvSmoothPanel *btnNTPStatus;
    TAdvSmoothPanel *btnServerStatus;
    TAdvSmoothPanel *btnVer;
    TAdvSmoothPanel *btnUser;
    TImage *ImageRecCli;
    TTimer *TimerAutoLock;
    TdxBarPopupMenu *pmSys;
    TdxBarButton *pmiLockWindow;
    TdxBarSeparator *dxBarSeparator1;
    TdxBarButton *pmiLogout;
    TdxBarButton *pmiSetPwd;
    TPanel *PanelLockWin;
    TCTrayIcon *TrayIcon;
    TAdvSmoothPanel *PanelStatusWarn;
    TcxLabel *cxLabel4;
    TAdvSmoothPanel *PanelStatusNormal;
    TcxLabel *cxLabel1;
    TAdvSmoothPanel *PanelStatusDisable;
    TcxLabel *cxLabel2;
    TAdvSmoothPanel *PanelStatusError;
    TcxLabel *cxLabel3;
    TAdvSmoothPanel *btnCurrTime;
    void __fastcall TimerInitTimer(TObject *Sender);
    void __fastcall TimerCheckTimer(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall TimerExitTimer(TObject *Sender);
    void __fastcall TimerInitAllFormTimer(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall TimerAutoLockTimer(TObject *Sender);
    void __fastcall pmiLockWindowClick(TObject *Sender);
    void __fastcall pmiLogoutClick(TObject *Sender);
    void __fastcall pmiSetPwdClick(TObject *Sender);
    void __fastcall PanelWorkMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall PanelStatusBarMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);


private:    // User declarations
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(EV_CONNECTION_CHANGE_MSG, TMessage, OnClientConnectionChangeMsg)
        MESSAGE_HANDLER(WM_INNER_MSG, TMessage, OnInnerMsg)
    END_MESSAGE_MAP(TForm)

    void __fastcall OnClientConnectionChangeMsg(TMessage Message);

    virtual void __fastcall InitSkin();
//  void __fastcall OnDISKickOffDCReq(UINT32 ulSeqID, GJson &Json);

    void __fastcall InitAllCtrl();
    void __fastcall ShowCurrentTime();

    void __fastcall HandleException(TObject *Sender, Exception *E);
    void __fastcall InitRecCli();

    void __fastcall MyOnMessage(tagMSG &Msg, bool &Handled);

public:     // User declarations
    __fastcall TFormMain(TComponent* Owner);

    void __fastcall SetStatusButtonStatus(TAdvSmoothPanel *btn, bool bConnected);
    void __fastcall OnConfirmSaveRsp(UINT32 ulSeqID, GJson &Json);

    void __fastcall Exit();
    void __fastcall LockWindow();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;

void SetFormParent(TForm *Child, TWinControl *Parent);


//---------------------------------------------------------------------------
#endif
