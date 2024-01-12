//---------------------------------------------------------------------------

#ifndef PATCPServerThreadH
#define PATCPServerThreadH

#define MSG_PA_TCP       20

class PATCPThread:public GThread
{
public:
    PATCPThread(HWND hWnd);
    GOS_THREAD_RET ThreadEntry(void* pPara);
    BOOL Init();

private:
    HWND        m_hWnd;
    //UINT8     m_aucAddr[4];
    UINT8       m_aucRecvBuf[10*1024];

    VOID        HandleRecvMsg();
    INT32       RecvMsg(SOCKET stSocket);
    INT32       ListenMsg(UINT32 ulTimeout);

};

//---------------------------------------------------------------------------
#endif
