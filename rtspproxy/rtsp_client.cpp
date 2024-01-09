#include "rtsp_client.h"



BOOL ParseRTSPClientMsg(CONN_INFO_T* pstConnInfo)
{


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




}

//����RTP���ݵ��ͻ���
VOID OnRTSPServerRTPMsg(CONN_INFO_T* pstConnInfo)
{
    gos_tcp_send(pstConnInfo->stClientSocket, pstConnInfo->szLocalRecvBuf, pstConnInfo->ulLocalRecvSize);
    pstConnInfo->ulLocalRecvSize = 0;
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






	}



}