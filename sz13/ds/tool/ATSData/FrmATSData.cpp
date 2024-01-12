//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"
#include "msg.h"
#include "ds_public.h"
#include "MsgDialog.h"
#include "ClientUtil.h"
#include "RemoteMsg.h"
#include "ClientInit.h"
#include "ATS.h"
#include "pis.h"
#include "FrmATSData.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxGraphics"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "dxSkinLilian"
#pragma link "dxSkinOffice2010Blue"
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
#pragma resource "*.dfm"

TFormATSData *FormATSData;

#pragma pack(push, 1)

#pragma pack(pop)

UINT8                   *g_pucATSRec = NULL;
VECTOR<ATS_REC_T*>      g_vATSRec;
VECTOR<ATS_REC_T*>      g_vRealATSRec;
bool                    g_bShowHex = false;
BOOL                    g_bShowTitle = TRUE;
UINT8                   g_aucATSAddr[4] = {0,0,0,0};
UINT16                  g_usATSPort = 0;
SOCKET                  g_stSock = INVALID_SOCKET;

BOOL    g_bATSUseNetOrder = FALSE;      // ATS数据是否采用网络字节序发送

bool    g_bAutoCloseATS = false;

int     g_iMaxRealRecNum = 1000;

AnsiString CharArrayToText(CHAR *szText, UINT32 ulMaxLen, UINT32 ulLen)
{
    char    acText[256];

	if (ulLen > ulMaxLen)
	{
	    ulLen = ulMaxLen;
	}

    memcpy(acText, szText, ulLen);
    acText[ulLen] = '\0';

    return acText;
}

INT32 CharArrayToInt(CHAR *szText, UINT32 ulMaxLen, INT32 ulLen)
{
    AnsiString  str = CharArrayToText(szText, ulMaxLen, ulLen);

    return str.ToIntDef(-1);
}

AnsiString GetSimpleTime(ATS_TIME_T &stTime)
{
    CHAR        acTime[32];
    INT32       iTimeZone = gos_get_timezone();
    INT32       iHour = stTime.ucHour;

    iHour += iTimeZone/3600;
    iHour = MOD(iHour, 24);
    sprintf(acTime, "%02u:%02u:%02u",
//      stTime.usYear, stTime.ucMonth, stTime.ucDay,
        iHour, stTime.ucMin, stTime.ucSec);

    return acTime;
}

bool IsATSInfoSame(ATS_INFO_T *pstATSInfo1, ATS_INFO_T *pstATSInfo2)
{
    ATS_INFO_T  stATSInfo2;

    memcpy(&stATSInfo2, pstATSInfo2, sizeof(stATSInfo2));

    stATSInfo2.iOtpTime = pstATSInfo1->iOtpTime;

    memcpy(&stATSInfo2.stArriveTime, &pstATSInfo1->stArriveTime, sizeof(stATSInfo2.stArriveTime));
    memcpy(&stATSInfo2.stDepartTime, &pstATSInfo1->stDepartTime, sizeof(stATSInfo2.stDepartTime));

    stATSInfo2.ucRate = pstATSInfo1->ucRate;
    stATSInfo2.ucSpeed = pstATSInfo1->ucSpeed;

    memcpy(stATSInfo2.aucResv, pstATSInfo1->aucResv, sizeof(stATSInfo2.aucResv));

    return memcmp(pstATSInfo1, &stATSInfo2, sizeof(stATSInfo2)) == 0;
}

bool IsATSInfoSame(ATS_INFO_T *pstATSInfo1, ATS_INFO_T *pstATSInfo2, UINT32 ulNum)
{
    for (UINT32 i=0; i<ulNum; i++)
    {
        if (!IsATSInfoSame(pstATSInfo1+i, pstATSInfo2+i))
        {
            return false;
        }
    }

    return true;

    return memcmp(pstATSInfo1, pstATSInfo2, ulNum*sizeof(ATS_INFO_T)) == 0;
}

UINT16 ATSNtohs(UINT16 usValue)
{
    if (g_bATSUseNetOrder)
    {
        return ntohs(usValue);
    }
    else
    {
        UINT8   *pucValue = (UINT8*)&usValue;

        usValue = (pucValue[1]<<8) | pucValue[0]; // windows字节序

        return usValue;
    }
}

UINT32 ATSNtohl(UINT32 ulValue)
{
    if (g_bATSUseNetOrder)
    {
        return ntohl(ulValue);
    }
    else
    {
        UINT8   *pucValue = (UINT8*)&ulValue;

        ulValue = (pucValue[3] << 24) | (pucValue[2] << 16) |
                  (pucValue[1] << 8 ) | pucValue[0];     // windows字节序
        return ulValue;
    }
}

void ATSInfoToHost(ATS_INFO_T *pstATSInfo)
{
    pstATSInfo->usRtuID             = ATSNtohs(pstATSInfo->usRtuID);
    pstATSInfo->usStationID         = ATSNtohs(pstATSInfo->usStationID);
    pstATSInfo->usUpsideStationID   = ATSNtohs(pstATSInfo->usUpsideStationID);
    pstATSInfo->usDownsideStationID = ATSNtohs(pstATSInfo->usDownsideStationID);
    pstATSInfo->usLogicSectionID    = ATSNtohs(pstATSInfo->usLogicSectionID);
    pstATSInfo->usPhysicSectionID   = ATSNtohs(pstATSInfo->usPhysicSectionID);
    pstATSInfo->usDriverNumber      = ATSNtohs(pstATSInfo->usDriverNumber);
    pstATSInfo->iOtpTime            = ATSNtohl(pstATSInfo->iOtpTime);
    pstATSInfo->stArriveTime.usYear = ATSNtohs(pstATSInfo->stArriveTime.usYear);
    pstATSInfo->stDepartTime.usYear = ATSNtohs(pstATSInfo->stDepartTime.usYear);
}

int ConvertATSInfo(ATS_INFO_T *pstATSInfo, ATS_INFO_T *pstNewATSInfo)
{
    UINT8       *pucLen = &pstATSInfo->ucTrainUnitIDLen;

    memset(pstNewATSInfo, 0, sizeof(ATS_INFO_T));

    pstNewATSInfo->ucTrainUnitIDLen = MIN(pucLen[0], sizeof(pstATSInfo->acTrainUnitID));
    memcpy(pstNewATSInfo->acTrainUnitID, ++pucLen, pstNewATSInfo->ucTrainUnitIDLen);
    pucLen += pstNewATSInfo->ucTrainUnitIDLen;

    pstNewATSInfo->ucServiceNumberLen = MIN(pucLen[0], sizeof(pstATSInfo->acServiceNumber));
    memcpy(pstNewATSInfo->acServiceNumber, ++pucLen, pstNewATSInfo->ucServiceNumberLen);
    pucLen += pstNewATSInfo->ucServiceNumberLen;

    pstNewATSInfo->ucOrderNumberLen = MIN(pucLen[0], sizeof(pstATSInfo->acOrderNumber));
    memcpy(pstNewATSInfo->acOrderNumber, ++pucLen, pstNewATSInfo->ucOrderNumberLen);
    pucLen += pstNewATSInfo->ucOrderNumberLen;

    pstNewATSInfo->ucDestIDLen = MIN(pucLen[0], sizeof(pstATSInfo->acDestID));
    memcpy(pstNewATSInfo->acDestID, ++pucLen, pstNewATSInfo->ucDestIDLen);
    pucLen += pstNewATSInfo->ucDestIDLen;

    memcpy(&pstNewATSInfo->usRtuID, pucLen, sizeof(ATS_INFO_T)-STRUCT_OFFSET(ATS_INFO_T, usRtuID));

    int iLen = sizeof(ATS_INFO_T)-STRUCT_OFFSET(ATS_INFO_T, usRtuID);

    iLen += pucLen-(UINT8*)pstATSInfo;

    return iLen;
}

int InitATSRec(ATS_REC_T *pstATSRec)
{
    if (strcmp(pstATSRec->acMagic, ATS_REC_MAGIC) != 0)
    {
        return -1;
    }
    
    NTOHL_IT(pstATSRec->ulTime);

    NTOHS_IT(pstATSRec->stRecData.stMsgHdr.usMsgLen);
    NTOHS_IT(pstATSRec->stRecData.stMsgHdr.stMsgTime.usYear);
    NTOHS_IT(pstATSRec->stRecData.stMsgHdr.usLineID);

    int     iOffset = 0;
    UINT8   *pucATSInfo = (UINT8*)pstATSRec->stRecData.astATSInfo;
    ATS_REC_T  stNewATSRec;
    ATS_INFO_T  *pstATSInfo;

    memcpy(&stNewATSRec, pstATSRec, STRUCT_OFFSET(ATS_REC_T, stRecData));
    memcpy(&stNewATSRec.stRecData, &pstATSRec->stRecData, STRUCT_OFFSET(ATS_MSG_ATSINFO_T, astATSInfo));

    for (UINT32 i=0; i<pstATSRec->stRecData.ucTrainNum; i++)
    {
        pstATSInfo = &stNewATSRec.stRecData.astATSInfo[i];

        iOffset += ConvertATSInfo((ATS_INFO_T*)(pucATSInfo+iOffset), pstATSInfo);

        ATSInfoToHost(pstATSInfo);

        // 初始化可能会变化的字段，这些字段不显示，方便重复记录过滤
        pstATSInfo->iOtpTime = 0;
        memset(&pstATSInfo->stArriveTime, 0, sizeof(pstATSInfo->stArriveTime));
        memset(&pstATSInfo->stDepartTime, 0, sizeof(pstATSInfo->stDepartTime));

        pstATSInfo->ucRate = 0;
        pstATSInfo->ucSpeed = 0;

        memset(pstATSInfo->aucResv, 0, sizeof(pstATSInfo->aucResv));
    }

    int iSpareNum = ARRAY_SIZE(pstATSRec->stRecData.astATSInfo) - pstATSRec->stRecData.ucTrainNum;
    int iLen = sizeof(ATS_REC_T) - iSpareNum*sizeof(pstATSRec->stRecData.astATSInfo[0]);

    memcpy(pstATSRec, &stNewATSRec, iLen);

    return iLen;
}

bool LoadATSRec(AnsiString strFile)
{
    CHAR    acFile[256];
    FILE    *fp = fopen(acFile, "rb");
    int     iFileLen = 0;
    int     iReadLen;
    ATS_REC_T   *pstATSRec;

    g_vATSRec.clear();

    //  sprintf(acFile, "%s/dis/ATS.dat", gos_get_root_path(), "strFile");
	sprintf(acFile, "%s", strFile.c_str());
    if (!gos_file_exist(acFile))
    {
        return false;
    }

    fp = fopen(acFile, "rb");
    if (!fp)
    {
        return false;
    }

    fseek(fp, 0, SEEK_END);
    iFileLen = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (iFileLen <= 0)
    {
        fclose(fp);
        return false;
    }

    GOS_FREE(g_pucATSRec);
    g_pucATSRec = (UINT8*)calloc(1, iFileLen);
    if (!g_pucATSRec)
    {
        fclose(fp);
        return false;
    }

    iReadLen = fread(g_pucATSRec, 1, iFileLen, fp);
    fclose(fp);

    if (iReadLen != iFileLen)
    {
        return false;
    }

    iReadLen = 0;
    pstATSRec = (ATS_REC_T*)g_pucATSRec;
    //   ulHdrLen = ((UINT8*)pstATSRec->stATSInfo.astATSInfo - g_pucATSRec);

    while (iReadLen < iFileLen)
    {
        int  iLen = InitATSRec(pstATSRec);
        
        if (iLen < 0)
        {
            break;
        }
        
        iReadLen += iLen;
        g_vATSRec.push_back(pstATSRec);

        pstATSRec = (ATS_REC_T*)(g_pucATSRec+iReadLen);
    }

    return true;
}

VOID SaveATSToFile(ATS_REC_T *pstATSRec)
{
    static FILE    *fp = NULL;

    if (!fp)
    {
        char    acFile[256];

        sprintf(acFile, "%s/ats.dat", gos_get_root_path());
        fp = fopen(acFile, "w+b");

        if (!fp)
        {
            return;
        }
    }

    UINT32  ulSpareTrainNum = ARRAY_SIZE(pstATSRec->stRecData.astATSInfo) - pstATSRec->stRecData.ucTrainNum;
    UINT32  ulRecLen = sizeof(ATS_REC_T) - ulSpareTrainNum*sizeof(pstATSRec->stRecData.astATSInfo[0]);

    HTONL_IT(pstATSRec->ulTime);

    if (ulRecLen != fwrite(pstATSRec, 1, ulRecLen, fp))
    {
        GosLog(LOG_ERROR, "save ATS info to file failed");
    }

    fflush(fp);
}

AnsiString GetIntValue(UINT8 ucValue)
{
    CHAR    acText[32];

	if (g_bShowHex)
    {
        sprintf(acText, "%02X", ucValue);
    }
    else
    {
        sprintf(acText, "%u", ucValue);
    }

    return acText;
}

AnsiString GetIntValue(UINT16 usValue)
{
    CHAR    acText[32];

    if (g_bShowHex)
    {
        sprintf(acText, "%04X", usValue);
    }
    else
    {
        sprintf(acText, "%u", usValue);
    }

    return acText;
}

AnsiString GetIntValue(UINT32 ulValue)
{
    CHAR    acText[32];

    if (g_bShowHex)
    {
        sprintf(acText, "%08X", ulValue);
    }
    else
    {
        sprintf(acText, "%u", ulValue);
    }
    return acText;
}

AnsiString GetIPText(UINT8 *pucIP)
{
    CHAR    acText[32];

    sprintf(acText, IP_FMT, IP_ARG(pucIP));

    return acText;
}

//---------------------------------------------------------------------------
__fastcall TFormATSData::TFormATSData(TComponent* Owner)
    : TSubForm(Owner)
{
    LabelInfo->Visible = false;

    sgRec->Cells[0][0] = "序号";
    sgRec->Cells[1][0] = "接收时间";
    sgRec->Cells[2][0] = "地址";
    sgRec->Cells[3][0] = "列车数";
    sgRec->Cells[4][0] = "消息长度";
    sgRec->Cells[5][0] = "发送时间";
    sgRec->Cells[6][0] = "线路号";
    sgRec->Cells[7][0] = "消息ID";
    sgRec->Cells[8][0] = "版本号";
    sgRec->Cells[9][0] = "记录索引";

    sgRec->ColWidths[0] = 50;
    sgRec->ColWidths[1] = 80;
	sgRec->ColWidths[2] = 132;
    sgRec->ColWidths[3] = 50;
    sgRec->ColWidths[4] = 80;
    sgRec->ColWidths[5] = 80;
    sgRec->ColWidths[6] = 60;
    sgRec->ColWidths[7] = 60;
    sgRec->ColWidths[8] = 60;
    sgRec->ColWidths[9] = 80;

    pcMain->ActivePageIndex = 0;
    tlRec->BringToFront();
}

//---------------------------------------------------------------------------
void __fastcall TFormATSData::ShowRealATSRec(ATS_REC_T &stRec)
{
    CHAR            acTime[32];
    TcxTreeListNode *Node;
    ATS_REC_T       *pstRec;

    // 检查重复
    if (cbFilterRec->Checked && g_vRealATSRec.size() > 0)
    {
        // TODO
        pstRec = g_vRealATSRec[g_vRealATSRec.size()-1];

        if (pstRec->stRecData.ucTrainNum == stRec.stRecData.ucTrainNum)
        {
            if (IsATSInfoSame(pstRec->stRecData.astATSInfo, stRec.stRecData.astATSInfo, pstRec->stRecData.ucTrainNum))
            {
                return;
            }
        }
    }

    pstRec = (ATS_REC_T*)calloc(1, sizeof(ATS_REC_T));
    memcpy(pstRec, &stRec, sizeof(stRec));

    gos_get_text_time_ex(&pstRec->ulTime, NULL, acTime);

 // Node->Values[0] = ++g_iRecNum;
    Node = tlRec->Add();

    Node->Values[0] = (INT64)pstRec;
    Node->Values[1] = acTime+11;
    Node->Values[2] = GetIPText(pstRec->aucAddr);
    Node->Values[3] = pstRec->stRecData.ucTrainNum;
    Node->Values[4] = pstRec->stRecData.stMsgHdr.usMsgLen;
    Node->Values[5] = GetSimpleTime(pstRec->stRecData.stMsgHdr.stMsgTime);
    Node->Values[6] = pstRec->stRecData.stMsgHdr.usLineID;
    Node->Values[7] = pstRec->stRecData.stMsgHdr.ucMsgID;
    Node->Values[8] = pstRec->stRecData.stMsgHdr.ucVer;

    if (tlRec->AbsoluteCount == 1)
    {
        Node->Selected = true;
    }

    g_vRealATSRec.push_back(pstRec);

    if (tlRec->AbsoluteCount >= g_iMaxRealRecNum)
    {
        tlRec->Root->GetLastChild()->Delete();

        pstRec = g_vRealATSRec[0];
        free(pstRec);
        g_vRealATSRec.erase(g_vRealATSRec.begin()+0);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormATSData::ShowATSRec(ATS_REC_T *pstRec, int iIndex)
{
    CHAR    acTime[32];
    int     r;

    if (!sgRec->Cells[0][1].IsEmpty())
    {
        sgRec->RowCount++;
    }

    r = sgRec->RowCount - 1;

    gos_get_text_time_ex(&pstRec->ulTime, NULL, acTime);
    sgRec->Cells[0][r] = r;
    sgRec->Cells[1][r] = acTime+11;
    sgRec->Cells[2][r] = GetIPText(pstRec->aucAddr);
    sgRec->Cells[3][r] = pstRec->stRecData.ucTrainNum;
    sgRec->Cells[4][r] = pstRec->stRecData.stMsgHdr.usMsgLen;
    sgRec->Cells[5][r] = GetSimpleTime(pstRec->stRecData.stMsgHdr.stMsgTime);
    sgRec->Cells[6][r] = pstRec->stRecData.stMsgHdr.usLineID;
    sgRec->Cells[7][r] = pstRec->stRecData.stMsgHdr.ucMsgID;
    sgRec->Cells[8][r] = pstRec->stRecData.stMsgHdr.ucVer;
    sgRec->Cells[9][r] = iIndex;
}
//---------------------------------------------------------------------------
void __fastcall TFormATSData::btnRefreshClick(TObject *Sender)
{
    LabelInfo->Visible = false;

	if (!LoadATSRec(edtFile->Text))
    {
        LabelInfo->Caption = "读取数据失败";
        LabelInfo->Visible = true;
        return;
    }

    char        acTime[32];
    ATS_REC_T   *pstRec;
    ATS_REC_T   *pstLastRec = NULL;
    UINT32      ulRecLen = 0;

    sgRec->RowCount = 2;
    sgRec->Cells[0][1] = "";
    sgRec->BeginUpdate();

    for (UINT32 i=0; i<g_vATSRec.size(); i++)
    {
        pstRec = g_vATSRec[i];

        if (cbFilterRec->Checked && pstLastRec)
        {
            ulRecLen = pstRec->stRecData.ucTrainNum*sizeof(pstRec->stRecData.astATSInfo[0]);

            if (pstRec->stRecData.ucTrainNum == pstLastRec->stRecData.ucTrainNum)
            {
//              if (IsATSInfoSame(pstRec->stRecData.astATSInfo, pstLastRec->stRecData.astATSInfo, pstRec->stRecData.ucTrainNum))
                if (memcmp(pstRec->stRecData.astATSInfo, pstLastRec->stRecData.astATSInfo, ulRecLen) == 0)
                {
                    continue;
                }
            }
		}

		ShowATSRec(pstRec, i);
        pstLastRec = pstRec;
    }

    sgRec->EndUpdate();

//  tlRec->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormATSData::ShowATSInfo(ATS_REC_T *pstRec)
{
    tlATS->Clear();

    /*
    if (tlRec->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode     *Node = tlRec->Selections[0];
    UINT32              ulIndex = Node->Values[0];

    if (iIndex >= vATERec.size())
    {
        return;
    }

    ATS_REC_T       *pstRec = vATERec[iIndex];
                  */

    tlATS->BeginUpdate();
    for (UINT32 i=0; i<pstRec->stRecData.ucTrainNum; i++)
    {
        ATS_INFO_T  *pstATS = &pstRec->stRecData.astATSInfo[i];

        TcxTreeListNode *Node = tlATS->Add();

        Node->Values[0]  = i+1;
		Node->Values[1]  = CharArrayToText(pstATS->acTrainUnitID, sizeof(pstATS->acTrainUnitID), pstATS->ucTrainUnitIDLen);            // 车体号
		Node->Values[2]  = CharArrayToText(pstATS->acServiceNumber, sizeof(pstATS->acServiceNumber), pstATS->ucServiceNumberLen);    // 服务号
		Node->Values[3]  = CharArrayToText(pstATS->acOrderNumber, sizeof(pstATS->acOrderNumber), pstATS->ucOrderNumberLen);     // 序列号
		Node->Values[4]  = CharArrayToText(pstATS->acDestID, sizeof(pstATS->acDestID), pstATS->ucDestIDLen);                // 目的地号
		Node->Values[5]  = pstATS->usDriverNumber;          // 司机号
		Node->Values[6]  = pstATS->ucCompartmentNum;        // 几节编组
		Node->Values[7]  = pstATS->usStationID;             // 当前车站编号
        Node->Values[8]  = pstATS->usUpsideStationID;       // 列车上行侧车站的编号(刚经过的车站，车辆段时为车辆段编号）
		Node->Values[9]  = pstATS->usDownsideStationID;     // 列车下行侧车站的编号(将到达的车站，车辆段时为车辆段编号）
        Node->Values[10] = pstATS->ucDirection;             // 运行方向：0为上行，1为下行，2未知
		Node->Values[11] = pstATS->ucOnTurnBackTrackFlag;   // 是否在折返站台：0不在，1在，2未知
		Node->Values[12] = pstATS->ucIsOnPlatform;          // 列车所在的站台编号，如果列车不在站台时，该值0
        Node->Values[13] = pstATS->ucOnTransferTrackFlag;   // 是否在转换轨，不在转换轨为0，在转换轨为1
        Node->Values[14] = pstATS->ucInOutDepot;            // 是否进出正线（不采用该字段来判断）,
        Node->Values[15] = pstATS->ucLostRealLocation;      // 是否已经失去该列车的真实位置，1失去，0未失去
//      Node->Values[16] = pstATS->acTrackName;
		Node->Values[16] = pstATS->usPhysicSectionID;
		Node->Values[17] = pstATS->usLogicSectionID;
		Node->Values[18] = pstATS->ucIndexInSection;
	}

    tlATS->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormATSData::tlATSGetCellHint(TcxCustomTreeList *Sender, TObject *ACell,
          UnicodeString &AText, bool &ANeedShow)
{
    AnsiString strName = ACell->ClassName();

    if (strName != "TcxTreeListEditCellViewInfo")
    {
        return;
    }

    TcxTreeListEditCellViewInfo     *Cell = (TcxTreeListEditCellViewInfo*)ACell;
    int                             iCol = Cell->Column->ItemIndex;
    TcxTreeListNode                 *Node = Cell->Node;
    UINT32                          ulValue = 0xffffffff;

    if (iCol < 0)
    {
        return;
    }

    AText = "";

    if (iCol == 10) // 运行方向：0为上行，1为下行，2未知  卡斯柯已经修改成：0：未知 1：上行 2：下行
	{
		ulValue = Node->Values[iCol];
		if (ulValue == ATS_DIRECTION_UP)
		{
			AText = "上行";
		}
		else if (ulValue == ATS_DIRECTION_DOWN)
		{
			AText = "下行";
		}
		else
		{
			AText = "未知";
		}
	}
	else if (iCol == 11) // 是否在折返站台：0不在，1在，2未知
	{
		ulValue = Node->Values[iCol];
		if (ulValue == 0)
		{
			AText = "";
		}
		else if (ulValue == 1)
		{
			AText = "在折返轨或存车轨";
		}
	}
	else if (iCol == 13) //是否在转换轨，不在转换轨为0，在转换轨为1
	{
		ulValue = Node->Values[iCol];
		if (ulValue == 0)
		{
			AText = "";
		}
		else if (ulValue == 1)
		{
			AText = "在转换轨";
		}
	}
	else if (iCol == 14) //车辆段进入转换轨时值为1,正线进入转换轨为2
	{
		ulValue = Node->Values[iCol];
		if (ulValue == 0)
		{
			AText = "进入正线";
		}
		else if (ulValue == 1)
		{
			AText = "进入段场";
		}
	}

    ANeedShow = !AText.IsEmpty();
}
//---------------------------------------------------------------------------
void __fastcall TFormATSData::cbShowHexPropertiesChange(TObject *Sender)
{
	g_bShowHex = cbShowHex->Checked;

    tlRec->BeginUpdate();
    tlRec->EndUpdate();

    tlATS->BeginUpdate();
    tlATS->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormATSData::tlATSGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    int     iCol = Sender->ItemIndex;

    if (ANode->Values[iCol].IsNull())
    {
        return;
    }

    if (iCol == 5 || iCol == 7 || iCol == 8 || iCol == 9 )
    {
        UINT32  ulValue = (UINT32)ANode->Values[iCol];

		Value = GetIntValue((UINT16)ulValue);
    }
    else if (iCol == 6 || iCol == 10 || iCol == 11 || iCol == 12 ||
			 iCol == 13 || iCol == 14 || iCol == 15 ||
			 iCol == 16 || iCol == 17 || iCol == 18 )
	{
        UINT32  ulValue = (UINT32)ANode->Values[iCol];

        Value = GetIntValue((UINT8)ulValue);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormATSData::tlRecFocusedNodeChanged(TcxCustomTreeList *Sender, TcxTreeListNode *APrevFocusedNode,
          TcxTreeListNode *AFocusedNode)
{
    if (AFocusedNode->Values[0].IsNull())
    {
        return;
    }

    INT64       i64Index = (INT64)AFocusedNode->Values[0];
    ATS_REC_T   *pstRec = (ATS_REC_T*)i64Index;

    ShowATSInfo(pstRec);
}
//---------------------------------------------------------------------------
void __fastcall TFormATSData::FormShow(TObject *Sender)
{
   //   btnRefreshClick(this);
}

//---------------------------------------------------------------------------
void __fastcall TFormATSData::TimerATSTimer(TObject *Sender)
{
    UINT32              ulTimeout = 1000;
    UINT32              i;
    INT32               iRet;
    static UINT32       ulTimeoutNum = 0;
    static UINT8        aucBuf[sizeof(ATS_FRAME_HDR_T)+ATS_MAX_DATA_LEN+sizeof(UINT16)];
    ATS_FRAME_HDR_T     *pstFrameHdr = (ATS_FRAME_HDR_T*)aucBuf;
    UINT8               *pucMsgData = (UINT8*)(pstFrameHdr+1);
    UINT16              *pusTail;

    if (cbPause->Checked)
    {
        return;
    }

    if (g_usATSPort == 0)
    {
        LabelATS->Caption = "请配置ATS地址";
        return;
    }

    if (g_stSock == INVALID_SOCKET)
    {
        g_stSock = gos_connect_tcp_server(g_aucATSAddr, g_usATSPort);
    }

    if (g_stSock == INVALID_SOCKET)
    {
        LabelATS->Caption = "无法连接到ATS服务器";
        return;
    }

    LabelATS->Caption = "";

    // 接收消息头
    iRet = gos_recv_tcp_msg(g_stSock, pstFrameHdr, sizeof(ATS_FRAME_HDR_T), ulTimeout);
    if (iRet == 0)
    {
        ulTimeoutNum++;
        if (ulTimeoutNum >= 3)
        {
            GosLog(LOG_ERROR, "recv ATS info timeout");
            goto err;
        }

        return;
    }

    NTOHS_IT(pstFrameHdr->usFrameHead);
    NTOHS_IT(pstFrameHdr->usDataLen);

    if (pstFrameHdr->usFrameHead != ATS_FRAME_HEAD)
    {
        GosLog(LOG_ERROR, "recv ATS info failed, invalid frame head 0x%04X", pstFrameHdr->usFrameHead);
        goto err;
    }

    // 接收数据部分和帧尾
    iRet = gos_recv_tcp_msg(g_stSock, pucMsgData, pstFrameHdr->usDataLen+sizeof(UINT16), ulTimeout);
    if (iRet == 0)
    {
        ulTimeoutNum++;
        if (ulTimeoutNum >= 3)
        {
            GosLog(LOG_ERROR, "recv ATS info timeout");
            goto err;
        }

        return;
    }

    if (iRet < 0)
    {
        GosLog(LOG_ERROR, "recv ATS of " IP_FMT " info failed: %s", IP_ARG(g_aucATSAddr), gos_get_socket_err());
        goto err;
    }

    if ((UINT32)iRet != (pstFrameHdr->usDataLen+sizeof(UINT16)))
    {
        GosLog(LOG_ERROR, "invalid ATS info length: %d", iRet);
        goto err;
    }

    pusTail = (UINT16*)(pucMsgData+pstFrameHdr->usDataLen);
    *pusTail = ntohs(*pusTail);
    if ((*pusTail) != ATS_FRAME_TAIL)
    {
        GosLog(LOG_ERROR, "invalid ATS frame tail 0x%04X", *pusTail);
        goto err;
    }

    if (pstFrameHdr->ucFrameCount == 0)
    {
        GosLog(LOG_ERROR, "frame count must be larger than 0");
        goto err;
    }

    if (pstFrameHdr->ucFrameIndex == 0)
    {
        GosLog(LOG_ERROR, "frame index must be larger than 0");
        goto err;
    }

    if (pstFrameHdr->ucFrameIndex > pstFrameHdr->ucFrameCount)
    {
        GosLog(LOG_ERROR, "frame index(%u) is larger than frame count %u", pstFrameHdr->ucFrameIndex, pstFrameHdr->ucFrameCount);
        goto err;
    }

    if (pstFrameHdr->ucFrameIndex == 1)
    {
        m_ulFrameNum = pstFrameHdr->ucFrameCount;
        m_ulLastFrameIndex = pstFrameHdr->ucFrameIndex;
        memcpy(m_aucRecvBuf, pucMsgData, pstFrameHdr->usDataLen);
        m_ulRecvMsgLen = pstFrameHdr->usDataLen;
    }
    else
    {
        if (m_ulFrameNum != pstFrameHdr->ucFrameCount)
        {
            GosLog(LOG_ERROR, "frame count(%u) should be %u", pstFrameHdr->ucFrameCount, m_ulFrameNum);
            goto err;
        }

        if ((m_ulLastFrameIndex+1) != pstFrameHdr->ucFrameIndex)
        {
            GosLog(LOG_ERROR, "frame index(%u) should be %u", pstFrameHdr->ucFrameCount, m_ulLastFrameIndex+1);
            goto err;
        }

        if ((m_ulRecvMsgLen + pstFrameHdr->usDataLen) > sizeof(m_aucRecvBuf))
        {
            GosLog(LOG_ERROR, "msg data length(%u) is out of bound(%u)", m_ulRecvMsgLen + pstFrameHdr->usDataLen, sizeof(m_aucRecvBuf));
            goto err;
        }

        memcpy(m_aucRecvBuf+m_ulRecvMsgLen, pucMsgData, pstFrameHdr->usDataLen);
        m_ulRecvMsgLen += pstFrameHdr->usDataLen;

        m_ulLastFrameIndex = pstFrameHdr->ucFrameIndex;
    }

    if (m_ulLastFrameIndex != m_ulFrameNum)
    {
        return;
    }

    if (OnATSMsg((ATS_MSG_HDR_T *)m_aucRecvBuf, m_ulRecvMsgLen, g_aucATSAddr) < 0)
    {
        goto err;
    }

    return;

err:
    GosLog(LOG_ERROR, "close ATS socket %u", g_stSock);
    CLOSE_SOCKET(g_stSock);
}

INT32 __fastcall TFormATSData::OnATSMsg(ATS_MSG_HDR_T *pstMsgHdr, INT32 iMsgDataLen, UINT8 *pucAddr)
{
    UINT32              ulTime = gos_get_current_time();
    ATS_MSG_ATSINFO_T   *pstATSInfo = (ATS_MSG_ATSINFO_T*)pstMsgHdr;

    if (iMsgDataLen < sizeof(ATS_MSG_HDR_T))
    {
        GosLog(LOG_ERROR, "invalid msg length %d", iMsgDataLen);
        return -1;
    }

    NTOHS_IT(pstMsgHdr->usMsgLen);
    NTOHS_IT(pstMsgHdr->stMsgTime.usYear);
    NTOHS_IT(pstMsgHdr->usLineID);

    if ((UINT32)iMsgDataLen < (sizeof(pstMsgHdr->usMsgLen) + pstMsgHdr->usMsgLen))
    {
        GosLog(LOG_ERROR, "invalid msg(%u) length %u", pstMsgHdr->ucMsgID, pstMsgHdr->usMsgLen);
        return -1;
    }

    if (pstMsgHdr->ucMsgID == ATS_MSGID_HEARTBEAT)
    {
        if (pstMsgHdr->usMsgLen != (sizeof(ATS_MSG_HEARTBEAT_T)-sizeof(pstMsgHdr->usMsgLen)))
        {
            GosLog(LOG_ERROR, "invalid msg length %u of heart beat", pstMsgHdr->usMsgLen);
            return -1;
        }

        return sizeof(ATS_MSG_HEARTBEAT_T);
    }

    if (pstMsgHdr->ucMsgID != ATS_MSGID_ATSINFO)
    {
        GosLog(LOG_ERROR, "unknown ATS msg id %u", pstMsgHdr->ucMsgID);
        return -1;
    }

    UINT32  ulMsgLen = STRUCT_OFFSET(ATS_MSG_ATSINFO_T, astATSInfo) + pstATSInfo->ucTrainNum*sizeof(pstATSInfo->astATSInfo[0]);

    if (ulMsgLen != (sizeof(UINT16) + pstMsgHdr->usMsgLen))
    {
        GosLog(LOG_ERROR, "invalid ATS msg length %u, while train count is %u", pstMsgHdr->usMsgLen, pstATSInfo->ucTrainNum);
        return -1;
    }

    static ATS_REC_T    stATSRec = {0};

    strcpy(stATSRec.acMagic, ATS_REC_MAGIC);
    stATSRec.ulTime = ulTime;
    memcpy(stATSRec.aucAddr, pucAddr, 4);
    memcpy(&stATSRec.stRecData, pstATSInfo, ulMsgLen);
    SaveATSToFile(&stATSRec);

    for (UINT32 i=0; i<pstATSInfo->ucTrainNum; i++)
    {
        ATSInfoToHost(&stATSRec.stRecData.astATSInfo[i]);
    }

    NTOHL_IT(stATSRec.ulTime);
    ShowRealATSRec(stATSRec);

    return ulMsgLen;

    //Log(LOG_DETAIL, "get ATS info successful");
}
//---------------------------------------------------------------------------
void __fastcall TFormATSData::btnSetClick(TObject *Sender)
{
    AnsiString  strAddr = edtATSAddr->Text;

    g_usATSPort = ATS_SERVER_PORT;
    if (!GetIPPort(strAddr, g_aucATSAddr, &g_usATSPort))
    {
        InfoMsg("地址错误");
        return;
    }

    if (0 == g_usATSPort)
    {
        g_usATSPort = ATS_SERVER_PORT;
    }

    CLOSE_SOCKET(g_stSock);
}
//---------------------------------------------------------------------------
void __fastcall TFormATSData::pcMainPageChanging(TObject *Sender, TcxTabSheet *NewPage,
          bool &AllowChange)
{
    if (NewPage == tsRealData)
    {
        tlRec->BringToFront();
    }
    else if (NewPage == tsShowFile)
    {
        sgRec->BringToFront();
        sgRec->Font = tlRec->Font;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormATSData::sgRecRowChanging(TObject *Sender, int OldRow, int NewRow,
          bool &Allow)
{
    int  iIndex = NewRow-1;

    AnsiString  strIndex = sgRec->Cells[9][NewRow];

    iIndex = strIndex.ToIntDef(-1);

    if (iIndex < 0 || iIndex >= g_vATSRec.size())
    {
        return;
    }

    ShowATSInfo(g_vATSRec[iIndex]);
}
//---------------------------------------------------------------------------

void __fastcall TFormATSData::edtFilePropertiesButtonClick(TObject *Sender, int AButtonIndex)
{
    if (OpenDialog->Execute())
    {
        edtFile->Text = OpenDialog->FileName;
        btnRefreshClick(this);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormATSData::sgRecClickCell(TObject *Sender, int ARow, int ACol)
{
    int  iIndex = -1;

    AnsiString  strIndex = sgRec->Cells[9][ARow];

    iIndex = strIndex.ToIntDef(-1);

    if (iIndex < 0 || iIndex >= g_vATSRec.size())
    {
        return;
    }

	ShowATSInfo(g_vATSRec[iIndex]);

}
//---------------------------------------------------------------------------

void __fastcall TFormATSData::btnExportRecClick(TObject *Sender)
{
	int  iIndex = -1;
    int  iRow = sgRec->Row;

    if (iRow < 0 || iRow >= sgRec->RowCount)
    {
        return;
    }

	AnsiString	strIndex = sgRec->Cells[9][iRow];

	iIndex = strIndex.ToIntDef(-1);

	if (iIndex < 0 || iIndex >= g_vATSRec.size())
	{
		return;
	}

    AnsiString  strRecs;
    CHAR        acRec[4096];
    ATS_REC_T   *pstRec = g_vATSRec[iIndex];

    strRecs = "{\n";
    for (UINT32 i=0; i<pstRec->stRecData.ucTrainNum; i++)
    {
		UINT8  *pucData = (UINT8*)&pstRec->stRecData.astATSInfo[i];
        UINT32  ulLen = 0;

        acRec[0] = '\0';
        for (UINT32 j=0; j<sizeof(pstRec->stRecData.astATSInfo[0]); j++)
        {
            ulLen += sprintf(acRec+ulLen, "0x%02X,", pucData[j]);
        }

        strRecs += acRec;
        strRecs += "\r\n";
	}

    strRecs += "};";

    gos_save_string_to_file("ats_rec.txt", strRecs.c_str());
}
//---------------------------------------------------------------------------

