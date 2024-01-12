#include "RecPublic.h"
#include "rec_util.h"
#include "MRSUDPPortManager.h"

#ifndef USE_FFMPEG_CMD
#include "ffmpeg_api.h"
#endif

extern BOOL g_bDCLogined;

#define AUDIO_HEAD_LEN      13   //接收到的音频帧数据头长度
#define MAX_H264PACK_LEN    2048 //接收到的H264包最大长度
#define VIDEO_HEAD_LEN      12   //接收到的视屏包包头长度
#define RTP_H264_PAYLOAD    102  //H264的payload type
#define RTP_VERSION         2

#define AMR_TYPE_4_75       0x04    //4.75k优先
#define AMR_TYPE_12_2       0x3c    //12.2k优先

#define AMR_LEN_4_75        13
#define AMR_LEN_12_2        32


typedef struct
{
    unsigned short v:2;     /* packet type */
    unsigned short p:1;     /* padding flag */
    unsigned short x:1;     /* header extension flag */
    unsigned short cc:4;    /* CSRC count */
    unsigned short pt:7;    /* payload type */
    unsigned short m:1;     /* marker bit */
    unsigned short seq;     /* sequence number */
    unsigned int ts;        /* timestamp */
    unsigned int ssrc;      /* synchronization source */
} rtp_hdr_t;

typedef struct
{
    UINT32  ulSSRC;
    UINT32  ulLen;
    UINT8   aucData[1500];    // 11.2 kbps AMR
}RTP_DATA_T;

GMutex                                  g_MutexRecThread;
std::map<UINT32, REC_THREAD_INFO_T>     g_mAudioRecThread;
std::map<UINT32, REC_THREAD_INFO_T>     g_mVideoRecThread;


extern CHAR     g_acFFMpegExe[256];


UINT32          g_ulLowerUDPPort; ///mrs.ini中获取该配置，若没有则默认为51000
UINT32          g_ulUpperUDPPort; ///mrs.ini中获取该配置，若没有则默认为56999
UINT32          g_ulRecvIdleTime = 100;

extern GMutex                           g_MutexUDPPort;
IntSet                                  g_sUDPPort;

static BOOL ReadAmrFramData(UINT8 *pucData, UINT32 ulDataLen, UINT8 *pucFrame, UINT32 ulMaxFrameLen, UINT32 &ulFrameLen)
{
    UINT32  ulFramHeadNum = 0;

    ulFrameLen = 0;
    // 获取帧头个数
    for(UINT32 i=0; i<ulDataLen; i++)
    {
        if (ulFrameLen >= ulMaxFrameLen)
        {
            GosLog(LOG_ERROR, "frame len is large than %u", ulMaxFrameLen);
            return FALSE;
        }

        if ((pucData[i] & 0x7f) == AMR_TYPE_4_75 )
        {
            ulFrameLen += AMR_LEN_4_75;
            ulFramHeadNum++;
        }
        else if ((pucData[i] & 0x7f) == AMR_TYPE_12_2 )
        {
            ulFrameLen += AMR_LEN_12_2;
            ulFramHeadNum++;
        }
        else
        {
            GosLog(LOG_ERROR, "unknown amr type %u", pucData[i] & 0x7f);
            ulFrameLen = 0;
            return TRUE;
        }

        // 一直读取到最后一个帧头 Last frame in this payload
        if ((pucData[i] & 0x80) == 0)
        {
            break;
        }      
    }
    // pucDataFrame 是指向AMR数据帧的指针
    UINT8 *pucDataFrame  = pucData + ulFramHeadNum;
    
    // 拼接帧头和数据 
    for (UINT32 j=0; j<ulFramHeadNum; j++)
    {
        if ((pucData[j] & 0x7f) == AMR_TYPE_4_75 )
        {
            *pucFrame = AMR_TYPE_4_75;
            memcpy(pucFrame+1, pucDataFrame, AMR_LEN_4_75 - 1);      
            pucDataFrame += AMR_LEN_4_75-1;
            pucFrame += AMR_LEN_4_75;
        }
        else if ( (pucData[j] & 0x7f) == AMR_TYPE_12_2 )
        {
            *pucFrame = AMR_TYPE_12_2;
            memcpy(pucFrame+1, pucDataFrame, AMR_LEN_12_2 - 1);       
            pucDataFrame += AMR_LEN_12_2-1;
            pucFrame += AMR_LEN_12_2;
        }   
    }

    return TRUE;
}

// TODO 如何判断执行过程中的异常退出导致的失败？
// ffmpeg -i a.h264 -i b.amr -map 0:v -map 1:a -c:v copy -vcodec copy c.mp4
BOOL MergeAudioVideoFile(CHAR *szAudioFile, CHAR *szVideoFile, CHAR *szDestFile)
{
#ifndef USE_FFMPEG_CMD  //if (!g_bUseFFMpegCmd)
    {
        return ffmpeg_MixFile(szAudioFile, szVideoFile, szDestFile) == 0;
    }
#endif

    CHAR        acCmd[1024];

    sprintf(acCmd, "%s -i %s -i %s -map 0:v -map 1:a -c:v copy -vcodec copy %s -loglevel panic", g_acFFMpegExe, szVideoFile, szAudioFile, szDestFile);

    gos_delete_file(szDestFile);
    GosLog(LOG_DETAIL, "ffmpeg cmd : %s", acCmd);
    gos_system(acCmd);

    return gos_get_file_size(szDestFile) > 0;
}

// ffmpeg -i 2_20210412111406_11002_13003.h264 -vcodec copy -an 2_20210412111406_11002_13003.mp4
BOOL ConvertVideoFile(CHAR *szVideoFile, CHAR *szDestFile)
{
#ifndef USE_FFMPEG_CMD //    if (!g_bUseFFMpegCmd)
    {
        return ffmpeg_ConvertVideoFile(szVideoFile, szDestFile) == 0;
    }
#endif

    CHAR        acCmd[1024];

    sprintf(acCmd, "%s -i %s -vcodec copy -an %s -loglevel panic", g_acFFMpegExe, szVideoFile, szDestFile);

    gos_delete_file(szDestFile);
    GosLog(LOG_DETAIL, "ffmpeg cmd : %s", acCmd);
    gos_system(acCmd);

    return gos_get_file_size(szDestFile) > 0;
}

BOOL ConvertAudioFile(CHAR *szAudioFile, CHAR *szDestFile)
{
#ifndef USE_FFMPEG_CMD  //    if (!g_bUseFFMpegCmd)
    {
        return ffmpeg_ConvertAudioFile(szAudioFile, szDestFile) == 0;
    }
#endif

    CHAR        acCmd[1024];

    sprintf(acCmd, "%s -i %s -vcodec copy %s -loglevel panic", g_acFFMpegExe, szAudioFile, szDestFile);

    gos_delete_file(szDestFile);
    GosLog(LOG_DETAIL, "ffmpeg cmd : %s", acCmd);
    gos_system(acCmd);

    return gos_get_file_size(szDestFile) > 0;
}

// 将两个单声道文件混音成一个音频文件
BOOL MixAudioFile(CHAR *szFile1, CHAR *szFile2, CHAR *szDestFile)
{
    CHAR        acCmd[1024+256] = {0};

    //ffmpeg -y -i tmp1.amr -i tmp2.amr -filter_complex amix=inputs=2:duration=first test.mp3

    sprintf(acCmd, "%s  -y -i %s -i %s -filter_complex amix=inputs=2:duration=first  %s -loglevel panic",
            g_acFFMpegExe, szFile1, szFile2, szDestFile);

    gos_delete_file(szDestFile);
    GosLog(LOG_DETAIL, "ffmpeg cmd : %s", acCmd);
    gos_system(acCmd);

    return gos_get_file_size(szDestFile) > 0;
}

BOOL ConvertP2PAmrFile(CHAR *szFile, CHAR *szDestFile)
{
    CHAR        acFile1[256];
    CHAR        acFile2[256];
    FILE        *fp = NULL;
    FILE        *fp1 = NULL;
    FILE        *fp2 = NULL;
    UINT8       *pucBuf = NULL;
    INT32       iLen = gos_get_file_size(szFile);
    INT32       iNum;
    INT32       iHdrLen = 6;  // AMR文件头长度
    UINT8        *pucPos;
    UINT32      ulFrameSize = 32;       // AMR文件帧长度（对应11.2kbps编码）

    if (iLen <= iHdrLen)
    {
        return FALSE;
    }

    fp = fopen(szFile, "rb");
    if (!fp)
    {
        return FALSE;
    }

    pucBuf = (UINT8*)calloc(1, iLen);
    if (iLen != fread(pucBuf, 1, iLen, fp))
    {
        fclose(fp);
        free(pucBuf);

        return FALSE;
    }

    fclose(fp);

    iNum = (iLen-iHdrLen)/(ulFrameSize*2);

    sprintf(acFile1, "%s/tmp1.amr", gos_get_root_path());
    sprintf(acFile2, "%s/tmp2.amr", gos_get_root_path());
//    sprintf(acFile1, "tmp1.amr");
//    sprintf(acFile2, "tmp2.amr");
    gos_format_path(acFile1);
    gos_format_path(acFile2);
    fp1 = fopen(acFile1, "wb");
    if (!fp1)
    {
        free(pucBuf);
        return FALSE;
    }

    fp2 = fopen(acFile2, "wb");
    if (!fp2)
    {
        fclose(fp1);
        gos_delete_file(acFile1);
        free(pucBuf);
        return FALSE;
    }

    //fwrite(pucBuf, 1, iHdrLen, fp1);
    //fwrite(pucBuf, 1, iHdrLen, fp2);
    fwrite("#!AMR-WB\n", 1, strlen("#!AMR-WB\n"), fp1);
    fwrite("#!AMR-WB\n", 1, strlen("#!AMR-WB\n"), fp2);

    pucPos = pucBuf+iHdrLen;

    iNum --;
    for (INT32 i=0; i<iNum; i++)
    {
        fwrite(pucPos, 1, ulFrameSize, fp1);
        pucPos += ulFrameSize;

        fwrite(pucPos, 1, ulFrameSize, fp2);
        pucPos += ulFrameSize;
    }

    fclose(fp1);
    fclose(fp2);
    free(pucBuf);

    BOOL    bRet = MixAudioFile(acFile1, acFile2, szDestFile);

    gos_delete_file(acFile1);
    gos_delete_file(acFile2);

    return bRet;
}

// TODO 通过ffmpeg函数实现，需要支持Linux版本的ffmpeg
// Duration: 00:00:10.64, start: 0.000000, bitrate: 4029 kb/s
DOUBLE GetMediaFileDuration(CHAR *szFile)
{
#ifndef USE_FFMPEG_CMD  //    if (!g_bUseFFMpegCmd)
    {
        return ffmpeg_GetMediaFileLength(szFile);
    }
#endif

    CHAR        acCmd[256];
    CHAR        acTmpFile[1024];
    DOUBLE      dDuration = -1;
    static GMutex   Mutex;

    AUTO_LOCK(Mutex);

    sprintf(acTmpFile, "%s/%s", gos_get_root_path(), "duration.tmp");
    gos_format_path(acTmpFile);
    gos_delete_file(acTmpFile);
    if (gos_file_exist(acTmpFile))
    {
        return -1;
    }

    sprintf(acCmd, "%s -i %s 2>%s -loglevel panic", g_acFFMpegExe, szFile, acTmpFile);
    GosLog(LOG_DETAIL, "ffmpeg cmd : %s", acCmd);
    gos_system(acCmd);

    INT32   iSize = gos_get_file_size(acTmpFile);

    if (iSize <= 0)
    {
        return -1;
    }

    FILE    *fp = fopen(acTmpFile, "rb");

    if (!fp)
    {
        return -1;
    }

    CHAR    *szInfo = (CHAR*)calloc(1, iSize+1);

    if (!szInfo)
    {
        fclose(fp);
        return -1;
    }

    if(iSize != fread(szInfo, 1, iSize, fp))
    {
        fclose(fp);
        goto end;
    }
    fclose(fp);

    CHAR    *szStart = strstr(szInfo, "Duration:");
    CHAR    *szEnd;

    if (!szStart)
    {
        goto end;
    }

    // 00:00:28.16,
    szStart += strlen("Duration:");
    szEnd = strchr(szStart, ',');
    if (!szEnd)
    {
        goto end;
    }

    *szEnd = '\0';
    dDuration = 0;

    CHAR        *szHour;
    CHAR        *szMin;
    CHAR        *szSec;
    CHAR        *szMillSec;

    szHour = szStart;
    szEnd = strchr(szHour, ':');
    if (!szEnd)
    {
        goto end;
    }
    *szEnd = '\0';

    szMin = szEnd+1;
    szEnd = strchr(szMin, ':');
    if (!szEnd)
    {
        goto end;
    }
    *szEnd = '\0';

    szSec = szEnd+1;
    szEnd = strchr(szSec, '.');
    if (!szEnd)
    {
        goto end;
    }
    *szEnd = '\0';

    szMillSec = szEnd+1;

    UINT32      aulTime[4];

    if (!gos_atou(szHour, &aulTime[0]) ||
        !gos_atou(szMin, &aulTime[1]) ||
        !gos_atou(szSec, &aulTime[2]) ||
        !gos_atou(szMillSec, &aulTime[3]) ) // .90 = 0.9s   .09 = 0.09s
    {
        goto end;
    }

    dDuration = aulTime[0]*3600 + aulTime[1]*60 + aulTime[2];
    dDuration += ((DOUBLE)aulTime[3])/100;

end:
    GOS_FREE(szInfo);
    return dDuration;
}

VOID RenameTmpFile(CHAR *szFile)
{
    CHAR    acNewFile[256];
    INT32   iLen = sprintf(acNewFile, szFile);

    if (iLen <= 4)
    {
        return;
    }

    if (strcmp(acNewFile+iLen-4, ".tmp") != 0)
    {
        return;
    }

    acNewFile[iLen-4] = '\0';
    if (!gos_rename_file(szFile, acNewFile))
    {
        GosLog(LOG_ERROR, "rename %s to %s failed", szFile, acNewFile);
    }
    else
    {
        GosLog(LOG_INFO, "rename %s to %s succ", szFile, acNewFile);
    }
}

//=====================================================================
//pBuf为H264 RTP视频数据包，nSize为RTP视频数据包字节长度，outSize为输出视频数据帧字节长度。
//返回值为指向视频数据帧的指针。
BYTE* Parse_RTP_Packet(BYTE pBuf[], unsigned short nSize, int* outSize, unsigned int &ulSSRC)
{
    if (nSize < VIDEO_HEAD_LEN)
    {
        GosLog(LOG_ERROR, "the pack size %d is too small", nSize);
        return NULL;
    }

    rtp_hdr_t m_RTP_Header;

    m_RTP_Header.v = ((pBuf[0] & 0xc0)>>6);
    m_RTP_Header.p = (pBuf[0] & 0x20)>>5;
    m_RTP_Header.x = (pBuf[0] & 0x10)>>4;
    m_RTP_Header.cc = pBuf[0] & 0x0f;
    m_RTP_Header.m = (pBuf[1] & 0x80)>>7;
    m_RTP_Header.pt = pBuf[1] & 0x7f;

    m_RTP_Header.seq = pBuf[2];
    m_RTP_Header.seq <<= 8;
    m_RTP_Header.seq += pBuf[3];

    m_RTP_Header.ts = pBuf[4];
    m_RTP_Header.ts <<= 8;
    m_RTP_Header.ts += pBuf[5];
    m_RTP_Header.ts <<= 8;
    m_RTP_Header.ts += pBuf[6];
    m_RTP_Header.ts <<= 8;
    m_RTP_Header.ts += pBuf[7];

    m_RTP_Header.ssrc = pBuf[8];
    m_RTP_Header.ssrc <<= 8;
    m_RTP_Header.ssrc += pBuf[9];
    m_RTP_Header.ssrc <<= 8;
    m_RTP_Header.ssrc += pBuf[10];
    m_RTP_Header.ssrc <<= 8;
    m_RTP_Header.ssrc += pBuf[11];

    ulSSRC = m_RTP_Header.ssrc;
    if (m_RTP_Header.v != RTP_VERSION)
    {
        GosLog(LOG_ERROR, "the RTP version(%u) is not correct", m_RTP_Header.v);
        return NULL;
    }

    /*
    若RTP头中的扩展比特位X置1,则一个长度可变的头扩展部分被加到RTP固定头之后,
    头扩展包含16比特的长度域,指示扩展项中32比特字的个数,不包括4个字节扩展头(因此零是有效值).
    RTP固定头之后只允许有一个头扩展.为允许多个互操作实现独立生成不同的头扩展,或某种特定实现有多种不同的头扩展,扩展项的前16比特用以识别标识符或参数.
    这16比特的格式由具体实现的上层协议定义.基本的RTP说明并不定义任何头扩展本身
    */
    UINT32 ulRTPHeadLenth = VIDEO_HEAD_LEN;
    UINT32 ulExtensionLenth = 0;

    // 若扩展比特位X置1，处理扩展头
    if (m_RTP_Header.x)
    {
        if (nSize <= (VIDEO_HEAD_LEN+2+2))  
        {
            GosLog(LOG_ERROR, "the pack size %d is too small", nSize);
            return NULL;
        }

        ulExtensionLenth = pBuf[14];
        ulExtensionLenth <<= 8;
        ulExtensionLenth += pBuf[15];

        //ulExtensionLenth = ntohs(GET_SHORT(pBuf));

        ulRTPHeadLenth = VIDEO_HEAD_LEN+2+2+ulExtensionLenth*4;

        if (nSize <= ulRTPHeadLenth)
        {
            GosLog(LOG_ERROR, "the pack size %d is too small", nSize);
            return NULL;
        }
    }

    BYTE    *pPayload = pBuf + ulRTPHeadLenth;
    int     iPayloadSize = nSize - ulRTPHeadLenth;
    BYTE    byPayloadType = pPayload[0] & 0x1f;

    if (iPayloadSize < 2)
    {
        GosLog(LOG_ERROR, "the pack size %d is too small", nSize);
        return NULL;
    }

    // 单个NAL单元包
    if ((1 <= byPayloadType) && (23 >= byPayloadType))
    {
        // 添加包头, 如果NALU为数据的开始，则用4字节表示即0x00000001
        pPayload -= 4;
        *((UINT32*)(pPayload)) = htonl(1);
        *outSize = iPayloadSize + 4;
        return pPayload;
    }
    else if (byPayloadType == 28)   // FU_A
    {
        if (pPayload[1] & 0x80)
        {
            /* the first packet of FU-A 
             https://blog.51cto.com/u_13700957/5283016
            1）第一个FU-A包的FU indicator：F应该为当前NALU头的F，而NRI应该为当前NALU头的NRI，Type则等于28，表明它是FU-A包。FU header生成方法：S = 1，E = 0，R = 0，Type则等于NALU头中的Type。
            2）后续的N个FU-A包的FU indicator和第一个是完全一样的，如果不是最后一个包，则FU header应该为：S = 0，E = 0，R = 0，Type等于NALU头中的Type。
            3）最后一个FU-A包FU header应该为：S = 0，E = 1，R = 0，Type等于NALU头中的Type。
            */

            //  计算真实的NAL头, 组包时，NAL unit type 必须自己拼装FU Indicator前三字节+ FU Header后五字节
            CHAR head = (pPayload[0] & 0xe0) | (pPayload[1] & 0x1f);
            pPayload += 1;
            pPayload[0] = head;
            iPayloadSize -= 1;

            // 添加包头, 如果NALU为数据的开始，则用4字节表示即0x00000001
            pPayload -= 4;
            *((UINT32*)(pPayload)) = htonl(1);
            *outSize = iPayloadSize + 4;
            return pPayload;
        }
        else
        {
            //  Not the first packet of FU-A, 后续的N个FU-A包处理, 去掉包头
            pPayload += 2;
            iPayloadSize -= 2;
            *outSize = iPayloadSize;
            return pPayload;
        }
    }
    else
    {
        GosLog(LOG_ERROR, "invalid RTP pack NAL type %u", byPayloadType);
        return NULL;
    }
}

INT32 RecvAudioPacket(int callid, SOCKET sockClient, FILE *fp)
{
    TIMEVAL     tv;
    FD_SET      readfds;
    int         iRet;
    int         iDataLen;
    char        recvBuff[2560];

    FD_ZERO(&readfds);
    FD_SET(sockClient, &readfds);
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    select(sockClient+1, &readfds, NULL, NULL, &tv);

    if (FD_ISSET(sockClient, &readfds))
    {
        memset(recvBuff, 0, sizeof(recvBuff));
        iRet = recv(sockClient, recvBuff, sizeof(recvBuff), 0);
        if (iRet == SOCKET_ERROR )
        {
            GosLog(LOG_ERROR, "recv audio data of call id %u error: %s", callid, gos_get_socket_err());
            return -1;
        }

        if (EWOULDBLOCK == iRet || EAGAIN == iRet)
        {
            //      GosLog(LOG_WARN, "recieve Audio Frame timeout");
            return 0;
        }

        // 接收到的数据没有音频部分
        if (iRet <= AUDIO_HEAD_LEN)
        {
            return 0;
        }

        iDataLen = iRet - AUDIO_HEAD_LEN;
        if (iDataLen != fwrite(recvBuff + AUDIO_HEAD_LEN, 1, iDataLen, fp))
        {
            GosLog(LOG_ERROR, "write rec file failed, call id is %u", callid);
            return -1;
        }

        return iDataLen;
    }

    return 0;
}

VOID RecvAudioData(UINT32 ulCallID, INT32 iLocalPort, CHAR *szFile)
{
    CHAR                *szBusinessName = (CHAR*)"RecvAudioData";
    REC_THREAD_INFO_T   stThreadInfo = {0};
    UINT32              ulRecvTime = gos_get_uptime();
    UINT8               aucLocalAddr[4] = {0,0,0,0};
    SOCKET              stSocket = MrsUDPPortManager::GetInstance().GetSocket(iLocalPort, szBusinessName); // gos_create_udp_server_socket(aucLocalAddr, iLocalPort);
    FILE                *fp = NULL;
    UINT8               aucFileHdr[6];
    INT32               iLen;
    REC_THREAD_INFO_T   *pstThreadInfo;

    GosLog(LOG_INFO, "recv audio data of call id %u at port %d", ulCallID, iLocalPort);

    if (stSocket == INVALID_SOCKET)
    {
        GosLog(LOG_ERROR, "create socket of call id %s failed", ulCallID);
        goto end;
    }

    gos_format_path(szFile);
    fp = fopen(szFile, "wb");
    if (!fp)
    {
        GosLog(LOG_ERROR, "create file %s failed", szFile);
        goto end;
    }

    stThreadInfo.ulThreadID = gos_get_thread_id();
    stThreadInfo.bClosed = FALSE;

    g_MutexRecThread.Mutex();
    g_mAudioRecThread[ulCallID] = stThreadInfo;
    g_MutexRecThread.Unmutex();

    // 写入文件头
    memset(aucFileHdr, 0, sizeof(aucFileHdr));
    memcpy(aucFileHdr, "#!AMR\n", 6);

    iLen = fwrite(aucFileHdr, 1, sizeof(aucFileHdr), fp);
    if (iLen != sizeof(aucFileHdr))
    {
        GosLog(LOG_ERROR, "write file %s failed", szFile);
        goto end;
    }

    GosLog(LOG_INFO, "start rec of call %u", ulCallID);

    while(1)
    {
        g_MutexRecThread.Mutex();

        if (g_mAudioRecThread.find(ulCallID) != g_mAudioRecThread.end())
        {
            pstThreadInfo = &g_mAudioRecThread[ulCallID];

            if (pstThreadInfo->bClosed)
            {
                g_MutexRecThread.Unmutex();

                break;
            }
        }

        g_MutexRecThread.Unmutex();

        // recv
        iLen = RecvAudioPacket(ulCallID, stSocket, fp);
        if (iLen < 0)
        {
            goto end;
        }

        if (iLen > 0)
        {
            ulRecvTime = gos_get_uptime();
        }

        if ((gos_get_uptime() - ulRecvTime) > g_ulRecvIdleTime)
        {
            goto end;
        }
    }

end:
    g_MutexRecThread.Mutex();

    GosLog(LOG_INFO, "stop rec of call %u", ulCallID);

    if (fp)
    {
        fclose(fp);
    }

    RenameTmpFile(szFile);
    CLOSE_SOCKET(stSocket);

    if (g_mAudioRecThread.find(ulCallID) != g_mAudioRecThread.end())
    {
        g_mAudioRecThread.erase(ulCallID);
    }

    g_MutexRecThread.Unmutex();

    MrsUDPPortManager::GetInstance().FreeUDPPort(iLocalPort);///FreeUDPPort(iLocalPort);

    GosLog(LOG_INFO, "free local port %u", iLocalPort);
}
///------------------------------------------------------------------------------------------
INT32 RecvP2PAudioPacket(UINT32 ulCallID, SOCKET stSock, RTP_DATA_T &stData) //FILE *fp)
{
    INT32   iRet;
    UINT8   aucRecvBuf[1024];
    SOCKADDR_IN     stAddr;

    iRet = gos_recv_udp_msg(stSock, aucRecvBuf, sizeof(aucRecvBuf), 1000, &stAddr);
    if (iRet == 0)
    {
        return iRet;
    }

    if (iRet < 0)
    {
        GosLog(LOG_ERROR, "recv callid %u faied, %s", ulCallID, gos_get_socket_err());
        return -1;
    }

    INT32   iDataLen = iRet - AUDIO_HEAD_LEN;

    if (iDataLen < 0)
    {
        GosLog(LOG_ERROR, "invalid RTP length(%d) of call id %u", iRet, ulCallID);
        return -1;
    }

    if ((aucRecvBuf[1] & 0x7f) != 0x72) //
    {
        GosLog(LOG_INFO, "RTP payload (%d) of call id %u", aucRecvBuf[1], ulCallID);
        return 0;
    }

    if (iDataLen > sizeof(stData.aucData))
    {
        GosLog(LOG_ERROR, "RTP length(%d) of call id %u is too large", iRet, ulCallID);
        return -1;
    }

    if (iDataLen <= 1)
    {
        return 0;
    }


    UINT8   aucFrameData[1024] = {0};
    UINT32  ulFrameLen = 0;

    if (!ReadAmrFramData(aucRecvBuf+AUDIO_HEAD_LEN, iDataLen, aucFrameData, sizeof(aucFrameData), ulFrameLen))
    {
        return -1;
    }

    stData.ulSSRC = GET_INT(aucRecvBuf + 8);
    stData.ulLen = ulFrameLen;
    memcpy(stData.aucData, aucFrameData, ulFrameLen);

    return ulFrameLen;
}

VOID RecvP2PAudioData(UINT32 ulCallID, INT32 iLocalPort, CHAR *szFile)
{
    CHAR                *szBusiness = (CHAR*)"RecvP2PAudioData";
    REC_THREAD_INFO_T   stThreadInfo = {0};
    UINT32              ulRecvTime = gos_get_uptime();
    UINT8               aucLocalAddr[4] = {0,0,0,0};
    SOCKET              stSocket = MrsUDPPortManager::GetInstance().GetSocket(iLocalPort, szBusiness); //gos_create_udp_server_socket(aucLocalAddr, iLocalPort);
    FILE                *fp1 = NULL;
    FILE                *fp2 = NULL;
    UINT8               aucFileHdr[6];
    INT32               iLen;
    REC_THREAD_INFO_T   *pstThreadInfo;
    UINT32              ulFirstSSRC = 0;
    RTP_DATA_T          stData;
//    CHAR                acFile[256];
    CHAR                acFile1[256];
    CHAR                acFile2[256];

    GosLog(LOG_INFO, "recv audio data of call id %u at port %d", ulCallID, iLocalPort);

    if (stSocket == INVALID_SOCKET)
    {
        GosLog(LOG_ERROR, "create socket of call id %s failed", ulCallID);
        goto end;
    }

    sprintf(acFile1, "%s/P2Ptmp.1.amr", gos_get_root_path());
    sprintf(acFile2, "%s/P2Ptmp.2.amr", gos_get_root_path());
    gos_format_path(acFile1);
    gos_format_path(acFile2);

    fp1 = fopen(acFile1, "wb");
    if (!fp1)
    {
        GosLog(LOG_ERROR, "create file %s failed", acFile1);
        goto end;
    }

    fp2 = fopen(acFile2, "wb");
    if (!fp2)
    {
        fclose(fp1);
        gos_delete_file(acFile1);
        GosLog(LOG_ERROR, "create file %s failed", acFile2);
        goto end;
    }

    stThreadInfo.ulThreadID = gos_get_thread_id();
    stThreadInfo.bClosed = FALSE;

    g_MutexRecThread.Mutex();
    g_mAudioRecThread[ulCallID] = stThreadInfo;
    g_MutexRecThread.Unmutex();

    // 写入文件头
    memset(aucFileHdr, 0, sizeof(aucFileHdr));
    memcpy(aucFileHdr, "#!AMR\n", 6);

    iLen = fwrite(aucFileHdr, 1, sizeof(aucFileHdr), fp1);
    if (iLen != sizeof(aucFileHdr))
    {
        GosLog(LOG_ERROR, "write file %s.1 failed", acFile1);
        goto end;
    }

    iLen = fwrite(aucFileHdr, 1, sizeof(aucFileHdr), fp2);
    if (iLen != sizeof(aucFileHdr))
    {
        GosLog(LOG_ERROR, "write file %s.2 failed", acFile2);
        goto end;
    }

    GosLog(LOG_INFO, "start rec of call %u", ulCallID);

    while(1)
    {
        g_MutexRecThread.Mutex();

        if (g_mAudioRecThread.find(ulCallID) != g_mAudioRecThread.end())
        {
            pstThreadInfo = &g_mAudioRecThread[ulCallID];

            if (pstThreadInfo->bClosed)
            {
                g_MutexRecThread.Unmutex();

                break;
            }
        }

        g_MutexRecThread.Unmutex();

        memset(&stData, 0 ,sizeof(stData));

        // recv
        iLen = RecvP2PAudioPacket(ulCallID, stSocket, stData);//fp);
        if (iLen < 0)
        {
            GosLog(LOG_ERROR, "recv called %u failed", ulCallID);
            goto end;
        }

        if (iLen > 0)
        {
            ulRecvTime = gos_get_uptime();
        }

        if ((gos_get_uptime() - ulRecvTime) > g_ulRecvIdleTime)
        {
            GosLog(LOG_ERROR, "recv called %u timeout", ulCallID);
            goto end;
        }

        if (ulFirstSSRC == 0)
        {
            ulFirstSSRC = stData.ulSSRC;
        }

        if (stData.ulLen > 0)
        {
            if (ulFirstSSRC == stData.ulSSRC)
            {
                iLen = fwrite(stData.aucData, 1, stData.ulLen, fp1);
            }
            else
            {
                iLen = fwrite(stData.aucData, 1, stData.ulLen, fp2);
            }

            if (iLen != stData.ulLen)
            {
                GosLog(LOG_ERROR, "write RTP data to file %s failed", szFile);
                goto end;
            }
        }
    }

end:
    g_MutexRecThread.Mutex();

    GosLog(LOG_INFO, "stop rec of call %u", ulCallID);

    if (fp1)
    {
        fclose(fp1);
    }

    if (fp2)
    {
        fclose(fp2);
    }

 ///   RenameTmpFile(szFile);
//  CLOSE_SOCKET(stSocket);

    MixAudioFile(acFile1, acFile2, szFile);//将接收到的两个单声道文件混音成一个文件
    gos_delete_file(acFile1);
    gos_delete_file(acFile2);

    if (g_mAudioRecThread.find(ulCallID) != g_mAudioRecThread.end())
    {
        g_mAudioRecThread.erase(ulCallID);
    }

    g_MutexRecThread.Unmutex();

    MrsUDPPortManager::GetInstance().FreeUDPPort(iLocalPort); ///FreeUDPPort(iLocalPort);

    return ;
}
///---------------------------------------------------------------------------------------
GOS_THREAD_RET RecvP2PVideoCallAudio(VOID *pvData)
{
    if (!pvData)
    {
        return FALSE;
    }

    P2P_CALL_STATUS_INFO_T *pstCallStatus = (P2P_CALL_STATUS_INFO_T*)pvData;

    //CHAR    acTime[32];
    CHAR    acFile[256];

    //gos_get_text_time_ex(NULL, "%04u%02u%02u%02u%02u%02u", acTime);

    sprintf(acFile, "%s/%u_%s_%u_%u.amr.tmp", g_stIniCfg.acRecTmpPath, REC_CALL_TYPE_P2P_VIDEO, pstCallStatus->acTime, pstCallStatus->ulCaller, pstCallStatus->ulCallee);

    RecvAudioData(pstCallStatus->ulCallID, pstCallStatus->iLocalPort, acFile);

    if (gos_get_file_size(acFile) <= 0)
    {
        gos_delete_file(acFile);
    }

    free(pvData);

    return 0;
}

GOS_THREAD_RET RecvP2PCallAudio(VOID *pvData)
{
    if (!pvData)
    {
        return FALSE;
    }

    P2P_CALL_STATUS_INFO_T *pstCallStatus = (P2P_CALL_STATUS_INFO_T*)pvData;

    CHAR    acTime[32];
    CHAR    acFile[256];

    gos_get_text_time_ex(NULL, "%04u%02u%02u%02u%02u%02u", acTime);

    sprintf(acFile, "%s/%u_%s_%u_%u.amr", g_stIniCfg.acRecTmpPath, REC_CALL_TYPE_P2P_AUDIO, acTime, pstCallStatus->ulCaller, pstCallStatus->ulCallee);

    RecvP2PAudioData(pstCallStatus->ulCallID, pstCallStatus->iLocalPort, acFile);

    free(pvData);

    return 0;
}

GOS_THREAD_RET RecvGroupCallAudio(VOID *pvData)
{
    if (!pvData)
    {
        return FALSE;
    }

    GROUP_CALL_STATUS_INFO_T *pstCallStatus = (GROUP_CALL_STATUS_INFO_T*)pvData;

    CHAR        acTime[32];
    CHAR        acFile[256];

    gos_get_text_time_ex(NULL, "%04u%02u%02u%02u%02u%02u", acTime);

    sprintf(acFile, "%s/%u_%s_" FORMAT_I64 ".amr.tmp", g_stIniCfg.acRecTmpPath, REC_CALL_TYPE_PTT_AUDIO, acTime, pstCallStatus->i64GroupID);

    RecvAudioData(pstCallStatus->ulCallID, pstCallStatus->iLocalPort, acFile);

    free(pvData);

    return 0;
}

INT32 RecvVideoPacket(UINT32 ulCallID, SOCKET sockClient, RTP_DATA_T &stData)
{
    TIMEVAL     tv;
    FD_SET      readfds;
    int         iRet;
    char        recvBuff[MAX_H264PACK_LEN];
    INT32       iFrameLen = 0;
    UINT8       *pucFrame;

    FD_ZERO(&readfds);
    FD_SET(sockClient, &readfds);
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    select(sockClient+1, &readfds, NULL, NULL, &tv);

    if (FD_ISSET(sockClient, &readfds))
    {
        memset(recvBuff, 0, sizeof(recvBuff));
        iRet = recv(sockClient, recvBuff, sizeof(recvBuff), 0);
        if (iRet == SOCKET_ERROR )
        {
            GosLog(LOG_ERROR, "recv video data of call id %u error: %s", ulCallID, gos_get_socket_err());
            return -1;
        }

        if (EWOULDBLOCK == iRet || EAGAIN == iRet)
        {
          //  GosLog(LOG_WARN, "recieve Video Frame timeout");
            return 0;
        }

        // 接收到的数据没有音频部分
        if (iRet <= VIDEO_HEAD_LEN)
        {
            return 0;
        }

        iFrameLen = 0;
        pucFrame = Parse_RTP_Packet((BYTE*)recvBuff, iRet, &iFrameLen, stData.ulSSRC);
        stData.ulLen = iFrameLen;

        if (pucFrame && iFrameLen > 0)
        {
            memcpy(stData.aucData, pucFrame, iFrameLen);
        }

        return 1;
    }

    return 0;
}

INT32 RecvVideoPacket(int callid, SOCKET sockClient, FILE *fp)
{
    TIMEVAL     tv;
    FD_SET      readfds;
    int         iRet;
    char        recvBuff[MAX_H264PACK_LEN];
    INT32       iFrameLen = 0;
    UINT8       *pucFrame;
    UINT32      ulSSRC;

    FD_ZERO(&readfds);
    FD_SET(sockClient, &readfds);
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    select(sockClient+1, &readfds, NULL, NULL, &tv);

    if (FD_ISSET(sockClient, &readfds))
    {
        memset(recvBuff, 0, sizeof(recvBuff));
        iRet = recv(sockClient, recvBuff, sizeof(recvBuff), 0);
        if (iRet == SOCKET_ERROR )
        {
            GosLog(LOG_ERROR, "recv video data of call id %u error: %s", callid, gos_get_socket_err());
            return -1;
        }

        if (EWOULDBLOCK == iRet || EAGAIN == iRet)
        {
            //      GosLog(LOG_WARN, "recieve Audio Frame timeout");
            return 0;
        }

        // 接收到的数据没有音频部分
        if (iRet <= VIDEO_HEAD_LEN)
        {
            return 0;
        }

        iFrameLen = 0;
        pucFrame = Parse_RTP_Packet((BYTE*)recvBuff, iRet, &iFrameLen, ulSSRC);
        if (pucFrame && iFrameLen > 0)
        {
            if (iFrameLen != fwrite(pucFrame, 1, iFrameLen, fp))
            {
                GosLog(LOG_ERROR, "write rec file failed, call id is %u", callid);
                return -1;
            }
        }

        return 1;
    }

    return 0;
}

VOID RecvVideoData(UINT32 ulCallID, INT32 iLocalPort, CHAR *szFile)
{
    CHAR                *szBusiness = (CHAR*)"RecvVideoData";
    REC_THREAD_INFO_T   stThreadInfo = {0};
    UINT8               aucLocalAddr[4] = {0,0,0,0};
    SOCKET              stSocket = MrsUDPPortManager::GetInstance().GetSocket(iLocalPort, szBusiness); //gos_create_udp_server_socket(aucLocalAddr, iLocalPort);
    FILE                *fp = NULL;
    INT32               iLen;
    UINT32              ulRecvTime = gos_get_uptime();
    REC_THREAD_INFO_T   *pstThreadInfo;

    GosLog(LOG_INFO, "recv video data of call id %u at port %d", ulCallID, iLocalPort);

    if (stSocket == INVALID_SOCKET)
    {
        GosLog(LOG_ERROR, "create socket of call id %s failed", ulCallID);
        goto end;
    }

    gos_format_path(szFile);
    fp = fopen(szFile, "wb");
    if (!fp)
    {
        GosLog(LOG_ERROR, "create file %s failed", szFile);
        goto end;
    }

    GosLog(LOG_INFO, "start video rec of %u", ulCallID);

    stThreadInfo.ulThreadID = gos_get_thread_id();
    stThreadInfo.bClosed = FALSE;

    g_MutexRecThread.Mutex();
    g_mVideoRecThread[ulCallID] = stThreadInfo;
    g_MutexRecThread.Unmutex();

    while(1)
    {
        g_MutexRecThread.Mutex();

        if (g_mVideoRecThread.find(ulCallID) != g_mVideoRecThread.end())
        {
            pstThreadInfo = &g_mVideoRecThread[ulCallID];

            if (pstThreadInfo->bClosed) //关闭file文件和socket流
            {
                g_MutexRecThread.Unmutex();
                goto end;
            }
        }

        g_MutexRecThread.Unmutex();

        // recv
        iLen = RecvVideoPacket(ulCallID, stSocket, fp);
        if (iLen < 0)
        {
            goto end;
        }

        if (iLen > 0)
        {
            ulRecvTime = gos_get_uptime();
        }

        if ((gos_get_uptime() - ulRecvTime) > g_ulRecvIdleTime)
        {
            goto end;
        }
    }

end:
    g_MutexRecThread.Mutex();

    GosLog(LOG_INFO, "stop video rec of %u", ulCallID);

    if (fp)
    {
        fclose(fp);
    }

    RenameTmpFile(szFile);
    CLOSE_SOCKET(stSocket);

    if (g_mVideoRecThread.find(ulCallID) != g_mVideoRecThread.end())
    {
        g_mVideoRecThread.erase(ulCallID);
    }

    g_MutexRecThread.Unmutex();

    MrsUDPPortManager::GetInstance().FreeUDPPort(iLocalPort);//FreeUDPPort(iLocalPort);
}

GOS_THREAD_RET RecvP2PVideoCallVideo(VOID *pvData)
{
    if (!pvData)
    {
        return FALSE;
    }

    P2P_CALL_STATUS_INFO_T *pstCallStatus = (P2P_CALL_STATUS_INFO_T*)pvData;

//  RECP2pvideocallStatusIndicator *pstCallStatus = (RECP2pvideocallStatusIndicator*)pvData;

    //CHAR    acTime[32];
    CHAR    acFile[256];

    //gos_get_text_time_ex(NULL, "%04u%02u%02u%02u%02u%02u", acTime);
    sprintf(acFile, "%s/%u_%s_%u_%u.h264.tmp", g_stIniCfg.acRecTmpPath, REC_CALL_TYPE_P2P_VIDEO, pstCallStatus->acTime, pstCallStatus->ulCaller, pstCallStatus->ulCallee);
    //sprintf(acFile, "%s/%u_%s_%u_%u", g_stIniCfg.acRecTmpPath, REC_CALL_TYPE_P2P_VIDEO, pstCallStatus->acTime, pstCallStatus->ulCaller, pstCallStatus->ulCallee);

    RecvVideoData(pstCallStatus->ulCallID, pstCallStatus->iLocalPort, acFile);

    free(pvData);

    return 0;
}

VOID CloseRecAudioThread(UINT32 ulCallID)
{
    g_MutexRecThread.Mutex();

    if (g_mAudioRecThread.find(ulCallID) != g_mAudioRecThread.end())
    {
        g_mAudioRecThread[ulCallID].bClosed = TRUE;
    }

    g_MutexRecThread.Unmutex();

}

VOID CloseRecVideoThread(UINT32 ulCallID)
{
    g_MutexRecThread.Mutex();

    if (g_mVideoRecThread.find(ulCallID) != g_mVideoRecThread.end())
    {
        g_mVideoRecThread[ulCallID].bClosed = TRUE;
    }

    g_MutexRecThread.Unmutex();
}

VOID CheckRecThread()
{
    g_MutexRecThread.Mutex();

    for (std::map<UINT32, REC_THREAD_INFO_T>::iterator it = g_mAudioRecThread.begin();
         it!=g_mAudioRecThread.end(); it++)
    {
    }

    g_MutexRecThread.Unmutex();

}
