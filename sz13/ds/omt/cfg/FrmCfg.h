//---------------------------------------------------------------------------

#ifndef FrmCfgH
#define FrmCfgH
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
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxImageComboBox.hpp"
#include "cxImageList.hpp"
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include "cxCheckBox.hpp"
#include "cxMaskEdit.hpp"
#include "cxClasses.hpp"
#include "AdvSmoothButton.hpp"
//---------------------------------------------------------------------------
class TFormCfg : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbMain;
    TcxTreeList *tlMain;
    TcxTreeListColumn *ColumnNeID;
    TcxTreeListColumn *ColumnDevName;
    TcxGroupBox *gbCtrl;
    TcxButton *btnDelDev;
    TcxButton *btnAddDev;
    TcxButton *btnRefresh;
    TcxButton *btnSetDev;
    TcxButton *btnImportDev;
    TOpenDialog *OpenDialog;
    TTimer *TimerCheck;
    TcxTreeListColumn *ColumnDevMAC;
    TcxTreeListColumn *ColumnOnlineTime;
    TcxTreeListColumn *ColumnOnlineSec;
    TcxTreeListColumn *ColumnSVer;
    TcxTreeListColumn *ColumnDevType;
    TcxTreeListColumn *ColumnDevIP;
    TcxTreeListColumn *ColumnStatus;
    TcxTreeListColumn *ColumnHVer;
    TcxImageList *ImageListDevStatus;
    TcxCheckBox *cbShowOfflineDev;
    TcxButton *btnDevInfo;
    TcxTreeListColumn *ColumnModel;
	TcxLabel *btnDevType;
    TcxComboBox *cbDevType;
    TcxLabel *cxLabel1;
    TcxTextEdit *edtSearch;
    TcxButton *btnQuery;
    TdxBarManager *BarManager;
    TdxBarPopupMenu *pmMain;
    TdxBarButton *pmiReboot;
    TdxBarButton *pmiDevInfo;
    TdxBarButton *pmiDevOperLog;
    TdxBarSeparator *dxBarSeparator1;
    TdxBarButton *pmiAddDev;
    TdxBarButton *pmiSetDev;
    TdxBarButton *pmiDelDev;
    TdxBarButton *pmiImportDev;
    TcxTreeListColumn *ColumnAndroidVer;
    TcxTreeListColumn *ColumnRadioVer;
    TdxBarButton *pmiDevDetail;
    TSaveDialog *SaveDialog;
    TcxButton *btnExportDev;
	TdxBarSeparator *dxBarSeparator2;
	TdxBarSeparator *dxBarSeparator3;
	TdxBarSubItem *dxBarSubItem1;
	TdxBarSeparator *dxBarSeparator4;
	TdxBarButton *pmiRefresh;
    void __fastcall btnAddDevClick(TObject *Sender);
    void __fastcall btnDelDevClick(TObject *Sender);
    void __fastcall btnRefreshClick(TObject *Sender);
    void __fastcall btnSetDevClick(TObject *Sender);
    void __fastcall btnImportDevClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall ColumnDevTypeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall ColumnOnlineTimeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall btnDevInfoClick(TObject *Sender);
    void __fastcall pmiOperLogClick(TObject *Sender);
    void __fastcall cbShowOfflineDevClick(TObject *Sender);
    void __fastcall btnQueryClick(TObject *Sender);
    void __fastcall edtSearchKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall cbDevTypePropertiesChange(TObject *Sender);
    void __fastcall TimerCheckTimer(TObject *Sender);
    void __fastcall pmiRebootClick(TObject *Sender);
    void __fastcall tlMainDblClick(TObject *Sender);
    void __fastcall edtSearchPropertiesChange(TObject *Sender);
    void __fastcall pmMainPopup(TObject *Sender);
	void __fastcall pmiDevDetailClick(TObject *Sender);
    void __fastcall btnExportDevClick(TObject *Sender);
	void __fastcall ColumnStatusGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);


private:    // User declarations
    void __fastcall OnGetNeBasicInfoRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall InitNode(TcxTreeListNode *Node, NE_BASIC_INFO_T *pstInfo);
    virtual void __fastcall InitSkin(){};

    bool IsDevListSame(VectorMap<AnsiString, NE_BASIC_INFO_T> &mInfo);
    //bool  CheckDevInfo(AnsiString &strDevName,AnsiString &strMAC);
public:     // User declarations
    __fastcall TFormCfg(TComponent* Owner);
    void __fastcall InitView();
    VectorMap<AnsiString, NE_BASIC_INFO_T>  m_mNEBasicInfo;
    //bool  CheckDevInfo(AnsiString &strDevName,AnsiString &strMAC);

};
//---------------------------------------------------------------------------
extern PACKAGE TFormCfg *FormCfg;
//---------------------------------------------------------------------------
#endif
