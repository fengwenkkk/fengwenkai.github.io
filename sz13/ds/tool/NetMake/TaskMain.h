#ifndef TaskMainH
#define TaskMainH


class TaskMain: public GBaseTask
{
public :
    TaskMain(UINT16 usDispatchID);

    BOOL Init();
    virtual VOID TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);

private:
    BOOL            m_bLoaded;

    VOID OnTimerCheckInd();
};

#endif
