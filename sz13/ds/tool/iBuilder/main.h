//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "cxControls.hpp"
#include "cxGraphics.hpp"
#include "cxLookAndFeelPainters.hpp"
#include "cxLookAndFeels.hpp"
#include "cxPC.hpp"
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
#include "dxTabbedMDI.hpp"
#include "cxClasses.hpp"
#include "cxCustomData.hpp"
#include "cxData.hpp"
#include "cxDataStorage.hpp"
#include "cxDBData.hpp"
#include "cxEdit.hpp"
#include "cxFilter.hpp"
#include "cxGrid.hpp"
#include "cxGridCustomTableView.hpp"
#include "cxGridCustomView.hpp"
#include "cxGridDBTableView.hpp"
#include "cxGridLevel.hpp"
#include "cxGridTableView.hpp"
#include "cxStyles.hpp"
#include "dxRibbon.hpp"
#include "dxRibbonSkins.hpp"
#include "dxSkinsForm.hpp"
#include <DB.hpp>
#include "dxLayoutContainer.hpp"
#include "dxLayoutControl.hpp"
#include "dxLayoutLookAndFeels.hpp"
#include <Grids.hpp>
#include "cxInplaceContainer.hpp"
#include "cxTL.hpp"
#include "cxTLdxBarBuiltInMenu.hpp"
#include "cxDBTL.hpp"
#include "cxTLData.hpp"
#include "cxMaskEdit.hpp"
#include "cxContainer.hpp"
#include "cxTreeView.hpp"
#include <ComCtrls.hpp>
#include "cxTextEdit.hpp"
#include <MPlayer.hpp>
#include "cxButtons.hpp"
#include <Menus.hpp>
#include "dxBar.hpp"
#include "dxBarExtItems.hpp"
#include "dxSkinsdxBarPainter.hpp"
#include "dxNavBar.hpp"
#include "dxNavBarBase.hpp"
#include "dxNavBarCollns.hpp"
#include "dxSkinsdxNavBarPainter.hpp"
#include "dxNavBarStyles.hpp"
#include "cxButtonEdit.hpp"
#include "cxCalc.hpp"
#include "cxHyperLinkEdit.hpp"
#include "cxListView.hpp"
#include "cxShellCommon.hpp"
#include "cxShellTreeView.hpp"
#include <ShlObj.hpp>
#include <ImgList.hpp>
#include "cxBarEditItem.hpp"
#include "cxCheckBox.hpp"
#include "cxSplitter.hpp"
#include "cxDropDownEdit.hpp"
#include "cxLabel.hpp"
#include "MDIForm.h"
#include "cxGroupBox.hpp"
#include "cxPropertiesStore.hpp"
#include "cxProgressBar.hpp"
#include "cxTrackBar.hpp"
#include "cxMemo.hpp"
#include <Dialogs.hpp>
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
#include "dxTaskbarProgress.hpp"
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxDrawTextUtils.hpp"
#include "dxBkgnd.hpp"
#include "dxPrnDev.hpp"
#include "dxPrnPg.hpp"
#include "dxPSCompsProvider.hpp"
#include "dxPSCore.hpp"
#include "dxPScxEditorProducers.hpp"
#include "dxPScxExtEditorProducers.hpp"
#include "dxPScxPageControlProducer.hpp"
#include "dxPSEdgePatterns.hpp"
#include "dxPSEngn.hpp"
#include "dxPSFillPatterns.hpp"
#include "dxPSGlbl.hpp"
#include "dxPSPDFExport.hpp"
#include "dxPSPDFExportCore.hpp"
#include "dxPSPrVwAdv.hpp"
#include "dxPSPrVwRibbon.hpp"
#include "dxPSPrVwStd.hpp"
#include "dxPSUtl.hpp"
#include "dxWrap.hpp"
//---------------------------------------------------------------------------

class TFormMain : public TSubForm
{
__published:    // IDE-managed Components
    TPanel *PanelTop;
    TcxButton *btnStart;
    TcxTreeList *tlSourceFile;
    TcxLabel *Label1;
    TcxTextEdit *edtProject;
    TcxButton *btnStop;
    TTimer *tmrCheck;
    TcxTreeListColumn *ColumnFile;
    TcxTreeListColumn *ColumnState;
    TcxButton *btnOpen;
    TOpenDialog *dlgOpen;
    TFileOpenDialog *OpenDialog;
    TcxButton *btnClear;
    TcxLabel *cxLabel1;
    TcxMemo *mmInfo;
    TcxTreeListColumn *ColumnFile2;
    TcxTreeListColumn *ColumnState2;
    TPanel *Panel1;
    TcxSplitter *cxSplitter1;
    TcxLabel *cxLabel2;
    TcxProgressBar *pbBuild;
    TdxSkinController *dxSkinController1;
    TcxButton *btnRebuild;
    TcxTreeListColumn *tlSourceFileColumn5;
    TcxTreeListColumn *tlSourceFileColumn6;
    TdxTaskbarProgress *tpBuild;
    TcxComboBox *cbCpuNum;
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall btnStartClick(TObject *Sender);
    void __fastcall tmrCheckTimer(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall btnStopClick(TObject *Sender);
    void __fastcall btnOpenClick(TObject *Sender);
    void __fastcall ColumnStateGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall tlSourceFileCustomDrawDataCell(TcxCustomTreeList *Sender, TcxCanvas *ACanvas,
          TcxTreeListEditCellViewInfo *AViewInfo, bool &ADone);
    void __fastcall btnClearClick(TObject *Sender);
    void __fastcall tlSourceFileClick(TObject *Sender);
    void __fastcall btnRebuildClick(TObject *Sender);
    void __fastcall tlSourceFileKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall tlSourceFileKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall tlSourceFileDblClick(TObject *Sender);



private:    // User declarations
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_BUILD_INNER_MSG, TMessage, OnInnerMsg)
    END_MESSAGE_MAP(TForm)

    char    m_acProjectPath[1024];

    void __fastcall OnInnerMsg(TMessage Message);

    void __fastcall LogBuilderInfo(BUILD_INFO_T *pstInfo);
    void __fastcall Log(AnsiString strText);

    UINT32 __fastcall GetFileState(char *szFile);
    TcxTreeListNode* __fastcall GetUnbuildSourceFile(UINT32 &ulColIndex);

    UINT32 __fastcall GetThreadNum();

    bool __fastcall InitBuilderPara();
    bool __fastcall InitSourceFile(bool bRebuild);

public:     // User declarations
    __fastcall TFormMain(TComponent* Owner);

    virtual void __fastcall InitSkin();

    void __fastcall MyOnMessage(tagMSG &Msg, bool &Handled);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif
