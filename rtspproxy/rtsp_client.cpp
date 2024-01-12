#include "rtsp_client.h"



/*******************************************************************************
* ��������: ReplaceRTSPUrl
* ��    ��: �滻URL
* ��    ��:
* ��������              ����/���   ����
* szKeyWord             IN          �滻���ֲ���Ĺؼ��� 
* szRTSPMsg             IN          ����ָ��
* szNewText             IN          �µ�����
* ��������:VOID
* ˵    ��:
*******************************************************************************/
GString ReplaceRTSPUrl(CHAR* szRTSPMsg, const CHAR* szKeyWord, CHAR* szNewText)
{
	GString strMsg;
	CHAR*   szMsg = szRTSPMsg;
	CHAR*   szStart;//�滻��־λ

	while (szMsg)
	{
		szStart = strstr(szMsg, szKeyWord);
	
		//û�о�ֱ�ӷ���ԭ��Ϣ
		if (!szStart)
		{
			strMsg.Append(szMsg);
			goto end;
		}
		szMsg += strlen(szKeyWord);




end:

}



/*******************************************************************************
* ��������: RelayRTSPServerSDPMsg
* ��    ��: �������SDP��Ϣ
* ��    ��:
* ��������              ����/���   ����
* pstConnInfo           IN          �ͻ�����Ϣ�ṹ��
* szRTSPMsg             IN          ����ָ��
* ��������:VOID
* ˵    ��: 
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

	//��ȡ��Ϣ����
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
* ��������: ParseRTSPServerMsg
* ��    ��: ��������ճ�����ָ�һ����������
* ��    ��:
* ��������              ����/���   ����
* pstConnInfo           IN          �ͻ�����Ϣ�ṹ��
* pszRTSPMsg            IN          ����ָ��
* ��������:BOOL
* ˵    ��:TRUE�����ݲ����������
*******************************************************************************/
static BOOL ParseRTSPServerMsg(CONN_INFO_T* pstConnInfo, CHAR** pszRTSPMsg)
{
	CHAR* szMsgHdr = pstConnInfo->szLocalRecvBuf;//���ؽ��ջ�������ʼָ�롣RTSP����ͷ
	CHAR* szMsgBody = NULL;//RTSP��������ָ��
	CHAR* szLen = 0;//��¼���ĳ��ȼ�ֵλ��
	CHAR* szEnd;//��¼����λ��


	UINT32  ulMsgHdrLen  = 0;
	UINT32  ulMsgBodyLen = 0;
	UINT32  ulMsgLen     = 0;

	CHAR *szRTSPMsg;
	*pszRTSPMsg = NULL;

	szEnd = strstr(szMsgHdr, "\r\n\r\n");//������Ϣͷ����λ
	
	if (!szEnd)
	{
		GosLog(LOG_INFO, "RTSPMsg find end fail,continue get msg!");
		return TRUE; // If the end of the message header is not found, return indicating more data is needed
	}

	szMsgBody = szEnd + 4;
	ulMsgHdrLen = szMsgBody - szMsgHdr;

	//��ȡ��Ϣ����
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
	memcpy(szRTSPMsg, szMsgHdr, ulMsgLen); // ������RTSP��Ϣ��ͷ��+���ģ����Ƶ�������ڴ���

	*pszRTSPMsg = szRTSPMsg;
	pstConnInfo->ulLocalRecvSize -= ulMsgLen; // ���½��ջ������Ĵ�С����ȥ�������Ϣ����
	if (pstConnInfo->ulLocalRecvSize > 0)
	{
		memmove(pstConnInfo->szLocalRecvBuf, pstConnInfo->szLocalRecvBuf + ulMsgLen, pstConnInfo->ulLocalRecvSize); // ����������ʣ��������ƶ�����ͷ
	}
	return TRUE; 


}

//����RTP���ݵ��ͻ���
VOID OnRTSPServerRTPMsg(CONN_INFO_T* pstConnInfo)
{
    gos_tcp_send(pstConnInfo->stClientSocket, pstConnInfo->szLocalRecvBuf, pstConnInfo->ulLocalRecvSize);
    pstConnInfo->ulLocalRecvSize = 0;
}



/*******************************************************************************
* ��������: HandleRTSPServerMsg
* ��    ��: ����ת������
* ��    ��:
* ��������              ����/���   ����
* pstConnInfo           IN          �ͻ�����Ϣ�ṹ��
* szRTSPMsg             IN          ����ָ��
* ��������:VOID
* ˵    ��:
*******************************************************************************/
static VOID HandleRTSPServerMsg(CONN_INFO_T* pstConnInfo, CHAR* szRTSPMsg)
{
	CHAR    acNewText[32];
	GString strTmp;

	GosLog(LOG_INFO, "Recv msg from RTSP server: \n%s", szRTSPMsg);

	//�ظ�������ȡ������ֱ��ת��
	if (strstr(szRTSPMsg, "Public:"))
	{
		gos_tcp_send(pstConnInfo->stClientSocket, szRTSPMsg, strlen(szRTSPMsg));
		GosLog(LOG_INFO, "Send msg to RTSP client: \n%s", szRTSPMsg);
	}
	//�����������
	else if (strstr(szRTSPMsg, "Content-Type: application/sdp"))
	{
		RelayRTSPServerSDPMsg(pstConnInfo, szRTSPMsg);
	}





}

/*******************************************************************************
* ��������: RecvRTSPServerMsg
* ��    ��: �ͻ��˽��ܿͻ���RTSP��Ϣ
* ��    ��:
* ��������              ����/���   ����
* pstConnInfo           IN          �ͻ�����Ϣ�ṹ��
* piError               IN          ������
* ��������:INT32
* ˵    ��:���󷵻�-1������0����0��û�д��󷵻�
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

	//��ȡ�ֽ����� FIONREAD����ȡ�׽��ֽ��ջ������е��ֽ���
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

	// ����������
	if (!pstConnInfo->szLocalRecvBuf)
	{
		ulMaxBufSize= 64 * 1024 - 1;
		pstConnInfo->szLocalRecvBuf = (CHAR*)calloc(1, ulMaxBufSize + 1);
	}

	//����RTSP��Ϣ,ulLocalRecvSize������Ϣ���ȣ�szLocalRecvBuf����������
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

	//���»���������
	pstConnInfo->ulLocalRecvSize += iRecvSize;

	//�Ա��Ľ��д���
	while (1)
	{
		if (pstConnInfo->ulLocalRecvSize == 0)
		{
			break;
		}
		pstConnInfo->szLocalRecvBuf[pstConnInfo->ulLocalRecvSize] = '\0';


		//��RTSP��Ϣֱ��ת���͸��ͻ���
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