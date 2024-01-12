//---------------------------------------------------------------------------

#ifndef DlgNewSDSH
#define DlgNewSDSH
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
#include "cxCheckBox.hpp"
#include "cxDropDownEdit.hpp"
#include "cxMaskEdit.hpp"
#include "cxButtonEdit.hpp"
#include "cxCustomData.hpp"
#include "cxInplaceContainer.hpp"
#include "cxStyles.hpp"
#include "cxTL.hpp"
#include "cxTLdxBarBuiltInMenu.hpp"
#include "cxMemo.hpp"
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
#include "cxGroupBox.hpp"
#include "cxRichEdit.hpp"
#include "cxClasses.hpp"
#include "cxDBExtLookupComboBox.hpp"
#include "cxExtEditRepositoryItems.hpp"
#include "cxCheckComboBox.hpp"
#include "cxEditRepositoryItems.hpp"
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxImageList.hpp"
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include "cxPC.hpp"
#include "dxBarBuiltInMenu.hpp"
//---------------------------------------------------------------------------
class TDialogNewSDS : public TDialogForm
{
__published:    // IDE-managed Components
    TcxButton *btnOK;
    TcxButton *btnCancel;
    TcxLabel *LabelTemplate;
    TcxComboBox *cbTitle;
    TdxBevel *dxBevel1;
    TcxCheckBox *cbNeedReply;
    TcxGroupBox *gbReceiver;
    TcxGroupBox *gbInfo;
    TcxTreeList *tlInfo;
    TcxTreeListColumn *Column1;
    TcxTreeListColumn *Column2;
    TcxTreeListColumn *Column3;
    TcxTreeListColumn *Column4;
    TcxGroupBox *gbPreview;
    TcxRichEdit *edtSDSInfo;
    TcxEditRepository *EditRepository;
    TcxEditRepositoryCheckComboBox *erccbStation;
    TcxEditRepositoryComboBoxItem *ercbStation;
    TcxEditRepositoryComboBoxItem *ercbDepot;
    TcxTreeList *tlUser;
    TcxTreeListColumn *ColumnUserName;
    TcxTreeListColumn *ColumnUserID;
    TcxTreeListColumn *ColumnUserType;
    TcxLabel *LabelSDSType;
    TcxComboBox *cbSDSType;
    TcxLabel *LabelSender;
    TcxTextEdit *edtSendUserName;
    TcxImageList *ImageListMain;
    TcxPageControl *pcMain;
    TcxTabSheet *tsGroup;
    TcxTabSheet *tsUser;
    TcxTreeList *tlGroup;
    TcxTreeListColumn *tlUserColumn1;
    TcxTreeListColumn *tlUserColumn2;
    TcxTreeListColumn *tlUserColumn3;
    TcxGroupBox *gbUser;
    TcxMemo *mmUser;
    void __fastcall btnCancelClick(TObject *Sender);
    void __fastcall btnOKClick(TObject *Sender);
    void __fastcall cbTitlePropertiesChange(TObject *Sender);
    void __fastcall Column2GetEditProperties(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          TcxCustomEditProperties *&EditProperties);
    void __fastcall tlInfoDataChanged(TObject *Sender);
    void __fastcall cbSDSTypePropertiesChange(TObject *Sender);
    void __fastcall tlInfoEdited(TcxCustomTreeList *Sender, TcxTreeListColumn *AColumn);
    void __fastcall EditValueChanged(TObject *Sender);
    void __fastcall tlUserCustomDrawDataCell(TcxCustomTreeList *Sender, TcxCanvas *ACanvas,
          TcxTreeListEditCellViewInfo *AViewInfo, bool &ADone);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall tlUserGetNodeImageIndex(TcxCustomTreeList *Sender, TcxTreeListNode *ANode,
          TcxTreeListImageIndexType AIndexType, TImageIndex &AIndex);
    void __fastcall tlUserNodeCheckChanged(TcxCustomTreeList *Sender, TcxTreeListNode *ANode,
          TcxCheckBoxState AState);
    void __fastcall tlGroupNodeCheckChanged(TcxCustomTreeList *Sender, TcxTreeListNode *ANode,
          TcxCheckBoxState AState);


private:    // User declarations
    SDS_INFO_T      m_stInfo;
    VECTOR<UINT32>  m_vUserID;
    VECTOR<INT64>   m_vGroupID;
    Dict            *m_DictSDSType;

    void __fastcall SetPreviewInfo();
    void __fastcall InitUser(VECTOR<UINT32> &vUserID);
    void __fastcall InitGroup(VECTOR<INT64> &vGroupID);
    virtual void __fastcall InitSkin();
    void __fastcall InitTitle(UINT32 ulSDSType);
    void __fastcall UpdateUser();
    bool __fastcall GetUserList(VECTOR<UINT32> &vUser);
    void __fastcall ShowSDSInfo();
    void __fastcall HideSDSInfo();

public:     // User declarations
    __fastcall TDialogNewSDS(TComponent* Owner);
    void __fastcall Init(VECTOR<UINT32> &vUserID, VECTOR<INT64> &vGroupID);
    void __fastcall Get(SDS_INFO_T &stInfo, VECTOR<UINT32> &vUserID, VECTOR<INT64> &vGroupID);
};
//---------------------------------------------------------------------------
extern PACKAGE TDialogNewSDS *DialogNewSDS;
//---------------------------------------------------------------------------
#endif
