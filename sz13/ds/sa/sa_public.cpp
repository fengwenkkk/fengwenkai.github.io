#include "sa_public.h"
#include <fcntl.h>

#ifdef _OSWIN32_
#include "vram_cfg.h"
#else
#include "../ds_public/vram_cfg.c"
#endif

extern VOID gos_stop_process(const CHAR* szProcName);
extern VOID gos_start_process(const CHAR *szWorkDir, const CHAR *szProcCmd);

#ifdef _OSWIN32_

///常用配置参数的获取与设置
static CHAR *GetRamCfgFile()
{
    static CHAR        acFile[512];

    sprintf(acFile, "%s/%s", gos_get_root_path(), "vram_cfg.ini");
    if (!gos_file_exist(acFile))
    {
        GosLog(LOG_ERROR, "open file %s failed", acFile);
        return NULL;
    }

    return acFile;
}

static GConfFile* GetCfgFile()
{
    CHAR        acFile[512];
    GConfFile   *pConf = NULL;

    sprintf(acFile, "%s/%s", gos_get_root_path(), "sa.ini");
    if (!gos_file_exist(acFile))
    {
        GosLog(LOG_ERROR, "open file %s failed", acFile);
        return NULL;
    }

    pConf = new GConfFile(acFile);

    return pConf;
}

BOOL CfgGet(const CHAR *szKey, CHAR *szValue, UINT32 ulMaxLen)
{
    GConfFile   *pConf = GetCfgFile();

    if (!pConf)
    {
        return FALSE;
    }

    BOOL    bRet = pConf->Get(szKey, szValue, ulMaxLen);

    delete pConf;

    return bRet;
}

BOOL CfgSet(const CHAR *szKey, CHAR *szValue)
{
    GConfFile   *pConf = GetCfgFile();

    if (!pConf)
    {
        return FALSE;
    }

    BOOL    bRet = pConf->Set(szKey, szValue);

    delete pConf;

    return bRet;
}

BOOL CfgDel(const CHAR *szKey)
{
    GConfFile   *pConf = GetCfgFile();

    if (!pConf)
    {
        return FALSE;
    }

    BOOL    bRet = pConf->Del("", szKey);

    delete pConf;

    return bRet;
}

BOOL CfgCommit()
{
    return TRUE;
}

int ram_get(char *szKey ,char *szValue, UINT32 len)
{
    CHAR *szRamCfgFile = GetRamCfgFile();

    if(-1 == vram_get(szRamCfgFile, szKey, szValue, len))  
    {
        return -1;
    }

    return 0;
}

int ram_set(char *szKey ,char *szValue)
{
    CHAR *szRamCfgFile = GetRamCfgFile();

    if(-1 == vram_set(szRamCfgFile, szKey, szValue))
    {
        return -1;
    }
    
    return 0;
}

BOOL RawCfgGet(const CHAR *szKey, CHAR *szValue, UINT32 ulMaxLen)
{
    return CfgGet(szKey, szValue, ulMaxLen);
}

BOOL RawCfgSet(const CHAR *szKey, CHAR *szValue)
{
    return CfgSet(szKey, szValue);
}

#else
int uci_config_add(const char *name, char *value)
{
    char cmd[1024];

    sprintf(cmd, "uci add %s %s",name, value);
    //gw_log(LOG_INFO,"cmd %s",cmd);
    system(cmd);
    return 0;
}

int uci_config_set(const char *name, char *value)
{
    char cmd[1024];

    sprintf(cmd, "uci set %s=%s",name, value);

//  gw_log(LOG_INFO,"cmd %s",cmd);
    system(cmd);
    return 0;
}

int uci_config_del(const char *name)
{
    char cmd[1024];

    sprintf(cmd, "uci delete %s",name);
    system(cmd);
    return 0;
}


int uci_config_get(const char *name, char *value, UINT32 len)
{
    FILE *pf = NULL;
    char cmd[1024];
    char buf[1024] = {0};
    char *vl=NULL;

    memset((void*)value, 0, len);
    sprintf(cmd, "uci get %s",name);
 //   GosLog(LOG_ERROR, "%s %d cmd %s \n",__FUNCTION__,__LINE__,cmd);
    if((pf = popen(cmd, "r")) == NULL)
    {
        return -1;
    }

    if (fgets(buf, sizeof(buf),pf) == NULL)
    {
        pclose(pf);
        return -1;
    }

    vl = strstr(buf, "Entry not found");
    if(vl)
    {
//      GosLog(LOG_ERROR, "%s %d not find the entry!",__FUNCTION__,__LINE__);
        pclose(pf);
        return -1;
    }

    vl = strstr(buf,"\n");
    if(vl)
    {
        *vl='\0';
    }

    if (strlen(buf) >= len)
    {
        pclose(pf);
        return -1;
    }

    strcpy(value, buf);
  //  gw_log(LOG_INFO,"%s %d  find the entry!value (%s) buf (%s) len is %d \n",__FUNCTION__,__LINE__,value,buf,strlen(buf));

    pclose(pf);
    return 0;
}

int uci_config_commit(void)
{
    char cmd[128];

    sprintf(cmd, "uci commit");
    system(cmd);
    return 0;
}

BOOL CfgGet(const CHAR *szKey, CHAR *szValue, UINT32 ulMaxLen)
{
    char fullname[1024]={0};

    sprintf(fullname, "ta30.config.%s",szKey);

    return 0 == uci_config_get(fullname, szValue, ulMaxLen);
}

BOOL CfgSet(const CHAR *szKey, CHAR *szValue)
{
    char fullname[1024]={0};

    sprintf(fullname, "ta30.config.%s",szKey);

    return 0 == uci_config_set(fullname, szValue);
}

BOOL CfgDel(const CHAR *szKey)
{
    char fullname[1024]={0};

    sprintf(fullname, "ta30.config.%s",szKey);

    return 0 == uci_config_del(fullname);
}

BOOL CfgCommit()
{
    return 0 == uci_config_commit();
}

BOOL RawCfgGet(const CHAR *szKey, CHAR *szValue, UINT32 ulMaxLen)
{
    return 0 == uci_config_get(szKey, szValue, ulMaxLen);
}

BOOL RawCfgSet(const CHAR *szKey, CHAR *szValue)
{
    return 0 == uci_config_set(szKey, szValue);
}

int ram_get(char *szKey ,char *szValue, UINT32 len)
{
    CHAR *szRamCfgFile = (char*)"/tmp/vram.ini";

    if(-1 == vram_get(szRamCfgFile, szKey, szValue, len))  
    {
        return -1;
    }

    return 0;
}

int ram_set(char *szKey ,char *szValue)
{
    CHAR *szRamCfgFile = (char*)"/tmp/vram.ini";

    if(-1 == vram_set(szRamCfgFile, szKey, szValue))
    {
        return -1;
    }
    
    return 0;
}

#endif

BOOL CfgGetIP(const CHAR *szKey, UINT8 *pucAddr)
{
    CHAR    acValue[32];

    if (!CfgGet(szKey, acValue, sizeof(acValue)))
    {
        return FALSE;
    }

    return gos_get_ip(acValue, pucAddr);
}

BOOL CfgGet(const CHAR *szKey, UINT32 *pulValue)
{
    CHAR    acValue[32];

    if (!CfgGet(szKey, acValue, sizeof(acValue)))
    {
        return FALSE;
    }

    return gos_atou(acValue, pulValue);
}

BOOL CfgGet(const CHAR *szKey, INT32 *piValue)
{
    CHAR    acValue[32];

    if (!CfgGet(szKey, acValue, sizeof(acValue)))
    {
        return FALSE;
    }

    return gos_atoi(acValue, piValue);
}

BOOL CfgGet(const CHAR *szKey, UINT16 *pusValue)
{
    UINT32  ulValue;

    if (!CfgGet(szKey, &ulValue) || ulValue > 0xffff)
    {
        return FALSE;
    }

    *pusValue = (UINT16)ulValue;

    return TRUE;
}

BOOL CfgSet(const CHAR *szKey, UINT32 ulValue)
{
    CHAR    acValue[32];

    sprintf(acValue, "%u", ulValue);

    return CfgSet(szKey, acValue);
}

BOOL CfgSet(const CHAR *szKey, INT32 iValue)
{
    CHAR    acValue[32];

    sprintf(acValue, "%d", iValue);

    return CfgSet(szKey, acValue);
}

///临时参数配置与获取

int ram_set(char *szKey, UINT32 ulValue)
{
    CHAR    acValue[32];
    
    sprintf(acValue, "%u", ulValue);
     
    return ram_set(szKey, acValue);
}

int ram_set(char *szKey, INT32 i32Value)
{
    CHAR    acValue[32];
    
    sprintf(acValue, "%d", i32Value);
     
    return ram_set(szKey, acValue);
}


int ram_get(char *szKey ,char *szValue)
{
    if (-1 == ram_get(szKey, szValue, sizeof(szValue)))
    {
        return -1;
    }

    return 0;
}

int ram_get(char *szKey, UINT32 *pulValue)
{
    CHAR    acValue[32] = {0};
    
    if (-1 == ram_get(szKey, acValue, sizeof(acValue)-1))
    {
        return -1;
    }
    
    if (!gos_atou(acValue, pulValue))
    {
        return -1;
    }
    
    return 0;
}

int ram_get(char *szKey, INT32 *pi32Value)
{
    CHAR    acValue[32];
    
    if (-1 == ram_get(szKey, acValue, sizeof(acValue)-1))
    {
        return -1;
    }
    
    if (!gos_atoi(acValue, pi32Value))
    {
        return -1;
    }

    return 0;
}

int ram_get(char *szKey, UINT32 *pulValue, UINT32 ulLen)
{
    return ram_get(szKey, pulValue);
}

//ifconfig br-lan:1 172.16.0.1 netmask 255.255.255.0 up
BOOL SetRtspLocalAddr()
{
    CHAR    aucCmd[256] = {0};
    CHAR    aucRtspLocalIP[32] = {0};
    CHAR    aucRtspLocalMask[32] = {0};

    if (!CfgGet(CFG_PIS_IF_IP, aucRtspLocalIP, sizeof(aucRtspLocalIP)))
    {
        GosLog(LOG_ERROR, "uci get CFG_PIS_IF_IP failed!");
        return FALSE;
    }

    if (!CfgGet(CFG_PIS_IF_MASK, aucRtspLocalMask, sizeof(aucRtspLocalMask)))
    {
        GosLog(LOG_ERROR, "uci get CFG_PIS_IF_MASK failed!");
        return FALSE;
    }

    sprintf(aucCmd, "ifconfig br-lan:3 %s netmask %s up", aucRtspLocalIP, aucRtspLocalMask);

    gos_system(aucCmd);
 
    return TRUE;
}

// =======================================================================
BOOL LoadConfig()
{
    BOOL        bRet = FALSE;
    CHAR        acValue[32];

    // train_unit_id
    if (!CfgGet(CFG_TRAIN_UNIT_ID, &g_ulTrainUnitID))
    {
        GosLog(LOG_ERROR, "failed to read cfg %s", CFG_TRAIN_UNIT_ID);
        goto end;
    }

    GosLog(LOG_INFO, "cfg value of %s is %u", CFG_TRAIN_UNIT_ID, g_ulTrainUnitID);

    // local ip
    if (!RawCfgGet(CFG_LOCAL_IP, acValue, sizeof(acValue)))
    {
        GosLog(LOG_ERROR, "failed to read cfg %s", CFG_LOCAL_IP);
        goto end;
    }

    if (!gos_get_ip(acValue, g_aucLocalAddr))
    {
        GosLog(LOG_ERROR, "invalid cfg value of %s: %s", CFG_LOCAL_IP, acValue);
        goto end;
    }

    GosLog(LOG_INFO, "cfg value of %s is "IP_FMT, CFG_LOCAL_IP, IP_ARG(g_aucLocalAddr));

    // gateway
    if (!RawCfgGet(CFG_LOCAL_GW, acValue, sizeof(acValue)))
    {
        GosLog(LOG_ERROR, "failed to read cfg %s", CFG_LOCAL_GW);
        goto end;
    }

    if (!gos_get_ip(acValue, g_aucGateway))
    {
        GosLog(LOG_ERROR, "invalid cfg value of %s: %s", CFG_LOCAL_GW, acValue);
        goto end;
    }

    GosLog(LOG_INFO, "cfg value of %s is "IP_FMT, CFG_LOCAL_GW, IP_ARG(g_aucGateway));

    // dis
    if (!CfgGet(CFG_DIS_IP, acValue, sizeof(acValue)))
    {
        GosLog(LOG_ERROR, "failed to read cfg %s", CFG_DIS_IP);
        goto end;
    }

    if (!gos_get_ip_port(acValue, g_stSACfg.stDISAddr.aucIP, &g_stSACfg.stDISAddr.usPort))
    {
        GosLog(LOG_ERROR, "invalid cfg value of %s: %s", CFG_DIS_IP, acValue);
        goto end;
    }

    if (g_stSACfg.stDISAddr.usPort == 0)
    {
        g_stSACfg.stDISAddr.usPort = GOS_DEFAULT_SERVER_PORT;
    }

    GosLog(LOG_INFO, "cfg value of %s is " IP_FMT "(%u)", CFG_DIS_IP, IP_ARG(g_stSACfg.stDISAddr.aucIP), g_stSACfg.stDISAddr.usPort);
   // GosLog(LOG_INFO, "cfg value of %s is " IP_FMT "(%u)", CFG_DIS_IP, IP_ARG(g_stSACfg.stDISAddr.aucIP), HTONS(g_stSACfg.stDISAddr.usPort));

    /********************************载入PA配置*********************************************/
    // pA
    if (!CfgGet(CFG_PA_IP, acValue, sizeof(acValue)))
    {
        GosLog(LOG_ERROR, "failed to read cfg %s", CFG_PA_IP);
        goto end;
    }

    if (!gos_get_ip(acValue, g_stSACfg.stPATCPAddr.aucIP))
    {
        GosLog(LOG_ERROR, "invalid cfg value of %s: %s", CFG_PA_IP, acValue);
        goto end;
    }

    g_stSACfg.stPATCPAddr.usPort = PA_TCP_PORT;

    GosLog(LOG_INFO, "cfg value of %s is " IP_FMT "(%u)", CFG_PA_IP, IP_ARG(g_stSACfg.stPATCPAddr.aucIP), g_stSACfg.stPATCPAddr.usPort);

    memcpy(g_stSACfg.stPAUDPAddr.aucIP, g_stSACfg.stPATCPAddr.aucIP, sizeof(g_stSACfg.stPAUDPAddr.aucIP));
    g_stSACfg.stPAUDPAddr.usPort = PA_RECV_DC_PORT;   //UDP Port


    GosLog(LOG_INFO, "cfg value of %s is " IP_FMT "(%u)", CFG_PA_IP, IP_ARG(g_stSACfg.stPAUDPAddr.aucIP), g_stSACfg.stPAUDPAddr.usPort);
    /*********************************载入PA配置结束*********************************************/

    // pis
    if (!CfgGet(CFG_PIS_IP, acValue, sizeof(acValue)))
    {
        GosLog(LOG_ERROR, "failed to read cfg %s", CFG_PIS_IP);
        goto end;
    }

    if (!gos_get_ip(acValue, g_stSACfg.stPISAddr.aucIP))
    {
        GosLog(LOG_ERROR, "invalid cfg value of %s: %s", CFG_PIS_IP, acValue);
        goto end;
    }

    g_stSACfg.stPISAddr.usPort = PIS_TCP_PORT;

    GosLog(LOG_INFO, "cfg value of %s is " IP_FMT "(%u)", CFG_PIS_IP, IP_ARG(g_stSACfg.stPISAddr.aucIP), g_stSACfg.stPISAddr.usPort);

    // APP
    if (!CfgGet(CFG_APP_IP, acValue, sizeof(acValue)))
    {
        GosLog(LOG_ERROR, "failed to read cfg %s", CFG_APP_IP);
        goto end;
    }

    if (!gos_get_ip(acValue, g_stSACfg.stAPPAddr.aucIP))
    {
        GosLog(LOG_ERROR, "invalid cfg value of %s: %s", CFG_APP_IP, acValue);
        goto end;
    }

    GosLog(LOG_INFO, "cfg value of %s is " IP_FMT, CFG_APP_IP, IP_ARG(g_stSACfg.stAPPAddr.aucIP));

    // DIS heart beat
    g_stSACfg.ulDISHeartBeatTime = 3;
    if (!CfgGet(CFG_DIS_HEARTBEAT_TIME, &g_stSACfg.ulDISHeartBeatTime))
    {
        GosLog(LOG_ERROR, "invalid cfg value of %s", CFG_DIS_HEARTBEAT_TIME);
        g_stSACfg.ulDISHeartBeatTime = 3;
    }

    if (g_stSACfg.ulDISHeartBeatTime < 3)
    {
        g_stSACfg.ulDISHeartBeatTime = 3;
    }
    else if (g_stSACfg.ulDISHeartBeatTime > 300)
    {
        g_stSACfg.ulDISHeartBeatTime = 300;
    }

    GosLog(LOG_INFO, "cfg value of %s is %u", CFG_DIS_HEARTBEAT_TIME, g_stSACfg.ulDISHeartBeatTime);

    // PIS heart beat
    g_stSACfg.ulPISHeartBeatTime = 3;
    if (!CfgGet(CFG_PIS_HEARTBEAT_TIME, &g_stSACfg.ulPISHeartBeatTime))
    {
        GosLog(LOG_ERROR, "invalid cfg value of %s", CFG_PIS_HEARTBEAT_TIME);
        g_stSACfg.ulPISHeartBeatTime = 3;
    }

    if (g_stSACfg.ulPISHeartBeatTime < 5)
    {
        g_stSACfg.ulPISHeartBeatTime = 5;
    }
    else if (g_stSACfg.ulPISHeartBeatTime > 300)
    {
        g_stSACfg.ulPISHeartBeatTime = 300;
    }

    GosLog(LOG_INFO, "cfg value of %s is %u", CFG_PIS_HEARTBEAT_TIME, g_stSACfg.ulPISHeartBeatTime);

    g_stSACfg.bAutoPISDiscover = FALSE;
    if (!CfgGet(CFG_PIS_DISCOVER_AUTO, (UINT32*)&g_stSACfg.bAutoPISDiscover))
    {
        GosLog(LOG_ERROR, "invalid cfg value of %s", CFG_PIS_DISCOVER_AUTO);
        g_stSACfg.bAutoPISDiscover = TRUE;
    }

    GosLog(LOG_INFO, "cfg value of %s is %u", CFG_PIS_DISCOVER_AUTO, g_stSACfg.bAutoPISDiscover);

    g_stSACfg.bIPHVideoProxyMode = FALSE;
    g_stSACfg.usRTSPStartPort = DEFAULT_RTSP_START_PORT;
    g_stSACfg.usRTSPEndPort = 0xffff;

    bRet = TRUE;

end:
    return bRet;
}

VOID PrintMsg(VOID *pvMsg, UINT32 ulLen)
{
    UINT8   *pucMsg = (UINT8*)pvMsg;

    printf("Msg = ");

    for (UINT32 i=0; i<ulLen; i++)
    {
        printf("%02X", *pucMsg++);
        if (MOD(i+1, 8) == 0)
        {
            printf("\n");
        }
    }

    printf("\n");

}

#if 0
// such as rtsp://192.168.90.223:554/h264/ch1/sub/av_stream
BOOL ParseVideoURL(CHAR *szVideoURL, UINT8 *pucRTSPServerAddr, UINT16 &usRTSPServerPort)
{
    CHAR    *szStart;
    CHAR    *szEnd;

    szStart = strstr(szVideoURL, "rtsp://");
    if (!szStart)
    {
        return FALSE;
    }

    szStart += strlen("rtsp://");
    szEnd = strchr(szStart, '/');
    if (!szEnd)
    {
        return FALSE;
    }

    UINT32  ulLen = szEnd - szStart;

    if (ulLen >= 16)
    {
        return FALSE;
    }

    CHAR    acAddr[32];

    memcpy(acAddr, szStart, ulLen);
    acAddr[ulLen] = '\0';

    return gos_get_ip_port(acAddr, pucRTSPServerAddr, &usRTSPServerPort);
}
#endif

// 获取未分配的端口
static BOOL GetUsedPort(IntSet &sPort)
{
// netstat -ano
//  协议      本地地址          外部地址        状态           PID
//  TCP    0.0.0.0:135          0.0.0.0:0       LISTENING       1084
   
    FILE    *fp;
	CHAR    acBuf[1024];
    CHAR    *szLine;
    CHAR    *szTmp;
    UINT32  ulPort;

#ifdef _OSWIN32_
    const CHAR      *szCmd = "netstat -ano";
//  协议      本地地址          外部地址        状态           PID
//  TCP    0.0.0.0:135          0.0.0.0:0       LISTENING       1084
#else
    const CHAR      *szCmd = "netstat -tunla";
//  Proto   Recv-Q  Send-Q  Local Address   Froeign Address     State
//  tcp     0       0       :::23           :::*                LISTEN

#endif

    sPort.Clear();

    fp = gos_popen(szCmd, "r");
    if (fp == NULL) 
    {
	    return FALSE;
	}

	while (1)
    {
        memset(acBuf, 0, sizeof(acBuf));
        if (fgets(acBuf, sizeof(acBuf)-1, fp) == NULL)
        {
            break;
        }

        // Windows: TCP    0.0.0.0:135 
        // Linux: tcp     0       0       :::23    
        szLine = gos_left_trim_string(acBuf);
        szLine = strchr(szLine, ' ');
        if (!szLine)
        {
            continue;
        }

		szLine = gos_left_trim_string(szLine);

#ifndef _OSWIN32_
        // 跳过2个空格
        szLine = strchr(szLine, ' ');
        if (!szLine)
        {
            continue;
        }
        szLine = gos_left_trim_string(szLine);

        szLine = strchr(szLine, ' ');
        if (!szLine)
        {
            continue;
        }
        szLine = gos_left_trim_string(szLine);
#endif

        szTmp = strchr(szLine, ' ');
        if (!szTmp)
        {
            continue;
        }

        *szTmp = '\0';
        szLine = gos_right_strchr(szLine, ':');
        if (!szLine)
        {
            continue;
        }

        szLine++;

        if (!gos_atou(szLine, &ulPort) ||
            ulPort == 0 ||
            ulPort > 0xffff )
        {
            continue;
        }

        sPort.Add(ulPort);
	}

	gos_pclose(fp);

    return TRUE;
}

BOOL GetFreePort(UINT16 ulStartPort, UINT16 usEndPort, UINT16 *pusPort, UINT16 *pusPort2)
{
    if (!pusPort ||
        !pusPort2 )
    {
        return FALSE;
    }

    *pusPort = 0;
    *pusPort2 = 0;

    IntSet  sUsedPort;

    if (!GetUsedPort(sUsedPort))
    {
        return FALSE;
    }

    for (UINT16 usPort=ulStartPort; usPort<=usEndPort; usPort++)
    {
        if (sUsedPort.Has(usPort))
        {
            continue;
        }

        if ((*pusPort) == 0)
        {
            *pusPort = usPort;
        }
        else if ((*pusPort2) == 0)
        {
            *pusPort2 = usPort;
            return TRUE;
        }
    }

    return FALSE;
}

BOOL GetFreePort(UINT16 ulStartPort, UINT16 usEndPort, UINT16 *pusPort)
{
    if (!pusPort)
    {
        return FALSE;
    }

    *pusPort = 0;

    IntSet  sUsedPort;

    if (!GetUsedPort(sUsedPort))
    {
        GosLog(LOG_ERROR, "GetUsedPort failed");
        return FALSE;
    }

    for (UINT16 usPort=ulStartPort; usPort<=usEndPort; usPort++)
    {
        if (sUsedPort.Has(usPort))
        {
            continue;
        }

        if ((*pusPort) == 0)
        {
            *pusPort = usPort;
            return TRUE;
        }
    }

    return FALSE;
}

// 不允许reuseraddr，保证端口没有被其他应用使用
SOCKET CreateUDPServerSocket(UINT8 *pucAddr, UINT16 usPort)
{
    SOCKET              stSocket;
    SOCKADDR_IN         stLocalAddr = {0};

    gos_init_socket();

    stSocket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (stSocket == INVALID_SOCKET)
    {
        return INVALID_SOCKET;
    }

    memset(&stLocalAddr, 0, sizeof(stLocalAddr));
    stLocalAddr.sin_family = PF_INET;
    stLocalAddr.sin_port   = htons(usPort);
    if (pucAddr)
    {
        memcpy(&stLocalAddr.sin_addr.s_addr, pucAddr, 4);
    }

  /*  if (setsockopt(stSocket, SOL_SOCKET, SO_REUSEADDR, (CHAR*)&ulFlag, sizeof(ulFlag)) == SOCKET_ERROR)
    {
        CLOSE_SOCKET(stSocket);

        return INVALID_SOCKET;
    }*/

    if (bind (stSocket, (SOCKADDR *) &stLocalAddr, sizeof (SOCKADDR)) == SOCKET_ERROR)
    {
        CLOSE_SOCKET (stSocket);

        return INVALID_SOCKET;
    }

    return stSocket;
}

// 预先占用端口
BOOL PreAllocPort(UINT16 usPort, SOCKET &stSocket)
{
    stSocket = CreateUDPServerSocket(NULL, usPort);

    return stSocket != INVALID_SOCKET;
}

// TODO 目前写死，只支持一路RTSP
BOOL AllocLocalRTSPServerAddr(UINT8 *pucLocalRTSPServerAddr, UINT16 &usLocalRTSPServerPort, SOCKET &stSocket)
{
#if 0
    memcpy(pucLocalRTSPServerAddr, g_aucLocalAddr, sizeof(g_aucLocalAddr));
    usLocalRTSPServerPort = DEFAULT_RTP_PORT;

    return TRUE;
#else
    UINT32  ulStartPort = g_stSACfg.usRTSPStartPort;

    memcpy(pucLocalRTSPServerAddr, g_aucLocalAddr, sizeof(g_aucLocalAddr));

    //for (UINT32 ulStartPort=g_stSACfg.usRTSPStartPort; ulStartPort<g_stSACfg.usRTSPEndPort; ulStartPort++)
    for (UINT32 i=0; i<5; i++)
    {
        if (GetFreePort(ulStartPort, g_stSACfg.usRTSPEndPort, &usLocalRTSPServerPort) &&
            PreAllocPort(usLocalRTSPServerPort, stSocket))
        {
            return TRUE;
        }
    }

    return FALSE;
#endif
}

#ifdef _OSWIN32_
static const CHAR   *g_szProcRTSPProxy = "RTSPProxy.exe";
#else
static const CHAR   *g_szProcRTSPProxy = "rtsp_proxy";
#endif

// 目前只限制一路RTSP流
VOID StartRTSP(UINT8 ucCarriageID, UINT8 ucIPHDevID, CHAR *szRTSPServerAddr, CHAR *szLocalRTSPServer)
{
    StopRTSP(ucCarriageID, ucIPHDevID);

    CHAR    acWordDir[256];
    CHAR    acCmd[256];

#ifdef _OSWIN32_
    strcpy(acWordDir, gos_get_root_path());
#else
    strcpy(acWordDir, "/bin");
#endif

#ifdef _OSWIN32_
    CHAR    acNewProc[256];
    CHAR    acFullProc[256];
    CHAR    acOriProc[256];

    sprintf(acNewProc, "RTSPProxy%u%u.exe", ucCarriageID, ucIPHDevID);
    sprintf(acFullProc, "%s\\%s", gos_get_root_path(), acNewProc);
    sprintf(acOriProc, "%s\\%s", gos_get_root_path(), g_szProcRTSPProxy);

    gos_copy_file(acOriProc, acFullProc);

    sprintf(acCmd, "%s -s%s -l%s", acNewProc, szRTSPServerAddr, szLocalRTSPServer);
#else
    sprintf(acCmd, "%s -s%s -l%s -i%u%u", 
            g_szProcRTSPProxy, szRTSPServerAddr, szLocalRTSPServer, ucCarriageID, ucIPHDevID);
#endif

    gos_start_process(acWordDir, acCmd);
}

VOID StopRTSP(UINT8 ucCarriageID, UINT8 ucIPHDevID)
{
    CHAR    *szRawProcName = (CHAR*)g_szProcRTSPProxy;

#ifdef _OSWIN32_
    CHAR    acNewProc[256];

    sprintf(acNewProc, "RTSPProxy%u%u.exe", ucCarriageID, ucIPHDevID);

    gos_stop_process(acNewProc);
    gos_delete_file(acNewProc);

#else
    CHAR    acCmd[256];
    CHAR    acBuf[1024];
    CHAR    *szStart;
    CHAR    *szEnd;
    FILE    *fp;
    INT32   iProcID;
    UINT32  i;
    CHAR    acInst[32];

    sprintf(acInst, "-i%u%u", ucCarriageID, ucIPHDevID);

#ifdef _BUSYBOX_
    sprintf(acCmd, "ps |grep %s | grep -v grep 2>/dev/null", szRawProcName);
#else
    sprintf(acCmd, "ps  -ef |grep %s | grep -v grep 2>/dev/null", szRawProcName);
#endif

    fp = popen(acCmd, "r");
    if (!fp)
    {
        GosLog(LOG_ERROR, "popen failed!");
        return;
    }

    while(!feof(fp))
    {
        memset(acBuf, 0, sizeof(acBuf));
        if (fgets(acBuf, sizeof(acBuf), fp) == 0)
        {
            break;
        }

        for (i=0; acBuf[i]!='\0'; i++)
        {
            if (isspace(acBuf[i]))
            {
                acBuf[i] = ' ';
            }
        }

        // root      7909  7908  0 03:33 pts/2    00:00:03 /home/fritt/inms/agent/nms_agent_v1.0.0 -subnet_id=16
        if (acBuf[sizeof(acBuf)-1] != 0)
        {
            continue;
        }

        if (strstr(acBuf, acInst) == NULL)
        {
            continue;
        }

        szStart = gos_left_trim_string(acBuf);
        szStart = strchr(szStart, ' ');
        if (!szStart)
        {
            continue;
        }

        szStart = gos_left_trim_string(szStart+1);
        // szStart now like: 7909  7908  0 03:33 pts/2

        szEnd = strchr(szStart, ' ');
        if (!szEnd)
        {
            continue;
        }

        // szEnd now like:   7908  0 03:33 pts/2

        *szEnd = '\0';
        if (gos_atoi(szStart, &iProcID))
        {
            sprintf(acCmd, "kill -9 %d", iProcID);
            gos_system(acCmd);
        }
    }

    pclose(fp);

#endif
}