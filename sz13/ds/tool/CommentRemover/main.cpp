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

char * LeftTrimString(char *szText)
{
    while(szText && *szText)
    {
        if (*szText == ' ' ||
            *szText == '\t' )
        {
            szText++;
            continue;
        }

        break;
    }

    return szText;
}

char* line_strstr(char *szBegin, char *szEnd, char *szMark)
{
    UINT32  ulLen = strlen(szMark);

    for (; szBegin<=szEnd-ulLen; szBegin++)
    {
        if (memcmp(szBegin, szMark, ulLen) == 0)
        {
            return szBegin;
        }
    }

    return NULL;
}

void FormatFile(char *szFile)
{
    int     iFileSize = gos_get_file_size(szFile);
    FILE    *fp = NULL;
    char    *szText = NULL;
    char    acFile[256];
    char    *szOut = NULL;
    char    *szCurr;
    char    *szBegin;
    char    *szEnd;
    char    *szComment;
    char    *szComment1;
    char    *szComment2;
    UINT32  ulLen = 0;
    char    acNewLine[4] = {0};
    UINT32  ulNewLineLen = 0;
    bool    bNewLine = true;

    sprintf(acFile, "%s.bak", szFile);
    gos_rename_file(szFile, acFile);

    if (iFileSize <= 0)
    {
        return;
    }

    fp = fopen(acFile, "rb");
    if (!fp)
    {
        return;
    }

    szText = (char*)calloc(1, (iFileSize+2)*2);
    szOut = szText + iFileSize+2;
    if (iFileSize != fread(szText, 1, iFileSize, fp))
    {
        fclose(fp);
        fp = NULL;
        goto end;
    }

    strcpy(szText+iFileSize, "\n");
    fclose(fp);
    fp = NULL;

    szBegin = strstr(szText, "\r\n");
    if (szBegin)
    {
        strcpy(acNewLine, "\r\n");
    }
    else
    {
        szBegin = strstr(szText, "\n");
        if (szBegin)
        {
            strcpy(acNewLine, "\n");
        }
        else
        {
            fclose(fp);
            return;
        }
    }

    ulNewLineLen = strlen(acNewLine);

    szCurr = szText;
    bNewLine = true;
    while(*szCurr)
    {
        szBegin = LeftTrimString(szCurr);
        szEnd = strstr(szBegin, acNewLine);
        if (!szEnd)
        {
            break;
        }

        szComment1 = line_strstr(szBegin, szEnd, "/*");
        szComment2 = line_strstr(szBegin, szEnd, "<!--");
        if (szComment1)
        {
            if (szComment2 == NULL || szComment2 > szComment1)
            {
                // 保留comment前的内容
                szComment = szComment1;
                memcpy(szOut+ulLen, szCurr, szComment-szCurr);
                ulLen += szComment-szCurr;

                szCurr = strstr(szComment+2, "*/");
                if (!szCurr)
                {
                    break;
                }

                szCurr += 2;
                bNewLine = false;
                continue;
            }
        }

        if (szComment2)
        {
            // 保留comment前的内容
            szComment = szComment2;
            memcpy(szOut+ulLen, szCurr, szComment-szCurr);
            ulLen += szComment-szCurr;

            szCurr = strstr(szComment+4, "-->");
            if (!szCurr)
            {
                break;
            }

            szCurr += 3;
            bNewLine = false;
            continue;
        }

        *szEnd = '\0';

        szComment = strstr(szBegin, "//");
        if (szComment)
        {
            // 保留comment前的内容
            memcpy(szOut+ulLen, szCurr, szComment-szCurr);
            ulLen += szComment-szCurr;

            memcpy(szOut+ulLen, acNewLine, ulNewLineLen);
            ulLen += ulNewLineLen;

            szCurr = szEnd+ulNewLineLen;
            bNewLine = true;
            continue;
        }

        // 空行
        if (*szBegin == '\0')
        {
            szCurr = szEnd+ulNewLineLen;
            if (!bNewLine)
            {
                memcpy(szOut+ulLen, acNewLine, ulNewLineLen);
                ulLen += ulNewLineLen;
            }

            bNewLine = true;
            continue;
        }

        memcpy(szOut+ulLen, szCurr, szEnd-szCurr);
        ulLen += szEnd-szCurr;

        memcpy(szOut+ulLen, acNewLine, ulNewLineLen);
        ulLen += ulNewLineLen;

        szCurr = szEnd+ulNewLineLen;
        bNewLine = true;
    }

    /*
    if (ulLen == fwrite(szOut, 1, ulLen, fp))
    {
        fclose(fp);
        fp = NULL;


        gos_rename_file(acFile, szFile);
    }     */

    gos_save_string_to_file(szFile, szOut);

end:
    if (fp)
    {
        fclose(fp);
    }

    GOS_FREE(szText);
}

void __fastcall TFormMain::InitSkin()
{
}

//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TSubForm(Owner)
{
    Application->Title = "Comment Remover";
    Caption = Application->Title;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnStartClick(TObject *Sender)
{
    AnsiString  strFile = edtFile->Text.Trim();

    if (!FileExists(strFile))
    {
        InfoMsg("文件不存在");
        return;
    }

    FormatFile(strFile.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::edtDirPropertiesButtonClick(TObject *Sender, int AButtonIndex)
{
    OpenDialog->Title = "打开文件";

    if (!OpenDialog->Execute())
    {
        return;
    }

    edtFile->Text = OpenDialog->FileName;
}
//---------------------------------------------------------------------------

