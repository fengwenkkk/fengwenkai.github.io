#include "g_include.h"
#include "rtsp_util.h"
#include "rtp.h"
#include "rtsp_client.h"
#include "rtsp_server.h"

SOCKET  g_stLocalRTSPServerSocket = INVALID_SOCKET;       //�ṩ���ͻ��˵ı���RTSP������

/*******************************************************************************
* ��������: InitLocalRTSPServerSocket
* ��    ��: �������ط�����������Ϣ
* ��    ��:
* ��������              ����/���   ����
* pucAddr               IN          ��������ַ
* usPort                IN          �����˿�
* ��������:SOCKET
* ˵    ��:���󷵻�INVALID_SOCKET��û�д��󷵻��׽��ֵ�ֵ
*******************************************************************************/
SOCKET InitLocalRTSPServerSocket(UINT8* pucAddr, UINT16 usPort)
{
    SOCKET              stSocket = INVALID_SOCKET;
    SOCKADDR_IN         stLocalAddr = { 0 };
    UINT32              ulOn = 1;
    
    stLocalAddr.sin_family = PF_INET;
    stLocalAddr.sin_port = htons(usPort);
    memcpy(&stLocalAddr.sin_addr.s_addr, pucAddr, 4);
    
    stSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (stSocket == INVALID_SOCKET)
    {
        GosLog(LOG_ERROR, "Local rtsp server socket failed:error(%s)", gos_get_socket_err());
        return INVALID_SOCKET;
    }
    
    if (setsockopt(stSocket, SOL_SOCKET, SO_REUSEADDR, (CHAR*)&ulOn, sizeof(ulOn)) == SOCKET_ERROR)//������
    {
        GosLog(LOG_ERROR, "Set local rtsp server failed on setsockopt,error(%s)", gos_get_socket_err());
        CLOSE_SOCKET(stSocket);
        return INVALID_SOCKET;
    }
    
    if (bind(stSocket, (SOCKADDR*)&stLocalAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
        GosLog(LOG_ERROR, "Bind local rtps server socket failed on " IP_FMT ":%u, %s", IP_ARG(pucAddr), usPort, gos_get_socket_err());
        CLOSE_SOCKET(stSocket);
        return INVALID_SOCKET;
    }
   
    //����
    if (listen(stSocket, 16) == SOCKET_ERROR)
    {
        GosLog(LOG_ERROR, "listen local rtps server socket failed, error(%s)", gos_get_socket_err());
        CLOSE_SOCKET(stSocket);
        return INVALID_SOCKET;
    }
   
    g_stLocalRTSPServerSocket = stSocket;
    
    return stSocket;
}

VOID OnRTSPClientRTPMsg(CONN_INFO_T* pstConnInfo)
{
    gos_tcp_send(pstConnInfo->stLocalSocket, pstConnInfo->szRecvBuf, pstConnInfo->ulRecvSize);
    pstConnInfo->ulRecvSize = 0;
}

BOOL ParseRTSPMsg(CONN_INFO_T* pstConnInfo, CHAR** pszRTSPMsg)
{
    CHAR* szMsgHdr = pstConnInfo->szRecvBuf;
    CHAR* szMsgBody = NULL;
    CHAR* szEnd;
    CHAR* szLen = 0;  
    UINT32  ulMsgHdrLen = 0;
    UINT32  ulMsgBodyLen = 0;
    UINT32  ulMsgLen = 0;
    CHAR* szRTSPMsg;

    *pszRTSPMsg = NULL;

    szEnd = strstr(szMsgHdr, "\r\n\r\n");
    if (!szEnd)
    {
        return TRUE;
    }

    szMsgBody = szEnd + 4;
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
    szRTSPMsg = (CHAR*)calloc(1, ulMsgLen + 1);
    if (!szRTSPMsg)
    {
        return FALSE;
    }

    memcpy(szRTSPMsg, szMsgHdr, ulMsgLen);
    *pszRTSPMsg = szRTSPMsg;

    pstConnInfo->ulRecvSize -= ulMsgLen;
    if (pstConnInfo->ulRecvSize > 0)
    {
        memmove(pstConnInfo->szRecvBuf, pstConnInfo->szRecvBuf + ulMsgLen, pstConnInfo->ulRecvSize);
    }

    return TRUE;
}

/*******************************************************************************
* ��������: ReplaceRTSPClientPort
* ��    ��: �������ط�����������Ϣ
* ��    ��:
* ��������              ����/���   ����
* pstConnInfo            IN         �ṹ��
* szRTSPMsg              IN         ��Ϣ
* ��������:BOOL
* ˵    ��: 
*******************************************************************************/
static BOOL ReplaceRTSPClientPort(CONN_INFO_T* pstConnInfo, CHAR* szRTSPMsg)
{
    GString strTmp;
    CHAR*   szMsg = strstr(szRTSPMsg, "client_port=");
    CHAR    acPort[32];

    if (!szMsg)
    {
        return TRUE;
    }

    szMsg += strlen("client_port=");
    CHAR* szEnd = strstr(szMsg, "\r\n");
    if (!szEnd)
    {
        GosLog(LOG_ERROR, "invalid msg from %s", szMsg);
        return FALSE;
    }

    CHAR* szEnd2 = strchr(szMsg, ';');
    if (szEnd2 && szEnd2 < szEnd)
    {
        szEnd = szEnd2;
    }

    UINT32 ulLen = szEnd - szMsg;
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

    ulLen = szMsg - szRTSPMsg;
    strTmp.Append(szRTSPMsg, ulLen, FALSE);
    strTmp.cat_sprintf("%u-%u", pstConnInfo->usLocalRTPPort, pstConnInfo->usLocalRTCPPort);
    strTmp.Append(szEnd);
    //�滻ԭ����
    strcpy(szRTSPMsg, strTmp.GetString());
    return TRUE;
}

static VOID OnRTSPClientMsg(CONN_INFO_T* pstConnInfo, const CHAR* szCmd, CHAR* szRTSPMsg)
{

    GString strMsg;

    CHAR    acMsg[4096];
    CHAR*   szMsg;
    CHAR*   szEnd;
    UINT32  ulLen = 0;
    
    //�滻��ַ
    if ((strlen(szRTSPMsg) + 16) >= sizeof(acMsg))
    {
        return;
    }

    szMsg = szRTSPMsg + strlen(szCmd);
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

    //���������滻
    if (strcmp(szCmd, "OPTIONS") == 0)//����������
    {
        GString     strAddr;
        UINT8       aucRTSPServerAddr[4];
        UINT16      usRTSPServerPort = 0;

        strAddr.Append(szMsg, szEnd - 1 - szMsg, FALSE);

        if (!gos_get_ip_port(strAddr.GetString(), aucRTSPServerAddr, &usRTSPServerPort))
        {
            GosLog(LOG_INFO, "Invalid rtsp addr %s", strAddr.GetString());
            return;
        }

        if (usRTSPServerPort == 0)
        {
            usRTSPServerPort = g_usDefaultRTSPServerPort;
        }

        if (!ConvertRTSPAddr(pstConnInfo, aucRTSPServerAddr, usRTSPServerPort))
        {
            GosLog(LOG_ERROR, "unknown RTSP addr from client: " IP_FMT  ":%u", IP_ARG(aucRTSPServerAddr), usRTSPServerPort);
            return;
        }

        if (!ConnectToRTSPServer(pstConnInfo))
        {
            return;
        }
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
        //if (!InitRTPSocket(pstConnInfo))
        //{
        //    DelConnInfo(pstConnInfo);
        //    return;
        //}

        if (!ReplaceRTSPClientPort(pstConnInfo, acMsg))
        {
            return;
        }
    }

    if (strcmp(szCmd, "PLAY") == 0)
    {
        /*"PLAY rtsp://192.0.1.133/av_stream RTSP/1.0 */

        if (!GetSIPValue(szRTSPMsg, "CSeq", &pstConnInfo->ulCSeq) ||
            !GetSIPValueAndLen(szRTSPMsg, "PLAY", pstConnInfo->acPlayParam, sizeof(pstConnInfo->acPlayParam)) ||
            !GetSIPValueAndLen(szRTSPMsg, "User-Agent", pstConnInfo->acUserAgent, sizeof(pstConnInfo->acUserAgent)) ||
            !GetSIPValueAndLen(szRTSPMsg, "Session", pstConnInfo->acSession, sizeof(pstConnInfo->acSession)))
        {
            GosLog(LOG_ERROR, "get parameter from PLAY msg failed");
        }
        else
        {
            GosLog(LOG_INFO, "get parameter from PLAY msg: CSeq: %u, User-Agent: %s, Session: %s",
                pstConnInfo->ulCSeq, pstConnInfo->acUserAgent, pstConnInfo->acSession);
        }
    }

    //�������ؿͻ���
    gos_tcp_send(pstConnInfo->stLocalSocket, acMsg, strlen(acMsg));

    GosLog(LOG_INFO, "Socket[%d] end msg to RTSP Local server: \n%s", pstConnInfo->stLocalSocket, acMsg);

    if (strcmp(szCmd, "TEARDOWN") == 0)
    {
        GosLog(LOG_INFO, "Exit");
        CloseApp();
    }
}

/*******************************************************************************
* ��������: OnRTSPClientMsg
* ��    ��: ת������ת��
* ��    ��:
* ��������              ����/���   ����
* pstConnInfo            IN         
* szRTSPMsg              IN         ��Ϣ
* ��������:VOID
* ˵    ��:
*******************************************************************************/
VOID HandleClientRTSPMsg(CONN_INFO_T* pstConnInfo, CHAR* szRTSPMsg)
{
    // OPTIONS rtsp://192.0.1.133:554/h264/ch1/sub/av_stream 
    // DESCRIBE rtsp://192.0.1.133:554/h264/ch1/sub/av_stream 
    // SETUP rtsp://192.0.1.133/av_stream/trackID=1 
    // PLAY rtsp://192.0.1.133/av_stream 
    // TEARDOWN rtsp://192.0.1.133/av_stream 
    // GET_PARAMETER ��rtsp://192.0.1.150:33333/av_stream RTSP/1.0

    const CHAR* aszCmds[] = { "OPTIONS", "DESCRIBE", "GET_PARAMETER", "SETUP", "PLAY", "TEARDOWN" };

    GosLog(LOG_INFO, "Recv RTSP client msg: \n%s", szRTSPMsg);

    for (UINT32 i = 0; i < ARRAY_SIZE(aszCmds); i++)
    {
        //ƥ���ַ���


        if (gos_str_like(szRTSPMsg, aszCmds[i]))
        {
            OnRTSPClientMsg(pstConnInfo, aszCmds[i], szRTSPMsg);
            return ;
        }
    }

    GosLog(LOG_ERROR, "unknown client msg:\n %s", szRTSPMsg);
}

/*******************************************************************************
* ��������:  RecvRTSPClientMsg
* ��    ��: ���ط�����������Ϣ
* ��    ��:
* ��������              ����/���   ����
* stSocket              IN          �ͻ����׽���
* piErrCode             IN          ������
* ��������:INT32
* ˵    ��:0��û����Ϣ������socket.-1:�������󣬷���-1
*******************************************************************************/
INT32 RecvRTSPClientMsg(SOCKET stSocket, INT32* piErrCode)
{
    INT32           iRecvSize;
    UINT32          ulTotalLen;//��¼����
    UINT32          ulByteRecv = 0;
    CHAR*           szRTSPMsg;
    CONN_INFO_T*    pstConnInfo = &g_stRTSPClientConnInfo;

    if (!pstConnInfo)
    {
        *piErrCode = 0;
        return 0;
    }

    if (ioctlsocket(stSocket, FIONREAD, (unsigned long*)&ulByteRecv) == SOCKET_ERROR)
    {
        GosLog(LOG_ERROR, "Local socket(%u) recv client msg failed!", stSocket);
        *piErrCode = gos_get_socket_err_code();
        return -1;
    }

    if (ulByteRecv == 0)
    {
        *piErrCode = 0;
        return 0;
    }

    ulTotalLen = 0;

    iRecvSize = recv(stSocket, (CHAR*)(pstConnInfo->szRecvBuf + pstConnInfo->ulRecvSize), pstConnInfo->ulMaxRecvBufSize - pstConnInfo->ulRecvSize, 0);
    if (iRecvSize < 0)
    {
        *piErrCode = -1;
        GosLog(LOG_ERROR, "Recv client (%u) failed: %s!", stSocket, gos_get_socket_err());

        return -1;
    }
    else if (iRecvSize == 0)
    {
        GosLog(LOG_ERROR, "Recv client (%u) failed by connect closed", stSocket);

        return 0;
    }

    //���³���
    pstConnInfo->ulRecvSize += iRecvSize;
    ulTotalLen = pstConnInfo->ulRecvSize;

    //ѭ������
    while (1)
    {
        pstConnInfo->szRecvBuf[pstConnInfo->ulRecvSize] = '\0';
        if (pstConnInfo->ulRecvSize == 0)
        {
            break;
        }

        //�ָ����Ϣ
        if (!ParseRTSPMsg(pstConnInfo, &szRTSPMsg))
        {
            return -1;
        }

        if (!szRTSPMsg)
        {
            break;
        }

        HandleClientRTSPMsg(pstConnInfo, szRTSPMsg);

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