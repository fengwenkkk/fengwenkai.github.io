//---------------------------------------------------------------------------

#ifndef FrmUserH
#define FrmUserH
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
#include <ImgList.hpp>
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
#include "cxRadioGroup.hpp"
//---------------------------------------------------------------------------
class TFormUser : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbLeft;
    TcxTreeList *tlMain;
    TcxTreeListColumn *ColumnUserID;
    TcxTreeListColumn *ColumnStatus;
    TcxTreeListColumn *ColumnType;
    TcxGroupBox *gbP2PDial;
    TcxButton *btnDialHangup;
    TcxButton *btnDial;
    TcxLabel *cxLabel21;
    TcxLabel *labelCallType;
    TcxTrackBar *tbP2PDialVolume;
    TcxTextEdit *edtCalleeNumber;
    TdxBarManager *BarManager;
    TdxBarPopupMenu *pmMain;
    TcxImageList *ImageListTreeUE;
    TdxBarButton *pmiPrivateCall;
    TcxGroupBox *gbCallPanel;
    TdxBarButton *pmiStartListen;
    TdxBarSeparator *dxBarSeparator1;
    TTimer *TimerCheck;
    TdxBarButton *pmiStopListen;
    TdxBarSeparator *dxBarSeparator2;
    TdxBarButton *pmiRefresh;
    TdxBarSeparator *dxBarSeparator3;
    TdxBarButton *pmiStartEnvListen;
    TdxBarButton *pmiStopEnvListen;
    TdxBarButton *pmiCallTransfer;
    TcxButton *btnStartEnvListen;
    TcxButton *btnStartListen;
    TcxButton *btnHangup;
    TcxButton *btnTransfer;
    TcxLabel *LabelDialStatus;
    TcxLabel *cxLabel1;
    TcxTextEdit *edtDialTransferNumbe;
    TcxGroupBox *gbWorkPanel;
    TcxButton *btnP2PCall;
    TcxButton *btnStopListen;
    TcxButton *btnStopEnvListen;
    TcxButton *btnP2PTransfer;
    TcxTreeListColumn *ColumnCellID;
    TcxTreeListColumn *ColumnCallStatus;
    TcxTreeListColumn *ColumnName;
    TdxBarSeparator *dxBarSeparator4;
    TdxBarButton *pmiBreakIn;
    TdxBarButton *pmiBreakOff;
    TdxBarButton *pmiSubUser;
    TcxTreeListColumn *ColumnFuncType;
    TdxBarButton *pmiShowAllUser;
    TdxBarSeparator *dxBarSeparator5;
    TdxBarButton *pmiHangup;
    TcxTreeListColumn *ColumnGPSInfo;
    TcxLabel *cxLabel2;
    TcxRadioButton *rbFullDuplex;
    TcxRadioButton *rbHalfDuplex;
    TTimer *TimerGPS;
    void __fastcall pmiPrivateCallClick(TObject *Sender);
    void __fastcall tlMainGetNodeImageIndex(TcxCustomTreeList *Sender, TcxTreeListNode *ANode,
          TcxTreeListImageIndexType AIndexType, TImageIndex &AIndex);
    void __fastcall btnDialClick(TObject *Sender);
    void __fastcall btnDialHangupClick(TObject *Sender);
    void __fastcall ColumnStatusGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall tbP2PDialVolumePropertiesChange(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall pmiStartListenClick(TObject *Sender);
    void __fastcall TimerCheckTimer(TObject *Sender);
    void __fastcall pmiStopListenClick(TObject *Sender);
    void __fastcall pmiRefreshClick(TObject *Sender);
    void __fastcall pmiStartEnvListenClick(TObject *Sender);
    void __fastcall pmiStopEnvListenClick(TObject *Sender);
    void __fastcall gbCallPanelResize(TObject *Sender);
    void __fastcall btnTransferClick(TObject *Sender);
    void __fastcall pmiBreakInClick(TObject *Sender);
    void __fastcall pmiBreakOffClick(TObject *Sender);
    void __fastcall pmiSubUserClick(TObject *Sender);
    void __fastcall ColumnCallStatusGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnFuncTypeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnTypeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall pmiShowAllUserClick(TObject *Sender);
    void __fastcall pmiHangupClick(TObject *Sender);
    void __fastcall TimerGPSTimer(TObject *Sender);

private:    // User declarations
    MAP<UINT32, TcxTreeListNode*>       m_mUserNode;

    UINT32      m_ulCurrListenUserID;
    UINT32      m_ulCurrListenUserTime;

    virtual void __fastcall InitSkin();

    void __fastcall InitWorkPanel();
    void __fastcall InitUser();

public:     // User declarations
    __fastcall TFormUser(TComponent* Owner);

    void __fastcall UpdateP2PCallStatus(EVENT_P2PCALL_STATUS_T &stEvent);
    void __fastcall UpdateUser(EVENT_USER_STATUS_T &stEvent);
    void __fastcall UpdateGISInfo(EVENT_GIS_REPORT_T &stEvent);
    void __fastcall UpdateTrainInfo();
    void __fastcall Dial(UINT32 ulUserID);
    void __fastcall StopListen();

    void __fastcall CallOver();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormUser *FormUser;
//---------------------------------------------------------------------------
#endif
