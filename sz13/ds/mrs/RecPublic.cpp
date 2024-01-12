#include "RecPublic.h"

static GMutex   g_DCLoginMutex;
static GMutex   g_UserMutex;

VECTOR<REC_OPER_INFO_T>         g_vOperatorInfo;
RawIntMap<PID_T>                g_mLoginedDCUser(16);

BOOL CheckCallType(UINT32 ulCallTypee)
{
    if (ulCallTypee == REC_CALL_TYPE_P2P_AUDIO      ||
        ulCallTypee == REC_CALL_TYPE_P2P_VIDEO      ||
        ulCallTypee == REC_CALL_TYPE_VIDEO_UPLOAD   ||
        ulCallTypee == REC_CALL_TYPE_PTT_AUDIO      ||
        ulCallTypee == REC_CALL_TYPE_PTT_VIDEO      ||
        ulCallTypee == REC_CALL_TYPE_IPH_AUDIO      ||
        ulCallTypee == REC_CALL_TYPE_IPH_VIDEO      )
    {
        return TRUE;
    }

    return FALSE;
}

BOOL IsAudioCall(UINT32 ulCallTypee)
{
    if (ulCallTypee == REC_CALL_TYPE_P2P_AUDIO      ||
        ulCallTypee == REC_CALL_TYPE_PTT_AUDIO      ||
        ulCallTypee == REC_CALL_TYPE_IPH_AUDIO      )
    {
        return TRUE;
    }

    return FALSE;
}

//=============== logined DC user ========================
VOID SetDCLogin(UINT32 ulDCUserID, PID_T *pstPID)
{
    g_DCLoginMutex.Mutex();
    g_mLoginedDCUser.Add(ulDCUserID, pstPID);
    g_DCLoginMutex.Unmutex();
}

VOID SetDCLogout(UINT32 ulDCUserID)
{
    g_DCLoginMutex.Mutex();
    g_mLoginedDCUser.Del(ulDCUserID);
    g_DCLoginMutex.Unmutex();
}

BOOL GetDCPID(UINT32 ulDCUserID, PID_T *pstPID)
{
    pstPID->usInstID = INVALID_INST_ID;
    pstPID->usTaskID = INVALID_TASK_ID;

    g_DCLoginMutex.Mutex();

    PID_T   *pstTmp = g_mLoginedDCUser.GetValue(ulDCUserID);

    if (pstTmp)
    {
        memcpy(pstPID, pstTmp, sizeof(PID_T));
    }

    g_DCLoginMutex.Unmutex();

    return (pstTmp != NULL);
}

BOOL IsDCLogined(UINT32 ulDCUserID)
{
    g_DCLoginMutex.Mutex();

    BOOL bHas = g_mLoginedDCUser.Has(ulDCUserID);

    g_DCLoginMutex.Unmutex();

    return bHas;
}

VOID GetLoginedDCUser(VECTOR<UINT32> &vUser)
{
    g_DCLoginMutex.Mutex();

    g_mLoginedDCUser.GetKeyList(vUser);

    g_DCLoginMutex.Unmutex();
}


//=============== opertor ========================
VOID InitOperatorInfo(VECTOR<REC_OPER_INFO_T> &vInfo)
{
    AUTO_LOCK(g_UserMutex);
    g_vOperatorInfo.assign(vInfo.begin(), vInfo.end());
}

VOID AddOperatorInfo(REC_OPER_INFO_T &stOperatorInfo)
{
    AUTO_LOCK(g_UserMutex);
    g_vOperatorInfo.push_back(stOperatorInfo);
}

VOID DelOperatorInfo(CHAR *szOperatorID)
{
    AUTO_LOCK(g_UserMutex);

    for (UINT32 i=0; i<g_vOperatorInfo.size(); i++)
    {
        if (strcmp(g_vOperatorInfo[i].acOperatorID, szOperatorID) == 0)
        {
            g_vOperatorInfo.erase(g_vOperatorInfo.begin()+i);
            break;
        }
    }
}

BOOL SetOperatorInfo(REC_OPER_INFO_T &stOperatorInfo)
{
    AUTO_LOCK(g_UserMutex);

    for (UINT32 i=0; i<g_vOperatorInfo.size(); i++)
    {
        if (strcmp(g_vOperatorInfo[i].acOperatorID, stOperatorInfo.acOperatorID) == 0)
        {
            memcpy(&g_vOperatorInfo[i], &stOperatorInfo, sizeof(stOperatorInfo));

            return TRUE;
        }
    }

    return FALSE;
}

BOOL SetOperatorPwd(CHAR *szOperatorID, CHAR *szNewPassword)
{
    AUTO_LOCK(g_UserMutex);

    for (UINT32 i=0; i<g_vOperatorInfo.size(); i++)
    {
        if (strcmp(g_vOperatorInfo[i].acOperatorID, szOperatorID) == 0)
        {
            strcpy(g_vOperatorInfo[i].acPassword, szNewPassword);

            return TRUE;
        }
    }

    return FALSE;
}

VOID GetOperatorInfo(VECTOR<REC_OPER_INFO_T> &vInfo)
{
    AUTO_LOCK(g_UserMutex);

    vInfo.clear();
    vInfo.assign(g_vOperatorInfo.begin(), g_vOperatorInfo.end());
}

BOOL CheckOperator(CHAR *szOperatorID, CHAR *szPassword, UINT32 &ulOperType, CHAR *szErrInfo)
{
    AUTO_LOCK(g_UserMutex);

    *szErrInfo = '\0';
    for (UINT32 i=0; i<g_vOperatorInfo.size(); i++)
    {
        if (strcmp(g_vOperatorInfo[i].acOperatorID, szOperatorID) == 0)
        {
            if (strcmp(g_vOperatorInfo[i].acPassword, szPassword) != 0)
            {
                strcpy(szErrInfo, "ÃÜÂë´íÎó");
                return FALSE;
            }

            ulOperType = g_vOperatorInfo[i].ulOperType;
            return TRUE;
        }
    }

    strcpy(szErrInfo, "Î´ÖªÓÃ»§");
    return FALSE;
}

BOOL GetOperatorInfo(CHAR *szOperatorID, REC_OPER_INFO_T &stInfo)
{
    AUTO_LOCK(g_UserMutex);

    for (UINT32 i=0; i<g_vOperatorInfo.size(); i++)
    {
        if (strcmp(g_vOperatorInfo[i].acOperatorID, szOperatorID) == 0)
        {
            memcpy(&stInfo, &g_vOperatorInfo[i], sizeof(stInfo));
            return TRUE;
        }
    }

    return FALSE;
}
