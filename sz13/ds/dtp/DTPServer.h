#ifndef _DTPSERVER_H
#define _DTPSERVER_H

#include "g_include.h"
#include "dtp_public.h"

class DTPServer;

class DTPServerMsgHandler
{
    friend class DTPServer;

public:
    virtual VOID MsgHandle(UINT16 usClientID, DTP_MSG_T *pstMsg)=0;
    virtual VOID OnAcceptClient(UINT16 usClientID)=0;
    virtual VOID OnDisconnectClient(UINT16 usClientID)=0;
};

class DTPServer
{
public:
    static DTPServer* New(UINT16 usInstID, UINT8 *pucAddr, UINT16 usPort,
                          DTPServerMsgHandler *pMsgHandle, GMemAlloc *pMemAlloc, BOOL bTCPMode);

    ~DTPServer();

    virtual BOOL  HasClient(UINT16 usClientID)=0;
    virtual BOOL  Send(UINT16 usServerModuleID, UINT16 usClientID, UINT16 usClientModuleID, UINT16 usMsgType, UINT32 ulSeqID, VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulTimeout=0)=0;
    virtual BOOL  SendReq(UINT16 usServerModuleID, UINT16 usClientID, UINT16 usClientModuleID, UINT16 usMsgType, VOID *pvMsg, UINT32 ulMsgLen)=0;
    virtual VOID  SendBroadcastMsg(UINT16 usServerModuleID, UINT16 usClientModuleID, UINT16 usMsgType, VOID *pvMsg, UINT32 ulMsgLen)=0;
    virtual VOID  SendGroupMsg(UINT16 usServerModuleID, UINT16 usClientModuleID, UINT8 ucClientType, UINT16 usMsgType, VOID *pvMsg, UINT32 ulMsgLen)=0;
    virtual VOID  GetClientList(UINT16 *pusClientID, UINT32 *pulClientNum)=0;
    virtual BOOL  GetClientAddr(UINT16 usClientID, UINT8 *pucClientAddr, UINT16 *pusPort)=0;
    virtual BOOL  SyncSend(UINT16 usSenderModule, UINT16 usClientID, UINT16 usClientModuleID, UINT16 usMsgType, VOID *pvMsg, UINT32 ulMsgLen,
                            VOID *pvRspMsg, UINT32 ulMaxRspMsgLen, UINT32 *pulRspMsgLen, UINT32 ulTimeout)=0;
    virtual VOID  Run()=0;
    virtual BOOL  IsRunning()=0;
    virtual BOOL  Start()=0;
    virtual VOID  Close()=0;

protected:
    BOOL    m_bTCPMode;
};

#endif

