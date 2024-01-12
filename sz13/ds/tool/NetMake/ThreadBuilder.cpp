#include "g_include.h"
#include "GCommon.h"
#include "GLog.h"
#include "ThreadBuilder.h"


extern HANDLE   g_hMainHandle;

CHAR* ThreadBuilder::BuilderFile(CHAR *szFile)
{
    char    acCmd[1024];
    int     i = 0;
    GString strCmd;
    char    acFile[256];
    char    *szTmp;
    char    acResultFile[256];

    gos_get_file_rawname(szFile, acFile);
    szTmp = strchr(acFile, '.');
    if (!szTmp)
    {
        return NULL;
    }

    strcpy(szTmp, ".obj");

    strCmd.Append("cd ");
    strCmd.Append(m_acProjectPath);
    strCmd.Append("\r\n");

    strCmd.Append(m_acBuilderCmd);

    sprintf(acResultFile, "%s/build%u.log", gos_get_root_path(), m_ulThreadIndex);
    gos_delete_file(acResultFile);

    //-oDebug\about.obj
//    strCmd.Append(" -oDebug\\"); // -o放到param一起
    strCmd.Append(acFile);

    // such as "led\LedCfg.cpp"
    strCmd.Append(" -H ");
    strCmd.Append(szFile);

    //-H sys\about.cpp
//    strCmd.Append(" -H ");
//    strCmd.Append(szFile);

    strCmd.Append(" >");
    strCmd.Append(acResultFile);

    szTmp = strCmd.GetString();

    if (!RunBatCmd(m_ulThreadIndex, szTmp))
    {
        return NULL;
    }

    INT32   iSize = gos_get_file_size(acResultFile);

    if (iSize <= 0)
    {
        return NULL;
    }

    CHAR    *szBuf = (CHAR*)calloc(1, iSize+1);

    if (!szBuf)
    {
        return NULL;
    }

    FILE    *fp = fopen(acResultFile, "rb");

    if (!fp)
    {
        free(szBuf);
        return NULL;
    }

    if (iSize != fread(szBuf, 1, iSize, fp))
    {
        free(szBuf);
        fclose(fp);
        return NULL;
    }

    fclose(fp);

    return szBuf;
}

ThreadBuilder::ThreadBuilder(UINT32 ulThreadIndex, void* pPara):GThread(pPara)
{
    m_ulThreadIndex = ulThreadIndex;
    m_acProjectPath[0] = '\0';
    m_acBuilderCmd[0] = '\0';
    m_acSourceFile[0] = '\0';

    m_bRunning = TRUE;
    m_bBuilding = FALSE;
    m_pbRunning = (BOOL*)pPara;
    *m_pbRunning = TRUE;
}

VOID ThreadBuilder::Stop()
{
    m_bRunning = FALSE;
}

VOID ThreadBuilder::SetProjectPath(CHAR *szProjectPath)
{
    strcpy(m_acProjectPath, szProjectPath);

//    chdir(m_acProjectPath);
}

VOID ThreadBuilder::SetBuilderCmd(CHAR *szBuilderCmd)
{
    strcpy(m_acBuilderCmd, szBuilderCmd);
}

BOOL ThreadBuilder::SetSourceFile(CHAR *szSourceFile)
{
    if (m_bBuilding)
    {
        return FALSE;
    }

    if (!m_Mutex.Mutex())
    {
        return FALSE;
    }

    if (m_acSourceFile[0] != '\0')
    {
        m_Mutex.Unmutex();
        return FALSE;
    }

    m_bBuilding = TRUE;

    strcpy(m_acSourceFile, szSourceFile);

    m_Mutex.Unmutex();

    return TRUE;
}

GOS_THREAD_RET ThreadBuilder::ThreadEntry(void* pPara)
{
    HANDLE  hWnd = pPara;

    while(m_bRunning)
    {
        gos_sleep_1ms(1);

        if (!m_Mutex.Mutex())
        {
            continue;
        }

        if (m_acSourceFile[0] == '\0')
        {
            m_Mutex.Unmutex();
            continue;
        }

        m_Mutex.Unmutex();

        CHAR    *szResult = BuilderFile(m_acSourceFile);

        SendBuilderInfo(m_acSourceFile, szResult);
        free(szResult);
        m_acSourceFile[0] = '\0';
        m_bBuilding = FALSE;
    }

    GosLog(LOG_INFO, "thread(%u) exit", m_ulThreadIndex);
    *m_pbRunning = FALSE;

    return 0;
}

//---------------------------------------------------------------------------
void SendInnerMsg(UINT32 ulMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    UINT32              ulTotalLen;
    BUILD_INNER_MSG_T   *pstMsg = (BUILD_INNER_MSG_T*)calloc(1, sizeof(BUILD_INNER_MSG_T));

    if (!pstMsg)
    {
        return;
    }

    pstMsg->ulMsgID = ulMsgID;

    if (ulMsgLen > sizeof(pstMsg->aucMsg))
    {
        GosLog(LOG_ERROR, "msg len %u is too large", ulMsgLen);
        return;
    }

    if (pvMsg && ulMsgLen > 0)
    {
        memcpy(pstMsg->aucMsg, pvMsg, ulMsgLen);
    }
    pstMsg->ulMsgLen = ulMsgLen;

    if (!g_hMainHandle)
    {
        return;
    }

    if (!PostMessage((HWND)g_hMainHandle, WM_BUILD_INNER_MSG, 0, (LPARAM)pstMsg))
    {
        GosLog(LOG_ERROR, "thread send msg failed");
        return;
    }
}

VOID ThreadBuilder::SendBuilderInfo(CHAR *szSourceFile, CHAR *szResult)
{
    BUILD_INFO_T    *pstMsg = (BUILD_INFO_T*)m_stInnerMsg.aucMsg;

    if (!szResult)
    {
        szResult = "";
    }

    memset(m_stInnerMsg.aucMsg, 0, sizeof(m_stInnerMsg.aucMsg));
    strcpy(pstMsg->acSourceFile, szSourceFile);
    strncpy(pstMsg->acBuilderInfo, szResult, sizeof(pstMsg->acBuilderInfo)-1);

    SendInnerMsg(MSG_BUILD_INFO, pstMsg, sizeof(BUILD_INFO_T));
}
