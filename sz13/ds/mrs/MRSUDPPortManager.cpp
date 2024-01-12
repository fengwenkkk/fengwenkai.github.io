#include "RecPublic.h"
#include "MRSUDPPortManager.h"

GMutex                          g_MutexUDPPort;
static UINT32                   g_ulMinUDPPortNum = 20; //预分配20个socket

extern UINT32          g_ulLowerUDPPort; ///mrs.ini中获取该配置，若没有则默认为51000
extern UINT32          g_ulUpperUDPPort; ///mrs.ini中获取该配置，若没有则默认为56999

MrsUDPPortManager::MrsUDPPortManager()
{
    m_pmUDPInfo = new RawIntMap<UDP_INFO_T>;
}

MrsUDPPortManager::~MrsUDPPortManager()
{
    if (!m_pmUDPInfo)
    {
        delete m_pmUDPInfo;
    }
}

VOID MrsUDPPortManager::FreeUDPPort(UINT16 usPort)
{
    g_MutexUDPPort.Mutex();

    UDP_INFO_T  *pstInfo = m_pmUDPInfo->GetValue(usPort);

    if (pstInfo)
    {
        if (m_pmUDPInfo->Size() > g_ulMinUDPPortNum)
        {
            GosLog(LOG_INFO, "free port %u, close socket %u", usPort, pstInfo->stSocket);
            CLOSE_SOCKET(pstInfo->stSocket);
            m_pmUDPInfo->Del(usPort);
        }
        else
        {
            pstInfo->bAlloced = FALSE;
        }
    }

    g_MutexUDPPort.Unmutex();
}

//获取已分配端口
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

SOCKET MrsUDPPortManager::GetSocket(UINT16 usPort, CHAR *szBusinessName)
{
    if ( usPort<g_ulLowerUDPPort || usPort>g_ulUpperUDPPort )
    {
        GosLog(LOG_ERROR, "business(%s) port(%u) is unknow", szBusinessName, usPort);

        return INVALID_SOCKET;
    }

    g_MutexUDPPort.Mutex();

    UDP_INFO_T  *pstInfo = m_pmUDPInfo->GetValue(usPort);

    if (pstInfo)
    {
        g_MutexUDPPort.Unmutex();
        return pstInfo->stSocket;
    }

    g_MutexUDPPort.Unmutex();

    return INVALID_SOCKET;
}

UINT16 MrsUDPPortManager::AllocUDPPort()
{
    UINT32    usPort = 0;
    UINT8     aucAddr[] = {0,0,0,0};
    SOCKET    stSocket  = INVALID_SOCKET;
    VECTOR<UDP_INFO_T*>   vInfo;

    IntSet  sUsedPort;

    if (!GetUsedPort(sUsedPort))
    {
        return FALSE;
    }

    g_MutexUDPPort.Mutex();

    //优先遍历之前使用过的端口，并且检查端口占用情况
    m_pmUDPInfo->GetValueList(vInfo); 
    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        if ( !vInfo[i]->bAlloced )
        {
            vInfo[i]->bAlloced = TRUE;
            usPort = vInfo[i]->usPort;
            stSocket = vInfo[i]->stSocket;
            g_MutexUDPPort.Unmutex();

            GosLog(LOG_INFO, "alloc socket %u on port %u, total socket is %u", stSocket, usPort, m_pmUDPInfo->Size());

            return usPort;
        }
    }

    for (UINT32 ulPortIndex=g_ulLowerUDPPort; ulPortIndex<g_ulUpperUDPPort; ulPortIndex++)
    {
        if (sUsedPort.Has(ulPortIndex))
        {
            continue;
        }

        if (m_pmUDPInfo->Has(ulPortIndex))
        {
            continue;
        }

        stSocket = gos_create_udp_server_socket(aucAddr, ulPortIndex);
        if (stSocket != INVALID_SOCKET)
        {
            UDP_INFO_T      stInfo = {0};

            stInfo.bAlloced = TRUE; /// 预分配的端口此时设置为true
            stInfo.stSocket = stSocket;
            stInfo.usPort   = ulPortIndex;
            
            UDP_INFO_T  *pstInfo = m_pmUDPInfo->Add(ulPortIndex, &stInfo);

            g_MutexUDPPort.Unmutex();

            if (!pstInfo)
            {
                return 0;
            }

            usPort = ulPortIndex;

            GosLog(LOG_INFO, "create socket %u on port %u, total socket is %u", stSocket, usPort, m_pmUDPInfo->Size());
            return usPort;
        }
    }

    g_MutexUDPPort.Unmutex();

    GosLog(LOG_ERROR, "get free UDP port failed");
    return 0;
}