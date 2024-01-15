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

CHAR        g_acCurrRTSPCmd[32] = { 0 };
UINT32      g_ulMaxConnectRTSPServerTime = 30;      // 超过此时间未成功连接到RTSP Server，自动退出

SOCKET      g_stLocalRTSPServerSock = INVALID_SOCKET;       // 提供给客户端的本地RTSP服务器



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



/*******************************************************************************
* 函数名称: GetSIPValue
* 功    能: 获取报文键值
* 参    数:
* 参数名称              输入/输出   描述
* szSIPText              IN         文本信息
* szKey                  IN         键值名字
* szValue                IN         键值值缓冲区
* ulMaxLen               IN         最大报文长度
* 函数返回:BOOL
* 说    明:
*******************************************************************************/
BOOL GetSIPValue(CHAR* szSIPText, const CHAR* szKey, UINT32* pulValue)
{
    CHAR    acValue[16];
    CHAR*   szValue= acValue;
    CHAR*   szStart = strstr(szSIPText, szKey); // 在SIP文本中查找键的起始位置
    CHAR*   szEnd;
    UINT32  ulLen;
    UINT32  ulMaxLen = sizeof(acValue);

    if (!szStart)
    {
        return FALSE; // 如果找不到键的起始位置，则返回FALSE表示提取失败
    }

    szStart += strlen(szKey) + 1; // 将起始位置向后移动到等号之后的位置
    szEnd = strstr(szStart, "\r\n"); // 在起始位置之后查找换行符的位置，表示值的结束位置
    if (!szEnd)
    {
        return FALSE; // 如果找不到值的结束位置，则返回FALSE表示提取失败
    }

    ulLen = (UINT32)(szEnd - szStart); // 计算值的长度
    if (ulLen >= ulMaxLen)
    {
        return FALSE; // 如果值的长度大于等于最大长度，则返回FALSE表示提取失败
    }

    memcpy(szValue, szStart, ulLen); // 将值复制到指定的缓冲区中
    szValue[ulLen] = '\0'; // 在缓冲区的末尾添加字符串结束符

    return gos_atou(acValue, pulValue); // 返回TRUE表示成功提取值
}

BOOL ParseRTSPPort(CHAR* szPort, UINT16& usRTPPort, UINT16& usRTCPPort)
{
    CHAR* szPort2 = strchr(szPort, '-');
    if (!szPort2)
    {
        return FALSE;
    }
    *szPort2++ = '\0';
    return gos_get_short(szPort, &usRTPPort) &&
        gos_get_short(szPort2, &usRTCPPort);
}

GString ReplaceRTSPMsg(CHAR* szRTSPMsg, const CHAR* szKey, CHAR* szNewValue)
{
    GString strMsg;
    CHAR* szMsg = strstr(szRTSPMsg, szKey);

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

    szMsg = gos_left_trim_string(szMsg + 1);

    CHAR* szEnd = strstr(szMsg, "\r\n");
    if (!szEnd)
    {
        strMsg.Append(szRTSPMsg);
        return strMsg;
    }

    CHAR* szEnd2 = strchr(szMsg, ';');

    if (szEnd2 && szEnd2 < szEnd)
    {
        szEnd = szEnd2;
    }

    UINT32      ulLen = szMsg - szRTSPMsg;

    strMsg.Append(szRTSPMsg, ulLen, FALSE);
    strMsg.Append(szNewValue);
    strMsg.Append(szEnd);

    return strMsg;
}
