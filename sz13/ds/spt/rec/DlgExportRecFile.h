// ---------------------------------------------------------------------------

#ifndef DlgExportRecFileH
#define DlgExportRecFileH
// ---------------------------------------------------------------------------
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
#include "dxBevel.hpp"
#include "cxCustomData.hpp"
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxInplaceContainer.hpp"
#include "cxStyles.hpp"
#include "cxTL.hpp"
#include "cxTLdxBarBuiltInMenu.hpp"
#include "cxDropDownEdit.hpp"
#include "cxMaskEdit.hpp"
#include "cxCheckBox.hpp"
#include "cxImageComboBox.hpp"
#include "cxImageList.hpp"
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include "FolderDialog.hpp"

#define BRD_STATUS_WAIT     1
#define BRD_STATUS_READY    2
#define BRD_STATUS_FAIL     3

typedef struct
{
    TRAIN_INFO_T    *pstTrain;
//  UINT32  ulTrainUnitID;
//  CHAR    acTrainID[16];
    UINT32  ulUserID1;
    UINT32  ulUserID2;
    UINT32  ulBrdStatus1;
    UINT32  ulBrdStatus2;
}TRAIN_BRD_INFO_T;

// ---------------------------------------------------------------------------
class TDialogExportRecFile : public TDialogForm
{
__published: // IDE-managed Components
    TcxButton *btnEnd;
    TdxBevel *dxBevel1;
    TcxProgressBar *pbFile;
    TcxTreeList *tlMain;
    TcxTreeListColumn *ColumnFile;
    TcxTreeListColumn *ColumnIndex;
    TcxLabel *cxLabel1;
    TcxButton *btnStart;
    TcxTreeListColumn *tlBrdInfoColumn7;
    TcxLabel *cxLabel2;
    TcxLabel *cxLabel3;
    TcxTextEdit *edtPath;
    TcxButton *btnPath;
    TFolderDialog *FolderDialog;
    void __fastcall btnEndClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall btnStartClick(TObject *Sender);
    void __fastcall btnPathClick(TObject *Sender);
    void __fastcall ColumnFileGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);

private: // User declarations
    VECTOR<DOWNLOAD_FILE_INFO_T>    m_vInfo;
    UINT32      m_ulCurrDownloadIndex;
    CHAR        m_acExportPath[256];

    virtual void __fastcall InitSkin();

    void __fastcall Stop();
    void __fastcall OnDownloadInd(UINT8 *pucMsg, UINT32 ulMsgLen);
    void __fastcall DownloadFile();

public: // User declarations
    __fastcall TDialogExportRecFile(TComponent* Owner);

    bool __fastcall Init(VECTOR<DOWNLOAD_FILE_INFO_T> &vInfo);
    void __fastcall Get(VECTOR<DOWNLOAD_FILE_INFO_T> &vInfo);
};

// ---------------------------------------------------------------------------
extern PACKAGE TDialogExportRecFile *DialogExportRecFile;

// ---------------------------------------------------------------------------
#endif
