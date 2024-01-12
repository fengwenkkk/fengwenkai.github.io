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

VECTOR<CONFIRM_REQUEST_T>   g_vConfirmRequest;

extern BOOL DownHttpFile(char *szUrl, char *szLocalFile, char *szUser, char *szPwd);
extern AnsiString g_strDCOperatorID;

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
    TWinControl     *Parent = tl->Parent;

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

BOOL _SyncSendServer(UINT16 usMsgID, GJsonParam &ReqJsonParam, GJson &RspJson, UINT32 ulTimeout)
{
    if (!g_pRemoteMsg)
    {
        return FALSE;
    }

    CHAR    *szReq = ReqJsonParam.GetString();
    static CHAR acRsp[64*1024];
    UINT32  ulRspLen = 0;

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
//  return TRUE;
}

/*
BOOL _SyncSendServer(UINT16 usMsgID, GJsonParam *ReqJsonParam, GJson &RspJson, UINT32 ulTimeout)
{
    return SyncSendServer(usMsgID, ReqJsonParam, &RspJson, ulTimeout);
}

BOOL _SyncSendServer(UINT16 usMsgID, GJsonParam &ReqJsonParam, GJson *RspJson, UINT32 ulTimeout)
{
    return SyncSendServer(usMsgID, &ReqJsonParam, RspJson, ulTimeout);
}


BOOL _SyncSendServer(UINT16 usMsgID, GJsonParam &ReqJsonParam, GJson &RspJson, UINT32 ulTimeout)
{
    return SyncSendServer(usMsgID, &ReqJsonParam, &RspJson, ulTimeout);
} */

BOOL GetGsonResult(GJson &RspJson)
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

BOOL _SyncSendServer(UINT16 usMsgID, GJsonParam &ReqJsonParam, UINT32 ulTimeout)
{
    GJson       RspJson;

    if (!_SyncSendServer(usMsgID, ReqJsonParam, RspJson, ulTimeout))
    {
        return FALSE;
    }

    return GetGsonResult(RspJson);
}

BOOL SyncSendServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonParam &MsgInfoParam, GJson &RspSubJson, UINT32 ulTimeout)
{
    GJsonParam  ReqParam;
    GJson       RspJson;

    ReqParam.Add("MsgName", szMsgName);
 // ReqParam.Add("DCUserID", g_ulCurrDCUserID);
    ReqParam.Add("DCOperatorID", g_strDCOperatorID);
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
 // ReqParam.Add("DCUserID", g_ulCurrDCUserID);
    ReqParam.Add("DCOperatorID", g_strDCOperatorID);
    ReqParam.Add("MsgSeqID", GetMsgSeqID());
    ReqParam.Add("MsgInfo", MsgInfoParam);

    if (!_SyncSendServer(usMsgID, ReqParam, RspJson, ulTimeout))
    {
        return FALSE;
    }

    return GetGsonResult(RspJson);
}

BOOL SyncSendServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonTupleParam &MsgInfoParam, UINT32 ulTimeout)
{
    GJsonParam  ReqParam;
    GJson       RspJson;

    ReqParam.Add("MsgName", szMsgName);
 // ReqParam.Add("DCUserID", g_ulCurrDCUserID);
    ReqParam.Add("DCOperatorID", g_strDCOperatorID);
    ReqParam.Add("MsgSeqID", GetMsgSeqID());
    ReqParam.Add("MsgInfo", MsgInfoParam);

    if (!_SyncSendServer(usMsgID, ReqParam, RspJson, ulTimeout))
    {
        return FALSE;
    }

    return GetGsonResult(RspJson);
}

BOOL SendServer(UINT16 usMsgID, const CHAR *szMsgName, UINT32 ulSeqID, GJsonParam &Param)
{
    GJsonParam      ReqParam;

    ReqParam.Add("MsgName", (CHAR*)szMsgName);
//  ReqParam.Add("DCUserID", g_ulCurrDCUserID);
    ReqParam.Add("DCOperatorID", g_strDCOperatorID);
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

    ReqParam.Add("MsgName", (CHAR*)szMsgName);
 // ReqParam.Add("DCUserID", g_ulCurrDCUserID);
    ReqParam.Add("DCOperatorID", g_strDCOperatorID);
    ReqParam.Add("MsgSeqID", ulSeqID);
    ReqParam.Add("MsgInfo", Param);

    CHAR    *szReq = ReqParam.GetString();

    return g_pRemoteMsg->SendReq(usMsgID, szReq, strlen(szReq)+1);
}

BOOL SendServer(UINT16 usMsgID, const CHAR *szMsgName, GJsonTupleParam &Param)
{
    return SendServer(usMsgID, szMsgName, GetMsgSeqID(), Param);
}

VOID AddConfirmRequest(UINT16 usMsgID, CHAR *szMsgName, GJsonParam &Param, UINT32 ulRetryTime=3600)
{
    CONFIRM_REQUEST_T   stReq = {0};
    GJsonParam          ReqParam;
    UINT32              ulSeqID = GetMsgSeqID();

    ReqParam.Add("MsgName", (CHAR*)szMsgName);
 // ReqParam.Add("DCUserID", g_ulCurrDCUserID);
    ReqParam.Add("MsgSeqID", ulSeqID);
    ReqParam.Add("MsgInfo", Param);

    stReq.ulSeqID = ulSeqID;
    stReq.usMsgID = usMsgID;
    stReq.szMsgInfo = strdup(ReqParam.GetString());
    stReq.ulDeadTime = gos_get_uptime() + ulRetryTime;

    g_vConfirmRequest.push_back(stReq);
    SendConfirmRequest();
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
    if (g_vConfirmRequest.size() > 5000 )
    {
        GosLog(LOG_FATAL, "too many confirm request: %u", g_vConfirmRequest.size());
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

VOID SaveDCOperator(CHAR *szOperator, BOOL bOffDuty)
{
    GJsonParam      Param;
    UINT32          ulRetryTime = 60*5;

    Param.Add("Operator", szOperator);
    Param.Add("Duty", bOffDuty);

    AddConfirmRequest(EV_CLIENT_REQ, MSG_DCSetOperator, Param, ulRetryTime);
}

void NowaitCommand(char *szCmd, bool bShow)
{
    ShellExecute(NULL, "open", szCmd, "", "", bShow?SW_SHOW:SW_HIDE);
}

void RunBatCmd(AnsiString strCmd)
{
    char acFile[512] = {0};

    //sprintf(acFile, "%s/%s", gos_get_root_path(), "temp.bat");
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

void AppRelogin()
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
