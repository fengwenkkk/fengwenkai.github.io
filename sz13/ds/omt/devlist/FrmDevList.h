//---------------------------------------------------------------------------

#ifndef FrmDevListH
#define FrmDevListH
//---------------------------------------------------------------------------
#include "MDIForm.h"
#include "FrmCfg.h"

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "cxContainer.hpp"
#include "cxControls.hpp"
#include "cxEdit.hpp"
#include "cxGraphics.hpp"
#include "cxGroupBox.hpp"
#include "cxLookAndFeelPainters.hpp"
#include "cxLookAndFeels.hpp"
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
#include <Vcl.ExtCtrls.hpp>
#include "AdvSmoothPanel.hpp"
#include "cxButtons.hpp"
#include <Vcl.Menus.hpp>
#include "cxImageList.hpp"
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include "cxClasses.hpp"
#include "dxBar.hpp"
//---------------------------------------------------------------------------
class TFormDevList : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbMap;
    TcxButton *btnCAD1;
    TcxImageList *ImageListServer;
    TdxBarManager *BarManager;
    TdxBarPopupMenu *pmMain;
    TdxBarButton *pmiDevInfo;
    TcxButton *btnDC;
    TTimer *TimerRefresh;
    TcxImageList *ImageListDC;
    TcxButton *btnTX;
    TcxButton *btnFX;
    TcxButton *btnCAD2;
    TcxButton *btnREC;
    TcxButton *btnATS1;
    TcxButton *btnATS2;
    TcxButton *btnCN;
    TcxButton *btnCNOmc;
    TcxButton *btnNTP;
    TcxButton *btnCenterAlarm;
    TcxImageList *ImageListTX;
    TcxImageList *ImageListFX;
    TcxButton *btnTAU;
    TcxImageList *ImageListTAU;
    TPanel *PanelMap;
    void __fastcall TimerRefreshTimer(TObject *Sender);
    void __fastcall btnDevMouseEnter(TObject *Sender);
    void __fastcall btnDevClick(TObject *Sender);
    void __fastcall PanelMapResize(TObject *Sender);
private:    // User declarations
    void __fastcall OnGetClusterStatusRsp(UINT32 ulSeqID, GJson &Json);
public:     // User declarations
    __fastcall TFormDevList(TComponent* Owner);
    void __fastcall InitDevList();
    void __fastcall InitView(TcxButton *btnDev, UINT32 ulDevType);
    void __fastcall InitViewServer(TcxButton *btnDev, CHAR *szNeID, INT32 iOnlinSec, CHAR *szClusterStatus);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormDevList *FormDevList;
//---------------------------------------------------------------------------
#endif
