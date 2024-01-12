/**
 * @file            stop_watch.hpp
 * @brief           用于记录多次时间间隔
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2022-02-24 18:22:12
 * @copyright       Copyright (c) 2021-2022 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_DEBUG_UTILITY_STOP_WATCH_HPP__
#define DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_DEBUG_UTILITY_STOP_WATCH_HPP__

#include <vector>
#include "gos_utility/detail/debug_utility/tick_count.hpp"
#include "gos_utility/detail/gos_log/log_stream.hpp"

namespace gos
{
class stop_watch
{
public:
    stop_watch()
    {
        m_tick.start();
    }

    ~stop_watch()
    {
        tick("finish");
        for (uint32_t i = 0; i < m_vecTimeString.size(); ++i)
        {
            LOGD << m_vecTimeString.at(i);
        }
    }

    void tick(const std::string& info = "")
    {
        m_tick.finish();
        std::string str = m_tick.get_time_string() + ": " + info;
        m_vecTimeString.push_back(str);
        m_tick.start();
    }

private:
    tick_count m_tick;                         ///< 计时器
    std::vector<std::string> m_vecTimeString;  ///< 记录时间字符串
};
}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_DEBUG_UTILITY_STOP_WATCH_HPP__