#ifndef PTT_H
#define PTT_H

#include "g_include.h"

namespace PTT
{
// �������
BOOL Dial(INT64 i64GroupID);

// �������
BOOL Close(INT64 i64GroupID);

/**
 * @brief           ������뻰Ȩ
 * @param           i64GroupID [in]
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
BOOL Request(INT64 i64GroupID);

/**
 * @brief           ����ͷŻ�Ȩ
 * @param           strGroupMDN    [in]
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
BOOL Release();//INT64 i64GroupID);

BOOL SetVolume(INT64 i64GroupID, UINT32 ulVolumePercent);
}  // namespace PTT

#endif
