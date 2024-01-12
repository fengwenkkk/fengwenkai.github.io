//---------------------------------------------------------------------------

#ifndef FrmIPHH
#define FrmIPHH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "cxButtons.hpp"
#include "cxContainer.hpp"
#include "cxControls.hpp"
#include "cxCustomData.hpp"
#include "cxEdit.hpp"
#include "cxGraphics.hpp"
#include "cxGroupBox.hpp"
#include "cxInplaceContainer.hpp"
#include "cxLabel.hpp"
#include "cxLookAndFeelPainters.hpp"
#include "cxLookAndFeels.hpp"
#include "cxStyles.hpp"
#include "cxTextEdit.hpp"
#include "cxTL.hpp"
#include "cxTLdxBarBuiltInMenu.hpp"
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
#include "MDIForm.h"
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include "dxBar.hpp"
#include "dxSkinsdxBarPainter.hpp"
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
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxPC.hpp"
#include "dxBarBuiltInMenu.hpp"
#include "pis.h"
#include "cxCheckBox.hpp"
#include "cxImageComboBox.hpp"
#include "cxSplitter.hpp"

#define IPH_VIDEO_STATE_NONE        0
#define IPH_VIDEO_STATE_PREVIEW     1
#define IPH_VIDEO_STATE_TALK        2

// 对讲接受状态
#define IPH_ACCEPT_SUCC         0   // 成功
#define IPH_ACCEPT_FAIL         1   // 失败
#define IPH_CLOSED              2   // 该对讲已经关闭
#define IPH_ACCEPT_WAIT_ACK     3   // 等待IPH_ACK

// 对讲接听状态
#define IPH_STOP_SUCC       0   // 成功
#define IPH_STOP_FAIL       1   // 失败


typedef struct
{
    UINT32  ulTrainUnitID;
    UINT32  ulCarriageID;
    UINT32  ulDoorID;
    UINT32  ulIPHDevID;
    UINT32  ulStatus;       // WAIT/TALK/OVER
    UINT8   aucIPHDevIP[4];
    CHAR    acVideoURL[64];
    UINT32  ulApplyTime;

    UINT32  ulTalkingDCUserID;
}IPH_INFO_T;

/*
typedef struct
{
    IPH_INFO_T          stIPHInfo;
    TcxGroupBox         *Ctrl;
    UINT32              ulVideoStatus;
    ThreadPlayAndRec    *ThreadVideo;
}IPH_STATUS_T;       */

//---------------------------------------------------------------------------
class TFormIPH : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbIPH;
    TdxBarManager *BarManager;
    TdxBarPopupMenu *pmMain;
    TPanel *PanelVideo;
    TTimer *TimerCheck;
    TTimer *TimerCheckRTP;
    TdxBarSeparator *dxBarSeparator1;
    TdxBarButton *pmiHangoff;
    TdxBarButton *pmiHangup;
    TcxTreeList *tlMain;
    TcxTreeListColumn *ColumnTime;
    TcxTreeListColumn *ColumnTrain;
    TcxTreeListColumn *ColumnCarriageID;
    TcxTreeListColumn *ColumnDevID;
    TcxTreeListColumn *ColumnStatus;
    TcxTreeListColumn *tlMainColumn1;
    TcxTreeListColumn *tlMainColumn2;
    TcxGroupBox *gbVideo;
    TcxGroupBox *gbWorkPanel;
    TcxButton *btnHangoff;
    TcxButton *btnHangup;
    TcxTreeListColumn *ColumnDCUserID;
    TcxTreeListColumn *ColumnUptime;
    TcxTreeListColumn *ColumnDevPos;
    TcxSplitter *Splitter;
    TPanel *PanelSide1;
    TPanel *PanelSide2;
    TdxBarSeparator *dxBarSeparator2;
    TdxBarButton *pmiAECMode;
    TdxBarPopupMenu *pmVideo;
    TdxBarButton *pmiPlayVideo;
    TdxBarButton *pmiStopVideo;
    TPanel *PanelEmpty;
    TTimer *TimerGetIPH;
    TTimer *TimerCheckIPHAck;
    TcxButton *btnCleaner;
    TdxBarButton *btnIPHPause;
    TdxBarButton *btnRenewIPH;
    void __fastcall pmiHangoffClick(TObject *Sender);
    void __fastcall pmiHangupClick(TObject *Sender);
    void __fastcall ColumnStatusGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall gbIPHResize(TObject *Sender);
    void __fastcall pmMainPopup(TObject *Sender);
    void __fastcall tlMainFocusedNodeChanged(TcxCustomTreeList *Sender, TcxTreeListNode *APrevFocusedNode,
          TcxTreeListNode *AFocusedNode);
    void __fastcall TimerCheckTimer(TObject *Sender);
    void __fastcall TimerCheckRTPTimer(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ColumnTrainGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnTimeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnDCUserIDGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall tlMainClick(TObject *Sender);
    void __fastcall ColumnDevPosGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall pmiAECModeClick(TObject *Sender);
    void __fastcall PanelVideoMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall pmiPlayVideoClick(TObject *Sender);
    void __fastcall pmiStopVideoClick(TObject *Sender);
    void __fastcall TimerGetIPHTimer(TObject *Sender);
    void __fastcall TimerCheckIPHAckTimer(TObject *Sender);
    void __fastcall btnCleanerClick(TObject *Sender);
    void __fastcall btnIPHPauseClick(TObject *Sender);
    void __fastcall btnRenewIPHClick(TObject *Sender);
private:    // User declarations
    virtual void __fastcall InitSkin();
//  void __fastcall UpdateGroupCallCtrlPos();
//  void __fastcall SetIPHParam();
    void __fastcall InitWorkPanel();
    void __fastcall OnApplyIPHTalkReq(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnStopIPHTalkReq(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnGetActiveIPHTalkRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnSAIPHApplyACKMsg(UINT32 ulSeqID, GJson &Json);

    void __fastcall StartVideo(AnsiString strUrl);
    UINT32 __fastcall SendIPHAccept(TcxTreeListNode *Node, BOOL bSyncMode);
    BOOL __fastcall SendIPHAccept();
    void __fastcall ShowIPHInfo(VECTOR<IPH_INFO_T> &vIPHInfo);
    TcxTreeListNode* __fastcall GetIPHNode(UINT32 ulTrainUnitID, UINT32 ulCarriageID, UINT32 ulIPHDevID);
    void __fastcall UpdateButtonStatus(TcxTreeListNode *Node);
    void __fastcall UpdateButtonStatus();
    void __fastcall ResizeVideoPanel();
    void __fastcall OnSAUPloadIPHAudioData(UINT32 ulSeqID, GJson &Json);
    void __fastcall StopIPHTalk();

public:     // User declarations
    __fastcall TFormIPH(TComponent* Owner);

    void __fastcall AutoAccept();
    void __fastcall Active();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormIPH *FormIPH;
//---------------------------------------------------------------------------
#endif
