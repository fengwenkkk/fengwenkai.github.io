//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmRecManage.h"
#include "FrmLogin.h"
#include "main.h"
#include "GTransFile.h"
#include "ThreadDownload.h"
#include "DlgExportRecFile.h"
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
#pragma link "FolderDialog"
#pragma resource "*.dfm"
TFormRecManage *FormRecManage;

static const UINT32 m_ulRecFileTypeAudio = 1;   // 录音
static const UINT32 m_ulRecFileTypeVedio = 2;   // 录像
static const UINT32 m_ulRecFileTypeAll = m_ulRecFileTypeAudio | m_ulRecFileTypeVedio;   // 所有

static ID_VALUE_T   s_astRecType[] = {
                        {REC_CALL_TYPE_P2P_AUDIO,   "语音个呼"},
                        {REC_CALL_TYPE_P2P_VIDEO,   "视频"},
                        //{REC_CALL_TYPE_VIDEO_UPLOAD,"视频回传"},
                        {REC_CALL_TYPE_PTT_AUDIO,   "语音组呼"},
                        //{REC_CALL_TYPE_PTT_VIDEO,   "视频组呼"},
                        {REC_CALL_TYPE_IPH,         "乘客对讲"},
                        {REC_CALL_TYPE_REAL_BRD,    "实时广播"},
                                };

static Dict g_DictRecType(s_astRecType, ARRAY_SIZE(s_astRecType));

static bool     m_bChangingPos = false;

static AnsiString   m_strStartTime = "";
static AnsiString   m_strEndTime = "";

static VECTOR<REC_FILE_INFO_T>  g_vRecFileInfo;

UINT32  g_ulGetDCRecFileSeqID = 0;

std::map<AnsiString, TcxTreeListNode*>  g_mFileNodeMap;

VECTOR<DOWNLOAD_FILE_INFO_T>   g_vDownloadFile;

extern BOOL DownloadFileFromServer(char *szRemoteFile, char *szLocalFile);

BOOL IsAudioFile(CHAR *FileType)
{

    /*if (memcmp(FileType, "wav", sizeof("wav")) == 0 ||
        memcmp(FileType, "mp3", sizeof("mp3")) == 0 ||
        memcmp(FileType, "amr", sizeof("mp3")) == 0  ) */

    if (memcmp(FileType, "wav", sizeof("wav")) == 0 ||
        memcmp(FileType, "mp3", sizeof("mp3")) == 0 ||
        memcmp(FileType, "amr", sizeof("mp3")) == 0  )
    {
        return TRUE;
    }

    return FALSE;
}

BOOL IsAudioCall(UINT32 ulCallTypee)
{
    if (ulCallTypee == REC_CALL_TYPE_P2P_AUDIO  ||
        ulCallTypee == REC_CALL_TYPE_PTT_AUDIO  ||
        ulCallTypee == REC_CALL_TYPE_IPH_AUDIO  ||
        ulCallTypee == REC_CALL_TYPE_REAL_BRD )
    {
        return TRUE;
    }

    return FALSE;
}

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

AnsiString GetRecFileName(AnsiString strURL)
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

BOOL GetRecFileList(UINT32 ulCallType, INT64 i64ResourceID,
               UINT32 ulStartTime, UINT32 ulEndTime,
               VECTOR<REC_FILE_INFO_T> &vRecFileInfo)
{
    GJsonParam  ReqJsonParam;
    GJson       RspJson;

    if (0 == ulEndTime)
    {
        ulEndTime = 0xffffffff;
    }

    ReqJsonParam.Add("StartTime", ulStartTime);
    ReqJsonParam.Add("EndTime", ulEndTime);
    ReqJsonParam.Add("CallType", ulCallType);
    ReqJsonParam.Add("Caller", i64ResourceID);
    ReqJsonParam.Add("Callee", i64ResourceID);
    ReqJsonParam.Add("GroupID", i64ResourceID);

    if (!SyncSendServer(EV_CLIENT_REQ, MSG_REC_GetRecFileList, ReqJsonParam, RspJson))
    {
        return FALSE;
    }

    vRecFileInfo.clear();

    if (RspJson.GetSubJsonCount() < 1)
    {
        return FALSE;
    }

    for (UINT32 i=0; i<RspJson.GetSubJsonCount(); i++)
    {
        GJson   *SubJson = RspJson.GetSubJson(i);
        REC_FILE_INFO_T     stRec = {0};

        if (!SubJson->GetValue("file",  stRec.acFile, sizeof(stRec.acFile))     ||
            !SubJson->GetValue("file_suffix", stRec.acFileSuffix, sizeof(stRec.acFileSuffix))     ||
            !SubJson->GetValue("time",      &stRec.ulTime)     ||
            !SubJson->GetValue("type",      &stRec.ulCallType) ||
            !SubJson->GetValue("len",       &stRec.dDuration)  ||
            !SubJson->GetValue("size",      &stRec.ulFileSize) ||
            !SubJson->GetValue("caller",    &stRec.ulCaller)   ||
            !SubJson->GetValue("callee",    &stRec.ulCallee)   ||
            !SubJson->GetValue("group_id",  &stRec.i64GroupID) ||
            !SubJson->GetValue("dc_user",   &stRec.ulDCUserID) ||
            !SubJson->GetValue("train_id",  &stRec.ulTranUnitID) )
        {
            return FALSE;
        }

        vRecFileInfo.push_back(stRec);
    }

    return TRUE;
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
    g_DictRecType.SetID(REC_CALL_TYPE_PTT_AUDIO);

    UINT32  ulTime = gos_get_current_time();

//  SetTime(deStartTime, ulTime - 24*3600);

    btnPause->Visible = false;
    btnPlay->Visible = true;

 // InitMsgHandler(EV_CLIENT_RSP, MSG_DCGetRecFileListRsp, OnGetRecFileListRsp);

    g_ulGetDCRecFileSeqID = gos_get_rand();

    sprintf(m_acTmpRecPath, "%s/%s", gos_get_root_path(), "tmp_rec");
    gos_delete_dir(m_acTmpRecPath);
    gos_delete_file(m_acTmpRecPath);
    gos_create_dir(m_acTmpRecPath);

    gbMain->Width = 1000;

    Application->CreateForm(__classid(TDialogExportRecFile), &DialogExportRecFile);
}

//---------------------------------------------------------------------------
void __fastcall TFormRecManage::btnQueryClick(TObject *Sender)
{
    UINT32  ulStartTime = GetTime(deStartTime->Date);
    UINT32  ulEndTime = GetTime(deEndTime->Date);
    UINT32  ulRecType;
    UINT32  ulSendUserID;
    AnsiString  strTmp;
    INT64   i64ResourceID = 0;

    g_DictRecType.GetID(cbRecType, &ulRecType);

    tlMain->Clear();
    g_mFileNodeMap.clear();

    strTmp = edtResourceID->Text.Trim();

    VECTOR<REC_FILE_INFO_T>     vRecFileInfo;

    if (!GetRecFileList(ulRecType, i64ResourceID,
                        ulStartTime, ulEndTime, vRecFileInfo))
    {
        InfoMsg("查询失败,请检查" + LabelResource->Caption);
        return;
    }

    vRecFileInfo.swap(g_vRecFileInfo);
    InitView(ulRecType, g_vRecFileInfo);
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::InitView(UINT32 ulRecType, VECTOR<REC_FILE_INFO_T> &vInfo)
{
    TcxTreeListNode     *Node;
    AnsiString          strTime;
    AnsiString          strFile;
    UINT32              ulTime;
    UINT32              ulAudioImageIndex = 0;
    UINT32              ulVideoImageIndex = 1;

    m_strStartTime = "";
    m_strEndTime = gos_get_text_time(NULL);

    tlMain->BeginUpdate();
    tlMain->Clear();
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        Node = tlMain->Add();

        strFile = vInfo[i].acFile;
        strTime = GetTextTime(vInfo[i].ulTime);

        Node->Values[0] = i;
        Node->Values[1] = strTime;
        Node->Values[2] = IsAudioFile(vInfo[i].acFileSuffix)?ulAudioImageIndex:ulVideoImageIndex;
        Node->Values[3] = strFile;
        Node->Values[4] = vInfo[i].ulDCUserID;
        Node->Values[5] = vInfo[i].ulTranUnitID;
        Node->Values[6] = vInfo[i].ulCallee;
        Node->Values[8] = vInfo[i].dDuration;
        Node->Values[9] = "";

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
void __fastcall TFormRecManage::ExportMultiRecFile(bool bExportAll)
{
    CHAR        acFile[1024];
    CHAR        *szFileType;
    UINT32      ulIndex;
    UINT32      ulRecType;
    AnsiString  strFile;
    AnsiString  strLocalFile = "";
    AnsiString  strFileType = "";
    CHAR        acLocalFile[1024];
    std::map<AnsiString, TcxTreeListNode*>       mInfo;
    TcxTreeListNode *Node;

    g_vDownloadFile.clear();

    tlMain->BeginUpdate();

    for (int i=0; i<tlMain->AbsoluteCount; i++)
    {
        Node = tlMain->AbsoluteItems[i];

        if (bExportAll || Node->Selected)
        {
            Node->Values[9] = "待导出";
        }
    }

    BOOL    bOpen = FALSE;
    DOWNLOAD_FILE_INFO_T    stDownloadFile;

    for (int i=0; i<tlMain->AbsoluteCount; i++)
    {
        Node = tlMain->AbsoluteItems[i];
        if (!bExportAll && !Node->Selected)
        {
            continue;
        }

        if (Node->Values[0].IsNull())
        {
            Node->Values[9] = "文件信息不全";
            continue;
        }

        ulIndex = Node->Values[0];
        if (ulIndex >= g_vRecFileInfo.size())
        {
            Node->Values[9] = "文件信息错误";
            continue;
        }

        StrCpy(acFile, g_vRecFileInfo[ulIndex].acFile);
        ulRecType = g_vRecFileInfo[ulIndex].ulCallType;
        strFile = GetRecFileName(acFile);
        sprintf(acLocalFile, "%s", strFile.c_str());

        memset(&stDownloadFile, 0, sizeof(stDownloadFile));
        stDownloadFile.ulRecType = ulRecType;
        strcpy(stDownloadFile.acFile, acFile);
        strcpy(stDownloadFile.acLocalFile, acLocalFile);
        g_vDownloadFile.push_back(stDownloadFile);

        mInfo[stDownloadFile.acFile] = Node;
    }

    tlMain->EndUpdate();

    DialogExportRecFile->Init(g_vDownloadFile);
    DialogExportRecFile->ShowModal();

    VECTOR<DOWNLOAD_FILE_INFO_T>    vInfo;

    DialogExportRecFile->Get(vInfo);

    tlMain->BeginUpdate();

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        Node = mInfo[vInfo[i].acFile];
        if (Node)
        {
            Node->Values[9] = vInfo[i].acResult;
        }
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
bool __fastcall TFormRecManage::DownloadFile(AnsiString &strLocalFile, AnsiString &strFileType)
{
    if (tlMain->SelectionCount == 0)
    {
        return false;
    }

    TcxTreeListNode *Node = tlMain->Selections[0];

    if (Node->Values[0].IsNull()  )
    {
        return false;
    }

    CHAR        acFile[256];
    CHAR        *szFileType;
    UINT32      ulIndex = Node->Values[0];
    UINT32      ulRecType;
    static CHAR         acLocalFile[256];
    static AnsiString   strLastDownloadFile = "";

    if (ulIndex >= g_vRecFileInfo.size())
    {
        return false;
    }

    StrCpy(acFile, g_vRecFileInfo[ulIndex].acFile);
    ulRecType = g_vRecFileInfo[ulIndex].ulCallType;
    szFileType = gos_get_file_postfix(acFile);

    if (strLocalFile.IsEmpty())
    {
        sprintf(acLocalFile, "%s/tmp_%u.%s", m_acTmpRecPath, gos_get_uptime_1ms(), szFileType);

    }
    else
    {
        StrCpy(acLocalFile, strLocalFile);
    }

    gos_delete_file(acLocalFile);

    GosLog(LOG_DETAIL, "download remote file %s", acFile);

    if (!DownloadFileFromServer(acFile, acLocalFile))
    {
        GosLog(LOG_ERROR, "download remote file %s to local %s failed", acFile, acLocalFile);
        //  InfoMsg("下载失败");
        return false;
    }

    GosLog(LOG_DETAIL, "download remote file %s succ", acFile);

    strFileType = szFileType;
    strFileType = strFileType.LowerCase();
    strLocalFile = acLocalFile;

    return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::pmiExportAllClick(TObject *Sender)
{
    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    ExportMultiRecFile(true);
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::pmiExportClick(TObject *Sender)
{
    if (tlMain->SelectionCount > 1)
    {
        ExportMultiRecFile(false);
        return;
    }

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
    AnsiString  strFileName = "";
    AnsiString  strLocalFile = "";
    AnsiString  strFileType = "";

    if (ulIndex >= g_vRecFileInfo.size())
    {
        return;
    }

    strFileName = g_vRecFileInfo[ulIndex].acFile;
    strFileType = gos_get_file_postfix(g_vRecFileInfo[ulIndex].acFile);
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
    else if (strFileType == "mkv")
    {
        SaveDialog->Filter = "*.mkv|*.mkv";
        SaveDialog->DefaultExt = "*.mkv";
    }

    strFileName = GetRecFileName(strFileName);
    SaveDialog->FileName = GetRecFileName(strFileName);

    if (!SaveDialog->Execute() || SaveDialog->FileName.IsEmpty())
    {
        return;
    }

    strFileName = SaveDialog->FileName;
    if (!DownloadFile(strFileName, strFileType))
    {
        InfoMsg("导出失败");
        return;
    }

    InfoMsg("导出成功");
}
//---------------------------------------------------------------------------
void __fastcall TFormRecManage::FormCreate(TObject *Sender)
{
    SetImage(btnQuery, IMG_QUERY);
    SetImage(btnQueryPrev, IMG_PREV);
    SetImage(btnQueryNext, IMG_NEXT);

    SetImage(pmiPlay, IMG_PLAY);
    SetImage(pmiExport, IMG_EXPORT);
    SetImage(pmiExportAll, IMG_EXPORT);

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

    if (WindowsMediaPlayer->playState == WMPPlayState::wmppsPaused &&
        strCurrPlayingFile == strSelectFile)
    {
        WindowsMediaPlayer->controls->play();
        TimerPlay->Enabled = true;

        return;
    }

    if (!strCurrPlayingFile.IsEmpty() &&
        strCurrPlayingFile != strSelectFile)
    {
        btnStopClick(this);
    }

    PanelSide1->Caption = "正在缓存";
    PanelSide1->Update();
    PanelSide1->Invalidate();
    gos_sleep_1ms(1);

    AnsiString  strLocalFile = "";
    AnsiString  strFileType = "";

    if (!DownloadFile(strLocalFile, strFileType))
    {
        PanelSide1->Caption = "";
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
    WindowsMediaPlayer->fullScreen =  false;
    WindowsMediaPlayer->enableContextMenu = false;
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
        tbPlay->Hint = WindowsMediaPlayer->controls->currentPositionString;
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
    PanelSide1->Caption = "";

    if (NewState == WMPPlayState::wmppsPlaying)
    {
        tbPlay->ShowHint = true;
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

        tbPlay->ShowHint = false;
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
    String  strURL = WindowsMediaPlayer->URL;

    if (!WindowsMediaPlayer->currentMedia ||
        WindowsMediaPlayer->playState == WMPPlayState::wmppsStopped ||
        strURL == "")
    {
        PanelSide1->Align = alClient;
        PanelSide1->Caption = "";
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

void __fastcall TFormRecManage::cbRecTypePropertiesChange(TObject *Sender)
{
    UINT32      ulRecType = REC_CALL_TYPE_PTT_AUDIO;

    g_DictRecType.GetID(&ulRecType);

    ColumnCaller->Visible = true;
    ColumnCallee->Visible = true;

    if (ulRecType == REC_CALL_TYPE_IPH)
    {
        LabelResource->Caption = "车体号";

        ColumnResource->Caption->Text = "调度台";
        ColumnCaller->Caption->Text = "车体号";
        ColumnCallee->Caption->Text = "车厢号";

        ColumnCallee->Visible = false;
        ColumnIPH->Visible = false;

    }
    else if(ulRecType == REC_CALL_TYPE_REAL_BRD)
    {
        LabelResource->Caption = "调度台ID";

        ColumnResource->Caption->Text = "调度台";

        ColumnIPH->Visible      = false;
        ColumnCallee->Visible   = false;
        ColumnCaller->Visible   = false;
    }
    else
    {
        if (ulRecType != REC_CALL_TYPE_PTT_AUDIO &&
            ulRecType != REC_CALL_TYPE_PTT_VIDEO )
        {
            ColumnCaller->Visible   = false;
            ColumnIPH->Visible      = false;
            ColumnResource->Visible = false;


            LabelResource->Caption = "主叫设备号码";

            //ColumnResource->Caption->Text = "用户号码";
            ColumnCallee->Caption->Text = "被叫";
        }
        else
        {
            LabelResource->Caption = "群组号码";

            ColumnResource->Caption->Text = "群组号码";

            ColumnIPH->Visible    = false;
            ColumnCaller->Visible = false;
            ColumnCallee->Visible = false;
            ColumnResource->Visible = false;

        }
    }

    tlMain->Clear();
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
void __fastcall TFormRecManage::PanelPlayCtrlResize(TObject *Sender)
{
    tbPlay->Width = LabelMediaLength->Left - 80 - tbPlay->Left;
}
//---------------------------------------------------------------------------

void __fastcall TFormRecManage::ColumnResourceGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[4].IsNull())
    {
        return;
    }

    INT64   i64Value = ANode->Values[4];

    if (i64Value == 0)
    {
        Value = "";
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormRecManage::ColumnCallerGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[5].IsNull())
    {
        return;
    }

    UINT32  ulValue = ANode->Values[5];

    if (ulValue == 0)
    {
        Value = "";
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormRecManage::ColumnCalleeGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[6].IsNull())
    {
        return;
    }

    UINT32  ulValue = ANode->Values[6];

    if (ulValue == 0)
    {
        Value = "";
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormRecManage::ColumnDurationGetDisplayText(TcxTreeListColumn *Sender,
          TcxTreeListNode *ANode, UnicodeString &Value)
{
    if (ANode->Values[8].IsNull())
    {
        return;
    }

    double  dValue = ANode->Values[8];
    char    acValue[32];

    sprintf(acValue, "%.1f", dValue);
    Value = acValue;
}
//---------------------------------------------------------------------------


