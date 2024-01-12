//---------------------------------------------------------------------------

#ifndef FrmGroupH
#define FrmGroupH
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
#include "cxPC.hpp"
//#include "cxPCdxBarPopupMenu.hpp"
#include "dxSkinscxPCPainter.hpp"
#include "cxProgressBar.hpp"
#include "cxCheckBox.hpp"
//#include "FrmGroupCallPanel.h"
#include "cxClasses.hpp"
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "dxBarBuiltInMenu.hpp"
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
#include "cxExtEditRepositoryItems.hpp"
#include "cxScrollBox.hpp"

//---------------------------------------------------------------------------
class TFormGroup : public TSubForm
{
__published:    // IDE-managed Components
    TcxTreeList *tlGroup;
    TcxTreeListColumn *ColumnGroupName;
    TcxTreeListColumn *ColumnGroupID;
    TcxTreeListColumn *ColumnStatus;
    TcxTreeListColumn *ColumnSpeaker;
    TdxBarManager *BarManager;
    TdxBarPopupMenu *pmGroup;
    TdxBarLargeButton *pmiSubGroup;
    TdxBarLargeButton *pmiUnsubGroup;
    TdxBarSeparator *dxBarSeparator1;
    TdxBarLargeButton *pmiJoinGroup;
    TcxButton *btnGroupMember;
    TcxButton *btnUnSubGroup;
    TcxPageControl *pcMain;
    TcxTabSheet *tsStaticGroup;
    TdxBarSeparator *dxBarSeparator2;
    TdxBarLargeButton *pmiShowMember;
    TTimer *TimerCheck;
    TcxTreeListColumn *ColumnSpeakTime;
    TTimer *TimerSpeakTime;
    TcxTreeListColumn *tlGroupColumn2;
    TcxButton *btnJoinGroup;
    TcxTabSheet *tsPatchGroup;
    TTimer *TimerCheckPatchGroup;
    TdxBarLargeButton *pmiCreatePatchGroup;
    TdxBarLargeButton *pmiDeletePatchGroup;
    TcxEditRepository *EditRepository;
    TcxEditRepositoryProgressBar *EditRepositoryProgressBar;
    TcxTabSheet *tsDynamicGroup;
    TTimer *TimerCheckTrainGroup;
    TcxScrollBox *ScrollBoxMain;
    TcxGroupBox *gbGroupCallPanel;
    TcxScrollBox *ScrollBoxLeft;
    TPanel *PanelMain;
    TTimer *TimerCheckStationGroup;
    TdxBarLargeButton *pmiStartListen;
    TdxBarLargeButton *pmiStopListen;
    TdxBarLargeButton *pmiCreateDynamicGroup;
    TdxBarSeparator *dxBarSeparator3;
    TdxBarLargeButton *pmiDeleteDynamicGroup;
    TdxBarLargeButton *pmiSetSubGroup;
    TdxBarLargeButton *pmiRefresh;
    TdxBarSeparator *dxBarSeparator4;
    TdxBarSeparator *dxBarSeparator5;
    TdxBarLargeButton *pmiNewGroupCall;
    TcxButton *btnCreateDynamicGroup;
    TcxButton *btnGroupCall;
    TcxGroupBox *gbWorkPanel;
    TcxButton *btnDelDynamicGroup;
    TcxButton *btnPTT;
    TcxTreeListColumn *ColumnCreator;
    TcxTabSheet *tsMultiGroup;
    TcxTreeList *tlMember;
    TcxTreeListColumn *tlUserColumn1;
    TcxTreeListColumn *tlUserColumn2;
    TcxTreeListColumn *tlUserColumn3;
    TcxGroupBox *gbGroupMember;
    TdxBarSeparator *dxBarSeparator6;
    TdxBarButton *pmiShowAllGroup;
    TdxBarButton *pmiSetGroupInfo;
    TdxBarSeparator *dxBarSeparator7;
    TdxBarButton *pmiNewVideoGroupCall;
    TdxBarButton *pmiCreateMultiGroup;
    TdxBarButton *pmiDeleteMultiGroup;
    TTimer *TimerCheckMultiGroup;
    void __fastcall tlGroupDblClick(TObject *Sender);
    void __fastcall pmiSubGroupClick(TObject *Sender);
    void __fastcall pmiUnsubGroupClick(TObject *Sender);
    void __fastcall tlGroupFocusedNodeChanged(TcxCustomTreeList *Sender, TcxTreeListNode *APrevFocusedNode,
          TcxTreeListNode *AFocusedNode);
    void __fastcall pmiShowMemberClick(TObject *Sender);
    void __fastcall TimerCheckTimer(TObject *Sender);
    void __fastcall ColumnStatusGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall pmiJoinGroupClick(TObject *Sender);
    void __fastcall TimerSpeakTimeTimer(TObject *Sender);
    void __fastcall pmiCreatePatchGroupClick(TObject *Sender);
    void __fastcall TimerCheckPatchGroupTimer(TObject *Sender);
    void __fastcall pmGroupPopup(TObject *Sender);
    void __fastcall pmiDeletePatchGroupClick(TObject *Sender);
    void __fastcall ColumnSpeakTimeGetEditProperties(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          TcxCustomEditProperties *&EditProperties);
    void __fastcall ColumnSpeakTimeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall TimerCheckTrainGroupTimer(TObject *Sender);
    void __fastcall TimerCheckStationGroupTimer(TObject *Sender);
    void __fastcall pmiStartListenClick(TObject *Sender);
    void __fastcall pmiStopListenClick(TObject *Sender);
    void __fastcall pmiCreateDynamicGroupClick(TObject *Sender);
    void __fastcall pmiDeleteDynamicGroupClick(TObject *Sender);
    void __fastcall pmiRefreshClick(TObject *Sender);
    void __fastcall pmiNewGroupCallClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall pmiSetSubGroupClick(TObject *Sender);
    void __fastcall pmiShowAllGroupClick(TObject *Sender);
    void __fastcall pmiSetGroupInfoClick(TObject *Sender);
    void __fastcall ColumnCreatorGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall btnPTTMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall btnPTTMouseEnter(TObject *Sender);
    void __fastcall btnPTTMouseLeave(TObject *Sender);
    void __fastcall btnPTTMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall pcMainPageChanging(TObject *Sender, TcxTabSheet *NewPage, bool &AllowChange);
    void __fastcall tlGroupClick(TObject *Sender);
    void __fastcall pmiNewVideoGroupCallClick(TObject *Sender);
    void __fastcall pmiCreateMultiGroupClick(TObject *Sender);
    void __fastcall pmiDeleteMultiGroupClick(TObject *Sender);
    void __fastcall TimerCheckMultiGroupTimer(TObject *Sender);

private:    // User declarations
    INT64   m_i64GroupID;

    virtual void __fastcall InitSkin();

    void __fastcall InitWorkPanel();
    void __fastcall ShowSubGroup();
    void __fastcall ShowAllGroup();
    void __fastcall AutoSubGroup();
    void __fastcall ShowGroup(UINT32 ulGroupType);
    TcxTreeListNode* __fastcall GetGroupNode(INT64 i64GroupID);
    void __fastcall SaveGroupStatus(INT64 i64GroupID, UINT32 ulGroupStatus);
    void __fastcall OnSaveGroupStatusRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnSaveGroupCallInfoRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnSaveGroupInfoRsp(UINT32 ulSeqID, GJson &Json);
    GROUP_INFO_T* __fastcall GetSelectGroup();
    TcxTreeListNode* __fastcall GetSelectNode();
    void __fastcall InitCtrlPanel();
    void __fastcall ShowGroupMember(VECTOR<UINT32> &vUser, VECTOR<INT64> &vGroup);
    void __fastcall ShowGroupInfo(TcxTabSheet *CurrPage=NULL);
    INT64 __fastcall GetFocusNodeGroupID();

public:     // User declarations
    __fastcall TFormGroup(TComponent* Owner);

    void __fastcall ShowGroupCallStatus(EVENT_GROUP_STATUS_T &stEvent);
    void __fastcall UpdateGroupCallInfo(TcxTreeListNode *Node, INT64 i64GroupID, UINT32 ulGroupStatus);

    void __fastcall FocusGroupCall(INT64 iGroupID);
    void __fastcall AddPatchGroup(AnsiString strGroupName, INT64 i64GroupID);

    void __fastcall SetGroupMember(INT64 i64GroupID);
    void __fastcall SetPatchGroupMember(INT64 i64PatchGroupID, INT64 i64OriGroupID);

    void __fastcall OnPTTDial();
    void __fastcall OnPTTRelease();
    void __fastcall RefreshPatchGroupPanel();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormGroup *FormGroup;
//---------------------------------------------------------------------------
#endif
