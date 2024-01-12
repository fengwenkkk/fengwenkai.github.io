#ifndef ThreadIPHH
#define ThreadIPHH

#include <vcl.h>

class ThreadIPH: public GThread
{
public:
    ThreadIPH(HWND hWin, UINT16 usUDPPort);
    VOID Stop();

    VOID StopPlay();
    VOID StartPlay(UINT16 usTrainUnitID, UINT8 ucCarriageID, UINT8 ucIPHDevID);

    virtual GOS_THREAD_RET ThreadEntry(void* pPara);

private:
    GMutex      m_Mutex;
    BOOL        m_bRunning;
    UINT16      m_usUDPPort;
    BOOL        m_bIsStoppingPlay;

    BOOL        m_bPlaying;
    HANDLE      m_hPcmPlay;
    HWND        m_hWin;

    UINT16      m_usTrainUnitID;
    UINT8       m_ucCarriageID;
    UINT8       m_ucIPHDevID;
};

//---------------------------------------------------------------------------
#endif
