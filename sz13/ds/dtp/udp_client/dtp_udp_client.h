#ifndef _DTP_UDP_CLIENT_H
#define _DTP_UDP_CLIENT_H

#include "g_include.h"
#include "dtp_public.h"
#include "DTPClient.h"

class DTPUDPClient:public DTPClient
{
public:
    static DTPUDPClient* New(DTP_CLIENT_CFG_T *pstClientCfg, DTPClientMsgHandler *pMsgHandle, GMemAlloc *pMemAlloc);
    ~DTPUDPClient();

    virtual VOID SetClientType(UINT8 ucClientType);
    virtual BOOL Init(DTP_CLIENT_CFG_T *pstClientCfg);
    virtual VOID SetServer(UINT8 *pucServerAddr, UINT16 usServerPort);
    virtual VOID GetServerAddr(UINT8 *pucAddr, UINT16 *pusPort);
    virtual VOID GetClientAddr(UINT8 *pucAddr, UINT16 *pusPort);

    virtual VOID Run();
    virtual VOID Start();
    virtual VOID Close();
    virtual VOID Suspend();
    virtual VOID Resume();

    virtual BOOL IsConnected();
    virtual UINT16 GetServerID();
    virtual VOID SetServerID(UINT16 usServerID);
    virtual BOOL Send(UINT16 usSenderModule, UINT16 usReceiverModule, UINT16 usMsgType, UINT32 ulSeqID, VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulTimeout=0);

    virtual BOOL SendReq(UINT16 usSenderModule, UINT16 usReceiverModule, UINT16 usMsgType, VOID *pvMsg, UINT32 ulMsgLen);

    virtual BOOL SyncSend(UINT16 usSenderModule, UINT16 usReceiverModule, UINT16 usMsgType, VOID *pvReqMsg, UINT32 ulReqMsgLen,
                          VOID *pvRspMsg, UINT32 ulMaxRspMsgLen, UINT32 *pulRspMsgLen, UINT32 ulTimeout);

    VOID   FreeMsgNode(VOID *pvMsg);
    DTP_CLIENT_INFO_T* GetHandle(){return m_pstClient;}

    BOOL SocketSend(SOCKET stSocket, VOID *pvMsg, UINT32 ulMsgLen, SOCKADDR_IN *pstServerAddr, BOOL bNeedLock=TRUE);

private:
    DTP_CLIENT_INFO_T       *m_pstClient;
    DTPClientMsgHandler     *m_pMsgHandle;
    GMemAlloc               *m_pMemAlloc;

    DTPUDPClient(DTPClientMsgHandler *pMsgHandle, GMemAlloc *pMemAlloc);

    BOOL Stop();
    BOOL ReleaseLink();

    BOOL SendNow(UINT16 usSenderModule, UINT16 usReceiverModule, UINT16 usMsgType, UINT32 ulSeqID, VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulTimeout=0);
    BOOL SendEx(UINT16 usSenderModule, UINT16 usReceiverModule, UINT16 usMsgType, UINT32 ulSeqID, VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulTimeout=0);

    BOOL AuthLink();
    BOOL MainLoop();

    BOOL   CheckLink();
    UINT8* AllocMsgNode(UINT32 ulMsgLen);

    BOOL Connect();
    VOID CloseConnect();
    BOOL RecvMsg();

    BOOL HandleRecvMsg(DTP_MSG_T *pstMsg);

    BOOL HandleLinkCheckRsp(DTP_LINK_CHECK_RSP_T *pstRsp);
    BOOL HandleLinkCheckReq(DTP_LINK_CHECK_REQ_T *pstReq);
    BOOL HandleLinkReleaseRsp(DTP_LINK_RELEASE_RSP_T *pstRsp);

    UINT32 _GetSeqID();
};

#endif

