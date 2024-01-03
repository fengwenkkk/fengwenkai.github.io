
#include "rtp.h"

/*******************************************************************************
* 函数名称: RecvRTPMsg
* 功    能: 接受RTP消息
* 参    数:
* 参数名称              输入/输出   描述
* pstConnInfo           IN          储存链接的客户端和服务器情况
* 函数返回:INT32
* 说    明:
*******************************************************************************/
INT32 RecvRTPMsg(CONN_INFO_T* pstConnInfo)
{
	SOCKADDR_IN     stRTPAddr;//储存接收源地址
	SOCKADDR_IN     stClientAddr;//RTSP客户端地址
    SOCKADDR_IN     stServerAddr; //接收到的RTSP客户端地址

	socklen_t       iLen = sizeof(stRTPAddr);
	INT32           iRecvLen;

	iRecvLen = recvfrom(pstConnInfo->stLocalRTPSocket, (CHAR*)pstConnInfo->pucLocalRTPRecvBuf, pstConnInfo->ulMaxLocalRTPRecvBufSize, 0, (SOCKADDR*)&stRTPAddr, &iLen);
	if (iRecvLen <= 0)
	{
		GosLog(LOG_ERROR, "LocalRTPSocket[%d] recv mesg length: %d ", pstConnInfo->stLocalRTPSocket, iRecvLen);
		return iRecvLen;
	}
	
	//判断RTP数据是否来自服务端，发送给本地服务器
	stRTPAddr.sin_port = ntohs(stRTPAddr.sin_port);
    if (memcmp(&stRTPAddr.sin_addr, pstConnInfo->aucRTSPServerAddr, 4) == 0)
    {
        if (pstConnInfo->usServerRTPPort != 0 && stRTPAddr.sin_port != pstConnInfo->usServerRTPPort) // 如果setup的应答消息没有server_port,则usServerRTPPort 为0
        {
            GosLog(LOG_ERROR, "ServerRTPPort error :%d / %d", pstConnInfo->usServerRTPPort, stRTPAddr.sin_port);
            return iRecvLen;
        }

        memcpy(&stClientAddr, &pstConnInfo->stClientAddr, sizeof(stClientAddr));
        stClientAddr.sin_port = htons(pstConnInfo->usClientRTPPort);

        sendto(pstConnInfo->stLocalRTPSocket, (CHAR*)pstConnInfo->pucLocalRTPRecvBuf, iRecvLen, 0, (SOCKADDR*)&stClientAddr, sizeof(SOCKADDR));
        GosLog(LOG_INFO, "Recv mesg from RTSPServer ,send mesg to LocalRTPSocket[%d] ,length: %d ", pstConnInfo->stLocalRTPSocket, iRecvLen);

        return iRecvLen;
    }

	//判断RTP数据是否来自客户端，发送给本地服务器
    if (memcmp(&stRTPAddr.sin_addr, &pstConnInfo->stClientAddr.sin_addr, 4) == 0)
    {
        if (pstConnInfo->usClientRTPPort != 0 &&
            stRTPAddr.sin_port != pstConnInfo->usClientRTPPort)
        {
            GosLog(LOG_ERROR, "ClientPort error :%d / %d", pstConnInfo->usClientRTPPort, stRTPAddr.sin_port);
            return iRecvLen;
        }

        memcpy(&stServerAddr, &pstConnInfo->stClientAddr, sizeof(stServerAddr));
        stServerAddr.sin_port = htons(pstConnInfo->usServerRTPPort);

        sendto(pstConnInfo->stLocalRTPSocket, (CHAR*)pstConnInfo->pucLocalRTPRecvBuf, iRecvLen, 0, (SOCKADDR*)&stServerAddr, sizeof(SOCKADDR));
        GosLog(LOG_INFO, "Recv mesg from RTSPClient ,send mesg to LocalRTPSocket[%d] ,length: %d ", pstConnInfo->stLocalRTPSocket, iRecvLen);
        return iRecvLen;
    }

    GosLog(LOG_INFO, "Recv mesg length: %d ",iRecvLen);
    return iRecvLen;

}


INT32 RecvRTCPMsg(CONN_INFO_T* pstConnInfo)
{
    SOCKADDR_IN     stRTCPAddr;
    socklen_t       iLen = sizeof(stRTCPAddr);
    INT32           iRecvLen;
    
    iRecvLen = recvfrom(pstConnInfo->stLocalRTCPSocket, (CHAR*)pstConnInfo->pucLocalRTCPRecvBuf, pstConnInfo->ulMaxLocalRTCPRecvBufSize, 0, (SOCKADDR*)&stRTCPAddr, &iLen);

   
    if (iRecvLen <= 0)
    {
        GosLog(LOG_ERROR, "LocalRTCPSocket[%d] recv mesg length: %d ", pstConnInfo->stLocalRTPSocket, iRecvLen);
        return iRecvLen;
    }

    // RTSP server -> local
    stRTCPAddr.sin_port = ntohs(stRTCPAddr.sin_port);
    if (memcmp(&stRTCPAddr.sin_addr, pstConnInfo->aucRTSPServerAddr, 4) == 0)
    {
        if (stRTCPAddr.sin_port != pstConnInfo->usServerRTCPPort)
        {
            GosLog(LOG_ERROR, "ServerRTCPPort error :%d / %d", pstConnInfo->usServerRTCPPort, stRTCPAddr.sin_port);
            return iRecvLen;
        }

        SOCKADDR_IN  stClientAddr;// 接收到的RTSP客户端地址

        memcpy(&stClientAddr, &pstConnInfo->stClientAddr, sizeof(stClientAddr));
        stClientAddr.sin_port = htons(pstConnInfo->usClientRTCPPort);

        sendto(pstConnInfo->stLocalRTCPSocket, (CHAR*)pstConnInfo->pucLocalRTCPRecvBuf, iRecvLen, 0, (SOCKADDR*)&stClientAddr, sizeof(SOCKADDR));

        return iRecvLen;
    }

    // RTSP client -> Local
    if (memcmp(&stRTCPAddr.sin_addr, &pstConnInfo->stClientAddr.sin_addr, 4) == 0)
    {
        if (stRTCPAddr.sin_port != pstConnInfo->usClientRTCPPort)
        {
            GosLog(LOG_ERROR, "ClientPort error :%d / %d", pstConnInfo->usClientRTCPPort, stRTCPAddr.sin_port);
            return iRecvLen;
        }

        SOCKADDR_IN  stServerAddr; // 接收到的RTSP客户端地址

        memcpy(&stServerAddr, &pstConnInfo->stClientAddr, sizeof(stServerAddr));
        stServerAddr.sin_port = htons(pstConnInfo->usServerRTCPPort);

        // local -> RTSP server
        sendto(pstConnInfo->stLocalRTCPSocket, (CHAR*)pstConnInfo->pucLocalRTCPRecvBuf, iRecvLen, 0, (SOCKADDR*)&stServerAddr, sizeof(SOCKADDR));

        return iRecvLen;
    }

    return iRecvLen;

}