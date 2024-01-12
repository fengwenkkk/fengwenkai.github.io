#ifndef HttpThreadH
#define HttpThreadH

#include "g_include.h"

class HttpThread:public GThread
{
public:
    HttpThread();
    GOS_THREAD_RET ThreadEntry(void* pPara);

private:
    SOCKET      m_stSocket;
    UINT8       m_aucRecvBuf[64*1024];

    VOID        HandleRecvMsg(SOCKET stClientSocket);
    INT32       RecvMsg(SOCKET stSocket);
    INT32       ListenMsg(UINT32 ulTimeout);
};


#endif
