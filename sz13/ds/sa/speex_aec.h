//------------------------------Í·ÎÄ¼þ---------------------------------//
 
#ifndef SPEEX_EC_H
#define SPEEX_EC_H
 
#include <stdio.h>
#include <stdlib.h>
#include <speex/speex_echo.h>
#include <speex/speex_preprocess.h>
 
class CSpeexEC
{
public:
	CSpeexEC();
	~CSpeexEC();
	void Init(int frame_size=160, int filter_length=1280, int sampling_rate=8000); 
	void DoAEC(short *mic, short *ref, short *out);
 
protected:
	void Reset();
 
private:
	bool      m_bHasInit;
	SpeexEchoState*   m_pState;
	SpeexPreprocessState* m_pPreprocessorState;
	int      m_nFrameSize;
	int      m_nFilterLen;
	int      m_nSampleRate;
	spx_int32_t*      m_pfNoise;
};
 
#endif
 
 
//-----------------------------AEC----------------------------------//
 