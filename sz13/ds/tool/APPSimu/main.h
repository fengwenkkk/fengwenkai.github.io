//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
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
#include "dxSkinscxPCPainter.hpp"
#include "cxGroupBox.hpp"
#include "MDIForm.h"
#include "cxMemo.hpp"
#include "cxLabel.hpp"
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
#include "dxBarBuiltInMenu.hpp"
#include "cxRadioGroup.hpp"
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxCheckBox.hpp"
//---------------------------------------------------------------------------

#if 0
typedef struct
{
    int iStationId;
    char acStationName[64];

}Station_Info;


typedef struct
{
    UINT8 iNow_Station;
    UINT8 iNextStation;
    UINT8 iPreStation;
    UINT8 iIsTurnback;
    UINT8 iIsTransfertrack;
    UINT8 iIsInOutDepot;

}Now_Station_Info;

typedef struct
{
   char acCarBodySerial[32];
   int iOutOfStockFlag;
   char acNowStation[32];
}OutOfStock;


class Carinfo{

public:
    UINT16 iCarBodySerial;
    UINT16 iCarServerSerial;
    UINT16 iCarOnewaySerial;
    AnsiString strCarDestid;
    UINT8 iCarDriverSerial;
    UINT8 iCarRollStockNumber;
    UINT8 iCarUpsideId;
    UINT8 iCarDownsideId;
    UINT8 iCarRunDirection;
    UINT8 iCarTurnbackFlag;
    UINT8 iCarTransferFlag;
    UINT8 iCarOnPlatformId;
    UINT8 iCarInOutDepot;
    UINT8 iCarTrackName;

    Carinfo();
    Carinfo(UINT16 BodySerial,UINT16 ServerSerial,UINT16 OnewaySerial,AnsiString Destid,UINT8 DriverSerial,\
    UINT8 RollStockNumber,UINT8 UpsideId,UINT8 DownsideId,UINT8 RunDirection,UINT8 TurnbackFlag,UINT8 TransferFlag,\
    UINT8 OnPlatformId,UINT8 InOutDepot,UINT8 TrackName);

    void CarOutStack();

};

Carinfo::Carinfo(){}

Carinfo::Carinfo(UINT16 BodySerial,UINT16 ServerSerial,UINT16 OnewaySerial,AnsiString Destid,UINT8 DriverSerial,\
    UINT8 RollStockNumber,UINT8 UpsideId,UINT8 DownsideId,UINT8 RunDirection,UINT8 TurnbackFlag,UINT8 TransferFlag,\
    UINT8 OnPlatformId,UINT8 InOutDepot,UINT8 TrackName):iCarBodySerial(BodySerial),iCarServerSerial(ServerSerial),\
    iCarOnewaySerial(OnewaySerial),strCarDestid(Destid),iCarDriverSerial(DriverSerial),iCarRollStockNumber(RollStockNumber),\
    iCarUpsideId(UpsideId),iCarDownsideId(DownsideId),iCarRunDirection(RunDirection),iCarTurnbackFlag(TurnbackFlag),iCarTransferFlag(TransferFlag),\
    iCarOnPlatformId(OnPlatformId),iCarInOutDepot(InOutDepot),iCarTrackName(TrackName){

}

void Carinfo::CarOutStack()
{



}

#endif

class TFormMain : public TSubForm
{
__published:    // IDE-managed Components
    TdxSkinController *dxSkinController1;
    TTimer *TimerCfg;
    TcxMemo *mmMsg;
    TPanel *PanelMain;
    TcxPageControl *pcMain;
    TPanel *Panel1;
    TcxLabel *cxLabel1;
    TcxTextEdit *edtTrainUnitID;
    TcxTabSheet *¹ã²¥;
    TcxButton *btnStartBrd;
    TcxTabSheet *cxTabSheet2;
    TcxTabSheet *cxTabSheet3;
    TcxButton *btnReqCall;
    TcxLabel *cxLabel2;
    TcxTextEdit *edtNewTrainID;
    TcxButton *btnSetTrainID;
    TcxLabel *cxLabel4;
    TcxRadioButton *rbCallDC;
    TcxRadioButton *rbCallStation;
    TcxLabel *cxLabel5;
    TcxTextEdit *edtUser;
    TcxTabSheet *tsDISCfg;
    TcxMemo *mmCfg;
    TTimer *TimerATS;
    TcxLabel *cxLabel3;
    TcxTextEdit *edtTrainID;
    TcxLabel *cxLabel6;
    TcxTextEdit *edtCurrGroupID;
    TcxLabel *cxLabel11;
    TcxTextEdit *edtRegisteredDCType;
    TcxLabel *cxLabel7;
    TcxTextEdit *edtStationID;
    TcxTextEdit *edtCellID;
    TcxLabel *cxLabel8;
    TcxLabel *cxLabel9;
    TcxTextEdit *edtCallType;
    TcxLabel *cxLabel10;
    TcxTextEdit *edtCallID;
    TcxLabel *cxLabel12;
    TcxTextEdit *edtDirection;
    TcxTabSheet *cxTabSheet5;
    TcxButton *btnSendSDSReply;
    TcxTabSheet *cxTabSheet6;
    TcxButton *btnLogin;
    TcxLabel *cxLabel13;
    TcxTextEdit *edtDCUser;
    TcxButton *btnGetSDSInfo;
    TcxTreeList *tlSDS;
    TcxTreeListColumn *Column1;
    TcxTreeListColumn *Column2;
    TcxTreeListColumn *tlMainColumn3;
    TcxTreeListColumn *tlMainColumn4;
    TcxTreeListColumn *tlMainColumn5;
    TcxTreeListColumn *tlMainColumn6;
    TcxTreeListColumn *tlMainColumn7;
    TcxTreeListColumn *tlMainColumn8;
    TcxButton *cxButton1;
    TcxTabSheet *cxTabSheet4;
    TcxButton *btnGetMasterStatus;
    TcxLabel *cxLabel14;
    TcxTextEdit *edtDBAAddr;
    TcxMemo *mmDBAInfo;
    TcxTabSheet *cxTabSheet7;
    TcxLabel *cxLabel15;
    TcxButton *cxButton3;
    TcxComboBox *cbTrainPos;
    TcxButton *btnRegister;
    TcxButton *btnUnregister;
    TcxComboBox *cbDCType;
    TcxLabel *cxLabel16;
    TcxLabel *cxLabel17;
    TcxRadioButton *rbUp;
    TcxRadioButton *rbDown;
    TcxLabel *cxLabel18;
    TcxTextEdit *edtDestID;
    TcxTextEdit *edtNewDestID;
    TcxTabSheet *tsIPH;
    TPanel *Panel2;
    TcxButton *btnIPHApply;
    TcxGroupBox *cxGroupBox1;
    TcxTextEdit *edtCarriageID1;
    TcxTextEdit *edtIPHDevID1;
    TcxLabel *cxLabel19;
    TcxLabel *cxLabel20;
    TcxLabel *cxLabel22;
    TcxTextEdit *edtIPHUrl1;
    TcxGroupBox *cxGroupBox2;
    TcxTextEdit *edtCarriageID2;
    TcxTextEdit *edtIPHDevID2;
    TcxLabel *cxLabel25;
    TcxLabel *cxLabel26;
    TcxLabel *cxLabel28;
    TcxTextEdit *edtIPHUrl2;
    TcxLabel *cxLabel31;
    TcxTextEdit *edtTrainUnitID1;
    TcxLabel *cxLabel21;
    TcxTextEdit *edtTrainUnitID2;
    TcxCheckBox *cbLocalRTSP;
    TcxCheckBox *cbBatchIPH;
    TTimer *TimerIPH;
    TcxCheckBox *cbCoverClosed;
    void __fastcall TimerCfgTimer(TObject *Sender);
    void __fastcall btnStartBrdClick(TObject *Sender);
    void __fastcall btnRegisterClick(TObject *Sender);
    void __fastcall btnUnregisterClick(TObject *Sender);
    void __fastcall btnReqCallClick(TObject *Sender);
    void __fastcall btnLoginClick(TObject *Sender);
    void __fastcall TimerATSTimer(TObject *Sender);
    void __fastcall btnGetSDSInfoClick(TObject *Sender);
    void __fastcall btnSendSDSReplyClick(TObject *Sender);
    void __fastcall cxButton1Click(TObject *Sender);
    void __fastcall btnGetMasterStatusClick(TObject *Sender);
    void __fastcall cxButton3Click(TObject *Sender);
    void __fastcall btnSetTrainIDClick(TObject *Sender);
    void __fastcall btnIPHApplyClick(TObject *Sender);
    void __fastcall cbLocalRTSPPropertiesChange(TObject *Sender);
    void __fastcall cbBatchIPHPropertiesChange(TObject *Sender);
    void __fastcall TimerIPHTimer(TObject *Sender);


private:    // User declarations
    void __fastcall GetDISCfg();
    void __fastcall LogMsg(AnsiString strMsgName, AnsiString strMsgInfo);
    void __fastcall Log(AnsiString strMsg);
    void Log(const char *szFormat, ...);

    void __fastcall OnDCSetTrainGroupReq(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnDCSetTrainIDReq(UINT32 ulSeqID, GJson &Json);

    void __fastcall OnGetDSCfgInfoRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnBroadcastStartReq(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnBroadcastEndReq(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnRequestCallRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnGroupCallEndReq(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnGetATSInfoRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnRegisterRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnGetLatestSDSInfo(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnSDSReplyRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall OnAutoSDSReplyReq(UINT32 ulSeqID, GJson &Json);

    void __fastcall Register(UINT32 ulDCType);
    void __fastcall ReportTrainInfo();
    void __fastcall ShowText(GJson &Json, const CHAR *szKey);
    void __fastcall GetMasterStatus();

public:     // User declarations
    __fastcall TFormMain(TComponent* Owner);
    void __fastcall showdata();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif
