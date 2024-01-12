#ifndef THREAD_CHECK_H
#define THREAD_CHECK_H

#define EV_CHECK_STATUS_REPORT_IND      1

#define CHECK_MSG_MAGIC     "CHK!"

typedef struct
{
    UINT32      ulDefaultStatus;

    BOOL        bMulticastMode;

    CHAR        acProcName[32];     // 被检测进程名称，

    UINT8       aucPeerAddr[4];     // 组播时为组播地址，否则为对端地址
    UINT16      usPort;             // 组播时为组播端口，否则为UDP端口
    UINT8       aucMasterAddr[4];   // 主用状态时启用的地址
    UINT8       aucSlaveAddr[4];    // 备用状态时启用的地址
    UINT8       aucGateway[4];      // 网关

    UINT32      ulNetmask;
    UINT8       aucNetmask[4];      // 子网掩码

    CHAR        *szIfName;          // 主用/备用地址所属的接口名称

    UINT32      ulSwitchCheckTime;  // 主备切换监测总时长

    CHAR        acStatusFile[256];  // 进程状态文件，文件内容空表示正常

    UINT32      ulAddIPWaitTime;    // 增加IP地址的检测时间，缺省15秒
    UINT32      ulAddrDelaySetTime; // 切换到slave状态时，其ip生效的延迟时间

}CLUSTER_CFG_T;

typedef struct
{
    UINT8                   acMsgMagic[4];
    UINT16                  usMsgType;
    UINT16                  usMsgLen;
}CHECK_MSG_HDR;

typedef struct
{
    UINT8                   ucRunStatus;
    UINT8                   ucRunning;
}CHECK_STATUS_REPORT_IND_T;



class ThreadCheck: public GThread
{
public:
    ThreadCheck(CLUSTER_CFG_T &stClusterCfg);
    ~ThreadCheck();

    VOID Stop();

    void Test();

    virtual GOS_THREAD_RET ThreadEntry(void* pPara);

    BOOL   Init();
#ifdef _OSWIN32_
    VOID EnableRecvMsg(BOOL bEnabled);
    VOID EnableSendMsg(BOOL bEnabled);
#endif

private:
    CHAR        m_acLogName[32];

    BOOL        m_bEnableRecvMsg;
    BOOL        m_bEnableSendMsg;

    SOCKET      m_stSock;
    BOOL        m_bRunning;

    SOCKADDR_IN m_stPeerAddr;

    UINT8       m_aucMsg[1024];

    UINT32      m_ulFirstCheckStatusFileFailTime;

    UINT32      m_ulDefaultStatus;
    UINT32      m_ulRunStatus;
    UINT32      m_ulPeerRunStatus;
    BOOL        m_bIsPeerRunning;

    UINT32      m_ulLastSendCheckTime;
    UINT32      m_ulLastRecvCheckTime;
    CLUSTER_CFG_T   m_stClusterCfg;

    VOID Log(UINT32 ulLogLevel, const CHAR *szLog, ...);

    INT32 RecvMsg();
    VOID OnRecvMsg(UINT16 usMsgType, VOID *pvMsg, UINT16 usMsgLen);
    VOID OnCheckStatusReportInd(VOID *pvMsg, UINT16 usMsgLen);

    BOOL SendMsg(UINT16 usMsgType, VOID *pvMsg, UINT16 usMsgLen);
    VOID SendCheckMsg();
    VOID Check();
    VOID CheckMasterStatus();
    VOID CheckSlaveStatus();
    VOID SwitchToMaster();
    VOID SwitchToSlave();
    BOOL AddIP(UINT8 *pucAddr);
    BOOL DeleteIP(UINT8 *pucAddr);

    BOOL IsLocalRunning();
    const CHAR* GetPeerRunning();
};

extern BOOL StartThreadCheck(CLUSTER_CFG_T &stClusterCfg);
extern BOOL LoadClusterConfig(CHAR *szCfgFile, CLUSTER_CFG_T &stClusterCfg);

//---------------------------------------------------------------------------
#endif
