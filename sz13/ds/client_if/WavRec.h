#ifndef WavRecH
#define WavRecH
//---------------------------------------------------------------------------

class WavRec
{
public:
    WavRec();
    ~WavRec();

    VOID Init(CHAR *szFile, UINT32 ulSampleRate, UINT32 ulSampleBytes, BOOL bMergeMode);
    BOOL Rec(BOOL bLeftChan, UINT8 *pucData, UINT32 ulLen);
    VOID StopRec();

private:
    GMutex              m_Mutex;
    BOOL                bInited;
    FILE                *m_afp[2];
    UINT32              m_aulTotalDataLen[2];
    CHAR                *m_szFile;

    UINT32              m_ulSampleRate;
    UINT32              m_ulSampleBytes;
    BOOL                m_bMergeMode;
};

BOOL WriteWavHdr(FILE *fp, UINT32 ulChanNum, UINT32 ulSampleRate, UINT32 ulSampleBytes);
BOOL MergeWavFile(CHAR *szFile1, CHAR *szFile2, CHAR *szFile, BOOL bMergeMode);

UINT32 GetWavFileMediaLength(CHAR *szFile);

#endif
