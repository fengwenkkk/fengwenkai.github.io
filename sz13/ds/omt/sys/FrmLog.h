//---------------------------------------------------------------------------

#ifndef FrmLogH
#define FrmLogH
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
class TFormLog : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbMain;
    TSaveDialog *SaveDialog;
    TcxTreeList *tlMain;
    TcxTreeListColumn *Column1;
    TcxTreeListColumn *ColumnTime;
    TcxTreeListColumn *ColumnUser;
    TcxGroupBox *gbCtrl;
    TcxButton *btnQuery;
    TcxLabel *cxLabel1;
    TcxDateEdit *deStartTime;
    TcxLabel *cxLabel2;
    TcxDateEdit *deEndTime;
    TcxLabel *cxLabel3;
    TcxTextEdit *edtDCUserID;
    TcxButton *btnNext;
    TcxButton *btnPrev;
    TcxButton *btnExport;
    TcxButton *btnPrint;
    TcxTreeListColumn *ColumnInfo;
    TcxTreeListColumn *tlMainColumn5;
    void __fastcall btnQueryClick(TObject *Sender);
    void __fastcall btnExportClick(TObject *Sender);
    void __fastcall btnPrintClick(TObject *Sender);
    void __fastcall btnNextClick(TObject *Sender);
    void __fastcall ColumnTimeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall btnPrevClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);

private:    // User declarations
    UINT32      m_ulStartTime;
    UINT32      m_ulEndTime;

    virtual void __fastcall InitSkin(){};

    void __fastcall InitView(VECTOR<OMT_OPER_LOG_INFO_T> &vInfo);

public:     // User declarations
    __fastcall TFormLog(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormLog *FormLog;
//---------------------------------------------------------------------------
#endif
