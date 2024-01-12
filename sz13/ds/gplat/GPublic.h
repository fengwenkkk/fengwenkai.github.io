#ifndef G_PUBLIC_H
#define G_PUBLIC_H

#include "g_include.h"
#include "GCommon.h"
#include "GBaseTask.h"
#include "GMsgQueue.h"
#include "GMsgPool.h"
#include "GProc.h"
#include "GMsgDispatch.h"
#include "GTimer.h"
#include "GSysTask.h"
#include "GSysLogTask.h"

typedef VOID (*GOS_MSG_PREHANDLE)(MSG_T *pstMsgHdr, VOID *pvMsg, UINT32 ulMsgLen);

/* º¯ÊýÉùÃ÷ */
UINT32 GosGetMaxMsgSize();
UINT32 GosGetSafeMsgSize();

UINT16 GosGetInstID();
BOOL GosLoadPlatConf(PLAT_CFG_T *pstCfg, CHAR *szPlatCfgFile=NULL, UINT16 usDefaultServerPort=GOS_DEFAULT_SERVER_PORT);
VOID GosRegisterMsgPreHandle(GOS_MSG_PREHANDLE pfHandle);
VOID GosSetMsgMaxWaitTime(UINT32 ulMsgMaxWaitTime);
VOID GosSetMaxMsgNodeNum(UINT32 ulMaxMsgNodeNum);
UINT32 GosGetMaxMsgNodeNum();
BOOL GosStart(PLAT_CFG_T *pstCfg);
BOOL GosCreateDTPClient(PEER_ADDR_T *pstServerAddr);
VOID GosFreeDTPClient(PEER_ADDR_T *pstServerAddr);
BOOL GosStartDTPClient(PEER_ADDR_T *pstServerAddr);
BOOL GosStartDTPClient(PEER_ADDR_T *pstServerAddr, UINT16 ulServerID);
BOOL GosStopDTPClient(PEER_ADDR_T *pstServerAddr);
GProc* GosGetProc(UINT16 usTaskID);

BOOL GosRegisterTask(GBaseTask *pTask);

VOID GosGetServerList(VECTOR<UINT16> &vServerInstID);
BOOL GosSendServer(UINT16 usSenderTaskID, UINT16 usReceiverTaskID, UINT32 ulSeqID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);
BOOL GosSendServer(UINT16 usSenderTaskID, UINT16 usReceiverTaskID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);
BOOL GosSendServer(UINT16 usSenderTaskID, PID_T *pstReceiver, UINT32 ulSeqID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);
BOOL GosSendServer(UINT16 usSenderTaskID, PID_T *pstReceiver, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);
VOID GosSendAllServer(UINT16 usSenderTaskID, UINT16 usReceiverTaskID, UINT32 ulSeqID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);

BOOL GosSend(UINT16 usSenderTaskID, PID_T *pstReceiver, UINT32 ulSeqID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);
BOOL GosSendRsp(UINT16 usSenderTaskID, PID_T *pstReceiver, UINT32 ulSeqID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);
BOOL GosSendLocal(PID_T *pstReceiver, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulSeqID);
VOID GosSendBroadcast(UINT16 usSenderTaskID, PID_T *pstReceiver, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);
BOOL GosSyncSendClient(UINT16 usSenderTaskID, PID_T *pstReceiver, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen,VOID *pvRspMsg,UINT32 ulMaxRspLen,UINT32 *ulRspMsgLen, UINT32 ulTimeout);
BOOL GosGetClientList(UINT16 *pusClientID, UINT32 *pulClientNum);
UINT16 GosGetTaskID();
BOOL GosSendClient(UINT16 usSenderTaskID, PID_T *pstReceiver, UINT32 ulSeqID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);
BOOL GosSendClient(UINT16 usSenderTaskID, PID_T *pstReceiver, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);
BOOL GosSendClient(UINT16 usSenderTaskID, UINT16 usClientID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);
VOID GosSendAllClient(UINT16 usSenderTaskID, UINT16 usReceiverTaskID, UINT8 ucClientType, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);
BOOL GosIsClientConnected();
BOOL GosIsClientConnected(UINT16 usServerInstID);
VOID GosFreeMsg(MSG_T *pstMsg);

BOOL GosInitBufferLog(UINT32 ulNum);

BOOL GosGetTaskStatus(UINT32 ulTaskID, GOS_TASK_STATUS_T *pstTaskStatus);
BOOL GosGetTaskStatus(VECTOR<GOS_TASK_STATUS_T> &vTaskStatus);
CHAR* GosGetTaskName(UINT16 usTaskID);

BOOL GosGetClientAddr(UINT16 usClientID, UINT8 *pucClientAddr, UINT16 *pusClientPort);

#endif
