//---------------------------------------------------------------------------

#ifndef FrmVideoWinH
#define FrmVideoWinH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "cxButtons.hpp"
#include "cxContainer.hpp"
#include "cxControls.hpp"
#include "cxEdit.hpp"
#include "cxGraphics.hpp"
#include "cxGroupBox.hpp"
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
#include "MDIForm.h"
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include "AdvSmoothPanel.hpp"
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
#include "cxClasses.hpp"
#include "dxBar.hpp"
#include "cxImage.hpp"
#include "dxGDIPlusClasses.hpp"
#include "cxCustomData.hpp"
#include "cxInplaceContainer.hpp"
#include "cxStyles.hpp"
#include "cxTextEdit.hpp"
#include "cxTL.hpp"
#include "cxTLdxBarBuiltInMenu.hpp"
//---------------------------------------------------------------------------
class TFormVideoWin : public TSubForm
{
__published:    // IDE-managed Components
    TcxButton *btnFullWin;
    TcxButton *btnStop;
    TcxGroupBox *gbMain;
    TcxButton *btnStartVideoDispatch;
    TcxButton *btnStopVideoDispatch;
    TAdvSmoothPanel *PanelMain;
    TdxBarPopupMenu *pmMain;
    TcxGroupBox *gbVideo;
    TdxBarManager *BarManager;
    TdxBarButton *pmiStop;
    TdxBarButton *pmiVideoTransfer;
    TdxBarSeparator *dxBarSeparator1;
    TdxBarButton *pmiOriginalFormat;
    TdxBarSeparator *dxBarSeparator2;
    TdxBarSeparator *dxBarSeparator3;
    TdxBarButton *pmiSetVideoSource;
    TdxBarSeparator *dxBarSeparator4;
    TdxBarSeparator *dxBarSeparator5;
    TdxBarButton *pmiVWallStart;
    TdxBarButton *pmiVWallStop;
    void __fastcall PanelMainResize(TObject *Sender);
    void __fastcall btnStopClick(TObject *Sender);
    void __fastcall btnFullWinClick(TObject *Sender);
    void __fastcall btnStopVideoDispatchClick(TObject *Sender);
    void __fastcall btnStartVideoDispatchClick(TObject *Sender);
    void __fastcall pmiOriginalFormatClick(TObject *Sender);
    void __fastcall pmMainPopup(TObject *Sender);
    void __fastcall gbVideoClick(TObject *Sender);
    void __fastcall gbVideoDragOver(TObject *Sender, TObject *Source, int X, int Y,
          TDragState State, bool &Accept);
    void __fastcall gbVideoDragDrop(TObject *Sender, TObject *Source, int X, int Y);
    void __fastcall pmiSetVideoSourceClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall pmiVWallStartClick(TObject *Sender);
    void __fastcall pmiVWallStopClick(TObject *Sender);


private:    // User declarations
    bool    m_bSelected;
    AnsiString  m_strCaption;
    bool    m_bVideoGroupCall;  // 是否是视频组呼
    bool    m_bCloseVideoGroupCall; // 是否要关闭视频组呼
    INT64   m_i64GroupID;
    bool    m_bPlaying;
    UINT32  m_ulUserID;
    UINT32  m_ulVideoFormat;
    UINT32  m_ulCameraType;
    BOOL    m_bNeedConfirm;
    BOOL    m_bHasVoice;
    UINT32  m_ulCallType;
    IntSet  m_sSelectUser;
    bool    m_bIsOutCalling; // 是否是呼出
    INT32   m_iVideoChanID;

    void __fastcall Init(bool bCloseTitle=true);
    void __fastcall MyOnMessage(tagMSG &Msg, bool &Handled);

public:     // User declarations
    __fastcall TFormVideoWin(TComponent* Owner);

    TcxGroupBox* __fastcall GetVideoPlayPanel(){return gbVideo;};

    UINT32 __fastcall GetCallType(){return m_ulCallType;};
    UINT32 __fastcall GetUserID(){return m_ulUserID;};
    bool __fastcall IsPlaying();
    bool __fastcall Call(UINT32 ulUserID, UINT32 ulVideoFormat, UINT32 ulCameraType, BOOL bNeedConfirm, BOOL bHasVoice);
    bool __fastcall Play(UINT32 ulUserID, UINT32 ulVideoFormat, UINT32 ulCameraType, BOOL bNeedConfirm, BOOL bHasVoice);
    bool __fastcall Recv(UINT32 ulCallType, UINT32 ulUserID, UINT32 ulVideoFormat, UINT32 ulCameraType);
    bool __fastcall Stop();
    bool __fastcall Play();
    void __fastcall SetVideoPlayWindow();
    void __fastcall InitVideoGroupCall(INT64 i64GroupID, UINT32 ulVideoFormat, UINT32 ulCameraType, BOOL bNeedConfirm, BOOL bHasVoice);
    INT64 __fastcall GetGroupID(){return m_i64GroupID;};
    bool __fastcall IsOutCalling(){return m_bIsOutCalling;};
};
//---------------------------------------------------------------------------
extern PACKAGE TFormVideoWin *FormVideoWin;

extern TFormVideoWin* GetSelectedGroupCallVideoWin();
//---------------------------------------------------------------------------
#endif
