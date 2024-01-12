#ifndef _DTP_UDP_SERVER_H
#define _DTP_UDP_SERVER_H

#include "gos_typedef.h"
#include "gos_api.h"
#include "dtp_public.h"
#include "DTPServer.h"

class DTPUDPServer: public DTPServer
{
public:
    static DTPUDPServer* New(UINT16 usInstID, UINT8 *pucAddr, UINT16 usPort,
        DTPServerMsgHandler *pMsgHandle,
        GMemAlloc *pMemAlloc);

    ~DTPUDPServer();

    virtual BOOL  HasClient(UINT16 usClientID);
    virtual BOOL  Send(UINT16 usServerModuleID, UINT16 usClientID, UINT16 usClientModuleID, UINT16 usMsgType, UINT32 ulSeqID, VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulTimeout=0);
    virtual BOOL  SendReq(UINT16 usServerModuleID, UINT16 usClientID, UINT16 usClientModuleID, UINT16 usMsgType, VOID *pvMsg, UINT32 ulMsgLen);
    virtual VOID  SendBroadcastMsg(UINT16 usServerModuleID, UINT16 usClientModuleID, UINT16 usMsgType, VOID *pvMsg, UINT32 ulMsgLen);
    virtual VOID  SendGroupMsg(UINT16 usServerModuleID, UINT16 usClientModuleID, UINT8 ucClientType, UINT16 usMsgType, VOID *pvMsg, UINT32 ulMsgLen);
    virtual VOID  GetClientList(UINT16 *pusClientID, UINT32 *pulClientNum);
    virtual BOOL  GetClientAddr(UINT16 usClientID, UINT8 *pucClientAddr, UINT16 *pusPort);
    virtual BOOL  SyncSend(UINT16 usSenderModule, UINT16 usClientID, UINT16 usClientModuleID, UINT16 usMsgType, VOID *pvMsg, UINT32 ulMsgLen,
                           VOID *pvRspMsg,UINT32 ulMaxRspMsgLen,  UINT32 *pulRspMsgLen, UINT32 ulTimeout);
    virtual VOID  Run();
    virtual BOOL  IsRunning();
    virtual BOOL  Start();
    virtual VOID  Close();

    HANDLE  GetHandle(){return (HANDLE)m_pstServer;};
    VOID    FreeMsgNode(VOID *pvMsg);
    VOID    PrintConnect();

    BOOL    SocketSend(DTP_MSG_HDR_T *pstMsg, SOCKADDR_IN *pstServerAddr, BOOL bNeedLock=TRUE);
    BOOL    GetMsgStat(std::vector<DTP_MSG_STAT_T> &vStat);

private:
    DTP_SERVER_INFO_T   *m_pstServer;
    GMemAlloc           *m_pMemAlloc;
    DTPServerMsgHandler *m_pMsgHandle;
    GHASH               *m_hMsgStatHash;

    DTPUDPServer(DTPServerMsgHandler *pMsgHandle, GMemAlloc *pMemAlloc);

    BOOL Init(UINT16 usInstID, UINT8 *pucAddr, UINT16 usPort);

    DTP_MSG_T* AllocMsgNode(UINT32 ulMsgLen);
    BOOL SendEx(UINT16 usServerModuleID, UINT16 usClientID, UINT16 usClientModuleID,
               UINT16 usMsgType, UINT32 ulSeqID, VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulTimeout=0);

    BOOL HandleLinkAuthReqMsg(DTP_CONN_INFO_T *pstConn, DTP_UDP_LINK_AUTH_REQ_T *pstReq);
    BOOL HandleLinkReleaseReqMsg(DTP_CONN_INFO_T *pstConn, DTP_LINK_RELEASE_REQ_T *pstReq);

    BOOL HandleClientMsg(DTP_CONN_INFO_T *pstConnInfo, DTP_MSG_T *pstMsg);
    BOOL HandleLinkCheckReqMsg(DTP_CONN_INFO_T *pstConn, DTP_MSG_T *pstReq);
    BOOL SendBySocket(DTP_MSG_HDR_T *pstMsgHdr, SOCKADDR_IN *pstServerAddr);

    INT32 ListenMsg(UINT32 ulTimeout);
    INT32 RecvMsg(DTP_CONN_INFO_T *pstConn, INT32 *piErrCode);
    UINT32 CheckRecvMsg(DTP_CONN_INFO_T *pstConnInfo, UINT8 *pucMsg, UINT32 ulTotalMsgLen);

    SOCKET CreateSocket();

    BOOL _CheckClient(UINT16 usLoginID);
    BOOL AllocClientID(DTP_CONN_INFO_T *pstConn);

    VOID _CloseSocket(SOCKET *pstSocket);
    VOID _InitConnectInfo(DTP_CONN_INFO_T *pstConn, BOOL bNeedMutex);
    DTP_CONN_INFO_T* _SetConnectInfo(SOCKADDR_IN *pstAddr);

    UINT32 _GetSeqID();
    BOOL SendNow(UINT16 usServerModuleID, UINT16 usClientID, UINT16 usClientModuleID,
                 UINT16 usMsgType, UINT32 ulSeqID, VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulTimeout);
};

#endif
