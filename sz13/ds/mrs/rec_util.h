#ifndef REC_UTIL_H
#define REC_UTIL_H


typedef struct
{
    UINT32      ulThreadID;
    BOOL        bClosed;
}REC_THREAD_INFO_T;

typedef struct
{
    UINT32      ulCallID;
    INT32       iLocalPort;
    INT64       i64GroupID;
}GROUP_CALL_STATUS_INFO_T;

typedef struct
{
    UINT32      ulCallID;
    INT32       iLocalPort;
    UINT32      ulCaller;
    UINT32      ulCallee;
    CHAR        acTime[32];
}P2P_CALL_STATUS_INFO_T;

//BYTE* Parse_RTP_Packet(BYTE pBuf[], unsigned short nSize, int* outSize);

GOS_THREAD_RET RecvP2PVideoCallAudio(VOID *pvData);
GOS_THREAD_RET RecvP2PVideoCallVideo(VOID *pvData);
GOS_THREAD_RET RecvP2PCallAudio(VOID *pvData);
GOS_THREAD_RET RecvGroupCallAudio(VOID *pvData);

VOID CloseRecAudioThread(UINT32 ulCallID);
VOID CloseRecVideoThread(UINT32 ulCallID);

#endif

