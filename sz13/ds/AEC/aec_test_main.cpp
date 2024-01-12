#include "g_include.h"

#include "AEC.h"

VOID OnMicData(UINT8 *pucData, UINT32 ulLen)
{
    static INT64        i64StartTime = 0;
    static INT64        i64LastTime = 0;
    INT64               i64Time = gos_get_uptime_1ns()/1000000;
   
    if (0 == i64StartTime)
    {
        i64StartTime = i64Time;
    }

    printf("send %d %u\n", (int)(i64Time-i64LastTime), ulLen);

    i64LastTime = i64Time;
}

UINT32 GetIPHMaxBufFrameNum()
{
    return 20;
}

static VOID OnAECCapData(UINT8 *pucData, UINT32 ulLen, UINT32 ulSampleRate, UINT32 ulBitsPerSample)
{
    UINT32          ulMaxBufFrameNum = GetIPHMaxBufFrameNum();  // 比如1000， 每秒对应16000/1000 = 16次发包
    UINT32          ulMaxBufSize = ulMaxBufFrameNum*16/8;
    static UINT8    *pucBuf = NULL;
    static UINT32   ulCurrDataLen = 0;
    INT32           iCurrSize = ulLen;
    UINT32          ulBufLen = 0;

    if (ulMaxBufFrameNum == 0)
    {
        OnMicData(pucData, ulLen);
        return;
    }

    if (!pucBuf)
    {
        pucBuf = (UINT8*)calloc(1, ulMaxBufSize);
    }

    while(1)
    {
        ulBufLen = ulMaxBufSize - ulCurrDataLen;
        if (iCurrSize < (INT32)ulBufLen)
        {
            memcpy(pucBuf+ulCurrDataLen, pucData, iCurrSize);
            ulCurrDataLen += iCurrSize;

            return;
        }

        memcpy(pucBuf+ulCurrDataLen, pucData, ulBufLen);
        OnMicData(pucBuf, ulMaxBufSize);

        ulCurrDataLen = 0;
        pucData += ulBufLen;
        iCurrSize -= ulBufLen;
    }
}

void test_AEC()
{
    gos_time_init();

    char *szCapture = "Internal Microphone (Cirrus Logic Superior High Definition Audio)";
    char *szRender = "扬声器 (Cirrus Logic Superior High Definition Audio)";
    UINT32 ulSampleRate = 16000;
    UINT32 ulBitsPerSample = 16;

    UINT8   aucData[1000];

    OnAECCapData(aucData, 65, 0,0);
    OnAECCapData(aucData, 77, 0,0);
    OnAECCapData(aucData, 18, 0,0);
    OnAECCapData(aucData, 40, 0,0);
    OnAECCapData(aucData, 31, 0,0);
    OnAECCapData(aucData, 9, 0,0);
    
        
    AECInit();//szCapture, szRender, ulSampleRate, ulBitsPerSample);

/*    AEC_DEV_T   stMic;
    AEC_DEV_T   stSpeaker;

    AECEnumDevice(&stMic, &stSpeaker);

    for (UINT32 i=0; i<stMic.ulNum; i++)
    {
        printf("%s\n", stMic.aacName[i]);
    }

    for (UINT32 i=0; i<stSpeaker.ulNum; i++)
    {
        printf("%s\n", stSpeaker.aacName[i]);
    }*/


    AECSetOnCapturedDataCB(OnAECCapData);

    AECStart("","",16000, 16);

    Sleep(10*1000);

    AECStop();

    getchar();
//    wavFile.Close();
}

#if 0

class PCMFilter
{
public:
    PCMFilter(UINT32 ulSampleBytes);
    UINT8* GetData();
    UINT32 GetDataLen();
    VOID PushData(UINT8 *pucData, UINT32 ulLen);

private:
    UINT8       *m_pucBuf;
    UINT32      m_ulBufLen;
    UINT32      m_ulLen;
    UINT32      m_ulSampleBytes;    // 采样字节数，1位或8位
    BOOL        m_bFirst;
    DOUBLE      m_dAvg;
    DOUBLE      m_dRate;
};

PCMFilter::PCMFilter(UINT32 ulSampleBytes)
{
    m_dAvg = 0;
    m_dRate = 0.95;
    m_bFirst = TRUE;

    m_ulBufLen = 1000;
    if (ulSampleBytes == 1 || 
        ulSampleBytes == 2)
    {
        m_pucBuf = calloc(ulSampleBytes, m_ulBufLen);
    }
    else
    {
        m_pucBuf = calloc();
    }

    m_ulLen = NULL;
}


UINT8* PCMFilter::GetData()
{
    return m_pucData;
}

UINT32 PCMFilter::GetDataLen()
{
    return m_ulLen;
}

VOID PCMFilter::PushData(UINT8 *pucData, UINT32 ulLen)
{
    INT32   iAvg = 0;

    for (int i=0; i<iNum; i++)
    {
        iAvg += asBuf[i];
    }

    iAvg /= iNum;

    if (bFirst)
    {
        dAvg = iAvg;
        bFirst = FALSE;
    }
    else
    {
        dAvg = dRate*dAvg + iAvg*(1-dRate);
    }
}

void remove_dc()
{
    char    *szFile = "d:/temp/iph_with_dc.wav";
    FILE    *fp = fopen(szFile, "rb");
    FILE    *fp2 = fopen("d:/temp/iph_with_dc2.wav", "wb");
    UINT8   acHdr[44];
    PCMFilter   Filter;

    fread(acHdr, 1, sizeof(acHdr), fp);
    fwrite(acHdr, 1, sizeof(acHdr), fp2);

    while(!feof(fp))
    {
        int iNum = fread(asBuf, 1, sizeof(asBuf), fp)/sizeof(short);
        int iAvg = 0;

        for (int i=0; i<iNum; i++)
        {
            iAvg += asBuf[i];
        }

        iAvg /= iNum;

        if (bFirst)
        {
            dAvg = iAvg;
            bFirst = FALSE;
        }
        else
        {
            dAvg = dRate*dAvg + iAvg*(1-dRate);
        }

        printf("%d %d\n", iAvg, (int)dAvg);

        for (int i=0; i<iNum; i++)
        {
            asBuf[i] -= dAvg;
        }

        fwrite(asBuf, 1, 2*iNum, fp2);
    }

    fclose(fp);
    fclose(fp2);

}

#endif

int main(int argc, char *argv[])
{
//    remove_dc();

#if 0
    FILE    *fp = fopen("c:\\rand.pcm", "w+b");
    static short  asTest[16000*180];

    srand(time(NULL));

    UINT32 j = 0;
    for (UINT32 i=0; i<ARRAY_SIZE(asTest); i++, j++)
    {
        if (MOD(i,2000) < 1000)
        {
            asTest[i] = -10;
        }
        else
        {
            asTest[i] = 10;
        }

        asTest[i] = rand();
      //  asTest[i] = htons(asTest[i]);
    }

    fwrite(asTest, 1, sizeof(asTest), fp);
    fclose(fp);
#endif

    test_AEC();

 	return 0;   
}