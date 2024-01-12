//---------------------------------------------------------------------------

#ifndef FrmSDSManageH
#define FrmSDSManageH
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
#include "cxCalendar.hpp"
#include "cxDateUtils.hpp"
#include "cxMaskEdit.hpp"
#include "dxCore.hpp"
#include <Vcl.ComCtrls.hpp>
#include "cxDataControllerConditionalFormattingRulesManagerDialog.hpp"
#include "cxImage.hpp"
#include "cxSplitter.hpp"

#include "FrmMediaPlayer.h"

//---------------------------------------------------------------------------
class TFormSDSManage : public TSubForm
{
__published:    // IDE-managed Components
    TcxGroupBox *gbMain;
    TcxGroupBox *gbCtrl;
    TcxButton *btnQuery;
    TcxButton *btnExport;
    TcxLabel *cxLabel1;
    TcxLabel *cxLabel2;
    TcxLabel *cxLabel3;
    TcxComboBox *cbSDSType;
    TSaveDialog *SaveDialog;
    TcxLabel *cxLabel5;
    TcxTextEdit *edtSendUserName;
    TcxLabel *cxLabel6;
    TcxTextEdit *edtKeyword;
    TcxButton *btnPrint;
    TcxTreeList *tlMain;
    TcxTreeListColumn *ColumnID;
    TcxTreeListColumn *ColumnSendTime;
    TcxTreeListColumn *ColumnSDSType;
    TcxTreeListColumn *ColumnForamt;
    TcxTreeListColumn *ColumnIndex;
    TcxTreeListColumn *ColumnSender;
    TcxTreeListColumn *ColumnConfirmRate;
    TcxTreeListColumn *ColumnContent;
    TcxTreeListColumn *ColumnNeedReply;
    TcxDateEdit *deStartTime;
    TcxDateEdit *deEndTime;
    TcxSplitter *cxSplitter1;
    TcxGroupBox *gbDetail;
    TPanel *PanelSDSInfo;
    TcxMemo *mmSDSInfo;
    TcxTreeList *tlSDSReply;
    TcxTreeListColumn *cxTreeListColumn1;
    TcxTreeListColumn *ColumnRecvTime;
    TcxTreeListColumn *ColumnConfirmTime;
    TPanel *PanelMediaPlayer;
    TPanel *PanelImage;
    TcxImage *ImageAttach;
    TcxSplitter *SplitterReply;
    TcxTreeListColumn *ColumnAttachFile;
    TcxButton *btnOpenAttachFile;

    void __fastcall btnQueryClick(TObject *Sender);
    void __fastcall btnExportClick(TObject *Sender);
    void __fastcall tlMainDblClick(TObject *Sender);
    void __fastcall btnPrintClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall tlMainFocusedNodeChanged(TcxCustomTreeList *Sender, TcxTreeListNode *APrevFocusedNode, TcxTreeListNode *AFocusedNode);
    void __fastcall ColumnNeedReplyGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);
    void __fastcall btnOpenAttachFileClick(TObject *Sender);
    void __fastcall ColumnSDSTypeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value);

private:    // User declarations
    bool        m_bInitView;
    TFormMediaPlayer    *m_FormMediaPlayer;

    Dict                *m_DictSDSType;

    virtual void __fastcall InitSkin(){};

    void __fastcall ShowSDSInfo(VECTOR<SDS_INFO_T> &vInfo);

    void __fastcall ShowDetail();
    void __fastcall OnGetSDSReplyRsp(UINT32 ulSeqID, GJson &Json);
    void __fastcall AutoShowSMSAttach(TcxTreeListNode *Node);

public:     // User declarations
    __fastcall TFormSDSManage(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TFormSDSManage *FormSDSManage;
//---------------------------------------------------------------------------
#endif
