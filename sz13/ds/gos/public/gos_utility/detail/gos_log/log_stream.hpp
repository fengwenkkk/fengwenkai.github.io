/**
 * @file            log_stream.hpp
 * @brief           日志流工具的实现
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-07-28 10:56:51
 *
 * @copyright       Copyright (c) 2021 GBCOM
 *
 */

#ifndef GOS_PUBLIC_GOS_UTILITY_LOG_STREAM_HPP__
#define GOS_PUBLIC_GOS_UTILITY_LOG_STREAM_HPP__

#include "g_include.h"
#include "gos_utility/detail/macro_switch.hpp"
#include "gos_utility/detail/gos_log/log_utility.hpp"
#include "gos_utility/detail/string_stream.hpp"
#include "gos_utility/detail/time_utility.hpp"

namespace gos
{
class log_stream
{
public:
    log_stream(const long long& thread_id, const std::string& file_name, const std::string& func_name, const int line,
               int level)
        : thread_id_(thread_id), file_name_(file_name), func_name_(func_name), line_(line), level_(level)
    {
    }

    ~log_stream()
    {
        std::string log = m_stream.str();
        if (log.empty())
        {
            return;
        }
        else
        {
            std::string strLog = log + gos::get_log_line_info(func_name_, file_name_, line_, thread_id_);
            static Logger* pLogger = NULL;
            if (pLogger == NULL)
            {
                pLogger = GLogFactory::GetInstance()->AddLogger("common", LOG_DETAIL, FALSE, 5, 50 * 1024 * 1024);
            }

            pLogger->Log(level_, strLog);
        }
    }

    /// 拷贝构造函数
    log_stream(const log_stream& other)
        : thread_id_(other.thread_id_),
          file_name_(other.file_name_),
          func_name_(other.func_name_),
          line_(other.line_),
          level_(other.level_),
          m_stream(other.m_stream)
    {
    }

    template <typename T>
    log_stream& operator<<(const T& data)
    {
        m_stream << data;
        return *this;
    }

    /// 获取日志字符串
    std::string str()
    {
        std::string log = m_stream.str();
        if (log.empty())
        {
            return "";
        }
        else
        {
            return log + gos::get_log_line_info(func_name_, file_name_, line_, thread_id_);
        }
    }

private:
    gos::string_stream m_stream;
    const long long thread_id_;
    const std::string file_name_;
    const std::string func_name_;
    const int line_;
    const int level_;
};

/// 空的流对象
class empty_stream
{
public:
    template <typename T>
    empty_stream& operator<<(const T& data)
    {
        (void)data;
        return *this;
    }
};

}  // namespace gos

#ifdef USE_LOG_STREAM_MACRO
#define LOG  gos::log_stream(gos::get_tid(), __FILE__, __FUNCTION__, __LINE__, LOG_INFO)
#define LOGD gos::log_stream(gos::get_tid(), __FILE__, __FUNCTION__, __LINE__, LOG_DETAIL)
#define LOGI gos::log_stream(gos::get_tid(), __FILE__, __FUNCTION__, __LINE__, LOG_INFO)
#define LOGW gos::log_stream(gos::get_tid(), __FILE__, __FUNCTION__, __LINE__, LOG_WARN)
#define LOGE gos::log_stream(gos::get_tid(), __FILE__, __FUNCTION__, __LINE__, LOG_ERROR)
#else
#define LOG  gos::empty_stream()
#define LOGD gos::empty_stream()
#define LOGI gos::empty_stream()
#define LOGW gos::empty_stream()
#define LOGE gos::empty_stream()
#endif

#endif  ///< GOS_PUBLIC_GOS_UTILITY_LOG_STREAM_HPP__