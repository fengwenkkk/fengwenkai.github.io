#include "RecPublic.h"
#include "TaskCheck.h"

#define CHECK_TIMER         TIMER10
#define EV_CHECK_TIMER      GET_TIMER_EV(CHECK_TIMER)

#define MODULE_CHECK        100

const DOUBLE    g_dMinMediaDuration = 0.5;

extern BOOL MergeAudioVideoFile(CHAR *szAudioFile, CHAR *szVideoFile, CHAR *szDestFile);
extern BOOL ConvertAudioFile(CHAR *szAudioFile, CHAR *szDestFile);
extern BOOL ConvertVideoFile(CHAR *szVideoFile, CHAR *szDestFile);
extern DOUBLE GetMediaFileDuration(CHAR *szFile);
extern BOOL ConvertP2PAmrFile(CHAR *szFile, CHAR *szDestFile);

extern BOOL g_bReserveOriFile;
extern CHAR g_acVideoFormat[16];
extern CHAR g_acAudioFormat[16];
//extern CHAR g_acMp4Format[16];

extern CHAR g_acOriVideoFormat[16];
extern CHAR g_acOriAudioFormat[16];

TaskCheck::TaskCheck(UINT16 usDispatchID):GBaseTask(MODULE_CHECK, (CHAR*)"CHECK", usDispatchID)
{
    m_pDao = NULL;
}

BOOL TaskCheck::Init()
{
    if (!m_pDao)
    {
        m_pDao = GetCfgDao();
        if (!m_pDao)
        {
            TaskLog(LOG_ERROR, "get db dao failed");

            return FALSE;
        }
    }

    SetLoopTimer(CHECK_TIMER, 10000);
    SetTaskStatus(TASK_STATUS_WORK);

    TaskLog(LOG_INFO, "Task init successful!");

    return TRUE;
}

BOOL ParseFile(CHAR *szFile, REC_FILE_INFO_T &stInfo, GOS_DATETIME_T &stFileTime)
{
    CHAR    acTmpFile[256];
    CHAR    *szStart;
    CHAR    *szEnd;
    UINT32  ulTimeLen = 14; // strlen("YYYYMMDDhhmmss");

    //memset(&stInfo, 0, sizeof(stInfo));   //?
    strcpy(stInfo.acFile, szFile);
    strcpy(acTmpFile, szFile);
    szStart = acTmpFile;
    szEnd = strchr(szStart, '_');
    if (!szEnd)
    {
        return FALSE;
    }

    *szEnd = '\0';

    if (!gos_atou(szStart, &stInfo.ulCallType))
    {
        return FALSE;
    }

    if (!CheckCallType(stInfo.ulCallType))
    {
        return FALSE;
    }

    szStart = szEnd+1;
    szEnd = strchr(szStart, '_');
    if (!szEnd)
    {
        return FALSE;
    }

    *szEnd = '\0';
    if ((szStart+ulTimeLen) != szEnd)
    {
        return FALSE;
    }

    if (!gos_parse_compact_time(szStart, &stFileTime))
    {
        return FALSE;
    }

    stInfo.ulTime = gos_mktime(&stFileTime);

    stInfo.ulCaller = INVALID_USER_ID;
    stInfo.ulCallee = INVALID_USER_ID;
    stInfo.i64GroupID = INVALID_GROUP_ID;

    if (stInfo.ulCallType == REC_CALL_TYPE_P2P_AUDIO ||
        stInfo.ulCallType == REC_CALL_TYPE_P2P_VIDEO ||
        stInfo.ulCallType == REC_CALL_TYPE_VIDEO_UPLOAD )
    {
        // _1001_13001.amr
        szStart = szEnd+1;
        szEnd = strchr(szStart, '_');
        if (!szEnd)
        {
            return FALSE;
        }
        *szEnd = '\0';

        if (!gos_atou(szStart, &stInfo.ulCaller))
        {
            return FALSE;
        }

        szStart = szEnd+1;
        szEnd = strchr(szStart, '.');
        if (!szEnd)
        {
            return FALSE;
        }
        *szEnd = '\0';

        if (!gos_atou(szStart, &stInfo.ulCallee))
        {
            return FALSE;
        }
    }
    else if (stInfo.ulCallType == REC_CALL_TYPE_PTT_AUDIO ||
             stInfo.ulCallType == REC_CALL_TYPE_PTT_VIDEO)
    {
        // _18003.amr
        szStart = szEnd+1;
        szEnd = strchr(szStart, '.');
        if (!szEnd)
        {
            return FALSE;
        }
        *szEnd = '\0';

        if (!gos_atoi64(szStart, &stInfo.i64GroupID))
        {
            return FALSE;
        }
    }
    else if (stInfo.ulCallType == REC_CALL_TYPE_IPH_AUDIO ||
             stInfo.ulCallType == REC_CALL_TYPE_IPH_VIDEO )
    {
        // TODO
        return FALSE;
    }

    return TRUE;
}

VOID MergeFile(CHAR *szFile, CHAR *szDestFile)
{
    CHAR    *szFilePostfix = gos_get_file_postfix(szFile);
    CHAR    acAudioFile[256];
    CHAR    acVideoFile[256];
    CHAR    acDestFile[256];

    *szDestFile = '\0';

    if (strcmp(szFilePostfix, g_acOriAudioFormat) == 0)
    {
        strcpy(acAudioFile, szFile);
        strcpy(acVideoFile, szFile);
        strcpy(acDestFile, szFile);

        strcpy(acVideoFile+strlen(acVideoFile)-strlen(g_acOriAudioFormat), g_acOriVideoFormat);
        strcpy(acDestFile+strlen(acDestFile)-strlen(g_acOriAudioFormat), g_acVideoFormat);

        if (!gos_file_exist(acVideoFile))
        {
            return;
        }
    }
    else if (strcmp(szFilePostfix, g_acOriVideoFormat) == 0)
    {
        strcpy(acAudioFile, szFile);
        strcpy(acVideoFile, szFile);
        strcpy(acDestFile, szFile);

        strcpy(acAudioFile+strlen(acAudioFile)-strlen(g_acOriVideoFormat), g_acAudioFormat);
        strcpy(acDestFile+strlen(acDestFile)-strlen(g_acOriVideoFormat), g_acVideoFormat);

        if (!gos_file_exist(acAudioFile))
        {
            return;
        }
    }
    else
    {
        return;
    }

    if (MergeAudioVideoFile(acAudioFile, acVideoFile, acDestFile))
    {
        strcpy(szDestFile, acDestFile);
    }
}

/*
对于tmp文件，判断时间，超时去除tmp后缀，
对于amr文件，如果视频文件不存在，则更新数据库，并挪动到目标文件夹（数据库中文件名无后缀）；如果视频文件（mp4）存在，则merge成新文件，然后删除原始文件
对于视频文件，如果音频文件存在，则merge成新文件，然后删除原始文件； 如果音频文件不存在，则转换成mp4文件，并更新数据库后挪动到目的地
*/

BOOL ReplaceFilePostfix(CHAR *szFile, CHAR *szPostfix)
{
    CHAR    *szPos = gos_right_strchr(szFile, '.');

    if (!szPos)
    {
        return FALSE;
    }

    if (*szPostfix == '.')
    {
        szPostfix ++;
    }

    strcpy(szPos+1, szPostfix);

    return TRUE;
}

// 如果不保留原始文件，则删除文件
BOOL MoveOriFile(CHAR *szFile, CHAR *szMonth, CHAR *szDay)
{
    if (!g_bReserveOriFile)
    {
        return gos_delete_file(szFile);
    }

    CHAR        acDestDir[256];
    CHAR        acDestFile[256];
    CHAR        *szSimpleFile = gos_get_simple_file_name(szFile);
    CHAR        *szOriDir = (CHAR*)"ori";

    sprintf(acDestDir, "%s/%s/%s/%s", g_stIniCfg.acRecRootPath, szOriDir, szMonth, szDay);
    if (!gos_file_exist(acDestDir))
    {
        gos_create_redir(acDestDir);
    }

    sprintf(acDestFile, "%s/%s", acDestDir, szSimpleFile);

    return gos_rename_file(szFile, acDestFile);
}

//TODO 根据不同文件类型添加数据库
BOOL TaskCheck::AddRecFile(CHAR *acFile, CHAR *acDestFile, REC_FILE_INFO_T &stInfo, CHAR *acFileMonth, CHAR *acFileDay)
{
    CHAR        acTmpFile[256];
    DOUBLE      dDuration;
    INT32       iFileSize;
    CHAR        *szSimpleFile;
    GOS_DATETIME_T  stFileTime;

    memset(acTmpFile, 0, sizeof(acTmpFile));
    sprintf(acTmpFile, "%s/cvt_tmp.%s", gos_get_root_path(), g_acAudioFormat);

    if (!ConvertAudioFile(acFile, acTmpFile))
    {
        TaskLog(LOG_ERROR, "convert file %s failed", acFile);
        return FALSE;
    }

    if (!gos_rename_file(acTmpFile, acDestFile))
    {
        TaskLog(LOG_ERROR, "rename file %s to %s failed" " %u", acTmpFile, acDestFile, __LINE__);
    }
    else
    {
        memset(&stInfo, 0, sizeof(stInfo));

        dDuration = GetMediaFileDuration(acDestFile);
        iFileSize = gos_get_file_size(acDestFile);

        stInfo.ulFileSize = iFileSize>0 ? iFileSize:0;
        stInfo.dDuration  = dDuration>0 ? dDuration:0;

        szSimpleFile = gos_get_simple_file_name(acDestFile);
        if (!ParseFile(szSimpleFile, stInfo, stFileTime))
        {
            return FALSE;
        }

        strcpy(stInfo.acFile, acDestFile);

        if (!m_pDao->AddRecFile(stInfo))
        {
            return FALSE;
        }

            MoveOriFile(acFile, acFileMonth, acFileDay);
        }

    return TRUE;
}

/**
 * @brief           修改文件后缀名
 * @param           szFile    [in/out] 待修改的文件名
 * @param           szSuffix  [in] 文件后缀
 * @return          BOOL
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *  CHAR acSuffix[16]    = "mp4"
 *  CHAR acDestFile[256] = "test.h264";
 *  ChangFileSuffix(acDestFile, acSuffix); //test.h264->test.mp4
 *
 * @endcode
 * @note
 */
static BOOL ChangFileSuffix(CHAR *szFile, CHAR *szSuffix)
{
    if (!szFile || !szSuffix)
    {
        return FALSE;
    }

    CHAR *szTmp  = NULL;
    CHAR *szDest = szFile;

    szTmp = gos_right_strchr(szFile, '.');
    if(!szTmp)
    {
        szDest = szDest + strlen(szDest) + 1;
        sprintf(szDest, ".%s",  szSuffix);
    }
    else
    {
        szTmp++;
        strcpy(szTmp, szSuffix);
    }

    return TRUE;
}

VOID TaskCheck::OnCheckTimer()
{
    HANDLE      hDir;
    CHAR        acFile[256];
    BOOL        bIsDir;
    CHAR        *szFilePostfix;
    CHAR        acDestDir[256];
    CHAR        acDestFile[256];
//  CHAR        acNewFile[256];
    UINT32      ulFileCreateTime;
    UINT32      ulFileModifyTime;
    UINT32      ulCurrTime = gos_get_current_time();
    INT32       iIdleTime;
    INT32       iFileSize;
    INT32       iMinFileSize = 100;
    CHAR        acFileMonth[32];
    CHAR        acFileDay[32];
    CHAR        *szSimpleFile;
    DOUBLE      dDuration;
    CHAR        acAudioFile[256];
    CHAR        acVideoFile[256];
    CHAR        acTmpFile[256];
//  UINT32      ulLen;
    REC_FILE_INFO_T stInfo;
    GOS_DATETIME_T  stFileTime;

    hDir = gos_open_dir(g_stIniCfg.acRecTmpPath);
    if (!hDir)
    {
        TaskLog(LOG_ERROR, "open rec path failed");
        return;
    }

    // 文件名形如：/home/rec/tmp/pa_20210101123456_1001_13001.amr
    while(gos_get_next_file(hDir, acFile, &bIsDir))
    {
        if (bIsDir)
        {
            continue;
        }

        if (strlen(acFile) <= (strlen(g_stIniCfg.acRecTmpPath)+1))
        {
            continue;
        }

        // 如果记录开始时间与当前时间之差超过阈值，也转正，如果move文件失败，则不转正
        gos_get_file_time(acFile, &ulFileCreateTime, &ulFileModifyTime);
        iIdleTime = ulCurrTime - ulFileModifyTime;

        szFilePostfix = gos_get_file_postfix(acFile);
        if (strcmp(szFilePostfix, "tmp") == 0)
        {
            if (iIdleTime > 0  && iIdleTime < g_stIniCfg.ulMaxRecFileIdleTime)
            {
                continue;
            }

            // 临时文件去除后缀，下次处理
            memset(acDestFile, 0, sizeof(acDestFile));
            memcpy(acDestFile, acFile, strlen(acFile)-4);    // 4 = strlen(".tmp")

            gos_rename_file(acFile, acDestFile);
            continue;
        }

        szSimpleFile = acFile+strlen(g_stIniCfg.acRecTmpPath)+1; // 11_20210101123456_1001_13001.amr
        iFileSize = gos_get_file_size(acFile);
        if (iFileSize <= iMinFileSize)
        {
            gos_delete_file(acFile);
            continue;
        }
        stInfo.ulFileSize = iFileSize;

        dDuration = GetMediaFileDuration(acFile);
        if (dDuration <= 0)
        {
            dDuration = 0;// ulFileModifyTime - ulFileCreateTime;
        }

        stInfo.dDuration = dDuration>0?dDuration:0;

        if (!ParseFile(szSimpleFile, stInfo, stFileTime))
        {
            continue;
        }

        sprintf(acFileMonth, "%04u%02u", stFileTime.usYear, stFileTime.ucMonth);
        sprintf(acFileDay, "%04u%02u%02u", stFileTime.usYear, stFileTime.ucMonth, stFileTime.ucDay);
        sprintf(acDestDir, "%s/%s/%s", g_stIniCfg.acRecRootPath, acFileMonth, acFileDay);
        sprintf(acDestFile, "%s/%s", acDestDir, szSimpleFile);
        gos_format_path(acDestDir);
        gos_format_path(acDestFile);

        if (!gos_file_exist(acDestDir))
        {
            if (!gos_create_redir(acDestDir))
            {
                TaskLog(LOG_ERROR, "create dir %s failed!", acDestDir);
                continue;
            }
        }

        // P2P音频文件存在问题，需要将其中两个声道合并
        if (stInfo.ulCallType == REC_CALL_TYPE_P2P_AUDIO)
        {
            //修改文件后缀为mp3
            ReplaceFilePostfix(acDestFile, g_acAudioFormat);

            sprintf(acTmpFile, "%s/cvt_tmp.%s", gos_get_root_path(), g_acAudioFormat);

            ///先将个呼文件test.amr转成tmp.mp3文件
            if (!ConvertAudioFile(acFile, acTmpFile))
            {
                TaskLog(LOG_ERROR, "convert file %s failed", acFile);
                if(gos_get_file_size(acFile) <= 100)
                {
                    MoveOriFile(acFile, acFileMonth, acFileDay);
                }
                gos_delete_file(acTmpFile);
                continue;
            }

            if (!gos_rename_file(acTmpFile, acDestFile))
            {
                TaskLog(LOG_ERROR, "rename file %s to %s failed" " %u", acTmpFile, acDestFile, __LINE__);
            }
            else
            {
                memset(&stInfo, 0, sizeof(stInfo));

                dDuration = GetMediaFileDuration(acDestFile);
                iFileSize = gos_get_file_size(acDestFile);

                stInfo.ulFileSize = iFileSize>0 ? iFileSize:0;
                stInfo.dDuration  = dDuration>0 ? dDuration:0;

                szSimpleFile = gos_get_simple_file_name(acDestFile);
                if (!ParseFile(szSimpleFile, stInfo, stFileTime))
                {
                    continue;
                }

                strcpy(stInfo.acFile, acDestFile);

                if (!m_pDao->AddRecFile(stInfo))
                {
                    continue;
                }
                MoveOriFile(acFile, acFileMonth, acFileDay); ///删除原始文件
            }

            continue;
        }


        ///对于amr文件，如果视频文件（mp4）不存在，则更新数据库，并挪动到目标文件夹（数据库中文件名无后缀）
        ///如果视频文件（mp4）存在，则merge成新文件，然后删除原始文件,
        if (strcmp(szFilePostfix, g_acOriAudioFormat)  == 0)
        {
            strcpy(acVideoFile, acDestFile);
            ReplaceFilePostfix(acVideoFile, g_acVideoFormat);

            sprintf(acTmpFile, "%s/cvt_tmp.%s", gos_get_root_path(), g_acVideoFormat);

            if (gos_file_exist(acVideoFile))
            {
                ReplaceFilePostfix(acDestFile, g_acVideoFormat);

                if (!MergeAudioVideoFile(acFile, acVideoFile, acTmpFile))
                {
                    TaskLog(LOG_ERROR, "merge file %s and %s failed", acFile, acVideoFile);
                    if(gos_get_file_size(acFile) <= 100)
                    {
                        MoveOriFile(acFile, acFileMonth, acFileDay);
                    }
                    gos_delete_file(acTmpFile);
                    continue;
                }

                dDuration = GetMediaFileDuration(acTmpFile);
                iFileSize = gos_get_file_size(acTmpFile);
                
                memset(&stInfo, 0, sizeof(stInfo));
                stInfo.ulFileSize = iFileSize>0 ? iFileSize:0;
                stInfo.dDuration  = dDuration>0 ? dDuration:0;
                
                szSimpleFile = gos_get_simple_file_name(acDestFile);
                if (!ParseFile(szSimpleFile, stInfo, stFileTime))
                {
                    continue;
                }
                
                strcpy(stInfo.acFile, acDestFile);

                if (dDuration > 0 && iFileSize > 0)
                {
                    if(!m_pDao->UpdateRecFile(acDestFile, dDuration, iFileSize, stInfo))
                    {
                        continue;
                    }
                }

                if (!gos_rename_file(acTmpFile, acDestFile))
                {
                    TaskLog(LOG_ERROR, "rename file %s to %s failed" " %u", acTmpFile, acDestFile, __LINE__);
                }
                else
                {
                    MoveOriFile(acFile, acFileMonth, acFileDay);
                    //MoveOriFile(acVideoFile, acFileMonth, acFileDay);
                }
            }
            else if (strcmp(g_acOriAudioFormat, g_acAudioFormat) == 0)
            {
                gos_copy_file(acFile, acDestFile);
                MoveOriFile(acFile, acFileMonth, acFileDay);
            }
            else //
            {
                ReplaceFilePostfix(acDestFile, g_acAudioFormat);

                sprintf(acTmpFile, "%s/cvt_tmp.%s", gos_get_root_path(), g_acAudioFormat);
                if (!ConvertAudioFile(acFile, acTmpFile))
                {
                    TaskLog(LOG_ERROR, "convert file %s failed", acFile);
                    if(gos_get_file_size(acFile) <= 100)
                    {
                        MoveOriFile(acFile, acFileMonth, acFileDay);
                    }
                    gos_delete_file(acTmpFile);
                    continue;
                }

                if (!gos_rename_file(acTmpFile, acDestFile))
                {
                    TaskLog(LOG_ERROR, "rename file %s to %s failed" " %u", acTmpFile, acDestFile, __LINE__);
                }
                else
                {
                    memset(&stInfo, 0, sizeof(stInfo));

                    dDuration = GetMediaFileDuration(acDestFile);
                    iFileSize = gos_get_file_size(acDestFile);

                    stInfo.ulFileSize = iFileSize>0 ? iFileSize:0;
                    stInfo.dDuration  = dDuration>0 ? dDuration:0;

                    szSimpleFile = gos_get_simple_file_name(acDestFile);
                    if (!ParseFile(szSimpleFile, stInfo, stFileTime))
                    {
                        continue;
                    }

                    strcpy(stInfo.acFile, acDestFile);

                    if (!m_pDao->AddRecFile(stInfo))
                    {
                        continue;
                    }

                    MoveOriFile(acFile, acFileMonth, acFileDay); ///删除原始文件
                }
            }
        }
        //对于视频文件，如果音频文件存在，则merge成新文件，然后删除原始文件； 如果音频文件不存在，则转换成mp4文件，并更新数据库后挪动到目的地
        else if (strcmp(szFilePostfix, g_acOriVideoFormat)  == 0)
        {
            ReplaceFilePostfix(acDestFile, g_acVideoFormat);
            strcpy(acAudioFile, acDestFile);
            ReplaceFilePostfix(acAudioFile, g_acAudioFormat);

            sprintf(acTmpFile, "%s/cvt_tmp.%s", gos_get_root_path(), g_acVideoFormat);

            if (gos_file_exist(acAudioFile))
            {
                if (!MergeAudioVideoFile(acAudioFile, acFile, acTmpFile))
                {
                    TaskLog(LOG_ERROR, "merge file %s and %s failed", acAudioFile, acFile);
                    if(GetMediaFileDuration(acFile) < g_dMinMediaDuration)
                    {
                        MoveOriFile(acFile, acFileMonth, acFileDay);
                    }
                    gos_delete_file(acTmpFile);
                    continue;
                }

                dDuration = GetMediaFileDuration(acTmpFile);
                iFileSize = gos_get_file_size(acTmpFile);

                memset(&stInfo, 0, sizeof(stInfo));
                stInfo.ulFileSize = iFileSize>0 ? iFileSize:0;
                stInfo.dDuration  = dDuration>0 ? dDuration:0;

                szSimpleFile = gos_get_simple_file_name(acDestFile);
                if (!ParseFile(szSimpleFile, stInfo, stFileTime))
                {
                    continue;
                }

                strcpy(stInfo.acFile, acDestFile);

                if (dDuration > 0 && iFileSize > 0)
                {
                    if(!m_pDao->UpdateRecFile(acAudioFile, acDestFile, dDuration, iFileSize, stInfo))
                    {
                        continue;
                    }
                }

                if (!gos_rename_file(acTmpFile, acDestFile))
                {
                    TaskLog(LOG_ERROR, "rename file %s to %s failed" " %u", acTmpFile, acDestFile, __LINE__);
                }
                else
                {
                    MoveOriFile(acFile, acFileMonth, acFileDay);
                    gos_delete_file(acAudioFile);// 生成视频文件后，音频文件删除
                }
            }
            else if (strcmp(g_acOriVideoFormat, g_acVideoFormat) == 0)
            {
                gos_copy_file(acFile, acDestFile);
                MoveOriFile(acFile, acFileMonth, acFileDay);
            }
            else
            {
                ReplaceFilePostfix(acTmpFile, g_acVideoFormat);// g_acMp4Format);

                if (!ConvertVideoFile(acFile, acTmpFile))
                {
                    TaskLog(LOG_ERROR, "convert file %s failed", acFile);
                    if(GetMediaFileDuration(acFile) < g_dMinMediaDuration)
                    {
                        MoveOriFile(acFile, acFileMonth, acFileDay);
                    }
                    gos_delete_file(acTmpFile);
                    continue;
                }

                if (!gos_rename_file(acTmpFile, acDestFile))
                {
                    TaskLog(LOG_ERROR, "rename file %s to %s failed" " %u", acTmpFile, acDestFile, __LINE__);
                }
                else
                {
                    memset(&stInfo, 0, sizeof(stInfo));

                    dDuration = GetMediaFileDuration(acDestFile);
                    iFileSize = gos_get_file_size(acDestFile);

                    stInfo.ulFileSize = iFileSize>0 ? iFileSize:0;
                    stInfo.dDuration  = dDuration>0 ? dDuration:0;

                    szSimpleFile = gos_get_simple_file_name(acDestFile);
                    if (!ParseFile(szSimpleFile, stInfo, stFileTime))
                    {
                        continue;
                    }

                    strcpy(stInfo.acFile, acDestFile);

                    if (!m_pDao->AddRecFile(stInfo))
                    {
                        continue;
                    }

                    MoveOriFile(acFile, acFileMonth, acFileDay); ///删除原始文件
                }
            }
        }
    }

    gos_close_dir(hDir);
}

VOID TaskCheck::TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    UINT32  ulTaskStatus = GetTaskStatus();

    switch(ulTaskStatus)
    {
        case TASK_STATUS_IDLE:
            switch(usMsgID)
            {
                case EV_TASK_INIT_REQ:
                    SendRsp();
                    SetTaskStatus(TASK_STATUS_INIT);
                    SetTimer(TIMER_INIT, 0);
                    break;

                default:
                    break;
            }

            break;

        case TASK_STATUS_INIT:
            switch(usMsgID)
            {
                case EV_INIT_IND:
                    if (!Init())
                    {
                        SetTimer(TIMER_INIT, 1000);
                        TaskLog(LOG_FATAL, "Init failed!");
                    }

                    break;

                default:
                    break;
            }

            break;

        case TASK_STATUS_WORK:
            switch(usMsgID)
            {
                case EV_CHECK_TIMER:
                    OnCheckTimer();
                    break;

                default:
                    TaskLog(LOG_ERROR, "unknown msg %u", usMsgID);
                    break;
            }

            break;

        default:
            break;
    }
}

