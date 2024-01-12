//---------------------------------------------------------------------------

#ifndef RemoteMsgH
#define RemoteMsgH
//---------------------------------------------------------------------------

#include "g_include.h"
#include "DTPClient.h"
#include "GCommon.h"
#include "InnerMsg.h"

typedef void (CLIENT_CONNECTION_CHANGE)(bool bConnected);

class RemoteMsg
{
public:
    /// static RemoteMsg* GetInstance();
    static RemoteMsg* New(UINT8 *pucServerAddr, UINT16 usServerPort, BOOL bTCPMode);
    BOOL Init(UINT8 *pucServerAddr, UINT16 usServerPort, BOOL bTCPMode);

    VOID GetClientAddr(UINT8 *pucAddr, UINT16 *pusPort=NULL);
    VOID SetClientConnectionChange(CLIENT_CONNECTION_CHANGE *pfClientConnectionChange);
    VOID SetServer(UINT8 *pucServerAddr, UINT16 usServerPort);
    BOOL IsConnected();
    BOOL SendReq(UINT16 usMsgType, VOID *pvMsg=NULL, UINT32 ulMsgLen=0);
    BOOL SyncSend(UINT16 usMsgType, VOID *pvMsg, UINT32 ulMsgLen, VOID* pvRspMsg,
                  UINT32 ulMaxRspMsgLen, UINT32 *pulRspMsgLen, UINT32 ulTimeout=0);
    VOID SetClientType(UINT8 ucClientType);
    CLIENT_CONNECTION_CHANGE* GetClientConnectionChangeFunc(){return m_pfClientConnectionChange;};

private:
    RemoteMsg();

    CLIENT_CONNECTION_CHANGE    *m_pfClientConnectionChange;

    DTPClient   *m_pDTPClient;
    BOOL        m_bReady;
};

extern RemoteMsg    *g_pRemoteMsg;

#endif
