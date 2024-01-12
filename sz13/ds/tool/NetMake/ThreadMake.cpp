#include "g_include.h"
#include "GCommon.h"
#include "MakeUtil.h"
#include "ThreadMake.h"
#include "MakeMsg.h"
#include "ZipUtil.h"


bool    g_bTestMode = false;

extern HANDLE   g_hMainHandle;

UINT32          g_ulMaxThreadMakeNum = 2;

static UINT32   g_ulAllocedTime = 0xffffffff;        // 收到alloc应答消息后，设置为此值，表示已经分配成功

extern UINT32   g_ulMakeingActiveTime;
extern bool     g_bAgentMode;

typedef struct
{
    ThreadMake      *pThread;
    BOOL            *pbRunning;
}THREAD_INFO_T;

static VECTOR<THREAD_INFO_T>       g_vThread;

static VECTOR<FILE_ALLOC_INFO_T>       g_vAllocInfo;
static GMutex       g_MutexAlloc;

BOOL GetObjFile(CHAR *szProject, CHAR *szFile, CHAR *szObjFile)
{
    CHAR    *szTmp = gos_get_simple_file_name(szFile);

    if (!szTmp)
    {
        return FALSE;
    }

    sprintf(szObjFile, "%s/%s", szProject, szTmp);
    szTmp = gos_right_strchr(szObjFile, '.');
    if (!szTmp)
    {
        return FALSE;
    }

    strcpy(szTmp, ".obj");

    return TRUE;
}

void InitThreadMake()
{
    THREAD_INFO_T       stInfo;

    if (!g_bAgentMode)
    {
        return;
    }

    for (UINT32 i=0; i<g_ulMaxThreadMakeNum; i++)
    {
        memset(&stInfo, 0, sizeof(stInfo));

        stInfo.pbRunning = (BOOL*)malloc(sizeof(BOOL));
        stInfo.pThread = new ThreadMake(i+1, stInfo.pbRunning);
//      stInfo.ulAllocTime = 0;

        g_vThread.push_back(stInfo);

        stInfo.pThread->Start();
    }

//  MakingFilePool::Init();
}

ThreadMake* GetIdleThreadMake()
{
    MAP<ThreadMake*, BOOL*>::iterator it;

    for (UINT32 i=0; i<g_vThread.size(); i++)
    {
        if (!g_vThread[i].pThread->IsMaking())
        {
            return g_vThread[i].pThread;
        }
    }

    return NULL;
}

namespace MakingFilePool {

VOID Init(CHAR *szTaskName)
{
    AUTO_LOCK(g_MutexAlloc);

    FILE_ALLOC_INFO_T   stAllocInfo;

    g_vAllocInfo.clear();
    for (UINT32 i=0; i<g_ulMaxThreadMakeNum; i++)
    {
        memset(&stAllocInfo, 0, sizeof(stAllocInfo));
        stAllocInfo.ulIndex = i;
        strcpy(stAllocInfo.stFileInfo.acMakeName, szTaskName);
        g_vAllocInfo.push_back(stAllocInfo);
    }
}

VOID Init(UINT32 ulIndex)
{
    AUTO_LOCK(g_MutexAlloc);

    if (ulIndex >= g_vAllocInfo.size())
    {
        return;
    }

    memset(&g_vAllocInfo[ulIndex], 0, sizeof(g_vAllocInfo[ulIndex]));
    g_vAllocInfo[ulIndex].ulIndex = ulIndex;

    GosLog(LOG_INFO, "init alloc %u", ulIndex);
}

BOOL IsEmpty()
{
    AUTO_LOCK(g_MutexAlloc);

    for (UINT32 i=0; i<g_vAllocInfo.size(); i++)
    {
        if (g_vAllocInfo[i].ulAllocTime != 0 ||
            g_vAllocInfo[i].stFileInfo.acMakeName[0] != '\0' )
        {
            return FALSE;
        }
    }

    return TRUE;
}

VOID Check()
{
    AUTO_LOCK(g_MutexAlloc);

    UINT32  ulCurrTime = gos_get_uptime();
    UINT32  ulTimeout = 10;

    for (UINT32 i=0; i<g_vAllocInfo.size(); i++)
    {
        if (g_vAllocInfo[i].ulAllocTime == 0 ||
            g_vAllocInfo[i].ulAllocTime == g_ulAllocedTime)
        {
            continue;
        }

        if ((g_vAllocInfo[i].ulAllocTime+ulTimeout) < ulCurrTime)
        {
            memset(&g_vAllocInfo[i], 0, sizeof(g_vAllocInfo[i]));
            g_vAllocInfo[i].ulIndex = i;

            GosLog(LOG_INFO, "alloc %u reset", i);
        }
    }
}

BOOL Alloc(CHAR *szTaskName, UINT32 &ulIndex)
{
    Check();

    AUTO_LOCK(g_MutexAlloc);

    for (UINT32 i=0; i<g_vAllocInfo.size(); i++)
    {
        if (g_vAllocInfo[i].ulAllocTime == 0 &&
            g_vAllocInfo[i].stFileInfo.acMakeName[0] != '\0' )
        {
            strcpy(g_vAllocInfo[i].stFileInfo.acMakeName, szTaskName);
            ulIndex = g_vAllocInfo[i].ulIndex;
            g_vAllocInfo[i].ulAllocTime = gos_get_uptime();

            GosLog(LOG_INFO, "alloc %u", ulIndex);

            return TRUE;
        }
    }

    return FALSE;
}

BOOL Alloced(UINT32 ulIndex, MAKING_FILE_INFO_T &stFileInfo)
{
    AUTO_LOCK(g_MutexAlloc);

    if (ulIndex >= g_vAllocInfo.size())
    {
        return FALSE;
    }

    if (!GetObjFile(stFileInfo.acProject, stFileInfo.acFile, stFileInfo.acObjFile))
    {
        return FALSE;
    }

    GosLog(LOG_INFO, "alloced %u with file %s", ulIndex, stFileInfo.acFile);

    g_vAllocInfo[ulIndex].ulAllocTime = g_ulAllocedTime;
    memcpy(&g_vAllocInfo[ulIndex].stFileInfo, &stFileInfo, sizeof(stFileInfo));

    sprintf(g_vAllocInfo[ulIndex].stFileInfo.acFile, "%s/%s", stFileInfo.acSrcPath, stFileInfo.acFile);

    return TRUE;
}

BOOL AllocFile(ThreadMake *pThread, MAKING_FILE_INFO_T &stFileInfo)
{
    AUTO_LOCK(g_MutexAlloc);

    for (UINT32 i=0; i<g_vAllocInfo.size(); i++)
    {
        if (g_vAllocInfo[i].stFileInfo.acFile[0] != '\0' &&
            g_vAllocInfo[i].pThread == NULL )
        {
            memcpy(&stFileInfo, &g_vAllocInfo[i].stFileInfo, sizeof(stFileInfo));
            g_vAllocInfo[i].pThread = pThread;

            GosLog(LOG_INFO, "alloc file %s to thread %u", stFileInfo.acFile, pThread->GetThreadIndex());

            return TRUE;
        }
    }

    return FALSE;
}

BOOL Free(ThreadMake *pThread, CHAR *szMakeName)
{
    AUTO_LOCK(g_MutexAlloc);

    for (UINT32 i=0; i<g_vAllocInfo.size(); i++)
    {
        if (g_vAllocInfo[i].pThread == pThread)
        {
            memset(&g_vAllocInfo[i], 0, sizeof(g_vAllocInfo[i]));
            g_vAllocInfo[i].ulIndex = i;
            strcpy(g_vAllocInfo[i].stFileInfo.acMakeName, szMakeName);

            GosLog(LOG_INFO, "alloc %u is free", i);

            return TRUE;
        }
    }

    GosLog(LOG_ERROR, "thread %u free failed", pThread->GetThreadIndex());

    return FALSE;
}

};

// 没有目录的obj
AnsiString GetObjName(CHAR *szFile)
{
    CHAR        acRawFile[128];
    CHAR        *szTmp;

    gos_get_file_rawname(szFile, acRawFile);
    szTmp = gos_right_strchr(acRawFile, '.');
    if (szTmp)
    {
        *szTmp = '\0';
    }

    strcat(acRawFile, ".obj");

    return acRawFile;
}

BOOL ThreadMake::Make()
{
    char        acResultFile[256];
    AnsiString  strCmd;
    FILE        *fp;

    m_strResult = "";

    strCmd = GetMakeCmd(m_stFileInfo);

    sprintf(acResultFile, "%s/build%u.log", gos_get_root_path(), m_ulThreadIndex);
    gos_delete_file(acResultFile);

    strCmd.cat_printf(" >%s", acResultFile);

    GosLog(LOG_INFO, "thread %u is making %s", m_ulThreadIndex, m_stFileInfo.acFile);

    if (g_bTestMode)
    {
        gos_sleep(2+MOD(gos_get_rand(),3));
        m_strResult = m_stFileInfo.acFile;
        return TRUE;
    }
    else if (!RunBatCmd(m_ulThreadIndex, strCmd.c_str()))
    {
        GosLog(LOG_INFO, "thread %u make %s failed", m_ulThreadIndex, m_stFileInfo.acFile);
        return FALSE;
    }

    INT32   iSize = gos_get_file_size(acResultFile);

    if (iSize <= 0)
    {
        GosLog(LOG_INFO, "thread %u make %s failed, invalid result file", m_ulThreadIndex, m_stFileInfo.acFile);
        return FALSE;
    }

    CHAR    *szBuf = (CHAR*)calloc(1, iSize+1);

    if (!szBuf)
    {
        return FALSE;
    }

    fp = fopen(acResultFile, "rb");
    if (!fp)
    {
        free(szBuf);
        GosLog(LOG_INFO, "thread %u make %s failed, open result file %s failed", m_ulThreadIndex, m_stFileInfo.acFile, acResultFile);

        return FALSE;
    }

    if (iSize != fread(szBuf, 1, iSize, fp))
    {
        free(szBuf);
        fclose(fp);
        GosLog(LOG_INFO, "thread %u make %s failed, read result file %s failed", m_ulThreadIndex, m_stFileInfo.acFile, acResultFile);

        return FALSE;
    }

    fclose(fp);

    m_strResult = szBuf;
    free(szBuf);

    GosLog(LOG_INFO, "thread %u make %s succ", m_ulThreadIndex, m_stFileInfo.acFile);

    return TRUE;
}

ThreadMake::ThreadMake(UINT32 ulThreadIndex, void* pPara):GThread(pPara)
{
    m_ulThreadIndex = ulThreadIndex;
    memset(&m_stFileInfo, 0, sizeof(m_stFileInfo));

    m_bRunning = TRUE;
    m_bBuilding = FALSE;
    m_pbRunning = (BOOL*)pPara;
    *m_pbRunning = TRUE;

//    m_ulAllocFileTime = 0;
}

VOID ThreadMake::Stop()
{
    m_bRunning = FALSE;
}

BOOL ThreadMake::IsMaking()
{
    AUTO_LOCK(m_Mutex);

    return m_bBuilding;
}

GOS_THREAD_RET ThreadMake::ThreadEntry(void* pPara)
{
    while(m_bRunning)
    {
        gos_sleep_1ms(1);

        if (!m_Mutex.Mutex())
        {
            continue;
        }

        if (m_stFileInfo.acFile[0] == '\0')
        {
            MakingFilePool::AllocFile(this, m_stFileInfo);
        }

        if (m_stFileInfo.acFile[0] == '\0')
        {
            m_Mutex.Unmutex();
            continue;
        }

        m_bBuilding = TRUE;

        m_Mutex.Unmutex();

        g_ulMakeingActiveTime = gos_get_uptime();
        if (Make())
        {
            SendMakeResult();
        }

        MakingFilePool::Free(this, m_stFileInfo.acMakeName);

        memset(&m_stFileInfo, 0, sizeof(m_stFileInfo));
        m_bBuilding = FALSE;
    }

    GosLog(LOG_INFO, "thread(%u) exit", m_ulThreadIndex);
    *m_pbRunning = FALSE;

    return 0;
}

//---------------------------------------------------------------------------
VOID ThreadMake::SendMakeResult()
{
    REPORT_MAKE_INFO_REQ_T  stMsg;
    CHAR                    *szObjFile = m_stFileInfo.acObjFile + strlen(gos_get_root_path()) + 1;

    memset(&stMsg, 0, sizeof(stMsg));
    strcpy(stMsg.acMakeName, m_stFileInfo.acMakeName);
    strcpy(stMsg.acFile, m_stFileInfo.acFile);
    strncpy(stMsg.acMakeResult, m_strResult.c_str(), sizeof(stMsg.acMakeResult)-1);
    if (gos_file_exist(m_stFileInfo.acObjFile))
    {
        char    acZipFile[256];

        sprintf(acZipFile, "%s.zip", m_stFileInfo.acObjFile);
        ZipFile(m_stFileInfo.acObjFile, acZipFile);
        if (acZipFile)
        {
            szObjFile = acZipFile + strlen(gos_get_root_path()) + 1;
            strcpy(stMsg.acObjFile, szObjFile);
            gos_delete_file(m_stFileInfo.acObjFile);
        }
        else
        {
            strcpy(stMsg.acObjFile, szObjFile);
        }
        GosLog(LOG_INFO, "send make result of %s, obj file is %s", stMsg.acFile, szObjFile);
    }
    else
    {
        GosLog(LOG_INFO, "send make result of %s, make failed", stMsg.acFile);
    }

    SendPeer(&m_stFileInfo.stPeerAddr, EV_REPORT_MAKE_INFO_REQ, &stMsg, sizeof(stMsg));
}

