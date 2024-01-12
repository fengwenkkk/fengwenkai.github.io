//---------------------------------------------------------------------------

#ifndef FrmRecManageH
#define FrmRecManageH
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
#include "cxSplitter.hpp"
#include "FolderDialog.hpp"
#include <Vcl.FileCtrl.hpp>

//---------------------------------------------------------------------------
class TFormRecManage : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbMain;
    TcxGroupBox *gbCtrl;
    TcxButton *btnQuery;
    TcxLabel *cxLabel1;
    TcxDateEdit *deStartTime;
    TcxLabel *cxLabel2;
    TcxDateEdit *deEndTime;
    TcxLabel *cxLabel3;
    TSaveDialog *SaveDialog;
    TcxGroupBox *gbPlay;
    TPanel *PanelPlay;
    TcxButton *btnPlay;
    TcxImageList *ImageListPlay;
    TcxButton *btnPause;
    TcxButton *btnStop;
    TdxBarManager *BarManager;
    TdxBarPopupMenu *pmMain;
    TcxImageList *ImageListRecType;
    TdxBarSeparator *dxBarSeparator1;
    TdxBarButton *pmiPlay;
    TdxBarButton *pmiExport;
    TdxSkinController *dxSkinController1;
    TPanel *PanelPlayCtrl;
    TPanel *Panel1;
    TcxComboBox *cbRecType;
    TTimer *TimerPlay;
    TTimer *TimerInit;
    TPanel *PanelMediaPlayer;
    TcxButton *btnMute;
    TcxButton *btnVolume;
    TcxTrackBar *tbVolume;
    TcxGroupBox *gbTrackBar;
    TcxLabel *LabelMediaLength;
    TcxTrackBar *tbPlay;
    TWindowsMediaPlayer *WindowsMediaPlayer;
    TPanel *PanelSide1;
    TPanel *PanelSide2;
    TcxTreeList *tlMain;
    TcxTreeListColumn *ColumnIndex;
    TcxTreeListColumn *ColumnTime;
    TcxTreeListColumn *ColumnType;
    TcxTreeListColumn *ColumnFile;
    TcxTreeListColumn *ColumnResource;
    TcxTreeListColumn *ColumnCaller;
    TcxTreeListColumn *ColumnCallee;
    TcxButton *btnQueryPrev;
    TcxButton *btnQueryNext;
    TcxLabel *LabelResource;
    TcxTextEdit *edtResourceID;
    TcxTreeListColumn *ColumnDuration;
    TTimer *TimerCheck;
    TcxSplitter *Splitter;
    TFolderDialog *FolderDialog;
    TcxTreeListColumn *ColumnStatus;
    TdxBarButton *pmiExportAll;
    TdxBarSeparator *dxBarSeparator2;
    TcxTreeListColumn *ColumnIPH;
    void __fastcall btnQueryClick(TObject *Sender);
    void __fastcall tlMainDblClick(TObject *Sender);
    void __fastcall btnPlayClick(TObject *Sender);
    void __fastcall pmiPlayClick(TObject *Sender);
    void __fastcall pmiExportClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall btnPauseClick(TObject *Sender);
    void __fastcall btnStopClick(TObject *Sender);
    void __fastcall TimerPlayTimer(TObject *Sender);
    void __fastcall PanelPlayResize(TObject *Sender);
    void __fastcall tbPlayMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall TimerInitTimer(TObject *Sender);
    void __fastcall tbPlayMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall tbVolumePropertiesChange(TObject *Sender);
    void __fastcall btnVolumeClick(TObject *Sender);
    void __fastcall btnMuteClick(TObject *Sender);
    void __fastcall WindowsMediaPlayerPlayStateChange(TObject *Sender, long NewState);
    void __fastcall tbPlayPropertiesGetPositionHint(TObject *Sender, const int APosition,
          UnicodeString &AHintText, bool &ACanShow, bool &AIsHintMultiLine);
    void __fastcall PanelMediaPlayerResize(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall btnQueryPrevClick(TObject *Sender);
    void __fastcall btnQueryNextClick(TObject *Sender);
    void __fastcall cbRecTypePropertiesChange(TObject *Sender);
    void __fastcall ColumnFileGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall TimerCheckTimer(TObject *Sender);
    void __fastcall pmiExportAllClick(TObject *Sender);
    void __fastcall PanelPlayCtrlResize(TObject *Sender);
    void __fastcall ColumnResourceGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnCallerGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnCalleeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnDurationGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);

private:    // User declarations
    char    m_acTmpRecPath[256];

    virtual void __fastcall InitSkin();

    void __fastcall ExportMultiRecFile(bool bExportAll);
    void __fastcall InitView(UINT32 ulRecType, VECTOR<REC_FILE_INFO_T> &vInfo);
    void __fastcall PlayRecFile(AnsiString strRtspUrl);
  //    void __fastcall ShowLocalRec(VECTOR<LOCAL_REC_INFO_T> &vInfo);
    bool __fastcall DownloadFile(AnsiString &strLocalFile, AnsiString &strFileType);
  //  void __fastcall OnGetRecFileListRsp(UINT32 ulSeqID, GJson &Json);
public:     // User declarations

    __fastcall TFormRecManage(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TFormRecManage *FormRecManage;
//---------------------------------------------------------------------------

#endif
