#include "RecPublic.h"
#include "CfgDao.h"
//#include "ats.h"
#include "TaskCfg.h"
#include "GTransFile.h"
#include "rec_util.h"

#define CHECK_TIMER         TIMER10
#define EV_CHECK_TIMER      GET_TIMER_EV(CHECK_TIMER)

extern PLAT_CFG_T       g_stLocalCfg;
extern MRS_NTP_CFG      g_stNTPCfg;
extern INT32            g_iAutoCloseHour;

//extern UINT32          g_usTransFilePort;

static const UINT32 INVAILD_TRAIN_ID = 0;

extern BOOL IsLocalAddr(UINT8 *pucAddr);
extern DOUBLE GetMediaFileDuration(CHAR *szFile);

TaskCfg::TaskCfg(UINT16 usDispatchID):GBaseTask(MODULE_CFG, "CFG", usDispatchID)
{
    m_bInitOperatro = FALSE;
    m_acOperatorID[0] = '\0';

    m_pDao = NULL;

    RegisterMsg(EV_CLIENT_REQ);
}

VOID TaskCfg::AddOperLog(const CHAR *szLog, ...)
{
    if (!m_pDao)
    {
        return;
    }

    CHAR            acLog[4096];
    va_list         vaLog;

    va_start(vaLog, szLog);

    memset(acLog, 0, sizeof(acLog));
    vsnprintf(acLog, sizeof(acLog)-1, szLog, vaLog);

    va_end(vaLog);

    m_pDao->AddOperLog(gos_get_current_time(), m_acOperatorID, acLog);
}

BOOL TaskCfg::Init()
{
    if (!m_pDao)
    {
        m_pDao = GetCfgDao();
        if (!m_pDao)
        {
            TaskLog(LOG_ERROR, "get db dao failed");

            return FALSE;
        }
    }

    if (!m_bInitOperatro)
    {
        VECTOR<REC_OPER_INFO_T>     vInfo;

        m_bInitOperatro = m_pDao->GetOperator(vInfo);
        if (!m_bInitOperatro)
        {
            TaskLog(LOG_ERROR, "get operator failed");
            return FALSE;
        }

        InitOperatorInfo(vInfo);
    }

    SetLoopTimer(CHECK_TIMER, 10*1000);
    SetTaskStatus(TASK_STATUS_WORK);

    TaskLog(LOG_INFO, "Task Cfg init successful!");

    return TRUE;
}

VOID TaskCfg::OnServerDisconnectClient(UINT16 usClientID)
{
}

VOID TaskCfg::OnClientReq(CHAR *szReq, UINT32 ulMsgLen)
{
    GJson       Json;
    GJson       SubJson;
    BOOL        bRet = Json.Parse(szReq);
    CHAR        *szMsgType;
    CHAR        *szMsgInfo;
    UINT32      ulSeqID;
    //UINT32        ulDCUserID;
    CHAR        *szOperatorID;

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "invalid req msg: %s", szReq);
        goto error;
    }

    szMsgType = Json.GetValue("MsgName");

    if (!szMsgType)
    {
        TaskLog(LOG_ERROR, "get MsgName failed: %s", szReq);
        goto error;
    }

    if (!Json.GetValue("MsgSeqID", &ulSeqID))
    {
        TaskLog(LOG_ERROR, "get MsgSeqID failed: %s", szReq);
        goto error;
    }

    szMsgInfo = Json.GetValue("MsgInfo");
    if (!szMsgInfo)
    {
        TaskLog(LOG_ERROR, "get MsgInfo failed: %s", szReq);
        goto error;
    }

    if (!SubJson.Parse(szMsgInfo))
    {
        TaskLog(LOG_ERROR, "parse MsgInfo failed: %s", szReq);
        goto error;
    }

    memset(m_acOperatorID, 0, sizeof(m_acOperatorID));
    szOperatorID = Json.GetValue("OperatorID");
    if (szOperatorID)
    {
        strncpy(m_acOperatorID, szOperatorID, sizeof(m_acOperatorID)-1);
    }

    TaskLog(LOG_INFO, "get client msg: %s", szReq);

    if (strcmp(szMsgType, MSG_REC_Login) == 0)
    {
        OnLogin(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_REC_Logout) == 0)
    {
        OnLogout(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_REC_GetOperator) == 0)
    {
        OnGetOperatorReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_REC_AddOperator) == 0)
    {
        OnAddOperatorReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_REC_DelOperator) == 0)
    {
        OnDelOperatorReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_REC_SetOperator) == 0)
    {
        OnSetOperatorReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_REC_SetPwd) == 0)
    {
        OnSetPwdReq(szReq, ulSeqID, SubJson);
    }
    /*else if (strcmp(szMsgType, MSG_DCSaveGroupStatus) == 0)
    {
        OnDCSaveXXXReq(szReq, szMsgType, ulSeqID, SubJson);
    }*/
    else if (strcmp(szMsgType, MSG_REC_GetRecFileList) == 0)
    {
        OnGetRecFileListReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_REC_DCUpFileReq) == 0)
    {
        OnDcUpFileReq(szReq, ulSeqID, SubJson);
    }
    else if (strcmp(szMsgType, MSG_REC_LoadNtpCfg))
    {
        OnSPTLoadNTPCfgReq(szReq, ulSeqID, SubJson);
    }
    else
    {
        TaskLog(LOG_DETAIL, "invalid client msg: %s", szReq);
        goto error;
    }

    return;

error:
    return;
}

VOID TaskCfg::OnSPTLoadNTPCfgReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonParam    JsonParam;

    JsonParam.AddIP("NTPAddr", g_stNTPCfg.aucNTPAddr);
    JsonParam.Add("NTPPort", g_stNTPCfg.usNTPPort);
    JsonParam.Add("NTPSyncPeriod", g_stNTPCfg.ulNTPSyncPeriod);

    SendRsp(MSG_REC_LoadNtpCfg, ulSeqID, JsonParam);
}

VOID TaskCfg::OnAsyncCallTest(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonTupleParam     JsonTupleParam;
    GJsonParam          RspParam;
    UINT32              ulDCUser = 0;
    UINT32              ulTrainUnitID = 0;

    if (!Json.GetValue("DCUserID", &ulDCUser))
    {
        TaskLog(LOG_ERROR, "REC Test Fail in msg %s", szReq);
        return;
    }

    TaskLog(LOG_ERROR, "REC Test Recv DCUserID is  %u", ulDCUser);

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "REC Test Fail in msg %s", szReq);
        return;
    }

    TaskLog(LOG_ERROR, "REC Test Recv TrainUnitID is  %u", ulTrainUnitID);

    RspParam.Add("Result", "Succ");

   // SendClient(GetMsgSender(), EV_CLIENT_RSP, MSG_REC_TEST, ulSeqID, RspParam);
}

VOID TaskCfg::OnRECTest(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonTupleParam     JsonTupleParam;
    GJsonParam          RspParam;
    UINT32              ulDCUser = 0;
    UINT32              ulTrainUnitID = 0;

    if (!Json.GetValue("DCUserID", &ulDCUser))
    {
        TaskLog(LOG_ERROR, "REC Test Fail in msg %s", szReq);
        return;
    }

    TaskLog(LOG_ERROR, "REC Test Recv DCUserID is  %u", ulDCUser);

    if (!Json.GetValue("TrainUnitID", &ulTrainUnitID))
    {
        TaskLog(LOG_ERROR, "REC Test Fail in msg %s", szReq);
        return;
    }

    TaskLog(LOG_ERROR, "REC Test Recv TrainUnitID is  %u", ulTrainUnitID);

    RspParam.Add("Result", "Succ");

//    SendRsp(MSG_REC_TEST, ulSeqID, RspParam);
}

static UINT32 GetFileTime(CHAR *acFile, GOS_DATETIME_T &stTime)
{
    CHAR        *szTmp;
    CHAR        acTime[32];
    // 获取时间
    szTmp = gos_right_strchr(acFile, '_');
    if (!szTmp)
    {
        return 0;
    }

    szTmp--;
    for (UINT32 i=0; *szTmp!='_' && szTmp!=acFile; i++)
    {
        szTmp--;
    }

    // _20200410184058277  or _20200410_184058
    UINT32 i=0;
    szTmp++;
    for ( ; *szTmp!='.'; i++)
    {
        acTime[i] = *szTmp;
        szTmp++;
    }

    acTime[i] = '\0';

    szTmp = gos_right_strchr(acTime, '_');

    CHAR *szTemp = szTmp++;

    strcpy(szTemp, szTmp);

    if (!gos_parse_compact_time(acTime, &stTime))
    {
        return 0xffffffff;
    }

    return gos_mktime(&stTime);
}

static UINT32 GetFileTime(CHAR *acFile)
{
    CHAR        *szTmp;
    CHAR        acTime[32];

    // 获取时间
    szTmp = gos_right_strchr(acFile, '_');
    if (!szTmp)
    {
        return 0;
    }

    szTmp--;
    for (UINT32 i=0; *szTmp!='_' && szTmp!=acFile; i++)
    {
        szTmp--;
    }

    // _20200410184058277  or _20200410_184058
    UINT32 i=0;
    szTmp++;
    for ( ; *szTmp!='.'; i++)
    {
        acTime[i] = *szTmp;
        szTmp++;
    }

    acTime[i] = '\0';

    szTmp = gos_right_strchr(acTime, '_');

    CHAR *szTemp = szTmp++;

    strcpy(szTemp, szTmp);

    return gos_compact_text_time_to_int(acTime);
}

static bool ParseRecFile(char *szFile, REC_FILE_INFO_T &stRec)
{
    CHAR        *szTmp;
    CHAR        acFile[256];
    CHAR        acDcUser[32];
    CHAR        acTrainUnitID[32];
    DOUBLE      dDuration;

    strcpy(stRec.acFile, szFile);

    stRec.i64GroupID = 0;
    dDuration = GetMediaFileDuration(szFile);

    stRec.dDuration =  dDuration>0 ? dDuration : 0;

    szTmp = gos_right_strchr(szFile, '\\');
    if (!szTmp)
    {
        return false;
    }

    strcpy(acFile, szTmp+1);

    //获取DCuserID和TrainUnitID
    szTmp++;
    memset(acDcUser, 0, sizeof(acDcUser));

    for (UINT32 i=0; *szTmp != '_'; i++)
    {
        acDcUser[i] = *szTmp;
        szTmp++;
    }

    gos_atou(acDcUser, &stRec.ulDCUserID);

    if (stRec.ulCallType == REC_CALL_TYPE_IPH)
    {
        szTmp++;
        memset(acTrainUnitID, 0, sizeof(acTrainUnitID));

        for (UINT32 i=0; *szTmp != '_'; i++)
        {
            acTrainUnitID[i] = *szTmp;
            szTmp++;
        }

        gos_atou(acTrainUnitID, &stRec.ulTranUnitID);
        gos_atou(acDcUser, &stRec.ulCallee); //被叫
        gos_atou(acTrainUnitID, &stRec.ulCaller); //主叫

    }
    else
    {
        stRec.ulTranUnitID = INVALID_TRAIN_UNIT_ID;
    }

//    *szTmp = '\0';

    //获取时间
    stRec.ulTime = GetFileTime(acFile);
    if (stRec.ulTime == 0xffffffff)
    {
        return false;
    }

    return true;
}

static BOOL GetRecFileSavePath(CHAR *szSimpleFile, CHAR *acDestDir, UINT32 ulCallType)
{
    CHAR          acFileMonth[32];
    CHAR          acFileDay[32];

    GOS_DATETIME_T  stFileTime;

    GetFileTime(szSimpleFile, stFileTime);

    sprintf(acFileMonth, "%04u%02u", stFileTime.usYear, stFileTime.ucMonth);
    sprintf(acFileDay, "%04u%02u%02u", stFileTime.usYear, stFileTime.ucMonth, stFileTime.ucDay);

    if (ulCallType == REC_CALL_TYPE_REAL_BRD)
    {
        sprintf(acDestDir, "%s/%s/%s/%s", g_stIniCfg.acRecRootPath, "PA_rec", acFileMonth, acFileDay);
    }
    else if (ulCallType == REC_CALL_TYPE_IPH)
    {
        sprintf(acDestDir, "%s/%s/%s/%s", g_stIniCfg.acRecRootPath, "IPH_rec", acFileMonth, acFileDay);
    }
    else
    {
        sprintf(acDestDir, "%s/%s/%s/%s", g_stIniCfg.acRecRootPath, "Other_rec", acFileMonth, acFileDay);
    }

    if (!gos_file_exist(acDestDir))
    {
        if (!gos_create_redir(acDestDir))
        {
            GosLog(LOG_ERROR, "create dir %s failed!", acDestDir);

            return FALSE;
        }
    }

    return TRUE;
}

/**
 * @brief           处理DC文件上传请求
 * @param           szReq     [in] DC发来的原始数据
 * @param           ulSeqID   [in] 消息序列号
 * @param           Json      [in]  原始消息中MsgInfo子序列
 * @return          VOID
 * @author          zhengwenjie(zhengwenjie@gbcom.com.cn)
 * @par 示例:
 * @code
 *
 * @endcode
 * @note
 * 1、文件保存时带路径
 * 2、文件下载到服务器本机后需要同步更新到数据库，两者最终效果为一次原子操作
 */
VOID TaskCfg::OnDcUpFileReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    CHAR          *szFileName;
    UINT32        ulFileSize;
    UINT32        ulFileType;
    CHAR          acDestDir[256];
    CHAR          acDestFile[256];
    UINT32        ulCallType;  //呼叫类型
    UINT16        usServerPort = g_stIniCfg.usDCTransServerPort; //对端文件服务器端口  TODO：可配置
    UINT8         aucServerAddr[4] = {0,0,0,0};
    GJsonParam    Param;

    REC_FILE_INFO_T     stRec;

    szFileName = Json.GetValue("FileName");

    if (!szFileName)
    {
        TaskLog(LOG_ERROR, "invalid FileName in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("FileSize", &ulFileSize))
    {
        TaskLog(LOG_ERROR, "invalid FileSize in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("FileType", &ulFileType))
    {
        TaskLog(LOG_ERROR, "invalid FileType in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("CallType", &ulCallType))
    {
        TaskLog(LOG_ERROR, "invalid CallType in msg %s", szReq);
        return;
    }

    if (!Json.GetIP("DCAddr", aucServerAddr))
    {
        TaskLog(LOG_ERROR, "invalid DCAddr in msg %s", szReq);
        return;
    }

    TaskLog(LOG_INFO, "FileName:%s FileSize:%u FileType:%u  ", szFileName, ulFileSize, ulFileType);

    CHAR        *szSimpleFileName;

    szSimpleFileName = gos_get_simple_file_name(szFileName);
    if (!szSimpleFileName)
    {
        return ;
    }

    if (!GetRecFileSavePath(szSimpleFileName, acDestDir, ulCallType))
    {
        return ;
    }
    sprintf(acDestFile, "%s/%s", acDestDir, szSimpleFileName);
    gos_format_path(acDestDir);
    gos_format_path(acDestFile);

    if (!TransFile(aucServerAddr, usServerPort, szFileName, acDestFile))
    {
        Param.Add("Result", "Fail");
        Param.Add("Cause", "Failed to download recfile");
    }
    else
    {
        memset(&stRec, 0, sizeof(REC_FILE_INFO_T));

        stRec.ulCallType = ulCallType;
        stRec.ulFileSize = ulFileSize;
        stRec.ulFileType = ulFileType;
        memcpy(stRec.acFile, szFileName, sizeof(stRec.acFile));

        ParseRecFile(acDestFile, stRec);
        if(!m_pDao->AddRecFile(stRec))
        {
            Param.Add("Result", "Fail");
            Param.Add("Cause", "Failed to write to database");

            gos_delete_file(acDestFile);
        }
        else
        {
            Param.Add("Result", "Succ");
        }
    }

    SendRsp(MSG_REC_DCUpFileReq, ulSeqID, Param);
}

VOID TaskCfg::OnGetRecFileListReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonTupleParam     JsonTupleParam;
    UINT32              ulStartTime = 0;
    UINT32              ulEndTime   = 0xffffffff;
    UINT32              ulCallType  = REC_CALL_TYPE_ALL;
    UINT32              ulCaller    = INVALID_USER_ID;
    UINT32              ulCallee    = INVALID_USER_ID;
    INT64               i64GroupID  = INVALID_GROUP_ID;
    BOOL                bRet;

    if (!Json.GetValue("StartTime", &ulStartTime))
    {
        TaskLog(LOG_ERROR, "invalid StartTime in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("EndTime", &ulEndTime))
    {
        TaskLog(LOG_ERROR, "invalid EndTime in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("CallType", &ulCallType))
    {
        TaskLog(LOG_ERROR, "invalid CallType in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Caller", &ulCaller))
    {
        TaskLog(LOG_ERROR, "invalid Caller in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Callee", &ulCallee))
    {
        TaskLog(LOG_ERROR, "invalid Callee in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("GroupID", &i64GroupID))
    {
        TaskLog(LOG_ERROR, "invalid GroupID in msg %s", szReq);
        return;
    }

    if (ulCallType == REC_CALL_TYPE_P2P_AUDIO ||
        ulCallType == REC_CALL_TYPE_P2P_VIDEO )
    {
        bRet = m_pDao->GetP2PRecFile(ulStartTime, ulEndTime, ulCallType, ulCaller, ulCallee, JsonTupleParam);
    }
    else if (ulCallType == REC_CALL_TYPE_PTT_AUDIO ||
             ulCallType == REC_CALL_TYPE_PTT_VIDEO )
    {
        bRet = m_pDao->GetPTTRecFile(ulStartTime, ulEndTime, ulCallType, i64GroupID, JsonTupleParam);
    }
    else if (ulCallType == REC_CALL_TYPE_IPH)
    {
        bRet = m_pDao->GetIPHRecFile(ulStartTime, ulEndTime, ulCallType, ulCaller, JsonTupleParam);
    }
    else if(ulCallType == REC_CALL_TYPE_REAL_BRD)
    {
        bRet = m_pDao->GetRealBrdRecFile(ulStartTime, ulEndTime, ulCallType, ulCaller, JsonTupleParam);
    }
    else
    {
        bRet = m_pDao->GetRecFile(ulStartTime, ulEndTime, ulCallType, JsonTupleParam);
    }

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "load rec file failed");
        return;
    }

    SendRsp(MSG_REC_GetRecFileList, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnSaveXXXReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
/*  UINT32  ulDCUserID;
    INT64   i64GroupID;
    UINT32  ulGroupStatus;

    if (!Json.GetValue("DCUserID", &ulDCUserID))
    {
        TaskLog(LOG_ERROR, "invalid DCUserID in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("GroupID", &i64GroupID))
    {
        TaskLog(LOG_ERROR, "invalid GroupID in msg %s", szMsgType);
        return;
    }

    if (!Json.GetValue("GroupStatus", &ulGroupStatus))
    {
        TaskLog(LOG_ERROR, "invalid GroupStatus in msg %s", szMsgType);
        return;
    }

    if (!m_pDao->SaveGroupStatus(ulDCUserID, i64GroupID, ulGroupStatus))
    {
        TaskLog(LOG_ERROR, "save status of group " FORMAT_I64 " failed", i64GroupID);
        return;
    }

    GJsonParam  Param;

    Param.Add("DCUserID", ulDCUserID);
    Param.Add("GroupID", i64GroupID);
    Param.Add("GroupStatus", ulGroupStatus);
    Param.Add("Result", "true");

    SendRsp(szMsgType, ulSeqID, Param); */
}

VOID TaskCfg::SendConfirmSaveRsp(UINT32 ulSeqID)
{
    GJsonParam  Param;

    Param.Add("Result", "true");
//  Param.Add("UserID", m_ulDCUserID);

    SendClient(GetMsgSender(), EV_CLIENT_RSP, MSG_REC_ConfirmSave, ulSeqID, Param);
}

VOID TaskCfg::OnLogin(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    REC_OPER_INFO_T     stUser = {0};
    GJsonParam          JsonParam;
    CHAR                acErrInfo[256];

    if (!Json.GetValue("User", stUser.acOperatorID, sizeof(stUser.acOperatorID)))
    {
        TaskLog(LOG_ERROR, "invalid User in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Pwd", stUser.acPassword, sizeof(stUser.acPassword)))
    {
        TaskLog(LOG_ERROR, "invalid Pwd in msg %s", szReq);
        return;
    }

    UINT32  ulOperType;
    BOOL    bRet = CheckOperator(stUser.acOperatorID, stUser.acPassword, ulOperType, acErrInfo);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "user %s login failed", stUser.acOperatorID);

        JsonParam.Add("Result", "Fail");
        JsonParam.Add("ErrInfo", acErrInfo);
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        JsonParam.Add("OperType", ulOperType);

        strcpy(m_acOperatorID, stUser.acOperatorID);
        AddOperLog("用户登录");

        TaskLog(LOG_INFO, "user %s login succ", stUser.acOperatorID);
    }

    SendRsp(MSG_REC_Login, ulSeqID, JsonParam);
}

VOID TaskCfg::OnLogout(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    CHAR            *szOperatorID;
    GJsonParam      JsonParam;

    szOperatorID = Json.GetValue("User");
    if (!szOperatorID || *szOperatorID == '\0')
    {
        TaskLog(LOG_ERROR, "invalid User in msg %s", szReq);
        return;
    }

    TaskLog(LOG_INFO, "user %s logout", szOperatorID);

    JsonParam.Add("Result", "Succ");

    AddOperLog("用户退出");
}

VOID TaskCfg::OnGetOperatorReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    GJsonTupleParam     JsonTupleParam;
    GJsonParam          JsonParam;
    VECTOR<REC_OPER_INFO_T>     vInfo;

    GetOperatorInfo(vInfo);

    for (UINT32 i=0; i<vInfo.size(); i++)
    {
        JsonParam.Clear();
        JsonParam.Add("Operator", vInfo[i].acOperatorID);
        JsonParam.Add("acName", vInfo[i].acName);
        JsonParam.Add("OperType", vInfo[i].ulOperType);

        JsonTupleParam.Add(JsonParam);
    }

    SendRsp(MSG_REC_GetOperator, ulSeqID, JsonTupleParam);
}

VOID TaskCfg::OnAddOperatorReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    REC_OPER_INFO_T     stInfo = {0};
    GJsonParam          JsonParam;

    if (!Json.GetValue("OperatorID", stInfo.acOperatorID, sizeof(stInfo.acOperatorID)))
    {
        TaskLog(LOG_ERROR, "invalid OperatorID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Name", stInfo.acName, sizeof(stInfo.acName)))
    {
        TaskLog(LOG_ERROR, "invalid Name in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Password", stInfo.acPassword, sizeof(stInfo.acPassword)))
    {
        TaskLog(LOG_ERROR, "invalid Password in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Type", &stInfo.ulOperType))
    {
        TaskLog(LOG_ERROR, "invalid Type in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->AddOperator(stInfo);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "add operator(%s) to db failed", stInfo.acOperatorID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        AddOperatorInfo(stInfo);

        AddOperLog("创建用户%s", stInfo.acOperatorID);
    }

    SendRsp(MSG_REC_AddOperator, ulSeqID, JsonParam);
}

VOID TaskCfg::OnDelOperatorReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    CHAR            *szOperatorID;
    GJsonParam      JsonParam;

    szOperatorID = Json.GetValue("OperatorID");
    if (!szOperatorID || szOperatorID[0] == '\0')
    {
        TaskLog(LOG_ERROR, "invalid OperatorID in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->DelOperator(szOperatorID);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "delete operator(%s) from db failed", szOperatorID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        DelOperatorInfo(szOperatorID);

        AddOperLog("删除用户%s", szOperatorID);
    }

    SendRsp(MSG_REC_DelOperator, ulSeqID, JsonParam);
}

VOID TaskCfg::OnSetOperatorReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    REC_OPER_INFO_T         stInfo = {0};
    GJsonParam              JsonParam;

    if (!Json.GetValue("OperatorID", stInfo.acOperatorID, sizeof(stInfo.acOperatorID)))
    {
        TaskLog(LOG_ERROR, "invalid OperatorID in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Name", stInfo.acName, sizeof(stInfo.acName)))
    {
        TaskLog(LOG_ERROR, "invalid Name in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Password", stInfo.acPassword, sizeof(stInfo.acPassword)))
    {
        TaskLog(LOG_ERROR, "invalid Password in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("Type", &stInfo.ulOperType))
    {
        TaskLog(LOG_ERROR, "invalid Type in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->SetOperator(stInfo);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "set operator(%s) to db failed", stInfo.acOperatorID);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        SetOperatorInfo(stInfo);

        AddOperLog("设置用户%s", stInfo.acOperatorID);
    }

    SendRsp(MSG_REC_SetOperator, ulSeqID, JsonParam);
}

VOID TaskCfg::OnSetPwdReq(CHAR *szReq, UINT32 ulSeqID, GJson &Json)
{
    CHAR                acUser[32];
    CHAR                acOldPwd[32];
    CHAR                acNewPwd[32];
    GJsonParam          JsonParam;

    if (!Json.GetValue("User", acUser, sizeof(acUser)))
    {
        TaskLog(LOG_ERROR, "invalid User in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("OldPwd", acOldPwd, sizeof(acOldPwd)))
    {
        TaskLog(LOG_ERROR, "invalid OldPwd in msg %s", szReq);
        return;
    }

    if (!Json.GetValue("NewPwd", acNewPwd, sizeof(acNewPwd)))
    {
        TaskLog(LOG_ERROR, "invalid NewPwd in msg %s", szReq);
        return;
    }

    BOOL bRet = m_pDao->SetOperatorPwd(acUser, acOldPwd, acNewPwd);

    if (!bRet)
    {
        TaskLog(LOG_ERROR, "set pwd of user(%s) failed", acUser);
        JsonParam.Add("Result", "Fail");
    }
    else
    {
        JsonParam.Add("Result", "Succ");
        SetOperatorPwd(acUser, acNewPwd);
    }

    SendRsp(MSG_REC_SetPwd, ulSeqID, JsonParam);

    AddOperLog("用户%s修改密码%s", acUser, bRet?"成功":"失败");
}

VOID CheckAutoClose()
{
    if (g_iAutoCloseHour < 0)
    {
        return;
    }

    if (gos_get_uptime() < 61*60) // 启动后1小时内不重启,避免在自动重启的时点反复重启
    {
        return;
    }

    GOS_DATETIME_T  stCurrTime;

    gos_get_localtime(NULL, &stCurrTime);
    if ((INT32)stCurrTime.ucHour != g_iAutoCloseHour)
    {
        return;
    }

    GosLog(LOG_FATAL, "auto exit by system config");
    exit(0);
}

VOID TaskCfg::OnCheckTimer()
{
    CheckAutoClose();

    m_pDao->CheckConn();
}

VOID TaskCfg::TaskEntry(UINT16 usMsgID, VOID *pvMsg, UINT32 ulMsgLen)
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
                        gos_sleep(1);
                        //exit(-1);
                    }

                    break;

                default:
                    break;
            }

            break;

        case TASK_STATUS_WORK:
            switch(usMsgID)
            {
                case EV_CHECK_TIMER:
                    OnCheckTimer();
                    break;

                case EV_CLIENT_REQ:
                    OnClientReq((CHAR*)pvMsg, ulMsgLen);
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



