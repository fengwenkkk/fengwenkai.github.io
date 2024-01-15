#include "g_include.h"

SOCKET  g_stLocalRTSPServerSocket = INVALID_SOCKET;       // 提供给客户端的本地RTSP服务器

/*******************************************************************************
* 函数名称: InitLocalRTSPServerSocket
* 功    能: 建立本地服务器监听消息
* 参    数:
* 参数名称              输入/输出   描述
* pucAddr               IN          服务器地址
* usPort                IN          监听端口
* 函数返回:SOCKET
* 说    明:错误返回INVALID_SOCKET，没有错误返回套接字的值
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
    if (setsockopt(stSocket, SOL_SOCKET, SO_REUSEADDR, (CHAR*)&ulOn, sizeof(ulOn)) == SOCKET_ERROR)//非阻塞
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
    //监听
    if (listen(stSocket, 16) == SOCKET_ERROR)
    {
        GosLog(LOG_ERROR, "listen local rtps server socket failed, error(%s)", gos_get_socket_err());
        CLOSE_SOCKET(stSocket);
        return INVALID_SOCKET;
    }
    g_stLocalRTSPServerSocket = stSocket;
    return stSocket;
}

