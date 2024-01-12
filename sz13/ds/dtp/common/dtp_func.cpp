#include "g_include.h"

#include "dtp_public.h"
#include "dtp_common.h"
#include "dtp_func.h"

/*******************************************************************************
* ��������: dtp_tcp_send
* ��    ��: tcp��Ϣ����
* ��    ��:
* ��������          ����/���       ����
* ��������: ������Ϣ
* ˵    ��:
*******************************************************************************/
BOOL dtp_tcp_send(SOCKET stSocket, VOID *pstReqMsg, UINT32 ulReqMsgLen)
{
    INT32   iLeftLen = ulReqMsgLen;
    INT32   iSendLen;
    CHAR    *szData = (CHAR*)pstReqMsg;

    if (stSocket == INVALID_SOCKET)
    {
        return FALSE;
    }

    while(iLeftLen > 0)
    {
        iSendLen = send(stSocket, szData, iLeftLen, 0);
        if (iSendLen < 0)
        {
            DTPLog(LOG_ERROR, "tcp_send: send failed, %s", (CHAR*)gos_get_socket_err());
            return FALSE;
        }

        szData += iSendLen;
        iLeftLen -= iSendLen;
    }

    return TRUE;
}

/*******************************************************************************
* ��������: dtp_tcp_recv
* ��    ��: tcp��Ϣ����
* ��    ��:
* ��������          ����/���       ����
* ��������: ������Ϣ
* ˵    ��:
*******************************************************************************/
INT32 dtp_tcp_recv(SOCKET stSock, VOID *pMsgBuf, UINT32 ulMsgBufLen, UINT32 ulTimeout)
{
    return gos_recv_tcp_msg(stSock, pMsgBuf, ulMsgBufLen, ulTimeout);
}

/*******************************************************************************
* ��������: dtp_udp_send
* ��    ��: udp��Ϣ����
* ��    ��:
* ��������          ����/���       ����
* ��������: ������Ϣ
* ˵    ��:
*******************************************************************************/
BOOL dtp_udp_send(SOCKET stSocket, VOID *pstReqMsg, UINT32 ulReqMsgLen, SOCKADDR_IN *pstServerAddr)
{
    if (!gos_udp_send(stSocket, pstReqMsg, ulReqMsgLen, pstServerAddr))
    {
        DTPLog(LOG_ERROR, "udp send failed, %s", gos_get_socket_err());
        return FALSE;
    }

    return TRUE;
}

/*******************************************************************************
* ��������: dtp_udp_recv
* ��    ��: udp��Ϣ����
* ��    ��:
* ��������          ����/���       ����
* ��������: ������Ϣ
* ˵    ��:
*******************************************************************************/
INT32 dtp_udp_recv(SOCKET stSock, VOID *pMsgBuf, UINT32 ulMsgBufLen, UINT32 ulTimeout)
{
    INT32   iSize = gos_recv_udp_msg(stSock, pMsgBuf, ulMsgBufLen, ulTimeout, NULL);

    if (iSize < 0)
    {
        DTPLog(LOG_ERROR, "udp recv failed, %s", gos_get_socket_err());
        return -1;
    }

    return iSize;
}

/*******************************************************************************
* ��������: dtp_get_fd_num
* ��    ��: ��� FD ����
* ��    ��:
* ��������          ����/���       ����
* fds               IN              �ļ�����������
* ��������: FD����
* ˵    ��:
*******************************************************************************/
UINT32 dtp_get_fd_num(FDSET* fds)
{
#ifdef _OSWIN32_
    return fds->fd_count;

#else
    UINT32  i;
    UINT32  ulCount = 0;

    for (i = 0; i < FD_SETSIZE; i++)
    {
        if (FD_ISSET(i, fds))
        {
            ulCount++;
        }
    }

    return ulCount;
#endif
}

UINT32 dtp_get_max_fd(FDSET *fds)
{
#ifdef _OSWIN32_
    UINT32  i;
    SOCKET  sock = 0;

    for (i=0; i<fds->fd_count; i++)
    {
        if (i == 0 || sock < fds->fd_array[i])
        {
            sock = fds->fd_array[i];
        }
    }

    return sock;
#else
    INT32   i;

    for (i=FD_SETSIZE-1; i>=0; i--)
    {
        if (FD_ISSET(i, fds))
        {
            return i;
        }
    }

    return 0;
#endif
}

/*******************************************************************************
* ��������: dtp_get_socket_by_fds
* ��    ��: ��ȡsocket
* ��    ��:
* ��������          ����/���       ����
* fds               IN              �ļ�����������
* ulIndex           IN              ����
* ��������: SOCKET�ļ�������
* ˵    ��:
*******************************************************************************/
SOCKET dtp_get_socket_by_fds(FDSET *fds, UINT32 ulIndex)
{
#ifdef _OSWIN32_
    if (ulIndex >= fds->fd_count)
    {
        return INVALID_SOCKET;
    }

    return fds->fd_array[ulIndex];
#else
    if (FD_ISSET(ulIndex, fds))
    {
        return ulIndex;
    }

    return INVALID_SOCKET;
#endif
}

BOOL dtp_msg_ntoh(DTP_MSG_HDR_T *pstMsgHdr)
{
    if (!pstMsgHdr)
    {
        return FALSE;
    }

    NTOHL_IT(pstMsgHdr->ulMsgMagic);
    NTOHS_IT(pstMsgHdr->usMsgVer);
    NTOHS_IT(pstMsgHdr->usSenderModule);
    NTOHS_IT(pstMsgHdr->usReceiverModule);
    NTOHL_IT(pstMsgHdr->ulMsgLen);
    NTOHL_IT(pstMsgHdr->ulSeqID);
    NTOHS_IT(pstMsgHdr->usMsgType);
    NTOHS_IT(pstMsgHdr->usTimeout);
    NTOHS_IT(pstMsgHdr->usCRC);

    return TRUE;
}

BOOL dtp_msg_hton(DTP_MSG_HDR_T *pstMsgHdr)
{
    if (!pstMsgHdr)
    {
        return FALSE;
    }

    HTONL_IT(pstMsgHdr->ulMsgMagic);
    HTONS_IT(pstMsgHdr->usMsgVer);
    HTONS_IT(pstMsgHdr->usSenderModule);
    HTONS_IT(pstMsgHdr->usReceiverModule);
    HTONL_IT(pstMsgHdr->ulMsgLen);
    HTONL_IT(pstMsgHdr->ulSeqID);
    HTONS_IT(pstMsgHdr->usMsgType);
    HTONS_IT(pstMsgHdr->usTimeout);
    HTONS_IT(pstMsgHdr->usCRC);

    return TRUE;
}

BOOL CmpSockAddr(SOCKADDR_IN *pstSrc, SOCKADDR_IN *pstDst)
{
    if (memcmp(&pstSrc->sin_addr, &pstDst->sin_addr, sizeof(pstDst->sin_addr)) == 0 &&
        pstSrc->sin_port == pstDst->sin_port)
    {
        return TRUE;
    }

    return FALSE;
}
