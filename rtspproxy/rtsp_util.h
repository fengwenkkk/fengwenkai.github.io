#pragma once
#ifndef RTSP_UTIL_H
#define RTSP_UTIL_H

#define RTSP_INTERLEAVED_MAGIC          0x24

#define RTSP_INTERLEAVED_MAG_HDR_LEN    4

typedef struct
{
    BOOL            bValid; //  RTSP Client 相关信息
    SOCKET          stClientSocket;                     // 接收到的客户端TCP连接

    SOCKADDR_IN     stClientAddr;                       // 接收到的RTSP客户端地址
    //    UINT32                  ulConnectSetupTime;                 /* 客户端连接建立时间 */

    //   DTP_CLIENT_STATUS_E     enClientStatus;                     /* 客户端连接状态 */
    //    UINT32                  ulLastRecvTime;                     /* 接收到的最后一次客户端消息时间 */

    //    UINT32                  ulRecvOffset;                       /* TCP 特有，当前缓冲区实际起始位置 */
    UINT32           ulRecvSize;                     // 接收到的客户端连接缓冲器当前长度
    UINT32           ulMaxRecvBufSize;
    CHAR*            szRecvBuf;                     // 接收到的客户端连接缓冲器

    // RTSP client的RTP信息
    UINT16                  usClientRTPPort;
    UINT16                  usClientRTCPPort;

    UINT32                  ulCSeq;
    CHAR                    acPlayParam[256];
    CHAR                    acUserAgent[128];
    CHAR                    acSession[128];

    //  RTSP Server 相关信息
    UINT8                   aucRTSPServerAddr[4];
    UINT16                  usRTSPServerPort;

    
    SOCKET                  stLocalSocket;                  // 接收后自动连接到RTSP服务器的TCP客户端（本地RTSP客户端）
    UINT32                  ulLocalRecvSize;                // 本地RTSP客户端缓冲器当前储存数据的长度
    CHAR*                   szLocalRecvBuf;                 // 本地RTSP客户端缓冲器长度
    UINT32                  ulMaxLocalRecvBufSize;

    UINT16                  usServerRTPPort;
    UINT16                  usServerRTCPPort;

    // LocalRTP信息
    UINT16                  usLocalRTPPort;
    UINT16                  usLocalRTCPPort;

    SOCKET                  stLocalRTPSocket;               // RTP (UDP) 服务器
    SOCKET                  stLocalRTCPSocket;              // RTCP (UDP) 服务器

    UINT8*                  pucLocalRTPRecvBuf;
    UINT32                  ulMaxLocalRTPRecvBufSize;

    UINT8*                  pucLocalRTCPRecvBuf;
    UINT32                  ulMaxLocalRTCPRecvBufSize;
}CONN_INFO_T;
#include "ds_public.h"
extern CONN_INFO_T     g_stRTSPClientConnInfo;
extern FD_SET  g_fdsAll;
extern UINT16  g_usDefaultRTSPServerPort;

extern UINT8   g_aucRTSPServerAddr[4];
extern UINT16  g_usRTSPServerPort;

extern UINT8   g_aucLocalRTSPServerAddr[4];
extern UINT16  g_usLocalRTSPServerPort;

extern UINT32  g_ulRTSPClientConnectTimeout;
extern UINT32  g_ulConnectRTSPServerSuccTime;
extern UINT32  g_ulRTSPClientTimeout;

extern CHAR     g_acCurrRTSPCmd[32];
extern UINT32   g_ulMaxConnectRTSPServerTime;
//extern SOCKET   g_stLocalRTSPServerSock;



VOID NewConnInfo(SOCKET stClientSocket, SOCKADDR_IN& stClientAddr);
VOID DelConnInfo(CONN_INFO_T* pstConnInfo);
BOOL ConnectToRTSPServer(CONN_INFO_T* pstConnInfo);

BOOL ConvertRTSPAddr(CONN_INFO_T* pstConnInfo, UINT8* pucRTSPServerAddr, UINT16 usRTSPServerPort);

BOOL GetSIPValue(CHAR* szSIPText, const CHAR* szKey, UINT32* pulValue);
BOOL GetSIPValueAndLen(CHAR* szSIPText, const CHAR* szKey, CHAR* szValue, UINT32 ulMaxLen);

GString ReplaceRTSPMsg(CHAR* szRTSPMsg, const CHAR* szKey, CHAR* szNewValue);
BOOL ParseRTSPPort(CHAR* szPort, UINT16& usRTPPort, UINT16& usRTCPPort);
BOOL IsRTSPInterleavedMsg(CHAR* szMsg, UINT32 ulLen);
BOOL IsRTSPMsg(CHAR* szMsg, UINT32 ulLen);
VOID CloseApp();

BOOL IsRTSPMsg(CHAR* szMsg, UINT32 ulLen);
BOOL IsRTSPInterleavedMsg(CHAR* szMsg, UINT32 ulLen);
BOOL InitRTPSocket(CONN_INFO_T* pstConnInfo);

#endif