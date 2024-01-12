//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MDIForm.h"
#include "g_include.h"
#include "msg.h"
#include "ds_public.h"
#include "MsgDialog.h"
#include "ClientUtil.h"
#include "RemoteMsg.h"
#include "ClientInit.h"
#include "ThreadOma.h"
#include "main.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "MDIForm"
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
#pragma link "cxRadioGroup"
#pragma link "cxImageList"
#pragma link "cxPropertiesStore"
#pragma resource "*.dfm"
TFormMain *FormMain;

CHAR            *g_szConfigFile = "oma.ini";
CHAR            *g_szVersion = "v1.0.3";
BOOL            g_bATSValid = TRUE;
BOOL            g_bNTPValid = TRUE;
BOOL            g_bMDCValid = TRUE;
UINT8           g_aucMdcAddr[4];
UINT8           g_aucOmcAddr[4];

#define OMA_NUM     1000

ThreadOma           *g_apThreadOma[OMA_NUM];
TcxTreeListNode     *g_apNode[OMA_NUM];

UINT32  g_ulOmaNum = 0;
BOOL    g_bSendPackage = TRUE;
UINT32  ulAlarmRaise = 0;
UINT32   ulAlarmClear=0;
GMutex  g_MutexAlarmRaise;
GMutex  g_MutexAlarmClear;
BOOL __fastcall TFormMain::InitThreadOma(int iNum)
{
    CHAR    acOmaMac[32];
    UINT16  usSnmpPort = 45600;
    UINT32  ulMAC1 = 0;
    UINT32  ulMAC2 = 0;
    tl->Clear();

    for (UINT32 i=0; i<ARRAY_SIZE(g_apThreadOma); i++)
    {
        if (g_apThreadOma[i])
        {
            g_apThreadOma[i]->Free();
        }
    }

    if (iNum <= 0 || iNum >= 1000)
    {
        return FALSE;
    }


    memset(g_apThreadOma, 0, sizeof(g_apThreadOma));
    g_ulOmaNum = 0;

    tl->BeginUpdate();
    for (int i=0; i<iNum; i++)
    {
        ThreadOma   *pThreadOma = new ThreadOma(OMC_DEV_TYPE_TX, TRUE);
        ulMAC1 = i/256;
        ulMAC2 = i%256;

        sprintf(acOmaMac, "00:34:CB:01:%02X:%02x", ulMAC1, ulMAC2);
        if (!pThreadOma->Init(acOmaMac, usSnmpPort+i+1, i))
        {
            GosLog(LOG_ERROR, "thread oma init failed");
            return FALSE;
        }

        g_ulOmaNum++;
        pThreadOma->Start();
        g_apThreadOma[i] = pThreadOma;

        TcxTreeListNode *Node = tl->Add();

        Node->Values[0] = i;
        Node->Values[1] = acOmaMac;
        Node->Values[2] = "";

        g_apNode[i] = Node;
    }
    tl->EndUpdate();

    return TRUE;
}

//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TSubForm(Owner)
{
    memset(g_apThreadOma, 0, sizeof(g_apThreadOma));
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerCheckTimer(TObject *Sender)
{
    tl->BeginUpdate();

    for (UINT32 i=0; i<g_ulOmaNum; i++)
    {
        g_apNode[i]->Values[2] = g_apThreadOma[i]->IsOnline()?"ÔÚÏß":"";
    }

    tl->EndUpdate();
    cxTextEdit1->Text = ulAlarmRaise;
    cxTextEdit2->Text = ulAlarmClear;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnReplyClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    for (UINT32 i=0; i<g_ulOmaNum; i++)
    {
        if (g_apThreadOma[i])
        {
            g_apThreadOma[i]->Free();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::rbAlarmClick(TObject *Sender)
{
    INT32       iAlarmTest = -1;

    if (rbAlarmRaise->Checked)
    {
        iAlarmTest = 1;
    }
    else if (rbAlarmClear->Checked)
    {
        iAlarmTest = 0;
    }

    BOOL    bIsStopSend = false;

    if (rbNoSend->Checked)
    {
        bIsStopSend = true;
    }

    for (UINT32 i=0; i<g_ulOmaNum; i++)
    {
        g_apThreadOma[i]->AlarmTest(iAlarmTest);
        g_apThreadOma[i]->SendPackageStatus(bIsStopSend);
    }

}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnStartClick(TObject *Sender)
{
    int iNum = edtNeNum->Text.ToIntDef(0);

    InitThreadOma(iNum);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerFlagTimer(TObject *Sender)
{
    return;
    static UINT32 ulFlag = 0;

    for (UINT32 i=0; i<g_ulOmaNum; i++)
    {
        g_apThreadOma[i]->AlarmTest(ulFlag%2);
    }

    ulFlag++;
}
//---------------------------------------------------------------------------

