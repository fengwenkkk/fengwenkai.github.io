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
#include "ATSThread.h"
#include "Train.h"
#include "Station.h"
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
#pragma link "cxRadioGroup"
#pragma link "AdvGrid"
#pragma link "AdvObj"
#pragma link "AdvUtil"
#pragma link "BaseGrid"
#pragma link "cxButtonEdit"
#pragma link "cxLabel"
#pragma link "dxBarBuiltInMenu"
#pragma link "cxSplitter"
#pragma link "cxSpinEdit"
#pragma link "cxTLData"
#pragma resource "*.dfm"

TFormATS *FormATS;

AnsiString  g_strVer = "2022.07.28";
ATSThread   *g_pATSThread = NULL;
BOOL        g_bInitData = FALSE;
GMutex      g_Mutex;
std::vector<ATS_INFO_T> g_vATSInfo;
std::map<UINT32, TcxTreeListNode*>  g_mNodeMap;

VECTOR<ATS_REC_T*>      g_vATSRec;  // 导入的ATS文件信息
UINT8                   *g_pucATSRec = NULL;

//TcxTreeListNode         *g_SelectedNode = NULL;
int                     g_iPlayIndex = -1;

VECTOR<STATION_INFO_T*>     g_vDepot;

UINT32      g_ulEndTime = 0;
BOOL        g_bInitATS = FALSE;

CHAR        *g_szVersion = "";
BOOL        g_bMDCValid = FALSE;
BOOL        g_bATSValid = FALSE;
BOOL        g_bNTPValid = FALSE;

extern AnsiString g_strSkinName;

Dict        g_DictSection;

static bool        g_bShowHex = false;
static BOOL        g_bShowTitle = TRUE;

extern bool        g_bATSUseNetOrder;

extern TColor GetDefaultSkinColor();

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

UINT8 ConvertTrainDirection(UINT8 ucDirection)
{
    if (ucDirection == TRAIN_DIRECTION_UPSIDE)
    {
        return ATS_DIRECTION_UP;
    }
    else if (ucDirection == TRAIN_DIRECTION_DOWNSIDE)
    {
        return ATS_DIRECTION_DOWN;
    }

    return ATS_DIRECTION_UNKNOWN;
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

bool IsDepotStation(UINT32 ulStationID)
{
    for (UINT32 i=0; i<g_vDepot.size(); i++)
    {
        if (g_vDepot[i]->ulStationID == ulStationID)
        {
            return true;
        }
    }

    return false;
}

void InitDictSection(UINT32 ulStationID, UINT32 ulDirection, BOOL bInDepotTrack)
{
    VECTOR<SECTION_INFO_T*> vInfo;

    Section::GetSection(ulStationID, ulDirection, bInDepotTrack, vInfo);

    g_DictSection.Clear();
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        g_DictSection.Add(vInfo[i]->ulSectionID, vInfo[i]->acSectionName);
    }
}

INT32 ToInt(Variant var)
{
    if (var.IsNull())
    {
        return -1;
    }

    AnsiString strValue = var;

    return strValue.ToIntDef(-1);
}


AnsiString GenTrainID(UINT32 ulTrainServiceNumber, CHAR *szDestID)
{
/*
    if (ulDirection == TRAIN_DIRECTION_UPSIDE)
    {
        return 37000 + ulTrainUnitID;
    }
    else
    {
        return 38000 + ulTrainUnitID;
    }   */

    return AnsiString(ulTrainServiceNumber) + szDestID;
}

AnsiString GetIDNameText(AnsiString strValue, UINT32 ulValue)
{
    return strValue + "(" + String(ulValue) + ")";
}

UINT32 GetMaxStationID()
{
    return 28;
}

UINT32 GetMinStationID()
{
    return 11;
}

UINT16 GetNextStationID(UINT32 ulStationID, UINT32 ulDirection)
{
    // 11->28
    if (ulDirection == TRAIN_DIRECTION_UPSIDE)
    {
        if (GetMaxStationID() == ulStationID)
        {
            return ulStationID;
        }

        return ulStationID+1;
    }
    // 28 -> 11
    else if (ulDirection == TRAIN_DIRECTION_DOWNSIDE)
    {
        if (GetMinStationID() == ulStationID)
        {
            return 30;
        }

        return ulStationID-1;
    }

    return INVALID_STATION_ID;
}

void __fastcall TFormATS::InitSkin()
{
    tlTrain->LookAndFeel->SkinName = g_strSkinName;
}
//---------------------------------------------------------------------------
__fastcall TFormATS::TFormATS(TComponent* Owner)
    : TSubForm(Owner)
{
    Height = 900;
    btnRefreshClick(this);

    Caption = Caption + " - " + g_strVer;
    Application->Title = Caption;

    pcMain->ActivePageIndex = 0;

    LabelInfo->Style->TextColor = clRed;

    tlTrain->Root->CheckGroupType = ncgCheckGroup;  ///显示根复选框
    tlTrain->OptionsView->CheckGroups = true;

    tlTrain->OptionsSelection->MultiSelect = true;

    btnSelectAllClick(this);
}

UINT32 GetMsgSeqID()
{
    static UINT32 ulMsgSeqID = 0;

    if (0 == ulMsgSeqID)
    {
        ulMsgSeqID = gos_get_current_time();
    }
    else
    {
        ulMsgSeqID++;
    }

    return ulMsgSeqID;
}

BOOL SyncSendServer(UINT16 usMsgID, GJsonParam *ReqJsonParam, GJson *RspJson, UINT32 ulTimeout)
{
    if (!g_pRemoteMsg)
    {
        return FALSE;
    }

    CHAR    *szReq = ReqJsonParam->GetString();
    static CHAR acRsp[64*1024];
    UINT32  ulRspLen = 0;

    memset(acRsp, 0, sizeof(acRsp));

    if (!g_pRemoteMsg->SyncSend(usMsgID, szReq, strlen(szReq)+1, acRsp, sizeof(acRsp)-1, &ulRspLen, ulTimeout))
    {
        return FALSE;
    }

    return RspJson->Parse(acRsp);
//  return TRUE;
}

/*
AnsiString SyncSendServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &MsgInfoParam, UINT32 ulTimeout)
{
    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    ReqJsonParam.Add("MsgName", szMsgName);
    ReqJsonParam.Add("MsgSeqID", GetMsgSeqID());
    ReqJsonParam.Add("MsgInfo", MsgInfoParam);

    if (!SyncSendServer(usMsgID, &ReqJsonParam, &RspJson, ulTimeout))
    {
        return "";
    }

    return RspJson.GetValue("MsgInfo");
}
*/
BOOL ParseATSInfo(GJson *pSubJson, TRAIN_INFO_T &stInfo)
{
    if (//!pSubJson->GetValue("UpdateTime", &stInfo.ulUpdateTime) ||
   //       !pSubJson->GetValue("DCUserID", &stInfo.ulDCUserID) ||
        !pSubJson->GetValue("UnitID", &stInfo.ulTrainUnitID) ||
        !pSubJson->GetValue("Type", &stInfo.ulTrainType) ||
//        !pSubJson->GetValue("TrainServiceNumber", stInfo.acTrainServiceNumber, sizeof(stInfo.acTrainServiceNumber)) ||
//        !pSubJson->GetValue("TrainSerialNumber", stInfo.acTrainSerialNumber, sizeof(stInfo.acTrainSerialNumber)) ||
        !pSubJson->GetValue("TrainID", stInfo.acTrainID, sizeof(stInfo.acTrainID)) ||
        !pSubJson->GetValue("DriverID", &stInfo.ulDriverID) ||
        !pSubJson->GetValue("ComeTime", &stInfo.ulComeTime) ||
        !pSubJson->GetValue("ParkTime", &stInfo.ulParkTime) ||
        !pSubJson->GetValue("LeaveTime", &stInfo.ulLeaveTime) ||
        !pSubJson->GetValue("DestID", stInfo.acDestID, sizeof(stInfo.acDestID)) ||
        !pSubJson->GetValue("RollingStockNum", &stInfo.ulRollingStockNum) ||
        !pSubJson->GetValue("Direction", &stInfo.ulDirection) ||
        !pSubJson->GetValue("StationID", &stInfo.ulStationID) ||
        !pSubJson->GetValue("PrevStationID", &stInfo.ulUpsideStationID) ||
        !pSubJson->GetValue("NextStationID", &stInfo.ulDownsideStationID) ||
        !pSubJson->GetValue("TurnbackTrackFlag", &stInfo.ulTurnbackTrackFlag) ||
        !pSubJson->GetValue("PlatformID", &stInfo.ulPlatformID) ||
        !pSubJson->GetValue("TransferTrackFlag", &stInfo.ulTransferTrackFlag) ||
        !pSubJson->GetValue("InOutDepot", &stInfo.ulInOutDepot) ||
        !pSubJson->GetValue("LostRealLocation", &stInfo.ulLostRealLocation) ||
        !pSubJson->GetValue("TrackName", stInfo.acTrackName, sizeof(stInfo.acTrackName)) ||
        !pSubJson->GetValue("TrainStationPos", &stInfo.iTrainStationPos) ||
        !pSubJson->GetValue("DefaultGroupID", &stInfo.i64DefaultGroupID) ||
        !pSubJson->GetValue("Register", &stInfo.bRegister) ||
        !pSubJson->GetValue("RegisterDepotID", &stInfo.ulRegisterDepotID) )
    {
        return FALSE;
    }

    if (!pSubJson->GetValue("SectionID", &stInfo.usSectionID)  )
    {
        stInfo.usSectionID = 0;
    }

    if (!pSubJson->GetValue("IndexInSection", &stInfo.ucIndexInSection) )
    {
        stInfo.ucIndexInSection = 0;
    }

    strcpy(stInfo.acTrainServiceNumber, stInfo.acTrainID);
    if (strlen(stInfo.acTrainID) > 3)
    {
        strcpy(stInfo.acTrainSerialNumber, stInfo.acTrainID+3);
        stInfo.acTrainServiceNumber[3] = '\0';
    }
    else
    {
        strcpy(stInfo.acTrainSerialNumber, "01");
    }

    return TRUE;
}

bool LoadTrainInfoFromFile(VECTOR<TRAIN_INFO_T> &vTrainInfo)
{
    CHAR    acFile[256];

    sprintf(acFile, "%s/%s", gos_get_root_path(), "ats.txt");
    int iSize = gos_get_file_size(acFile);
    if (iSize <= 0)
    {
        return false;
    }

    GJson       Json;

    if (!Json.ParseFile(acFile))
    {
        GosLog(LOG_ERROR, "parse local ATS file %s failed", acFile);
        return false;
    }

    GJson           *pSubJson;
    VECTOR<GJson*>  &vSubJson = Json.GetSubJson();
    TRAIN_INFO_T    stInfo = {0};
    VECTOR<TRAIN_INFO_T>    vInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        pSubJson = vSubJson[i];
        if (!ParseATSInfo(pSubJson, stInfo))
        {
            GosLog(LOG_ERROR, "parse train info failed on %s", pSubJson->GetText());
            return false;
        }

        //  vInfo.push_back(stInfo);
        for (UINT32 j=0; j<vTrainInfo.size(); j++)
        {
            if (vTrainInfo[j].ulTrainUnitID == stInfo.ulTrainUnitID)
            {
                stInfo.ulTrainType = vTrainInfo[j].ulTrainType;
                stInfo.i64DefaultGroupID = vTrainInfo[j].i64DefaultGroupID;

                memcpy(&vTrainInfo[j], &stInfo, sizeof(stInfo));
                break;
            }
        }
    }

    return true;
}

void __fastcall TFormATS::SaveTrainInfoToFile()
{
    VECTOR<TRAIN_INFO_T>    vTrainInfo;
//    GJsonTupleParam         JsonTupleParam;

    if (tlTrain->AbsoluteCount == 0)
    {
        return;
    }

    CHAR    acFile[256];
    FILE    *fp;

    sprintf(acFile, "%s/%s", gos_get_root_path(), "ats.txt");
    fp = fopen(acFile, "wt");
    if (!fp)
    {
        return;
    }

    fprintf(fp, "[\r\n");

    for (int i=0; i<tlTrain->AbsoluteCount; i++)
    {
        TRAIN_INFO_T    stTrain = {0};
        TcxTreeListNode *Node = tlTrain->AbsoluteItems[i];

        stTrain.ulTrainUnitID = ToInt(Node->Values[0]);// - 1400;             // 车组号
        StrCpy(stTrain.acTrainServiceNumber, Node->Texts[1]);   // 服务号
        StrCpy(stTrain.acTrainSerialNumber, Node->Texts[2]);    // 序列号
        stTrain.ulUpsideStationID = ToInt(Node->Values[3]);
        stTrain.ulStationID = ToInt(Node->Values[4]);            // 当前车站编号
        stTrain.ulDownsideStationID = ToInt(Node->Values[5]);
        stTrain.ulDirection = ToInt(Node->Values[6]);            // 运行方向：0为上行，1为下行，2未知
        StrCpy(stTrain.acDestID, Node->Texts[7]);                // 目的地号
        stTrain.ulTurnbackTrackFlag = ToInt(Node->Values[8]);  // 是否在折返站台：0不在，1在，2未知
        stTrain.ulTransferTrackFlag = ToInt(Node->Values[9]);  // 是否在转换轨，不在转换轨为0，在转换轨为1
        stTrain.ulLostRealLocation = ToInt(Node->Values[10]);     // 是否已经失去该列车的真实位置，1失去，0未失去
        stTrain.ulPlatformID = ToInt(Node->Values[11]);         // 列车所在的站台编号，如果列车不在站台时，该值0
        stTrain.ulInOutDepot = ToInt(Node->Values[12]);           // 是否进出正线（不采用该字段来判断）
        stTrain.ulDriverID = 0;//ToInt(Node->Values[13]);          // 司机号
        stTrain.usSectionID = ToInt(Node->Values[13]);
        stTrain.ucIndexInSection = ToInt(Node->Values[14]);

        sprintf(stTrain.acTrainID, "%s%s", stTrain.acTrainServiceNumber, stTrain.acTrainSerialNumber);

        GJsonParam      JsonParam;

//        JsonParam.Add("UpdateTime", stTrain.ulUpdateTime);
  //      JsonParam.Add("DCUserID", stTrain.ulDCUserID);
        JsonParam.Add("UnitID", stTrain.ulTrainUnitID);
        JsonParam.Add("Type", stTrain.ulTrainType);
        JsonParam.Add("TrainID", stTrain.acTrainID);
        JsonParam.Add("TrainServiceNumber", stTrain.acTrainServiceNumber);
        JsonParam.Add("TrainSerialNumber", stTrain.acTrainSerialNumber);
        JsonParam.Add("DriverID", stTrain.ulDriverID);
        JsonParam.Add("ComeTime", stTrain.ulComeTime);
        JsonParam.Add("ParkTime", stTrain.ulParkTime);
        JsonParam.Add("LeaveTime", stTrain.ulLeaveTime);
        JsonParam.Add("DestID", stTrain.acDestID);
        JsonParam.Add("RollingStockNum", stTrain.ulRollingStockNum);
        JsonParam.Add("Direction", stTrain.ulDirection);
        JsonParam.Add("StationID", stTrain.ulStationID);
        JsonParam.Add("PrevStationID", stTrain.ulUpsideStationID);
        JsonParam.Add("NextStationID", stTrain.ulDownsideStationID);
        JsonParam.Add("TurnbackTrackFlag", stTrain.ulTurnbackTrackFlag);
        JsonParam.Add("PlatformID", stTrain.ulPlatformID);
        JsonParam.Add("TransferTrackFlag", stTrain.ulTransferTrackFlag);
        JsonParam.Add("InOutDepot", stTrain.ulInOutDepot);
        JsonParam.Add("LostRealLocation", stTrain.ulLostRealLocation);
        JsonParam.Add("TrackName", stTrain.acTrackName);
        JsonParam.Add("TrainStationPos", stTrain.iTrainStationPos);
        JsonParam.Add("DefaultGroupID", stTrain.i64DefaultGroupID);
//      JsonParam.Add("DepotGroupID", stTrain.i64DepotGroupID);
//      JsonParam.Add("BrdGroupID", stTrain.i64BrdGroupID);
        JsonParam.Add("Register", stTrain.bRegister);
        JsonParam.Add("RegisterDepotID", stTrain.ulRegisterDepotID);
        JsonParam.Add("SectionID", stTrain.usSectionID);
        JsonParam.Add("IndexInSection", stTrain.ucIndexInSection);

//      JsonTupleParam.Add(JsonParam);

        fprintf(fp, "%s%s\r\n", i==0?"":",",JsonParam.GetString());
    }

    fprintf(fp, "]");

//    CHAR    *szTrainInfo = JsonTupleParam.GetString();
    fclose(fp);
}

//---------------------------------------------------------------------------
void __fastcall TFormATS::btnRefreshClick(TObject *Sender)
{
    static bool bLoadStation = false;
    static bool bLoadSection = false;

    if (!bLoadStation)
    {
        if (!LoadStationInfo())
        {
            GosLog(LOG_ERROR, "load station info failed");
            return;
        }

        Station::GetDepotStationInfo(g_vDepot);

        bLoadStation = true;
    }

    if (!bLoadSection)
    {
        if (!LoadSectionInfo())
        {
            GosLog(LOG_ERROR, "load section info failed");
            return;
        }

        bLoadSection = true;
    }

    if (!LoadTrainInfo())
    {
        GosLog(LOG_ERROR, "load train info failed");
        return;
    }

    if (!g_bInitATS)
    {
        VECTOR<TRAIN_INFO_T>    vTrainInfo;

        Train::GetTrainInfo(vTrainInfo);

        if (LoadTrainInfoFromFile(vTrainInfo))
        {
            Train::InitTrainInfo(vTrainInfo, 0);
        }

        g_bInitATS = TRUE;
    }

    g_bInitData = TRUE;

    tlTrain->BeginUpdate();
  //    tlTrain->Clear();
    TcxTreeListNode *Node;

    VECTOR<TRAIN_INFO_T*>   vTrainInfo = Train::GetTrainInfo();

    for (UINT32 i=0; i<vTrainInfo.size(); i++)
    {
        TRAIN_INFO_T    *pstTrain = vTrainInfo[i];

        if (g_mNodeMap.find(pstTrain->ulTrainUnitID) == g_mNodeMap.end())
        {
            Node = tlTrain->Add();
            g_mNodeMap[pstTrain->ulTrainUnitID] = Node;
        }
        else
        {
            Node = g_mNodeMap[pstTrain->ulTrainUnitID];
        }

        Node->Values[0] = pstTrain->ulTrainUnitID;              // 车组号
        Node->Values[1] = pstTrain->acTrainServiceNumber;    // 服务号
        Node->Values[2] = pstTrain->acTrainSerialNumber;    // 序列号
        Node->Values[3] = pstTrain->ulUpsideStationID;      // 上行站
        Node->Values[4] = pstTrain->ulStationID;
        Node->Values[5] = pstTrain->ulDownsideStationID;
        Node->Values[6] = pstTrain->ulDirection;
        Node->Values[7] = pstTrain->acDestID;
        Node->Values[8] = pstTrain->ulTurnbackTrackFlag;
        Node->Values[9] = pstTrain->ulTransferTrackFlag;
        Node->Values[10] = pstTrain->ulLostRealLocation;
        Node->Values[11] = pstTrain->ulPlatformID;
        Node->Values[12] = pstTrain->ulInOutDepot;
        Node->Values[13] = pstTrain->usSectionID;
        Node->Values[14] = pstTrain->ucIndexInSection;
    }

    tlTrain->EndUpdate();

    SaveTrainInfoToFile();

    ShowTrainInfo();
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::Check()
{
    if (!g_pATSThread)
    {
        g_pATSThread = new ATSThread();

        g_pATSThread->Start();
    }

    if (!g_bInitData)
    {
        btnRefreshClick(this);
    }

    SaveATSInfo();
}
//---------------------------------------------------------------------------
void GetATSInfo(VECTOR<ATS_INFO_T> &vInfo)
{
    g_Mutex.Mutex();

    vInfo.clear();
    vInfo.assign(g_vATSInfo.begin(), g_vATSInfo.end());

    g_Mutex.Unmutex();
}

#define CVT_STR(v, ac, len)     CvtStr(v, ac, sizeof(ac), len)

BOOL CvtStr(System::Variant v, CHAR *sz, UINT32 ulMaxLen, UINT8 &ucLen)
{
    AnsiString  str = v;

    memset(sz, 0, ulMaxLen);
    ucLen = str.Length();
    if (ucLen > ulMaxLen)
    {
        return FALSE;
    }

    memcpy(sz, str.c_str(), ucLen);
    /*
    if (ucLen < ulMaxLen)
    {
        memset(sz, '0', ulMaxLen-ucLen);
    }

    memcpy(sz+ulMaxLen-ucLen, str.c_str(), ucLen);

    ucLen = ulMaxLen;           */

    return TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::PlayATSRec()
{
    int  iIndex = -1;

    if (tlRec->AbsoluteCount == 0)
    {
        return;
    }

    if (g_iPlayIndex < 0)
    {
        if (tlRec->SelectionCount > 0)
        {
            g_iPlayIndex = tlRec->Selections[0]->AbsoluteIndex;
        }
        else
        {
            g_iPlayIndex = 0;
        }
    }

    TcxTreeListNode     *Node = tlRec->AbsoluteItems[g_iPlayIndex];

    Node->Selected = true;
    Node->MakeVisible();

    if (m_iPlayPeriod > 0)
    {
        g_iPlayIndex++;
        if (g_iPlayIndex >= tlRec->AbsoluteCount)
        {
            g_iPlayIndex = 0;
        }
    }

    iIndex = (int)Node->Values[9];
	if (iIndex < 0 || iIndex >= g_vATSRec.size())
	{
		return;
	}

    g_Mutex.Mutex();

    g_vATSInfo.clear();

    ATS_REC_T   *pstRec = g_vATSRec[iIndex];

    for (UINT32 i=0; i<pstRec->stRecData.ucTrainNum; i++)
    {
		g_vATSInfo.push_back(pstRec->stRecData.astATSInfo[i]);
	}

    g_Mutex.Unmutex();
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::SaveATSInfo()
{
    if (!g_bInitATS)
    {
        return;
    }

    if (pcMain->ActivePage != tsATSData)
    {
        if (cbPlayRec->Checked)
        {
            PlayATSRec();
        }

        return;
    }

    std::vector<ATS_INFO_T> vATSInfo;
    TcxTreeListNode     *Node = tlTrain->Root->getFirstChild();

    for(;Node; Node=Node->getNextSibling())
    {
        ATS_INFO_T      stATS = {0};
        INT32           iTrainUnitID = Node->Values[0];// - 1400;

   /*     if (iTrainUnitID < 0)
        {
            Node->Values[0] = 1400;
            continue;
        }      */

        if (!Node->Checked)
        {
            continue;
        }

        System::Variant vTrainUnitID = iTrainUnitID;

        CVT_STR(vTrainUnitID, stATS.acTrainUnitID, stATS.ucTrainUnitIDLen);
        CVT_STR(Node->Values[1], stATS.acServiceNumber, stATS.ucServiceNumberLen);
        CVT_STR(Node->Values[2], stATS.acOrderNumber, stATS.ucOrderNumberLen);
        CVT_STR(Node->Values[7], stATS.acDestID, stATS.ucDestIDLen);

        stATS.usUpsideStationID = ToInt(Node->Values[3]);      // 列车上行侧车站的编号
        stATS.usStationID = ToInt(Node->Values[4]);            // 当前车站编号
        stATS.usDownsideStationID = ToInt(Node->Values[5]);    // 列车下行侧车站的编号
        stATS.ucDirection = ToInt(Node->Values[6]);            // 运行方向：0为上行，1为下行，2未知
        stATS.ucOnTurnBackTrackFlag = ToInt(Node->Values[8]);  // 是否在折返站台：0不在，1在，2未知
        stATS.ucOnTransferTrackFlag = ToInt(Node->Values[9]);  // 是否在转换轨，不在转换轨为0，在转换轨为1
        stATS.ucLostRealLocation = ToInt(Node->Values[10]);     // 是否已经失去该列车的真实位置，1失去，0未失去
        stATS.ucIsOnPlatform = ToInt(Node->Values[11]);         // 列车是否在站台，如果列车不在站台时，该值0
        stATS.ucInOutDepot = ToInt(Node->Values[12]);           // 是否进出正线（不采用该字段来判断）
        stATS.usDriverNumber = 0;//ToInt(Node->Values[13]);          // 司机号
        stATS.usPhysicSectionID = ToInt(Node->Values[13]);          // 司机号
        stATS.ucIndexInSection = ToInt(Node->Values[14]);          // 司机号

        stATS.ucDirection = ConvertTrainDirection(stATS.ucDirection);

        vATSInfo.push_back(stATS);
    }

    g_Mutex.Mutex();

    vATSInfo.swap(g_vATSInfo);

    g_Mutex.Unmutex();
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::tlTrainFocusedNodeChanged(TcxCustomTreeList *Sender, TcxTreeListNode *APrevFocusedNode,
          TcxTreeListNode *AFocusedNode)
{
    ShowTrainInfo();
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::UpdateTrainInfo()
{
    g_ulEndTime = gos_get_uptime() + 5;

    SaveTrainInfoToFile();

    Check();

    TimerRefreshTimer(this);

    ShowTrainInfo();
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::ShowTrainInfo()
{
    if (tlTrain->SelectionCount == 0)
    {
        return ;
    }

    TcxTreeListNode     *Node = tlTrain->Selections[0];
    UINT32              ulStationID;
    UINT32              ulDirection;
    UINT32              ulInDepotTrack;
    UINT32              ulSectionID;

    ulStationID = Node->Values[4];
    edtCurrStation->Text = ulStationID;//Node->Values[4];

    edtTrainUnitID->Text = Node->Texts[0];
    edtServiceNumber->Text = Node->Texts[1];
    edtDestID->Text = Node->Texts[7];

    ulDirection = Node->Values[6];
    if (ulDirection == TRAIN_DIRECTION_UPSIDE)
    {
        rbUpSide->Checked = true;
    }
    else
    {
        rbDownSide->Checked = true;
    }

    edtOrderNumber->Text = Node->Texts[2];

    AnsiString  strServiceNumber = edtServiceNumber->Text;
    AnsiString  strOrderNumber = edtOrderNumber->Text;

//    strServiceNumber = strServiceNumber.SubString(strServiceNumber.Length()-3+1, 3);
//    strOrderNumber = strOrderNumber.SubString(strOrderNumber.Length()-4+1, 4);
    edtTrainID->Text = strServiceNumber + strOrderNumber;

    UINT32  ulTransferFlag = Node->Values[9];
    UINT32  ulTurnbackFlag = Node->Values[8];

    cbTransferTrack->Checked = ulTransferFlag;
    cbTurnbackTrack->Checked = ulTurnbackFlag;

    ulSectionID = Node->Values[13];

    ulInDepotTrack = ulTurnbackFlag;
    InitDictSection(ulStationID, ulDirection, ulInDepotTrack);
    g_DictSection.Init(cbSection);

    g_DictSection.SetID(cbSection, ulSectionID);

    edtIndexInSection->Text = Node->Texts[14];

    if (btnDepart->Enabled)
    {
        btnDepart->BringToFront();
    }
    else if (btnDeposit->Enabled)
    {
        btnDeposit->BringToFront();
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormATS::btnDepartClick(TObject *Sender)
{
    if (tlTrain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlTrain->Selections[0];

    UINT32  ulStationID = 30; // 航头定修段;
    UINT32  ulDirection = TRAIN_DIRECTION_UPSIDE;
    CHAR    acDestID[6] = "001";

    Node->Values[3] = ulStationID;
    Node->Values[4] = ulStationID;
    Node->Values[5] = ulStationID;
    Node->Values[6] = ulDirection;
    Node->Values[7] = acDestID;

    Node->Values[12] = 0;

    UpdateTrainInfo();
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::btnTransferTrackClick(TObject *Sender)
{
    if (tlTrain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlTrain->Selections[0];


    UINT32  ulStationID = Node->Values[4];  //93; // 航头定修段;
    UINT32  ulDirection = Node->Values[6];

    Node->Values[3] = ulStationID;
    Node->Values[4] = ulStationID;
    Node->Values[5] = ulStationID;

    UpdateTrainInfo();
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::btnNextStationClick(TObject *Sender)
{
    if (tlTrain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlTrain->Selections[0];

    UINT32  ulNextStationID = Node->Values[5];
    UINT32  ulStationID = Node->Values[4];
    UINT32  ulPrevStationID = Node->Values[3];
    UINT32  ulDirection = Node->Values[6];

    if (ulDirection == TRAIN_DIRECTION_DOWNSIDE)
    {
        if (ulStationID == GetMinStationID())
        {
            ulStationID = 30;
            ulPrevStationID = ulStationID;
            ulNextStationID = ulStationID;
        }
        else
        {
            if (ulStationID != ulPrevStationID)
            {
                ulPrevStationID = ulStationID;
            }
            else
            {
                ulStationID--;
                ulPrevStationID = ulStationID;
                ulNextStationID = ulStationID;
            }
        }
    }
    else if (ulDirection == TRAIN_DIRECTION_UPSIDE)
    {
        if (ulStationID == GetMaxStationID())
        {
            ulDirection = TRAIN_DIRECTION_DOWNSIDE;
        }
        else
        {
            if (ulStationID != ulNextStationID)
            {
                ulNextStationID = ulStationID;
            }
            else
            {
                ulStationID++;
                ulPrevStationID = ulStationID;
                ulNextStationID = ulStationID;
            }
        }
    }
    else
    {
        return;
    }

    Node->Values[5] = ulNextStationID;
    Node->Values[4] = ulStationID;
    Node->Values[3] = ulPrevStationID;
    Node->Values[6] = ulDirection;

    UpdateTrainInfo();
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::btnTurnBackClick(TObject *Sender)
{
    if (tlTrain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlTrain->Selections[0];
    UINT32  ulTrainUnitID = Node->Values[0];

//  Node->Values[2] = GenTrainID(TRAIN_DIRECTION_UPSIDE, ulTrainUnitID);
    Node->Values[8] = 1;
    Node->Values[6] = TRAIN_DIRECTION_DOWNSIDE;
    Node->Values[9] = "01";

    UpdateTrainInfo();
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::btnDepositClick(TObject *Sender)
{
    if (tlTrain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlTrain->Selections[0];

    UINT32  ulStationID = Node->Values[4];
    UINT32  ulDirection = Node->Values[6];

    if (ulStationID != 29 ||
        ulStationID != 30 )
    {
        return;
    }

    Node->Values[11] = 0;
    Node->Values[12] = 0;

    UpdateTrainInfo();
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::TimerRefreshTimer(TObject *Sender)
{
    if (gos_get_uptime() > g_ulEndTime)
    {
        btnRefresh->Enabled = true;
    }
    else
    {
        btnRefresh->Enabled = false;
    }

    return;
    btnRefreshClick(this);
    TimerRefresh->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::ColumnStationGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    return;

    if (ANode->Values[4].IsEmpty())
    {
        return;
    }

    UINT32  ulStationID = ANode->Values[4];

    Value = GetIDNameText(Station::GetStationName(ulStationID ), ulStationID);
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::ColumnDirectionGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    if (ANode->Values[6].IsEmpty())
    {
        return;
    }

    UINT32  ulDirection = ANode->Values[6];

    //Value = ulDirection;
    if (ulDirection == TRAIN_DIRECTION_UPSIDE)
    {
        Value = "上行";
    }
    else if (ulDirection == TRAIN_DIRECTION_DOWNSIDE)
    {
        Value = "下行";
    }
    else
    {
        Value = "未知";
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormATS::ColumnTransferTrackFlagGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    if (ANode->Values[9].IsEmpty())
    {
        return;
    }

    UINT32  ulFlag = ANode->Values[9];

    Value = ulFlag==0?"否":"是";
}
//---------------------------------------------------------------------------

void __fastcall TFormATS::ColumnTurnbackTrackFlagGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
          UnicodeString &Value)
{
    if (ANode->Values[8].IsEmpty())
    {
        return;
    }

    UINT32  ulFlag = ANode->Values[8];

    Value = ulFlag==0?"否":"是";
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::btnInitClick(TObject *Sender)
{
    if (tlTrain->SelectionCount == 0)
    {
        InfoMsg("请选择需初始化的记录");
        return;
    }

    TcxTreeListNode     *Node = tlTrain->Selections[0];

    UINT32  ulTrainUnitID = Node->Values[0];
    UINT32  ulStationID;
    CHAR    acDestID[8];
    UINT32  ulDirection;
    CHAR    acServiceNumber[16];
    UINT32  ulTransferFlag;
    UINT32  ulTurnbackFlag;
    CHAR    acTrainOrderNumber[16];
    AnsiString  strValue;

    if (!ToInt(edtCurrStation->Text, &ulStationID))
    {
        InfoMsg("当前站取值非法，只支持整数");
        return;
    }

    strValue = edtServiceNumber->Text.Trim();
    if (strValue.Length() < 1 ||
        strValue.Length() > 3 )
    {
        InfoMsg("服务号取值非法，1-3位字符编码");
        return;
    }

    StrCpy(acServiceNumber, strValue);

    strValue = edtOrderNumber->Text.Trim();
    if (strValue.Length() < 2 ||
        strValue.Length() > 4 )
    {
        InfoMsg("序列号取值非法，2-4位字符编码");
        return;
    }

    StrCpy(acTrainOrderNumber, strValue);

    strValue = edtDestID->Text.Trim();
    if (strValue.Length() == 0 ||
        strValue.Length() > 3)
    {
        InfoMsg("目的地取值非法，最多3位字符编码");
        return;
    }

    if (strValue.Length() == 1)
    {
        strValue = "0" + strValue;
    }

    StrCpy(acDestID, strValue);

    if (rbUpSide->Checked)
    {
        ulDirection = TRAIN_DIRECTION_UPSIDE;
    }
    else
    {
        ulDirection = TRAIN_DIRECTION_DOWNSIDE;
    }

    ulTransferFlag = cbTransferTrack->Checked;
    ulTurnbackFlag = cbTurnbackTrack->Checked;

    UINT32  ulSectionID;
    UINT32  ulIndexInSection;

    if (!g_DictSection.GetID(&ulSectionID))
    {
        InfoMsg("物理区非法");
        return;
    }

    if (!ToInt(edtIndexInSection->Text, &ulIndexInSection) )
    {
        InfoMsg("区内序号取值非法");
        return;
    }

    Node->Values[1] = acServiceNumber;
    Node->Values[2] = acTrainOrderNumber;
    Node->Values[3] = ulStationID;
    Node->Values[4] = ulStationID;
    Node->Values[5] = ulStationID;
    Node->Values[6] = ulDirection;
    Node->Texts[7] = acDestID;
    Node->Values[8] = ulTurnbackFlag;
    Node->Values[9] = ulTransferFlag;
    Node->Values[13] = ulSectionID;
    Node->Values[14] = ulIndexInSection;

//    SaveTrainInfoToFile();
    UpdateTrainInfo();
}
//---------------------------------------------------------------------------
// 上行curr/up/down: 545 驶入 555 停靠 556 驶出, up是上一站
// 下行curr/up/down: 556 驶入 555 停靠 545 驶出，up是下一站
void __fastcall TFormATS::btnLeaveStationClick(TObject *Sender)
{
    if (tlTrain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlTrain->Selections[0];

    UINT32  ulStationID = Node->Values[4];
    UINT32  ulDirection = Node->Values[6];
    UINT32  ulUpsideStation = ulStationID;
    UINT32  ulDownsideStation = ulStationID;

    if (ulDirection == TRAIN_DIRECTION_UPSIDE)
    {
        if (ulUpsideStation == GetMaxStationID())
        {
            ulDownsideStation = 29;
        }
        else
        {
            ulDownsideStation ++;
        }
    }
    else if (ulDirection == TRAIN_DIRECTION_DOWNSIDE)
    {
        if (ulUpsideStation == GetMinStationID())
        {
            ulUpsideStation = 30;
        }
        else
        {
            ulUpsideStation--;
        }
    }
    else
    {
        return;
    }

    Node->Values[3] = ulUpsideStation;
    Node->Values[4] = ulStationID;
    Node->Values[5] = ulDownsideStation;

    UpdateTrainInfo();
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::btnEnterStationClick(TObject *Sender)
{
    if (tlTrain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlTrain->Selections[0];

    UINT32  ulStationID = Node->Values[4];
    UINT32  ulDirection = Node->Values[6];
    UINT32  ulUpsideStation = Node->Values[3];
    UINT32  ulDownsideStation = Node->Values[5];

    if (ulDirection == TRAIN_DIRECTION_UPSIDE)      // 445 -> 545
    {
        ulStationID = ulDownsideStation;
    }
    else if (ulDirection == TRAIN_DIRECTION_DOWNSIDE)   // 656 - > 556
    {
        ulStationID = ulUpsideStation;
    }
    else
    {
        return;
    }

    Node->Values[3] = ulUpsideStation;
    Node->Values[4] = ulStationID;
    Node->Values[5] = ulDownsideStation;

    UpdateTrainInfo();
}
//---------------------------------------------------------------------------

void __fastcall TFormATS::btnLeaveTransferTrackClick(TObject *Sender)
{
    if (tlTrain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlTrain->Selections[0];

    UINT32  ulStationID = Node->Values[4];  //93; // 航头定修段;
    UINT32  ulDirection = Node->Values[6];

    if (ulDirection == TRAIN_DIRECTION_UPSIDE)
    {
        ulStationID = 1;

        Node->Values[9] = 0;
    }
    else if (ulDirection == TRAIN_DIRECTION_DOWNSIDE)
    {
        ulStationID = 93;

        Node->Values[9] = 0;
    }
    else
    {
        return;
    }

    Node->Values[3] = ulStationID;
    Node->Values[4] = ulStationID;
    Node->Values[5] = ulStationID;

    UpdateTrainInfo();
}
//---------------------------------------------------------------------------

void __fastcall TFormATS::ColumnSectionIDGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[13].IsNull())
    {
        return;
    }

    UINT32  ulValue = ANode->Values[13];

    SECTION_INFO_T   *pstSection = Section::GetSection(ulValue);

    Value = ulValue;
    if (pstSection)
    {
        Value += String(" - ") + pstSection->acSectionName;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::cbTurnbackTrackPropertiesEditValueChanged(TObject *Sender)
{
    UINT32  ulStationID;
    UINT32  ulDirection;
    UINT32  ulInDepotTrack;

    if (!ToInt(edtCurrStation->Text, &ulStationID))
    {
        goto err;
    }

    if (rbUpSide->Checked)
    {
        ulDirection = TRAIN_DIRECTION_UPSIDE;
    }
    else
    {
        ulDirection = TRAIN_DIRECTION_DOWNSIDE;
    }

    ulInDepotTrack = cbTurnbackTrack->Checked;

    InitDictSection(ulStationID, ulDirection, ulInDepotTrack);
    g_DictSection.Init(cbSection);

    return;

err:
    g_DictSection.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::edtCurrStationPropertiesEditValueChanged(TObject *Sender)
{
    cbTurnbackTrackPropertiesEditValueChanged(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::rbUpSideClick(TObject *Sender)
{
    cbTurnbackTrackPropertiesEditValueChanged(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::edtFilePropertiesButtonClick(TObject *Sender, int AButtonIndex)
{
    if (OpenDialog->Execute())
    {
        edtFile->Text = OpenDialog->FileName;
        btnLoadFileClick(this);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::btnLoadFileClick(TObject *Sender)
{
    LabelInfo->Visible = false;

    g_iPlayIndex = -1;

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

    tlRec->Visible = false;
    tlRec->Clear();
    tlRec->BeginUpdate();

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

    tlRec->EndUpdate();
    tlRec->Visible = true;

    if (tlRec->AbsoluteCount > 0)
    {
        tlRec->AbsoluteItems[0]->Selected = true;
        tlRecClick(this);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::ShowATSRec(ATS_REC_T *pstRec, int iIndex)
{
    CHAR    acTime[32];
    int     r;

    gos_get_text_time_ex(&pstRec->ulTime, NULL, acTime);

    TcxTreeListNode *Node = tlRec->Add();

    Node->Values[0] = tlRec->AbsoluteCount;
    Node->Values[1] = acTime+11;
    Node->Values[2] = GetIPText(pstRec->aucAddr);
    Node->Values[3] = pstRec->stRecData.ucTrainNum;
    Node->Values[4] = pstRec->stRecData.stMsgHdr.usMsgLen;
    Node->Values[5] = GetSimpleTime(pstRec->stRecData.stMsgHdr.stMsgTime);
    Node->Values[6] = pstRec->stRecData.stMsgHdr.usLineID;
    Node->Values[7] = pstRec->stRecData.stMsgHdr.ucMsgID;
    Node->Values[8] = pstRec->stRecData.stMsgHdr.ucVer;
    Node->Values[9] = iIndex;
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::ShowATSInfo(ATS_REC_T *pstRec)
{
    tlATS->Clear();

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
        Node->Values[19] = pstATS->ucDriveMode;
	}

    tlATS->EndUpdate();
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

    if (iCol == 5 || iCol == 7 || iCol == 8 || iCol == 9 )
    {
        UINT32  ulValue = (UINT32)ANode->Values[iCol];

		Value = GetIntValue((UINT16)ulValue);
    }
    else if (iCol == 6 || iCol == 10 || iCol == 11 || iCol == 12 ||
			 iCol == 13 || iCol == 14 || iCol == 15 ||
			 iCol == 16 || iCol == 17 || iCol == 18 || iCol == 19)
	{
        UINT32  ulValue = (UINT32)ANode->Values[iCol];

        Value = GetIntValue((UINT8)ulValue);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::cbShowHexPropertiesChange(TObject *Sender)
{
	g_bShowHex = cbShowHex->Checked;

    tlATS->BeginUpdate();
    tlATS->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::TimerSaveATSRecTimer(TObject *Sender)
{
    m_iPlayPeriod = edtPlayPeriod->Text.ToIntDef(-1);

    if (m_iPlayPeriod > 0)
    {
        TimerSaveATSRec->Interval = m_iPlayPeriod*1000;
    }

    Check();
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::cbPlayRecPropertiesEditValueChanged(TObject *Sender)
{
    edtPlayPeriod->Enabled = cbPlayRec->Checked;
    if (!cbPlayRec->Checked)
    {
        g_iPlayIndex = -1;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::edtPlayPeriodPropertiesEditValueChanged(TObject *Sender)
{
    m_iPlayPeriod = edtPlayPeriod->Text.ToIntDef(-1);
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::tlRecFocusedNodeChanged(TcxCustomTreeList *Sender, TcxTreeListNode *APrevFocusedNode,
          TcxTreeListNode *AFocusedNode)
{
    tlRecClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::tlRecClick(TObject *Sender)
{
    if (tlRec->SelectionCount == 0)
    {
        return;
    }

    int  iIndex = (int)tlRec->Selections[0]->Values[9];

    if (iIndex < 0 || iIndex >= g_vATSRec.size())
    {
        return;
    }

    ShowATSInfo(g_vATSRec[iIndex]);
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::btnSelectAllClick(TObject *Sender)
{
    for (int i=0; i<tlTrain->AbsoluteCount; i++)
    {
        tlTrain->AbsoluteItems[i]->Checked = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormATS::btnUnSelectAllClick(TObject *Sender)
{
    for (int i=0; i<tlTrain->AbsoluteCount; i++)
    {
        tlTrain->AbsoluteItems[i]->Checked = false;
    }
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
    else if (iCol == 19) //车辆段进入转换轨时值为1,正线进入转换轨为2
	{
		ulValue = Node->Values[iCol];
		if (ulValue == ATS_DRIVE_MODE_AM)
        {
            AText = "AM";
        }
        else if (ulValue == ATS_DRIVE_MODE_CM)
        {
            AText = "CM";
        }
        else if (ulValue == ATS_DRIVE_MODE_RM)
        {
            AText = "RM";
        }
        else if (ulValue == ATS_DRIVE_MODE_EUM)
        {
            AText = "EUM";
        }
        else if (ulValue == ATS_DRIVE_MODE_FAM)
        {
            AText = "FAM";
        }
        else if (ulValue == ATS_DRIVE_MODE_CAM)
        {
            AText = "CAM";
        }
        else if (ulValue == ATS_DRIVE_MODE_RRM)
        {
            AText = "RRM";
        }
        else if (ulValue == ATS_DRIVE_MODE_NULL)
        {
            AText = "";
        }
	}

    ANeedShow = !AText.IsEmpty();
}
//---------------------------------------------------------------------------


