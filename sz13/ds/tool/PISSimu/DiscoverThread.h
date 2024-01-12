#ifndef DiscoverThreadH
#define DiscoverThreadH
//---------------------------------------------------------------------------

#define MSG_DISCOVER            10
/*
class DiscoverThread:public GThread
{
public:
    DiscoverThread(HWND hWnd, UINT8 *pucLocalAddr, UINT8 *pucTrainAddr);
    GOS_THREAD_RET ThreadEntry(void* pPara);

private:
    HWND        m_hWnd;
    UINT8       m_aucLocalAddr[4];
    UINT8       m_aucTrainAddr[4];
    UINT8       m_aucBuf[64*1024];

    BOOL DiscoverPISServer(UINT8 *pucLocalAddr);
    VOID SendMsg(PIS_FIND_PIS_REQ_T &stMsg);
}; */


#endif
