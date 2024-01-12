#ifndef BASE_TASK_H
#define BASE_TASK_H

typedef struct
{
    UINT16                      usMsgID;
    UINT8                       ucUniqueMsg;
    GOS_UNIQUE_MSG_CMP_FUNC     *pfUniqueMsgCmp;
}TASK_MSG_DESC_T;

#define ALLOC_BUFFER(_STRUCT)   (_STRUCT*)AllocBuffer(sizeof(_STRUCT))

//#define TaskLog(ulLogLevel, ...)  { _TaskLog(__FILE__, __FUNCTION__, __LINE__, ulLogLevel, ##__VA_ARGS__);}
//#define TaskMsgLog(ulLogLevel, ...)   { _TaskMsgLog(__FILE__, __FUNCTION__, __LINE__, ulLogLevel, ##__VA_ARGS__); }

#define TaskLog(ulLogLevel, ...)        GosLog(GetTaskName(), ulLogLevel, ##__VA_ARGS__)
#define TaskMsgLog(ulLogLevel, ...)     GosMsgLog(GetTaskName(), ulLogLevel, ##__VA_ARGS__)


class GBaseTask
{
public :
    GBaseTask(UINT16 usTaskID, const CHAR *szTaskName, UINT16 usDispathID, BOOL bCheckTask=TRUE);

    virtual VOID TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)=0;

    VOID SetSenderType(UINT8 ucSenderType){m_ucSenderType=ucSenderType;}
    UINT8 GetSenderType(){return m_ucSenderType;}

    VOID SetTaskName(const CHAR *szTaskName);
    CHAR* GetTaskName(){return m_szTaskName;}

    VOID RegisterMsg(UINT16 usMsgID, BOOL bUniqueMsg=FALSE, GOS_UNIQUE_MSG_CMP_FUNC *pfUniqueMsgCmp=NULL);
    std::vector<TASK_MSG_DESC_T>& GetMsgList();

    UINT16 GetTaskID(){return m_usTaskID;}
    UINT16 GetDispatchID(){return m_usDispatchID;}
    VOID SetDispatchID(UINT16 usDispatchID){m_usDispatchID = usDispatchID;}

    BOOL SetTimer(UINT16 usTimerID, UINT32 ulTimeout, UINT32 ulPara);
    BOOL SetTimer(UINT16 usTimerID, UINT32 ulTimeout, VOID *pvPara=NULL);
    BOOL SetLoopTimer(UINT16 usTimerID, UINT32 ulTimeout, UINT32 ulPara);
    BOOL SetLoopTimer(UINT16 usTimerID, UINT32 ulTimeout, VOID *pvPara=NULL);
    BOOL ClearTimer(UINT16 usTimerID);

    UINT32 GetTaskStatus(){return m_ulTaskStatus;}
    VOID SetTaskStatus(UINT32 ulTaskStatus){m_ulTaskStatus=ulTaskStatus;}

    VOID GetSelfPID(PID_T *pstPID);
    BOOL SendServer(UINT16 usMsgID, VOID *pvMsg=NULL, UINT32 ulMsgLen=0);
    BOOL SendServer(UINT16 usServerTaskID, UINT16 usMsgID, VOID *pvMsg=NULL, UINT32 ulMsgLen=0);
    BOOL SendServer(PID_T *pstServerPID, UINT16 usMsgID, VOID *pvMsg=NULL, UINT32 ulMsgLen=0);
    BOOL SendServer(PEER_ADDR_T *pstServerAddr, UINT16 usMsgID, VOID *pvMsg=NULL, UINT32 ulMsgLen=0);
    BOOL SendServer(PEER_ADDR_T *pstServerAddr, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulSeqID);

    BOOL SendLocal(UINT16 usReceiverTaskID, UINT16 usMsgID, CHAR *szMsg);
    BOOL SendLocal(UINT16 usReceiverTaskID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);
    UINT32 GetReqMsgSeqID();
    BOOL SendRsp();
    BOOL SendRsp(CHAR *szMsg);
    BOOL SendRsp(VOID *pvMsg, UINT32 ulMsgLen);
    BOOL SendRsp(const CHAR *szMsgName, UINT32 ulSeqID, BOOL bRet);
    BOOL SendRsp(const CHAR *szMsgName, UINT32 ulSeqID, GJsonParam &Param);
    BOOL SendRsp(const CHAR *szMsgName, UINT32 ulSeqID, const std::string& strMsg);
    BOOL SendRsp(const CHAR *szMsgName, UINT32 ulSeqID, GJsonTupleParam &Param);
    VOID SendBroadcast(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);
    BOOL SyncSendClient(PID_T *pstPID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen, VOID *pvRspMsg, UINT32 ulMaxRspLen, UINT32 *ulRspMsgLen, UINT32 ulTimeout);
    VOID SetMsgSender(PID_T *pstSender){memcpy(&m_stSender, pstSender, sizeof(PID_T));}
    VOID SetMsgSeqID(UINT32 ulMsgSeqID){m_ulMsgSeqID = ulMsgSeqID;}

    VOID SetMsgID(UINT16 usMsgID){m_usMsgID = usMsgID;}
    VOID SetMsgData(VOID *pvMsgData){m_pvMsgData = pvMsgData;}
    VOID SetMsgLen(UINT32 ulMsgLen){m_ulMsgLen = ulMsgLen;}

    BOOL SendClient(PID_T *pstPID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen, UINT32 ulSeqID);
    BOOL SendClient(PID_T *pstPID, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);
    BOOL SendClient(PID_T *pstPID, UINT16 usMsgID, const CHAR *szMsgName, UINT32 ulSeqID, GJsonParam &Param);
    VOID SendAllClient(UINT8 ucClientType, UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);
    VOID SendAllClient(UINT8 ucClientType, UINT16 usMsgID, CHAR *szMsg);
    VOID SendAllClient(UINT8 ucClientType, UINT16 usMsgID, const CHAR *szMsgName, UINT32 ulSeqID, GJsonParam &Param);
    VOID SendAllClient(UINT8 ucClientType, UINT16 usMsgID, const CHAR *szMsgName, UINT32 ulSeqID, const std::string &strMsgInfo);
    VOID SendAllClient(UINT8 ucClientType, UINT16 usMsgID, const CHAR *szMsgName, UINT32 ulSeqID, GJsonTupleParam &Param);

    PID_T* GetMsgSender(){return &m_stSender;}
    VOID   GetMsgSender(PID_T *pstSender){memcpy(pstSender, &m_stSender, sizeof(PID_T));}
    UINT16 GetMsgSenderInstID(){return m_stSender.usInstID;}
    UINT32 GetMsgSeqID(){return m_ulMsgSeqID;}
    UINT16 GetMsgID(){return m_usMsgID;}
    VOID* GetMsgData(){return m_pvMsgData;}
    UINT32 GetMsgLen(){return m_ulMsgLen;}

    UINT32 GetMsgStartTime(){return m_ulMsgStartTime;}
    UINT32 GetMsgEndTime(){return m_ulMsgEndTime;}
    VOID   SetMsgStartTime(UINT32 ulTime){m_ulMsgStartTime = ulTime;}
    VOID   SetMsgEndTime(UINT32 ulTime){m_ulMsgEndTime = ulTime;}

    BOOL GetCheckTask(){return m_bCheckTask;}

    virtual UINT16 GetServerID(PEER_ADDR_T *pstServerAddr);

    virtual BOOL Start(PEER_ADDR_T *pstServerAddr);
    virtual BOOL Start(PEER_ADDR_T *pstServerAddr, UINT16 ulServerID);
    virtual BOOL Stop(PEER_ADDR_T *pstServerAddr);

    virtual VOID OnClientConnectServer(UINT16 usServerID){};
    virtual VOID OnClientDisconnectServer(UINT16 usServerID){};
    virtual VOID OnServerAcceptClient(UINT16 usClientID){};
    virtual VOID OnServerDisconnectClient(UINT16 usClientID){};

protected:
    VOID*                   AllocBuffer(UINT32 ulBufSize);
    VOID                    FreeBuffer();
    VOID                    OnTaskCheck();

    VOID _TaskLog(const CHAR *szFile, const CHAR *szFunc, UINT32 ulLine, UINT32 ulLogLevel, const CHAR *szFormat, ...);
    VOID _TaskMsgLog(const CHAR *szFile, const CHAR *szFunc, UINT32 ulLine, UINT32 ulLogLevel, const CHAR *szFormat, ...);

private:
    UINT16                  m_usTaskID;
    UINT16                  m_usDispatchID;
    CHAR                    *m_szTaskName;
    UINT8                   m_ucSenderType;

    std::vector<TASK_MSG_DESC_T>    m_vMsgList;

    UINT32                  m_ulTaskStatus;

    PID_T                   m_stSender;         /* 当前接收到的消息的消息源 */
    UINT32                  m_ulMsgSeqID;       /* 当前接收到的消息的序列号 */
    UINT16                  m_usMsgID;          /* 当前接收到的消息的消息ID */
    VOID                    *m_pvMsgData;
    UINT32                  m_ulMsgLen;

    UINT32                  m_ulBufSize;
    VOID                    *m_pvBuf;

    UINT32                  m_ulLocalMsgSeqID;

    UINT32                  m_ulMsgStartTime;
    UINT32                  m_ulMsgEndTime;
    BOOL                    m_bCheckTask;
};

#endif
