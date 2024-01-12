//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "FrmLocalRecManage.h"
#include "FrmLogin.h"
#include "main.h"
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
#pragma link "cxImage"
#pragma link "dxGDIPlusClasses"
#pragma link "cxSplitter"
#pragma resource "*.dfm"
TFormLocalRecManage *FormLocalRecManage;

extern BOOL HttpDownloadFile(AnsiString strUrl, AnsiString strLocalFile);

static const UINT32 m_ulCallTypeP2P = 1;    // 个呼
static const UINT32 m_ulCallTypePTT = 2;    // 组呼
static const UINT32 m_ulCallTypeIPH = 4;    // IPH
static const UINT32 m_ulCallTypeAll = m_ulCallTypeP2P | m_ulCallTypePTT | m_ulCallTypeIPH;  // 所有

// TODO  改成宏定义
static const UINT32 m_ulRecTypeAudio = 1;   // 录音
static const UINT32 m_ulRecTypeVedio = 2;   // 录像
static const UINT32 m_ulRecTypeAll = m_ulRecTypeAudio | m_ulRecTypeVedio;   // 所有

static ID_VALUE_T   s_astCallType[] = { {m_ulCallTypeAll,   "所有"},
                                        {m_ulCallTypePTT,   "组呼"},
                                        {m_ulCallTypeP2P,   "个呼"},
                                        {m_ulCallTypeIPH,   "IPH"},
                                        };

static ID_VALUE_T   s_astRecType[] = {  {m_ulRecTypeAll,    "所有"},
                                        {m_ulRecTypeAudio,  "录音"},
                                        {m_ulRecTypeVedio,  "录像"}};

static Dict g_DictCallType(s_astCallType, ARRAY_SIZE(s_astCallType));
static Dict g_DictRecType(s_astRecType, ARRAY_SIZE(s_astRecType));

static VECTOR<LOCAL_REC_INFO_T>        g_vRecInfo;
static bool m_bAutoChangePos = true;

TRect g_stDisplayRect;
static bool     m_bChangingPos = false;

UINT32  g_ulMaxGroupCallRecordTime = 300;       // 组呼最大记录时长
UINT32  g_ulMaxGroupCallRecordIdleTime = 15;    // 组呼无话权人最大等待时间，超过此值结束本次录音

extern CHAR     g_acNativeServicePath[256];
extern UINT8    g_aucDISClientAddr[4];

extern BOOL SendServer(UINT16 usMsgID, const CHAR *szMsgName, UINT32 ulSeqID, GJsonTupleParam &Param);

extern UINT32 GetWavFileMediaLength(CHAR *szFile);
extern int GetMediaFileLength(char *szFile);

typedef struct
{
    UINT32  ulRecType;
    UINT32  ulRecCallType;
    UINT32  ulStartTime;
    UINT32  ulLastTime;
}REC_INFO_T;

RawIntMap<REC_INFO_T>       g_mP2PCallRecInfo;
RawInt64Map<REC_INFO_T>     g_mGroupCallRecInfo;

void StartP2PCallRecord(UINT32 ulUserID, UINT32 ulRecType, UINT32 ulRecCallType)
{
    if (g_mP2PCallRecInfo.Has(ulUserID))
    {
        return;
    }

    REC_INFO_T  stRecInfo = {0};

    stRecInfo.ulRecType = ulRecType;
    stRecInfo.ulRecCallType = ulRecCallType;
    stRecInfo.ulStartTime = gos_get_uptime();

    if (!eSDK_StartRecord(ulUserID, ulRecType, ulRecCallType))
    {
        return;
    }

    g_mP2PCallRecInfo.Add(ulUserID, &stRecInfo);
}

void StopP2PCallRecord(UINT32 ulUserID)//, UINT32 ulRecType, UINT32 ulRecCallType)
{
    REC_INFO_T  *pstRecInfo = g_mP2PCallRecInfo.GetValue(ulUserID);

    if (!pstRecInfo)
    {
        return;
    }

    if (!eSDK_StopRecord(ulUserID, pstRecInfo->ulRecType, pstRecInfo->ulRecCallType))
    {
        return;
    }

    g_mP2PCallRecInfo.Del(ulUserID);
}

void StartGroupCallRecord(INT64 i64GroupID, UINT32 ulRecType)
{
    REC_INFO_T  *pstRecInfo = g_mGroupCallRecInfo.GetValue(i64GroupID);

    if (pstRecInfo)
    {
        pstRecInfo->ulLastTime = gos_get_uptime();

        return;
    }

    REC_INFO_T  stRecInfo;

    stRecInfo.ulRecType = ulRecType;
    stRecInfo.ulRecCallType = REC_CALL_TYPE_PTT;
    stRecInfo.ulStartTime = gos_get_uptime();
    stRecInfo.ulLastTime = stRecInfo.ulStartTime;

    if (!eSDK_StartRecord(i64GroupID, ulRecType, stRecInfo.ulRecCallType))
    {
        return;
    }

    g_mGroupCallRecInfo.Add(i64GroupID, &stRecInfo);
}

// 检查并判断是否需要另存为新的
void UpdateGroupCallRecord(INT64 i64GroupID)
{
    REC_INFO_T  *pstRecInfo = g_mGroupCallRecInfo.GetValue(i64GroupID);

    if (!pstRecInfo)
    {
        return;
    }

    // 单次组呼录音最大时长
    UINT32  ulTime = gos_get_uptime();
    UINT32  ulRecType = pstRecInfo->ulRecType;

    if ((ulTime - pstRecInfo->ulStartTime) > g_ulMaxGroupCallRecordTime ||
        (ulTime - pstRecInfo->ulLastTime)  > g_ulMaxGroupCallRecordIdleTime )
    {
        if (!eSDK_StopRecord(i64GroupID, ulRecType, pstRecInfo->ulRecCallType))
        {
            return;
        }

        g_mGroupCallRecInfo.Del(i64GroupID);

        StartGroupCallRecord(i64GroupID, ulRecType);
    }
}

void StopGroupCallRecord(INT64 i64GroupID)
{
    REC_INFO_T  *pstRecInfo = g_mGroupCallRecInfo.GetValue(i64GroupID);

    if (!pstRecInfo)
    {
        return;
    }

    if (!eSDK_StopRecord(i64GroupID, pstRecInfo->ulRecType, pstRecInfo->ulRecCallType))
    {
        return;
    }

    g_mGroupCallRecInfo.Del(i64GroupID);
}

void SetTime(TcxDateEdit *edt, UINT32 ulTime)
{
    edt->Date = GetDateTime(ulTime);
}

void SetTime(TcxDateEdit *edt, AnsiString strTime)
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

CHAR* GetCallType(UINT32 ulCallType)
{
    for (UINT32 i=0; i<ARRAY_SIZE(s_astCallType); i++)
    {
        if (s_astCallType[i].ulID == ulCallType)
        {
            return s_astCallType[i].szValue;
        }
    }

    return "";
}

CHAR* GetRecType(UINT32 ulRecType)
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

CHAR* GetFileDisplayName(CHAR *szFile)
{
    CHAR    *szTmp = gos_right_strchr(szFile, '\\');

    if (!szTmp)
    {
        return szFile;
    }
    else
    {
        return szTmp+1;
    }
}

INT32 GetImageIndex(AnsiString &strFile)
{
    int     iLen = strFile.Length();
    CHAR    *szEnd = strFile.c_str() + iLen;

    if (iLen > 4 && strcasecmp(szEnd-4, ".wav") == 0)
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
}  */

bool ParseRecFile(char *szFile, LOCAL_REC_INFO_T &stRec)
{
    // 12000_20200410_184058277.wav 组呼
    // 12000_20200410184058277.MP4 视频
    // 1001_1002_20200410_184058277.wav 个呼
    CHAR        *szTmp;
    char        acTime[32];
    char        acFile[256];

    memset(&stRec, 0, sizeof(stRec));
    strcpy(stRec.acFile, szFile);

    szTmp = gos_right_strchr(szFile, '\\');
    if (!szTmp)
    {
        return false;
    }

    strcpy(acFile, szTmp+1);

    // 获取文件格式
    szTmp = gos_right_strchr(acFile, '.');
    if (!szTmp)
    {
        return false;
    }

    if (strcasecmp(szTmp+1, "wav") == 0)
    {
        stRec.ulRecType = m_ulRecTypeAudio;
    }
    else if (strcasecmp(szTmp+1, "mp4") == 0)
    {
        stRec.ulRecType = m_ulRecTypeVedio;
    }
    else
    {
        return false;
    }

    *szTmp = '\0';

    // 获取时间
    szTmp = gos_right_strchr(acFile, '_');
    if (!szTmp)
    {
        return false;
    }

    // _20200410184058277  or _20200410_184058
    UINT32  ulLen = strlen(szTmp);

    if (ulLen != 15 &&
        ulLen != 10 )
    {
        return false;
    }

    memset(acTime, 0, sizeof(acTime));
    if (ulLen == 10)
    {
        memcpy(acTime+8, szTmp+1, 6);
        *szTmp = '\0';

        szTmp = gos_right_strchr(acFile, '_');
        if (!szTmp)
        {
            return false;
        }

        if (strlen(szTmp) != 9)
        {
            return false;
        }

        memcpy(acTime, szTmp+1, 8);
        *szTmp = '\0';
    }
    else // if (ulLen == 18)
    {
        memcpy(acTime, szTmp+1, 14);
        *szTmp = '\0';
    }

    stRec.ulTime = gos_compact_text_time_to_int(acTime);
    if (stRec.ulTime == 0xffffffff)
    {
        return false;
    }

    // 获取
    szTmp = strchr(acFile, '_');
    if (!szTmp)    // 组呼
    {
        /*
        if (!gos_atoi64(acFile, &stRec.i64ResourceID))
        {
            return false;
        } */
        strncpy(stRec.acResourceID, acFile, sizeof(stRec.acResourceID)-1);
        stRec.ulCallType = m_ulCallTypePTT;
    }
    else  // 个呼
    {
        UINT32      ulUserID;

        *szTmp = '\0';
        /*if (!gos_atou(acFile, &ulUserID))
        {
            return false;
        }

        stRec.i64ResourceID = ulUserID;*/
        strncpy(stRec.acResourceID, acFile, sizeof(stRec.acResourceID)-1);
        stRec.ulCallType = m_ulCallTypeP2P;
    }

    return true;
}

bool ParseIPHRecFile(char *szFile, LOCAL_REC_INFO_T &stRec)
{
// 1002_10104_3_2_20200702_100709.mp4
// 1002_10104_3_2_20200702_100709.wav
    CHAR        *szTmp;
    char        acTime[32];
    char        acFile[256];
    UINT32      ulLen;

    memset(&stRec, 0, sizeof(stRec));
    stRec.ulCallType = m_ulCallTypeIPH;
    strcpy(stRec.acFile, szFile);

    szTmp = gos_right_strchr(szFile, '\\');
    if (!szTmp)
    {
        return false;
    }

    strcpy(acFile, szTmp+1);

    // 获取文件格式
    szTmp = gos_right_strchr(acFile, '.');
    if (!szTmp)
    {
        return false;
    }

    if (strcasecmp(szTmp+1, "wav") == 0)
    {
        stRec.ulRecType = m_ulRecTypeAudio;
//      stRec.ulMediaLength = GetWavFileMediaLength(stRec.acFile);
    }
    else if (strcasecmp(szTmp+1, "mp4") == 0)
    {
        stRec.ulRecType = m_ulRecTypeVedio;
//        stRec.ulMediaLength = GetMP4FileMediaLength(stRec.acFile);
    }
    else
    {
        return false;
    }

    *szTmp = '\0';

    // 获取时间
    szTmp = gos_right_strchr(acFile, '_');
    if (!szTmp)
    {
        return false;
    }

    ulLen = strlen(szTmp);
    if (ulLen != 7)
    {
        return false;
    }

    memset(acTime, 0, sizeof(acTime));
    memcpy(acTime+8, szTmp+1, 6);

    *szTmp = '\0';
    szTmp = gos_right_strchr(acFile, '_');
    if (!szTmp)
    {
        return false;
    }

    ulLen = strlen(szTmp);
    if (ulLen != 9)
    {
        return false;
    }

    memcpy(acTime, szTmp+1, 8);
    stRec.ulTime = gos_compact_text_time_to_int(acTime);
    if (stRec.ulTime == 0xffffffff)
    {
        return false;
    }
    *szTmp = '\0';

    szTmp = gos_right_strchr(acFile, '_');
    if (!szTmp)
    {
        return false;
    }

    if (!gos_atou(szTmp+1, &stRec.ulIPHDevID))
    {
        return false;
    }
    *szTmp = '\0';

    szTmp = gos_right_strchr(acFile, '_');
    if (!gos_atou(szTmp+1, &stRec.ulRollingStockID))
    {
        return false;
    }
    *szTmp = '\0';

    szTmp = gos_right_strchr(acFile, '_');
    if (!gos_atou(szTmp+1, &stRec.ulTrainUnitID))
    {
        return false;
    }
    *szTmp = '\0';

    strncpy(stRec.acResourceID, acFile, sizeof(stRec.acResourceID)-1);

    return true;
}

AnsiString GetMediaLength(int iTime)
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

void __fastcall TFormLocalRecManage::InitSkin()
{
    tbPlay->Style->LookAndFeel->SkinName = g_strVolumeSkinName;
    WindowsMediaPlayer->uiMode = "none";
}
//---------------------------------------------------------------------------
__fastcall TFormLocalRecManage::TFormLocalRecManage(TComponent* Owner)
    : TSubForm(Owner)
{
    g_DictRecType.Init(cbRecType);
    g_DictCallType.Init(cbCallType);

    g_DictCallType.SetID(m_ulCallTypeAll);
    g_DictRecType.SetID(m_ulRecTypeAll);

    UINT32  ulTime = gos_get_current_time();

    SetTime(deStartTime, ulTime - 24*3600);

    btnPause->Visible = false;
    btnPlay->Visible = true;

    // REC CLI
    InitMsgHandler(EV_DC_DIS_RSP, MSG_DCGetRecFileListReq, OnGetRecFileListReq);
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::btnQueryClick(TObject *Sender)
{
    UINT32  ulStartTime = GetTime(deStartTime->Date);
    UINT32  ulEndTime = 0xffffffff;
    UINT32  ulRecType;
    UINT32  ulCallType;
    AnsiString  strTmp;
    INT64   i64ResourceID = -1;

    gbPlay->Visible = false;

    if (!deEndTime->Text.IsEmpty())
    {
        ulEndTime = GetTime(deEndTime->Date);
    }

    g_DictRecType.GetID(&ulRecType);
    g_DictCallType.GetID(&ulCallType);

    strTmp = edtResourceID->Text.Trim();
    if (!strTmp.IsEmpty())
    {
        if (!gos_atoi64(strTmp.c_str(), &i64ResourceID) ||
            i64ResourceID <= 0)
        {
            edtResourceID->SetFocus();
            InfoMsg("号码非法");
            return;
        }
    }

    CHAR        acRecFileDir[256];

    sprintf(acRecFileDir, "%s\\%s", g_acNativeServicePath, "data\\rec");
    g_vRecInfo.clear();
    if (!GetLocalRecFileList(acRecFileDir, ulStartTime, ulEndTime, ulRecType, ulCallType, strTmp.c_str()))
    {
        InfoMsg("查询失败");
        return;
    }

    if (ulCallType & m_ulCallTypeIPH)
    {
        sprintf(acRecFileDir, "%s\\%s", g_acNativeServicePath, "data\\iph_rec");
        if (!GetIPHRecFileList(acRecFileDir, ulStartTime, ulEndTime, ulRecType))
        {
            InfoMsg("查询失败");
            return;
        }
    }

    InitView();
}
//---------------------------------------------------------------------------
bool __fastcall TFormLocalRecManage::GetLocalRecFileList(CHAR *szDir, UINT32 ulStartTime, UINT32 ulEndTime,
                                                UINT32 ulRecType, UINT32 ulCallType, CHAR *szResourceID)
{
    HANDLE      hDir;
    CHAR        acFile[256];
    BOOL        bIsDir;
    LOCAL_REC_INFO_T    stRec;

    if (!gos_file_exist(szDir))
    {
        return true;
    }

    hDir = gos_open_dir(szDir);
    if (!hDir)
    {
        GosLog(LOG_ERROR, "open dir %s failed", szDir);
        return false;
    }

    while (gos_get_next_file(hDir, acFile, &bIsDir))
    {
        if (bIsDir)
        {
            GetLocalRecFileList(acFile, ulStartTime, ulEndTime, ulRecType, ulCallType, szResourceID);
            continue;
        }

        if (szResourceID && *szResourceID != '\0')
        {
            if (strstr(acFile, szResourceID) == NULL)
            {
                continue;
            }
        }

        if (!ParseRecFile(acFile, stRec))
        {
            continue;
        }

        if (stRec.ulTime < ulStartTime ||
            stRec.ulTime > ulEndTime )
        {
            continue;
        }

        if ((ulRecType & stRec.ulRecType) &&
            (ulCallType & stRec.ulCallType) )
        {
            if (szResourceID && *szResourceID != '\0')
            {
                if (strstr(stRec.acResourceID, szResourceID) == NULL)
                {
                    continue;
                }
            }

            g_vRecInfo.push_back(stRec);
        }
    }

    gos_close_dir(hDir);

    return true;
}
//---------------------------------------------------------------------------
bool __fastcall TFormLocalRecManage::GetIPHRecFileList(CHAR *szDir, UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulRecType)
{
    HANDLE      hDir;
    CHAR        acFile[256];
    BOOL        bIsDir;
    LOCAL_REC_INFO_T    stRec;

    if (!gos_file_exist(szDir))
    {
        return true;
    }

    hDir = gos_open_dir(szDir);
    if (!hDir)
    {
        GosLog(LOG_ERROR, "open dir %s failed", szDir);
        return false;
    }

    while (gos_get_next_file(hDir, acFile, &bIsDir))
    {
        if (bIsDir)
        {
            GetIPHRecFileList(acFile, ulStartTime, ulEndTime, ulRecType);
            continue;
        }

        if (!ParseIPHRecFile(acFile, stRec))
        {
            continue;
        }

        if (stRec.ulTime < ulStartTime ||
            stRec.ulTime > ulEndTime )
        {
            continue;
        }

        if (ulRecType & stRec.ulRecType)
        {
            g_vRecInfo.push_back(stRec);
        }
    }

    gos_close_dir(hDir);

    return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::InitView()
{
    TcxTreeListNode     *Node;

    tlMain->BeginUpdate();
    tlMain->Clear();
    for (UINT32 i=0; i<g_vRecInfo.size(); i++)
    {
        Node = tlMain->Add();

        Node->Values[0] = GetTextTime(g_vRecInfo[i].ulTime);
        Node->Values[1] = GetCallType(g_vRecInfo[i].ulCallType);
        Node->Values[2] = g_vRecInfo[i].ulRecType;
        Node->Values[3] = g_vRecInfo[i].acResourceID;
        Node->Values[4] = GetFileDisplayName(g_vRecInfo[i].acFile);
        Node->Values[5] = g_vRecInfo[i].acFile;
    }

    tlMain->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::tlMainDblClick(TObject *Sender)
{
    btnPlayClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::pmiPlayClick(TObject *Sender)
{
    btnPlayClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::pmiExportClick(TObject *Sender)
{
    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlMain->Selections[0];

    if (Node->Values[5].IsNull() )
    {
        return;
    }

    CHAR        acFile[256];

    StrCpy(acFile, Node->Texts[5]);

    CHAR        *szFileType = gos_get_file_postfix(acFile);

    SaveDialog->Filter = "*.*|*.*";
    SaveDialog->DefaultExt = "*.*";

    if (szFileType)
    {
        if (strcasecmp(szFileType, "wav") == 0)
        {
            SaveDialog->Filter = "*.wav|*.wav";
            SaveDialog->DefaultExt = "*.wav";
        }
        else if (strcasecmp(szFileType, "mp4") == 0)
        {
            SaveDialog->Filter = "*.mp4|*.mp4";
            SaveDialog->DefaultExt = "*.mp4";
        }
    }

    if (!SaveDialog->Execute())
    {
        return;
    }

    AnsiString  strFile = SaveDialog->FileName;

    if (!gos_copy_file(acFile, strFile.c_str()))
    {
        ErrMsg("导出失败");
    }
    else
    {
        InfoMsg("导出成功");
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::FormCreate(TObject *Sender)
{
    SetImage(btnQuery, IMG_QUERY);
    SetImage(pmiPlay, IMG_PLAY);
    SetImage(pmiExport, IMG_EXPORT);

    SetImage(btnPlay, IMG_PLAY);
    SetImage(btnPause, IMG_PAUSE);
    SetImage(btnStop, IMG_STOP);
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::btnPlayClick(TObject *Sender)
{
    if (WindowsMediaPlayer->playState == WMPPlayState::wmppsPaused)
    {
        WindowsMediaPlayer->controls->play();
        TimerPlay->Enabled = true;

     // btnPlay->Visible = false;
//      btnPause->Visible = true;

        return;
    }

    if (tlMain->SelectionCount == 0)
    {
        return;
    }

    TcxTreeListNode *Node = tlMain->Selections[0];

    if (Node->Values[5].IsNull() )
    {
        return;
    }

    CHAR        acFile[256];
    UINT32      ulFileType = Node->Values[2];

    gbPlay->Visible = true;

    if (ulFileType == m_ulRecTypeAudio)
    {
        PanelPlay->Visible = false;
        gbPlay->Height = gbTrackBar->Height + 2;

        SplitterMain->Visible = false;
    }
    else
    {
        SplitterMain->Visible = true;
//      SplitterMain->Align = salTop;
//      SplitterMain->Align = salBottom;

        PanelPlay->Visible = true;
        gbPlay->Height = PanelPlay->Height + gbTrackBar->Height + 2;
    }

    StrCpy(acFile, Node->Texts[5]);

    PlayRecFile(acFile);
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::PlayRecFile(CHAR *szFile)
{
    WindowsMediaPlayer->URL = String(szFile); //edtURL->Text;
    WindowsMediaPlayer->controls->play();
    WindowsMediaPlayer->uiMode = "none";

//  btnPlay->Visible = false;
//  btnPause->Visible = true;

/*
    try
    {
        if(g_MediaPlayer->PlayState != vmpsPaused)
        {
            g_MediaPlayer->FileName = szFile;
        }

        g_MediaPlayer->AspectRatio = true; // 保持高宽比
        g_MediaPlayer->Play();

        tbPlay->Properties->Max = g_MediaPlayer->Duration;
        tbPlay->Position = 0;
        gbTrackBar->Enabled = (tbPlay->Properties->Max > 0);
        LabelMediaLength->Caption = GetMediaLength(tbPlay->Properties->Max);

        if (g_MediaPlayer->AudioOnly || tbPlay->Properties->Max <= 0)
        {
            PanelMediaPlayer->Visible = false;
        }
        else
        {
            PanelMediaPlayer->Visible = true;
        }

        int     iInterval = tbPlay->Properties->Max;

        if (iInterval <= 0)
        {
            TimerPlay->Enabled = false;
        }
        else
        {
            TimerPlay->Interval = iInterval;
            TimerPlay->Enabled = true;
        }
    }
    catch(Exception &e)
    {
    }

    if(!g_MediaPlayer->LavSplitterOK)
    {
        InfoMsg("未加载LAV解码器");
        return;
    }
*/

}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::btnPauseClick(TObject *Sender)
{
//  g_MediaPlayer->Pause();

    TimerPlay->Enabled = false;

    WindowsMediaPlayer->controls->pause();
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::btnStopClick(TObject *Sender)
{
//  g_MediaPlayer->Stop();

    WindowsMediaPlayer->controls->stop();

    TimerPlay->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::TimerPlayTimer(TObject *Sender)
{
//  if (g_MediaPlayer->PlayState != vmpsRunning)
    if (!m_bChangingPos && tbPlay)
    {
        tbPlay->Position = WindowsMediaPlayer->controls->currentPosition;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::PanelPlayResize(TObject *Sender)
{
//  g_MediaPlayer->AdjustVideoSize();
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::tbPlayMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    m_bChangingPos = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::tbPlayMouseUp(TObject *Sender, TMouseButton Button,
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
void __fastcall TFormLocalRecManage::TimerInitTimer(TObject *Sender)
{
    TimerInit->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::WindowsMediaPlayerPlayStateChange(TObject *Sender,
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

            int iInterval = 1000*iTime/tbPlay->Width;

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
        gbPlay->Visible = false;
    }
    else if (NewState == WMPPlayState::wmppsPaused)
    {
        btnPlay->Visible = true;
        btnPause->Visible = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::tbPlayPropertiesGetPositionHint(TObject *Sender,
          const int APosition, UnicodeString &AHintText, bool &ACanShow, bool &AIsHintMultiLine)
{
    AHintText = GetMediaLength(tbPlay->Position);
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::PanelMediaPlayerResize(TObject *Sender)
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
void __fastcall TFormLocalRecManage::FormClose(TObject *Sender, TCloseAction &Action)
{
    m_bChangingPos = true;
}

static int CompareFile(const void *t1, const void *t2)
{
    LOCAL_REC_INFO_T    *s = (LOCAL_REC_INFO_T*)t1;
    LOCAL_REC_INFO_T    *d = (LOCAL_REC_INFO_T*)t2;

    return s->ulTime - d->ulTime;
}

void SortRecInfo()
{
    UINT32  ulNum = g_vRecInfo.size();

    if (ulNum <= 1)
    {
        return;
    }

    LOCAL_REC_INFO_T        *pstRec = (LOCAL_REC_INFO_T*)calloc(ulNum, sizeof(LOCAL_REC_INFO_T));

    if (!pstRec)
    {
        return;
    }

    UINT32  i = 0;

    for (i=0; i<ulNum; i++)
    {
        memcpy(pstRec+i, &g_vRecInfo[i], sizeof(LOCAL_REC_INFO_T));
    }

    qsort(pstRec, ulNum, sizeof(LOCAL_REC_INFO_T), CompareFile);

    for (i=0; i<ulNum; i++)
    {
        memcpy(&g_vRecInfo[i], pstRec+i, sizeof(LOCAL_REC_INFO_T));
    }

    free(pstRec);
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::OnGetRecFileListReq(UINT32 ulSeqID, GJson &Json)
{
    UINT32      ulStartTime;
    UINT32      ulEndTime;
    UINT32      ulIPHDevID;
    CHAR        *szVideoURL;
    CHAR        acRecFileDir[256];
    UINT32      ulRecFileType = m_ulRecTypeAll;

    if (!Json.GetValue("StartTime", &ulStartTime))
    {
        GosLog(LOG_ERROR, "invalid StartTime in msg GetRecFileListReq");
        return;
    }

    if (!Json.GetValue("EndTime", &ulEndTime))
    {
        GosLog(LOG_ERROR, "invalid ulEndTime in msg GetRecFileListReq");
        return;
    }

 /* if (!Json.GetValue("RecFileType", &ulRecFileType))
    {
        GosLog(LOG_ERROR, "invalid RecFileType in msg GetRecFileListReq");
        return;
    } */

    if (ulEndTime == 0)
    {
        ulEndTime = 0xffffffff;
    }

    sprintf(acRecFileDir, "%s\\%s", g_acNativeServicePath, "data\\iph_rec");
    g_vRecInfo.clear();
    if (!GetIPHRecFileList(acRecFileDir, ulStartTime, ulEndTime, ulRecFileType))
    {
        return;
    }

    if (g_vRecInfo.size() == 0)
    {
        return;
    }

    // TODO对数据进行排序
    SortRecInfo();

    UINT32      ulOffset = strlen(g_acNativeServicePath)+1;

    GJsonTupleParam     JsonTupleParam;
    UINT32              i = 0;

    for (i=0; i<g_vRecInfo.size(); i++)
    {
        LOCAL_REC_INFO_T    &stInfo = g_vRecInfo[i];
        GJsonParam          JsonParam;

        JsonParam.Add("File", stInfo.acFile);
        JsonParam.Add("Time", stInfo.ulTime);
        JsonParam.Add("RecType", stInfo.ulRecType);
        JsonParam.Add("ResourceID", stInfo.acResourceID);
        JsonParam.Add("TrainUnitID", stInfo.ulTrainUnitID);
        JsonParam.Add("RollingStockID", stInfo.ulRollingStockID);
        JsonParam.Add("IPHDevID", stInfo.ulIPHDevID);
        JsonParam.AddIP("DCAddr", g_aucDISClientAddr);
        JsonParam.Add("MediaLength", "00000");

        JsonTupleParam.Add(JsonParam);

        if (JsonTupleParam.GetStringSize() > 30*1024 )
        {
            break;
        }
    }

    SendServer(EV_DC_DIS_REQ, MSG_DCGetRecFileListRsp, ulSeqID, JsonTupleParam);

    UINT32  ulNum = i;

    // 重新发送
    JsonTupleParam.Clear();

    for (i=0; i<ulNum; i++)
    {
        LOCAL_REC_INFO_T    &stInfo = g_vRecInfo[i];
        GJsonParam          JsonParam;

        if (stInfo.ulRecType == m_ulRecTypeAudio)
        {
            stInfo.ulMediaLength = GetWavFileMediaLength(stInfo.acFile);
        }
        else
        {
            stInfo.ulMediaLength = GetMediaFileLength(stInfo.acFile);
        }

        JsonParam.Add("File", stInfo.acFile);
        JsonParam.Add("Time", stInfo.ulTime);
        JsonParam.Add("RecType", stInfo.ulRecType);
        JsonParam.Add("ResourceID", stInfo.acResourceID);
        JsonParam.Add("TrainUnitID", stInfo.ulTrainUnitID);
        JsonParam.Add("RollingStockID", stInfo.ulRollingStockID);
        JsonParam.Add("IPHDevID", stInfo.ulIPHDevID);
        JsonParam.AddIP("DCAddr", g_aucDISClientAddr);
        JsonParam.Add("MediaLength", stInfo.ulMediaLength);

        JsonTupleParam.Add(JsonParam);

        if (JsonTupleParam.GetStringSize() > 30*1024 )
        {
            break;
        }
    }

    SendServer(EV_DC_DIS_REQ, MSG_DCGetRecFileListRsp, ulSeqID, JsonTupleParam);
}
//---------------------------------------------------------------------------


