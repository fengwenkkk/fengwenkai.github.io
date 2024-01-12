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
#include "FrmATS.h"
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
#pragma resource "*.dfm"

TFormATS *FormATS;

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

int     g_iMaxRealRecNum = 1000;

void LoadAudioDevCfg()
{
}

void ATSInfoToH(ATS_INFO_T *pstATSInfo)
{
    NTOHL_IT(pstATSInfo->ulTrainUnitID);      // 车组号
    NTOHL_IT(pstATSInfo->ulTrainServiceNumber);   // 服务号
    NTOHS_IT(pstATSInfo->usTrainSerialNumber);    // 序列号
    NTOHS_IT(pstATSInfo->usDriveNumber);          // 司机号
    NTOHS_IT(pstATSInfo->usRollingStockNumber);   // 几节编组
    NTOHS_IT(pstATSInfo->usStationID);            // 当前车站编号
    NTOHS_IT(pstATSInfo->usUpsideStationID);      // 列车上行侧车站的编号
    NTOHS_IT(pstATSInfo->usDownsideStationID);
}

void ATSInfoToH(GET_ATS_INFO_RSP_T *pstATSInfo)
{
    NTOHS_IT(pstATSInfo->stHdr.usTransID);
    NTOHS_IT(pstATSInfo->stHdr.usProtocolID);
    NTOHS_IT(pstATSInfo->stHdr.usLength);
    NTOHS_IT(pstATSInfo->usTrainNum);

    for (UINT32 i=0; i<pstATSInfo->usTrainNum; i++)
    {
        ATSInfoToH(&pstATSInfo->astATSInfo[i]);
    }
}

void ATSInfoToH(ATS_REC_T *pstATSRec)
{
    NTOHL_IT(pstATSRec->ulTime);
    ATSInfoToH(&pstATSRec->stATSInfo);
}

bool LoadATSRec(AnsiString strFile)
{
    CHAR    acFile[256];
    FILE    *fp = fopen(acFile, "rb");
    int     iFileLen = 0;
    int     iReadLen;
    ATS_REC_T   *pstATSRec;
    UINT32      ulHdrLen = 0;

    g_vATSRec.clear();

//  sprintf(acFile, "%s/dis/ATS.dat", gos_get_root_path(), "strFile");
    sprintf(acFile, "%s", strFile.c_str());
    if (!FileExists(acFile))
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
    ulHdrLen = ((UINT8*)pstATSRec->stATSInfo.astATSInfo - g_pucATSRec);

    while (iReadLen < iFileLen)
    {
        ATSInfoToH(pstATSRec);
        iReadLen += ulHdrLen + pstATSRec->stATSInfo.usTrainNum*sizeof(pstATSRec->stATSInfo.astATSInfo[0]);
        g_vATSRec.push_back(pstATSRec);

        pstATSRec = (ATS_REC_T*)(g_pucATSRec+iReadLen);
    }

    return true;
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
__fastcall TFormATS::TFormATS(TComponent* Owner)
    : TSubForm(Owner)
{
    LabelInfo->Caption = "";

    sgRec->Cells[0][0] = "序号";
    sgRec->Cells[1][0] = "时间";
    sgRec->Cells[2][0] = "地址";
    sgRec->Cells[3][0] = "事务标识";
    sgRec->Cells[4][0] = "协议标识";
    sgRec->Cells[5][0] = "长度";
    sgRec->Cells[6][0] = "单元标识";
    sgRec->Cells[7][0] = "功能码";
    sgRec->Cells[8][0] = "起始地址";
    sgRec->Cells[9][0] = "寄存器个数";
    sgRec->Cells[10][0] = "记录位置";

    pcMain->ActivePageIndex = 0;
    tlRec->BringToFront();
}

//---------------------------------------------------------------------------
void __fastcall TFormATS::ShowRealATSRec(ATS_REC_T &stRec)
{
    CHAR            acTime[32];
    TcxTreeListNode *Node;
    ATS_REC_T       *pstRec;

    // 检查重复
    if (cbFilterRec->Checked && g_vRealATSRec.size() > 0)
    {
        pstRec = g_vRealATSRec[g_vRealATSRec.size()-1];

        UINT32 ulRecLen = pstRec->stATSInfo.usTrainNum*sizeof(pstRec->stATSInfo.astATSInfo[0]);

        if (pstRec->stATSInfo.usTrainNum == stRec.stATSInfo.usTrainNum)
        {
            if (memcmp(pstRec->stATSInfo.astATSInfo, stRec.stATSInfo.astATSInfo, ulRecLen) == 0)
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
    Node->Values[3] = pstRec->stATSInfo.stHdr.usTransID;
    Node->Values[4] = pstRec->stATSInfo.stHdr.usProtocolID;
    Node->Values[5] = pstRec->stATSInfo.stHdr.usLength;
    Node->Values[6] = pstRec->stATSInfo.stHdr.ucUnitID;
    Node->Values[7] = pstRec->stATSInfo.ucFunctionCode;
    Node->Values[8] = pstRec->stATSInfo.ucByteCount;
    Node->Values[9] = pstRec->stATSInfo.usTrainNum;
    Node->Values[10] = "";

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
void __fastcall TFormATS::ShowATSRec(ATS_REC_T *pstRec, int iIndex)
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
    sgRec->Cells[3][r] = pstRec->stATSInfo.stHdr.usTransID;
    sgRec->Cells[4][r] = pstRec->stATSInfo.stHdr.usProtocolID;
    sgRec->Cells[5][r] = pstRec->stATSInfo.stHdr.usLength;
    sgRec->Cells[6][r] = pstRec->stATSInfo.stHdr.ucUnitID;
    sgRec->Cells[7][r] = pstRec->stATSInfo.ucFunctionCode;
    sgRec->Cells[8][r] = pstRec->stATSInfo.ucByteCount;
    sgRec->Cells[9][r] = pstRec->stATSInfo.usTrainNum;
    sgRec->Cells[10][r] = iIndex;
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::btnRefreshClick(TObject *Sender)
{
    LabelInfo->Caption = "";

    if (!LoadATSRec(edtFile->Text))
    {
        LabelInfo->Caption = "读取数据失败";
        return;
    }

//    tlRec->Clear();
//  tlRec->BeginUpdate();

//  TcxTreeListNode *Node;

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
            ulRecLen = pstRec->stATSInfo.usTrainNum*sizeof(pstRec->stATSInfo.astATSInfo[0]);

            if (pstRec->stATSInfo.usTrainNum == pstLastRec->stATSInfo.usTrainNum)
            {
                if (memcmp(pstRec->stATSInfo.astATSInfo, pstLastRec->stATSInfo.astATSInfo, ulRecLen) == 0)
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
void __fastcall TFormATS::ShowATSInfo(ATS_REC_T *pstRec)
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
    for (UINT32 i=0; i<pstRec->stATSInfo.usTrainNum; i++)
    {
        ATS_INFO_T  *pstATS = &pstRec->stATSInfo.astATSInfo[i];

        TcxTreeListNode *Node = tlATS->Add();

        Node->Values[0]  = i+1;
        Node->Values[1]  = pstATS->ulTrainUnitID;           // 车体号
        Node->Values[2]  = pstATS->ulTrainServiceNumber;    // 服务号
        Node->Values[3]  = pstATS->usTrainSerialNumber;     // 序列号
        Node->Values[4]  = pstATS->acDestID;                // 目的地号
        Node->Values[5]  = pstATS->usDriveNumber;           // 司机号
        Node->Values[6]  = pstATS->usRollingStockNumber;    // 几节编组
        Node->Values[7]  = pstATS->usStationID;             // 当前车站编号
        Node->Values[8]  = pstATS->usUpsideStationID;       // 列车上行侧车站的编号(刚经过的车站，车辆段时为车辆段编号）
        Node->Values[9]  = pstATS->usDownsideStationID;     // 列车下行侧车站的编号(将到达的车站，车辆段时为车辆段编号）
        Node->Values[10] = pstATS->ucDirection;             // 运行方向：0为上行，1为下行，2未知
        Node->Values[11] = pstATS->ucOnTurnbackTrackFlag;   // 是否在折返站台：0不在，1在，2未知
        Node->Values[12] = pstATS->ucOnPlatformID;          // 列车所在的站台编号，如果列车不在站台时，该值0
        Node->Values[13] = pstATS->ucOnTransferTrackFlag;   // 是否在转换轨，不在转换轨为0，在转换轨为1
        Node->Values[14] = pstATS->ucInOutDepot;            // 是否进出正线（不采用该字段来判断）,
        Node->Values[15] = pstATS->ucLostRealLocation;      // 是否已经失去该列车的真实位置，1失去，0未失去
        Node->Values[16] = pstATS->acTrackName;
    }

    tlATS->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::tlATSGetCellHint(TcxCustomTreeList *Sender, TObject *ACell,
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
        if (ulValue == TRAIN_DIRECTION_UPSIDE)
        {
            AText = "上行";
        }
        else if (ulValue == TRAIN_DIRECTION_DOWNSIDE)
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
        if (ulValue == TRAIN_ON_TURNBACK_TRACK_NO)
        {
            AText = "不在折返轨";
        }
        else if (ulValue == TRAIN_ON_TURNBACK_TRACK_YES)
        {
            AText = "在折返轨";
        }
        else
        {
            AText = "未知";
        }
    }
    else if (iCol == 13) //是否在转换轨，不在转换轨为0，在转换轨为1
    {
        ulValue = Node->Values[iCol];
        if (ulValue == 0)
        {
            AText = "不在转换轨";
        }
        else if (ulValue == 1)
        {
            AText = "在转换轨";
        }
    }
    else if (iCol == 14) //车辆段进入转换轨时值为1,正线进入转换轨为2
    {
        ulValue = Node->Values[iCol];
        if (ulValue == 1)
        {
            AText = "车辆段进入转换轨";
        }
        else if (ulValue == 2)
        {
            AText = "正线进入转换轨";
        }
    }

    ANeedShow = !AText.IsEmpty();
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::cbShowHexPropertiesChange(TObject *Sender)
{
    g_bShowHex = cbShowHex->Checked;

    tlRec->BeginUpdate();
    tlRec->EndUpdate();

    tlATS->BeginUpdate();
    tlATS->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::tlRecGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    int     iCol = Sender->ItemIndex;

    if (ANode->Values[iCol].IsNull())
    {
        return;
    }

    if (iCol == 3 || iCol == 4 || iCol == 5 || iCol == 9)
    {
        UINT32  ulValue = (UINT32)ANode->Values[iCol];

        Value = GetIntValue((UINT16)ulValue);
    }
    else if (iCol == 6 || iCol == 7 || iCol == 8)
    {
        UINT32  ulValue = (UINT32)ANode->Values[iCol];

        Value = GetIntValue((UINT8)ulValue);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::tlATSGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    int     iCol = Sender->ItemIndex;

    if (ANode->Values[iCol].IsNull())
    {
        return;
    }

    if (iCol == 1 || iCol == 2)
    {
        UINT32  ulValue = (UINT32)ANode->Values[iCol];

        Value = GetIntValue(ulValue);
    }
    else if (iCol == 3 || iCol == 5 || iCol == 6 ||
             iCol == 7 || iCol == 8 || iCol == 9 )
    {
        UINT32  ulValue = (UINT32)ANode->Values[iCol];

        Value = GetIntValue((UINT16)ulValue);
    }
    else if (iCol == 10 || iCol == 11 || iCol == 12 ||
             iCol == 13 || iCol == 14 || iCol == 15 )
    {
        UINT32  ulValue = (UINT32)ANode->Values[iCol];

        Value = GetIntValue((UINT8)ulValue);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::tlRecFocusedNodeChanged(TcxCustomTreeList *Sender, TcxTreeListNode *APrevFocusedNode,
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
void __fastcall TFormATS::FormShow(TObject *Sender)
{
   //   btnRefreshClick(this);
}

//---------------------------------------------------------------------------
void __fastcall TFormATS::TimerATSTimer(TObject *Sender)
{
    GET_ATS_INFO_REQ_T  stReq = {0};
    UINT32              ulTimeout = 1000;
    UINT32              i;
    INT32               iRet;
    static ATS_REC_T    stATSRec = {0};
    GET_ATS_INFO_RSP_T  *pstRsp = &stATSRec.stATSInfo;
    UINT32              ulHdrLen = (UINT32)((UINT8*)pstRsp->astATSInfo - (UINT8*)pstRsp);
    static UINT32       ulTimeoutNum = 0;
    VECTOR<ATS_INFO_T>  vATSInfo;
    UINT32              ulRecvLen = 0;
    UINT32              ulTime = gos_get_current_time();
    static UINT32       m_ulTransID = 0;

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

    m_ulTransID++;
    stReq.stHdr.usTransID = m_ulTransID;
    stReq.stHdr.usProtocolID = ATS_PROTOCOL_ID;
    stReq.stHdr.usLength = 6;   // len 后面的报文字节数
    stReq.stHdr.ucUnitID = ATS_UNIT_ID;

    stReq.ucFunctionCode = ATS_FUNC_CODE_REQ;
    stReq.usStartAddr = 0;
    stReq.usRegisterNum = sizeof(pstRsp->astATSInfo)/sizeof(UINT16);    // 2880

    if (cb2881->Checked)
    {
        stReq.usRegisterNum++;
    }

    HTONS_IT(stReq.stHdr.usTransID);
    HTONS_IT(stReq.stHdr.usProtocolID);
    HTONS_IT(stReq.stHdr.usLength);
    HTONS_IT(stReq.usStartAddr);
    HTONS_IT(stReq.usRegisterNum);

    if (!gos_tcp_send(g_stSock, &stReq, sizeof(stReq)))
    {
        GosLog(LOG_ERROR, "send msg to ATS failed: %s", gos_get_socket_err());
        goto err;
    }

    gos_sleep_1ms(100);
    iRet = gos_recv_tcp_msg(g_stSock, pstRsp, sizeof(GET_ATS_INFO_RSP_T ), ulTimeout);
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

    ulTimeoutNum = 0;

    if (iRet <= 0)
    {
        GosLog(LOG_ERROR, "recv ATS info failed: %s", gos_get_socket_err());
        goto err;
    }

    if (iRet < ulHdrLen)
    {
        GosLog(LOG_ERROR, "invalid ATS info length: %u", iRet);
        goto err;
    }

    NTOHS_IT(pstRsp->stHdr.usTransID);
    NTOHS_IT(pstRsp->stHdr.usProtocolID);
    NTOHS_IT(pstRsp->stHdr.usLength);
    NTOHS_IT(pstRsp->usTrainNum);

    ulRecvLen = ulHdrLen + stATSRec.stATSInfo.usTrainNum*sizeof(stATSRec.stATSInfo.astATSInfo[0]);
    if (ulRecvLen > iRet)
    {
        GosLog(LOG_ERROR, "invalid ATS train number: %u", stATSRec.stATSInfo.usTrainNum);
        goto err;
    }

    if (pstRsp->stHdr.usProtocolID != ATS_PROTOCOL_ID)
    {
        GosLog(LOG_ERROR, "recv ATS info failed, unknown protocol %u", pstRsp->stHdr.usProtocolID);
        goto err;
    }

    if (pstRsp->stHdr.ucUnitID != ATS_UNIT_ID)
    {
        GosLog(LOG_ERROR, "recv ATS info failed, unknown unit id %u", pstRsp->stHdr.ucUnitID);
        goto err;
    }

    if (pstRsp->ucFunctionCode == ATS_FUNC_CODE_ERR)
    {
        GosLog(LOG_ERROR, "recv ATS info failed, ATS return err code");
        goto err;
    }

    if (pstRsp->ucFunctionCode != ATS_FUNC_CODE_RSP)
    {
        GosLog(LOG_ERROR, "recv ATS info failed, unknown func code %u", pstRsp->ucFunctionCode);
        goto err;
    }

    ulTime = gos_get_current_time();

    for (i=0; i<pstRsp->usTrainNum; i++)
    {
        NTOHL_IT(pstRsp->astATSInfo[i].ulTrainUnitID);          // 车组号
        NTOHL_IT(pstRsp->astATSInfo[i].ulTrainServiceNumber);   // 服务号
        NTOHS_IT(pstRsp->astATSInfo[i].usTrainSerialNumber);    // 序列号
        NTOHS_IT(pstRsp->astATSInfo[i].usDriveNumber);          // 司机号
        NTOHS_IT(pstRsp->astATSInfo[i].usRollingStockNumber);   // 几节编组
        NTOHS_IT(pstRsp->astATSInfo[i].usStationID);            // 当前车站编号
        NTOHS_IT(pstRsp->astATSInfo[i].usUpsideStationID);      // 列车上行侧车站的编号
        NTOHS_IT(pstRsp->astATSInfo[i].usDownsideStationID);

        vATSInfo.push_back(pstRsp->astATSInfo[i]);
    }

    stATSRec.ulTime = gos_get_current_time();
    memcpy(stATSRec.aucAddr, g_aucATSAddr, sizeof(g_aucATSAddr));

    ShowRealATSRec(stATSRec);

    GosLog(LOG_INFO, "get ATS info successful");
    return;

err:
    CLOSE_SOCKET(g_stSock);
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::btnSetClick(TObject *Sender)
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
void __fastcall TFormATS::pcMainPageChanging(TObject *Sender, TcxTabSheet *NewPage,
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
void __fastcall TFormATS::sgRecRowChanging(TObject *Sender, int OldRow, int NewRow,
          bool &Allow)
{
    int  iIndex = NewRow-1;

    AnsiString  strIndex = sgRec->Cells[10][NewRow];

    iIndex = strIndex.ToIntDef(-1);

    if (iIndex < 0 || iIndex >= g_vATSRec.size())
    {
        return;
    }

    ShowATSInfo(g_vATSRec[iIndex]);
}
//---------------------------------------------------------------------------

void __fastcall TFormATS::edtFilePropertiesButtonClick(TObject *Sender, int AButtonIndex)
{
    if (OpenDialog->Execute())
    {
        edtFile->Text = OpenDialog->FileName;
        btnRefreshClick(this);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::sgRecClickCell(TObject *Sender, int ARow, int ACol)
{
    int  iIndex = -1;

    AnsiString  strIndex = sgRec->Cells[10][ARow];

    iIndex = strIndex.ToIntDef(-1);

    if (iIndex < 0 || iIndex >= g_vATSRec.size())
    {
        return;
    }

    ShowATSInfo(g_vATSRec[iIndex]);

}
//---------------------------------------------------------------------------

