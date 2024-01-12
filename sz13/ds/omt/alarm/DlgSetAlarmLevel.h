//---------------------------------------------------------------------------

#ifndef DlgSetAlarmLevelH
#define DlgSetAlarmLevelH
//---------------------------------------------------------------------------
#include "Dialog.h"
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "cxButtons.hpp"
#include "cxContainer.hpp"
#include "cxControls.hpp"
#include "cxDropDownEdit.hpp"
#include "cxEdit.hpp"
#include "cxGraphics.hpp"
#include "cxLabel.hpp"
#include "cxLookAndFeelPainters.hpp"
#include "cxLookAndFeels.hpp"
#include "cxMaskEdit.hpp"
#include "cxTextEdit.hpp"
#include "dxBevel.hpp"
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
#include "dxSkinLilian.hpp"
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
#include "dxSkinsCore.hpp"
#include "dxSkinsDefaultPainters.hpp"
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
#include <Vcl.Menus.hpp>
#include "cxCheckBox.hpp"

//---------------------------------------------------------------------------
class TDialogSetAlarmLevel : public TDialogForm
{
__published:    // IDE-managed Components
    TcxComboBox *cbAlarmLevel;
    TcxLabel *lbAlarmType;
    TcxLabel *cxLabel3;
    TdxBevel *dxBevel1;
    TcxButton *btnOK;
    TcxButton *btnCancel;
    TcxLabel *cxLabel4;
    TcxCheckBox *cbIgnore;
	TcxComboBox *cbAlarmID;
	TcxLabel *lbDevType;
	TcxTextEdit *edtDevType;
    void __fastcall btnCancelClick(TObject *Sender);
    void __fastcall btnOKClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
private:    // User declarations
	UINT32 m_ulAlarmID;
    UINT32 m_ulDevType;
	UINT32 m_ulAlarmLevel;
	UINT32 m_ulSetAlarmLevel;
	UINT32 m_ulSetAlarmID;
public:     // User declarations
	__fastcall TDialogSetAlarmLevel(TComponent* Owner);
	void __fastcall Init(UINT32 ulDevType);
	void __fastcall Init(UINT32 ulDevType, UINT32 ulAlarmType,UINT32 ulAlarmLevel);
	void __fastcall GetSetAlarmCfg(UINT32 &ulSetAlarmLevel, BOOL &bSetAlarmStat);
	void __fastcall GetSetAlarmCfg(UINT32 &ulSetAlarmID, UINT32 &ulSetAlarmLevel, BOOL &bSetAlarmStatus);
};
//---------------------------------------------------------------------------
extern PACKAGE TDialogSetAlarmLevel *DialogSetAlarmLevel;
//---------------------------------------------------------------------------
#endif
