/**
 * @file            interval_time_singleton_factory.hpp
 * @brief           记录某类型的对象，每个对象的存活时间
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2022-02-24 18:18:10
 * @copyright       Copyright (c) 2021-2022 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_DEBUG_UTILITY_INTERVAL_TIME_SINGLETON_FACTORY_HPP__
#define DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_DEBUG_UTILITY_INTERVAL_TIME_SINGLETON_FACTORY_HPP__

#include <map>
#include <string>
#include "gos_utility/detail/singleton.hpp"
#include "gos_utility/detail/gos_log/log_stream.hpp"
#include "gos_utility/detail/debug_utility/interval_time.hpp"

namespace gos
{
/**
 * @brief           记录某类型的对象，每个对象的存活时间
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-02-14 11:41:15
 * @note
 */
class interval_time_singleton_factory : public singleton<interval_time_singleton_factory>
{
    friend class singleton<interval_time_singleton_factory>;

public:
    /**
     * @brief           开始记录某对象存活时间
     * @param           p    [in]   指向该对象的指针
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2022-02-14 11:42:48
     * @note
     */
    template <typename T>
    void start(T* const p)
    {
        std::string str = gos::to_string(p);
        interval_time timer(str);
        timer.start();
        m_interval_time[str] = timer;
    }

    /**
     * @brief           结束记录对象存活时间，并打印
     * @param           p    [in]   指向该对象的指针
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2022-02-14 11:43:32
     * @note
     * lijiancong(2022-02-14): 该结果有 us 级别的微小误差，但具体多少仍未测量
     */
    template <typename T>
    void finish(T* const p)
    {
        std::string strKey(gos::to_string(p));
        if (m_interval_time.find(strKey) != m_interval_time.end())
        {
            m_interval_time[strKey].finish();
            LOGD << m_interval_time[strKey].report();
            m_interval_time.erase(strKey);
        }
    }

private:
    std::map<std::string, interval_time> m_interval_time;

    interval_time_singleton_factory() {}
};
}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_DEBUG_UTILITY_INTERVAL_TIME_SINGLETON_FACTORY_HPP__