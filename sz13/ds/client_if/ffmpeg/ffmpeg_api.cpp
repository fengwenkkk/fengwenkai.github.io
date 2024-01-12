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

//int sdl_thread_exit  = 0;
//int sdl_thread_pause = 0;

extern "C" void gos_sleep_1ms(unsigned int ms);

int sfp_refresh_thread(void *pvParam)
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

//---------------------------------------------------------------------------
int SDLPlay(char *szURL, void* hWndPlayer, int bTcpMode, int *piRunning, unsigned int *piPlayStatus)
{
    AVFormatContext *pFormatCtx;
    int             videoindex;
    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;
    AVFrame *pFrame,*pFrameYUV;
    uint8_t *out_buffer;
    AVPacket *packet;
    int ret, got_picture;

    //------------SDL----------------
    int screen_w,screen_h;
    SDL_Window *screen;
    SDL_Renderer* sdlRenderer;
    SDL_Texture* sdlTexture;
    SDL_Rect sdlRect;
 // SDL_Thread *video_tid;
    SDL_Event event;

    struct SwsContext *img_convert_ctx = NULL;

    *piPlayStatus = SDL_PLAY_START;

    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();

    AVDictionary* options = NULL;

    if (bTcpMode)
    {
        av_dict_set(&options, "rtsp_transport", "tcp", 0);
    }

    if(avformat_open_input(&pFormatCtx,szURL,NULL, &options)!=0)
    {
        *piPlayStatus = SDL_PLAY_ERROR;

        printf("Couldn't open input stream.\n");
        return -1;
    }

    pFormatCtx->probesize = 10*1024;   // 100*1024
    pFormatCtx->max_analyze_duration = 2 * AV_TIME_BASE;
    pFormatCtx->flags &= AVFMT_FLAG_NOBUFFER;
    if (avformat_find_stream_info(pFormatCtx, &options) < 0)
    {
        printf("Couldn't find stream information.\n");
//      return -1;
    }

    videoindex = -1;
    for(unsigned int i=0; i<pFormatCtx->nb_streams; i++)
    {
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoindex=i;
            break;
        }
    }

    if(videoindex==-1)
    {
        *piPlayStatus = SDL_PLAY_ERROR;
        avformat_free_context(pFormatCtx);
        printf("Didn't find a video stream.\n");
        return -1;
    }

    pCodecCtx = pFormatCtx->streams[videoindex]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);

    if (pCodec==NULL)
    {
        *piPlayStatus = SDL_PLAY_ERROR;
        avformat_free_context(pFormatCtx);
        printf("Codec not found.\n");
        return -1;
    }

    if (avcodec_open2(pCodecCtx, pCodec, NULL)<0)
    {
        *piPlayStatus = SDL_PLAY_ERROR;
        avformat_free_context(pFormatCtx);
        printf("Could not open codec.\n");
        return -1;
    }

    pFrame = av_frame_alloc();
    pFrameYUV=av_frame_alloc();
    out_buffer=(uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
//  out_buffer=(uint8_t *)malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
    avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);

    av_dump_format(pFormatCtx,0,szURL,0);

    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
        pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);


    //SDL 2.0 Support for multiple windows
    screen_w = pCodecCtx->width;
    screen_h = pCodecCtx->height;
//  screen = SDL_CreateWindow("Simplest ffmpeg player's Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
//      screen_w, screen_h,SDL_WINDOW_OPENGL);
    screen = SDL_CreateWindowFrom((void*)hWndPlayer);
    if (!screen/*screen*/)
    {
        *piPlayStatus = SDL_PLAY_ERROR;

        av_frame_free(&pFrameYUV);
        av_frame_free(&pFrame);

        avcodec_close(pCodecCtx);
        avformat_close_input(&pFormatCtx);

        av_free(out_buffer);

        printf("SDL: could not create window - exiting:%s\n",SDL_GetError());
        return -1;
    }

    sdlRenderer = SDL_CreateRenderer((SDL_Window*)screen, -1, 0);
    //IYUV: Y + U + V  (3 planes)
    //YV12: Y + V + U  (3 planes)
    sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,pCodecCtx->width,pCodecCtx->height);

    sdlRect.x=0;
    sdlRect.y=0;
    sdlRect.w=screen_w;
    sdlRect.h=screen_h;

    packet=(AVPacket *)av_malloc(sizeof(AVPacket));

    SDL_THREAD_PARAM_T  stParam = {0};

    SDL_CreateThread(sfp_refresh_thread, NULL, (void*)&stParam);
    //------------SDL End------------
    //Event Loop

    while(1)
    {
        if (!(*piRunning))
        {
            stParam.sdl_thread_exit = 1;
        }
        //Wait
        SDL_WaitEvent(&event);
        if(event.type==SFM_REFRESH_EVENT)
        {
            //------------------------------
            while(1)
            {
                ret = av_read_frame(pFormatCtx, packet);
                if (ret < 0)
                {
                    break;
                }

                if (packet->stream_index == videoindex)
                {
                    break;
                }

                break;
            }

            if (ret < 0)
            {
                if (ret == AVERROR_EOF)
                {
                    break;
                }

                stParam.sdl_thread_exit = 1;

                *piPlayStatus = SDL_PLAY_BREAK;
                break;
            }
            else
            {
                if (packet->stream_index == videoindex)
                {
                    ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
                    if(ret < 0)
                    {
                        printf("Decode Error.\n");
                        break;
                    }

                    if(got_picture)
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

                        av_packet_unref(packet);
                    }
                }
                av_free_packet(packet);
            }
        }
        else if(event.type==SDL_KEYDOWN)
        {
/*          //Pause
            if(event.key.keysym.sym==SDLK_SPACE)
            {
                stParam.sdl_thread_pause = !stParam.sdl_thread_pause;
            } */
        }
        else if(event.type==SDL_QUIT)
        {
            stParam.sdl_thread_exit = 1;
        }
        else if(event.type==SFM_BREAK_EVENT)
        {
            break;
        }
    }

    if (*piPlayStatus != SDL_PLAY_BREAK)
    {
        *piPlayStatus = SDL_PLAY_END;
    }

    stParam.sdl_thread_exit = 1;

    for (int i=0; i<10; i++)
    {
        if (stParam.iThreadExited)
        {
            break;
        }

        SDL_Delay(20);
    }

    sws_freeContext(img_convert_ctx);

  //    SDL_Quit();
    //--------------
    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);

    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    SDL_DestroyTexture(sdlTexture);

//  SDL_DestroyWindow(screen);

//  SDL_Quit();
    av_free(out_buffer);

    return 0;
}

int GetMediaFileLength(char *szFile)
{
    AVFormatContext *pFormatCtx = NULL;

    av_register_all();
    avformat_network_init();

    if (avformat_open_input(&pFormatCtx, szFile, NULL,NULL) !=0 ||
        pFormatCtx == NULL )
    //if(avformat_open_input(&pFormatCtx, "http://vfx.mtime.cn/Video/2019/01/15/mp4/190115161611510728_480.mp4", NULL, NULL)!=0)
    //if(avformat_open_input(&pFormatCtx, "/home/llw/Desktop/output.mp4", NULL, NULL)!=0)
    {
        return 0;
    }

    double dTime = pFormatCtx->duration/((double)1000000);  //duration单位是us，转化为秒

    avformat_close_input(&pFormatCtx);  //释放动作

    return dTime+0.5;
}
