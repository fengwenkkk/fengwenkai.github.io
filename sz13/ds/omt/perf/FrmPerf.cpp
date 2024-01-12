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
#include "GTransFile.h"
#include "FrmPerf.h"
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
#pragma link "cxPC"
#pragma link "dxBarBuiltInMenu"
#pragma resource "*.dfm"
TFormPerf *FormPerf;

extern Dict  g_DictDevType;
extern UINT32   g_usTransFilePort;
extern AnsiString GetDevTypeText(UINT32 ulDevType);
extern TColor GetSkinColor(AnsiString strSkinName);
extern void InitChart(TChart *cht);

CHAR* UnzipFile(CHAR *szZipFile)
{
    FILE    *fp = NULL;
    INT32   iFileSize = gos_get_file_size(szZipFile);
    CHAR    *szBuf = NULL;
    CHAR    *szOutBuf = NULL;
    UINT32  ulOutLen;
    UINT32  ulUnzipOutLen;
    BOOL    bRet = FALSE;
    CHAR    acDir[512];
    HANDLE  hZip = NULL;

    if (iFileSize <= 0)
    {
        return NULL;
    }

    szBuf = (CHAR*)calloc(1, iFileSize);
    fp = fopen(szZipFile, "rb");
    if (!fp)
    {
        goto end;
    }

    if (iFileSize != fread(szBuf, 1, iFileSize, fp))
    {
        goto end;
    }

    hZip = gos_comp_init();
    ulOutLen = gos_get_decomp_size(szBuf);

    szOutBuf = (CHAR*)calloc(1, ulOutLen+1);
    if (!szOutBuf)
    {
        goto end;
    }

    gos_decomp(hZip, szBuf, szOutBuf, &ulUnzipOutLen);


    if (ulUnzipOutLen != ulOutLen)
    {
        goto end;
    }

    bRet = TRUE;

end:
    if (fp)
    {
        fclose(fp);
    }

    GOS_FREE(szBuf);
    if (!bRet)
    {
        GOS_FREE(szOutBuf);
        szOutBuf = NULL;
    }

    gos_comp_free(hZip);

    return szOutBuf;
}
__fastcall TFormPerf::TFormPerf(TComponent* Owner)
    : TSubForm(Owner)
{
#ifdef HAVE_TAU
	tsTrafficStatus->TabVisible = true;
#endif
    gbChart->Width = 1000;

    //InitMsgHandler(EV_OMT_GET_TXSTATUS_RSP, OnGetTxStatusRsp);
    //InitMsgHandler(EV_OMT_GET_ALLTXSTATUS_RSP, OnGetAllTxStatusRsp);

    InitMsgHandler(EV_OMT_OMC_RSP, MSG_OMT_GetAllDevStatus, OnGetAllDevStatusRsp);
    InitMsgHandler(EV_OMT_OMC_RSP, MSG_OMT_GetDevStatus, OnGetDevStatusRsp);
    InitMsgHandler(EV_OMT_OMC_RSP, MSG_OMT_GetLteStatus, OnGetLteStatusRsp);
    InitMsgHandler(EV_OMT_OMC_RSP, MSG_OMT_GetTrafficStatus, OnGetTrafficStatusRsp);

    InitChart(chtLte);
    InitChart(chtBasic);
    InitChart(chtTraffic);
    UpdateDevList();

    pcMain->ActivePageIndex = 0;
    cbLastTimePropertiesChange(this);
    ChangePage();

    g_DictDevType.Init(cbDevType, TRUE, TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TFormPerf::OnGetTxStatusRsp(UINT8 *pucMsg, UINT32 ulMsgLen)
{

    OMT_GET_TXSTATUS_RSP_T  *pstRsp = (OMT_GET_TXSTATUS_RSP_T*)pucMsg;

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

    SetChartAxisRange(chtLte->LeftAxis, -110, 40);
	SetChartAxisPrecision(chtLte->LeftAxis, 10);

}
//---------------------------------------------------------------------------
void __fastcall TFormPerf::OnGetAllDevStatusRsp(UINT32 ulSeqID, GJson &Json)
{
    UINT32          ulTime;
    CHAR            acNeID[NE_ID_LEN];
    BOOL            bOnline;
    UINT32          ulCpuUsage;
    UINT32          ulMemUsage;
    UINT32          ulDiskUsage;
    INT32           iClusterStatus;
    UINT32          ulStationID;
    INT32           iPCI;
    INT32           iRSRP;
    INT32           iRSRQ;
    INT32           iRSSI;
    INT32           iSINR;
    CHAR            acFile[256] = {0};
    CHAR            acLocalFile[256] = {0};
    NE_BASIC_INFO_T *pstNeInfo = NULL;
    VectorMap<AnsiString, NE_BASIC_INFO_T>  mNEBasicInfo;
    NEBasic::GetNEBasicInfo(mNEBasicInfo);

    if (!Json.GetValue("File", acFile, sizeof(acFile)))
    {
        GosLog(LOG_ERROR, "invalid msg format: %s", Json.GetText());
        return;
    }

    // 设置客户端本地保存文件目录
    sprintf(acLocalFile, "%s/%s", gos_get_root_path(), acFile);

	if (!TransFile(g_stLocalCfg.aucOMCAddr, g_usTransFilePort, acFile, acLocalFile))
	{
        GosLog(LOG_ERROR, "DownLoad file %s fail", acFile);
        return;
    }

	// 解析Json文件
	GJson      JsonResult;

	if (!JsonResult.ParseFile(acLocalFile))
	{
		gos_delete_file(acLocalFile);
		GosLog(LOG_ERROR, "ParseFile %s fail", acLocalFile);
		return;
	}
	gos_delete_file(acLocalFile);

	VECTOR<GJson*>  &vSubJson = JsonResult.GetSubJson();

    // 每设置一次 TreeList 格式，都会重绘一次界面。
    // 调用BeginUpdate方法, 所有设置完成之后，重绘一次，减少中间的重绘过程。
    tlMain->BeginUpdate();
    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        GJson   *pSubJson = vSubJson[i];

        iClusterStatus = -1;
        iPCI = -1;

        if (!pSubJson->GetValue("Time", &ulTime) ||
            !pSubJson->GetValue("NeID", acNeID, sizeof(acNeID)) ||
            !pSubJson->GetValue("Online", &bOnline) ||
            !pSubJson->GetValue("Cpu", &ulCpuUsage) ||
            !pSubJson->GetValue("Mem", &ulMemUsage) ||
            !pSubJson->GetValue("Disk", &ulDiskUsage) )
        {
            GosLog(LOG_ERROR, "invalid msg in basic %s", pSubJson->GetText());
            continue;
        }

        pstNeInfo = mNEBasicInfo.GetValue(acNeID);
        if (!pstNeInfo)
        {
            GosLog(LOG_ERROR, "unknown NeID in %s", pSubJson->GetText());
            continue;
        }

        if (pstNeInfo->ulDevType == OMC_DEV_TYPE_DIS)
        {
            if (!pSubJson->GetValue("ClusterStatus", &iClusterStatus) )
            {
                GosLog(LOG_ERROR, "invalid msg in ClusterStatus %s", pSubJson->GetText());
                continue;
            }
        }
        else if (pstNeInfo->ulDevType == OMC_DEV_TYPE_TX ||
                 pstNeInfo->ulDevType == OMC_DEV_TYPE_FX )
        {
            if (!pSubJson->GetValue("Station", &ulStationID) ||
                !pSubJson->GetValue("PCI", &iPCI) ||
                !pSubJson->GetValue("RSRP", &iRSRP) ||
                !pSubJson->GetValue("RSRQ", &iRSRQ) ||
                !pSubJson->GetValue("RSSI", &iRSSI) ||
                !pSubJson->GetValue("SINR", &iSINR) )
            {
                GosLog(LOG_ERROR, "invalid msg in LTE %s", pSubJson->GetText());
                continue;
            }
        }
        else if (pstNeInfo->ulDevType == OMC_DEV_TYPE_TAU)
         {
            if (!pSubJson->GetValue("PCI", &iPCI)   ||
                !pSubJson->GetValue("RSRP", &iRSRP) ||
                !pSubJson->GetValue("RSRQ", &iRSRQ) ||
                !pSubJson->GetValue("RSSI", &iRSSI) ||
                !pSubJson->GetValue("SINR", &iSINR) )
            {
                GosLog(LOG_ERROR, "invalid msg in TAU LTE %s", pSubJson->GetText());
                continue;
            }
         }

        TcxTreeListNode *Node = tlMain->Add();

        Node->Values[0] = ulTime;
        Node->Values[1] = acNeID;
        Node->Values[2] = pstNeInfo->ulDevType;
        Node->Values[3] = bOnline;
        Node->Values[4] = ulCpuUsage;
        Node->Values[5] = ulMemUsage;;
        Node->Values[6] = ulDiskUsage;
        Node->Values[7] = iClusterStatus;
        Node->Values[8] = iPCI;
        Node->Values[9] = iRSRP;
        Node->Values[10] = iRSRQ;
        Node->Values[11] = iRSSI;
        Node->Values[12] = iSINR;
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormPerf::OnGetDevStatusRsp(UINT32 ulSeqID, GJson &Json)
{
    TcxTreeListNode *Node;
    VECTOR<GJson*>  vSubJson;
    GJson           JsonResult;
    CHAR            acFile[256];
    CHAR            acLocalFile[256];
    UINT32          ulTime;
    CHAR            acNeID[NE_ID_LEN];
    BOOL            bOnline;
    UINT32          ulCpuUsage;
    UINT32          ulMemUsage;
    UINT32          ulDiskUsage;
    char            acTime[32];
    NE_BASIC_INFO_T *pstNeInfo;
    VectorMap<AnsiString, NE_BASIC_INFO_T>  mNEBasicInfo;
    NEBasic::GetNEBasicInfo(mNEBasicInfo);
    AnsiString      strNeID = GetNeIDByDevName(cbDevName->Text.Trim());

	vSubJson.clear();
	if (Json.GetValue("File", acFile, sizeof(acFile)))
    {
        // 设置客户端本地保存文件目录
		sprintf(acLocalFile, "%s/%s", gos_get_root_path(), acFile);
		// 从服务器下载文件
		if (!TransFile(g_stLocalCfg.aucOMCAddr, g_usTransFilePort, acFile, acLocalFile))
		{
			GosLog(LOG_ERROR, "DownLoad file %s fail", acFile);
			return;
		}
		// 解压压缩文件
		CHAR *szBuf = UnzipFile(acLocalFile);
        if (!szBuf)
		{
			gos_delete_file(acLocalFile);
            GosLog(LOG_ERROR, "UnZipFile file %s fail", acLocalFile);
            return;
        }
        // 解析Json文件
        if (!JsonResult.ParseArray(szBuf))
		{
			gos_delete_file(acLocalFile);
            GOS_FREE(szBuf);
            GosLog(LOG_ERROR, "ParseFile %s fail", acFile);
            return;
		}
		gos_delete_file(acLocalFile);
		GOS_FREE(szBuf);

        vSubJson = JsonResult.GetSubJson();
    }
    else
    {
        vSubJson = Json.GetSubJson();
    }

    tlMain->BeginUpdate();
	tlMain->Clear();
    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        GJson   *pSubJson = vSubJson[i];
        if (!pSubJson->GetValue("NeID", acNeID, sizeof(acNeID)))
        {
            GosLog(LOG_ERROR, "invalid ne_id in %s", pSubJson->GetText());
            continue;
        }

        if (strcmp(acNeID, strNeID.c_str()) != 0)
        {
            continue;
        }

        if (!pSubJson->GetValue("Time", &ulTime) ||
            !pSubJson->GetValue("Online", &bOnline) ||
            !pSubJson->GetValue("Cpu", &ulCpuUsage) ||
            !pSubJson->GetValue("Mem", &ulMemUsage) ||
            !pSubJson->GetValue("Disk", &ulDiskUsage))
        {
            GosLog(LOG_ERROR, "invalid msg in %s", pSubJson->GetText());
            continue;
        }

        pstNeInfo = mNEBasicInfo.GetValue(acNeID);
        if (!pstNeInfo)
        {
            GosLog(LOG_ERROR, "unknown ne_id in %s", pSubJson->GetText());
            continue;
        }

        Node = tlMain->Add();

        Node->Values[0] = ulTime;
        Node->Values[1] = acNeID;
        Node->Values[2] = pstNeInfo->ulDevType;
        Node->Values[3] = bOnline;
        Node->Values[4] = ulCpuUsage;
        Node->Values[5] = ulMemUsage;;
        Node->Values[6] = ulDiskUsage;

        gos_get_text_time_ex(&ulTime, NULL, acTime);

        if (i == 0)
        {
            SeriesOnline->AddNullXY(ulTime, 100*bOnline, acTime+11);
            SeriesCpu->AddNullXY(ulTime, ulCpuUsage, acTime+11);
            SeriesMem->AddNullXY(ulTime, ulMemUsage, acTime+11);
            SeriesDisk->AddNullXY(ulTime, ulDiskUsage, acTime+11);
        }
        else
        {
            SeriesOnline->AddXY(ulTime, 100*bOnline, acTime+11);
            SeriesCpu->AddXY(ulTime, ulCpuUsage, acTime+11);
            SeriesMem->AddXY(ulTime, ulMemUsage, acTime+11);
            SeriesDisk->AddXY(ulTime, ulDiskUsage, acTime+11);
        }
    }

    tlMain->EndUpdate();

    SetChartAxisRange(chtBasic->LeftAxis, 0, 100);
}
//---------------------------------------------------------------------------
void __fastcall TFormPerf::OnGetLteStatusRsp(UINT32 ulSeqID, GJson &Json)
{
    TcxTreeListNode *Node;
    VECTOR<GJson*>  vSubJson;
    GJson           JsonResult;
    UINT32          ulTime;
    CHAR            acNeID[NE_ID_LEN];
    BOOL            bOnline;
    UINT32          ulCpuUsage;
    UINT32          ulMemUsage;
    UINT32          ulDiskUsage;
    INT32           iClusterStatus;
    INT32           iPCI;
    INT32           iRSRP;
    INT32           iRSRQ;
    INT32           iRSSI;
    INT32           iSINR;
    char            acTime[32];
    CHAR            acFile[256];
    CHAR            acLocalFile[256];
    AnsiString      strNeID = GetNeIDByDevName(cbDevName->Text.Trim());
    NE_BASIC_INFO_T *pstNeInfo;
    VectorMap<AnsiString, NE_BASIC_INFO_T>  mNEBasicInfo;

    NEBasic::GetNEBasicInfo(mNEBasicInfo);
    tlMain->BeginUpdate();
    tlMain->Clear();

    SeriesRSRP->Clear();
    SeriesRSRQ->Clear();
    SeriesRSSI->Clear();
    SeriesSINR->Clear();

    SeriesOnline->Clear();
    SeriesCpu->Clear();
    SeriesMem->Clear();
    SeriesDisk->Clear();

    vSubJson.clear();
    if (Json.GetValue("File", acFile, sizeof(acFile)))
    {
        // 设置客户端本地保存文件目录
        sprintf(acLocalFile, "%s/%s", gos_get_root_path(), acFile);

		if (!TransFile(g_stLocalCfg.aucOMCAddr, g_usTransFilePort, acFile, acLocalFile))
        {
            GosLog(LOG_ERROR, "DownLoad file %s fail", acFile);
            return;
        }
		// 解压压缩文件
        CHAR *szBuf = UnzipFile(acLocalFile);
        if (!szBuf)
		{
			gos_delete_file(acLocalFile);
            GosLog(LOG_ERROR, "UnZipFile file %s fail", acLocalFile);
            return;
        }
        // 解析Json文件
        if (!JsonResult.ParseArray(szBuf))
		{
			gos_delete_file(acLocalFile);
            GOS_FREE(szBuf);
            GosLog(LOG_ERROR, "ParseFile %s fail", acLocalFile);
            return;
        }
		gos_delete_file(acLocalFile);
		GOS_FREE(szBuf);

        vSubJson = JsonResult.GetSubJson();
    }
    else
    {
        vSubJson = Json.GetSubJson();
    }

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        GJson   *pSubJson = vSubJson[i];

        iClusterStatus = -1;
        iPCI = -1;

        if (!pSubJson->GetValue("NeID", acNeID, sizeof(acNeID)))
        {
            GosLog(LOG_ERROR, "invalid NeID in %s", pSubJson->GetText());
            continue;
        }

        if (strcmp(acNeID, strNeID.c_str()) != 0)
        {
            continue;
        }

        if (!pSubJson->GetValue("Time", &ulTime) ||
            !pSubJson->GetValue("Online", &bOnline) ||
            !pSubJson->GetValue("Cpu", &ulCpuUsage) ||
            !pSubJson->GetValue("Mem", &ulMemUsage) ||
            !pSubJson->GetValue("Disk", &ulDiskUsage) ||
            !pSubJson->GetValue("PCI", &iPCI) ||
            !pSubJson->GetValue("RSRP", &iRSRP) ||
            !pSubJson->GetValue("RSRQ", &iRSRQ) ||
            !pSubJson->GetValue("RSSI", &iRSSI) ||
            !pSubJson->GetValue("SINR", &iSINR) )
        {
            GosLog(LOG_ERROR, "invalid msg in %s", pSubJson->GetText());
            continue;
        }

        pstNeInfo = mNEBasicInfo.GetValue(acNeID);
        if (!pstNeInfo)
        {
            GosLog(LOG_ERROR, "unknown NeID in %s", pSubJson->GetText());
            continue;
        }

        Node = tlMain->Add();

        Node->Values[0] = ulTime;
        Node->Values[1] = acNeID;
        Node->Values[2] = pstNeInfo->ulDevType;
        Node->Values[3] = bOnline;
        Node->Values[4] = ulCpuUsage;
        Node->Values[5] = ulMemUsage;;
        Node->Values[6] = ulDiskUsage;
        Node->Values[7] = iClusterStatus;
        Node->Values[8] = iPCI;
        Node->Values[9] = iRSRP;
        Node->Values[10] = iRSRQ;
        Node->Values[11] = iRSSI;
        Node->Values[12] = iSINR;

        gos_get_text_time_ex(&ulTime, NULL, acTime);

        if (i == 0)
        {
            SeriesRSRP->AddNullXY(ulTime, iRSRP, acTime+11);
            SeriesRSRQ->AddNullXY(ulTime, iRSRQ, acTime+11);
            SeriesRSSI->AddNullXY(ulTime, iRSSI, acTime+11);
            SeriesSINR->AddNullXY(ulTime, iSINR, acTime+11);

            SeriesOnline->AddNullXY(ulTime, 100*bOnline, acTime+11);
            SeriesCpu->AddNullXY(ulTime, ulCpuUsage, acTime+11);
            SeriesMem->AddNullXY(ulTime, ulMemUsage, acTime+11);
            SeriesDisk->AddNullXY(ulTime, ulDiskUsage, acTime+11);
        }
        else
        {
            SeriesRSRP->AddXY(ulTime, iRSRP, acTime+11);
            SeriesRSRQ->AddXY(ulTime, iRSRQ, acTime+11);
            SeriesRSSI->AddXY(ulTime, iRSSI, acTime+11);
            SeriesSINR->AddXY(ulTime, iSINR, acTime+11);

            SeriesOnline->AddXY(ulTime, 100*bOnline, acTime+11);
            SeriesCpu->AddXY(ulTime, ulCpuUsage, acTime+11);
            SeriesMem->AddXY(ulTime, ulMemUsage, acTime+11);
            SeriesDisk->AddXY(ulTime, ulDiskUsage, acTime+11);
        }
    }

    tlMain->EndUpdate();

    SetChartAxisRange(chtLte->LeftAxis, -110, 40);
    SetChartAxisRange(chtBasic->LeftAxis, 0, 100);
}
//---------------------------------------------------------------------------
void __fastcall TFormPerf::InitTrafficStatusView(VECTOR<TRAFFIC_RATE_STATISTICS_T> &vRate, CHAR *szNeID, UINT32 ulDevType)
{
    TcxTreeListNode *Node;
    CHAR    acTime[32] = {0};

    tlMain->BeginUpdate();
    for (UINT32 i=0; i<vRate.size(); ++i)
    {
        Node = tlMain->Add();
        Node->Values[0] = vRate[i].ulTime;
        Node->Values[1] = szNeID;
        Node->Values[2] = ulDevType;
        Node->Values[16] = vRate[i].ulWanTxRate;
        Node->Values[17] = vRate[i].ulWanRxRate;
        Node->Values[18] = vRate[i].ulLan1TxRate;
        Node->Values[19] = vRate[i].ulLan1RxRate;
        Node->Values[20] = vRate[i].ulLan2TxRate;
        Node->Values[21] = vRate[i].ulLan2RxRate;
        Node->Values[22] = vRate[i].ulLan3TxRate;
        Node->Values[23] = vRate[i].ulLan3RxRate;
        Node->Values[24] = vRate[i].ulLan4TxRate;
        Node->Values[25] = vRate[i].ulLan4RxRate;

        gos_get_text_time_ex(&vRate[i].ulTime, NULL, acTime);
        if (i == 0)
        {
            SeriesWanTx->AddNullXY(vRate[i].ulTime,  vRate[i].ulWanTxRate,  acTime+11);
            SeriesWanRx->AddNullXY(vRate[i].ulTime,  vRate[i].ulWanRxRate,  acTime+11);
            SeriesLan1Tx->AddNullXY(vRate[i].ulTime, vRate[i].ulLan1TxRate, acTime+11);
            SeriesLan1Rx->AddNullXY(vRate[i].ulTime, vRate[i].ulLan1RxRate, acTime+11);
            SeriesLan2Tx->AddNullXY(vRate[i].ulTime, vRate[i].ulLan2TxRate, acTime+11);
            SeriesLan2Rx->AddNullXY(vRate[i].ulTime, vRate[i].ulLan2RxRate, acTime+11);
            SeriesLan3Tx->AddNullXY(vRate[i].ulTime, vRate[i].ulLan3TxRate, acTime+11);
            SeriesLan3Rx->AddNullXY(vRate[i].ulTime, vRate[i].ulLan3RxRate, acTime+11);
            SeriesLan4Tx->AddNullXY(vRate[i].ulTime, vRate[i].ulLan4TxRate, acTime+11);
            SeriesLan4Rx->AddNullXY(vRate[i].ulTime, vRate[i].ulLan4RxRate, acTime+11);
        }
        else
        {
            SeriesWanTx->AddXY(vRate[i].ulTime,  vRate[i].ulWanTxRate,  acTime+11);
            SeriesWanRx->AddXY(vRate[i].ulTime,  vRate[i].ulWanRxRate,  acTime+11);
            SeriesLan1Tx->AddXY(vRate[i].ulTime, vRate[i].ulLan1TxRate, acTime+11);
            SeriesLan1Rx->AddXY(vRate[i].ulTime, vRate[i].ulLan1RxRate, acTime+11);
            SeriesLan2Tx->AddXY(vRate[i].ulTime, vRate[i].ulLan2TxRate, acTime+11);
            SeriesLan2Rx->AddXY(vRate[i].ulTime, vRate[i].ulLan2RxRate, acTime+11);
            SeriesLan3Tx->AddXY(vRate[i].ulTime, vRate[i].ulLan3TxRate, acTime+11);
            SeriesLan3Rx->AddXY(vRate[i].ulTime, vRate[i].ulLan3RxRate, acTime+11);
            SeriesLan4Tx->AddXY(vRate[i].ulTime, vRate[i].ulLan4TxRate, acTime+11);
            SeriesLan4Rx->AddXY(vRate[i].ulTime, vRate[i].ulLan4RxRate, acTime+11);
        }
    }

    tlMain->EndUpdate();
    SetChartAxisRange(chtTraffic->LeftAxis, 0, 200);
}
//---------------------------------------------------------------------------
void __fastcall TFormPerf::OnGetTrafficStatusRsp(UINT32 ulSeqID, GJson &Json)
{
    VECTOR<GJson*>  vSubJson;
    GJson           JsonResult;
    CHAR            acNeID[NE_ID_LEN];
    UINT32          ulTime;
    UINT64          u64WanTx;
    UINT64          u64WanRx;
    UINT64          u64Lan1Tx;
    UINT64          u64Lan1Rx;
    UINT64          u64Lan2Tx;
    UINT64          u64Lan2Rx;
    UINT64          u64Lan3Tx;
    UINT64          u64Lan3Rx;
    UINT64          u64Lan4Tx;
    UINT64          u64Lan4Rx;

    UINT32          ulPreTime;
    UINT64          u64PreWanTx;
    UINT64          u64PreWanRx;
    UINT64          u64PreLan1Tx;
    UINT64          u64PreLan1Rx;
    UINT64          u64PreLan2Tx;
    UINT64          u64PreLan2Rx;
    UINT64          u64PreLan3Tx;
    UINT64          u64PreLan3Rx;
    UINT64          u64PreLan4Tx;
    UINT64          u64PreLan4Rx;

    CHAR            acFile[256];
    CHAR            acLocalFile[256];
    BOOL            bIsFirst = TRUE;
    VECTOR<TRAFFIC_RATE_STATISTICS_T> vRate;
    AnsiString      strNeID = GetNeIDByDevName(cbDevName->Text.Trim());
    NE_BASIC_INFO_T *pstNeInfo;
    VectorMap<AnsiString, NE_BASIC_INFO_T>  mNEBasicInfo;

    NEBasic::GetNEBasicInfo(mNEBasicInfo);

    vSubJson.clear();
    if (Json.GetValue("File", acFile, sizeof(acFile)))
    {
        // 设置客户端本地保存文件目录
        sprintf(acLocalFile, "%s/%s", gos_get_root_path(), acFile);

		if (!TransFile(g_stLocalCfg.aucOMCAddr, g_usTransFilePort, acFile, acLocalFile))
        {
            GosLog(LOG_ERROR, "DownLoad file %s fail", acFile);
            return;
        }
		// 解压压缩文件
        CHAR *szBuf = UnzipFile(acLocalFile);
        if (!szBuf)
		{
			gos_delete_file(acLocalFile);
			GosLog(LOG_ERROR, "UnZipFile file %s fail", acLocalFile);
            return;
        }
        // 解析Json文件
        if (!JsonResult.ParseArray(szBuf))
		{
			gos_delete_file(acLocalFile);
			GOS_FREE(szBuf);
			GosLog(LOG_ERROR, "ParseFile %s fail", acFile);
			return;
		}
		gos_delete_file(acLocalFile);
		GOS_FREE(szBuf);

        vSubJson = JsonResult.GetSubJson();
    }
    else
    {
        vSubJson = Json.GetSubJson();
    }

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        UINT32  ulInterval = 0;
        TRAFFIC_RATE_STATISTICS_T stRate = {0};
        GJson   *pSubJson = vSubJson[i];

        if (!pSubJson->GetValue("NeID", acNeID, sizeof(acNeID)))
        {
            GosLog(LOG_ERROR, "invalid NeID in %s", pSubJson->GetText());
            continue;
        }

        if (strcmp(acNeID, strNeID.c_str()) != 0)
        {
            continue;
        }

        if (!pSubJson->GetValue("Time", &ulTime) ||
            !pSubJson->GetValue("WanTx", &u64WanTx)   ||
            !pSubJson->GetValue("WanRx", &u64WanRx)   ||
            !pSubJson->GetValue("Lan1Tx", &u64Lan1Tx) ||
            !pSubJson->GetValue("Lan1Rx", &u64Lan1Rx) ||
            !pSubJson->GetValue("Lan2Tx", &u64Lan2Tx) ||
            !pSubJson->GetValue("Lan2Rx", &u64Lan2Rx) ||
            !pSubJson->GetValue("Lan3Tx", &u64Lan3Tx) ||
            !pSubJson->GetValue("Lan3Rx", &u64Lan3Rx) ||
            !pSubJson->GetValue("Lan4Tx", &u64Lan4Tx) ||
            !pSubJson->GetValue("Lan4Rx", &u64Lan4Rx))
        {
            GosLog(LOG_ERROR, "invalid msg in %s", pSubJson->GetText());
            continue;
        }
        //较上一次的时间间隔
        ulInterval = ulTime - ulPreTime;
        if (!bIsFirst && ulInterval > 0)
        {
            // >>10 相当于除1024，速率单位有B/s,转换成KB/s
            stRate.ulTime = ulTime;
            stRate.ulWanTxRate  = (u64WanTx  - u64PreWanTx ) > 0 ? ((u64WanTx  - u64PreWanTx )>>10)/ulInterval : 0;
            stRate.ulWanRxRate  = (u64WanRx  - u64PreWanRx ) > 0 ? ((u64WanRx  - u64PreWanRx )>>10)/ulInterval : 0;
            stRate.ulLan1TxRate = (u64Lan1Tx - u64PreLan1Tx) > 0 ? ((u64Lan1Tx - u64PreLan1Tx)>>10)/ulInterval : 0;
            stRate.ulLan1RxRate = (u64Lan1Rx - u64PreLan1Rx) > 0 ? ((u64Lan1Rx - u64PreLan1Rx)>>10)/ulInterval : 0;
            stRate.ulLan2TxRate = (u64Lan2Tx - u64PreLan2Tx) > 0 ? ((u64Lan2Tx - u64PreLan2Tx)>>10)/ulInterval : 0;
            stRate.ulLan2RxRate = (u64Lan2Rx - u64PreLan2Rx) > 0 ? ((u64Lan2Rx - u64PreLan2Rx)>>10)/ulInterval : 0;
            stRate.ulLan3TxRate = (u64Lan3Tx - u64PreLan3Tx) > 0 ? ((u64Lan3Tx - u64PreLan3Tx)>>10)/ulInterval : 0;
            stRate.ulLan3RxRate = (u64Lan3Rx - u64PreLan3Rx) > 0 ? ((u64Lan3Rx - u64PreLan3Rx)>>10)/ulInterval : 0;
            stRate.ulLan4TxRate = (u64Lan4Tx - u64PreLan4Tx) > 0 ? ((u64Lan4Tx - u64PreLan4Tx)>>10)/ulInterval : 0;
            stRate.ulLan4RxRate = (u64Lan4Rx - u64PreLan4Rx) > 0 ? ((u64Lan4Rx - u64PreLan4Rx)>>10)/ulInterval : 0;
            vRate.push_back(stRate);
        }
        else
        {
            bIsFirst = FALSE;
            // 获取设备的基础信息
            pstNeInfo = mNEBasicInfo.GetValue(acNeID);
            if (!pstNeInfo)
            {
                GosLog(LOG_ERROR, "unknown NeID in %s", pSubJson->GetText());
                break;
            }
        }
        // 保留上一次的数据
        ulPreTime = ulTime;
        u64PreWanTx = u64WanTx;
        u64PreWanRx = u64WanRx;
        u64PreLan1Tx = u64Lan1Tx;
        u64PreLan1Rx = u64Lan1Rx;
        u64PreLan2Tx = u64Lan2Tx;
        u64PreLan2Rx = u64Lan2Rx;
        u64PreLan3Tx = u64Lan3Tx;
        u64PreLan3Rx = u64Lan3Rx;
        u64PreLan4Tx = u64Lan4Tx;
        u64PreLan4Rx = u64Lan4Rx;
    }

    InitTrafficStatusView(vRate, pstNeInfo->acNeID, pstNeInfo->ulDevType);
}
//---------------------------------------------------------------------------
void __fastcall TFormPerf::OnGetAllTxStatusRsp(UINT8 *pucMsg, UINT32 ulMsgLen)
{
    OMT_GET_ALLTXSTATUS_RSP_T  *pstRsp = (OMT_GET_ALLTXSTATUS_RSP_T*)pucMsg;

    if (ulMsgLen < STRUCT_OFFSET(OMT_GET_ALLTXSTATUS_RSP_T, astRec))
    {
        GosLog(LOG_ERROR, "invalid msg len %u of OMT_GET_ALLTXSTATUS_RSP", ulMsgLen);
        return;
    }

    NTOHL_IT(pstRsp->ulNum);

    if (ulMsgLen != (STRUCT_OFFSET(OMT_GET_ALLTXSTATUS_RSP_T, astRec) + pstRsp->ulNum*sizeof(pstRsp->astRec[0])))
    {
        GosLog(LOG_ERROR, "invalid msg len %u of OMT_GET_ALLTXSTATUS_RSP", ulMsgLen);
        return;
    }

    TcxTreeListNode     *Node;

    tlMain->BeginUpdate();
    tlMain->Clear();

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
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormPerf::btnRefreshClick(TObject *Sender)
{
	GJsonParam  Param;
    UINT32  ulTime = GetTime(deStartTime->Date);
    // 清理TreeList和Series
    tlMain->Clear();
    SeriesOnline->Clear();
    SeriesCpu->Clear();
    SeriesMem->Clear();
    SeriesDisk->Clear();
    SeriesRSRP->Clear();
    SeriesRSRQ->Clear();
    SeriesRSSI->Clear();
    SeriesSINR->Clear();
    SeriesWanTx->Clear();
    SeriesWanRx->Clear();
    SeriesLan1Tx->Clear();
    SeriesLan1Rx->Clear();
    SeriesLan2Tx->Clear();
    SeriesLan2Rx->Clear();
    SeriesLan3Tx->Clear();
    SeriesLan3Rx->Clear();
    SeriesLan4Tx->Clear();
    SeriesLan4Rx->Clear();

    // 选择实时数据
	if (cbLastTime->Checked)
    {
        ulTime = 0xffffffff;
    }

    if (pcMain->ActivePage == tsAll)
    {
        Param.Add("Time", ulTime);
        SendServer(EV_OMT_OMC_REQ, MSG_OMT_GetAllDevStatus, Param);
    }
    else if (pcMain->ActivePage == tsBasicStatus)
    {
        if (cbDevName->Text.IsEmpty())
        {
            InfoMsg("请选择设备");
            return;
        }

        AnsiString strNeID = GetNeIDByDevName(cbDevName->Text.Trim());
        if (strNeID == "")
        {
            InfoMsg("未匹配到设备");
            return;
        }

        Param.Add("NeID", strNeID);
        Param.Add("Time", ulTime);
        SendServer(EV_OMT_OMC_REQ, MSG_OMT_GetDevStatus, Param);
    }
    else if(pcMain->ActivePage == tsLteStatus)
    {
        if (cbDevName->Text.IsEmpty())
        {
            InfoMsg("请选择设备");
            return;
        }

        AnsiString strNeID = GetNeIDByDevName(cbDevName->Text.Trim());
        if (strNeID == "")
        {
            InfoMsg("未匹配到设备");
            return;
        }

        Param.Add("NeID", strNeID);
        Param.Add("Time", ulTime);
        SendServer(EV_OMT_OMC_REQ, MSG_OMT_GetLteStatus, Param);
    }
    else if (pcMain->ActivePage == tsTrafficStatus)
    {
        if (cbDevName->Text.IsEmpty())
        {
            InfoMsg("请选择设备");
            return;
        }

        AnsiString strNeID = GetNeIDByDevName(cbDevName->Text.Trim());
        if (strNeID == "")
        {
            InfoMsg("未匹配到设备");
            return;
        }

        Param.Add("NeID", strNeID);
        Param.Add("Time", ulTime);
        SendServer(EV_OMT_OMC_REQ, MSG_OMT_GetTrafficStatus, Param);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormPerf::FormCreate(TObject *Sender)
{
    SetImage(btnRefresh, IMG_REFRESH);
}
//---------------------------------------------------------------------------
void __fastcall TFormPerf::ColumnNeIDGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    if (ANode->Values[1].IsNull())
    {
        return;
    }

    AnsiString  strNeID = ANode->Values[1];

    Value = GetDevNameByNeID(strNeID);
}
//---------------------------------------------------------------------------
void __fastcall TFormPerf::ColumnTimeGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    if (ANode->Values[0].IsNull())
    {
        return;
    }

	UINT32  ulTime = ANode->Values[0];

	Value = GetTextTime(ulTime);
}
//---------------------------------------------------------------------------
void __fastcall TFormPerf::UpdateDevList()
{
    AnsiString  strDevName = cbDevName->Text;

    cbDevTypePropertiesChange(this);

    cbDevName->ItemIndex = cbDevName->Properties->Items->IndexOf(strDevName);
}
//---------------------------------------------------------------------------
void __fastcall TFormPerf::cbDevTypePropertiesChange(TObject *Sender)
{
    UINT32  ulDevType;
    VectorMap<AnsiString, NE_BASIC_INFO_T>  mNEBasicInfo;
    NEBasic::GetNEBasicInfo(mNEBasicInfo);

    cbDevName->Properties->Items->Clear();

    if (!g_DictDevType.GetID(cbDevType, &ulDevType))
    {
        return;
    }

    cbDevName->Properties->Items->BeginUpdate();

    for (UINT32 i=0; i<mNEBasicInfo.Size(); i++)
    {
        NE_BASIC_INFO_T *pstInfo = mNEBasicInfo.GetValueByIndex(i);

        if (pstInfo->ulDevType == ulDevType
            && *(pstInfo->acDevName) != '\0')
        {
            cbDevName->Properties->Items->Add(pstInfo->acDevName);
        }

    }

    cbDevName->ItemIndex = -1;

    cbDevName->Properties->Items->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormPerf::ChangePage()
{
	TcxTabSheet *ActivePage = pcMain->ActivePage;

	if (ActivePage == NULL)
	{
        return;
	}

  //	gbMain->Parent = ActivePage;

	tlMain->Visible = true;
	tlMain->BeginUpdate();
    tlMain->Clear();

    if (ActivePage == tsAll)
    {
        gbChart->Visible = false;
		Splitter->Visible = false;

        lbDevType->Visible = false;
        cbDevType->Visible = false;
        lbDevName->Visible = false;
        cbDevName->Visible = false;

        ColumnOnline->Visible = true;
        // 主备状态，目前只有CAD服务器有此状态
        //ColumnClusterStatus->Visible = true;
        ColumnCpuUsage->Visible = true;
        ColumnMemUsage->Visible = true;
        ColumnDiskUsage->Visible = true;

        ColumnPCI->Visible = true;
        ColumnRSRP->Visible = true;
        ColumnRSRQ->Visible = true;
        ColumnRSSI->Visible = true;
        ColumnSINR->Visible = true;

        ColumnWANTx->Visible =  false;
        ColumnWANRx->Visible =  false;
        ColumnLAN1Tx->Visible = false;
        ColumnLAN1Rx->Visible = false;
        ColumnLAN2Tx->Visible = false;
        ColumnLAN2Rx->Visible = false;
        ColumnLAN3Tx->Visible = false;
        ColumnLAN3Rx->Visible = false;
        ColumnLAN4Tx->Visible = false;
        ColumnLAN4Rx->Visible = false;
    }
    else if (ActivePage == tsBasicStatus)   // 之前获取设备基础状态信息和LTE信息是在一个消息完成的
    {
		g_DictDevType.Init(cbDevType, TRUE, TRUE);
        gbChart->Visible = true;
        Splitter->Visible = true;
        chtBasic->BringToFront();

        lbDevType->Visible = true;
        cbDevType->Visible = true;
        cbDevType->Enabled = true;
        cbDevType->ItemIndex = 1;
        lbDevName->Visible = true;
        cbDevName->Visible = true;

        ColumnCpuUsage->Visible = true;
        ColumnMemUsage->Visible = true;
        ColumnDiskUsage->Visible = true;

		ColumnOnline->Visible = false;
        ColumnPCI->Visible = false;
        ColumnRSRP->Visible = false;
        ColumnRSRQ->Visible = false;
        ColumnRSSI->Visible = false;
        ColumnSINR->Visible = false;

        ColumnWANTx->Visible =  false;
        ColumnWANRx->Visible =  false;
        ColumnLAN1Tx->Visible = false;
        ColumnLAN1Rx->Visible = false;
        ColumnLAN2Tx->Visible = false;
        ColumnLAN2Rx->Visible = false;
        ColumnLAN3Tx->Visible = false;
        ColumnLAN3Rx->Visible = false;
        ColumnLAN4Tx->Visible = false;
        ColumnLAN4Rx->Visible = false;
    }
    else if (ActivePage == tsLteStatus)
    {
        cbDevType->Properties->Items->Clear();
        cbDevType->Properties->Items->Add("TAU");
        cbDevType->Properties->Items->Add("车载台");
        cbDevType->Properties->Items->Add("固定台");
        gbChart->Visible = true;
        gbChart->Align =  alRight;
        Splitter->Visible = true;
        chtLte->BringToFront();
        tlMain->Visible = true;

        lbDevType->Visible = true;
        cbDevType->Visible = true;
        cbDevType->Enabled = true;
        cbDevType->ItemIndex = 1;
        lbDevName->Visible = true;
        cbDevName->Visible = true;

		ColumnOnline->Visible = false;
        ColumnCpuUsage->Visible = false;
        ColumnMemUsage->Visible = false;
        ColumnDiskUsage->Visible = false;

        ColumnWANTx->Visible =  false;
        ColumnWANRx->Visible =  false;
        ColumnLAN1Tx->Visible = false;
        ColumnLAN1Rx->Visible = false;
        ColumnLAN2Tx->Visible = false;
        ColumnLAN2Rx->Visible = false;
        ColumnLAN3Tx->Visible = false;
        ColumnLAN3Rx->Visible = false;
        ColumnLAN4Tx->Visible = false;
        ColumnLAN4Rx->Visible = false;

        ColumnPCI->Visible = true;
        ColumnRSRP->Visible = true;
        ColumnRSRQ->Visible = true;
        ColumnRSSI->Visible = true;
        ColumnSINR->Visible = true;
    }
    else if(ActivePage == tsTrafficStatus)
	{
		ColumnOnline->Visible = false;
        ColumnCpuUsage->Visible = false;
        ColumnMemUsage->Visible = false;
        ColumnDiskUsage->Visible = false;
        ColumnPCI->Visible = false;
        ColumnRSRP->Visible = false;
        ColumnRSRQ->Visible = false;
        ColumnRSSI->Visible = false;
        ColumnSINR->Visible = false;

        ColumnWANTx->Visible = true;
        ColumnWANRx->Visible = true;
        ColumnLAN1Tx->Visible = true;
        ColumnLAN1Rx->Visible = true;
        ColumnLAN2Tx->Visible = true;
        ColumnLAN2Rx->Visible = true;
        ColumnLAN3Tx->Visible = true;
        ColumnLAN3Rx->Visible = true;
        ColumnLAN4Tx->Visible = true;
        ColumnLAN4Rx->Visible = true;

        gbChart->Visible = true;
        Splitter->Visible = true;
        chtTraffic->BringToFront();

        lbDevType->Visible = true;
        cbDevType->Visible = true;
        cbDevType->ItemIndex = 0;
        cbDevType->Enabled = false;
        lbDevName->Visible = true;
        cbDevName->Visible = true;
    }

	tlMain->EndUpdate();

    UINT32  ulTime = GetTime(deStartTime->Date);
    if (cbLastTime->Checked)
    {
        ulTime = gos_get_current_time();
    }

    if (cbLastTime->Checked)
    {
        ulTime = 0xffffffff;
    }

    GJsonParam  Param;
    Param.Add("Time", ulTime);
    SendServer(EV_OMT_OMC_REQ, MSG_OMT_GetAllDevStatus, Param);

    //btnRefreshClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormPerf::cbLastTimePropertiesChange(TObject *Sender)
{
    deStartTime->Enabled = !cbLastTime->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFormPerf::pcMainChange(TObject *Sender)
{
    ChangePage();
}
//---------------------------------------------------------------------------

void __fastcall TFormPerf::ColumnDevTypeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[2].IsNull())
    {
        return;
    }

    UINT32  ulDevType = ANode->Values[2];

    Value = GetDevTypeText(ulDevType);
}
//---------------------------------------------------------------------------

void __fastcall TFormPerf::ColumnOnlineGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    if (ANode->Values[3].IsNull())
    {
        return;
    }

    BOOL    bOnline = ANode->Values[3];

    Value = bOnline?"在线":"离线";
}
//---------------------------------------------------------------------------
