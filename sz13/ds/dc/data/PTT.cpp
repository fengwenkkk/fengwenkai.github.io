#include "ClientInclude.h"
#include "AudioDev.h"
#include "PTT.h"

// 以下函数在eSDK中定义，但不对外公开，目的是强制业务层不直接调用eSDK_前缀的函数
extern BOOL eSDK_PTTDial(INT64 i64GroupID, BOOL bRetry);
extern BOOL eSDK_PTTRelease(INT64 i64GroupID);
extern BOOL eSDK_PTTDialMultiGroup(INT64 i64GroupID);
extern BOOL eSDK_PTTReleaseMultiGroup(INT64 i64GroupID);

extern void UpdateMainGroupCall(INT64 i64GroupID = INVALID_GROUP_ID);

namespace PTT
{
// 组呼发起
BOOL Dial(INT64 i64GroupID)
{
    GROUP_INFO_T    *pstGroup = Group::GetGroup(i64GroupID);
    BOOL            bRet;

    if (pstGroup && pstGroup->ulGroupType == MULTI_GROUP)
    {
        bRet = eSDK_PTTDialMultiGroup(i64GroupID);
    }
    else
    {
        bRet = eSDK_PTTDial(i64GroupID, TRUE);
    }

    if (bRet)
    {
        Group::SetMainGroupID(i64GroupID);
        UpdateMainGroupCall(i64GroupID);
    }

    return bRet;
}

// 组呼抢权
BOOL Request(INT64 i64GroupID)
{
    return Dial(i64GroupID);
}

// 释放话权
BOOL Release()//INT64 i64GroupID)
{
    // 组号采用PTTDial后产生的组号
    INT64   i64GroupID = Group::GetMainGroupID();
    BOOL    bRet;

    if (i64GroupID == INVALID_GROUP_ID)
    {
        return TRUE;
    }

    GROUP_INFO_T    *pstGroup = Group::GetGroup(i64GroupID);

    if (pstGroup && pstGroup->ulGroupType == MULTI_GROUP)
    {
        bRet = eSDK_PTTReleaseMultiGroup(i64GroupID);
    }
    else
    {
        bRet = eSDK_PTTRelease(i64GroupID);
    }

    if (!bRet)
    {
        GosLog(LOG_ERROR, "group " FORMAT_I64 " ptt release failed", i64GroupID);
    }

    // 通播组呼叫时，无话权空闲状态，调度台话权释放时自动强拆
    if (AutoCloseMulticastCall())
    {
        if (GroupCfg::IsMulticastGroup(i64GroupID))
        {
            eSDK_GroupBreakoff(i64GroupID);
        }
    }

    return bRet;
}

// 组呼结束
BOOL Close(INT64 i64GroupID)
{
    return TRUE;
}

BOOL SetVolume(INT64 i64GroupID, UINT32 ulVolumePercent)
{
    BOOL    bRet = eSDK_VolumeControl(i64GroupID, ulVolumePercent);

    if (!bRet)
    {
        GosLog(LOG_ERROR, "set group " FORMAT_I64 " volumn failed", i64GroupID);
    }

    return bRet;
}

}  // namespace PTT
