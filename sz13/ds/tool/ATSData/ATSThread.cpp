#include "g_include.h"
#include "ats.h"
#include "ATSThread.h"

UINT16  g_usATSTCPPort = 2910;

extern GMutex   g_Mutex;
extern std::vector<ATS_INFO_T> g_vATSInfo;
extern UINT32 dtp_get_fd_num(FDSET* fds);
extern SOCKET dtp_get_socket_by_fds(FDSET *fds, UINT32 ulIndex);

//---------------------------------------------------------------------------
void ATSInfoToNet(ATS_INFO_T *pstATSInfo)
{
    HTONL_IT(pstATSInfo->ulTrainUnitID);      // 车组号
    HTONL_IT(pstATSInfo->ulTrainServiceNumber);   // 服务号
    HTONS_IT(pstATSInfo->usTrainSerialNumber);    // 序列号
    HTONS_IT(pstATSInfo->usDriveNumber);          // 司机号
    HTONS_IT(pstATSInfo->usRollingStockNumber);   // 几节编组
    HTONS_IT(pstATSInfo->usStationID);            // 当前车站编号
    HTONS_IT(pstATSInfo->usUpsideStationID);      // 列车上行侧车站的编号
    HTONS_IT(pstATSInfo->usDownSideStationID);
}

void GetATSInfo(UINT32 ulMaxNum, GET_ATS_INFO_RSP_T *pstRsp)
{
    g_Mutex.Mutex();

    UINT32 ulNum = ulMaxNum;

    if (ulNum > g_vATSInfo.size())
    {
        ulNum = g_vATSInfo.size();
    }

    for (UINT32 i=0; i<ulNum; i++)
    {
        memcpy(&pstRsp->astATSInfo[i], &g_vATSInfo[i], sizeof(ATS_INFO_T));
        ATSInfoToNet(&pstRsp->astATSInfo[i]);
    }

    pstRsp->usTrainNum = ulNum;         // 在线列车数

    pstRsp->stHdr.usLength = 3 + sizeof(UINT16) + ulMaxNum*sizeof(ATS_INFO_T);

    g_Mutex.Unmutex();
}

static void PrintSocketErr()
{
    CHAR    *szErr = gos_get_socket_err();

    szErr += 0;
}

#define PRINT_SOCK_ERR  PrintSocketErr()

static SOCKET CreateSocket()
{
    SOCKET              stSocket = INVALID_SOCKET;
    SOCKADDR_IN         stLocalAddr = {0};
    UINT32              ulFlag = 1;

    gos_init_socket();

    stLocalAddr.sin_family = PF_INET;
    stLocalAddr.sin_port   = HTONS(g_usATSTCPPort);
    memset(&stLocalAddr.sin_addr.s_addr, 0, 4);

    stSocket = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (stSocket == INVALID_SOCKET)
    {
        PRINT_SOCK_ERR;
       // Log(DTP_LOG_ERR, "server_create_socket: create socket failed: %s", gos_get_socket_err());
        return INVALID_SOCKET;
    }

    if (setsockopt(stSocket, SOL_SOCKET, SO_REUSEADDR, (CHAR*)&ulFlag, sizeof(ulFlag)) == SOCKET_ERROR)
    {
       // Log(DTP_LOG_ERR, "server_create_socket: setsockopt failed, %s", gos_get_socket_err());
        CLOSE_SOCKET (stSocket);

        return INVALID_SOCKET;
    }

    if (bind (stSocket, (SOCKADDR *) &stLocalAddr, sizeof (SOCKADDR)) == SOCKET_ERROR)
    {
        //Log(DTP_LOG_ERR, "server_create_socket: bind socket failed, %s", gos_get_socket_err());
        CLOSE_SOCKET (stSocket);

        return INVALID_SOCKET;
    }

    // mpk
    if (listen (stSocket, 16) == SOCKET_ERROR)
    {
    //    Log(DTP_LOG_ERR, "server_create_socket: listen socket failed, %s", gos_get_socket_err());
        CLOSE_SOCKET (stSocket);

        return INVALID_SOCKET;
    }

    return stSocket;
}

INT32 ATSThread::RecvMsg(SOCKET stSocket)
{
    INT32       iRecvSize;
    UINT32      ulByteToRecved = 0;

    /* 看看有多少字节可读 */
    if (ioctlsocket(stSocket, FIONREAD, (unsigned long*)&ulByteToRecved) == SOCKET_ERROR)
    {
        return -1;
    }

    /* 如果没有字节可读, 则对端已经关闭连接, 返回错误 */
    if (ulByteToRecved == 0)
    {
        return 0;
    }

    /* 处理上次RecvMsg的剩余数据 */
    iRecvSize = recv(stSocket, (CHAR*)m_aucRecvBuf, sizeof(m_aucRecvBuf), 0);
    if (iRecvSize != sizeof(GET_ATS_INFO_REQ_T))
    {
        return -1;
    }

    return iRecvSize;
}

INT32 ATSThread::ListenMsg(UINT32 ulTimeout)
{
    FDSET       fds;
    FDSET       fdsAll;
    TIMEVAL     stTimeout = {0};
    SOCKET      stClientSocket = INVALID_SOCKET;
    SOCKADDR    stAddr;
    INT32       iAddrLen = sizeof(SOCKADDR);
    INT32       iRet;
    INT32       iRecvLen;
    INT32       iErrCode = 0;
    UINT32      ulFdCount;
    UINT32      ulTotalFdCount;
    UINT32      i;
    INT32       iNoDelay = 1;
    BOOL        bNeedClose;
    SOCKET      stSocket = m_stSocket;

    FD_ZERO(&fdsAll);
    FD_SET(stSocket, &fdsAll);

again:
    fds = fdsAll;

    stTimeout.tv_sec = ulTimeout;
    iRet = select(FD_SETSIZE, &fds, NULL, NULL, &stTimeout);
    if (iRet == 0)
    {
        goto again;
    }

    if (iRet < 0)
    {
        if (errno == EINTR)
        {
            goto again;
        }
    }

    if (FD_ISSET(stSocket, &fds))
    {
       // FD_CLR(stSocket, &fds);
accept_again:
        stClientSocket = accept (stSocket, &stAddr, (socklen_t*)&iAddrLen);
        if (stClientSocket == SOCKET_ERROR)
        {
            if (/*errno == ECONNABORTED || */ errno == EINTR)
            {
                goto accept_again;
            }

            goto again;
            //return -1;
        }

        /* 设置NoDelay方式 */
        setsockopt(stClientSocket, IPPROTO_TCP, TCP_NODELAY, (CHAR*)&iNoDelay, sizeof(iNoDelay));

        FD_SET(stClientSocket, &fdsAll);
    }

    /* 处理 tcp 端口上的消息 */
    ulFdCount = 0;
    ulTotalFdCount = dtp_get_fd_num(&fds);
    for (i = 0; i < FD_SETSIZE; i++)
    {
        bNeedClose = FALSE;

        if (ulFdCount >= ulTotalFdCount)
        {
            break;
        }

        stClientSocket = dtp_get_socket_by_fds(&fds, i);
        if (stClientSocket == INVALID_SOCKET)
        {
            continue;
        }

        if (stClientSocket == stSocket)
        {
            continue;
        }

        ulFdCount++;

        if (!FD_ISSET(stClientSocket, &fds))
        {
            continue;
        }

        /* 有消息到, 进行处理 */
        iErrCode = 0;
        iRecvLen = RecvMsg(stClientSocket);
        if (iRecvLen <= 0)
        {
            FD_CLR(stClientSocket, &fdsAll);
            CLOSE_SOCKET(stClientSocket);
        }
        else
        {
            HandleRecvMsg(stClientSocket);
        }
    }

    goto again;
}

VOID ATSThread::HandleRecvMsg(SOCKET stClientSocket)
{
    GET_ATS_INFO_REQ_T  *pstReq = (GET_ATS_INFO_REQ_T*)m_aucRecvBuf;
    static GET_ATS_INFO_RSP_T   stRsp;
    UINT32  ulMaxATSNum = NTOHS(pstReq->usRegisterNum)*sizeof(UINT16)/sizeof(ATS_INFO_T);

    memset(&stRsp, 0, sizeof(stRsp));
    stRsp.stHdr.usTransID = pstReq->stHdr.usTransID;
    stRsp.stHdr.usProtocolID = HTONS(ATS_PROTOCOL_ID);
    stRsp.stHdr.ucUnitID = ATS_UNIT_ID;

    stRsp.ucFunctionCode = ATS_FUNC_CODE_RSP;       // 功能码   0x04, 如果失败，则为0x84
    stRsp.ucByteCount = 0;

    GetATSInfo(ulMaxATSNum, &stRsp);

    HTONS_IT(stRsp.stHdr.usLength);
    HTONS_IT(stRsp.usTrainNum);

    gos_tcp_send(stClientSocket, &stRsp, sizeof(stRsp));

}

ATSThread::ATSThread():GThread(NULL)
{
}

GOS_THREAD_RET ATSThread::ThreadEntry(void* pPara)
{
    UINT32      ulTimeout = 2;

    m_stSocket = CreateSocket();
    if (m_stSocket == INVALID_SOCKET)
    {
        exit(0);
        return 0;
    }

    while(1)
    {
        ListenMsg(ulTimeout);
    }
}
