#include "RecPublic.h"
#include "pdb_app.h"

#include "CfgDao.h"

extern UINT32       g_ulDBType;
extern PDBEnvBase   *g_PDBEnv;

extern REC_CFG_T g_stIniCfg;

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

        if (g_ulDBType != STORE_ON_FILE && !pInstance->Init(pPDBEnv))
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

BOOL CfgDao::AddOperLog(UINT32 ulTime, CHAR *szOperatorID, CHAR *szLogInfo)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.Bind(&ulTime);
    Params.BindString(szOperatorID);
    Params.BindString(szLogInfo);

    iAffectedRow = m_pPDBApp->Update("INSERT INTO OperLog(Time, OperatorID, LogInfo) VALUES(?,?,?) ", Params);

    return iAffectedRow > 0;
}

VOID ParseRecFile(CHAR *szFile, CHAR *szSimpleFile, CHAR *&szPostfix)
{
    szPostfix = gos_get_file_postfix(szFile);
    if (!szPostfix)
    {
        szPostfix = (CHAR*)"";
    }

    szFile = gos_get_simple_file_name(szFile);
    if (!szFile)
    {
        *szSimpleFile = '\0';
        return;
    }

    strcpy(szSimpleFile, szFile);

    CHAR    *szPos = gos_right_strchr(szSimpleFile, '.');

    if (szPos)
    {
        *szPos = '\0';
    }
}

BOOL CfgDao::AddRecFile(REC_FILE_INFO_T &stRec)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;
    CHAR            acRecFile[256];
    CHAR            *szFilePostfix;

    ParseRecFile(stRec.acFile, acRecFile, szFilePostfix);

    //Params.BindString(acRecFile);
    Params.BindString(stRec.acFile);//绝对路径
    Params.BindString(szFilePostfix); //file type
    Params.Bind(&stRec.ulTime);
    Params.Bind(&stRec.dDuration);
    Params.Bind(&stRec.ulFileSize);
    Params.Bind(&stRec.ulCallType);
    Params.Bind(&stRec.ulCaller);
    Params.Bind(&stRec.ulCallee);
    Params.Bind(&stRec.i64GroupID);
    Params.Bind(&stRec.ulTranUnitID);
    Params.Bind(&stRec.ulDCUserID);
    //Params.BindString(stRec.acIPHDevInfo);

    Params.BindString(szFilePostfix);
    Params.Bind(&stRec.ulTime);
    Params.Bind(&stRec.dDuration);
    Params.Bind(&stRec.ulFileSize);
    Params.Bind(&stRec.ulCallType);
    Params.Bind(&stRec.ulCaller);
    Params.Bind(&stRec.ulCallee);
    Params.Bind(&stRec.i64GroupID);
    Params.Bind(&stRec.ulTranUnitID);
    Params.Bind(&stRec.ulDCUserID);
    //Params.BindString(stRec.acIPHDevInfo);

    iAffectedRow = m_pPDBApp->Update("INSERT INTO rec_file(file_name, file_type, file_time, duration, file_size, call_type, caller, callee, group_id, train_id, dc_user) VALUES(?,?,?,?,?,?,?,?,?,?,?) "
            "ON DUPLICATE KEY UPDATE file_type=?, file_time=?, duration=?, file_size=?, call_type=?, caller=?, callee=?, group_id=?, train_id=?, dc_user=?", Params);
    if (iAffectedRow <= 0)
    {
        DaoLog(LOG_ERROR, "add rec_file(%s) failed", stRec.acFile);
        return FALSE;
    }

    return TRUE;
}

BOOL CfgDao::UpdateRecFile(CHAR *szRawRecFile, CHAR *szNewRecFile, DOUBLE dDuration, UINT32 ulFileSize, REC_FILE_INFO_T &stInfo)
{
    INT32               iAffectedRow = 0;
    PDBParam            Params;
    CHAR                acRecFile[256];
    CHAR                *szFilePostfix;

    ParseRecFile(szNewRecFile, acRecFile, szFilePostfix);

    Params.BindString(szNewRecFile);
    Params.BindString(szFilePostfix);
    Params.Bind(&dDuration);
    Params.Bind(&ulFileSize);
    Params.BindString(szRawRecFile);

    iAffectedRow = m_pPDBApp->Update("UPDATE rec_file SET file_name=?, file_type=?,duration=?, file_size=? WHERE file_name=?", Params);
    if (iAffectedRow < 0)
    {
        DaoLog(LOG_ERROR, "update rec_file(%s) failed", acRecFile);
        return FALSE;
    }

    if (iAffectedRow == 0)
    {
        if (!AddRecFile(stInfo))
        {
            return FALSE;
        }

    }

    return TRUE;
}

BOOL CfgDao::UpdateRecFile(CHAR *szRecFile, DOUBLE dDuration, UINT32 ulFileSize, REC_FILE_INFO_T &stInfo)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;
    CHAR            acRecFile[256];
    CHAR            *szFilePostfix;

    ParseRecFile(szRecFile, acRecFile, szFilePostfix);

    Params.BindString(szFilePostfix);
    Params.Bind(&dDuration);
    Params.Bind(&ulFileSize);
    Params.BindString(acRecFile);

    iAffectedRow = m_pPDBApp->Update("UPDATE rec_file SET file_type=?,duration=?, file_size=? WHERE file_name=?", Params);
    if (iAffectedRow < 0)
    {
        //DaoLog(LOG_ERROR, "update rec_file(%s) failed", acRecFile);
        DaoLog(LOG_ERROR, "update rec_file(%s) failed", szRecFile);
        return FALSE;
    }

    if (iAffectedRow == 0)
    {
        if (!AddRecFile(stInfo))
        {
            return FALSE;
        }

    }

    return TRUE;
}

BOOL CfgDao::ClearRecFile(UINT32 ulDeadTime)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.Bind(&ulDeadTime);

    iAffectedRow = m_pPDBApp->Update("DELETE FROM rec_file WHERE file_time<?", Params);
    DaoLog(LOG_INFO, "delete %d records from rec_file", iAffectedRow);

    return iAffectedRow >= 0;
}

VOID ConvertRecFile(REC_FILE_INFO_T &stRec, GJsonParam &Param)
{
//    CHAR        acFile[1024];
//    CHAR        acMonthDir[32];
//    CHAR        acDayDir[32];
    UINT32      ulTimeDirLen = 15; // 15 = strlen("202104\20210409")

    //TODO 规整查询录音录像文件时的路径
    //gos_get_text_time_ex(&stRec.ulTime, "%04u%02u", acMonthDir);
    //gos_get_text_time_ex(&stRec.ulTime, "%04u%02u%02u", acDayDir);
    //sprintf(acFile, "%s/%s/%s/%s", g_stIniCfg.acRecRootPath, acMonthDir, acDayDir, stRec.acFile);
    //gos_format_path(acFile);

    Param.Clear();
    Param.Add("file", stRec.acFile);
    //Param.Add("file", acFile);
    Param.Add("time", stRec.ulTime);
    Param.Add("type", stRec.ulCallType);
    Param.Add("len", stRec.dDuration);
    Param.Add("size", stRec.ulFileSize);
    Param.Add("caller", stRec.ulCaller);
    Param.Add("callee", stRec.ulCallee);
    Param.Add("group_id", stRec.i64GroupID);
    Param.Add("dc_user", stRec.ulDCUserID);
    Param.Add("train_id", stRec.ulTranUnitID);
    Param.Add("file_suffix", stRec.acFileSuffix);
    //Param.Add("iph", stRec.acIPHDevInfo);
}

BOOL CfgDao::GetRecFile(UINT32 ulDeadTime, VECTOR<std::string> &vInfo)
{
    CHAR                acFile[128] = {0};
    INT32               iRow = 0;
    PDBRET              enRet = PDB_CONTINUE;
    PDBRsltParam        Params;
    CHAR                acSQL[1024];

    Params.Bind(acFile, sizeof(acFile));

    sprintf(acSQL, "SELECT file_name FROM rec_file WHERE file_time<=%u AND call_type>0 ORDER BY file_time DESC",ulDeadTime);

    iRow = m_pPDBApp->Query(acSQL, Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query rec_file failed");
        return FALSE;
    }

    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        std::string strFileName = acFile;

        vInfo.push_back(strFileName);
        memset(acFile, 0 ,sizeof(acFile));

        if (vInfo.size() > 60*1024)
        {
            break;
        }
    }

    m_pPDBApp->CloseQuery();

    return PDB_ERROR != enRet;

}

BOOL CfgDao::GetRecFile(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulCallType, GJsonTupleParam &vRec)
{
    INT32               iRow = 0;
    PDBRET              enRet = PDB_CONTINUE;
    PDBRsltParam        Params;
    REC_FILE_INFO_T     stRec = {0};
    CHAR                acSQL[1024];
    CHAR                acCallType[128];
    CHAR                acFileType[32];
    GJsonParam          Param;

    vRec.Clear();
    acFileType[0] = '\0';

    Params.Bind(stRec.acFile, sizeof(stRec.acFile));
    Params.Bind(acFileType, sizeof(acFileType));
    Params.Bind(&stRec.ulTime);
    Params.Bind(&stRec.dDuration);
    Params.Bind(&stRec.ulFileSize);
    Params.Bind(&stRec.ulCallType);
    Params.Bind(&stRec.ulCaller);
    Params.Bind(&stRec.ulCallee);
    Params.Bind(&stRec.i64GroupID);
    Params.Bind(stRec.acFileSuffix, sizeof(stRec.acFileSuffix));
    //Params.Bind(stRec.acIPHDevInfo, sizeof(stRec.acIPHDevInfo));

    if (ulCallType == REC_CALL_TYPE_ALL)
    {
        sprintf(acCallType, " AND call_type>0"); // SQL语句中Where条件包含time和call_type，保证采用某个索引
    }
    else
    {
        sprintf(acCallType, " AND call_type=%u", ulCallType);
    }

    if (ulStartTime == 0)
    {
        sprintf(acSQL, "SELECT file_name, file_type, file_time, duration, file_size, call_type, caller, callee, group_id, file_type  FROM rec_file WHERE file_time<=%u %s ORDER BY file_time DESC",
                ulEndTime, acCallType);
    }
    else
    {
        sprintf(acSQL, "SELECT file_name, file_type, file_time, duration, file_size, call_type, caller, callee, group_id, file_type  FROM rec_file WHERE file_time>=%u AND file_time<=%u %s ORDER BY file_time",
                ulStartTime, ulEndTime, acCallType);
    }

    iRow = m_pPDBApp->Query(acSQL, Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query rec_file failed");
        return FALSE;
    }

    memset(&stRec, 0 ,sizeof(stRec));
    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        ///sprintf(stRec.acFile+strlen(stRec.acFile), ".%s", acFileType);
        ConvertRecFile(stRec, Param);
        vRec.Add(Param);
        memset(&stRec, 0 ,sizeof(stRec));
        acFileType[0] = '\0';

        if (vRec.GetStringSize() > 60*1024)
        {
            break;
        }
    }

    m_pPDBApp->CloseQuery();

    return PDB_ERROR != enRet;
}

BOOL CfgDao::GetP2PRecFile(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulCallType, UINT32 ulCaller, UINT32 ulCallee, GJsonTupleParam &vRec)
{
    INT32               iRow = 0;
    PDBRET              enRet = PDB_CONTINUE;
    PDBRsltParam        Params;
    REC_FILE_INFO_T     stRec = {0};
    CHAR                acSQL[1024];
    CHAR                acCallType[128];
    CHAR                acUser[128];
    GJsonParam          Param;

    vRec.Clear();

    Params.Bind(stRec.acFile, sizeof(stRec.acFile));
    Params.Bind(&stRec.ulTime);
    Params.Bind(&stRec.dDuration);
    Params.Bind(&stRec.ulFileSize);
    Params.Bind(&stRec.ulCallType);
    Params.Bind(&stRec.ulCaller);
    Params.Bind(&stRec.ulCallee);
    Params.Bind(stRec.acFileSuffix, sizeof(stRec.acFileSuffix));
//  Params.Bind(&stRec.i64GroupID);

    if (ulCallType == REC_CALL_TYPE_ALL)
    {
        sprintf(acCallType, " AND call_type>0"); // SQL语句中Where条件包含time和call_type，保证采用某个索引
    }
    else
    {
        sprintf(acCallType, " AND call_type=%u", ulCallType);
    }

    if (ulCaller == INVALID_USER_ID)
    {
        if (ulCallee == INVALID_USER_ID)
        {
            return GetRecFile(ulStartTime, ulEndTime, ulCallType, vRec);
        }
        else
        {
            sprintf(acUser, " AND callee=%u", ulCallee);
        }
    }
    else
    {
        sprintf(acUser, " AND caller=%u ", ulCaller);
    }

    if (ulStartTime == 0)
    {
        sprintf(acSQL, "SELECT file_name, file_time, duration, file_size, call_type, caller, callee, file_type FROM rec_file WHERE file_time<=%u %s %s ORDER BY file_time DESC",
                ulEndTime, acCallType, acUser);
    }
    else
    {
        sprintf(acSQL, "SELECT file_name, file_time, duration, file_size, call_type, caller, callee, file_type FROM rec_file WHERE file_time>=%u AND file_time<=%u %s %s ORDER BY file_time",
                ulStartTime, ulEndTime, acCallType, acUser);
    }

    iRow = m_pPDBApp->Query(acSQL, Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query rec_file failed");
        return FALSE;
    }

    memset(&stRec, 0 ,sizeof(stRec));
    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        ConvertRecFile(stRec, Param);
        vRec.Add(Param);
        memset(&stRec, 0 ,sizeof(stRec));

        if (vRec.GetStringSize() > 60*1024)
        {
            break;
        }
    }

    m_pPDBApp->CloseQuery();

    return PDB_ERROR != enRet;
}

BOOL CfgDao::GetRealBrdRecFile(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulCallType, INT64 DCUser, GJsonTupleParam &vRec)
{
    INT32               iRow = 0;
    PDBRET              enRet = PDB_CONTINUE;
    PDBRsltParam        Params;
    REC_FILE_INFO_T     stRec = {0};
    CHAR                acSQL[1024];
    CHAR                acCallType[128];
    CHAR                acUser[128];
    GJsonParam          Param;

    vRec.Clear();

    Params.Bind(stRec.acFile, sizeof(stRec.acFile));
    Params.Bind(&stRec.ulTime);
    Params.Bind(&stRec.dDuration);
    Params.Bind(&stRec.ulFileSize);
    Params.Bind(&stRec.ulCallType);
    Params.Bind(&stRec.i64GroupID);

    Params.Bind(&stRec.ulDCUserID);
    Params.Bind(&stRec.ulTranUnitID);
    Params.Bind(stRec.acFileSuffix, sizeof(stRec.acFileSuffix));

    if (ulCallType == REC_CALL_TYPE_ALL)
    {
        sprintf(acCallType, " AND call_type>0"); // SQL语句中Where条件包含time和call_type，保证采用某个索引
    }
    else
    {
        sprintf(acCallType, " AND call_type=%u", ulCallType);
    }

    if (DCUser == INVALID_USER_ID)
    {
        return GetRecFile(ulStartTime, ulEndTime, ulCallType, vRec);
    }
    else
    {
        sprintf(acUser, " AND dc_user=" FORMAT_I64 , DCUser);
    }

    if (ulStartTime == 0)
    {
        sprintf(acSQL, "SELECT file_name, file_time, duration, file_size, call_type, group_id , dc_user, train_id, file_type FROM rec_file WHERE file_time<=%u %s %s ORDER BY file_time DESC",
                ulEndTime, acCallType, acUser);
    }
    else
    {
        sprintf(acSQL, "SELECT file_name, file_time, duration, file_size, call_type, group_id , dc_user, train_id, file_type FROM rec_file WHERE file_time>=%u AND file_time<=%u %s %s ORDER BY file_time",
                ulStartTime, ulEndTime, acCallType, acUser);
    }

    iRow = m_pPDBApp->Query(acSQL, Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query rec_file failed");
        return FALSE;
    }

    memset(&stRec, 0 ,sizeof(stRec));
    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        ConvertRecFile(stRec, Param);
        vRec.Add(Param);
        memset(&stRec, 0 ,sizeof(stRec));

        if (vRec.GetStringSize() > 60*1024)
        {
            break;
        }
    }

    m_pPDBApp->CloseQuery();

    return PDB_ERROR != enRet;
}

BOOL CfgDao::GetIPHRecFile(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulCallType, INT64 TrainID, GJsonTupleParam &vRec)
{
    INT32               iRow = 0;
    PDBRET              enRet = PDB_CONTINUE;
    PDBRsltParam        Params;
    REC_FILE_INFO_T     stRec = {0};
    CHAR                acSQL[1024];
    CHAR                acCallType[128];
    CHAR                acUser[128];
    GJsonParam          Param;

    vRec.Clear();

    Params.Bind(stRec.acFile, sizeof(stRec.acFile));
    Params.Bind(&stRec.ulTime);
    Params.Bind(&stRec.dDuration);
    Params.Bind(&stRec.ulFileSize);
    Params.Bind(&stRec.ulCallType);
//  Params.Bind(&stRec.ulCaller);
//  Params.Bind(&stRec.ulCallee);
    Params.Bind(&stRec.i64GroupID);


    Params.Bind(&stRec.ulDCUserID);
    Params.Bind(&stRec.ulTranUnitID);
    Params.Bind(stRec.acFileSuffix, sizeof(stRec.acFileSuffix));

    if (ulCallType == REC_CALL_TYPE_ALL)
    {
        sprintf(acCallType, " AND call_type>0"); // SQL语句中Where条件包含time和call_type，保证采用某个索引
    }
    else
    {
        sprintf(acCallType, " AND call_type=%u", ulCallType);
    }

    if (TrainID == INVALID_TRAIN_UNIT_ID)
    {
        return GetRecFile(ulStartTime, ulEndTime, ulCallType, vRec);
    }
    else
    {
        sprintf(acUser, " AND train_id=" FORMAT_I64 , TrainID);
    }

    if (ulStartTime == 0)
    {
        sprintf(acSQL, "SELECT file_name, file_time, duration, file_size, call_type, group_id , dc_user, train_id, file_type FROM rec_file WHERE file_time<=%u %s %s ORDER BY file_time DESC",
                ulEndTime, acCallType, acUser);
    }
    else
    {
        sprintf(acSQL, "SELECT file_name, file_time, duration, file_size, call_type, group_id , dc_user, train_id, file_type FROM rec_file WHERE file_time>=%u AND file_time<=%u %s %s ORDER BY file_time",
                ulStartTime, ulEndTime, acCallType, acUser);
    }

    iRow = m_pPDBApp->Query(acSQL, Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query rec_file failed");
        return FALSE;
    }

    memset(&stRec, 0 ,sizeof(stRec));
    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        ConvertRecFile(stRec, Param);
        vRec.Add(Param);
        memset(&stRec, 0 ,sizeof(stRec));

        if (vRec.GetStringSize() > 60*1024)
        {
            break;
        }
    }

    m_pPDBApp->CloseQuery();

    return PDB_ERROR != enRet;
}

BOOL CfgDao::GetPTTRecFile(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulCallType, INT64 i64GroupID, GJsonTupleParam &vRec)//VECTOR<REC_INFO_T> &vRec)
{
    INT32               iRow = 0;
    PDBRET              enRet = PDB_CONTINUE;
    PDBRsltParam        Params;
    REC_FILE_INFO_T     stRec = {0};
    CHAR                acSQL[1024];
    CHAR                acCallType[128];
    CHAR                acUser[128];
    GJsonParam          Param;

    vRec.Clear();

    Params.Bind(stRec.acFile, sizeof(stRec.acFile));
    Params.Bind(&stRec.ulTime);
    Params.Bind(&stRec.dDuration);
    Params.Bind(&stRec.ulFileSize);
    Params.Bind(&stRec.ulCallType);
//  Params.Bind(&stRec.ulCaller);
//  Params.Bind(&stRec.ulCallee);
    Params.Bind(&stRec.i64GroupID);
    Params.Bind(stRec.acFileSuffix, sizeof(stRec.acFileSuffix));

    if (ulCallType == REC_CALL_TYPE_ALL)
    {
        sprintf(acCallType, " AND call_type>0"); // SQL语句中Where条件包含time和call_type，保证采用某个索引
    }
    else
    {
        sprintf(acCallType, " AND call_type=%u", ulCallType);
    }

    if (i64GroupID == INVALID_GROUP_ID)
    {
        return GetRecFile(ulStartTime, ulEndTime, ulCallType, vRec);
    }
    else
    {
        sprintf(acUser, " AND group_id=" FORMAT_I64 , i64GroupID);
    }

    if (ulStartTime == 0)
    {
        sprintf(acSQL, "SELECT file_name, file_time, duration, file_size, call_type, group_id, file_type FROM rec_file WHERE file_time<=%u %s %s ORDER BY file_time DESC",
                ulEndTime, acCallType, acUser);
    }
    else
    {
        sprintf(acSQL, "SELECT file_name, file_time, duration, file_size, call_type, group_id, file_type FROM rec_file WHERE file_time>=%u AND file_time<=%u %s %s ORDER BY file_time",
                ulStartTime, ulEndTime, acCallType, acUser);
    }

    iRow = m_pPDBApp->Query(acSQL, Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query rec_file failed");
        return FALSE;
    }

    memset(&stRec, 0 ,sizeof(stRec));
    FOR_ROW_FETCH(enRet, m_pPDBApp)
    {
        ConvertRecFile(stRec, Param);
        vRec.Add(Param);
        memset(&stRec, 0 ,sizeof(stRec));

        if (vRec.GetStringSize() > 60*1024)
        {
            break;
        }
    }

    m_pPDBApp->CloseQuery();

    return PDB_ERROR != enRet;
}

// operator
BOOL CfgDao::GetOperator(VECTOR<REC_OPER_INFO_T> &vInfo)
{
    INT32                   iRow = 0;
    PDBRET                  enRet = PDB_CONTINUE;
    PDBRsltParam            Params;
    REC_OPER_INFO_T         stInfo = {0};

    vInfo.clear();

    Params.Bind(stInfo.acOperatorID, sizeof(stInfo.acOperatorID));
    Params.Bind(stInfo.acName, sizeof(stInfo.acName));
    Params.Bind(stInfo.acPassword, sizeof(stInfo.acPassword));
    Params.Bind(&stInfo.ulOperType);

    iRow = m_pPDBApp->Query("SELECT oper_id, name, pwd, type FROM oper", Params);

    if (INVALID_ROWCOUNT == iRow)
    {
        DaoLog(LOG_ERROR, "query oper failed");
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

BOOL CfgDao::AddOperator(REC_OPER_INFO_T &stRec)
{
    return SetOperator(stRec);
}

BOOL CfgDao::DelOperator(CHAR *szOperID)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.BindString(szOperID);

    iAffectedRow = m_pPDBApp->Update("DELETE FROM oper WHERE oper_id=?", Params);

    if (INVALID_ROWCOUNT == iAffectedRow)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CfgDao::SetOperatorPwd(CHAR *szOperID, CHAR *szOldPwd, CHAR *szNewPwd)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.BindString(szNewPwd);
    Params.BindString(szOperID);
    Params.BindString(szOldPwd);

    iAffectedRow = m_pPDBApp->Update("UPDATE oper SET pwd=? WHERE oper_id=? AND pwd=?", Params);

    return iAffectedRow > 0;
}

BOOL CfgDao::SetOperator(REC_OPER_INFO_T &stRec)
{
    INT32           iAffectedRow = 0;
    PDBParam        Params;

    Params.Bind(stRec.acOperatorID, sizeof(stRec.acOperatorID));
    Params.Bind(stRec.acName, sizeof(stRec.acName));
    Params.Bind(stRec.acPassword, sizeof(stRec.acPassword));
    Params.Bind(&stRec.ulOperType);

    Params.Bind(stRec.acName, sizeof(stRec.acName));
    Params.Bind(stRec.acPassword, sizeof(stRec.acPassword));
    Params.Bind(&stRec.ulOperType);

    iAffectedRow = m_pPDBApp->Update("INSERT INTO oper(oper_id, name, pwd, type) "
        "VALUES(?,?,?,?) ON DUPLICATE KEY UPDATE name=?, pwd=?, type=?", Params);

    return iAffectedRow > 0;
}
