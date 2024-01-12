//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"
#include "MsgDialog.h"
#include "GCommon.h"
#include "ClientUtil.h"
#include "main.h"
#include "ThreadFormat.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxControls"
#pragma link "cxGraphics"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxPC"
#pragma link "dxSkinBlack"
#pragma link "dxSkinBlue"
#pragma link "dxSkinCaramel"
#pragma link "dxSkinCoffee"
#pragma link "dxSkinDarkRoom"
#pragma link "dxSkinDarkSide"
#pragma link "dxSkinFoggy"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkiniMaginary"
#pragma link "dxSkinLilian"
#pragma link "dxSkinLiquidSky"
#pragma link "dxSkinLondonLiquidSky"
#pragma link "dxSkinMcSkin"
#pragma link "dxSkinMoneyTwins"
#pragma link "dxSkinOffice2007Black"
#pragma link "dxSkinOffice2007Blue"
#pragma link "dxSkinOffice2007Green"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinOffice2007Silver"
#pragma link "dxSkinOffice2010Black"
#pragma link "dxSkinOffice2010Blue"
#pragma link "dxSkinOffice2010Silver"
#pragma link "dxSkinPumpkin"
#pragma link "dxSkinsCore"
#pragma link "dxSkinscxPCPainter"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinsdxStatusBarPainter"
#pragma link "dxSkinSeven"
#pragma link "dxSkinSharp"
#pragma link "dxSkinSilver"
#pragma link "dxSkinSpringTime"
#pragma link "dxSkinStardust"
#pragma link "dxSkinSummer2008"
#pragma link "dxSkinValentine"
#pragma link "dxSkinXmas2008Blue"
#pragma link "dxStatusBar"
#pragma link "dxTabbedMDI"
#pragma link "cxClasses"
#pragma link "cxCustomData"
#pragma link "cxData"
#pragma link "cxDataStorage"
#pragma link "cxDBData"
#pragma link "cxEdit"
#pragma link "cxFilter"
//#pragma link "cxGrid"
#pragma link "cxStyles"
#pragma link "dxRibbon"
#pragma link "dxRibbonSkins"
#pragma link "dxSkinsForm"
#pragma link "dxLayoutContainer"
#pragma link "dxLayoutControl"
#pragma link "dxLayoutLookAndFeels"
#pragma link "cxInplaceContainer"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "cxDBTL"
#pragma link "cxTLData"
#pragma link "cxMaskEdit"
#pragma link "cxContainer"
#pragma link "cxTreeView"
#pragma link "cxTextEdit"
#pragma link "cxButtons"
#pragma link "dxBar"
#pragma link "dxBarExtItems"
#pragma link "dxSkinsdxBarPainter"
#pragma link "dxNavBar"
#pragma link "dxNavBarBase"
#pragma link "dxNavBarCollns"
#pragma link "dxSkinsdxNavBarPainter"
#pragma link "dxNavBarStyles"
#pragma link "cxButtonEdit"
#pragma link "cxCalc"
#pragma link "cxHyperLinkEdit"
#pragma link "cxListView"
#pragma link "cxShellCommon"
#pragma link "cxShellTreeView"
#pragma link "cxBarEditItem"
#pragma link "cxCheckBox"
#pragma link "cxSplitter"
#pragma link "cxDropDownEdit"
#pragma link "cxLabel"
#pragma link "cxGroupBox"
#pragma link "cxPropertiesStore"
#pragma link "cxProgressBar"
#pragma link "cxTrackBar"
#pragma link "cxMemo"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinDevExpressStyle"
#pragma link "dxSkinHighContrast"
#pragma link "dxSkinMetropolis"
#pragma link "dxSkinMetropolisDark"
#pragma link "dxSkinOffice2013DarkGray"
#pragma link "dxSkinOffice2013LightGray"
#pragma link "dxSkinOffice2013White"
#pragma link "dxSkinOffice2016Colorful"
#pragma link "dxSkinOffice2016Dark"
#pragma link "dxSkinSevenClassic"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinTheAsphaltWorld"
#pragma link "dxSkinTheBezier"
#pragma link "dxSkinVisualStudio2013Blue"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "dxSkinVisualStudio2013Light"
#pragma link "dxSkinVS2010"
#pragma link "dxSkinWhiteprint"
#pragma link "dxTaskbarProgress"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxDrawTextUtils"
#pragma link "dxBkgnd"
#pragma link "dxPrnDev"
#pragma link "dxPrnPg"
#pragma link "dxPSCompsProvider"
#pragma link "dxPSCore"
#pragma link "dxPScxEditorProducers"
#pragma link "dxPScxExtEditorProducers"
#pragma link "dxPScxPageControlProducer"
#pragma link "dxPSEdgePatterns"
#pragma link "dxPSEngn"
#pragma link "dxPSFillPatterns"
#pragma link "dxPSGlbl"
#pragma link "dxPSPDFExport"
#pragma link "dxPSPDFExportCore"
#pragma link "dxPSPrVwAdv"
#pragma link "dxPSPrVwRibbon"
#pragma link "dxPSPrVwStd"
#pragma link "dxPSUtl"
#pragma link "dxWrap"
#pragma link "trayicon"
#pragma link "FolderDialog"
#pragma resource "*.dfm"
TFormMain *FormMain;


AnsiString  g_strVer = "V1.0";

ThreadFormat    *g_ThreadFormat = NULL;

void __fastcall TFormMain::InitSkin()
{
}

//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TSubForm(Owner)
{
    Application->Title = "Formater - " + g_strVer;
    Caption = Application->Title;

    g_ThreadFormat = new ThreadFormat();
    g_ThreadFormat->Start();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    if (g_ThreadFormat)
    {
        g_ThreadFormat->Stop();
        delete g_ThreadFormat;
    }

    CanClose = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::GetFilePostfix(VECTOR<AnsiString> &vFilePostfix)
{
    AnsiString  strText = edtFileType->Text.Trim().LowerCase();
    AnsiString  strType;

    vFilePostfix.clear();
    while(!strText.IsEmpty())
    {
        int iPos = strText.Pos(";");

        if (iPos > 0)
        {
            strType = strText.SubString(1, iPos-1).Trim();
            strText = strText.SubString(iPos+1, strText.Length());

            vFilePostfix.push_back(strType);
        }
        else if (!strText.IsEmpty())
        {
            vFilePostfix.push_back(strText);
            break;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnOpenClick(TObject *Sender)
{
    if (edtDir->Text.IsEmpty())
    {
        return;
    }

    char    acPath[256];

    StrCpy(acPath, edtDir->Text);

    if (!gos_file_exist(acPath))
    {
        InfoMsg("目录不存在");
        return;
    }

    cbFormatPropertiesChange(this);

    VECTOR<AnsiString>     vFilePostfix;
    VECTOR<AnsiString>     vFile;

    GetFilePostfix(vFilePostfix);

    if (!GetFileList(acPath, vFilePostfix, vFile, TRUE))
    {
        InfoMsg("获取文件列表失败");
        return;
    }

    InitFormatFile(vFile);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::TimerCheckTimer(TObject *Sender)
{
    UINT32  ulTotalNum = 0;
    UINT32  ulCurrNum = 0;

    GetFormatInfo(ulTotalNum, ulCurrNum);

    if (ulTotalNum > 0)
    {
        pbMake->Properties->Max = ulTotalNum;
        pbMake->Position = ulCurrNum;
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::cbFormatPropertiesChange(TObject *Sender)
{
    FILE_FORMAT_CFG_T   stCfg = {0};

    stCfg.bConvertTab = cbConvertTab->Checked;
    stCfg.bTrimRight  = cbTrimRight->Checked;
    stCfg.bUnixFile   = cbUnixFile->Checked;
    stCfg.bAnsiFile   = cbAnsiFile->Checked;

    g_ThreadFormat->SetCfg(stCfg);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::cxButton1Click(TObject *Sender)
{
    ClearFormatFile();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::btnCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::edtDirPropertiesButtonClick(TObject *Sender, int AButtonIndex)
{
    FolderDialog->Title = "打开目录或文件";

    if (!FolderDialog->Execute())
    {
        return;
    }

    edtDir->Text = FolderDialog->Directory;
}
//---------------------------------------------------------------------------

