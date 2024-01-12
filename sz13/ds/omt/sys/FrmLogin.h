//---------------------------------------------------------------------------

#ifndef FrmLoginH
#define FrmLoginH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "cxButtons.hpp"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <Menus.hpp>
#include "cxGraphics.hpp"
#include "cxLookAndFeelPainters.hpp"
#include "cxLookAndFeels.hpp"

#include "MDIForm.h"
#include "cxContainer.hpp"
#include "cxControls.hpp"
#include "cxEdit.hpp"
#include "cxTextEdit.hpp"
#include "cxMaskEdit.hpp"
#include "dxSkinsForm.hpp"
#include "cxLabel.hpp"
#include "cxDropDownEdit.hpp"
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include "AdvSmoothLabel.hpp"
#include "AdvSmoothPanel.hpp"
#include "cxImage.hpp"
#include "cxProgressBar.hpp"
#include "dxGDIPlusClasses.hpp"
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
#include "AdvCircularProgress.hpp"
#include "cxHint.hpp"
#include "dxCustomHint.hpp"
#include "dxScreenTip.hpp"
#include "dxActivityIndicator.hpp"
#include "cxCustomData.hpp"
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxInplaceContainer.hpp"
#include "cxStyles.hpp"
#include "cxTL.hpp"
#include "cxTLdxBarBuiltInMenu.hpp"

//---------------------------------------------------------------------------
class TFormLogin : public TForm
{
__published:    // IDE-managed Components
    TdxSkinController *SkinController;
    TcxProgressBar *pbInit;
    TAdvSmoothPanel *PanelLogin;
    TcxTextEdit *edtUser;
    TcxTextEdit *edtPwd;
    TLabel *lbPwd;
    TLabel *lbUser;
    TcxButton *btnExit;
    TcxButton *btnLogin;
    TLabel *LabelTitle;
    TImage *Image1;
    void __fastcall btnLoginClick(TObject *Sender);
    void __fastcall btnExitClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall edtUserKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall edtPwdKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall edtUserPropertiesChange(TObject *Sender);
    void __fastcall edtPwdPropertiesChange(TObject *Sender);



private:    // User declarations
    Dict        m_DictDCType;

    void __fastcall LoginReady();

public:     // User declarations
    __fastcall TFormLogin(TComponent* Owner);
    VOID _fastcall  Init();
    bool __fastcall LoginCheck();
    void __fastcall SetInitProgress(UINT32 ulPos, UINT32 ulMaxPos, AnsiString strInitInfo);
    /*virtual*/ void __fastcall InitSkin();
    void __fastcall SetInitMode();
    void __fastcall InitProgress();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormLogin *FormLogin;
extern void SetInitInfo(AnsiString strInitInfo);
extern void SetInitInfoEx(AnsiString strInitInfo);
extern void SetInitOver();
//---------------------------------------------------------------------------
#endif
