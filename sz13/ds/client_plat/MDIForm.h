//---------------------------------------------------------------------------

#ifndef MDIFormH
#define MDIFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
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
#include "dxSkinsForm.hpp"
#include "dxSkinSharp.hpp"
#include "dxSkinSilver.hpp"
#include "dxSkinSpringTime.hpp"
#include "dxSkinStardust.hpp"
#include "dxSkinSummer2008.hpp"
#include "dxSkinValentine.hpp"
#include "dxSkinXmas2008Blue.hpp"
#include "cxTL.hpp"
#include "cxVGrid.hpp"
#include "dxBar.hpp"
#include "cxTrackBar.hpp"
#include "cxDropDownEdit.hpp"
#include "cxRadioGroup.hpp"
#include "cxTextEdit.hpp"
#include "dxStatusBar.hpp"
#include "cxProgressBar.hpp"
#include "cxLabel.hpp"
#include "dxLayoutLookAndFeels.hpp"
#include "cxColorComboBox.hpp"
#include "dxBevel.hpp"
#include "cxPC.hpp"
#include "cxCheckComboBox.hpp"
#include "cxFontNameComboBox.hpp"
#include "cxCheckBox.hpp"
#include "cxContainer.hpp"
#include "cxControls.hpp"
#include "cxEdit.hpp"
#include "cxGraphics.hpp"
#include "cxGroupBox.hpp"
#include "cxCheckGroup.hpp"
#include "cxLookAndFeelPainters.hpp"
#include <ExtCtrls.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <VCLTee.Chart.hpp>
#include <vector>
#include <map>

#include "g_include.h"
#include "InnerMsg.h"
#include "cxStyles.hpp"

typedef void __fastcall (__closure * INNER_MSG_HANDLER)(UINT8 *pucMsg, UINT32 ulMsgLen);
typedef void __fastcall (__closure * JSON_MSG_HANDLER)(UINT32 ulSeqID, GJson &Json);

typedef void __fastcall (__closure * OnTreeListCustomDrawDataCell_FUNC)(TcxCustomTreeList *Sender, TcxCanvas *ACanvas,
          TcxTreeListEditCellViewInfo *AViewInfo, bool &ADone);

//---------------------------------------------------------------------------
class TSubForm : public TForm
{
__published:    // IDE-managed Components
    TPanel *MainPanel;

private:    // User declarations
    std::vector<Cxlookandfeels::TcxLookAndFeel*>        m_vLookAndFeel;
    void __fastcall _SetSkin(TcxTreeList *TreeList, bool bMainTree, bool bSave);

public:     // User declarations
    __fastcall TSubForm(TComponent* Owner, bool bRegisterSkin=true);

    virtual void __fastcall InitSkin();
    void __fastcall SetFont();
    void __fastcall SetSkin();

    void __fastcall SetParentCtrl(TWinControl *Ctrl);
    int __fastcall ShowModal(void);

    UINT32  m_ulPrivilege;
    std::map<UINT32, HANDLE>        m_mInnerMsgHandler;
    IntSet                          m_sJsonMsgType;
    std::map<AnsiString, HANDLE>    m_mJsonMsgHandler;

    AnsiString                      m_strJsonMsgType;
    GJson                           m_JsonMsg;

    std::map<AnsiString, OnTreeListCustomDrawDataCell_FUNC>   m_mOnTreeListCustomDrawDataCell;

protected:
    BEGIN_MESSAGE_MAP
    MESSAGE_HANDLER(WM_INNER_MSG, TMessage, OnInnerMsg)
    END_MESSAGE_MAP(TForm)

    void __fastcall OnInnerMsg(TMessage Message);
    void __fastcall OnJsonMsg(TMessage Message);

    bool __fastcall InitMsgHandler(UINT32 ulMsgType, INNER_MSG_HANDLER pfHandler);
    bool __fastcall InitMsgHandler(UINT32 ulMsgType, AnsiString strMsgName, JSON_MSG_HANDLER pfHandler);
    INNER_MSG_HANDLER __fastcall GetInnerMsgHandler(UINT32 ulMsgType);
    AnsiString __fastcall GetJsonMsgType(){return m_strJsonMsgType;};
    GJson& __fastcall GetJsonMsg(){return m_JsonMsg;};

    void __fastcall OnTreeListGetNodeHeight(TcxCustomTreeList *Sender, TcxTreeListNode *ANode, int &AHeight);
    void __fastcall OnTreeListColumnSizeChanged(TcxCustomTreeList *Sender, TcxTreeListColumn *AColumn);
    void __fastcall TreeListCustomDrawBackgroundCell(TcxCustomTreeList *Sender,
                              TcxCanvas *ACanvas, TcxTreeListCustomCellViewInfo *AViewInfo, bool &ADone);
    void __fastcall OnTreeListCustomDrawDataCell(TcxCustomTreeList *Sender, TcxCanvas *ACanvas,
          TcxTreeListEditCellViewInfo *AViewInfo, bool &ADone);
    void __fastcall SetSkin(Cxlookandfeels::TcxLookAndFeel* LookAndFeel, AnsiString strSkinName);
    void __fastcall SetSkin(TdxSkinController* SkinController);
    void __fastcall SetSkin(TdxLayoutCxLookAndFeel* LayoutCxLookAndFeel);
    void __fastcall SetSkin(TcxPageControl* PageControl);
    void __fastcall SetSkin(Cxlookandfeels::TcxLookAndFeel* LookAndFeel);
    void __fastcall SetSkin(Cxlookandfeels::TcxLookAndFeelController* LookAndFeelController);
    void __fastcall SetSkin(TdxBarManager* BarManager);
    void __fastcall SetSkin(TcxTreeList *TreeList, bool bMainTree=false);
    void __fastcall SetSkin(TcxVerticalGrid *Grid);
    void __fastcall SetSkin(TcxLabel *Label);
    void __fastcall SetSkin(TcxGroupBox *GroupBox);
    void __fastcall SetSkin(TcxButton *Button);
    void __fastcall SetSkin(TcxTrackBar *TrackBar);
    void __fastcall SetSkin(TcxColorComboBox *ComboBox);
    void __fastcall SetSkin(TcxComboBox *ComboBox);
    void __fastcall SetSkin(TcxCheckComboBox *CheckComboBox);
    void __fastcall SetSkin(TcxRadioButton *RadioButton);
    void __fastcall SetSkin(TcxTextEdit *TextEdit);
    void __fastcall SetSkin(TcxMaskEdit *TextEdit);
    void __fastcall SetSkin(TcxSpinEdit *SpinEdit);
    void __fastcall SetSkin(TcxCheckListBox *CheckListBox);
    void __fastcall SetSkin(TcxCheckGroup *CheckGroup);
    void __fastcall SetSkin(TcxDateEdit *DateEdit);
    void __fastcall SetSkin(TcxTimeEdit *TimeEdit);
    void __fastcall SetSkin(TcxCheckBox *CheckBox);
    void __fastcall SetSkin(TcxButtonEdit *ButtonEdit);
    void __fastcall SetSkin(TcxProgressBar *ProgressBar);
    void __fastcall SetSkin(TdxStatusBar *StatusBar);
    void __fastcall SetSkin(TcxMemo *Memo);
    void __fastcall SetSkin(TcxEditStyle *EditStyle);
    void __fastcall SetSkin(TChart *pChart);
    void __fastcall SetSkin(TcxFontNameComboBox *cbSystemFont);
    void __fastcall SetSkin(TLabel *Label);
    void __fastcall SetSkin(TdxBevel *Bevel);

    void __fastcall InitSkin(TComponent *Ctrl);
};
//---------------------------------------------------------------------------
extern PACKAGE TSubForm *SubForm;

extern TcxStyle *g_TreeListInactiveStyle;

extern void InitTreeListStyle();
extern void InitNeTree(TcxTreeList *Tree, TcxTreeListNode *RootNode=NULL, bool bClear=true);
extern TcxTreeListNode* InitPerfTree(TcxTreeListNode *RootNode);
extern void SetNodeImage(TcxTreeListNode *Node, int iImageIndex);
extern TcxTreeListNode* AddTreeNode(TcxTreeList *Tree, AnsiString strName, int iImageIndex);
extern TcxTreeListNode* AddTreeNode(TcxTreeListNode *ParentNode, AnsiString strName, int iImageIndex);
extern void SetTreeNodeName(TcxTreeListNode *Node, AnsiString strName);

extern void GetFormList(VECTOR<TSubForm*> &vForm);

extern void InitSystemFont();
extern TColor GetDefaultTextColor();
extern void SetDefaultFontSize(int iFontSize);
extern void SetDefaultFontName(AnsiString strFontName);
extern bool IsShowModal();
extern void SetDefaultSkinName(AnsiString strSkinName);
extern AnsiString GetDefaultSkinName();

extern void SetTreeListContentColor(TColor Color);
extern void SetTreeListSkinName(AnsiString strSkinName);

//---------------------------------------------------------------------------
#endif
