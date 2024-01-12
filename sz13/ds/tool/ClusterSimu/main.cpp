//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"
#include "MsgDialog.h"
#include "ClientUtil.h"
#include "msg.h"
#include "GLog.h"
#include "main.h"
#include "ThreadCheck.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxGraphics"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
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
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSeven"
#pragma link "dxSkinSharp"
#pragma link "dxSkinSilver"
#pragma link "dxSkinSpringTime"
#pragma link "dxSkinStardust"
#pragma link "dxSkinSummer2008"
#pragma link "dxSkinValentine"
#pragma link "dxSkinXmas2008Blue"
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
#pragma link "cxMemo"
#pragma link "dxBarBuiltInMenu"
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
#pragma link "cxLabel"
#pragma link "cxCheckBox"
#pragma link "cxCalendar"
#pragma link "dxStatusBar"
#pragma resource "*.dfm"

TFormMain *FormMain;

ThreadCheck     *g_ThreadCheck1 = NULL;
ThreadCheck     *g_ThreadCheck2 = NULL;

UINT16  g_usMasterUDPPort = 43000 + 1;
UINT16  g_usSlaveUDPPort = 43000 + 2;

AnsiString GetStatus(UINT32 ulStatus)
{
    AnsiString  strStatus = "未知";

    if (ulStatus == CLUSTER_STATUS_MASTER)
    {
        strStatus = "主机";
    }
    else if (ulStatus == CLUSTER_STATUS_SLAVE)
    {
        strStatus = "备机";
    }
    else if (ulStatus == CLUSTER_STATUS_CHECK_FAIL)
    {
        strStatus = "检测失败";
    }

    strStatus = AnsiString(ulStatus) + AnsiString(" ") + strStatus;

    return strStatus;
}

//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TSubForm(Owner)
{
    GosInitLog();

   //   InitMsgHandler(MSG_PIS, OnPISMsg);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::OnInnerMsg(UINT8 *pucMsg, UINT32 ulMsgLen)
{
  /*    PIS_MSG_HDR_T   *pstMsg = (PIS_MSG_HDR_T*)pucMsg;

    if (pstMsg->usMsgType == EV_PIS_GET_TRAIN_INFO)
    {
        OnSAGetTrainInfoReq();
    }
     */
}
//---------------------------------------------------------------------------
/*void __fastcall TFormMain::OnGetSelfCheckReq(PIS_SELFCHECK_STATUS_REQ_T *pstMsg)
{
    if (0 == pstMsg->ucCheckStatus)
    {
        edtSelfCheckInfo->Text = "0:正常";
    }
    else
    {
        edtSelfCheckInfo->Text = AnsiString(pstMsg->ucCheckStatus) + ":失败";
    }
}    */
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerCheckTimer(TObject *Sender)
{
    TimerCheck->Interval = 1000;

    UINT8   aucMasterAddr[4];
    UINT32  ulStatus;
    UINT32  ulPeerStatus;

    if (!GetIP(edtMasterAddr->Text, aucMasterAddr))
    {
        return;
    }

    if (cbOffline1->Checked)
    {
        if (g_ThreadCheck1)
        {
            g_ThreadCheck1->Stop();
            g_ThreadCheck1 = NULL;
        }
    }
    else
    {
        if (!g_ThreadCheck1)
        {
            UINT8   aucLocalAddr[] = {127,0,0,1};
            UINT32  ulDefaultStatus = CLUSTER_STATUS_MASTER;
            CHAR    *szIfName = "eth1";

            g_ThreadCheck1 = new ThreadCheck(g_usMasterUDPPort, aucLocalAddr, g_usSlaveUDPPort, ulDefaultStatus, aucMasterAddr, szIfName);
            if (!g_ThreadCheck1->Init())
            {
                delete g_ThreadCheck1;
                g_ThreadCheck1 = NULL;

                return;
            }

            g_ThreadCheck1->Start();

            gb1->Enabled = true;
        }
        else
        {
            ulStatus = g_ThreadCheck1->GetStatus();
            ulPeerStatus = g_ThreadCheck1->GetPeerStatus();

            edtStatus1->Text = GetStatus(ulStatus);
            edtPeerStatus1->Text = GetStatus(ulPeerStatus);
        }
    }

    if (cbOffline2->Checked)
    {
        if (g_ThreadCheck2)
        {
            g_ThreadCheck2->Stop();
            g_ThreadCheck2 = NULL;
        }
    }
    else
    {
        if (!g_ThreadCheck2)
        {
            UINT8   aucLocalAddr[] = {127,0,0,1};
            UINT32  ulDefaultStatus = CLUSTER_STATUS_SLAVE;
            CHAR    *szIfName = "eth2";

            g_ThreadCheck2 = new  ThreadCheck(g_usSlaveUDPPort, aucLocalAddr, g_usMasterUDPPort, ulDefaultStatus, aucMasterAddr, szIfName);
            if (!g_ThreadCheck2->Init())
            {
                delete g_ThreadCheck2;
                g_ThreadCheck2 = NULL;

                return;
            }

            g_ThreadCheck2->Start();
            gb2->Enabled = true;
        }
        else
        {
            ulStatus = g_ThreadCheck2->GetStatus();
            ulPeerStatus = g_ThreadCheck2->GetPeerStatus();

            edtStatus2->Text = GetStatus(ulStatus);
            edtPeerStatus2->Text = GetStatus(ulPeerStatus);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::cbDisableSend1Click(TObject *Sender)
{
    if (g_ThreadCheck1)
    {
        g_ThreadCheck1->EnableSendMsg(!cbDisableSend1->Checked);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::cbDisableSend2Click(TObject *Sender)
{
    if (g_ThreadCheck2)
    {
        g_ThreadCheck2->EnableSendMsg(!cbDisableSend2->Checked);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::cbDisableRecv1Click(TObject *Sender)
{
    if (g_ThreadCheck1)
    {
        g_ThreadCheck1->EnableRecvMsg(!cbDisableRecv1->Checked);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::cbDisableRecv2Click(TObject *Sender)
{
    if (g_ThreadCheck2)
    {
        g_ThreadCheck2->EnableRecvMsg(!cbDisableRecv2->Checked);
    }
}
//---------------------------------------------------------------------------

