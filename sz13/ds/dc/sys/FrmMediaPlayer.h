//---------------------------------------------------------------------------

#ifndef FrmMediaPlayerH
#define FrmMediaPlayerH
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
#include "cxSpinEdit.hpp"
#include "cxTimeEdit.hpp"
#include "cxImageList.hpp"
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.MPlayer.hpp>
#include "cxClasses.hpp"
#include "cxImageComboBox.hpp"
#include "dxSkinsForm.hpp"
#include "cxTrackBar.hpp"

#include "WMPLib_OCX.h"
#include <Vcl.OleCtrls.hpp>
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxImage.hpp"
#include "dxGDIPlusClasses.hpp"
#include "cxSplitter.hpp"

//---------------------------------------------------------------------------
class TFormMediaPlayer : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbPlay;
    TPanel *PanelPlay;
    TcxImageList *ImageListPlay;
    TdxBarManager *BarManager;
    TdxBarSeparator *dxBarSeparator1;
    TdxBarButton *pmiPlay;
    TdxBarButton *pmiExport;
    TTimer *TimerPlay;
    TPanel *PanelMediaPlayer;
    TcxGroupBox *gbTrackBar;
    TcxLabel *LabelMediaLength;
    TcxTrackBar *tbPlay;
    TWindowsMediaPlayer *WindowsMediaPlayer;
    TPanel *PanelSide1;
    TPanel *PanelSide2;
    TcxButton *btnPlay;
    TcxButton *btnPause;
    TcxButton *btnStop;
    TcxSplitter *SplitterMain;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall btnPauseClick(TObject *Sender);
    void __fastcall btnStopClick(TObject *Sender);
    void __fastcall TimerPlayTimer(TObject *Sender);
    void __fastcall tbPlayMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall tbPlayMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall WindowsMediaPlayerPlayStateChange(TObject *Sender, long NewState);
    void __fastcall tbPlayPropertiesGetPositionHint(TObject *Sender, const int APosition,
          UnicodeString &AHintText, bool &ACanShow, bool &AIsHintMultiLine);
    void __fastcall PanelMediaPlayerResize(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall btnPlayClick(TObject *Sender);

private:    // User declarations
    bool m_bChangingPos;
    String  m_strFile;

    virtual void __fastcall InitSkin();

public:     // User declarations
    __fastcall TFormMediaPlayer(TComponent* Owner);

    void __fastcall SetFile(AnsiString strFile);
    void __fastcall Play();
    void __fastcall Stop();

};
//---------------------------------------------------------------------------
extern PACKAGE TFormMediaPlayer *FormMediaPlayer;
//---------------------------------------------------------------------------

#endif
