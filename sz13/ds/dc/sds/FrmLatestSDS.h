//---------------------------------------------------------------------------

#ifndef FrmLatestSDSH
#define FrmLatestSDSH
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
#include <Vcl.Dialogs.hpp>
#include "cxCalendar.hpp"
#include "cxDateUtils.hpp"
#include "cxMaskEdit.hpp"
#include "dxCore.hpp"
#include <Vcl.ComCtrls.hpp>
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxSplitter.hpp"
#include "cxImage.hpp"
#include "dxGDIPlusClasses.hpp"
#include <Vcl.Graphics.hpp>

#include "FrmMediaPlayer.h"
#include "cxImageComboBox.hpp"
//---------------------------------------------------------------------------
class TFormLatestSDS : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbMain;
    TcxTreeList *tlMain;
    TcxTreeListColumn *ColumnSDSID;
    TcxTreeListColumn *ColumnSendTime;
    TcxTreeListColumn *ColumnSDSType;
    TcxTreeListColumn *ColumnFormat;
    TcxTreeListColumn *ColumnIndex;
    TcxTreeListColumn *ColumnSender;
    TcxTreeListColumn *ColumnContent;
    TcxTreeListColumn *ColumnNeedReply;
    TcxMemo *mmSDSInfo;
    TPanel *PanelSDSInfo;
    TcxTreeList *tlSDSReply;
    TcxTreeListColumn *cxTreeListColumn1;
    TcxTreeListColumn *ColumnConfirmTime;
    TTimer *TimerCheck;
    TcxGroupBox *gbDetail;
    TcxTreeListColumn *ColumnConfirmRate;
    TcxGroupBox *gbWorkPanel;
    TcxTreeListColumn *ColumnSendSuccRate;
    TcxTreeListColumn *ColumnRecvTime;
    TcxButton *btnAdd;
    TcxButton *btnReply;
    TcxButton *btnRefresh;
    TcxButton *btnResend;
    TcxButton *btnAddSms;
    TcxButton *btnOpenAttachFile;
    TcxTreeListColumn *ColumnAttachFile;
    TPanel *PanelMediaPlayer;
    TcxSplitter *cxSplitter1;
    TcxImage *ImageAttach;
    TcxSplitter *SplitterReply;
    TPanel *PanelImage;
    void __fastcall btnRefreshClick(TObject *Sender);
    void __fastcall tlMainFocusedNodeChanged(TcxCustomTreeList *Sender, TcxTreeListNode *APrevFocusedNode,
          TcxTreeListNode *AFocusedNode);
    void __fastcall btnAddClick(TObject *Sender);
    void __fastcall TimerCheckTimer(TObject *Sender);
    void __fastcall tlMainDblClick(TObject *Sender);
    void __fastcall ColumnNeedReplyGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall btnReplyClick(TObject *Sender);
    void __fastcall ColumnSDSTypeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall tlSDSReplyGetCellHint(TcxCustomTreeList *Sender, TObject *ACell,
          UnicodeString &AText, bool &ANeedShow);
    void __fastcall btnResendClick(TObject *Sender);
    void __fastcall ColumnRecvTimeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnConfirmTimeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall btnAddSmsClick(TObject *Sender);
    void __fastcall btnOpenAttachFileClick(TObject *Sender);
    void __fastcall tlMainClick(TObject *Sender);
    void __fastcall ColumnAttachFileGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
private:    // User declarations
    bool        m_bInitView;
    CHAR        m_acSMSPath[512];
    TFormMediaPlayer    *m_FormMediaPlayer;

    virtual void __fastcall InitSkin(){};

    void __fastcall InitWorkPanel();
    void __fastcall ShowDetail();
    void __fastcall ShowSDSInfo(VECTOR<SDS_INFO_T> &vInfo);
    void __fastcall OnGetSDSInfoRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnGetSDSReplyRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnSaveSDSReplyRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnSaveSDSRecvRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall AutoShowSMSAttach(TcxTreeListNode *Node);

    void __fastcall LoadLocalSMSInfo(VECTOR<SDS_INFO_T> &vInfo, UINT32 ulStartTime);
    void __fastcall AddLocalSMSInfo(VECTOR<SDS_INFO_T> &vInfo);

public:     // User declarations

    __fastcall TFormLatestSDS(TComponent* Owner);

    void __fastcall ShowNewSDS(EVENT_SDS_REPORT_T &stSDS);
    void __fastcall ShowNewSMS(EVENT_SDS_REPORT_T &stSDS);
    void __fastcall NewSDS(UINT32 ulUserID);
    void __fastcall NewSDS(VECTOR<UINT32> &vUserID);
    void __fastcall NewSDS(VECTOR<INT64> &vGroupID);
    void __fastcall NewSDS(VECTOR<UINT32> &vUserID, VECTOR<INT64> &vGroupID);

    void __fastcall NewSMS(UINT32 ulUserID);
    void __fastcall NewSMS(VECTOR<UINT32> &vUserID);
    void __fastcall NewSMS(VECTOR<INT64> &vGroupID);
    void __fastcall NewSMS(VECTOR<UINT32> &vUserID, VECTOR<INT64> &vGroupID);
    void __fastcall UpdateSDSSendStatus(EVENT_SDS_SEND_STATUS_T &stSDS);
    void __fastcall RefreshSDSInfo();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormLatestSDS *FormLatestSDS;
//---------------------------------------------------------------------------
#endif
