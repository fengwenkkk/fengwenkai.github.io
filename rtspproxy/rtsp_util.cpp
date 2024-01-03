#include "g_include.h"
#include "ds_public.h"
#include "rtsp.h"
#include "rtsp_util.h"
//#include "rtsp_client.h"
#include "rtsp_server.h"

//全局变量设置
UINT32      g_ulRTSPClientTimeout = 3600 * 24;
UINT32      g_ulRTSPClientConnectTimeout = 30;
UINT8       g_aucRTSPServerAddr[4] = { 0,0,0,0 };//{192,0,1,203};
UINT16      g_usRTSPServerPort = DEFAULT_RTP_PORT;//554
UINT8       g_aucLocalRTSPServerAddr[4] = { 0,0,0,0 };//{192,0,1,203};
UINT16      g_usLocalRTSPServerPort = 0;
UINT16      g_usDefaultRTSPServerPort = DEFAULT_RTP_PORT;
FD_SET      g_fdsAll;

CONN_INFO_T     g_stRTSPClientConnInfo = { 0 };
UINT32          g_ulConnectRTSPServerSuccTime = 0;

static UINT32  g_ulLowerUDPPort = 36500;
static UINT32  g_ulUpperUDPPort = 0xffff;


/*******************************************************************************
* 函数名称: NewConnInfo
* 功    能: 记录新的接收端信息，用于日志记录
* 参    数:
* 参数名称              输入/输出   描述
* stClientSocket        IN          accept的客户端套接字
* stClientAddr          IN          客户端地址
* 函数返回:VOID
* 说    明:
*******************************************************************************/
VOID NewConnInfo(SOCKET stClientSocket, SOCKADDR_IN& stClientAddr)
{
    CONN_INFO_T     stConnInfo;
    UINT32          ulBufSize = 64 * 1024;
    
    memset(&stConnInfo, 0, sizeof(stConnInfo));
    stConnInfo.bValid = TRUE;
    stConnInfo.ulMaxRecvBufSize = ulBufSize - 1;
    stConnInfo.ulMaxLocalRecvBufSize = ulBufSize - 1;
    stConnInfo.ulMaxLocalRTPRecvBufSize = ulBufSize - 1;
    stConnInfo.ulMaxLocalRTCPRecvBufSize = ulBufSize - 1;

    stConnInfo.stClientSocket = stClientSocket;
    stConnInfo.stLocalSocket = INVALID_SOCKET;
    stConnInfo.stLocalRTPSocket = INVALID_SOCKET;
    stConnInfo.stLocalRTCPSocket = INVALID_SOCKET;

    memcpy(&stConnInfo.stClientAddr, &stClientAddr, sizeof(stClientAddr));

    stConnInfo.szRecvBuf = (CHAR*)calloc(1, stConnInfo.ulMaxRecvBufSize + 1 + stConnInfo.ulMaxLocalRecvBufSize + 1 + stConnInfo.ulMaxLocalRTPRecvBufSize + 1 + stConnInfo.ulMaxLocalRTCPRecvBufSize + 1);
    stConnInfo.szLocalRecvBuf = stConnInfo.szRecvBuf + stConnInfo.ulMaxRecvBufSize + 1;
    stConnInfo.pucLocalRTPRecvBuf = (UINT8*)(stConnInfo.szLocalRecvBuf + stConnInfo.ulMaxLocalRecvBufSize + 1);
    stConnInfo.pucLocalRTCPRecvBuf = stConnInfo.pucLocalRTPRecvBuf + stConnInfo.ulMaxLocalRTPRecvBufSize + 1;
    
    if (g_stRTSPClientConnInfo.bValid)
    {
        FD_CLR(g_stRTSPClientConnInfo.stClientSocket, &g_fdsAll);
        CLOSE_SOCKET(g_stRTSPClientConnInfo.stClientSocket);
        GOS_FREE(g_stRTSPClientConnInfo.szRecvBuf);
    }
    memcpy(&g_stRTSPClientConnInfo, &stConnInfo, sizeof(stConnInfo));

    FD_SET(stClientSocket, &g_fdsAll);
}

VOID CloseApp()
{
    gos_sleep_1ms(500);
    CLOSE_SOCKET(g_stLocalRTSPServerSock);
    exit(0);
}
