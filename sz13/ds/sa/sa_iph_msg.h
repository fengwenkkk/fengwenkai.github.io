#ifndef SA_IPH_MSG_H
#define SA_IPH_MSG_H

#define RTSP_STATUS_IDLE            0
#define RTSP_STATUS_PRE_ALLOC       1   // SA已经分配端口
#define RTSP_STATUS_WORK            2   // DC已经接听
#define RTPS_STATUS_PAUSE           3   // 临时关闭视频（保留端口资源）


typedef struct  
{
    UINT8           aucRTSPServerAddr[4];
    UINT16          usRTSPServerPort;

    UINT8           aucLocalRTSPServerAddr[4];
    UINT16          usLocalRTSPServerPort;

    SOCKET          stPreAllocedSocket;

    UINT32          ulApplyUptime;
    UINT32          ulRTSPStatus;
}RTSP_INFO_T;

extern BOOL        g_bNeedSendSelfCheckMsg;
extern UINT32      g_ulTotalCheckPISCount;
extern UINT32      g_ulRecBrdCfgNum;
extern ID_VALUE_T  g_astRecBrdCfg[1024];


VOID SendPISHeartBeatMsg();
VOID SendPISSelfCheckMsg();
VOID SendPISGetTrainInfoMsg();
VOID SendPISRecBrdCfgMsg();

VOID OnPISMsg(PIS_MSG_HDR_T *pstMsg);

BOOL SendPISMsg(UINT16 usMsgType, VOID *pvMsg=NULL, UINT16 usMsgLen=0);

#endif