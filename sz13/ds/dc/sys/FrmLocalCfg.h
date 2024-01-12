//---------------------------------------------------------------------------

#ifndef FrmLocalCfgH
#define FrmLocalCfgH
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
#include "cxPC.hpp"
#include "cxSpinEdit.hpp"
#include "cxTimeEdit.hpp"
#include "dxBarBuiltInMenu.hpp"
#include "cxClasses.hpp"
#include "cxLocalization.hpp"
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxScrollBox.hpp"
#include "cxCheckBox.hpp"
#include "cxRadioGroup.hpp"
//---------------------------------------------------------------------------
class TFormLocalCfg : public TSubForm
{
__published:    // IDE-managed Components
    TcxPageControl *pcMain;
    TcxTabSheet *tsHistory;
    TcxScrollBox *sbMain;
    TcxLabel *cxLabel1;
    TcxComboBox *cbMainSpeaker;
    TcxLabel *cxLabel2;
    TcxComboBox *cbMainMic;
    TcxLabel *cxLabel3;
    TcxComboBox *cbSlaveSpeaker;
    TcxComboBox *cbSlaveMic;
    TcxTabSheet *cxTabSheet1;
    TcxGroupBox *gbMain;
    TcxLabel *cxLabel5;
    TcxTextEdit *edtDISAddr;
    TcxLabel *cxLabel6;
    TcxTextEdit *edtMDCAddr;
    TcxLabel *cxLabel7;
    TcxLabel *cxLabel8;
    TcxTextEdit *edtATSAddr2;
    TcxTextEdit *edtATSAddr1;
    TcxLabel *cxLabel9;
    TcxLabel *cxLabel10;
    TcxTextEdit *edtNTPAddr;
    TcxTextEdit *edtOMCAddr;
    TcxTabSheet *tsSMS;
    TcxButton *btnCancelBasic;
    TcxButton *btnSetBasic;
    TcxButton *btnSetAudio;
    TcxButton *btnCancelAudio;
    TcxButton *btnRefresh;
    TcxGroupBox *gbAudioCfgMain;
    TcxGroupBox *gbBasicCfgMain;
    TcxGroupBox *gbBasicCfg;
    TcxGroupBox *gbAudioCfg;
    TcxButton *btnRefreshSpeaker;
    TcxLabel *cxLabel11;
    TcxComboBox *cbMonSpeaker;
    TcxLabel *cxLabel13;
    TcxLabel *cxLabel15;
    TcxSpinEdit *edtMainSpeakerVolume;
    TcxLabel *cxLabel4;
    TcxSpinEdit *edtMainMicVolume;
    TcxLabel *cxLabel12;
    TcxSpinEdit *edtSlaveSpeakerVolume;
    TcxLabel *cxLabel17;
    TcxSpinEdit *edtSlaveMicVolume;
    TcxLabel *cxLabel18;
    TcxSpinEdit *edtMonSpeakerVolume;
    TcxLabel *cxLabel14;
    TcxSpinEdit *edtRealBrdVolume;
    TcxLabel *cxLabel16;
    TcxSpinEdit *edtVoiceAlarmVolume;
    TcxTabSheet *tsParam;
    TcxGroupBox *gbServiceCfg;
    TcxButton *btnCancelParam;
    TcxButton *btnSetParam;
    TcxLabel *cxLabel20;
    TcxLabel *cxLabel21;
    TcxLabel *cxLabel22;
    TcxLabel *cxLabel23;
    TcxCheckBox *cbShowCallWin;
    TcxCheckBox *cbOriginalVideoFormat;
    TcxCheckBox *cbIPHRecMergeMode;
    TcxRadioButton *rbIPHVideoTCPMode;
    TcxRadioButton *rbIPHVideoUDPMode;
    TcxButton *btnDefaultParam;
    TcxGroupBox *gbServiceCfgMain;
    TcxLabel *cxLabel19;
    TcxSpinEdit *edtIPHMicVolumeRate;
    TcxTabSheet *tsSetPwd;
    TcxGroupBox *gbSetPwd;
    TcxGroupBox *cxGroupBox2;
    TcxButton *btnSetPwd;
    TcxLabel *cxLabel26;
    TcxLabel *cxLabel27;
    TcxLabel *cxLabel28;
    TcxLabel *cxLabel29;
    TcxTextEdit *edtUser;
    TcxTextEdit *edtOldPwd;
    TcxTextEdit *edtNewPwd;
    TcxTextEdit *edtNewPwd2;
    TcxLabel *cxLabel25;
    TcxTextEdit *edtNTPSyncPeriod;
    TcxButton *btnCancelPwd;
    TcxLabel *cxLabel24;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall btnSetAudioClick(TObject *Sender);
    void __fastcall btnRefreshClick(TObject *Sender);
    void __fastcall pcMainPageChanging(TObject *Sender, TcxTabSheet *NewPage, bool &AllowChange);
    void __fastcall gbBasicCfgMainResize(TObject *Sender);
    void __fastcall gbAudioCfgMainResize(TObject *Sender);
    void __fastcall btnRefreshSpeakerClick(TObject *Sender);
    void __fastcall btnCancelAudioClick(TObject *Sender);
    void __fastcall btnDefaultParamClick(TObject *Sender);
    void __fastcall btnSetParamClick(TObject *Sender);
    void __fastcall btnCancelParamClick(TObject *Sender);
    void __fastcall btnCancelBasicClick(TObject *Sender);
    void __fastcall gbServiceCfgMainResize(TObject *Sender);
    void __fastcall btnSetPwdClick(TObject *Sender);
    void __fastcall gbSetPwdResize(TObject *Sender);
    void __fastcall btnCancelPwdClick(TObject *Sender);

private:    // User declarations
    virtual void __fastcall InitSkin(){};
    void __fastcall Init(bool bInit=false);

public:     // User declarations
    __fastcall TFormLocalCfg(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormLocalCfg *FormLocalCfg;
//---------------------------------------------------------------------------
#endif
