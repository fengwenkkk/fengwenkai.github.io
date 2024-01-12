//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"

#include "MDIForm.h"
#include "TableCfg.h"
#include "MsgDialog.h"
#include "Resource.h"
#include "ModulePrint.h"

#include "dxPScxCommon.hpp"
#include "dxPSCore.hpp"
#include "dxPrnDlg.hpp"
#include "dxPScxTLLnk.hpp"
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
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxImage"
#pragma link "cxImageList"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "dxGDIPlusClasses"
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
#pragma link "cxClasses"
#pragma link "cxDrawTextUtils"
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
#pragma link "cxCustomData"
#pragma link "cxInplaceContainer"
#pragma link "cxStyles"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "cxLocalization"
#pragma link "cxMemo"
#pragma link "cxTextEdit"
#pragma link "cxButtons"
#pragma link "cxHint"
#pragma link "dxCustomHint"
#pragma link "dxScreenTip"
//#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma resource "*.dfm"

extern const char *g_szDevExpLocIni;

static TFormResource *FormResource = NULL;

typedef struct
{
    UINT32      ulImageID;
    TcxButton   *btn;
}IMG_INFO_T;

TFormResource* GetFormResource()
{
    if (!FormResource)
    {
        Application->CreateForm(__classid(TFormResource), &FormResource);
    }

    return FormResource;
}

void FreeFormResource()
{
	if (FormResource)
	{
		delete FormResource;
		FormResource = NULL;
    }
}

TcxStyle* GetTreeListHeaderStyle()
{
    return GetFormResource()->StyleTreeListHeader;
}

TdxSmartGlyph* GetTreeListHeaderGlyph(int iTreeListNodeHeight)
{
    TdxSmartGlyph   *Glyph;

    if (iTreeListNodeHeight <= 24)
    {
        Glyph = GetFormResource()->tlHeaderHeight->Columns[0]->Caption->Glyph;
    }
    else if (iTreeListNodeHeight <= 26)
    {
        Glyph = GetFormResource()->tlHeaderHeight->Columns[1]->Caption->Glyph;
    }
    else if (iTreeListNodeHeight <= 28)
    {
        Glyph = GetFormResource()->tlHeaderHeight->Columns[2]->Caption->Glyph;
    }
    else
    {
        Glyph = GetFormResource()->tlHeaderHeight->Columns[3]->Caption->Glyph;
    }

    return Glyph;
}

TcxButton* __fastcall TFormResource::GetImage(UINT32 ulImageID)
{
    static IMG_INFO_T   astImage[] = {
                        {IMG_OK,            btnOK},
                        {IMG_CANCEL,        btnCancel},
                        {IMG_STOP,          btnStop},
                        {IMG_CLOSE,         btnClose},
                        {IMG_EXIT,          btnExit},
                        {IMG_HIDE,          btnHide},

                        {IMG_PRINT,         btnPrint},
                        {IMG_QUERY,         btnQuery},
                        {IMG_IMPORT,        btnImport},
                        {IMG_EXPORT,        btnExport},

                        {IMG_PLAY,          btnPlay},
                        {IMG_PAUSE,         btnPause},
                        {IMG_RECV,          btnRecv},
                        {IMG_DOWNLOAD,      btnDownload},
                        {IMG_NEXT,          btnNext},
                        {IMG_PREV,          btnPrev},

                        {IMG_PTT_DIAL,      btnPTTDial},
                        {IMG_ACCEPT_CALL,   btnAcceptCall},
                        {IMG_REJECT_CALL,   btnRejectCall},
                        {IMG_P2P_CALL,      btnDial},
                        {IMG_HANGOFF,       btnAcceptCall},
                        {IMG_HANGUP,        btnRejectCall},

                        {IMG_START_LISTEN,  btnStartListen},
                        {IMG_STOP_LISTEN,   btnStopListen},

                        {IMG_SDS,           btnSDS},
                        {IMG_SDS_REPLY,     btnSDSReply},
                        {IMG_BRD,           btnBrd},

                        {IMG_ALL_LINE,      btnAllLine},
                        {IMG_UP_LINE,       btnUpLine},
                        {IMG_DOWN_LINE,     btnDownLine},

                        {IMG_ADD,           btnAdd},
                        {IMG_DEL,           btnDel},
                        {IMG_SET,           btnSet},
                        {IMG_REFRESH,       btnRefresh},
                        {IMG_SWITCH,        btnSwitch},

                        {IMG_SUB_GROUP,     btnSubGroup},
                        {IMG_UNSUB_GROUP,   btnUnsubGroup},
                        {IMG_JOIN_GROUP,    btnJoinGroup},
                        {IMG_MEMBER,        btnMember},

            };

    for (UINT32 i=0; i<ARRAY_SIZE(astImage); i++)
    {
        if (astImage[i].ulImageID == ulImageID)
        {
            return astImage[i].btn;
        }
    }

    return NULL;
}

//---------------------------------------------------------------------------
__fastcall TFormResource::TFormResource(TComponent* Owner)
    : TForm(Owner)
{
    InitDevExpLocalizer();
}
//---------------------------------------------------------------------------
void __fastcall TFormResource::InitDevExpLocalizer()
{
    char    acIniFile[256];

    sprintf(acIniFile, "%s/%s", gos_get_root_path(), "dev_loc.ini");

    GosLog(LOG_INFO, "init localizer");

#if 0
    if (!gos_file_exist(acIniFile))
    {
        FILE    *fp = fopen(acIniFile, "w+b");

        if (!fp)
        {
            return;
        }

        fwrite(g_szDevExpLocIni, 1, strlen(g_szDevExpLocIni)+1, fp);
        fclose(fp);
    }
#else
    mmLocalize->Lines->SaveToFile(acIniFile);
#endif

    try
    {
        if (gos_file_exist(acIniFile) )
        {
            DevExpLocalizer->StorageType = lstIni;
            DevExpLocalizer->FileName = acIniFile;

            GosLog(LOG_INFO, "start localizer");
            DevExpLocalizer->Active = true;
            DevExpLocalizer->Locale = 2052;

            GosLog(LOG_INFO, "init localizer successful");
        }
    }
    catch(Exception &e)
    {
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormResource::SetImage(TcxButton *btn, UINT32 ulImadeID)
{
    TcxButton    *Button = GetImage(ulImadeID);

    if (!Button)
    {
        return;
    }

    btn->Glyph = Button->Glyph;
}
//---------------------------------------------------------------------------
void __fastcall TFormResource::SetImage(TdxBarButton *btn, UINT32 ulImadeID)
{
    TcxButton    *Button = GetImage(ulImadeID);

    if (!Button)
    {
        return;
    }

    btn->Glyph = Button->Glyph;
}
//---------------------------------------------------------------------------
void __fastcall TFormResource::SetImage(TdxBarLargeButton *btn, UINT32 ulImadeID)
{
    TcxButton    *Button = GetImage(ulImadeID);

    if (!Button)
    {
        return;
    }

    btn->Glyph = Button->Glyph;
}
//---------------------------------------------------------------------------
void SetImage(TcxButton *btn, UINT32 ulImadeID)
{
    GetFormResource()->SetImage(btn, ulImadeID);
}
//---------------------------------------------------------------------------
void SetImage(TdxBarButton *btn, UINT32 ulImadeID)
{
    GetFormResource()->SetImage(btn, ulImadeID);
}
//---------------------------------------------------------------------------
void SetImage(TdxBarLargeButton *btn, UINT32 ulImadeID)
{
    GetFormResource()->SetImage(btn, ulImadeID);
}
//---------------------------------------------------------------------------
void Print(TcxTreeList *treeList, AnsiString strTitle)
{
    if (!DataModulePrint)
    {
        DataModulePrint = new TDataModulePrint(Application);
    }

    DataModulePrint->PrintComponent(treeList, strTitle);
}


void __fastcall TFormResource::tlMainCustomDrawBackgroundCell(TcxCustomTreeList *Sender,
          TcxCanvas *ACanvas, TcxTreeListCustomCellViewInfo *AViewInfo,
          bool &ADone)
{
    TColor  ContentColor = ACanvas->Brush->Color;

    SetTreeListContentColor(ContentColor);
}
//---------------------------------------------------------------------------

