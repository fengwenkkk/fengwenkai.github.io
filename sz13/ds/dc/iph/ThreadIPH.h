#ifndef ThreadIPHH
#define ThreadIPHH

#include <vcl.h>

class ThreadIPH: public GThread
{
public:
    ThreadIPH(HWND hWin, UINT16 usUDPPort, WavRec *pWavRec);
    VOID Stop();

    VOID StopPlay();
    VOID StartPlay(UINT16 usTrainUnitID, UINT8 ucCarriageID, UINT8 ucIPHDevID);
    VOID PushData(UINT8 ucChanNum, UINT16 usSampleRate, UINT8 ucSampleBits,
                         UINT8 *pucPCMData, UINT16 usPCMDataLen);

    virtual GOS_THREAD_RET ThreadEntry(void* pPara);

private:
    WavRec      *m_pWavRec;
    GMutex      m_Mutex;
    BOOL        m_bRunning;
    UINT16      m_usUDPPort;
    BOOL        m_bIsStoppingPlay;

    BOOL        m_bPlaying;
//  HANDLE      m_hPcmPlay;
    HWND        m_hWin;

    UINT16      m_usTrainUnitID;
    UINT8       m_ucCarriageID;
    UINT8       m_ucIPHDevID;

    AnsiString  m_strPlayDevice;
};

//---------------------------------------------------------------------------
#endif
