#ifndef IPHThreadH
#define IPHThreadH
//---------------------------------------------------------------------------

#define MSG_PIS             1

class IPHThread:public GThread
{
public:
    IPHThread(HWND hWnd);
    GOS_THREAD_RET ThreadEntry(void* pPara);
    BOOL Init();

private:
    UINT8       m_aucRecvBuf[64*1024];
    HWND        m_hWnd;
    HANDLE      m_hPcmPlay;

    VOID        HandleRecvMsg();
    INT32       ListenMsg(UINT32 ulTimeout);

};


#endif
