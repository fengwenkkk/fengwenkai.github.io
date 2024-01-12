#ifndef RTP_H
#define RTP_H

BOOL InitRTPSocket(CONN_INFO_T *pstConnInfo);
INT32 RecvRTPMsg(CONN_INFO_T *pstConnInfo);
INT32 RecvRTCPMsg(CONN_INFO_T *pstConnInfo);

#endif