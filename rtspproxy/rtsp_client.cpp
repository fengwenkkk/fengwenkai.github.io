#include "rtsp_client.h"



/*******************************************************************************
* 函数名称: ReplaceRTSPUrl
* 功    能: 替换URL
* 参    数:
* 参数名称              输入/输出   描述
* szKeyWord             IN          替换部分不变的关键字 
* szRTSPMsg             IN          数组指针
* szNewText             IN          新的内容
* 函数返回:VOID
* 说    明:
*******************************************************************************/
GString ReplaceRTSPUrl(CHAR* szRTSPMsg, const CHAR* szKeyWord, CHAR* szNewText)
{
	GString strMsg;
	CHAR*   szMsg = szRTSPMsg;
	CHAR*   szStart;//替换标志位

	while (szMsg)
	{
		szStart = strstr(szMsg, szKeyWord);
	
		//没有就直接返回原信息
		if (!szStart)
		{
			strMsg.Append(szMsg);
			goto end;
		}
		szMsg += strlen(szKeyWord);




end:

}



/*******************************************************************************
* 函数名称: RelayRTSPServerSDPMsg
* 功    能: 处理控制SDP信息
* 参    数:
* 参数名称              输入/输出   描述
* pstConnInfo           IN          客户端信息结构体
* szRTSPMsg             IN          数组指针
* 函数返回:VOID
* 说    明: 
*******************************************************************************/
static VOID RelayRTSPServerSDPMsg(CONN_INFO_T* pstConnInfo, CHAR* szRTSPMsg)
{
	GString     strMsg;
	CHAR        acIP[32];
	CHAR        acNewIP[32];
	CHAR        acContentLen[32];
	CHAR        acNewContentLen[32];
	CHAR*       szNewMsg;
	CHAR*       szMsgBody = strstr(szRTSPMsg, "\r\n\r\n");
	UINT32      ulMsgBodyLen = 0;

	if (!szMsgBody)
	{
		GosLog(LOG_ERROR, "invalid sdp msg: %s", szRTSPMsg);
		return;
	}

	sprintf(acIP, IP_FMT, IP_ARG(pstConnInfo->aucRTSPServerAddr));

	//获取信息长度
	szMsgBody += 4; // \r\n\r\n
	ulMsgBodyLen = strlen(szMsgBody);
	sprintf(acContentLen, "Content-Length: %u", ulMsgBodyLen);

	if (g_usLocalRTSPServerPort == g_usDefaultRTSPServerPort)
	{
		sprintf(acNewIP, IP_FMT, IP_ARG(g_aucLocalRTSPServerAddr));
	}
	else
	{
		sprintf(acNewIP, IP_FMT ":%u", IP_ARG(g_aucLocalRTSPServerAddr), g_usLocalRTSPServerPort);
	}

	strMsg = ReplaceRTSPUrl(szRTSPMsg, "rtsp://", acNewIP);

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

	CHAR *szRTSPMsg;
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

	ulMsgLen = ulMsgHdrLen + ulMsgBodyLen;
	szRTSPMsg = (CHAR*)calloc(1, ulMsgLen + 1);
	if (!szRTSPMsg)
	{
		return FALSE;
	}
	memcpy(szRTSPMsg, szMsgHdr, ulMsgLen); // 将整个RTSP消息（头部+正文）复制到分配的内存中

	*pszRTSPMsg = szRTSPMsg;
	pstConnInfo->ulLocalRecvSize -= ulMsgLen; // 更新接收缓冲区的大小，减去处理的消息长度
	if (pstConnInfo->ulLocalRecvSize > 0)
	{
		memmove(pstConnInfo->szLocalRecvBuf, pstConnInfo->szLocalRecvBuf + ulMsgLen, pstConnInfo->ulLocalRecvSize); // 将缓冲区中剩余的数据移动到开头
	}
	return TRUE; 


}

//发送RTP数据到客户端
VOID OnRTSPServerRTPMsg(CONN_INFO_T* pstConnInfo)
{
    gos_tcp_send(pstConnInfo->stClientSocket, pstConnInfo->szLocalRecvBuf, pstConnInfo->ulLocalRecvSize);
    pstConnInfo->ulLocalRecvSize = 0;
}



/*******************************************************************************
* 函数名称: HandleRTSPServerMsg
* 功    能: 处理并转发报文
* 参    数:
* 参数名称              输入/输出   描述
* pstConnInfo           IN          客户端信息结构体
* szRTSPMsg             IN          数组指针
* 函数返回:VOID
* 说    明:
*******************************************************************************/
static VOID HandleRTSPServerMsg(CONN_INFO_T* pstConnInfo, CHAR* szRTSPMsg)
{
	CHAR    acNewText[32];
	GString strTmp;

	GosLog(LOG_INFO, "Recv msg from RTSP server: \n%s", szRTSPMsg);

	//回复参数获取的命令直接转发
	if (strstr(szRTSPMsg, "Public:"))
	{
		gos_tcp_send(pstConnInfo->stClientSocket, szRTSPMsg, strlen(szRTSPMsg));
		GosLog(LOG_INFO, "Send msg to RTSP client: \n%s", szRTSPMsg);
	}
	//处理控制信令
	else if (strstr(szRTSPMsg, "Content-Type: application/sdp"))
	{
		RelayRTSPServerSDPMsg(pstConnInfo, szRTSPMsg);
	}





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
		
		if (!ParseRTSPServerMsg(pstConnInfo, &szRTSPMsg))
		{
			GosLog(LOG_ERROR, "ParseRTSPServerMsg : failed");
			return -1;
		}
		if (!szRTSPMsg)
		{
			break;
		}

		HandleRTSPServerMsg(pstConnInfo, szRTSPMsg);

		free(szRTSPMsg);
	}



}