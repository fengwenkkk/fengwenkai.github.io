//---------------------------------------------------------------------------

#pragma hdrstop

#include "ThreadRecFileUpload.h"
#include "ClientInclude.h"
#include "msg.h"


static const UINT32 m_ulRecTypeAudio     = 1;   // 音频文件
static const UINT32 m_ulRecTypeMp4       = 2;   // 视频文件
static const UINT32 m_ulRecTypeUnknown   = 3;   // 其他文件

static const UINT32 m_ulCallTypeIPH       = 10;   // IPH呼叫
static const UINT32 m_ulCallTypeRealBrd   = 11;   // 实时广播

static bool ParseRecFile(char *szFile, LOCAL_TEMP_REC_INFO_T &stRec)
{
    // 12000_20200410_184058277.wav 组呼
    // 12000_20200410184058277.MP4 视频
    // 1001_1002_20200410_184058277.wav 个呼
    CHAR        *szTmp;
    INT32       iFileSize;
    char        acTime[32];
    char        acFile[256];

    memset(&stRec, 0, sizeof(stRec));
    strcpy(stRec.acFile, szFile);

    szTmp = gos_right_strchr(szFile, '\\');
    if (!szTmp)
    {
        return false;
    }

    strcpy(acFile, szTmp+1);  //将文件名拷贝到acFile

    //获取文件大小
    iFileSize = gos_get_file_size(szFile);

    if (iFileSize <= 0)
    {
        return false;
    }

    stRec.ulFileSize = iFileSize;

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
        stRec.ulRecType = m_ulRecTypeMp4;
    }
    else
    {
        stRec.ulRecType = m_ulRecTypeUnknown;
        //return false;
    }

    return true;
}

BOOL ThreadRecFileUp::GetRealBrdFileList(CHAR *szDir)
{
    HANDLE      hDir;
    CHAR        acFile[256];
    BOOL        bIsDir;
    LOCAL_TEMP_REC_INFO_T    stRec;

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
            if (!GetRealBrdFileList(acFile))
            {
                gos_close_dir(hDir);
                return false;
            }

            continue;
        }

        if (!ParseRecFile(acFile, stRec))
        {
            continue;
        }

        stRec.ulCallType = REC_CALL_TYPE_REAL_BRD;

        m_vRecInfo.push_back(stRec);
    }

    gos_close_dir(hDir);

    return true;
}

BOOL ThreadRecFileUp::GetIPHRecFileList(CHAR *szDir)
{
    HANDLE      hDir;
    CHAR        acFile[256];
    BOOL        bIsDir;
    LOCAL_TEMP_REC_INFO_T    stRec;

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
            if (!GetIPHRecFileList(acFile))
            {
                gos_close_dir(hDir);
                return false;
            }

            continue;
        }

        if (!ParseRecFile(acFile, stRec))
        {
            continue;
        }

        stRec.ulCallType = REC_CALL_TYPE_IPH;

        m_vRecInfo.push_back(stRec);
    }

    gos_close_dir(hDir);

    return true;
}

VOID ThreadRecFileUp::RecFileUpload(BOOL bIsIPH)
{
    UINT32      ulTimeOut;
    UINT32      ulFileUploadFailCount = 0;
    GJsonParam  Param;
    CHAR        *szCause;
    CHAR        *szResult;
    bool        bRetry = false;
    UINT8       aucClientAddr[4] = {0,0,0,0};

    GosLog(LOG_ERROR, "%u files will be uploading", m_vRecInfo.size());

    g_pMRSRemoteMsg->GetClientAddr(aucClientAddr, NULL);

    for (UINT32 i=0; i<m_vRecInfo.size(); i++)
    {
        LOCAL_TEMP_REC_INFO_T   &stInfo = m_vRecInfo[i];
        GJson                   RspJson;

        Param.Clear();

        Param.Add("FileName", stInfo.acFile);
        Param.Add("FileSize", stInfo.ulFileSize);
        Param.Add("FileType", stInfo.ulRecType);
        Param.Add("CallType", stInfo.ulCallType);
        Param.AddIP("DCAddr", aucClientAddr);

        ulTimeOut = stInfo.ulFileSize / (1024*1024*5);
        if (ulTimeOut <= 0)
        {
            ulTimeOut = 10;
        }

retry:
        SyncSendMrsServer(EV_CLIENT_REQ, MSG_REC_DCUpFileReq, Param, RspJson, ulTimeOut);

        szResult = RspJson.GetValue("Result");
        if (!szResult)
        {
            szResult = (CHAR*)"timeout";
        }

        if (strcmp(szResult, "Succ") != 0)
        {
            szCause = RspJson.GetValue("Cause");
            GosLog(LOG_ERROR, "upload rec file %s failed: %s", stInfo.acFile, szCause);

            if (ulTimeOut < 120)
            {
                ulTimeOut = 120;
                goto retry;
            }
        }
        else
        {
            RecFileMove(stInfo.acFile, bIsIPH);
        }
    }
}

BOOL ThreadRecFileUp::RecFileMove(CHAR *szOldFilePath, BOOL bIsIPH)
{
    CHAR        *szTmp;
    char        acNewPath[256];
    char        acNewFile[256];

    szTmp = gos_right_strchr(szOldFilePath, '\\');
    if (!szTmp)
    {
        GosLog(LOG_INFO, "invalid rec file %s", szOldFilePath);
        return FALSE;
    }

    if (!bIsIPH)
    {
        LocalRec::GetPARecDir(acNewPath);
    }
    else
    {
        LocalRec::GetIPHRecDir(acNewPath);
    }

    if (!gos_file_exist(acNewPath))
    {
        gos_create_redir(acNewPath);
    }

    sprintf(acNewFile, "%s\\%s", acNewPath, szTmp+1);
    gos_rename_file(szOldFilePath, acNewFile);

    if (gos_file_exist(acNewFile) )
    {
        GosLog(LOG_INFO, "move rec file %s to %s succ", szOldFilePath, acNewFile);
        return TRUE;
    }
    else
    {
        GosLog(LOG_ERROR, "move rec file %s to %s failed", szOldFilePath, acNewFile);
        return FALSE;
    }
}

GOS_THREAD_RET ThreadRecFileUp::ThreadEntry(VOID* pvPara)
{
    GosLog(LOG_INFO, "thread RecFileUp start running");

    while(1)
    {
        UINT32  ulNum = 0;

        m_vRecInfo.clear();
        GetIPHRecFileList(m_acTempRecIPHFileDir);
        ulNum = m_vRecInfo.size();
        RecFileUpload(TRUE);

        m_vRecInfo.clear();
        GetRealBrdFileList(m_acTempRecRealBrdFileDir);
        ulNum += m_vRecInfo.size();
        RecFileUpload(FALSE);

        if (ulNum < 10)
        {
            gos_sleep(30);
        }
        else
        {
            gos_sleep(1);
        }
    }
}

ThreadRecFileUp::ThreadRecFileUp():GThread((void*)NULL)
{
    LocalRec::GetIPHRecTmpDir(m_acTempRecIPHFileDir);
    LocalRec::GetPARecTmpDir(m_acTempRecRealBrdFileDir);
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
