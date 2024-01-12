//---------------------------------------------------------------------------

#ifndef MakeMsgH
#define MakeMsgH

typedef struct
{
    IP_ADDR_T   stPeerAddr;
    UINT16      usLen;
    UINT8       aucMsg[1];
}INNER_MAKE_MSG_T;

bool InitUDPServer(UINT16 usPort);
bool SendPeer(IP_ADDR_T *pstPeerAddr, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);

#endif
