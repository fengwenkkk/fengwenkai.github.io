//---------------------------------------------------------------------------

#ifndef FrmPISH
#define FrmPISH
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
#include "cxPCdxBarPopupMenu.hpp"
#include "dxSkinscxPCPainter.hpp"
#include "cxGroupBox.hpp"
#include "MDIForm.h"
//---------------------------------------------------------------------------

class TFormPIS : public TSubForm
{
__published:    // IDE-managed Components
    TdxSkinController *dxSkinController1;
    TTimer *Timer;
    TcxButton *btnIPHApply;
    TcxGroupBox *cxGroupBox1;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TcxTextEdit *edtTrainUnitID1;
    TcxTextEdit *edtCarriageID1;
    TcxTextEdit *edtIPHDevID1;
    TcxGroupBox *cxGroupBox2;
    TLabel *Label4;
    TLabel *Label5;
    TLabel *Label6;
    TcxTextEdit *edtTrainUnitID2;
    TcxTextEdit *edtCarriageID2;
    TcxTextEdit *edtIPHDevID2;
    TcxButton *btnIPHAlarm1;
    TcxButton *btnIPHhandup1;
    TcxButton *btnIPHhandup2;
    TcxButton *btnIPHAlarm2;
    TTimer *TimerSendPCM;
    TLabel *Label7;
    TcxTextEdit *edtDCAddr1;
    TLabel *Label8;
    TcxTextEdit *edtDCAddr2;
    void __fastcall btnIPHApplyClick(TObject *Sender);
    void __fastcall btnIPHhandup1Click(TObject *Sender);
    void __fastcall btnIPHhandup2Click(TObject *Sender);
    void __fastcall TimerSendPCMTimer(TObject *Sender);
    void __fastcall TimerTimer(TObject *Sender);

private:    // User declarations
    UINT8   m_aucMsgBuf[64*1024];

    void __fastcall OnPISMsg(UINT8 *pucMsg, UINT32 ulMsgLen);
    void __fastcall OnIPHTalkApplyRsp(IPH_TALK_APPLY_RSP_T *pstMsg);
    void __fastcall OnIPHAudioDownload(IPH_PCM_DATA_T *pstMsg);
    void __fastcall OnIPHTalkStopRsp(IPH_TALK_STOP_RSP_T *pstMsg);
    void __fastcall SendPCMData();
public:     // User declarations
    __fastcall TFormPIS(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormPIS *FormPIS;
//---------------------------------------------------------------------------
#endif
