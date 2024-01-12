#include "ClientInclude.h"
#include "AudioDev.h"
#include "PTT.h"

// ���º�����eSDK�ж��壬�������⹫����Ŀ����ǿ��ҵ��㲻ֱ�ӵ���eSDK_ǰ׺�ĺ���
extern BOOL eSDK_PTTDial(INT64 i64GroupID, BOOL bRetry);
extern BOOL eSDK_PTTRelease(INT64 i64GroupID);
extern BOOL eSDK_PTTDialMultiGroup(INT64 i64GroupID);
extern BOOL eSDK_PTTReleaseMultiGroup(INT64 i64GroupID);

extern void UpdateMainGroupCall(INT64 i64GroupID = INVALID_GROUP_ID);

namespace PTT
{
// �������
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

// �����Ȩ
BOOL Request(INT64 i64GroupID)
{
    return Dial(i64GroupID);
}

// �ͷŻ�Ȩ
BOOL Release()//INT64 i64GroupID)
{
    // ��Ų���PTTDial����������
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

    // ͨ�������ʱ���޻�Ȩ����״̬������̨��Ȩ�ͷ�ʱ�Զ�ǿ��
    if (AutoCloseMulticastCall())
    {
        if (GroupCfg::IsMulticastGroup(i64GroupID))
        {
            eSDK_GroupBreakoff(i64GroupID);
        }
    }

    return bRet;
}

// �������
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
