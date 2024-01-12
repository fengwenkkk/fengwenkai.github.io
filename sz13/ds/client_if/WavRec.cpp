#include "g_include.h"
#include "WavRec.h"

#pragma pack(push, 1)
typedef struct
{
    UINT8           aucChunkID[4];      // 文件头标识，一般就是" RIFF" 四个字母
    UINT32          ulChunkSize;        // 整个数据文件的大小，不包括上面ID和Size本身    = 4 + ( 8 + SubChunk1Size ) + ( 8 + SubChunk2Size )
    UINT8           aucRiffFormat[4];   // 一般就是" WAVE" 四个字母

    UINT8           aucFormat[4];       // 格式说明块，本字段一般就是"fmt "
    UINT32          ulForamtLen;        // 本数据块的大小，不包括ID和Size字段本身 （对于PCM，值为16）
    UINT16          usFormatTag;
    UINT16          usChannels;
    UINT32          ulSamplesPerSec;
    UINT32          ulAvgBytesPerSec;
    UINT16          usBlockAlign;
    UINT16          usBitsPerSample;

    UINT8           aucData[4];
    UINT32          ulDataLen;
}WAVE_FILE_HDR_T;

#pragma pack(pop)

UINT32 GetWavFileMediaLength(CHAR *szFile)
{
    FILE    *fp = fopen(szFile, "rb");

    if (!fp)
    {
        return 0;
    }

    WAVE_FILE_HDR_T     stHdr;

    memset(&stHdr, 0, sizeof(stHdr));
    fread(&stHdr, 1, sizeof(stHdr), fp);

    fclose(fp);

    double dMediaLen = ((double)stHdr.ulDataLen)/stHdr.ulAvgBytesPerSec + 0.5;

    return (UINT32)dMediaLen;
}

WavRec::WavRec()
{
    bInited = FALSE;

    memset(m_afp, 0, sizeof(m_afp));
    m_aulTotalDataLen[2];
    m_szFile = NULL;

    m_ulSampleRate = 0;
    m_ulSampleBytes = 0;
    m_bMergeMode = FALSE;
}

VOID WavRec::Init(CHAR *szFile, UINT32 ulSampleRate, UINT32 ulSampleBytes, BOOL bMergeMode)
{
    StopRec();

    m_Mutex.Mutex();

    CHAR    acFile[512];

    bInited = TRUE;
    memset(m_aulTotalDataLen, 0, sizeof(m_aulTotalDataLen));
    m_ulSampleRate = ulSampleRate;
    m_ulSampleBytes = ulSampleBytes;
    m_bMergeMode = bMergeMode;

    m_szFile = strdup(szFile);

    sprintf(acFile, "%s.left.wav", szFile);
    m_afp[0] = fopen(acFile, "w+b");
    if (m_afp[0])
    {
        WriteWavHdr(m_afp[0], 1, m_ulSampleRate, m_ulSampleBytes);
    }

    sprintf(acFile, "%s.right.wav", szFile);
    m_afp[1] = fopen(acFile, "w+b");
    if (m_afp[1])
    {
        WriteWavHdr(m_afp[1], 1, m_ulSampleRate, m_ulSampleBytes);
    }

    m_Mutex.Unmutex();
}

WavRec::~WavRec()
{
    StopRec();
}

VOID WavRec::StopRec()
{
    m_Mutex.Mutex();

    if (!m_szFile)
    {
        m_Mutex.Unmutex();
        return;
    }

    CHAR        acLeftFile[512];
    CHAR        acRightFile[512];

    sprintf(acLeftFile, "%s.left.wav", m_szFile);
    sprintf(acRightFile, "%s.right.wav", m_szFile);

    if (m_afp[0])
    {
        fclose(m_afp[0]);
        m_afp[0] = NULL;
    }

    if (m_afp[1])
    {
        fclose(m_afp[1]);
        m_afp[1] = NULL;
    }

    if (gos_get_file_size(acLeftFile) <= (int)sizeof(WAVE_FILE_HDR_T) )
    {
        gos_delete_file(acLeftFile);
    }

    if (gos_get_file_size(acRightFile) <= (int)sizeof(WAVE_FILE_HDR_T) )
    {
        gos_delete_file(acRightFile);
    }

    MergeWavFile(acLeftFile, acRightFile, m_szFile, m_bMergeMode);

    GOS_FREE(m_szFile);

    bInited = FALSE;

    m_Mutex.Unmutex();
}

BOOL WriteWavHdr(FILE *fp, UINT32 ulChanNum, UINT32 ulSampleRate, UINT32 ulSampleBytes)
{
    WAVE_FILE_HDR_T     stFileHdr = {0};

    memcpy(stFileHdr.aucChunkID, "RIFF", 4);
    stFileHdr.ulChunkSize = 36;     // 要变化
    memcpy(stFileHdr.aucRiffFormat, "WAVE", 4);

    memcpy(stFileHdr.aucFormat, "fmt ", 4);
    stFileHdr.ulForamtLen = 16;                  //块大小    16
    stFileHdr.usFormatTag = 1;                  //音频格式一般为WAVE_FORMAT_PCM
    stFileHdr.usChannels = ulChanNum;           //采样声道数
    stFileHdr.ulSamplesPerSec = ulSampleRate;   //采样率
    stFileHdr.ulAvgBytesPerSec = ulSampleRate*ulSampleBytes*ulChanNum;  //88200每秒字节数  采样率*采样精度（指的bit位数）/8(字节位数)
    stFileHdr.usBlockAlign = ulChanNum*ulSampleBytes;           //块大小 = 采样字节*声道数
    stFileHdr.usBitsPerSample = ulSampleBytes*8;// stFileHdr.usBlockAlign*8;

    memcpy(stFileHdr.aucData, "data", 4);
    stFileHdr.ulDataLen = 0;

    return fwrite(&stFileHdr, 1, sizeof(stFileHdr), fp)== sizeof(stFileHdr);
}

BOOL WavRec::Rec(BOOL bLeftChan, UINT8 *pucData, UINT32 ulLen)
{
    m_Mutex.Mutex();

    if (!bInited)
    {
        m_Mutex.Unmutex();
        return FALSE;
    }

    UINT32      ulChan = bLeftChan?0:1;
    FILE        *fp = m_afp[ulChan];

    if (!fp)
    {
        m_Mutex.Unmutex();
        return FALSE;
    }

    fseek(fp, 0, SEEK_END);
    fwrite(pucData, 1, ulLen, fp);
    m_aulTotalDataLen[ulChan] += ulLen;

    UINT32      ulOffsetChunkSize = STRUCT_OFFSET(WAVE_FILE_HDR_T, ulChunkSize);
    UINT32      ulOffsetDataLen = STRUCT_OFFSET(WAVE_FILE_HDR_T, ulDataLen);
    UINT32      ulChunkSize = 36 + m_aulTotalDataLen[ulChan];

    fseek(fp, ulOffsetChunkSize, SEEK_SET);
    fwrite(&ulChunkSize, 1, sizeof(ulChunkSize), fp);

    fseek(fp, ulOffsetDataLen, SEEK_SET);
    fwrite(&m_aulTotalDataLen[ulChan], 1, sizeof(m_aulTotalDataLen[ulChan]), fp);

    fflush(fp);

    m_Mutex.Unmutex();

    return TRUE;
}

static INT16 Merge(INT16 sLeft, INT16 sRight)
{
#if 0
    INT32   iValue = ((INT32)sLeft) + sRight;

    iVvalue /= 2;
    if (iValue > 0x7fff)
    {
        iValue = 0x7fff;
    }
    else if (iValue < -0x8000)
    {
        iValue = -0x8000;
    }

    return (INT16)iValue;

#else
    INT32   iValue = ((INT32)sLeft) + sRight;
    INT32   iValue2 = ((INT32)sLeft)*sRight;
    INT32   iValue3 = 32768;

    if (sLeft < 0 && sRight < 0)
    {
        iValue = iValue + iValue2/iValue3;
    }
    else
    {
        iValue = iValue - iValue2/iValue3;
    }

    if (iValue > 0x7fff)
    {
        iValue = 0x7fff;
    }
    else if (iValue < -0x8000)
    {
        iValue = -0x8000;
    }

    return (INT16)iValue;
#endif
}

BOOL MergeWavFile(CHAR *szFile1, CHAR *szFile2, CHAR *szFile, BOOL bMergeMode)
{
    if (gos_file_exist(szFile1) &&
        !gos_file_exist(szFile2) )
    {
        return gos_rename_file(szFile1, szFile);
    }
    else if (!gos_file_exist(szFile1) &&
             gos_file_exist(szFile2) )
    {
        return gos_rename_file(szFile2, szFile);
    }

    FILE    *fp1 = fopen(szFile1, "rb");
    FILE    *fp2 = fopen(szFile2, "rb");
    FILE    *fp = fopen(szFile, "w+b");
    UINT32  ulLen1 = 0;
    UINT32  ulLen2 = 0;
    UINT32  ulLen = 0;
    UINT8   *pucData1 = NULL;
    UINT8   *pucData2;
    UINT32  i;
    WAVE_FILE_HDR_T stHdr = {0};
    UINT32  ulSampleRate;
    UINT32  ulSampleBytes;
    UINT32  ulChunkSize;
    INT16   *psLeft;
    INT16   *psRight;
    INT16   sMerge;
    UINT32  ulChanNum;
    UINT32  ulDataLen = 0;
    UINT32  ulOffsetChunkSize = STRUCT_OFFSET(WAVE_FILE_HDR_T, ulChunkSize);
    UINT32  ulOffsetDataLen = STRUCT_OFFSET(WAVE_FILE_HDR_T, ulDataLen);
    BOOL    bRet = FALSE;

    if (!fp1 || !fp2 || !fp)
    {
        goto end;
    }

    fread(&stHdr, 1, sizeof(stHdr), fp1);
    ulLen1 = stHdr.ulDataLen;
    ulSampleRate = stHdr.ulSamplesPerSec;
    ulSampleBytes = stHdr.usBitsPerSample/8;

    if (stHdr.usChannels != 1)
    {
        goto end;
    }

    fread(&stHdr, 1, sizeof(stHdr), fp2);
    if (ulSampleRate != stHdr.ulSamplesPerSec ||
        ulSampleBytes != (UINT32)stHdr.usBitsPerSample/8)
    {
        goto end;
    }

    if (stHdr.usChannels != 1)
    {
        goto end;
    }

    ulLen2 = stHdr.ulDataLen;

    if (ulLen1 == 0 && ulLen2 == 0)
    {
        goto end;
    }

    ulLen = ulLen1>ulLen2?ulLen1:ulLen2;
    pucData1 = (UINT8*)calloc(1, 2*ulLen);
    if (!pucData1)
    {
        goto end;
    }

    pucData2 = pucData1+ulLen;
    fread(pucData1, 1, ulLen1, fp1);
    fread(pucData2, 1, ulLen2, fp2);

    if (ulSampleBytes != 2)
    {
        bMergeMode = FALSE;
    }

    if (bMergeMode)
    {
        ulChanNum = 1;
    }
    else
    {
        ulChanNum = 2;
    }

    if (!WriteWavHdr(fp, ulChanNum, ulSampleRate, ulSampleBytes))
    {
        goto end;
    }

    for (UINT32 i=0; i<ulLen/ulSampleBytes; i++)
    {
        if (bMergeMode)
        {
            psLeft = (INT16*)(pucData1+i*ulSampleBytes);
            psRight = (INT16*)(pucData2+i*ulSampleBytes);
            sMerge = Merge(psLeft[0], psRight[0]);
            fwrite(&sMerge, 1, ulSampleBytes, fp);
            ulDataLen += ulSampleBytes;
        }
        else
        {
            fwrite(pucData1+i*ulSampleBytes, 1, ulSampleBytes, fp);
            fwrite(pucData2+i*ulSampleBytes, 1, ulSampleBytes, fp);

            ulDataLen += ulSampleBytes*2;
        }
    }

    ulChunkSize = 36 + ulDataLen;
    fseek(fp, ulOffsetChunkSize, SEEK_SET);
    fwrite(&ulChunkSize, 1, sizeof(ulChunkSize), fp);

    fseek(fp, ulOffsetDataLen, SEEK_SET);
    fwrite(&ulDataLen, 1, sizeof(ulDataLen), fp);

    bRet = TRUE;

end:
    if (fp1)
    {
        fclose(fp1);
    }

    if (fp2)
    {
        fclose(fp2);
    }

    if (fp)
    {
        fclose(fp);
    }

    GOS_FREE(pucData1);

    return bRet;
}
