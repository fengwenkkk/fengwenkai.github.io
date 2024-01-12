/**
 * @file            object_live_time.hpp
 * @brief           对象生存时间对象
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2022-02-24 18:12:33
 * @copyright       Copyright (c) 2021-2022 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_DEBUG_UTILITY_OBJECT_LIVE_TIME_HPP__
#define DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_DEBUG_UTILITY_OBJECT_LIVE_TIME_HPP__

#include "gos_utility/detail/gos_log/log_stream.hpp"
#include "gos_utility/detail/debug_utility/interval_time.hpp"

#ifdef __GOS_USE_VS__
#pragma warning(push)
#pragma warning(disable : 4355)
#endif

namespace gos
{
/**
 * @brief           记录某对象的存活时间, 析构函数自动打印
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-02-14 11:11:21
 * @note
 */
class object_live_time : protected interval_time
{
public:
    object_live_time() : interval_time(gos::to_string(this))
    {
        start();
    }

    ~object_live_time()
    {
        finish();
        LOGD << report();
    }
};
}  // namespace gos

#ifdef __GOS_USE_VS__
#pragma warning(pop)
#endif

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_DEBUG_UTILITY_OBJECT_LIVE_TIME_HPP__