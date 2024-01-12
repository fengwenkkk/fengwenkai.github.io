#include "g_include.h"
#include "rtsp_util.h"
#include "rtsp_client.h"

INT32 RecvRTPMsg(CONN_INFO_T *pstConnInfo)
{
    SOCKADDR_IN     stAddr;
    socklen_t       iLen = sizeof(stAddr);

    INT32   iRecvLen = recvfrom(pstConnInfo->stLocalRTPSocket, (CHAR*)pstConnInfo->pucLocalRTPRecvBuf, pstConnInfo->ulMaxLocalRTPRecvBufSize, 0, (SOCKADDR *)&stAddr, &iLen);

    stAddr.sin_port = ntohs(stAddr.sin_port);
    if (iRecvLen <= 0)
    {
        return iRecvLen;
    }

    // RTSP server -> local
    if (memcmp(&stAddr.sin_addr, pstConnInfo->aucRTSPServerAddr, 4) == 0)
    {
        if (pstConnInfo->usServerRTPPort != 0 &&  // 如果setup的应答消息没有server_port,则usServerRTPPort 为0
            stAddr.sin_port != pstConnInfo->usServerRTPPort)
        {
            return iRecvLen;
        }
        
        SOCKADDR_IN             stClientAddr;                       // 接收到的RTSP客户端地址

        memcpy(&stClientAddr, &pstConnInfo->stClientAddr, sizeof(stClientAddr));
        stClientAddr.sin_port = htons(pstConnInfo->usClientRTPPort);

        // local -> RTSP client
        sendto(pstConnInfo->stLocalRTPSocket, (CHAR*)pstConnInfo->pucLocalRTPRecvBuf, iRecvLen, 0,(SOCKADDR *)&stClientAddr, sizeof(SOCKADDR));

        return iRecvLen;
    }

    // RTSP client -> Local
    if (memcmp(&stAddr.sin_addr, &pstConnInfo->stClientAddr.sin_addr, 4) == 0)
    {
        if (pstConnInfo->usClientRTPPort != 0 &&
            stAddr.sin_port != pstConnInfo->usClientRTPPort)
        {
            return iRecvLen;
        }

        SOCKADDR_IN             stServerAddr;                       // 接收到的RTSP客户端地址

        memcpy(&stServerAddr, &pstConnInfo->stClientAddr, sizeof(stServerAddr));
        stServerAddr.sin_port = htons(pstConnInfo->usServerRTPPort);

        // local -> RTSP server
        sendto(pstConnInfo->stLocalRTPSocket, (CHAR*)pstConnInfo->pucLocalRTPRecvBuf, iRecvLen, 0,(SOCKADDR *)&stServerAddr, sizeof(SOCKADDR));

        return iRecvLen;
    }

    return iRecvLen;
}

INT32 RecvRTCPMsg(CONN_INFO_T *pstConnInfo)
{
    SOCKADDR_IN     stAddr;
    socklen_t       iLen = sizeof(stAddr);

    INT32   iRecvLen = recvfrom(pstConnInfo->stLocalRTCPSocket, (CHAR*)pstConnInfo->pucLocalRTCPRecvBuf, pstConnInfo->ulMaxLocalRTCPRecvBufSize, 0, (SOCKADDR *)&stAddr, &iLen);

    stAddr.sin_port = ntohs(stAddr.sin_port);
    if (iRecvLen <= 0)
    {
        return iRecvLen;
    }

    // RTSP server -> local
    if (memcmp(&stAddr.sin_addr, pstConnInfo->aucRTSPServerAddr, 4) == 0)
    {
        if (stAddr.sin_port != pstConnInfo->usServerRTCPPort)
        {
            return iRecvLen;
        }
        
        SOCKADDR_IN             stClientAddr;                       // 接收到的RTSP客户端地址

        memcpy(&stClientAddr, &pstConnInfo->stClientAddr, sizeof(stClientAddr));
        stClientAddr.sin_port = htons(pstConnInfo->usClientRTCPPort);

        // local -> RTSP client
        sendto(pstConnInfo->stLocalRTCPSocket, (CHAR*)pstConnInfo->pucLocalRTCPRecvBuf, iRecvLen, 0,(SOCKADDR *)&stClientAddr, sizeof(SOCKADDR));

        return iRecvLen;
    }

    // RTSP client -> Local
    if (memcmp(&stAddr.sin_addr, &pstConnInfo->stClientAddr.sin_addr, 4) == 0)
    {
        if (stAddr.sin_port != pstConnInfo->usClientRTCPPort)
        {
            return iRecvLen;
        }

        SOCKADDR_IN             stServerAddr;                       // 接收到的RTSP客户端地址

        memcpy(&stServerAddr, &pstConnInfo->stClientAddr, sizeof(stServerAddr));
        stServerAddr.sin_port = htons(pstConnInfo->usServerRTCPPort);

        // local -> RTSP server
        sendto(pstConnInfo->stLocalRTCPSocket, (CHAR*)pstConnInfo->pucLocalRTCPRecvBuf, iRecvLen, 0,(SOCKADDR *)&stServerAddr, sizeof(SOCKADDR));

        return iRecvLen;
    }

    return iRecvLen;
}
