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
#include "cxImage.hpp"
#include "dxGDIPlusClasses.hpp"
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

typedef enum
{
    PAGE_NONE = 0,
    PAGE_MAINLINE,
    PAGE_DEPOT_TRAIN,
    PAGE_TRAIN,
    PAGE_STATION,
    PAGE_GROUP,
    PAGE_USER,

    PAGE_VIDEO,
    PAGE_IPH,
    PAGE_SDS,
    PAGE_QUERY,
    PAGE_RECCLI,
    PAGE_SYS,
    PAGE_LOCALCFG,
    PAGE_TEST,

    PAGE_NUM
}PAGE_E;

//---------------------------------------------------------------------------
class TFormMain : public TSubForm
{
__published:    // IDE-managed Components
    TcxImageList *ImageListStatus;
    TdxBarManager *MainBarManager;
    TdxSkinController *dxSkinController1;
    TcxImageList *ImageListCall;
    TcxImageList *ImageListMain;
    TcxImageList *ImageListBarManager;
    TPanel *PanelBottom;
    TTimer *TimerCheck;
    TdxBarLargeButton *pmPTTDial;
    TdxBarLargeButton *pmStartRealPlay;
    TdxBarLargeButton *pmStopRealPlay;
    TdxBarLargeButton *pmRecvVideoPlay;
    TdxBarLargeButton *pmP2PDial;
    TTimer *TimerCheckService;
    TTimer *TimerRegisterDIS;
    TPanel *PanelMain;
    TdxBar *MainToolbar;
    TcxImageList *ImageListToolbar;
    TTimer *TimerCheckDynamicGroup;
    TAdvSmoothPanel *PanelWork;
    TPanel *PanelTop;
    TcxImageList *ImageListToolbar1;
    TAdvSmoothPanel *PanelCallIn;
    TcxImageList *ImageListDev;
    TTimer *TimerExit;
    TAdvSmoothPanel *PanelResource;
    TcxButton *btnTrain;
    TcxButton *btnGroup;
    TcxButton *btnUser;
    TAdvSmoothPanel *PanelToolbar;
    TcxButton *btnVideo;
    TcxButton *btnSDS;
    TcxButton *btnIPH;
    TcxButton *btnQuery;
    TcxButton *btnSys;
    TcxButton *btnHelp;
    TcxButton *btnExit;
    TcxButton *btnTest;
    TAdvSmoothPanel *PanelLogo;
    TcxButton *btnMinimize;
    TAdvSmoothPanel *PanelStatusBar;
    TPanel *PanelMon;
    TcxButton *btnEmpty;
    TcxButton *btnStation;
    TcxImage *imgLogo;
    TAdvSmoothLabel *AdvSmoothLabel1;
    TcxButton *btnMainLine;
    TcxButton *btnLeftEmpty;
    TcxButton *btnTopEmpty;
    TAdvSmoothPanel *PanelInbox;
    TImageList *ImageListStatusBar;
    TcxButton *btnBackPage;
    TcxButton *btnLocalCfg;
    TTimer *TimerInit;
    TcxSplitter *SplitterMain;
    TTimer *TimerPTT;
    TAdvSmoothPanel *btnMicStatus;
    TAdvSmoothPanel *PanelStatusNormal;
    TcxLabel *cxLabel1;
    TAdvSmoothPanel *PanelStatusDisable;
    TcxLabel *cxLabel2;
    TAdvSmoothPanel *PanelStatusError;
    TcxLabel *cxLabel3;
    TAdvSmoothPanel *btnMainGroup;
    TAdvSmoothPanel *btnCurrSpeaker;
    TAdvSmoothPanel *btnCurrTime;
    TAdvSmoothPanel *btnSpeakerStatus;
    TAdvSmoothPanel *btnNTPStatus;
    TAdvSmoothPanel *btnATSStatus;
    TAdvSmoothPanel *btnDISStatus;
    TAdvSmoothPanel *btnMDCStatus;
    TAdvSmoothPanel *btnDCVer;
    TAdvSmoothPanel *btnDCUser;
    TAdvSmoothPanel *btnDCType;
    TAdvSmoothSlider *SliderMic;
    TAdvSmoothPanel *PanelStatusWarn;
    TcxLabel *cxLabel4;
    TTimer *TimerAutoLock;
    TdxBarPopupMenu *pmSys;
    TdxBarButton *pmiLockWindow;
    TdxBarSeparator *dxBarSeparator1;
    TdxBarButton *pmiLogout;
    TdxBarButton *pmiSetPwd;
    TPanel *PanelLockWin;
    TCTrayIcon *TrayIcon;
    TTimer *TimerGetCfgVer;
    TTimer *TimerResize;
    TcxButton *btnDepotTrain;
    void __fastcall TimerInitTimer(TObject *Sender);
    void __fastcall TimerCheckTimer(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall TimerCheckServiceTimer(TObject *Sender);
    void __fastcall TimerRegisterDISTimer(TObject *Sender);
    void __fastcall TimerCheckDynamicGroupTimer(TObject *Sender);
    void __fastcall btnHelpClick(TObject *Sender);
    void __fastcall TimerExitTimer(TObject *Sender);
    void __fastcall btnMainLineClick(TObject *Sender);
    void __fastcall btnMinimizeClick(TObject *Sender);
    void __fastcall btnGroupClick(TObject *Sender);
    void __fastcall btnUserClick(TObject *Sender);
    void __fastcall btnExitClick(TObject *Sender);
    void __fastcall btnTrainClick(TObject *Sender);
    void __fastcall btnStationClick(TObject *Sender);
    void __fastcall btnVideoClick(TObject *Sender);
    void __fastcall btnSDSClick(TObject *Sender);
    void __fastcall btnIPHClick(TObject *Sender);
    void __fastcall btnQueryClick(TObject *Sender);
    void __fastcall btnSysClick(TObject *Sender);
    void __fastcall btnTestClick(TObject *Sender);
    void __fastcall btnToolbarMouseEnter(TObject *Sender);
    void __fastcall btnToolbarMouseLeave(TObject *Sender);
    void __fastcall btnBackPageClick(TObject *Sender);
    void __fastcall btnLocalCfgClick(TObject *Sender);
    void __fastcall TimerPTTTimer(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall btnLeftToolbarMouseEnter(TObject *Sender);
    void __fastcall btnLeftToolbarMouseLeave(TObject *Sender);
    void __fastcall btnMicStatusClick(TObject *Sender);
    void __fastcall SliderMicStateChanged(TObject *Sender, TAdvSmoothSliderState State,
          double Value);
    void __fastcall TimerAutoLockTimer(TObject *Sender);
    void __fastcall pmiLockWindowClick(TObject *Sender);
    void __fastcall pmiLogoutClick(TObject *Sender);
    void __fastcall pmiSetPwdClick(TObject *Sender);
    void __fastcall PanelWorkMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall PanelStatusBarMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall TimerGetCfgVerTimer(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall TimerResizeTimer(TObject *Sender);
    void __fastcall btnDepotTrainClick(TObject *Sender);


private:    // User declarations
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(EV_CONNECTION_CHANGE_MSG, TMessage, OnClientConnectionChangeMsg)
        MESSAGE_HANDLER(WM_INNER_MSG, TMessage, OnInnerMsg)
    END_MESSAGE_MAP(TForm)

    void __fastcall OnClientConnectionChangeMsg(TMessage Message);

    virtual void __fastcall InitSkin();
    void __fastcall OnGetESDKEvent(UINT8 *pucMsg, UINT32 ulMsgLen);
    void __fastcall OnGetAPPMsg(UINT8 *pucMsg, UINT32 ulMsgLen);
    void __fastcall OnAppRegisterReq(GJson &Json, UINT32 ulSeqID);
    void __fastcall OnAppUnregisterReq(GJson &Json, UINT32 ulSeqID);
    void __fastcall OnGetCfgDataVerRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnLoadCfgDataRsp(UINT32 ulSeqID, GJson &Json);

    void __fastcall OnDCRegisterDISRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnDISKickOffDCReq(UINT32 ulSeqID, GJson &Json);

    void __fastcall InitAllCtrl();
    void __fastcall ShowCurrentTime();

    void __fastcall StartESDKLocalService();
    TcxButton* __fastcall GetToolbarButton(PAGE_E enPage);
    PAGE_E __fastcall GetPageID(TdxBarLargeButton *btn);

    void __fastcall SetMDCStatus(bool bConnected);
    void __fastcall CheckSpeakMic(BOOL bForce);
    void __fastcall HandleException(TObject *Sender, Exception *E);

    void __fastcall MyOnMessage(tagMSG &Msg, bool &Handled);
    void __fastcall InitCtrlPos();

public:     // User declarations
    __fastcall TFormMain(TComponent* Owner);

    void __fastcall ActivePage(PAGE_E enPage);
    void __fastcall BackPage();
    void __fastcall UpdateATSStatus(BOOL bValid);
    void __fastcall SetStatusButtonStatus(TAdvSmoothPanel *btn, bool bConnected);

    void __fastcall OnNativeSrvStatus(BOOL bValid);
    void eSDKEventCallBack(INT32 iEventType, char *szEvent, UINT32 ulSize);

    void __fastcall OnConfirmSaveRsp(UINT32 ulSeqID, GJson &Json);

    void __fastcall Exit();
    void __fastcall LockWindow();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;

void SetFormParent(TForm *Child, TWinControl *Parent);
void UpdateMainGroupCall(INT64 i64GroupID=INVALID_GROUP_ID);
void ShowGroupCallWin(int x, int y, INT64 i64GroupID, AnsiString strGroupName, BOOL bIsPatchGroup=FALSE);
void ShowGroupCallWin(int x, int y, INT64 i64GroupID);
void ShowGroupCallWin(INT64 i64GroupID, AnsiString strGroupName, BOOL bIsPatchGroup=FALSE);
void ShowGroupCallWin(INT64 i64GroupID);

//---------------------------------------------------------------------------
#endif
