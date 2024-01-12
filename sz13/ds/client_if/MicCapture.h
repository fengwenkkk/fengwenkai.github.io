#ifndef MicCaptureH
#define MicCaptureH

typedef void (*ON_MIC_DATA)(unsigned char *pucBuf, unsigned int ulBufSize);

class MicCapture
{
public:
    MicCapture(ON_MIC_DATA pfOnMicData);
    ~MicCapture();

    BOOL Open(int iSampleRate, int iBitsPerSample, int iChanNum, int iBufSize=1000);
    void Close();

    BOOL Start();
    void Stop();

private:
    void            *m_MicCap;
    ON_MIC_DATA     m_pfOnMicData;
};

#endif
