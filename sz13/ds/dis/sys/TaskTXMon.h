#ifndef TASK_TX_MON_H
#define TASK_TX_MON_H

#include "DISPublic.h"

class TaskTXMon: public GBaseTask
{
public :
    TaskTXMon(UINT16 usDispatchID, TELNET_INFO_T *pstInfo, UINT32 ulTrainUnitID, UINT32 ulTC);

    BOOL Init();
    virtual VOID TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);

private:
    CHAR        m_acMsg[64*1024];
    CfgDao      *m_pDao;
    GTelnet     *m_pTelnet;
    UINT32      m_ulTrainUnitID;
    UINT32      m_ulTC;
    UINT32      m_ulCellID;

    virtual VOID OnServerDisconnectClient(UINT16 usClientID);

    VOID CheckRecBrdInfo();

    VOID OnCheckTimer();
};

#endif
