#include "rtsp_client.h"


/*******************************************************************************
* ��������: RecvRTSPClientMsg
* ��    ��: ���ܿͻ���RTSP��Ϣ
* ��    ��:
* ��������              ����/���   ����
* pstConnInfo           IN          �ͻ�����Ϣ�ṹ��
* piError               IN          ������
* ��������:INT32
* ˵    ��:���󷵻�-1������0����0��û�д��󷵻�
*******************************************************************************/
INT32 RecvRTSPClientMsg(CONN_INFO_T* pstConnInfo,INT32 *piError)
{
	UINT32 ulByteRecvCount;
	UINT32 ulMaxBufSize;
	INT32  iRecvSize;
	//��ȡ�ֽ����� FIONREAD����ȡ�׽��ֽ��ջ������е��ֽ���
	if (ioctlsocket(pstConnInfo->stLocalSocket, FIONREAD, (unsigned long*)&ulByteRecvCount) == SOCKET_ERROR)
	{
		GosLog(LOG_ERROR, "RecvRTSPClientMsg: ioctl socket(%u) failed!", pstConnInfo->stLocalSocket);
		*piError = gos_get_socket_err_code();
		return -1; 
	}
	if (ulByteRecvCount == 0)
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





	}



}