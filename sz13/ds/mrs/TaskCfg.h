#ifndef TASK_CFG_H
#define TASK_CFG_H

#include "RecPublic.h"
#include "CfgDao.h"


class TaskCfg: public GBaseTask
{
public :
    TaskCfg(UINT16 usDispatchID);

    BOOL Init();
    virtual VOID TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);

private:
    BOOL        m_bInitOperatro;
    CHAR        m_acOperatorID[32];

    CfgDao      *m_pDao;

    virtual VOID OnServerDisconnectClient(UINT16 usClientID);

    VOID AddOperLog(const CHAR *szOperLog, ...);

    VOID OnCheckTimer();

    VOID OnClientReq(CHAR *szReq, UINT32 ulMsgLen);

    VOID SendConfirmSaveRsp(UINT32 ulSeqID);

    VOID OnLogin(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnLogout(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnSaveXXXReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnGetOperatorReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnAddOperatorReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnDelOperatorReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);
    VOID OnSetOperatorReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnSetPwdReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnGetRecFileListReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnDcUpFileReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnSPTLoadNTPCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnRECTest(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

    VOID OnAsyncCallTest(CHAR *szReq, UINT32 ulSeqID, GJson &Json);

};

#endif
