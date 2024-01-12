#ifndef TASK_REC_H
#define TASK_REC_H

#include "RecPublic.h"

class TaskRec: public GBaseTask
{
public :
    TaskRec(UINT16 usDispatchID);
    BOOL Init();
    virtual VOID TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);

private:

    VOID OnCheckTimer();

};

#endif
