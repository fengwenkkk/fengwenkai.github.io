// ---------------------------------------------------------------------------

#ifndef DlgRealBrdH
#define DlgRealBrdH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "cxButtons.hpp"
#include "cxContainer.hpp"
#include "cxControls.hpp"
#include "cxEdit.hpp"
#include "cxGraphics.hpp"
#include "cxLabel.hpp"
#include "cxLookAndFeelPainters.hpp"
#include "cxLookAndFeels.hpp"
#include "cxTrackBar.hpp"
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
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include "cxTextEdit.hpp"

#include "Dialog.h"
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
#include "dxBevel.hpp"
#include "cxCustomData.hpp"
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxInplaceContainer.hpp"
#include "cxStyles.hpp"
#include "cxTL.hpp"
#include "cxTLdxBarBuiltInMenu.hpp"
#include "cxDropDownEdit.hpp"
#include "cxMaskEdit.hpp"
#include "cxCheckBox.hpp"
#include "cxImageComboBox.hpp"
#include "cxImageList.hpp"
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>

class PCMDataBuffer
{
public:
    PCMDataBuffer(UINT32 ulMaxBufSize);
    VOID Init();
    VOID PushData(UINT8 *pucData, UINT32 ulLen);
    VOID GetData(UINT8 *pucData, UINT32 ulMaxLen, UINT32 &ulLen);
    UINT32 GetDataLen();

private:
    GMutex  m_Mutex;

    INT64   m_i64StartTime;     // 第一包数据
    INT64   m_i64LastSendTime;  // 上一次发送时间

    UINT8   *m_pucDataBuf;
    UINT32  m_ulCurrDataLen;
    UINT32  m_ulMaxBufSize;
};


#define BRD_STATUS_WAIT     1
#define BRD_STATUS_READY    2
#define BRD_STATUS_FAIL     3

typedef struct
{
    TRAIN_INFO_T    stTrain;
    UINT32  ulUserID1;
    UINT32  ulUserID2;
    UINT32  ulBrdStatus1;
    UINT32  ulBrdStatus2;
}TRAIN_BRD_INFO_T;

// ---------------------------------------------------------------------------
class TDialogRealBrd : public TDialogForm
{
__published: // IDE-managed Components
    TTimer *TimerBar;
    TTimer *TimerCheck;
    TcxButton *btnEnd;
    TcxButton *btnPTT;
    TdxBevel *dxBevel1;
    TcxLabel *cxLabel4;
    TcxTextEdit *edtBrdType;
    TcxLabel *cxLabel6;
    TcxLabel *cxLabel7;
    TcxProgressBar *pbReady;
    TcxProgressBar *pbTime;
    TcxTreeList *tlBrdInfo;
    TcxTreeListColumn *Column2;
    TcxTreeListColumn *ColumnBrdStatus;
    TcxTreeListColumn *ColumnTrainUntiID;
    TcxLabel *cxLabel1;
    TcxTreeListColumn *tlBrdInfoColumn4;
    TcxTreeListColumn *tlBrdInfoColumn5;
    TcxButton *btnStart;
    TcxCheckBox *cbPTTBrdMode;
    TcxTreeListColumn *tlBrdInfoColumn6;
    TcxTreeListColumn *tlBrdInfoColumn7;
    TcxLabel *cxLabel2;
    TcxImageList *ImageListTrain;
    TcxTextEdit *edtBrdStatus;
    TcxTrackBar *tbVolume;
    TTimer *TimerSend;
    void __fastcall btnEndClick(TObject *Sender);
    void __fastcall TimerBarTimer(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall TimerCheckTimer(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall btnStartClick(TObject *Sender);
    void __fastcall cbPTTBrdModePropertiesChange(TObject *Sender);
    void __fastcall btnPTTMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall btnPTTMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall ColumnBrdStatusGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall btnPTTMouseEnter(TObject *Sender);
    void __fastcall btnPTTMouseLeave(TObject *Sender);
    void __fastcall tlBrdInfoCustomDrawDataCell(TcxCustomTreeList *Sender, TcxCanvas *ACanvas,
          TcxTreeListEditCellViewInfo *AViewInfo, bool &ADone);
    void __fastcall TimerSendTimer(TObject *Sender);

private: // User declarations
    UINT32  m_ulStartTime;
    UINT32  m_ulBrdType;
    UINT32  m_ulTimeout;
    bool    m_bStartBroadcast;

    virtual void __fastcall InitSkin();

    void __fastcall StopBrd();
    void __fastcall NotifyTrainBrdStart();
    BOOL __fastcall Init(UINT32 ulBrdType, VECTOR<TRAIN_BRD_INFO_T> &vBrdTrain, INT64 i64GroupID);

    void __fastcall OnBroadcastStartRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall CheckTrainBrdGroup();
    void __fastcall OnSaveRealBrdInfoRsp(UINT32 ulSeqID, GJson &Json);

public:  // User declarations
    __fastcall TDialogRealBrd(TComponent *Owner);
    BOOL __fastcall Brd(UINT32 ulBrdType, VECTOR<TRAIN_INFO_T *> &vBrdTrain);
    BOOL __fastcall LineBrd(UINT32 ulBrdType);
    void __fastcall NotifyTrainBrdEnd();

    void __fastcall OnPTTDial();
    void __fastcall OnPTTRelease();
};

// ---------------------------------------------------------------------------
extern PACKAGE TDialogRealBrd *DialogRealBrd;

BOOL InterLockStationRealBrd(UINT32 ulStationID);
BOOL StationRealBrd(UINT32 ulStationID);
BOOL TrainRealBrd(UINT32 ulTrainUnitID);
BOOL TrainRealBrd(VECTOR<UINT32> &vTrainUnitID);
BOOL MainLineTrainRealBrd();
BOOL UpLineTrainRealBrd();
BOOL DownLineTrainRealBrd();

// ---------------------------------------------------------------------------
#endif
