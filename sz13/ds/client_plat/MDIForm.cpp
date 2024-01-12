//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"
#include "GCommon.h"

#include "MDIForm.h"
#include "cxTL.hpp"
#include "TableCfg.h"
#include "Resource.h"
#include "ClientUtil.h"

#pragma package(smart_init)
#pragma link "cxLookAndFeels"

#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinsForm"

#pragma link "cxCheckBox"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxGroupBox"
#pragma link "cxCheckGroup"
#pragma link "cxLookAndFeelPainters"
#pragma resource "*.dfm"

TFont       *g_SystemFont = NULL;
bool        g_bShowModal = false;

AnsiString  g_strSkinName = "Office2010Blue";

static AnsiString  g_strDefaultFontName = "ÀŒÃÂ";//µ»œﬂ";//ÀŒÃÂ";   Œ¢»Ì—≈∫⁄
static AnsiString  g_strDefaultTimeFontName = "ÀŒÃÂ";//"Tahoma";
static AnsiString  g_strTreeListFontName = "ÀŒÃÂ";
static AnsiString  g_strGroupBoxFontName = "Œ¢»Ì—≈∫⁄";
static AnsiString  g_strPageControlFontName = "Œ¢»Ì—≈∫⁄";
static AnsiString  g_strButtonFontName = "Œ¢»Ì—≈∫⁄";
static AnsiString  g_strTreeListHeaderFontName = "Œ¢»Ì—≈∫⁄";

static int         g_iDefaultFontSize = 11;
static int         g_iTreeListFontSize = 12;
static int         g_iTreeListHeaderFontSize = 12;
static int         g_iPageControlFontSize = 12;
static int         g_iButtonFontSize = 12;
static int         g_iGroupBoxFontSize = 12;
static int         g_iTreeListNodeHeight = 26;

static TcxStyle     *g_StyleTreeList = NULL;
static TcxStyle     *g_StyleTreeListSelection = NULL;
static TcxStyle     *g_StyleTreeListContentEven = NULL;
static TcxStyle     *g_StyleTreeListHeader = NULL;

static TColor       g_clTreeListSelection   = (TColor)RGB(110,150,200); //RGB(90,123,165); //RGB(0x31, 0x6A, 0xC5);
static TColor       g_clTreeListContentEven = (TColor)RGB(0xBF, 0xCD, 0xDE); //RGB(0xCB, 0xDE, 0xF3); // RGB(0xCF, 0xDD, 0xEE);

static TColor       g_clDisabledColor = (TColor)RGB(0xBF, 0xCD, 0xDE); // 0xBF, 0xCD, 0xDE

static TColor       g_clDefaultTextColor = (TColor)RGB(0x1E, 0x39, 0x5B);

static MAP<AnsiString, TSubForm*>       s_mFormMap;

std::map<AnsiString, AnsiString>    g_mSkin;

AnsiString  g_strTreeListSkinName = "Blue";
AnsiString  g_strPageControlSkinName = "";//"Office2007Blue";

VECTOR<TcxTreeList*>    m_vMainTreeList;

extern BOOL InitInnerMsg(UINT16 usMsgType, TForm *Form);
extern TdxSmartGlyph* GetTreeListHeaderGlyph(int iTreeListNodeHeight);

typedef struct
{
    const char      *szSkinName;
    UINT32          ulSkinColor;
}SKIN_COLOR_T;

SKIN_COLOR_T        g_astSkinColor[] = {
    {"Black"                    , 0xE6E6E6 },
    {"Blue"                     , 0xFFEFE3 },
    {"Blueprint"                , 0xB07756 },
    {"Caramel"                  , 0xF1F5F7 },
    {"Coffee"                   , 0xF0F8FB },
    {"DarkRoom"                 , 0x505050 },
    {"DarkSide"                 , 0x858585 },
    {"DevExpressDarkStyle"      , 0x585858 },
    {"DevExpressStyle"          , 0xEFECEB },
    {"Foggy"                    , 0xECE6E5 },
    {"GlassOceans"              , 0xFFFBF1 },
    {"iMaginary"                , 0xFAFAFA },
    {"Lilian"                   , 0xFBF9F9 },
    {"LiquidSky"                , 0xFFFBF1 },
    {"LondonLiquidSky"          , 0xF7F5F3 },
    {"McSkin"                   , 0xE8E8E8 },
    {"Metropolis"               , 0xFFFFFF },
    {"MetropolisDark"           , 0x333333 },
    {"MoneyTwins"               , 0xFEF1E3 },
    {"Office2007Black"          , 0xF2F1F0 },
    {"Office2007Blue"           , 0xFFEFE3 },
    {"Office2007Green"          , 0xE5EFDE },
    {"Office2007Pink"           , 0xE2DEEE },
    {"Office2007Silver"         , 0xF2F1F0 },
    {"Office2010Black"          , 0xFFFFFF },
    {"Office2010Blue"           , 0xEEDDCF },
    {"Office2010Silver"         , 0xF1EDE9 },
    {"Office2013DarkGray"       , 0xE3E3E3 },
    {"Office2013LightGray"      , 0xF6F6F6 },
    {"Office2013White"          , 0xFFFFFF },
    {"Office2016Colorful"       , 0xF0F0F0 },
    {"Office2016Dark"           , 0x696969 },
    {"Pumpkin"                  , 0x858585 },
    {"Seven"                    , 0xFFEFE3 },
    {"SevenClassic"             , 0xF1F1F1 },
    {"Sharp"                    , 0xCFC0BE },
    {"SharpPlus"                , 0x61514E },
    {"Silver"                   , 0xF2F1F0 },
    {"SpringTime"               , 0xCFFAE0 },
    {"Stardust"                 , 0xF8F2EF },
    {"Summer2008"               , 0xD8EFF8 },
    {"TheAsphaltWorld"          , 0xFDFDFD },
    {"TheBezier"                , 0xFFFFFF },
    {"Valentine"                , 0xEAEAFF },
    {"VisualStudio2013Blue"     , 0xE9DBD6 },
    {"VisualStudio2013Dark"     , 0x302D2D },
    {"VisualStudio2013Light"    , 0xF2EEEE },
    {"VS2010"                   , 0xD8C7BC },
    {"Whiteprint"               , 0xFFFFFF },
    {"Xmas2008Blue"             , 0xFFEFE3 },
};

static TcxStyle* GetStyleText()
{
    static  TcxStyle    *Style = NULL;

    if (!Style)
    {
        Style = new TcxStyle(Application);

        Style->Font = g_SystemFont;

        Style->TextColor = g_clDefaultTextColor;
    }

    return Style;
}

TColor GetDefaultTextColor()
{
    return g_clDefaultTextColor;
}

TColor GetTreeListTextColor()
{
 //   TColor  ColorText = clWindowText;
    TColor  ColorText = g_clDefaultTextColor;//(TColor)RGB(0x1E, 0x39, 0x5B);

    return ColorText;
}

VOID SetTreeListParam(int iFontSize, int iNodeHeight)
{
    g_iTreeListFontSize = iFontSize;
    g_iTreeListNodeHeight = iNodeHeight;
}

/*
static TcxStyle* GetTreeListHeaderStyle()
{
    static TcxStyle *Style = NULL;

    if (!Style)
    {
        Style = new TcxStyle(Application);
        Style->Font->Height = g_iTreeListNodeHeight;
    }

    return Style;
} */

TColor GetSkinColor(AnsiString strSkinName)
{
    for (UINT32 i=0; i<ARRAY_SIZE(g_astSkinColor); i++)
    {
        if (strSkinName == g_astSkinColor[i].szSkinName)
        {
            return (TColor)g_astSkinColor[i].ulSkinColor;
        }
    }

    return (TColor)0xFFFFFF;
}

TColor GetDefaultSkinColor()
{
    return GetSkinColor(g_strSkinName);
}

void SetTreeListSkinName(AnsiString strSkinName)
{
    g_strTreeListSkinName = strSkinName;
}

void SetTreeListContentColor(TColor Color)
{
    if (!g_StyleTreeList)
    {
        g_StyleTreeList = new TcxStyle(Application);

        g_StyleTreeList->Color = Color;
    }
}

void SetDefaultSkinName(AnsiString strSkinName)
{
    g_strSkinName = strSkinName;
}

AnsiString GetDefaultSkinName()
{
    return g_strSkinName;
}

AnsiString GetAppSkinName()
{
    if (g_mSkin.find("Application") == g_mSkin.end())
    {
        return g_strSkinName;
    }

    return g_mSkin["Application"];
}

AnsiString GetSkinName(AnsiString strCtrlName)
{
    if (!g_strTreeListSkinName.IsEmpty() &&
        strCtrlName == "TcxTreeList")
    {
        return g_strTreeListSkinName;
    }

    if (!g_strPageControlSkinName.IsEmpty() &&
        strCtrlName == "TcxPageControl")
    {
        return g_strPageControlSkinName;
    }

    if (g_mSkin.find(strCtrlName) == g_mSkin.end())
    {
        return GetAppSkinName();
    }

    return g_mSkin[strCtrlName];
}

BOOL IsNativeStyle(AnsiString strSkinName)
{
    if (strSkinName == "" ||
        strSkinName == "UserSkin")
    {
        return TRUE;
    }

    return FALSE;
}

BOOL IsNativeStyle()
{
    return IsNativeStyle(GetAppSkinName());
}

bool IsMainTreeList(TcxTreeList *treeList)
{
    for (UINT32 i=0; i<m_vMainTreeList.size(); i++)
    {
        if (m_vMainTreeList[i] == treeList)
        {
            return true;
        }
    }

    return false;
}

void SetNodeImage(TcxTreeListNode *Node, int iImageIndex)
{
    Node->ImageIndex = iImageIndex;
    Node->SelectedIndex = iImageIndex;
}

TcxTreeListNode* AddTreeNode(TcxTreeList *Tree, AnsiString strName, int iImageIndex)
{
    TcxTreeListNode     *Node = Tree->Add();

    Node->Values[0]         = strName;
    Node->ImageIndex        = iImageIndex;
    Node->SelectedIndex     = iImageIndex;
    Node->StateIndex        = iImageIndex;
    Node->OverlayIndex      = iImageIndex;
    Node->OverlayStateIndex = iImageIndex;

    return Node;
}

TcxTreeListNode* AddTreeNode(TcxTreeListNode *ParentNode, AnsiString strName, int iImageIndex)
{
    TcxTreeListNode     *Node = ParentNode->AddChild();

    Node->Values[0]         = strName;
    Node->ImageIndex        = iImageIndex;
    Node->SelectedIndex     = iImageIndex;
    Node->StateIndex        = iImageIndex;
    Node->OverlayIndex      = iImageIndex;
    Node->OverlayStateIndex = iImageIndex;

    return Node;
}

void SetTreeNodeName(TcxTreeListNode *Node, AnsiString strName)
{
    Node->Values[0] = strName;
}

void SetFont(TFont *FontDst, TFont *Font)
{
    if (Font)
    {
        FontDst->Name = Font->Name;
        FontDst->Size = Font->Size;
    }
//    FontDst->Color = Font->Color;
//    FontDst->Height = Font->Height;
//    FontDst->Style = Font->Style;
//    FontDst->Charset = Font->Charset;
}

void SetFont(TComponent *Ctrl, TFont *Font)
{
    AnsiString strClassName = Ctrl->ClassName();

    if (strClassName == "TdxBarManager")
    {
        SetFont(((TdxBarManager*)Ctrl)->Font, Font);
    }
    else if (strClassName == "TdxBar")
    {
        SetFont(((TdxBar*)Ctrl)->Font, Font);
    }
    else if (strClassName == "TdxBarPopupMenu")
    {
        SetFont(((TdxBarPopupMenu*)Ctrl)->Font, Font);
    }
    else if (strClassName == "TcxButton")
    {
      //  SetFont(((TcxButton*)Ctrl)->Font, Font);
        ((TcxButton*)Ctrl)->Font->Size = g_iButtonFontSize;
        ((TcxButton*)Ctrl)->Font->Name = g_strButtonFontName;
    }
    else if (strClassName == "TcxTreeList")
    {
     //   SetFont(((TcxTreeList*)Ctrl)->Font, Font);
        ((TcxTreeList*)Ctrl)->Font->Size = g_iTreeListFontSize;
    }
    else if (strClassName == "TcxVerticalGrid")
    {
        SetFont(((TcxVerticalGrid*)Ctrl)->Font, Font);
    }
    else if (strClassName == "TcxTreeListColumn")
    {
//        SetFont(((TcxTreeListColumn*)Ctrl)->Font, Font);
    }
    else if (strClassName == "TcxTrackBar")
    {
        ((TcxTrackBar*)Ctrl)->ParentFont = true;
        SetFont(((TcxTrackBar*)Ctrl)->Style->Font, Font);
    }
    else if (strClassName == "TcxLabel")
    {
       // ((TcxLabel*)Ctrl)->ParentFont = true;
        ((TcxLabel*)Ctrl)->Transparent = true;
        SetFont(((TcxLabel*)Ctrl)->Style->Font, Font);
        ((TcxLabel*)Ctrl)->Style->TextColor = g_clDefaultTextColor;
    }
    else if (strClassName == "TLabel")
    {
        SetFont(((TLabel*)Ctrl)->Font, Font);
        ((TLabel*)Ctrl)->Font->Color = g_clDefaultTextColor;
    }
    else if (strClassName == "TcxTextEdit")
    {
       // ((TcxTextEdit*)Ctrl)->ParentFont = true;
        SetFont(((TcxTextEdit*)Ctrl)->Style->Font, Font);
        ((TcxTextEdit*)Ctrl)->Style->TextColor = g_clDefaultTextColor;
    }
    else if (strClassName == "TcxMaskEdit")
    {
        SetFont(((TcxMaskEdit*)Ctrl)->Style->Font, Font);
        ((TcxMaskEdit*)Ctrl)->Style->TextColor = g_clDefaultTextColor;
    }
    else if (strClassName == "TcxDateEdit")
    {
       ((TcxDateEdit*)Ctrl)->ParentFont = false;
    //    ((TcxDateEdit*)Ctrl)->Style->Font->Name = g_strDefaultTimeFontName;
        SetFont(((TcxDateEdit*)Ctrl)->Style->Font, Font);
        ((TcxDateEdit*)Ctrl)->Properties->View = cavClassic;
        ((TcxDateEdit*)Ctrl)->Style->TextColor = g_clDefaultTextColor;

        TDateButtons     DateButton;

        DateButton << btnOk;
        DateButton << btnClear;
        DateButton << btnNow;
        DateButton << btnToday;
        ((TcxDateEdit*)Ctrl)->Properties->DateButtons = DateButton;

        ((TcxDateEdit*)Ctrl)->Properties->AssignedValues->DateButtons = true;
    }
    else if (strClassName == "TcxTimeEdit")
    {
        //((TcxTimeEdit*)Ctrl)->ParentFont = true;
        //((TcxTimeEdit*)Ctrl)->Style->Font->Name = g_strDefaultTimeFontName;
        SetFont(((TcxTimeEdit*)Ctrl)->Style->Font, Font);
        ((TcxTimeEdit*)Ctrl)->Style->TextColor = g_clDefaultTextColor;
    }
    else if (strClassName == "TcxComboBox")
    {
       // ((TcxComboBox*)Ctrl)->ParentFont = true;
        SetFont(((TcxComboBox*)Ctrl)->Style->Font, Font);
        ((TcxComboBox*)Ctrl)->Style->TextColor = g_clDefaultTextColor;
    }
    else if (strClassName == "TcxRadioButton")
    {
        ((TcxRadioButton*)Ctrl)->ParentFont = false;
        //((TcxRadioButton*)Ctrl)->ParentFont = true;
        SetFont(((TcxRadioButton*)Ctrl)->Font, Font);
        ((TcxRadioButton*)Ctrl)->Font->Color = g_clDefaultTextColor;
    }
    else if (strClassName == "TcxCheckBox")
    {
       // ((TcxCheckBox*)Ctrl)->ParentFont = true;
        SetFont(((TcxCheckBox*)Ctrl)->Style->Font, Font);
        ((TcxCheckBox*)Ctrl)->Style->TextColor = g_clDefaultTextColor;
    }
    else if (strClassName == "TcxGroupBox")
    {
       //((TcxGroupBox*)Ctrl)->ParentFont = true;

       ((TcxGroupBox*)Ctrl)->Style->Font->Name = g_strGroupBoxFontName;
       ((TcxGroupBox*)Ctrl)->Style->Font->Size = g_iGroupBoxFontSize;
    }
    else if (strClassName == "TcxPageControl")
    {
        ((TcxPageControl*)Ctrl)->Font->Size = g_iPageControlFontSize;
        ((TcxPageControl*)Ctrl)->Font->Name = g_strPageControlFontName;
    }
    else if (strClassName == "TcxTabSheet")
    {
        //((TcxTabSheet*)Ctrl)->ParentFont = true;

        ((TcxTabSheet*)Ctrl)->Font->Size = g_iPageControlFontSize;
        ((TcxTabSheet*)Ctrl)->Font->Name = g_strPageControlFontName;
    }
    else if (strClassName == "TcxSpinEdit")
    {
        SetFont(((TcxCheckBox*)Ctrl)->Style->Font, Font);
        ((TcxSpinEdit*)Ctrl)->Style->TextColor = g_clDefaultTextColor;
    }
    else if (strClassName == "TcxButtonEdit")
    {
        SetFont(((TcxButtonEdit*)Ctrl)->Style->Font, Font);
        ((TcxButtonEdit*)Ctrl)->Style->TextColor = g_clDefaultTextColor;
    }
    else if (strClassName == "TcxRichEdit")
    {
        ((TcxRichEdit*)Ctrl)->ParentFont = false;
        SetFont(((TcxRichEdit*)Ctrl)->Style->Font, Font);
        ((TcxRichEdit*)Ctrl)->Style->TextColor = g_clDefaultTextColor;
        ((TcxRichEdit*)Ctrl)->Properties->MemoMode = true;
    }
    else if (strClassName == "TdxBarButton")
    {
        ((TdxBarButton*)Ctrl)->Style = GetStyleText();
    }
    else if (strClassName == "TdxBarLargeButton")
    {
        ((TdxBarLargeButton*)Ctrl)->Style = GetStyleText();
    }
    else if (strClassName == "TcxMemo")
    {
        SetFont(((TcxMemo*)Ctrl)->Style->Font, Font);
        ((TcxMemo*)Ctrl)->Style->TextColor = g_clDefaultTextColor;
    }
    else if (strClassName == "TdxStatusBar")
    {
        ((TdxStatusBar*)Ctrl)->ParentFont = true;
    }
    else if (strClassName == "TcxCheckGroup")
    {
        SetFont(((TcxCheckGroup*)Ctrl)->Style->Font, Font);
        ((TcxCheckGroup*)Ctrl)->Style->TextColor = g_clDefaultTextColor;
    }
    else if (strClassName == "TcxCheckComboBox")
    {
        SetFont(((TcxCheckComboBox*)Ctrl)->Style->Font, Font);
        ((TcxCheckComboBox*)Ctrl)->Style->TextColor = g_clDefaultTextColor;
    }
    else if (strClassName == "TcxCheckListBox")
    {
        SetFont(((TcxCheckListBox*)Ctrl)->Style->Font, Font);
        ((TcxCheckListBox*)Ctrl)->Style->TextColor = g_clDefaultTextColor;
    }
    else if (strClassName == "TPanel")
    {
        SetFont(((TPanel*)Ctrl)->Font, Font);
        ((TPanel*)Ctrl)->Font->Color = g_clDefaultTextColor;
    }
/*    else if (strClassName == "TcxStyle")
    {
        SetFont(((TcxStyle*)Ctrl)->Font, Font);
        ((TcxStyle*)Ctrl)->TextColor = g_clDefaultTextColor;
    }  */
}

void SetSystemFont(AnsiString strFontName, int iFontSize)
{
    g_SystemFont->Name = strFontName;
    g_SystemFont->Size = iFontSize;
}

void SetDefaultFontSize(int iFontSize)
{
    g_iDefaultFontSize = iFontSize;
}

void SetDefaultFontName(AnsiString strFontName)
{
    if (!strFontName.IsEmpty())
    {
        g_strDefaultFontName = strFontName;
        g_strTreeListFontName = strFontName;
    }
}

void InitSystemFont()
{
    if (!g_SystemFont)
    {
        g_SystemFont = new TFont();

        SetSystemFont(g_strDefaultFontName, g_iDefaultFontSize);
    }
}

void __fastcall TSubForm::SetSkin(TdxSkinController* SkinController)
{
    bool bNativeStyle = IsNativeStyle();

    SkinController->SkinName = GetAppSkinName();

    SkinController->NativeStyle = bNativeStyle;
    SkinController->UseSkins = !bNativeStyle;
}

void __fastcall TSubForm::SetSkin(Cxlookandfeels::TcxLookAndFeel* LookAndFeel, AnsiString strSkinName)
{
    bool bNativeStyle = IsNativeStyle();

    LookAndFeel->AssignedValues >> lfvKind;

    if (bNativeStyle)
    {
        LookAndFeel->AssignedValues << lfvNativeStyle;
        LookAndFeel->NativeStyle = bNativeStyle;
    }
    else
    {
        LookAndFeel->AssignedValues >> lfvNativeStyle;
        LookAndFeel->NativeStyle = bNativeStyle;
    }

    LookAndFeel->AssignedValues << lfvSkinName;
    LookAndFeel->SkinName = strSkinName;

    LookAndFeel->Kind = lfStandard;
    LookAndFeel->AssignedValues << lfvKind;

    LookAndFeel->ScrollbarMode = sbmClassic;
    LookAndFeel->AssignedValues << lfvScrollbarMode;
}

void __fastcall TSubForm::SetSkin(Cxlookandfeels::TcxLookAndFeel* LookAndFeel)
{
    SetSkin(LookAndFeel, GetAppSkinName());
}

void __fastcall TSubForm::SetSkin(TdxLayoutCxLookAndFeel* LayoutCxLookAndFeel)
{
    SetSkin(LayoutCxLookAndFeel->LookAndFeel);
}

void __fastcall TSubForm::SetSkin(TcxPageControl* PageControl)
{
    AnsiString  strSkinName = GetSkinName("TcxPageControl");

    SetSkin(PageControl->LookAndFeel, strSkinName);
}

void __fastcall TSubForm::SetSkin(Cxlookandfeels::TcxLookAndFeelController* LookAndFeelController)
{
    bool bNativeStyle = IsNativeStyle();

    LookAndFeelController->SkinName = GetAppSkinName();

    LookAndFeelController->NativeStyle = bNativeStyle;
}

void __fastcall TSubForm::SetSkin(TdxBarManager* BarManager)
{
    bool bNativeStyle = IsNativeStyle();

    if (bNativeStyle)
    {
        BarManager->Style = bmsStandard;
    }
    else
    {
        BarManager->Style = bmsUseLookAndFeel;
    }

    AnsiString  strSkinName = GetSkinName("TdxBarManager");

    SetSkin(BarManager->LookAndFeel, strSkinName);
    BarManager->ImageOptions->StretchGlyphs = false;
}

void __fastcall TSubForm::OnTreeListGetNodeHeight(TcxCustomTreeList *Sender, TcxTreeListNode *ANode, int &AHeight)
{
    AHeight = g_iTreeListNodeHeight;
}

void __fastcall TSubForm::SetSkin(TcxTreeList *treeList, bool bMainTree)
{
    _SetSkin(treeList, bMainTree, true);
}

void __fastcall TSubForm::OnTreeListCustomDrawDataCell(TcxCustomTreeList *Sender, TcxCanvas *ACanvas,
          TcxTreeListEditCellViewInfo *AViewInfo, bool &ADone)
{
    AnsiString      strTreeListName = Sender->Name;

    if (m_mOnTreeListCustomDrawDataCell.find(strTreeListName) != m_mOnTreeListCustomDrawDataCell.end())
    {
        OnTreeListCustomDrawDataCell_FUNC   EventOnTreeListCustomDrawDataCell = m_mOnTreeListCustomDrawDataCell[strTreeListName];

        if (EventOnTreeListCustomDrawDataCell &&
            EventOnTreeListCustomDrawDataCell != OnTreeListCustomDrawDataCell)
        {
            EventOnTreeListCustomDrawDataCell(Sender, ACanvas, AViewInfo, ADone);
        }
    }

    if (AViewInfo->Node && AViewInfo->Node->Selected)
    {
        ACanvas->Canvas->Brush->Color = g_clTreeListSelection;
    }
}

void __fastcall TSubForm::_SetSkin(TcxTreeList *treeList, bool bMainTree, bool bSave)
{
    AnsiString  strSkinName = GetSkinName("TcxTreeList");

    if (!bMainTree)
    {
        strSkinName = GetAppSkinName();
    }

    treeList->ParentFont = false;

    SetSkin(treeList->LookAndFeel, strSkinName);

    treeList->OptionsSelection->HideFocusRect = false;
    treeList->OptionsSelection->InvertSelect = false;
    treeList->OptionsView->TreeLineColor = Vcl::Graphics::clDefault;//clGradientActiveCaption;
    treeList->OptionsView->GridLineColor = Vcl::Graphics::clDefault;//clInactiveCaption;
    treeList->OptionsBehavior->CellHints = true;

    treeList->OptionsCustomizing->ColumnCustomizing = false;
    treeList->OptionsCustomizing->ColumnMoving = false;
    treeList->OptionsCustomizing->ColumnVertSizing = false;

    if (bMainTree && bSave)
    {
        m_vMainTreeList.push_back(treeList);
    }

    treeList->OptionsView->Indicator = false;

    if (treeList->LookAndFeel->NativeStyle)
    {
        treeList->OptionsView->GridLineColor = (TColor)0xE8E7E6;
    }

    treeList->OptionsSelection->CellSelect = false;

    if (bMainTree)
    {
        treeList->OptionsView->GridLines = tlglNone;
        treeList->OptionsView->Headers = false;
        treeList->OptionsView->ShowRoot = true;
        treeList->OptionsView->ColumnAutoWidth = true;
        treeList->OptionsView->TreeLineStyle = tllsSolid;
        treeList->OptionsData->Editing = false;
    }
    else
    {
        treeList->OptionsView->GridLines = tlglBoth;
        treeList->OptionsView->Headers = true;
        treeList->OptionsView->ShowRoot = false;
        treeList->OptionsView->ColumnAutoWidth = false;
        treeList->OptionsView->TreeLineStyle = tllsNone;
    }

    treeList->OptionsView->HeaderAutoHeight = true;

    treeList->OnColumnSizeChanged = NULL;

    LoadTableInfo(treeList);

    if (!g_StyleTreeList)
    {
        g_StyleTreeList = new TcxStyle(this);

        g_StyleTreeList->Color = GetSkinColor(g_strSkinName);
    }
    g_StyleTreeList->Font->Size = g_iTreeListFontSize;
    g_StyleTreeList->Font->Name = g_strTreeListFontName;//g_SystemFont->Name;

    if (!g_StyleTreeListSelection)
    {
        g_StyleTreeListSelection = new TcxStyle(this);

        g_StyleTreeListSelection->Color = g_clTreeListSelection;
	}
    g_StyleTreeListSelection->Font->Size = g_StyleTreeList->Font->Size;
    g_StyleTreeListSelection->Font->Name = g_strTreeListFontName;//g_StyleTreeList->Font;

    if (!g_StyleTreeListContentEven)
    {
        g_StyleTreeListContentEven = new TcxStyle(this);

        g_StyleTreeListContentEven->Color = g_clTreeListContentEven;
	}
    g_StyleTreeListContentEven->Font->Size = g_StyleTreeList->Font->Size;
    g_StyleTreeListContentEven->Font->Name = g_strTreeListFontName;

    g_StyleTreeList->TextColor = GetTreeListTextColor();
    g_StyleTreeListContentEven->TextColor = GetTreeListTextColor();

    treeList->Styles->Content = g_StyleTreeList;
    treeList->Styles->ContentEven = g_StyleTreeListContentEven;
    treeList->Styles->ContentOdd = g_StyleTreeList;
    treeList->Styles->Background = g_StyleTreeList;
//    treeList->Styles->Selection = g_StyleTreeListSelection;

    if (bMainTree)
    {
        treeList->Styles->UseOddEvenStyles = bFalse;
    }
    else
    {
        treeList->Styles->UseOddEvenStyles = bTrue;
    }

    treeList->OnColumnSizeChanged = OnTreeListColumnSizeChanged;
    treeList->OnGetNodeHeight = OnTreeListGetNodeHeight;

    // ∑¿÷π∂‡÷÷µ˜”√∫Û£¨m_mOnTreeListCustomDrawDataCell[strTreeListName]µƒ÷µª·…Ë÷√≥…OnTreeListCustomDrawDataCell
    if (treeList->OnCustomDrawDataCell &&
        treeList->OnCustomDrawDataCell != OnTreeListCustomDrawDataCell)
    {
        AnsiString  strTreeListName = treeList->Name;

        m_mOnTreeListCustomDrawDataCell[strTreeListName] = treeList->OnCustomDrawDataCell;
    }

    treeList->OnCustomDrawDataCell = OnTreeListCustomDrawDataCell;

  //  ::SetFont(treeList, g_SystemFont);

    for (int i=0; i<treeList->ColumnCount; i++)
    {
        treeList->Columns[i]->Caption->AlignVert = vaCenter;
    }

#if 0
    TdxSmartGlyph   *Glyph = GetTreeListHeaderGlyph(g_iTreeListNodeHeight);

    if (Glyph)
    {
        for (int i=0; i<treeList->ColumnCount; i++)
        {
            if (treeList->Columns[i]->Visible)
            {
                treeList->Columns[i]->Caption->Glyph = Glyph;
                break;
            }
        }
    }
#endif

    if (!g_StyleTreeListHeader)
    {
        g_StyleTreeListHeader = new TcxStyle(this);

     //   g_StyleTreeListHeader->Color = GetSkinColor(g_strSkinName);
    }

    g_StyleTreeListHeader->Font->Name = g_strTreeListHeaderFontName;
    g_StyleTreeListHeader->Font->Size = g_iTreeListHeaderFontSize;
    g_StyleTreeListHeader->TextColor = GetTreeListTextColor();

    treeList->Styles->ColumnHeader = g_StyleTreeListHeader;//GetTreeListHeaderStyle();
}

void __fastcall TSubForm::SetSkin(TcxVerticalGrid *Grid)
{
    SetSkin(Grid->LookAndFeel);
}

void __fastcall TSubForm::SetSkin(TcxLabel *Label)
{
    SetSkin(Label->Style->LookAndFeel);

    Label->Transparent = true;

//    Label->Style->TextColor = Label->Style->LookAndFeel->Painter->DefaultEditorTextColor(FALSE);
}

void __fastcall TSubForm::SetSkin(TLabel *Label)
{
    Label->Transparent = true;
}

void __fastcall TSubForm::SetSkin(TcxGroupBox *GroupBox)
{
    AnsiString  strSkinName = GetSkinName("TcxGroupBox");

    SetSkin(GroupBox->Style->LookAndFeel, strSkinName);
    GroupBox->Transparent = true;
}

void __fastcall TSubForm::SetSkin(TcxButton *Button)
{
    AnsiString  strSkinName = GetSkinName("TcxButton");

    SetSkin(Button->LookAndFeel, strSkinName);
}

void __fastcall TSubForm::SetSkin(TChart *pChart)
{
    if (!g_SystemFont)
    {
        return;
    }

    pChart->Title->Font->Name            = g_SystemFont->Name;
    pChart->BottomAxis->LabelsFont->Name = g_SystemFont->Name;
    pChart->LeftAxis->LabelsFont->Name   = g_SystemFont->Name;
    pChart->Legend->Font->Name           = g_SystemFont->Name;

    pChart->Title->Font->Size            = 12;
    pChart->Legend->Font->Size           = g_SystemFont->Size;
}

void __fastcall TSubForm::SetSkin(TcxTrackBar *TrackBar)
{
    AnsiString  strSkinName = GetSkinName("TcxTrackBar");

    SetSkin(TrackBar->Style->LookAndFeel, strSkinName);
}

void __fastcall TSubForm::SetSkin(TcxComboBox *ComboBox)
{
    AnsiString  strSkinName = GetSkinName("TcxComboBox");

    ComboBox->Properties->DropDownListStyle = lsFixedList;
    SetSkin(ComboBox->Style->LookAndFeel, strSkinName);

    if (ComboBox->Properties->DropDownRows < 16)
    {
        ComboBox->Properties->DropDownRows = 16;
    }

    ComboBox->ParentColor = false;
    ComboBox->StyleDisabled->Color = g_clDisabledColor;
}

void __fastcall TSubForm::SetSkin(TcxCheckComboBox *ComboBox)
{
    SetSkin(ComboBox->Style->LookAndFeel);
}

void __fastcall TSubForm::SetSkin(TcxColorComboBox *ComboBox)
{
    ComboBox->Properties->DropDownListStyle = lsEditFixedList;
    SetSkin(ComboBox->Style->LookAndFeel);
}

void __fastcall TSubForm::SetSkin(TcxRadioButton *RadioButton)
{
    AnsiString  strSkinName = GetSkinName("TcxRadioButton");

    SetSkin(RadioButton->LookAndFeel, strSkinName);
    RadioButton->Transparent = true;
}

void __fastcall TSubForm::SetSkin(TcxTextEdit *TextEdit)
{
    AnsiString  strSkinName = GetSkinName("TcxTextEdit");

    SetSkin(TextEdit->Style->LookAndFeel, strSkinName);

    if (IsNativeStyle(TextEdit->Style->LookAndFeel->SkinName))
    {
        TextEdit->StyleDisabled->Color = clWindow;
    }
    else
    {
        TextEdit->StyleDisabled->Color = g_clDisabledColor;
        // TextEdit->Style->LookAndFeel->Painter->DefaultEditorBackgroundColor(FALSE);
    }

 //   TextEdit->StyleDisabled->TextColor = TextEdit->Style->LookAndFeel->Painter->DefaultEditorTextColor(FALSE);
}

void __fastcall TSubForm::SetSkin(TcxMaskEdit *MaskEdit)
{
    SetSkin(MaskEdit->Style->LookAndFeel);
}

void __fastcall TSubForm::SetSkin(TcxSpinEdit *SpinEdit)
{
    SetSkin(SpinEdit->Style->LookAndFeel);
}

void __fastcall TSubForm::SetSkin(TcxCheckListBox *CheckListBox)
{
    SetSkin(CheckListBox->Style->LookAndFeel);

    CheckListBox->ParentColor = false;
    CheckListBox->StyleDisabled->Color = g_clDisabledColor;
}

void __fastcall TSubForm::SetSkin(TcxCheckGroup *CheckGroup)
{
    SetSkin(CheckGroup->Style->LookAndFeel);

    CheckGroup->ParentColor = false;
    CheckGroup->StyleDisabled->Color = g_clDisabledColor;
}

void __fastcall TSubForm::SetSkin(TcxDateEdit *DateEdit)
{
    AnsiString  strSkinName = GetSkinName("TcxDateEdit");

    SetSkin(DateEdit->Style->LookAndFeel, strSkinName);
}

void __fastcall TSubForm::SetSkin(TcxTimeEdit *TimeEdit)
{
    SetSkin(TimeEdit->Style->LookAndFeel);
}

void __fastcall TSubForm::SetSkin(TcxCheckBox *CheckBox)
{
    AnsiString  strSkinName = GetSkinName("TcxCheckBox");

    SetSkin(CheckBox->Style->LookAndFeel, strSkinName);
    CheckBox->Transparent = true;
}

void __fastcall TSubForm::SetSkin(TcxButtonEdit *ButtonEdit)
{
    SetSkin(ButtonEdit->Style->LookAndFeel);
}

void __fastcall TSubForm::SetSkin(TcxProgressBar *ProgressBar)
{
    AnsiString  strSkinName = GetSkinName("TcxProgressBar");

    SetSkin(ProgressBar->Style->LookAndFeel, strSkinName);
}

void __fastcall TSubForm::SetSkin(TdxStatusBar *StatusBar)
{
    AnsiString  strSkinName = GetSkinName("TdxStatusBar");

    SetSkin(StatusBar->LookAndFeel, strSkinName);
}

void __fastcall TSubForm::SetSkin(TcxMemo *Memo)
{
    AnsiString  strSkinName = GetSkinName("TcxMemo");

    SetSkin(Memo->Style->LookAndFeel, strSkinName);
}

void __fastcall TSubForm::SetSkin(TdxBevel *Bevel)
{
    SetSkin(Bevel->LookAndFeel);
}

void __fastcall TSubForm::SetSkin(TcxEditStyle *EditStyle)
{
    SetSkin(EditStyle->LookAndFeel);
}

void __fastcall TSubForm::SetSkin(TcxFontNameComboBox *cbSystemFont)
{
    SetSkin(cbSystemFont->Style->LookAndFeel);
}

void GetFormList(VECTOR<TSubForm*> &vForm)
{
    MAP<AnsiString, TSubForm*>::iterator it = s_mFormMap.begin();

    vForm.clear();

    for (; it!=s_mFormMap.end(); it++)
    {
        TSubForm    *Form = it->second;

        vForm.push_back(Form);
    }
}

//---------------------------------------------------------------------------
__fastcall TSubForm::TSubForm(TComponent* Owner, bool bRegisterSkin)
    : TForm(Owner)
{
    InitSystemFont();

    AnsiString strClassName = ClassName();

    GosLog(LOG_INFO, "create form %s", strClassName.c_str());

    if (bRegisterSkin)
    {
        s_mFormMap[strClassName] = this;

        SetSkin();
        InitSkin();

        SetFont();
    }
}

bool __fastcall TSubForm::InitMsgHandler(UINT32 ulMsgType, INNER_MSG_HANDLER pfHandler)
{
    InitInnerMsg(ulMsgType, (TForm*)this);
    std::vector<INNER_MSG_HANDLER>      *pvMsgHandler;

    if (!FIND_MAP_KEY(m_mInnerMsgHandler, ulMsgType))
    {
        pvMsgHandler = new std::vector<INNER_MSG_HANDLER>();
    }
    else
    {
        pvMsgHandler = (std::vector<INNER_MSG_HANDLER>*)m_mInnerMsgHandler[ulMsgType];
    }

    pvMsgHandler->push_back(pfHandler);

    m_mInnerMsgHandler[ulMsgType] = pvMsgHandler;

    return true;
}

bool __fastcall TSubForm::InitMsgHandler(UINT32 ulMsgType, AnsiString strMsgName, JSON_MSG_HANDLER pfHandler)
{
    AnsiString      strKey = strMsgName + AnsiString("_") + AnsiString(ulMsgType);

    InitInnerMsg(ulMsgType, (TForm*)this);

    m_sJsonMsgType.Add(ulMsgType);

    std::vector<JSON_MSG_HANDLER>      *pvMsgHandler;

    if (!FIND_MAP_KEY(m_mJsonMsgHandler, strKey))
    {
        pvMsgHandler = new std::vector<JSON_MSG_HANDLER>();
    }
    else
    {
        pvMsgHandler = (std::vector<JSON_MSG_HANDLER>*)m_mJsonMsgHandler[strKey];
    }

    pvMsgHandler->push_back(pfHandler);

    m_mJsonMsgHandler[strKey] = pvMsgHandler;

    return true;
}

void __fastcall TSubForm::OnJsonMsg(TMessage Message)
{
    INNER_MSG_T *pstMsg = (INNER_MSG_T *)Message.LParam;
    AnsiString  strKey;
    GJson       &Json = m_JsonMsg;
    GJson       SubJson;
    CHAR        *szMsg = (CHAR*)pstMsg->aucMsg;
    BOOL        bRet = Json.Parse(szMsg);
    CHAR        *szMsgType;
    CHAR        *szMsgInfo;
    UINT32      ulSeqID;
    UINT32      ulTime = gos_get_uptime_1ms();
    CHAR        *szZipped;
    std::string strOriMsg;
    std::vector<JSON_MSG_HANDLER>  *pvMsgHandler;
    std::vector<JSON_MSG_HANDLER>::iterator it;

    if (!bRet)
    {
        GosLog(LOG_ERROR, "invalid json msg: %s", szMsg);
        goto error;
    }

    szMsgType = Json.GetValue("MsgName");
    if (!szMsgType)
    {
        GosLog(LOG_ERROR, "get MsgName failed: %s", szMsg);
        goto error;
    }

    strKey = AnsiString(szMsgType) + AnsiString("_") + AnsiString(pstMsg->ulMsgType);
    if (!FIND_MAP_KEY(m_mJsonMsgHandler, strKey))
    {
        goto error;
    }

    if (!Json.GetValue("MsgSeqID", &ulSeqID))
    {
        GosLog(LOG_ERROR, "get MsgSeqID failed: %s", szMsg);
        goto error;
    }

    szMsgInfo = Json.GetValue("MsgInfo");
    if (!szMsgInfo)
    {
        GosLog(LOG_ERROR, "get MsgInfo failed: %s", szMsg);
        goto error;
    }

    szZipped = Json.GetValue("Zipped");
    if (szZipped && strcasecmp(szZipped, "true") == 0)
    {
        strOriMsg = UnzipString(szMsgInfo);
        szMsgInfo = (CHAR*)strOriMsg.c_str();
    }

    if (!SubJson.Parse(szMsgInfo))
    {
        GosLog(LOG_ERROR, "parse MsgInfo failed: %s", szMsgInfo);
 //       gos_save_string_to_file("d:\\test.log", szMsgInfo);
        goto error;
    }

    m_strJsonMsgType = szMsgType;

    pvMsgHandler = (std::vector<JSON_MSG_HANDLER>*)m_mJsonMsgHandler[strKey];
    it = pvMsgHandler->begin();
    for (; it!=pvMsgHandler->end(); it++)
    {
        JSON_MSG_HANDLER   pfHandler = *it;

        pfHandler(ulSeqID, SubJson);
    }

    ulTime = gos_get_uptime_1ms() - ulTime;

    if (ulTime > 200)
    {
        GosLog(LOG_DETAIL, "json msg: type = 0x%X, name = %s, time = %u", pstMsg->ulMsgType, szMsgType, ulTime);
    }

error:
    free(pstMsg);
}

void __fastcall TSubForm::OnInnerMsg(TMessage Message)
{
    INNER_MSG_T         *pstMsg = (INNER_MSG_T *)Message.LParam;

    if (m_sJsonMsgType.Has(pstMsg->ulMsgType))
    {
        OnJsonMsg(Message);
        return;
    }

    if (!FIND_MAP_KEY(m_mInnerMsgHandler, pstMsg->ulMsgType))
    {
        GosLog(LOG_ERROR, "invalid msg type = 0x%X", pstMsg->ulMsgType);
        free(pstMsg);
        return;
    }

    std::vector<INNER_MSG_HANDLER>  *pvMsgHandler = (std::vector<INNER_MSG_HANDLER>*)m_mInnerMsgHandler[pstMsg->ulMsgType];
    std::vector<INNER_MSG_HANDLER>::iterator it = pvMsgHandler->begin();

    UINT32  ulTime = gos_get_uptime_1ms();

    for (; it!=pvMsgHandler->end(); it++)
    {
        INNER_MSG_HANDLER   pfHandler = *it;

        pfHandler(pstMsg->aucMsg, pstMsg->ulMsgLen);
    }

    ulTime = gos_get_uptime_1ms() - ulTime;

    if (ulTime > 200)
    {
        GosLog(LOG_DETAIL, "Async msg: type = 0x%X, time = %u", pstMsg->ulMsgType, ulTime);
    }

    free(pstMsg);
}

//---------------------------------------------------------------------------
void __fastcall TSubForm::SetFont()
{
    if (!g_SystemFont)
    {
        return;
    }

    ::SetFont(Font, g_SystemFont);
    for (int i=0; i<ComponentCount; i++)
    {
        ::SetFont(Components[i], Font);
    }
}

void __fastcall TSubForm::InitSkin(TComponent *Ctrl)
{
    AnsiString strClassName = Ctrl->ClassName();

         if (strClassName == "TdxSkinController")                        SetSkin((TdxSkinController*)Ctrl);
    else if (strClassName == "TdxLayoutCxLookAndFeel")                   SetSkin((TdxLayoutCxLookAndFeel*)Ctrl);
    else if (strClassName == "TcxPageControl")                           SetSkin((TcxPageControl*)Ctrl);
    else if (strClassName == "Cxlookandfeels::TcxLookAndFeelController") SetSkin((Cxlookandfeels::TcxLookAndFeelController*)Ctrl);
    else if (strClassName == "TdxBarManager")                            SetSkin((TdxBarManager*)Ctrl);
    else if (strClassName == "TcxTreeList")
    {
        bool bMainTreeList = IsMainTreeList((TcxTreeList*)Ctrl);
        _SetSkin((TcxTreeList*)Ctrl, bMainTreeList, false);
    }
    else if (strClassName == "TcxVerticalGrid")                          SetSkin((TcxVerticalGrid*)Ctrl);
    else if (strClassName == "TcxLabel")                                 SetSkin((TcxLabel*)Ctrl);
    else if (strClassName == "TcxGroupBox")                              SetSkin((TcxGroupBox*)Ctrl);
    else if (strClassName == "TcxButton")                                SetSkin((TcxButton*)Ctrl);
    else if (strClassName == "TcxTrackBar")                              SetSkin((TcxTrackBar*)Ctrl);
    else if (strClassName == "TcxColorComboBox")                         SetSkin((TcxColorComboBox*)Ctrl);
    else if (strClassName == "TcxComboBox")                              SetSkin((TcxComboBox*)Ctrl);
    else if (strClassName == "TcxCheckComboBox")                         SetSkin((TcxCheckComboBox*)Ctrl);
    else if (strClassName == "TcxRadioButton")                           SetSkin((TcxRadioButton*)Ctrl);
    else if (strClassName == "TcxTextEdit")                              SetSkin((TcxTextEdit*)Ctrl);
    else if (strClassName == "TcxMaskEdit")                              SetSkin((TcxMaskEdit*)Ctrl);
    else if (strClassName == "TcxSpinEdit")                              SetSkin((TcxSpinEdit*)Ctrl);
    else if (strClassName == "TcxCheckListBox")                          SetSkin((TcxCheckListBox*)Ctrl);
    else if (strClassName == "TcxDateEdit")                              SetSkin((TcxDateEdit*)Ctrl);
    else if (strClassName == "TcxTimeEdit")                              SetSkin((TcxTimeEdit*)Ctrl);
    else if (strClassName == "TcxCheckBox")                              SetSkin((TcxCheckBox*)Ctrl);
    else if (strClassName == "TcxButtonEdit")                            SetSkin((TcxButtonEdit*)Ctrl);
    else if (strClassName == "TcxProgressBar")                           SetSkin((TcxProgressBar*)Ctrl);
    else if (strClassName == "TdxStatusBar")                             SetSkin((TdxStatusBar*)Ctrl);
    else if (strClassName == "TcxMemo")                                  SetSkin((TcxMemo*)Ctrl);
    else if (strClassName == "TcxFontNameComboBox")                      SetSkin((TcxFontNameComboBox*)Ctrl);
    else if (strClassName == "TLabel")                                   SetSkin((TLabel*)Ctrl);
    else if (strClassName == "TdxBevel")                                 SetSkin((TdxBevel*)Ctrl);

}
//---------------------------------------------------------------------------
void __fastcall TSubForm::InitSkin()
{
    SetSkin();
}
void __fastcall TSubForm::SetSkin()
{
    for (int i=0; i<ComponentCount; i++)
    {
        InitSkin(Components[i]);
    }
}

void __fastcall TSubForm::OnTreeListColumnSizeChanged(TcxCustomTreeList *Sender, TcxTreeListColumn *AColumn)
{
    SaveTableInfo(Sender);
}

void __fastcall TSubForm::TreeListCustomDrawBackgroundCell(TcxCustomTreeList *Sender,
          TcxCanvas *ACanvas, TcxTreeListCustomCellViewInfo *AViewInfo, bool &ADone)
{
    TColor  ContentColor = ACanvas->Brush->Color;

    SetTreeListContentColor(ContentColor);
}

int __fastcall TSubForm::ShowModal(void)
{
    g_bShowModal = true;

    int iRet = TForm::ShowModal();

    g_bShowModal = false;

    return iRet;
}

void __fastcall TSubForm::SetParentCtrl(TWinControl *Ctrl)
{
    MainPanel->Parent = Ctrl;
}

bool IsShowModal()
{
    return g_bShowModal;
}
