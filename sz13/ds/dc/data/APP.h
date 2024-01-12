#ifndef APP_H
#define APP_H

#include "g_include.h"

/// 业务信息
class LoginInfo
{
public:
    LoginInfo()
    {
        m_ulDCType = DC_TYPE_ALL;
        m_ulDepotID = DEPOT_ID_MAINLINE;
    }

    UINT32      m_ulDCType;             ///< 调度员的类型
    UINT32      m_ulDepotID;            ///< 调度台管理范围

    UINT32      m_ulSDKLoginShortNum;   ///< 核心网登陆的调度台号码如：3002、3003
    AnsiString  m_strSDKLoginPassword;  ///< 核心网登录密码

    AnsiString  m_strDCLoginAccount;    ///< 调度台登录DIS账号,如tom
    AnsiString  m_strDCLoginName;       ///< 调度台登录DIS账号显示名称，如汤姆
    AnsiString  m_strDCLoginPassword;   ///< 调度台登录DIS密码
};

namespace APP
{
/// 登录状态定义
enum LOGIN_STATUS
{
    STATUS_NOT_LOGIN,  ///< 没有登录的状态
    STATUS_LOGIN,      ///< 登录后状态
};

bool AllowDISOffline();
void AllowDISOffline(bool bAllowDISOffline);

bool IsClosing();
void SetCloseState(bool bClosing);
bool IsUIInited();
void SetUIInited();

//UINT32 GetCurrDCUserID();
VOID SetCurrDCUserID(UINT32 ulCurrDCUserID);

void SetEventHandle(HWND hWnd);
bool Login(HWND hWnd);
bool Logout();

bool IsDCLogin();
bool IsSDKLogin();

void SetDCLoginStatus(LOGIN_STATUS e);
void SetSDKLoginStatus(LOGIN_STATUS e);
void SetDCAccount(AnsiString strDCAccount, AnsiString strDCPassword, UINT32 ulDCType);
AnsiString GetDCLoginAccount();
AnsiString GetDCLoginPassword();
AnsiString GetDCLoginName();

bool IsCurrentDCAdmin();
// 后面要废弃
bool IsCurrentSDKShortNum(const UINT32& ulShortNum);
// 用本函数代替上面的函数
bool IsCurrDCUserID(UINT32 ulUserID);
bool IsAdminAccount(AnsiString strDCOperator);

void SetSDKAccount(UINT32 ulDCUserID);
AnsiString GetSDKPassword();
AnsiString GetSDKLoginAccount();
UINT32 GetSDKLoginShortNum();

UINT32 GetDCType();
UINT32 GetDepotID();
UINT32 GetCurrDCUserID();
int GetDCLoginPasswordMaxLength();

void SetDCType(UINT32 ulDCType);
void SetDepotID(UINT32 ulDepotID);
bool HasDCType(UINT32 ulDCType);
}  // namespace Login

#endif
