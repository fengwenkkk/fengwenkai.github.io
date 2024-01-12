#ifndef _DTPCLIENT_H
#define _DTPCLIENT_H

#include "g_include.h"
#include "dtp_public.h"

class DTPClient;

class DTPClientMsgHandler//: public DTPMsgHandler
{
    friend class DTPClient;

public:
    VOID SetDTPClient(DTPClient *pDTPClient){m_pDTPClient = pDTPClient;}
    DTPClient* GetDTPClient(){return m_pDTPClient;}

    virtual VOID MsgHandle(UINT16 usServerID, DTP_MSG_T *pstMsg)=0;
    virtual VOID OnConnectServer(UINT16 usServerID)=0;
    virtual VOID OnDisconnectServer(UINT16 usServerID)=0;

protected:
    DTPClient   *m_pDTPClient;
};

class DTPClient
{
public:
    static DTPClient* New(DTP_CLIENT_CFG_T *pstClientCfg, DTPClientMsgHandler *pMsgHandle, GMemAlloc *pMemAlloc, BOOL bTCPMode);

    static VOID Free(DTPClient *pDTPClient);
    virtual VOID SetClientType(UINT8 ucClientType)=0;
    virtual BOOL Init(DTP_CLIENT_CFG_T *pstClientCfg)=0;
    virtual VOID SetServer(UINT8 *pucServerAddr, UINT16 usServerPort)=0;
    virtual VOID GetServerAddr(UINT8 *pucAddr, UINT16 *pusPort)=0;
    virtual VOID GetClientAddr(UINT8 *pucAddr, UINT16 *pusPort)=0;

    virtual VOID Run()=0;
    virtual VOID Close()=0;
    virtual VOID Suspend()=0;
    virtual VOID Resume()=0;

    virtual BOOL IsConnected()=0;
    virtual UINT16 GetServerID()=0;
    virtual VOID SetServerID(UINT16 usServerID)=0;

    virtual BOOL Send(UINT16 usSenderModule, UINT16 usReceiverModule, UINT16 usMsgType, UINT32 ulSeqID, VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulTimeout=0)=0;

    virtual BOOL SendReq(UINT16 usSenderModule, UINT16 usReceiverModule, UINT16 usMsgType, VOID *pvMsg, UINT32 ulMsgLen)=0;

    virtual BOOL SyncSend(UINT16 usSenderModule, UINT16 usReceiverModule, UINT16 usMsgType, VOID *pvReqMsg, UINT32 ulReqMsgLen,
                          VOID *pvRspMsg, UINT32 ulMaxRspMsgLen, UINT32 *pulRspMsgLen, UINT32 ulTimeout)=0;

protected:
    BOOL    m_bTCPMode;

    ~DTPClient();
};

#endif

