#include "ClientInclude.h"

namespace eSDKData
{

static UINT32 SDS_TYPE_NORMAL_SMS  = 1;     // ���Ե�Ե��Ⱥ�����
static UINT32 SDS_TYPE_STATUS_SMS  = 2;     // �ն�Ⱥ��������̨��״̬����
static UINT32 SDS_TYPE_MM_SMS      = 4;     // ���Ե�Ե��Ⱥ����Ż��߲���

static UINT32 SDS_TYPE_P2P_IM      = 201;   // �Ե㷢��ʱ��Ϣ��
static UINT32 SDS_TYPE_GROUP_IM    = 202;   // ��Ⱥ�鼴ʱ��Ϣ��
static UINT32 SDS_TYPE_IM_REPLY    = 203;   // ���ͼ�ʱ��Ϣ��ִ

static ID_VALUE_T   s_astSDSType[] = {
                        {SDS_TYPE_NORMAL_SMS,   "����Ϣ"},
                        {SDS_TYPE_STATUS_SMS,   "״̬��Ϣ"},
                        {SDS_TYPE_MM_SMS,       "����"},
                        {SDS_TYPE_P2P_IM,       "���ӹ���"},
                        {SDS_TYPE_GROUP_IM,     "���ӹ���"},
                        {SDS_TYPE_IM_REPLY,     "������ִ"} };


AnsiString GetSDSTypeText(UINT32 ulSDSType)
{
    for (UINT32 i=0; i<ARRAY_SIZE(s_astSDSType); i++)
    {
        if (s_astSDSType[i].ulID == ulSDSType)
        {
            return s_astSDSType[i].szValue;
        }
    }

    return "";
}

BOOL IsRealMsg(UINT32 ulSDSType)
{
    if (ulSDSType == SDS_TYPE_P2P_IM ||
        ulSDSType == SDS_TYPE_GROUP_IM)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL IsMMMsg(UINT32 ulSDSType)
{
    if (ulSDSType == SDS_TYPE_MM_SMS)
    {
        return TRUE;
    }

    return FALSE;
}

AnsiString GetUserCallStatus(UINT32 ulStatus)
{
    if (ulStatus == USER_STATUS_UNKNOWN             ) return "δ֪";
    if (ulStatus == USER_STATUS_REGOK               ) return "ע��ɹ�";
    if (ulStatus == USER_STATUS_REGFAIL             ) return "ע��ʧ��";
    if (ulStatus == USER_STATUS_INVITING            ) return "������";
    if (ulStatus == USER_STATUS_RINGING             ) return "������";
    if (ulStatus == USER_STATUS_TXMTING             ) return "ͨ����";
    if (ulStatus == USER_STATUS_TXMTIDLE            ) return "����";
    if (ulStatus == USER_STATUS_HALFVOICE_ACTIVATED ) return "���ͨ����";
    if (ulStatus == USER_STATUS_HALFVOICE_CLOSED    ) return "����";

    return "";
}

} // namespace eSDKData

