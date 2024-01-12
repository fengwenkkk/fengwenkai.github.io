//---------------------------------------------------------------------------

#ifndef FrmDevFileH
#define FrmDevFileH
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
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxImageComboBox.hpp"
#include "cxImageList.hpp"
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include "cxCheckBox.hpp"
#include "cxCalendar.hpp"
#include "cxDateUtils.hpp"
#include "cxMaskEdit.hpp"
#include "dxCore.hpp"
#include <Vcl.ComCtrls.hpp>
#include "cxClasses.hpp"

//---------------------------------------------------------------------------
class TFormDevFile : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbMain;
    TcxTreeList *tlMain;
    TcxTreeListColumn *ColumnNeID;
    TcxTreeListColumn *ColumnFileName;
    TcxGroupBox *gbCtrl;
    TcxButton *btnQuery;
    TOpenDialog *OpenDialog;
    TcxTreeListColumn *ColumnFileSize;
    TcxTreeListColumn *ColumnFileType;
    TcxTreeListColumn *ColumnFileTime;
    TcxButton *btnExport;
    TLabel *Label1;
    TcxComboBox *cbDev;
    TcxLabel *cxLabel1;
    TcxDateEdit *deStartTime;
    TcxLabel *cxLabel2;
    TcxDateEdit *deEndTime;
    TcxButton *btnQueryFile;
    TcxButton *btnPlay;
    TLabel *Label2;
    TcxComboBox *cbFileType;
    TLabel *Label3;
    TcxComboBox *cbDevType;
    TcxButton *btnDownload;
    TcxTreeListColumn *ColumnIndex;
    TcxTreeListColumn *ColumnState;
    TdxBarManager *BarManager;
    TdxBarButton *pmiDownLoad;
    TdxBarButton *pmiExport;
    TdxBarButton *pmiPlay;
    TdxBarButton *pmiBrowser;
	TdxBarPopupMenu *pMain;
	TTimer *TimerRefreshFileList;
	TSaveDialog *SaveDialog;
    void __fastcall btnQueryClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall ColumnFileTypeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall cbDevTypePropertiesChange(TObject *Sender);
    void __fastcall btnDownloadClick(TObject *Sender);
    void __fastcall ColumnStateGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall btnExportClick(TObject *Sender);
    void __fastcall btnQueryFileClick(TObject *Sender);
    void __fastcall tlMainFocusedNodeChanged(TcxCustomTreeList *Sender, TcxTreeListNode *APrevFocusedNode,
          TcxTreeListNode *AFocusedNode);
    void __fastcall btnPlayClick(TObject *Sender);
    void __fastcall tlMainSelectionChanged(TObject *Sender);
	void __fastcall TimerRefreshFileListTimer(TObject *Sender);
private:    // User declarations
    virtual void __fastcall InitSkin(){};
	AnsiString __fastcall DownloadCurrentFile();
	AnsiString  m_strNeID;
	AnsiString  m_strFile;

public:     // User declarations

    __fastcall TFormDevFile(TComponent* Owner);
    void __fastcall UpdateDevList();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormDevFile *FormDevFile;
//---------------------------------------------------------------------------
#endif
