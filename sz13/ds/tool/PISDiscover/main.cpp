//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "g_include.h"
#include "pis.h"
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;

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

UINT8 g_aucLocalAddr[4];
UINT8 g_aucGateway[4];
UINT8 g_aucPISAddr[4];

BOOL DiscoverPISServer(UINT8 *pucLocalAddr, UINT8 *pucGateway, UINT8 *pucPISAddr)
{
    SOCKET      stSock;
    SOCKADDR_IN stLocalAddr;
    SOCKADDR_IN stPeerAddr;
    IP_MREQ     stMulticast;
    UINT8       aucRecvBuf[1024];
    TIMEVAL     tv;
    FD_SET      fds;
    INT32       iRet;
    INT32       iReuse = 1;
    INT32       iRecvLen;
    UINT32      i;
    BOOL        bRet = FALSE;
    UINT8       ucLoop;
    PIS_FIND_PIS_REQ_T  stReq;
    PIS_FIND_PIS_RSP_T  *pstRsp = (PIS_FIND_PIS_RSP_T*)aucRecvBuf;

    printf("dev discover start\n");

    memcpy(stReq.aucLocalAddr, pucLocalAddr, sizeof(stReq.aucLocalAddr));
    memcpy(stReq.aucGateway, pucGateway, sizeof(stReq.aucGateway));
    memcpy(stReq.aucPISAddr, pucPISAddr, sizeof(stReq.aucPISAddr));

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
//  stAddr.sin_addr.s_addr = inet_addr(PIS_MULTICAST_ADDR);
    memcpy(&stLocalAddr.sin_addr.s_addr, pucLocalAddr, 4);

    memset(&stPeerAddr, 0, sizeof(stPeerAddr));
    stPeerAddr.sin_family = AF_INET;
    stPeerAddr.sin_port = htons(PIS_MULTICAST_PORT);
    stPeerAddr.sin_addr.s_addr = inet_addr(PIS_MULTICAST_ADDR);

    memset(&stMulticast, 0, sizeof(stMulticast));
    stMulticast.imr_multiaddr.s_addr = inet_addr(PIS_MULTICAST_ADDR);
   //   stMulticast.imr_interface.s_addr = INADDR_ANY;      // 发送组播消息的源主机地址
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

    gos_udp_send(stSock, &stReq, sizeof(stReq), &stPeerAddr);

    for (i=0; i<5; i++)
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
            continue;
        }

        if (FD_ISSET(stSock, &fds))
        {
            memset(aucRecvBuf, 0, sizeof(aucRecvBuf));
            iRecvLen = recv(stSock, (char*)aucRecvBuf, sizeof(aucRecvBuf), 0);
            if (iRecvLen != sizeof(PIS_FIND_PIS_RSP_T))
            {
            //  printf("Recv msg len should be %d\n", iRecvLen);
                continue;
            }
            else
            {
                printf("Recv PIS server ack msg, PIS addr is "IP_FMT"\n", IP_ARG(pstRsp->aucPISAddr));

                if (memcmp(pucPISAddr, pstRsp->aucPISAddr, sizeof(pstRsp->aucPISAddr)) != 0)
                {
                    printf("PIS addr should be "IP_FMT"\n", IP_ARG(pucPISAddr));
                    goto err;
                }

                bRet = TRUE;
                break;
            }
        }
    }

err:
    closesocket(stSock);
    return bRet;
}
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TForm(Owner)
{
    gos_init_socket();
}

void ShowMsg(const char *szInfo)
{
    String  strInfo = szInfo;
    wchar_t *szCaption = L"提示";

    Application->MessageBoxW(strInfo.c_str(), szCaption);
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::btnSetClick(TObject *Sender)
{
    if (!GetIP(edtLocalAddr->Text, g_aucLocalAddr))
    {
        ShowMsg("车载台地址错误");
        return;
    }

    if (!GetIP(edtGateway->Text, g_aucGateway))
    {
        ShowMsg("网关地址错误");
        return;
    }

    if (!GetIP(edtPISAddr->Text, g_aucPISAddr))
    {
        ShowMsg("PIS地址错误");
        return;
    }

    BOOL bRet = DiscoverPISServer(g_aucLocalAddr, g_aucGateway, g_aucPISAddr);

    if (!bRet)
    {
        ShowMsg("测试失败");
    }
    else
    {
        ShowMsg("测试成功");
    }
}
void __fastcall TFormMain::btnCloseClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------

