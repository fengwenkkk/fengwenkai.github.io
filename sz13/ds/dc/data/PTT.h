#ifndef PTT_H
#define PTT_H

#include "g_include.h"

namespace PTT
{
// 发起组呼
BOOL Dial(INT64 i64GroupID);

// 结束组呼
BOOL Close(INT64 i64GroupID);

/**
 * @brief           组呼申请话权
 * @param           i64GroupID [in]
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
BOOL Request(INT64 i64GroupID);

/**
 * @brief           组呼释放话权
 * @param           strGroupMDN    [in]
 * @return          BOOL
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
BOOL Release();//INT64 i64GroupID);

BOOL SetVolume(INT64 i64GroupID, UINT32 ulVolumePercent);
}  // namespace PTT

#endif
