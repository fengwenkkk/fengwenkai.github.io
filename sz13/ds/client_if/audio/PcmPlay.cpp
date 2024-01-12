#include "windows.h"
#include <mmsystem.h>
#include <dsound.h>
#include "PcmPlay.h"

#include "thread.h"

#include "g_include.h"

GUID MY_IID_IDirectSoundBuffer8 = {0x6825a449, 0x7524, 0x4d82, {0x92, 0x0f, 0x50, 0xe3, 0x6a, 0xb3, 0xab, 0x1e}};

#define MAX_AUDIO_BUF       4

class PcmPlay:public Thread
{
public:
    PcmPlay();
    virtual ~PcmPlay();
    BOOL Init(HWND hWnd, int channels, int sample_rate, int bits_per_sample);
    virtual int Run();
    void PushData(char *pData, int iLenth);

private:
    char *m_Audiobuf;
    int m_iAudioLen;
    GMutex m_bufLock;
    int m_iBufNofitySize;
    int m_iBufNum;

    void Log(int iLogLebel, const char *szFormat, ...);

private:
    BOOL            m_bInit;
    IDirectSound8 * m_pDS;
    IDirectSoundBuffer8 *m_pDSBuffer8;  //used to manage sound buffers.
    IDirectSoundBuffer *m_pDSBuffer;
    IDirectSoundNotify8 *m_pDSNotify;
    DSBPOSITIONNOTIFY m_pDSPosNotify[MAX_AUDIO_BUF];
    HANDLE m_event[MAX_AUDIO_BUF];

    DWORD m_ulOffset;
    DWORD m_ulRes;
    WAVEFORMATEX m_WAVEFORMATEX;
};

PcmPlay::PcmPlay()
{
    m_bInit = FALSE;
    m_iAudioLen = 0;
    m_pDS = NULL;
    m_pDSBuffer8 = NULL;    //used to manage sound buffers.
    m_pDSBuffer = NULL;
    m_pDSNotify = NULL;
    m_iBufNum = 16;

    for (int i = 0; i < MAX_AUDIO_BUF; i++)
    {
        m_event[i] = NULL;
    }

    m_ulRes = WAIT_OBJECT_0;
    m_Audiobuf = NULL;
}

PcmPlay::~PcmPlay()
{
    if (NULL != m_pDSBuffer)
    {
        m_pDSBuffer->Release();
        m_pDSBuffer = NULL;
    }

    if (NULL != m_pDSBuffer8)
    {
        m_pDSBuffer8->Release();
        m_pDSBuffer8 = NULL;
    }

    if (NULL != m_pDS)
    {
        m_pDS->Release();
        m_pDS = NULL;
    }

    for (int i = 0; i<MAX_AUDIO_BUF; i++)
    {
        if (NULL != m_event[i])
        {
            CloseHandle(m_event[i]);
            m_event[i] = NULL;
        }
    }

    if (NULL!= m_Audiobuf)
    {
        delete[] m_Audiobuf;
    }
}

void PcmPlay::Log(int iLogLevel, const char *szFormat, ...)
{
    va_list vaLog;
    char    acLog[4096];

    va_start(vaLog, szFormat);

    memset(acLog, 0, sizeof(acLog));
    vsnprintf(acLog, sizeof(acLog)-1, szFormat, vaLog);

    va_end(vaLog);

    GosLog(iLogLevel, "== PCM DEBUG == %s", acLog);
}

int PcmPlay::Run()
{
    LPVOID buf = NULL;
    DWORD  buf_len = 0;

    if (!m_bInit)
    {
        return -1;
    }

    if ((m_ulRes >= WAIT_OBJECT_0) && (m_ulRes <= WAIT_OBJECT_0 + 3))
    {
        m_pDSBuffer8->Lock(m_ulOffset, m_iBufNofitySize, &buf, &buf_len, NULL, NULL, 0);
        if (m_iAudioLen > m_iBufNofitySize)
        {
            GAutoLock lock(m_bufLock);
            memcpy(buf, m_Audiobuf, m_iBufNofitySize);
            m_iAudioLen = m_iAudioLen - m_iBufNofitySize;
            memmove(m_Audiobuf, m_Audiobuf + m_iBufNofitySize, m_iAudioLen);

        //    Log(LOG_DETAIL, "push %u", m_iBufNofitySize);
        }
        else
        {
            memset(buf, 0, m_iBufNofitySize);
        //    Log(LOG_WARN, "push empty");
        }

        m_ulOffset += buf_len;
        m_ulOffset %= (m_iBufNofitySize * MAX_AUDIO_BUF);
        m_pDSBuffer8->Unlock(buf, buf_len, NULL, 0);
    }

    m_ulRes = WaitForMultipleObjects(MAX_AUDIO_BUF, m_event, FALSE, INFINITE);
    return 1;
}

BOOL PcmPlay::Init(HWND hWnd, int channels, int sample_rate, int bits_per_sample)
{
    m_iBufNofitySize = sample_rate*channels;
    m_ulOffset = m_iBufNofitySize;
    m_Audiobuf = new char[m_iBufNofitySize * m_iBufNum];

    //Init DirectSound
    if (FAILED(DirectSoundCreate8(NULL, &m_pDS, NULL)))
    {
        Log(LOG_ERROR, "open direct sound failed");
        return FALSE;
    }

    if (FAILED(m_pDS->SetCooperativeLevel(hWnd, DSSCL_NORMAL)))
    {
        Log(LOG_ERROR, "open direct sound at SetCooperativeLevel");
        return FALSE;
    }

    DSBUFFERDESC dsbd;

    memset(&dsbd, 0, sizeof(dsbd));
    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2;
    dsbd.dwBufferBytes = MAX_AUDIO_BUF * m_iBufNofitySize;
    dsbd.lpwfxFormat = &m_WAVEFORMATEX;//(WAVEFORMATEX*)malloc(sizeof(WAVEFORMATEX));
    dsbd.lpwfxFormat->wFormatTag = WAVE_FORMAT_PCM;
    /* format type */
    (dsbd.lpwfxFormat)->nChannels = channels;
    /* number of channels (i.e. mono, stereo...) */
    (dsbd.lpwfxFormat)->nSamplesPerSec = sample_rate;
    /* sample rate */
    (dsbd.lpwfxFormat)->nAvgBytesPerSec = sample_rate * (bits_per_sample / 8)*channels;
    /* for buffer estimation */
    (dsbd.lpwfxFormat)->nBlockAlign = (bits_per_sample / 8)*channels;
    /* block size of data */
    (dsbd.lpwfxFormat)->wBitsPerSample = bits_per_sample;
    /* number of bits per sample of mono data */
    (dsbd.lpwfxFormat)->cbSize = 0;

    if (FAILED(m_pDS->CreateSoundBuffer(&dsbd, &m_pDSBuffer, NULL)))
    {
        Log(LOG_ERROR, "open direct sound at CreateSoundBuffer");
        return FALSE;
    }

    if (FAILED(m_pDSBuffer->QueryInterface(MY_IID_IDirectSoundBuffer8, (LPVOID*)&m_pDSBuffer8)))
    {
        Log(LOG_ERROR, "open direct sound at QueryInterface of IID_IDirectSoundBuffer8");
        return FALSE;
    }
    //Get IDirectSoundNotify8
    if (FAILED(m_pDSBuffer8->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&m_pDSNotify)))
    {
        Log(LOG_ERROR, "open direct sound at QueryInterface of IID_IDirectSoundNotify");
        return FALSE;
    }

    for (int i = 0; i<MAX_AUDIO_BUF; i++)
    {
        m_pDSPosNotify[i].dwOffset = i * m_iBufNofitySize;
        m_event[i] = ::CreateEvent(NULL, FALSE, FALSE, NULL);
        m_pDSPosNotify[i].hEventNotify = m_event[i];
    }

    m_pDSNotify->SetNotificationPositions(MAX_AUDIO_BUF, m_pDSPosNotify);
    m_pDSNotify->Release();
    m_pDSBuffer8->SetCurrentPosition(0);
    m_pDSBuffer8->Play(0, 0, DSBPLAY_LOOPING);

    m_bInit = TRUE;

    return TRUE;
}

void PcmPlay::PushData(char *pData, int iLen)
{
    GAutoLock lock(m_bufLock);

    if ((m_iAudioLen+iLen) > m_iBufNofitySize*m_iBufNum)
    {
   //     Log(LOG_WARN, "buf over flow, lost data %d", iLen);
        iLen = m_iBufNofitySize*m_iBufNum - m_iAudioLen;
    }

    if (iLen > 0)
    {
        memcpy(m_Audiobuf+m_iAudioLen, pData, iLen);
        m_iAudioLen += iLen;
    }
  //  Log(LOG_DETAIL, "buf add data %d", iLen);
}

HANDLE PCMPlayInit(HWND hWnd, UINT32 ulChanNum, UINT32 ulSampleRate, UINT32 ulBitsPerSample)
{
    PcmPlay     *pcmPlay = new PcmPlay();

    if (!pcmPlay->Init(hWnd, ulChanNum, ulSampleRate, ulBitsPerSample))
    {
        delete pcmPlay;
        return NULL;
    }

    return (HANDLE)pcmPlay;
}

VOID PCMPlayStart(HANDLE hPcmPlay)
{
    PcmPlay     *pcmPlay = (PcmPlay*)hPcmPlay;

    if (pcmPlay)
    {
        pcmPlay->Start();
    }
}

VOID PCMPlayStop(HANDLE hPcmPlay)
{
    PcmPlay     *pcmPlay = (PcmPlay*)hPcmPlay;

    if (pcmPlay)
    {
        pcmPlay->Stop();

        delete pcmPlay;
    }
}

VOID PCMPlayPushData(HANDLE hPcmPlay, VOID *pvData, UINT32 ulLen)
{
    PcmPlay     *pcmPlay = (PcmPlay*)hPcmPlay;

    if (pcmPlay)
    {
        pcmPlay->PushData((char*)pvData, (int)ulLen);
    }
}

