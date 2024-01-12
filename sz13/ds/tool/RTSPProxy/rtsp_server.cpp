#include "g_include.h"
#include "rtsp_util.h"
#include "rtp.h"
#include "rtsp_client.h"
#include "rtsp_server.h"

static CHAR         g_acRecvMsg[64*1024];
static UINT32       g_ulCurrRecvMsgLen = 0;
static UINT32       g_ulLastRTSPClientTime = 0;

VOID CheckRTSPClientTimeout()
{
    UINT32  ulTime = gos_get_uptime();

    if ((ulTime - g_ulLastRTSPClientTime) >= g_ulRTSPClientTimeout)
    {
        GosLog(LOG_ERROR, "check rtsp client timeout");
        CloseApp();
    }
}

/**
 * @brief           创建RTSP TCP sever socket
 * @param           pucAddr    [in]  指定的本地IP地址(一般由LTE系统提供) TODO：待确认
 * @param           usPort     [in]  默认55554
 * @return          SOCKET 
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *  
 * @endcode
 * @note
 * 1、TCP_socket
 */
SOCKET InitLocalRTSPServerSocket(UINT8 *pucAddr, UINT16 usPort)
{
    SOCKET              stSocket = INVALID_SOCKET;
    SOCKADDR_IN         stLocalAddr = {0};
    UINT32              ulFlag = 1;
    
    stLocalAddr.sin_family = PF_INET;
    stLocalAddr.sin_port   = htons(usPort);
    memcpy(&stLocalAddr.sin_addr.s_addr, pucAddr, 4);

    stSocket = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (stSocket == INVALID_SOCKET)
    {
        GosLog(LOG_ERROR, "create local rtsp server socket failed: %s", gos_get_socket_err());
        return INVALID_SOCKET;
    }

    if (setsockopt(stSocket, SOL_SOCKET, SO_REUSEADDR, (CHAR*)&ulFlag, sizeof(ulFlag)) == SOCKET_ERROR)
    {
        GosLog(LOG_ERROR, "set local rtsp server failed on setsockopt, %s", gos_get_socket_err());
        CLOSE_SOCKET(stSocket);

        return INVALID_SOCKET;
    }

    if (bind (stSocket, (SOCKADDR *) &stLocalAddr, sizeof (SOCKADDR)) == SOCKET_ERROR)
    {
        GosLog(LOG_ERROR, "bind local rtps server socket failed on " IP_FMT ":%u, %s", IP_ARG(pucAddr), usPort, gos_get_socket_err());
        CLOSE_SOCKET(stSocket);

        return INVALID_SOCKET;
    }

    // mpk
    if (listen(stSocket, 16) == SOCKET_ERROR)
    {
        GosLog(LOG_ERROR, "listen local rtps server socket failed, %s", gos_get_socket_err());
        CLOSE_SOCKET(stSocket);

        return INVALID_SOCKET;
    }
    
    g_stLocalRTSPServerSock = stSocket;
    
    return stSocket;
}

GString ReplaceRTSPMsg(CHAR *szRTSPMsg, const CHAR *szKey, CHAR *szNewValue)
{
    GString strMsg;
    CHAR    *szMsg = strstr(szRTSPMsg, szKey);

    if (!szMsg)
    {
        strMsg.Append(szRTSPMsg);
        return strMsg;
    }

    szMsg += strlen(szKey);
    szMsg = gos_left_trim_string(szMsg);
    if (*szMsg != '=')
    {
        strMsg.Append(szRTSPMsg);
        return strMsg;
    }

    szMsg = gos_left_trim_string(szMsg+1);

    CHAR    *szEnd = strstr(szMsg, "\r\n");
    if (!szEnd)
    {
        strMsg.Append(szRTSPMsg);
        return strMsg;
    }

    CHAR    *szEnd2 = strchr(szMsg, ';');

    if (szEnd2 && szEnd2 < szEnd)
    {
        szEnd = szEnd2;
    }

    UINT32      ulLen = szMsg-szRTSPMsg;

    strMsg.Append(szRTSPMsg, ulLen, FALSE);
    strMsg.Append(szNewValue);
    strMsg.Append(szEnd);
        
    return strMsg;
}

BOOL ReplaceRTSPClientPort(CONN_INFO_T *pstConnInfo, CHAR *szRTSPMsg)
{
    CHAR    *szMsg = strstr(szRTSPMsg, "client_port=");

    if (!szMsg)
    {
       return TRUE;
    }

    //GosLog(LOG_ERROR, "invalid SET msg %s", szMsg);
    //return FALSE;

    //Transport: RTP/AVP;unicast;client_port=9444-9445
    szMsg += strlen("client_port=");
    CHAR    *szEnd = strstr(szMsg, "\r\n");
    if (!szEnd)
    {
        GosLog(LOG_ERROR, "invalid msg from %s", szMsg);
        return FALSE;
    }

    CHAR    *szEnd2 = strchr(szMsg, ';');

    if (szEnd2 && szEnd2 < szEnd)
    {
        szEnd = szEnd2;
    }

    CHAR        acPort[32];            
    UINT32      ulLen = szEnd - szMsg;
    
    if (ulLen >= sizeof(acPort))
    {
        GosLog(LOG_ERROR, "invalid client_port len of %s", szMsg);
        return FALSE;
    }

    memcpy(acPort, szMsg, ulLen);
    acPort[ulLen] = '\0';

    if (!ParseRTSPPort(acPort, pstConnInfo->usClientRTPPort, pstConnInfo->usClientRTCPPort))
    {
        GosLog(LOG_ERROR, "parse rtp port %s failed", acPort);
        return FALSE;
    }

    GString     strTmp;

    ulLen = szMsg-szRTSPMsg;

    strTmp.Append(szRTSPMsg, ulLen, FALSE);
    strTmp.cat_sprintf("%u-%u", pstConnInfo->usLocalRTPPort, pstConnInfo->usLocalRTCPPort);
    strTmp.Append(szEnd);

    strcpy(szRTSPMsg, strTmp.GetString());

    return TRUE;
}


/*
"OPTIONS rtsp://192.168.90.223:554/h264/ch1/sub/av_stream RTSP/1.0
CSeq: 2
User-Agent: LibVLC/3.0.16 (LIVE555 Streaming Media v2016.11.28)"
*/
static VOID OnRTSPClientMsg(CONN_INFO_T *pstConnInfo, const CHAR *szCmd, CHAR *szRTSPMsg)//, UINT8 *pucRTSPServerAddr, UINT16 usRTSPServerPort)
{
    //替换地址、端口
    CHAR    acMsg[4096];
    CHAR    *szMsg;
    CHAR    *szEnd;
    UINT32  ulLen = 0;
    GString strMsg;

    if ((strlen(szRTSPMsg)+16) >= sizeof(acMsg))
    {
        return;
    }

    szMsg = szRTSPMsg+strlen(szCmd);
    szMsg = gos_left_trim_string(szMsg);
    if (!gos_str_like(szMsg, "rtsp://"))
    {
        return;
    }

    szMsg += strlen("rtsp://");
    ulLen = szMsg - szRTSPMsg;
    strMsg.Append(szRTSPMsg, ulLen, FALSE);

    szEnd = strchr(szMsg, '/');
    if (!szEnd)
    {
        return;
    }

    if (strcmp(szCmd, "OPTIONS") == 0)
    {
        GString     strAddr;
        UINT8       aucRTSPServerAddr[4];
        UINT16      usRTSPServerPort = 0;

        strAddr.Append(szMsg, szEnd-1-szMsg, FALSE);
        
        if (!gos_get_ip_port(strAddr.GetString(), aucRTSPServerAddr, &usRTSPServerPort))
        {
            GosLog(LOG_INFO, "invalid rtsp addr %s", strAddr.GetString());
            return;
        }

        if (usRTSPServerPort == 0)
        {
            usRTSPServerPort = g_usDefaultRTSPServerPort;
        }

        if (!ConvertRTSPAddr(pstConnInfo, aucRTSPServerAddr, usRTSPServerPort))
        {
            GosLog(LOG_ERROR, "unknown RTSP addr from client: "IP_FMT ":%u", IP_ARG(aucRTSPServerAddr), usRTSPServerPort);
            return;
        }

        if (g_ulConnectRTSPServerSuccTime == 0)
        {
            g_ulConnectRTSPServerSuccTime = gos_get_sys_uptime();
        }

        if (!ConnectToRTSPServer(pstConnInfo))
        {
            if ((gos_get_sys_uptime() - g_ulConnectRTSPServerSuccTime) >= g_ulMaxConnectRTSPServerTime)
            {
                GosLog(LOG_INFO, "failed to connect to RTSP Server in %u seconds", g_ulMaxConnectRTSPServerTime);
                CloseApp();
            }

            return;
        }

        g_ulConnectRTSPServerSuccTime = gos_get_sys_uptime();
    }

    if (pstConnInfo->usRTSPServerPort == g_usDefaultRTSPServerPort)
    {
        strMsg.cat_sprintf(IP_FMT, IP_ARG(pstConnInfo->aucRTSPServerAddr));
    }
    else
    {
        strMsg.cat_sprintf(IP_FMT ":%u", IP_ARG(pstConnInfo->aucRTSPServerAddr), pstConnInfo->usRTSPServerPort);
    }

    strMsg.Append(szEnd);

    strcpy(acMsg, strMsg);

    if (strcmp(szCmd, "SETUP") == 0)
    {
        if (!InitRTPSocket(pstConnInfo))
        {
            DelConnInfo(pstConnInfo);
            return;
        }       

        if (!ReplaceRTSPClientPort(pstConnInfo, acMsg))
        {
            return;
        }
    }

    if (strcmp(szCmd, "PLAY") == 0)
    {
        /*"PLAY rtsp://192.0.1.133/av_stream RTSP/1.0
        CSeq: 5
        User-Agent: LibVLC/3.0.16 (LIVE555 Streaming Media v2016.11.28)
        Session: 1210734934526142475  */

        if (!GetSIPValue(szRTSPMsg, "CSeq", &pstConnInfo->ulCSeq) ||
            !GetSIPValue(szRTSPMsg, "PLAY", pstConnInfo->acPlayParam, sizeof(pstConnInfo->acPlayParam)) ||
            !GetSIPValue(szRTSPMsg, "User-Agent", pstConnInfo->acUserAgent, sizeof(pstConnInfo->acUserAgent)) ||
            !GetSIPValue(szRTSPMsg, "Session", pstConnInfo->acSession, sizeof(pstConnInfo->acSession)))
        {
            GosLog(LOG_ERROR, "get parameter from PLAY msg failed");
        }
        else
        {
            GosLog(LOG_INFO, "get parameter from PLAY msg: CSeq: %u, User-Agent: %s, Session: %s",
                   pstConnInfo->ulCSeq, pstConnInfo->acUserAgent, pstConnInfo->acSession);
        }
    }

    gos_tcp_send(pstConnInfo->stLocalSocket, acMsg, strlen(acMsg));

    GosLog(LOG_INFO, "send msg to RTSP server: \n%s", acMsg);

    if (strcmp(szCmd, "TEARDOWN") == 0)
    {
        GosLog(LOG_INFO, "auto exit when tear down");
        CloseApp();
    }
}

VOID OnRTSPClientMsg(CONN_INFO_T *pstConnInfo, CHAR *szRTSPMsg)
{
    // OPTIONS rtsp://192.0.1.133:554/h264/ch1/sub/av_stream 
    // DESCRIBE rtsp://192.0.1.133:554/h264/ch1/sub/av_stream 
    // SETUP rtsp://192.0.1.133/av_stream/trackID=1 
    // PLAY rtsp://192.0.1.133/av_stream 
    // TEARDOWN rtsp://192.0.1.133/av_stream 
    // GET_PARAMETER ：rtsp://192.0.1.150:33333/av_stream RTSP/1.0

    const CHAR      *aszCmds[] = {"OPTIONS", "DESCRIBE", "GET_PARAMETER", "SETUP", "PLAY", "TEARDOWN"};

    GosLog(LOG_INFO, "recv RTSP client msg: \n%s", szRTSPMsg);

    g_ulLastRTSPClientTime = gos_get_uptime();

    for (UINT32 i=0; i<ARRAY_SIZE(aszCmds); i++)
    {
        if (gos_str_like(szRTSPMsg, aszCmds[i]) )
        {
            strcpy(g_acCurrRTSPCmd, aszCmds[i]);
            OnRTSPClientMsg(pstConnInfo, aszCmds[i], szRTSPMsg);
            return;
        }
    }

    GosLog(LOG_ERROR, "unknown client msg: %s", szRTSPMsg);
}

BOOL ParseRTSPMsg(CONN_INFO_T *pstConnInfo, CHAR **pszRTSPMsg)
{
    CHAR    *szMsgHdr = pstConnInfo->szRecvBuf;
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
        if (!GetSIPValue(szLen, "Content-Length", &ulMsgBodyLen) ||
            ulMsgBodyLen == 0)
        {

            return FALSE;
        }

        if ((ulMsgHdrLen + ulMsgBodyLen) < pstConnInfo->ulRecvSize)
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

    pstConnInfo->ulRecvSize -= ulMsgLen;
    if (pstConnInfo->ulRecvSize > 0)
    {
        memmove(pstConnInfo->szRecvBuf, pstConnInfo->szRecvBuf+ulMsgLen, pstConnInfo->ulRecvSize);
    }

    return TRUE;
}

VOID OnRTSPClientRTPMsg(CONN_INFO_T *pstConnInfo)
{
    gos_tcp_send(pstConnInfo->stLocalSocket, pstConnInfo->szRecvBuf, pstConnInfo->ulRecvSize);
    pstConnInfo->ulRecvSize = 0;
}

INT32 RecvRTSPClientMsg(SOCKET stSocket, INT32 *piErrCode)
{
    INT32           iRecvSize;
    UINT32          ulTotalLen;
    UINT32          ulByteToRecved = 0;
    CHAR            *szRTSPMsg;
    CONN_INFO_T     *pstConnInfo = &g_stRTSPClientConnInfo;//(stSocket);

    if (!pstConnInfo)
    {
        *piErrCode = 0;// SECONNRESET;
        return 0;
    }

#if 1
    /* 看看有多少字节可读 */
    if (ioctlsocket (stSocket, FIONREAD, (unsigned long*)&ulByteToRecved) == SOCKET_ERROR)
    {
        GosLog(LOG_ERROR, "server_recv_msg: ioctl socket(%u) failed!", stSocket);
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

    /* 开始处理该连接上的数据 */
    ulTotalLen = 0;

    /* 处理上次RecvMsg的剩余数据 */
    iRecvSize = recv(stSocket, (CHAR*)(pstConnInfo->szRecvBuf + pstConnInfo->ulRecvSize), pstConnInfo->ulMaxRecvBufSize - pstConnInfo->ulRecvSize, 0);
    if (iRecvSize < 0)
    {
        *piErrCode = -1;
        GosLog(LOG_ERROR, "recv client(%u) failed: %s!", stSocket, gos_get_socket_err());

        return -1;
    }
    else if (iRecvSize == 0)
    {
        GosLog(LOG_ERROR, "recv client(%u) failed by connect closed", stSocket);

        return 0;
    }

    pstConnInfo->ulRecvSize += iRecvSize;
    ulTotalLen  = pstConnInfo->ulRecvSize;
   // pucMsgStart = pstConnInfo->pucRecvBuf + pstConnInfo->ulRecvOffset;

    while(1)
    {
        pstConnInfo->szRecvBuf[pstConnInfo->ulRecvSize] = '\0';
        if (pstConnInfo->ulRecvSize == 0)
        {
            break;
        }

        if (!IsRTSPMsg(pstConnInfo->szRecvBuf, pstConnInfo->ulRecvSize))
        {
            OnRTSPClientRTPMsg(pstConnInfo);
            continue;
        }

        if (!ParseRTSPMsg(pstConnInfo, &szRTSPMsg))
        {
            return -1;
        }

        if (!szRTSPMsg)
        {
            break;
        }

        OnRTSPClientMsg(pstConnInfo, szRTSPMsg);
        free(szRTSPMsg);
    }

    if (IsRTSPInterleavedMsg(pstConnInfo->szRecvBuf, pstConnInfo->ulRecvSize))
    {
        GosLog(LOG_INFO, "recv client rtsp interleaved msg");
    }
    else
    {
        if (pstConnInfo->ulRecvSize > 0)
        {
            if (IsRTSPMsg(pstConnInfo->szRecvBuf, pstConnInfo->ulRecvSize))
            {
                GosLog(LOG_INFO, "recv client msg %s", pstConnInfo->szRecvBuf);
            }
            else
            {
                GosLog(LOG_INFO, "recv unknown client msg len %u", pstConnInfo->ulRecvSize);
            }
        }
    }

    *piErrCode = 0;
    return iRecvSize;
}
