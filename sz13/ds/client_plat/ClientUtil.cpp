#pragma hdrstop

#include <vcl.h>
#include <string>
#include "g_include.h"
#include "InnerMsg.h"

#define _UNICODE_

#include "ClientUtil.h"
#include "cxTLExportLink.hpp"

bool EnablePriv()
{
    CHAR    *szPrivilegeName = SE_DEBUG_NAME;
    HANDLE  hProc, hToken;
    TOKEN_PRIVILEGES TP;

    /// 获取进程句柄
    hProc = GetCurrentProcess();

    // 打开进程令牌环
    if (!OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        return false;
    }

    // 获得进程本地唯一ID
    if (!LookupPrivilegeValue(NULL, szPrivilegeName, &TP.Privileges[0].Luid))
    {
        CloseHandle(hToken);
        return false;
    }

    TP.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    TP.PrivilegeCount = 1;

    // 调整权限
    if (!AdjustTokenPrivileges(hToken, false, &TP, sizeof(TP), 0, 0))
    {
        CloseHandle(hToken);
        return false;
    }

    CloseHandle(hToken);
    return true;
}

void CloseApp()
{
    CloseInnerMsg();
    Application->Terminate();
}

//---------------------------------------------------------------------------
void Focus(TWinControl *control)
{
    if ((NULL != control) &&
        (TRUE == control->Visible))
    {
        control->SetFocus();
    }

    return;
}

AnsiString GetTextTime(UINT32 ulSecond, CHAR *szFormat)
{
    CHAR            acTime[32];
    time_t          stCurrTime;
    struct tm       stLocalTime;
    struct tm       *pstTime;

    pstTime = localtime((time_t*)&ulSecond);
    if (!pstTime)
    {
        return "";
    }

    memcpy(&stLocalTime, pstTime, sizeof(stLocalTime));

    if (NULL == szFormat || szFormat[0] == '\0')
    {
        szFormat = "%04d-%02d-%02d %02d:%02d:%02d";
    }

    sprintf(acTime, szFormat,
            (1900+stLocalTime.tm_year),
            (1+stLocalTime.tm_mon),
            stLocalTime.tm_mday,
            stLocalTime.tm_hour,
            stLocalTime.tm_min,
            stLocalTime.tm_sec);

    return AnsiString(acTime);
}

AnsiString GetTimeText(UINT32 ulTime)
{
    char    *szText = gos_get_text_time(&ulTime);

    szText += 11;

    return szText;
}

AnsiString GetTimeText()
{
    return GetTimeText(gos_get_current_time());
}

AnsiString GetTextUptime(UINT32 ul10ms)
{
    UINT32  ulSecond = ul10ms/100;
    UINT32  ulMinute = ulSecond / 60;
    UINT32  ulHour = ulMinute / 60;
    UINT32  ulDay = ulHour / 24;

    ulHour %= 24;
    ulMinute %= 60;
    ulSecond %= 60;

    CHAR    acHour[32];
    CHAR    acMinute[32];
    CHAR    acSecond[32];

    sprintf(acHour, "%d小时", ulHour);
    sprintf(acMinute, "%d分", ulMinute);
    sprintf(acSecond, "%d秒", ulSecond);

    if (ulDay)
    {
        return AnsiString(ulDay) + "天" + AnsiString(acHour) + AnsiString(acMinute) + AnsiString(acSecond);
    }

    if (ulHour)
    {
        return AnsiString(acHour) + AnsiString(acMinute) + AnsiString(acSecond);
    }

    return AnsiString(acMinute) + AnsiString(acSecond);
}

TDateTime GetDateTime(UINT32 ulTime)
{
    time_t      stTime = ulTime;
    struct tm   *pstLocalTime = localtime(&stTime);
    SYSTEMTIME  stSystemTime;

    stSystemTime.wYear   = pstLocalTime->tm_year+1900;
    stSystemTime.wMonth  = pstLocalTime->tm_mon+1;
    stSystemTime.wDay    = pstLocalTime->tm_mday;
    stSystemTime.wHour   = pstLocalTime->tm_hour;
    stSystemTime.wMinute = pstLocalTime->tm_min;
    stSystemTime.wSecond = pstLocalTime->tm_sec;
    stSystemTime.wMilliseconds = 0;

    return SystemTimeToDateTime(stSystemTime);
}

UINT32 GetTime(const TDateTime &DateTime)
{
    unsigned short  usYear;
    unsigned short  usMonth;
    unsigned short  usDay;
    unsigned short  usHour;
    unsigned short  usMinute;
    unsigned short  usSecord;
    unsigned short  usMilliSecord;
    struct tm       stLocalTime = {0};

    DateTime.DecodeDate(&usYear, &usMonth, &usDay);
    DateTime.DecodeTime(&usHour, &usMinute, &usSecord, &usMilliSecord);

    stLocalTime.tm_sec = usSecord;
    stLocalTime.tm_min = usMinute;
    stLocalTime.tm_hour = usHour;
    stLocalTime.tm_mday = usDay;
    stLocalTime.tm_mon  = usMonth-1;
    stLocalTime.tm_year = usYear-1900;

    INT32 iSecond = (INT32)mktime(&stLocalTime);
    if (iSecond < 0)
    {
        iSecond = 0;
    }

    return (UINT32)iSecond;
}

void ExecShellCmd(AnsiString strCmd)
{
    ShellExecuteA(NULL, NULL, strCmd.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

AnsiString GetPwdText(AnsiString strPwd)
{
    return AnsiString::StringOfChar('*', strPwd.Length());
}

AnsiString GetTextFileSize(INT64 i64FileSize)
{
    CHAR    acText[64];

    if (i64FileSize < 0)
    {
        return 0;
    }

    if (i64FileSize < 1024)
    {
        sprintf(acText, "%d B", i64FileSize);
    }
    else if (i64FileSize < 1024*1024)
    {
        sprintf(acText, "%d KB", i64FileSize/1024);
    }
    else
    {
        sprintf(acText, "%d MB", i64FileSize/(1024*1024));
    }

    return acText;
}

AnsiString GetTextFileSize(CHAR *szFile)
{
    INT64   i64Size = gos_get_file_size_ex(szFile);

    return GetTextFileSize(i64Size);
}

AnsiString GetTextFileTime(INT32 iTime)
{
    if (iTime < 0)
    {
        return "";
    }

    CHAR    acText[64];

    if (iTime < 60)
    {
        sprintf(acText, "%d秒", iTime);
    }
    else if (iTime < 60*60)
    {
        sprintf(acText, "%d分%02d秒", iTime/60, MOD(iTime, 60));
    }
    else
    {
        sprintf(acText, "%d小时%02d分%02d秒", iTime/3600, MOD(iTime/60, 60), MOD(iTime, 60));
    }

    return acText;
}

AnsiString GetTextIP(UINT8 *pucIP)
{
    CHAR    acIP[32];

    sprintf(acIP, IP_FMT, IP_ARG(pucIP));

    return acIP;
}

AnsiString GetTextIP(UINT8 *aucIP, UINT32 ulNetMask)
{
    char    acIP[32];

    sprintf(acIP, "%d.%d.%d.%d/%d", aucIP[0], aucIP[1], aucIP[2], aucIP[3], ulNetMask);

    return AnsiString(acIP);
}

AnsiString GetTextMAC(UINT8 *pucMAC)
{
    CHAR    acMAC[64];

    sprintf(acMAC, GOS_MAC_FMT, GOS_MAC_ARG(pucMAC));

    return acMAC;
}

Dict::Dict()
{
    m_cb = NULL;
}

Dict::Dict(ID_VALUE_T *pstMapRec, UINT32 ulMapRecNum)
{
    m_cb = NULL;

    for (UINT32 i=0; i<ulMapRecNum; i++)
    {
        Add(pstMapRec+i);
    }
}

BOOL Dict::Add(ID_VALUE_T *pstRec)
{
    ID_VALUE_T  stRec = {0};

    for (UINT32 i=0; i<m_MapRec.size(); i++)
    {
        if (m_MapRec[i].ulID == pstRec->ulID)
        {
            return FALSE;
        }
    }

    stRec.ulID = pstRec->ulID;
    stRec.szValue = strdup(pstRec->szValue);

    m_MapRec.push_back(stRec);

    return TRUE;
}

BOOL Dict::Add(UINT32 ulID, CHAR *szValue)
{
    ID_VALUE_T      stRec;

    stRec.ulID = ulID;
    stRec.szValue = szValue;

    return Add(&stRec);
}

VOID Dict::Clear()
{
    for (UINT32 i=0; i<m_MapRec.size(); i++)
    {
        free(m_MapRec[i].szValue);
    }

    m_MapRec.clear();
}

VOID Dict::Init(TcxComboBox *cb, BOOL bClear, BOOL bSkipFirst)
{
    if (!cb)
    {
        return;
    }

    m_cb = cb;
    if (bClear)
    {
        /// 清除下拉框内的内容
        cb->Properties->Items->Clear();
    }

    /// 在下拉框内添加内容
    for (UINT32 i=0; i<m_MapRec.size(); i++)
    {
        if (bSkipFirst && i == 0)
        {
            continue;
        }

        cb->Properties->Items->Add(m_MapRec[i].szValue);
    }

    cb->ItemIndex = -1;
}

VOID Dict::SetID(UINT32 ulID)
{
    SetID(m_cb, ulID);
}

void Dict::SetID(TcxComboBox *cb, UINT32 ulID)
{
    if (cb == NULL)
    {
        cb = m_cb;
    }

    if (cb)
    {
        AnsiString strValue = GetValue(ulID);
        cb->ItemIndex = cb->Properties->Items->IndexOf(strValue);
    }
}

BOOL Dict::GetID(UINT8 *pucID)
{
    return GetID(m_cb, pucID);
}

BOOL Dict::GetID(UINT16 *pusID)
{
    return GetID(m_cb, pusID);
}

BOOL Dict::GetID(UINT32 *pulID)
{
    return GetID(m_cb, pulID);
}

BOOL Dict::GetID(TcxComboBox *cb, UINT8 *pucID)
{
    UINT32      ulID = 0;

    if (!GetID(cb, &ulID) || ulID > 0xff)
    {
        return FALSE;
    }

    *pucID = (UINT8)ulID;
    return TRUE;
}

BOOL Dict::GetID(TcxComboBox *cb, UINT16 *pusID)
{
    UINT32      ulID = 0;

    if (!GetID(cb, &ulID) || ulID > 0xffff)
    {
        return FALSE;
    }

    *pusID = (UINT16)ulID;
    return TRUE;
}

BOOL Dict::GetID(TcxComboBox *cb, UINT32 *pulID)
{
    AnsiString  strValue = cb->Text;

    return GetID(strValue.c_str(), pulID);
}

BOOL Dict::GetID(CHAR* szValue, UINT32 *pulID)
{
    if (!szValue)
    {
        return FALSE;
    }

    for (UINT32 i=0; i<m_MapRec.size(); i++)
    {
        if (strcmp(m_MapRec[i].szValue, szValue) == 0)
        {
            *pulID = m_MapRec[i].ulID;
            return TRUE;
        }
    }

    return FALSE;
}

CHAR* Dict::GetValue(UINT32 ulID)
{
    for (UINT32 i=0; i<m_MapRec.size(); i++)
    {
        if (m_MapRec[i].ulID == ulID)
        {
            return m_MapRec[i].szValue;
        }
    }

    return "";
}

BOOL Dict::GetID(Variant value, UINT32 *pulID)
{
    if (value.IsNull() || value.IsEmpty())
    {
        return GetID("", pulID);
    }

    AnsiString strValue = value;

    return GetID(strValue.c_str(), pulID);
}

TcxCategoryRow* AddGroupRow(TcxVerticalGrid *Grid, AnsiString strGroupName)
{
    TcxCategoryRow *GroupRow = (TcxCategoryRow*)Grid->Add(__classid(TcxCategoryRow));
    GroupRow->Properties->Caption = strGroupName;

    return GroupRow;
}

TcxEditorRow* AddTextRow(TcxCustomVerticalGrid *Grid, TcxCategoryRow *GroupRow, AnsiString strCaption, AnsiString strValue, TColor clColor)
{
    TcxEditorRow            *EditorRow;
    TcxTextEditProperties   *Properties;
    TcxStyle                *pstStyle = NULL;

    EditorRow = (TcxEditorRow*)Grid->AddChild(GroupRow, __classid(TcxEditorRow));
    EditorRow->Properties->Caption = strCaption;

    EditorRow->Properties->Options->Editing = false;

    EditorRow->Properties->EditPropertiesClassName = "TcxTextEditProperties";
    EditorRow->Properties->DataBinding->ValueType = "String";
    EditorRow->Properties->Value = strValue;
    Properties = (TcxTextEditProperties*)EditorRow->Properties->EditProperties;
    Properties->ReadOnly = true;

    if (clColor != clWhite)
    {
        pstStyle = new TcxStyle(EditorRow);

        EditorRow->Styles->Content = pstStyle;

        EditorRow->Styles->Content->Color = clColor;
    }

    return EditorRow;
}

TcxEditorRow* AddTextRow(TcxCategoryRow *GroupRow, AnsiString strCaption, AnsiString strValue, TColor clColor)
{
    return AddTextRow(GroupRow->VerticalGrid, GroupRow, strCaption, strValue, clColor);
}

AnsiString GetClassName(TObject *obj)
{
    return obj->ClassName();
}

void SetVerticalGridHeaderWidth(TcxVerticalGrid *Grid, UINT32 ulWidth)
{
    Grid->OptionsView->RowHeaderMinWidth = ulWidth;
    Grid->OptionsView->RowHeaderWidth = ulWidth;
}

UINT32 TimeToInt(AnsiString strTime)
{
    AnsiString strTmp = StringReplace(strTime, "/", "-", TReplaceFlags(TReplaceFlags()<<rfReplaceAll));
    return gos_text_time_to_int(strTmp.c_str());
}

#ifdef _UNICODE_
UINT32 TimeToInt(UnicodeString strTime)
{
    AnsiString strTmp = strTime;
    return TimeToInt(strTmp);
}
#endif

void SetChartAxisRange(TChartAxis *Axis, double dNewMin, double dNewMax)
{
    Axis->Automatic = false;

    if (Axis->Maximum < dNewMin)
    {
        Axis->Maximum = dNewMax;
        Axis->Minimum = dNewMin;
    }
    else
    {
        Axis->Minimum = dNewMin;
        Axis->Maximum = dNewMax;
    }
}

void SetChartAxisRange(TChartAxis *Axis, TChartSeries *Series)
{
    double dNewMin = Series->MinYValue()*0.9;
    double dNewMax = Series->MaxYValue()*1.1;

    SetChartAxisRange(Axis, dNewMin, dNewMax);
}

void SetChartAxisIncrement(TChartAxis *Axis, UINT32 ulIncementNum)
{
    Axis->Increment = (Axis->Maximum - Axis->Minimum)/ulIncementNum;
}

void SetChartAxisPrecision(TChartAxis *Axis, int iPrecision)
{
    char    acPrecision[32];

    if (iPrecision <= 0)
    {
        if (Axis->Increment < 1)
        {
            Axis->Increment = 1;
        }

        if ((Axis->Maximum - Axis->Minimum)/Axis->Increment < 1)
        {
            Axis->Maximum = Axis->Minimum + Axis->Increment;
        }
        Axis->AxisValuesFormat = "0";
        return;
    }

    if (iPrecision > 12)
    {
        iPrecision = 12;
    }

    acPrecision[0] = '0';
    acPrecision[1] = '.';
    memset(acPrecision+2, '#', iPrecision+1);
    acPrecision[3+iPrecision] = '\0';

    Axis->AxisValuesFormat = acPrecision;
}

bool ExportTreeListToExcel(TcxCustomTreeList *tl, AnsiString strFile)
{
    if (tl->Count <= 0)
    {
        return false;
    }

    tl->BeginUpdate();
    tl->LookAndFeel->NativeStyle = true;

    cxExportTLToExcel(strFile, tl);

    tl->LookAndFeel->NativeStyle = false;
    tl->EndUpdate();

    return gos_file_exist(strFile.c_str());
}

bool ExportTreeListToCsv(TcxCustomTreeList *tl, AnsiString strFile)
{
    TcxTreeListNode             *Node;
    UINT32                      ulColumnCount = tl->ColumnCount;

    if (tl->Count <= 0)
    {
        return false;
    }

    FILE    *fp = fopen(strFile.c_str(), "w+t");

    if(!fp)
    {
        return false;
    }

    for (UINT32 i=0; i<ulColumnCount; i++)
    {
        if (tl->Columns[i]->Visible)
        {
            AnsiString      strText = tl->Columns[i]->Caption->Text;
            fprintf(fp, "%s%s", i>0?",":"", strText.c_str());
        }
    }

    fprintf(fp, "\n");

    Node = tl->Root->getFirstChild();

    while(Node)
    {
        if (Node->Visible)
        {
            for (UINT32 i=0; i<ulColumnCount; i++)
            {
                if (tl->Columns[i]->Visible)
                {
                    AnsiString      strText = Node->Texts[i];
                    fprintf(fp, "%s%s", i>0?",":"", strText.c_str());
                }
            }

            fprintf(fp, "\n");
        }

        Node = Node->getNextSibling();
    }

    fclose(fp);

    return true;
}

bool RunProc(char* szProcName, char* szProcCmd, char* szStartDir)
{
    STARTUPINFOA             si;
    PROCESS_INFORMATION     pi;

    ZeroMemory(&si,sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi,sizeof(pi));

    if (szProcName && szProcName[0] == '\0')
    {
        szProcName = NULL;
    }

    BOOL bRet = CreateProcessA((const char*)szProcName, szProcCmd,
                             NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL,
                             (const char*)szStartDir, &si,&pi);
    if (bRet)
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return bRet;
}

AnsiString DoubleText(double dValue, int iNum)
{
    char    acText[32];

    sprintf(acText, "%.*f", iNum, dValue);

    return acText;
}

// yyyy-mm-dd hh:mi:ss
BOOL TextTimeToInt(AnsiString strTime, UINT32 *pulTime)
{
    strTime = strTime.Trim();
    if (strTime.Length() != (int)strlen("yyyy-mm-dd hh:mi:ss"))
    {
        return FALSE;
    }

    char    acTime[32];

    strcpy(acTime, strTime.c_str());

    UINT32  aulPos[6] = {4, 7, 10, 13, 16, 19};
    UINT32  aulMax[6] = {2038, 12, 31, 23, 59, 59};
    CHAR    acMark[6] = {'-','-',' ',':',':','\0'};
    char    *szTmp;
    UINT32  aulTmp[6];
    GOS_DATETIME_T  stTime = {0};

    for (UINT32 i=0; i<6; i++)
    {
        if (acTime[aulPos[i]] != acMark[i])
        {
            return FALSE;
        }

        acTime[aulPos[i]] = '\0';
        if (i == 0)
        {
            szTmp = acTime;
        }
        else
        {
            szTmp = &acTime[aulPos[i]] + 1;
        }

        if (!gos_atou(szTmp, &aulTmp[i]) ||
            aulTmp[i] > aulMax[i])
        {
            return FALSE;
        }
    }

    stTime.usYear   = aulTmp[0];
    stTime.ucMonth  = aulTmp[1];
    stTime.ucDay    = aulTmp[2];
    stTime.ucHour   = aulTmp[3];
    stTime.ucMinute = aulTmp[4];
    stTime.ucSecond = aulTmp[5];

    *pulTime = gos_mktime(&stTime);

    gos_get_text_time_ex(pulTime, NULL, acTime);
    if (strcmp(strTime.c_str(), acTime) == 0)
    {
        return TRUE;
    }

    return FALSE;
}

AnsiString GetIntText(INT64 i64Value)
{
    if (i64Value == 0)
    {
        return "";
    }

    return i64Value;
}

bool ImportFromFile(TcxTreeList *tl, char *szCSV, bool bWithIndexColumn)
{
    FILE    *fp = fopen(szCSV, "rb");

    if (!fp)
    {
        return false;
    }

    fseek(fp, 0, SEEK_END);

    int iLen = ftell(fp);
    int iSize;

    fseek(fp, 0, SEEK_SET);

    char    *szText = (char*)malloc(iLen+1);

    if (!szText)
    {
        fclose(fp);
        return false;
    }

    iSize = fread(szText, 1, iLen, fp);
    fclose(fp);

    if (iSize != iLen)
    {
        fclose(fp);
        return false;
    }

    fclose(fp);

    szText[iLen] = '\0';

    char    *szCurr = szText;
    char    *szLine;
    char    *szField;
    UINT32  i;
    bool    bEnd = false;
    UINT32  ulFieldIndex = 0;
    UINT32  ulRowIndex = 0;
    TcxTreeListNode *Node;

    tl->BeginUpdate();

    while(!bEnd)
    {
        szLine = szCurr;
        szCurr = strchr(szCurr, '\n');
        if (!szCurr)
        {
            bEnd = true;
        }
        else
        {
            *szCurr++ = '\0';
        }

        szLine = gos_trim_string(szLine);
        if (*szLine == '\0')
        {
            continue;
        }

        szField = szLine;

        Node = tl->Add();

        ulFieldIndex = 0;
        if (bWithIndexColumn)
        {
            Node->Texts[ulFieldIndex++] = ++ulRowIndex;
        }

        for (i=0; szLine[i]; i++)
        {
            if (szLine[i] == ',')
            {
                szLine[i] = '\0';
                Node->Texts[ulFieldIndex++] = szField;
                szField = szLine+i+1;
            }
        }

        Node->Texts[ulFieldIndex++] = szField;
    }

    tl->EndUpdate();
    free(szText);

    return true;

fail:
    tl->EndUpdate();
    free(szText);

    return false;
}

INT32 StrToInt(AnsiString str)
{
    return str.ToIntDef(-1);
}

INT64 StrToInt64(AnsiString str)
{
    INT64   i64Value = -1;

    if (!gos_atoi64(str.c_str(), &i64Value))
    {
        return -1;
    }

    return i64Value;
}

int StrCmp(char *sz, AnsiString str)
{
    return ::strcmp(sz, str.c_str());
}

int StrCmp(AnsiString str, char *sz)
{
    return ::strcmp(str.c_str(), sz);
}

char* StrCpy(char *sz, AnsiString str)
{
    return ::strcpy(sz, str.c_str());
}

BOOL ToFloat(AnsiString str, DOUBLE *d)
{
    return gos_atof(str.c_str(), d);
}

BOOL ToInt(AnsiString str, UINT32 *u)
{
    return gos_atou(str.c_str(), u);
}

BOOL ToInt(AnsiString str, INT32 *i)
{
    return gos_atoi(str.c_str(), i);
}

BOOL ToInt64(AnsiString str, INT64 *i)
{
    return gos_atoi64(str.c_str(), i);
}

BOOL ToInt(AnsiString str, UINT16 *u)
{
    UINT32  ulValue;

    if (!ToInt(str, &ulValue) ||
        ulValue > 0xffff)
    {
        return FALSE;
    }

    *u = (UINT16)ulValue;

    return TRUE;
}

BOOL GetIP(AnsiString strAddr, UINT8 *pucAddr)
{
    return gos_get_ip(strAddr.c_str(), pucAddr);
}

BOOL GetIPPort(AnsiString strAddr, UINT8 *pucAddr, UINT16 *pusPort)
{
    return gos_get_ip_port(strAddr.c_str(), pucAddr, pusPort);
}

#ifdef _UNICODE_
int StrCmp(char *sz, UnicodeString str)
{
    return StrCmp(sz, (AnsiString)str);
}

int StrCmp(UnicodeString str, char *sz)
{
    return StrCmp((AnsiString)str, sz);
}

char* StrCpy(char *sz, UnicodeString str)
{
    return StrCpy(sz, (AnsiString)str);
}

BOOL ToFloat(UnicodeString str, DOUBLE *d)
{
    return ToFloat((AnsiString)str, d);
}

BOOL ToInt(UnicodeString str, UINT32 *u)
{
    return ToInt((AnsiString)str, u);
}

BOOL ToInt(UnicodeString str, INT32 *i)
{
    return ToInt((AnsiString)str, i);
}

BOOL ToInt64(UnicodeString str, INT64 *i)
{
    return ToInt64((AnsiString)str, i);
}

UINT64 ToInt64(UINT32 ulHigh, UINT32 ulLow)
{
    UINT64      u64Value = ulHigh;

    u64Value = (u64Value<<32) | ulLow;

    return u64Value;
}

BOOL GetIP(UnicodeString strAddr, UINT8 *pucAddr)
{
    return GetIP((AnsiString)strAddr, pucAddr);
}

BOOL GetIPPort(UnicodeString strAddr, UINT8 *pucAddr, UINT16 *pusPort)
{
    return GetIPPort((AnsiString)strAddr, pucAddr, pusPort);
}
#endif

AnsiString UTF8ToAnsi(CHAR* szUtf8)
{
    RawByteString       str = szUtf8;

    return ::Utf8ToAnsi(str);

//    return gos::Utf8ToGbk(szUtf8).c_str();
    /// int         wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szUtf8, strlen(szUtf8), NULL, 0);
    /// wchar_t*    wszString = new wchar_t[wcsLen + 1];

    /// ::MultiByteToWideChar(CP_UTF8, NULL, szUtf8, strlen(szUtf8), wszString, wcsLen);
    /// wszString[wcsLen] = '\0';

    /// int ansiLen = ::WideCharToMultiByte(CP_ACP, NULL, wszString, wcslen(wszString), NULL, 0, NULL, NULL);
    /// char *szAnsi = (char *)malloc(ansiLen + 1);
    /// AnsiString str;

    /// ::WideCharToMultiByte(CP_ACP, NULL, wszString, wcslen(wszString), szAnsi, ansiLen, NULL, NULL);
    /// szAnsi[ansiLen] = '\0';
    /// str = szAnsi;

    /// delete[] wszString;
    /// free(szAnsi);

    /// return str;
}

void EncryptString(CHAR *szIn, CHAR *szOut)
{
    UINT8       ucTmp;
    UINT32      ulLen = strlen(szIn);
    CHAR        acHex[] = "0123456789abcdef";

    for (UINT32 i=0; i<ulLen; i++)
    {
        ucTmp = szIn[i];
        gos_encrypt_byte(&ucTmp);

        szOut[2*i] = acHex[ucTmp>>4];
        szOut[2*i+1] = acHex[ucTmp&0x0f];
    }

    szOut[2*ulLen] = '\0';
}

void DecryptString(CHAR *szIn, CHAR *szOut)
{
    UINT8       ucTmp1;
    UINT8       ucTmp2;
    UINT32      ulLen = strlen(szIn);

    if (MOD(ulLen, 2) == 1)
    {
        *szOut = '\0';
        return;
    }

    ulLen /= 2;
    for (UINT32 i=0; i<ulLen; i++)
    {
        ucTmp1 = HEX_CHAR_TO_INT(szIn[2*i]);
        ucTmp2 = HEX_CHAR_TO_INT(szIn[2*i+1]);
        if (ucTmp1 > 0x0f || ucTmp2 > 0x0f)
        {
            *szOut = '\0';
            return;
        }

        ucTmp1 = (ucTmp1<<4) | ucTmp2;
        gos_decrypt_byte(&ucTmp1);
        szOut[i] = ucTmp1;
    }

    szOut[ulLen] = '\0';
}

VOID RemoveCommonMember(VECTOR<UINT32> &s, VECTOR<UINT32> &d)
{
    for (int i=s.size()-1; i>=0; i--)
    {
        for (UINT32 j=0; j<d.size(); j++)
        {
            if (s[i] == d[j])
            {
                s.erase(s.begin()+i);
                d.erase(d.begin()+j);
                break;
            }
        }
    }
}

VOID RemoveMember(VECTOR<UINT32> &s, UINT32 d)
{
    for (UINT32 i=0; i<s.size(); i++)
    {
        if (s[i] == d)
        {
            s.erase(s.begin()+i);
            break;
        }
    }
}

VOID RemoveCommonMember(IntSet &s, IntSet &d)
{
    VECTOR<UINT32>  v;

    s.GetList(v);

    for (UINT32 i=0; i<v.size(); i++)
    {
        if (d.Del(v[i]))
        {
            s.Del(v[i]);
        }
    }
}

VOID RemoveCommonMember(Int64Set &s, Int64Set &d)
{
    VECTOR<INT64>  v;

    s.GetList(v);

    for (UINT32 i=0; i<v.size(); i++)
    {
        if (d.Del(v[i]))
        {
            s.Del(v[i]);
        }
    }
}

VOID AppendMember(VECTOR<UINT32> &d, VECTOR<UINT32> &s)
{
    for (UINT32 i=0; i<s.size(); i++)
    {
        d.push_back(s[i]);
    }
}

BOOL Exist(VECTOR<UINT32> &s, UINT32 d)
{
    for (UINT32 i=0; i<s.size(); i++)
    {
        if (s[i] == d)
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL IsSame(VECTOR<UINT32> &s, VECTOR<UINT32> &d)
{
    if (s.size() != d.size())
    {
        return FALSE;
    }

    for (UINT32 i=0; i<s.size(); i++)
    {
        if (!Exist(d, s[i]))
        {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL Exist(VECTOR<INT64> &s, INT64 d)
{
    for (UINT32 i=0; i<s.size(); i++)
    {
        if (s[i] == d)
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL IsSame(VECTOR<INT64> &s, VECTOR<INT64> &d)
{
    if (s.size() != d.size())
    {
        return FALSE;
    }

    for (UINT32 i=0; i<s.size(); i++)
    {
        if (!Exist(d, s[i]))
        {
            return FALSE;
        }
    }

    return TRUE;
}

AnsiString GetConfValue(CHAR *szFile, const CHAR *szGroup, const CHAR *szKey)
{
    GConfFile   Conf(szFile);
    CHAR        *szValue = Conf.Get(szGroup, szKey);

    if (!szValue)
    {
        szValue = "";
    }

    return szValue;
}

bool ParseStringTuple(AnsiString strIn, AnsiString strSeperator, VECTOR<AnsiString> &vOut)
{
    int iPos;
    AnsiString  strTmp;

    vOut.clear();

    if (strSeperator.IsEmpty())
    {
        return false;
    }

    while(!strIn.IsEmpty())
    {
        iPos = strIn.Pos(strSeperator);

        if (iPos >= 1)
        {
            strTmp = strIn.SubString(1, iPos-1);
            strIn = strIn.SubString(iPos+1, strIn.Length());
            vOut.push_back(strTmp);
        }
        else
        {
            vOut.push_back(strIn);
            break;
        }
    }

    return true;
}

SmartString::SmartString()
{
    m_szStr = NULL;
}

SmartString::SmartString(char *str)
{
    m_szStr = NULL;
    if (str)
    {
        AnsiString strAnsi = Utf8ToAnsi(str);
        m_szStr = strdup(strAnsi.c_str());
    }
}

SmartString::~SmartString()
{
    GOS_FREE(m_szStr);
}

SmartString::SmartString(const SmartString &other)
{
    m_szStr = strdup(other.m_szStr);
}

SmartString &SmartString::operator=(const SmartString &other)
{
    if (&other != this)
    {
        GOS_FREE(m_szStr);
        m_szStr = strdup(other.m_szStr);
    }
    return *this;
}

SmartString::operator CHAR *()
{
    return m_szStr;
}

SmartString &SmartString::operator=(char *str)
{
    if (str != m_szStr)
    {
        GOS_FREE(m_szStr);

        if (str)
        {
            AnsiString strAnsi = Utf8ToAnsi(str);
            m_szStr = strdup(strAnsi.c_str());
        }
    }

    return *this;
}

AnsiString FromStyledString(AnsiString str)
{
    CHAR *szStr = str.c_str();
    AnsiString strRet = "";
    int iLen = strlen(szStr);

    for (int i = 0; i < iLen;)
    {
        if ((i + 2) < iLen && memcmp(szStr + i, "\\\"", 2) == 0)
        {
            strRet += '\"';
            i += 2;
        }
        else
        {
            strRet += szStr[i];
            i++;
        }
    }

    return strRet;
}

AnsiString ToStyledString(CHAR *szStr)
{
    AnsiString strRet = "";

    for (; *szStr; szStr++)
    {
        if (*szStr == '"')
        {
            strRet += "\\\"";
        }
        else
        {
            strRet.cat_sprintf("%c", *szStr);
        }
    }

    return strRet;
}

AnsiString ToStyledString(AnsiString str)
{
    return ToStyledString(str.c_str());
}

std::string GetEdtText(TcxTextEdit * edt)
{
    AnsiString str = edt->Text;
    return str.c_str();
}

void SetEditable(TcxTextEdit *edt, bool bEditable)
{
    edt->Enabled = bEditable;  //Properties->ReadOnly = !bEditable;
}

void SetEditable(TcxComboBox *cb, bool bEditable)
{
    cb->Enabled = bEditable; //Properties->ReadOnly = !bEditable;
}

void SetEditable(TcxCheckListBox *clb, bool bEditable)
{
    clb->Enabled = bEditable;
}

bool ReplaceString(AnsiString &strSrc, AnsiString strFrom, AnsiString strTo)
{
    AnsiString  strOut;

    bool    bExist = false;

    while(1)
    {
        int     iPos = strSrc.Pos(strFrom);

        if (iPos <= 0)
        {
            strOut += strSrc;
            break;
        }

        bExist = true;
        strOut += strSrc.SubString(1, iPos-1) + strTo;
        strSrc = strSrc.SubString(iPos+strFrom.Length(), strSrc.Length());
    }

    strSrc = strOut;

    return bExist;
}

AnsiString DecodeJsonMsg(AnsiString strMsgInfo)
{
    ReplaceString(strMsgInfo, "&quot;", "\"");

    return strMsgInfo;
}

BOOL DecodeJsonMsg(CHAR *szInfo, UINT32 ulMaxLen)
{
    AnsiString strOut = DecodeJsonMsg(szInfo);

    if (strOut.Length() >= ulMaxLen)
    {
        return FALSE;
    }

    StrCpy(szInfo, strOut);
    return TRUE;
}

std::string UnzipString(CHAR *szMsgInfo)
{
    INT32   iUnbase64Len = strlen(szMsgInfo)+16;
    UINT8   *pucMsg = (UINT8*)calloc(1, iUnbase64Len);
    std::string str;

    if (!gos_base64_decode((UINT8*)szMsgInfo, strlen(szMsgInfo), pucMsg, &iUnbase64Len))
    {
        free(pucMsg);
        return str;
    }

    HANDLE  hZip = gos_comp_init();
    UINT32  ulUnzipLen = gos_get_decomp_size((CHAR*)pucMsg);
    CHAR    *szOriMsg = (CHAR*)calloc(1, ulUnzipLen+1);

    gos_decomp(hZip, (CHAR*)pucMsg, szOriMsg, &ulUnzipLen);
    str = szOriMsg;

    free(pucMsg);
    free(szOriMsg);

    gos_comp_free(hZip);

    return str;
}
