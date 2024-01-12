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

#include "MakeUtil.h"
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdGlobal.hpp>
#include <IdSocketHandle.hpp>
#include <IdUDPBase.hpp>
#include <IdUDPServer.hpp>
#include <IdUDPClient.hpp>
#include "trayicon.h"

#define EV_INNER_MAKE_MSG               (WM_INNER_MSG + 1)

//---------------------------------------------------------------------------

class TFormMain : public TSubForm
{
__published:    // IDE-managed Components
    TPanel *PanelTop;
    TcxTreeList *tlSourceFile;
    TcxLabel *Label1;
    TcxButton *btnStop;
    TTimer *TimerCheckTask;
    TcxTreeListColumn *ColumnFile;
    TcxTreeListColumn *ColumnState;
    TcxButton *btnOpen;
    TFileOpenDialog *OpenDialog;
    TcxButton *btnClear;
    TcxMemo *mmInfo;
    TcxTreeListColumn *ColumnFile2;
    TcxTreeListColumn *ColumnState2;
    TPanel *Panel1;
    TcxSplitter *cxSplitter1;
    TcxLabel *cxLabel2;
    TcxProgressBar *pbMake;
    TdxSkinController *dxSkinController1;
    TcxButton *btnRebuild;
    TcxTreeListColumn *tlSourceFileColumn5;
    TcxTreeListColumn *tlSourceFileColumn6;
    TTimer *TimerQueryMake;
    TcxButton *btnStart;
    TTimer *TimerInit;
    TTimer *TimerCheckObj;
    TcxButtonEdit *edtProject;
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall TimerCheckTaskTimer(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall btnStopClick(TObject *Sender);
    void __fastcall btnOpenClick(TObject *Sender);
    void __fastcall ColumnStateGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall tlSourceFileCustomDrawDataCell(TcxCustomTreeList *Sender, TcxCanvas *ACanvas,
          TcxTreeListEditCellViewInfo *AViewInfo, bool &ADone);
    void __fastcall btnClearClick(TObject *Sender);
    void __fastcall btnRebuildClick(TObject *Sender);
    void __fastcall TimerQueryMakeTimer(TObject *Sender);
    void __fastcall btnStartClick(TObject *Sender);
    void __fastcall TimerInitTimer(TObject *Sender);
    void __fastcall TrayIconMinimize(TObject *Sender);
    void __fastcall TrayIconRestore(TObject *Sender);
    void __fastcall tlSourceFileFocusedNodeChanged(TcxCustomTreeList *Sender, TcxTreeListNode *APrevFocusedNode,
          TcxTreeListNode *AFocusedNode);
    void __fastcall tlSourceFileFocusedColumnChanged(TcxCustomTreeList *Sender, TcxTreeListColumn *APrevFocusedColumn,
          TcxTreeListColumn *AFocusedColumn);
    void __fastcall TimerCheckObjTimer(TObject *Sender);
    void __fastcall edtProjectPropertiesButtonClick(TObject *Sender, int AButtonIndex);



private:    // User declarations
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(EV_INNER_MAKE_MSG, TMessage, OnInnerMakeMsg)
        MESSAGE_HANDLER(WM_INNER_MSG, TMessage, OnInnerMsg)
    END_MESSAGE_MAP(TForm)

    void __fastcall OnInnerMakeMsg(TMessage Message);

    char    m_acProjectPath[1024];

    void __fastcall OnInnerMsg(TMessage Message);

    void __fastcall LogBuilderInfo(BUILD_INFO_T *pstInfo);
    void __fastcall Log(AnsiString strText);

    void __fastcall MakeDone();
    UINT32 __fastcall GetFileState(char *szFile);
    TcxTreeListNode* __fastcall GetUnbuildSourceFile(UINT32 &ulColIndex);

    void __fastcall ShowMakeResult(AnsiString strFile, char *szResult, UINT32 ulResult);

    bool __fastcall InitBuilderPara();
    bool __fastcall InitSourceFile(char *szFile, bool bRebuild);

  //    void __fastcall SendPeer(IP_ADDR_T *pstPeerAddr, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);
    void __fastcall SendAllPeer(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);

    void __fastcall OnPeerMsg(IP_ADDR_T *pstPeerAddr, UINT16 usMsgID, UINT8 *pucMsg, UINT32 ulMsgLen);

    void __fastcall OnGetMakeTaskReq(IP_ADDR_T *pstPeerAddr);
    void __fastcall OnGetMakeTaskRsp(IP_ADDR_T *pstPeerAddr, GET_MAKE_TASK_RSP_T *pstMsg);

    void __fastcall OnStopMakeTaskReq(IP_ADDR_T *pstPeerAddr, STOP_MAKE_TASK_REQ_T *pstMsg);

    void __fastcall OnAllocMakeFileReq(IP_ADDR_T *pstPeerAddr, ALLOC_MAKE_FILE_REQ_T *pstMsg);
    void __fastcall OnAllocMakeFileRsp(IP_ADDR_T *pstPeerAddr, ALLOC_MAKE_FILE_RSP_T *pstMsg);
    void __fastcall OnReportMakeInfoReq(IP_ADDR_T *pstPeerAddr, REPORT_MAKE_INFO_REQ_T *pstMsg);

    void __fastcall AllocMakeFileReq();
    void __fastcall InitTrayIcon();
    void __fastcall ShowMakeResult();
    void __fastcall LoadProjectFile(char *szFile);

public:     // User declarations
    __fastcall TFormMain(TComponent* Owner);

    virtual void __fastcall InitSkin();

};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif
