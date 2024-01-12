#ifndef TASK_DB_SYNC_H
#define TASK_DB_SYNC_H

#include "DISPublic.h"

class TaskDBSync: public GBaseTask
{
public :
    TaskDBSync(UINT16 usDispatchID);

    BOOL Init();
    BOOL InitCheckTimer();

    virtual VOID TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen);

private:
    GMutex      *m_Mutex;
    CfgDao      *m_pDao;
    CfgDao      *m_pPeerDao;

    BOOL InitPeerDBEnv();
    BOOL InitPeerDBConn();
    VOID OnTimerCheck(VOID *pvMsg, UINT32 ulMsgLen);

    VOID BackupTable();
    VOID ClearTable();

    VOID SyncAppCfg();
    VOID SyncAutoJoinGroup();
    VOID SyncBrdZoneInfo();
    VOID SyncCellInfo();
    VOID SyncDCOperator();
    VOID SyncDCUser();
    VOID SyncDepot();
    VOID SyncGroupCallInfo();
    VOID SyncGroupCfg();
    VOID SyncGroupInfo();
    VOID SyncIPHTalkInfo();
    VOID SyncMultiGroup();
    VOID SyncOperLog();
    VOID SyncP2PCallInfo();
    VOID SyncPortalUE();
    VOID SyncPredefinedSMS();
    VOID SyncRealBrdInfo();
    VOID SyncRecBrdCfg();
    VOID SyncRecBrdInfo();
    VOID SyncRecBrdReply();
    VOID SyncReqCallInfo();
    VOID SyncSDSIndex();
    VOID SyncSDSInfo();
    VOID SyncSDSReply();
    VOID SyncSDSTemplate();
    VOID SyncDestInfo();
    VOID SyncStation();
    VOID SyncSection();
    VOID SyncStationUE();
    VOID SyncSubGroup();
    VOID SyncSubUser();
    VOID SyncSubVideoGroup();
    VOID SyncSubVideoUser();
    VOID SyncTrain();
    VOID SyncTrainUE();

};

#endif
