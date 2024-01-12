/**
 * @file            time_utility.hpp
 * @brief           用于存放时间相关的工具函数
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-04-13 10:34:56
 *
 * @copyright       Copyright (c) 2021 GBCOM
 *
 */

#ifndef GOS_UTILITY_TIME_UTILITY_HPP__
#define GOS_UTILITY_TIME_UTILITY_HPP__

#include <time.h>
#include "gos_utility/detail/string_utility.hpp"
#include "gos_utility/detail/singleton.hpp"

#ifndef _WIN32
#include <sys/time.h>
#else
#include <windows.h>
#include <winsock2.h>
#include <time.h>
#pragma comment(lib, "winmm.lib")
#endif

namespace gos
{
/**
 * @brief           获取当前毫秒级时间戳
 * @return          unsigned long long
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
inline unsigned long long get_current_time_ms()
{
    struct timeval tv;

    gos_gettimeofday(&tv);

    return ((unsigned long long)tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

/**
 * @brief           获取当前秒级时间戳
 * @return          int
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
inline UINT32 get_current_time_stamp()
{
    return time(NULL);
}

/**
 * @brief           获取编译时间的时间戳
 * @return          int
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
inline unsigned get_compile_time_stamp()
{
    time_t compile_time_s = 0;  ///< 单位为秒
    tm tm;
    tm.tm_min = 0;
    tm.tm_mon = 0;
    tm.tm_sec = 0;
    tm.tm_hour = 0;
    tm.tm_mday = 0;
    tm.tm_wday = 0;
    tm.tm_yday = 0;
    tm.tm_year = 0;
    /// tm.tm_zone = 0;
    tm.tm_isdst = 0;
    /// tm.tm_gmtoff = 0;
    char Mmm[4] = {0};
    strcpy(Mmm, "Jan");
    sscanf(__DATE__, "%3s %d %d", Mmm, &tm.tm_mday, &tm.tm_year);

    std::string Month(Mmm);
    if (strcmp(Mmm, "Jan") == 0)
    {
        tm.tm_mon = 1;
    }
    else if (strcmp(Mmm, "Feb") == 0)
    {
        tm.tm_mon = 2;
    }
    else if (strcmp(Mmm, "Mar") == 0)
    {
        tm.tm_mon = 3;
    }
    else if (strcmp(Mmm, "Apr") == 0)
    {
        tm.tm_mon = 4;
    }
    else if (strcmp(Mmm, "May") == 0)
    {
        tm.tm_mon = 5;
    }
    else if (strcmp(Mmm, "Jun") == 0)
    {
        tm.tm_mon = 6;
    }
    else if (strcmp(Mmm, "Jul") == 0)
    {
        tm.tm_mon = 7;
    }
    else if (strcmp(Mmm, "Aug") == 0)
    {
        tm.tm_mon = 8;
    }
    else if (strcmp(Mmm, "Sep") == 0)
    {
        tm.tm_mon = 9;
    }
    else if (strcmp(Mmm, "Oct") == 0)
    {
        tm.tm_mon = 10;
    }
    else if (strcmp(Mmm, "Nov") == 0)
    {
        tm.tm_mon = 11;
    }
    else if (strcmp(Mmm, "Dec") == 0)
    {
        tm.tm_mon = 12;
    }
    else
    {
        tm.tm_mon = 0;
    }

    sscanf(__TIME__, "%d:%d:%d", &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
    compile_time_s = mktime(&tm);
    if (compile_time_s <= 0 || (unsigned)compile_time_s > gos::get_current_time_stamp())
    {
        /// std::cout << "get_compile_time_stamp() is failed!" << std::endl;
        compile_time_s = -1;
    }
    return static_cast<unsigned>(compile_time_s);
}

/**
 * @brief           转换时间戳到字符串
 * @param           Time        [in]    时间戳
 * @param           Format      [in]    格式字符串
 * @return          std::string
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
inline std::string get_time_string(UINT32 ulTime, const char *Format = "%Y-%m-%d %H:%M:%S")
{
    time_t t = ulTime;
    tm buf;
#ifdef _WIN32
#ifdef _BCB_
    localtime_s(&t, &buf);
#else
    localtime_s(&buf, &t);
#endif
#else
    localtime_r(&t, &buf);
#endif
    char p[32] = {0};
    strftime(p, sizeof(p), Format, &buf);
    return std::string(p);
}

inline std::string get_time_string()
{
    return get_time_string(gos::get_current_time_stamp());
}

/**
 * @brief           获取毫秒级时间戳的格式化文本
 * @param           time    [in] 毫秒级时间戳
 * @return          std::string
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
inline std::string get_time_ms_string(const long long time)
{
    return get_time_string(time / 1000) + "." + gos::to_string(time % 1000);
}

/**
 * @brief           获取当前毫秒级时间格式化文本
 * @return          std::string
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
inline std::string get_current_time_ms_string()
{
    return get_time_ms_string(get_current_time_ms());
}

class timer : public gos::singleton<timer>
{
public:
    uint64_t get_current_ticks()
    {
        gos::lock_guard<gos::mutex> Guard(mutex);
        return get_current_ticks_impl();
    }

private:
    gos::mutex mutex;
    /**
     * @brief           获取系统启动(开机时间)到现在的 tick count, 精度为us
     * @return          uint64_t
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2022-02-10 10:41:42
     * @note
     * 参考资料:
     * https://github.com/catchorg/Catch2/blob/Catch1.x/include/internal/catch_timer.hpp
     */
    inline uint64_t get_current_ticks_impl()
    {
#ifdef _WIN32
        static uint64_t hz = 0, hzo = 0;
        if (!hz)
        {
            QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER *>(&hz));
            QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&hzo));
        }
        uint64_t t;
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&t));
        return ((t - hzo) * 1000000) / hz;
#else
        timeval t;
        gettimeofday(&t, NULL);
        return static_cast<uint64_t>(t.tv_sec) * 1000000ull + static_cast<uint64_t>(t.tv_usec);
#endif
    }
};

inline void sleep_ms(const uint32_t interval)
{
    gos_sleep_1ms(interval);
}

}  // namespace gos

#endif  ///< GOS_UTILITY_TIME_UTILITY_HPP__
