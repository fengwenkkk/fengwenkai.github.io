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
#include "dxSkinLilian.hpp"
#include "dxSkinsCore.hpp"
#include "dxSkinsDefaultPainters.hpp"
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
#include "cxClasses.hpp"
#include "cxMemo.hpp"
#include "dxBarBuiltInMenu.hpp"
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
#include "cxLabel.hpp"
#include "cxCheckBox.hpp"
#include "cxCalendar.hpp"
#include "dxStatusBar.hpp"
#include "cxButtonEdit.hpp"
#include <Vcl.Dialogs.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <Vcl.MPlayer.hpp>
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
//---------------------------------------------------------------------------

class TFormPIS : public TSubForm
{
__published:    // IDE-managed Components
    TcxButton *btnIPHApply;
    TcxGroupBox *cxGroupBox1;
    TcxTextEdit *edtCarriageID1;
    TcxTextEdit *edtIPHDevID1;
    TcxGroupBox *cxGroupBox2;
    TcxTextEdit *edtCarriageID2;
    TcxTextEdit *edtIPHDevID2;
    TcxButton *btnIPHhandup1;
    TcxButton *btnIPHhandup2;
    TTimer *TimerSendPCM;
    TcxTextEdit *edtDCAddr1;
    TcxTextEdit *edtDCAddr2;
    TcxPageControl *pcMain;
    TcxTabSheet *tsIPH;
    TcxTabSheet *tsRecBrd;
    TPanel *Panel1;
    TcxButton *btnRecBrdRsp;
    TcxTextEdit *edtCurrRecID;
    TcxMemo *mmBrd;
    TPanel *Panel2;
    TTimer *TimerInit;
    TcxTabSheet *cxTabSheet1;
    TcxTextEdit *edtCurrAddr;
    TcxTextEdit *edtGateway;
    TcxTextEdit *edtSAAddr;
    TcxLabel *cxLabel2;
    TcxLabel *cxLabel3;
    TcxLabel *cxLabel5;
    TcxLabel *cxLabel6;
    TcxLabel *cxLabel7;
    TcxLabel *cxLabel8;
    TdxSkinController *dxSkinController1;
    TcxLabel *cxLabel9;
    TcxLabel *cxLabel10;
    TcxLabel *cxLabel11;
    TcxLabel *cxLabel12;
    TcxLabel *cxLabel13;
    TcxLabel *cxLabel14;
    TcxButton *btnStartDiscover;
    TcxComboBox *cbLocalAddr;
    TcxLabel *cxLabel15;
    TcxTextEdit *edtIPHUrl1;
    TcxLabel *cxLabel16;
    TcxTextEdit *edtIPHUrl2;
    TcxLabel *cxLabel1;
    TcxTextEdit *edtTrainUnitID;
    TcxTabSheet *cxTabSheet2;
    TcxCheckBox *cbAutoMode;
    TcxCheckBox *cbOpenDevCover;
    TcxTabSheet *cxTabSheet3;
    TcxLabel *cxLabel4;
    TcxTextEdit *edtSelfCheckInfo;
    TcxLabel *cxLabel17;
    TcxTextEdit *edtHeartBeatTime;
    TcxLabel *cxLabel18;
    TcxTextEdit *edtIPHAudio1;
    TcxLabel *cxLabel19;
    TcxTextEdit *edtIPHStatus1;
    TcxLabel *cxLabel20;
    TcxTextEdit *edtIPHAudio2;
    TcxLabel *cxLabel21;
    TcxTextEdit *edtIPHStatus2;
    TcxMemo *mmGetTrainInfo;
    TcxTabSheet *cxTabSheet4;
    TcxButton *btnStartRec;
    TcxButton *btnStopRec;
    TTimer *TimerRec;
    TcxTextEdit *edtRecTime;
    TcxLabel *cxLabel22;
    TcxTextEdit *edtTrainAddr;
    TLabel *LabelInfo;
    TcxButton *cxButton1;
    TcxButton *cxButton2;
    TcxButton *cxButton3;
    TcxButton *cxButton4;
    TcxButton *cxButton5;
    TcxButton *cxButton6;
    TcxTabSheet *tsWavAna;
    TcxButton *btnAna;
    TcxLabel *cxLabel23;
    TcxButtonEdit *edtWavFile;
    TOpenDialog *OpenDialog;
    TcxLabel *cxLabel24;
    TcxLabel *cxLabel25;
    TcxTextEdit *edtSampleBytes;
    TcxLabel *cxLabel26;
    TcxTextEdit *edtWavLength;
    TcxTextEdit *edtChanNum;
    TcxButton *btnCreateTcpSocket;
    TcxTabSheet *cxTabSheet5;
    TcxButton *btnPlayPCM;
    TTimer *TimerPlayPCM;
    TcxButton *btnStopPCM;
    TcxButton *btnInitPCM;
    TcxButton *btnPausePCM;
    TcxButton *cxButton7;
    TcxMemo *mmAudioList;
    TcxButton *btnSetVolume;
    TcxTextEdit *edtAudioVolume;
    TcxTextEdit *edtAudioDev;
    TcxButton *btnGetVolume;
    TcxButton *cxButton8;
    TcxButton *btnSaveChart;
    TcxButton *cxButton10;
    TTimer *TimerTestMicVolume;
    TcxTextEdit *edtCurrMicVolume;
    TLabel *Label1;
    TcxMemo *mmMicVolume;
    TMediaPlayer *MediaPlayer1;
    TMediaPlayer *MediaPlayer;
    TcxButton *cxButton11;
    TcxButton *cxButton12;
    TChart *cht;
    TcxTabSheet *tsRealBrd;
    TPanel *Panel3;
    TcxButton *btnCreatSock;
    TcxButton *btnRealBrdStart;
    TcxButton *btnBrdBusy;
    TcxTextEdit *edtCurrRealBrdID;
    TcxLabel *cxLabel27;
    TcxCheckBox *cbIPHRunningStatus;
    TcxCheckBox *cbPAMaster;
    TcxComboBox *cbPARunningStatus;
    TcxLabel *cxLabel28;
    TcxLabel *LabelMicHint;
    TcxCheckBox *cbAECMode;
    TcxButton *cxButton13;
    TcxButton *btnPlayWav;
    TcxButton *btnOpenBmpFile;
    TcxButton *btnIPHDevReset;
    TcxTextEdit *edtIPHRecvAudioAddr;
    TcxLabel *cxLabel29;
    TcxTextEdit *edtIPHRecvAudioPort;
    TcxLabel *cxLabel30;
    TcxTextEdit *edtPARecvAudioPort;
    TcxLabel *cxLabel31;
    TcxTextEdit *edtRealBrdCarriageID;
    TcxLabel *cxLabel32;
    TcxLabel *cxLabel33;
    TcxTextEdit *edtIPHDoorID2;
    TcxLabel *cxLabel34;
    TcxTextEdit *edtIPHDoorID1;
    TcxButton *btnIPHTalkAck;
    TcxCheckBox *cbIPHPauseFail;
    void __fastcall btnIPHApplyClick(TObject *Sender);
    void __fastcall btnIPHhandup1Click(TObject *Sender);
    void __fastcall btnIPHhandup2Click(TObject *Sender);
    void __fastcall TimerSendPCMTimer(TObject *Sender);
    void __fastcall btnRecBrdRspClick(TObject *Sender);
    void __fastcall TimerInitTimer(TObject *Sender);
    void __fastcall btnStartDiscoverClick(TObject *Sender);
    void __fastcall btnStartRecClick(TObject *Sender);
    void __fastcall TimerRecTimer(TObject *Sender);
    void __fastcall btnStopRecClick(TObject *Sender);
    void __fastcall cxButton1Click(TObject *Sender);
    void __fastcall cxButton2Click(TObject *Sender);
    void __fastcall cxButton3Click(TObject *Sender);
    void __fastcall cxButton4Click(TObject *Sender);
    void __fastcall cxButton5Click(TObject *Sender);
    void __fastcall cxButton6Click(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall edtWavFilePropertiesButtonClick(TObject *Sender, int AButtonIndex);
    void __fastcall btnAnaClick(TObject *Sender);
    void __fastcall btnCreateTcpSocketClick(TObject *Sender);
    void __fastcall btnPlayPCMClick(TObject *Sender);
    void __fastcall TimerPlayPCMTimer(TObject *Sender);
    void __fastcall btnStopPCMClick(TObject *Sender);
    void __fastcall btnInitPCMClick(TObject *Sender);
    void __fastcall btnPausePCMClick(TObject *Sender);
    void __fastcall cxButton7Click(TObject *Sender);
    void __fastcall cxButton8Click(TObject *Sender);
    void __fastcall btnSetVolumeClick(TObject *Sender);
    void __fastcall btnGetVolumeClick(TObject *Sender);
    void __fastcall btnSaveChartClick(TObject *Sender);
    void __fastcall cxButton10Click(TObject *Sender);
    void __fastcall TimerTestMicVolumeTimer(TObject *Sender);
    void __fastcall cxButton11Click(TObject *Sender);
    void __fastcall cxButton12Click(TObject *Sender);
    void __fastcall btnCreatSockClick(TObject *Sender);
    void __fastcall btnRealBrdStartClick(TObject *Sender);
    void __fastcall btnBrdBusyClick(TObject *Sender);
    void __fastcall cbAECModePropertiesChange(TObject *Sender);
    void __fastcall cxButton13Click(TObject *Sender);
    void __fastcall btnPlayWavClick(TObject *Sender);
    void __fastcall btnOpenBmpFileClick(TObject *Sender);
    void __fastcall btnIPHDevResetClick(TObject *Sender);
    void __fastcall btnIPHTalkAckClick(TObject *Sender);


private:    // User declarations
    UINT8   m_aucMsgBuf[64*1024];

    bool __fastcall SendMsg(UINT16 usMsgType, void *pvMsg, UINT32 ulMsgLen);

    void __fastcall InitLocalAddr();
    void __fastcall InitPARunningStatus();
    void __fastcall OnPISMsg(UINT8 *pucMsg, UINT32 ulMsgLen);
    void __fastcall OnSAGetTrainInfoReq();
    void __fastcall OnGetSelfCheckReq(PIS_SELFCHECK_STATUS_REQ_T *pstMsg);
    //void __fastcall OnDiscoverMsg(UINT8 *pucMsg, UINT32 ulMsgLen);
    void __fastcall OnIPHTalkApplyRsp(IPH_TALK_APPLY_RSP_T *pstMsg);
    void __fastcall OnIPHAudioDownload(IPH_PCM_DATA_T *pstMsg);
    void __fastcall OnIPHTalkStopRsp(IPH_TALK_STOP_RSP_T *pstMsg);
    void __fastcall OnPAStartRecBrdReq(PA_REC_BRD_REQ_T *pstMsg);
    void __fastcall OnLTEPauseIPHTalk(IPH_TALK_PAUSE_REQ_T *pstMsg);
    void __fastcall OnPAGetCfgReq();
    void __fastcall SendPCMData();

    void __fastcall OnPATCPMsg(UINT8 *pucMsg, UINT32 ulMsgLen);
    BOOL __fastcall SendPATCPMsg(UINT16 usMsgType, void *pvMsg, UINT32 ulMsgLen);
    void __fastcall OnRealBrdStartReq(PA_REAL_BRD_REQ_T *pstMsg);
    void __fastcall OnRealBrdStopReq(PA_STOP_REAL_BRD_REQ_T *pstMsg);
    void __fastcall OnSAGetPASysInfoReq();
    void __fastcall OnSAGetIPHSysInfoReq();
    void __fastcall OnSASelfCheckReportedReq(PIS_SELFCHECK_STATUS_REQ_T *MsgData);
public:     // User declarations
    __fastcall TFormPIS(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormPIS *FormPIS;
//---------------------------------------------------------------------------
#endif
