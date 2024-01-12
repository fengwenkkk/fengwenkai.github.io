#if 0
#include "speex_aec.h"
 
CSpeexEC::CSpeexEC()
{
	m_bHasInit   = false;
	m_pState   = NULL;
	m_pPreprocessorState  = NULL;
	m_nFrameSize   = 160;
	m_nFilterLen   = 160*8;
	m_nSampleRate  = 8000;
	m_pfNoise   = NULL;
}
 
CSpeexEC::~CSpeexEC()
{
	Reset();
}
 
void CSpeexEC::Init(int frame_size, int filter_length, int sampling_rate)
{
	Reset(); 
 
	if (frame_size<=0 || filter_length<=0 || sampling_rate<=0)
	{
		m_nFrameSize  = 160;
		m_nFilterLen  = 160*8;
		m_nSampleRate = 8000;
	}
	else
	{
		m_nFrameSize  = frame_size;
		m_nFilterLen  = filter_length;
		m_nSampleRate = sampling_rate;
	}
 
	m_pState = speex_echo_state_init(m_nFrameSize, m_nFilterLen);
	m_pPreprocessorState = speex_preprocess_state_init(m_nFrameSize, m_nSampleRate);
	m_pfNoise = new spx_int32_t[m_nFrameSize+1];
	m_bHasInit = true;
}
 
void CSpeexEC::Reset()
{
	if (m_pState != NULL)
	{
		speex_echo_state_destroy(m_pState);
		m_pState = NULL;
	}
	if (m_pPreprocessorState != NULL)
	{
		speex_preprocess_state_destroy(m_pPreprocessorState);
		m_pPreprocessorState = NULL;
	}
	if (m_pfNoise != NULL)
	{
		delete []m_pfNoise;
		m_pfNoise = NULL;
	}
	m_bHasInit = false;
}
 
 
void CSpeexEC::DoAEC(short* mic, short* ref, short* out)
{
	if (!m_bHasInit)
		return;
 
    /*1.1.9版本中所使用的函数*/
	//speex_echo_cancel(m_pState, mic, ref, out, m_pfNoise);
	//speex_preprocess(m_pPreprocessorState, (__int16 *)out, m_pfNoise);
	/*1.2beta3-win32版本中使用的函数，从参数可以看出最新版本没有参数m_pfNoise,所以CSpeex中可以删除数据成员m_pfNoise*/
	/*本文依然保留，是为了测试两个版本的差别，从结果来看，至少人耳似乎听不出有多大差别*/
	speex_echo_cancellation(m_pState, mic, ref, out);
	speex_preprocess_run(m_pPreprocessorState, (__int16 *)out);
 
}
 
//----------------------------Demo-----------------------------------//
 
#include "speex_aec.h"
#include <stdio.h>
#include <speex/speex_echo.h>
 
#define NN 160

void test_main()
{
	FILE *ref_fd;
	FILE *mic_fd;
	FILE *out_fd;
	short ref[NN];
	short mic[NN];
	short out[NN];
	
	ref_fd = fopen("FarEnd.pcm", "r+b");
	mic_fd = fopen("NearEnd.pcm", "r+b");
	out_fd = fopen("out.pcm", "w+b");
 
 
	CSpeexEC ec;
	ec.Init();
 
	while (fread(mic, 1, NN*2, mic_fd))
	{
		fread(ref, 1, NN*2, ref_fd);  
		ec.DoAEC(mic, ref, out);
		fwrite(out, 1, NN*2, out_fd);
	}
 
	fclose(ref_fd);
	fclose(mic_fd);
	fclose(out_fd);
}

#endif

#include "speex/speex_echo.h"
#include "speex/speex_preprocess.h"

#include "g_include.h"

#pragma comment(lib, "D:\\code_sz\\lib\\libspeex.lib")
#pragma comment(lib, "D:\\code_sz\\lib\\libspeexdsp.lib")


/*
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> */


#define NN      128
#define TAIL    (NN*8)

int test_scho_main(char *szEchoFile, char *szRefFile, char *szOutFile)
{
   FILE *echo_fd, *ref_fd, *e_fd;
   short echo_buf[NN], ref_buf[NN], e_buf[NN];
   SpeexEchoState *st;
   SpeexPreprocessState *den;
   int sampleRate = 16000; 

   echo_fd = fopen(szEchoFile, "rb");
   ref_fd  = fopen(szRefFile, "rb");
   e_fd    = fopen(szOutFile, "wb");

   st = speex_echo_state_init(NN, TAIL);
   den = speex_preprocess_state_init(NN, sampleRate);
   speex_echo_ctl(st, SPEEX_ECHO_SET_SAMPLING_RATE, &sampleRate);
   speex_preprocess_ctl(den, SPEEX_PREPROCESS_SET_ECHO_STATE, st);

   while (!feof(ref_fd) && !feof(echo_fd))
   {   
      fread(ref_buf, sizeof(short), NN, ref_fd);
      fread(echo_buf, sizeof(short), NN, echo_fd);
      speex_echo_cancellation(st, ref_buf, echo_buf, e_buf);
      speex_preprocess_run(den, e_buf);
      fwrite(e_buf, sizeof(short), NN, e_fd);
   }   
   speex_echo_state_destroy(st);
   speex_preprocess_state_destroy(den);

   fclose(e_fd);
   fclose(echo_fd);
   fclose(ref_fd);
   return 0;
}