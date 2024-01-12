#include "ClientInclude.h"
#include "AudioDev.h"
#include "P2P.h"

// 以下函数在eSDK中定义，但不对外公开，目的是强制业务层不直接调用eSDK_前缀的函数
extern BOOL eSDK_P2PDial(UINT32 ulUserID);
extern BOOL eSDK_P2PRecv(UINT32 ulUserID);
extern BOOL eSDK_P2PBreakin(UINT32 ulUserID);
extern BOOL eSDK_P2PHangup(UINT32 ulUserID);

extern BOOL eSDK_StartVideoCall(UINT32 ulUserID, UINT32 ulVideoFormat, UINT32 ulCameraType, BOOL bNeedConfirm, BOOL bHasVoice);
extern BOOL eSDK_StartRealPlay(UINT32 ulUserID, UINT32 ulVideoFormat, UINT32 ulCameraType, BOOL bNeedConfirm, BOOL bHasVoice);
extern BOOL eSDK_RecvVideoPlay(UINT32 ulUserID);
extern BOOL eSDK_StopRealPlay(UINT32 ulUserID);

namespace Audio
{
static UINT32 s_ulP2PCallingUserID = INVALID_USER_ID;

VOID AssignUserSpeakerAndMic(UINT32 ulUserID)
{
    if (ulUserID == INVALID_USER_ID)
    {
        return;
    }

    s_ulP2PCallingUserID = ulUserID;
    AssignUserSpeaker(ulUserID, GetESDKMainSpeaker());
    AssignUserMic(ulUserID, GetESDKMainMic());
}

VOID SwitchUserSpeakerAndMic()
{
    AssignUserSpeakerAndMic(s_ulP2PCallingUserID);
}

}

namespace P2P
{
static bool         s_bIsCalling = false;               // 点对点语音业务是否存在
static UINT32       s_ulPeerUserID;                     // 对端用户号码
static P2P_TYPE_E   s_eP2PType = P2P::TYPE_INVALID;     // 点对点类型

BOOL IsP2PCallOver(UINT32 ulStatus)
{
    if (ulStatus == CALL_STATUS_RECVED)     return FALSE;
    if (ulStatus == CALL_STATUS_RECV_SUCC)  return FALSE;
    if (ulStatus == CALL_STATUS_HANGUP)     return TRUE;
    if (ulStatus == CALL_STATUS_CLOSE)      return TRUE;
    if (ulStatus == CALL_STATUS_REJECT)     return TRUE;
    if (ulStatus == CALL_STATUS_BUSY)       return TRUE;
    if (ulStatus == CALL_STATUS_NOANSWER)   return TRUE;

    return FALSE;
}

bool CallOver(UINT32 ulUserID)
{
    if (!s_bIsCalling)
    {
        s_ulPeerUserID = INVALID_USER_ID;
        s_eP2PType = P2P::TYPE_INVALID;

        return true;
    }

    if (ulUserID != s_ulPeerUserID)
    {
        GosLog(LOG_ERROR, "close unknown P2P call of %u", ulUserID);
        return false;
    }

    GosLog(LOG_INFO, "P2P call is end! peer is %u", ulUserID);

    s_bIsCalling = false;
    s_ulPeerUserID = INVALID_USER_ID;
    s_eP2PType = P2P::TYPE_INVALID;

    return true;
}

void CallOver()
{
    s_bIsCalling = false;
    s_ulPeerUserID = INVALID_USER_ID;
    s_eP2PType = P2P::TYPE_INVALID;
}

bool IsCalling()
{
    return s_bIsCalling;
}

void SetCalling()
{
    s_bIsCalling = true;
}

UINT32 GetPeerUserID()
{
    if (IsCalling())
    {
        return s_ulPeerUserID;
    }
    else
    {
        return INVALID_USER_ID;
    }
}

BOOL Dial(UINT32 ulUserID)
{
    if (s_bIsCalling)
    {
        GosLog(LOG_ERROR, "P2P is already exist!");
        return FALSE;
    }

    if (!eSDK_P2PDial(ulUserID))
    {
        return FALSE;
    }

    Audio::AssignUserSpeakerAndMic(ulUserID);

    // 保存呼叫信息
    s_bIsCalling = true;
    s_ulPeerUserID = ulUserID;
    s_eP2PType = TYPE_CALL_OUT;

    return TRUE;
}

// 接听
BOOL Recv(UINT32 ulUserID)
{
    if (!eSDK_P2PRecv(ulUserID))
    {
        GosLog(LOG_ERROR, "P2P accept failed, user is %u", ulUserID);
        return FALSE;
    }

    Audio::AssignUserSpeakerAndMic(ulUserID);

    // 保存呼叫信息
    s_bIsCalling = true;
    s_ulPeerUserID = ulUserID;
    s_eP2PType = TYPE_CALL_IN;

    return TRUE;
}

BOOL Breakin(UINT32 ulUserID)
{
    if (s_bIsCalling)
    {
        return FALSE;
    }

    if (!eSDK_P2PBreakin(ulUserID))
    {
        return FALSE;
    }

    Audio::AssignUserSpeakerAndMic(ulUserID);

    // 保存呼叫信息
    s_bIsCalling = true;
    s_ulPeerUserID = ulUserID;
    s_eP2PType = TYPE_CALL_OUT;

    return TRUE;
}

// 挂断
BOOL Hangup()
{
    if (!s_bIsCalling)
    {
        return TRUE;
    }

    if (s_ulPeerUserID != INVALID_USER_ID)
    {
        if (!eSDK_P2PHangup(s_ulPeerUserID) ||
            !eSDK_P2PBreakoff(s_ulPeerUserID))
        {
            return FALSE;
        }

        AssignUserSpeaker(s_ulPeerUserID, GetESDKMonSpeaker());
    }

    Audio::s_ulP2PCallingUserID = INVALID_USER_ID;

    CallOver();

    return TRUE;
}

BOOL Hangup(UINT32 ulUserID)
{
    if (!s_bIsCalling)
    {
        return TRUE;
    }

    if (ulUserID != s_ulPeerUserID)
    {
        return FALSE;
    }

    if (ulUserID != INVALID_USER_ID)
    {
        if (!eSDK_P2PHangup(ulUserID) ||
            !eSDK_P2PBreakoff(ulUserID))
        {
            return FALSE;
        }

        AssignUserSpeaker(ulUserID, GetESDKMonSpeaker());
    }

    Audio::s_ulP2PCallingUserID = INVALID_USER_ID;

    CallOver();

    return TRUE;
}

// 拒接
BOOL Reject(UINT32 ulPeerUserID)
{
/*    if (!BusinessData::P2P::IsCalling())
    {
        GosLog(LOG_ERROR, "P2P Business doesn't exist!");
        return FALSE;
    }

    UINT32  ulPeerUserID = BusinessData::P2P::GetPeerUserID(); */

    return eSDK_P2PReject(ulPeerUserID);
}

// 呼叫转接
BOOL CallTransfer(UINT32 ulTransferUserID)
{
    if (!s_bIsCalling)
    {
        return FALSE;
    }

    if (s_ulPeerUserID == INVALID_USER_ID)
    {
        GosLog(LOG_ERROR, "P2P is not exist! call transfer to %u failed", ulTransferUserID);
        return FALSE;
    }

    if (!eSDK_P2PTransfer(s_ulPeerUserID, ulTransferUserID) )
    {
        return FALSE;
    }

    CallOver(s_ulPeerUserID);
    return TRUE;
}

// 视频点呼
BOOL VideoCall(UINT32 ulUserID, UINT32 ulVideoFormat, UINT32 ulCameraType, BOOL bNeedConfirm, BOOL bHasVoice)
{
    if (bHasVoice && s_bIsCalling)
    {
        GosLog(LOG_ERROR, "P2P is already exist!");
        return FALSE;
    }

    if (!eSDK_StartVideoCall(ulUserID, ulVideoFormat, ulCameraType, bNeedConfirm, bHasVoice))
    {
        return FALSE;
    }

    if (bHasVoice)
    {
        Audio::AssignUserSpeakerAndMic(ulUserID);
    }

    // 保存呼叫信息
    s_ulPeerUserID = ulUserID;

    if (bHasVoice)
    {
        s_bIsCalling = true;
    }
    else
    {
        s_bIsCalling = false;
    }

    s_eP2PType = TYPE_CALL_OUT;

    return TRUE;
}

// 主动发起的视频上拉
BOOL PullVideo(UINT32 ulUserID, UINT32 ulVideoFormat, UINT32 ulCameraType, BOOL bNeedConfirm, BOOL bHasVoice)
{
    if (bHasVoice && s_bIsCalling)
    {
        GosLog(LOG_ERROR, "P2P is already exist!");
        return FALSE;
    }

    if (!eSDK_StartRealPlay(ulUserID, ulVideoFormat, ulCameraType, bNeedConfirm, bHasVoice))
    {
        return FALSE;
    }

    if (bHasVoice)
    {
        Audio::AssignUserSpeakerAndMic(ulUserID);
    }

    // 保存呼叫信息
    s_ulPeerUserID = ulUserID;

    if (bHasVoice)
    {
        s_bIsCalling = true;
    }
    else
    {
        s_bIsCalling = false;
    }

    s_eP2PType = TYPE_CALL_OUT;

    return TRUE;
}

BOOL RecvVideo(UINT32 ulUserID)
{
    if (!eSDK_RecvVideoPlay(ulUserID))
    {
        return FALSE;
    }

    Audio::AssignUserSpeakerAndMic(ulUserID);

    return TRUE;
}

BOOL StopVideo(UINT32 ulUserID)
{
    if (ulUserID != INVALID_USER_ID &&
        !eSDK_StopRealPlay(ulUserID))
    {
        GosLog(LOG_ERROR, "stop video of %u failed", ulUserID);

        // 此处不返回，因为视频窗口将强制关闭
//        return FALSE;
    }

    CallOver(ulUserID);

    return TRUE;
}

BOOL RejectVideo(UINT32 ulUserID)
{
    return eSDK_P2PVideoReject(ulUserID);
}

BOOL SetVolume(UINT32 ulVolumePercent)
{
    if (s_ulPeerUserID == INVALID_USER_ID)
    {
        return FALSE;
    }

    return eSDK_VolumeControl(s_ulPeerUserID, ulVolumePercent);
}


}  // namespace P2P

