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
#include "FrmCfg.h"
#include "FrmTopo.h"
#include "FrmNeInfo.h"
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
#pragma link "cxImageComboBox"
#pragma link "cxImageList"
#pragma link "cxCheckBox"
#pragma link "cxCalendar"
#pragma link "cxDateUtils"
#pragma link "cxMaskEdit"
#pragma link "dxCore"
#pragma link "cxSplitter"
#pragma link "cxClasses"
#pragma resource "*.dfm"
TFormTopo *FormTopo;

int     g_iTopoColCount = 20;

extern TColor GetSkinColor(AnsiString strSkinName);

extern BOOL HasAlarm(CHAR *szNeID);

void InitChart(TChart *cht)
{
    AnsiString strSkinName = GetDefaultSkinName();
    TColor  ColorBackground = GetSkinColor(strSkinName);

    cht->Title->Visible                = true;
    cht->Title->Font->Color            = clNavy;
    cht->Title->Font->Size             = 12;

    cht->Align                         = alClient;
  //    cht->View3D                        = FALSE;
  //    cht->Legend->Visible               = FALSE;
//  cht->Walls->Visible                = FALSE;
    cht->ParentColor                   = false;
    cht->ParentBackground              = false;

    cht->Gradient->StartColor = ColorBackground; // RGB(234,234,234);
    cht->Gradient->EndColor = ColorBackground;  //RGB(234,234,234);
    cht->Gradient->MidColor = ColorBackground;  //RGB(255,255,255);
    cht->Gradient->Visible             = true;

    cht->Zoom->Allow                   = false;
    cht->AllowPanning                  = Teeprocs::pmNone;

    cht->BevelInner                    = bvNone;
    cht->BevelOuter                    = bvNone;
    cht->BevelWidth                    = 1;

  /*    cht->Border->Color                 = TColor(0x00FF8080);//chtMain->Border->Color;
    cht->Border->Style                 = psSolid;
    cht->Border->Visible               = TRUE;
    cht->Border->EndStyle              = esFlat;
    cht->Border->Width                 = 1;  */

    cht->LeftAxis->Grid->Style         = psDot;
    cht->LeftAxis->Grid->SmallDots     = true;
    //cht->LeftAxis->Grid->Color         = RGB(234,234,234); //clBlue;
    cht->LeftAxis->Grid->Color         = RGB(192,192,192); //clBlue;
    cht->LeftAxis->Grid->Width         = 1;
    cht->LeftAxis->Axis->Width         = 1;
    cht->LeftAxis->LabelsFont->Color   = clNavy;
    cht->LeftAxis->RoundFirstLabel     = false;

    if (cht->LeftAxis->AxisValuesFormat == "#,##0.0")
    {
        cht->LeftAxis->AxisValuesFormat = "0.0";
    }
    else if (cht->LeftAxis->AxisValuesFormat == "#,##0")
    {
        cht->LeftAxis->AxisValuesFormat = "0";
    }

//    cht->LeftAxis->Increment           = 5;
    cht->RightAxis->Grid->Style        = psDot;
    cht->RightAxis->Grid->Color        = clBlue;

   //   cht->BottomAxis->Visible           = TRUE;
    cht->BottomAxis->Grid->Width       = 1;
    cht->BottomAxis->Grid->Style       = psDot;
    cht->BottomAxis->Grid->SmallDots   = true;
    cht->BottomAxis->Grid->Color       = TColor(0x00FFB9B9);//chtMain->BottomAxis->Grid->Color;
    cht->BottomAxis->Axis->Width       = 1;
    cht->BottomAxis->LabelsFont->Color = clNavy;

    cht->BottomAxis->RoundFirstLabel   = false;
    cht->BottomAxis->Automatic         = true;

    cht->BackWall->Color = 12615680;
    cht->BackWall->Pen->Color = -1325465600;
    cht->Frame->Color = cht->BackWall->Pen->Color;//-1325465600;

//  cht->LeftAxis->Axis->Visible = false;
    cht->RightAxis->Axis->Visible = false;
    cht->TopAxis->Axis->Visible = false;
//  cht->BottomAxis->Axis->Visible = false;
//  cht->Walls->Visible = true;

   //   cht->TopAxis->Automatic            = TRUE;
}

//---------------------------------------------------------------------------
__fastcall TFormTopo::TFormTopo(TComponent* Owner)
    : TSubForm(Owner)
{
//  InitMsgHandler(EV_OMT_GET_TXSTATUS_RSP, OnGetTxStatusRsp);

    InitChart(chtTopo);
    chtTopo->Title->Visible = false;

    InitDevList();
}
//---------------------------------------------------------------------------
void __fastcall TFormTopo::OnGetTxStatusRsp(UINT8 *pucMsg, UINT32 ulMsgLen)
{
/*  OMT_GET_TXSTATUS_RSP_T  *pstRsp = (OMT_GET_TXSTATUS_RSP_T*)pucMsg;

    if (ulMsgLen < STRUCT_OFFSET(OMT_GET_TXSTATUS_RSP_T, astRec))
    {
        GosLog(LOG_ERROR, "invalid msg len %u of OMT_GET_TXSTATUS_RSP", ulMsgLen);
        return;
    }

    NTOHL_IT(pstRsp->ulNum);

    if (ulMsgLen != (STRUCT_OFFSET(OMT_GET_TXSTATUS_RSP_T, astRec) + pstRsp->ulNum*sizeof(pstRsp->astRec[0])))
    {
        GosLog(LOG_ERROR, "invalid msg len %u of OMT_GET_TXSTATUS_RSP", ulMsgLen);
        return;
    }

    TcxTreeListNode     *Node;

    tlMain->BeginUpdate();
    tlMain->Clear();

    SeriesRSRP->Clear();
    SeriesRSRQ->Clear();
    SeriesRSSI->Clear();
    SeriesSINR->Clear();

    char    acTime[32];

    for (UINT32 i=0; i<pstRsp->ulNum; i++)
    {
        NTOHL_IT(pstRsp->astRec[i].ulTime);
        NTOHL_IT(pstRsp->astRec[i].stStatus.stLteStatus.ulStationID);
        NTOHL_IT(pstRsp->astRec[i].stStatus.stResStatus.ulCpuUsage);
        NTOHL_IT(pstRsp->astRec[i].stStatus.stResStatus.ulMemUsage);
        NTOHL_IT(pstRsp->astRec[i].stStatus.stResStatus.ulDiskUsage);
        NTOHL_IT(pstRsp->astRec[i].stStatus.stLteStatus.ulPCI);
        NTOHL_IT(pstRsp->astRec[i].stStatus.stLteStatus.iRSRP);
        NTOHL_IT(pstRsp->astRec[i].stStatus.stLteStatus.iRSRQ);
        NTOHL_IT(pstRsp->astRec[i].stStatus.stLteStatus.iRSSI);
        NTOHL_IT(pstRsp->astRec[i].stStatus.stLteStatus.iSINR);
        NTOHL_IT(pstRsp->astRec[i].stStatus.ulRunStatus);

        Node = tlMain->Add();
        Node->Values[0] = pstRsp->astRec[i].ulTime;
        Node->Values[1] = pstRsp->astRec[i].acNeID;
        Node->Values[2] = pstRsp->astRec[i].stStatus.stResStatus.ulCpuUsage;
        Node->Values[3] = pstRsp->astRec[i].stStatus.stResStatus.ulMemUsage;
        Node->Values[4] = pstRsp->astRec[i].stStatus.stResStatus.ulDiskUsage;
        Node->Values[5] = pstRsp->astRec[i].stStatus.stLteStatus.ulPCI;
        Node->Values[6] = pstRsp->astRec[i].stStatus.stLteStatus.iRSRP;
        Node->Values[7] = pstRsp->astRec[i].stStatus.stLteStatus.iRSRQ;
        Node->Values[8] = pstRsp->astRec[i].stStatus.stLteStatus.iRSSI;
        Node->Values[9] = pstRsp->astRec[i].stStatus.stLteStatus.iSINR;

        gos_get_text_time_ex(&pstRsp->astRec[i].ulTime, NULL, acTime);

        if (i == 0)
        {
            SeriesRSRP->AddNullXY(pstRsp->astRec[i].ulTime, pstRsp->astRec[i].stStatus.stLteStatus.iRSRP, acTime+11);
            SeriesRSRQ->AddNullXY(pstRsp->astRec[i].ulTime, pstRsp->astRec[i].stStatus.stLteStatus.iRSRQ, acTime+11);
            SeriesRSSI->AddNullXY(pstRsp->astRec[i].ulTime, pstRsp->astRec[i].stStatus.stLteStatus.iRSSI, acTime+11);
            SeriesSINR->AddNullXY(pstRsp->astRec[i].ulTime, pstRsp->astRec[i].stStatus.stLteStatus.iSINR, acTime+11);
        }
        else
        {
            SeriesRSRP->AddXY(pstRsp->astRec[i].ulTime, pstRsp->astRec[i].stStatus.stLteStatus.iRSRP, acTime+11);
            SeriesRSRQ->AddXY(pstRsp->astRec[i].ulTime, pstRsp->astRec[i].stStatus.stLteStatus.iRSRQ, acTime+11);
            SeriesRSSI->AddXY(pstRsp->astRec[i].ulTime, pstRsp->astRec[i].stStatus.stLteStatus.iRSSI, acTime+11);
            SeriesSINR->AddXY(pstRsp->astRec[i].ulTime, pstRsp->astRec[i].stStatus.stLteStatus.iSINR, acTime+11);
        }
    }

    tlMain->EndUpdate();

    SetChartAxisRange(chtLte->LeftAxis, -120, 30);
//    SetChartAxisPrecision(chtLte->LeftAxis, 10);
    */
}

//---------------------------------------------------------------------------
void __fastcall TFormTopo::btnRefreshClick(TObject *Sender)
{
   /*   UINT32  ulTime = GetTime(deStartTime->Date);

    if (cbLastTime->Checked)
    {
        ulTime = gos_get_current_time();
    }

    if (cbAllDev->Checked)
    {
        OMT_GET_ALLTXSTATUS_REQ_T      stReq = {0};

        stReq.ulTime = 0xffffffff;
        HTONL_IT(stReq.ulTime);

        g_pRemoteMsg->SendReq(EV_OMT_GET_ALLTXSTATUS_REQ, &stReq, sizeof(stReq));
    }
*/
}
//---------------------------------------------------------------------------
void __fastcall TFormTopo::FormCreate(TObject *Sender)
{
    SetImage(btnRefresh, IMG_REFRESH);
}
//---------------------------------------------------------------------------
void __fastcall TFormTopo::InitDevList()
{
    VECTOR<NE_BASIC_INFO_T*>        vTX;
    VECTOR<NE_BASIC_INFO_T*>        vFX;
    VECTOR<NE_BASIC_INFO_T*>        vDC;
    VECTOR<NE_BASIC_INFO_T*>        vDIS;
    UINT32  i;
    NE_BASIC_INFO_T         *pstInfo;

    for (UINT32 i=0; i<g_mNEBasicInfo.Size(); i++)
    {
        NE_BASIC_INFO_T     *pstInfo = g_mNEBasicInfo.GetValueByIndex(i);

        if (pstInfo->ulDevType == OMC_DEV_TYPE_TX)
        {
            vTX.push_back(pstInfo);
        }
        else if (pstInfo->ulDevType == OMC_DEV_TYPE_FX)
        {
            vFX.push_back(pstInfo);
        }
        else if (pstInfo->ulDevType == OMC_DEV_TYPE_DC)
        {
            vDC.push_back(pstInfo);
        }
        else if (pstInfo->ulDevType == OMC_DEV_TYPE_DIS)
        {
            vDIS.push_back(pstInfo);
        }
    }

    int     x,y;
    int     iLeft = 20;
    int     iTop;
    int     iTXLineNum;
    int     iFXLineNum;
    int     iSkinX = 100;
    int     iSkinY = 110;
    BOOL    bHasAlarm;
    TPointSeries *Series;

    for (i=0; i<chtTopo->SeriesCount(); i++)
    {
        chtTopo->Series[i]->BeginUpdate();
        chtTopo->Series[i]->Clear();
    }

    iTop = 20 + iSkinY;
    for (i=0; i<vDIS.size(); i++)
    {
        pstInfo = vDIS[i];
        bHasAlarm = HasAlarm(pstInfo->acNeID);

        if (pstInfo->iOnlineSec < 0)
        {
            Series = SeriesDISOffline;
        }
        else
        {
            Series = bHasAlarm?SeriesDISAlarm:SeriesDISOnline;
        }

        x = MOD(i, g_iTopoColCount);
        y = i/g_iTopoColCount;

        x = iLeft + x*iSkinX;
        y = iTop + y*iSkinY;

        Series->AddXY(x, y, pstInfo->acDevName);
    }

    if (vDC.size() <= g_iTopoColCount)
    {
        iTop -= iSkinY;
    }

    for (i=0; i<vDC.size(); i++)
    {
        pstInfo = vDC[i];
        bHasAlarm = HasAlarm(pstInfo->acNeID);

        if (pstInfo->iOnlineSec < 0)
        {
            Series = SeriesDCOffline;
        }
        else
        {
            Series = bHasAlarm?SeriesDCAlarm:SeriesDCOnline;
        }

        if (vDC.size() <= g_iTopoColCount)
        {
            x = MOD(i, g_iTopoColCount);
            y = i/g_iTopoColCount;
        }
        else
        {
            x = MOD(i+vDIS.size(), g_iTopoColCount);
            y = (i+vDIS.size())/g_iTopoColCount;
        }

        x = iLeft + x*iSkinX;
        y = iTop + y*iSkinY;

        Series->AddXY(x, y, pstInfo->acDevName);
    }

    iTop = 40 + iSkinY;
    iFXLineNum = 2;
    for (i=0; i<vFX.size(); i++)
    {
        pstInfo = vFX[i];
        bHasAlarm = HasAlarm(pstInfo->acNeID);

        if (pstInfo->iOnlineSec < 0)
        {
            Series = SeriesFXOffline;
        }
        else
        {
            Series = bHasAlarm?SeriesFXAlarm:SeriesFXOnline;
        }

        x = MOD(i, g_iTopoColCount);
        y = iFXLineNum - i/g_iTopoColCount;

        x = iLeft + x*iSkinX;
        y = iTop + y*iSkinY;

        Series->AddXY(x, y, pstInfo->acDevName);
    }

    iTop = 60 + (1+iFXLineNum)*iSkinY;
    iTXLineNum = (vTX.size()+g_iTopoColCount-1)/g_iTopoColCount;
    for (i=0; i<vTX.size(); i++)
    {
        pstInfo = vTX[i];
        bHasAlarm = HasAlarm(pstInfo->acNeID);

        if (pstInfo->iOnlineSec < 0)
        {
            Series = SeriesTXOffline;
        }
        else
        {
            Series = bHasAlarm?SeriesTXAlarm:SeriesTXOnline;
        }

        x = MOD(i, g_iTopoColCount);
        y = iTXLineNum - i/g_iTopoColCount;

        x = iLeft + x*iSkinX;
        y = iTop + y*iSkinY;

        Series->AddXY(x, y, pstInfo->acDevName);
    }

    for (i=0; i<chtTopo->SeriesCount(); i++)
    {
        chtTopo->Series[i]->EndUpdate();
    }

}
//---------------------------------------------------------------------------
void __fastcall TFormTopo::TimerCheckTimer(TObject *Sender)
{
    InitDevList();
}
//---------------------------------------------------------------------------

void __fastcall TFormTopo::SeriesDISOnlineClick(TChartSeries *Sender, int ValueIndex,
          TMouseButton Button, TShiftState Shift, int X, int Y)
{
   // ShowMessage("click DisOnline");
    AnsiString   strNeID = "7427EAA0954B";
    FormNeInfo->Init(strNeID);
    FormNeInfo->Parent = this;
    FormNeInfo->Visible = true;
    double tmpX,tmpY;
    TPointSeries *Series;
//  chtTopo->Series[0]->GetCursorValues(tmpX,tmpY);

    ShowMessage(chtTopo->Series[0]->LegendTitle);

//  ShowMessage(tmpY);
}
//---------------------------------------------------------------------------


void __fastcall TFormTopo::SeriesFXOfflineClick(TChartSeries *Sender, int ValueIndex,
          TMouseButton Button, TShiftState Shift, int X, int Y)
{
    TPointSeries *Series;
    ShowMessage(chtTopo->SeriesCount());
}
//---------------------------------------------------------------------------
void __fastcall TFormTopo::DevInfoClick(TObject *Sender)
{
    AnsiString   strNeID = "7427EAA0954B";
    FormNeInfo->Init(strNeID);
    FormNeInfo->Parent = this;
    FormNeInfo->Visible = true;
}
//---------------------------------------------------------------------------

