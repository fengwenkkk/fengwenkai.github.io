//---------------------------------------------------------------------------

#ifndef ResourceH
#define ResourceH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "cxContainer.hpp"
#include "cxControls.hpp"
#include "cxEdit.hpp"
#include "cxGraphics.hpp"
#include "cxImage.hpp"
#include "cxImageList.hpp"
#include "cxLookAndFeelPainters.hpp"
#include "cxLookAndFeels.hpp"
#include "dxGDIPlusClasses.hpp"
#include "dxSkinBlack.hpp"
#include "dxSkinBlue.hpp"
#include "dxSkinBlueprint.hpp"
#include "dxSkinCaramel.hpp"
#include "dxSkinCoffee.hpp"
#include "dxSkinDarkRoom.hpp"
#include "dxSkinDarkSide.hpp"
#include "dxSkinDevExpressDarkStyle.hpp"
#include "dxSkinDevExpressStyle.hpp"
#include "dxSkinFoggy.hpp"
#include "dxSkinGlassOceans.hpp"
#include "dxSkinHighContrast.hpp"
#include "dxSkiniMaginary.hpp"
#include "dxSkinLilian.hpp"
#include "dxSkinLiquidSky.hpp"
#include "dxSkinLondonLiquidSky.hpp"
#include "dxSkinMcSkin.hpp"
#include "dxSkinMetropolis.hpp"
#include "dxSkinMetropolisDark.hpp"
#include "dxSkinMoneyTwins.hpp"
#include "dxSkinOffice2007Black.hpp"
#include "dxSkinOffice2007Blue.hpp"
#include "dxSkinOffice2007Green.hpp"
#include "dxSkinOffice2007Pink.hpp"
#include "dxSkinOffice2007Silver.hpp"
#include "dxSkinOffice2010Black.hpp"
#include "dxSkinOffice2010Blue.hpp"
#include "dxSkinOffice2010Silver.hpp"
#include "dxSkinOffice2013DarkGray.hpp"
#include "dxSkinOffice2013LightGray.hpp"
#include "dxSkinOffice2013White.hpp"
#include "dxSkinOffice2016Colorful.hpp"
#include "dxSkinOffice2016Dark.hpp"
#include "dxSkinPumpkin.hpp"
#include "dxSkinsCore.hpp"
#include "dxSkinsDefaultPainters.hpp"
#include "dxSkinSeven.hpp"
#include "dxSkinSevenClassic.hpp"
#include "dxSkinSharp.hpp"
#include "dxSkinSharpPlus.hpp"
#include "dxSkinSilver.hpp"
#include "dxSkinSpringTime.hpp"
#include "dxSkinStardust.hpp"
#include "dxSkinSummer2008.hpp"
#include "dxSkinTheAsphaltWorld.hpp"
#include "dxSkinTheBezier.hpp"
#include "dxSkinValentine.hpp"
#include "dxSkinVisualStudio2013Blue.hpp"
#include "dxSkinVisualStudio2013Dark.hpp"
#include "dxSkinVisualStudio2013Light.hpp"
#include "dxSkinVS2010.hpp"
#include "dxSkinWhiteprint.hpp"
#include "dxSkinXmas2008Blue.hpp"
#include <System.ImageList.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ImgList.hpp>
#include "cxClasses.hpp"
#include "cxDrawTextUtils.hpp"
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
#include "cxCustomData.hpp"
#include "cxInplaceContainer.hpp"
#include "cxStyles.hpp"
#include "cxTL.hpp"
#include "cxTLdxBarBuiltInMenu.hpp"
#include "cxLocalization.hpp"
#include "cxMemo.hpp"
#include "cxTextEdit.hpp"
#include "cxButtons.hpp"
#include <Vcl.Menus.hpp>
#include "cxHint.hpp"
#include "dxCustomHint.hpp"
#include "dxScreenTip.hpp"
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
//#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"

typedef enum
{
    IMG_OK,
    IMG_CANCEL,
    IMG_STOP,
    IMG_CLOSE,
    IMG_EXIT,
    IMG_HIDE,

    IMG_PRINT,
    IMG_QUERY,
    IMG_IMPORT,
    IMG_EXPORT,

    IMG_PLAY,
    IMG_PAUSE,
    IMG_RECV,
    IMG_DOWNLOAD,
    IMG_NEXT,
    IMG_PREV,

    IMG_PTT_DIAL,
    IMG_ACCEPT_CALL,
    IMG_REJECT_CALL,
    IMG_P2P_CALL,
    IMG_HANGOFF,
    IMG_HANGUP,

    IMG_START_LISTEN,
    IMG_STOP_LISTEN,

    IMG_SDS,
    IMG_SDS_REPLY,
    IMG_BRD,

    IMG_ALL_LINE,
    IMG_UP_LINE,
    IMG_DOWN_LINE,

    IMG_ADD,
    IMG_DEL,
    IMG_SET,
    IMG_REFRESH,
    IMG_SWITCH,     // ÇÐ»»

    IMG_SUB_GROUP,
    IMG_UNSUB_GROUP,
    IMG_JOIN_GROUP,
    IMG_MEMBER,

    IMG_NUM
}IMG_ID_E;

//---------------------------------------------------------------------------
class TFormResource : public TForm
{
__published:    // IDE-managed Components
    TPageControl *PageControl1;
    TTabSheet *TabSheet1;
    TTabSheet *TabSheet2;
    TTabSheet *TabSheet3;
    TcxImageList *ImageListButton;
    TcxLocalizer *DevExpLocalizer;
    TcxButton *btnCancel;
    TcxButton *btnPrint;
    TcxButton *btnExport;
    TcxButton *btnAcceptCall;
    TcxButton *btnRejectCall;
    TcxButton *btnBrd;
    TcxButton *btnSDS;
    TcxButton *btnAdd;
    TcxButton *btnRefresh;
    TcxButton *btnDel;
    TcxButton *btnSDSReply;
    TcxButton *btnSet;
    TcxButton *btnStop;
    TcxButton *btnClose;
    TcxButton *btnQuery;
    TcxButton *btnSubGroup;
    TcxButton *btnNext;
    TcxButton *btnPrev;
    TcxButton *btnUnsubGroup;
    TcxButton *btnJoinGroup;
    TcxButton *btnMember;
    TcxButton *btnExit;
    TcxButton *btnImport;
    TcxButton *btnAllLine;
    TcxButton *btnUpLine;
    TcxButton *btnSwitch;
    TcxButton *btnDownLine;
    TcxButton *btnPTTDial;
    TcxButton *btnOK;
    TcxButton *btnDownload;
    TcxButton *btnPlay;
    TcxButton *btnRecv;
    TcxMemo *mmLocalize;
    TcxHintStyleController *HintStyleController;
    TcxButton *btnHide;
    TcxButton *btnDial;
    TcxButton *btnStartListen;
    TcxButton *btnStopListen;
    TcxButton *btnPause;
    TcxTreeList *tlMain;
    TcxStyleRepository *cxStyleRepository1;
    TcxStyle *StyleTreeListHeader;
    TcxTreeList *tlHeaderHeight;
    TcxTreeListColumn *Column24;
    TcxTreeListColumn *Column26;
    TcxTreeListColumn *Column28;
    TcxTreeListColumn *Column32;
    void __fastcall tlMainCustomDrawBackgroundCell(TcxCustomTreeList *Sender, TcxCanvas *ACanvas,
          TcxTreeListCustomCellViewInfo *AViewInfo, bool &ADone);

private:    // User declarations
    void __fastcall InitDevExpLocalizer();
    TcxButton* __fastcall GetImage(UINT32 ulImageID);

public:     // User declarations
    __fastcall TFormResource(TComponent* Owner);

    void __fastcall SetImage(TcxButton *btn, UINT32 ulImadeID);
    void __fastcall SetImage(TdxBarButton *btn, UINT32 ulImadeID);
    void __fastcall SetImage(TdxBarLargeButton *btn, UINT32 ulImadeID);
};
//---------------------------------------------------------------------------
//extern PACKAGE TFormResource *FormResource;
//---------------------------------------------------------------------------

extern void FreeFormResource();

extern void SetImage(TcxButton *btn, UINT32 ulImadeID);
extern void SetImage(TdxBarButton *btn, UINT32 ulImadeID);
extern void SetImage(TdxBarLargeButton *btn, UINT32 ulImadeID);

extern void Print(TcxTreeList *treeList, AnsiString strTitle);
extern TcxStyle* GetTreeListHeaderStyle();

#endif
