#pragma once
#ifndef RTSP_SERVER_H
#define RTSP_SERVER_H

extern SOCKET  g_stLocalRTSPServerSocket;

SOCKET InitLocalRTSPServerSocket(UINT8* pucAddr, UINT16 usPort);


#endif