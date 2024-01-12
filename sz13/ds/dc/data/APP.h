#ifndef APP_H
#define APP_H

#include "g_include.h"

/// ҵ����Ϣ
class LoginInfo
{
public:
    LoginInfo()
    {
        m_ulDCType = DC_TYPE_ALL;
        m_ulDepotID = DEPOT_ID_MAINLINE;
    }

    UINT32      m_ulDCType;             ///< ����Ա������
    UINT32      m_ulDepotID;            ///< ����̨����Χ

    UINT32      m_ulSDKLoginShortNum;   ///< ��������½�ĵ���̨�����磺3002��3003
    AnsiString  m_strSDKLoginPassword;  ///< ��������¼����

    AnsiString  m_strDCLoginAccount;    ///< ����̨��¼DIS�˺�,��tom
    AnsiString  m_strDCLoginName;       ///< ����̨��¼DIS�˺���ʾ���ƣ�����ķ
    AnsiString  m_strDCLoginPassword;   ///< ����̨��¼DIS����
};

namespace APP
{
/// ��¼״̬����
enum LOGIN_STATUS
{
    STATUS_NOT_LOGIN,  ///< û�е�¼��״̬
    STATUS_LOGIN,      ///< ��¼��״̬
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
// ����Ҫ����
bool IsCurrentSDKShortNum(const UINT32& ulShortNum);
// �ñ�������������ĺ���
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
