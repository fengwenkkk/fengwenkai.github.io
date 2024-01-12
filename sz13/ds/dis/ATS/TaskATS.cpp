#include "DISPublic.h"
#include "ats.h"
#include "TaskATS.h"

#define TIMER_GET_ATS           TIMER11
#define EV_TIMER_GET_ATS_IND    GET_TIMER_EV(TIMER_GET_ATS)

UINT16  g_usATSLineID = 0xffff;
BOOL    g_bATSUseNetOrder = FALSE;      // ATS数据是否采用网络字节序发送

UINT32      g_ulATSMsgNum = 0;
INT64       g_i64ATSMsgSize = 0;
UINT32      g_ulATSLinkCheckPeriod = 3; //如果3秒钟未收到ATS的消息，则断开连接

extern VOID SetATSValid(BOOL bValid);
extern BOOL CharArrayToInt(CHAR *szText, UINT32 ulLen, UINT32 *pulValue);

UINT16 ATSNtohs(UINT16 usValue)
{
    if (g_bATSUseNetOrder)
    {
        return ntohs(usValue);
    }
    else
    {
        UINT8   *pucValue = (UINT8*)&usValue;

        usValue = (pucValue[1]<<8) | pucValue[0]; // windows字节序

        return usValue;
    }
}

UINT32 ATSNtohl(UINT32 ulValue)
{
    if (g_bATSUseNetOrder)
    {
        return ntohl(ulValue);
    }
    else
    {
        UINT8   *pucValue = (UINT8*)&ulValue;

        ulValue = (pucValue[3] << 24) | (pucValue[2] << 16) |
                  (pucValue[1] << 8 ) | pucValue[0];     // windows字节序
        return ulValue;
    }
}

UINT16 ATSHtons(UINT16 usValue)
{
    if (g_bATSUseNetOrder)
    {
        return htons(usValue);
    }
    else
    {
        UINT8   aucValue[2];

        aucValue[0] = (UINT8)usValue;
        aucValue[1] = usValue>>8;

        return GET_SHORT(aucValue); // windows字节序
    }
}

UINT32 ATSHtonl(UINT32 ulValue)
{
    if (g_bATSUseNetOrder)
    {
        return htonl(ulValue);
    }
    else
    {
        UINT8   aucValue[4];

        aucValue[0] = (UINT8)ulValue;
        aucValue[1] = (UINT8)(ulValue>>8);
        aucValue[2] = (UINT8)(ulValue>>16);
        aucValue[3] = (UINT8)(ulValue>>24);

        return GET_INT(aucValue); // windows字节序
    }
}

/*
void test_ATS()
{
    //UINT16  usValue = 0x0102;
    UINT8       aucValue[2] = {0x02, 0x01};
    UINT8       aucValue2[2] = {0x02, 0x01};
    UINT16      usValue = 0;

    memcpy(&usValue, aucValue, sizeof(aucValue));

    UINT16      usHost = ATSNtohs(usValue);
    UINT16      usNet = ATSHtons(usHost);

    memcpy(aucValue2, &usNet, sizeof(aucValue2));

    printf("ori_byte = 0x%02u%02u, host = %u, net = %u, new_byte=0x%02u%02u\n",
            aucValue[0], aucValue[1], usHost, usNet, aucValue2[0], aucValue2[1]);

    return;
}*/

void ATSInfoToHost(ATS_INFO_T *pstATSInfo)
{
    pstATSInfo->usRtuID             = ATSNtohs(pstATSInfo->usRtuID);
    pstATSInfo->usStationID         = ATSNtohs(pstATSInfo->usStationID);
    pstATSInfo->usUpsideStationID   = ATSNtohs(pstATSInfo->usUpsideStationID);
    pstATSInfo->usDownsideStationID = ATSNtohs(pstATSInfo->usDownsideStationID);
    pstATSInfo->usLogicSectionID    = ATSNtohs(pstATSInfo->usLogicSectionID);
    pstATSInfo->usPhysicSectionID   = ATSNtohs(pstATSInfo->usPhysicSectionID);
    pstATSInfo->usDriverNumber      = ATSNtohs(pstATSInfo->usDriverNumber);
    pstATSInfo->iOtpTime            = ATSNtohl(pstATSInfo->iOtpTime);
    pstATSInfo->stArriveTime.usYear = ATSNtohs(pstATSInfo->stArriveTime.usYear);
    pstATSInfo->stDepartTime.usYear = ATSNtohs(pstATSInfo->stDepartTime.usYear);
}

TaskATS::TaskATS(UINT16 usDispatchID):GBaseTask(MODULE_ATS, "ATS", usDispatchID, FALSE)
{
    m_pDao = NULL;
    m_ulValidAddrIndex = 1;
    m_acZipFile[0] = '\0';
    m_acFile[0] = '\0';

    m_szFilePrefix = (CHAR*)"ats_";
    m_fp = NULL;
    memset(&m_stLastFileTime, 0, sizeof(m_stLastFileTime));
}

BOOL TaskATS::Init()
{
    if (!m_pDao)
    {
        m_pDao = GetCfgDao();
        if (!m_pDao)
        {
            TaskLog(LOG_ERROR, "get db dao failed");

            return FALSE;
        }
    }

    SetTaskStatus(TASK_STATUS_WORK);

    //SetLoopTimer(TIMER_GET_ATS, g_ulGetATSTime*1000);
    SetTimer(TIMER_GET_ATS, 100);

    TaskLog(LOG_INFO, "Task ATS init successful!");

    return TRUE;
}

VOID TaskATS::OnServerDisconnectClient(UINT16 usClientID)
{
}

VOID TaskATS::RemoveATSFile()
{
    CHAR    acDir[512];

    sprintf(acDir, "%s/%s", gos_get_root_path(), "data");

    RemoveFile(acDir, m_szFilePrefix, GetLocalCfg().ulMaxATSFileNum);
}

VOID TaskATS::CheckFile()
{
    UINT32          ulTime = gos_get_current_time();
    GOS_DATETIME_T  stCurrTime;
    CHAR            acTime[32];
    CHAR            acDir[512];
    CHAR            acFile[512] = {0};

    sprintf(acDir, "%s/%s", gos_get_root_path(), "data");
    if (!gos_file_exist(acDir))
    {
        gos_create_redir(acDir);
    }

    gos_get_localtime(&ulTime, &stCurrTime);
    if (stCurrTime.usYear  != m_stLastFileTime.usYear  ||
        stCurrTime.ucMonth != m_stLastFileTime.ucMonth ||
        stCurrTime.ucDay   != m_stLastFileTime.ucDay )
    {
        if (m_fp)
        {
            fclose(m_fp);
            m_fp = NULL;

            // 压缩
            CHAR    acCmd[1024];

            if (m_acZipFile)
            {
#ifdef _OSWIN32_
                sprintf(acCmd, "rar a %s data\\%s", m_acZipFile, m_acFile);
#else
                sprintf(acCmd, "tar -czvf %s -C %s %s", m_acZipFile, acDir, m_acFile);
#endif
                gos_system(acCmd);

                if (gos_file_exist(m_acZipFile))
                {
                    sprintf(acFile, "%s/%s", acDir, m_acFile);
                    gos_delete_file(acFile);
                }
            }
        }

        RemoveATSFile();
    }

    if (m_fp)
    {
        return;
    }

    gos_get_text_time_ex(&ulTime, "%04u%02u%02u", acTime);
    sprintf(acFile, "%s/%s%s.dat", acDir, m_szFilePrefix, acTime);

    if (gos_file_exist(acFile))
    {
        m_fp = fopen(acFile, "a+b");
    }
    else
    {
        m_fp = fopen(acFile, "w+b");
    }

    if (m_fp)
    {
        memcpy(&m_stLastFileTime, &stCurrTime, sizeof(stCurrTime));

        sprintf(m_acFile, "%s%s.dat", m_szFilePrefix, acTime);
        sprintf(m_acZipFile, "%s/%s%s.tar.gz", acDir, m_szFilePrefix, acTime);
    }
}

VOID TaskATS::SaveATSToFile(ATS_REC_T *pstATSRec)
{
    UINT32  ulSpareTrainNum = ARRAY_SIZE(pstATSRec->stRecData.astATSInfo) - pstATSRec->stRecData.ucTrainNum;
    UINT32  ulRecLen = sizeof(ATS_REC_T) - ulSpareTrainNum*sizeof(pstATSRec->stRecData.astATSInfo[0]);

    CheckFile();

    HTONL_IT(pstATSRec->ulTime);
    HTONS_IT(pstATSRec->stRecData.stMsgHdr.usMsgLen);
    HTONS_IT(pstATSRec->stRecData.stMsgHdr.stMsgTime.usYear);
    HTONS_IT(pstATSRec->stRecData.stMsgHdr.usLineID);

    if (ulRecLen != fwrite(pstATSRec, 1, ulRecLen, m_fp))
    {
        TaskLog(LOG_ERROR, "save ATS info to file failed");
    }

    fflush(m_fp);
}

BOOL TaskATS::SendHeartBeatMsg(SOCKET  stSock)
{
    UINT8               aucMsg[sizeof(ATS_FRAME_HDR_T)+sizeof(ATS_MSG_HEARTBEAT_T)+sizeof(UINT16)];
    ATS_FRAME_HDR_T     *pstFrameHdr = (ATS_FRAME_HDR_T*)aucMsg;
    ATS_MSG_HEARTBEAT_T *pstMsg = (ATS_MSG_HEARTBEAT_T*)(pstFrameHdr+1);
    GOS_DATETIME_T      stTime;
    UINT16              *pusTail = (UINT16*)(pstMsg+1);

    gos_get_localtime(NULL, &stTime);
    memset(aucMsg, 0, sizeof(aucMsg));

    pstFrameHdr->usFrameHead  = ATSHtons(ATS_FRAME_HEAD);
    pstFrameHdr->ucFrameCount = 1;
    pstFrameHdr->ucFrameIndex = 1;
    pstFrameHdr->usDataLen = ATSHtons(sizeof(ATS_MSG_HEARTBEAT_T));

    pstMsg->stMsgHdr.usMsgLen = ATSHtons(sizeof(ATS_MSG_HEARTBEAT_T)-sizeof(UINT16));
    pstMsg->stMsgHdr.stMsgTime.usYear = ATSHtons(stTime.usYear);
    pstMsg->stMsgHdr.stMsgTime.ucMonth = stTime.ucMonth;
    pstMsg->stMsgHdr.stMsgTime.ucDay = stTime.ucDay;
    pstMsg->stMsgHdr.stMsgTime.ucHour = stTime.ucHour;
    pstMsg->stMsgHdr.stMsgTime.ucMin = stTime.ucMinute;
    pstMsg->stMsgHdr.stMsgTime.ucSec = stTime.ucSecond;

    pstMsg->stMsgHdr.usLineID = ATSHtons(g_usATSLineID);
    pstMsg->stMsgHdr.ucMsgID = ATS_MSGID_HEARTBEAT;
    pstMsg->stMsgHdr.ucVer = ATS_VER;

    *pusTail = ATSHtons(ATS_FRAME_TAIL);

    return gos_tcp_send(stSock, aucMsg, (sizeof(ATS_FRAME_HDR_T)+sizeof(ATS_MSG_HEARTBEAT_T)+sizeof(UINT16)));
}

VOID TaskATS::OnTimerGetATSInd()
{
    while(1)
    {
        if (!GetATSInfo())
        {
            gos_sleep_1ms(200);
        }
    }
}

BOOL TaskATS::GetATSInfo()
{
    LOCAL_CFG_T         &stLocalCfg = GetLocalCfg();
    UINT8               *pucAddr = stLocalCfg.astATSAddr[m_ulValidAddrIndex].aucIP;
    UINT16              usPort = stLocalCfg.astATSAddr[m_ulValidAddrIndex].usPort;
    static SOCKET       stSock = INVALID_SOCKET;
    UINT32              ulTimeout = 1000;
    INT32               iRet;
    ATS_FRAME_HDR_T     *pstFrameHdr = (ATS_FRAME_HDR_T*)AllocBuffer(sizeof(ATS_FRAME_HDR_T)+64*1024);
    UINT8               *pucMsgData = (UINT8*)(pstFrameHdr+1);
    UINT16              usTail;
 //   UINT32              ulMaxTimeoutNum = 3;
   // static UINT32       ulTimeoutNum = 0;
    UINT32              ulATSServerNum = (stLocalCfg.astATSAddr[1].usPort == 0)?1:2;
    UINT32              ulFrameLen;
    UINT32              ulCurrTime = 0;
    static UINT32       ulLastSendHeartBeatTime = 0;
//    static UINT32       ulLastPrintTime = 0;
//    UINT32              ulByteToRecved = 0;
    static UINT32       ulLastValidRecvTime = 0;

start:
    if (stSock == INVALID_SOCKET)
    {
        m_ulFrameNum = 0;
        m_ulLastFrameIndex = 0;
        m_ulRecvMsgLen = 0;

   //     ulTimeoutNum = 0;
        if (ulATSServerNum == 1)
        {
            m_ulValidAddrIndex = 0;
        }
        else
        {
            m_ulValidAddrIndex = 1-m_ulValidAddrIndex;
        }

        pucAddr = stLocalCfg.astATSAddr[m_ulValidAddrIndex].aucIP;
        usPort = stLocalCfg.astATSAddr[m_ulValidAddrIndex].usPort;
        stSock = gos_connect_tcp_server(pucAddr, usPort);
        ulLastValidRecvTime = gos_get_uptime_1ms();

        if (stSock == INVALID_SOCKET)
        {
            TaskMsgLog(LOG_ERROR, "connect ATS " IP_FMT " port(%u) failed", IP_ARG(pucAddr), usPort);
        }
        else
        {
            ulLastSendHeartBeatTime = gos_get_uptime_1ms();
            TaskMsgLog(LOG_INFO, "connect ATS " IP_FMT " port(%u) succ", IP_ARG(pucAddr), usPort);
        }
    }

    if (stSock == INVALID_SOCKET)
    {
        if (ulATSServerNum == 1)
        {
            SetATSValid(FALSE);

            // 连接失败需要等待时间长一点
            gos_sleep(1);

            return TRUE;
        }

        m_ulValidAddrIndex = 1-m_ulValidAddrIndex;
        pucAddr = stLocalCfg.astATSAddr[m_ulValidAddrIndex].aucIP;
        usPort = stLocalCfg.astATSAddr[m_ulValidAddrIndex].usPort;
        stSock = gos_connect_tcp_server(pucAddr, usPort);
        ulLastValidRecvTime = gos_get_uptime_1ms();

        if (stSock == INVALID_SOCKET)
        {
            SetATSValid(FALSE);

            TaskMsgLog(LOG_ERROR, "connect ATS " IP_FMT " port(%u) failed", IP_ARG(pucAddr), usPort);

            // 连接失败需要等待时间长一点
            gos_sleep(1);

            return TRUE;
        }
        else
        {
            ulLastSendHeartBeatTime = gos_get_uptime_1ms();
            TaskMsgLog(LOG_INFO, "connect ATS " IP_FMT " port(%u) succ", IP_ARG(pucAddr), usPort);
        }
    }

    ulCurrTime = gos_get_uptime_1ms();
    if ((ulCurrTime - ulLastSendHeartBeatTime) >= 1000)
    {
        if (!SendHeartBeatMsg(stSock))
        {
            /// 发送数据失败时关闭socket重新发起连接
            TaskMsgLog(LOG_ERROR, "send heart beat failed");
            goto err;
        }

        ulLastSendHeartBeatTime = ulCurrTime;
    }

#if 0
    /* 看看有多少字节可读 */
    if (ioctlsocket (stSock, FIONREAD, (unsigned long*)&ulByteToRecved) == SOCKET_ERROR)
    {
        GosLog(LOG_ERROR, "server_recv_msg: ioctl socket(%u) failed!", stSock);
        return FALSE;
    }

    /* 如果没有字节可读, 则对端已经关闭连接, 返回错误 */
    if (ulByteToRecved == 0)
    {
        // 连续3秒没有收到数据断开连接（主要的保活检测手段）
        if ((gos_get_uptime_1ms() - ulLastValidRecvTime) > 3000)
        {
            GosLog(LOG_ERROR, "socket(%u) have not received any data for 3 seconds", stSock);
            goto err;
        }

        GosLog(LOG_ERROR, "socket(%u) have no data", stSock);

        return FALSE;
    }

    ulLastValidRecvTime = gos_get_uptime_1ms();

#endif

    // 接收消息头
    iRet = gos_recv_tcp_msg(stSock, pstFrameHdr, sizeof(ATS_FRAME_HDR_T), ulTimeout);
    if (iRet < 0)
    {
        TaskMsgLog(LOG_ERROR, "recv ATS of " IP_FMT " info failed: %s", IP_ARG(pucAddr), gos_get_socket_err());

        goto err;
    }
/*    else if (iRet == 0)
    {
        ulTimeoutNum++;
        if (ulTimeoutNum >= ulMaxTimeoutNum)
        {
            ulTimeoutNum = 0;
            TaskMsgLog(LOG_ERROR, "recv ATS info timeout %u", ulTimeoutNum);
            goto err;
        }

        return FALSE;
    }*/

    else if (iRet == 0)
    {
        // 连续3秒没有收到数据断开连接（主要的保活检测手段）
        if ((gos_get_uptime_1ms() - ulLastValidRecvTime) > g_ulATSLinkCheckPeriod*1000)
        {
            GosLog(LOG_ERROR, "socket(%u) have not received any data for %u seconds", stSock, g_ulATSLinkCheckPeriod);
            goto err;
        }

        return FALSE;
    }
    else
    {
        ulLastValidRecvTime = gos_get_uptime_1ms();
    }

//    ulTimeoutNum = 0;
    pstFrameHdr->usFrameHead = ATSNtohs(pstFrameHdr->usFrameHead);
    pstFrameHdr->usDataLen = ATSNtohs(pstFrameHdr->usDataLen);

    if (pstFrameHdr->usFrameHead != ATS_FRAME_HEAD)
    {
        TaskMsgLog(LOG_ERROR, "recv ATS info failed, invalid frame head 0x%04X", pstFrameHdr->usFrameHead);
        goto err;
    }

    // 接收数据部分和帧尾
    iRet = gos_recv_tcp_msg(stSock, pucMsgData, pstFrameHdr->usDataLen+sizeof(UINT16), ulTimeout);
    if (iRet < 0)
    {
        TaskMsgLog(LOG_ERROR, "recv ATS of " IP_FMT " info failed: %s", IP_ARG(pucAddr), gos_get_socket_err());
        goto err;
    }
    else if (iRet == 0)
    {/*
        TaskMsgLog(LOG_ERROR, "recv ATS info timeout");
        goto err; */

        // 连续3秒没有收到数据断开连接（主要的保活检测手段）
        if ((gos_get_uptime_1ms() - ulLastValidRecvTime) > g_ulATSLinkCheckPeriod*1000)
        {
            GosLog(LOG_ERROR, "socket(%u) have not received any data for %u seconds", stSock, g_ulATSLinkCheckPeriod);
            goto err;
        }

        return FALSE;
    }
    else
    {
        ulLastValidRecvTime = gos_get_uptime_1ms();
    }

    if ((UINT32)iRet != (pstFrameHdr->usDataLen+sizeof(UINT16)))
    {
        TaskMsgLog(LOG_ERROR, "invalid ATS info length: %d", iRet);
        goto err;
    }

    usTail = GET_SHORT(pucMsgData+pstFrameHdr->usDataLen);
    usTail = ATSNtohs(usTail);
    if (usTail != ATS_FRAME_TAIL)
    {
        TaskMsgLog(LOG_ERROR, "invalid ATS frame tail 0x%04X", usTail);
        goto err;
    }

    if (pstFrameHdr->ucFrameCount == 0)
    {
        TaskMsgLog(LOG_ERROR, "frame count must be larger than 0");
        goto err;
    }

    if (pstFrameHdr->ucFrameIndex == 0)
    {
        TaskMsgLog(LOG_ERROR, "frame index must be larger than 0");
        goto err;
    }

    if (pstFrameHdr->ucFrameIndex > pstFrameHdr->ucFrameCount)
    {
        TaskMsgLog(LOG_ERROR, "frame index(%u) is larger than frame count %u", pstFrameHdr->ucFrameIndex, pstFrameHdr->ucFrameCount);
        goto err;
    }

    ulFrameLen = sizeof(ATS_FRAME_HDR_T)+pstFrameHdr->usDataLen+sizeof(UINT16);

    if (pstFrameHdr->ucFrameIndex == 1)
    {
        m_ulFrameNum = pstFrameHdr->ucFrameCount;
        m_ulLastFrameIndex = pstFrameHdr->ucFrameIndex;
        memcpy(m_aucRecvBuf, pucMsgData, pstFrameHdr->usDataLen);
        m_ulRecvMsgLen = pstFrameHdr->usDataLen;
    }
    else
    {
        if (m_ulFrameNum != pstFrameHdr->ucFrameCount)
        {
            TaskMsgLog(LOG_ERROR, "frame count(%u) should be %u", pstFrameHdr->ucFrameCount, m_ulFrameNum);
            goto err;
        }

        if ((m_ulLastFrameIndex+1) != pstFrameHdr->ucFrameIndex)
        {
            TaskMsgLog(LOG_ERROR, "frame index(%u) should be %u", pstFrameHdr->ucFrameCount, m_ulLastFrameIndex+1);
            goto err;
        }

        if ((m_ulRecvMsgLen + pstFrameHdr->usDataLen) > sizeof(m_aucRecvBuf))
        {
            TaskMsgLog(LOG_ERROR, "msg data length(%u) is out of bound(%u)", m_ulRecvMsgLen + pstFrameHdr->usDataLen, sizeof(m_aucRecvBuf));
            goto err;
        }

        memcpy(m_aucRecvBuf+m_ulRecvMsgLen, pucMsgData, pstFrameHdr->usDataLen);
        m_ulRecvMsgLen += pstFrameHdr->usDataLen;

        m_ulLastFrameIndex = pstFrameHdr->ucFrameIndex;
    }

    if (m_ulLastFrameIndex != m_ulFrameNum)
    {
        goto start;
    }

    g_ulATSMsgNum += m_ulFrameNum;
    g_i64ATSMsgSize += m_ulRecvMsgLen;

    if (!OnATSMsg(stSock, m_aucRecvBuf, m_ulRecvMsgLen, pucAddr))
    {
        goto err;
    }

    SetATSValid(TRUE);

    if (!stLocalCfg.bAutoCloseATS)
    {
        return TRUE;
    }

    m_ulValidAddrIndex = 1-m_ulValidAddrIndex;

    TaskLog(LOG_INFO, "close ATS socket %u", stSock);
    CLOSE_SOCKET(stSock);
    return TRUE;

err:
    TaskMsgLog(LOG_ERROR, "close ATS socket %u", stSock);
    CLOSE_SOCKET(stSock);

    SetATSValid(FALSE);

    return FALSE;
}

VOID TaskATS::GetATSMsg(UINT8 *pucFrame, UINT32 ulFrameLen)
{
    ATS_FRAME_HDR_T     *pstFrame = (ATS_FRAME_HDR_T*)pucFrame;
    UINT8               *pucMsg = m_aucMsgData;
    UINT32              ulMsgLen  = 0;
    UINT32              ulCurrPos = 0;

    while(ulCurrPos < ulFrameLen)
    {
        pstFrame = (ATS_FRAME_HDR_T*)(pucFrame+ulCurrPos);

        if ((ulCurrPos + sizeof(ATS_FRAME_HDR_T) +  sizeof(UINT16)) > ulFrameLen)
        {
            TaskMsgLog(LOG_ERROR, "invalid ATS msg frame");

            break;
        }

        if ((ulCurrPos + sizeof(ATS_FRAME_HDR_T) + pstFrame->usDataLen + sizeof(UINT16)) > ulFrameLen)
        {
            TaskMsgLog(LOG_ERROR, "invalid ATS msg frame");
            break;
        }

        memcpy(pucMsg, pstFrame+1, pstFrame->usDataLen);
        pucMsg += pstFrame->usDataLen;
        ulMsgLen += pstFrame->usDataLen;

        ulCurrPos += sizeof(ATS_FRAME_HDR_T) +  pstFrame->usDataLen + sizeof(UINT16);
        if (ulCurrPos == ulFrameLen)
        {
            break;
        }
    }

    m_ulMsgDataLen = ulMsgLen;
}

BOOL TaskATS::OnATSMsg(SOCKET stSock, UINT8 *pucMsg, UINT32 ulMsgLen, UINT8 *pucAddr)
{
    UINT32              ulTime = gos_get_current_time();
    ATS_MSG_HDR_T       *pstMsgHdr;
    UINT32              ulCurrMsgLen;
    INT32               iMsgDataLen = ulMsgLen;    // 剩余长度
    VECTOR<ATS_INFO_T>  vATSInfo;
    UINT32              ulCurrentTime = gos_get_uptime_1ms();
    static              UINT32 ulFramCount = 0;
    static              UINT32 ulHeartbeatTimeOutCount = 0;
    static              UINT32 ulLastRecvHeartBeatTime = ulCurrentTime;


    while (iMsgDataLen > 0)
    {
        if (iMsgDataLen < sizeof(ATS_MSG_HDR_T))
        {
            TaskLog(LOG_ERROR, "invalid msg length %d", iMsgDataLen);
            return FALSE;
        }

        pstMsgHdr = (ATS_MSG_HDR_T*)pucMsg;

        pstMsgHdr->usMsgLen = ATSNtohs(pstMsgHdr->usMsgLen);
        pstMsgHdr->stMsgTime.usYear = ATSNtohs(pstMsgHdr->stMsgTime.usYear);
        pstMsgHdr->usLineID = ATSNtohs(pstMsgHdr->usLineID);

        if (g_usATSLineID != 0xffff)
        {
            g_usATSLineID = pstMsgHdr->usLineID;
        }

        ulCurrMsgLen = sizeof(pstMsgHdr->usMsgLen) + pstMsgHdr->usMsgLen;
        if ((UINT32)iMsgDataLen < ulCurrMsgLen)
        {
            TaskLog(LOG_ERROR, "invalid msg(%u) length %u", pstMsgHdr->ucMsgID, pstMsgHdr->usMsgLen);
            return FALSE;
        }

        iMsgDataLen -= ulCurrMsgLen;

        // 心跳消息处理
        if (pstMsgHdr->ucMsgID == ATS_MSGID_HEARTBEAT)
        {
            if (ulCurrMsgLen != sizeof(ATS_MSG_HEARTBEAT_T))
            {
                TaskLog(LOG_ERROR, "invalid msg length %u of heart beat", pstMsgHdr->usMsgLen);
                return FALSE;
            }

            SendHeartBeatMsg(stSock);

            ulHeartbeatTimeOutCount = 0;
            pucMsg += ulCurrMsgLen;
            continue;
        }

        // 其他消息不处理
        if (pstMsgHdr->ucMsgID != ATS_MSGID_ATSINFO)
        {
          //  TaskLog(LOG_ERROR, "unknown ATS msg id %u, msg len %u", pstMsgHdr->ucMsgID, ulCurrMsgLen);
            pucMsg += ulCurrMsgLen;
            ulHeartbeatTimeOutCount = 0;
            continue;
        }

        //将ATSInfo 保存到文件
        if (GetLocalCfg().bSaveATSToFile)
        {
            static ATS_REC_T    stATSRec = {0};

            strcpy(stATSRec.acMagic, ATS_REC_MAGIC);
            stATSRec.ulTime = ulTime;
            memcpy(stATSRec.aucAddr, pucAddr, 4);

            if (ulCurrMsgLen > sizeof(stATSRec.stRecData))
            {
                ulCurrMsgLen = sizeof(stATSRec.stRecData);
            }
            memcpy(&stATSRec.stRecData, pstMsgHdr, ulCurrMsgLen);
            SaveATSToFile(&stATSRec);
        }

        // 处理ATS数据
        INT32 iATSInfoLen = pstMsgHdr->usMsgLen+sizeof(pstMsgHdr->usMsgLen) - sizeof(ATS_MSG_HDR_T);

        ///GosLog(LOG_ERROR, "year: %u, mon: %u, day: %u, hour: %u, min: %u, sec: %u ", pstMsgHdr->stMsgTime.usYear, pstMsgHdr->stMsgTime.ucMonth, pstMsgHdr->stMsgTime.ucDay, pstMsgHdr->stMsgTime.ucHour, pstMsgHdr->stMsgTime.ucMin, pstMsgHdr->stMsgTime.ucSec);

        if (ParseATSInfo((UINT8*)(pstMsgHdr+1), iATSInfoLen, vATSInfo))
        {
            ulHeartbeatTimeOutCount = 0;
            for (UINT32 i=0; i<vATSInfo.size(); i++)
            {
                ATSInfoToHost(&vATSInfo[i]);
            }

            ///TCT 对接测试时启用
            #if 0
            UINT32 ulTrainUnitID;
            UINT32 ulServerNum;
            UINT32 ulOrdeNum;
            CHAR   acDestID[5] = {0};

            for (UINT32 i = 0; i < vATSInfo.size(); i++)
            {
                memcpy(acDestID, vATSInfo[i].acDestID, 3);

                CharArrayToInt(vATSInfo[i].acServiceNumber, sizeof(vATSInfo[i].acServiceNumber), &ulServerNum);
                CharArrayToInt(vATSInfo[i].acTrainUnitID, sizeof(vATSInfo[i].acTrainUnitID), &ulTrainUnitID);
                CharArrayToInt(vATSInfo[i].acOrderNumber, sizeof(vATSInfo[i].acOrderNumber), &ulOrdeNum);

                GosLog(LOG_ERROR, "total train num:%d, this is NO:%u", vATSInfo.size(), i);

                GosLog(LOG_ERROR, "year: %u, mon: %u, day: %u, hour: %u, min: %u, sec: %u ucARFlag: %u, ucSpeed: %u, acDestID: %s, acOrderNumber: %u, acServiceNumber: %u, acTrainUnitID: %u iOtpTime: %d, ucActiveTCID: %u, ucControlMode: %u, ucDirection: %u, ucDriveMode: %u, ucEBFlag: %u  ucInOutDepot: %u, ucIsOnPlatform: %u, ucIsOpenDoor: %u, ucIsStopped: %u, ucLostRealLocation: %u, ucOnTransferTrackFlag: %u ucOnTurnBackTrackFlag: %u, usStationID: %u, usDownsideStationID: %u, usUpsideStationID: %u, rate:%u, StopDeviation:%x, SnowStatus:%u, FireStatus:%u", pstMsgHdr->stMsgTime.usYear, pstMsgHdr->stMsgTime.ucMonth, pstMsgHdr->stMsgTime.ucDay, pstMsgHdr->stMsgTime.ucHour, pstMsgHdr->stMsgTime.ucMin, pstMsgHdr->stMsgTime.ucSec,
                    vATSInfo[i].ucARFlag, vATSInfo[i].ucSpeed, acDestID, ulOrdeNum, ulServerNum,
                    ulTrainUnitID, vATSInfo[i].iOtpTime, vATSInfo[i].ucActiveTCID,
                    vATSInfo[i].ucControlMode, vATSInfo[i].ucDirection, vATSInfo[i].ucDriveMode,
                    vATSInfo[i].ucEBFlag,vATSInfo[i].ucInOutDepot, vATSInfo[i].ucIsOnPlatform, vATSInfo[i].ucIsOpenDoor,
                    vATSInfo[i].ucIsStopped, vATSInfo[i].ucLostRealLocation, vATSInfo[i].ucOnTransferTrackFlag, vATSInfo[i].ucOnTurnBackTrackFlag,
                    vATSInfo[i].usStationID, vATSInfo[i].usDownsideStationID, vATSInfo[i].usUpsideStationID, vATSInfo[i].ucRate, vATSInfo[i].ucStopDeviation, vATSInfo[i].ucSnowStatus, vATSInfo[i].ucFireStatus);
            }
            #endif

            ///GosLog(LOG_ERROR, "year: %u, mon: %u, day: %u, hour: %u, min: %u, sec: %u TurnBackTrackFlag:%u, OnTransferTrackFlag:%u, LostRealLocation:%u Transfer_Flag:%u", pstMsgHdr->stMsgTime.usYear, pstMsgHdr->stMsgTime.ucMonth, pstMsgHdr->stMsgTime.ucDay, pstMsgHdr->stMsgTime.ucHour, pstMsgHdr->stMsgTime.ucMin, pstMsgHdr->stMsgTime.ucSec, vATSInfo[0].ucOnTurnBackTrackFlag, vATSInfo[0].ucOnTransferTrackFlag, vATSInfo[0].ucLostRealLocation, vATSInfo[0].ucInOutDepot);

            ulFramCount++;

            InitATSInfo(vATSInfo);
        }

        pucMsg += ulCurrMsgLen;
        continue;
    }

    return TRUE;
}

BOOL TaskATS::ParseATSInfo(UINT8 *pucMsg, INT32 iMsgLen, VECTOR<ATS_INFO_T> &vATSInfo)
{
    vATSInfo.clear();

    if (iMsgLen <= 0)
    {
        return FALSE;
    }

    UINT8       *pucMsgEndPos = pucMsg + iMsgLen;
    UINT32      ulTrainNum = *pucMsg++;
    INT32       iLeftMsgLen = iMsgLen - sizeof(UINT8);
    ATS_INFO_T  stInfo;
    UINT32      ulMinTrainInfoLen = sizeof(ATS_INFO_T) - sizeof(stInfo.acTrainUnitID) - sizeof(stInfo.acServiceNumber) - sizeof(stInfo.acOrderNumber) - sizeof(stInfo.acDestID);
    UINT32      ulTrainInfoVarLen; // 可变部分长度
    UINT32      ulTrainInfoFixLen = sizeof(ATS_INFO_T) - STRUCT_OFFSET(ATS_INFO_T, usRtuID);

    while (pucMsg < pucMsgEndPos)
    {
        iLeftMsgLen = pucMsgEndPos - pucMsg;
        if (iLeftMsgLen < ulMinTrainInfoLen)
        {
            TaskMsgLog(LOG_ERROR, "invalid ATS msg");
            return FALSE;
        }

        ulTrainInfoVarLen = 0;
        memset(&stInfo, 0, sizeof(stInfo));

        // TrainUnit
        stInfo.ucTrainUnitIDLen = *pucMsg++;
        if (stInfo.ucTrainUnitIDLen > sizeof(stInfo.acTrainUnitID))
        {
            TaskMsgLog(LOG_ERROR, "TrainUnitID len is too big: %u", stInfo.ucTrainUnitIDLen);
            return FALSE;
        }

        if (stInfo.ucTrainUnitIDLen)
        {
            memcpy(stInfo.acTrainUnitID, pucMsg, stInfo.ucTrainUnitIDLen);
            pucMsg += stInfo.ucTrainUnitIDLen;
            ulTrainInfoVarLen += (sizeof(UINT8) + stInfo.ucTrainUnitIDLen);
        }

        // ServiceNumber
        stInfo.ucServiceNumberLen = *pucMsg++;
        if (stInfo.ucServiceNumberLen > sizeof(stInfo.acServiceNumber))
        {
            TaskMsgLog(LOG_ERROR, "ServiceNumber len is too big: %u", stInfo.ucServiceNumberLen);
            return FALSE;
        }

        if (stInfo.ucServiceNumberLen)
        {
            memcpy(stInfo.acServiceNumber, pucMsg, stInfo.ucServiceNumberLen);
            pucMsg += stInfo.ucServiceNumberLen;
            ulTrainInfoVarLen += (sizeof(UINT8) + stInfo.ucServiceNumberLen);
        }

        // OrderNumber
        stInfo.ucOrderNumberLen = *pucMsg++;
        if (stInfo.ucOrderNumberLen > sizeof(stInfo.acOrderNumber))
        {
            TaskMsgLog(LOG_ERROR, "OrderNumber len is too big: %u", stInfo.ucOrderNumberLen);
            return FALSE;
        }

        if (stInfo.ucOrderNumberLen)
        {
            memcpy(stInfo.acOrderNumber, pucMsg, stInfo.ucOrderNumberLen);
            pucMsg += stInfo.ucOrderNumberLen;
            ulTrainInfoVarLen += (sizeof(UINT8) + stInfo.ucOrderNumberLen);
        }

        // DestID
        stInfo.ucDestIDLen = *pucMsg++;
        if (stInfo.ucDestIDLen > sizeof(stInfo.acDestID))
        {
            TaskMsgLog(LOG_ERROR, "DestID len is too big: %u", stInfo.ucDestIDLen);
            return FALSE;
        }

        if (stInfo.ucDestIDLen)
        {
            memcpy(stInfo.acDestID, pucMsg, stInfo.ucDestIDLen);
            pucMsg += stInfo.ucDestIDLen;
            ulTrainInfoVarLen += (sizeof(UINT8) + stInfo.ucDestIDLen);
        }

        if ((ulTrainInfoVarLen+ulTrainInfoFixLen) > iLeftMsgLen)
        {
            TaskMsgLog(LOG_ERROR, "invalid ATS msg");
            return FALSE;
        }

        memcpy(&stInfo.usRtuID, pucMsg, ulTrainInfoFixLen);
        pucMsg += ulTrainInfoFixLen;

        vATSInfo.push_back(stInfo);
    }

    if (ulTrainNum != vATSInfo.size())
    {
        TaskMsgLog(LOG_WARN, "invalid train number(%u), it should be %u", ulTrainNum, vATSInfo.size());
    }

    return TRUE;
}

VOID TaskATS::TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    UINT32  ulTaskStatus = GetTaskStatus();

    switch(ulTaskStatus)
    {
        case TASK_STATUS_IDLE:
            switch(usMsgID)
            {
                case EV_TASK_INIT_REQ:
                    SendRsp();
                    SetTaskStatus(TASK_STATUS_INIT);
                    SetTimer(TIMER_INIT, 0);
                    break;

                default:
                    break;
            }

            break;

        case TASK_STATUS_INIT:
            switch(usMsgID)
            {
                case EV_INIT_IND:
                    if (!Init())
                    {
                        SetTimer(TIMER_INIT, 1000);
                        TaskLog(LOG_FATAL, "Init failed!");
                        gos_sleep(1);
                        //exit(-1);
                    }

                    break;

                default:
                    break;
            }

            break;

        case TASK_STATUS_WORK:
            switch(usMsgID)
            {
                case EV_TASK_CHECK_REQ:
                    SendRsp();
                    break;

                case EV_TIMER_GET_ATS_IND:
                    OnTimerGetATSInd();
                    break;

                default:
                    TaskLog(LOG_ERROR, "unknown msg %u", usMsgID);
                    break;
            }

            break;

        default:
            break;
    }
}
