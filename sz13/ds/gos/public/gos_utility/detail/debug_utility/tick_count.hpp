/**
 * @file            tick_count.hpp
 * @brief           生存时间相关工具对象实现
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2022-02-14 10:51:55
 * @copyright       Copyright (c) 2021-2022 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_TICK_COUNT_HPP__
#define DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_TICK_COUNT_HPP__

#include <string>
#include <map>

#include "gos_utility/detail/macro_switch.hpp"
#include "gos_utility/detail/algorithm_utility.hpp"
#include "gos_utility/detail/string_utility.hpp"
#include "gos_utility/detail/time_utility.hpp"
#include "gos_utility/detail/gos_log/log_stream.hpp"
#include "gos_utility/detail/singleton.hpp"

namespace gos
{
/**
 * @brief           记录时间间隔的对象
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-02-14 10:50:43
 * @note
 */
class tick_count
{
public:
    tick_count() : m_StartTime(0), m_FinishTime(0) {}
    tick_count(const tick_count& Other)
    {
        m_StartTime = Other.m_StartTime;
        m_FinishTime = Other.m_FinishTime;
    }

    tick_count& operator=(const tick_count& Other)
    {
        if (this != &Other)
        {
            m_StartTime = Other.m_StartTime;
            m_FinishTime = Other.m_FinishTime;
        }

        return *this;
    }

    /// 记录开始时间
    void start()
    {
        m_StartTime = gos::timer::GetInstance().get_current_ticks();
    }

    /// 记录结束时间
    void finish()
    {
        m_FinishTime = gos::timer::GetInstance().get_current_ticks();
    }

    /// 获取间隔时间(单位为 us)
    uint64_t get_us() const
    {
        return m_FinishTime - m_StartTime;
    }

    /// 获取间隔时间(单位为 ms)
    uint64_t get_ms() const
    {
        return get_us() / 1000;
    }

    /// 获取间隔时间(单位为 s)
    double get_s() const
    {
        return static_cast<double>(get_us() / 1000000.0);
    }

    /// 获取间隔时间字符串
    std::string get_time_string() const
    {
        std::string strTime;
        uint64_t u64Time = get_us();
        if (u64Time < 1000)
        {
            strTime += gos::to_string(get_us()) + " (us)";
        }
        else if (u64Time < 1000 * 1000)
        {
            strTime += gos::to_string(get_ms()) + " (ms)";
        }
        else
        {
            strTime += gos::to_string(get_s()) + " (s)";
        }
        return strTime;
    }

    void clear()
    {
        m_StartTime = 0;
        m_FinishTime = 0;
    }

private:
    uint64_t m_StartTime;
    uint64_t m_FinishTime;
};

}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_TICK_COUNT_HPP__