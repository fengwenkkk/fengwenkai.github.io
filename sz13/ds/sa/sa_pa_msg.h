#ifndef SA_PA_MSG_H
#define SA_PA_MSG_H

VOID OnPAMsg(PA_MSG_HDR_T *pstMsg);

VOID SendPAHeartBeatMsg();
VOID SendPASelfCheckMsg();
VOID SendPAGetTrainInfoMsg();
VOID SendPARecBrdCfgMsg();

BOOL SendPAMsg(UINT16 usMsgType, VOID *pvMsg=NULL, UINT16 usMsgLen=0);

BOOL DiscoverPISServer(UINT8 *pucLocalAddr, UINT8 *pucGateway, UINT8 *pucPISAddr);

BOOL GetSelfCheckStatus(UINT8 *pucCheckStatus);

#endif