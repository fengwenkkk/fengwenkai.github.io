#ifndef PISThreadH
#define PISThreadH
//---------------------------------------------------------------------------

#define MSG_PIS             1

class PISThread:public GThread
{
public:
    PISThread(HWND hWnd, UINT8 *pucAddr);
    GOS_THREAD_RET ThreadEntry(void* pPara);
    BOOL Init();

private:
    HWND        m_hWnd;
    UINT8       m_aucAddr[4];
    UINT8       m_aucRecvBuf[64*1024];

    VOID        HandleRecvMsg();
    INT32       RecvMsg(SOCKET stSocket);
    INT32       ListenMsg(UINT32 ulTimeout);

};


#endif
