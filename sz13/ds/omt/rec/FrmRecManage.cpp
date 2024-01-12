//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmLocalRecManage.h"
#include "FrmRecManage.h"
#include "FrmLogin.h"
#include "main.h"
#include "GTransFile.h"
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
#pragma link "cxCalendar"
#pragma link "cxDateUtils"
#pragma link "cxMaskEdit"
#pragma link "dxCore"
#pragma link "cxSpinEdit"
#pragma link "cxTimeEdit"
#pragma link "cxImageList"
#pragma link "cxClasses"
#pragma link "cxImageComboBox"
#pragma link "dxSkinsForm"
#pragma link "cxTrackBar"

#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "WMPLib_OCX"
#pragma link "cxDataControllerConditionalFormattingRulesManagerDialog"
#pragma link "cxSplitter"
#pragma resource "*.dfm"
TFormRecManage *FormRecManage;

extern BOOL HttpDownloadFile(AnsiString strUrl, AnsiString strLocalFile);

static const UINT32 m_ulRecFileTypeAudio = 1;   // 录音
static const UINT32 m_ulRecFileTypeVedio = 2;   // 录像
static const UINT32 m_ulRecFileTypeAll = m_ulRecFileTypeAudio | m_ulRecFileTypeVedio;   // 所有

static ID_VALUE_T   s_astRecType[] = {
                        {REC_TYPE_P2P_AUDIO,    "语音个呼"},
                        {REC_TYPE_P2P_VIDEO,    "视频个呼"},
                        {REC_TYPE_VIDEO_UPLOAD, "视频回传"},
                        {REC_TYPE_PTT,          "组呼"},
                        {REC_TYPE_IPH,          "乘客对讲"},
                                };

static Dict g_DictRecType(s_astRecType, ARRAY_SIZE(s_astRecType));

static bool     m_bChangingPos = false;

static AnsiString   m_strStartTime = "";
static AnsiString   m_strEndTime = "";

static VECTOR<ESDK_REC_FILE_INFO_T>     g_vRecFileInfo;

UINT32  g_ulGetDCRecFileSeqID = 0;

std::map<AnsiString, TcxTreeListNode*>  g_mFileNodeMap;

extern UINT32  g_usTransFilePort;

extern bool ESDKLogin();
extern BOOL HttpDownloadFile(AnsiString strUrl, AnsiString strLocalFile);


static AnsiString GetMediaLength(int iTime)
{
    if (iTime <= 0)
    {
        return "";
    }

    char    acText[64];

    if (iTime < 3600)
    {
        sprintf(acText, "%02d:%02d", iTime/60, MOD(iTime, 60));
    }
    else
    {
        sprintf(acText, "%d:%02d:%02d", iTime/3600, MOD(iTime/60, 60), MOD(iTime, 60));
    }

    return acText;
}

static void SetTime(TcxDateEdit *edt, UINT32 ulTime)
{
    edt->Date = GetDateTime(ulTime);
}

static void SetTime(TcxDateEdit *edt, AnsiString strTime)
{
    if (strTime.Length() != 19)
    {
        return;
    }

    UINT32 ulTime = gos_text_time_to_int(strTime.c_str());

    if (ulTime != 0xffffffff)
    {
        edt->Date = GetDateTime(ulTime);
    }
}

static CHAR* GetRecType(UINT32 ulRecType)
{
    for (UINT32 i=0; i<ARRAY_SIZE(s_astRecType); i++)
    {
        if (s_astRecType[i].ulID == ulRecType)
        {
            return s_astRecType[i].szValue;
        }
    }

    return "";
}

static INT32 GetImageIndex(AnsiString &strFile)
{
    int     iLen = strFile.Length();
    CHAR    *szEnd = strFile.c_str() + iLen;

    if (iLen > 4 && strcasecmp(szEnd-4, ".amr") == 0)
    {
        // audio
        return 0;
    }

    if (iLen > 4 && strcasecmp(szEnd-4, ".mp4") == 0)
    {
        // video
        return 1;
    }

    return -1;
}

static INT32 GetImageIndex(UINT32 ulRecType)
{
    if (ulRecType == m_ulRecFileTypeAudio)
    {
        // audio
        return 0;
    }
    else if (ulRecType == m_ulRecFileTypeVedio)
    {
        // video
        return 1;
    }

    return -1;
}

static void FormatUrlRTSP(AnsiString &strFile, AnsiString strUser, AnsiString strPwd)
{
    AnsiString  strTmp = "";
    int         iPos = strFile.Pos("://");

    if (iPos <= 0)
    {
        return;
    }

    strTmp = strFile.SubString(1, iPos-1);
    strTmp += "://";
    strTmp += strUser + ":" + strPwd + "@";
    strTmp += strFile.SubString(iPos+3, strFile.Length());

    strFile = strTmp;
}

/*
// 20200306145319
AnsiString FormatTime(CHAR *szTime)
{
    CHAR    acTime[32];

    if (strlen(szTime) != 14)
    {
        return szTime;
    }

    sprintf(acTime, "%c%c%c%c-%c%c-%c%c %c%c:%c%c:%c%c",
            szTime[0], szTime[1], szTime[2], szTime[3],
            szTime[4], szTime[5], szTime[6], szTime[7],
            szTime[8], szTime[9], szTime[10], szTime[11],
            szTime[12], szTime[13]);

    return acTime;
}     */

/*
void GetRecFileName(AnsiString strURL, AnsiString &strFile, AnsiString &strTime)
{
    // 4071_1001_20200306145319_678315.amr
    CHAR        acURL[1024];
    CHAR        *szTmp;
    CHAR        *szFile = strFile.c_str();

    memset(acURL, 0, sizeof(acURL));
    szTmp = gos_right_strchr(szFile, '/');
    if (szTmp)
    {
        szFile = szTmp+1;
    }

    strncpy(acURL, szFile, sizeof(acURL)-1);
    strFile = szFile;
    strTime = "";

    szFile = gos_right_strchr(acURL, '_');
    if (!szFile)
    {
        return;
    }

    strFile = szFile+1;
    *szFile = '\0';

    szTmp = gos_right_strchr(acURL, '_');
    if (!szTmp)
    {
        return;
    }

    strTime = FormatTime(szTmp+1);
}  */

static AnsiString GetRecFileName(AnsiString strURL)
{
    // 4071_1001_20200306145319_678315.amr
    CHAR        *szTmp;

    szTmp = gos_right_strchr(strURL.c_str(), '/');
    if (!szTmp)
    {
        szTmp = gos_right_strchr(strURL.c_str(), '\\');
    }

    if (szTmp)
    {
        return szTmp+1;
    }

    return strURL;
}

void __fastcall TFormRecManage::InitSkin()
{
//  tbPlay->Style->LookAndFeel->SkinName = "MoneyTwins";
//  tbVolume->Style->LookAndFeel->SkinName = g_strVolumeSkinName;// "MoneyTwins";

//  WindowsMediaPlayer->uiMode = "none";
}
//---------------------------------------------------------------------------
__fastcall TFormRecManage::TFormRecManage(TComponent* Owner)
    : TSubForm(Owner)
{
    g_DictRecType.Init(cbRecType);
    g_DictRecType.SetID(REC_TYPE_PTT);

    UINT32  ulTime = gos_get_current_time();

    SetTime(deStartTime, ulTime - 24*3600);

    btnPause->Visible = false;
    btnPlay->Visible = true;

    InitMsgHandler(EV_DC_DIS_RSP, MSG_DCGetRecFileListRsp, OnGetRecFileListRsp);

    g_ulGetDCRecFileSeqID = gos_get_rand();

    sprintf(m_acTmpRecPath, "%s/%s", gos_get_root_path(), "tmp_rec");
    gos_delete_dir(m_acTmpRecPath);
    gos_delete_file(m_acTmpRecPath);
    gos_create_dir(m_acTmpRecPath);

    gbMain->Width = 1000;
}

//---------------------------------------------------------------------------
void __fastcall TFormRecManage::btnQueryClick(TObject *Sender)
{
    UINT32  ulStartTime = GetTime(deStartTime->Date);
    UINT32  ulEndTime = GetTime(deEndTime->Date);
    UINT32  ulRecType;
    UINT32  ulSendUserID;
    AnsiString  strTmp;
    INT32   iCaller = -1;
    INT32   iCallee = -1;
    INT64   i64ResourceID = -1;

    g_DictRecType.GetID(cbRecType, &ulRecType);

    tlMain->Clear();
    g_mFileNodeMap.clear();

    strTmp = edtResourceID->Text.Trim();
    if (!strTmp.IsEmpty())
    {
        if (!gos_atoi64(strTmp.c_str(), &i64ResourceID) ||
            i64ResourceID <= 0)
        {
            edtResourceID->SetFocus();
            InfoMsg(LabelResource->Caption + "非法");
            return;
        }
    }

    strTmp = edtCaller->Text.Trim();
    if (!strTmp.IsEmpty())
    {
        if (!ToInt(strTmp, &iCaller) || iCaller <= 0)
        {
            edtCaller->SetFocus();
            InfoMsg("主叫非法");
            return;
        }
    }

    strTmp = edtCallee->Text.Trim();
    if (!strTmp.IsEmpty())
    {
        if (!ToInt(strTmp, &iCallee) || iCallee <= 0)
        {
            edtCallee->SetFocus();
            InfoMsg("被叫非法");
            return;
        }
    }

    if (ulRecType == REC_TYPE_PTT )
    {
        if (i64ResourceID <= 0)
        {
            InfoMsg(LabelResource->Caption + "不能为空");
            return;
        }
    }
    else if (ulRecType == REC_TYPE_P2P_AUDIO ||
             ulRecType == REC_TYPE_P2P_VIDEO ||
             ulRecType == REC_TYPE_VIDEO_UPLOAD )
    {
        if (iCaller <= 0 &&
            iCallee <= 0 &&
            i64ResourceID <= 0)
        {
            InfoMsg("资源号、主叫和被叫不能都为空");
            return;
        }
    }

    if (ulRecType != REC_TYPE_IPH)
    {
        if (!eSDK_IsLogined())
        {
            if (!ESDKLogin())
            {
                InfoMsg("连接集群服务器失败");
                return;
            }
        }

        if (!eSDK_LoginMRS(g_stLocalCfg.aucMRSAddr))
        {
            InfoMsg("连接录音录像服务器失败");
            return;
        }

        VECTOR<ESDK_REC_FILE_INFO_T>    vRecFileInfo;

        if (!eSDK_GetRecFileList(ulRecType, iCaller, iCallee, i64ResourceID,
                                 ulStartTime, ulEndTime, vRecFileInfo))
        {
            InfoMsg("查询失败");
            return;
        }

        vRecFileInfo.swap(g_vRecFileInfo);
        InitView(ulRecType, g_vRecFileInfo);
    }
    else
    {
        GJsonParam  JsonParam;

        //JsonParam.Add("TrainUnitID", i64ResourceID);
        JsonParam.Add("StartTime", ulStartTime);
        JsonParam.Add("EndTime", ulEndTime);

        g_ulGetDCRecFileSeqID++;
        SendServer(EV_DC_DIS_REQ, MSG_DCGetRecFileListReq, g_ulGetDCRecFileSeqID, JsonParam);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::InitView(UINT32 ulRecType, VECTOR<ESDK_REC_FILE_INFO_T> &vInfo)
{
    TcxTreeListNode     *Node;
    AnsiString          strTime;
    AnsiString          strFile;
    UINT32              ulTime;

    m_strStartTime = "";
    m_strEndTime = gos_get_text_time(NULL);

    tlMain->BeginUpdate();
    tlMain->Clear();
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        Node = tlMain->Add();

        strFile = vInfo[i].strHttpUrl; //GetRecFileName(vInfo[i].strHttpUrl);//, strTime);
        strTime = GetTextTime(vInfo[i].ulStartTime);

        Node->Values[0] = i;
        Node->Values[1] = strTime;
        Node->Values[2] = GetImageIndex(strFile);
        Node->Values[3] = strFile;
        Node->Values[4] = vInfo[i].i64ResourceID;
        Node->Values[5] = vInfo[i].ulCaller;
        Node->Values[6] = vInfo[i].ulCallee;
        Node->Values[7] = vInfo[i].strRtspUrl;
        Node->Values[8] = vInfo[i].strHttpUrl;
        Node->Values[9] = 0;
        Node->Values[10] = vInfo[i].ulEndTime - vInfo[i].ulStartTime;
        Node->Values[11] = ulRecType;

        if (strTime.Length() != 19)
        {
            continue;
        }

        if (strTime > m_strStartTime)
        {
            m_strStartTime = strTime;
        }

        if (strTime < m_strEndTime)
        {
            m_strEndTime = strTime;
        }
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::tlMainDblClick(TObject *Sender)
{
    btnPlayClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::pmiPlayClick(TObject *Sender)
{
    btnPlayClick(this);
}

//---------------------------------------------------------------------------
BOOL DownloadDCRecFile(char *szDCAddr, char *szFile, char *szLocalFile)
{
    UINT8   aucServerAddr[4];
    UINT16  usServerPort = g_usTransFilePort;

    if (!gos_get_ip(szDCAddr, aucServerAddr))
    {
        return FALSE;
    }

    return TransFile(aucServerAddr, usServerPort, szFile, szLocalFile);
}
//---------------------------------------------------------------------------
bool __fastcall TFormRecManage::DownloadFile(AnsiString &strLocalFile, AnsiString &strFileType)
{
    if (tlMain->SelectionCount == 0)
    {
        return false;
    }

    TcxTreeListNode *Node = tlMain->Selections[0];

    if (Node->Values[0].IsNull() ||
        Node->Values[11].IsNull() )
    {
        return false;
    }

    CHAR        acUrlHttp[1024];
    CHAR        acFile[256];
    CHAR        *szFileType;
    UINT32      ulIndex = Node->Values[0];
    UINT32      ulRecType = Node->Values[11];
    static CHAR     acLocalFile[256];
    static AnsiString  g_strLastDownloadFile = "";

    if (ulRecType == REC_TYPE_IPH)
    {
        if (Node->Values[12].IsNull() )
        {
            return false;
        }

        AnsiString  strDCAddr = Node->Values[12];
        AnsiString  strFile = Node->Values[3];
        UINT32      ulRecFileType = Node->Values[2];

        StrCpy(acFile, strFile);
        szFileType = gos_get_file_postfix(acFile);

        if (g_strLastDownloadFile != acFile ||
            !gos_file_exist(acFile))
        {
            sprintf(acLocalFile, "%s/tmp_%u.%s", m_acTmpRecPath, gos_get_uptime_1ms(), szFileType);

            gos_delete_file(acLocalFile);

            GosLog(LOG_DETAIL, "download remote file %s", acFile);
            if (!DownloadDCRecFile(strDCAddr.c_str(), acFile, acLocalFile))
            {
                GosLog(LOG_ERROR, "download dc(%s) file %s to local %s failed", strDCAddr.c_str(), acFile, acLocalFile);
                //InfoMsg("下载失败");
                return false;
            }

            GosLog(LOG_DETAIL, "download dc file %s succ", acFile);

            g_strLastDownloadFile = acFile;
        }
    }
    else
    {
        if (ulIndex >= g_vRecFileInfo.size())
        {
            return false;
        }

        StrCpy(acUrlHttp, g_vRecFileInfo[ulIndex].strHttpUrl);
        strcpy(acFile, acUrlHttp);

        szFileType = gos_get_file_postfix(acFile);

        sprintf(acLocalFile, "%s/tmp_%u.%s", m_acTmpRecPath, gos_get_uptime_1ms(), szFileType);

        gos_delete_file(acLocalFile);

        GosLog(LOG_DETAIL, "download remote file %s", acUrlHttp);
        if (!HttpDownloadFile(acUrlHttp, acLocalFile))
        {
            GosLog(LOG_ERROR, "download remote file %s to local %s failed", acUrlHttp, acLocalFile);
        //  InfoMsg("下载失败");
            return false;
        }

        GosLog(LOG_DETAIL, "download remote file %s succ", acUrlHttp);
    }

    strFileType = szFileType;
    strFileType = strFileType.LowerCase();
    strLocalFile = acLocalFile;

    return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::pmiExportClick(TObject *Sender)
{
/*  if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlMain->Selections[0];

    if (Node->Values[0].IsNull() ||
        Node->Values[10].IsNull() )
    {
        return;
    }

    CHAR        acUrlHttp[1024];
    CHAR        acFile[256];
    CHAR        *szFileType;
    CHAR        acLocalFile[256];
    UINT32      ulIndex = Node->Values[0];
    UINT32      ulRecType = Node->Values[10];

    if (ulRecType == REC_TYPE_IPH)
    {
        if (Node->Values[11].IsNull() )
        {
            return;
        }

        AnsiString  strDCAddr = Node->Values[11];
        AnsiString  strFile = Node->Values[3];
        UINT32      ulRecFileType = Node->Values[2];

        StrCpy(acFile, strFile);
        szFileType = gos_get_file_postfix(acFile);
        sprintf(acLocalFile, "%s/%s.%s", gos_get_root_path(), "tmp_rec", szFileType);

        gos_delete_file(acLocalFile);

        GosLog(LOG_DETAIL, "download remote file %s", acFile);
        if (!DownloadDCRecFile(strDCAddr.c_str(), acFile, acLocalFile))
        {
            GosLog(LOG_ERROR, "download dc(%s) file %s to local %s failed", strDCAddr.c_str(), acFile, acLocalFile);
            InfoMsg("下载失败");
            return;
        }

        goto export_file;
    }

    if (ulIndex >= g_vRecFileInfo.size())
    {
        return;
    }

    StrCpy(acUrlHttp, g_vRecFileInfo[ulIndex].strHttpUrl);
    strcpy(acFile, acUrlHttp);

    szFileType = gos_get_file_postfix(acFile);

    sprintf(acLocalFile, "%s/%s.%s", gos_get_root_path(), "tmp_rec", szFileType);

    gos_delete_file(acLocalFile);

    GosLog(LOG_DETAIL, "download remote file %s", acUrlHttp);
    if (!HttpDownloadFile(acUrlHttp, acLocalFile))
    {
        GosLog(LOG_ERROR, "download remote file %s to local %s failed", acUrlHttp, acLocalFile);
        InfoMsg("下载失败");
        return;
    }

    GosLog(LOG_DETAIL, "download remote file %s succ", acUrlHttp);

    */

    AnsiString  strLocalFile = "";
    AnsiString  strFileType = "";

    if (!DownloadFile(strLocalFile, strFileType))
    {
        InfoMsg("下载失败");
        return;
    }

    SaveDialog->Filter = "*.*|*.*";
    SaveDialog->DefaultExt = "*.*";

    if (strFileType == "wav")
    {
        SaveDialog->Filter = "*.wav|*.wav";
        SaveDialog->DefaultExt = "*.wav";
    }
    else if (strFileType == "amr")
    {
        SaveDialog->Filter = "*.amr|*.amr";
        SaveDialog->DefaultExt = "*.amr";
    }
    else if (strFileType == "mp4")
    {
        SaveDialog->Filter = "*.mp4|*.mp4";
        SaveDialog->DefaultExt = "*.mp4";
    }

    if (!SaveDialog->Execute())
    {
        return;
    }

    AnsiString  strFile = SaveDialog->FileName;

    if (!gos_copy_file(strLocalFile.c_str(), strFile.c_str()))
    {
        ErrMsg("导出失败");
    }
    else
    {
        InfoMsg("导出成功");
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::FormCreate(TObject *Sender)
{
    SetImage(btnQuery, IMG_QUERY);
    SetImage(btnQueryPrev, IMG_PREV);
    SetImage(btnQueryNext, IMG_NEXT);

    SetImage(pmiPlay, IMG_PLAY);
    SetImage(pmiExport, IMG_EXPORT);

    SetImage(btnPlay, IMG_PLAY);
    SetImage(btnPause, IMG_PAUSE);
    SetImage(btnStop, IMG_STOP);
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::btnPlayClick(TObject *Sender)
{
    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlMain->Selections[0];

    if (Node->Values[3].IsNull() )
    {
        return;
    }

    AnsiString  strSelectFile = Node->Values[3];
    AnsiString  strCurrPlayingFile = WindowsMediaPlayer->URL;

    if (!strCurrPlayingFile.IsEmpty() &&
        strCurrPlayingFile != strSelectFile)
    {
        btnStopClick(this);
    }
    else if (WindowsMediaPlayer->playState == WMPPlayState::wmppsPaused)
    {
        WindowsMediaPlayer->controls->play();
        TimerPlay->Enabled = true;

        return;
    }

    /*
    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlMain->Selections[0];

    if (Node->Values[0].IsNull() )
    {
        return;
    }

    UINT32      ulIndex = Node->Values[0];

    if (ulIndex >= g_vRecFileInfo.size())
    {
        return;
    }

    CHAR        acUrlHttp[1024];
//  CHAR        acFile[256];
    CHAR        *szFileType;

//  StrCpy(acFile, g_vRecFileInfo[ulIndex].strRtspUrl);
    StrCpy(acUrlHttp, g_vRecFileInfo[ulIndex].strHttpUrl);
    szFileType = gos_get_file_postfix(acUrlHttp);

    CHAR    acLocalFile[256];

    sprintf(acLocalFile, "%s/%s.%s", gos_get_root_path(), "tmp_rec", szFileType);

    gos_delete_file(acLocalFile);

    GosLog(LOG_DETAIL, "download remote file %s", acUrlHttp);
    if (!HttpDownloadFile(acUrlHttp, acLocalFile))
    {
        GosLog(LOG_ERROR, "download remote file %s to local %s failed", acUrlHttp, acLocalFile);
        InfoMsg("播放失败");
        return;
    }

    GosLog(LOG_DETAIL, "download remote file %s succ", acUrlHttp);
    */

    AnsiString  strLocalFile = "";
    AnsiString  strFileType = "";

    if (!DownloadFile(strLocalFile, strFileType))
    {
        InfoMsg("回放失败");
        return;
    }

    PlayRecFile(strLocalFile.c_str());
}

//---------------------------------------------------------------------------
void __fastcall TFormRecManage::PlayRecFile(AnsiString strUrl)
{
//  FormatUrlRTSP(strRtspUrl, g_stLocalCfg.acDCUserName, g_stLocalCfg.acDCUserPwd);

    WindowsMediaPlayer->URL = strUrl;
    WindowsMediaPlayer->controls->play();
    WindowsMediaPlayer->uiMode = "none";
    WindowsMediaPlayer->stretchToFit = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::btnPauseClick(TObject *Sender)
{
    WindowsMediaPlayer->controls->pause();

    TimerPlay->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::btnStopClick(TObject *Sender)
{
//  g_MediaPlayer->Stop();

    WindowsMediaPlayer->controls->stop();
    WindowsMediaPlayer->URL = "";

    TimerPlay->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::TimerPlayTimer(TObject *Sender)
{
//  if (g_MediaPlayer->PlayState != vmpsRunning)
    if (!m_bChangingPos && tbPlay)
    {
        tbPlay->Position = WindowsMediaPlayer->controls->currentPosition;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::PanelPlayResize(TObject *Sender)
{
//  g_MediaPlayer->AdjustVideoSize();
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::tbPlayMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    m_bChangingPos = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::tbPlayMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    int iPos = X - 12;
    int iMax = tbPlay->Width - 24;

    if (iPos > 0 && iMax > 50)
    {
        tbPlay->Position = tbPlay->Properties->Max*iPos/iMax;
    }

    WindowsMediaPlayer->controls->currentPosition = tbPlay->Position;
    m_bChangingPos = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::TimerInitTimer(TObject *Sender)
{
//  g_MediaPlayer->hWndVideo = PanelMediaPlayer->Handle;
    TimerInit->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::tbVolumePropertiesChange(TObject *Sender)
{
//  g_MediaPlayer->Volume = tbVolume->Position;
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::btnVolumeClick(TObject *Sender)
{
    btnVolume->Visible = false;
    btnMute->Visible = true;
    tbVolume->Enabled = false;
//  g_MediaPlayer->Volume = 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::btnMuteClick(TObject *Sender)
{
    btnVolume->Visible = true;
    btnMute->Visible = false;
    tbVolume->Enabled = true;
 // g_MediaPlayer->Volume = tbVolume->Position;
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::WindowsMediaPlayerPlayStateChange(TObject *Sender,
          long NewState)
{
/*
[controls] wmp.controls //播放器基本控制
11 controls.play; //播放
12 controls.pause; //暂停
13 controls.stop; //停止
14 controls.currentPosition:double; //当前进度
15 controls.currentPositionString:string; //当前进度，字符串格式。如“00:23”
16 controls.fastForward; //快进
17 controls.fastReverse; //快退
18 controls.next; //下一曲
19 controls.previous; //上一曲

wmp.settings //播放器基本设置
22 settings.volume:integer; //音量，0-100
23 settings.autoStart:Boolean; //是否自动播放
24 settings.mute:Boolean; //是否静音
25 settings.playCount:integer; //播放次数

[currentMedia] wmp.currentMedia //当前媒体属性
28 currentMedia.duration:double; //媒体总长度
29 currentMedia.durationString:string; //媒体总长度，字符串格式。如“03:24”
30 currentMedia.getItemInfo(const string); //获取当前媒体信息"Title"=媒体标题，"Author"=艺术家，"Copyright"=版权信息，"Description"=媒体内容描述，"Duration"=持续时间（秒），"FileSize"=文件大小，"FileType"=文件类型，"sourceURL"=原始地址
31 currentMedia.setItemInfo(const string); //通过属性名设置媒体信息
32 currentMedia.name:string; //同 currentMedia.getItemInfo("Title")

playState:integer; //播放状态，1=停止，2=暂停，3=播放，6=正在缓冲，9=正在连接，10=准备就绪
enum class WMPPlayState
*/

// Player.settings.setMode("loop", true);///实现循环播放

    if (NewState == WMPPlayState::wmppsPlaying)
    {
   //       tbPlay->Properties->Max = WindowsMediaPlayer->currentMedia->duration;
        if (WindowsMediaPlayer->currentMedia)
        {
            INT32       iTime = WindowsMediaPlayer->currentMedia->duration;

            tbPlay->Properties->Max = iTime;
            LabelMediaLength->Caption = GetMediaLength(iTime);

            int iInterval = tbPlay->Width<=0?1000:1000*iTime/tbPlay->Width;

            if (iTime <= 0)
            {
                TimerPlay->Enabled = false;
            }
            else
            {
                TimerPlay->Interval = iInterval<=500?500:iInterval;
                TimerPlay->Enabled = true;

                btnPlay->Visible = false;
                btnPause->Visible = true;
                btnStop->Enabled = true;

                gbTrackBar->Enabled = true;
            }

            PanelMediaPlayerResize(this);
        }
    }
    else if (NewState == WMPPlayState::wmppsStopped)
    {
        btnPlay->Visible = true;
        btnPause->Visible = false;
        btnStop->Enabled = false;
        LabelMediaLength->Caption = "";

        PanelSide1->Align = alClient;

        gbTrackBar->Enabled = false;
        tbPlay->Position = 0;
    }
    else if (NewState == WMPPlayState::wmppsPaused)
    {
        btnPlay->Visible = true;
        btnPause->Visible = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::tbPlayPropertiesGetPositionHint(TObject *Sender,
          const int APosition, UnicodeString &AHintText, bool &ACanShow, bool &AIsHintMultiLine)
{
    AHintText = GetMediaLength(tbPlay->Position);
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::PanelMediaPlayerResize(TObject *Sender)
{
    if (!WindowsMediaPlayer->currentMedia)
    {
        PanelSide1->Align = alClient;
        PanelSide1->BringToFront();

        return;
    }

    int iMediaWidth = WindowsMediaPlayer->currentMedia->imageSourceWidth;
    int iMediaHeight = WindowsMediaPlayer->currentMedia->imageSourceHeight;
    int iTotalWidth = WindowsMediaPlayer->Parent->Width;
    int iTotalHeight = WindowsMediaPlayer->Parent->Height;

    if (iMediaWidth <= 0 || iMediaHeight <= 0)
    {
        PanelSide1->Align = alClient;
        PanelSide1->BringToFront();

        return;
    }

    double  dRateOri = ((double)iTotalWidth)/iTotalHeight;
    double  dRate = ((double)iMediaWidth)/iMediaHeight;
    int     iPlayWidth;
    int     iPlayHeight;

    if (dRateOri > dRate)
    {
        iPlayHeight = iTotalHeight;
        iPlayWidth = iTotalHeight*iMediaWidth/iMediaHeight;

        PanelSide1->Align = alLeft;
        PanelSide1->Width = (iTotalWidth - iPlayWidth)/2;

        PanelSide2->Align = alRight;
        PanelSide2->Width = iTotalWidth - PanelSide1->Width - iPlayWidth;
    }
    else
    {
        iPlayWidth = iTotalWidth;
        iPlayHeight = iTotalWidth*iMediaHeight/iMediaWidth;

        PanelSide1->Align = alTop;
        PanelSide1->Height = (iTotalHeight - iPlayHeight)/2;

        PanelSide2->Align = alBottom;
        PanelSide2->Height = iTotalHeight - PanelSide1->Height - iPlayHeight;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::FormClose(TObject *Sender, TCloseAction &Action)
{
    m_bChangingPos = true;
}
//---------------------------------------------------------------------------

void __fastcall TFormRecManage::btnQueryPrevClick(TObject *Sender)
{
    deStartTime->Clear();
    SetTime(deEndTime, m_strStartTime);

    btnQueryClick(this);
}
//---------------------------------------------------------------------------

void __fastcall TFormRecManage::btnQueryNextClick(TObject *Sender)
{
    SetTime(deStartTime, m_strEndTime);
    deEndTime->Clear();

    btnQueryClick(this);
}

//---------------------------------------------------------------------------
void __fastcall TFormRecManage::OnGetRecFileListRsp(UINT32 ulSeqID, GJson &Json)
{
    if (g_ulGetDCRecFileSeqID != ulSeqID)
    {
        return;
    }

    VECTOR<GJson*>  &vSubJson = Json.GetSubJson();
    VECTOR<LOCAL_REC_INFO_T>    vLocalRecInfo;

    for (UINT32 i=0; i<vSubJson.size(); i++)
    {
        GJson   *SubJson = vSubJson[i];

        LOCAL_REC_INFO_T    stInfo = {0};

        memset(&stInfo, 0, sizeof(stInfo));

        if (!SubJson->GetValue("File", stInfo.acFile, sizeof(stInfo.acFile))  ||
            !SubJson->GetValue("Time", &stInfo.ulTime)  ||
            !SubJson->GetValue("RecType", &stInfo.ulRecType)  ||
            !SubJson->GetValue("ResourceID", stInfo.acResourceID, sizeof(stInfo.acResourceID))  ||
            !SubJson->GetValue("TrainUnitID", &stInfo.ulTrainUnitID)  ||
            !SubJson->GetValue("RollingStockID", &stInfo.ulRollingStockID)  ||
            !SubJson->GetValue("IPHDevID", &stInfo.ulIPHDevID) ||
            !SubJson->GetIP("DCAddr", stInfo.aucDCAddr) )
        {
            GosLog(LOG_ERROR, "get file rec of MsgInfo failed: %s", Json.GetText());
            return;
        }

        if (!SubJson->GetValue("MediaLength", &stInfo.ulMediaLength))
        {
            stInfo.ulMediaLength = 0;
        }

        vLocalRecInfo.push_back(stInfo);
    }

    ShowLocalRec(vLocalRecInfo);
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::ShowLocalRec(VECTOR<LOCAL_REC_INFO_T> &vInfo)
{
    TcxTreeListNode     *Node;
    AnsiString          strTime;
    AnsiString          strFile;
    UINT32              ulTime;

    m_strStartTime = "";
    m_strEndTime = gos_get_text_time(NULL);

    tlMain->BeginUpdate();
 // tlMain->Clear();

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        LOCAL_REC_INFO_T   &stInfo  = vInfo[i];

        strFile = stInfo.acFile;
        strTime = GetTextTime(stInfo.ulTime);

        if (g_mFileNodeMap.find(strFile) != g_mFileNodeMap.end())
        {
            Node = g_mFileNodeMap[strFile];
            Node->Values[10] = stInfo.ulMediaLength;
            continue;
        }

        Node = tlMain->Add();

        g_mFileNodeMap[strFile] = Node;

        Node->Values[0] = i;
        Node->Values[1] = strTime;
        Node->Values[2] = GetImageIndex(stInfo.ulRecType);
        Node->Values[3] = strFile;
        Node->Values[4] = stInfo.acResourceID;
        Node->Values[5] = stInfo.ulTrainUnitID;
        Node->Values[6] = stInfo.ulRollingStockID;
        Node->Values[7] = "";
        Node->Values[8] = "";
        Node->Values[9] = stInfo.ulIPHDevID;
        Node->Values[10] = stInfo.ulMediaLength;
        Node->Values[11] = REC_TYPE_IPH;
        Node->Values[12] = GetTextIP(stInfo.aucDCAddr);

        if (strTime.Length() != 19)
        {
            continue;
        }

        if (strTime > m_strStartTime)
        {
            m_strStartTime = strTime;
        }

        if (strTime < m_strEndTime)
        {
            m_strEndTime = strTime;
        }
    }

    tlMain->EndUpdate();
}

void __fastcall TFormRecManage::cbRecTypePropertiesChange(TObject *Sender)
{
    UINT32      ulRecType = REC_TYPE_PTT;

    g_DictRecType.GetID(&ulRecType);

    edtCaller->Enabled = false;
    edtCallee->Enabled = false;

    ColumnCaller->Visible = true;
    ColumnCallee->Visible = true;

    if (ulRecType == REC_TYPE_IPH)
    {
        LabelResource->Caption = "车体号";

        ColumnResource->Caption->Text = "调度台";
        ColumnCaller->Caption->Text = "车体号";
        ColumnCallee->Caption->Text = "车厢号";
    }
    else
    {
        if (ulRecType != REC_TYPE_PTT)
        {
            edtCaller->Enabled = true;
            edtCallee->Enabled = true;

            LabelResource->Caption = "资源号";

            ColumnResource->Caption->Text = "资源号";

            ColumnCaller->Caption->Text = "主叫";
            ColumnCallee->Caption->Text = "被叫";
        }
        else
        {
            LabelResource->Caption = "群组号";

            ColumnResource->Caption->Text = "群组号";

            ColumnCaller->Visible = false;
            ColumnCallee->Visible = false;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::ColumnFileGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[3].IsNull())
    {
        return;
    }

    AnsiString  strFile = ANode->Values[3];
    Value = GetRecFileName(strFile);
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::TimerCheckTimer(TObject *Sender)
{
    CHAR    acFile[1024];
    BOOL    bIsDirFile;
    HANDLE  hDir = NULL;
    UINT32  ulFileCreateTime;
    UINT32  ulFileModifyTime;
    UINT32  ulKeepTime = 3600;

    hDir = gos_open_dir(m_acTmpRecPath);
    if (!hDir)
    {
        return;
    }

    while(gos_get_next_file(hDir, acFile, &bIsDirFile))
    {
        if (bIsDirFile)
        {
            continue;
        }

        ulFileCreateTime = 0;
        gos_get_file_time(acFile, &ulFileCreateTime, &ulFileModifyTime);
        if ((ulFileCreateTime + ulKeepTime) < gos_get_current_time())
        {
            gos_delete_file(acFile);
        }
    }

    gos_close_dir(hDir);
}
//---------------------------------------------------------------------------

