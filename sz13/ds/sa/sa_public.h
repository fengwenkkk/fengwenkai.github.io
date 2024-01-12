#ifndef SA_PUBLIC_H
#define SA_PUBLIC_H

#include "g_include.h"
#include "dtp_public.h"
#include "GCommon.h"
#include "msg.h"
#include "ds_public.h"
#include "pis.h"

#include "sa_dis_msg.h"
#include "sa_pa_msg.h"
#include "sa_iph_msg.h"

#define MAX_MSG_BUF_SIZE    (32*1024)

#define RECV_FAIL           0xffffffff
#define RECV_TIMEOUT        0
#define RECV_NEEDMORE       2
#define RECV_DIS_SUCC       3
#define RECV_PIS_SUCC       4
#define RECV_PA_SUCC        5
#define RECV_MSG_INVALID    10

typedef struct
{
    IP_ADDR_T   stDISAddr;
    IP_ADDR_T   stPISAddr;

    IP_ADDR_T   stPATCPAddr; //添加PA_IP loadcfg时载入　
    IP_ADDR_T   stPAUDPAddr;

    IP_ADDR_T   stAPPAddr;

    UINT32      ulPISHeartBeatTime; // 单位秒
    UINT32      ulDISHeartBeatTime; // 单位秒
    UINT32      ulSelfCheckMaxTime; // 单位秒

    BOOL        bAutoPISDiscover;   // 尝试连接PIS，不通则发起组播

    BOOL        bIPHVideoProxyMode; // TRUE采用RTSP代理模式，缺省FALSE
    UINT16      usRTSPStartPort;
    UINT16      usRTSPEndPort;
}SA_CFG_T;

extern SA_CFG_T g_stSACfg;
extern SOCKET   g_stDISUDPClientSock;
extern SOCKET   g_stPISTCPClientSock;

extern UINT32   g_ulTrainUnitID;

extern SOCKADDR_IN g_stDISAddr;
extern SOCKADDR_IN g_stPAUDPAddr;


extern CHAR        g_acSAID[128];
extern UINT8       g_aucLocalAddr[4];
extern UINT8       g_aucGateway[4];

extern SOCKET      g_stDISUDPClientSock;
extern SOCKET      g_stPISTCPClientSock;
extern SOCKET      g_stPATCPClientSock;
extern SOCKET      g_stPAUDPClientSock;


BOOL LoadConfig();

BOOL CfgGet(const CHAR *szKey, CHAR *szValue, UINT32 ulMaxLen);
BOOL CfgGetIP(const CHAR *szKey, UINT8 *pucAddr);
BOOL CfgGet(const CHAR *szKey, UINT32 *pulValue);
BOOL CfgGet(const CHAR *szKey, INT32 *piValue);
BOOL CfgGet(const CHAR *szKey, UINT16 *pusValue);
BOOL CfgSet(const CHAR *szKey, CHAR *szValue);
BOOL CfgSet(const CHAR *szKey, UINT32 ulValue);
BOOL CfgCommit();

VOID PrintMsg(VOID *pvMsg, UINT32 ulLen);

VOID InitPISSocket();
VOID InitPASocket();

BOOL AllocLocalRTSPServerAddr(UINT8 *pucLocalRTSPServerAddr, UINT16 &usLocalRTSPServerPort, SOCKET &stSocket);
VOID StartRTSP(UINT8 ucCarriageID, UINT8 ucIPHDevID, CHAR *szRTSPServerAddr, CHAR *szLocalRTSPServer);
VOID StopRTSP(UINT8 ucCarriageID, UINT8 ucIPHDevID);

VOID GetRtspSwitchStatus();
BOOL SetRtspLocalAddr();

int ram_get(CHAR *szKey, CHAR *szValue, UINT32 ulMaxLen);
int ram_get(CHAR *szKey, UINT32*pulValue);
int ram_get(char *szKey, INT32 *pi32Value);
int ram_get(CHAR *szKey, UINT32*pulValue, UINT32 ulMaxLen);
int ram_set(CHAR *szKey, CHAR *szValue);
int ram_set(char *szKey, UINT32 ulValue);
int ram_set(char *szKey, INT32 i32Value);

#endif
