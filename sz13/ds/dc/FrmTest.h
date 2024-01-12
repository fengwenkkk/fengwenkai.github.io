//---------------------------------------------------------------------------

#ifndef FrmTestH
#define FrmTestH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "cxButtons.hpp"
#include "cxContainer.hpp"
#include "cxControls.hpp"
#include "cxEdit.hpp"
#include "cxGraphics.hpp"
#include "cxLookAndFeelPainters.hpp"
#include "cxLookAndFeels.hpp"
#include "cxMemo.hpp"
#include "cxTextEdit.hpp"
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
#include "cxPC.hpp"
#include "dxSkinscxPCPainter.hpp"
#include "cxLabel.hpp"
#include "cxCheckBox.hpp"
#include "dxBar.hpp"
#include "dxSkinsdxBarPainter.hpp"
#include <MPlayer.hpp>
#include "cxClasses.hpp"
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
#include "cxPropertiesStore.hpp"
#include "cxDropDownEdit.hpp"
#include "cxMaskEdit.hpp"
#include "cxSplitter.hpp"
#include "cxSpinEdit.hpp"
#include "cxImage.hpp"
#include "dxGDIPlusClasses.hpp"
#include "cxCustomData.hpp"
#include "cxImageComboBox.hpp"
#include "cxInplaceContainer.hpp"
#include "cxStyles.hpp"
#include "cxTL.hpp"
#include "cxTLdxBarBuiltInMenu.hpp"
#include "cxButtonEdit.hpp"
#include <Vcl.Dialogs.hpp>
#include "cxGroupBox.hpp"
//---------------------------------------------------------------------------
class TFormTest : public TSubForm
{
__published:    // IDE-managed Components
    TcxMemo *mmSDKEvent;
    TcxPageControl *pcMain;
    TcxTabSheet *cxTabSheet1;
    TcxTabSheet *cxTabSheet2;
    TcxButton *btnGetGroup;
    TcxButton *btnGetUser;
    TcxButton *btnGeMIC;
    TcxButton *btnP2PHangup;
    TcxButton *btnP2PReject;
    TcxButton *btnP2PRecv;
    TcxButton *btnP2PDial;
    TcxTabSheet *cxTabSheet3;
    TcxButton *btnJoinGroup;
    TcxButton *btnSubGroup;
    TcxButton *btnPTT;
    TcxButton *btnPTTRelease;
    TcxButton *btnPTTHangup;
    TcxTabSheet *cxTabSheet4;
    TcxTabSheet *cxTabSheet5;
    TcxButton *cxButton43;
    TcxButton *cxButton44;
    TPanel *Panel7;
    TcxButton *btnDeletePatchMember;
    TcxButton *btnCreatePatchGroup;
    TcxButton *cxButton4;
    TcxTextEdit *edtPatchGroupMember;
    TcxButton *btnGroupBreakoff;
    TPanel *Panel1;
    TcxTextEdit *edtUser;
    TcxButton *btnGetPatchGroup;
    TcxTabSheet *cxTabSheet6;
    TcxButton *btnGetGroups;
    TcxTextEdit *edtDnaUser;
    TcxTextEdit *edtDnaGroup;
    TcxButton *btnCancelDynamicGroup;
    TcxTabSheet *cxTabSheet7;
    TcxButton *cxButton9;
    TcxButton *cxButton10;
    TcxLabel *cxLabel1;
    TcxLabel *cxLabel2;
    TcxTextEdit *edtGroup;
    TcxButton *btnUnsubGroup;
    TcxButton *btnStartGroupDiscreetListen;
    TcxButton *btnStopGroupDiscreetListen;
    TcxButton *btnStartUserDiscreetListen;
    TcxButton *btnStopUserDiscreetListen;
    TcxButton *btnPTTEmergency;
    TcxButton *btnP2PTransfer;
    TcxLabel *cxLabel3;
    TcxTextEdit *edtTransferUser;
    TcxButton *btnLogin;
    TcxCheckBox *cbAllowLog;
    TcxButton *btnGetGroupUsers;
    TTimer *TimerDynamicGroup;
    TcxButton *btnSkinCfg;
    TcxTabSheet *cxTabSheet9;
    TcxTextEdit *edtRTPUrl;
    TLabel *Label1;
    TcxButton *btnStartPreview;
    TcxButton *btnStopPreview;
    TcxTabSheet *cxTabSheet10;
    TPanel *PanelRTP;
    TcxButton *btnPlayPCM;
    TcxButton *btnGetPatchMember;
    TdxBarManager *dxBarManager1;
    TcxButton *cxButton1;
    TcxButton *cxButton2;
    TcxButton *cxButton3;
    TcxButton *cxButton5;
    TcxButton *cxButton6;
    TcxButton *cxButton7;
    TPanel *Panel2;
    TcxTextEdit *edtCallType;
    TcxTextEdit *edtCaller;
    TcxTextEdit *edtCallee;
    TcxTextEdit *edtResourceID;
    TcxButton *btnClearLog;
    TcxButton *cxButton11;
    TcxTextEdit *edtURL;
    TcxTextEdit *edtDynamicGroup;
    TTimer *TimerCheckRTP;
    TcxButton *btnAddDynamicGroupMember;
    TcxButton *btnDelDynamicGroupMember;
    TcxTabSheet *cxTabSheet11;
    TcxButton *cxButton14;
    TcxButton *cxButton15;
    TcxTextEdit *edtMultiGroup1;
    TcxTextEdit *edtMultiGroup2;
    TcxLabel *cxLabel4;
    TcxButton *btnGetGroupMember;
    TcxButton *cxButton17;
    TcxTabSheet *tsSMS;
    TcxButton *btnSendSMS;
    TcxButton *cxButton19;
    TcxButton *cxButton20;
    TcxButton *btnCreateDynamicGroup;
    TcxLabel *cxLabel5;
    TcxLabel *cxLabel6;
    TcxLabel *cxLabel7;
    TcxTextEdit *edtGroupName;
    TcxMemo *mmInfo;
    TPanel *Panel3;
    TcxCheckBox *cbTriggerEventReport;
    TcxButton *cxButton12;
    TcxButton *cxButton13;
    TcxLabel *cxLabel8;
    TcxTextEdit *edtLoginUser;
    TcxLabel *cxLabel9;
    TcxTextEdit *edtLoginPwd;
    TcxButton *btnLogout;
    TcxButton *cxButton16;
    TcxLabel *cxLabel10;
    TcxTextEdit *edtDnaUser2;
    TcxTextEdit *edtDnaGroup2;
    TcxTabSheet *cxTabSheet8;
    TcxPropertiesStore *cxPropertiesStore1;
    TTimer *TimerStartBrd;
    TcxLabel *LabelBrd;
    TTimer *TimerStopBrd;
    TcxLabel *cxLabel12;
    TcxCheckBox *cbBrd;
    TcxTextEdit *edtBrdTrain;
    TcxTextEdit *edtDeletePeriod;
    TcxTextEdit *edtPatchGroupName;
    TcxTextEdit *edtPatchGroupMember2;
    TcxTextEdit *edtMRSAddr;
    TcxLabel *cxLabel11;
    TcxTabSheet *cxTabSheet12;
    TcxButton *cxButton21;
    TcxButton *cxButton22;
    TcxLabel *cxLabel13;
    TcxComboBox *cbMonSpeaker;
    TcxButton *cxButton23;
    TcxButton *cxButton24;
    TcxSplitter *cxSplitter1;
    TcxLabel *cxLabel14;
    TcxButton *cxButton26;
    TcxComboBox *cbMic;
    TcxButton *cxButton27;
    TcxButton *btnInitPCM;
    TcxButton *cxButton28;
    TcxButton *btnPausePCM;
    TcxButton *cxButton29;
    TcxButton *btnStopPCM;
    TcxSpinEdit *edtMicVolume;
    TTimer *TimerPlayPCM;
    TcxButton *cxButton25;
    TcxLabel *cxLabel15;
    TcxTextEdit *edtHttpUrl;
    TcxButton *cxButton30;
    TcxLabel *cxLabel16;
    TcxLabel *cxLabel17;
    TcxLabel *cxLabel18;
    TcxLabel *cxLabel19;
    TcxLabel *cxLabel20;
    TcxLabel *cxLabel21;
    TcxTextEdit *edtRecStartTime;
    TcxTextEdit *edtRecEndTime;
    TcxButton *cxButton31;
    TcxButton *cxButton32;
    TcxLabel *cxLabel22;
    TcxTextEdit *edtSMSText;
    TcxLabel *cxLabel23;
    TOpenDialog *OpenDialog;
    TcxLabel *cxLabel24;
    TcxTextEdit *edtSMSUser1;
    TcxTextEdit *edtSMSUser2;
    TcxLabel *cxLabel25;
    TcxTextEdit *edtSMSGroup1;
    TcxTextEdit *edtSMSGroup2;
    TcxButtonEdit *edtSMSFile;
    TcxTextEdit *edtCheckGroup;
    TcxTabSheet *cxTabSheet13;
    TcxButton *cxButton8;
    TcxButton *cxButton18;
    TcxTabSheet *cxTabSheet14;
    TcxLabel *cxLabel26;
    TcxTextEdit *edtVideoChanID;
    TcxButton *cxButton33;
    TcxButton *cxButton34;
    TcxLabel *cxLabel27;
    TcxTextEdit *edtVWallUserID;
    TcxButton *cxButton35;
    TcxLabel *cxLabel28;
    TcxTextEdit *edtWavFile;
    TcxButton *cxButton36;
    TcxGroupBox *cxGroupBox1;
    TcxButton *cxButton37;
    void __fastcall cxButton43Click(TObject *Sender);
    void __fastcall btnPTTReleaseClick(TObject *Sender);
    void __fastcall btnPTTHangupClick(TObject *Sender);
    void __fastcall btnGroupBreakoffClick(TObject *Sender);
    void __fastcall btnGetGroupClick(TObject *Sender);
    void __fastcall btnP2PDialClick(TObject *Sender);
    void __fastcall btnP2PHangupClick(TObject *Sender);
    void __fastcall btnP2PRecvClick(TObject *Sender);
    void __fastcall btnP2PRejectClick(TObject *Sender);
    void __fastcall btnGetUserClick(TObject *Sender);
    void __fastcall cxButton44Click(TObject *Sender);
    void __fastcall btnGeMICClick(TObject *Sender);
    void __fastcall btnSubGroupClick(TObject *Sender);
    void __fastcall btnCreatePatchGroupClick(TObject *Sender);
    void __fastcall btnGetPatchGroupClick(TObject *Sender);
    void __fastcall btnGetGroupsClick(TObject *Sender);
    void __fastcall btnCancelDynamicGroupClick(TObject *Sender);
    void __fastcall cxButton9Click(TObject *Sender);
    void __fastcall cxButton10Click(TObject *Sender);
    void __fastcall btnJoinGroupClick(TObject *Sender);
    void __fastcall btnUnsubGroupClick(TObject *Sender);
    void __fastcall btnPTTClick(TObject *Sender);
    void __fastcall btnStartGroupDiscreetListenClick(TObject *Sender);
    void __fastcall btnStopGroupDiscreetListenClick(TObject *Sender);
    void __fastcall btnStartUserDiscreetListenClick(TObject *Sender);
    void __fastcall btnStopUserDiscreetListenClick(TObject *Sender);
    void __fastcall btnPTTEmergencyClick(TObject *Sender);
    void __fastcall btnP2PTransferClick(TObject *Sender);
    void __fastcall btnLoginClick(TObject *Sender);
    void __fastcall cbTriggerEventReportPropertiesChange(TObject *Sender);
    void __fastcall btnStartPreviewClick(TObject *Sender);
    void __fastcall btnStopPreviewClick(TObject *Sender);
    void __fastcall btnPlayPCMClick(TObject *Sender);
    void __fastcall btnGetPatchMemberClick(TObject *Sender);
    void __fastcall cxButton3Click(TObject *Sender);
    void __fastcall cxButton5Click(TObject *Sender);
    void __fastcall cxButton6Click(TObject *Sender);
    void __fastcall cxButton7Click(TObject *Sender);
    void __fastcall btnClearLogClick(TObject *Sender);
    void __fastcall btnAddDynamicGroupMemberClick(TObject *Sender);
    void __fastcall btnDelDynamicGroupMemberClick(TObject *Sender);
    void __fastcall cxButton15Click(TObject *Sender);
    void __fastcall cxButton14Click(TObject *Sender);
    void __fastcall btnGetGroupMemberClick(TObject *Sender);
    void __fastcall cxButton17Click(TObject *Sender);
    void __fastcall cxButton19Click(TObject *Sender);
    void __fastcall cxButton20Click(TObject *Sender);
    void __fastcall btnCreateDynamicGroupClick(TObject *Sender);
    void __fastcall cxButton12Click(TObject *Sender);
    void __fastcall cxButton13Click(TObject *Sender);
    void __fastcall btnLogoutClick(TObject *Sender);
    void __fastcall cxButton16Click(TObject *Sender);
    void __fastcall TimerStartBrdTimer(TObject *Sender);
    void __fastcall cbBrdPropertiesChange(TObject *Sender);
    void __fastcall TimerStopBrdTimer(TObject *Sender);
    void __fastcall cxButton4Click(TObject *Sender);
    void __fastcall cxButton21Click(TObject *Sender);
    void __fastcall cxButton22Click(TObject *Sender);
    void __fastcall cxButton23Click(TObject *Sender);
    void __fastcall cxButton24Click(TObject *Sender);
    void __fastcall cxButton26Click(TObject *Sender);
    void __fastcall cxButton27Click(TObject *Sender);
    void __fastcall btnInitPCMClick(TObject *Sender);
    void __fastcall cxButton28Click(TObject *Sender);
    void __fastcall btnPausePCMClick(TObject *Sender);
    void __fastcall cxButton29Click(TObject *Sender);
    void __fastcall btnStopPCMClick(TObject *Sender);
    void __fastcall TimerPlayPCMTimer(TObject *Sender);
    void __fastcall cxButton31Click(TObject *Sender);
    void __fastcall cxButton32Click(TObject *Sender);
    void __fastcall edtSMSFilePropertiesButtonClick(TObject *Sender, int AButtonIndex);
    void __fastcall btnSendSMSClick(TObject *Sender);
    void __fastcall btnConfirmGroupValidClick(TObject *Sender);
    void __fastcall btnQueryPatchGroupInfoClick(TObject *Sender);
    void __fastcall btnDeletePatchMemberClick(TObject *Sender);
    void __fastcall cxButton8Click(TObject *Sender);
    void __fastcall btnSkinCfgClick(TObject *Sender);
    void __fastcall cxButton18Click(TObject *Sender);
    void __fastcall cxButton33Click(TObject *Sender);
    void __fastcall cxButton34Click(TObject *Sender);
    void __fastcall cxButton35Click(TObject *Sender);
    void __fastcall cxButton36Click(TObject *Sender);
    void __fastcall cxButton37Click(TObject *Sender);

private:    // User declarations
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_INNER_MSG, TMessage, OnInnerMsg)
    END_MESSAGE_MAP(TForm)

    void __fastcall OnGetESDKEvent(UINT8 *pucMsg, UINT32 ulMsgLen);
    void eSDKEventCallBack(INT32 iEventType, char *szEvent, UINT32 ulSize);

    virtual void __fastcall InitSkin(){};
    UINT32 __fastcall GetUserID();
    INT64 __fastcall GetGroupID();

    void __fastcall OnDCGetRecBrdCfgRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall Info(AnsiString strInfo);

public:     // User declarations
    __fastcall TFormTest(TComponent* Owner);
    void __fastcall Log(AnsiString strLog, BOOL bForceShow=FALSE);

};
//---------------------------------------------------------------------------
extern PACKAGE TFormTest *FormTest;
//---------------------------------------------------------------------------
#endif
