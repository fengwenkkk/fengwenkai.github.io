//---------------------------------------------------------------------------

#ifndef FrmCallInH
#define FrmCallInH
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
#include "cxClasses.hpp"
#include "cxImageComboBox.hpp"
#include "cxImageList.hpp"
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.MPlayer.hpp>
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxCheckBox.hpp"
#include "cxSplitter.hpp"
#include "AdvSplitter.hpp"
#include "cxScrollBox.hpp"
#include "dxBevel.hpp""
#include "dxBevel.hpp"
#include "cxScrollBox.hpp"
#include "WMPLib_OCX.h"
#include <Vcl.OleCtrls.hpp>
#include <string>

#define CALL_STATE_IDLE             0   // 空闲
#define CALL_STATE_WAIT             1   // 等待
#define CALL_STATE_ACCEPT           2   // 通话中
#define CALL_STATE_CLOSED           9   // 结束

#define CALL_STATE_NOT_CONFIRMED    4   // 未确认
#define CALL_STATE_CONFIRMED        5   // 确认
#define CALL_STATE_REJECTED         6   // 拒绝

#define CALL_STATE_TALKING      CALL_STATE_ACCEPT

//---------------------------------------------------------------------------
class TFormCallIn : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbMain;
    TcxTreeList *tlCallIn;
    TcxTreeListColumn *cxTreeListColumn10;
    TcxTreeListColumn *tlCallInColumnCallType;
    TcxTreeListColumn *ColumnCaller;
    TcxTreeListColumn *ColumnTrainOrStation;
    TcxTreeListColumn *cxTreeListColumn13;
    TcxTreeListColumn *tlMainColumn6;
    TcxTreeListColumn *tlMainColumn7;
    TcxTreeListColumn *tlMainColumn8;
    TcxTreeListColumn *tlMainColumn9;
    TdxBarManager *BarManager;
    TdxBarSeparator *dxBarSeparator1;
    TdxBarPopupMenu *pmMain;
    TcxTreeListColumn *ColumnAccept;
    TcxTreeListColumn *ColumnClose;
    TcxTreeListColumn *ColumnDCUser;
    TcxImageList *ImageListCall;
    TdxBarButton *pmiHangup;
    TdxBarButton *pmiConfirm;
    TTimer *TimerCheck;
    TcxTreeListColumn *tlMainColumn13;
    TMediaPlayer *MediaPlayer;
    TTimer *TimerCheckCall;
    TcxTreeListColumn *tlMainColumn14;
    TcxTreeListColumn *ColumnUEType;
    TcxTreeListColumn *ColumnReqTime;
    TdxBarButton *pmiCallTransfer;
    TdxBarSeparator *dxBarSeparator2;
    TPanel *PanelMain;
    TdxBarButton *pmiAccept;
    TAdvSplitter *SplitterTop;
    TPanel *Panel1;
    TcxButton *btnAccept;
    TcxButton *btnConfirm;
    TcxButton *btnHangup;
    TcxButton *btnCall;
    TdxBevel *dxBevel1;
    TcxScrollBox *sbStation;
    TcxScrollBox *sbMain;
    TWindowsMediaPlayer *WindowsMediaPlayer;
    TcxGroupBox *gbCallInfo;
    TcxTreeList *tlCallInfo;
    TcxTreeListColumn *cxTreeListColumn35;
    TcxTreeListColumn *cxTreeListColumn36;
    TcxTreeListColumn *ColumnCurrSpeaker;
    TcxTreeListColumn *cxTreeListColumn39;
    TPanel *Panel4;
    TdxBevel *dxBevel4;
    TcxButton *btnJoinGroup;
    TcxButton *btnExitGroup;
    TcxTreeListColumn *tlCallInfoColumn5;
    TdxBarPopupMenu *pmCallInfo;
    TdxBarButton *pmiCall;
    TdxBarButton *pmiExitGroup;
    TdxBarSeparator *dxBarSeparator3;
    TdxBarButton *btnListen;
    TdxBarSeparator *dxBarSeparator4;
    TdxBarButton *pmiBreakoff;
    TcxTreeListColumn *tlCallInColumn17;
    TdxBarSeparator *dxBarSeparator5;
    void __fastcall tlCallInColumnCallTypeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value);
    void __fastcall cxTreeListColumn10GetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall btnAcceptClick(TObject *Sender);
    void __fastcall btnHangupClick(TObject *Sender);
    void __fastcall tlCallInCustomDrawDataCell(TcxCustomTreeList *Sender, TcxCanvas *ACanvas,
          TcxTreeListEditCellViewInfo *AViewInfo, bool &ADone);
    void __fastcall cxTreeListColumn13GetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall btnConfirmClick(TObject *Sender);
    void __fastcall TimerCheckTimer(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall btnCallClick(TObject *Sender);
    void __fastcall tlCallInFocusedNodeChanged(TcxCustomTreeList *Sender, TcxTreeListNode *APrevFocusedNode,
          TcxTreeListNode *AFocusedNode);
    void __fastcall tlCallInDataChanged(TObject *Sender);
    void __fastcall TimerCheckCallTimer(TObject *Sender);
    void __fastcall ColumnTrainOrStationGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnDCUserGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnCallerGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall MainPanelResize(TObject *Sender);
    void __fastcall pmiAcceptClick(TObject *Sender);
    void __fastcall pmiCallTransferClick(TObject *Sender);
    void __fastcall pmiConfirmClick(TObject *Sender);
    void __fastcall pmiHangupClick(TObject *Sender);
    void __fastcall btnJoinGroupClick(TObject *Sender);
    void __fastcall btnExitGroupClick(TObject *Sender);
    void __fastcall tlCallInfoColumn5GetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnCurrSpeakerGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall tlCallInfoDblClick(TObject *Sender);
    void __fastcall tlCallInfoCustomDrawDataCell(TcxCustomTreeList *Sender, TcxCanvas *ACanvas,
          TcxTreeListEditCellViewInfo *AViewInfo, bool &ADone);
    void __fastcall btnListenClick(TObject *Sender);
    void __fastcall pmiBreakoffClick(TObject *Sender);

private:    // User declarations
    virtual void __fastcall InitSkin();
    void __fastcall SendRequestCallRsp(UINT32 ulReqTime, UINT32 ulUserID, UINT32 ulCallType, INT64 i64DynamicGroupID, UINT32 ulStationUserID, VECTOR<UINT32> &vTrainUserID);
    void __fastcall CloseRequestCall(UINT32 ulUserID, UINT32 ulCallType);
    void __fastcall ApplyReqCall(UINT32 ulUserID, UINT32 ulCallType);

    void __fastcall StartP2PEmergencyCallAlarm();
    void __fastcall StartTrainReqAlarm(UINT32 ulTrainUnitID);
    void __fastcall StopTrainReqAlarm(UINT32 ulTrainUnitID);
    void __fastcall StartEmergencyCallAlarm(INT64 i64GroupID);
    void __fastcall StopEmergencyCallAlarm(INT64 i64GroupID);
    void __fastcall StartReqCallAlarm(BOOL bEmergencyReqCall, UINT32 ulUserID);
    void __fastcall StopReqCallAlarm(UINT32 ulReqCallType, UINT32 ulUserID);
    void __fastcall StartIPHTalkAlarm(UINT32 ulTrainUnitID);
    void __fastcall StopIPHTalkAlarm(UINT32 ulTrainUnitID);

    void __fastcall StartVoiceAlarm();
    void __fastcall StopVoiceAlarm();

    void __fastcall ShowGroupCallWin(INT64 i64GroupID, AnsiString strCaption="");
    void __fastcall ShowP2PCallWin(UINT32 ulUserID, bool bEmergencyCall, bool bAutoAccept);
    void __fastcall OnDCAcceptReqCallInd(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnDCSetTrainPosInd(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnDCSetTrainDepotIDInd(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnAppEmergencyCallReq(UINT32 ulSeqID, GJson &Json);
    void __fastcall HandleReqCall(TcxTreeListNode *Node, UINT32 ulUserID, UINT32 ulReqCallType);
    void __fastcall AutoSelectNode(TcxTreeListNode *Node);
    void __fastcall CheckVoiceAlarm();
    void __fastcall NewHalfDuplexP2PCall(EVENT_P2PCALL_STATUS_T &stEvent);

    /**
     * @brief           在 tlCallIn 表中插入一条记录
     * @param           ulUserID          [in]  User 短号码
     * @param           ulCallerID        [in]  呼叫方ID
     * @param           ulReqCallType     [in]  请呼类型
     * @param           ulUEType          [in]  呼叫方类型
     * @param           ulReqTime         [in]  请求时间
     * @param           ulDefaultGroupID  [in]  默认组ID
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-09-09 10:27:10
     * @note
     */
    void __fastcall InsertCallInInfoList(UINT32 ulUserID, UINT32 ulCallerID,
                                         UINT32 ulReqCallType, UINT32 ulUEType,
                                         UINT32 ulReqTime, UINT32 ulDefaultGroupID,
                                         AnsiString strGPSInfo);

public:     // User declarations
    __fastcall TFormCallIn(TComponent* Owner);

    void __fastcall PauseVoiceAlarm();
    void __fastcall ResumeVoiceAlarm();

    void __fastcall StartSDSAlarm();

    void __fastcall TrainRegister(UINT32 ulTrainUnitID, UINT32 ulDepodID);
    void __fastcall OnSetTrainPosReq(GJson &Json, UINT32 ulSeqID);
    void __fastcall OnRequestCallReq(GJson &Json, UINT32 ulSeqID);

    void __fastcall NewEmergencyCall(INT64 i64GroupID, UINT32 ulSpeaker);
    void __fastcall CloseEmergencyCall(INT64 i64GroupID);

    void __fastcall NewP2PEmergencyCall(EVENT_P2PCALL_STATUS_T &stEvent);
    void __fastcall NewP2PCall(EVENT_P2PCALL_STATUS_T &stEvent);
    void __fastcall StopP2PCall(UINT32 ulUserID);

    void __fastcall NewP2PVideoCall(EVENT_P2PVIDEO_STATUS_T &stEvent);
    void __fastcall StopP2PVideoCall(UINT32 ulUserID);

    void __fastcall NewIPHTalk(UINT32 ulTrainUnitID);
    void __fastcall UpdateIPHTalk(IntSet &sTrain);
    void __fastcall AcceptIPHTalk(UINT32 ulTrainUnitID);
    void __fastcall StopIPHTalk(UINT32 ulTrainUnitID);

    void __fastcall UpdateGroupCallStatus(EVENT_GROUP_STATUS_T &stEvent);
    void __fastcall StopGroupCall(INT64 i64GroupID);

    void __fastcall PickUpEmergencyCall();
    void __fastcall PickUpP2PCall(bool bEmergencyCall);
    void __fastcall PickUpP2PVideo();
    void __fastcall PickUpIPH();
    void __fastcall PickUpReqCall();

    void __fastcall PickUpTrainReqOrTrainEmergencyCallDC(INT64& i64DynamicGroupID);
    void __fastcall PickUpStationReqCallOrStationEmergencyCallDC(INT64& i64DynamicGroupID);
    void __fastcall PickUpTrainReqCallStation(INT64& i64DynamicGroupID);
    void __fastcall PickUpStationReqCallTrain(INT64& i64DynamicGroupID);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormCallIn *FormCallIn;
//---------------------------------------------------------------------------
#endif
