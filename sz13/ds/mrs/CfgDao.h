#ifndef CFG_DAO_H
#define CFG_DAO_H

#define SYNC_ADD        1
#define SYNC_DEL        (-1)
#define SYNC_NULL       0

#define DaoLog(ulLogLevel, szFormat, ...)      { GosLog("DAO", ulLogLevel, szFormat, ##__VA_ARGS__); }

class CfgDao
{
public :
    CfgDao();
    ~CfgDao();

    BOOL TransBegin();
    BOOL TransRollback();
    BOOL TransCommit();

    BOOL Init(PDBEnvBase *pPDBEnv);

    BOOL CheckConn();

    BOOL Update(const CHAR *szSQL);
    BOOL AddOperLog(UINT32 ulTime, CHAR *szOperatorID, CHAR *szLogInfo);

    BOOL AddRecFile(REC_FILE_INFO_T &stRec);
    BOOL UpdateRecFile(CHAR *szRecFile, DOUBLE dDuration, UINT32 ulFileSize, REC_FILE_INFO_T &stInfo);
    //BOOL UpdateRecFile(CHAR *szRecFile, DOUBLE dDuration, UINT32 ulFileSize);
    //BOOL UpdateRecFile(CHAR *szRawRecFile, CHAR *szNewRecFile, DOUBLE dDuration, UINT32 ulFileSize);
    BOOL UpdateRecFile(CHAR *szRawRecFile, CHAR *szNewRecFile, DOUBLE dDuration, UINT32 ulFileSize, REC_FILE_INFO_T &stInfo);
    BOOL ClearRecFile(UINT32 ulDeadTime);

    BOOL GetRecFile(UINT32 ulDeadTime, VECTOR<std::string> &vInfo);
    BOOL GetRecFile(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulCallType, GJsonTupleParam &vRec);
    BOOL GetP2PRecFile(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulCallType, UINT32 ulCaller, UINT32 ulCallee, GJsonTupleParam &vInfo);
    BOOL GetPTTRecFile(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulCallType, INT64 i64GroupID, GJsonTupleParam &vRec);
    BOOL GetIPHRecFile(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulCallType, INT64 TrainID, GJsonTupleParam &vRec);
    BOOL GetRealBrdRecFile(UINT32 ulStartTime, UINT32 ulEndTime, UINT32 ulCallType, INT64 DCUser, GJsonTupleParam &vRec);

    BOOL GetOperator(VECTOR<REC_OPER_INFO_T> &vInfo);
    BOOL AddOperator(REC_OPER_INFO_T &stInfo);
    BOOL SetOperator(REC_OPER_INFO_T &stInfo);
    BOOL DelOperator(CHAR *szOperID);
    BOOL SetOperatorPwd(CHAR *szOperID, CHAR *szOldPwd, CHAR *szNewPwd);

private:
    PDBApp  *m_pPDBApp;
};

extern CfgDao* GetCfgDao();
extern CfgDao* GetCfgDao(PDBEnvBase *pPDBEnv);

#endif
