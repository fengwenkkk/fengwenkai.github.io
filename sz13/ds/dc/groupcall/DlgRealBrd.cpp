//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "UICfg.h"
#include "DlgRealBrd.h"
#include "AudioDev.h"
#include "MicCapture.h"
#include "WavRec.h"
#include "AudioVolume.h"
#include "pis.h"
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxEdit"
#pragma link "cxGraphics"
#pragma link "cxLabel"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "cxTrackBar"
#pragma link "dxSkinBlue"
#pragma link "dxSkinGlassOceans"
#pragma link "dxSkinOffice2007Pink"
#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinSharp"
#pragma link "cxTextEdit"
#pragma link "cxProgressBar"
#pragma link "dxSkinBlueprint"
#pragma link "dxSkinDevExpressDarkStyle"
#pragma link "dxSkinSharpPlus"
#pragma link "dxSkinVisualStudio2013Dark"
#pragma link "dxBevel"
#pragma link "cxCustomData"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxInplaceContainer"
#pragma link "cxStyles"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "cxDropDownEdit"
#pragma link "cxMaskEdit"
#pragma link "cxCheckBox"
#pragma link "cxImageComboBox"
#pragma link "cxImageList"
#pragma resource "*.dfm"
TDialogRealBrd *DialogRealBrd;

static ID_VALUE_T   s_astBrdType[] = {
                                {SINGLE_TRAIN,  "单列列车"},
                                {MULTI_TRAIN,   "多列列车"},
                                {MAINLINE_TRAIN,"正线列车"},
                                {UPLINE_TRAIN,  "上行列车"},
                                {DOWNLINE_TRAIN,"下行列车"},
                                {INTERLOCK_STATION_TRAIN,   "连锁区列车"},
                                {STATION_TRAIN,             "站管区列车"},
};

static Dict g_DictBrdType(s_astBrdType, ARRAY_SIZE(s_astBrdType));

extern MicCapture      *g_MicCapture;

static UINT32 g_ulBrdSeqID = 0;

static VECTOR<TRAIN_BRD_INFO_T> g_vBrdTrain;

static SOCKET g_SockSAPcmData = INVALID_SOCKET;

static UINT8  g_aucSAServerIP[4] = {0, 0, 0, 0};
static UINT16 g_usSAServerPort = SA_SERVER_PORT;
static UINT32 g_ulPCMPkgIndex = 0;

static UINT16 g_usPCMCaptureSampleRate = 16000;
static UINT8  g_ucPCMCaptureBitsPerSample = 16;
static UINT8  g_ucPCMCaptureChanNum = 1;

static WavRec g_PAWavRec;

static INT64  g_i64CurrBrdGroupID = INVALID_GROUP_ID;  // 当前广播组

//static BOOL     g_bUsePCMDataBuf = TRUE;
static UINT32   g_ulFirstSendPCMTime = 0;
static UINT32   g_ulSendPCMNum = 0;
static PCMDataBuffer  *m_pPCMDataBuf = NULL;

extern VOID ClearMainGroupAndSpeaker();


PCMDataBuffer::PCMDataBuffer(UINT32 ulMaxBufSize)
{
    m_pucDataBuf = NULL;
    m_ulCurrDataLen = 0;
    m_ulMaxBufSize = ulMaxBufSize;

    if (m_ulMaxBufSize < 8000)
    {
        m_ulMaxBufSize = 8000;
    }

    m_pucDataBuf = (UINT8*)calloc(1, m_ulMaxBufSize);
}

VOID PCMDataBuffer::Init()
{
    m_ulCurrDataLen = 0;
    m_i64StartTime = 0;
    m_i64LastSendTime = 0;
}

VOID PCMDataBuffer::PushData(UINT8 *pucData, UINT32 ulLen)
{
    AUTO_LOCK(m_Mutex);

    UINT32          ulBufLen = m_ulMaxBufSize - m_ulCurrDataLen;    // 剩余缓冲区大小

    if (ulLen > ulBufLen)
    {
        GosLog(LOG_ERROR, "Brd PCMDataBuffer data buf is overflow, %u bytes are discarded", ulLen-ulBufLen);
        ulLen = ulBufLen;
    }

    memcpy(m_pucDataBuf+m_ulCurrDataLen, pucData, ulLen);
    m_ulCurrDataLen += ulLen;

    GosLog(LOG_DETAIL, "Brd PCMDataBuffer push data %u, total is %u", ulLen, m_ulCurrDataLen);
}

UINT32 PCMDataBuffer::GetDataLen()
{
    return m_ulCurrDataLen;
}

/**
 * @brief           获取缓冲区PCM数据
 * @param           pucData    [out] 缓冲区保存的PCM数据
 * @param           ulMaxLen    [in] 期望获取的数据的最大长度
 * @param           ulLen    [out] 实际返回的数据长度
 * @return          VOID
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *
 * @endcode
 * @note
 */
VOID PCMDataBuffer::GetData(UINT8 *pucData, UINT32 ulMaxLen, UINT32 &ulLen)
{
    AUTO_LOCK(m_Mutex);

    if (m_ulCurrDataLen > ulMaxLen)
    {
        ulLen = ulMaxLen;
    }
    else
    {
        ulLen = m_ulCurrDataLen;
    }

    memcpy(pucData, m_pucDataBuf, ulLen);
    m_ulCurrDataLen -= ulLen;

    if (m_ulCurrDataLen > 0)
    {
        memmove(m_pucDataBuf, m_pucDataBuf+ulLen, m_ulCurrDataLen);
    }

    GosLog(LOG_DETAIL, "pop data %u, current is %u", ulLen, m_ulCurrDataLen);
}

static VOID GetPAAudioOption()
{
    g_usPCMCaptureSampleRate = GetPAAudioSampleRate();
    g_ucPCMCaptureBitsPerSample = GetPAAudioBitsPerSample();

    return ;
}

INT64 GetCurrBrdGroupID()
{
    return g_i64CurrBrdGroupID;
}

AnsiString GetBrdType(UINT32 ulBrdType)
{
    return g_DictBrdType.GetValue(ulBrdType);
}

/**
 * @brief           输出一个录音文件保存路径，若该路径不存在则创建它并输出
 * @param           szFile    [out] 输出录音文件的 file path
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *
 * @endcode
 * @note
 * 录音文件保存的根路径为DC.exe运行路径的\\data\\pa_rec （临时文件为pa_tmp)
 */
static void GetPAAudioRecFileName(CHAR *szFile)
{
    CHAR    acTime[32];
    CHAR    acDir[256];

    // 文件先保持在临时目录下
    LocalRec::GetPARecTmpDir(acDir);

    gos_get_text_time_ex(NULL, "%u%02u%02u_%02u%02u%02u", acTime);

    sprintf(szFile, "%s\\%u", acDir, APP::GetCurrDCUserID());
    if (!gos_file_exist(szFile))
    {
        gos_create_redir(szFile);
    }

    // 第一级为调度台id、第二为文件， 1001_time.wav  (DC_时间）
    sprintf(szFile + strlen(szFile), "\\%u_%s.wav", APP::GetCurrDCUserID(), acTime);
}

//----------------------------MIC Data Handle--------------------
static VOID SendPCMDataToDIS(UINT8 *pucBuf, UINT32 ulBufSize)
{
    CHAR            *szPcmData = NULL;
    UINT32          ulPcmDataMAXLen = GetPAMaxPkgSize();
    UINT32          ulMsgHdrLen = sizeof(PA_MSG_HDR_T)+STRUCT_OFFSET(PA_PCM_DATA_T, aucPCMData);
    UINT32          ulPcmPkgSize = ulPcmDataMAXLen + ulMsgHdrLen;
    UINT32          ulPcmDataPkgSize = (2*ulPcmPkgSize)+1;

    GJsonTupleParam     TupleParam;
    GJsonParam          Param;

    for (UINT32 i=0; i<g_vBrdTrain.size(); i++)
    {
        GJsonParam          SubParam;

        SubParam.Add("TrainUnitID", g_vBrdTrain[i].stTrain.ulTrainUnitID);
        TupleParam.Add(SubParam);
    }

    if (TupleParam.GetTupleNum() == 0)
    {
        GosLog(LOG_ERROR, "real brd TupleParam size is 0");
        return;
    }

    szPcmData = (CHAR *)calloc(1, ulPcmDataPkgSize);
    if (!szPcmData)
    {
        GosLog(LOG_INFO, "szPcmData calloc error");
        return;
    }

    while(ulBufSize > 0)
    {
        Param.Clear();

        Param.Add("Train", TupleParam);

        UINT32  ulCurrPcmDataLen = ulBufSize;

        if (ulCurrPcmDataLen > ulPcmDataMAXLen)
        {
            ulCurrPcmDataLen = ulPcmDataMAXLen;
        }

        ulBufSize -= ulCurrPcmDataLen;

        gos_byte_to_hexstr(pucBuf, ulCurrPcmDataLen, szPcmData);
        pucBuf += ulCurrPcmDataLen;

        GosLog(LOG_INFO, "send BRD PCM to dis, datalen is:%d", strlen(szPcmData));

        Param.Add("BrdSeqID", g_ulBrdSeqID);
        Param.Add("DataIndex", g_ulPCMPkgIndex++);
        Param.Add("SampleRate", g_usPCMCaptureSampleRate);
        Param.Add("ChannelNum", g_ucPCMCaptureChanNum);
        Param.Add("SampleBits", g_ucPCMCaptureBitsPerSample);
        Param.Add("PCMData", szPcmData);

        SendServer(EV_DC_DIS_SA_REQ, MSG_DCSendRealBrdAudioData, g_ulPCMPkgIndex-1, Param);
    }

    free(szPcmData);
}

static VOID OnMicData(UINT8 *pucBuf, UINT32 ulBufSize)
{
    if (DirectSendPAAudio())
    {
        // 不支持直接发送给PA
        GosLog(LOG_ERROR, "direct send PA audio failed");
       // SendPCMDataToPA(pucBuf, ulBufSize);
    }
    else
    {
        if (m_pPCMDataBuf)
        {
            m_pPCMDataBuf->PushData(pucBuf, ulBufSize);
        }
        else
        {
            SendPCMDataToDIS(pucBuf, ulBufSize);
        }
    }

    g_PAWavRec.Rec(TRUE, pucBuf, ulBufSize);

    GosLog(LOG_INFO, "Send Brd PCM data %u", ulBufSize);
}
//----------------------------MIC Data Handle End---------------------

//----------------------------MIC Handle------------------------------

static VOID CloseMic()
{
    if (g_MicCapture)
    {
        delete g_MicCapture;
        g_MicCapture = NULL;
    }
}

static BOOL OpenMic()
{
    CloseMic();

    UINT32 ulPAMaxPkgSize = GetPAMaxPkgSize();  // 1000

    g_MicCapture = new MicCapture(OnMicData); //注册麦克抓取到的数据处理函数
    if (!g_MicCapture->Open(g_usPCMCaptureSampleRate, g_ucPCMCaptureBitsPerSample,
                            g_ucPCMCaptureChanNum, ulPAMaxPkgSize))
    {
        CloseMic();
        return FALSE;
    }

    return TRUE;
}

static BOOL StartGrabAudio()
{
    if (!g_MicCapture)
    {
        GosLog(LOG_INFO, "start grab mic data failed, can't open mic");
        return FALSE;
    }

    g_MicCapture->Start();

    GosLog(LOG_INFO, "start grab mic data");

    return TRUE;
}

static VOID PauseGrabAudio()
{
    if (g_MicCapture)
    {
        g_MicCapture->Stop();
    }

    GosLog(LOG_INFO, "pause grab mic data");
}

static VOID StopGrabAudio()
{
    if (g_MicCapture)
    {
        g_MicCapture->Stop();
    }

    CloseMic();

    GosLog(LOG_INFO, "stop grab mic data");
}
//----------------------------MIC Handle End---------------------------------

//---------------------------------------------------------------------------
BOOL InterLockStationRealBrd(UINT32 ulStationID)
{
    if (ulStationID == INVALID_STATION_ID)
    {
        return false;
    }

    VECTOR<TRAIN_INFO_T *> vTrain;

    Train::GetTrainOfInterLockStation(ulStationID, vTrain);

    BOOL bRet = DialogRealBrd->Brd(INTERLOCK_STATION_TRAIN, vTrain);

    if (bRet)
    {
        DialogRealBrd->ShowModal();
    }

    return bRet;
}

//---------------------------------------------------------------------------
BOOL StationRealBrd(UINT32 ulStationID)
{
    if (ulStationID == INVALID_STATION_ID)
    {
        return false;
    }

    VECTOR<TRAIN_INFO_T *> vTrain;

    Train::GetTrainOfStation(ulStationID, vTrain);

    BOOL bRet = DialogRealBrd->Brd(STATION_TRAIN, vTrain);

    if (bRet)
    {
        DialogRealBrd->ShowModal();
    }

    return bRet;
}

//---------------------------------------------------------------------------
BOOL TrainRealBrd(UINT32 ulTrainUnitID)
{
    TRAIN_INFO_T            *pstTrain = Train::GetTrainInfo(ulTrainUnitID);
    VECTOR<TRAIN_INFO_T*>   vTrain;

    if (!pstTrain)
    {
        return false;
    }

    vTrain.push_back(pstTrain);

    BOOL bRet = DialogRealBrd->Brd(SINGLE_TRAIN, vTrain);

    if (bRet)
    {
        DialogRealBrd->ShowModal();
    }

    return bRet;
}
//---------------------------------------------------------------------------
BOOL TrainRealBrd(VECTOR<UINT32> &vTrainUnitID)
{
    TRAIN_INFO_T            *pstTrain;
    VECTOR<TRAIN_INFO_T*>   vTrain;

    for (UINT32 i=0; i<vTrainUnitID.size(); i++)
    {
        pstTrain = Train::GetTrainInfo(vTrainUnitID[i]);
        if (pstTrain)
        {
            vTrain.push_back(pstTrain);
        }
    }

    BOOL bRet = DialogRealBrd->Brd(MULTI_TRAIN, vTrain);

    if (bRet)
    {
        DialogRealBrd->ShowModal();
    }

    return bRet;
}
//---------------------------------------------------------------------------
BOOL MainLineTrainRealBrd()
{
    BOOL bRet = DialogRealBrd->LineBrd(MAINLINE_TRAIN);

    if (bRet)
    {
        DialogRealBrd->ShowModal();
    }

    return bRet;
}
//---------------------------------------------------------------------------
BOOL UpLineTrainRealBrd()
{
    BOOL bRet = DialogRealBrd->LineBrd(UPLINE_TRAIN);

    if (bRet)
    {
        DialogRealBrd->ShowModal();
    }

    return bRet;
}
//---------------------------------------------------------------------------
BOOL DownLineTrainRealBrd()
{
    BOOL bRet = DialogRealBrd->LineBrd(DOWNLINE_TRAIN);

    if (bRet)
    {
        DialogRealBrd->ShowModal();
    }

    return bRet;
}
//---------------------------------------------------------------------------
void __fastcall TDialogRealBrd::InitSkin()
{
    btnPTT->Font->Size = 24;
    btnPTT->Font->Name = UICfg::GetButtonPTTFontName();
}
//---------------------------------------------------------------------------
__fastcall TDialogRealBrd::TDialogRealBrd(TComponent* Owner)
    : TDialogForm(Owner)
{
    m_ulStartTime = 0;
    m_bStartBroadcast = false;

    GetPAAudioOption();

    UINT32  ulPADelaySendTime = GetPADelaySendTime();

    if (ulPADelaySendTime > 0)
    {
        UINT32      ulMaxBufSize = 16000*2*MAX(500, ulPADelaySendTime)/1000;

        m_pPCMDataBuf = new PCMDataBuffer(ulMaxBufSize);
    }

    InitMsgHandler(EV_SA_DIS_DC_REQ, MSG_DCStartRealBrd, OnBroadcastStartRsp);
}
//---------------------------------------------------------------------------
BOOL __fastcall TDialogRealBrd::LineBrd(UINT32 ulBrdType)
{
    VECTOR<TRAIN_BRD_INFO_T>    vBrdTrain;
    VECTOR<TRAIN_INFO_T*>       &vTrain = Train::GetTrainInfo();
    BOOL                        bNeedBrd;
    TRAIN_INFO_T                *pstTrain;
    TRAIN_BRD_INFO_T            stBrdInfo = {0};
    INT64                       i64GroupID = INVALID_GROUP_ID;
    GROUP_CFG_T                 *pstGroupCfg = GroupCfg::GetGroupCfg();

    if (!pstGroupCfg)
    {
        return FALSE;
    }

    if (ulBrdType == MAINLINE_TRAIN)
    {
        i64GroupID = pstGroupCfg->i64MainLineTrainBrdGroupID;
    }
    else if (ulBrdType == UPLINE_TRAIN)
    {
        i64GroupID = pstGroupCfg->i64UpLineTrainBrdGroupID;
    }
    else if (ulBrdType == DOWNLINE_TRAIN)
    {
        i64GroupID = pstGroupCfg->i64DownLineTrainBrdGroupID;
    }
    else
    {
        return FALSE;
    }

    for (UINT32 i=0; i<vTrain.size(); i++)
    {
        bNeedBrd = FALSE;
        pstTrain = vTrain[i];

        if (ulBrdType == MAINLINE_TRAIN)
        {
            bNeedBrd = TRUE;
        }
        else if (ulBrdType == UPLINE_TRAIN &&
                 pstTrain->ulDirection == TRAIN_DIRECTION_UPSIDE)
        {
            bNeedBrd = TRUE;
        }
        else if (ulBrdType == DOWNLINE_TRAIN &&
                 pstTrain->ulDirection == TRAIN_DIRECTION_DOWNSIDE)
        {
            bNeedBrd = TRUE;
        }

        if (bNeedBrd)
        {
            memset(&stBrdInfo, 0, sizeof(stBrdInfo));
            memcpy(&stBrdInfo.stTrain, pstTrain, sizeof(stBrdInfo.stTrain));
            Train::GetTrainUserID(pstTrain->ulTrainUnitID, &stBrdInfo.ulUserID1, &stBrdInfo.ulUserID2);

            vBrdTrain.push_back(stBrdInfo);
        }
    }

    return Init(ulBrdType, vBrdTrain, i64GroupID);
}
//---------------------------------------------------------------------------
BOOL __fastcall TDialogRealBrd::Brd(UINT32 ulBrdType, VECTOR<TRAIN_INFO_T *> &vTrain)
{
    VECTOR<UINT32>              vUser;
    VECTOR<TRAIN_BRD_INFO_T>    vBrdTrain;
    TRAIN_BRD_INFO_T            stBrdInfo;

    // 添加广播组成员
    if (ulBrdType != SINGLE_TRAIN &&
        ulBrdType != MULTI_TRAIN  &&
        ulBrdType != INTERLOCK_STATION_TRAIN &&
        ulBrdType != STATION_TRAIN )
    {
        return false;
    }

    for (UINT32 i=0; i<vTrain.size(); i++)
    {
        memcpy(&stBrdInfo.stTrain, vTrain[i], sizeof(stBrdInfo.stTrain));
        Train::GetTrainUserID(stBrdInfo.stTrain.ulTrainUnitID, &stBrdInfo.ulUserID1, &stBrdInfo.ulUserID2);

        if (stBrdInfo.ulUserID1 != INVALID_USER_ID)
        {
            vUser.push_back(stBrdInfo.ulUserID1);
        }

        if (stBrdInfo.ulUserID2 != INVALID_USER_ID)
        {
            vUser.push_back(stBrdInfo.ulUserID2);
        }

        vBrdTrain.push_back(stBrdInfo);
    }

    if (vUser.size() == 0)
    {
        WarnMsg("广播成员为空");
        return false;
    }

    return Init(ulBrdType, vBrdTrain, DynamicGroup::GetBrdGroupID());
}

BOOL GetBrdSeqID(UINT32 &ulBrdSeqID)
{
    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    if (!SyncSendServer(EV_DC_DIS_SA_REQ, MSG_DCGetMaxRealBrdSeqID, ReqJsonParam, RspJson))
    {
        return FALSE;
    }

    if (!RspJson.GetValue("MaxSeqID", &ulBrdSeqID))
    {
        return FALSE;
    }

    return TRUE;
}

//---------------------------------------------------------------------------
BOOL __fastcall TDialogRealBrd::Init(UINT32 ulBrdType, VECTOR<TRAIN_BRD_INFO_T> &vBrdTrain, INT64 i64GroupID)
{
    m_ulStartTime = 0;
    g_ulBrdSeqID = 0;

    ClearMainGroupAndSpeaker();

    if (vBrdTrain.size() == 0)
    {
        InfoMsg("没有选择列车");
        return FALSE;
    }

    if (!GetBrdSeqID(g_ulBrdSeqID))
    {
        InfoMsg("获取广播序列号失败");
        return FALSE;
    }

    m_ulTimeout = g_ulTrainBroadcastMaxPeriod;

    ///edtGroupID->Text = i64GroupID;
    edtBrdStatus->Text = "";
    btnStart->Enabled = false;
    btnEnd->Enabled = false;
    btnPTT->Enabled = false;
    TimerBar->Enabled = false;
    pbTime->Position = 0;
    pbTime->Properties->Max = m_ulTimeout;
    pbTime->Properties->Text = 0;
    pbReady->Position = 0;

    edtBrdType->Text = GetBrdType(ulBrdType);

    g_i64CurrBrdGroupID = i64GroupID;
    m_ulBrdType = ulBrdType;
    m_bStartBroadcast = false;

    TimerCheck->Enabled = false;

    g_vBrdTrain.clear();
    g_vBrdTrain.assign(vBrdTrain.begin(), vBrdTrain.end());

    for (UINT32 i=0; i<g_vBrdTrain.size(); i++)
    {
        g_vBrdTrain[i].ulBrdStatus1 = BRD_STATUS_WAIT;
        g_vBrdTrain[i].ulBrdStatus2 = BRD_STATUS_WAIT;
    }

    tlBrdInfo->BeginUpdate();
    tlBrdInfo->Clear();

    for (UINT32 i=0; i<g_vBrdTrain.size(); i++)
    {
        TcxTreeListNode *Node = tlBrdInfo->Add();

        Node->Values[0] = g_vBrdTrain[i].stTrain.ulTrainUnitID;
        Node->Values[1] = g_vBrdTrain[i].stTrain.acTrainID;
        Node->Values[2] = Station::GetStationName(g_vBrdTrain[i].stTrain.ulStationID);
        Node->Values[3] = g_vBrdTrain[i].stTrain.ulDirection;
        Node->Values[4] = BRD_STATUS_WAIT;
        Node->Values[5] = g_vBrdTrain[i].ulBrdStatus1;
        Node->Values[6] = g_vBrdTrain[i].ulBrdStatus2;
    }

    tlBrdInfo->EndUpdate();

    if (!g_MicCapture)
    {
        if (!OpenMic())
        {
            InfoMsg("打开麦克风失败");
            return FALSE;
        }
    }
    else
    {
        InfoMsg("打开麦克风失败");
        return FALSE;
    }

    NotifyTrainBrdStart();

    TimerCheck->Enabled = true;
    TimerCheck->Interval = 2000;

    UINT32  ulVolume = 0;

    tbVolume->Position = ulVolume;

    return TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TDialogRealBrd::btnEndClick(TObject *Sender)
{
/*    NotifyTrainBrdEnd();

    StopGrabAudio();

    g_PAWavRec.StopRec();       */

    //StopBrd();
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TDialogRealBrd::StopBrd()
{
    TimerSend->Enabled = false;

    TimerCheck->Enabled = false;
    TimerBar->Enabled = false;

    StopGrabAudio();
    g_PAWavRec.StopRec();

    // 恢复缺省音量
    SetMicVolume(GetMainMic(), GetMainMicVolume());

/*    if (!m_bStartBroadcast)
    {
        return;
    } */

    NotifyTrainBrdEnd();

    if (m_ulStartTime > 0)
    {
        AnsiString  strTrainList = "";

        for (UINT32 i=0; i<g_vBrdTrain.size(); i++)
        {
            if (i > 0)
            {
                strTrainList += ",";
            }

            strTrainList += g_vBrdTrain[i].stTrain.ulTrainUnitID;
        }

        SaveRealBrdInfo(m_ulStartTime, gos_get_current_time(), strTrainList.c_str());
    }

    m_bStartBroadcast = false;

 //   SetAudioVolume(GetMainMic(), GetMainMicVolume());

    g_i64CurrBrdGroupID = INVALID_GROUP_ID;
}
//---------------------------------------------------------------------------
void __fastcall TDialogRealBrd::TimerBarTimer(TObject *Sender)
{
    if (!TimerBar->Enabled)
    {
        return;
    }

    if (pbTime->Position == pbTime->Properties->Max)
    {
        StopBrd();

        /*NotifyTrainBrdEnd();
        StopGrabAudio();

        g_PAWavRec.StopRec();    */

        InfoMsg("广播超时结束");
        Close();
        return;
    }

    pbTime->Position++;
    pbTime->Properties->Text = AnsiString((int)pbTime->Position) + " 秒";

    edtBrdStatus->Repaint();
    edtBrdStatus->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TDialogRealBrd::FormClose(TObject *Sender, TCloseAction &Action)
{
    StopBrd();
  /*
    NotifyTrainBrdEnd();
    StopGrabAudio();

    g_PAWavRec.StopRec();
*/
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TDialogRealBrd::NotifyTrainBrdStart()
{
    GJsonTupleParam     TupleParam;
    GJsonParam          Param;
    VECTOR<UINT32>      vUser;

    for (UINT32 i=0; i<g_vBrdTrain.size(); i++)
    {
        GJsonParam          SubParam;

        if (g_vBrdTrain[i].ulBrdStatus1 == BRD_STATUS_READY ||
            g_vBrdTrain[i].ulBrdStatus2 == BRD_STATUS_READY )
        {
            continue;
        }

        SubParam.Add("TrainUnitID", g_vBrdTrain[i].stTrain.ulTrainUnitID);
        TupleParam.Add(SubParam);

        GosLog(LOG_INFO, "notify train user %u to start real brd", g_vBrdTrain[i].ulUserID1);
    }

    if (TupleParam.GetTupleNum() == 0)
    {
        TimerCheck->Enabled = false;
        return;
    }

    Param.Add("BrdSeqID", g_ulBrdSeqID);
    Param.Add("Train", TupleParam);
    Param.Add("MaxPeriod", m_ulTimeout);

    SendServer(EV_DC_DIS_SA_REQ, MSG_DCStartRealBrd, Param);

    GosLog(LOG_INFO, "start real brd, seq ID is %u", g_ulBrdSeqID);
}
//---------------------------------------------------------------------------
void __fastcall TDialogRealBrd::NotifyTrainBrdEnd()
{
    GJsonTupleParam     TupleParam;
    GJsonParam          Param;

    for (UINT32 i=0; i<g_vBrdTrain.size(); i++)
    {
        GJsonParam          SubParam;

        SubParam.Add("TrainUnitID", g_vBrdTrain[i].stTrain.ulTrainUnitID);
        TupleParam.Add(SubParam);

        GosLog(LOG_INFO, "notify train user %u real brd", g_vBrdTrain[i].stTrain.ulTrainUnitID);
    }

    if (TupleParam.GetTupleNum() == 0)
    {
        TimerCheck->Enabled = false;
        return;
    }

    Param.Add("BrdSeqID", g_ulBrdSeqID);
    Param.Add("Train", TupleParam);

    SendServer(EV_DC_DIS_SA_REQ, MSG_DCStopRealBrd, Param);

    GosLog(LOG_INFO, "stop real brd %u", g_ulBrdSeqID);

    TimerBar->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TDialogRealBrd::OnBroadcastStartRsp(UINT32 ulSeqID, GJson &Json)
{
    UINT32  ulGroupStatus;
    UINT32  ulDCUserID;
    UINT32  ulTrainUserID;
    UINT32  ulTrainUnitID;
    UINT32  ulBrdSeqID;
    INT64   i64GroupID;
    UINT32  ulResult;
    UINT32  ulBrdStatus;
    TRAIN_BRD_INFO_T    *pstTrainBrdInfo = NULL;

    if (!Json.GetValue("BrdSeqID", &ulBrdSeqID))
    {
        GosLog(LOG_ERROR, "invalid BrdSeqID in msg %s", Json.GetText());
        return;
    }

    if (ulBrdSeqID != g_ulBrdSeqID)
    {
        GosLog(LOG_ERROR, "mismatch BrdSeqID(%u vs %u) in msg %s", ulBrdSeqID, g_ulBrdSeqID, Json.GetText());
        return;
    }

    if (!Json.GetValue("DCUserID", &ulDCUserID))
    {
        GosLog(LOG_ERROR, "invalid DCUserID in msg %s", Json.GetText());
        return;
    }

    if (ulDCUserID != APP::GetCurrDCUserID())
    {
        GosLog(LOG_ERROR, "invalid DC user(%u) in msg %s", ulDCUserID, Json.GetText());
        return;
    }

    if (!Json.GetValue("Result", &ulResult))
    {
        GosLog(LOG_ERROR, "invalid Result in msg %s", Json.GetText());
        return;
    }

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        GosLog(LOG_ERROR, "invalid TrainUnitID in msg %s", Json.GetText());
        return;
    }

    if (ulResult != APP_OK)
    {
        GosLog(LOG_ERROR, "train %u return %s to real brd", ulTrainUnitID, "fail");
        ulBrdStatus = BRD_STATUS_FAIL;
    }
    else
    {
        GosLog(LOG_INFO, "train %u return %s to real brd", ulTrainUnitID, "ready");
        ulBrdStatus = BRD_STATUS_READY;
    }

    for (UINT32 i=0; i<g_vBrdTrain.size(); i++)
    {
        pstTrainBrdInfo = &g_vBrdTrain[i];

        if (pstTrainBrdInfo->stTrain.ulTrainUnitID == ulTrainUnitID)
        {
            //  如果已经是ready状态，不更新状态
            if (pstTrainBrdInfo->ulBrdStatus1 != BRD_STATUS_READY)
            {
                pstTrainBrdInfo->ulBrdStatus1 = ulBrdStatus;
                pstTrainBrdInfo->ulBrdStatus2 = ulBrdStatus;
            }
            break;
        }
    }

    if (!pstTrainBrdInfo)
    {
        return;
    }

    if (tlBrdInfo->AbsoluteCount <= 0)
    {
        return;
    }

    if (ulResult && !btnEnd->Enabled)
    {
        btnStart->Enabled = true;
    }

    UINT32  ulReadyNum = 0;

    for (int i=0; i<tlBrdInfo->AbsoluteCount; i++)
    {
        TcxTreeListNode     *Node = tlBrdInfo->AbsoluteItems[i];

        if (pstTrainBrdInfo->stTrain.ulTrainUnitID == (UINT32)Node->Values[0])
        {
            Node->Values[5] = pstTrainBrdInfo->ulBrdStatus1;
            Node->Values[6] = pstTrainBrdInfo->ulBrdStatus2;

            if (pstTrainBrdInfo->ulBrdStatus1 == BRD_STATUS_READY ||
                pstTrainBrdInfo->ulBrdStatus2 == BRD_STATUS_READY )
            {
                Node->Values[4] = BRD_STATUS_READY;
            }
            else if (pstTrainBrdInfo->ulBrdStatus1 == BRD_STATUS_FAIL ||
                     pstTrainBrdInfo->ulBrdStatus2 == BRD_STATUS_FAIL )
            {
                Node->Values[4] = BRD_STATUS_FAIL;
            }
        }

        if ((UINT32)Node->Values[5] == BRD_STATUS_READY ||
            (UINT32)Node->Values[6] == BRD_STATUS_READY )
        {
            ulReadyNum++;
        }
    }

    pbReady->Position = 100*ulReadyNum/tlBrdInfo->AbsoluteCount;
}
//---------------------------------------------------------------------------
void __fastcall TDialogRealBrd::TimerCheckTimer(TObject *Sender)
{
    if (!TimerCheck->Enabled)
    {
        return;
    }

    TimerCheck->Interval = 1000;
    NotifyTrainBrdStart();
}
//---------------------------------------------------------------------------
void __fastcall TDialogRealBrd::FormCreate(TObject *Sender)
{
    SetImage(btnStart, IMG_BRD);
    SetImage(btnEnd, IMG_STOP);
}
//---------------------------------------------------------------------------
void __fastcall TDialogRealBrd::btnStartClick(TObject *Sender)
{
    pbTime->Position = 0;

    TimerBar->Enabled = true;
    m_bStartBroadcast = true;

    btnStart->Enabled = false;
    btnEnd->Enabled = true;
    btnPTT->Enabled = true;

    m_ulStartTime = gos_get_current_time();

    // 设置为广播音量
    SetMicVolume(GetMainMic(), GetRealBrdVolume());
}
//---------------------------------------------------------------------------
void __fastcall TDialogRealBrd::cbPTTBrdModePropertiesChange(TObject *Sender)
{
    btnPTT->Visible = cbPTTBrdMode->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TDialogRealBrd::btnPTTMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    OnPTTDial();
}
//---------------------------------------------------------------------------
void __fastcall TDialogRealBrd::btnPTTMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    OnPTTRelease();
}
//---------------------------------------------------------------------------
void __fastcall TDialogRealBrd::ColumnBrdStatusGetDisplayText(TcxTreeListColumn *Sender, TcxTreeListNode *ANode,
                                                              UnicodeString &Value)
{
    if (ANode->Values[4].IsNull())
    {
        return;
    }

    UINT32 ulValue = ANode->Values[4];

    if (ulValue == BRD_STATUS_WAIT)
    {
        Value = "";
    }
    else if (ulValue == BRD_STATUS_READY)
    {
        Value = "已准备";
    }
    else if (ulValue == BRD_STATUS_FAIL)
    {
        Value = "失败";
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogRealBrd::btnPTTMouseEnter(TObject *Sender)
{
    btnPTT->LookAndFeel->SkinName = UICfg::GetPTTSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TDialogRealBrd::btnPTTMouseLeave(TObject *Sender)
{
    btnPTT->LookAndFeel->SkinName = GetDefaultSkinName();
}
//---------------------------------------------------------------------------
void __fastcall TDialogRealBrd::tlBrdInfoCustomDrawDataCell(TcxCustomTreeList *Sender,
          TcxCanvas *ACanvas, TcxTreeListEditCellViewInfo *AViewInfo,
          bool &ADone)
{
    if (!AViewInfo->Node || AViewInfo->Node->Values[4].IsNull() ||
        AViewInfo->Column->ItemIndex != 4)
    {
        return;
    }

    INT32   iStatus = AViewInfo->Node->Values[4];

    if (iStatus == BRD_STATUS_FAIL)
    {
        ACanvas->Canvas->Brush->Color = UICfg::GetBrdFailColor();
    }
}
//---------------------------------------------------------------------------
void __fastcall TDialogRealBrd::OnPTTDial()
{
    if (!btnPTT->Enabled)
    {
        return;
    }

    if (!StartGrabAudio())
    {
        edtBrdStatus->Text = "广播对讲失败";
        edtBrdStatus->Style->TextColor = clRed;
    }
    else
    {
        edtBrdStatus->Text = "广播中";
        edtBrdStatus->Style->TextColor = clWindowText;

        btnPTT->LookAndFeel->SkinName = UICfg::GetPTTDownSkinName();

        UINT32  ulPADelaySendTime = GetPADelaySendTime();

        if (ulPADelaySendTime &&
            !TimerSend->Enabled)
        {
            TimerSend->Interval = ulPADelaySendTime;
            TimerSend->Enabled = true;
            g_ulSendPCMNum = 0;
        }
    }

    CHAR acRecFile[256];
    UINT32 ulPCMFrameLen = g_ucPCMCaptureBitsPerSample / 8;

    GetPAAudioRecFileName(acRecFile);

    g_PAWavRec.Init(acRecFile, g_usPCMCaptureSampleRate, ulPCMFrameLen, FALSE);

    edtBrdStatus->Repaint();
    edtBrdStatus->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TDialogRealBrd::OnPTTRelease()
{
    PauseGrabAudio();

    edtBrdStatus->Text = "广播暂停";
    edtBrdStatus->Style->TextColor = (TColor)0x80D0;  // 橙色
    btnPTT->LookAndFeel->SkinName = GetDefaultSkinName();

    edtBrdStatus->Repaint();
    edtBrdStatus->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TDialogRealBrd::TimerSendTimer(TObject *Sender)
{
    if (!TimerSend->Enabled)
    {
        return;
    }

    TimerSend->Interval = 1;

    UINT32  ulCurrTime = gos_get_uptime_1ms();

    UINT32  ulMaxSendDataLen = GetPAMaxPkgSize();    // 1000
    UINT32  ulSendBytesPerSec = g_usPCMCaptureSampleRate*g_ucPCMCaptureBitsPerSample/8;//32000
    double  dPeriod = ((double)1000*ulMaxSendDataLen)/ulSendBytesPerSec;   // 毫秒  , 1000/32 = 31.25 //31.25计算的规则
    UINT32  ulLen;

    if (g_ulSendPCMNum > 0 &&
        (ulCurrTime - g_ulFirstSendPCMTime) < g_ulSendPCMNum*dPeriod)
    {
        return;
    }

    if (0 == g_ulSendPCMNum)
    {
        g_ulFirstSendPCMTime = ulCurrTime;
    }

    GosLog(LOG_INFO, "send BRD stat: %u %.2f",
            ulCurrTime - g_ulFirstSendPCMTime,
            (ulCurrTime - g_ulFirstSendPCMTime)/dPeriod);

    g_ulSendPCMNum++;

    UINT8 *pucData = NULL;

    pucData = (UINT8*)malloc(ulMaxSendDataLen);

    m_pPCMDataBuf->GetData(pucData, ulMaxSendDataLen, ulLen);

    if (ulLen > 0)
    {
        SendPCMDataToDIS(pucData, ulLen);
    }

    free(pucData);
}
//---------------------------------------------------------------------------


