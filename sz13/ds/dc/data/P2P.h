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
/// ��Ե�ҵ�����Ͷ���
enum P2P_TYPE_E
{
    TYPE_INVALID = -1,  ///< ��Ч����
    TYPE_CALL_OUT = 1,  ///< ����
    TYPE_CALL_IN = 2,   ///< ����
};

/// ҵ��״̬, Ϊҵ����Ƶ�״̬���������ͨ��״̬�޹�
enum BUSINESS_STATUS
{
    INVALID_STATUS = -1,  ///< ��Ч״̬
    SPEAKING_STATUS = 1,  ///< ������֪ͨ,ͨ���ѽ���
    HANG_UP_STATUS = 2,   ///< �Ҷϵȴ��������ص�
    REJECT_STATUS = 3,    ///< �ܽӵȴ��������ص�
    TRANFER_STATUS = 4,   ///< ת�ӵȴ��������ص�
};

BOOL IsP2PCallOver(UINT32 ulStatus);

bool IsCalling();
void SetCalling();
UINT32 GetPeerUserID();
bool CallOver(UINT32 ulUserID);
void CallOver();

/**
 * @brief           ����������������
 * @param           ulPeerUserShortNum [in] ��Ҫ�������û��Ķ̺���
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
BOOL Dial(UINT32 ulPeerUserID);

/**
 * @brief           ����ܽ�
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
BOOL Reject(UINT32 ulPeerUserID);

/**
 * @brief           ��������Ҷ�
 * @param           ulCallingNumber [in]    ����ĶԶ˺��룬 ����У��Ҷϵĵ���Ƿ���ȷ
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-08-30 10:51:48
 * @note
 */
BOOL Hangup();
BOOL Hangup(UINT32 ulUserID);

/**
 * @brief           ������Ե�ͨ��
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-08-27 18:08:41
 * @note
 */
BOOL Recv(UINT32 ulUserID);

/**
 * @brief           ���ת��
 * @param           strMDN          [in]    Ҫת�ӵ�����̨����
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
BOOL CallTransfer(UINT32 ulTRansferUserID);

/**
 * @brief           ǿ�� breakin
 * @param           ulUserID           [in]     ���ͨ����UserID
 * @param           stReturn           [out]    SDK���ص�ǿ��������
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-08-25 13:29:25
 * @note
 */
BOOL CallBreakIn(UINT32 ulUserID);

/**
 * @brief           ���õ������
 * @param           iVolumePercent          [in]  �����ٷֱ� 0~100
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
