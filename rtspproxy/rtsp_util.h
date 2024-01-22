#pragma once
#ifndef RTSP_UTIL_H
#define RTSP_UTIL_H

#define RTSP_INTERLEAVED_MAGIC          0x24

#define RTSP_INTERLEAVED_MAG_HDR_LEN    4

typedef struct
{
    BOOL            bValid; //  RTSP Client �����Ϣ
    SOCKET          stClientSocket;                     // ���յ��Ŀͻ���TCP����

    SOCKADDR_IN     stClientAddr;                       // ���յ���RTSP�ͻ��˵�ַ
    //    UINT32                  ulConnectSetupTime;                 /* �ͻ������ӽ���ʱ�� */

    //   DTP_CLIENT_STATUS_E     enClientStatus;                     /* �ͻ�������״̬ */
    //    UINT32                  ulLastRecvTime;                     /* ���յ������һ�οͻ�����Ϣʱ�� */

    //    UINT32                  ulRecvOffset;                       /* TCP ���У���ǰ������ʵ����ʼλ�� */
    UINT32           ulRecvSize;                     // ���յ��Ŀͻ������ӻ�������ǰ����
    UINT32           ulMaxRecvBufSize;
    CHAR*            szRecvBuf;                     // ���յ��Ŀͻ������ӻ�����

    // RTSP client��RTP��Ϣ
    UINT16                  usClientRTPPort;
    UINT16                  usClientRTCPPort;

    UINT32                  ulCSeq;
    CHAR                    acPlayParam[256];
    CHAR                    acUserAgent[128];
    CHAR                    acSession[128];

    //  RTSP Server �����Ϣ
    UINT8                   aucRTSPServerAddr[4];
    UINT16                  usRTSPServerPort;

    
    SOCKET                  stLocalSocket;                  // ���պ��Զ����ӵ�RTSP��������TCP�ͻ��ˣ�����RTSP�ͻ��ˣ�
    UINT32                  ulLocalRecvSize;                // ����RTSP�ͻ��˻�������ǰ�������ݵĳ���
    CHAR*                   szLocalRecvBuf;                 // ����RTSP�ͻ��˻���������
    UINT32                  ulMaxLocalRecvBufSize;

    UINT16                  usServerRTPPort;
    UINT16                  usServerRTCPPort;

    // LocalRTP��Ϣ
    UINT16                  usLocalRTPPort;
    UINT16                  usLocalRTCPPort;

    SOCKET                  stLocalRTPSocket;               // RTP (UDP) ������
    SOCKET                  stLocalRTCPSocket;              // RTCP (UDP) ������

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