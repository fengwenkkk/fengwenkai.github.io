//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"
#include "GLog.h"
#include "msg.h"
#include "ds_public.h"
#include "MsgDialog.h"
#include "ClientUtil.h"
#include "RemoteMsg.h"
#include "ClientInit.h"
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxGraphics"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "dxSkinLilian"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxDropDownEdit"
#pragma link "cxEdit"
#pragma link "cxMaskEdit"
#pragma link "cxTextEdit"
#pragma link "cxCustomData"
#pragma link "cxInplaceContainer"
#pragma link "cxStyles"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "dxSkinsForm"
#pragma link "cxPC"
#pragma link "dxSkinscxPCPainter"
#pragma link "cxGroupBox"
#pragma link "cxClasses"
#pragma link "cxCheckBox"
#pragma link "cxSplitter"
#pragma link "cxLabel"
#pragma link "dxBarBuiltInMenu"
#pragma link "AdvGrid"
#pragma link "AdvObj"
#pragma link "AdvUtil"
#pragma link "BaseGrid"
#pragma link "cxButtonEdit"
#pragma link "SHDocVw_OCX"
#pragma link "cxMemo"
#pragma link "cxImage"
#pragma link "dxGDIPlusClasses"
#pragma resource "*.dfm"

TFormMain *FormMain;

BOOL    g_bShowTitle = TRUE;

TIdContext  *g_AContext = NULL;

UINT32  g_ulZoneID = 0;

BOOL LoadAudioDevCfg()
{
}

UINT32 GetZoneID(AnsiString strMsg)
{
// "zone_ids": [1,2,3]
    char    *szMsg = strMsg.c_str();
    char    *szTmp;

    szMsg = strstr(szMsg, "\"zone_ids\":");
    if (!szMsg)
    {
        return 0;
    }

    szMsg += strlen("\"zone_ids\":");
    szMsg = gos_trim_string(szMsg);
    if (*szMsg != '[')
    {
        return 0;
    }

    szMsg++;
    szMsg = gos_trim_string(szMsg);
    szTmp = strchr(szMsg, ']');
    if (!szTmp)
    {
        return 0;
    }

    char    acTmp[16];
    UINT32  ulLen = szTmp-szMsg;
    UINT32  ulValue = 0;

    if (ulLen > 12)
    {
        return 0;
    }

    memset(acTmp, 0, sizeof(acTmp));
    memcpy(acTmp, szMsg, szTmp-szMsg);

    if (!gos_atou(acTmp, &ulValue))
    {
        return 0;
    }

    return ulValue;
}

//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TSubForm(Owner)
{
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::SendRsp(TIdContext *AContext)
{
    if (!AContext)
    {
        return;
    }

    String str = "";
    CHAR    acMsg[1024];
    bool    bRet = cbReturnSucc->Checked;

    sprintf(acMsg, "[{\"id\": %u, \"result\": %s, \"reason\": \"%s\" }]",
            g_ulZoneID, bRet?"true":"false", bRet?"广播成功":"广播失败");
    str = acMsg;

    AContext->Connection->IOHandler->WriteLn(str);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TCPServerExecute(TIdContext *AContext)
{
    String str = "";

    while (!AContext->Connection->IOHandler->InputBufferIsEmpty())
    {
        str += AContext->Connection->IOHandler->ReadString(1);//, TEncoding::UTF8);
    }

    mm->Lines->Append("== new req ==");
    mm->Lines->Append(str);

    g_ulZoneID = GetZoneID(str);

    g_AContext = AContext;

    if (cbAutoReply->Checked)
    {
        SendRsp(AContext);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnReplyClick(TObject *Sender)
{
    SendRsp(g_AContext);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::cxButton1Click(TObject *Sender)
{
//  tl->Columns[0]->Caption->Glyph = tl->Columns[1]->Caption->Glyph;

    TMemoryStream       *g_EmptyPicStream = new TMemoryStream();


    g_EmptyPicStream->Clear();
 // imgEmpty->AutoSize = true;
    imgEmpty->Picture->SaveToStream(g_EmptyPicStream);


        tl->Columns[0]->Caption->Glyph->LoadFromStream(g_EmptyPicStream);

}
//---------------------------------------------------------------------------

