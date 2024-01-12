#ifndef TASK_CHECK_H
#define TASK_CHECK_H

#include "RecPublic.h"

class TaskCheck: public GBaseTask
{
public :
    TaskCheck(UINT16 usDispatchID);
    BOOL Init();
    BOOL AddRecFile(CHAR *acFile, CHAR *acDestFile, REC_FILE_INFO_T &stInfo, CHAR *acFileMonth, CHAR *acFileDay);
    virtual VOID TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);

private:
    CfgDao      *m_pDao;

    VOID OnCheckTimer();

};

#endif
