//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "DlgSetSkin.h"
#include "Main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxControls"
#pragma link "cxGraphics"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxSplitter"
#pragma link "dxSkinBlue"
#pragma link "dxSkinDarkRoom"
#pragma link "dxSkinDarkSide"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinLilian"
#pragma link "dxSkinMcSkin"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "cxCustomData"
#pragma link "cxInplaceContainer"
#pragma link "cxStyles"
#pragma link "cxTextEdit"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "cxCustomData"
#pragma link "cxInplaceContainer"
#pragma link "cxStyles"
#pragma link "cxTextEdit"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "cxClasses"
#pragma link "cxCheckBox"
#pragma link "cxCheckGroup"
#pragma link "cxDropDownEdit"
#pragma link "cxEdit"
#pragma link "cxVGrid"
#pragma link "dxBar"
#pragma link "cxRadioGroup"
#pragma link "cxMaskEdit"
#pragma link "cxContainer"
#pragma link "cxGroupBox"
#pragma link "cxEditRepositoryItems"
#pragma link "cxExtEditRepositoryItems"
#pragma link "cxCheckComboBox"
#pragma link "cxLabel"
#pragma link "cxSpinEdit"
#pragma link "cxPC"
#pragma link "dxSkinscxPCPainter"
#pragma link "cxButtons"
#pragma link "cxFontNameComboBox"
#pragma link "dxLayoutContainer"
#pragma link "dxLayoutControl"
#pragma link "dxLayoutControlAdapters"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "dxBevel"
#pragma resource "*.dfm"

TDialogSetSkin *DialogSetSkin;

#if 1 //__BORLANDC__ > 0x0700
char *g_aszSkinName[] = {
    "Black",
    "Blue",
    "Blueprint",
    "Caramel",
    "Coffee",
    "DarkRoom",
    "DarkSide",
    "DevExpressDarkStyle",
    "DevExpressStyle",
    "Foggy",
    "GlassOceans",
//  "HighContrast",
    "iMaginary",
    "Lilian",
    "LiquidSky",
    "LondonLiquidSky",
    "McSkin",
    "Metropolis",
    "MetropolisDark",
    "MoneyTwins",
    "Office2007Black",
    "Office2007Blue",
    "Office2007Green",
//  "Office2007Pink",
    "Office2007Silver",
    "Office2010Black",
    "Office2010Blue",
    "Office2010Silver",
    "Office2013DarkGray",
    "Office2013LightGray",
    "Office2013White",
    "Office2016Colorful",
    "Office2016Dark",
//  "Pumpkin",
//  "Seven",
//  "SevenClassic",
    "Sharp",
    "SharpPlus",
    "Silver",
//  "SpringTime",
    "Stardust",
//  "Summer2008",
    "TheAsphaltWorld",
    "TheBezier",
//  "Valentine",
    "VisualStudio2013Blue",
    "VisualStudio2013Dark",
    "VisualStudio2013Light",
    "VS2010",
    "Whiteprint",
//  "Xmas2008Blue",
} ;
#else
char *g_aszSkinName[] = {
    "MoneyTwins",
    "Lilian",
    "Blueprint",
    "SharpPlus",
    "Blue",
    "Office2007Blue",
    "Office2010Blue",
    "VisualStudio2013Blue",
    "Seven",
    "VS2010",
};
#endif

char *g_aszCtrlName[] = {
    "Application",
    "TcxButton",
    "TcxTreeList",
    "TcxDateEdit",
    "TcxPageControl",
    "TcxGroupBox",
    "TcxTextEdit",
    "TcxCheckBox",
    "TcxComboBox",
    "TcxRadioButton",
    "TcxMemo",
    "TcxTrackBar",
    "TcxProgressBar",
    "TdxStatusBar",
    "TdxBarManager",
};

extern std::map<AnsiString, AnsiString> g_mSkin;

extern CHAR g_acLocalCfgFile[1024];

void LoadSkinCfg()
{
    CHAR    *szFile = g_acLocalCfgFile;
    CHAR    *szValue;

    g_mSkin["Application"] = GetDefaultSkinName();

    return;

    /*
    if (!gos_file_exist(szFile))
    {
        return;
    }

    GConfFile   Conf(szFile);

    for (UINT32 i=0; i<ARRAY_SIZE(g_aszCtrlName); i++)
    {
        szValue = Conf.Get("skin", g_aszCtrlName[i]);
        if (szValue)
        {
            g_mSkin[g_aszCtrlName[i]] = szValue;
            if (strcmp(g_aszCtrlName[i], "Application") == 0)
            {
                g_strSkinName = szValue;
            }
            else if (strcmp(g_aszCtrlName[i], "TcxTreeList") == 0)
            {
                SetTreeListSkinName(szValue);
            }
        }
    } */
}

void SaveSkinCfg()
{
    CHAR    acFile[256];
    CHAR    *szValue;

    sprintf(acFile, "%s/%s", gos_get_root_path(), g_acLocalCfgFile);

    if (!gos_file_exist(acFile))
    {
        return;
    }

    GConfFile   Conf(acFile);

    AnsiString  strAppSkinName = g_mSkin["Application"];

    for (UINT32 i=0; i<ARRAY_SIZE(g_aszCtrlName); i++)
    {
        if (g_mSkin.find(g_aszCtrlName[i]) == g_mSkin.end())
        {
            continue;
        }

        AnsiString strValue = g_mSkin[g_aszCtrlName[i]];

        if (strValue == strAppSkinName &&
            strcmp(g_aszCtrlName[i], "Application") != 0)
        {
            Conf.Del("skin", g_aszCtrlName[i]);
        }
        else
        {
            Conf.Set("skin", g_aszCtrlName[i], strValue.c_str());
        }
    }

    Conf.Save();
}

void __fastcall TDialogSetSkin::InitSkin()
{
}
//---------------------------------------------------------------------------
__fastcall TDialogSetSkin::TDialogSetSkin(TComponent* Owner)
    : TDialogForm(Owner)
{
    for (UINT32 i=0; i<ARRAY_SIZE(g_aszCtrlName); i++)
    {
        cbCtrl->Properties->Items->Add(g_aszCtrlName[i]);
    }

    for (UINT32 i=0; i<ARRAY_SIZE(g_aszSkinName); i++)
    {
        cbSkin->Properties->Items->Add(g_aszSkinName[i]);
    }

    cbCtrl->ItemIndex = 0;

   //   Init();
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetSkin::Init()
{
    m_mSkin = g_mSkin;

    cbCtrlPropertiesChange(this);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetSkin::btnOKClick(TObject *Sender)
{
    m_mSkin = g_mSkin;

    SetSystemSkin();

    SaveSkinCfg();

    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetSkin::btnCancelClick(TObject *Sender)
{
    //Init();
    g_mSkin = m_mSkin;

    SetSystemSkin();

    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetSkin::SetSystemSkin()
{
    VECTOR<TSubForm*>   vForm;

    GetFormList(vForm);
    for (UINT32 i=0; i<vForm.size(); i++)
    {
        TSubForm    *Form = vForm[i];

        Form->SetSkin();
        Form->InitSkin();
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetSkin::FormClose(TObject *Sender, TCloseAction &Action)
{
    btnCancelClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetSkin::cbCtrlPropertiesChange(TObject *Sender)
{
    AnsiString  strCtrl = cbCtrl->Text;

    if (g_mSkin.find(strCtrl) == g_mSkin.end())
    {
        AnsiString  strSkin = g_mSkin["Application"];
        cbSkin->ItemIndex = cbSkin->Properties->Items->IndexOf(strSkin);
    }
    else
    {
        AnsiString  strSkin = g_mSkin[strCtrl];
        cbSkin->ItemIndex = cbSkin->Properties->Items->IndexOf(strSkin);
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetSkin::FormActivate(TObject *Sender)
{
    Init();
}
//---------------------------------------------------------------------------
void __fastcall TDialogSetSkin::cbSkinPropertiesChange(TObject *Sender)
{
    AnsiString  strCtrl = cbCtrl->Text;

    g_mSkin[strCtrl] = cbSkin->Text;

    SetSystemSkin();
}
//---------------------------------------------------------------------------

void __fastcall TDialogSetSkin::FormCreate(TObject *Sender)
{
    SetImage(btnOK, IMG_OK);
    SetImage(btnCancel, IMG_CANCEL);
}
//---------------------------------------------------------------------------

