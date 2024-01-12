/**
* ��򵥵�SDL2������Ƶ�����ӣ�SDL2����PCM��
* Simplest Audio Play SDL2 (SDL2 play PCM)
*
* ������ʹ��SDL2����PCM��Ƶ�������ݡ�SDLʵ�����ǶԵײ��ͼ
* API��Direct3D��OpenGL���ķ�װ��ʹ���������Լ���ֱ�ӵ��õײ�
* API��
*
* �������ò�������:
*
* [��ʼ��]
* SDL_Init(): ��ʼ��SDL��
* SDL_OpenAudio(): ���ݲ������洢��SDL_AudioSpec������Ƶ�豸��
* SDL_PauseAudio(): ������Ƶ���ݡ�
*
* [ѭ����������]
* SDL_Delay(): ��ʱ�ȴ�������ɡ�
*
* This software plays PCM raw audio data using SDL2.
* SDL is a wrapper of low-level API (DirectSound).
* Use SDL is much easier than directly call these low-level API.
*
* The process is shown as follows:
*
* [Init]
* SDL_Init(): Init SDL.
* SDL_OpenAudio(): Opens the audio device with the desired
*                  parameters (In SDL_AudioSpec).
* SDL_PauseAudio(): Play Audio.
*
* [Loop to play data]
* SDL_Delay(): Wait for completetion of playback.
*/

#include <stdio.h>

#define __STDC__

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include "libswresample/swresample.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_Audio.h"
};

#undef __STDC__

#include "ffmpeg_rec.h"

#include "g_include.h"

#define SAMPLE_BUF_NUM      1024

static Uint32  audio_len = 0;  //��Ƶ���ݴ�С
static Uint8   *audio_pos = NULL; //ָ����Ƶ���ݵ�ָ��


extern int read_pcm_data(unsigned char *buffer, unsigned int buf_len);


#define LOG_NULL            0
#define LOG_DETAIL          1
#define LOG_INFO            2
#define LOG_WARN            3
#define LOG_ERROR           4
#define LOG_FAIL            5
#define LOG_FATAL           6

/**�ص�����(��ϵͳ����)
*  ����������typedef void (SDLCALL * SDL_AudioCallback)
*            (void *userdata, Uint8 * stream, int len);
*  This function is called when the audio device needs more data.
*
*  userdata: An application-specific parameter saved in the SDL_AudioSpec structure(SDL_AudioSpec�ṹ�е��û��Զ������ݣ�һ������¿��Բ���)
*  stream:   A pointer to the audio data buffer.(��ָ��ָ����Ҫ������Ƶ������)
*  len:      The length of that buffer in bytes.(��Ƶ�������Ĵ�С,���ֽ�Ϊ��λ)
*
*  Once the callback returns, the buffer will no longer be valid.
*  Stereo samples are stored in a LRLRLR ordering.
*
*  You can choose to avoid callbacks and use SDL_QueueAudio() instead, if
*  you like. Just open your audio device with a NULL callback.
*/

static SDL_AudioFormat  g_DeviceFormat = AUDIO_S16SYS;

void fill_audio(void *userdata, Uint8 *stream, int len)
{
    //SDL2�б�������ʹ��SDL_memset()��stream�е���������Ϊ0
    SDL_memset(stream, 0, len);

    if (audio_len == 0)     /*  Only  play  if  we  have  data  left  */
    {
        return;
    }

//  len = (len > audio_len ? audio_len : len);  /*  Mix  as  much  data  as  possible  */
    if (len > (int)audio_len)
    {
        len = (int)audio_len;
    }

    /**
    *  ����������extern DECLSPEC void SDLCALL
    *  SDL_MixAudio(Uint8 * dst, const Uint8 * src, Uint32 len, int volume);
    *  This takes two audio buffers of the playing audio format and mixes
    *  them, performing addition, volume adjustment, and overflow clipping.
    *  The volume ranges from 0 - 128, and should be set to ::SDL_MIX_MAXVOLUME
    *  for full audio volume.  Note this does not change hardware volume.
    *  This is provided for convenience -- you can mix your own audio data.
    *
    *  #define SDL_MIX_MAXVOLUME 128
    */

    SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
    audio_pos += len;
    audio_len -= len;
}

void fill_audio_device(void *userdata, Uint8 *stream, int len)
{
    //SDL2�б�������ʹ��SDL_memset()��stream�е���������Ϊ0
    SDL_memset(stream, 0, len);

    if (audio_len == 0)     /*  Only  play  if  we  have  data  left  */
    {
        return;
    }

//  len = (len > audio_len ? audio_len : len);  /*  Mix  as  much  data  as  possible  */
    if (len > (int)audio_len)
    {
        len = (int)audio_len;
    }

    /**
    *  ����������extern DECLSPEC void SDLCALL
    *  SDL_MixAudio(Uint8 * dst, const Uint8 * src, Uint32 len, int volume);
    *  This takes two audio buffers of the playing audio format and mixes
    *  them, performing addition, volume adjustment, and overflow clipping.
    *  The volume ranges from 0 - 128, and should be set to ::SDL_MIX_MAXVOLUME
    *  for full audio volume.  Note this does not change hardware volume.
    *  This is provided for convenience -- you can mix your own audio data.
    *
    *  #define SDL_MIX_MAXVOLUME 128
    */

    SDL_MixAudioFormat(stream, audio_pos, g_DeviceFormat, len, SDL_MIX_MAXVOLUME);
    audio_pos += len;
    audio_len -= len;
}

int SDLPlayPcm(UINT32 ulSampleRate, UINT32 ulChanNum,
               UINT32 ulBytesPerSample, BOOL *pbRunning, UINT32 *pulPlayStatus)
{
    unsigned char pcm_buffer[SAMPLE_BUF_NUM*4]; // ���2�ֽڲ�����˫����
    int pcm_buffer_size = SAMPLE_BUF_NUM*ulChanNum*ulBytesPerSample; //4096; //ÿ�ζ�ȡ4096�ֽڵ����ݣ�ͬʱҲ����Ƶ֡��С
//    int data_count = 0;

    *pulPlayStatus = SDL_PLAY_START;

    //��ʼ��SDL
    if (SDLInit() < 0)
    {
        *pulPlayStatus = SDL_PLAY_ERROR;
        GosLog(LOG_ERROR, "Could not initialize SDL: %s", SDL_GetError());
        return -1;
    }

    //SDL_AudioSpec��ʼ��
    SDL_AudioSpec wanted_spec;

    wanted_spec.freq = ulSampleRate;            //��Ƶ���ݵĲ����ʣ����õ���48000,44100�ȣ�
    wanted_spec.format = AUDIO_S16SYS;          //��Ƶ���ݵĸ�ʽ
    wanted_spec.channels = ulChanNum;           //�����������絥����ȡֵΪ1��������ȡֵΪ2��
    wanted_spec.silence = 0;                    //���þ�����ֵ
    wanted_spec.samples = SAMPLE_BUF_NUM;       //��Ƶ�������еĲ���������Ҫ�������2��n�η���
    wanted_spec.callback = fill_audio;          //�����Ƶ�������Ļص�����

    //����Ƶ
    if (SDL_OpenAudio(&wanted_spec, NULL) < 0)
    {
        *pulPlayStatus = SDL_PLAY_ERROR;
        GosLog(LOG_ERROR, "can't open audio: %s", SDL_GetError());
        return -1;
    }

    //������Ƶ����
    SDL_PauseAudio(0);

 //   *pbRunning = 1;
    while (*pbRunning)
    {
        int ret = read_pcm_data(pcm_buffer, pcm_buffer_size);

        if (ret <= 0)
        {
            SDL_Delay(1);
            continue;
        }

    //  printf("Now Playing %10d Bytes data.\n", data_count);
//      data_count += pcm_buffer_size;

        audio_pos = (Uint8 *)pcm_buffer;
        //Audio buffer length
        audio_len = ret;

        while (*pbRunning && audio_len > 0)//Wait until finish
        {
            SDL_Delay(1);
        }
    }

    *pulPlayStatus = SDL_PLAY_BREAK;

    SDL_CloseAudio();

//  SDL_Quit();
    return 0;
}

int SDLPlayPcm(char *szDevice, UINT32 ulSampleRate, UINT32 ulChanNum,
               UINT32 ulBytesPerSample, BOOL *pbRunning, UINT32 *pulPlayStatus)
{
    if (!szDevice || *szDevice == '\0')
    {
        return SDLPlayPcm(ulSampleRate, ulChanNum, ulBytesPerSample, pbRunning, pulPlayStatus);
    }

    unsigned char pcm_buffer[SAMPLE_BUF_NUM*4]; // ���2�ֽڲ�����˫����
    int pcm_buffer_size = SAMPLE_BUF_NUM*ulChanNum*ulBytesPerSample; //4096; //ÿ�ζ�ȡ4096�ֽڵ����ݣ�ͬʱҲ����Ƶ֡��С
    int device_id = -1;

    *pulPlayStatus = SDL_PLAY_START;

    //��ʼ��SDL
    if (SDLInit() < 0)
    {
        *pulPlayStatus = SDL_PLAY_ERROR;
        GosLog(LOG_ERROR, "Could not initialize SDL: %s", SDL_GetError());
        return -1;
    }

    //SDL_AudioSpec��ʼ��
    SDL_AudioSpec wanted_spec;
    SDL_AudioSpec get_spec;

    memset(&wanted_spec, 0, sizeof(wanted_spec));
    wanted_spec.freq = ulSampleRate;            //��Ƶ���ݵĲ����ʣ����õ���48000,44100�ȣ�
    wanted_spec.format = AUDIO_S16SYS;          //��Ƶ���ݵĸ�ʽ
    wanted_spec.channels = ulChanNum;           //�����������絥����ȡֵΪ1��������ȡֵΪ2��
    wanted_spec.silence = 0;                    //���þ�����ֵ
    wanted_spec.samples = SAMPLE_BUF_NUM;       //��Ƶ�������еĲ���������Ҫ�������2��n�η���
    wanted_spec.callback = fill_audio_device;   //�����Ƶ�������Ļص�����

    //����Ƶ
    device_id = SDL_OpenAudioDevice(szDevice, 0, &wanted_spec, &get_spec, 0);
    if (device_id < 2)
    {
        *pulPlayStatus = SDL_PLAY_ERROR;
        GosLog(LOG_ERROR, "can't open audio: %s", SDL_GetError());
        return -1;
    }

    //������Ƶ����
    SDL_PauseAudioDevice(device_id, 0);

 //   *pbRunning = 1;
    while (*pbRunning)
    {
        int ret = read_pcm_data(pcm_buffer, pcm_buffer_size);

        if (ret <= 0)
        {
            SDL_Delay(1);
            continue;
        }


    //  printf("Now Playing %10d Bytes data.\n", data_count);
//      data_count += pcm_buffer_size;

        audio_pos = (Uint8 *)pcm_buffer;
        //Audio buffer length
        audio_len = ret;

        while (*pbRunning && audio_len > 0)//Wait until finish
        {
            SDL_Delay(1);
        }
    }

    *pulPlayStatus = SDL_PLAY_BREAK;

    SDL_CloseAudioDevice(device_id);

//  SDL_Quit();
    return 0;
}

int SDLGetAudioDeviceNum(BOOL bIsCapture)
{
    if (SDLInit() < 0)
    {
        GosLog(LOG_ERROR, "Could not initialize SDL: %s", SDL_GetError());
        return -1;
    }

    return SDL_GetNumAudioDevices(bIsCapture);
}

const char * SDLGetAudioDeviceName(BOOL bIsCapture, UINT32 iIndex)
{
    if (SDLInit() < 0)
    {
        GosLog(LOG_ERROR, "Could not initialize SDL: %s", SDL_GetError());
        return "";
    }

    return SDL_GetAudioDeviceName(iIndex, bIsCapture);
}

#if 0
int main(int argc, char* argv[])
{
    unsigned int ulSampleRate = 16000;
    unsigned int ulChanNum = 1;

    ffmpeg_play_pcm(ulSampleRate, ulChanNum);
}
#endif
