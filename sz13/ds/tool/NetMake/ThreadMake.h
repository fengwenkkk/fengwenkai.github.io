#ifndef ThreadMakeH
#define ThreadMakeH

class ThreadMake :public GThread
{
public:
    ThreadMake(UINT32 ulThreadIndex, void* pPara);
    VOID Stop();

    GOS_THREAD_RET ThreadEntry(void* pPara);
    BOOL    IsMaking();
    UINT32 GetThreadIndex(){return m_ulThreadIndex;};

private:

    BOOL    *m_pbRunning;
    BOOL    m_bRunning;
    UINT32  m_ulThreadIndex;

    BOOL    m_bBuilding;
    GMutex  m_Mutex;

    MAKING_FILE_INFO_T  m_stFileInfo;

    AnsiString  m_strResult;

    BOOL    m_bBuilderSucc;

    BOOL    Make();
    VOID    SendMakeResult();
};

typedef struct
{
    UINT32      ulIndex;
    UINT32      ulAllocTime;

    MAKING_FILE_INFO_T  stFileInfo;

    ThreadMake  *pThread;
}FILE_ALLOC_INFO_T;

void InitThreadMake();
ThreadMake* GetIdleThreadMake();

namespace MakingFilePool
{
    VOID Init(CHAR *szTaskName);
    VOID Init(UINT32 ulIndex);
    BOOL IsEmpty();
    BOOL Alloc(CHAR *szTaskName, UINT32 &ulIndex);
    BOOL Alloced(UINT32 ulIndex, MAKING_FILE_INFO_T &stInfo);
    BOOL AllocFile(ThreadMake *pThread, MAKING_FILE_INFO_T &stFileInfo);
    BOOL Free(ThreadMake *pThread, CHAR *szMakeName);
}

#endif
