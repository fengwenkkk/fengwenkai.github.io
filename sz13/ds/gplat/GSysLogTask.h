#ifndef SYS_LOG_TASK
#define SYS_LOG_TASK

typedef struct
{
    UINT32                  ulTime;
    UINT32                  ulLogLevel;
    CHAR                    acLogModule[MAX_LOG_MODULE_LEN+1];
    CHAR                    *szLog;
    UINT32                  ulLen;
}LOG_MODULE_BUFFER_T;

class GBufferLog
{
public:
    static GBufferLog* GetInstance();
    static GBufferLog* GetInstance(UINT32 ulMaxLogNum);
    UINT32 GetMaxLogNum(){return m_ulMaxLogNum;}

    GList* GetList(){return m_pLogList;}

    BOOL Add(LOG_MODULE_BUFFER_T *pLog);

    BOOL Remove(UINT32 ulNum = 1);              //删除前Num条

    BOOL Save(CHAR *szFileName);
    BOOL Load(CHAR *szFileName);

    BOOL FreeList();
    BOOL Init(UINT32 ulMaxLogNum);

    GList *m_pLogList;
    HANDLE m_MemPool;

private:
    GBufferLog();
    ~GBufferLog();

    BOOL InitList();
    BOOL InitMemPool();
    BOOL RemoveNode(UINT32 ulLen);

    GMutex  *m_pLogMutex;

    UINT8   *m_pucCurrent;                          //当前数据储存地址
    UINT32  m_ulCount;
    UINT32  m_ulDefaultLen;
    UINT32  m_ulMaxLogNum;
};

class GSysLogTask: public GBaseTask
{
public :
    GSysLogTask(UINT16 usDispatchID);
    ~GSysLogTask();
    virtual VOID TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);

    VOID OnGetLogCfgReq(VOID *pvMsg, UINT32 ulMsgLen);
    VOID OnSetLogCfgReq(VOID *pvMsg, UINT32 ulMsgLen);

    VOID OnGetLogReq(VOID *pvMsg, UINT32 ulMsgLen);
    VOID OnInitLogReq();

};

#endif
