//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"

#include "MsgDialog.h"
#include "TableCfg.h"
#include "ModulePrint.h"

#include "dxPScxCommon.hpp"
#include "dxPSCore.hpp"
#include "dxPrnDlg.hpp"
#include "dxPScxTLLnk.hpp"
#include "dxBkgnd.hpp"
#include "dxPgsDlg.hpp"
#include "dxPrnDev.hpp"
#include "dxPrnPg.hpp"
#include "dxPSCompsProvider.hpp"
#include "dxPSCore.hpp"
#include "dxPScxEditorProducers.hpp"
#include "dxPScxExtEditorProducers.hpp"
#include "dxPScxPageControlProducer.hpp"
#include "dxPSEdgePatterns.hpp"
#include "dxPSEngn.hpp"
#include "dxPSFillPatterns.hpp"
#include "dxPSGlbl.hpp"
#include "dxPSPDFExport.hpp"
#include "dxPSPDFExportCore.hpp"
#include "dxPSPrVwAdv.hpp"
#include "dxPSPrVwRibbon.hpp"
#include "dxPSPrVwStd.hpp"
#include "dxPSUtl.hpp"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma classgroup "Vcl.Controls.TControl"
#pragma link "cxClasses"
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
#pragma link "dxSkinBlue"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "dxWrap"
#pragma link "cxStyles"
#pragma resource "*.dfm"
TDataModulePrint *DataModulePrint;
//---------------------------------------------------------------------------
__fastcall TDataModulePrint::TDataModulePrint(TComponent* Owner)
    : TDataModule(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TDataModulePrint::PrintComponent(TcxTreeList *treeList, AnsiString strTitle)
{
    AnsiString              strTableName = treeList->Name;
    TcxTreeListReportLink   *Link = (TcxTreeListReportLink *)Printer->AddLink(treeList);
    TABLE_PRINT_CFG_T       stNewPrintCfg;

    if (!Link)
    {
        return;
    }

    Link->OptionsFormatting->UseNativeStyles = true;
    Link->Styles->Header = StylePrinter;
    Link->PrinterPage->PageHeader->Font = StylePrintTitle->Font;
    Link->Styles->Content = StyleContent;
    Link->OptionsView->Footers = false;
    Link->OptionsView->BandHeaders = false;
    Link->OptionsSize->AutoWidth = true;
    Link->PrinterPage->CenterOnPageH = true;

    TABLE_CFG_T *pstTableCfg = LoadTableCfg(strTableName.c_str());

    if (pstTableCfg == NULL)
    {
        Link->PrinterPage->Margins->Left = 25000;
        Link->PrinterPage->Margins->Right = 12700;
        Link->PrinterPage->Margins->Top = 40000;
        Link->PrinterPage->Margins->Bottom = 30000;
    }
    else
    {
        Link->PrinterPage->Margins->Left   = pstTableCfg->stPageMargin.ulLeft;
        Link->PrinterPage->Margins->Right  = pstTableCfg->stPageMargin.ulRight;
        Link->PrinterPage->Margins->Top    = pstTableCfg->stPageMargin.ulTop;
        Link->PrinterPage->Margins->Bottom = pstTableCfg->stPageMargin.ulBottom;

        if (pstTableCfg->usPageOientation)
        {
            Link->PrinterPage->Orientation = Dxprndev::poLandscape;
        }
        else
        {
            Link->PrinterPage->Orientation = Dxprndev::poPortrait;
        }

        Link->Styles->Header->Font->Size   = pstTableCfg->stPrintTitleCfg.ulFontSize;
        Link->Styles->Header->Font->Height = pstTableCfg->stPrintTitleCfg.ulHeight;
    }

    Link->PrinterPage->PageHeader->CenterTitle->Text = strTitle;
    Link->PrinterPage->PageFooter->CenterTitle->Text = "[Page #]/[Total Pages]\r\n";//[第Page #页 / 共Pages #页]";

    Printer->Preview();

    if (!strTableName.IsEmpty())
    {
        if (Link->PrinterPage->Orientation == Printers::poPortrait)
        {
            stNewPrintCfg.usPageOientation = 0;
        }
        else
        {
            stNewPrintCfg.usPageOientation = 1;
        }

        stNewPrintCfg.stPageMargin.ulTop    = Link->PrinterPage->Margins->Top;
        stNewPrintCfg.stPageMargin.ulBottom = Link->PrinterPage->Margins->Bottom;
        stNewPrintCfg.stPageMargin.ulLeft   = Link->PrinterPage->Margins->Left;
        stNewPrintCfg.stPageMargin.ulRight  = Link->PrinterPage->Margins->Right;

        stNewPrintCfg.stPrintTitleCfg.ulFontSize = Link->Styles->Header->Font->Size;
        stNewPrintCfg.stPrintTitleCfg.ulBold     = 0;
        stNewPrintCfg.stPrintTitleCfg.ulHeight   = Link->Styles->Header->Font->Height;
        SaveTableCfg(strTableName.c_str(), &stNewPrintCfg);
    }

    try
    {
        Link->Print(false);//PrintEx(pnAll, 1, true);
        InfoMsg("打印结束");
    }
    catch(...)
    {
    }

    Printer->DeleteAllLinks();
}

