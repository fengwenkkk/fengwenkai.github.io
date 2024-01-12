#ifndef P2P_H
#define P2P_H

#include "g_include.h"

namespace Audio
{
VOID AssignUserSpeakerAndMic(UINT32 ulUserID);
VOID SwitchUserSpeakerAndMic();
}

namespace P2P
{
/// 点对点业务类型定义
enum P2P_TYPE_E
{
    TYPE_INVALID = -1,  ///< 无效类型
    TYPE_CALL_OUT = 1,  ///< 呼出
    TYPE_CALL_IN = 2,   ///< 呼入
};

/// 业务状态, 为业务设计的状态与核心网的通话状态无关
enum BUSINESS_STATUS
{
    INVALID_STATUS = -1,  ///< 无效状态
    SPEAKING_STATUS = 1,  ///< 核心网通知,通话已建立
    HANG_UP_STATUS = 2,   ///< 挂断等待核心网回调
    REJECT_STATUS = 3,    ///< 拒接等待核心网回调
    TRANFER_STATUS = 4,   ///< 转接等待核心网回调
};

BOOL IsP2PCallOver(UINT32 ulStatus);

bool IsCalling();
void SetCalling();
UINT32 GetPeerUserID();
bool CallOver(UINT32 ulUserID);
void CallOver();

/**
 * @brief           主动发起语音呼出
 * @param           ulPeerUserShortNum [in] 需要呼出的用户的短号码
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
BOOL Dial(UINT32 ulPeerUserID);

/**
 * @brief           点呼拒接
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
BOOL Reject(UINT32 ulPeerUserID);

/**
 * @brief           语音点呼挂断
 * @param           ulCallingNumber [in]    传入的对端号码， 用于校验挂断的点呼是否正确
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-08-30 10:51:48
 * @note
 */
BOOL Hangup();
BOOL Hangup(UINT32 ulUserID);

/**
 * @brief           接听点对点通话
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-08-27 18:08:41
 * @note
 */
BOOL Recv(UINT32 ulUserID);

/**
 * @brief           点呼转接
 * @param           strMDN          [in]    要转接到的手台号码
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
BOOL CallTransfer(UINT32 ulTRansferUserID);

/**
 * @brief           强插 breakin
 * @param           ulUserID           [in]     这个通话的UserID
 * @param           stReturn           [out]    SDK返回的强插操作结果
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-08-25 13:29:25
 * @note
 */
BOOL CallBreakIn(UINT32 ulUserID);

/**
 * @brief           设置点呼音量
 * @param           iVolumePercent          [in]  音量百分比 0~100
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-08 15:33:33
 * @note
 */
BOOL SetVolume(UINT32 ulVolumePercent);

BOOL Breakin(UINT32 ulUserID);

BOOL VideoCall(UINT32 ulUserID, UINT32 ulVideoFormat, UINT32 ulCameraType, BOOL bNeedConfirm, BOOL bHasVoice);
BOOL PullVideo(UINT32 ulUserID, UINT32 ulVideoFormat, UINT32 ulCameraType, BOOL bNeedConfirm, BOOL bHasVoice);
BOOL RecvVideo(UINT32 ulUserID);
BOOL StopVideo(UINT32 ulUserID);
BOOL RejectVideo(UINT32 ulUserID);

}  // namespace P2P

#endif
