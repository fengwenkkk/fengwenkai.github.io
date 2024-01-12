#include <vcl.h>

#include "g_include.h"
#include "InnerMsg.h"

typedef struct
{
    UINT32          ulMsgType;
    TForm           *Form;
}INNER_MSG_MAP_T;

static BOOL             s_bReady = FALSE;
static BOOL             s_bClosed = FALSE;
static HANDLE           s_hInnerMsgMutex = NULL;
static std::vector<INNER_MSG_MAP_T>     s_vInnerMsg;

static BOOL Init()
{
    if (!s_hInnerMsgMutex)
    {
        s_hInnerMsgMutex = gos_mutex_init();
    }

    return s_hInnerMsgMutex != NULL;
}

static BOOL Lock()
{
    if (!Init())
    {
        return FALSE;
    }

    return gos_mutex(s_hInnerMsgMutex);
}

static VOID Unlock()
{
    gos_unmutex(s_hInnerMsgMutex);
}

/// 这个函数不调用，窗口中注册的异步消息是无法被触发的
VOID InnerMsgReady()
{
    if (Lock())
    {
        s_bReady = TRUE;
        Unlock();
    }
}

BOOL InitInnerMsg(UINT16 usMsgType, TForm *Form)
{
    if (!Lock())
    {
        return FALSE;
    }

    BOOL    bExist = FALSE;

    for (UINT32 i=0; i<s_vInnerMsg.size(); i++)
    {
        if (s_vInnerMsg[i].ulMsgType == (UINT32)usMsgType &&
            s_vInnerMsg[i].Form == Form)
        {
            bExist = TRUE;
            break;
        }
    }

    if (!bExist)
    {
        INNER_MSG_MAP_T stMsg;

        stMsg.ulMsgType = usMsgType;
        stMsg.Form = Form;

        s_vInnerMsg.push_back(stMsg);
    }

    Unlock();

    return TRUE;
}

BOOL CloseInnerMsg()
{
    if (!Lock())
    {
        return FALSE;
    }

    s_bClosed = TRUE;

    Unlock();

    return TRUE;
}

BOOL SendInnerMsg(HWND Handle, UINT32 ulMsgType, UINT8 *pucMsg, UINT32 ulMsgLen)
{
    if (!Handle)
    {
        return FALSE;
    }

    INNER_MSG_T     *pstMsg = (INNER_MSG_T*)malloc(STRUCT_OFFSET(INNER_MSG_T, aucMsg)+ulMsgLen);

    if (!pstMsg)
    {
        return FALSE;
    }

    pstMsg->ulMsgType = ulMsgType;
    pstMsg->ulMsgLen = ulMsgLen;
    if (ulMsgLen)
    {
        memcpy(pstMsg->aucMsg, pucMsg, ulMsgLen);
    }

    //发送成功由接收方释放消息
    if (!PostMessage(Handle, WM_INNER_MSG, 0, (LPARAM)pstMsg))
    {
        free(pstMsg);
        return FALSE;
    }

    return TRUE;
}

BOOL SendInnerMsg(UINT32 ulMsgType, UINT8 *pucMsg, UINT32 ulMsgLen)
{
    BOOL            bRet = FALSE;

    if (!Lock())
    {
        return FALSE;
    }

    if (!s_bReady || s_bClosed)
    {
        Unlock();
        return FALSE;
    }

    for (UINT32 i=0; i<s_vInnerMsg.size(); i++)
    {
        if (s_vInnerMsg[i].ulMsgType == ulMsgType)
        {
            if (!s_vInnerMsg[i].Form)
            {
                continue;
            }

            HWND Handle = (HWND)s_vInnerMsg[i].Form->Handle;

            SendInnerMsg(Handle, ulMsgType, pucMsg, ulMsgLen);
            bRet = TRUE;
        }
    }

    Unlock();

    return bRet;
}

