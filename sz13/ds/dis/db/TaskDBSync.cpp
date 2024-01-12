#include "TaskDBSync.h"
#include "TaskATS.h"

#define TIMER_CHECK_STATUS                  TIMER4
#define EV_CHECK_STATUS_TIMER               EV_TIMER4

static UINT32  g_ulCheckPeriod = 3;
static UINT32  g_ulSyncRecNum = 300;

PDBEnvBase      *g_PeerPDBEnv = NULL;

extern PDB_CONN_PARAM_T    g_stPeerConnParam;

BOOL TaskDBSync::InitPeerDBEnv()
{
    /*初始化数据库环境 */
    if (g_stPeerConnParam.ulDBType == PDBT_NOSQL)
    {
        g_PeerPDBEnv = new PDBNoSQLEnv();
    }
#if 1//def HAVE_MYSQL
    else if (g_stPeerConnParam.ulDBType == PDBT_MYSQL)
    {
        g_PeerPDBEnv = new PDBMySQLEnv();
    }
#endif
#ifdef _OSWIN32_
    else if (g_stPeerConnParam.ulDBType & PDBT_ODBC)
    {
        g_PeerPDBEnv = new PDBODBCEnv();
    }
#endif
#ifdef HAVE_ORACLE
    else if (g_stPeerConnParam.ulDBType == PDBT_ORACLE)
    {
        g_PeerPDBEnv = new PDBOracleEnv();
    }
#endif

    if (g_PeerPDBEnv == NULL)
    {
        TaskLog(LOG_FATAL, "init peer db env failed, db type is %u", g_stPeerConnParam.ulDBType);
        return FALSE;
    }

    TaskLog(LOG_INFO, "peer db param: type=%u, server=%s, port=%u, dbname=%s, character-set=%s",
        g_stPeerConnParam.ulDBType, g_stPeerConnParam.acServer, g_stPeerConnParam.usPort,
        g_stPeerConnParam.acDBName, g_stPeerConnParam.acCharacterSet);

    return TRUE;
}

BOOL TaskDBSync::InitPeerDBConn()
{
    static BOOL bInit = FALSE;

    if (bInit)
    {
        return TRUE;
    }

    UINT32  ulTimeout = 10;
    BOOL bRet = g_PeerPDBEnv->InitEnv(&g_stPeerConnParam,
                                PDB_PREFETCH_ROW_NUM,
                                ulTimeout,
                                1);

    if (PDB_SUCCESS != bRet)
    {
        TaskLog(LOG_FATAL, "init peer db connection failed");
        return FALSE;
    }

    bInit = TRUE;
    TaskLog(LOG_FATAL, "init peer db connection successful!");
    return TRUE;
}

TaskDBSync::TaskDBSync(UINT16 usDispatchID):GBaseTask(MODULE_DBSYNC, (CHAR*)"Sync", usDispatchID)
{
    m_pDao = NULL;
    m_pPeerDao = NULL;
}

BOOL TaskDBSync::Init()
{
    if (!m_pDao)
    {
        m_pDao = GetCfgDao();
        if (m_pDao == NULL)
        {
            return FALSE;
        }
    }

    if (!g_PeerPDBEnv)
    {
        if (!InitPeerDBEnv())
        {
            return FALSE;
        }
    }

    if (!m_pPeerDao)
    {
        if (!InitPeerDBConn())
        {
            return FALSE;
        }

        m_pPeerDao = GetCfgDao(g_PeerPDBEnv);
        if (m_pDao == NULL)
        {
            return FALSE;
        }
    }

    if(!InitCheckTimer())
    {
        TaskLog(LOG_ERROR, "init check timer failed");
        return FALSE;
    }

    TaskLog(LOG_INFO, "Task init successful!");
    SetTaskStatus(TASK_STATUS_WORK);

    return TRUE;
}

BOOL TaskDBSync::InitCheckTimer()
{
    ClearTimer(TIMER_CHECK_STATUS);

    if (!SetLoopTimer(TIMER_CHECK_STATUS, g_ulCheckPeriod*1000))
    {
        return FALSE;
    }

    return TRUE;
}

VOID TaskDBSync::SyncOperLog()
{
    VECTOR<OPER_LOG_INFO_T>     vAddRec;
    VECTOR<OPER_LOG_INFO_T>     vDelRec;

    if (!m_pDao->GetSyncingOperLog(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        OPER_LOG_INFO_T     &stRec = vAddRec[i];

        if (m_pPeerDao->SetOperLog(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverOperLog(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync operlog succ: %u %u", stRec.ulTime, stRec.ulUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync operlog failed: %u %u", stRec.ulTime, stRec.ulUserID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        OPER_LOG_INFO_T     &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverOperLog(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverOperLog(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync operlog succ: %u %u", stRec.ulTime, stRec.ulUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync operlog failed: %u %u", stRec.ulTime, stRec.ulUserID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncGroupCallInfo()
{
    VECTOR<GROUP_CALL_INFO_T>       vAddRec;
    VECTOR<GROUP_CALL_INFO_T>       vDelRec;

    if (!m_pDao->GetSyncingGroupCallInfo(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        GROUP_CALL_INFO_T   &stRec = vAddRec[i];

        if (m_pPeerDao->SetGroupCallInfo(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverGroupCallInfo(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync GroupCallInfo succ: %u %u", stRec.ulTime, stRec.ulDCUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync GroupCallInfo failed: %u %u", stRec.ulTime, stRec.ulDCUserID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        GROUP_CALL_INFO_T       &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverGroupCallInfo(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverGroupCallInfo(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync GroupCallInfo succ: %u %u", stRec.ulTime, stRec.ulDCUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync GroupCallInfo failed: %u %u", stRec.ulTime, stRec.ulDCUserID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncGroupCfg()
{
    GROUP_CFG_T     stRec;
    INT32           iSyncFlag;

    if (!m_pDao->GetGroupCfg(stRec, iSyncFlag) ||
        iSyncFlag == SYNC_NULL)
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    if (m_pPeerDao->SetGroupCfg(stRec, SYNC_NULL) )
    {
        m_pDao->SyncOverGroupCfg();

        TaskLog(LOG_DETAIL, "sync GroupCfg succ");
    }
    else
    {
        TaskLog(LOG_ERROR, "sync GroupCfg failed");
        goto end;
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncP2PCallInfo()
{
    VECTOR<P2P_CALL_INFO_T>     vAddRec;
    VECTOR<P2P_CALL_INFO_T>     vDelRec;

    if (!m_pDao->GetSyncingP2PCallInfo(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        P2P_CALL_INFO_T &stRec = vAddRec[i];

        if (m_pPeerDao->SetP2PCallInfo(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverP2PCallInfo(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync P2PCallInfo succ: %u", stRec.ulSeqID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync P2PCallInfo failed: %u", stRec.ulSeqID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        P2P_CALL_INFO_T     &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverP2PCallInfo(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverP2PCallInfo(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync P2PCallInfo succ: %u", stRec.ulSeqID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync P2PCallInfo failed: %u", stRec.ulSeqID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncIPHTalkInfo()
{
    VECTOR<IPH_TALK_INFO_T>     vAddRec;
    VECTOR<IPH_TALK_INFO_T>     vDelRec;

    if (!m_pDao->GetSyncingIPHTalkInfo(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        IPH_TALK_INFO_T &stRec = vAddRec[i];

        if (m_pPeerDao->SetIPHTalkInfo(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverIPHTalkInfo(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync IPHTalkInfo succ: %u %u %u %u", stRec.ulApplyTime, stRec.ulTrainUnitID, stRec.ulCarriageID, stRec.ulIPHDevID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync IPHTalkInfo failed: %u %u %u %u", stRec.ulApplyTime, stRec.ulTrainUnitID, stRec.ulCarriageID, stRec.ulIPHDevID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        IPH_TALK_INFO_T     &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverIPHTalkInfo(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverIPHTalkInfo(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync IPHTalkInfo succ: %u %u %u %u", stRec.ulApplyTime, stRec.ulTrainUnitID, stRec.ulCarriageID, stRec.ulIPHDevID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync IPHTalkInfo failed: %u %u %u %u", stRec.ulApplyTime, stRec.ulTrainUnitID, stRec.ulCarriageID, stRec.ulIPHDevID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncReqCallInfo()
{
    VECTOR<REQ_CALL_INFO_T>     vAddRec;
    VECTOR<REQ_CALL_INFO_T>     vDelRec;

    if (!m_pDao->GetSyncingReqCallInfo(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        REQ_CALL_INFO_T &stRec = vAddRec[i];

        if (m_pPeerDao->SetReqCallInfo(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverReqCallInfo(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync ReqCallInfo succ: %u %u", stRec.ulReqTime, stRec.ulReqUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync ReqCallInfo failed: %u %u", stRec.ulReqTime, stRec.ulReqUserID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        REQ_CALL_INFO_T     &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverReqCallInfo(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverReqCallInfo(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync ReqCallInfo succ: %u %u", stRec.ulReqTime, stRec.ulReqUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync ReqCallInfo failed: %u %u", stRec.ulReqTime, stRec.ulReqUserID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncRealBrdInfo()
{
    VECTOR<REAL_BRD_INFO_T>     vAddRec;
    VECTOR<REAL_BRD_INFO_T>     vDelRec;

    if (!m_pDao->GetSyncingRealBrdInfo(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        REAL_BRD_INFO_T &stRec = vAddRec[i];

        if (m_pPeerDao->SetRealBrdInfo(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverRealBrdInfo(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync RealBrdInfo succ: %u %u", stRec.ulStartTime, stRec.ulDCUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync RealBrdInfo failed: %u %u", stRec.ulStartTime, stRec.ulDCUserID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        REAL_BRD_INFO_T     &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverRealBrdInfo(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverRealBrdInfo(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync RealBrdInfo succ: %u %u", stRec.ulStartTime, stRec.ulDCUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync RealBrdInfo failed: %u %u", stRec.ulStartTime, stRec.ulDCUserID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncRecBrdCfg()
{
    VECTOR<REC_BRD_CFG_T>      vAddRec;
    VECTOR<REC_BRD_CFG_T>      vDelRec;

    if (!m_pDao->GetSyncingRecBrdCfg(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        REC_BRD_CFG_T  &stRec = vAddRec[i];

        if (m_pPeerDao->SetRecBrdCfg(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverRecBrdCfg(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync RecBrd succ: %u", stRec.ulRecID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync RecBrd failed: %u", stRec.ulRecID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        REC_BRD_CFG_T      &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverRecBrdCfg(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverRecBrdCfg(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync RecBrd succ: %u", stRec.ulRecID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync RecBrd failed: %u", stRec.ulRecID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncRecBrdInfo()
{
    VECTOR<REC_BRD_INFO_T>      vAddRec;
    VECTOR<REC_BRD_INFO_T>      vDelRec;

    if (!m_pDao->GetSyncingRecBrdInfo(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        REC_BRD_INFO_T  &stRec = vAddRec[i];

        if (m_pPeerDao->SetRecBrdInfo(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverRecBrdInfo(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync RecBrdInfo succ: %u", stRec.ulBrdSeqID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync RecBrdInfo failed: %u", stRec.ulBrdSeqID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        REC_BRD_INFO_T      &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverRecBrdInfo(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverRecBrdInfo(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync RecBrdInfo succ: %u", stRec.ulBrdSeqID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync RecBrdInfo failed: %u", stRec.ulBrdSeqID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncRecBrdReply()
{
    VECTOR<REC_BRD_REPLY_T>     vAddRec;
    VECTOR<REC_BRD_REPLY_T>     vDelRec;

    if (!m_pDao->GetSyncingRecBrdReply(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        REC_BRD_REPLY_T &stRec = vAddRec[i];

        if (m_pPeerDao->SetRecBrdReply(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverRecBrdReply(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync RecBrdReply succ: %u %u", stRec.ulBrdSeqID, stRec.ulTrainUnitID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync RecBrdReply failed: %u %u", stRec.ulBrdSeqID, stRec.ulTrainUnitID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        REC_BRD_REPLY_T     &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverRecBrdReply(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverRecBrdReply(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync RecBrdReply succ: %u %u", stRec.ulBrdSeqID, stRec.ulTrainUnitID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync RecBrdReply failed: %u %u", stRec.ulBrdSeqID, stRec.ulTrainUnitID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncSDSIndex()
{
    SDS_INDEX_T     stRec;

    if (!m_pDao->GetSyncingSDSIndex(stRec))
    {
        return;
    }

    if (m_pPeerDao->SetSDSIndex(stRec) )
    {
        m_pDao->SetSDSIndex(stRec);

        TaskLog(LOG_DETAIL, "sync SDSIndex succ");
    }
    else
    {
        TaskLog(LOG_ERROR, "sync SDSIndex failed");
    }
}

VOID TaskDBSync::SyncSDSInfo()
{
    VECTOR<SDS_INFO_T>      vAddRec;
    VECTOR<SDS_INFO_T>      vDelRec;

    if (!m_pDao->GetSyncingSDSInfo(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        SDS_INFO_T  &stRec = vAddRec[i];

        if (m_pPeerDao->SetSDSInfo(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverSDSInfo(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync SDSInfo succ: %u", stRec.ulSDSID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync SDSInfo failed: %u", stRec.ulSDSID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        SDS_INFO_T      &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverSDSInfo(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverSDSInfo(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync SDSInfo succ: %u", stRec.ulSDSID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync SDSInfo failed: %u", stRec.ulSDSID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncSDSReply()
{
    VECTOR<SDS_REPLY_T>     vAddRec;
    VECTOR<SDS_REPLY_T>     vDelRec;

    if (!m_pDao->GetSyncingSDSReply(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        SDS_REPLY_T &stRec = vAddRec[i];

        if (m_pPeerDao->SetSDSReply(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverSDSReply(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync SDSReply succ: %u", stRec.ulSDSID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync SDSReply failed: %u", stRec.ulSDSID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        SDS_REPLY_T     &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverSDSReply(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverSDSReply(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync SDSReply succ: %u %u", stRec.ulSDSID, stRec.ulUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync SDSReply failed: %u %u", stRec.ulSDSID, stRec.ulUserID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncSubGroup()
{
    VECTOR<SUB_GROUP_T>     vAddRec;
    VECTOR<SUB_GROUP_T>     vDelRec;

    if (!m_pDao->GetSyncingSubGroup(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        SUB_GROUP_T &stRec = vAddRec[i];

        if (m_pPeerDao->SetSubGroup(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverSubGroup(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync SubGroup succ: %u " FORMAT_I64, stRec.ulDCUserID, stRec.i64GroupID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync SubGroup failed: %u " FORMAT_I64, stRec.ulDCUserID, stRec.i64GroupID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        SUB_GROUP_T     &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverSubGroup(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverSubGroup(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync SubGroup succ: %u " FORMAT_I64, stRec.ulDCUserID, stRec.i64GroupID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync SubGroup failed: %u " FORMAT_I64, stRec.ulDCUserID, stRec.i64GroupID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncSubVideoGroup()
{
    VECTOR<SUB_GROUP_T>     vAddRec;
    VECTOR<SUB_GROUP_T>     vDelRec;

    if (!m_pDao->GetSyncingSubVideoGroup(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        SUB_GROUP_T &stRec = vAddRec[i];

        if (m_pPeerDao->SetSubVideoGroup(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverSubVideoGroup(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync SubVideoGroup succ: %u " FORMAT_I64, stRec.ulDCUserID, stRec.i64GroupID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync SubVideoGroup failed: %u " FORMAT_I64, stRec.ulDCUserID, stRec.i64GroupID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        SUB_GROUP_T     &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverSubVideoGroup(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverSubVideoGroup(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync SubVideoGroup succ: %u " FORMAT_I64, stRec.ulDCUserID, stRec.i64GroupID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync SubVideoGroup failed: %u " FORMAT_I64, stRec.ulDCUserID, stRec.i64GroupID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncSubUser()
{
    VECTOR<SUB_USER_T>      vAddRec;
    VECTOR<SUB_USER_T>      vDelRec;

    if (!m_pDao->GetSyncingSubUser(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        SUB_USER_T  &stRec = vAddRec[i];

        if (m_pPeerDao->SetSubUser(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverSubUser(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync SubUser succ: %u %u", stRec.ulDCUserID, stRec.ulUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync SubUser failed: %u %u", stRec.ulDCUserID, stRec.ulUserID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        SUB_USER_T      &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverSubUser(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverSubUser(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync SubUser succ: %u %u", stRec.ulDCUserID, stRec.ulUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync SubUser failed: %u %u", stRec.ulDCUserID, stRec.ulUserID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncSubVideoUser()
{
    VECTOR<SUB_USER_T>      vAddRec;
    VECTOR<SUB_USER_T>      vDelRec;

    if (!m_pDao->GetSyncingSubVideoUser(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        SUB_USER_T  &stRec = vAddRec[i];

        if (m_pPeerDao->SetSubVideoUser(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverSubVideoUser(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync SubVideoUser succ: %u %u", stRec.ulDCUserID, stRec.ulUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync SubVideoUser failed: %u %u", stRec.ulDCUserID, stRec.ulUserID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        SUB_USER_T      &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverSubVideoUser(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverSubVideoUser(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync SubVideoUser succ: %u %u", stRec.ulDCUserID, stRec.ulUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync SubVideoUser failed: %u %u", stRec.ulDCUserID, stRec.ulUserID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncDCOperator()
{
    VECTOR<DC_OPERATOR_INFO_T>      vAddRec;
    VECTOR<DC_OPERATOR_INFO_T>      vDelRec;

    if (!m_pDao->GetSyncingDCOperator(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        DC_OPERATOR_INFO_T  &stRec = vAddRec[i];

        if (m_pPeerDao->SetDCOperator(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverDCOperator(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync DCOperator succ: %s", stRec.acOperatorID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync DCOperator failed: %s", stRec.acOperatorID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        DC_OPERATOR_INFO_T      &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverDCOperator(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverDCOperator(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync DCOperator succ: %s", stRec.acOperatorID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync DCOperator failed: %s", stRec.acOperatorID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncDCUser()
{
    VECTOR<DC_USER_INFO_T>      vAddRec;
    VECTOR<DC_USER_INFO_T>      vDelRec;

    if (!m_pDao->GetSyncingDCUser(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        DC_USER_INFO_T  &stRec = vAddRec[i];

        if (m_pPeerDao->SetDCUser(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverDCUser(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync DCUser succ: %u", stRec.ulUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync DCUser failed: %u", stRec.ulUserID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        DC_USER_INFO_T      &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverDCUser(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverDCUser(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync DCUser succ: %u", stRec.ulUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync DCUser failed: %u", stRec.ulUserID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncDepot()
{
    VECTOR<DEPOT_INFO_T>      vAddRec;
    VECTOR<DEPOT_INFO_T>      vDelRec;

    if (!m_pDao->GetSyncingDepotCfg(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        DEPOT_INFO_T  &stRec = vAddRec[i];

        if (m_pPeerDao->SetDepotCfg(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverDepotCfg(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync Depot succ: %u", stRec.ulDepotID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync Depot failed: %u", stRec.ulDepotID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        DEPOT_INFO_T      &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverDepotCfg(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverDepotCfg(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync Depot succ: %u", stRec.ulDepotID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync Depot failed: %u", stRec.ulDepotID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncCellInfo()
{
    VECTOR<CELL_INFO_T>     vAddRec;
    VECTOR<CELL_INFO_T>     vDelRec;

    if (!m_pDao->GetSyncingCellInfo(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        CELL_INFO_T &stRec = vAddRec[i];

        if (m_pPeerDao->SetCellInfo(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverCellInfo(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync CellInfo succ: %u %u", stRec.ulTime, stRec.ulTrainUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync CellInfo failed: %u %u", stRec.ulTime, stRec.ulTrainUserID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        CELL_INFO_T     &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverCellInfo(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverCellInfo(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync CellInfo succ: %u %u", stRec.ulTime, stRec.ulTrainUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync CellInfo failed: %u %u", stRec.ulTime, stRec.ulTrainUserID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncGroupInfo()
{
    VECTOR<GROUP_INFO_T>        vAddRec;
    VECTOR<GROUP_INFO_T>        vDelRec;

    if (!m_pDao->GetSyncingGroupInfo(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        GROUP_INFO_T    &stRec = vAddRec[i];

        if (m_pPeerDao->SetGroupInfo(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverGroupInfo(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync GroupInfo succ: " FMT_I64, stRec.i64GroupID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync GroupInfo failed: " FMT_I64, stRec.i64GroupID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        GROUP_INFO_T        &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverGroupInfo(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverGroupInfo(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync GroupInfo succ: " FMT_I64, stRec.i64GroupID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync GroupInfo failed: " FMT_I64, stRec.i64GroupID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncTrain()
{
    VECTOR<TRAIN_T>     vAddRec;
    VECTOR<TRAIN_T>     vDelRec;

    if (!m_pDao->GetSyncingTrainInfo(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        TRAIN_T &stRec = vAddRec[i];

        if (m_pPeerDao->SetTrainInfo(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverTrainInfo(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync Train succ: %u", stRec.ulTrainUnitID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync Train failed: %u", stRec.ulTrainUnitID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        TRAIN_T     &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverTrainInfo(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverTrainInfo(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync Train succ: %u", stRec.ulTrainUnitID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync Train failed: %u", stRec.ulTrainUnitID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncSDSTemplate()
{
    VECTOR<SDS_TEMPLATE_T>        vAddRec;
    VECTOR<SDS_TEMPLATE_T>        vDelRec;

    if (!m_pDao->GetSyncingSDSTemplate(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        SDS_TEMPLATE_T    &stRec = vAddRec[i];

        if (m_pPeerDao->SetSDSTemplate(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverSDSTemplate(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync SDSTemplate succ: %u", stRec.ulTemplateID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync SDSTemplate failed: %u", stRec.ulTemplateID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        SDS_TEMPLATE_T        &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverSDSTemplate(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverSDSTemplate(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync SDSTemplate succ: %u", stRec.ulTemplateID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync SDSTemplate failed: %u", stRec.ulTemplateID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncDestInfo()
{
    VECTOR<DEST_INFO_T>      vAddRec;
    VECTOR<DEST_INFO_T>      vDelRec;

    if (!m_pDao->GetSyncingDestInfo(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        DEST_INFO_T  &stRec = vAddRec[i];

        if (m_pPeerDao->SetDestInfo(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverDestInfo(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync dest info succ: %u", stRec.ulStationID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync dest info failed: %u", stRec.ulStationID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        DEST_INFO_T      &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverDestInfo(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverDestInfo(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync dest info succ: %u", stRec.ulStationID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync dest info failed: %u", stRec.ulStationID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncStation()
{
    VECTOR<STATION_INFO_T>      vAddRec;
    VECTOR<STATION_INFO_T>      vDelRec;

    if (!m_pDao->GetSyncingStation(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        STATION_INFO_T  &stRec = vAddRec[i];

        if (m_pPeerDao->SetStation(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverStation(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync Station succ: %u", stRec.ulStationID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync Station failed: %u", stRec.ulStationID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        STATION_INFO_T      &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverStation(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverStation(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync Station succ: %u", stRec.ulStationID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync Station failed: %u", stRec.ulStationID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncSection()
{
    VECTOR<SECTION_INFO_T>      vAddRec;
    VECTOR<SECTION_INFO_T>      vDelRec;

    if (!m_pDao->GetSyncingSection(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        SECTION_INFO_T  &stRec = vAddRec[i];

        if (m_pPeerDao->SetSection(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverSection(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync Section succ: %u", stRec.ulSectionID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync Section failed: %u", stRec.ulSectionID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        SECTION_INFO_T      &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverSection(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverSection(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync Section succ: %u", stRec.ulSectionID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync Section failed: %u", stRec.ulSectionID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncStationUE()
{
    VECTOR<STATION_UE_INFO_T>       vAddRec;
    VECTOR<STATION_UE_INFO_T>       vDelRec;

    if (!m_pDao->GetSyncingStationUE(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        STATION_UE_INFO_T   &stRec = vAddRec[i];

        if (m_pPeerDao->SetStationUE(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverStationUE(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync StationUE succ: %u", stRec.ulUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync StationUE failed: %u", stRec.ulUserID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        STATION_UE_INFO_T       &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverStationUE(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverStationUE(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync StationUE succ: %u", stRec.ulUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync StationUE failed: %u", stRec.ulUserID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncTrainUE()
{
    VECTOR<TRAIN_UE_INFO_T>     vAddRec;
    VECTOR<TRAIN_UE_INFO_T>     vDelRec;

    if (!m_pDao->GetSyncingTrainUE(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        TRAIN_UE_INFO_T &stRec = vAddRec[i];

        if (m_pPeerDao->SetTrainUE(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverTrainUE(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync TrainUE succ: %u", stRec.ulUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync TrainUE failed: %u", stRec.ulUserID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        TRAIN_UE_INFO_T     &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverTrainUE(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverTrainUE(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync TrainUE succ: %u", stRec.ulUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync TrainUE failed: %u", stRec.ulUserID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncPortalUE()
{
    VECTOR<PORTAL_UE_INFO_T>        vAddRec;
    VECTOR<PORTAL_UE_INFO_T>        vDelRec;

    if (!m_pDao->GetSyncingPortalUE(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        PORTAL_UE_INFO_T    &stRec = vAddRec[i];

        if (m_pPeerDao->SetPortalUE(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverPortalUE(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync PortalUE succ: %u", stRec.ulUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync PortalUE failed: %u", stRec.ulUserID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        PORTAL_UE_INFO_T        &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverPortalUE(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverPortalUE(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync PortalUE succ: %u", stRec.ulUserID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync PortalUE failed: %u", stRec.ulUserID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncPredefinedSMS()
{
    VECTOR<PREDEFINED_SMS_INFO_T>        vAddRec;
    VECTOR<PREDEFINED_SMS_INFO_T>        vDelRec;

    if (!m_pDao->GetSyncingPredefinedSMS(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        PREDEFINED_SMS_INFO_T    &stRec = vAddRec[i];

        if (m_pPeerDao->SetPredefinedSMS(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverPredefinedSMS(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync PredefinedSMS succ: %u", stRec.ulSMSID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync PredefinedSMS failed: %u", stRec.ulSMSID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        PREDEFINED_SMS_INFO_T        &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverPredefinedSMS(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverPredefinedSMS(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync PredefinedSMS succ: %u", stRec.ulSMSID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync PredefinedSMS failed: %u", stRec.ulSMSID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncBrdZoneInfo()
{
    VECTOR<BRD_ZONE_INFO_T>     vAddRec;
    VECTOR<BRD_ZONE_INFO_T>     vDelRec;

    if (!m_pDao->GetSyncingBrdZoneInfo(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        BRD_ZONE_INFO_T &stRec = vAddRec[i];

        if (m_pPeerDao->SetBrdZoneInfo(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverBrdZoneInfo(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync BrdZoneInfo succ: %u %u", stRec.ulStationID, stRec.ulZoneID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync BrdZoneInfo failed: %u %u", stRec.ulStationID, stRec.ulZoneID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        BRD_ZONE_INFO_T     &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverBrdZoneInfo(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverBrdZoneInfo(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync BrdZoneInfo succ: %u %u", stRec.ulStationID, stRec.ulZoneID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync BrdZoneInfo failed: %u %u", stRec.ulStationID, stRec.ulZoneID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncAutoJoinGroup()
{
    VECTOR<SUB_GROUP_T>     vAddRec;
    VECTOR<SUB_GROUP_T>     vDelRec;

    if (!m_pDao->GetSyncingAutoJoinGroup(g_ulSyncRecNum, vAddRec, vDelRec) ||
        (vAddRec.size() == 0 && vDelRec.size() == 0) )
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        SUB_GROUP_T &stRec = vAddRec[i];

        if (m_pPeerDao->SetAutoJoinGroup(stRec, SYNC_NULL) )
        {
            m_pDao->SyncOverAutoJoinGroup(stRec, SYNC_ADD);

            TaskLog(LOG_DETAIL, "sync AutoJoinGroup succ: %u " FORMAT_I64, stRec.ulDCUserID, stRec.i64GroupID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync AutoJoinGroup failed: %u " FORMAT_I64, stRec.ulDCUserID, stRec.i64GroupID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        SUB_GROUP_T     &stRec = vDelRec[i];

        if (m_pPeerDao->SyncOverAutoJoinGroup(stRec, SYNC_DEL) )
        {
            m_pDao->SyncOverAutoJoinGroup(stRec, SYNC_DEL);

            TaskLog(LOG_DETAIL, "sync AutoJoinGroup succ: %u " FORMAT_I64, stRec.ulDCUserID, stRec.i64GroupID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync AutoJoinGroup failed: %u " FORMAT_I64, stRec.ulDCUserID, stRec.i64GroupID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }
}

VOID TaskDBSync::SyncMultiGroup()
{
    VECTOR<MULTI_GROUP_INFO_T> vAddRec;
    VECTOR<MULTI_GROUP_INFO_T> vDelRec;

    if (!m_pDao->GetSyncingMultiGroupInfo(g_ulSyncRecNum, vAddRec, vDelRec))
    {
        return;
    }

    if (vAddRec.size() == 0 && vDelRec.size() == 0)
    {
        return;
    }

    m_pPeerDao->TransBegin();
    m_pDao->TransBegin();

    for (UINT32 i=0; i<vAddRec.size(); i++)
    {
        if (m_pPeerDao->SetMultiGroupInfo(vAddRec[i], SYNC_NULL))
        {
            m_pDao->SyncOverMultiGroupInfo(vAddRec[i], SYNC_ADD);
            TaskLog(LOG_DETAIL, "Sync MultiGroupInfo succ: " FORMAT_I64, vAddRec[i].i64GroupID);
        }
        else
        {
            TaskLog(LOG_ERROR, "Sync MultiGroupInfo failed: " FORMAT_I64, vAddRec[i].i64GroupID);
            goto end;
        }
    }

    for (UINT32 i=0; i<vDelRec.size(); i++)
    {
        if (m_pPeerDao->SyncOverMultiGroupInfo(vDelRec[i], SYNC_DEL))
        {
            m_pDao->SyncOverMultiGroupInfo(vDelRec[i], SYNC_DEL);
            TaskLog(LOG_DETAIL, "sync MultiGroupInfo succ: " FORMAT_I64, vDelRec[i].i64GroupID);
        }
        else
        {
            TaskLog(LOG_ERROR, "sync MultiGroupInfo failed: " FORMAT_I64, vDelRec[i].i64GroupID);
            goto end;
        }
    }

end:
    if (m_pPeerDao->TransCommit())
    {
        m_pDao->TransCommit();
    }
    else
    {
        m_pDao->TransRollback();
    }

    return;
}

VOID TaskDBSync::BackupTable()
{
    CHAR    acCmd[1024];
    CHAR    acBackupDir[256];
    CHAR    acFile[256];
    CHAR    acTime[256];
    CHAR    *szFilePrefix = (CHAR*)"db_";

    TaskLog(LOG_FATAL, "start backup tables");

    sprintf(acBackupDir, "%s/bak", gos_get_root_path());
    if (!gos_file_exist(acBackupDir))
    {
        gos_create_dir(acBackupDir);
    }

    gos_get_text_time_ex(NULL, "%04u%02u%02u", acTime);
    sprintf(acFile, "%s/%s%s", acBackupDir, szFilePrefix, acTime);

#ifdef _OSWIN32_
    sprintf(acCmd, "%s/backup_db.bat %s", gos_get_root_path(), acFile);
#else
    sprintf(acCmd, "%s/backup_db.sh %s", gos_get_root_path(), acFile);
#endif

    gos_system(acCmd);

    // 清理过期文件
    RemoveFile(acBackupDir, szFilePrefix, GetLocalCfg().ulBackupFileNum);
}

VOID TaskDBSync::ClearTable()
{
    UINT32  ulCurrTime = gos_get_current_time();
    UINT32  ulDeadTime = gos_get_current_time() - GetLocalCfg().ulResvDataDays*86400;
    static UINT32   ulLastClearTime = 0;
    UINT32          ulUptime = gos_get_uptime();
    GOS_DATETIME_T  stTime;

    gos_get_localtime(&ulCurrTime, &stTime);
    if (stTime.ucHour != GetLocalCfg().ulClearDataHour)
    {
        return;
    }

    // 2小时内不重复清理
    if (ulLastClearTime > 0 &&
        (ulLastClearTime+2*86400) > ulUptime)
    {
        return;
    }

    ulLastClearTime = gos_get_uptime();

    BackupTable();

    TaskLog(LOG_FATAL, "start clear tables");

    m_pDao->ClearCellInfo(ulDeadTime);
    m_pDao->ClearGroupCallInfo(ulDeadTime);
    m_pDao->ClearIPHTalkInfo(ulDeadTime);
    m_pDao->ClearOperLog(ulDeadTime);
    m_pDao->ClearP2PCallInfo(ulDeadTime);
    m_pDao->ClearRealBrdInfo(ulDeadTime);
    m_pDao->ClearRecBrdInfo(ulDeadTime);
    m_pDao->ClearReqCallInfo(ulDeadTime);
    m_pDao->ClearSDSInfo(ulDeadTime);

    TaskLog(LOG_FATAL, "clear tables over");
}

VOID TaskDBSync::OnTimerCheck(VOID *pvMsg, UINT32 ulMsgLen)
{
    if (!m_pDao->CheckConn())
    {
        TaskLog(LOG_FATAL, "check db failed");
        return;
    }

    ClearTable();

    if (!m_pPeerDao->CheckConn())
    {
        TaskLog(LOG_FATAL, "check peer db failed");
        return;
    }

//  SyncAppCfg();
    SyncAutoJoinGroup();
    SyncBrdZoneInfo();
    SyncCellInfo();
    SyncDCOperator();
    SyncDCUser();
    SyncDepot();
    SyncDestInfo();
    SyncGroupCallInfo();
    SyncGroupCfg();
    SyncGroupInfo();
    SyncIPHTalkInfo();
    SyncMultiGroup();
    SyncOperLog();
    SyncP2PCallInfo();
    SyncPortalUE();
    SyncPredefinedSMS();
    SyncRealBrdInfo();
    SyncRecBrdCfg();
    SyncRecBrdInfo();
    SyncRecBrdReply();
    SyncReqCallInfo();
    SyncSDSIndex();
    SyncSDSInfo();
    SyncSDSReply();
    SyncSDSTemplate();
    SyncStation();
    SyncSection();
    SyncStationUE();
    SyncSubGroup();
    SyncSubUser();
    SyncSubVideoGroup();
    SyncSubVideoUser();
    SyncTrain();
    SyncTrainUE();
}

VOID TaskDBSync::TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
{
    UINT32  ulTaskStatus = GetTaskStatus();

    switch(ulTaskStatus)
    {
    case TASK_STATUS_IDLE:
        switch(usMsgID)
        {
        case EV_TASK_INIT_REQ:
            SendRsp();
            SetTaskStatus(TASK_STATUS_INIT);
            SetTimer(TIMER_INIT, 0);
            break;

        default:
            break;
        }

        break;

    case TASK_STATUS_INIT:
        switch(usMsgID)
        {
        case EV_INIT_IND:
            if (!Init())
            {
                SetTimer(TIMER_INIT, 1000);
                TaskLog(LOG_FATAL, "Init failed!");
            }

            break;

        default:
            break;
        }

        break;

    case TASK_STATUS_WORK:
        switch(usMsgID)
        {
        case EV_CHECK_STATUS_TIMER:
            OnTimerCheck(pvMsg, ulMsgLen);
            break;

        default:
            TaskLog(LOG_ERROR, "unknown msg %u", usMsgID);
            break;
        }

        break;

    default:
        break;
    }
}
