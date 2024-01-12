//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClientInclude.h"
#include "WavRec.h"
#include "ffmpeg_api.h"
#include "FrmLocalRecManage.h"
#include "FrmLogin.h"
#include "LocalRec.h"
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
#pragma link "dxSkinGlassOceans"
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

static const UINT32 m_ulCallTypeP2P = 1;    // ����
static const UINT32 m_ulCallTypePTT = 2;    // ���
static const UINT32 m_ulCallTypeIPH = 4;    // IPH
static const UINT32 m_ulCallTypePA  = 8;    // PA
static const UINT32 m_ulCallTypeAll = m_ulCallTypeP2P | m_ulCallTypePTT | m_ulCallTypeIPH | m_ulCallTypePA;  // ����

// TODO  �ĳɺ궨��
static const UINT32 m_ulRecTypeAudio = 1;   // ¼��
static const UINT32 m_ulRecTypeVedio = 2;   // ¼��
static const UINT32 m_ulRecTypeAll = m_ulRecTypeAudio | m_ulRecTypeVedio;   // ����

static ID_VALUE_T   s_astCallType[] = { {m_ulCallTypeAll,   "����"},
                                        {m_ulCallTypePTT,   "���"},
                                        {m_ulCallTypeP2P,   "����"},
                                        {m_ulCallTypeIPH,   "IPH"},
                                        {m_ulCallTypePA,    "�㲥"},
                                        };

static ID_VALUE_T   s_astRecType[] = {  {m_ulRecTypeAll,    "����"},
                                        {m_ulRecTypeAudio,  "¼��"},
                                        {m_ulRecTypeVedio,  "¼��"}};

static Dict g_DictCallType(s_astCallType, ARRAY_SIZE(s_astCallType));
static Dict g_DictRecType(s_astRecType, ARRAY_SIZE(s_astRecType));

static VECTOR<LOCAL_REC_INFO_T>        g_vRecInfo;
static bool m_bAutoChangePos = true;

static bool     m_bChangingPos = false;

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

bool ParseRecFile(char *szFile, LOCAL_REC_INFO_T &stRec)
{
    // 12000_20200410_184058277.wav ���
    // 12000_20200410184058277.MP4 ��Ƶ
    // 1001_1002_20200410_184058277.wav ����
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

    // ��ȡ�ļ���ʽ
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

    // ��ȡʱ��
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

    // ��ȡ
    szTmp = strchr(acFile, '_');
    if (!szTmp)    // ���
    {
        strncpy(stRec.acResourceID, acFile, sizeof(stRec.acResourceID)-1);
        stRec.ulCallType = m_ulCallTypePTT;
    }
    else  // ����
    {
        UINT32      ulUserID;

        *szTmp = '\0';
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

    // ��ȡ�ļ���ʽ
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

    // ��ȡʱ��
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

bool ParsePARecFile(char *szFile, LOCAL_REC_INFO_T &stRec)
{
// 13002_20200702_100709.wav
    CHAR        *szTmp;
    char        acTime[32];
    char        acFile[256];
    UINT32      ulLen;

    memset(&stRec, 0, sizeof(stRec));
    stRec.ulCallType = m_ulCallTypePA;
    strcpy(stRec.acFile, szFile);

    szTmp = gos_right_strchr(szFile, '\\');
    if (!szTmp)
    {
        return false;
    }

    strcpy(acFile, szTmp+1);

    // ��ȡ�ļ���ʽ
    szTmp = gos_right_strchr(acFile, '.');
    if (!szTmp)
    {
        return false;
    }

    if (strcasecmp(szTmp+1, "wav") == 0)
    {
        stRec.ulRecType = m_ulRecTypeAudio;
    }
    else
    {
        return false;
    }

    *szTmp = '\0';

    VECTOR<AnsiString>  vStr;

    ParseStringTuple(acFile, "_", vStr);
    if (vStr.size() != 3)
    {
        return false;
    }

    AnsiString  strTime = vStr[1] + vStr[2];

    stRec.ulTime = gos_compact_text_time_to_int(strTime.c_str());
    if (stRec.ulTime == 0xffffffff)
    {
        return false;
    }

    strncpy(stRec.acResourceID, vStr[0].c_str(), sizeof(stRec.acResourceID)-1);

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
    tbPlay->Style->LookAndFeel->SkinName = UICfg::GetVolumeSkinName();
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

    //SetTime(deStartTime, ulTime - 24*3600);

    btnPause->Visible = false;
    btnPlay->Visible = true;

    gbPlay->Height += (576 - PanelMediaPlayer->Height);

    TimerClearData->Interval = 1000*3600;
    TimerClearDataTimer(this);

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
    SplitterMain->Visible = false;

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
            InfoMsg("����Ƿ�");
            return;
        }
    }

    CHAR        acRecFileDir[256];

    sprintf(acRecFileDir, "%s\\%s", GetNativeServicePath(), "data\\rec");
    g_vRecInfo.clear();
    if (!GetLocalRecFileList(acRecFileDir, ulStartTime, ulEndTime, ulRecType, ulCallType, strTmp.c_str()))
    {
        InfoMsg("��ѯʧ��");
        return;
    }

    if (ulCallType & m_ulCallTypeIPH)
    {
        LocalRec::GetIPHRecDir(acRecFileDir);
        if (!GetIPHRecFileList(acRecFileDir, ulStartTime, ulEndTime, ulRecType))
        {
            InfoMsg("��ѯʧ��");
            return;
        }

        LocalRec::GetIPHRecTmpDir(acRecFileDir);
        if (!GetIPHRecFileList(acRecFileDir, ulStartTime, ulEndTime, ulRecType))
        {
            InfoMsg("��ѯʧ��");
            return;
        }
    }

    if (ulCallType & m_ulCallTypePA)
    {
        LocalRec::GetPARecDir(acRecFileDir);
        if (!GetPARecFileList(acRecFileDir, ulStartTime, ulEndTime, ulRecType))
        {
            InfoMsg("��ѯʧ��");
            return;
        }

        LocalRec::GetPARecTmpDir(acRecFileDir);
        if (!GetPARecFileList(acRecFileDir, ulStartTime, ulEndTime, ulRecType))
        {
            InfoMsg("��ѯʧ��");
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

        // �ļ�̫С��ɾ��
        if (gos_get_file_size(acFile) < (int)200)
        {
            gos_delete_file(acFile);
            continue;
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
bool __fastcall TFormLocalRecManage::GetPARecFileList(CHAR *szDir, UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulRecType)
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
            GetPARecFileList(acFile, ulStartTime, ulEndTime, ulRecType);
            continue;
        }

        if (!ParsePARecFile(acFile, stRec))
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
        ErrMsg("����ʧ��");
    }
    else
    {
        InfoMsg("�����ɹ�");
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
    WindowsMediaPlayer->stretchToFit = true;
    WindowsMediaPlayer->fullScreen =  false;
    WindowsMediaPlayer->enableContextMenu = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::btnPauseClick(TObject *Sender)
{
    TimerPlay->Enabled = false;

    WindowsMediaPlayer->controls->pause();
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::btnStopClick(TObject *Sender)
{
    WindowsMediaPlayer->controls->stop();

    TimerPlay->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::TimerPlayTimer(TObject *Sender)
{
    if (!m_bChangingPos && tbPlay)
    {
        tbPlay->Position = WindowsMediaPlayer->controls->currentPosition;
        tbPlay->Hint = WindowsMediaPlayer->controls->currentPositionString;
    }
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
[controls] wmp.controls //��������������
11 controls.play; //����
12 controls.pause; //��ͣ
13 controls.stop; //ֹͣ
14 controls.currentPosition:double; //��ǰ����
15 controls.currentPositionString:string; //��ǰ���ȣ��ַ�����ʽ���硰00:23��
16 controls.fastForward; //���
17 controls.fastReverse; //����
18 controls.next; //��һ��
19 controls.previous; //��һ��

wmp.settings //��������������
22 settings.volume:integer; //������0-100
23 settings.autoStart:Boolean; //�Ƿ��Զ�����
24 settings.mute:Boolean; //�Ƿ���
25 settings.playCount:integer; //���Ŵ���

[currentMedia] wmp.currentMedia //��ǰý������
28 currentMedia.duration:double; //ý���ܳ���
29 currentMedia.durationString:string; //ý���ܳ��ȣ��ַ�����ʽ���硰03:24��
30 currentMedia.getItemInfo(const string); //��ȡ��ǰý����Ϣ"Title"=ý����⣬"Author"=�����ң�"Copyright"=��Ȩ��Ϣ��"Description"=ý������������"Duration"=����ʱ�䣨�룩��"FileSize"=�ļ���С��"FileType"=�ļ����ͣ�"sourceURL"=ԭʼ��ַ
31 currentMedia.setItemInfo(const string); //ͨ������������ý����Ϣ
32 currentMedia.name:string; //ͬ currentMedia.getItemInfo("Title")

playState:integer; //����״̬��1=ֹͣ��2=��ͣ��3=���ţ�6=���ڻ��壬9=�������ӣ�10=׼������
enum class WMPPlayState
*/

// Player.settings.setMode("loop", true);///ʵ��ѭ������

    if (NewState == WMPPlayState::wmppsPlaying)
    {
        tbPlay->ShowHint = true;
        if (WindowsMediaPlayer->currentMedia)
        {
            INT32       iTime = WindowsMediaPlayer->currentMedia->duration;

            tbPlay->Properties->Max = iTime;
            LabelMediaLength->Caption = GetMediaLength(iTime);

            if (iTime <= 0 || tbPlay->Width <= 0)
            {
                TimerPlay->Enabled = false;
            }
            else
            {
                int iInterval = 1000*iTime/tbPlay->Width;

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
        SplitterMain->Visible = false;

        tbPlay->ShowHint = false;
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
// ����Ϣ��������18���ߵ�spt��ѯʱ����ͨ��dis���dc��ѯIPH��PA�ļ�
// TODO ȱPA������
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

    if (ulEndTime == 0)
    {
        ulEndTime = 0xffffffff;
    }

    g_vRecInfo.clear();

    LocalRec::GetIPHRecDir(acRecFileDir);
    if (!GetIPHRecFileList(acRecFileDir, ulStartTime, ulEndTime, ulRecFileType))
    {
        return;
    }

    if (g_vRecInfo.size() == 0)
    {
        return;
    }

    // TODO�����ݽ�������
    SortRecInfo();

    GJsonTupleParam     JsonTupleParam;
    UINT32              i = 0;
    UINT8               aucDISClientAddr[4] = {0,0,0,0};

    g_pRemoteMsg->GetClientAddr(aucDISClientAddr, NULL);

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
        JsonParam.AddIP("DCAddr", aucDISClientAddr);
        JsonParam.Add("MediaLength", "00000");

        JsonTupleParam.Add(JsonParam);

        if (JsonTupleParam.GetStringSize() > 30*1024 )
        {
            break;
        }
    }

    SendServer(EV_DC_DIS_REQ, MSG_DCGetRecFileListRsp, ulSeqID, JsonTupleParam);

    UINT32  ulNum = i;

    // ���·���
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
        JsonParam.AddIP("DCAddr", aucDISClientAddr);
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
void __fastcall TFormLocalRecManage::FormResize(TObject *Sender)
{
    gbPlay->Visible = !gbPlay->Visible;
    gbPlay->Visible = !gbPlay->Visible;
}
//---------------------------------------------------------------------------
void __fastcall TFormLocalRecManage::TimerClearDataTimer(TObject *Sender)
{
    UINT32  ulStartTime = 0;
    UINT32  ulDataReserveDay = LocalCfg::GetInstance().GetDataReserveDay();
    UINT32  ulEndTime = gos_get_current_time() - ulDataReserveDay*86400;
    UINT32  ulRecType = 0xffffffff;
    UINT32  ulCallType = 0xffffffff;
    AnsiString  strTmp;
    INT64   i64ResourceID = -1;
    CHAR    acRecFileDir[256];

    sprintf(acRecFileDir, "%s\\%s", GetNativeServicePath(), "data\\rec");
    g_vRecInfo.clear();
    if (!GetLocalRecFileList(acRecFileDir, ulStartTime, ulEndTime, ulRecType, ulCallType, NULL))
    {
        GosLog(LOG_FATAL, "clear rec data failed");
    }

    LocalRec::GetIPHRecDir(acRecFileDir);
    if (!GetIPHRecFileList(acRecFileDir, ulStartTime, ulEndTime, ulRecType))
    {
        GosLog(LOG_FATAL, "clear iph rec data failed");
    }

    LocalRec::GetPARecDir(acRecFileDir);
    if (!GetPARecFileList(acRecFileDir, ulStartTime, ulEndTime, ulRecType))
    {
        GosLog(LOG_FATAL, "clear pa rec data failed");
    }

    for (UINT32 i=0; i<g_vRecInfo.size(); i++)
    {
        gos_delete_file(g_vRecInfo[i].acFile);
        GosLog(LOG_INFO, "clear rec file %s", g_vRecInfo[i].acFile);
    }
}
//---------------------------------------------------------------------------

