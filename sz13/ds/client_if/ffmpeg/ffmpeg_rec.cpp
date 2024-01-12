#include <stdio.h>

#define __STDC__

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include "libswresample/swresample.h"
#include "SDL2/SDL.h"
};

#undef __STDC__

#include "ffmpeg_rec.h"

#include "g_include.h"

int SDLInit()
{
    int iRet = SDL_WasInit(SDL_INIT_EVERYTHING);

    if (iRet == 0)
    {
        int iRet = SDL_Init(SDL_INIT_EVERYTHING);

        return iRet;
        //if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
    }

    return 0;
}

static int SDLThreadEntry(void *pvParam)
{
    SDL_THREAD_PARAM_T  *pstParam = (SDL_THREAD_PARAM_T*)pvParam;

    pstParam->iThreadExited = 0;
    pstParam->sdl_thread_exit  = 0;
    pstParam->sdl_thread_pause = 0;

    while (!pstParam->sdl_thread_exit)
    {
        if (!pstParam->sdl_thread_pause)
        {
            SDL_Event event;
            event.type = SFM_REFRESH_EVENT;
            SDL_PushEvent(&event);
        }

        SDL_Delay(30);
    }

    pstParam->sdl_thread_exit = 0;
    pstParam->sdl_thread_pause = 0;
    //Break
    SDL_Event event;
    event.type = SFM_BREAK_EVENT;
    SDL_PushEvent(&event);

    pstParam->iThreadExited = 1;

    return 0;
}

AVStream* AddVideoStream(AVFormatContext* oc,AVCodecContext** pOutCodecCtx,AVFormatContext* ic,int index,bool mark,bool open)
{
    AVCodec   *video_codec=NULL;
    AVStream  *VideoSt=NULL;

    video_codec=avcodec_find_encoder(ic->streams[index]->codecpar->codec_id);
    if(!video_codec)
    {
        return NULL;
    }

    VideoSt=avformat_new_stream(oc,video_codec);
    if (!VideoSt)
    {
        return NULL;
    }

    *pOutCodecCtx=avcodec_alloc_context3(video_codec);
    if(!(*pOutCodecCtx))
    {
        return NULL;
    }

    if(avcodec_parameters_copy(VideoSt->codecpar,ic->streams[index]->codecpar)<0)
    {
        return NULL;
    }

    if(avcodec_parameters_to_context(*pOutCodecCtx,VideoSt->codecpar)<0)
    {
        return NULL;
    }

    if(av_q2d(ic->streams[index]->avg_frame_rate)<20.0||av_q2d(ic->streams[index]->avg_frame_rate)>30.0||ic->streams[index]->avg_frame_rate.den==0)
    {
        (*pOutCodecCtx)->time_base.den=25;
    }
    else
    {
        (*pOutCodecCtx)->time_base.den  = av_q2d(ic->streams[index]->avg_frame_rate);//Ö¡ÂÊ£º 30
    }

    VideoSt->time_base.num=1;
    VideoSt->time_base.den=ic->streams[index]->time_base.den;

    if(!mark)
    {
        if(oc->oformat->flags&AVFMT_GLOBALHEADER)
            (*pOutCodecCtx)->flags|=AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    if(open)
    {
        if(avcodec_open2(*pOutCodecCtx,video_codec,NULL)<0)
            return NULL;
    }

    return VideoSt;
}

int Search_I_Frame(uint8_t* buf,int size)
{
    int i=0,j=0;

    for(i=size-1;i>=3;i--)
    {
        if(buf[i] == 0x01 && buf[i-1] == 0x00 && buf[i-2] == 0x00 && buf[i-3] == 0x00)
            break;
    }

    j=i-3;
    if(i<3)
    {
        return -1;
    }
    else
    {
        return j;
    }
}

//---------------------------------------------------------------------------
int SDLPlayAndRec(const char *szURL, const char *szFile,
                  void *pScreen, int bTcpMode, unsigned int ulOpenTimeout,
                  unsigned int ulMaxRunTime, int &bRunning, int &bPause,
                  unsigned int &ulPlayStatus, int &iVideoWidth, int &iVideoHeight)
{
    AVFormatContext *pFormatCtx = NULL;
    AVFormatContext *ofmt = NULL;
    int             videoindex;
    AVCodecContext  *pCodecCtx = NULL;
    AVCodec         *pCodec = NULL;
    AVFrame         *pFrame = NULL;
    AVFrame         *pFrameYUV;
    uint8_t         *out_buffer = NULL;
    AVPacket        *packet = NULL;
    int             ret;
    int             i;
    int             size;
    int             got_picture;
    int             sps_pps_size;
    int             video_frame_index_rtsp = 0;
    int             video_duration;
    int             pos;
    AVPacket        pktV;
    AVStream        *video_stream = NULL;
    AVOutputFormat  *fmt = NULL;
    AVCodecContext  *out_video_ctx = NULL;
    //------------SDL----------------
  //    int screen_w,screen_h;
    SDL_Window *screen = (SDL_Window*)pScreen;
    SDL_Renderer* sdlRenderer = NULL;
    SDL_Texture* sdlTexture = NULL;
//  SDL_Thread *video_tid;
    SDL_Event event;
    struct SwsContext *img_convert_ctx = NULL;
    AVDictionary* options = NULL;
    bool        save_file = false;
    SDL_THREAD_PARAM_T  stParam = {0};
    unsigned int        ulTime;
    char            acOpenTimeout[32];
    int             iSDLThreadRunning = 0;
    unsigned int    bLastPauseStatus = bPause;

    if (ulOpenTimeout == 0)
    {
        ulOpenTimeout = 1;
    }
    else if (ulOpenTimeout > 60)
    {
        ulOpenTimeout = 60;
    }

    sprintf(acOpenTimeout, "%d", 1000000*ulOpenTimeout);

    iVideoWidth = -1;
    iVideoHeight = -1;

    ulPlayStatus = SDL_PLAY_START;
    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();

    if (SDLInit() < 0)
    {
        ulPlayStatus = SDL_PLAY_ERROR;
        return -1;
    }

    if (bTcpMode)
    {
        av_dict_set(&options, "rtsp_transport", "tcp", 0);
    }

    av_dict_set(&options, "buffer_size", "1024000", 0);
    av_dict_set(&options, "stimeout", acOpenTimeout, 0);

    if (avformat_open_input(&pFormatCtx, szURL, NULL, &options)!=0)
    {
        ulPlayStatus = SDL_PLAY_ERROR;

//      printf("Couldn't open input stream");
        return -1;
    }

    pFormatCtx->probesize = 10*1024;   // 100*1024
    pFormatCtx->max_analyze_duration = 2 * AV_TIME_BASE;
    pFormatCtx->flags &= AVFMT_FLAG_NOBUFFER;
    if (avformat_find_stream_info(pFormatCtx, &options) < 0)
    {
        printf("Couldn't find stream information.\n");
        goto end;
//      return -1;
    }

    videoindex = -1;
    for(i=0; i<(int)pFormatCtx->nb_streams; i++)
    {
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoindex=i;
            break;
        }
    }

    if (videoindex == -1)
    {
        ulPlayStatus = SDL_PLAY_ERROR;

        goto end;
   /*
        avformat_free_context(pFormatCtx);
        printf("Didn't find a video stream.\n");
        return -1; */
    }

    pCodecCtx = pFormatCtx->streams[videoindex]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);

    if (pCodec==NULL)
    {
        ulPlayStatus = SDL_PLAY_ERROR;
        goto end;
    /*
        avformat_free_context(pFormatCtx);
        printf("Codec not found.\n");
        return -1;  */
    }

    if (avcodec_open2(pCodecCtx, pCodec, NULL)<0)
    {
        ulPlayStatus = SDL_PLAY_ERROR;
        goto end;

 /*     avformat_free_context(pFormatCtx);
        printf("Could not open codec.\n");
        return -1; */
    }

    pFrame = av_frame_alloc();
    pFrameYUV = av_frame_alloc();
    out_buffer=(uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
//  out_buffer=(uint8_t *)malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
    avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);

    av_dump_format(pFormatCtx,0,szURL,0);

    iVideoWidth = pCodecCtx->width;
    iVideoHeight = pCodecCtx->height;

    if (pCodecCtx->width <= 0 ||
        pCodecCtx->height <= 0)
    {
        goto end;
    }

    // output
    ret = avformat_alloc_output_context2(&ofmt,NULL,NULL,szFile);
    if (ret >= 0)
    {
        fmt = ofmt->oformat;

        video_stream = AddVideoStream(ofmt,&out_video_ctx,pFormatCtx,videoindex,false,false);
        if (!video_stream)
        {
            ulPlayStatus = SDL_PLAY_ERROR;
            goto end;
        }

        sps_pps_size = pFormatCtx->streams[videoindex]->codecpar->extradata_size;

        for (i=0; i<sps_pps_size; i++)
        {
            video_stream->codecpar->extradata[i] = pFormatCtx->streams[videoindex]->codecpar->extradata[i];
        }

        video_stream->codecpar->extradata_size = sps_pps_size;
        if (!(fmt->flags & AVFMT_NOFILE))
        {
            ret = avio_open(&ofmt->pb, szFile, AVIO_FLAG_WRITE);
            if (ret < 0)
            {
                goto next;
            }
        }

        ret = avformat_write_header(ofmt, NULL);
        if (ret < 0)
        {
            goto next;
        }

        if (out_video_ctx->time_base.den == 0)
        {
            goto next;
        }

        video_duration = video_stream->time_base.den/out_video_ctx->time_base.den;

        save_file = true;
    }

next:
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
                                     pCodecCtx->width, pCodecCtx->height,
                                     AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);


    //SDL 2.0 Support for multiple windows
    //screen_w = pCodecCtx->width;
//  screen_h = pCodecCtx->height;
//  screen = SDL_CreateWindow("Simplest ffmpeg player's Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
//      screen_w, screen_h,SDL_WINDOW_OPENGL);

/*  if (!screen)
    {
        screen = SDL_CreateWindowFrom((void*)hWndPlayer);
//      screen = SDL_CreateWindow("", 0, 0, 200,160,  SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    }
  */

    if (!screen/*screen*/)
    {
        ulPlayStatus = SDL_PLAY_ERROR;

        GosLog(LOG_ERROR, "rec_and_play SDL_CreateWindowFrom failed:%s", SDL_GetError());

        goto end;
    }

    sdlRenderer = SDL_CreateRenderer((SDL_Window*)screen, -1, 0);
    //IYUV: Y + U + V  (3 planes)
    //YV12: Y + V + U  (3 planes)
    sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,pCodecCtx->width,pCodecCtx->height);
    packet=(AVPacket *)av_malloc(sizeof(AVPacket));
    SDL_CreateThread(SDLThreadEntry, NULL, (void*)&stParam);
    iSDLThreadRunning = 1;
    //------------SDL End------------
    //Event Loop
    ulTime = gos_get_uptime();

    while(1)
    {
        if (!(bRunning) )
        {
            ulPlayStatus = SDL_PLAY_BREAK;
            break;
        }

        if (ulMaxRunTime > 0 &&
            (gos_get_uptime() - ulTime) > ulMaxRunTime)
        {
            ulPlayStatus = SDL_PLAY_BREAK;
            break;
        }

   //     GosLog(LOG_DETAIL, "rec_and_play WaitEvent");
        //Wait
        SDL_WaitEvent(&event);

        if (event.type == SDL_KEYDOWN)
        {
/*          //Pause
            if(event.key.keysym.sym==SDLK_SPACE)
            {
                stParam.sdl_thread_pause = !stParam.sdl_thread_pause;
            } */
            continue;
        }
        else if(event.type == SDL_QUIT)
        {
            ulPlayStatus = SDL_PLAY_BREAK;
            break;
        }
        else if(event.type == SFM_BREAK_EVENT)
        {
            continue;

            // todo
    /*      *piPlayStatus = SDL_PLAY_BREAK;
            break;  */
        }
        else if (event.type != SFM_REFRESH_EVENT)
        {
            continue;
        }

        //------------------------------
     //   GosLog(LOG_DETAIL, "rec_and_play read frame begin");
        while(bRunning)
        {
            if (bLastPauseStatus != bPause)
            {
                if (bPause)
                {
                    av_read_pause(pFormatCtx);
                    GosLog(LOG_INFO, "pause read frame");
                }
                else
                {
                    av_read_play(pFormatCtx);
                    GosLog(LOG_INFO, "continue read frame");
                }

                bLastPauseStatus = bPause;
            }

            if (bPause)
            {
                gos_sleep_1ms(200);
                continue;
            }

            ret = av_read_frame(pFormatCtx, packet);
            if (ret < 0)
            {
                break;
            }

            if (packet->stream_index == videoindex)
            {
                break;
            }

            gos_sleep_1ms(1);
        }

   //     GosLog(LOG_DETAIL, "rec_and_play read frame end");

        if (!bRunning)
        {
            ulPlayStatus = SDL_PLAY_BREAK;
            break;
        }

        if (ret < 0)
        {
            if (ret == AVERROR_EOF)
            {
                ulPlayStatus = SDL_PLAY_END;
            }
            else
            {
                ulPlayStatus = SDL_PLAY_BREAK;
            }

            break;
        }

        if (packet->stream_index == videoindex)
        {
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
            if (ret < 0)
            {
                //printf("Decode Error.\n");
                ulPlayStatus = SDL_PLAY_ERROR;
                break;
            }

            if (got_picture)
            {
                sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
                //SDL---------------------------
               /*   SDL_UpdateTexture(sdlTexture,
                                  NULL,
                                  pFrameYUV->data[0],
                                  pFrameYUV->linesize[0] ); */
                SDL_UpdateYUVTexture(sdlTexture,                   // sdl texture
                         NULL,                          // sdl rect
                         pFrameYUV->data[0],            // y plane
                         pFrameYUV->linesize[0],        // y pitch
                         pFrameYUV->data[1],            // u plane
                         pFrameYUV->linesize[1],        // u pitch
                         pFrameYUV->data[2],            // v plane
                         pFrameYUV->linesize[2]         // v pitch
                         );

                SDL_RenderClear( sdlRenderer );
                //SDL_RenderCopy( sdlRenderer, sdlTexture, &sdlRect, &sdlRect );
                SDL_RenderCopy( sdlRenderer, sdlTexture, NULL, NULL);
                SDL_RenderPresent( sdlRenderer );
                //SDL End-----------------------

                // rec
                pos = Search_I_Frame(packet->data, packet->size);
                if (pos < 0)
                {
                    //ret = pos;
                    av_packet_unref(packet);
                    continue;
                }

                if (save_file)
                {
                    size = packet->size-pos;
                    av_new_packet(&pktV,size);
                    memset(pktV.data,0,size);
                    memcpy(pktV.data,packet->data+pos,size);
                    pktV.flags = packet->flags;
                    pktV.pts = video_duration*video_frame_index_rtsp;
                    pktV.pts = av_rescale_q_rnd(pktV.pts, pFormatCtx->streams[videoindex]->time_base, video_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
                    pktV.dts = pktV.pts;
                    pktV.duration = av_rescale_q(packet->duration, pFormatCtx->streams[videoindex]->time_base, video_stream->time_base);
                    pktV.stream_index = videoindex;
                    pktV.flags |= AV_PKT_FLAG_KEY;
                    ret = av_interleaved_write_frame(ofmt, &pktV) ;
                    if (ret < 0)
                    {
                        ret = -1;
                        av_packet_unref(&pktV);
                        av_packet_unref(packet);

                        av_free_packet(packet);

                        continue;
                    }

                    video_frame_index_rtsp++;
                    av_packet_unref(&pktV);
                    av_packet_unref(packet);
                }
            }
        }

        av_free_packet(packet);
    }

    if (save_file)
    {
        av_write_trailer(ofmt);
    }

end:
    stParam.sdl_thread_exit = 1;

    GosLog(LOG_INFO, "rec_and_play stopping");

    while(iSDLThreadRunning) //for (int i=0; i<10; i++)
    {
        if (stParam.iThreadExited)
        {
            break;
        }

        gos_sleep_1ms(5);
    }

    GosLog(LOG_INFO, "rec_and_play stopped");

    sws_freeContext(img_convert_ctx);

  //    SDL_Quit();
    //--------------
    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);

    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    if (out_video_ctx)
    {
        avcodec_free_context(&out_video_ctx);
    }

    if (ofmt && !(fmt->flags & AVFMT_NOFILE))
    {
        avio_close(ofmt->pb);
    }

    if (ofmt)
    {
        avformat_free_context(ofmt);
    }

    if (sdlTexture)
    {
        SDL_DestroyTexture(sdlTexture);
    }


    //  SDL_FreeSurface(windowSurface);
    if (sdlRenderer)
    {
        SDL_DestroyRenderer(sdlRenderer);
    }
//      SDL_DestroyWindow(screen);

//  SDL_Quit();
    av_free(out_buffer);

    return 0;
}

void* SDLCreateWindow(void *hWndPlayer)
{
    SDLInit();
    return (void*)SDL_CreateWindowFrom(hWndPlayer);
}

void SDLFreeWindow(void *screen)
{
    if (screen)
    {
        SDL_DestroyWindow((SDL_Window *)screen);
    }
}
