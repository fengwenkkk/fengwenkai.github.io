#ifndef _DTP_FUNC_H
#define _DTP_FUNC_H

INT32   dtp_connect_timeout(SOCKET stSocket, SOCKADDR *pstSockAddr, INT32 iAddrLen, UINT32 ulTimeout);
BOOL    dtp_tcp_send(SOCKET stSocket, VOID *pstReqMsg, UINT32 ulReqMsgLen);
INT32   dtp_tcp_recv(SOCKET stSocket, VOID *pMsgBuf, UINT32 ulMsgBufLen, UINT32 ulTimeout);
BOOL    dtp_udp_send(SOCKET stSocket, VOID *pstReqMsg, UINT32 ulReqMsgLen, SOCKADDR_IN *pstServerAddr);
INT32   dtp_udp_recv(SOCKET stSocket, VOID *pMsgBuf, UINT32 ulMsgBufLen, UINT32 ulTimeout);
UINT32  dtp_get_fd_num(FDSET* fds);
UINT32  dtp_get_max_fd(FDSET *fds);
SOCKET  dtp_get_socket_by_fds(FDSET *fds, UINT32 ulIndex);

BOOL    dtp_msg_ntoh(DTP_MSG_HDR_T *pstMsgHdr);
BOOL    dtp_msg_hton(DTP_MSG_HDR_T *pstMsgHdr);
BOOL    CmpSockAddr(SOCKADDR_IN *pstSrc, SOCKADDR_IN *pstDst);

#endif

