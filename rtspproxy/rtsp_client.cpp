#include "rtsp_client.h"



BOOL ParseRTSPClientMsg(CONN_INFO_T* pstConnInfo)
{


}

/*******************************************************************************
* 函数名称: RecvRTSPClientMsg
* 功    能: 接受客户端RTSP消息
* 参    数:
* 参数名称              输入/输出   描述
* pstConnInfo           IN          客户端信息结构体
* piError               IN          错误码
* 函数返回:INT32
* 说    明:错误返回-1，数据0返回0，没有错误返回
*******************************************************************************/
INT32 RecvRTSPClientMsg(CONN_INFO_T* pstConnInfo,INT32 *piError)
{
	UINT32 ulByteRecv;
	UINT32 ulMaxBufSize;
	INT32  iRecvSize;
	CHAR* szRTSPMsg;

	if (!pstConnInfo)
	{
		GosLog(LOG_INFO, "PstConnInfo is empty!");
		*piError = 0;
		return 0;
	}

	//获取字节数， FIONREAD：获取套接字接收缓冲区中的字节数
	if (ioctlsocket(pstConnInfo->stLocalSocket, FIONREAD, (unsigned long*)&ulByteRecv) == SOCKET_ERROR)
	{
		GosLog(LOG_ERROR, "RecvRTSPClientMsg: ioctl socket(%u) failed!", pstConnInfo->stLocalSocket);
		*piError = gos_get_socket_err_code();
		return -1; 
	}
	if (ulByteRecv == 0)
	{
		piError = 0;
		return 0;
	}

	// 建立缓冲区
	if (!pstConnInfo->szLocalRecvBuf)
	{
		ulMaxBufSize= 64 * 1024 - 1;
		pstConnInfo->szLocalRecvBuf = (CHAR*)calloc(1, ulMaxBufSize + 1);
	}

	//接收RTSP信息,ulLocalRecvSize本地信息长度，szLocalRecvBuf缓冲区长度
	iRecvSize = recv(pstConnInfo->stLocalSocket, (CHAR*)(pstConnInfo->szLocalRecvBuf + pstConnInfo->ulLocalRecvSize), ulMaxBufSize - pstConnInfo->ulLocalRecvSize, 0);
	if (iRecvSize < 0)
	{
		GosLog(LOG_ERROR, "RecvRTSPClientMsg: recv msg from socket(%u) failed,error!", pstConnInfo->stLocalSocket);
		return -1;
	}
	if (iRecvSize == 0)
	{
		GosLog(LOG_ERROR, "RecvRTSPServerMsg: recv msg len %u from socket(%u) failed", iRecvSize, pstConnInfo->stLocalSocket);
		return -1;
	}

	//更新缓冲区长度
	pstConnInfo->ulLocalRecvSize += iRecvSize;

	//对报文进行处理
	while (1)
	{
		if (pstConnInfo->ulLocalRecvSize == 0)
		{
			break;
		}

		//区分RTSP是请求还是响应







	}



}