#pragma once
#ifndef RTP_H
#define RTP_H
#include "g_include.h"

#include "rtsp_util.h"
INT32 RecvRTPMsg(CONN_INFO_T* pstConnInfo);

INT32 RecvRTCPMsg(CONN_INFO_T* pstConnInfo);

#endif
