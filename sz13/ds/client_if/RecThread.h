#ifndef RecThreadH
#define RecThreadH
//---------------------------------------------------------------------------

//#include "ffmpeg_pcm2wav.h"

typedef struct
{
    UINT8       aucRec[2];
    INT64       i64Time;
}REC_BUF_T;

#define REC_STATE_IDLE      0
#define REC_STATE_WORK      1
#define REC_STATE_CLOSING   2

#define REC_STATE_FREEING   10
#define REC_STATE_FREED     11

#define REC_STATE_CLOSED    REC_STATE_IDLE

class RecThread:public GThread
{
public:
    RecThread(BOOL bMergeMode);
    GOS_THREAD_RET ThreadEntry(void* pPara);
    BOOL StartRec(CHAR *szFile, UINT32 ulSampleRate, UINT32 ulSampleBytes);
    VOID StopRec();
    VOID Free();
    VOID RecLocal(UINT8 *pucData, UINT32 ulLen, INT64 i64Time);
    VOID RecRemote(UINT8 *pucData, UINT32 ulLen, INT64 i64Time);

private:
    GMutex              m_Mutex;
    BOOL                m_bMergeMode;
    FILE                *m_fp;
    UINT32              m_ulTotalDataLen;

    UINT32              m_ulRecState;
    BOOL                m_bNeedSave;

    UINT32              m_ulSampleRate;
    UINT32              m_ulSampleBytes;
    UINT32              m_ulBytesPerSec;

    VECTOR<REC_BUF_T>   m_vLocalRec;
    VECTOR<REC_BUF_T>   m_vRemoteRec;

    VECTOR<REC_BUF_T>   m_vTmpLocalRec;
    VECTOR<REC_BUF_T>   m_vTmpRemoteRec;

    VOID WriteWaveFileHdr();
    INT64 GetSampleTime(UINT32 ulIndex);
    VOID Save(INT64 i64StartTime, INT64 i64EndTime);
    VOID SaveCheck(BOOL bSaveAll);
};

#endif
