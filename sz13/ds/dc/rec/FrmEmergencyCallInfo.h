//---------------------------------------------------------------------------

#ifndef FrmEmergencyCallInfoH
#define FrmEmergencyCallInfoH
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
//---------------------------------------------------------------------------
class TFormEmergencyCallInfo : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbMain;
    TSaveDialog *SaveDialog;
    TcxPageControl *pcMain;
    TcxTabSheet *tsP2P;
    TcxTabSheet *tsPTT;
    TcxTabSheet *tsHistory;
    TcxTreeList *tlMain;
    TcxTreeListColumn *Column1;
    TcxTreeListColumn *Column2;
    TcxTreeListColumn *tlMainColumn3;
    TcxTreeListColumn *tlMainColumn5;
    TcxTreeListColumn *tlMainColumn6;
    TcxGroupBox *gbCtrl;
    TcxButton *btnQuery;
    TcxLabel *cxLabel1;
    TcxDateEdit *deStartTime;
    TcxLabel *cxLabel2;
    TcxDateEdit *deEndTime;
    TcxLabel *cxLabel3;
    TcxLabel *cxLabel4;
    TcxComboBox *cbCDRType;
    TcxTextEdit *edtResourceID;
    TcxButton *btnNext;
    TcxButton *btnPrev;
    TcxButton *btnExport;
    TcxButton *btnPrint;
    TcxTreeList *tlGroupCall;
    TcxTreeListColumn *cxTreeListColumn2;
    TcxTreeListColumn *cxTreeListColumn3;
    TcxTreeListColumn *cxTreeListColumn4;
    TcxTreeListColumn *cxTreeListColumn5;
    TcxTreeList *tlP2PCall;
    TcxTreeListColumn *cxTreeListColumn7;
    TcxTreeListColumn *cxTreeListColumn8;
    TcxTreeListColumn *cxTreeListColumn9;
    TcxTreeListColumn *cxTreeListColumn10;
    TcxTreeListColumn *tlP2PColumn5;
    TcxTreeListColumn *tlMainColumn7;
    TcxTreeListColumn *tlMainColumn8;
    TcxTreeListColumn *tlGroupCallColumn5;
    TcxTreeListColumn *tlMainColumn9;
    TcxTabSheet *tsEmergencyCall;
    TcxTabSheet *tsBrd;
    TTimer *TimerInit;
    void __fastcall btnExportClick(TObject *Sender);
    void __fastcall cxTreeListColumn9GetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall tlP2PColumn5GetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall cxTreeListColumn4GetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall tlMainColumn5GetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall btnPrintClick(TObject *Sender);
    void __fastcall btnNextClick(TObject *Sender);
    void __fastcall Column2GetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall btnPrevClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall TimerInitTimer(TObject *Sender);

private:    // User declarations
    UINT32      m_ulStartTime;
    UINT32      m_ulEndTime;

    virtual void __fastcall InitSkin(){};

    void __fastcall InitView(VECTOR<P2P_CALL_INFO_T> &vInfo);
    void __fastcall InitView(VECTOR<GROUP_CALL_INFO_T> &vInfo);

public:     // User declarations
    __fastcall TFormEmergencyCallInfo(TComponent* Owner);

    void __fastcall NewP2PCall(UINT32 ulCallType, UINT32 ulCaller, UINT32 ulCallee, UINT32 ulStatus);
    void __fastcall NewGroupCall(UINT32 ulCallType, INT64 i64GroupID, UINT32 ulSpeaker);

};
//---------------------------------------------------------------------------
extern PACKAGE TFormEmergencyCallInfo *FormEmergencyCallInfo;
//---------------------------------------------------------------------------
#endif
