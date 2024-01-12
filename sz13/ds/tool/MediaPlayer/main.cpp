#include <vcl.h>
#pragma hdrstop

#include "g_include.h"
#include "ds_public.h"
#include "MsgDialog.h"
#include "RemoteMsg.h"

#include "ClientUtil.h"
#include "eSDK.h"
#include "eLTE_SDK.h"
#include "main.h"
#include "ThreadPlayAndRec.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxGraphics"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "dxSkinLilian"

#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinsForm"

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
#pragma link "cxPC"
#pragma link "dxSkinscxPCPainter"
#pragma link "cxGroupBox"
#pragma link "cxMemo"
#pragma link "cxLabel"
#pragma link "cxClasses"

#pragma link "dxBarBuiltInMenu"
#pragma link "cxRadioGroup"
#pragma link "cxProgressBar"

#pragma link "cxButtonEdit"
#pragma link "cxImageList"
#pragma link "cxCheckBox"
#pragma resource "*.dfm"

TFormMain *FormMain;

ThreadPlayAndRec    *g_ThreadPlayAndRec = NULL;

//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TSubForm(Owner)
{
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::btnStartClick(TObject *Sender)
{
    AnsiString  strUrl = edtURL->Text.Trim();
    HWND        hWndPlayer = gbVideo->Handle;
    CHAR        acRecFile[256] = {0};

    if (cbRec->Checked)
    {
        sprintf(acRecFile, "%s/iph_rec_%u.mp4", gos_get_root_path(), gos_get_current_time());
        gos_format_path(acRecFile);
    }

    if (!g_ThreadPlayAndRec)
    {
        g_ThreadPlayAndRec = new ThreadPlayAndRec(hWndPlayer, cbTcpMode->Checked);
        g_ThreadPlayAndRec->Start();
    }

    GosLog(LOG_INFO, "start IPH video: %s", strUrl.c_str());

    gbVideo->Visible = true;
    g_ThreadPlayAndRec->SetURL(strUrl.c_str());
    g_ThreadPlayAndRec->PlayAndRec(acRecFile);

}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnStopClick(TObject *Sender)
{
    if (g_ThreadPlayAndRec)
    {
        g_ThreadPlayAndRec->Stop();
    }

    gbVideo->Visible = false;
    //Update();
//    gbVideo->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    cbKeepPlay->Checked = false;

    if (g_ThreadPlayAndRec)
    {
        g_ThreadPlayAndRec->Stop();
        g_ThreadPlayAndRec->Free();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::Timer1Timer(TObject *Sender)
{
    if (g_ThreadPlayAndRec && cbKeepPlay->Checked)
    {
        g_ThreadPlayAndRec->KeepPlay();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::edtURLPropertiesButtonClick(TObject *Sender, int AButtonIndex)
{
    if (!OpenDialog->Execute())
    {
        return;
    }

    edtURL->Text = OpenDialog->FileName;
}
//---------------------------------------------------------------------------

