#ifndef MSG_DISPATCH_H
#define MSG_DISPATCH_H

BOOL        GosDispatchMsg(MSG_T *pstMsgHdr, VOID *pvMsg, UINT32 ulMsgLen);
VOID        GosDispatchSystemMsg(MSG_T *pstMsg);

#endif
