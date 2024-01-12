#include "g_include.h"
#include "ds_public.h"
#include "pis.h"
#include "InnerMsg.h"
#include "DiscoverThread.h"

typedef struct ip_mreq
{
    struct in_addr  imr_multiaddr; //多播组的IP地址
    struct in_addr  imr_interface; //加入的客服端主机IP地址
}IP_MREQ;

// from ws2ipdef.h
#define IP_MULTICAST_TTL          10 // IP multicast TTL (hop limit).
#define IP_MULTICAST_LOOP         11 // IP multicast loopback.
#define IP_ADD_MEMBERSHIP         12 // Add an IP group membership.
#define IP_DROP_MEMBERSHIP        13 // Drop an IP group membership.

DiscoverThread::DiscoverThread(HWND hWnd, UINT8 *pucLocalAddr, UINT8 *pucTrainAddr):GThread(NULL)
{
    m_hWnd = hWnd;
    memcpy(m_aucLocalAddr, pucLocalAddr, 4);

    memset(m_aucTrainAddr, 0, sizeof(m_aucTrainAddr));
    if (pucTrainAddr)
    {
        memcpy(m_aucTrainAddr, pucTrainAddr, 4);
    }
}

BOOL DiscoverThread::DiscoverPISServer(UINT8 *pucLocalAddr)
{
    SOCKET      stSock;
    SOCKADDR_IN stLocalAddr;
    SOCKADDR_IN stPeerAddr;
    SOCKADDR_IN stPISAddr;
    IP_MREQ     stMulticast;
    UINT8       aucRecvBuf[1024];
    TIMEVAL     tv;
    FD_SET      fds;
    INT32       iRet;
    INT32       iReuse = 1;
    INT32       iRecvLen;
    UINT8       ucLoop;
    socklen_t   iSocketLen = sizeof(SOCKADDR_IN);
    UINT32      i;
    PIS_FIND_PIS_REQ_T  *pstReq = (PIS_FIND_PIS_REQ_T*)aucRecvBuf;
    PIS_FIND_PIS_RSP_T  stRsp;

    printf("pis server start\n");

    gos_init_socket();
    stSock = socket(AF_INET, SOCK_DGRAM, 0);
    if(stSock <= 0)
    {
        printf("create socket failed\n");
        return FALSE;
    }

    /*端口释放后立即就可以被再次使用*/
    if (setsockopt(stSock, SOL_SOCKET, SO_REUSEADDR, (char *)&iReuse, sizeof(iReuse)) < 0)
    {
        printf("Setting SO_REUSEADDR failed\n");
        goto err;
    }

    memset(&stLocalAddr, 0, sizeof(stLocalAddr));
    stLocalAddr.sin_family = AF_INET;
    stLocalAddr.sin_port = htons(PIS_MULTICAST_PORT);
    memcpy(&stLocalAddr.sin_addr.s_addr, pucLocalAddr, 4);
    //stAddr.sin_addr.s_addr = INADDR_ANY;

    memset(&stPeerAddr, 0, sizeof(stPeerAddr));
    stPeerAddr.sin_family = AF_INET;
    stPeerAddr.sin_port = htons(PIS_MULTICAST_PORT);
    stPeerAddr.sin_addr.s_addr = inet_addr(PIS_MULTICAST_ADDR);

    memset(&stMulticast, 0, sizeof(stMulticast));
    stMulticast.imr_multiaddr.s_addr = inet_addr(PIS_MULTICAST_ADDR);
    //stMulticast.imr_interface.s_addr = INADDR_ANY;
    memcpy(&stMulticast.imr_interface.s_addr, pucLocalAddr, 4);

    if (setsockopt(stSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&stMulticast, sizeof(stMulticast)) < 0)
        //if(setsockopt(stSock, IPPROTO_IP, 12, (char *)&stMulticast, sizeof(stMulticast)) < 0)
    {
        printf("setsockopt multicast failed\n");
        goto err;
    }

    ucLoop = 0;
    if (setsockopt(stSock, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&ucLoop, sizeof(ucLoop)) < 0)
    {
        printf("setsockopt multicast loop fail!\n");
        goto err;
    }

    if (bind(stSock, (struct sockaddr*)&stLocalAddr, sizeof(stLocalAddr)) != 0)
    {
        printf("bind failed:%s\n", gos_get_socket_err());
        goto err;
    }

    for (i=0; i<0xffffffff; i++)
    {
        FD_ZERO(&fds);
        FD_SET(stSock, &fds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        iRet = select(stSock + 1, &fds, NULL, NULL, &tv);
        if (iRet < 0)
        {
            printf("select err\n");
            goto err;
        }
        else if (iRet == 0)
        {
            printf("wait msg\n");
            continue;
        }

        if (FD_ISSET(stSock, &fds))
        {
            memset(aucRecvBuf, 0, sizeof(aucRecvBuf));
            iRecvLen = recvfrom(stSock, (char*)aucRecvBuf, sizeof(aucRecvBuf), 0, (SOCKADDR*)&stPISAddr, &iSocketLen);

            if (GET_INT(m_aucTrainAddr) != 0)
            {
                if (memcmp(&stPISAddr.sin_addr, m_aucTrainAddr, 4) != 0)
                {
                    continue;
                }
            }

            if (iRecvLen != sizeof(PIS_FIND_PIS_REQ_T))
            {
                printf("Recv msg len should be %d\n", iRecvLen);
                continue;//goto err;
            }
            else
            {
                printf("Recv discover PIS req msg, PIS addr is "IP_FMT", RADIO addr is "IP_FMT", Gateway addr is "IP_FMT"\n",
                        IP_ARG(pstReq->aucPISAddr), IP_ARG(pstReq->aucLocalAddr), IP_ARG(pstReq->aucGateway));

                PIS_FIND_PIS_REQ_T  stMsg;

                memcpy(&stMsg, pstReq, sizeof(PIS_FIND_PIS_REQ_T));
                SendMsg(stMsg);

                memcpy(stRsp.aucPISAddr, pstReq->aucPISAddr, sizeof(stRsp.aucPISAddr));
                //gos_udp_send(stSock, &stRsp, sizeof(stRsp), &stPISAddr);
                gos_udp_send(stSock, &stRsp, sizeof(stRsp), &stPeerAddr); // 用组播地址发送应答消息
            }
        }
    }

    closesocket(stSock);
    return TRUE;

err:
    closesocket(stSock);
    return FALSE;
}

VOID DiscoverThread::SendMsg(PIS_FIND_PIS_REQ_T &stMsg)
{
    UINT32          ulMsgLen = sizeof(stMsg);
    INNER_MSG_T     *pstMsg = (INNER_MSG_T*)calloc(1, STRUCT_OFFSET(INNER_MSG_T, aucMsg)+ulMsgLen);

    if (!pstMsg)
    {
        return;
    }

    pstMsg->ulMsgType = MSG_DISCOVER;
    pstMsg->ulMsgLen = ulMsgLen;
    memcpy(pstMsg->aucMsg, &stMsg, pstMsg->ulMsgLen);

    if (!PostMessage(m_hWnd, WM_INNER_MSG, 0, (LPARAM)pstMsg))
    {
        free(pstMsg);
    }
}

GOS_THREAD_RET DiscoverThread::ThreadEntry(void* pPara)
{
    while(1)
    {
        if (!DiscoverPISServer(m_aucLocalAddr))
        {
            gos_sleep_1ms(100);
        }
    }
}
