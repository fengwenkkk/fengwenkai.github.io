/**
 * @file            thread_utility.hpp
 * @brief           跟线程有关的工具
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2022-02-22 18:32:09
 * @copyright       Copyright (c) 2021-2022 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_THREAD_THREAD_UTILITY_HPP__
#define DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_THREAD_THREAD_UTILITY_HPP__

#include "gos_utility/detail/macro_switch.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include "unistd.h"
#include "sys/sysinfo.h"
#endif

#ifdef __GOS_CXX11_AFTER__
#include <thread>
#endif

namespace gos
{

/**
 * @brief           获取当前CPU支持并发的线程数
 * @return          unsigned
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-02-22 19:16:14
 * @note
 * windows 下使用CPU数目替代
 */
inline unsigned hardware_concurrency()
{
#ifndef __GOS_CXX11_AFTER__
#ifdef _WIN32
    /// Window 系统, C++11 以前
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return static_cast<unsigned>(sysInfo.dwNumberOfProcessors);
#else
    /// Linux 系统, C++11 以前
    return static_cast<unsigned>(sysconf(_SC_NPROCESSORS_CONF));
#endif
#else
    /// STL 跨平台实现: C++11 以后
    return std::thread::hardware_concurrency();
#endif
}
}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_THREAD_THREAD_UTILITY_HPP__