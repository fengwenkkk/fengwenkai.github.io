#include <vcl.h>
#include "ClientInclude.h"
#include "cxTL.hpp"

typedef struct
{
    UINT32      ulSeqID;
    UINT16      usMsgID;
    CHAR        *szMsgInfo;
    UINT32      ulDeadTime;
}CONFIRM_REQUEST_T;

static VECTOR<CONFIRM_REQUEST_T>    g_vConfirmRequest;

extern BOOL DownloadHttpFile(char *szUrl, char *szLocalFile, char *szUser, char *szPwd);

UINT32 GetMsgSeqID()
{
    static UINT32   ulMsgSeqID = 0;
    UINT32          ulTime = gos_get_current_time();

    if (0 == ulMsgSeqID)
    {
        ulMsgSeqID = ulTime;
    }
    else
    {
        if (ulMsgSeqID < ulTime)
        {
            ulMsgSeqID = ulTime;
        }
        else
        {
            ulMsgSeqID++;
        }
    }

    return ulMsgSeqID;
}

void InitTreeListPos(TcxTreeList *tl)
{
    TWinControl *Parent = tl->Parent;

    tl->Align = alNone;
    tl->Left = 0;
    tl->Top = 0;
    tl->Width = Parent->Width;
    tl->Height = Parent->Height - 21 - 52;

    tl->Anchors << akLeft;
    tl->Anchors << akTop;
    tl->Anchors << akRight;
    tl->Anchors << akBottom;
}

BOOL _SyncSendMrsServer(UINT16 usMsgID, GJsonParam &ReqJsonParam, GJson &RspJson, UINT32 ulTimeout)
{
    if (!g_pMRSRemoteMsg)
    {
        return FALSE;
    }

    CHAR    *szReq = ReqJsonParam.GetString();
    static CHAR acRsp[64*1024];
    UINT32  ulRspLen = 0;

    memset(acRsp, 0, sizeof(acRsp));

    if (ulTimeout == 0)
    {
        return g_pMRSRemoteMsg->SendReq(usMsgID, szReq, strlen(szReq)+1);
    }

    if (!g_pMRSRemoteMsg->SyncSend(usMsgID, szReq, strlen(szReq)+1, acRsp, sizeof(acRsp)-1, &ulRspLen, ulTimeout))
    {
        return FALSE;
    }

    return RspJson.Parse(acRsp);
}

BOOL _SyncSendServer(UINT16 usMsgID, GJsonParam &ReqJsonParam, GJson &RspJson, UINT32 ulTimeout)
{
    if (!g_pRemoteMsg)
    {
        return FALSE;
    }

    CHAR        *szReq = ReqJsonParam.GetString();
    static CHAR acRsp[64*1024];
    UINT32      ulRspLen = 0;

    memset(acRsp, 0, sizeof(acRsp));

    if (ulTimeout == 0)
    {
        return g_pRemoteMsg->SendReq(usMsgID, szReq, strlen(szReq)+1);
    }

    if (!g_pRemoteMsg->SyncSend(usMsgID, szReq, strlen(szReq)+1, acRsp, sizeof(acRsp)-1, &ulRspLen, ulTimeout))
    {
        return FALSE;
    }

    return RspJson.Parse(acRsp);
}

BOOL GetJsonResult(GJson &RspJson)
{
    CHAR    *szMsgInfo = RspJson.GetValue("MsgInfo");
    GJson   SubJson;

    if (!SubJson.Parse(szMsgInfo))
    {
        GosLog(LOG_ERROR, "parse MsgInfo failed: %s", szMsgInfo);
        return FALSE;
    }

    CHAR    *szResult = SubJson.GetValue("Result");

    if (szResult && strcasecmp(szResult, "Succ") == 0)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL _SyncSendMrsServer(UINT16 usMsgID, GJsonParam &ReqJsonParam, UINT32 ulTimeout)
{
    GJson       RspJson;

    if (!_SyncSendMrsServer(usMsgID, ReqJsonParam, RspJson, ulTimeout))
    {
        return FALSE;
    }

    return GetJsonResult(RspJson);
}

BOOL _SyncSendServer(UINT16 usMsgID, GJsonParam &ReqJsonParam, UINT32 ulTimeout)
{
    GJson       RspJson;

    if (!_SyncSendServer(usMsgID, ReqJsonParam, RspJson, ulTimeout))
    {
        return FALSE;
    }

    return GetJsonResult(RspJson);
}

BOOL SyncSendServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &MsgInfoParam, GJson &RspSubJson, UINT32 ulTimeout)
{
    GJsonParam  ReqParam;
    GJson       RspJson;

    ReqParam.Add("MsgName", szMsgName);
    ReqParam.Add("DCUserID", APP::GetSDKLoginShortNum());
    ReqParam.Add("DCOperatorID", APP::GetDCLoginAccount());
    ReqParam.Add("MsgSeqID", GetMsgSeqID());
    ReqParam.Add("MsgInfo", MsgInfoParam);

    if (!_SyncSendServer(usMsgID, ReqParam, RspJson, ulTimeout))
    {
        return FALSE;
    }

    CHAR    *szMsgInfo = RspJson.GetValue("MsgInfo");

    if (!RspSubJson.Parse(szMsgInfo))
    {
        GosLog(LOG_ERROR, "parse MsgInfo failed: %s", szMsgInfo);
        return FALSE;
    }

    return TRUE;
}

BOOL SyncSendServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &MsgInfoParam, UINT32 ulTimeout)
{
    GJsonParam  ReqParam;
    GJson       RspJson;

    ReqParam.Add("MsgName", szMsgName);
    ReqParam.Add("DCUserID", APP::GetSDKLoginShortNum());
    ReqParam.Add("DCOperatorID", APP::GetDCLoginAccount());
    ReqParam.Add("MsgSeqID", GetMsgSeqID());
    ReqParam.Add("MsgInfo", MsgInfoParam);

    if (!_SyncSendServer(usMsgID, ReqParam, RspJson, ulTimeout))
    {
        return FALSE;
    }

    return GetJsonResult(RspJson);
}

BOOL SyncSendServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonTupleParam &MsgInfoParam, UINT32 ulTimeout)
{
    GJsonParam  ReqParam;
    GJson       RspJson;

    ReqParam.Add("MsgName", szMsgName);
    ReqParam.Add("DCUserID", APP::GetSDKLoginShortNum());
    ReqParam.Add("DCOperatorID", APP::GetDCLoginAccount());
    ReqParam.Add("MsgSeqID", GetMsgSeqID());
    ReqParam.Add("MsgInfo", MsgInfoParam);

    if (!_SyncSendServer(usMsgID, ReqParam, RspJson, ulTimeout))
    {
        return FALSE;
    }

    return GetJsonResult(RspJson);
}

BOOL SendServer(UINT16 usMsgID, const CHAR *szMsgName, UINT32 ulSeqID, GJsonParam &Param)
{
    GJsonParam      ReqParam;

    ReqParam.Add("MsgName", (CHAR *)szMsgName);
    ReqParam.Add("DCUserID", APP::GetSDKLoginShortNum());
    ReqParam.Add("DCOperatorID", APP::GetDCLoginAccount());
    ReqParam.Add("MsgSeqID", ulSeqID);
    ReqParam.Add("MsgInfo", Param);

    CHAR    *szReq = ReqParam.GetString();

    return g_pRemoteMsg->SendReq(usMsgID, szReq, strlen(szReq)+1);
}

BOOL SendServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &Param)
{
    return SendServer(usMsgID, szMsgName, GetMsgSeqID(), Param);
}

BOOL SendServer(UINT16 usMsgID, const CHAR *szMsgName)
{
    GJsonParam  Param;

    return SendServer(usMsgID, szMsgName, Param);
}

BOOL SendServer(UINT16 usMsgID, const CHAR *szMsgName, UINT32 ulSeqID, GJsonTupleParam &Param)
{
    GJsonParam      ReqParam;

    ReqParam.Add("MsgName", (CHAR *)szMsgName);
    ReqParam.Add("DCUserID", APP::GetSDKLoginShortNum());
    ReqParam.Add("DCOperatorID", APP::GetDCLoginAccount());
    ReqParam.Add("MsgSeqID", ulSeqID);
    ReqParam.Add("MsgInfo", Param);

    CHAR    *szReq = ReqParam.GetString();

    return g_pRemoteMsg->SendReq(usMsgID, szReq, strlen(szReq)+1);
}

BOOL SendServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonTupleParam &Param)
{
    return SendServer(usMsgID, szMsgName, GetMsgSeqID(), Param);
}

VOID AddConfirmRequest(UINT16 usMsgID, const CHAR *szMsgName, CHAR *szMsg, UINT32 ulRetryTime=300)
{
    CONFIRM_REQUEST_T   stReq = {0};
    GJsonParam          ReqParam;
    UINT32              ulSeqID = GetMsgSeqID();

    ReqParam.Add("MsgName", (CHAR *)szMsgName);
    ReqParam.Add("DCUserID", APP::GetCurrDCUserID());
    ReqParam.Add("MsgSeqID", ulSeqID);
    ReqParam.Add("MsgInfo", szMsg, FALSE);

    stReq.ulSeqID = ulSeqID;
    stReq.usMsgID = usMsgID;
    stReq.szMsgInfo = strdup(ReqParam.GetString());
    stReq.ulDeadTime = gos_get_uptime() + ulRetryTime;

    g_vConfirmRequest.push_back(stReq);
    SendConfirmRequest();
}

VOID AddConfirmRequest(UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &Param, UINT32 ulRetryTime=300)
{
    AddConfirmRequest(usMsgID, szMsgName, Param.GetString(), ulRetryTime);
}

VOID AddConfirmRequest(UINT16 usMsgID, const CHAR *szMsgName, GJsonTupleParam &Param, UINT32 ulRetryTime=300)
{
    AddConfirmRequest(usMsgID, szMsgName, Param.GetString(), ulRetryTime);
}

VOID DeleteConfirmRequest(UINT32 ulSeqID)
{
    for (UINT32 i=0; i<g_vConfirmRequest.size(); i++)
    {
        if (g_vConfirmRequest[i].ulSeqID == ulSeqID)
        {
            free(g_vConfirmRequest[i].szMsgInfo);
            g_vConfirmRequest.erase(g_vConfirmRequest.begin()+i);
            return;
        }
    }
}

VOID SendConfirmRequest()
{
    UINT32      ulMaxConfireRequestNum = 1000;

    while (g_vConfirmRequest.size() > ulMaxConfireRequestNum)
    {
        GosLog(LOG_FATAL, "too many confirm request: %u", g_vConfirmRequest.size());
        free(g_vConfirmRequest[0].szMsgInfo);
        g_vConfirmRequest.erase(g_vConfirmRequest.begin());
    }

    if (!g_pRemoteMsg->IsConnected())
    {
        return;
    }

    UINT32  ulCurrTime = gos_get_uptime();

    while(g_vConfirmRequest.size() > 0)
    {
        if (g_vConfirmRequest[0].ulDeadTime < ulCurrTime)
        {
            free(g_vConfirmRequest[0].szMsgInfo);
            g_vConfirmRequest.erase(g_vConfirmRequest.begin());
            continue;
        }

        break;
    }

    if (g_vConfirmRequest.size() == 0)
    {
        return;
    }

    CONFIRM_REQUEST_T   &stReq = g_vConfirmRequest[0];

    g_pRemoteMsg->SendReq(stReq.usMsgID, stReq.szMsgInfo, strlen(stReq.szMsgInfo)+1);
}

VOID SaveP2PCallInfo(UINT32 ulCallType, UINT32 ulCaller, UINT32 ulCallee, UINT32 ulStatus)
{
    GJsonParam Param;

    Param.Add("CallType", ulCallType);
    Param.Add("Caller", ulCaller);
    Param.Add("Callee", ulCallee);
    Param.Add("Status", ulStatus);
    Param.Add("Time", gos_get_current_time());

    AddConfirmRequest(EV_DC_DIS_REQ, MSG_DCSaveP2PCallInfo, Param);
}

VOID SaveP2PVideoCallInfo(UINT32 ulCallType, UINT32 ulCaller, UINT32 ulCallee, UINT32 ulStatus)
{
    GJsonParam Param;

    Param.Add("CallType", ulCallType);
    Param.Add("Caller", ulCaller);
    Param.Add("Callee", ulCallee);
    Param.Add("Status", ulStatus);
    Param.Add("Time", gos_get_current_time());

    AddConfirmRequest(EV_DC_DIS_REQ, MSG_DCSaveP2PVideoCallInfo, Param);
}

VOID SaveGroupCallInfo(UINT32 ulCallType, INT64 i64GroupID, UINT32 ulSpeaker)
{
    GJsonParam Param;

    Param.Add("CallType", ulCallType);
    Param.Add("GroupID", i64GroupID);
    Param.Add("Speaker", ulSpeaker);
    Param.Add("Time", gos_get_current_time());

    AddConfirmRequest(EV_DC_DIS_REQ, MSG_DCSaveGroupCallInfo, Param);
}

VOID SaveRealBrdInfo(UINT32 ulStartTime, UINT32 ulEndTime, CHAR *szTrainList)
{
    GJsonParam Param;

    Param.Add("StartTime", ulStartTime);
    Param.Add("EndTime", ulEndTime);
    Param.Add("TrainList", szTrainList);

    AddConfirmRequest(EV_DC_DIS_REQ, MSG_DCSaveRealBrdInfo, Param);
}

VOID SaveGroupInfo(INT64 i64GroupID, CHAR *szGroupName, UINT32 ulFuncType)
{
    GJsonParam Param;

    Param.Add("GroupID", i64GroupID);
    Param.Add("GroupName", szGroupName);
    Param.Add("FuncType", ulFuncType);

    AddConfirmRequest(EV_DC_DIS_REQ, MSG_DCSaveGroupInfo, Param);
}

VOID SaveUESDSReply(CHAR *szUserID, UINT32 ulSDSID)
{
    GJsonParam      Param;

    Param.Add("UserID", szUserID);
    Param.Add("SDSID", ulSDSID);
    Param.Add("Time", gos_get_current_time());

    AddConfirmRequest(EV_DC_DIS_REQ, MSG_DCSetUESDSReply, Param);
}

VOID SaveUESDSRecv(CHAR *szReceiver, UINT32 ulSDSID, BOOL bSendSucc)
{
    GJsonParam      Param;

    Param.Add("UserID", szReceiver);
    Param.Add("SDSID", ulSDSID);
    Param.Add("SendResult", bSendSucc);
    Param.Add("Time", gos_get_current_time());

    AddConfirmRequest(EV_DC_DIS_REQ, MSG_DCSetUESDSRecv, Param);
}

VOID SaveNewSDS(CHAR *szUserID, CHAR *szUserName, CHAR *szSDSInfo)
{
    GJsonParam      Param;

    Param.Add("SendUserID", szUserID);
    Param.Add("SendUserName", szUserName);
    Param.Add("SendTime", gos_get_current_time());
    Param.Add("SDSInfo", EncodeSMSMsg(szSDSInfo));

    AddConfirmRequest(EV_DC_DIS_REQ, MSG_DCSaveNewSDS, Param);
}

VOID SaveNewSMS(CHAR *szUserID, CHAR *szUserName, CHAR *szSMSInfo, CHAR *szFile)
{
    GJsonParam      Param;

    Param.Add("SendUserID", szUserID);
    Param.Add("SendUserName", szUserName);
    Param.Add("SendTime", gos_get_current_time());
    Param.Add("SMSInfo", EncodeSMSMsg(szSMSInfo));
    Param.Add("File", szFile);

    AddConfirmRequest(EV_DC_DIS_REQ, MSG_DCSaveNewSMS, Param);
}

VOID SaveDCOperator(CHAR *szOperator, BOOL bOffDuty)
{
    GJsonParam      Param;

    Param.Add("Operator", szOperator);
    Param.Add("Duty", bOffDuty);

    AddConfirmRequest(EV_DC_DIS_REQ, MSG_DCSetOperator, Param);
}

VOID SaveGISInfo(UINT32 ulUserID, DOUBLE dLongtitude, DOUBLE dLatitude)
{
    GJsonParam      Param;

    Param.Add("UserID", ulUserID);
    Param.Add("Lon", dLongtitude);
    Param.Add("Lat", dLatitude);

    AddConfirmRequest(EV_DC_DIS_REQ, MSG_DCSaveGISInfo, Param);
}

VOID SaveMultiGroupInfo(VECTOR<SDK_MULTI_GROUP_INFO_T> &vGroup)
{
    GJsonTupleParam TupleParam;
    CHAR            acGroupMember[2048];

    for (UINT32 i=0; i<vGroup.size(); i++)
    {
        GJsonParam      Param;
        AnsiString      strMember = "";
        SDK_MULTI_GROUP_INFO_T  &stGroup = vGroup[i];

        for (UINT32 j=0; j<stGroup.ulMemberNum; j++)
        {
            if (j > 0)
            {
                strMember += ",";
            }

            strMember += stGroup.ai64MemberGroupID[j];
        }

        Param.Add("GroupID", stGroup.i64GroupID);
        Param.Add("GroupName", stGroup.acGroupName);
        Param.Add("GroupMember", strMember);

        TupleParam.Add(Param);
    }

    AddConfirmRequest(EV_DC_DIS_REQ, MSG_DCSaveMultiGroupInfo, TupleParam);
}

VOID SaveOperLog(const char *szOperLog, ...)
{
    CHAR            acLog[4096];
    va_list         vaLog;

    va_start(vaLog, szOperLog);

    memset(acLog, 0, sizeof(acLog));
    vsnprintf(acLog, sizeof(acLog)-1, szOperLog, vaLog);

    va_end(vaLog);

    GJsonParam      Param;

    Param.Add("OperLog", acLog);
    Param.Add("OperTime", gos_get_current_time());

    AddConfirmRequest(EV_DC_DIS_REQ, MSG_DCSaveOperLog, Param);
}

BOOL HttpDownloadFile(AnsiString strUrl, AnsiString strLocalFile)
{
    CHAR        *szUrl = strUrl.c_str();
    CHAR        *szLocalFile = strLocalFile.c_str();
    CHAR        *szUser = (CHAR*)APP::GetSDKLoginAccount().c_str();
    CHAR        *szPwd = (CHAR*)APP::GetSDKPassword().c_str();

    return DownloadHttpFile(szUrl, szLocalFile, szUser, szPwd);
}

void NowaitCommand(char *szCmd, bool bShow)
{
    ShellExecute(NULL, "open", szCmd, "", "", bShow?SW_SHOW:SW_HIDE);
}

void RunBatCmd(AnsiString strCmd)
{
    char acFile[512] = {0};

    sprintf_s(acFile, sizeof(acFile), "%s/%s", gos_get_root_path(), "temp.bat");

    gos_delete_file(acFile);

    FILE *f = fopen(acFile, "w+t");
    if (f == NULL)
    {
        return;
    }

    fprintf(f, strCmd.c_str());
    fclose(f);

    NowaitCommand(acFile, false);
}

void DCRelogin()
{
    CHAR    acProcName[128];
    CHAR    acModuleName[512];
    char    acCmd[1024];
    int     i = 0;

    if (GetModuleFileName(NULL, acModuleName, sizeof(acModuleName)-1) == 0)
    {
        return;
    }

    gos_get_proc_name(acProcName, TRUE);

    i += sprintf(acCmd+i, "set /a cnt = 0\n");
    i += sprintf(acCmd+i, ":loop\n");
    i += sprintf(acCmd+i, "ping 127.0.0.1 -n 1\n");
    i += sprintf(acCmd+i, "set /a cnt += 1\n");
    i += sprintf(acCmd+i, "tasklist /nh | find /i \"%s\" > null\n", acProcName);
    i += sprintf(acCmd+i, "if ERRORLEVEL 1 (goto start)\n");
    i += sprintf(acCmd+i, "if \"%cnt%\" == \"8\" (goto start) else (goto loop)\n");
    i += sprintf(acCmd+i, ":start\n");
    i += sprintf(acCmd+i, "taskkill /im \"%s\" /f\n", acProcName);
    i += sprintf(acCmd+i, "\"%s\" &\n", acModuleName);

    RunBatCmd(acCmd);
}
