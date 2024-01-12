#ifndef ThreadRTPPlayH
#define ThreadRTPPlayH

class ThreadRTPPlay :public GThread
{
public:
    ThreadRTPPlay(BOOL bTcpMode, HWND pPara, UINT32 *pulPlayStatus);

    VOID Stop();

    GOS_THREAD_RET ThreadEntry(VOID* pPara);
    VOID   SetURL(CHAR *szURL);

private:
    BOOL        m_bTcpMode;
    BOOL        m_bRunning;
    UINT32      *m_pulPlayStatus;
    CHAR        *m_szURL;
};


#endif
