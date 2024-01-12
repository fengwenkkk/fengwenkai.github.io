//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"

#include "MsgDialog.h"
#include "msg.h"
#include "ds_public.h"
#include "ClientUtil.h"
#include "Resource.h"
#include "ClientCommon.h"
#include "RemoteMsg.h"
#include "ClientInit.h"

#include "FrmTextReader.h"
#include "DlgSetTrainUE.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxCustomData"
#pragma link "cxDropDownEdit"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxGroupBox"
#pragma link "cxInplaceContainer"
#pragma link "cxLabel"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxMemo"
#pragma link "cxStyles"
#pragma link "cxTextEdit"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
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
#pragma link "MDIForm"
#pragma link "dxBar"
#pragma link "cxProgressBar"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxCheckBox"
#pragma resource "*.dfm"
TFormTextReader *FormTextReader;

extern "C" UINT32 gos_unicode_to_string(UINT16 usUnicode, CHAR *szText);

//---------------------------------------------------------------------------
__fastcall TFormTextReader::TFormTextReader(TComponent* Owner)
    : TSubForm(Owner)
{
    cbWordWrapPropertiesChange(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormTextReader::Init(char *szFile)
{
	cbWordWrap->Checked = false;

	CHAR    *szRawFile = gos_right_strchr(szFile, '/');
    if (!szRawFile)
    {
        szRawFile = szFile;
    }
    else
    {
        szRawFile++;
    }

    mmMain->Text = "";
    Caption = "日志文件 - " + String(szRawFile);
    int iFileSize = gos_get_file_size(szFile);

    if (iFileSize <= 0)
    {
        return;
    }

    iFileSize ++;

    FILE    *fp = fopen(szFile, "rb");

    if (!fp)
    {
        return;
    }

    char    *szText = (char*)calloc(1, iFileSize*4+4);

    // fread 居然会改变编码格式
    fread(szText, 1, iFileSize, fp);
    fclose(fp);

    mmMain->Text = Utf8ToAnsi(szText);
    free(szText);
}
//---------------------------------------------------------------------------
void __fastcall TFormTextReader::FormCreate(TObject *Sender)
{
    SetImage(btnClose, IMG_CLOSE);
}
//---------------------------------------------------------------------------
void __fastcall TFormTextReader::btnCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormTextReader::FormClose(TObject *Sender, TCloseAction &Action)
{
    Action = caHide;
}
//---------------------------------------------------------------------------
void __fastcall TFormTextReader::cbWordWrapPropertiesChange(TObject *Sender)
{
    mmMain->Properties->WordWrap = cbWordWrap->Checked;

    if (cbWordWrap->Checked)
    {
        mmMain->Properties->ScrollBars = ssVertical;
    }
    else
    {
        mmMain->Properties->ScrollBars = ssBoth;
    }
}
//---------------------------------------------------------------------------

