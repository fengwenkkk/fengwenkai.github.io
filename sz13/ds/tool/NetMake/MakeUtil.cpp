#include "g_include.h"
#include "GPublic.h"
#include "MakeUtil.h"
#include "TaskMake.h"
#include "TaskMain.h"
#include "GTransFile.h"
#include "ZipUtil.h"

char            g_acCfgFile[256] = {0};

UINT16          g_usServerUDPPort = 65011;      // agent的端口
UINT16          g_usClientUDPPort = 65012;

UINT32          g_usServerTransFilePort = 65111;    // agent的端口
UINT32          g_usClientTransFilePort = 65112;

//UINT32          g_ulMakerNum = 3;
UINT32          g_ulMakeTimeout = 60;

UINT32          g_ulDownloadObjThreadNum = 6;
UINT32          g_ulFileKeepTime = 8*3600; // 文件保留8小时

//PLAT_CFG_T        g_stLocalCfg = {0};
MAKE_ENV_T      g_stMakeEnv;

VECTOR<IP_ADDR_T>       g_vPeerInfo;

VectorMap<AnsiString, FILE_MAKE_INFO_T>   g_mFileMakeInfo;

extern UINT32       g_ulMaxThreadMakeNum;
extern bool         g_bAgentMode;
extern AnsiString   g_strObjPath;

namespace ObjFile {

static VECTOR<OBJ_FILE_INFO_T>      g_vObjFile;
static GMutex                       g_MutexObjFile;

void Clear()
{
    AUTO_LOCK(g_MutexObjFile);

    g_vObjFile.clear();
}

bool Get(OBJ_FILE_INFO_T &stInfo)
{
    AUTO_LOCK(g_MutexObjFile);

    memset(&stInfo, 0, sizeof(stInfo));
    if (g_vObjFile.size() == 0)
    {
        return false;
    }

    memcpy(&stInfo, &g_vObjFile[0], sizeof(stInfo));

    g_vObjFile.erase(g_vObjFile.begin());

    return true;
}

//static GMutex       s_MutexDownload;

// 下载文件
bool Add(UINT8 *pucServerAddr, char *szRemoteFile, char *szLocalFile)
{
    BOOL    bRet;
    UINT8   aucServerAddr[4];
    UINT16  usServerPort = g_usServerTransFilePort;
   //   UINT32  ulMaxDataLen = 32*1024;
//  BOOL    bCancel = FALSE;
    //AnsiString  strCmd;

    AUTO_LOCK(g_MutexObjFile);

    /*
    if (!TransFile(pucServerAddr, usServerPort, szRemoteFile, szLocalFile, ulMaxDataLen, &bCancel))
    {
        return false;
    }

    return gos_get_file_size(szLocalFile) > 0;    */

    OBJ_FILE_INFO_T    stInfo = {0};//(DOWNLOAD_FILE_INFO_T*)malloc(sizeof(DOWNLOAD_FILE_INFO_T));

    memcpy(stInfo.aucServerAddr, pucServerAddr, sizeof(stInfo.aucServerAddr));
    stInfo.usServerPort = usServerPort;
    strcpy(stInfo.acRemoteFile, szRemoteFile);
    strcpy(stInfo.acLocalFile, szLocalFile);

    g_vObjFile.push_back(stInfo);

    /*
    gos_format_path(szRemoteFile);
    gos_format_path(szLocalFile);
    strCmd.printf("%s\\TransClient.exe -h="IP_FMT" -p=%u -r=%s -l=%s", gos_get_root_path(), IP_ARG(aucServerAddr), usServerPort, szRemoteFile, szLocalFile);

    RunNowaitCmd(strCmd.c_str());    */

//  gos_create_thread(DownloadFileThreadEntry, 32*1024, pstInfo);



    return true;
}


}

static GOS_THREAD_RET DownloadFileThreadEntry(VOID *pvData)
{
    UINT32  ulMaxDataLen = 32*1024;
    BOOL    bCancel = FALSE;
    BOOL    bRet;
    OBJ_FILE_INFO_T     stInfo;

    while(1)
    {
        if (!ObjFile::Get(stInfo))
        {
            gos_sleep_1ms(1);
            continue;
        }

        GosLog(LOG_INFO, "starting async download file %s from "IP_FMT, stInfo.acRemoteFile, IP_ARG(stInfo.aucServerAddr));
        bRet = TransFile(stInfo.aucServerAddr, stInfo.usServerPort, stInfo.acRemoteFile, stInfo.acLocalFile, ulMaxDataLen, &bCancel);

        if (bRet)
        {
            if (gos_get_file_size(stInfo.acLocalFile) <= 0)
            {
                bRet = FALSE;
            }
        }

        if (!bRet)
        {
            gos_delete_file(stInfo.acLocalFile);
            GosLog(LOG_ERROR, "async download file %s from "IP_FMT" failed", stInfo.acRemoteFile, IP_ARG(stInfo.aucServerAddr));
        }
        else
        {
            GosLog(LOG_INFO, "async download file %s from "IP_FMT" succ", stInfo.acRemoteFile, IP_ARG(stInfo.aucServerAddr));

            CHAR    acLocalFile[256];
            CHAR    *szFilePostfix;

            strcpy(acLocalFile, stInfo.acLocalFile);
            szFilePostfix = gos_right_strchr(acLocalFile, '.');
            if (szFilePostfix &&
                strcmp(szFilePostfix, ".zip") == 0)
            {
                *szFilePostfix = '\0';

                if (!UnzipFile(stInfo.acLocalFile, acLocalFile))
                {
                    GosLog(LOG_ERROR, "unzip %s failed", stInfo.acLocalFile);
                }
                else
                {
                    GosLog(LOG_INFO, "unzip %s succ", stInfo.acLocalFile);
                }
            }
        }
    }


   //   s_MutexDownload.Mutex();


   //   free(pvData);

   // s_MutexDownload.Unmutex();

    return 0;
}

void InitDownloadThread()
{
    for (UINT32 i=0; i<g_ulDownloadObjThreadNum; i++)
    {
        gos_create_thread(DownloadFileThreadEntry, 32*1024, NULL);
    }
}

AnsiString ToPath(AnsiString strPath)
{
    if (strPath.Pos(" ") > 1)
    {
        if (strPath.Length() > 1 &&
            strPath[1] != '"')
        {
            strPath = "\"" + strPath + "\"";
        }
    }

    return strPath;
}

AnsiString ToPath(char *szPath)
{
    if (szPath == NULL)
    {
        return "";
    }

    return ToPath((AnsiString)szPath);
}

AnsiString ToSimplePath(AnsiString strPath)
{
    int iLen = strPath.Length();

    if (iLen >= 2 &&
        strPath[1] == '"' &&
        strPath[iLen] == '"')
    {
        return strPath.SubString(2, iLen-2);
    }

    return strPath;
}


bool ReplaceStr(AnsiString &strSrc, AnsiString strFromStr, AnsiString strToStr)
{
    bool    bExist = false;

    while(1)
    {
        int     iPos = strSrc.Pos(strFromStr);

        if (iPos <= 0)
        {
            break;
        }

        bExist = true;
        strSrc = strSrc.SubString(1, iPos-1) + strToStr + strSrc.SubString(iPos+strFromStr.Length(), strSrc.Length());
    }

    return bExist;
}

bool ReplaceStr(AnsiString &strSrc, const char *szFromStr, const char *szToStr)
{
    return ::ReplaceStr(strSrc, (AnsiString)szFromStr, (AnsiString)szToStr);
}

AnsiString ConvertPath(AnsiString strPath)
{
    if (strPath.Pos("$") > 0)
    {
        return "";
    }

    if (strPath.Length() > 2 &&
        strPath[1] == '.' &&
        (strPath[2] == '\\' || strPath[2] == '/') )
    {
        strPath = strPath.SubString(3, strPath.Length());
    }

    if (strPath.Length() > 1 &&
        (strPath[strPath.Length()] == '\\' ||
         strPath[strPath.Length()] == '/' ) )
    {
        strPath = strPath.SubString(1, strPath.Length()-1);
    }

    if (strPath.Pos(" ") > 0)
    {
        return "\"" + strPath + "\"";
    }

    return strPath;
}

void FormatPath(AnsiString &strPath)
{
    AnsiString      strNewPath = "";
    AnsiString      strTmpPath;

    strPath += ";";
    while(1)
    {
        int iPos = strPath.Pos(";");

        if (iPos <= 0)
        {
            break;
        }

        strTmpPath = strPath.SubString(1, iPos-1);
        strPath = strPath.SubString(iPos+1, strPath.Length());

        strTmpPath = ConvertPath(strTmpPath);
        if (!strTmpPath.IsEmpty())
        {
            strNewPath += strTmpPath + ";";
        }
    }

    strPath = strNewPath.SubString(1, strNewPath.Length()-1);
}

AnsiString GetMakeStateText(UINT32 ulState)
{
    if (ulState == MAKE_STATE_IDLE)     return "待编译";
    if (ulState == MAKE_STATE_ALLOCED)  return "编译中";
    if (ulState == MAKE_STATE_SUCC)     return "成功";
    if (ulState == MAKE_STATE_FAIL)     return "失败";
    if (ulState == MAKE_STATE_NOFILE)   return "文件不存在";
    if (ulState == MAKE_STATE_COMPILED) return "已编译";

    return "未知";
}


// 根据make order排序（倒序）
void SortFileMakeInfo(VECTOR<FILE_MAKE_INFO_T> &vInfo)
{
    FILE_MAKE_INFO_T    stTmp;

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        for (UINT32 j=i+1; j<vInfo.size(); j++)
        {
            if (vInfo[i].ulMakeOrder < vInfo[j].ulMakeOrder)
            {
                memcpy(&stTmp, &vInfo[i], sizeof(stTmp));
                memcpy(&vInfo[i], &vInfo[j], sizeof(stTmp));
                memcpy(&vInfo[j], &stTmp, sizeof(stTmp));
            }
        }
    }
}

namespace FileMakeInfo {

static VECTOR<AnsiString>           g_vFilterFile;

void InitFilterFile(char *szFilterFile)
{
    char    *szTmp;

    g_vFilterFile.clear();

    szFilterFile = gos_left_trim_string(szFilterFile);
    while(*szFilterFile)
    {
        szTmp = strchr(szFilterFile, ' ');
        if (!szTmp)
        {
            szTmp = strchr(szFilterFile, '\t');
        }

        if (szTmp)
        {
            *szTmp = '\0';
        }

        g_vFilterFile.push_back(szFilterFile);

        if (!szTmp)
        {
            break;
        }

        szFilterFile = gos_left_trim_string(szTmp+1);
    }
}

bool IsFilteredFile(char *szFilterFile)
{
    for (UINT32 i=0; i<g_vFilterFile.size(); i++)
    {
        if (strcasecmp(g_vFilterFile[i].c_str(), szFilterFile) == 0)
        {
            return true;
        }
    }

    return false;
}

BOOL IsMakeDone(UINT32 ulState)
{
    if (//ulState == MAKE_STATE_SUCC      ||
        ulState == MAKE_STATE_FAIL      ||
        ulState == MAKE_STATE_NOFILE    ||
        ulState == MAKE_STATE_COMPILED  ||
        ulState == MAKE_STATE_FILTERED  )
    {
        return TRUE;
    }

    return FALSE;
}

UINT32 GetTotalMakeCount()
{
    return g_mFileMakeInfo.Size();
}

UINT32 GetUnMakeCount()
{
    UINT32      ulCount = 0;

    for (UINT32 i=0; i<g_mFileMakeInfo.Size(); i++)
    {
        FILE_MAKE_INFO_T    *pstInfo = g_mFileMakeInfo[i];

        if (!IsMakeDone(pstInfo->ulState))
        {
            ulCount++;
        }
    }

    return ulCount;
}

BOOL IsAllMakeDone()
{
    Check();

    for (UINT32 i=0; i<g_mFileMakeInfo.Size(); i++)
    {
        FILE_MAKE_INFO_T    *pstInfo = g_mFileMakeInfo[i];

        if (!IsMakeDone(pstInfo->ulState))
        {
//            GosLog(LOG_WARN, "file %s is not made", pstInfo->strFile.c_str());
            return FALSE;
        }
    }

    ClearObjZipFile();

    return TRUE;
}

VOID Init(VECTOR<FILE_MAKE_INFO_T> &vInfo)
{
 // SortFileMakeInfo(vInfo);

    g_mFileMakeInfo.Clear();

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        g_mFileMakeInfo.Add(vInfo[i].strFile, vInfo[i]);
    }
}

FILE_MAKE_INFO_T* Get(AnsiString strFile)
{
    return g_mFileMakeInfo.GetValue(strFile);
}

bool SetResult(AnsiString strFile, IP_ADDR_T *pstPeer, AnsiString strResult, bool bSucc)
{
    FILE_MAKE_INFO_T    *pstInfo = g_mFileMakeInfo.GetValue(strFile);

    if (!pstInfo)
    {
        return false;
    }

    if (gos_file_exist(pstInfo->strObjFile.c_str()))
    {
        pstInfo->strResult = "";
        memset(&pstInfo->stPeer, 0, sizeof(pstInfo->stPeer));
        pstInfo->ulState = MAKE_STATE_SUCC;
        return true;
    }

    pstInfo->strResult = strResult;
    memcpy(&pstInfo->stPeer, pstPeer, sizeof(pstInfo->stPeer));
    pstInfo->ulState = bSucc?MAKE_STATE_SUCC:MAKE_STATE_FAIL;

    return true;
}

bool GetResult(AnsiString strFile, IP_ADDR_T &stPeer, AnsiString &strResult, UINT32 &ulState)
{
    FILE_MAKE_INFO_T    *pstInfo = g_mFileMakeInfo.GetValue(strFile);

    if (!pstInfo)
    {
        return false;
    }

    strResult = pstInfo->strResult;
    ulState = pstInfo->ulState;
    memcpy(&stPeer, &pstInfo->stPeer, sizeof(stPeer));

    return true;
}

VOID CheckObj(VECTOR<FILE_MAKE_INFO_T*> &vLocalMake)
{
    CHAR    acLocalObjFile[256];
    CHAR    *szObjFile;
    CHAR    *szTmp;

    vLocalMake.clear();

    for (UINT32 i=0; i<g_mFileMakeInfo.Size(); i++)
    {
        FILE_MAKE_INFO_T    *pstInfo = g_mFileMakeInfo[i];

        if (pstInfo->ulState == MAKE_STATE_IDLE  ||
            pstInfo->ulState == MAKE_STATE_ALLOCED||
            pstInfo->ulState == MAKE_STATE_SUCC )
        {
            szObjFile = gos_get_simple_file_name(pstInfo->strFile.c_str());
            sprintf(acLocalObjFile, "%s/%s", g_strObjPath.c_str(), szObjFile);

            szTmp = gos_right_strchr(acLocalObjFile, '.');
            if (!szTmp)
            {
                continue;
            }

            strcpy(szTmp, ".obj");

            if (gos_file_exist(acLocalObjFile))
            {
                pstInfo->ulState = MAKE_STATE_COMPILED;
                vLocalMake.push_back(pstInfo);
            }
        }
    }
}

VOID Check()
{
    UINT32  ulCurrTime = gos_get_uptime();
    UINT32  ulTimeout = g_ulMakeTimeout;

    for (UINT32 i=0; i<g_mFileMakeInfo.Size(); i++)
    {
        FILE_MAKE_INFO_T    *pstInfo = g_mFileMakeInfo[i];

        if (pstInfo->ulState == MAKE_STATE_ALLOCED)
        {
            if ((pstInfo->ulAllocTime + ulTimeout) < ulCurrTime)
            {
                pstInfo->ulState = MAKE_STATE_IDLE;
                pstInfo->ulAllocTime = 0;
                pstInfo->ulAllocNum = 0;
            }
        }
    }
}

void DeleteObjAndZipFile(FILE_MAKE_INFO_T *pstInfo, BOOL bDeleteObjFile, BOOL bDeleteZipFile)
{
    char    acFile[256];
    char    acSrcFile[256];
    char    *szTmp;

    strcpy(acSrcFile, pstInfo->strFile.c_str());
    gos_format_path(acSrcFile);
    szTmp = gos_right_strchr(acSrcFile, '\\');
    if (!szTmp)
    {
        szTmp = acSrcFile;
    }
    else
    {
        szTmp++;
    }

    sprintf(acFile, "%s\\%s", g_strObjPath.c_str(), szTmp);
    gos_format_path(acFile);

    szTmp = gos_right_strchr(acFile, '.');
    if (!szTmp)
    {
        return;
    }

    if (bDeleteObjFile)
    {
        strcpy(szTmp, ".obj");
        gos_delete_file(acFile);
    }

    if (bDeleteZipFile)
    {
        strcpy(szTmp, ".obj.zip");
        gos_delete_file(acFile);
    }
}

void DeleteObjAndZipFile(FILE_MAKE_INFO_T *pstInfo)
{
    return DeleteObjAndZipFile(pstInfo, TRUE, TRUE);
}

void DeleteZipFile(FILE_MAKE_INFO_T *pstInfo)
{
    return DeleteObjAndZipFile(pstInfo, FALSE, TRUE);
}

FILE_MAKE_INFO_T* GetIdle(IP_ADDR_T *pstPeer)
{
    Check();

    // 先优先分配未分配的文件
    for (UINT32 i=0; i<g_mFileMakeInfo.Size(); i++)
    {
        FILE_MAKE_INFO_T    *pstInfo = g_mFileMakeInfo[i];

        if (pstInfo->ulState == MAKE_STATE_IDLE)
        {
            pstInfo->ulState = MAKE_STATE_ALLOCED;
            memcpy(&pstInfo->stPeer, pstPeer, sizeof(pstInfo->stPeer));
            pstInfo->ulAllocTime = gos_get_uptime();
            pstInfo->ulAllocNum = 1;

            if (pstInfo->strFile == "data\TrainPosCall.cpp")
            {
                i += 0;
            }

            GosLog(LOG_INFO, "file %s alloced to "IP_FMT, pstInfo->strFile.c_str(), IP_ARG(pstPeer->aucIP));

            DeleteObjAndZipFile(pstInfo);
            return pstInfo;
        }
    }

    // 再分配已经分配但未收到结果的文件
    for (UINT32 i=0; i<g_mFileMakeInfo.Size(); i++)
    {
        FILE_MAKE_INFO_T    *pstInfo = g_mFileMakeInfo[i];

        if (pstInfo->ulState == MAKE_STATE_ALLOCED &&
            pstInfo->ulAllocNum == 1 &&
            memcmp(pstPeer, &pstInfo->stPeer, sizeof(pstInfo->stPeer)) != 0)
        {
            pstInfo->ulState = MAKE_STATE_ALLOCED;
            memcpy(&pstInfo->stPeer, pstPeer, sizeof(pstInfo->stPeer));
            pstInfo->ulAllocTime = gos_get_uptime();
            pstInfo->ulAllocNum = 2;

            GosLog(LOG_INFO, "file %s realloced to "IP_FMT, pstInfo->strFile.c_str(), IP_ARG(pstPeer->aucIP));

            DeleteObjAndZipFile(pstInfo);
            return pstInfo;
        }
    }

    return NULL;
}

BOOL HasIdle()
{
    Check();

    for (UINT32 i=0; i<g_mFileMakeInfo.Size(); i++)
    {
        FILE_MAKE_INFO_T    *pstInfo = g_mFileMakeInfo[i];

        if (pstInfo->ulState == MAKE_STATE_IDLE)
        {
            return TRUE;
        }
    }

    return FALSE;
}

void Rebuild()
{
    for (UINT32 i=0; i<g_mFileMakeInfo.Size(); i++)
    {
        FILE_MAKE_INFO_T    *pstInfo = g_mFileMakeInfo[i];

        if (pstInfo->ulState != MAKE_STATE_FILTERED)
        {
            pstInfo->ulState = MAKE_STATE_IDLE;
        }

        pstInfo->strResult = "";
        memset(&pstInfo->stPeer, 0, sizeof(pstInfo->stPeer));
        pstInfo->ulAllocTime = 0;
        pstInfo->ulAllocNum = 1;
    }

    ClearObjZipFile();
}

void ClearObjZipFile()
{
    for (UINT32 i=0; i<g_mFileMakeInfo.Size(); i++)
    {
        FILE_MAKE_INFO_T    *pstInfo = g_mFileMakeInfo[i];

        DeleteZipFile(pstInfo);
    }
}

};


void WaitCommand(CHAR *szCmd, BOOL bShow)
{
    SHELLEXECUTEINFO ShExecInfo = {0};

    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = NULL;
    ShExecInfo.lpFile = szCmd;
    ShExecInfo.lpParameters = "";
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = bShow?SW_SHOW:SW_HIDE;
    ShExecInfo.hInstApp = NULL;

    ShellExecuteEx(&ShExecInfo);

    WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
}

BOOL RunBatCmd(UINT32 ulIndex, CHAR *szCmd)
{
    char acFile[512];

    sprintf(acFile, "%s/temp%u.bat", gos_get_root_path(), ulIndex);

    gos_delete_file(acFile);

    FILE *fp = fopen(acFile, "wb");
    if (fp == NULL)
    {
        return FALSE;
    }

    fwrite(szCmd, 1, strlen(szCmd), fp);
    fclose(fp);

    WaitCommand(acFile, false);

    return TRUE;
}

BOOL RunBatCmd(CHAR *szCmd)
{
    char acFile[512];

    sprintf(acFile, "%s/zip_tmp.bat", gos_get_root_path());

    gos_delete_file(acFile);

    FILE *fp = fopen(acFile, "wb");
    if (fp == NULL)
    {
        return FALSE;
    }

    fwrite(szCmd, 1, strlen(szCmd), fp);
    fclose(fp);

    WaitCommand(acFile, false);

    return TRUE;
}

BOOL RunNowaitCmd(CHAR *szCmd)
{
    char acFile[512];
    static UINT32   i=0;

    i++;
    if (i >=1000)
    {
        i = 1;
    }

    sprintf(acFile, "%s/tempd%u.bat &", gos_get_root_path(), i);

    gos_delete_file(acFile);

    FILE *fp = fopen(acFile, "wb");
    if (fp == NULL)
    {
        return FALSE;
    }

    fwrite(szCmd, 1, strlen(szCmd), fp);
    fclose(fp);

    WaitCommand(acFile, false);

    gos_delete_file(acFile);

    return TRUE;
}

bool IsFileExists(AnsiString strFile)
{
    if (strFile.IsEmpty())
    {
        return false;
    }

    if (strFile[1] == '"')
    {
        strFile = strFile.SubString(2, strFile.Length()-2);
    }

    return gos_file_exist(strFile.c_str());
}

bool LoadCfg()
{
    char    *szCfgFile = (char*)"net_make.ini";

    sprintf(g_acCfgFile, "%s/%s", gos_get_root_path(), szCfgFile);

    if (!gos_file_exist(g_acCfgFile))
    {
        return false;
    }

/*  if (!GosLoadPlatConf(&g_stLocalCfg, szCfgFile, g_usServerPort))
    {
        GosLog(LOG_FATAL, "load config file %s failed", g_acCfgFile);
        return false;
    }
  */

    GConfFile   Conf(g_acCfgFile);
    CHAR        *szValue;
    UINT32  ulValue;
    char        *szMakeOption = "-y -Q -k -r- -j10 -c -tM -tW -Hc -C8 -w-par -Od -v -vi-";

    g_stMakeEnv.strBccExe     = ToPath(Conf.Get("env", "bcc_exe"));
    g_stMakeEnv.strBccIncPath = ToPath(Conf.Get("env", "bcc_include"));
    g_stMakeEnv.strBccHppPath = ToPath(Conf.Get("env", "bcc_hpp"));
    g_stMakeEnv.strTeePath    = ToPath(Conf.Get("env", "tchart_path"));
    g_stMakeEnv.strDevExpPath = ToPath(Conf.Get("env", "devexp_path"));

    szValue = Conf.Get("env", "make_option");
    if (szValue)
    {
        g_stMakeEnv.strMakeOption = szValue;
    }
    else
    {
        g_stMakeEnv.strMakeOption = szMakeOption;
    }

    if (Conf.Get("maker_num", &ulValue))
    {
        if (ulValue == 0)
        {
            ulValue = 1;
        }

        g_ulMaxThreadMakeNum = ulValue;
    }

    if (Conf.Get("download_num", &ulValue))
    {
        if (ulValue == 0)
        {
            ulValue = 1;
        }
        else if (ulValue > 8)
        {
            ulValue = 8;
        }

        g_ulDownloadObjThreadNum = ulValue;
    }

    if (!IsFileExists(g_stMakeEnv.strBccExe))
    {
        GosLog(LOG_ERROR, "invalid bcc_exe in conf file");
        return false;
    }

    if (!IsFileExists(g_stMakeEnv.strBccIncPath))
    {
        GosLog(LOG_ERROR, "invalid bcc_include in conf file");
        return false;
    }

    if (!IsFileExists(g_stMakeEnv.strBccHppPath))
    {
        GosLog(LOG_ERROR, "invalid bcc_hpp in conf file");
        return false;
    }

    if (!IsFileExists(g_stMakeEnv.strTeePath))
    {
        GosLog(LOG_ERROR, "invalid tchart_path in conf file");
        return false;
    }

    if (!IsFileExists(g_stMakeEnv.strDevExpPath))
    {
        GosLog(LOG_ERROR, "invalid devexp_path in conf file");
        return false;
    }

    if (g_stMakeEnv.strMakeOption.IsEmpty())
    {
        GosLog(LOG_ERROR, "invalid make_option in conf file");
        return false;
    }

//  VECTOR<UINT32>  vGroupID;
    CHAR            acGroupName[32];
    IP_ADDR_T       stPeerInfo = {0};
    PEER_ADDR_T     *pstPeerAddr;
    VECTOR<CHAR*>   vPeerKey;
//  UINT32          ulPeerGroupID = Conf.GetGroupKeyID("peer");

    Conf.GetGroupKeyList(vPeerKey, "peer");

    g_vPeerInfo.clear();
//  g_stLocalCfg.ulPeerNum = 0;
    for (UINT32 i=0; i<vPeerKey.size(); i++)
    {
        if (!gos_str_like(vPeerKey[i], "ip") )
        {
            continue;
        }

        szValue = Conf.Get("peer", vPeerKey[i]);
        if (!gos_get_ip_port(szValue, stPeerInfo.aucIP, &stPeerInfo.usPort))
        {
            continue;
        }

        if (stPeerInfo.usPort == 0)
        {
            stPeerInfo.usPort = g_usClientUDPPort;
        }

        g_vPeerInfo.push_back(stPeerInfo);

/*      pstPeerAddr = &g_stLocalCfg.astPeerAddr[g_stLocalCfg.ulPeerNum++];
        memcpy(pstPeerAddr->aucIP, stPeerInfo.aucIP, sizeof(stPeerInfo.aucIP));
        pstPeerAddr->usPort = stPeerInfo.usPort;
        pstPeerAddr->ucTCPMode = FALSE;    */
    }

    if (g_bAgentMode)
    {
        SET_IP(stPeerInfo.aucIP, 127,0,0,1);
        stPeerInfo.usPort = g_usClientUDPPort;
        g_vPeerInfo.push_back(stPeerInfo);
    }

    return true;
}

// 生成系统路径
AnsiString GetSysPath()
{
    AnsiString  strPath = "";
    AnsiString  strMark = ";";
    AnsiString  astrBccInc[6];
    AnsiString  strBccIncPath = ToSimplePath(g_stMakeEnv.strBccIncPath);

    astrBccInc[0] = ToPath(strBccIncPath + "\\dinkumware");
    astrBccInc[1] = ToPath(strBccIncPath + "\\windows\\crtl");
    astrBccInc[2] = ToPath(strBccIncPath + "\\windows\\sdk");
    astrBccInc[3] = ToPath(strBccIncPath + "\\windows\\rtl");
    astrBccInc[4] = ToPath(strBccIncPath + "\\windows\\vcl");
    astrBccInc[5] = ToPath(strBccIncPath + "\\windows\\fmx");

    strPath = astrBccInc[0] + strMark +
              astrBccInc[1] + strMark +
              astrBccInc[2] + strMark +
              astrBccInc[3] + strMark +
              astrBccInc[4] + strMark +
              astrBccInc[5] + strMark +
              g_stMakeEnv.strBccHppPath + strMark +
              g_stMakeEnv.strBccIncPath + strMark +
              g_stMakeEnv.strTeePath + strMark +
              g_stMakeEnv.strDevExpPath;

    return strPath;
}

// 合并重复内容
AnsiString MergeString(AnsiString strOld, const char cMark=';')
{
    MAP<AnsiString, bool>   mStr;
    AnsiString      strNew = "";
    AnsiString      strTmp;
    int             iPos;

    strOld += cMark;
    while(1)
    {
        iPos = strOld.Pos(cMark);
        if (iPos <= 0)
        {
            break;
        }

        strTmp = strOld.SubString(1, iPos-1);
        strOld = strOld.SubString(iPos+1, strOld.Length());

        mStr[strTmp] = true;
    }

    MAP<AnsiString, bool>::iterator it;

    for (it=mStr.begin(); it!=mStr.end(); it++)
    {
        if (!strNew.IsEmpty())
        {
            strNew += cMark;
        }

        strNew += it->first;
    }

    return strNew;
}

bool GetXmlValue(char *szText, char *szTextEnd, char *szKey, AnsiString &strValue)
{
    char    acKeyStart[256];
    char    acKeyEnd[256];
    char    *szStart;
    char    *szEnd;

    strValue = "";

    sprintf(acKeyStart, "<%s>", szKey);
    sprintf(acKeyEnd, "</%s>", szKey);

    szStart = strstr(szText, acKeyStart);
    if (!szStart || szStart >= szTextEnd)
    {
        return false;
    }

    szStart += strlen(acKeyStart);
    szEnd = strstr(szStart, acKeyEnd);
    if (!szEnd || szEnd >= szTextEnd)
    {
        return false;
    }

    int iLen = szEnd - szStart;

    strValue = AnsiString(szStart, iLen);
    return true;
}

// -D_DEBUG;NO_STRICT;_BCB_;_NMS_CLIENT;_UNICODE_
/*
bool GetProjectParam(char *szProjFile, AnsiString &strDefine, AnsiString &strIncPath, AnsiString &strLibPath, AnsiString &strObjPath)
{
    int     iSize = gos_get_file_size(szProjFile);
    char    *szBuf;
    FILE    *fp;
    AnsiString  strTmp;

    if (iSize <= 0)
    {
        return false;
    }

    szBuf = (char*)calloc(1, iSize+1);
    if (!szBuf)
    {
        return false;
    }

    fp = fopen(szProjFile, "rb");
    if (!fp)
    {
        free(szBuf);
        return false;
    }

    if (iSize != fread(szBuf, 1, iSize, fp))
    {
        free(szBuf);
        fclose(fp);
        return false;
    }

    fclose(fp);

    bool    bRet = false;
    char    *szKey = strstr(szBuf, "<PropertyGroup Condition=\"'$(Base)'!=''\">");
    char    *szEnd;

    if (!szKey)
    {
        goto end;
    }

    szKey += strlen("<PropertyGroup Condition=\"'$(Base)'!=''\">");
    szEnd = strstr(szKey, "</PropertyGroup>");
    if (!szEnd)
    {
        goto end;
    }

 //   *szEnd = '\0';

    if (!GetXmlValue(szKey, "Defines", strDefine) ||
        !GetXmlValue(szKey, "IncludePath", strIncPath) ||
        !GetXmlValue(szKey, "ILINK_LibraryPath", strLibPath) ||
        !GetXmlValue(szKey, "IntermediateOutputDir", strObjPath) )
    {
        goto end;
    }

    // get debug obj path
    szKey = strstr(szBuf, "<PropertyGroup Condition=\"'$(Cfg_1)'!=''\">");
    if (szKey && GetXmlValue(szKey, "IntermediateOutputDir", strTmp))
    {
        strObjPath = strTmp;
    }

    bRet = true;

end:
    free(szBuf);
    return bRet;
    // <PropertyGroup Condition="'$(Base)'!=''">
    // <Defines>NO_STRICT;_BCB_;_NMS_CLIENT;_UNICODE_;$(Defines)</Defines>
}
*/

void RemoveEndMark(AnsiString &str, const char cMark=';')
{
    if (str.Length() >= 1 &&
        str[str.Length()] == cMark)
    {
        str = str.SubString(1, str.Length()-1);
    }
}

/*
"<PropertyGroup Condition=\"'$(Base)'!=''\">");
    */
AnsiString GetXMLValue(char *szXML, const char *szSection, const char *szAttr, const char *szParam)
{
    char    acTmp[1024];
    UINT32  ulLen = sprintf(acTmp, "<%s %s>", szSection, szAttr);
    char    *szKey = strstr(szXML, acTmp);
    char    *szEnd;
    AnsiString  strValue;

    if (!szKey)
    {
        return "";
    }

    szKey += ulLen;

    sprintf(acTmp, "</%s>", szSection);
    szEnd = strstr(szKey, acTmp);
    if (!szEnd)
    {
        return "";
    }

    if (!GetXmlValue(szKey, szEnd, (char*)szParam, strValue) )
    {
        return "";
    }

    return strValue;
}

AnsiString GetMultiXMLValue(char *szXML, const char *szSection, const char *szKey, AnsiString strMark)
{
    AnsiString  strRet = "";
    AnsiString  strTmp;

    strTmp = GetXMLValue(szXML, szSection, "Condition=\"'$(Base)'!=''\"", szKey);
    if (!strTmp.IsEmpty())
    {
        strRet += strTmp + strMark;
    }

    strTmp = GetXMLValue(szXML, szSection, "Condition=\"'$(Base_Win32)'!=''\"", szKey);
    if (!strTmp.IsEmpty())
    {
        strRet += strTmp + strMark;
    }

    strTmp = GetXMLValue(szXML, szSection, "Condition=\"'$(Cfg_1)'!=''\"", szKey);
    if (!strTmp.IsEmpty())
    {
        strRet += strTmp + strMark;
    }

    strTmp = GetXMLValue(szXML, szSection, "Condition=\"'$(Cfg_1_Win32)'!=''\"", szKey);
    if (!strTmp.IsEmpty())
    {
        strRet += strTmp + strMark;
    }

    return strRet;
}

AnsiString GetSingleXMLValue(char *szXML, const char *szSection, const char *szKey)
{
    AnsiString  strRet = "";
    AnsiString  strTmp;

    strTmp = GetXMLValue(szXML, szSection, "Condition=\"'$(Cfg_1_Win32)'!=''\"", szKey);
    if (!strTmp.IsEmpty())
    {
        return strTmp;
    }

    strTmp = GetXMLValue(szXML, szSection, "Condition=\"'$(Cfg_1)'!=''\"", szKey);
    if (!strTmp.IsEmpty())
    {
        return strTmp;
    }

    strTmp = GetXMLValue(szXML, szSection, "Condition=\"'$(Base_Win32)'!=''\"", szKey);
    if (!strTmp.IsEmpty())
    {
        return strTmp;
    }

    strTmp = GetXMLValue(szXML, szSection, "Condition=\"'$(Base)'!=''\"", szKey);
    if (!strTmp.IsEmpty())
    {
        return strTmp;
    }

    return "";
}

void GetProjectParam(char *szProjFile, AnsiString &strIncPath, AnsiString &strObjPath, AnsiString &strDefine)
{
    int     iSize = gos_get_file_size(szProjFile);
    char    *szBuf;
    FILE    *fp;

    strIncPath = "";
    strObjPath = "";

    if (iSize <= 0)
    {
        return;
    }

    szBuf = (char*)calloc(1, iSize+1);
    if (!szBuf)
    {
        return;
    }

    fp = fopen(szProjFile, "rb");
    if (!fp)
    {
        free(szBuf);
        return;
    }

    if (iSize != fread(szBuf, 1, iSize, fp))
    {
        free(szBuf);
        fclose(fp);
        return;
    }

    fclose(fp);

    strIncPath = GetMultiXMLValue(szBuf, "PropertyGroup", "IncludePath", ";");
    FormatPath(strIncPath);

    ReplaceStr(strIncPath, "$(IncludePath)", "");
    ReplaceStr(strIncPath, "$(BDS)\\include", "");
    ReplaceStr(strIncPath, ";;", ";");

    strObjPath = GetSingleXMLValue(szBuf, "PropertyGroup", "IntermediateOutputDir");

    strDefine = GetMultiXMLValue(szBuf, "PropertyGroup", "Defines", ";");
    ReplaceStr(strDefine, "$(Defines)", "");
    ReplaceStr(strDefine, ";;", ";");

    free(szBuf);

    RemoveEndMark(strIncPath);
    RemoveEndMark(strObjPath);
    RemoveEndMark(strDefine);

    strIncPath = MergeString(strIncPath, ';');
    strDefine = MergeString(strDefine, ';');
}

UINT32 GetCpuNum()
{
    SYSTEM_INFO     sysInfo = {0};

    GetSystemInfo(&sysInfo);

    return sysInfo.dwNumberOfProcessors;
}

UINT32 GetCpuCoreNum()
{
    return GetCpuNum();

    char    acResultFile[256];
    char    acCmdFile[256];
    FILE    *fp;
    char    acBuf[4*1024];
    char    *szTmp = NULL;
    UINT32  ulNum = 0;
    int     iSize;
    int     i;

    sprintf(acResultFile, "%s/cpu.info", gos_get_root_path());
    sprintf(acCmdFile, "%s/temp.bat", gos_get_root_path());

    fp = fopen(acCmdFile, "w+t");
    if (!fp)
    {
        goto fail;
    }

    fprintf(fp, "WMIC CPU Get NumberOfLogicalProcessors /Format:List > %s", acResultFile);
    fclose(fp);

    WaitCommand(acCmdFile, false);

    iSize = gos_get_file_size(acResultFile);
    if (iSize <= 0)
    {
        goto fail;
    }

    fp = fopen(acResultFile, "rb");
    if (!fp)
    {
        goto fail;
    }

    memset(acBuf, 0, sizeof(acBuf));
    fread(acBuf, 1, sizeof(acBuf)-1, fp);
    fclose(fp);

    memset(acBuf, ' ', 2);


    // 0D 00 0A 00 0D 00 0A 00
    for (i=1; i<iSize/2; i++)
    {
        acBuf[i] = acBuf[2*i];
    }

    acBuf[i] = '\0';

    szTmp = gos_trim_string(acBuf);

    szTmp = strstr(szTmp, "NumberOfLogicalProcessors");
    if (!szTmp)
    {
        goto fail;
    }

    szTmp += strlen("NumberOfLogicalProcessors");
    szTmp = strchr(szTmp, '=');
    if (!szTmp)
    {
        goto fail;
    }

    szTmp++;
    szTmp = gos_trim_string(szTmp);
    if (!gos_atou(szTmp, &ulNum) || ulNum == 0)
    {
        goto fail;
    }

    return ulNum;

fail:
    return GetCpuNum();
}

void SaveProject(AnsiString strProject)
{
}

AnsiString ToAbsPath(char *szSrcIncPath, AnsiString strProjectPath)
{
    AnsiString  strPath = "";
    CHAR        acPath[256];
    UINT32      ulLen = 0;

    RemoveEndMark(strProjectPath, ';');

    memset(acPath, 0, sizeof(acPath));
    while(1)
    {
        if (*szSrcIncPath == ';' ||
            *szSrcIncPath == '\0' )
        {
            strPath.cat_printf("%s\\%s;", strProjectPath.c_str(), acPath);
            memset(acPath, 0, sizeof(acPath));
            ulLen = 0;

            if (*szSrcIncPath == '\0')
            {
                break;
            }
            else
            {
                szSrcIncPath++;
            }
        }

        acPath[ulLen++] = *szSrcIncPath++;
    }

    return strPath;
}

AnsiString GetMakeCmd(MAKING_FILE_INFO_T &stFileInfo)
{
    AnsiString  strCmd = "";
    AnsiString  strSysPath = GetSysPath();
    AnsiString  strSrcPath;
    AnsiString  strMakeFile;
    AnsiString  strIncPath;

    strSrcPath.printf("%s\\%s", stFileInfo.acProject, stFileInfo.acSrcPath);
    strMakeFile.printf("%s\\%s", stFileInfo.acProject, stFileInfo.acFile);

    strIncPath = ToAbsPath(stFileInfo.acIncPath, strSrcPath);
    strIncPath = strSrcPath+";" + strIncPath;
    RemoveEndMark(strIncPath, ';');

    strCmd.cat_printf("set BCC_EXE=%s\r\n", g_stMakeEnv.strBccExe.c_str());
    strCmd.cat_printf("set SRC_INC_PATH=%s\r\n", strIncPath.c_str());
    strCmd.cat_printf("set SYS_PATH=%s\r\n", strSysPath.c_str());
    //"-y -Q -k -r- -j10 -c -tM -tW -Hc -C8  -w-par -Od -v -vi- ");
    strCmd.cat_printf("set MAKE_OPTION=%s\r\n", stFileInfo.acMakeOption);

    strCmd.cat_printf("%%BCC_EXE%% -D_DEBUG;WIN32 -nDebug -I%%SRC_INC_PATH%%;%%SYS_PATH%% "
            "%%MAKE_OPTION%% -o%s %s", stFileInfo.acObjFile, strMakeFile.c_str());

    return strCmd;
}


// 生成压缩文件， rar.exe a -r %ZIP_FILE% -n*.c -n*.cpp -n*.h -n*.hpp -n*.dfm -n*.cbproj *.dc.groupproj dc gos gplat client_if client_plat ds_public dtp eLTE_SDK ffmpeg_api ffmpeg4.0.1 libcurl snmp
bool ZipMakeFile(char *szProjectDir, VECTOR<std::string> &vMakePath, char *szZipFile)
{
    VECTOR<std::string> vFilePostfix;

    vFilePostfix.push_back("c");
    vFilePostfix.push_back("cpp");
    vFilePostfix.push_back("h");
    vFilePostfix.push_back("hpp");
    vFilePostfix.push_back("dfm");
    vFilePostfix.push_back("cbproj");

    return ZipDir(szProjectDir, vMakePath, vFilePostfix, szZipFile);

        /*
    AnsiString  strCmd;

    gos_delete_file(szZipFile);
    if (gos_file_exist(szZipFile))
    {
        return false;
    }

    strCmd.printf("%c:\r\n", szProjectDir[0]);
    strCmd.cat_printf("cd %s\r\n", szProjectDir);
    strCmd.cat_printf("%s\\rar.exe a -r %s -n*.c -n*.cpp -n*.h -n*.hpp -n*.dfm -n*.cbproj", gos_get_root_path(), szZipFile);

    for (UINT32 i=0; i<vMakePath.size(); i++)
    {
        strCmd.cat_printf(" %s", vMakePath[i].c_str());
    }

    RunBatCmd(strCmd.c_str());

    return gos_file_exist(szZipFile);    */
}

// 下载文件 (client -> server)
bool DownloadMakeDir(UINT8 *pucServerAddr, AnsiString strMakeName, char *szZipFile, UINT32 ulFileLen)
{
    BOOL    bRet;
    UINT8   aucServerAddr[4];
    UINT16  usServerPort = g_usClientTransFilePort;
    CHAR    *szFile = szZipFile;
    CHAR    acLocalFile[256];
    UINT32  ulMaxDataLen = 32*1024;
    BOOL    bCancel = FALSE;
    char    acCmd[1024];
    char    acDir[256];
    int     iFileSize;
    bool    bLocalAddr = false;
    UINT8   aucLocalAddr[] = {127,0,0,1};

    sprintf(acLocalFile, "%s\\%s", gos_get_root_path(), szZipFile);

    if (memcmp(pucServerAddr, aucLocalAddr, 4) == 0)
    {
        bLocalAddr = true;
        bRet = true;
    }
    else
    {
        bRet = TransFile(pucServerAddr, usServerPort, szFile, acLocalFile, ulMaxDataLen, &bCancel);
        if (!bRet)
        {
            GosLog(LOG_ERROR, "Trans remtoe file %s to %s failed", szFile, acLocalFile);
            return false;
        }

        iFileSize = gos_get_file_size(acLocalFile);
        if (ulFileLen != iFileSize)
        {
            GosLog(LOG_ERROR, "file length %d of %s should be %u", iFileSize, acLocalFile, ulFileLen);
            return false;
        }
    }

    sprintf(acDir, "%s\\%s", gos_get_root_path(), strMakeName.c_str());
    if (!gos_file_exist(acDir))
    {
        gos_create_dir(acDir);
    }

    if (!UnzipDir(acDir, acLocalFile))
    {
        GosLog(LOG_ERROR, "unzip file %s failed", acLocalFile);
        return false;
    }

    if (!bLocalAddr)
    {
        gos_delete_file(acLocalFile);
    }

    return true;

    /*
    // 解压缩文件：rar.exe x -y ds1.rar .\ds1
    sprintf(acDir, "%s\\%s", gos_get_root_path(), strMakeName.c_str());
    sprintf(acCmd, "%s\\rar.exe x -y %s %s", gos_get_root_path(), acLocalFile, acDir);

    if (gos_file_exist(acDir))
    {
        gos_delete_dir(acDir);
    }
    else
    {
        gos_create_dir(acDir);
    }

    WaitCommand(acCmd, false);

    return true;                   */
}

bool InitMakePath(char *szRootPath, char *szProjectPath, char *szMakePath, VECTOR<std::string> &vMakePath)
{
    vMakePath.clear();

    std::string  strPath;

    strPath = szProjectPath;
    vMakePath.push_back(strPath);

    char    *szEnd;

    szMakePath = gos_left_trim_string(szMakePath);

    while(*szMakePath)
    {
        strPath = "";//.printf("%s\\", szRootPath);
        for (szEnd=szMakePath; ; szEnd++)
        {
            if (isspace(*szEnd) || *szEnd == '\0')
            {
                vMakePath.push_back(strPath);

                if (*szEnd != '\0')
                {
                    szMakePath = gos_left_trim_string(szEnd+1);
                }
                else
                {
                    szMakePath = szEnd;
                }

                break;
            }

            strPath += *szEnd;
        }
    }

    return true;
}

void ClearOldTask(CHAR *szResvDir)
{
    CHAR    acLastTime[32];
    CHAR    acFile[256];
    CHAR    *szRootDir = gos_get_root_path();
    CHAR    *szDir;
    HANDLE  hDir;
    BOOL    bIsDir;
    UINT32  ulTimeLen;
    UINT32  ulLastTime = gos_get_current_time() - g_ulFileKeepTime;
    bool    bDelete = true;

    gos_get_text_time_ex(&ulLastTime, "%04u%02u%02u_%02u%02u%02u", acLastTime);
    ulTimeLen = strlen(acLastTime);
    hDir = gos_open_dir(szRootDir);

    while(gos_get_next_file(hDir, acFile, &bIsDir))
    {
        szDir = gos_right_strchr(acFile, '\\');
        if (!szDir)
        {
            continue;
        }

        szDir++;
        if (strlen(szDir) <= ulTimeLen)
        {
            continue;
        }

        if (szDir[8] != '_')
        {
            continue;
        }

        bDelete = false;
        if (szResvDir)
        {
            if (gos_str_like(acFile, szResvDir) )
            {
                continue;
            }
            else
            {
                bDelete = true;
            }
        }
        else if ((szResvDir && !gos_str_like(acFile, szResvDir) != 0 ) ||
                 strncmp(szDir, acLastTime, ulLastTime) < 0)
        {
            bDelete = true;
        }

        if (bDelete)
        {
            if (bIsDir)
            {
                gos_delete_dir_full(acFile);
                GosLog(LOG_INFO, "remove dir %s", szDir);
            }
            else
            {
                gos_delete_file(acFile);
                GosLog(LOG_INFO, "remove file %s", szDir);
            }
        }
    }

    gos_close_dir(hDir);
}
