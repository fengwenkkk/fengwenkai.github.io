//---------------------------------------------------------------------------

#ifndef FrmNeInfoH
#define FrmNeInfoH
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
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxClasses.hpp"

typedef struct
{
    AnsiString strName;
    AnsiString strValue;
}NAME_VALUE_T;

//---------------------------------------------------------------------------
class TFormNeInfo : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbBasicInfo;
    TcxTreeList *tlInfo;
    TcxTreeListColumn *Column1;
    TcxTreeListColumn *Column2;
    TcxTreeListColumn *Column3;
    TcxGroupBox *gbCtrl;
    TcxButton *btnRefresh;
    TcxTreeListColumn *Column4;
    TcxButton *btnClose;
    TcxGroupBox *gbStatus;
    TcxTreeList *tlStatus;
    TcxTreeListColumn *cxTreeListColumn1;
    TcxTreeListColumn *cxTreeListColumn2;
    TcxTreeListColumn *cxTreeListColumn3;
    TcxTreeListColumn *cxTreeListColumn4;
    TcxStyleRepository *cxStyleRepository1;
    TcxStyle *StyleTitle;
    void __fastcall btnRefreshClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall btnCloseClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:    // User declarations
    AnsiString      m_strNeID;

    virtual void __fastcall InitSkin(){};

    void __fastcall ShowNeInfo(VECTOR<NAME_VALUE_T> &vInfo, VECTOR<NAME_VALUE_T> &vStatus);

public:     // User declarations

    void __fastcall Init(AnsiString strNeID);
    __fastcall TFormNeInfo(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TFormNeInfo *FormNeInfo;
//---------------------------------------------------------------------------
#endif
