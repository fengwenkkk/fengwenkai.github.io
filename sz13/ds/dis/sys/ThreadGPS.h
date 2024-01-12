#ifndef THREAD_GPS_H
#define THREAD_GPS_H

#define EV_CHECK_STATUS_REPORT_IND      1

class ThreadGPS: public GThread
{
public:
    ThreadGPS(UINT16 usPort);
    ~ThreadGPS();

    VOID Stop();

    virtual GOS_THREAD_RET ThreadEntry(void* pPara);

    BOOL   Init();

private:
    SOCKET      m_stSock;
    BOOL        m_bRunning;
    UINT16      m_usPort;

    UINT8       m_aucMsg[8*1024];

    UINT32      m_ulLastSendCheckTime;
    UINT32      m_ulLastRecvCheckTime;

  //  VOID DaoLog(UINT32 ulLogLevel, const CHAR *szLog, ...);

    INT32 RecvMsg();
    VOID OnRecvMsg(CHAR *szMsg);
};

extern BOOL StartThreadGPS(UINT16 usPort);

//---------------------------------------------------------------------------
#endif
