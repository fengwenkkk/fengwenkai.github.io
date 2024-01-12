#ifndef SYS_TASK_H
#define SYS_TASK_H

typedef struct
{
    UINT16      usTaskID;
    BOOL        bInited;
    BOOL        bCheckTask;

    UINT32      ulLastCheckTime;
    UINT32      ulLastEchoTime;
}TASK_INFO_T;

class GSysTask: public GBaseTask
{
public:
    GSysTask(UINT16 usDispatchID);
    virtual VOID TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);
    BOOL RegisterTask(GBaseTask *pTask);

private:
    std::vector<TASK_INFO_T>    m_vTaskInfo;
    GMutex                      *m_Mutex;

    VOID CheckTask();
    BOOL GetMsgStat();

    VOID OnTaskInitMsg(VOID *pvMsg, UINT32 ulMsgLen);
    VOID OnTaskCheckMsg(VOID *pvMsg, UINT32 ulMsgLen);

    VOID SendTaskInitMsg(UINT16 usTaskID);
    VOID SendTaskCheckMsg(UINT16 usTaskID);
};

#endif
