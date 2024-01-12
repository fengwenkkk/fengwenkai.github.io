// ---------------------------------------------------------------------------

#ifndef DlgRecBrdH
#define DlgRecBrdH
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
#include "cxRadioGroup.hpp"
#include "cxDropDownEdit.hpp"
#include "cxMaskEdit.hpp"
#include "dxBevel.hpp"
#include "cxCustomData.hpp"
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxImageComboBox.hpp"
#include "cxImageList.hpp"
#include "cxInplaceContainer.hpp"
#include "cxStyles.hpp"
#include "cxTL.hpp"
#include "cxTLdxBarBuiltInMenu.hpp"
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include "cxSpinEdit.hpp"

// ---------------------------------------------------------------------------
class TDialogRecBrd : public TDialogForm
{
__published: // IDE-managed Components
    TcxButton *btnBrd;
    TcxButton *btnClose;
    TTimer *TimerCheck;
    TcxLabel *cxLabel2;
    TcxComboBox *cbRec;
    TdxBevel *dxBevel1;
    TcxLabel *cxLabel3;
    TcxLabel *cxLabel1;
    TcxImageList *ImageListTrain;
    TcxLabel *cxLabel5;
    TcxTreeList *tlMain;
    TcxTreeListColumn *ColumnTrain;
    TcxTreeListColumn *ColumnBrdStatus;
    TcxButton *btnBrdFail;
    TcxTreeListColumn *tlBrdInfoColumnTrainID;
    TcxTreeListColumn *ColumnStationID;
    TcxTreeListColumn *ColumnTrainDirection;
    TcxTextEdit *edtBrdType;
    TcxSpinEdit *edtBrdTime;
    TcxLabel *cxLabel4;

    void __fastcall btnBrdClick(TObject *Sender);
    void __fastcall btnCloseClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall ColumnBrdStatusGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall tlMainCustomDrawDataCell(TcxCustomTreeList *Sender, TcxCanvas *ACanvas,
          TcxTreeListEditCellViewInfo *AViewInfo, bool &ADone);
    void __fastcall ColumnTrainGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall TimerCheckTimer(TObject *Sender);

private: // User declarations
    GJsonTupleParam     m_TupleParam;
    Dict                m_DictRecBrdCfg;

    virtual void __fastcall InitSkin();

    void __fastcall InitTree();
    void __fastcall InitRecBrdInfo();
    void __fastcall TrainBrd(VECTOR<TRAIN_INFO_T*> &vTrain);
    void __fastcall OnDCStartRecBrdRsp(UINT32 ulSeqID, GJson &Json);

public: // User declarations
    __fastcall TDialogRecBrd(TComponent* Owner);

    void __fastcall Init();
    void __fastcall TrainBrd(VECTOR<UINT32> &vTrain);
    void __fastcall TrainBrd(UINT32 ulTrainID);
    void __fastcall LineBrd(UINT32 ulBrdType);
    void __fastcall InterLockStationBrd(UINT32 ulStationID);
    void __fastcall StationBrd(UINT32 ulStationID);
};

// ---------------------------------------------------------------------------
extern PACKAGE TDialogRecBrd *DialogRecBrd;
// ---------------------------------------------------------------------------

void TrainRecBrd(VECTOR<UINT32> &vTrain);
void TrainRecBrd(UINT32 ulTrainID);
void LineRecBrd(UINT32 ulBrdType);
void InterLockStationRecBrd(UINT32 ulStationID);
void StationRecBrd(UINT32 ulStationID);

#endif
