#ifndef INNER_MSG_H
#define INNER_MSG_H

#define WM_INNER_MSG                (WM_USER+0x00)
#define WM_DAS_MSG                  (WM_USER+0x01)

#pragma pack (push)
#pragma pack (4)

typedef struct
{
    UINT32      ulMsgType;
    UINT32      ulMsgLen;
    UINT8       aucMsg[8];
}INNER_MSG_T;

#pragma pack (pop)

VOID InnerMsgReady();
BOOL CloseInnerMsg();
BOOL SendInnerMsg(UINT32 ulMsgType, UINT8* pucMsg, UINT32 ulMsgLen);

#endif
