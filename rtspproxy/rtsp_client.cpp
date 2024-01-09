#include "rtsp_client.h"



BOOL ParseRTSPClientMsg(CONN_INFO_T* pstConnInfo)
{


}

/*******************************************************************************
* 函数名称: ParseRTSPServerMsg
* 功    能: 处理缓存区粘包，分割一个完整报文
* 参    数:
* 参数名称              输入/输出   描述
* pstConnInfo           IN          客户端信息结构体
* pszRTSPMsg            IN          数组指针
* 函数返回:BOOL
* 说    明:TRUE：数据不够或处理结束
*******************************************************************************/
static BOOL ParseRTSPServerMsg(CONN_INFO_T* pstConnInfo, CHAR** pszRTSPMsg)
{
	CHAR* szMsgHdr = pstConnInfo->szLocalRecvBuf;//本地接收缓存区起始指针。RTSP报文头
	CHAR* szMsgBody = NULL;//RTSP报文主体指针
	CHAR* szLen = 0;//记录报文长度键值位置
	CHAR* szEnd;//记录结束位置


	UINT32  ulMsgHdrLen  = 0;
	UINT32  ulMsgBodyLen = 0;
	UINT32  ulMsgLen     = 0;

	*pszRTSPMsg = NULL;

	szEnd = strstr(szMsgHdr, "\r\n\r\n");//查找消息头结束位
	
	if (!szEnd)
	{
		GosLog(LOG_INFO, "RTSPMsg find end fail,continue get msg!");
		return TRUE; // If the end of the message header is not found, return indicating more data is needed
	}

	szMsgBody = szEnd + 4;
	ulMsgHdrLen = szMsgBody - szMsgHdr;

	//获取信息长度
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




}

//发送RTP数据到客户端
VOID OnRTSPServerRTPMsg(CONN_INFO_T* pstConnInfo)
{
    gos_tcp_send(pstConnInfo->stClientSocket, pstConnInfo->szLocalRecvBuf, pstConnInfo->ulLocalRecvSize);
    pstConnInfo->ulLocalRecvSize = 0;
}


/*******************************************************************************
* 函数名称: RecvRTSPServerMsg
* 功    能: 客户端接受客户端RTSP消息
* 参    数:
* 参数名称              输入/输出   描述
* pstConnInfo           IN          客户端信息结构体
* piError               IN          错误码
* 函数返回:INT32
* 说    明:错误返回-1，数据0返回0，没有错误返回
*******************************************************************************/
INT32 RecvRTSPServerMsg(CONN_INFO_T* pstConnInfo,INT32 *piError)
{
	UINT32 ulByteRecv;
	UINT32 ulMaxBufSize;
	INT32  iRecvSize;
	CHAR* szRTSPMsg;

	//if (!pstConnInfo)
	//{
	//	GosLog(LOG_INFO, "PstConnInfo is empty!");
	//	*piError = 0;
	//	return 0;
	//}

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
		pstConnInfo->szLocalRecvBuf[pstConnInfo->ulLocalRecvSize] = '\0';


		//非RTSP消息直接转发送给客户端
		if (pstConnInfo->ulLocalRecvSize < 4 ||
			memcmp(pstConnInfo->szLocalRecvBuf, "RTSP", 4) != 0)
		{
			OnRTSPServerRTPMsg(pstConnInfo);
			continue;
		}






	}



}