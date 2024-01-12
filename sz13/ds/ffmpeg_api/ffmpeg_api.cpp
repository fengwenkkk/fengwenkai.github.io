//#include <stdio.h>

#define __STDC_CONSTANT_MACROS

/*
#ifdef __STDC__
#undef __STDC__
#endif
#define __STDC__    0
*/

//#define AV_NOWARN_DEPRECATED


extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
};

//#undef __STDC__

//#undef attribute_deprecated
//#define attribute_deprecateds

extern "C" void gos_sleep_1ms(unsigned int ms);
extern "C" unsigned long gos_get_uptime_1ms();

double ffmpeg_GetMediaFileLength(const char *szFile)
{
    AVFormatContext *ifmt = NULL;
    avformat_network_init();
    ifmt=avformat_alloc_context();

    int ret = avformat_open_input(&ifmt,szFile,NULL,NULL);
    double dTime = -1;

    avformat_find_stream_info(ifmt, 0);

    if (ret<0)
    {
        ret = -1;
        goto end;
    }

    dTime = (double)ifmt->duration;
    dTime = dTime/1000000; // 转换成秒

end:
    avformat_close_input(&ifmt);
    avformat_free_context(ifmt);

    return dTime;
}


/**
* 最简单的基于FFmpeg的视音频复用器
* Simplest FFmpeg Muxer
*
* 雷霄骅 Lei Xiaohua
* leixiaohua1020@126.com
* 中国传媒大学/数字电视技术
* Communication University of China / Digital TV Technology
* http://blog.csdn.net/leixiaohua1020
*
* 本程序可以将视频码流和音频码流打包到一种封装格式中。
* 程序中将AAC编码的音频码流和H.264编码的视频码流打包成
* MPEG2TS封装格式的文件。
* 需要注意的是本程序并不改变视音频的编码格式。
*
* This software mux a video bitstream and a audio bitstream
* together into a file.
* In this example, it mux a H.264 bitstream (in MPEG2TS) and
* a AAC bitstream file together into MP4 format file.
*
*/



/*
FIX: H.264 in some container format (FLV, MP4, MKV etc.) need
"h264_mp4toannexb" bitstream filter (BSF)
*Add SPS,PPS in front of IDR frame
*Add start code ("0,0,0,1") in front of NALU
H.264 in some container (MPEG2TS) don't need this BSF.
*/
//'1': Use H.264 Bitstream Filter
#define USE_H264BSF 0

/*
FIX:AAC in some container format (FLV, MP4, MKV etc.) need
"aac_adtstoasc" bitstream filter (BSF)
*/
//'1': Use AAC Bitstream Filter
#define USE_AACBSF 0

int ffmpeg_MixFile(const char *szAudioFile, const char *szVideoFile, const char *szMixFile)
{
    AVOutputFormat *ofmt = NULL;
    //Input AVFormatContext and Output AVFormatContext
    AVFormatContext *ifmt_ctx_v = NULL, *ifmt_ctx_a = NULL,*ofmt_ctx = NULL;
    AVPacket pkt;
    int ret;
    unsigned int i;
    int videoindex_v=-1,videoindex_out=-1;
    int audioindex_a=-1,audioindex_out=-1;
    int frame_index=0;
    int64_t cur_pts_v=0,cur_pts_a=0;

    const char *in_filename_v = szVideoFile; //"cuc_ieschool.h264";
    const char *in_filename_a = szAudioFile; //"huoyuanjia.mp3";
    const char *out_filename = szMixFile;

    av_register_all();
    avcodec_register_all();
    avformat_network_init();

    //Input
    if ((ret = avformat_open_input(&ifmt_ctx_v, in_filename_v, 0, 0)) < 0)
    {
        printf( "Could not open input file.");
        goto end;
    }

    if ((ret = avformat_find_stream_info(ifmt_ctx_v, 0)) < 0)
    {
        printf( "Failed to retrieve input stream information");
        goto end;
    }

    if ((ret = avformat_open_input(&ifmt_ctx_a, in_filename_a, 0, 0)) < 0)
    {
        printf( "Could not open input file.");
        goto end;
    }

    if ((ret = avformat_find_stream_info(ifmt_ctx_a, 0)) < 0)
    {
        printf( "Failed to retrieve input stream information");
        goto end;
    }

    av_dump_format(ifmt_ctx_v, 0, in_filename_v, 0);
    av_dump_format(ifmt_ctx_a, 0, in_filename_a, 0);

    //Output
    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
    if (!ofmt_ctx)
    {
        printf( "Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        goto end;
    }
    ofmt = ofmt_ctx->oformat;

    for (i = 0; i < ifmt_ctx_v->nb_streams; i++)
    {
        //Create output AVStream according to input AVStream
        if (ifmt_ctx_v->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            AVStream            *in_stream = ifmt_ctx_v->streams[i];
            AVCodecParameters   *in_codecpar = in_stream->codecpar;
            AVCodec             *codec = avcodec_find_decoder(in_codecpar->codec_id);
            AVStream            *out_stream = avformat_new_stream(ofmt_ctx, codec);//in_stream->codec->codec);


/*            AVCodecContext *cc = avcodec_alloc_context3(codec);
            avcodec_parameters_to_context(cc, in_codecpar);
            avcodec_open2(cc, codec, NULL);*/

            videoindex_v=i;
            if (!out_stream)
            {
                printf( "Failed allocating output stream\n");
                ret = AVERROR_UNKNOWN;
                goto end;
            }

            videoindex_out=out_stream->index;
            //Copy the settings of AVCodecContext
            if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0)
            {
                printf( "Failed to copy context from input to output stream codec context\n");
                goto end;
            }

            avcodec_parameters_copy(out_stream->codecpar, in_codecpar);
            avcodec_parameters_from_context(out_stream->codecpar, in_stream->codec);

            out_stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    /*      if (out_stream->codecpar->codec_id == AV_CODEC_ID_NONE)
            {
                out_stream->codecpar->codec_id = AV_CODEC_ID_H264;
            }

            if (out_stream->codecpar->bit_rate <= 0)
            {
                out_stream->codecpar->bit_rate = 512000;
            }

            if (out_stream->codecpar->width <= 0)
            {
                out_stream->codecpar->width = 1080;
                out_stream->codecpar->height = 720;
            }*/

            printf("video codec id=%d, tag=%d, bit_rate=%d, width=%d, height=%d\n",
                    out_stream->codecpar->codec_id, out_stream->codecpar->codec_tag, out_stream->codecpar->bit_rate,
                    out_stream->codecpar->width, out_stream->codecpar->height);

            out_stream->time_base = in_stream->codec->time_base;

        /*  if (out_stream->codec->time_base.num == 0)
            {
                out_stream->codec->time_base.num = 1;
            }

            if (out_stream->codec->time_base.den == 0)
            {
                out_stream->codec->time_base.den = 25;
            } */

#ifndef WIN32
            out_stream->codecpar->codec_tag = 0;
#endif
            out_stream->codec->codec_tag = 0;
            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            {
                out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;//CODEC_FLAG_GLOBAL_HEADER;
            }

            break;
        }
    }

    for (i = 0; i < ifmt_ctx_a->nb_streams; i++)
    {
        //Create output AVStream according to input AVStream
        if(ifmt_ctx_a->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            AVStream            *in_stream = ifmt_ctx_a->streams[i];
            AVCodecParameters   *in_codecpar = in_stream->codecpar;
            AVCodecID           codec_id = in_codecpar->codec_id; //   AV_CODEC_ID_AAC
            AVCodec             *codec = NULL;//avcodec_find_decoder(codec_id);//avcodec_find_decoder(in_codecpar->codec_id);           // AV_CODEC_ID_PCM_S16LE);//
            AVStream            *out_stream = avformat_new_stream(ofmt_ctx, codec);
//      AVStream            *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);

    /*        AVCodecContext *cc = avcodec_alloc_context3(codec);

            cc->bit_rate = 16000*2*2;
            cc->sample_rate = 16000;
            cc->sample_fmt = AV_SAMPLE_FMT_S16;

            cc->channels = 1;
            cc->channel_layout = av_get_default_channel_layout(2);
            cc->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

            avcodec_parameters_to_context(cc, in_codecpar);
            avcodec_open2(cc, codec, NULL); */

            audioindex_a=i;
            if (!out_stream)
            {
                printf( "Failed allocating output stream\n");
                ret = AVERROR_UNKNOWN;
                goto end;
            }
            audioindex_out=out_stream->index;
            //Copy the settings of AVCodecContext

            if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0)
            {
                printf( "Failed to copy context from input to output stream codec context\n");
                goto end;
            }

            avcodec_parameters_copy(out_stream->codecpar, in_codecpar);
            avcodec_parameters_from_context(out_stream->codecpar, in_stream->codec);

//          out_stream->codec->codec = codec;//in_stream->codec->codec;
//          out_stream->codecpar->codec_id = AV_CODEC_ID_AAC;

            out_stream->time_base = in_stream->codec->time_base;

    /*      if (out_stream->codec->time_base.num == 0)
            {
                out_stream->codec->time_base.num = 1;
            }

            if (out_stream->codec->time_base.den == 0)
            {
                out_stream->codec->time_base.den = 8000;
            }

            if (out_stream->codecpar->bit_rate <= 0)
            {
                out_stream->codecpar->bit_rate = 12000;
            }*/

            printf("audio codec id=%d, tag=%d, bit_rate=%d, bits/sample=%d\n",
                out_stream->codecpar->codec_id, out_stream->codecpar->codec_tag, out_stream->codecpar->bit_rate,
                out_stream->codecpar->bits_per_coded_sample);

#if 1//ndef WIN32
            //out_stream->codecpar->codec_tag = 0;
#endif
            out_stream->codec->codec_tag = 0;
            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            {
                out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
            }

            break;
        }
    }

    av_dump_format(ofmt_ctx, 0, out_filename, 1);
    if (!(ofmt->flags & AVFMT_NOFILE))
    {
        if (avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE) < 0)
        {
            printf( "Could not open output file '%s'", out_filename);
            goto end;
        }
    }

    //Write file header
    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0)
    {
        printf( "avformat_write_header failed: %d\n", ret);
        goto end;
    }

    //FIX
#if USE_H264BSF
    AVBitStreamFilterContext* h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");
#endif
#if USE_AACBSF
    //AVBitStreamFilterContext* aacbsfc =  av_bitstream_filter_init("aac_adtstoasc");
#endif

    while (1)
    {
        AVFormatContext *ifmt_ctx;
        int stream_index=0;
        AVStream *in_stream, *out_stream;

        //Get an AVPacket
        if(av_compare_ts(cur_pts_v,ifmt_ctx_v->streams[videoindex_v]->time_base,cur_pts_a,ifmt_ctx_a->streams[audioindex_a]->time_base) <= 0)
        {
            ifmt_ctx=ifmt_ctx_v;
            stream_index=videoindex_out;

            if(av_read_frame(ifmt_ctx, &pkt) >= 0)
            {
                do
                {
                    in_stream  = ifmt_ctx->streams[pkt.stream_index];
                    out_stream = ofmt_ctx->streams[stream_index];

                    if(pkt.stream_index==videoindex_v)
                    {
                        //FIX：No PTS (Example: Raw H.264)
                        //Simple Write PTS
                        if(pkt.pts==AV_NOPTS_VALUE)
                        {
                            //Write PTS
                            AVRational time_base1=in_stream->time_base;
                            //Duration between 2 frames (us)
                            int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate);
                            //Parameters
                            pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            pkt.dts=pkt.pts;
                            pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            frame_index++;
                        }

                        cur_pts_v=pkt.pts;
                        break;
                    }
                }while(av_read_frame(ifmt_ctx, &pkt) >= 0);
            }
            else
            {
                break;
            }
        }
        else
        {
            ifmt_ctx=ifmt_ctx_a;
            stream_index=audioindex_out;
            if(av_read_frame(ifmt_ctx, &pkt) >= 0)
            {
                do
                {
                    in_stream  = ifmt_ctx->streams[pkt.stream_index];
                    out_stream = ofmt_ctx->streams[stream_index];

                    if(pkt.stream_index==audioindex_a)
                    {
                        //FIX：No PTS
                        //Simple Write PTS
                        if(pkt.pts==AV_NOPTS_VALUE)
                        {
                            //Write PTS
                            AVRational time_base1=in_stream->time_base;
                            //Duration between 2 frames (us)
                            int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate);
                            //Parameters
                            pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            pkt.dts=pkt.pts;
                            pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            frame_index++;
                        }
                        cur_pts_a=pkt.pts;

                        break;
                    }
                }
                while(av_read_frame(ifmt_ctx, &pkt) >= 0);
            }
            else
            {
                break;
            }
        }

        //FIX:Bitstream Filter
#if USE_H264BSF
        av_bitstream_filter_filter(h264bsfc, in_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif
#if USE_AACBSF
        av_bitstream_filter_filter(aacbsfc, out_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif


        //Convert PTS/DTS
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        pkt.stream_index=stream_index;

    //  printf("Write 1 Packet. size:%5d\tpts:%lld\n",pkt.size,pkt.pts);
        //Write
        if (av_interleaved_write_frame(ofmt_ctx, &pkt) < 0)
        {
            printf( "Error muxing packet\n");
            break;
        }
        av_free_packet(&pkt);

    }
    //Write file trailer
    av_write_trailer(ofmt_ctx);

#if USE_H264BSF
    av_bitstream_filter_close(h264bsfc);
#endif
#if USE_AACBSF
    av_bitstream_filter_close(aacbsfc);
#endif

end:
    avformat_close_input(&ifmt_ctx_v);
    avformat_close_input(&ifmt_ctx_a);
    /* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
    {
        avio_close(ofmt_ctx->pb);
    }

    avformat_free_context(ofmt_ctx);
    if (ret < 0 && ret != AVERROR_EOF)
    {
        return -1;
    }

    return 0;
}

int ffmpeg_ConvertAudioFile(const char *szAudioFile, const char *szOutFile)
{
    AVOutputFormat *ofmt = NULL;
    AVFormatContext *ifmt_ctx_a = NULL;
    AVFormatContext *ofmt_ctx = NULL;
    AVPacket pkt;
    int ret;
    unsigned int i;
    int audioindex_a=-1,audioindex_out=-1;
    int frame_index=0;
    int64_t cur_pts_a=0;
    const char *in_filename_a = szAudioFile; //"huoyuanjia.mp3";
    const char *out_filename = szOutFile;

    //av_register_all();
    if ((ret = avformat_open_input(&ifmt_ctx_a, in_filename_a, 0, 0)) < 0)
    {
        printf( "Could not open input file.");
        goto end;
    }

    if ((ret = avformat_find_stream_info(ifmt_ctx_a, 0)) < 0)
    {
        printf( "Failed to retrieve input stream information");
        goto end;
    }

    av_dump_format(ifmt_ctx_a, 0, in_filename_a, 0);

    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
    if (!ofmt_ctx)
    {
        printf( "Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        goto end;
    }
    ofmt = ofmt_ctx->oformat;

    for (i = 0; i < ifmt_ctx_a->nb_streams; i++)
    {
        //Create output AVStream according to input AVStream
        if(ifmt_ctx_a->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            AVStream            *in_stream = ifmt_ctx_a->streams[i];
            AVCodecParameters   *in_codecpar = in_stream->codecpar;
            AVCodecID           codec_id = in_codecpar->codec_id;//AV_CODEC_ID_MP3;//
            AVCodec             *codec = avcodec_find_decoder(codec_id); // AV_CODEC_ID_PCM_S16LE);//
            AVStream            *out_stream = avformat_new_stream(ofmt_ctx, codec);

            //AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);

            audioindex_a=i;
            if (!out_stream)
            {
                printf( "Failed allocating output stream\n");
                ret = AVERROR_UNKNOWN;
                goto end;
            }
            audioindex_out=out_stream->index;
            //Copy the settings of AVCodecContext

            if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0)
            {
                printf( "Failed to copy context from input to output stream codec context\n");
                goto end;
            }

            avcodec_parameters_copy(out_stream->codecpar, in_codecpar);
            avcodec_parameters_from_context(out_stream->codecpar, in_stream->codec);

            out_stream->time_base = in_stream->codec->time_base;
            if (out_stream->codec->time_base.num == 0)
            {
                out_stream->codec->time_base.num = 1;
            }

            if (out_stream->codec->time_base.den == 0)
            {
                out_stream->codec->time_base.den = 8000;
            }

            if (out_stream->codecpar->bit_rate <= 0)
            {
                out_stream->codecpar->bit_rate = 12000;
            }

            out_stream->codecpar->codec_tag = 0;
            out_stream->codec->codec_tag = 0;
            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            {
                out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;//CODEC_FLAG_GLOBAL_HEADER;
            }

            break;
        }
    }

    //  printf("==========Output Information==========\n");
    av_dump_format(ofmt_ctx, 0, out_filename, 1);
    //  printf("======================================\n");
    //Open output file
    if (!(ofmt->flags & AVFMT_NOFILE))
    {
        if (avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE) < 0)
        {
            printf( "Could not open output file '%s'", out_filename);
            goto end;
        }
    }

    //Write file header
    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0)
    {
        printf( "avformat_write_header failed: %d\n", ret);
        goto end;
    }

    //FIX
#if USE_H264BSF
    AVBitStreamFilterContext* h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");
#endif
#if USE_AACBSF
    //AVBitStreamFilterContext* aacbsfc =  av_bitstream_filter_init("aac_adtstoasc");
#endif

    while (1)
    {
        AVFormatContext *ifmt_ctx;
        int stream_index=0;
        AVStream *in_stream, *out_stream;

        //Get an AVPacket
        {
            ifmt_ctx=ifmt_ctx_a;
            stream_index=audioindex_out;
            if(av_read_frame(ifmt_ctx, &pkt) >= 0)
            {
                do
                {
                    in_stream  = ifmt_ctx->streams[pkt.stream_index];
                    out_stream = ofmt_ctx->streams[stream_index];

                    if(pkt.stream_index==audioindex_a)
                    {
                        //FIX：No PTS
                        //Simple Write PTS
                        if(pkt.pts==AV_NOPTS_VALUE)
                        {
                            //Write PTS
                            AVRational time_base1=in_stream->time_base;
                            //Duration between 2 frames (us)
                            int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate);
                            //Parameters
                            pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            pkt.dts=pkt.pts;
                            pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            frame_index++;
                        }
                        cur_pts_a=pkt.pts;

                        break;
                    }
                }
                while(av_read_frame(ifmt_ctx, &pkt) >= 0);
            }
            else
            {
                break;
            }
        }

        //FIX:Bitstream Filter
#if USE_H264BSF
        av_bitstream_filter_filter(h264bsfc, in_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif
#if USE_AACBSF
        av_bitstream_filter_filter(aacbsfc, out_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif


        //Convert PTS/DTS
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        pkt.stream_index=stream_index;

        //  printf("Write 1 Packet. size:%5d\tpts:%lld\n",pkt.size,pkt.pts);
        //Write
        if (av_interleaved_write_frame(ofmt_ctx, &pkt) < 0)
        {
            printf( "Error muxing packet\n");
            break;
        }
        av_free_packet(&pkt);

    }
    //Write file trailer
    av_write_trailer(ofmt_ctx);

#if USE_H264BSF
    av_bitstream_filter_close(h264bsfc);
#endif
#if USE_AACBSF
    av_bitstream_filter_close(aacbsfc);
#endif

end:
    avformat_close_input(&ifmt_ctx_a);
    /* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
    {
        avio_close(ofmt_ctx->pb);
    }

    avformat_free_context(ofmt_ctx);
    if (ret < 0 && ret != AVERROR_EOF)
    {
        return -1;
    }
    return 0;
}

int ffmpeg_ConvertVideoFile(const char *szVideoFile, const char *szMixFile)
{
    AVOutputFormat *ofmt = NULL;
    AVFormatContext *ifmt_ctx_v = NULL;
    AVFormatContext *ofmt_ctx = NULL;
    AVPacket pkt;
    int ret, i;
    int videoindex_v=-1,videoindex_out=-1;
    int frame_index=0;
    int64_t cur_pts_v=0,cur_pts_a=0;

    const char *in_filename_v = szVideoFile; //"cuc_ieschool.h264";
    const char *out_filename = szMixFile;

    av_register_all();
    //Input
    if ((ret = avformat_open_input(&ifmt_ctx_v, in_filename_v, 0, 0)) < 0)
    {
        printf( "Could not open input file.");
        goto end;
    }

    if ((ret = avformat_find_stream_info(ifmt_ctx_v, 0)) < 0)
    {
        printf( "Failed to retrieve input stream information");
        goto end;
    }

    av_dump_format(ifmt_ctx_v, 0, in_filename_v, 0);
    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
    if (!ofmt_ctx)
    {
        printf( "Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        goto end;
    }
    ofmt = ofmt_ctx->oformat;

    for (i = 0; i < ifmt_ctx_v->nb_streams; i++)
    {
        //Create output AVStream according to input AVStream
        if (ifmt_ctx_v->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            AVStream *in_stream = ifmt_ctx_v->streams[i];
            AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);

            videoindex_v=i;
            if (!out_stream)
            {
                printf( "Failed allocating output stream\n");
                ret = AVERROR_UNKNOWN;
                goto end;
            }

            videoindex_out=out_stream->index;
            //Copy the settings of AVCodecContext
            if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0)
            {
                printf( "Failed to copy context from input to output stream codec context\n");
                goto end;
            }

            avcodec_parameters_from_context(out_stream->codecpar, in_stream->codec);

            out_stream->codecpar->codec_tag = 0;
            out_stream->codec->codec_tag = 0;
            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            {
                out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;//CODEC_FLAG_GLOBAL_HEADER;
            }

            break;
        }
    }

    av_dump_format(ofmt_ctx, 0, out_filename, 1);
    if (!(ofmt->flags & AVFMT_NOFILE))
    {
        if (avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE) < 0)
        {
            printf( "Could not open output file '%s'", out_filename);
            goto end;
        }
    }

    //Write file header
    if (avformat_write_header(ofmt_ctx, NULL) < 0)
    {
        printf( "Error occurred when opening output file\n");
        goto end;
    }

    //FIX
#if USE_H264BSF
    AVBitStreamFilterContext* h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");
#endif

    while (1)
    {
        AVFormatContext *ifmt_ctx;
        int stream_index=0;
        AVStream *in_stream, *out_stream;

        //Get an AVPacket
//      if(av_compare_ts(cur_pts_v,ifmt_ctx_v->streams[videoindex_v]->time_base,cur_pts_a,ifmt_ctx_a->streams[audioindex_a]->time_base) <= 0)
        if (1)
        {
            ifmt_ctx=ifmt_ctx_v;
            stream_index=videoindex_out;

            if(av_read_frame(ifmt_ctx, &pkt) >= 0)
            {
                do
                {
                    in_stream  = ifmt_ctx->streams[pkt.stream_index];
                    out_stream = ofmt_ctx->streams[stream_index];

                    if(pkt.stream_index==videoindex_v)
                    {
                        //FIX：No PTS (Example: Raw H.264)
                        //Simple Write PTS
                        if(pkt.pts==AV_NOPTS_VALUE)
                        {
                            //Write PTS
                            AVRational time_base1=in_stream->time_base;
                            //Duration between 2 frames (us)
                            int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate);
                            //Parameters
                            pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            pkt.dts=pkt.pts;
                            pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            frame_index++;
                        }

                        cur_pts_v=pkt.pts;
                        break;
                    }
                }while(av_read_frame(ifmt_ctx, &pkt) >= 0);
            }
            else
            {
                break;
            }
        }

        //FIX:Bitstream Filter
#if USE_H264BSF
        av_bitstream_filter_filter(h264bsfc, in_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif

        //Convert PTS/DTS
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        pkt.stream_index=stream_index;

        //  printf("Write 1 Packet. size:%5d\tpts:%lld\n",pkt.size,pkt.pts);
        //Write
        if (av_interleaved_write_frame(ofmt_ctx, &pkt) < 0)
        {
            printf( "Error muxing packet\n");
            break;
        }
        av_free_packet(&pkt);

    }
    //Write file trailer
    av_write_trailer(ofmt_ctx);

#if USE_H264BSF
    av_bitstream_filter_close(h264bsfc);
#endif

end:
    avformat_close_input(&ifmt_ctx_v);
    /* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
    {
        avio_close(ofmt_ctx->pb);
    }

    avformat_free_context(ofmt_ctx);
    if (ret < 0 && ret != AVERROR_EOF)
    {
        return -1;
    }
    return 0;
}

void test_mix_file()
{
    char    *szAudioFile = (char*)"e:\\rec\\test.amr";
    char    *szVideoFile = (char*)"e:\\rec\\test.h264";
    char    *szVideoFile1 = (char*)"e:\\rec\\test1.mkv";
    char    *szMixFile = (char*)"e:\\rec\\test.mp4";
    unsigned long   ulTime = gos_get_uptime_1ms();

    //int iLen = ffmpeg_GetMediaFileLength("f:\\test2.mp3");//szMixFile);
    //printf("file size = %d\n", iLen);

//  ffmpeg_ConvertAudioFile(szAudioFile, "e:\\rec\\test.mp3");
//  ffmpeg_ConvertVideoFile(szVideoFile, szVideoFile1);

    ffmpeg_MixFile(szAudioFile, szVideoFile1, szMixFile);

    printf("time1 = %u\n", gos_get_uptime_1ms()-ulTime);
    ulTime = gos_get_uptime_1ms();

//  ffmpeg_ConvertVideoFile(szVideoFile, "e:\\rec\\test1.mp4");

    printf("time2 = %u\n", gos_get_uptime_1ms()-ulTime);

    getchar();
}


#if 0

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

extern "C"
{
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
};

/* 5 seconds stream duration */
#define STREAM_DURATION 200.0
#define STREAM_FRAME_RATE 25 /* 25 images/s */
#define STREAM_NB_FRAMES ((int)(STREAM_DURATION * STREAM_FRAME_RATE))
#define STREAM_PIX_FMT AV_PIX_FMT_YUV420P /* default pix_fmt */

static AVFormatContext *input_fmt_ctx = NULL;
static int sws_flags = SWS_BICUBIC;
static AVPacket input_pkt;
static int video_stream_idx = -1;
static uint8_t *video_dst_data[4] = {NULL};
static int video_dst_linesize[4];
static AVStream *input_video_stream;
static AVFrame *input_frame;
static AVCodecContext *video_dec_ctx = NULL;
static int video_dst_bufsize;


static int open_codec_context(int *stream_idx, AVFormatContext *fmt_ctx, enum AVMediaType type)
{
    int ret;
    AVStream *st;
    AVCodecContext *dec_ctx = NULL;
    AVCodec *dec = NULL;

    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0) {
        return ret;
    } else {
        *stream_idx = ret;
        st = fmt_ctx->streams[*stream_idx];

        /* find decoder for the stream */
        dec_ctx = st->codec;
        dec = avcodec_find_decoder(dec_ctx->codec_id);
        if (!dec) {
            fprintf(stderr, "Failed to find %s codec\n",
                av_get_media_type_string(type));
            return ret;
        }

        if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
            fprintf(stderr, "Failed to open %s codec\n",
                av_get_media_type_string(type));
            return ret;
        }
    }

    return 0;
}

/* Add an output stream. */
static AVStream *add_stream(AVFormatContext *oc, AVCodec **codec,
    enum AVCodecID codec_id)
{
    AVCodecContext *c;
    AVStream *st;

    /* find the encoder */
    *codec = avcodec_find_encoder(codec_id);
    if (!(*codec)) {
        fprintf(stderr, "Could not find encoder for '%s'\n",
            avcodec_get_name(codec_id));
        exit(1);
    }

    st = avformat_new_stream(oc, *codec);
    if (!st) {
        fprintf(stderr, "Could not allocate stream\n");
        exit(1);
    }
    st->id = oc->nb_streams-1;
    c = st->codec;

    switch ((*codec)->type) {
    case AVMEDIA_TYPE_AUDIO:
        c->sample_fmt = AV_SAMPLE_FMT_FLTP;
        c->bit_rate = 64000;
        c->sample_rate = 44100;
        c->channels = 2;
        break;

    case AVMEDIA_TYPE_VIDEO:
        c->codec_id = codec_id;

        c->bit_rate = 400000;
        c->time_base.den = STREAM_FRAME_RATE;
        c->time_base.num = 1;
        c->gop_size = 12;
        c->pix_fmt = STREAM_PIX_FMT;
        break;

    default:
        break;
    }

    /* Some formats want stream headers to be separate. */
    if (oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    return st;
}

/**************************************************************/
/* audio output */

static float t, tincr, tincr2;

static uint8_t **src_samples_data;
static int src_samples_linesize;
static int src_nb_samples;

static int max_dst_nb_samples;
uint8_t **dst_samples_data;
int dst_samples_linesize;
int dst_samples_size;

struct SwrContext *swr_ctx = NULL;

#undef av_err2str

char* av_err2str(int err)
{
    static char acText[32];

    sprintf(acText, "%d", err);

    return acText;
}

static void open_audio(AVFormatContext *oc, AVCodec *codec, AVStream *st)
{
    AVCodecContext *c;
    int ret;

    c = st->codec;

    /* open it */
    ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        fprintf(stderr, "Could not open audio codec: %s\n", av_err2str(ret));
        exit(1);
    }

    /* init signal generator */
    t = 0;
    tincr = 2 * M_PI * 110.0 / c->sample_rate;
    /* increment frequency by 110 Hz per second */
    tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;

    src_nb_samples = c->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE ?
        10000 : c->frame_size;

    ret = av_samples_alloc_array_and_samples(&src_samples_data, &src_samples_linesize, c->channels,
        src_nb_samples, c->sample_fmt, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate source samples\n");
        exit(1);
    }

    /* create resampler context */
    if (c->sample_fmt != AV_SAMPLE_FMT_S16) {
        swr_ctx = swr_alloc();
        if (!swr_ctx) {
            fprintf(stderr, "Could not allocate resampler context\n");
            exit(1);
        }

        /* set options */
        av_opt_set_int (swr_ctx, "in_channel_count", c->channels, 0);
        av_opt_set_int (swr_ctx, "in_sample_rate", c->sample_rate, 0);
        av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
        av_opt_set_int (swr_ctx, "out_channel_count", c->channels, 0);
        av_opt_set_int (swr_ctx, "out_sample_rate", c->sample_rate, 0);
        av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", c->sample_fmt, 0);

        /* initialize the resampling context */
        if ((ret = swr_init(swr_ctx)) < 0) {
            fprintf(stderr, "Failed to initialize the resampling context\n");
            exit(1);
        }
    }

    /* compute the number of converted samples: buffering is avoided
    * ensuring that the output buffer will contain at least all the
    * converted input samples */
    max_dst_nb_samples = src_nb_samples;
    ret = av_samples_alloc_array_and_samples(&dst_samples_data, &dst_samples_linesize, c->channels,
        max_dst_nb_samples, c->sample_fmt, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate destination samples\n");
        exit(1);
    }
    dst_samples_size = av_samples_get_buffer_size(NULL, c->channels, max_dst_nb_samples,
        c->sample_fmt, 0);
}

/* Prepare a 16 bit dummy audio frame of 'frame_size' samples and
* 'nb_channels' channels. */
static void get_audio_frame(int16_t *samples, int frame_size, int nb_channels)
{
    int j, i, v;
    int16_t *q;

    q = samples;
    for (j = 0; j < frame_size; j++) {
        v = (int)(sin(t) * 10000);
        for (i = 0; i < nb_channels; i++)
            *q++ = v;
        t += tincr;
        tincr += tincr2;
    }
}

static void write_audio_frame(AVFormatContext *oc, AVStream *st)
{
    AVCodecContext *c;
    AVPacket pkt = { 0 }; // data and size must be 0;
    AVFrame *frame = av_fram_alloc();
    int got_packet, ret, dst_nb_samples;

    av_init_packet(&pkt);
    c = st->codec;

    get_audio_frame((int16_t *)src_samples_data[0], src_nb_samples, c->channels);

    /* convert samples from native format to destination codec format, using the resampler */
    if (swr_ctx) {
        /* compute destination number of samples */
        dst_nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx, c->sample_rate) + src_nb_samples,
            c->sample_rate, c->sample_rate, AV_ROUND_UP);
        if (dst_nb_samples > max_dst_nb_samples) {
            av_free(dst_samples_data[0]);
            ret = av_samples_alloc(dst_samples_data, &dst_samples_linesize, c->channels,
                dst_nb_samples, c->sample_fmt, 0);
            if (ret < 0)
                exit(1);
            max_dst_nb_samples = dst_nb_samples;
            dst_samples_size = av_samples_get_buffer_size(NULL, c->channels, dst_nb_samples,
                c->sample_fmt, 0);
        }

        /* convert to destination format */
        ret = swr_convert(swr_ctx,
            dst_samples_data, dst_nb_samples,
            (const uint8_t **)src_samples_data, src_nb_samples);
        if (ret < 0) {
            fprintf(stderr, "Error while converting\n");
            exit(1);
        }
    } else {
        dst_samples_data[0] = src_samples_data[0];
        dst_nb_samples = src_nb_samples;
    }

    frame->nb_samples = dst_nb_samples;
    avcodec_fill_audio_frame(frame, c->channels, c->sample_fmt,
        dst_samples_data[0], dst_samples_size, 0);

    ret = avcodec_encode_audio2(c, &pkt, frame, &got_packet);
    if (ret < 0) {
        fprintf(stderr, "Error encoding audio frame: %s\n", av_err2str(ret));
        exit(1);
    }

    if (!got_packet)
        return;

    pkt.stream_index = st->index;

    /* Write the compressed frame to the media file. */
    ret = av_interleaved_write_frame(oc, &pkt);
    if (ret != 0) {
        fprintf(stderr, "Error while writing audio frame: %s\n",
            av_err2str(ret));
        exit(1);
    }
    avcodec_free_frame(&frame);
}

static void close_audio(AVFormatContext *oc, AVStream *st)
{
    avcodec_close(st->codec);
    av_free(src_samples_data[0]);
    av_free(dst_samples_data[0]);
}

/***********************bbs.ChinaFFmpeg.com****孙悟空***********************/
/* video output */

static AVFrame *frame;
static AVPicture src_picture, dst_picture;
static int frame_count;

static void open_video(AVFormatContext *oc, AVCodec *codec, AVStream *st)
{
    int ret;
    AVCodecContext *c = st->codec;

    /* open the codec */
    ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        fprintf(stderr, "Could not open video codec: %s\n", av_err2str(ret));
        exit(1);
    }

    /* allocate and init a re-usable frame */
    frame = av_fram_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    /* Allocate the encoded raw picture. */
    ret = avpicture_alloc(&dst_picture, c->pix_fmt, c->width, c->height);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate picture: %s\n", av_err2str(ret));
        exit(1);
    }

    /* If the output format is not YUV420P, then a temporary YUV420P
    * picture is needed too. It is then converted to the required
    * output format. */
    if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
        ret = avpicture_alloc(&src_picture, AV_PIX_FMT_YUV420P, c->width, c->height);
        if (ret < 0) {
            fprintf(stderr, "Could not allocate temporary picture: %s\n",
                av_err2str(ret));
            exit(1);
        }
    }

    /* copy data and linesize picture pointers to frame */
    *((AVPicture *)frame) = dst_picture;
}

static void write_video_frame(AVFormatContext *oc, AVStream *st)
{
    int ret;
    static struct SwsContext *sws_ctx;
    AVCodecContext *c = st->codec;

    // fill_yuv_image(&dst_picture, frame_count, c->width, c->height);


    if (oc->oformat->flags & AVFMT_RAWPICTURE) {
        /* Raw video case - directly store the picture in the packet */
        AVPacket pkt;
        av_init_packet(&pkt);

        pkt.flags |= AV_PKT_FLAG_KEY;
        pkt.stream_index = st->index;
        pkt.data = dst_picture.data[0];
        pkt.size = sizeof(AVPicture);

        ret = av_interleaved_write_frame(oc, &pkt);
    } else {
        AVPacket pkt = { 0 };
        int got_packet;
        av_init_packet(&pkt);

        /* encode the image */
        ret = avcodec_encode_video2(c, &pkt, frame, &got_packet);
        if (ret < 0) {
            fprintf(stderr, "Error encoding video frame: %s\n", av_err2str(ret));
            exit(1);
        }
        /* If size is zero, it means the image was buffered. */

        if (!ret && got_packet && pkt.size) {
            pkt.stream_index = st->index;

            /* Write the compressed frame to the media file. */
            ret = av_interleaved_write_frame(oc, &pkt);
        } else {
            ret = 0;
        }
    }
    if (ret != 0) {
        fprintf(stderr, "Error while writing video frame: %s\n", av_err2str(ret));
        exit(1);
    }
    frame_count++;
}

static void close_video(AVFormatContext *oc, AVStream *st)
{
    avcodec_close(st->codec);
    av_free(src_picture.data[0]);
    av_free(dst_picture.data[0]);
    av_free(frame);
}

/**************************************************************/
/* media file output */

int ffmpeg_ConvertFile(const char *input_file, const char *filename)
{

    AVOutputFormat *fmt;
    AVFormatContext *oc;
    AVStream *audio_st, *video_st;
    AVCodec *audio_codec, *video_codec;
    double audio_time, video_time;
    int ret;
    int decoded;
    int got_frame;

    /* Initialize libavcodec, and register all codecs and formats. */
    av_register_all();

    if (avformat_open_input( &input_fmt_ctx, input_file, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open source file %s\n", input_file);
        exit(-1);
    }

    /* retrieve stream information */
    if (avformat_find_stream_info(input_fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        exit(1);
    }

    if (open_codec_context(&video_stream_idx, input_fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0) {
        input_video_stream = input_fmt_ctx->streams[video_stream_idx];
        video_dec_ctx = input_video_stream->codec;
        ret = av_image_alloc(video_dst_data, video_dst_linesize,
            video_dec_ctx->width, video_dec_ctx->height,
            video_dec_ctx->pix_fmt, 1);
        if (ret < 0) {
            fprintf(stderr, "Could not allocate raw video buffer\n");
        }
        video_dst_bufsize = ret;
    }


    input_frame = av_fram_alloc();
    if (!input_frame) {
        fprintf(stderr, "Could not allocate frame\n");
        ret = AVERROR(ENOMEM);
        exit(-1);
    }

    /* allocate the output media context */
    avformat_alloc_output_context2(&oc, NULL, NULL, filename);
    if (!oc) {
        printf("Could not deduce output format from file extension: using MPEG.\n");
        avformat_alloc_output_context2(&oc, NULL, "mpeg", filename);
    }
    if (!oc) {
        return 1;
    }
    fmt = oc->oformat;

    video_st = NULL;
    audio_st = NULL;

    fmt->video_codec = AV_CODEC_ID_H264;
    if (fmt->video_codec != AV_CODEC_ID_NONE) {
        video_st = add_stream(oc, &video_codec, AV_CODEC_ID_H264);
    }
    if (video_st->codec) {
        video_st->codec->width = video_dec_ctx->width;
        video_st->codec->height = video_dec_ctx->height;
    }
    fmt->audio_codec = AV_CODEC_ID_MP3;
    if (fmt->audio_codec != AV_CODEC_ID_NONE) {
        audio_st = add_stream(oc, &audio_codec, AV_CODEC_ID_MP3);
    }

    /* Now that all the parameters are set, we can open the audio and
    * video codecs and allocate the necessary encode buffers. */
    if (video_st)
        open_video(oc, video_codec, video_st);
    if (audio_st)
        open_audio(oc, audio_codec, audio_st);

    av_dump_format(oc, 0, filename, 1);


    /* open the output file, if needed */
    if (!(fmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&oc->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            fprintf(stderr, "Could not open '%s': %s\n", filename,
                av_err2str(ret));
            return 1;
        }
    }

    /* Write the stream header, if any. */
    ret = avformat_write_header(oc, NULL);
    if (ret < 0) {
        fprintf(stderr, "Error occurred when opening output file: %s\n",
            av_err2str(ret));
        return 1;
    }

    av_init_packet(&input_pkt);
    input_pkt.data = NULL;
    input_pkt.size = 0;


    if (frame)
        frame->pts = 0;
    for (;;) {
        /* Compute current audio and video time. */
        audio_time = audio_st ? audio_st->pts.val * av_q2d(audio_st->time_base) : 0.0;
        video_time = video_st ? video_st->pts.val * av_q2d(video_st->time_base) : 0.0;

        if ((!audio_st || audio_time >= STREAM_DURATION) &&
            (!video_st || video_time >= STREAM_DURATION))
            break;

        /* write interleaved audio and video frames */
        if (!video_st || (video_st && audio_st && audio_time < video_time)) {
            write_audio_frame(oc, audio_st);
        } else {
            av_read_frame(input_fmt_ctx, &input_pkt);

            decoded = input_pkt.size;
            if (input_pkt.stream_index == video_stream_idx) {
                /* decode video frame */
                ret = avcodec_decode_video2(video_dec_ctx, input_frame, &got_frame, &input_pkt);
                if (ret < 0) {
                    fprintf(stderr, "Error decoding video frame\n");
                    return ret;
                }

                if (got_frame) {
                    av_image_copy(&dst_picture, video_dst_linesize,
                        (const uint8_t **)(input_frame->data), input_frame->linesize,
                        video_dec_ctx->pix_fmt, video_dec_ctx->width, video_dec_ctx->height);
                }
            }

            write_video_frame(oc, video_st);
            frame->pts += av_rescale_q(1, video_st->codec->time_base, video_st->time_base);
        }
    }

    av_write_trailer(oc);

    /* Close each codec. */
    if (video_st)
        close_video(oc, video_st);
    if (audio_st)
        close_audio(oc, audio_st);

    if (!(fmt->flags & AVFMT_NOFILE))
        avio_close(oc->pb);

    avformat_free_context(oc);
    av_free(input_frame);
    av_free(video_dst_data[0]);
    avcodec_close(video_dec_ctx);
    return 0;
}

#endif


#if 0

static AVFormatContext*  m_pFormatCtx    = nullptr;      // 媒体格式上下文
static bool              m_bGlobalHeader = true;         // 音视频编解码器是否需要标记 AV_CODEC_FLAG_GLOBAL_HEADER
static AVStream*         m_pVideoStream  = nullptr;      // 视频流信息
static AVStream*         m_pAudioStream  = nullptr;      // 音频流信息


/**
* @brief 打开音视频混流器
* @param
* @return 返回错误码， 0 表示正常; < 0 表示错误码
*
*/
int32_t MuxerOpen(
    const char*           pszFilePath,        // 要保存的媒体文件,通常是.mp4文件
    const AVCodecContext* pVideoEncCtx,       // 视频编码器上下文
    const AVCodecContext* pAudioEncCtx  )     // 音频编码器上下文
{
    int res = 0;

    // 创建输出流格式上下文
    res = avformat_alloc_output_context2(&m_pFormatCtx, nullptr, nullptr, pszFilePath);
    if (nullptr == m_pFormatCtx || res < 0)
    {
        LOGE("<MuxerOpen> [ERROR] fail to avformat_alloc_output_context2()\n");
        return -1;
    }
    if (m_pFormatCtx->oformat->flags & AVFMT_GLOBALHEADER)
    {
        m_bGlobalHeader = true;
    }


    // 创建写入的视频流
    m_pVideoStream = avformat_new_stream(m_pFormatCtx, nullptr);
    if (nullptr == m_pVideoStream)
    {
        LOGE("<MuxerOpen> [ERROR] fail to create video stream\n");
        avformat_free_context(m_pFormatCtx);
        return -2;
    }
    res = avcodec_parameters_from_context(m_pVideoStream->codecpar, pVideoEncCtx);
    if (res < 0)
    {
        LOGE("<MuxerOpen> [ERROR] fail to video avcodec_parameters_from_context(), res=%d\n", res);
        avformat_free_context(m_pFormatCtx);
        return -2;
    }
    m_pVideoStream->time_base = pVideoEncCtx->time_base;


    // 创建写入的音频流
    m_pAudioStream = avformat_new_stream(m_pFormatCtx, nullptr);
    if (nullptr == m_pAudioStream)
    {
        LOGE("<MuxerOpen> [ERROR] fail to create video stream\n");
        avformat_free_context(m_pFormatCtx);
        return -2;
    }
    res = avcodec_parameters_from_context(m_pAudioStream->codecpar, pAudioEncCtx);
    if (res < 0)
    {
        LOGE("<MuxerOpen> [ERROR] fail to audio avcodec_parameters_from_context(), res=%d\n", res);
        avformat_free_context(m_pFormatCtx);
        return -2;
    }
    m_pAudioStream->time_base = pVideoEncCtx->time_base;


    // 打开文件IO上下文
    res = avio_open(&m_pFormatCtx->pb, pszFilePath, AVIO_FLAG_WRITE);
    if (res < 0)
    {
        LOGE("<MuxerOpen> [ERROR] fail to avio_open(), res=%d\n", res);
        avformat_free_context(m_pFormatCtx);
        return -2;
    }

    //
    // 写入文件头信息
    //
    res = avformat_write_header(m_pFormatCtx, nullptr);
    if (res < 0)
    {
        LOGE("<MuxerOpen> [ERROR] fail to FF_avformat_write_header(), res=%d\n", res);
        avformat_free_context(m_pFormatCtx);
        return -3;
    }

    return 0;
}


/**
* @brief  关闭音视频混流器
* @param  无
* @return 无
*
*/
void MuxerClose()
{
    // 写入尾信息
    if (m_pFormatCtx != nullptr)
    {
        av_write_trailer(m_pFormatCtx);
    }

    // 先关IO上下文
    if (m_pFormatCtx->pb != nullptr)
    {
        avio_closep(&m_pFormatCtx->pb);
        m_pFormatCtx->pb = nullptr;
    }

    // 再释放媒体格式上下文
    if (m_pFormatCtx != nullptr)
    {
        avformat_free_context(m_pFormatCtx);
        m_pFormatCtx = nullptr;
    }

    // 流文件直接在 avformat_free_context()内部已经销毁了
    m_pVideoStream = nullptr;
    m_pAudioStream = nullptr;

}


/**
* @brief  写入编码后的音频或者视频数据包
* @param  无
* @return 无
*
*/
int32_t MuxerWrite(bool bVideoPkt, AVPacket* pInPacket)
{
    // 设置写入数据包的流索引
    if (bVideoPkt)
    {
        pInPacket->stream_index = m_pVideoStream->index;
    }
    else
    {
        pInPacket->stream_index = m_pAudioStream->index;
    }

    // 写入媒体文件
    int res = av_interleaved_write_frame(m_pFormatCtx, pInPacket);
    return res;
}
#endif
