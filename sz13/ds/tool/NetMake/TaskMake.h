#ifndef TAskMakeH
#define TAskMakeH


class TaskMake: public GBaseTask
{
public :
    TaskMake(UINT16 usDispatchID, UINT16 usTaskID);

    BOOL Init();
    virtual VOID TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);

private:
    BOOL            m_bLoaded;


};

#endif
