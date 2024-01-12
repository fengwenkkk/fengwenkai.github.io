#include <stdio.h>

//#define __STDC__

//#undef __STDC__

#include "ffmpeg_pcm2wav.h"


FfmpegRecPCM::FfmpegRecPCM()
{
    m_ulAudioFormat = AUDIO_FMT_WAV;
    m_AudioCodec = NULL;
    m_AudioCodecContext = NULL;
    m_FormatContext = NULL;
    m_OutputFormat = NULL;
    m_ulSampleRate = 16000;      //PCM sample rate
    m_ulChannels = 2;            //PCM channel number
    m_ulBitePerSample = 16;

    m_ulFrameCount = 0;
    m_ulCurrFrameDataLen = 0;

    m_Frame = NULL;
}

FfmpegRecPCM::~FfmpegRecPCM()
{
    if (m_AudioCodecContext)
    {
        avcodec_free_context(&m_AudioCodecContext);
    }

    if (!m_FormatContext)
    {
        avformat_free_context(m_FormatContext);
    }
}

bool FfmpegRecPCM::InitFormat(char *szFile)
{
    // ������� Format ������
    int ret = avformat_alloc_output_context2(&m_FormatContext, NULL, NULL, szFile);

    if (ret < 0)
    {
        return false;
    }

    m_OutputFormat = m_FormatContext->oformat;

    // ������Ƶ��
    AVStream *audioStream = avformat_new_stream(m_FormatContext, m_AudioCodec);
    if (audioStream == NULL)
    {
        avformat_free_context(m_FormatContext);
        m_FormatContext = NULL;
        return false;
    }

    // �������еĲ���
    audioStream->id = m_FormatContext->nb_streams - 1;
    audioStream->time_base.num = 1;    // TODO
    audioStream->time_base.den = m_ulSampleRate;
    ret = avcodec_parameters_from_context(audioStream->codecpar, m_AudioCodecContext);
    if (ret < 0)
    {
        avformat_free_context(m_FormatContext);
        m_FormatContext = NULL;
        return false;
    }

    // ��ӡFormatContext��Ϣ
   // av_dump_format(m_FormatContext, 0, audioFileName.toLocal8Bit().data(), 1);

    // ���ļ�IO
    if (!(m_OutputFormat->flags & AVFMT_NOFILE))
    {
        ret = avio_open(&m_FormatContext->pb, szFile, AVIO_FLAG_WRITE);
        if (ret < 0)
        {
            avformat_free_context(m_FormatContext);
            return false;
        }
    }

    return true;
}

bool FfmpegRecPCM::Start(char *szFile)
{
    av_register_all();
    avcodec_register_all();

    m_ulFrameCount = 0;
    m_ulCurrFrameDataLen = 0;

    AVCodecID codecID = AV_CODEC_ID_PCM_S16LE;
    if (m_ulAudioFormat == AUDIO_FMT_MP3)
    {
        codecID = AV_CODEC_ID_MP3;
    }

    // ����Codec
    m_AudioCodec = avcodec_find_encoder(codecID);
    if (m_AudioCodec == NULL)
    {
        return false;
    }

    // ����������������
    m_AudioCodecContext = avcodec_alloc_context3(m_AudioCodec);
    if (m_AudioCodecContext == NULL)
    {
        return false;
    }

    // ���ò���
    m_AudioCodecContext->bit_rate = m_ulSampleRate*m_ulChannels*m_ulBitePerSample;
    m_AudioCodecContext->sample_rate = m_ulSampleRate;
    if (m_ulAudioFormat == AUDIO_FMT_WAV)
    {
        m_AudioCodecContext->sample_fmt = AV_SAMPLE_FMT_S16;
    }
    else
    {
        m_AudioCodecContext->sample_fmt = AV_SAMPLE_FMT_S16P;
    }

    m_AudioCodecContext->channels = m_ulChannels;
    m_AudioCodecContext->channel_layout = av_get_default_channel_layout(2);
    m_AudioCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    // �򿪱�����
    int ret = avcodec_open2(m_AudioCodecContext, m_AudioCodec, NULL);
    if (ret < 0)
    {
        return false;
    }

    // ������װ
    if (!InitFormat(szFile))
    {
        return false;
    }

    // д���ļ�ͷ
    ret = avformat_write_header(m_FormatContext, NULL);
    if (ret < 0)
    {
        return false;
    }

    // ����Frame
    m_Frame = av_frame_alloc();
    if (!m_Frame)
    {
        return false;
    }

    int nb_samples = 0;
    if (m_AudioCodecContext->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
    {
        nb_samples = m_ulSampleRate;
        nb_samples = 10000;
    }
    else
    {
        nb_samples = m_AudioCodecContext->frame_size;
    }

    // ����Frame�Ĳ���
    m_Frame->nb_samples = nb_samples;
    m_Frame->format = m_AudioCodecContext->sample_fmt;
    m_Frame->channel_layout = m_AudioCodecContext->channel_layout;

    // ���������ڴ�
    ret = av_frame_get_buffer(m_Frame, 0);
    if (ret < 0)
    {
        av_frame_free(&m_Frame);
        m_Frame = NULL;
        return false;
    }

    // ����FrameΪ��д
    ret = av_frame_make_writable(m_Frame);
    if (ret < 0)
    {
        av_frame_free(&m_Frame);
        m_Frame = NULL;
        return false;
    }

    return true;
}

void FfmpegRecPCM::Stop()
{
    if (m_ulCurrFrameDataLen > 0)
    {
        SaveData();
    }

    // д���ļ�β
    av_write_trailer(m_FormatContext);
    // �ر��ļ�IO
    avio_closep(&m_FormatContext->pb);
    // �ͷ�Frame�ڴ�

    if (m_Frame)
    {
        av_frame_free(&m_Frame);
        m_Frame = NULL;
    }
}

bool FfmpegRecPCM::RecPcmData(unsigned char *pucData, unsigned int ulDataLen)//QString fileName, QByteArray pcmData)
{
 /* av_register_all();
    avcodec_register_all();

    AVCodecID codecID = AV_CODEC_ID_PCM_S16LE;
    if (m_ulAudioFormat == AUDIO_FMT_MP3)
    {
        codecID = AV_CODEC_ID_MP3;
    }

    // ����Codec
    m_AudioCodec = avcodec_find_encoder(codecID);
    if (m_AudioCodec == NULL)
    {
        return;
    }

    // ����������������
    m_AudioCodecContext = avcodec_alloc_context3(m_AudioCodec);
    if (m_AudioCodecContext == NULL)
    {
        return;
    }

    // ���ò���
    m_AudioCodecContext->bit_rate = 64000;
    m_AudioCodecContext->sample_rate = 44100;
    if (formatType == AudioFormat_WAV)
    {
        m_AudioCodecContext->sample_fmt = AV_SAMPLE_FMT_S16;
    }
    else
    {
        m_AudioCodecContext->sample_fmt = AV_SAMPLE_FMT_S16P;
    }

    m_AudioCodecContext->channels = 1;
    m_AudioCodecContext->channel_layout = av_get_default_channel_layout(2);
    m_AudioCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    // �򿪱�����
    int ret = avcodec_open2(m_AudioCodecContext, m_AudioCodec, NULL);
    if (ret < 0)
        goto end;

    // ������װ
    if (!initFormat(fileName))
        goto end;

    // д���ļ�ͷ
    result = avformat_write_header(m_FormatContext, NULL);
    if (result < 0)
        goto end;

    // ����Frame
    AVFrame *frame = av_frame_alloc();
    if (frame == nullptr)
        goto end;

int nb_samples = 0;
    if (m_AudioCodecContext->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
    {
        nb_samples = 10000;
    }
    else
    {
        nb_samples = m_AudioCodecContext->frame_size;
    }

    // ����Frame�Ĳ���
    m_Frame->nb_samples = nb_samples;
    m_Frame->format = m_AudioCodecContext->sample_fmt;
    m_Frame->channel_layout = m_AudioCodecContext->channel_layout;

    // ���������ڴ�
    ret = av_frame_get_buffer(m_Frame, 0);
    if (ret < 0)
    {
        av_frame_free(&m_Frame);
        goto end;
    }

    // ����FrameΪ��д
    ret = av_frame_make_writable(m_Frame);
    if (ret < 0)
    {
        av_frame_free(&m_Frame);
        goto end;
    }
             */

    if (!m_Frame)
    {
        return false;
    }

    int perFrameDataSize = m_Frame->linesize[0];
    if (m_ulAudioFormat == AUDIO_FMT_MP3)
    {
        perFrameDataSize *= 2;
    }

    // ����Packet
    // ��������
    if (m_ulAudioFormat == AUDIO_FMT_WAV)
    {
        // �ϳ�WAV�ļ�
        if (m_ulCurrFrameDataLen == 0)
        {
            memset(m_Frame->data[0], 0, perFrameDataSize);
        }

        while(ulDataLen > 0)
        {
            int     iLeftLen = perFrameDataSize - m_ulCurrFrameDataLen;

            if (iLeftLen == 0)
            {
                m_ulCurrFrameDataLen = 0;
                SaveData();
                iLeftLen = perFrameDataSize;
            }

            if (iLeftLen > ulDataLen)
            {
                iLeftLen = ulDataLen;
            }

            ulDataLen -= iLeftLen;

            memcpy(m_Frame->data[0]+m_ulCurrFrameDataLen, pucData, iLeftLen);
            pucData += iLeftLen;
            m_ulCurrFrameDataLen += iLeftLen;
        }
    }
/*      else
    {
        memset(frame->data[0], 0, frame->linesize[0]);
        memset(frame->data[1], 0, frame->linesize[0]);
#if 1
        // �ϳ�MP3�ļ�
        int channelLayout = av_get_default_channel_layout(2);

        // ��ʽת�� S16->S16P
        SwrContext *swrContext = swr_alloc_set_opts(nullptr, channelLayout, AV_SAMPLE_FMT_S16P, 44100, \
            channelLayout, AV_SAMPLE_FMT_S16, 44100, 0, nullptr);
        swr_init(swrContext);

        uchar *pSrcData[1] = {0};
        pSrcData[0] = (uchar*)&(pcmData.data()[perFrameDataSize * i]);

        swr_convert(swrContext, frame->data, frame->nb_samples, \
            (const uint8_t **)pSrcData, frame->nb_samples);

        swr_free(&swrContext);
        AVRational rational;
        rational.den = m_AudioCodecContext->sample_rate;
        rational.num = 1;
        //frame->pts = av_rescale_q(0, rational, m_AudioCodecContext->time_base);
#endif
    }  */

    /*
    frame->pts = m_ulFrameCount++;
    // ����Frame
    ret = avcodec_send_frame(m_AudioCodecContext, frame);
    if (ret < 0)
    {
        return false;
    }

    // ���ձ�����Packet
    ret = avcodec_receive_packet(m_AudioCodecContext, pkt);
    if (ret < 0)
    {
        av_packet_free(&pkt);
        continue;
    }

    // д���ļ�
    av_packet_rescale_ts(pkt, m_AudioCodecContext->time_base, m_FormatContext->streams[0]->time_base);
    pkt->stream_index = 0;
    ret = av_interleaved_write_frame(m_FormatContext, pkt);
    if (ret < 0)
        continue;

    av_packet_free(&pkt);
                           */
}

bool FfmpegRecPCM::SaveData()
{
    AVPacket *pkt = av_packet_alloc();

    if (pkt == NULL)
    {
        return false;
    }

    av_init_packet(pkt);

    m_Frame->pts = m_ulFrameCount++;

    // ����Frame
    int ret = avcodec_send_frame(m_AudioCodecContext, m_Frame);
    if (ret < 0)
    {
        av_packet_free(&pkt);
        return false;
    }

    // ���ձ�����Packet
    ret = avcodec_receive_packet(m_AudioCodecContext, pkt);
    if (ret < 0)
    {
        av_packet_free(&pkt);
        return false;
    }

    // д���ļ�
    av_packet_rescale_ts(pkt, m_AudioCodecContext->time_base, m_FormatContext->streams[0]->time_base);
    pkt->stream_index = 0;
    ret = av_interleaved_write_frame(m_FormatContext, pkt);
    if (ret < 0)
    {
        av_packet_free(&pkt);
        return false;
    }

    av_packet_free(&pkt);
    return true;
}

#define PI      3.14159265

void test_pcm()
{
    FfmpegRecPCM    rec;
    char            *szFile = "e:\\rec\\test_wav.wav";
    short           *psBuf = (short*)calloc(2, 2*16000);
    double          dTmp;

    rec.Start(szFile);

    for (int i=0; i<10; i++)
    {
        for (int j=0; j<16000; j++)
        {
            dTmp = sin((2*PI*j)/1000);
            dTmp *= 32767;
            psBuf[j] = dTmp;
        }

        rec.RecPcmData((unsigned char*)psBuf, 2*2*16000);
    }

    rec.Stop();

}

