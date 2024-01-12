//---------------------------------------------------------------------------

#ifndef FrmVideoCallH
#define FrmVideoCallH
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
#include "cxLookAndFeelPainters.hpp"
#include "cxLookAndFeels.hpp"
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
#include <ImgList.hpp>

#include "FrmVideoWin.h"
#include "cxClasses.hpp"
#include "cxImageList.hpp"
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
#include <System.ImageList.hpp>
#include "cxImageComboBox.hpp"
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxPC.hpp"
#include "dxBarBuiltInMenu.hpp"
#include "cxScrollBox.hpp"
//---------------------------------------------------------------------------
class TFormVideoCall : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbMain;
    TcxGroupBox *gbVideo;
    TcxGroupBox *gbVideoWin;
    TdxBarManager *BarManager;
    TdxBarPopupMenu *pmVideo;
    TdxBarLargeButton *pmiStopVideo;
    TdxBarSeparator *dxBarSeparator1;
    TdxBarLargeButton *pmiVideoRealPlay;
    TcxImageList *ImageListTreeUE;
    TdxBarPopupMenu *pmExitFullScreen;
    TdxBarLargeButton *pmiExitFullScreen;
    TTimer *TimerCheck;
    TPanel *PanelWork;
    TcxGroupBox *gbWorkPanel;
    TcxButton *btnVideoRealPlay;
    TcxButton *btnVideoP2PCall;
    TcxButton *btnPTT;
    TcxPageControl *pcMain;
    TcxTabSheet *tsUser;
    TcxTabSheet *tsGroup;
    TcxScrollBox *cxScrollBox1;
    TcxTreeList *tlGroup;
    TcxTreeListColumn *cxTreeListColumn1;
    TcxTreeListColumn *cxTreeListColumn2;
    TcxTreeListColumn *cxTreeListColumn3;
    TdxBarSeparator *dxBarSeparator2;
    TdxBarButton *pmiShowAllUser;
    TdxBarButton *pmiSetSubUser;
    TdxBarSeparator *dxBarSeparator3;
    TdxBarButton *pmiRefresh;
    TcxTreeList *tlUser;
    TcxTreeListColumn *ColumnName;
    TcxTreeListColumn *ColumnUser;
    TcxTreeListColumn *ColumnType;
    TcxTreeListColumn *ColumnStatus;
    TdxBarButton *pmiShowAllGroup;
    TdxBarButton *pmiSetSubGroup;
    TdxBarSeparator *dxBarSeparator4;
    TTimer *TimerInit;
    TcxTreeListColumn *tlGroupColumn4;
    TcxButton *btnVideoGroupCall;
    TdxBarButton *pmiVideoCall;
    TcxImageList *ImageListGroup;
    void __fastcall pmiVideoRealPlayClick(TObject *Sender);
    void __fastcall tlUserGetNodeImageIndex(TcxCustomTreeList *Sender, TcxTreeListNode *ANode,
          TcxTreeListImageIndexType AIndexType, TImageIndex &AIndex);
    void __fastcall pmiExitFullScreenClick(TObject *Sender);
    void __fastcall gbVideoWinResize(TObject *Sender);
    void __fastcall gbVideoDblClick(TObject *Sender);
    void __fastcall TimerCheckTimer(TObject *Sender);
    void __fastcall pmiStopVideoClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall pmiShowAllUserClick(TObject *Sender);
    void __fastcall pmiSetSubUserClick(TObject *Sender);
    void __fastcall pmiRefreshClick(TObject *Sender);
    void __fastcall ColumnNameGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall pcMainPageChanging(TObject *Sender, TcxTabSheet *NewPage, bool &AllowChange);
    void __fastcall pcMainChange(TObject *Sender);
    void __fastcall pmiShowAllGroupClick(TObject *Sender);
    void __fastcall pmiSetSubGroupClick(TObject *Sender);
    void __fastcall TimerInitTimer(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall gbWorkPanelResize(TObject *Sender);
    void __fastcall tlGroupGetNodeImageIndex(TcxCustomTreeList *Sender, TcxTreeListNode *ANode,
          TcxTreeListImageIndexType AIndexType, TImageIndex &AIndex);
    void __fastcall btnPTTMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall btnPTTMouseEnter(TObject *Sender);
    void __fastcall btnPTTMouseLeave(TObject *Sender);
    void __fastcall btnPTTMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall btnVideoGroupCallClick(TObject *Sender);
    void __fastcall tlUserBeginDragNode(TcxCustomTreeList *Sender, TcxTreeListNode *ANode,
          bool &Allow);
    void __fastcall tlUserEndDrag(TObject *Sender, TObject *Target, int X, int Y);
    void __fastcall pmiVideoCallClick(TObject *Sender);



private:    // User declarations
    bool    m_bInit;
    UINT32  m_ulDragUserID;
    MAP<UINT32, TcxTreeListNode*>       m_mUserNode;

    virtual void __fastcall InitSkin();

    void __fastcall InitWorkPanel();

    void __fastcall InitUser();
    void __fastcall InitGroup();
    void __fastcall NewVideoWin();
    bool __fastcall GetVideoCfg(UINT32 ulUserID, UINT32 &ulVideoFormat, UINT32 &ulCameraType, BOOL &bNeedConfirm, BOOL &bHasVoice);
    bool __fastcall StartVideoCall(UINT32 ulUserID, BOOL bIsVideoCall);
    UINT32 __fastcall GetSelectUser();
    INT64 __fastcall GetSelectGroup();
    void __fastcall SetButton();

    void __fastcall OnSaveP2PVideoCallInfoRsp(UINT32 ulSeqID, GJson &Json);

public:     // User declarations
    __fastcall TFormVideoCall(TComponent* Owner);

    void __fastcall UpdateUser(EVENT_USER_STATUS_T &stEvent);
    void __fastcall StopVideoPlay();
    void __fastcall FullScreenVideoWin(TFormVideoWin *pWin);
    bool __fastcall RecvVideoCall(UINT32 ulCallType, UINT32 ulUserID, UINT32 ulVideoFormat, UINT32 ulCameraType);
    void __fastcall ResizeVideoWin();
    TdxBarPopupMenu* GetExitFullScreenPopopMenu(){return pmExitFullScreen;};
    void __fastcall UpdateP2PVideoCallStatus(EVENT_P2PVIDEO_STATUS_T &stEvent);
    void __fastcall SaveP2PVideoCallInfo(UINT32 ulCallType, UINT32 ulCaller, UINT32 ulCallee, UINT32 ulStatus);
    void __fastcall OnPTTDial();
    void __fastcall OnPTTRelease();
    UINT32 __fastcall GetDragUserID(){return m_ulDragUserID;};
    bool __fastcall StartVideoGroupCall(INT64 i64GroupID);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormVideoCall *FormVideoCall;

//---------------------------------------------------------------------------
#endif
