#ifndef ThreadBuilderH
#define ThreadBuilderH

#define WM_BUILD_INNER_MSG        (WM_USER+0x20)

#define MSG_BUILD_INFO      1

#pragma pack(push, 4)
typedef struct
{
    UINT32      ulMsgID;
    UINT32      ulMsgLen;
    UINT8       aucMsg[64*1024];
}BUILD_INNER_MSG_T;

typedef struct
{
    CHAR        acSourceFile[256];
    CHAR        acBuilderInfo[60*1024];
}BUILD_INFO_T;

#pragma pack(pop)


class ThreadBuilder :public GThread
{
public:
    ThreadBuilder(UINT32 ulThreadIndex, void* pPara);

    VOID Stop();

    GOS_THREAD_RET ThreadEntry(void* pPara);
    VOID   SetProjectPath(CHAR *szProjectPath);
    VOID   SetBuilderCmd(CHAR *szBuilderCmd);
    BOOL   SetSourceFile(CHAR *szSourceFile);

private:
    BOOL    *m_pbRunning;
    BOOL    m_bRunning;
    UINT32  m_ulThreadIndex;

    BOOL    m_bBuilding;
    GMutex  m_Mutex;
    BUILD_INNER_MSG_T   m_stInnerMsg;

    CHAR    m_acProjectPath[1024];
    CHAR    m_acBuilderCmd[64*1024];
    CHAR    m_acSourceFile[256];

    BOOL    m_bBuilderSucc;
    CHAR    m_acBuilderInfo[64*1024];

    CHAR*   BuilderFile(CHAR *szFile);
    VOID    SendBuilderInfo(CHAR *szSourceFile, CHAR *szResult);
};


#endif
