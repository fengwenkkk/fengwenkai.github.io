#include "ClientInclude.h"

namespace eSDKData
{

static UINT32 SDS_TYPE_NORMAL_SMS  = 1;     // 可以点对点或发群组短信
static UINT32 SDS_TYPE_STATUS_SMS  = 2;     // 终端群发给调度台的状态短信
static UINT32 SDS_TYPE_MM_SMS      = 4;     // 可以点对点或发群组短信或者彩信

static UINT32 SDS_TYPE_P2P_IM      = 201;   // 对点发即时消息。
static UINT32 SDS_TYPE_GROUP_IM    = 202;   // 发群组即时消息。
static UINT32 SDS_TYPE_IM_REPLY    = 203;   // 发送即时消息回执

static ID_VALUE_T   s_astSDSType[] = {
                        {SDS_TYPE_NORMAL_SMS,   "短消息"},
                        {SDS_TYPE_STATUS_SMS,   "状态消息"},
                        {SDS_TYPE_MM_SMS,       "彩信"},
                        {SDS_TYPE_P2P_IM,       "电子工单"},
                        {SDS_TYPE_GROUP_IM,     "电子工单"},
                        {SDS_TYPE_IM_REPLY,     "工单回执"} };


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
    if (ulStatus == USER_STATUS_UNKNOWN             ) return "未知";
    if (ulStatus == USER_STATUS_REGOK               ) return "注册成功";
    if (ulStatus == USER_STATUS_REGFAIL             ) return "注册失败";
    if (ulStatus == USER_STATUS_INVITING            ) return "呼叫中";
    if (ulStatus == USER_STATUS_RINGING             ) return "振铃中";
    if (ulStatus == USER_STATUS_TXMTING             ) return "通话中";
    if (ulStatus == USER_STATUS_TXMTIDLE            ) return "空闲";
    if (ulStatus == USER_STATUS_HALFVOICE_ACTIVATED ) return "组呼通话中";
    if (ulStatus == USER_STATUS_HALFVOICE_CLOSED    ) return "空闲";

    return "";
}

} // namespace eSDKData

