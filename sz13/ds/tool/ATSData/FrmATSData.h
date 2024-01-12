//---------------------------------------------------------------------------

#ifndef FrmATSDataH
#define FrmATSDataH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
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
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include "cxContainer.hpp"
#include "cxControls.hpp"
#include "cxDropDownEdit.hpp"
#include "cxEdit.hpp"
#include "cxMaskEdit.hpp"
#include "cxTextEdit.hpp"
#include "cxCustomData.hpp"
#include "cxInplaceContainer.hpp"
#include "cxStyles.hpp"
#include "cxTL.hpp"
#include "cxTLdxBarBuiltInMenu.hpp"
#include "dxSkinsForm.hpp"
#include "cxPC.hpp"
#include "cxPCdxBarPopupMenu.hpp"
#include "dxSkinscxPCPainter.hpp"
#include "cxGroupBox.hpp"
#include "MDIForm.h"
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
#include "cxCheckBox.hpp"
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
#include "cxSplitter.hpp"
#include "cxLabel.hpp"
#include "dxBarBuiltInMenu.hpp"
#include "AdvGrid.hpp"
#include "AdvObj.hpp"
#include "AdvUtil.hpp"
#include "BaseGrid.hpp"
#include <Vcl.Grids.hpp>
#include "cxButtonEdit.hpp"
#include <Vcl.Dialogs.hpp>
//---------------------------------------------------------------------------

class TFormATSData : public TSubForm
{
__published:    // IDE-managed Components
    TdxSkinController *dxSkinController1;
    TcxTreeList *tlATS;
    TcxTreeListColumn *Column1;
    TcxTreeListColumn *Column2;
    TcxTreeListColumn *Column3;
    TcxTreeListColumn *Column4;
    TcxTreeListColumn *Column5;
    TcxTreeListColumn *Column6;
    TcxTreeListColumn *Column7;
    TcxTreeListColumn *Column8;
    TcxTreeListColumn *Column9;
    TcxTreeListColumn *Column10;
    TcxTreeListColumn *Column11;
    TcxTreeListColumn *Column12;
    TcxTreeListColumn *Column13;
    TcxTreeListColumn *Column14;
    TcxTreeListColumn *Column15;
    TcxTreeListColumn *Column16;
    TcxSplitter *cxSplitter1;
    TcxTreeListColumn *Column0;
    TcxPageControl *pcMain;
    TcxTabSheet *tsShowFile;
    TcxTabSheet *tsRealData;
    TPanel *Panel3;
    TcxButton *btnRefresh;
    TcxLabel *LabelInfo;
    TPanel *Panel1;
    TcxLabel *LabelATS;
    TLabel *Label1;
    TcxTextEdit *edtATSAddr;
    TTimer *TimerATS;
    TPanel *PanelLeft;
    TcxTreeList *tlRec;
    TcxTreeListColumn *tlRecColumn10;
    TcxTreeListColumn *cxTreeListColumn1;
    TcxTreeListColumn *cxTreeListColumn2;
    TcxTreeListColumn *cxTreeListColumn3;
    TcxTreeListColumn *cxTreeListColumn4;
    TcxTreeListColumn *cxTreeListColumn5;
    TcxTreeListColumn *cxTreeListColumn6;
    TcxTreeListColumn *tlRecColumn7;
    TcxTreeListColumn *tlRecColumn8;
    TAdvStringGrid *sgRec;
    TcxButton *btnSet;
    TcxCheckBox *cbPause;
    TLabel *Label2;
    TcxButtonEdit *edtFile;
    TFileOpenDialog *OpenDialog;
    TcxCheckBox *cbShowHex;
    TcxCheckBox *cbFilterRec;
	TcxTreeListColumn *tlATSColumn18;
	TcxTreeListColumn *tlATSColumn19;
    TcxButton *btnExportRec;
    void __fastcall btnRefreshClick(TObject *Sender);
    void __fastcall tlATSGetCellHint(TcxCustomTreeList *Sender, TObject *ACell, UnicodeString &AText,
          bool &ANeedShow);
    void __fastcall cbShowHexPropertiesChange(TObject *Sender);
    void __fastcall tlATSGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall tlRecFocusedNodeChanged(TcxCustomTreeList *Sender, TcxTreeListNode *APrevFocusedNode,
          TcxTreeListNode *AFocusedNode);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall TimerATSTimer(TObject *Sender);
    void __fastcall btnSetClick(TObject *Sender);
    void __fastcall pcMainPageChanging(TObject *Sender, TcxTabSheet *NewPage, bool &AllowChange);
    void __fastcall sgRecRowChanging(TObject *Sender, int OldRow, int NewRow, bool &Allow);
    void __fastcall edtFilePropertiesButtonClick(TObject *Sender, int AButtonIndex);
    void __fastcall sgRecClickCell(TObject *Sender, int ARow, int ACol);
    void __fastcall btnExportRecClick(TObject *Sender);


private:    // User declarations
    UINT32                  m_ulFrameNum;
    UINT32                  m_ulLastFrameIndex;     // 最后一次接收到的FrameIndex
    UINT32                  m_ulRecvMsgLen;
    UINT8                   m_aucRecvBuf[64*1024];

    void __fastcall SaveATSInfo();
    void __fastcall ShowATSInfo(ATS_REC_T *pstRec);
    void __fastcall ShowRealATSRec(ATS_REC_T &stRec);
    void __fastcall ShowATSRec(ATS_REC_T *pstRec, int iIndex);
    INT32 __fastcall OnATSMsg(ATS_MSG_HDR_T *pstMsgHdr, INT32 iMsgDataLen, UINT8 *pucAddr);

public:     // User declarations
    __fastcall TFormATSData(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormATSData *FormATSData;
//---------------------------------------------------------------------------
#endif
