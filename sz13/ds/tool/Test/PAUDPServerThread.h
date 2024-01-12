//---------------------------------------------------------------------------

#ifndef PAUDPServerThreadH
#define PAUDPServerThreadH

class PAUDPThread:public GThread
{
public:
    PAUDPThread(HWND hWnd);
    GOS_THREAD_RET ThreadEntry(void* pPara);
    BOOL Init();

private:
    UINT8       m_aucRecvBuf[64*1024];
    HWND        m_hWnd;
    HANDLE      m_hPcmPlay;

    INT32       ListenMsg(UINT32 ulTimeout);

};

//---------------------------------------------------------------------------
#endif
