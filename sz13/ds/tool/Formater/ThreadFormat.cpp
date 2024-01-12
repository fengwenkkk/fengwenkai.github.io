#include "g_include.h"
#include "GCommon.h"
#include "ThreadFormat.h"

static VECTOR<AnsiString>       g_vFile;
static GMutex                   g_MutexFile;
static UINT32                   g_ulFormattedNum = 0;

static bool g_bThreadRunning = false;

void InitFormatFile(VECTOR<AnsiString> &vFile)
{
    AUTO_LOCK(g_MutexFile);

    g_vFile.clear();
    g_vFile.assign(vFile.begin(), vFile.end());
    g_ulFormattedNum = 0;
}

void ClearFormatFile()
{
    AUTO_LOCK(g_MutexFile);

    g_vFile.clear();
    g_ulFormattedNum = 0;
}

void GetFormatInfo(UINT32 &ulTotalNum, UINT32 &ulCurrNum)
{
    AUTO_LOCK(g_MutexFile);

    ulTotalNum = g_ulFormattedNum + g_vFile.size();
    ulCurrNum = g_ulFormattedNum;
}

static void FormatedOneFile()
{
    AUTO_LOCK(g_MutexFile);

    g_ulFormattedNum++;
}

static AnsiString GetFile()
{
    AnsiString  strFile = "";

    AUTO_LOCK(g_MutexFile);

    if (g_vFile.size() > 0)
    {
        strFile = g_vFile[g_vFile.size()-1];
        g_vFile.erase(g_vFile.begin()+g_vFile.size()-1);
    }

    return strFile;
}

VOID ThreadFormat::SetCfg(FILE_FORMAT_CFG_T &stCfg)
{
    AUTO_LOCK(g_MutexFile);

    memcpy(&m_stFormatCfg, &stCfg, sizeof(stCfg));
}

ThreadFormat::ThreadFormat():GThread(NULL)
{
}

VOID ThreadFormat::Stop()
{
    m_bRunning = FALSE;

    for (UINT32 i=0; i<20; i++)
    {
        if (!g_bThreadRunning)
        {
            break;
        }

        gos_sleep_1ms(100);
    }
}

void ThreadFormat::FormatFile(char *szFile)
{
    int     iFileSize = gos_get_file_size(szFile);
    FILE    *fp = NULL;
    char    *szBuf = NULL;
    char    acFile[256];
    char    *szLine;
    char    *szNextLine;
    char    *szEnd;
    char    *szLineBuf;
    char    *szTmp;
    char    *szFormattedLine;
    UINT32  ulLineBufOffset;
    UINT32  ulBlankLen = 0;

    if (iFileSize <= 0)
    {
        return;
    }

    fp = fopen(szFile, "rb");
    if (!fp)
    {
        return;
    }

    szBuf = (char*)calloc(1, (iFileSize+1)*5);
    szLineBuf = szBuf + iFileSize+1;
    if (iFileSize != fread(szBuf, 1, iFileSize, fp))
    {
        goto end;
    }

    fclose(fp);

    sprintf(acFile, "%s.tmp", szFile);
    fp = fopen(acFile, "wb");
    if (!fp)
    {
        goto end;
    }

    szLine = szBuf;
    while(*szLine)
    {
        szFormattedLine = szLine;
        szEnd = strchr(szLine, '\n');
        if (szEnd)
        {
            *szEnd = '\0';
            szNextLine = szEnd+1;
        }
        else
        {
            szEnd = szLine+strlen(szLine);
            szNextLine = NULL;
        }

     // if (m_stFormatCfg.bUnixFile) // 去除\r
        if (szEnd != szLine)
        {
            if ((szEnd-1) >= szLine &&
                *(szEnd-1) == '\r')
            {
                szEnd--;
                *szEnd = '\0';
            }
        }

        if (m_stFormatCfg.bTrimRight) // 去除右侧空格
        {
            for (szEnd=szLine+strlen(szLine)-1; szEnd>=szLine; szEnd--)
            {
                if (isspace(*szEnd))
                {
                    *szEnd = '\0';
                }
                else
                {
                    break;
                }
            }
        }

        if (m_stFormatCfg.bConvertTab) // 去除TAB
        {
            ulLineBufOffset = 0;
            for (szTmp=szLine; *szTmp; szTmp++)
            {
                if (*szTmp == '\t')
                {
                    ulBlankLen = 4 - MOD(ulLineBufOffset, 4);
                    memcpy(szLineBuf+ulLineBufOffset, "    ", ulBlankLen);
                    ulLineBufOffset += ulBlankLen;
                }
                else
                {
                    szLineBuf[ulLineBufOffset++] = *szTmp;
                }
            }

            szLineBuf[ulLineBufOffset] = '\0';

            szFormattedLine = szLineBuf;
        }

        fwrite(szFormattedLine, 1, strlen(szFormattedLine), fp);
        if (m_stFormatCfg.bUnixFile) // 无\r
        {
            fwrite("\n", 1, 1, fp);
        }
        else
        {
            fwrite("\r\n", 1, 2, fp);
        }

        if (!szNextLine)
        {
            break;
        }

        szLine = szNextLine;
    }

    fclose(fp);
    fp = NULL;

    gos_rename_file(acFile, szFile);

end:
    if (fp)
    {
        fclose(fp);
    }

    GOS_FREE(szBuf);
}

GOS_THREAD_RET ThreadFormat::ThreadEntry(void* pPara)
{
    AnsiString  strFile;

    g_bThreadRunning = true;

    while(m_bRunning)
    {
        strFile = GetFile();

        if (strFile.IsEmpty())
        {
            gos_sleep_1ms(1);
            continue;
        }

        FormatFile(strFile.c_str());

        FormatedOneFile();
    }

    g_bThreadRunning = false;

    return 0;
}

BOOL GetFileList(CHAR *szRootDir, VECTOR<AnsiString> &vFilePostfix, VECTOR<AnsiString> &vFile, BOOL bClear)
{
    HANDLE      hDir = gos_open_dir(szRootDir);
    BOOL        bRet = FALSE;
    CHAR        acFile[512];
    BOOL        bIsDir;
    CHAR        *szFilePostfix;
    UINT32      i;

    if (bClear)
    {
        vFile.clear();
    }

    while(gos_get_next_file(hDir, acFile, &bIsDir))
    {
        if (bIsDir)
        {
            if (!GetFileList(acFile, vFilePostfix, vFile, FALSE))
            {
                goto end;
            }
        }
        else
        {
            szFilePostfix = gos_get_file_postfix(acFile);
            for (i=0; i<vFilePostfix.size(); i++)
            {
                if (strcasecmp(vFilePostfix[i].c_str(), szFilePostfix) == 0)
                {
                    vFile.push_back(acFile);
                }
            }
        }
    }

    bRet = TRUE;

end:
    gos_close_dir(hDir);

    return bRet;
}

BOOL GetFileList(CHAR *szRootDir, VECTOR<AnsiString> &vSubDir, VECTOR<AnsiString> &vFilePostfix, VECTOR<AnsiString> &vFile)
{
    CHAR    acRootDir[512];

    vFile.clear();

    for (UINT32 i=0; i<vSubDir.size(); i++)
    {
        sprintf(acRootDir, "%s/%s", szRootDir, vSubDir[i].c_str());
        if (!GetFileList(acRootDir, vFilePostfix, vFile, FALSE))
        {
            return FALSE;
        }
    }

    return TRUE;
}


