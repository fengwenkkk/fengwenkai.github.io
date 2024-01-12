#include "g_include.h"
#include "rtsp_util.h"
#include "rtsp_client.h"

static UINT32  g_ulRTSPServerDescribeFailTime = 0;

// 连接RTSP服务器的TCP客户端
SOCKET InitRTSPClientSocket(UINT8 *pucAddr, UINT16 usPort)
{
    SOCKET stSock = gos_connect_tcp_server(pucAddr, usPort);
    if (stSock == INVALID_SOCKET)
    {
        GosLog(LOG_ERROR, "connect to RTSP server "IP_FMT"(%u) failed", IP_ARG(pucAddr), usPort);

        return INVALID_SOCKET;
    }

    GosLog(LOG_INFO, "connect to RTSP server "IP_FMT"(%u) succ", IP_ARG(pucAddr), usPort);

    return stSock;
}

static BOOL ParseRTSPServerMsg(CONN_INFO_T *pstConnInfo, CHAR **pszRTSPMsg)
{
    CHAR    *szMsgHdr = pstConnInfo->szLocalRecvBuf;
    CHAR    *szMsgBody = NULL;
    CHAR    *szEnd;
    CHAR    *szLen = 0;  // Content-Length: 292
    UINT32  ulMsgHdrLen = 0;
    UINT32  ulMsgBodyLen = 0;
    UINT32  ulMsgLen = 0;
    CHAR    *szRTSPMsg;

    *pszRTSPMsg = NULL;

    szEnd = strstr(szMsgHdr, "\r\n\r\n");
    if (!szEnd)
    {
        return TRUE;
    }

    szMsgBody = szEnd+4;
    ulMsgHdrLen = szMsgBody - szMsgHdr;
    szLen = strstr(szMsgHdr, "Content-Length");
    if (szLen)
    {
        if (!GetSIPValue(szLen, "Content-Length", &ulMsgBodyLen))
        {
            GosLog(LOG_ERROR, "get Content-Length failed on msg: %s", szLen);
            return FALSE;
        }

        if ((ulMsgHdrLen + ulMsgBodyLen) > pstConnInfo->ulLocalRecvSize)
        {
            return TRUE;
        }
    }

    ulMsgLen = ulMsgHdrLen + ulMsgBodyLen;
    szRTSPMsg = (CHAR*)calloc(1, ulMsgLen+1);
    if (!szRTSPMsg)
    {
        return FALSE;
    }

    memcpy(szRTSPMsg, szMsgHdr, ulMsgLen);
    *pszRTSPMsg = szRTSPMsg;

    pstConnInfo->ulLocalRecvSize -= ulMsgLen;
    if (pstConnInfo->ulLocalRecvSize > 0)
    {
        memmove(pstConnInfo->szLocalRecvBuf, pstConnInfo->szLocalRecvBuf+ulMsgLen, pstConnInfo->ulLocalRecvSize);
    }

    return TRUE;
}

GString ReplaceRTSPStr(CHAR *szMsg, CHAR *szOld, CHAR *szNew)
{
    GString     str;
    CHAR        *szTmp;

    while(1)
    {
        szTmp = strstr(szMsg, szOld);
        if (!szTmp)
        {
            str.Append(szMsg);
            break;
        }

        str.Append(szMsg, szTmp-szMsg, FALSE);
        str.Append(szNew);

        szMsg = szTmp+strlen(szOld);
    }

    return str;
}

static BOOL ReplaceRTSPUrl(GString &strMsg, const CHAR *szKeyWord, CHAR *szNewText)
{
    CHAR        *szRTSPMsg = strMsg.GetString();
    CHAR        *szMsg = strstr(szRTSPMsg, szKeyWord);

    if (!szMsg)
    {
        return FALSE;
    }

    szMsg += strlen(szKeyWord);
    szMsg = gos_left_trim_string(szMsg);

    CHAR    *szEnd = strstr(szMsg, "/");

    if (!szEnd)
    {
        return FALSE;
    }

    GString     strNewStr;

    strNewStr.Append(szRTSPMsg, szMsg-szRTSPMsg, FALSE);
    strNewStr.Append(szNewText);
    strNewStr.Append(szEnd);

    strNewStr = strMsg;

    return TRUE;
}

// such as  abc: rtsp://1.2.3.4:400/a xxx rtsp://1.2.3.5/b yyy
GString ReplaceRTSPUrl(CHAR *szRTSPMsg, const CHAR *szKeyWord, CHAR *szNewText)
{
    GString     strMsg;
    CHAR        *szMsg = szRTSPMsg;
    CHAR        *szStart;
    CHAR        *szEnd;

    while(szMsg)
    {
        szStart = strstr(szMsg, szKeyWord);
        if (!szStart)
        {
            strMsg.Append(szMsg);
            goto end;
        }

        szStart += strlen(szKeyWord);
        szStart = gos_left_trim_string(szStart);
        szEnd = strstr(szStart, "/");

        if (!szEnd)
        {
            goto end;
        }

        strMsg.Append(szMsg, szStart-szMsg, FALSE);
        strMsg.Append(szNewText);

        szMsg = szEnd;
    }

end:
    if (strMsg.Length() == 0)
    {
        strMsg.Append(szMsg);
    }

    return strMsg;
}

static VOID RelayRTSPServerMsg(CONN_INFO_T *pstConnInfo, CHAR *szRTSPMsg)
{
    gos_tcp_send(pstConnInfo->stClientSocket, szRTSPMsg, strlen(szRTSPMsg));
    GosLog(LOG_INFO, "send msg to RTSP client: \n%s", szRTSPMsg);
}

/* ...
Content-Base: rtsp://192.0.1.133/av_stream
Content-Length: 292

v=0
o=StreamingServer 3433055887 1290545076275814 IN IP4 192.0.1.133
...
*/
static VOID RelayRTSPServerSDPMsg(CONN_INFO_T *pstConnInfo, CHAR *szRTSPMsg)
{
    GString     strMsg;
    CHAR        acIP[32];
    CHAR        acNewIP[32];
    CHAR        acContentLen[32];
    CHAR        acNewContentLen[32];
    CHAR        *szNewMsg;
    CHAR        *szMsgBody = strstr(szRTSPMsg, "\r\n\r\n");
    UINT32      ulMsgBodyLen = 0;

    if (!szMsgBody)
    {
        GosLog(LOG_ERROR, "invalid sdp msg: %s", szRTSPMsg);
        return;
    }

    sprintf(acIP, IP_FMT, IP_ARG(pstConnInfo->aucRTSPServerAddr));

    szMsgBody += 4; // \r\n\r\n
    ulMsgBodyLen = strlen(szMsgBody);
    sprintf(acContentLen, "Content-Length: %u", ulMsgBodyLen);

    if (g_usLocalRTSPServerPort == g_usDefaultRTSPServerPort)
    {
        sprintf(acNewIP, IP_FMT, IP_ARG(g_aucLocalRTSPServerAddr));
    }
    else
    {
        sprintf(acNewIP, IP_FMT ":%u", IP_ARG(g_aucLocalRTSPServerAddr), g_usLocalRTSPServerPort);
    }

    strMsg = ReplaceRTSPUrl(szRTSPMsg, "rtsp://", acNewIP);

    sprintf(acNewIP, IP_FMT, IP_ARG(g_aucLocalRTSPServerAddr));
    strMsg = ReplaceRTSPStr(strMsg.GetString(), acIP, acNewIP);// TODO有的IP采用的机器名称
    szNewMsg = strMsg.GetString(); 
    szMsgBody = strstr(szNewMsg, "\r\n\r\n");
    if (!szMsgBody)
    {
        return;
    }

    szMsgBody += 4; // \r\n\r\n
    ulMsgBodyLen = strlen(szMsgBody);

    // Content-Length: 292 -> ...
    sprintf(acNewContentLen, "Content-Length: %u", ulMsgBodyLen);
    strMsg = ReplaceRTSPStr(szNewMsg, acContentLen, acNewContentLen);

    szNewMsg = strMsg.GetString();
    gos_tcp_send(pstConnInfo->stClientSocket, szNewMsg, strlen(szNewMsg));

    GosLog(LOG_INFO, "send SDP msg to RTSP client: \n%s", szNewMsg);
}

static VOID OnRTSPServerMsg(CONN_INFO_T *pstConnInfo, CHAR *szRTSPMsg)
{
    CHAR    acNewText[32];
    GString strTmp;

    GosLog(LOG_INFO, "recv msg from RTSP server: \n%s", szRTSPMsg);

    if (strstr(szRTSPMsg, "Public:"))
    {
        RelayRTSPServerMsg(pstConnInfo, szRTSPMsg);
    } 
    else if (strstr(szRTSPMsg, "Content-Type: application/sdp"))
    {
        RelayRTSPServerSDPMsg(pstConnInfo, szRTSPMsg);
    }
    else if (strstr(szRTSPMsg, "client_port"))
    {
        CHAR        acPort[32];   
        CHAR        *szMsg = strstr(szRTSPMsg, "server_port=");

        strTmp.Append(szRTSPMsg);

        if (szMsg)
        {  
            szMsg += strlen("server_port=");

            CHAR    *szEnd = strstr(szMsg, "\r\n");
        
            if (!szEnd)
            {
                GosLog(LOG_ERROR, "invalid msg from %s", szMsg);
                return;
            }

            CHAR    *szEnd2 = strchr(szMsg, ';');

            if (szEnd2 && szEnd2 < szEnd)
            {
                szEnd = szEnd2;
            }

            UINT32      ulLen = szEnd - szMsg;

            if (ulLen >= sizeof(acPort))
            {
                GosLog(LOG_ERROR, "invalid server_port len of %s", szMsg);
                return;
            }

            memcpy(acPort, szMsg, ulLen);
            acPort[ulLen] = '\0';

            if (!ParseRTSPPort(acPort, pstConnInfo->usServerRTPPort, pstConnInfo->usServerRTCPPort))
            {
                GosLog(LOG_ERROR, "invalid rtsp server port %s", acPort);
                return;
            }

            sprintf(acNewText, "%u-%u", pstConnInfo->usLocalRTPPort, pstConnInfo->usLocalRTCPPort);
            strTmp = ReplaceRTSPMsg(strTmp.GetString(), "server_port", acNewText);
        }
        
        sprintf(acNewText, "%u-%u", pstConnInfo->usClientRTPPort, pstConnInfo->usClientRTCPPort);
        strTmp = ReplaceRTSPMsg(strTmp.GetString(), "client_port", acNewText);

        sprintf(acNewText, IP_FMT, IP_ARG(g_aucLocalRTSPServerAddr));
        strTmp = ReplaceRTSPMsg(strTmp.GetString(), "source", acNewText);
        
        RelayRTSPServerMsg(pstConnInfo, strTmp.GetString());
    }
    else if (strstr(szRTSPMsg, "RTP-Info: url=rtsp://"))
    {
        if (g_usLocalRTSPServerPort == g_usDefaultRTSPServerPort)
        {
            sprintf(acNewText, IP_FMT, IP_ARG(g_aucLocalRTSPServerAddr));
        }
        else
        {
            sprintf(acNewText, IP_FMT":%u", IP_ARG(g_aucLocalRTSPServerAddr), g_usLocalRTSPServerPort);
        }

       // strTmp = ReplaceRTSPUrl(szRTSPMsg, "RTP-Info: url=rtsp://", acNewText);
        strTmp = ReplaceRTSPUrl(szRTSPMsg, "rtsp://", acNewText);
        
        RelayRTSPServerMsg(pstConnInfo, strTmp.GetString());
    }
    else 
    {
        RelayRTSPServerMsg(pstConnInfo, szRTSPMsg);
    }

    if (strcmp(g_acCurrRTSPCmd, "DESCRIBE") == 0)
    {
        if (strstr(szRTSPMsg, "404 NOT FOUND"))
        {
            if (g_ulRTSPServerDescribeFailTime == 0)
            {
                g_ulRTSPServerDescribeFailTime = gos_get_sys_uptime();
            }

            if ((gos_get_sys_uptime()-g_ulRTSPServerDescribeFailTime) >= g_ulMaxConnectRTSPServerTime)
            {
                GosLog(LOG_INFO, "describe RTSP url failed for %u seconds", g_ulMaxConnectRTSPServerTime); 
                CloseApp();
            }
        }
        else
        {
            g_ulRTSPServerDescribeFailTime = 0;
        }
    }
}

// RTP Over RTSP模式
VOID OnRTSPServerRTPMsg(CONN_INFO_T *pstConnInfo)
{
    gos_tcp_send(pstConnInfo->stClientSocket, pstConnInfo->szLocalRecvBuf, pstConnInfo->ulLocalRecvSize);
    pstConnInfo->ulLocalRecvSize = 0;

#if 0
    static UINT32   ulLastTime = 0;
    UINT32          ulCurrTime = gos_get_uptime();

    if (pstConnInfo->acSession[0] &&
        (ulCurrTime-ulLastTime) > 20)
    {
        ulLastTime = ulCurrTime;

        CHAR    acMsg[1024];
        CHAR    acNewIP[32];

        sprintf(acMsg, "GET_PARAMETER %s\r\n"
                        "CSeq: %u\r\n"
                        "User-Agent: %s\r\n"
                        "Session: %s\r\n\r\n",
                        //IP_ARG(pstConnInfo->aucRTSPServerAddr),
                        //pstConnInfo->usRTSPServerPort,
                        pstConnInfo->acPlayParam,
                        ++pstConnInfo->ulCSeq,
                        pstConnInfo->acUserAgent,
                        pstConnInfo->acSession);

        if (pstConnInfo->usRTSPServerPort == g_usDefaultRTSPServerPort)
        {
            sprintf(acNewIP, IP_FMT, IP_ARG(pstConnInfo->aucRTSPServerAddr));
        }
        else
        {
            sprintf(acNewIP, IP_FMT ":%u", IP_ARG(pstConnInfo->aucRTSPServerAddr), pstConnInfo->usRTSPServerPort);
        }

        GString strMsg = ReplaceRTSPUrl(acMsg, "rtsp://", acNewIP);
        CHAR    *szMsg = strMsg.GetString();

        gos_tcp_send(pstConnInfo->stLocalSocket, szMsg, strlen(szMsg));
    }
#endif
}

// 接收RTSP服务器的消息
INT32 RecvRTSPServerMsg(CONN_INFO_T *pstConnInfo, INT32 *piErrCode)
{
    INT32           iRecvSize;
    UINT32          ulByteToRecved = 0;
    UINT32          ulMaxBufSize = 64*1024-1;
    CHAR            *szRTSPMsg;
    SOCKET          stSocket = pstConnInfo->stLocalSocket;

#if 1
    /* 看看有多少字节可读 */
    if (ioctlsocket(pstConnInfo->stLocalSocket, FIONREAD, (unsigned long*)&ulByteToRecved) == SOCKET_ERROR)
    {
        GosLog(LOG_ERROR, "RecvRTSPServerMsg: ioctl socket(%u) failed!", stSocket);
        *piErrCode = gos_get_socket_err_code();
        return -1;
    }

    /* 如果没有字节可读, 则对端已经关闭连接, 返回错误 */
    if (ulByteToRecved == 0)
    {
        *piErrCode = 0;// SECONNRESET;
        return 0;
    }

#endif

    /* 处理上次RecvMsg的剩余数据 */
    if (!pstConnInfo->szLocalRecvBuf)
    {
        pstConnInfo->szLocalRecvBuf = (CHAR*)calloc(1, ulMaxBufSize+1);
    }

    iRecvSize = recv(pstConnInfo->stLocalSocket, (CHAR*)(pstConnInfo->szLocalRecvBuf + pstConnInfo->ulLocalRecvSize), ulMaxBufSize - pstConnInfo->ulLocalRecvSize, 0);
    if (iRecvSize < 0)
    {
        *piErrCode = -1;
        GosLog(LOG_ERROR, "RecvRTSPServerMsg: recv client(%u) failed: %s!", stSocket, gos_get_socket_err());

        return -1;
    }
    else if (iRecvSize == 0)
    {
        GosLog(LOG_ERROR, "RecvRTSPServerMsg: recv client(%u) failed by connect closed", stSocket);

        return -1;
    }

    pstConnInfo->ulLocalRecvSize += iRecvSize;

    while(1)
    {
        if (pstConnInfo->ulLocalRecvSize == 0)
        {
            break;
        }

        pstConnInfo->szLocalRecvBuf[pstConnInfo->ulLocalRecvSize] = '\0';

        // 非RTSP消息(RTP消息）  SendHeartBeatMsgToRTSPServer();
        if (//!IsRTSPMsg(pstConnInfo->szLocalRecvBuf, pstConnInfo->ulLocalRecvSize))
            pstConnInfo->ulLocalRecvSize < 4 || 
            memcmp(pstConnInfo->szLocalRecvBuf, "RTSP", 4) != 0)
        {
            OnRTSPServerRTPMsg(pstConnInfo);
            continue;
        }

        if (!ParseRTSPServerMsg(pstConnInfo, &szRTSPMsg))
        {
            return -1;
        }

        if (!szRTSPMsg)
        {
            break;
        }

        OnRTSPServerMsg(pstConnInfo, szRTSPMsg);
        free(szRTSPMsg);
    }
   
    *piErrCode = 0;
    return iRecvSize;
}
