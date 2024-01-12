//---------------------------------------------------------------------------

#ifndef DlgNewSMSH
#define DlgNewSMSH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>

#include "Dialog.h"
#include "cxContainer.hpp"
#include "cxControls.hpp"
#include "cxEdit.hpp"
#include "cxGraphics.hpp"
#include "cxLabel.hpp"
#include "cxLookAndFeelPainters.hpp"
#include "cxLookAndFeels.hpp"
#include "cxTextEdit.hpp"
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
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "cxCustomData.hpp"
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxGroupBox.hpp"
#include "cxInplaceContainer.hpp"
#include "cxPC.hpp"
#include "cxStyles.hpp"
#include "cxTL.hpp"
#include "cxTLdxBarBuiltInMenu.hpp"
#include "dxBarBuiltInMenu.hpp"
#include "cxButtonEdit.hpp"
#include "cxButtons.hpp"
#include "cxMaskEdit.hpp"
#include <Vcl.Menus.hpp>
#include "cxImageList.hpp"
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include "cxCheckBox.hpp"
#include "cxClasses.hpp"
#include "cxEditRepositoryItems.hpp"
#include "cxExtEditRepositoryItems.hpp"
#include "cxMemo.hpp"
#include "cxRichEdit.hpp"
#include "dxBevel.hpp"
//---------------------------------------------------------------------------
class TDialogNewSMS : public TDialogForm
{
__published:    // IDE-managed Components
    TOpenDialog *OpenDialog;
    TcxImageList *ImageListMain;
    TcxEditRepository *EditRepository;
    TcxEditRepositoryCheckComboBox *erccbStation;
    TcxEditRepositoryComboBoxItem *ercbStation;
    TcxEditRepositoryComboBoxItem *ercbDepot;
    TcxGroupBox *gbReceiver;
    TcxPageControl *pcMain;
    TcxTabSheet *tsUser;
    TcxTreeList *tlUser;
    TcxTreeListColumn *ColumnUserName;
    TcxTreeListColumn *ColumnUserID;
    TcxTreeListColumn *ColumnUserType;
    TcxTabSheet *tsGroup;
    TcxTreeList *tlGroup;
    TcxTreeListColumn *tlUserColumn1;
    TcxTreeListColumn *tlUserColumn2;
    TcxTreeListColumn *tlUserColumn3;
    TcxButton *btnSend;
    TcxButton *btnCancel;
    TcxGroupBox *gbPreview;
    TcxRichEdit *edtSDSInfo;
    TcxGroupBox *gbAddFile;
    TcxButtonEdit *edtSMSFile;
    TcxGroupBox *gbUser;
    TcxMemo *mmUser;
    TcxTextEdit *edtSendUserName;
    TcxLabel *cxLabel1;
    TdxBevel *dxBevel1;
    void __fastcall edtSMSFilePropertiesButtonClick(TObject *Sender, int AButtonIndex);
    void __fastcall btnSendClick(TObject *Sender);
    void __fastcall btnCancelClick(TObject *Sender);
    void __fastcall tlUserCustomDrawDataCell(TcxCustomTreeList *Sender, TcxCanvas *ACanvas,
          TcxTreeListEditCellViewInfo *AViewInfo, bool &ADone);
    void __fastcall tlUserGetNodeImageIndex(TcxCustomTreeList *Sender, TcxTreeListNode *ANode,
          TcxTreeListImageIndexType AIndexType, TImageIndex &AIndex);
    void __fastcall FormShow(TObject *Sender);



private:    // User declarations
    void __fastcall InitGroup(VECTOR<INT64> &vGroupID);
    void __fastcall InitUser(VECTOR<UINT32> &vUserID);
    virtual void __fastcall InitSkin();

    VECTOR<UINT32>  m_vUserID;
    VECTOR<INT64>   m_vGroupID;

    AnsiString      m_strSMSTitle;
    AnsiString      m_strSMFile;
    //void __fastcall Init(VECTOR<UINT32> &vUserID, VECTOR<INT64> &vGroupID);

public:     // User declarations
   void __fastcall Init(VECTOR<UINT32> &vUserID, VECTOR<INT64> &vGroupID);
   bool __fastcall GetUserList(VECTOR<UINT32> &vUser);
   void __fastcall Get(AnsiString &strSMSTitle, AnsiString &strSMSFile, VECTOR<UINT32> &vUserID, VECTOR<INT64> &vGroupID);
    __fastcall TDialogNewSMS(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TDialogNewSMS *DialogNewSMS;
//---------------------------------------------------------------------------
#endif
