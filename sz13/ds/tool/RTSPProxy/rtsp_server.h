#ifndef RTSP_SERVER_H
#define RTSP_SERVER_H

VOID CloseLocalRTSPServerSocket();
SOCKET InitLocalRTSPServerSocket(UINT8 *pucAddr, UINT16 usPort);
INT32 RecvRTSPClientMsg(SOCKET stSocket, INT32 *piErrCode);
VOID CheckRTSPClientTimeout();

#endif