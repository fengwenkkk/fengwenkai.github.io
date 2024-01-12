#ifndef RTSP_CLIENT_H
#define RTSP_CLIENT_H

SOCKET InitRTSPClientSocket(UINT8 *pucAddr, UINT16 usPort);
INT32 RecvRTSPServerMsg(CONN_INFO_T *pstConnInfo, INT32 *piErrCode);

#endif