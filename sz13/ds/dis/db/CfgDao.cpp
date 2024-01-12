#include "DISPublic.h"
#include "pdb_app.h"

#include "CfgDao.h"

extern PDBEnvBase   *g_PDBEnv;

VOID FormatSyncFlag(INT32 &iSyncFlag)
{
    if (iSyncFlag > 0)
    {
        iSyncFlag = gos_get_current_time();
    }
    else if (iSyncFlag < 0)
    {
        iSyncFlag = 0 - gos_get_current_time();
    }
}

CfgDao* GetCfgDao()
{
    return GetCfgDao(g_PDBEnv);
}

CfgDao* GetCfgDao(PDBEnvBase *pPDBEnv)
{
    CfgDao  *pInstance = NULL;

    if (!pInstance)
    {
        pInstance = new CfgDao();

        if (!pInstance->Init(pPDBEnv))
        {
            delete pInstance;
            pInstance = NULL;
        }
    }

    return pInstance;
}

BOOL CfgDao::Init(PDBEnvBase *pPDBEnv)
{
    if (!pPDBEnv)
    {
        return FALSE;
    }

    m_pPDBApp = new PDBApp(pPDBEnv);
    if (!m_pPDBApp)
    {
        return FALSE;
    }

    if (!m_pPDBApp->AllocConn())
    {
        DaoLog(LOG_ERROR, "alloc Conn fail");

        return FALSE;
    }

    return TRUE;
}

CfgDao::CfgDao()
{
    m_pPDBApp = NULL;
}

CfgDao::~CfgDao()
{
    if (m_pPDBApp)
    {
        delete m_pPDBApp;
    }
}

BOOL CfgDao::TransBegin()
{
    return m_pPDBApp->TransBegin();
}

BOOL CfgDao::TransRollback()
{
    return m_pPDBApp->TransRollback();
}

BOOL CfgDao::TransCommit()
{
    if (m_pPDBApp->TransCommit())
    {
        return TRUE;
    }

    m_pPDBApp->TransRollback();
    return FALSE;
}

BOOL CfgDao::CheckConn()
{
    return m_pPDBApp->CheckConnStat();
}

BOOL CfgDao::Update(const CHAR *szSQL)
{
    return m_pPDBApp->Update(szSQL) >= 0;
}

BOOL CfgDao::IsRecordExist(const CHAR *szTable, const CHAR *szKey, UINT32 ulKey)
{
    INT32                   iRow = 0;
    UINT32                  ulNum = 0;
    PDBRsltParam            Params;
    PDBRET                  enRet;
    CHAR                    acSQL[512];

    Params.Bind(&ulNum);
    sprintf(acSQL, "SELECT COUNT(*) FROM %s WHERE %s=%u", szTable, szKey, ulKey);
    iRow = m_pPDBApp->Query(acSQL, Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query %s failed", szTable);
        return FALSE;
    }

    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
    }

    m_pPDBApp->CloseQuery();

    return ulNum>0;
}

BOOL CfgDao::GetGroupCfg(GROUP_CFG_T &stCfg, INT32 &iSyncFlag)
{
    INT32                   iRow = 0;
    PDBRET                  enRet;
    PDBRsltParam            Params;

    Params.Bind(&iSyncFlag);
    Params.Bind(&stCfg.i64UpLineTrainGroupID);
    Params.Bind(&stCfg.i64DownLineTrainGroupID);
    Params.Bind(&stCfg.i64MainLineTrainGroupID);
    Params.Bind(&stCfg.i64UpLineTrainBrdGroupID);
    Params.Bind(&stCfg.i64DownLineTrainBrdGroupID);
    Params.Bind(&stCfg.i64MainLineTrainBrdGroupID);
    Params.Bind(&stCfg.i64AllStationGroupID);
    Params.Bind(&stCfg.i64StartTempDynamicGroupID);
    Params.Bind(&stCfg.i64EndTempDynamicGroupID);
    Params.Bind(&stCfg.i64DriverGroupID);
    Params.Bind(&stCfg.i64DepotDriverGroupID);

    iRow = m_pPDBApp->Query("SELECT SyncFlag, UpLineTrainGroupID, DownLineTrainGroupID, MainLineTrainGroupID, UpLineTrainBrdGroupID, DownLineTrainBrdGroupID, MainLineTrainBrdGroupID, "
                            "AllStationGroupID, StartTempDynamicGroupID, EndTempDynamicGroupID, DriverGroupID, DepotDriverGroupID FROM GroupCfg", Params);
    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query GroupCfg failed");
        return FALSE;
    }

    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        break;
    }

    m_pPDBApp->CloseQuery();

    return (PDB_ERROR != enRet);
}

BOOL CfgDao::SetGroupCfg(GROUP_CFG_T &stCfg, INT32 iSyncFlag)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    FormatSyncFlag(iSyncFlag);

    Params.Bind(&iSyncFlag);
    Params.Bind(&stCfg.i64UpLineTrainGroupID);
    Params.Bind(&stCfg.i64DownLineTrainGroupID);
    Params.Bind(&stCfg.i64MainLineTrainGroupID);
    Params.Bind(&stCfg.i64UpLineTrainBrdGroupID);
    Params.Bind(&stCfg.i64DownLineTrainBrdGroupID);
    Params.Bind(&stCfg.i64MainLineTrainBrdGroupID);
    Params.Bind(&stCfg.i64AllStationGroupID);
    Params.Bind(&stCfg.i64StartTempDynamicGroupID);
    Params.Bind(&stCfg.i64EndTempDynamicGroupID);
    Params.Bind(&stCfg.i64DriverGroupID);
    Params.Bind(&stCfg.i64DepotDriverGroupID);

    iAffectedRow = m_pPDBApp->Update((CHAR*)"UPDATE GroupCfg SET SyncFlag=?, UpLineTrainGroupID=?, DownLineTrainGroupID=?, MainLineTrainGroupID=?, UpLineTrainBrdGroupID=?, DownLineTrainBrdGroupID=?, "
        "MainLineTrainBrdGroupID=?, AllStationGroupID=?, StartTempDynamicGroupID=?, EndTempDynamicGroupID=?, DriverGroupID=?, DepotDriverGroupID=?", Params);
    if (iAffectedRow < 0)
    {
        DaoLog(LOG_ERROR, "update GroupCfg failed");
        return FALSE;
    }

    if (iAffectedRow > 0)
    {
        return TRUE;
    }

    iAffectedRow = m_pPDBApp->Update((CHAR*)"INSERT INTO GroupCfg(SyncFlag, UpLineTrainGroupID, DownLineTrainGroupID, MainLineTrainGroupID, UpLineTrainBrdGroupID, DownLineTrainBrdGroupID, "
        "MainLineTrainBrdGroupID, AllStationGroupID, StartTempDynamicGroupID, EndTempDynamicGroupID, DriverGroupID, DepotDriverGroupID) VALUES(?,?,?,?,?,?,?,?,?,?,?,?)", Params);
    if (iAffectedRow <= 0)
    {
        DaoLog(LOG_ERROR, "update GroupCfg failed");
        return FALSE;
    }

    return TRUE;
}

BOOL CfgDao::SyncOverGroupCfg()
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    iAffectedRow = m_pPDBApp->Update("UPDATE GroupCfg SET SyncFlag=0");
    if (iAffectedRow <= 0)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CfgDao::GetAppCfg(APP_BASIC_CFG_T *pstCfg)
{
    INT32                   iRow = 0;
    PDBRET                  enRet;
    PDBRsltParam            Params;

    Params.Bind(&pstCfg->bPTTAllow);
    Params.Bind(&pstCfg->ulBroadcastTimeout);
    Params.Bind(&pstCfg->ulBroadcastVolume);
    Params.Bind(&pstCfg->ulHangoffTime);
    Params.Bind(pstCfg->acNTPServerAddr, sizeof(pstCfg->acNTPServerAddr));
    Params.Bind(&pstCfg->ulNTPServerPort);
    Params.Bind(&pstCfg->ulNTPSyncPeriod);
    Params.Bind(&pstCfg->ulFXAutoRebootPeriod);
    Params.Bind(&pstCfg->ulFXAutoRebootTime);

    iRow = m_pPDBApp->Query("SELECT PTTAllow, BrdTimeout, BrdVolume, HangoffTime, NTPServerAddr, NTPServerPort, NTPSyncPeriod, "
                            "FXAutoRebootPeriod, FXAutoRebootTime FROM AppCfg", Params);
    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query AppCfg failed");
        return FALSE;
    }

    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        break;
    }

    m_pPDBApp->CloseQuery();

    return (PDB_ERROR != enRet);
}

BOOL CfgDao::SaveAppCfg(APP_BASIC_CFG_T *pstCfg)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.Bind(&pstCfg->bPTTAllow);
    Params.Bind(&pstCfg->ulBroadcastTimeout);
    Params.Bind(&pstCfg->ulBroadcastVolume);
    Params.Bind(&pstCfg->ulHangoffTime);
    Params.Bind(pstCfg->acNTPServerAddr, sizeof(pstCfg->acNTPServerAddr));
    Params.Bind(&pstCfg->ulNTPServerPort);
    Params.Bind(&pstCfg->ulNTPSyncPeriod);
    Params.Bind(&pstCfg->ulFXAutoRebootPeriod);
    Params.Bind(&pstCfg->ulFXAutoRebootTime);

    iAffectedRow = m_pPDBApp->Update((CHAR*)"UPDATE APPCfg SET PTTAllow=?, BrdTimeout=?, BrdVolume=?, HangoffTime=?, "
        "NTPServerAddr=?, NTPServerPort=?, NTPSyncPeriod=?, FXAutoRebootPeriod=?, FXAutoRebootTime=?", Params);
    if (iAffectedRow <= 0)
    {
        DaoLog(LOG_ERROR, "update APPCfg failed");
        return FALSE;
    }

    return TRUE;
}

BOOL CfgDao::GetSDSTemplate(VECTOR<SDS_TEMPLATE_T> &vInfo)
{
    INT32                   iRow = 0;
    PDBRET                  enRet = PDB_CONTINUE;
    PDBRsltParam            Params;
    SDS_TEMPLATE_T          stInfo = {0};

    vInfo.clear();

    Params.Bind(&stInfo.ulTemplateID);
    Params.Bind(&stInfo.ulSDSType);
    Params.Bind(stInfo.acTitle, sizeof(stInfo.acTitle));
    Params.Bind(stInfo.acSDSInfo, sizeof(stInfo.acSDSInfo));

    iRow = m_pPDBApp->Query("SELECT TemplateID, SDSType, Title, SDSInfo FROM SDSTemplate WHERE SyncFlag>=0 ORDER BY TemplateID", Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query SDSTemplate failed");
        return FALSE;
    }

    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        vInfo.push_back(stInfo);
    }

    m_pPDBApp->CloseQuery();

    return (PDB_ERROR != enRet);
}

BOOL CfgDao::GetMaxSDSTemplateID(UINT32 *pulMaxID)
{
    INT32                   iRow = 0;
    PDBRET                  enRet;
    PDBRsltParam            Params;
    UINT32                  ulCount;

    Params.Bind(pulMaxID);
    Params.Bind(&ulCount);

    iRow = m_pPDBApp->Query("SELECT MAX(TemplateID), COUNT(*) FROM SDSTemplate WHERE SyncFlag>=0", Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query SDSTemplate failed");
        return FALSE;
    }

    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        if (ulCount == 0)
        {
            *pulMaxID = 0;
        }
        break;
    }

    m_pPDBApp->CloseQuery();

    if (PDB_ERROR == enRet)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CfgDao::AddSDSTemplate(SDS_TEMPLATE_T &stInfo)
{
    return SetSDSTemplate(stInfo, SYNC_ADD);
}

BOOL CfgDao::DelSDSTemplate(UINT32 ulTemplateID)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;
    INT32           iSyncFlag = SYNC_DEL;

    FormatSyncFlag(iSyncFlag);

    Params.Bind(&iSyncFlag);
    Params.Bind(&ulTemplateID);

    iAffectedRow = m_pPDBApp->Update("UPDATE SDSTemplate SET SyncFlag=? WHERE TemplateID=?", Params);

    return iAffectedRow >= 0;
}

BOOL CfgDao::SetSDSTemplate(SDS_TEMPLATE_T &stRec)
{
    return SetSDSTemplate(stRec, SYNC_ADD);
}

BOOL CfgDao::SetSDSTemplate(SDS_TEMPLATE_T &stRec, INT32 iSyncFlag)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    FormatSyncFlag(iSyncFlag);

    Params.Bind(&iSyncFlag);
    Params.Bind(&stRec.ulTemplateID);
    Params.Bind(&stRec.ulSDSType);
    Params.Bind(stRec.acTitle, sizeof(stRec.acTitle));
    Params.Bind(stRec.acSDSInfo, sizeof(stRec.acSDSInfo));

    Params.Bind(&iSyncFlag);
    Params.Bind(&stRec.ulSDSType);
    Params.Bind(stRec.acTitle, sizeof(stRec.acTitle));
    Params.Bind(stRec.acSDSInfo, sizeof(stRec.acSDSInfo));

    iAffectedRow = m_pPDBApp->Update("INSERT INTO SDSTemplate(SyncFlag, TemplateID, SDSType, Title, SDSInfo) "
        "VALUES(?,?,?,?,?) ON DUPLICATE KEY UPDATE SyncFlag=?, SDSType=?, Title=?, SDSInfo=?", Params);
    if (iAffectedRow <= 0)
    {
        DaoLog(LOG_ERROR, "set SDSTemplate failed, template ID is %u", stRec.ulTemplateID);
        return FALSE;
    }

    return TRUE;
}

BOOL CfgDao::GetSyncingSDSTemplate(UINT32 ulMaxRecNum, VECTOR<SDS_TEMPLATE_T> &vAddRec, VECTOR<SDS_TEMPLATE_T> &vDelRec)
{
    INT32                   iRow = 0;
    PDBRET                  enRet = PDB_CONTINUE;
    PDBRsltParam            Params;
    SDS_TEMPLATE_T       stRec;
    INT32                   iSyncFlag;
    CHAR                    acSQL[1024];

    vAddRec.clear();
    vDelRec.clear();

    Params.Bind(&iSyncFlag);
    Params.Bind(&stRec.ulTemplateID);
    Params.Bind(&stRec.ulSDSType);
    Params.Bind(stRec.acTitle, sizeof(stRec.acTitle));
    Params.Bind(stRec.acSDSInfo, sizeof(stRec.acSDSInfo));

    sprintf(acSQL, "SELECT SyncFlag, TemplateID, SDSType, Title, SDSInfo FROM SDSTemplate WHERE SyncFlag!=0");

    iRow = m_pPDBApp->Query(acSQL, Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query SDSTemplate failed");
        return FALSE;
    }

    memset(&stRec, 0, sizeof(stRec));
    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        if (iSyncFlag > 0)
        {
            vAddRec.push_back(stRec);
        }
        else
        {
            vDelRec.push_back(stRec);
        }

        if ((vAddRec.size()+vDelRec.size()) >= ulMaxRecNum)
        {
            break;
        }

        memset(&stRec, 0, sizeof(stRec));
    }

    m_pPDBApp->CloseQuery();

    return PDB_ERROR != enRet;
}

BOOL CfgDao::SyncOverSDSTemplate(SDS_TEMPLATE_T &stRec, INT32 iSyncFlag)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.Bind(&stRec.ulTemplateID);

    if (iSyncFlag > 0)
    {
        iAffectedRow = m_pPDBApp->Update("UPDATE SDSTemplate SET SyncFlag=0 WHERE TemplateID=?", Params);
        if (iAffectedRow <= 0)
        {
            return FALSE;
        }
    }
    else if (iSyncFlag < 0)
    {
        iAffectedRow = m_pPDBApp->Update("DELETE FROM SDSTemplate WHERE TemplateID=?", Params);
        if (iAffectedRow < 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CfgDao::GetPredefinedSMS(VECTOR<PREDEFINED_SMS_INFO_T> &vSMS)
{
    INT32                   iRow = 0;
    PDBRET                  enRet = PDB_CONTINUE;
    PDBRsltParam            Params;
    PREDEFINED_SMS_INFO_T   stSMS = {0};

    vSMS.clear();

    Params.Bind(&stSMS.ulSMSID);
    Params.Bind(&stSMS.ulSDSType);
    Params.Bind(&stSMS.ulSMSTemplateType);
    Params.Bind(stSMS.acSMSInfo, sizeof(stSMS.acSMSInfo));

    iRow = m_pPDBApp->Query("SELECT SMSID, SDSType, SMSTemplateType, SMSInfo FROM PredefinedSMS WHERE SyncFlag>=0 ORDER BY SDSType, SMSID", Params);
    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query PredefinedSMS failed");
        return FALSE;
    }

    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        vSMS.push_back(stSMS);
        memset(&stSMS, 0, sizeof(stSMS));
    }

    m_pPDBApp->CloseQuery();

    return PDB_ERROR != enRet;
}

BOOL CfgDao::AddPredefinedSMS(PREDEFINED_SMS_INFO_T &stInfo)
{
    return SetPredefinedSMS(stInfo, SYNC_ADD);
}

BOOL CfgDao::SetPredefinedSMS(PREDEFINED_SMS_INFO_T &stInfo)
{
    return SetPredefinedSMS(stInfo, SYNC_ADD);
}

BOOL CfgDao::SetPredefinedSMS(PREDEFINED_SMS_INFO_T &stRec, INT32 iSyncFlag)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    FormatSyncFlag(iSyncFlag);

    Params.Bind(&iSyncFlag);
    Params.Bind(&stRec.ulSMSID);
    Params.Bind(&stRec.ulSDSType);
    Params.Bind(&stRec.ulSMSTemplateType);
    Params.Bind(stRec.acSMSInfo, sizeof(stRec.acSMSInfo));

    Params.Bind(&iSyncFlag);
    Params.Bind(&stRec.ulSDSType);
    Params.Bind(&stRec.ulSMSTemplateType);
    Params.Bind(stRec.acSMSInfo, sizeof(stRec.acSMSInfo));

    iAffectedRow = m_pPDBApp->Update("INSERT INTO PredefinedSMS(SyncFlag, SMSID, SDSType, SMSTemplateType, SMSInfo) "
        "VALUES(?,?,?,?,?) ON DUPLICATE KEY UPDATE SyncFlag=?, SDSType=?, SMSTemplateType=?, SMSInfo=?", Params);
    if (iAffectedRow <= 0)
    {
        DaoLog(LOG_ERROR, "set PredefinedSMS failed, SMSID is %u", stRec.ulSMSID);
        return FALSE;
    }

    return TRUE;
}

BOOL CfgDao::DelPredefinedSMS(UINT32 ulSMSID)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;
    INT32           iSyncFlag = SYNC_DEL;

    FormatSyncFlag(iSyncFlag);

    Params.Bind(&iSyncFlag);
    Params.Bind(&ulSMSID);

    iAffectedRow = m_pPDBApp->Update("UPDATE PredefinedSMS SET SyncFlag=? WHERE SMSID=?", Params);

    return iAffectedRow >= 0;
}

BOOL CfgDao::GetSyncingPredefinedSMS(UINT32 ulMaxRecNum, VECTOR<PREDEFINED_SMS_INFO_T> &vAddRec, VECTOR<PREDEFINED_SMS_INFO_T> &vDelRec)
{
    INT32                   iRow = 0;
    PDBRET                  enRet = PDB_CONTINUE;
    PDBRsltParam            Params;
    PREDEFINED_SMS_INFO_T   stRec;
    INT32                   iSyncFlag;
    CHAR                    acSQL[1024];

    vAddRec.clear();
    vDelRec.clear();

    Params.Bind(&iSyncFlag);
    Params.Bind(&stRec.ulSMSID);
    Params.Bind(&stRec.ulSDSType);
    Params.Bind(&stRec.ulSMSTemplateType);
    Params.Bind(stRec.acSMSInfo, sizeof(stRec.acSMSInfo));

    sprintf(acSQL, "SELECT SyncFlag, SMSID, SDSType, SMSTemplateType, SMSInfo FROM PredefinedSMS WHERE SyncFlag!=0");

    iRow = m_pPDBApp->Query(acSQL, Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query PredefinedSMS failed");
        return FALSE;
    }

    memset(&stRec, 0, sizeof(stRec));
    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        if (iSyncFlag > 0)
        {
            vAddRec.push_back(stRec);
        }
        else
        {
            vDelRec.push_back(stRec);
        }

        if ((vAddRec.size()+vDelRec.size()) >= ulMaxRecNum)
        {
            break;
        }

        memset(&stRec, 0, sizeof(stRec));
    }

    m_pPDBApp->CloseQuery();

    return PDB_ERROR != enRet;
}

BOOL CfgDao::SyncOverPredefinedSMS(PREDEFINED_SMS_INFO_T &stRec, INT32 iSyncFlag)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.Bind(&stRec.ulSMSID);

    if (iSyncFlag > 0)
    {
        iAffectedRow = m_pPDBApp->Update("UPDATE PredefinedSMS SET SyncFlag=0 WHERE SMSID=?", Params);
        if (iAffectedRow <= 0)
        {
            return FALSE;
        }
    }
    else if (iSyncFlag < 0)
    {
        iAffectedRow = m_pPDBApp->Update("DELETE FROM PredefinedSMS WHERE SMSID=?", Params);
        if (iAffectedRow < 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CfgDao::GetDCDevInfo(VECTOR<DC_DEV_INFO_T> &vInfo)
{
    INT32                   iRow = 0;
    PDBRET                  enRet = PDB_CONTINUE;
    PDBRsltParam            Params;
    DC_DEV_INFO_T           stInfo = {0};

    vInfo.clear();

    Params.Bind(&stInfo.ulDevID);
    Params.Bind(stInfo.acName, sizeof(stInfo.acName));
    Params.Bind(stInfo.acIPAddr, sizeof(stInfo.acIPAddr));
    Params.Bind(stInfo.acMacAddr, sizeof(stInfo.acMacAddr));
    Params.Bind(stInfo.acGateway, sizeof(stInfo.acGateway));

    iRow = m_pPDBApp->Query("SELECT DevID, Name, IPAddr, MACAddr, Gateway FROM DCInfo ORDER BY DevID", Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query DCInfo failed");
        return FALSE;
    }

    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        vInfo.push_back(stInfo);
        memset(&stInfo, 0, sizeof(stInfo));
    }

    m_pPDBApp->CloseQuery();

    return PDB_ERROR != enRet;
}

BOOL CfgDao::GetAutoJoinGroup(UINT32 ulDCUserID, VECTOR<INT64> &vGroupID)
{
    INT32                   iRow = 0;
    PDBRET                  enRet = PDB_CONTINUE;
    PDBRsltParam            Params;
    INT64                   i64GroupID;
    CHAR                    acSQL[256];

    vGroupID.clear();
    Params.Bind(&i64GroupID);

    sprintf(acSQL, "SELECT GroupID FROM AutoJoinGroup WHERE DCUserID=%u AND SyncFlag>=0", ulDCUserID);
    iRow = m_pPDBApp->Query(acSQL, Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query AutoJoinGroup failed");
        return FALSE;
    }

    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        vGroupID.push_back(i64GroupID);
    }

    m_pPDBApp->CloseQuery();

    if (PDB_ERROR != enRet)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL CfgDao::SaveGroupStatus(UINT32 ulDCUserID, INT64 i64GroupID, UINT32 ulGroupStatus)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;
    INT32           iSyncFlag = gos_get_current_time();

    Params.Bind(&iSyncFlag);
    Params.Bind(&ulDCUserID);
    Params.Bind(&i64GroupID);

    if (ulGroupStatus == GROUP_STATUS_SUB ||
        ulGroupStatus == GROUP_STATUS_JOIN )
    {
        if (!IsAutoJoinGroupExist(ulDCUserID, i64GroupID))
        {
            iAffectedRow = m_pPDBApp->Update((CHAR*)"INSERT INTO AutoJoinGroup(SyncFlag, DCUserID, GroupID) VALUES(?,?,?)", Params);
            if (iAffectedRow <= 0)
            {
                DaoLog(LOG_ERROR, "add group(" FORMAT_I64 ") status(%u) failed", i64GroupID, ulGroupStatus);
                return FALSE;
            }
        }
    }
    else if (ulGroupStatus == GROUP_STATUS_UNSUB)
    {
        iSyncFlag = -iSyncFlag;
        iAffectedRow = m_pPDBApp->Update((CHAR*)"UPDATE AutoJoinGroup SET SyncFlag=? WHERE DCUserID=? AND GroupID=?", Params);
    }

    return TRUE;
}

BOOL CfgDao::GetDepotCfg(VECTOR<DEPOT_INFO_T> &vDepot)
{
    INT32                   iRow = 0;
    PDBRET                  enRet = PDB_CONTINUE;
    PDBRsltParam            Params;
    DEPOT_INFO_T            stDepot = {0};

    vDepot.clear();

    stDepot.ulDepotID = DEPOT_ID_MAINLINE;
    stDepot.ulTrainDirection = TRAIN_DIRECTION_UNKNOWN;
    strcpy(stDepot.acDepotName, "正线");
    vDepot.push_back(stDepot);

    Params.Bind(&stDepot.ulDepotID);
    Params.Bind(stDepot.acDepotName, sizeof(stDepot.acDepotName));
    Params.Bind(&stDepot.ulTrainDirection);

    iRow = m_pPDBApp->Query("SELECT DepotID, DepotName, TrainDirection FROM Depot WHERE SyncFlag>=0 ORDER BY DepotID", Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query depot failed");
        return FALSE;
    }

    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        if (stDepot.ulDepotID == DEPOT_ID_MAINLINE)
        {
            DaoLog(LOG_ERROR, "depot id mustn't be %u", DEPOT_ID_MAINLINE);
            m_pPDBApp->CloseQuery();
            return FALSE;
        }

        vDepot.push_back(stDepot);
    }

    m_pPDBApp->CloseQuery();

    if (PDB_ERROR != enRet)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL CfgDao::AddDepotCfg(DEPOT_INFO_T &stInfo)
{
    return SetDepotCfg(stInfo, SYNC_ADD);
}

BOOL CfgDao::DelDepotCfg(UINT32 ulDepotID)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;
    INT32           iSyncFlag = SYNC_DEL;

    FormatSyncFlag(iSyncFlag);

    Params.Bind(&iSyncFlag);
    Params.Bind(&ulDepotID);

    iAffectedRow = m_pPDBApp->Update("UPDATE Depot SET SyncFlag=? WHERE DepotID=?", Params);

    return iAffectedRow >= 0;
}

BOOL CfgDao::SetDepotCfg(DEPOT_INFO_T &stInfo)
{
    return SetDepotCfg(stInfo, SYNC_ADD);
}

BOOL CfgDao::SetDepotCfg(DEPOT_INFO_T &stInfo, INT32 iSyncFlag)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    FormatSyncFlag(iSyncFlag);

    Params.Bind(&iSyncFlag);
    Params.Bind(&stInfo.ulDepotID);
    Params.BindString(stInfo.acDepotName);
    Params.Bind(&stInfo.ulTrainDirection);

    Params.Bind(&iSyncFlag);
    Params.BindString(stInfo.acDepotName);
    Params.Bind(&stInfo.ulTrainDirection);

    iAffectedRow = m_pPDBApp->Update("INSERT INTO Depot(SyncFlag, DepotID, DepotName, TrainDirection) "
        "VALUES(?,?,?,?) ON DUPLICATE KEY UPDATE SyncFlag=?, DepotName=?, TrainDirection=?", Params);

    return iAffectedRow > 0;
}

BOOL CfgDao::GetSyncingDepotCfg(UINT32 ulMaxRecNum, VECTOR<DEPOT_INFO_T> &vAddRec, VECTOR<DEPOT_INFO_T> &vDelRec)
{
    INT32                   iRow = 0;
    PDBRET                  enRet = PDB_CONTINUE;
    PDBRsltParam            Params;
    DEPOT_INFO_T            stRec;
    INT32                   iSyncFlag;
    CHAR                    acSQL[1024];

    vAddRec.clear();
    vDelRec.clear();

    Params.Bind(&iSyncFlag);
    Params.Bind(&stRec.ulDepotID);
    Params.Bind(stRec.acDepotName, sizeof(stRec.acDepotName));
    Params.Bind(&stRec.ulTrainDirection);

    sprintf(acSQL, "SELECT SyncFlag, DepotID, DepotName, TrainDirection FROM Depot WHERE SyncFlag!=0");

    iRow = m_pPDBApp->Query(acSQL, Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query Depot failed");
        return FALSE;
    }

    memset(&stRec, 0, sizeof(stRec));
    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        if (iSyncFlag > 0)
        {
            vAddRec.push_back(stRec);
        }
        else
        {
            vDelRec.push_back(stRec);
        }

        if ((vAddRec.size()+vDelRec.size()) >= ulMaxRecNum)
        {
            break;
        }

        memset(&stRec, 0, sizeof(stRec));
    }

    m_pPDBApp->CloseQuery();

    return PDB_ERROR != enRet;
}

BOOL CfgDao::SyncOverDepotCfg(DEPOT_INFO_T &stRec, INT32 iSyncFlag)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.Bind(&stRec.ulDepotID);

    if (iSyncFlag > 0)
    {
        iAffectedRow = m_pPDBApp->Update("UPDATE Depot SET SyncFlag=0 WHERE DepotID=?", Params);
        if (iAffectedRow <= 0)
        {
            return FALSE;
        }
    }
    else if (iSyncFlag < 0)
    {
        iAffectedRow = m_pPDBApp->Update("DELETE FROM Depot WHERE DepotID=?", Params);
        if (iAffectedRow < 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CfgDao::IsAutoJoinGroupExist(UINT32 ulDCUserID, INT64 i64GroupID)
{
    INT32                   iRow = 0;
    PDBRET                  enRet = PDB_CONTINUE;
    UINT32                  ulNum = 0;
    PDBRsltParam            Params;
    CHAR                    acSQL[512];

    Params.Bind(&ulNum);
    sprintf(acSQL, "SELECT COUNT(*) FROM AutoJoinGroup WHERE DCUserID=%u AND GroupID=" FORMAT_I64, ulDCUserID, i64GroupID);
    iRow = m_pPDBApp->Query(acSQL, Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query SubGroup failed");
        return FALSE;
    }

    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
    }

    m_pPDBApp->CloseQuery();

    return ulNum>0;
}

BOOL ConvertStationList(CHAR *szStationList, UINT32 *pulStationList, UINT32 ulMaxStationNum, UINT32 &ulStationNum)
{
    CHAR    *szTmp;

    ulStationNum = 0;
    if (*szStationList == '\0')
    {
        return TRUE;
    }

    strcat(szStationList, ",");
    while(*szStationList)
    {
        szTmp = strchr(szStationList, ',');
        if (!szTmp)
        {
            break;
        }

        *szTmp++ = '\0';
        if (ulStationNum >= ulMaxStationNum)
        {
            return FALSE;
        }

        if (!gos_atou(szStationList, pulStationList))
        {
            return FALSE;
        }

        pulStationList++;
        ulStationNum++;
        szStationList = szTmp;
    }

    return TRUE;
}

VOID ConvertStationList(UINT32 *pulStationList, UINT32 ulStationNum, CHAR *szStationList)
{
    if (ulStationNum == 0)
    {
        *szStationList = '\0';
        return;
    }

    for (UINT32 i=0; i<ulStationNum; i++)
    {
        szStationList += sprintf(szStationList, "%s%u", i==0?"":",", pulStationList[i]);
    }
}

BOOL CfgDao::GetDCUser(VECTOR<DC_USER_INFO_T> &vUser)
{
    INT32                   iRow = 0;
    PDBRET                  enRet = PDB_CONTINUE;
    PDBRsltParam            Params;
    DC_USER_INFO_T          stUser = {0};
    CHAR                    acStationList[256];

    vUser.clear();

    Params.Bind(&stUser.ulUserID);
    Params.Bind(stUser.acName, sizeof(stUser.acName));
    Params.Bind(&stUser.ulDCType);
    Params.Bind(&stUser.ulDepotID);
    Params.Bind(acStationList, sizeof(acStationList));

    iRow = m_pPDBApp->Query("SELECT UserID, Name, DCType, DepotID, StationList FROM DCUser WHERE SyncFlag>=0 ORDER BY UserID", Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query DCUser failed");
        return FALSE;
    }

    memset(&stUser, 0, sizeof(stUser));
    acStationList[0] = '\0';
    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        if (!ConvertStationList(acStationList, stUser.aulStationList, ARRAY_SIZE(stUser.aulStationList), stUser.ulStationNum))
        {
            DaoLog(LOG_ERROR, "invalid station list: %s", acStationList);
            stUser.ulStationNum = 0;
        }

        vUser.push_back(stUser);
        memset(&stUser, 0, sizeof(stUser));
        acStationList[0] = '\0';
    }

    m_pPDBApp->CloseQuery();

    return PDB_ERROR != enRet;
}

BOOL CfgDao::AddDCUser(DC_USER_INFO_T &stRec)
{
    stRec.ulDepotID = DEPOT_ID_UNKNOWN;

    return SetDCUser(stRec, SYNC_ADD);
}

BOOL CfgDao::SetDCUser(DC_USER_INFO_T &stRec, INT32 iSyncFlag)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;
    CHAR            acStationList[256];

    ConvertStationList(stRec.aulStationList, stRec.ulStationNum, acStationList);

    FormatSyncFlag(iSyncFlag);

    Params.Bind(&iSyncFlag);
    Params.Bind(&stRec.ulUserID);
    Params.BindString(stRec.acName);
    Params.Bind(&stRec.ulDCType);
    Params.Bind(&stRec.ulDepotID);
    Params.BindString(acStationList);

    Params.Bind(&iSyncFlag);
    Params.BindString(stRec.acName);
    Params.Bind(&stRec.ulDCType);
    Params.Bind(&stRec.ulDepotID);
    Params.BindString(acStationList);

    iAffectedRow = m_pPDBApp->Update("INSERT INTO DCUser(SyncFlag, UserID, Name, DCType, DepotID, StationList) "
        "VALUES(?,?,?,?,?,?) ON DUPLICATE KEY UPDATE SyncFlag=?, Name=?, DCType=?, DepotID=?, StationList=?", Params);
    return iAffectedRow > 0;
}

BOOL CfgDao::DelDCUser(UINT32 ulUserID)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.Bind(&ulUserID);
    iAffectedRow = m_pPDBApp->Update("DELETE FROM DCUser WHERE UserID=?", Params);

    if (INVALID_ROWCOUNT == iAffectedRow)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CfgDao::GetSyncingDCUser(UINT32 ulMaxRecNum, VECTOR<DC_USER_INFO_T> &vAddRec, VECTOR<DC_USER_INFO_T> &vDelRec)
{
    INT32                   iRow = 0;
    PDBRET                  enRet = PDB_CONTINUE;
    PDBRsltParam            Params;
    DC_USER_INFO_T          stRec;
    INT32                   iSyncFlag;
    CHAR                    acSQL[1024];
    CHAR                    acStationList[256];

    vAddRec.clear();
    vDelRec.clear();

    Params.Bind(&iSyncFlag);
    Params.Bind(&stRec.ulUserID);
    Params.Bind(stRec.acName, sizeof(stRec.acName));
    Params.Bind(&stRec.ulDCType);
    Params.Bind(&stRec.ulDepotID);
    Params.Bind(acStationList, sizeof(acStationList));

    sprintf(acSQL, "SELECT SyncFlag, UserID, Name, DCType, DepotID, StationList FROM DCUser WHERE SyncFlag!=0");

    iRow = m_pPDBApp->Query(acSQL, Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query DCUser failed");
        return FALSE;
    }

    memset(&stRec, 0, sizeof(stRec));
    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        if (ConvertStationList(acStationList, stRec.aulStationList, ARRAY_SIZE(stRec.aulStationList), stRec.ulStationNum))
        {
            stRec.ulStationNum = 0;
        }

        if (iSyncFlag > 0)
        {
            vAddRec.push_back(stRec);
        }
        else
        {
            vDelRec.push_back(stRec);
        }

        if ((vAddRec.size()+vDelRec.size()) >= ulMaxRecNum)
        {
            break;
        }

        memset(&stRec, 0, sizeof(stRec));
    }

    m_pPDBApp->CloseQuery();

    return PDB_ERROR != enRet;
}

BOOL CfgDao::SyncOverDCUser(DC_USER_INFO_T &stRec, INT32 iSyncFlag)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.Bind(&stRec.ulUserID);

    if (iSyncFlag > 0)
    {
        iAffectedRow = m_pPDBApp->Update("UPDATE DCUser SET SyncFlag=0 WHERE UserID=?", Params);
        if (iAffectedRow <= 0)
        {
            return FALSE;
        }
    }
    else if (iSyncFlag < 0)
    {
        iAffectedRow = m_pPDBApp->Update("DELETE FROM DCUser WHERE UserID=?", Params);
        if (iAffectedRow < 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CfgDao::GetMaxSDSID(UINT32 *pulMaxSDSID)
{
    INT32                   iRow = 0;
    PDBRET                  enRet;
    PDBRsltParam            Params;
    UINT32                  ulCount;

    Params.Bind(pulMaxSDSID);
    Params.Bind(&ulCount);

    iRow = m_pPDBApp->Query("SELECT MAX(SDSID), COUNT(*) FROM SDSInfo", Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query SDSInfo failed");
        return FALSE;
    }

    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        if (ulCount == 0)
        {
            *pulMaxSDSID = 0;
        }
        break;
    }

    m_pPDBApp->CloseQuery();

    if (PDB_ERROR == enRet)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CfgDao::GetSDSIndex(SDS_INDEX_T &stInfo)
{
    INT32                   iRow = 0;
    PDBRET                  enRet;
    PDBRsltParam            Params;
    CHAR                    acSQL[256];
    UINT32                  ulDay = gos_get_day_from_time(gos_get_current_time());
    UINT32                  ulCount = 0;

    Params.Bind(&stInfo.ulFormalDispatchSDSIndex);
    Params.Bind(&stInfo.ulOralDispatchSDSDay);
    Params.Bind(&stInfo.ulOralDispatchSDSIndex);
    Params.Bind(&stInfo.ulOperateSDSDay);
    Params.Bind(&stInfo.ulOperateSDSIndex);

    sprintf(acSQL, "SELECT FormalDispatchSDSIndex, OralDispatchSDSDay, OralDispatchSDSIndex, OperateSDSDay, OperateSDSIndex FROM SDSIndex");
    iRow = m_pPDBApp->Query(acSQL, Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query SDSIndex failed");
        return FALSE;
    }

    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        ulCount ++;
        break;
    }

    m_pPDBApp->CloseQuery();

    if (PDB_ERROR == enRet)
    {
        return FALSE;
    }

    if (ulCount == 0)
    {
        INT32       iSyncFlag = SYNC_ADD;

        FormatSyncFlag(iSyncFlag);

        sprintf(acSQL, "INSERT INTO SDSIndex(FormalDispatchSDSIndex, OralDispatchSDSDay, OralDispatchSDSIndex, OperateSDSDay, OperateSDSIndex, SyncFlag) "
                    "VALUES(%u, %u, %u, %u, %u, %d)", FORMAL_DISPATCH_SDS_INDEX_MIN, ulDay, ORAL_DISPATCH_SDS_INDEX_MIN, ulDay, OPERATE_SDS_INDEX_MIN, iSyncFlag);
        m_pPDBApp->Update(acSQL);

        return FALSE;
    }

    return TRUE;
}

BOOL CfgDao::UpdateSDSIndex(UINT32 ulSDSDay, UINT32 ulFormalDispatchSDSIndex, UINT32 ulOralDispatchSDSIndex, UINT32 ulOperateSDSIndex)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.Bind(&ulFormalDispatchSDSIndex);
    Params.Bind(&ulSDSDay);
    Params.Bind(&ulOralDispatchSDSIndex);
    Params.Bind(&ulSDSDay);
    Params.Bind(&ulOperateSDSIndex);

    iAffectedRow = m_pPDBApp->Update("UPDATE SDSIndex SET FormalDispatchSDSIndex=?, OralDispatchSDSDay=?, OralDispatchSDSIndex=?, OperateSDSDay=?, OperateSDSIndex=?", Params);
    if (iAffectedRow < 0)
    {
        return FALSE;
    }

    if (iAffectedRow == 0)
    {
        CHAR    acSQL[1024];

        INT32       iSyncFlag = SYNC_ADD;

        FormatSyncFlag(iSyncFlag);

        sprintf(acSQL, "INSERT INTO SDSIndex(FormalDispatchSDSIndex, OralDispatchSDSDay, OralDispatchSDSIndex, OperateSDSDay, OperateSDSIndex, SyncFlag) "
                    "VALUES(%u, %u, %u, %u, %u, %d)", ulFormalDispatchSDSIndex, ulSDSDay, ulOralDispatchSDSIndex, ulSDSDay, ulOperateSDSIndex, iSyncFlag);
        m_pPDBApp->Update(acSQL);

        return iAffectedRow>0;
    }

    return TRUE;
}

BOOL CfgDao::GetSyncingSDSIndex(SDS_INDEX_T &stInfo)
{
    INT32                   iRow = 0;
    PDBRET                  enRet;
    PDBRsltParam            Params;
    CHAR                    acSQL[256];
    UINT32                  ulCount = 0;

    Params.Bind(&stInfo.ulFormalDispatchSDSIndex);
    Params.Bind(&stInfo.ulOralDispatchSDSDay);
    Params.Bind(&stInfo.ulOralDispatchSDSIndex);
    Params.Bind(&stInfo.ulOperateSDSDay);
    Params.Bind(&stInfo.ulOperateSDSIndex);

    sprintf(acSQL, "SELECT FormalDispatchSDSIndex, OralDispatchSDSDay, OralDispatchSDSIndex, OperateSDSDay, OperateSDSIndex FROM SDSIndex WHERE SyncFlag!=0");
    iRow = m_pPDBApp->Query(acSQL, Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query SDSIndex failed");
        return FALSE;
    }

    memset(&stInfo, 0, sizeof(stInfo));
    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        ulCount ++;
        break;
    }

    m_pPDBApp->CloseQuery();

    if (PDB_ERROR == enRet)
    {
        return FALSE;
    }

    return ulCount > 0;
}

BOOL CfgDao::SetSDSIndex(SDS_INDEX_T &stInfo)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;
    INT32           iSyncFlag = SYNC_NULL;

    Params.Bind(&stInfo.ulFormalDispatchSDSIndex);
    Params.Bind(&stInfo.ulOralDispatchSDSDay);
    Params.Bind(&stInfo.ulOralDispatchSDSIndex);
    Params.Bind(&stInfo.ulOperateSDSDay);
    Params.Bind(&stInfo.ulOperateSDSIndex);
    Params.Bind(&iSyncFlag);

    iAffectedRow = m_pPDBApp->Update("UPDATE SDSIndex SET FormalDispatchSDSIndex=?, OralDispatchSDSDay=?, OralDispatchSDSIndex=?, OperateSDSDay=?, OperateSDSIndex=?, SyncFlag=?", Params);
    if (iAffectedRow < 0)
    {
        return FALSE;
    }

    if (iAffectedRow == 0)
    {
        CHAR    acSQL[1024];

        sprintf(acSQL, "INSERT INTO SDSIndex(FormalDispatchSDSIndex, OralDispatchSDSDay, OralDispatchSDSIndex, OperateSDSDay, OperateSDSIndex, SyncFlag) "
            "VALUES(%u, %u, %u, %u, %u, %d)", stInfo.ulFormalDispatchSDSIndex, stInfo.ulOralDispatchSDSDay, stInfo.ulOralDispatchSDSIndex, stInfo.ulOperateSDSDay, stInfo.ulOperateSDSIndex, iSyncFlag);
        m_pPDBApp->Update(acSQL);

        return iAffectedRow>0;
    }

    return TRUE;
}

BOOL CfgDao::GetSDSIDBySDSIndex(UINT32 ulSDSIndex, UINT32 *pulSDSID)
{
    INT32                   iRow = 0;
    PDBRET                  enRet;
    PDBRsltParam            Params;
    CHAR                    acSQL[1024];

    Params.Bind(pulSDSID);
    sprintf(acSQL, "SELECT SDSID FROM SDSInfo WHERE SDSIndex=%u AND SendTime=(SELECT MAX(SendTime) FROM SDSInfo WHERE SDSIndex=%u)",
                    ulSDSIndex, ulSDSIndex);
    iRow = m_pPDBApp->Query(acSQL, Params);
    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query SDSInfo failed");
        return FALSE;
    }

    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        break;
    }

    m_pPDBApp->CloseQuery();

    if (PDB_ERROR == enRet)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CfgDao::SetSDSInfo(UINT32 ulSDSID, BOOL bSendFlag)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;
    INT32           iSyncFlag = SYNC_ADD;

    FormatSyncFlag(iSyncFlag);

    Params.Bind(&iSyncFlag);
    Params.Bind(&bSendFlag);
    Params.Bind(&ulSDSID);

    iAffectedRow = m_pPDBApp->Update("UPDATE SDSInfo SET SyncFlag=?, SendFlag=? WHERE SDSID=?", Params);
    if (iAffectedRow <= 0)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CfgDao::DelSDSInfo(UINT32 ulSDSID)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;
    BOOL            bRet = FALSE;
    INT32           iSyncFlag = SYNC_DEL;

    FormatSyncFlag(iSyncFlag);

    if (!TransBegin())
    {
        return FALSE;
    }

    Params.Bind(&iSyncFlag);
    Params.Bind(&ulSDSID);
    iAffectedRow = m_pPDBApp->Update("UPDATE SDSInfo SET SyncFlag=? WHERE SDSID=?", Params);
    if (INVALID_ROWCOUNT == iAffectedRow)
    {
        goto end;
    }

    iAffectedRow = m_pPDBApp->Update("UPDATE SDSReply SET SyncFlag=? WHERE SDSID=?", Params);
    if (INVALID_ROWCOUNT == iAffectedRow)
    {
        goto end;
    }

    if (!TransCommit())
    {
        goto end;
    }

    bRet = TRUE;

end:
    if (!bRet)
    {
        TransRollback();
    }

    return bRet;
}

BOOL CfgDao::GetCellInfo(UINT32 ulStartTime, UINT32 ulMaxNum, VECTOR<CELL_INFO_T> &vInfo)
{
    INT32                   iRow = 0;
    PDBRET                  enRet;
    PDBRsltParam            Params;
    UINT32                  ulNum = 0;
    UINT32                  ulLastTime = 0;
    CELL_INFO_T             stRec;
    CHAR                    acSQL[1024];

    Params.Bind(&stRec.ulTime);
    Params.Bind(&stRec.ulTrainUserID);
    Params.Bind(&stRec.ulTrainUnitID);
    Params.Bind(&stRec.ulCellID);
    Params.Bind(&stRec.ulStationID);
    Params.Bind(&stRec.ulUpsideStationID);
    Params.Bind(&stRec.ulDownsideStationID);
    Params.Bind(&stRec.ulTransferTrackFlag);
    Params.Bind(&stRec.ulTurnbackTrackFlag);
    Params.Bind(&stRec.ulDirection);

    sprintf(acSQL, "SELECT Time, UserID, TrainUnitID, CellID, StationID, UpsideStationID, DownsideStationID, TransferTrackFlag, TurnbackTrackFlag, Direction "
        "FROM CellInfo WHERE Time>=%u", ulStartTime);
    iRow = m_pPDBApp->Query(acSQL, Params);

    vInfo.clear();
    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query CellInfo failed");
        return FALSE;
    }

    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        ulNum ++;
        if (ulNum > ulMaxNum)
        {
            if (ulLastTime > 0 && ulLastTime != stRec.ulTime)
            {
                break;
            }
        }

        vInfo.push_back(stRec);

        ulLastTime = stRec.ulTime;
        memset(&stRec, 0, sizeof(stRec));
    }

    m_pPDBApp->CloseQuery();

    if (PDB_ERROR == enRet)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CfgDao::GetRecBrdCfg(VECTOR<REC_BRD_CFG_T> &vInfo)
{
    INT32                   iRow = 0;
    PDBRET                  enRet;
    REC_BRD_CFG_T           stInfo;
    PDBRsltParam            Params;
    CHAR                    acSQL[256];

    Params.Bind(&stInfo.ulRecID);
    Params.Bind(stInfo.acRecInfo, sizeof(stInfo.acRecInfo));
    sprintf(acSQL, "SELECT RecID, RecInfo FROM RecBrd WHERE SyncFlag>=0 ORDER By RecID");
    iRow = m_pPDBApp->Query(acSQL, Params);

    vInfo.clear();
    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query RecBrd failed");
        return FALSE;
    }

    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        vInfo.push_back(stInfo);
    }

    m_pPDBApp->CloseQuery();

    if (PDB_ERROR == enRet)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CfgDao::AddRecBrdCfg(REC_BRD_CFG_T &stInfo)
{
    return SetRecBrdCfg(stInfo, SYNC_ADD);
}

BOOL CfgDao::DelRecBrdCfg(UINT32 ulRecID)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;
    INT32           iSyncFlag = SYNC_DEL;

    FormatSyncFlag(iSyncFlag);

    Params.Bind(&iSyncFlag);
    Params.Bind(&ulRecID);

    iAffectedRow = m_pPDBApp->Update("UPDATE RecBrd SET SyncFlag=? WHERE RecID=?", Params);

    return iAffectedRow >= 0;
}

BOOL CfgDao::SetRecBrdCfg(REC_BRD_CFG_T &stRec)
{
    return SetRecBrdCfg(stRec, SYNC_ADD);
}

BOOL CfgDao::SetRecBrdCfg(REC_BRD_CFG_T &stRec, INT32 iSyncFlag)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    FormatSyncFlag(iSyncFlag);

    Params.Bind(&iSyncFlag);
    Params.Bind(&stRec.ulRecID);
    Params.Bind(stRec.acRecInfo, sizeof(stRec.acRecInfo));

    Params.Bind(&iSyncFlag);
    Params.Bind(stRec.acRecInfo, sizeof(stRec.acRecInfo));

    iAffectedRow = m_pPDBApp->Update("INSERT INTO RecBrd(SyncFlag, RecID, RecInfo) "
        "VALUES(?,?,?) ON DUPLICATE KEY UPDATE SyncFlag=?, RecInfo=?", Params);
    if (iAffectedRow <= 0)
    {
        DaoLog(LOG_ERROR, "set RecBrd failed, rec ID is %u", stRec.ulRecID);
        return FALSE;
    }

    return TRUE;
}

BOOL CfgDao::GetSyncingRecBrdCfg(UINT32 ulMaxRecNum, VECTOR<REC_BRD_CFG_T> &vAddRec, VECTOR<REC_BRD_CFG_T> &vDelRec)
{
    INT32                   iRow = 0;
    PDBRET                  enRet = PDB_CONTINUE;
    PDBRsltParam            Params;
    REC_BRD_CFG_T           stRec;
    INT32                   iSyncFlag;
    CHAR                    acSQL[1024];

    vAddRec.clear();
    vDelRec.clear();

    Params.Bind(&iSyncFlag);
    Params.Bind(&stRec.ulRecID);
    Params.Bind(stRec.acRecInfo, sizeof(stRec.acRecInfo));

    sprintf(acSQL, "SELECT SyncFlag, RecID, RecInfo FROM RecBrd WHERE SyncFlag!=0");

    iRow = m_pPDBApp->Query(acSQL, Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query RecBrd failed");
        return FALSE;
    }

    memset(&stRec, 0, sizeof(stRec));
    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        if (iSyncFlag > 0)
        {
            vAddRec.push_back(stRec);
        }
        else
        {
            vDelRec.push_back(stRec);
        }

        if ((vAddRec.size()+vDelRec.size()) >= ulMaxRecNum)
        {
            break;
        }

        memset(&stRec, 0, sizeof(stRec));
    }

    m_pPDBApp->CloseQuery();

    return PDB_ERROR != enRet;
}

BOOL CfgDao::SyncOverRecBrdCfg(REC_BRD_CFG_T &stRec, INT32 iSyncFlag)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.Bind(&stRec.ulRecID);

    if (iSyncFlag > 0)
    {
        iAffectedRow = m_pPDBApp->Update("UPDATE RecBrd SET SyncFlag=0 WHERE RecID=?", Params);
        if (iAffectedRow <= 0)
        {
            return FALSE;
        }
    }
    else if (iSyncFlag < 0)
    {
        iAffectedRow = m_pPDBApp->Update("DELETE FROM RecBrd WHERE RecID=?", Params);
        if (iAffectedRow < 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*
BOOL CfgDao::SetRecBrdCfg(VECTOR<REC_BRD_CFG_T> &vCfg)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;
    REC_BRD_CFG_T   stCfg;

    TransBegin();

    iAffectedRow = m_pPDBApp->Update("DELETE FROM RecBrd", Params);
    if (iAffectedRow < 0)
    {
        goto fail;
    }

    Params.Bind(&stCfg.ulRecID);
    Params.BindString(stCfg.acRecInfo);

    for (UINT32 i=0; i<vCfg.size(); i++)
    {
        memcpy(&stCfg, &vCfg[i], sizeof(stCfg));
        iAffectedRow = m_pPDBApp->Update("INSERT INTO RecBrd(RecID, RecInfo) VALUES(?,?)", Params);
        if (iAffectedRow <= 0)
        {
            goto fail;
        }
    }

    if (!TransCommit())
    {
        goto fail;
    }

    return TRUE;

fail:
    TransRollback();
    return FALSE;
}
*/

BOOL CfgDao::GetMaxRecBrdSeqID(UINT32 *pulMaxSeqID)
{
    INT32                   iRow = 0;
    PDBRET                  enRet;
    PDBRsltParam            Params;
    UINT32                  ulCount;

    Params.Bind(pulMaxSeqID);
    Params.Bind(&ulCount);

    iRow = m_pPDBApp->Query("SELECT MAX(BrdSeqID), COUNT(*) FROM RecBrdInfo", Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query RecBrdInfo failed");
        return FALSE;
    }

    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        if (ulCount == 0)
        {
            *pulMaxSeqID = 0;
        }
        break;
    }

    m_pPDBApp->CloseQuery();

    if (PDB_ERROR == enRet)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CfgDao::GetMaxRealBrdSeqID(UINT32 *pulMaxSeqID)
{
    INT32                   iRow = 0;
    PDBRET                  enRet;
    PDBRsltParam            Params;
    UINT32                  ulCount;

    Params.Bind(pulMaxSeqID);
    Params.Bind(&ulCount);

    iRow = m_pPDBApp->Query("SELECT MAX(StartTime), COUNT(*) FROM RealBrdInfo", Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query RealBrdInfo failed");
        return FALSE;
    }

    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        if (ulCount == 0)
        {
            *pulMaxSeqID = 0;
        }
        break;
    }

    m_pPDBApp->CloseQuery();

    if (PDB_ERROR == enRet)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CfgDao::SetTrainID(UINT32 ulTrainUnitID, CHAR *szTrainID, CHAR *szDestID, UINT32 ulDirection)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.BindString(szTrainID);
    Params.BindString(szDestID);
    Params.Bind(&ulDirection);
    Params.Bind(&ulTrainUnitID);

    iAffectedRow = m_pPDBApp->Update("UPDATE Train SET TrainID=?,DestID=?,Direction=? WHERE TrainUnitID=?", Params);
    if (iAffectedRow < 0)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CfgDao::GetOperLogInfo(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulDCUserID, UINT32 ulMaxNum, GJsonTupleParam &JsonTupleParam)
{
    INT32                   iRow = 0;
    PDBRET                  enRet = PDB_CONTINUE;
    PDBRsltParam            Params;
    UINT32                  ulTime;
    UINT32                  ulUserID;
    CHAR                    acDCOperatorID[32];
    CHAR                    acLog[1024];
    GJsonParam              JsonParam;
    CHAR                    acSQL[256];

    JsonTupleParam.Clear();

    Params.Bind(&ulTime);
    Params.Bind(&ulUserID);
    Params.Bind(acDCOperatorID, sizeof(acDCOperatorID));
    Params.Bind(acLog, sizeof(acLog));

    if (ulDCUserID == INVALID_USER_ID)
    {
        sprintf(acSQL, "SELECT Time, UserID, DCOperatorID, LogInfo FROM OperLog WHERE Time>=%u AND Time<=%u AND SyncFlag>=0 ORDER BY Time %s",
                ulStartTime, ulEndTime, ulStartTime==0?"DESC":"");
    }
    else
    {
        sprintf(acSQL, "SELECT Time, UserID, DCOperatorID, LogInfo FROM OperLog WHERE Time>=%u AND Time<=%u AND UserID=%u AND SyncFlag>=0 ORDER BY Time %s",
                ulStartTime, ulEndTime, ulDCUserID, ulStartTime==0?"DESC":"");
    }

    iRow = m_pPDBApp->Query(acSQL, Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query OperLog failed");
        return FALSE;
    }

    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        JsonParam.Clear();
        JsonParam.Add("Time", ulTime);
        JsonParam.Add("DCUserID", ulUserID);
        JsonParam.Add("DCOperatorID", acDCOperatorID);
        JsonParam.Add("LogInfo", acLog);

        JsonTupleParam.Add(JsonParam);

        if (JsonTupleParam.GetStringSize() >= GetSafeQueryRecLen())
        {
            break;
        }
    }

    m_pPDBApp->CloseQuery();

    return PDB_ERROR != enRet;
}

BOOL CfgDao::AddOperLog(UINT32 ulTime, UINT32 ulDCUserID, CHAR *szOperatorID, CHAR *szLogInfo)
{
    OPER_LOG_INFO_T     stRec;

    memset(&stRec, 0, sizeof(stRec));
    stRec.ulTime = ulTime;
    stRec.ulUserID = ulDCUserID;

    if (*szOperatorID == '\0')
    {
        sprintf(stRec.acDCOperatorID, "%u", ulDCUserID);
    }
    else
    {
        strncpy(stRec.acDCOperatorID, szOperatorID, sizeof(stRec.acDCOperatorID)-1);
    }

    strncpy(stRec.acLogInfo, szLogInfo, sizeof(stRec.acLogInfo)-1);

    return SetOperLog(stRec, SYNC_ADD);
}

BOOL CfgDao::SetOperLog(OPER_LOG_INFO_T &stRec, INT32 iSyncFlag)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    FormatSyncFlag(iSyncFlag);

    Params.Bind(&iSyncFlag);
    Params.Bind(&stRec.ulTime);
    Params.Bind(&stRec.ulUserID);
    Params.BindString(stRec.acDCOperatorID);
    Params.BindString(stRec.acLogInfo);

    Params.Bind(&iSyncFlag);
    Params.BindString(stRec.acLogInfo);

    iAffectedRow = m_pPDBApp->Update("INSERT INTO OperLog(SyncFlag, Time, UserID, DCOperatorID, LogInfo) VALUES(?,?,?,?,?) "
                                     "ON DUPLICATE KEY UPDATE SyncFlag=?, LogInfo=?", Params);
    return iAffectedRow > 0;
}

BOOL CfgDao::GetSyncingOperLog(UINT32 ulMaxRecNum, VECTOR<OPER_LOG_INFO_T> &vAddRec, VECTOR<OPER_LOG_INFO_T> &vDelRec)
{
    INT32                   iRow = 0;
    PDBRET                  enRet = PDB_CONTINUE;
    PDBRsltParam            Params;
    OPER_LOG_INFO_T         stRec;
    INT32                   iSyncFlag;
    CHAR                    acSQL[256];

    vAddRec.clear();
    vDelRec.clear();

    Params.Bind(&iSyncFlag);
    Params.Bind(&stRec.ulTime);
    Params.Bind(&stRec.ulUserID);
    Params.Bind(stRec.acDCOperatorID, sizeof(stRec.acDCOperatorID));
    Params.Bind(stRec.acLogInfo, sizeof(stRec.acLogInfo));

    sprintf(acSQL, "SELECT SyncFlag, Time, UserID, DCOperatorID, LogInfo FROM OperLog WHERE SyncFlag!=0");

    iRow = m_pPDBApp->Query(acSQL, Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query OperLog failed");
        return FALSE;
    }

    memset(&stRec, 0, sizeof(stRec));
    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        if (iSyncFlag > 0)
        {
            vAddRec.push_back(stRec);
        }
        else
        {
            vDelRec.push_back(stRec);
        }

        if ((vAddRec.size()+vDelRec.size()) >= ulMaxRecNum)
        {
            break;
        }

        memset(&stRec, 0, sizeof(stRec));
    }

    m_pPDBApp->CloseQuery();

    return PDB_ERROR != enRet;
}

BOOL CfgDao::SyncOverOperLog(OPER_LOG_INFO_T &stRec, INT32 iSyncFlag)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.Bind(&stRec.ulTime);
    Params.BindString(stRec.acDCOperatorID);

    if (iSyncFlag > 0)
    {
        iAffectedRow = m_pPDBApp->Update("UPDATE OperLog SET SyncFlag=0 WHERE Time=? AND DCOperatorID=?", Params);
        if (iAffectedRow <= 0)
        {
            return FALSE;
        }
    }
    else if (iSyncFlag < 0)
    {
        iAffectedRow = m_pPDBApp->Update("DELETE FROM OperLog WHERE Time=? AND DCOperatorID=?", Params);
        if (iAffectedRow < 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}

INT32 CfgDao::GetRecNum(CHAR *szTable)
{
    INT32                   iRow = 0;
    UINT32                  ulNum = 0;
    PDBRsltParam            Params;
    PDBRET                  enRet;
    CHAR                    acSQL[512];

    Params.Bind(&ulNum);

    sprintf(acSQL, "SELECT COUNT(*) FROM %s", szTable);
    iRow = m_pPDBApp->Query(acSQL, Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query %s failed", szTable);
        return -1;
    }

    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
    }

    m_pPDBApp->CloseQuery();

    return ulNum;
}

BOOL CfgDao::ClearTable(CHAR *szTable, CHAR *szFieldName, UINT32 ulMaxResvNum)
{
    CHAR        acSQL[1024];
    INT32       iDeleteNum = 0;
    INT32       iAffectedRow;

    iDeleteNum = GetRecNum(szTable);
    if (iDeleteNum < 0)
    {
        DaoLog(LOG_ERROR, "get rec number of %s failed", szTable);
        return FALSE;
    }

    iDeleteNum -= ulMaxResvNum;
    if (iDeleteNum <= 0)
    {
        return TRUE;
    }

    sprintf(acSQL, "DELETE FROM %s ORDER BY %s LIMIT %d", szTable, szFieldName, iDeleteNum);

    iAffectedRow = m_pPDBApp->Update(acSQL);
    if (iAffectedRow < 0)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CfgDao::ClearCellInfo(UINT32 ulDeadTime)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.Bind(&ulDeadTime);

    iAffectedRow = m_pPDBApp->Update("DELETE FROM CellInfo WHERE Time<?", Params);
    DaoLog(LOG_INFO, "delete %d records from CellInfo", iAffectedRow);

    return iAffectedRow >= 0;
}

BOOL CfgDao::ClearGroupCallInfo(UINT32 ulDeadTime)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.Bind(&ulDeadTime);

    iAffectedRow = m_pPDBApp->Update("DELETE FROM GroupCallInfo WHERE Time<?", Params);
    DaoLog(LOG_INFO, "delete %d records from GroupCallInfo", iAffectedRow);

    return iAffectedRow >= 0;
}

BOOL CfgDao::ClearIPHTalkInfo(UINT32 ulDeadTime)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.Bind(&ulDeadTime);

    iAffectedRow = m_pPDBApp->Update("DELETE FROM IPHTalkInfo WHERE ApplyTime<?", Params);
    DaoLog(LOG_INFO, "delete %d records from IPHTalkInfo", iAffectedRow);

    return iAffectedRow >= 0;
}

BOOL CfgDao::ClearOperLog(UINT32 ulDeadTime)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.Bind(&ulDeadTime);

    iAffectedRow = m_pPDBApp->Update("DELETE FROM OperLog WHERE Time<?", Params);
    DaoLog(LOG_INFO, "delete %d records from OperLog", iAffectedRow);

    return iAffectedRow >= 0;
}

BOOL CfgDao::ClearP2PCallInfo(UINT32 ulDeadTime)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.Bind(&ulDeadTime);

    iAffectedRow = m_pPDBApp->Update("DELETE FROM P2PCallInfo WHERE Time<?", Params);
    DaoLog(LOG_INFO, "delete %d records from P2PCallInfo", iAffectedRow);

    return iAffectedRow >= 0;
}

BOOL CfgDao::ClearRealBrdInfo(UINT32 ulDeadTime)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.Bind(&ulDeadTime);

    iAffectedRow = m_pPDBApp->Update("DELETE FROM RealBrdInfo WHERE StartTime<?", Params);
    DaoLog(LOG_INFO, "delete %d records from RealBrdInfo", iAffectedRow);

    return iAffectedRow >= 0;
}

BOOL CfgDao::ClearRecBrdInfo(UINT32 ulDeadTime)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    if (!ClearRecBrdReply(ulDeadTime))
    {
        return FALSE;
    }

    Params.Bind(&ulDeadTime);

    iAffectedRow = m_pPDBApp->Update("DELETE FROM RecBrdInfo WHERE Time<?", Params);
    DaoLog(LOG_INFO, "delete %d records from RecBrdInfo", iAffectedRow);

    return iAffectedRow >= 0;
}

BOOL CfgDao::ClearRecBrdReply(UINT32 ulDeadTime)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.Bind(&ulDeadTime);

    iAffectedRow = m_pPDBApp->Update("DELETE FROM RecBrdReply WHERE BrdSeqID IN (SELECT BrdSeqID FROM RecBrdInfo WHERE Time<?)", Params);
    DaoLog(LOG_INFO, "delete %d records from RecBrdReply", iAffectedRow);

    return iAffectedRow >= 0;
}

BOOL CfgDao::ClearReqCallInfo(UINT32 ulDeadTime)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.Bind(&ulDeadTime);

    iAffectedRow = m_pPDBApp->Update("DELETE FROM ReqCallInfo WHERE ReqTime<?", Params);
    DaoLog(LOG_INFO, "delete %d records from ReqCallInfo", iAffectedRow);

    return iAffectedRow >= 0;
}

BOOL CfgDao::ClearSDSInfo(UINT32 ulDeadTime)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    // 先删除关联子表
    if (!ClearSDSReply(ulDeadTime))
    {
        return FALSE;
    }

    Params.Bind(&ulDeadTime);

    iAffectedRow = m_pPDBApp->Update("DELETE FROM SDSInfo WHERE SendTime<?", Params);
    DaoLog(LOG_INFO, "delete %d records from SDSInfo", iAffectedRow);

    return iAffectedRow >= 0;
}

BOOL CfgDao::ClearSDSReply(UINT32 ulDeadTime)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.Bind(&ulDeadTime);

    iAffectedRow = m_pPDBApp->Update("DELETE FROM SdsReply WHERE SDSID IN (SELECT SDSID FROM SDSInfo WHERE SendTime<?)", Params);
    DaoLog(LOG_INFO, "delete %d records from SdsReply", iAffectedRow);

    return iAffectedRow >= 0;
}
