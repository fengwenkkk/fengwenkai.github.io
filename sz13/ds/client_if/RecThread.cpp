#include "g_include.h"
#include "RecThread.h"

#pragma pack(push, 1)
typedef struct
{
    UINT8           aucChunkID[4];      // 文件头标识，一般就是" RIFF" 四个字母
    UINT32          ulChunkSize;        // 整个数据文件的大小，不包括上面ID和Size本身 = 4 + ( 8 + SubChunk1Size ) + ( 8 + SubChunk2Size )
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

RecThread::RecThread(BOOL bMergeMode):GThread(NULL)
{
//  m_pRecPCM = new FfmpegRecPCM();
    m_fp = NULL;
    m_ulRecState = REC_STATE_IDLE;
    m_bNeedSave = FALSE;
    m_bMergeMode = bMergeMode;   // merge后为单声道
}

VOID RecThread::WriteWaveFileHdr()
{
    WAVE_FILE_HDR_T     stFileHdr = {0};
    UINT32              ulChanNum = 2;

    if (m_bMergeMode)
    {
        ulChanNum = 1;
    }

    memcpy(stFileHdr.aucChunkID, "RIFF", 4);
    stFileHdr.ulChunkSize = 36;     // 要变化
    memcpy(stFileHdr.aucRiffFormat, "WAVE", 4);

    memcpy(stFileHdr.aucFormat, "fmt ", 4);
    stFileHdr.ulForamtLen = 16;                  //块大小    16
    stFileHdr.usFormatTag = 1;              //音频格式一般为WAVE_FORMAT_PCM
    stFileHdr.usChannels = ulChanNum;               //采样声道数
    stFileHdr.ulSamplesPerSec = m_ulSampleRate; //采样率
    stFileHdr.ulAvgBytesPerSec = m_ulSampleRate*m_ulSampleBytes*ulChanNum;  //88200每秒字节数  采样率*采样精度（指的bit位数）/8(字节位数)
    stFileHdr.usBlockAlign = ulChanNum*m_ulSampleBytes;         //块大小 = 采样字节*声道数
    stFileHdr.usBitsPerSample = m_ulSampleBytes*8;// stFileHdr.usBlockAlign*8;

    memcpy(stFileHdr.aucData, "data", 4);
    stFileHdr.ulDataLen = 0;

    fwrite(&stFileHdr, 1, sizeof(stFileHdr), m_fp);
}

BOOL RecThread::StartRec(CHAR *szFile, UINT32 ulSampleRate, UINT32 ulSampleBytes)
{
    if (m_ulRecState != REC_STATE_IDLE)
    {
        return FALSE;
    }

    m_fp = fopen(szFile, "w+b");
    if (!m_fp)
    {
        return FALSE;
    }

    m_ulTotalDataLen = 0;
    m_ulSampleRate = ulSampleRate;
    m_ulSampleBytes = ulSampleBytes;
    m_ulBytesPerSec = ulSampleRate*ulSampleBytes;

    WriteWaveFileHdr();

    m_ulRecState = REC_STATE_WORK;
    m_bNeedSave = FALSE;

    return TRUE;
}

VOID RecThread::StopRec()
{
    if (m_ulRecState != REC_STATE_WORK)
    {
        return;
    }

    m_ulRecState = REC_STATE_CLOSING;
}

#define IN_RANGE(x)   ((x) >= i64StartTime && (x) <= i64EndTime)

INT16 Merge(INT16 sLeft, INT16 sRight)
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

VOID RecThread::Save(INT64 i64StartTime, INT64 i64EndTime)
{
    if (i64StartTime > i64EndTime)
    {
        return;
    }

    GosLog(LOG_INFO, "push rec time %.1f %.1f",
            ((double)i64StartTime)/1000000000,
            ((double)i64EndTime)/1000000000);

    UINT32  i;
    INT64   i64RemoteTime;
    INT64   i64LocalTime;
    UINT32  ulLocalIndex = 0;
    UINT32  ulRemoteIndex = 0;
    UINT32  ulLocalNum = m_vLocalRec.size();
    UINT32  ulRemoteNum = m_vRemoteRec.size();
    REC_BUF_T           stRec;
    VECTOR<REC_BUF_T>   vRec;

    while(1)
    {
        if (ulLocalIndex >= ulLocalNum && ulRemoteIndex >= ulRemoteNum)
        {
            break;
        }

        i64LocalTime  = -1;
        i64RemoteTime = -1;

        if (ulLocalIndex < ulLocalNum)
        {
            i64LocalTime = m_vLocalRec[ulLocalIndex].i64Time;
        }

        if (ulRemoteIndex < ulRemoteNum)
        {
            i64RemoteTime = m_vRemoteRec[ulRemoteIndex].i64Time;
        }

        memset(&stRec, 0 ,sizeof(stRec));

        if (i64LocalTime < 0 ||
            (i64RemoteTime > 0 && i64RemoteTime < i64LocalTime))
        {
            if (!IN_RANGE(i64RemoteTime) )
            {
                break;
            }

            if (!m_bMergeMode)
            {
                vRec.push_back(stRec);
                vRec.push_back(m_vRemoteRec[ulRemoteIndex++]);
            }
            else
            {
                vRec.push_back(m_vRemoteRec[ulRemoteIndex]);
                vRec.push_back(m_vRemoteRec[ulRemoteIndex++]);
            }

            continue;
        }

        if (i64RemoteTime < 0 ||
            (i64LocalTime > 0 && i64LocalTime < i64RemoteTime))
        {
            if (!IN_RANGE(i64LocalTime) )
            {
                break;
            }

            if (!m_bMergeMode)
            {
                vRec.push_back(m_vLocalRec[ulLocalIndex++]);
                vRec.push_back(stRec);
            }
            else
            {
                vRec.push_back(m_vLocalRec[ulLocalIndex]);
                vRec.push_back(m_vLocalRec[ulLocalIndex++]);
            }

            continue;
        }

        // must be: ulRemoteStartTime == ulLocalStartTime
        if (i64LocalTime != i64RemoteTime)
        {
            GosLog(LOG_ERROR, "impossible time: "FORMAT_I64" and "FORMAT_I64, i64LocalTime, i64RemoteTime);
        }

        if (!IN_RANGE(i64LocalTime) )
        {
            break;
        }

        vRec.push_back(m_vLocalRec[ulLocalIndex++]);
        vRec.push_back(m_vRemoteRec[ulRemoteIndex++]);
    }

    VECTOR<REC_BUF_T>       vTmp;

    if (ulLocalIndex >= m_vLocalRec.size())
    {
        m_vLocalRec.clear();
    }
    else
    {
        vTmp.assign(m_vLocalRec.begin()+ulLocalIndex, m_vLocalRec.end());
        vTmp.swap(m_vLocalRec);
    }

    if (ulRemoteIndex >= m_vRemoteRec.size())
    {
        m_vRemoteRec.clear();
    }
    else
    {
        vTmp.assign(m_vRemoteRec.begin()+ulRemoteIndex, m_vRemoteRec.end());
        vTmp.swap(m_vRemoteRec);
    }

    GosLog(LOG_INFO, "current rec number is %u and %u", m_vLocalRec.size(), m_vRemoteRec.size());

    UINT32  ulNum = vRec.size()/2;
    UINT32  ulLen = 2*m_ulSampleBytes*ulNum;
    UINT8   *pucData = (UINT8*)calloc(1, ulLen);

    if (!pucData)
    {
        return;
    }

    i64StartTime = vRec[0].i64Time?vRec[0].i64Time:vRec[1].i64Time;
    i64EndTime = vRec[2*ulNum-2].i64Time?vRec[2*ulNum-2].i64Time:vRec[2*ulNum-1].i64Time;

    GosLog(LOG_ERROR, "push rec number %u, time %.1f %.1f", ulNum,
            ((double)i64StartTime)/1000000000,
            ((double)i64EndTime)/1000000000);

    INT16  *psLeft;
    INT16  *psRight;
    INT16  sMerge;

    for (UINT32 i=0; i<ulNum; i++)
    {
        if (m_bMergeMode && m_ulSampleBytes == sizeof(INT16))
        {
            psLeft = (INT16*)vRec[2*i].aucRec;
            psRight = (INT16*)vRec[2*i+1].aucRec;
            sMerge = Merge(psLeft[0], psRight[0]);

            memcpy(pucData+i*m_ulSampleBytes, &sMerge, m_ulSampleBytes);
        }
        else  //TODO
        {
            memcpy(pucData+i*2*m_ulSampleBytes, vRec[2*i].aucRec, m_ulSampleBytes);
            memcpy(pucData+i*2*m_ulSampleBytes+m_ulSampleBytes, vRec[2*i+1].aucRec, m_ulSampleBytes);
        }
    }

    if (m_bMergeMode)
    {
        ulLen /= 2;
    }

    //m_pRecPCM->RecPcmData(pucData, ulLen);
    fseek(m_fp, 0, SEEK_END);
    fwrite(pucData, 1, ulLen, m_fp);
    free(pucData);
    m_ulTotalDataLen += ulLen;

    UINT32      ulOffsetChunkSize = STRUCT_OFFSET(WAVE_FILE_HDR_T, ulChunkSize);
    UINT32      ulOffsetDataLen = STRUCT_OFFSET(WAVE_FILE_HDR_T, ulDataLen);
    UINT32      ulChunkSize = 36 + m_ulTotalDataLen;

    fseek(m_fp, ulOffsetChunkSize, SEEK_SET);
    fwrite(&ulChunkSize, 1, sizeof(ulChunkSize), m_fp);

    fseek(m_fp, ulOffsetDataLen, SEEK_SET);
    fwrite(&m_ulTotalDataLen, 1, sizeof(m_ulTotalDataLen), m_fp);

    fflush(m_fp);
}

VOID RecThread::SaveCheck(BOOL bSaveAll)
{
    m_Mutex.Mutex();

    m_bNeedSave = FALSE;
    if (m_vTmpLocalRec.size() > 0)
    {
        for (UINT32 i=0; i<m_vTmpLocalRec.size(); i++)
        {
            m_vLocalRec.push_back(m_vTmpLocalRec[i]);
        }

        m_vTmpLocalRec.clear();

        GosLog(LOG_DETAIL, "local rec number is %u", m_vLocalRec.size());
    }

    if (m_vTmpRemoteRec.size() > 0)
    {
        for (UINT32 i=0; i<m_vTmpRemoteRec.size(); i++)
        {
            m_vRemoteRec.push_back(m_vTmpRemoteRec[i]);
        }

        m_vTmpRemoteRec.clear();

        GosLog(LOG_DETAIL, "remote rec number is %u", m_vRemoteRec.size());
    }

    m_Mutex.Unmutex();

    if (bSaveAll)
    {
        if (m_vLocalRec.size() > 0)
        {
            if (m_vRemoteRec.size() == 0)
            {
                REC_BUF_T   stBuf = {0};

                m_vRemoteRec.push_back(stBuf);
            }

            // remote时间等于local时间，保证全部被存储
            m_vRemoteRec[m_vRemoteRec.size()-1].i64Time = m_vLocalRec[m_vLocalRec.size()-1].i64Time;
        }
        else
        {
            if (m_vRemoteRec.size() > 0)
            {
                REC_BUF_T   stBuf = {0};

                m_vLocalRec.push_back(stBuf);

                // remote时间等于local时间，保证全部被存储
                m_vLocalRec[m_vLocalRec.size()-1].i64Time = m_vRemoteRec[m_vRemoteRec.size()-1].i64Time;
            }
        }
    }

    if (m_vLocalRec.size() == 0)
    {
        return;
    }

    if (m_vRemoteRec.size() == 0)
    {
        Save(m_vLocalRec[0].i64Time, m_vLocalRec[m_vLocalRec.size()-1].i64Time);
        return;
    }

    INT64   i64StartTime = MIN(m_vRemoteRec[0].i64Time, m_vLocalRec[0].i64Time);
    INT64   i64EndTime = MIN(m_vRemoteRec[m_vRemoteRec.size()-1].i64Time, m_vLocalRec[m_vLocalRec.size()-1].i64Time);

    Save(i64StartTime, i64EndTime);
}

INT64 RecThread::GetSampleTime(UINT32 ulIndex)
{
    INT64   i64Value = ulIndex;

    i64Value *= 1000000000;
    i64Value /= m_ulSampleRate;

    return i64Value;
}

VOID RecThread::RecLocal(UINT8 *pucData, UINT32 ulLen, INT64 i64Time)
{
    if (m_ulRecState != REC_STATE_WORK)
    {
        return;
    }

    AUTO_LOCK(m_Mutex);

    REC_BUF_T   stBuf = {0};
    UINT32      ulRecNum = ulLen/m_ulSampleBytes;

    m_bNeedSave = TRUE;

//  i64Time = i64Time - GetSampleTime(ulRecNum);

    for (UINT32 i=0; i<ulRecNum; i++)
    {
        memcpy(stBuf.aucRec, pucData+i*m_ulSampleBytes, m_ulSampleBytes);
        stBuf.i64Time = i64Time + GetSampleTime(i);
        m_vTmpLocalRec.push_back(stBuf);
    }
}

VOID RecThread::RecRemote(UINT8 *pucData, UINT32 ulLen, INT64 i64Time)
{
    if (m_ulRecState != REC_STATE_WORK)
    {
        return;
    }

    AUTO_LOCK(m_Mutex);

    REC_BUF_T   stBuf = {0};
    UINT32      ulRecNum = ulLen/m_ulSampleBytes;

    if (m_vTmpLocalRec.size() > 0)
    {
        m_bNeedSave = TRUE;
    }

    for (UINT32 i=0; i<ulRecNum; i++)
    {
        memcpy(stBuf.aucRec, pucData+i*m_ulSampleBytes, m_ulSampleBytes);
        stBuf.i64Time = i64Time + GetSampleTime(i);//i*1000/m_ulSampleRate;
        m_vTmpRemoteRec.push_back(stBuf);
    }
}

VOID RecThread::Free()
{
    if (m_ulRecState == REC_STATE_FREED)
    {
        return;
    }

    m_ulRecState = REC_STATE_FREEING;

    for (UINT32 i=0; i<50; i++)
    {
        if (m_ulRecState == REC_STATE_FREED)
        {
            break;
        }

        gos_sleep_1ms(100);
    }
}

GOS_THREAD_RET RecThread::ThreadEntry(void* pPara)
{
    UINT32      ulSavePeriod = 3000;
    UINT32      ulLastCheckTime = gos_get_uptime_1ms();
    UINT32      ulTime;

    while(GetThreadStatus() == THREAD_STATUS_RUN)
    {
        for (UINT32 i=0; i<10; i++)
        {
            gos_sleep_1ms(100);

            if (m_ulRecState == REC_STATE_FREEING)
            {
                SaveCheck(TRUE);
                fclose(m_fp);
                m_fp = NULL;
                goto end;
            }
        }

        if (m_ulRecState == REC_STATE_CLOSING)
        {
            SaveCheck(TRUE);
            fclose(m_fp);
            m_fp = NULL;

            m_ulRecState = REC_STATE_CLOSED;
            continue;
        }

        if (m_ulRecState != REC_STATE_WORK)
        {
            continue;
        }

        if (m_bNeedSave)
        {
            SaveCheck(FALSE);
            ulLastCheckTime = gos_get_uptime_1ms();
            continue;
        }

        ulTime = gos_get_uptime_1ms() - ulLastCheckTime;
        if (ulTime >= ulSavePeriod)
        {
            SaveCheck(FALSE);
            ulLastCheckTime = gos_get_uptime_1ms();
        }
    }

end:
    m_ulRecState = REC_STATE_FREED;

    return (GOS_THREAD_RET)0;
}
