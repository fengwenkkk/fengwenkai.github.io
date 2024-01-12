//---------------------------------------------------------------------------

#ifndef ffmpeg_pcm2wavH
#define ffmpeg_pcm2wavH
//---------------------------------------------------------------------------

extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
}

#define AUDIO_FMT_WAV       1
#define AUDIO_FMT_MP3       2

class FfmpegRecPCM
{
public:
    FfmpegRecPCM();
    ~FfmpegRecPCM();

    bool Start(char *szFile);
    bool RecPcmData(unsigned char *pucData, unsigned int ulDataLen);
    void Stop();

private:
    AVCodec         *m_AudioCodec;
    AVCodecContext  *m_AudioCodecContext;
    AVFormatContext *m_FormatContext;
    AVOutputFormat  *m_OutputFormat;

    AVFrame         *m_Frame;
    unsigned int    m_ulAudioFormat;
    unsigned int    m_ulSampleRate;     // 8000/16000/44100
    unsigned int    m_ulChannels;
    unsigned int    m_ulBitePerSample;

    unsigned int    m_ulFrameCount;
    unsigned int    m_ulCurrFrameDataLen;

    bool InitFormat(char *szFile);
    bool SaveData();
};

#endif
