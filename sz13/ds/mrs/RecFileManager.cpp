#include "RecFileManager.h"


RecFileManager::RecFileManager()
{
    m_pDao = NULL;
}

BOOL RecFileManager::Init(CHAR *szDirOri, CHAR *szDirTemp, UINT32 ulTempSaveDays, UINT32 ulOriSaveDays, UINT32 ulRecSaveDays)
{
    memset(m_acDirOri, 0, sizeof(m_acDirOri));
    memset(m_acDirTemp, 0, sizeof(m_acDirTemp));
    memcpy(m_acDirOri, szDirOri, strlen(szDirOri));
    memcpy(m_acDirTemp, szDirTemp, strlen(szDirTemp));

    m_ulRecSaveTime = ulRecSaveDays*24*60*60;
    m_ulOriSaveTime = ulOriSaveDays*24*60*60;
    m_ulTempSaveTime = ulTempSaveDays*24*60*60;

    
    if (!m_pDao)
    {
        m_pDao = GetCfgDao();
        if (!m_pDao)
        {
            GosLog(LOG_ERROR, "get db dao failed");

            return FALSE;
        }
    }

    return TRUE;
}

VOID RecFileManager::Start()
{
    CheckOriFile();
    CheckTempFile();
    CheckRecFile();
}

//VOID TempFileManager::free()
//{
//    m_bRunning = FALSE;
//}

//GOS_THREAD_RET TempFileManager::ThreadEntry(void* pPara)
//{
//    m_bRunning = TRUE;
//    while(m_bRunning)
//    {
//        CheckOriFile();
//        CheckTempFile();
//        free();
//    }
//
//    GosLog(LOG_INFO, "dir(temp&ori) clean end!");
//
//    return 0;
//}

/**
 * @brief           检查Temp目录中的文件，删除时间过长的文件
 * @return          VOID 
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *  
 * @endcode
 * @note
 * 目前temp只保存7天的文件，超出7天删除
 * TODO：参数可配
 */

VOID RecFileManager::CheckTempFile()
{
    UINT32          ulTime = gos_get_current_time();

    if (!gos_file_exist(m_acDirTemp))
    {
        return ;
    }

    if (!GetTempRecFileList(m_acDirTemp))
    {
        GosLog(LOG_ERROR, "Get Ori Rec File List failed!");
        m_vRecOriList.clear();
        return ;
    }

    for (UINT32 i=0; i<m_vRecTempList.size(); i++)
    {
        std::string strFileName = m_vRecTempList[i];

        //https://blog.csdn.net/frank_liuxing/article/details/18604517
        //关于stat函数的使用参考上述博客
        struct _stat stFileInfo;
        if (_stat(strFileName.c_str(), &stFileInfo) != 0)
        {
            continue ;
        }

        UINT32 ulFileTime = static_cast<unsigned int>(stFileInfo.st_ctime);
        
        //temp目录保保存7天的文件
        //if ((ulTime - ulFileTime) > (7*24*60*60))
        if ((ulTime - ulFileTime) > (m_ulTempSaveTime))
        {
            gos_delete_file(strFileName.c_str());
        } 
    }

    m_vRecTempList.clear();

    return ;
}

BOOL RecFileManager::GetTempRecFileList(CHAR *szDir)
{
    HANDLE      hDir;
    CHAR        acFile[256];
    BOOL        bIsDir;

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
            if (!GetTempRecFileList(acFile))
            {
                gos_close_dir(hDir);
                return false;
            }

            continue;
        }

        std::string    strRecFileName = acFile;

        m_vRecTempList.push_back(strRecFileName);
    }

    gos_close_dir(hDir);

    return true;
}

BOOL RecFileManager::GetOriRecFileList(CHAR *szDir)
{
    HANDLE      hDir;
    CHAR        acFile[256];
    BOOL        bIsDir;

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
            if (!GetOriRecFileList(acFile))
            {
                gos_close_dir(hDir);
                return false;
            }

            continue;
        }

        std::string    strRecFileName = acFile;

        m_vRecOriList.push_back(strRecFileName);
    }

    gos_close_dir(hDir);

    return true;
}

/**
 * @brief           
 * @return          VOID 
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *  
 * @endcode
 * @note
 */
VOID RecFileManager::CheckOriFile()
{
    UINT32          ulTime = gos_get_current_time();

    if (!gos_file_exist(m_acDirOri))
    {
        return ;
    }

    if (!GetOriRecFileList(m_acDirOri))
    {
        GosLog(LOG_ERROR, "Get Ori Rec File List failed!");
        m_vRecOriList.clear();
        return ;
    }

    for (UINT32 i=0; i<m_vRecOriList.size(); i++)
    {
        std::string strFileName = m_vRecOriList[i];

        //https://blog.csdn.net/frank_liuxing/article/details/18604517
        //关于stat函数的使用参考上述博客
        struct _stat stFileInfo;
        if (_stat(strFileName.c_str(), &stFileInfo) != 0)
        {
            continue ;
        }

        UINT32 ulFileTime = static_cast<unsigned int>(stFileInfo.st_ctime);
        
        //ori目录缺省保存30天的文件
        //if ((ulTime - ulFileTime) > (30*24*60*60))
        if ((ulTime - ulFileTime) > m_ulOriSaveTime)
        {
            gos_delete_file(strFileName.c_str());
        } 
    }

    m_vRecOriList.clear();

    return ;
}

VOID RecFileManager::CheckRecFile()
{
    UINT32          ulTime = gos_get_current_time();
    UINT32          ulDeadTime = ulTime - m_ulRecSaveTime;//(365*24*60*60);

    VECTOR<std::string> vFileNameInfo;
    
    m_pDao->GetRecFile(ulDeadTime, vFileNameInfo);
    m_pDao->ClearRecFile(ulDeadTime);

    for (UINT32 i=0; i<vFileNameInfo.size(); i++)
    {
        gos_delete_file(vFileNameInfo[i].c_str());
    }
    return ;
}