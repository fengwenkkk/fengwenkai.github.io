#include "g_include.h"
#include "rtsp_util.h"
#ifndef  RTSP_CLIENT_H
#define  RTSP_CLIENT_H




INT32 RecvRTSPServerMsg(CONN_INFO_T* pstConnInfo);
GString ReplaceRTSPUrl(CHAR* szRTSPMsg, const CHAR* szKeyWord, CHAR* szNewText);
GString ReplaceRTSPStr(CHAR* szMsg, CHAR* szOld, CHAR* szNew);
static GString ReplaceRTSPNum(CHAR* szRTSPMsg, const CHAR* szKey, CHAR* szNewValue);
static VOID RelayRTSPServerSDPMsg(CONN_INFO_T* pstConnInfo, CHAR* szRTSPMsg);
VOID RelayRTSPServerPortMsg(CONN_INFO_T* pstConnInfo, CHAR* szRTSPMsg, CHAR* acNewText);
static BOOL ParseRTSPServerMsg(CONN_INFO_T* pstConnInfo, CHAR** pszRTSPMsg);
VOID OnRTSPServerRTPMsg(CONN_INFO_T* pstConnInfo);
static VOID RelayRTSPServerMsg(CONN_INFO_T* pstConnInfo, CHAR* szRTSPMsg);
static VOID HandleRTSPServerMsg(CONN_INFO_T* pstConnInfo, CHAR* szRTSPMsg);
INT32 RecvRTSPServerMsg(CONN_INFO_T* pstConnInfo, INT32* piError);
SOCKET InitRTSPClientSocket(UINT8* pucAddr, UINT16 usPort);

#endif // ! RTSP_CLIENT_H

