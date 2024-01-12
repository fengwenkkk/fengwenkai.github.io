#include "ClientInclude.h"
#include "AudioDev.h"
#include "APP.h"
#include "eLTE_Types.h"
#include "eLTE_SDK.h"

namespace APP
{
static LOGIN_STATUS s_DCLoginStatus = STATUS_NOT_LOGIN;   ///< 调度台账号登录状态
static LOGIN_STATUS s_SDKLoginStatus = STATUS_NOT_LOGIN;  ///< SDK 登录状态
static LoginInfo s_BusinessInfo;                          ///< 登录业务信息
static const int DC_LOGIN_PASSWORD_MAX_LENGTH = 36;       ///< 调度员密码长度
static bool g_bAllowDISOffline = FALSE;      // 是否允许DIS离线时调度台继续运行

bool AllowDISOffline()
{
    return g_bAllowDISOffline;
}

void AllowDISOffline(bool bAllowDISOffline)
{
    g_bAllowDISOffline = bAllowDISOffline;
}

bool IsDCLogin()
{
    return s_DCLoginStatus == STATUS_LOGIN;
}

bool IsSDKLogin()
{
    return s_SDKLoginStatus == STATUS_LOGIN;
}

void SetDCLoginStatus(LOGIN_STATUS e)
{
    GosLog(LOG_DETAIL, "Set DC Login Status %d -> %d", (int)s_DCLoginStatus, (int)e);
    s_DCLoginStatus = e;
}

void SetSDKLoginStatus(LOGIN_STATUS e)
{
    GosLog(LOG_DETAIL, "Set SDK Login Status %d -> %d", (int)s_SDKLoginStatus, (int)e);
    s_SDKLoginStatus = e;
}

// 没有被调用
void SetDCAccount(AnsiString strDCAccount, AnsiString strDCPassword, UINT32 ulDCType)
{
    s_BusinessInfo.m_strDCLoginAccount = strDCAccount;
    s_BusinessInfo.m_strDCLoginName = DCOperator::GetDCOperatorName(strDCAccount);
    s_BusinessInfo.m_strDCLoginPassword = strDCPassword;

    s_BusinessInfo.m_ulDCType = ulDCType;
}

AnsiString GetDCLoginAccount()
{
    return s_BusinessInfo.m_strDCLoginAccount;
}

AnsiString GetDCLoginPassword()
{
    return s_BusinessInfo.m_strDCLoginPassword;
}

AnsiString GetDCLoginName()
{
    return s_BusinessInfo.m_strDCLoginName;
}

bool IsCurrentDCAdmin()
{
    return IsDCLogin() && s_BusinessInfo.m_strDCLoginAccount == "admin";
}

// 后面要废弃
bool IsCurrentSDKShortNum(const UINT32& ulShortNum)
{
    return IsSDKLogin() && (GetSDKLoginShortNum() == ulShortNum);
}

// 用本函数代替上面的函数
bool IsCurrDCUserID(UINT32 ulUserID)
{
    return IsSDKLogin() && (GetSDKLoginShortNum() == ulUserID);
}

bool IsAdminAccount(AnsiString strDCOperator)
{
    return strDCOperator == "admin";
}

void SetSDKAccount(UINT32 ulDCUserID)//, AnsiString strPwd)
{
    s_BusinessInfo.m_ulSDKLoginShortNum = ulDCUserID;
    s_BusinessInfo.m_strSDKLoginPassword = ulDCUserID;  // 用户名和密码相同，需确认华为eUDC是否如此设置
}

AnsiString GetSDKPassword()
{
    return s_BusinessInfo.m_strSDKLoginPassword;
}

AnsiString GetSDKLoginAccount()
{
    return s_BusinessInfo.m_ulSDKLoginShortNum;
}

UINT32 GetSDKLoginShortNum()
{
    return s_BusinessInfo.m_ulSDKLoginShortNum;
}

UINT32 GetDCType()
{
    return s_BusinessInfo.m_ulDCType;
}

UINT32 GetDepotID()
{
    return s_BusinessInfo.m_ulDepotID;
}

UINT32 GetCurrDCUserID()
{
    return s_BusinessInfo.m_ulSDKLoginShortNum;
}

int GetDCLoginPasswordMaxLength()
{
    return DC_LOGIN_PASSWORD_MAX_LENGTH;
}

void SetDCType(UINT32 ulDCType)
{
    s_BusinessInfo.m_ulDCType = ulDCType;
}

void SetDepotID(UINT32 ulDepotID)
{
    s_BusinessInfo.m_ulDepotID = ulDepotID;
}

bool HasDCType(UINT32 ulDCType)
{
    return (s_BusinessInfo.m_ulDCType & ulDCType);
}

static HWND     g_Handle = NULL;
static BOOL     g_bClosing = FALSE;
static BOOL     g_bInitAllForm = FALSE;

bool IsClosing()
{
    return g_bClosing;
}

void SetCloseState(bool bClosing)
{
    g_bClosing = bClosing;
}

bool IsUIInited()
{
    return g_bInitAllForm;
}

void SetUIInited()
{
    g_bInitAllForm = true;
}

static VOID __stdcall ELTE_EventCallBack(INT32 iEventType, VOID* pEventBuf,
                                         UINT32 uiBufSize, VOID* pUserData)
{
    if (g_bClosing)
    {
        return;
    }

    if (!g_Handle || !g_bInitAllForm)
    {
        if (iEventType == ELTE_Event_NotifyResourceStatus)          // 资源状态变化
        {
            EVENT_RESOURCE_STATUS_T     stEvent = {0};

            if (!ParseEventResourceStatus((CHAR*)pEventBuf, &stEvent))
            {
                GosLog(LOG_ERROR, "parse NotifyResourceStatus failed");
                return;
            }

            if (stEvent.ulStatusType  == RESOURCE_STATUS_TYPE_REG_PROXY &&
                stEvent.ulStatusValue == RESOURCE_STATUS_REGOK &&
                stEvent.i64ResourceID == GetCurrDCUserID())
            {
            //  15：资源注册状态指示，代理注册 RESREGSTATUS_PROXY。
                eSDK_Logined();
                GosLog(LOG_INFO, "get the login notification of MDC");
                return;
            }
        }

        return;
    }

    UINT32          ulEventMsgLen = STRUCT_OFFSET(ESDK_EVENT_T, acMsg)+uiBufSize+1;
    UINT32          ulTotalMsgLen = STRUCT_OFFSET(INNER_MSG_T, aucMsg)+ulEventMsgLen;
    INNER_MSG_T     *pstMsg = (INNER_MSG_T*)calloc(1, ulTotalMsgLen);
    ESDK_EVENT_T    *pstEvent;
    HWND            Handle = g_Handle;

    if (!pstMsg)
    {
        return;
    }

    pstEvent = (ESDK_EVENT_T*)pstMsg->aucMsg;

    pstEvent->iEventType = iEventType;
    pstEvent->ulMsgLen = uiBufSize;
    if (pstEvent->ulMsgLen)
    {
        memcpy(pstEvent->acMsg, pEventBuf, pstEvent->ulMsgLen);
    }

    pstMsg->ulMsgType = EV_ESDK_MSG;
    pstMsg->ulMsgLen = ulEventMsgLen;

    if (!g_bClosing && !PostMessage(Handle, WM_INNER_MSG, 0, (LPARAM)pstMsg))
    {
        free(pstMsg);
    }
}

void SetEventHandle(HWND hWnd)
{
    g_Handle = hWnd;
}

bool Login(HWND hWnd)
{
    g_Handle = hWnd;

    GosLog(LOG_INFO, "Init SDK ");

    BOOL    bRet = eSDK_Init(ELTE_EventCallBack);
    UINT32  ulCurrDCUserID = GetCurrDCUserID();
    UINT8   aucLocalIP[4];
    IP_ADDR_T   stMDCAddr = LocalCfg::GetInstance().GetMdcIP();
    AnsiString  strDCUserID = APP::GetSDKLoginAccount();
    AnsiString  strDCPwd = APP::GetSDKPassword();

    if (!bRet)
    {
        GosLog(LOG_FATAL, "SDK Init failed!");
        return false;
    }

    GosLog(LOG_INFO, "Init SDK successful");
    if (!GetLocalIP(stMDCAddr.aucIP, aucLocalIP))
    {
        GosLog(LOG_FATAL, "connect to MDC server failed");
        return false;
    }

    DC_USER_INFO_T  *pstUserInfo = DCUser::GetDCUser(ulCurrDCUserID);
    if (!pstUserInfo)
    {
        GosLog(LOG_INFO, "login aborted: %u is not a DC user", ulCurrDCUserID);
        return false;
    }

    eSDK_TriggerStatusReport(TRUE);

    bRet = eSDK_Login(strDCUserID.c_str(), strDCPwd.c_str(), stMDCAddr.aucIP, stMDCAddr.usPort, aucLocalIP);
    if (!bRet)
    {
        GosLog(LOG_FATAL, "login to MDC failed");
        return false;
    }

    APP::SetSDKLoginStatus(APP::STATUS_LOGIN);
    GosLog(LOG_FATAL, "login to MDC successfully");
    return true;
}

bool Logout()
{
    AnsiString  strUserID = GetCurrDCUserID();

    if (eSDK_Logout(strUserID.c_str()))
    {
        APP::SetSDKLoginStatus(APP::STATUS_NOT_LOGIN);
        return true;
    }
    else
    {
        return false;
    }
}


}  // namespace Login
