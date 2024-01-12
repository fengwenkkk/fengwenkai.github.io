/**
 * @file            interval_time.hpp
 * @brief           间隔时间记录类
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2022-02-24 18:10:42
 * @copyright       Copyright (c) 2021-2022 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_DEBUG_UTILITY_INTERVAL_TIME_HPP__
#define DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_DEBUG_UTILITY_INTERVAL_TIME_HPP__

#include "gos_utility/detail/gos_log/log_stream.hpp"
#include "gos_utility/detail/debug_utility/tick_count.hpp"

namespace gos
{
/**
 * @brief           记录间隔时间的对象
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-02-14 14:34:20
 * @note
 */
class interval_time : public tick_count
{
public:
    interval_time() {}
    interval_time(const std::string& strHintInfo) : m_strHintInfo(strHintInfo) {}

    interval_time(const interval_time& stOther) : tick_count(stOther)
    {
        m_strHintInfo = stOther.m_strHintInfo;
    }

    interval_time& operator=(const interval_time& stOther)
    {
        if (this != &stOther)
        {
            tick_count::operator=(stOther);
            m_strHintInfo = stOther.m_strHintInfo;
        }
        return *this;
    }

    std::string report()
    {
        std::string strRet;
        if (!m_strHintInfo.empty())
        {
            strRet += m_strHintInfo + " ";
        }

        strRet += "interval time: " + get_time_string();
        return strRet;
    }

private:
    std::string m_strHintInfo;
};
}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_DEBUG_UTILITY_INTERVAL_TIME_HPP_