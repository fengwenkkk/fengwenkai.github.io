#include "g_include.h"
#include "ds_public.h"
#include "rtsp_util.h"
#include "rtsp_client.h"
#include "rtsp_server.h"

UINT16  g_usDefaultRTSPServerPort = DEFAULT_RTP_PORT;

UINT8   g_aucRTSPServerAddr[4] = {0,0,0,0};//{192,0,1,133};
UINT16  g_usRTSPServerPort = DEFAULT_RTP_PORT;

UINT8   g_aucLocalRTSPServerAddr[4] = {0,0,0,0};//{192,168,90,223};
UINT16  g_usLocalRTSPServerPort = 0;

FD_SET  g_fdsAll;

CONN_INFO_T     g_stRTSPClientConnInfo = {0};

SOCKET      g_stLocalRTSPServerSock = INVALID_SOCKET;       // 提供给客户端的本地RTSP服务器
CHAR        g_acCurrRTSPCmd[32] = {0};
UINT32      g_ulMaxConnectRTSPServerTime = 30;      // 超过此时间未成功连接到RTSP Server，自动退出
UINT32      g_ulRTSPClientTimeout = 3600*24;        // 超过此时间未收到客户端的RTSP消息时，自动退出
UINT32      g_ulRTSPClientConnectTimeout = 30;  // 超过此时间客户端未成功连接到RTSP Server，自动退出

UINT32      g_ulConnectRTSPServerSuccTime = 0;


static UINT32  g_ulLowerUDPPort = 36500;
static UINT32  g_ulUpperUDPPort = 0xffff;

// 不允许reuseraddr，保证端口没有被其他应用使用
SOCKET CreateUDPServerSocket(UINT8 *pucAddr, UINT16 usPort)
{
    SOCKET              stSocket;
    SOCKADDR_IN         stLocalAddr = {0};
    UINT32              ulFlag = 1;

    gos_init_socket();

    stSocket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (stSocket == INVALID_SOCKET)
    {
        return INVALID_SOCKET;
    }

    memset(&stLocalAddr, 0, sizeof(stLocalAddr));
    stLocalAddr.sin_family = PF_INET;
    stLocalAddr.sin_port   = htons(usPort);
    if (pucAddr)
    {
        memcpy(&stLocalAddr.sin_addr.s_addr, pucAddr, 4);
    }

  /*  if (setsockopt(stSocket, SOL_SOCKET, SO_REUSEADDR, (CHAR*)&ulFlag, sizeof(ulFlag)) == SOCKET_ERROR)
    {
        CLOSE_SOCKET(stSocket);

        return INVALID_SOCKET;
    }*/

    if (bind (stSocket, (SOCKADDR *) &stLocalAddr, sizeof (SOCKADDR)) == SOCKET_ERROR)
    {
        CLOSE_SOCKET (stSocket);

        return INVALID_SOCKET;
    }

    return stSocket;
}

// 创建本地RTP\RTCP UDP SOCKET
BOOL InitRTPSocket(CONN_INFO_T *pstConnInfo)
{
    UINT8       aucAddr[] = {0,0,0,0};

    for (UINT32 ulPort=g_ulLowerUDPPort; ulPort<g_ulUpperUDPPort; ulPort+=2)
    {
        pstConnInfo->stLocalRTPSocket = CreateUDPServerSocket(aucAddr, ulPort);
        if (pstConnInfo->stLocalRTPSocket == INVALID_SOCKET)
        {
            continue;
        }

        pstConnInfo->stLocalRTCPSocket = CreateUDPServerSocket(aucAddr, ulPort+1);
        if (pstConnInfo->stLocalRTCPSocket == INVALID_SOCKET)
        {
            CLOSE_SOCKET(pstConnInfo->stLocalRTPSocket);
            continue;
        }

        pstConnInfo->usLocalRTPPort = ulPort;
        pstConnInfo->usLocalRTCPPort = ulPort+1;

        GosLog(LOG_INFO, "create rtp socket succ on port %u and %u", pstConnInfo->usLocalRTPPort, pstConnInfo->usLocalRTCPPort);

        FD_SET(pstConnInfo->stLocalRTPSocket, &g_fdsAll);
        FD_SET(pstConnInfo->stLocalRTCPSocket, &g_fdsAll);        

        return TRUE;
    }
     
    GosLog(LOG_ERROR, "create rtp socket failed");
    return FALSE;
}

VOID NewConnInfo(SOCKET stClientSocket, SOCKADDR_IN &stClientAddr)
{
    CONN_INFO_T     stConnInfo;
    UINT32          ulBufSize = 64*1024;

    memset(&stConnInfo, 0, sizeof(stConnInfo));
    stConnInfo.bValid = TRUE;
    stConnInfo.ulMaxRecvBufSize = ulBufSize-1;
    stConnInfo.ulMaxLocalRecvBufSize = ulBufSize-1;
    stConnInfo.ulMaxLocalRTPRecvBufSize = ulBufSize-1;
    stConnInfo.ulMaxLocalRTCPRecvBufSize = ulBufSize-1;

    stConnInfo.stClientSocket = stClientSocket;
    stConnInfo.stLocalSocket = INVALID_SOCKET;
    stConnInfo.stLocalRTPSocket = INVALID_SOCKET;
    stConnInfo.stLocalRTCPSocket = INVALID_SOCKET;

    memcpy(&stConnInfo.stClientAddr, &stClientAddr, sizeof(stClientAddr));

    stConnInfo.szRecvBuf = (CHAR*)calloc(1, stConnInfo.ulMaxRecvBufSize+1+stConnInfo.ulMaxLocalRecvBufSize+1+stConnInfo.ulMaxLocalRTPRecvBufSize+1+stConnInfo.ulMaxLocalRTCPRecvBufSize+1);
    stConnInfo.szLocalRecvBuf = stConnInfo.szRecvBuf+stConnInfo.ulMaxRecvBufSize+1;
    stConnInfo.pucLocalRTPRecvBuf = (UINT8*)(stConnInfo.szLocalRecvBuf+stConnInfo.ulMaxLocalRecvBufSize+1);
    stConnInfo.pucLocalRTCPRecvBuf = stConnInfo.pucLocalRTPRecvBuf+stConnInfo.ulMaxLocalRTPRecvBufSize+1;
   
    if (g_stRTSPClientConnInfo.bValid)
    {
        FD_CLR(g_stRTSPClientConnInfo.stClientSocket, &g_fdsAll);

        CLOSE_SOCKET(g_stRTSPClientConnInfo.stClientSocket);
       // CLOSE_SOCKET(g_stRTSPClientConnInfo.stLocalSocket);
       // CLOSE_SOCKET(g_stRTSPClientConnInfo.stLocalRTPSocket);
       // CLOSE_SOCKET(g_stRTSPClientConnInfo.stLocalRTCPSocket);

        GOS_FREE(g_stRTSPClientConnInfo.szRecvBuf);
    }

    memcpy(&g_stRTSPClientConnInfo, &stConnInfo, sizeof(stConnInfo));
   
    FD_SET(stClientSocket, &g_fdsAll);
    //FD_SET(pstConnInfo->stLocalSocket, &g_fdsAll);
}

BOOL ConnectToRTSPServer(CONN_INFO_T *pstConnInfo)
{
    if (pstConnInfo->stLocalSocket != INVALID_SOCKET)
    {
        return TRUE;
    }

    pstConnInfo->stLocalSocket = InitRTSPClientSocket(pstConnInfo->aucRTSPServerAddr, pstConnInfo->usRTSPServerPort);
    if (pstConnInfo->stLocalSocket == INVALID_SOCKET)
    {
        GosLog(LOG_ERROR, "init RTSP client socket failed");
        return FALSE;
    }
    
    FD_SET(pstConnInfo->stLocalSocket, &g_fdsAll);

    GosLog(LOG_INFO, "create RTSP client socket %u", pstConnInfo->stLocalSocket);

    return TRUE;
}

// 将客户端请求中的rtsp流地址，替换成真正的流地址（在客户端请求的源地址、端口与真正的源地址、端口需要建立一个映射表），目前缺省只有一个目的地址
BOOL ConvertRTSPAddr(CONN_INFO_T *pstConnInfo, UINT8 *pucRTSPServerAddr, UINT16 usRTSPServerPort)
{
    // TODO 目前写死
    memcpy(pstConnInfo->aucRTSPServerAddr, g_aucRTSPServerAddr, sizeof(g_aucRTSPServerAddr));
    pstConnInfo->usRTSPServerPort =  g_usRTSPServerPort; //g_usLocalRTSPServerPort;

    return TRUE;
}

VOID DelConnInfo(CONN_INFO_T *pstConnInfo)
{
    SOCKET      stClientSocket = pstConnInfo->stClientSocket;

    GosLog(LOG_INFO, "close client socket %u", stClientSocket);

    CLOSE_SOCKET(pstConnInfo->stClientSocket);
    CLOSE_SOCKET(pstConnInfo->stLocalSocket);
    CLOSE_SOCKET(pstConnInfo->stLocalRTPSocket);
    CLOSE_SOCKET(pstConnInfo->stLocalRTCPSocket);

    GOS_FREE(pstConnInfo->szRecvBuf);
}

/*
CONN_INFO_T* GetConnInfo(SOCKET stClientSocket)
{
    if (g_stRTSPClientConnInfo.stClientSocket == stClientSocket)
    {
        return &g_stRTSPClientConnInfo;
    }

    return NULL;
}

CONN_INFO_T* GetConnInfoByLocalSocket(SOCKET stLocalSocket)
{   
    if (g_stRTSPClientConnInfo.stLocalSocket == stLocalSocket)
    {
        return &g_stRTSPClientConnInfo;
    }

    return NULL;
}

CONN_INFO_T* GetConnToRecvLocalSocket(FD_SET &fds)
{
    for (UINT32 i=0; i<g_vRTSPClientConnInfo.Size(); i++)
    {
        if (FD_ISSET(g_vRTSPClientConnInfo[i]->stLocalSocket, &fds))
        {
            return g_vRTSPClientConnInfo[i];
        }
    }

    return NULL;
}

CONN_INFO_T* GetConnToRecvLocalRTPSocket(FD_SET &fds)
{
    for (UINT32 i=0; i<g_vRTSPClientConnInfo.Size(); i++)
    {
        if (FD_ISSET(g_vRTSPClientConnInfo[i]->stLocalRTPSocket, &fds))
        {
            return g_vRTSPClientConnInfo[i];
        }
    }

    return NULL;
}

CONN_INFO_T* GetConnToRecvLocalRTCPSocket(FD_SET &fds)
{
    for (UINT32 i=0; i<g_vRTSPClientConnInfo.Size(); i++)
    {
        if (FD_ISSET(g_vRTSPClientConnInfo[i]->stLocalRTCPSocket, &fds))
        {
            return g_vRTSPClientConnInfo[i];
        }
    }

    return NULL;
}
*/

BOOL GetSIPValue(CHAR *szSIPText, const CHAR *szKey, CHAR *szValue, UINT32 ulMaxLen)
{
    CHAR    *szStart = strstr(szSIPText, szKey);
    CHAR    *szEnd;
    UINT32  ulLen;

    if (!szStart)
    {
        return FALSE;
    }

    szStart += strlen(szKey)+1; // 1=strlen("=")
    szEnd = strstr(szStart, "\r\n");
    if (!szEnd)
    {
        return FALSE;
    }

    ulLen = (UINT32)(szEnd -szStart);
    if (ulLen >= ulMaxLen)
    {
        return FALSE;
    }

    memcpy(szValue, szStart, ulLen);
    szValue[ulLen] = '\0';

    return TRUE;
}

BOOL GetSIPValue(CHAR *szSIPText, const CHAR *szKey, UINT32 *pulValue)
{
    CHAR    acValue[16];

    if (!GetSIPValue(szSIPText, szKey, acValue, sizeof(acValue)))
    {
        return FALSE;
    }

    return gos_atou(acValue, pulValue);
}

BOOL ParseRTSPPort(CHAR *szPort, UINT16 &usRTPPort, UINT16 &usRTCPPort)
{
    CHAR    *szPort2 = strchr(szPort, '-');

    if (!szPort2)
    {
        return FALSE;
    }

    *szPort2++ = '\0';

    return gos_get_short(szPort, &usRTPPort) &&
           gos_get_short(szPort2, &usRTCPPort);
}

BOOL IsRTSPInterleavedMsg(CHAR *szMsg, UINT32 ulLen)
{
    if (ulLen < RTSP_INTERLEAVED_MAG_HDR_LEN)
    {
        return FALSE;
    }

    if (RTSP_INTERLEAVED_MAGIC == ((UINT8)szMsg[0]))
    {
        return TRUE;
    }

    return FALSE;
}

BOOL IsRTSPMsg(CHAR *szMsg, UINT32 ulLen)
{
    if (ulLen < RTSP_INTERLEAVED_MAG_HDR_LEN)
    {
        return FALSE;
    }

    if (IsRTSPInterleavedMsg(szMsg, ulLen))
    {
        // RTP消息
        return FALSE;
    }

    for (UINT32 i=0; i<RTSP_INTERLEAVED_MAG_HDR_LEN; i++)
    {
        if (!isascii(szMsg[i]))
        {
            return FALSE;
        }
    }

    return TRUE;
}

VOID CloseApp()
{
    gos_sleep_1ms(500);
    CLOSE_SOCKET(g_stLocalRTSPServerSock);
    exit(0);
}