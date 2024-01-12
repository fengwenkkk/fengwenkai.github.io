/**
 * @file            logger.hpp
 * @brief           日志具体实例
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-09-23 16:11:51
 *
 * @copyright       Copyright (c) 2021 GBCOM
 *
 */

#ifndef GOS_PUBLIC_GOS_UTILITY_DETAIL_GOS_LOG_LOGGER_HPP__
#define GOS_PUBLIC_GOS_UTILITY_DETAIL_GOS_LOG_LOGGER_HPP__

#include <string>
#include "gos_utility/detail/gos_log/log_sink.hpp"
#include "gos_utility/detail/gos_log/log_utility.hpp"
#include "gos_utility/detail/thread/mutex_utility.hpp"

namespace gos
{
template <typename Mutex>
class base_logger
{
public:
    base_logger(const std::string& strLoggerName) : logger_name(strLoggerName) {}
    virtual ~base_logger() {}
    void Log(const int log_level, const std::string& strLog)
    {
        for (size_t i = 0; i < vecSink.size(); ++i)
        {
            if (vecSink.at(i)->should_log(log_level))
            {
                vecSink.at(i)->log(log_level, strLog);
            }

            static int s_count = 0;
            if (vecSink.at(i)->should_flush(log_level) || s_count > 1000)
            {
                s_count = 0;
                vecSink.at(i)->flush();
            }
            else
            {
                s_count++;
            }
        }
    }

    void flush_all()
    {
        for (size_t i = 0; i < vecSink.size(); ++i)
        {
            vecSink.at(i)->flush();
        }
    }

protected:
    std::string logger_name;

    void AddSink(base_sink<Mutex>* p)
    {
        vecSink.push_back(p);
    }

private:
    std::vector<base_sink<Mutex>*> vecSink;
};

class empty_logger : public base_logger<gos::mutex>
{
public:
    empty_logger() : base_logger<gos::mutex>("empty") {}
    virtual ~empty_logger() {}
    virtual void Log(int log_level, const std::string& strLog)
    {
        (void)log_level;
        (void)strLog;
    }
};

class detail_logger : public base_logger<gos::mutex>
{
public:
    detail_logger() : base_logger<gos::mutex>("detail")
    {
        /// AddSink(new stdout_sink<gos::mutex>());
        AddSink(new rotating_file_sink<gos::mutex>(gos::level_enum::trace, gos::level_enum::trace,
                                                   (gos::get_root_path() + "log/detail/detail.log")));
    }
    virtual ~detail_logger() {}
};

class profiler_logger : public base_logger<gos::mutex>
{
public:
    profiler_logger() : base_logger<gos::mutex>("profiler")
    {
        AddSink(new rotating_file_sink<gos::mutex>(gos::level_enum::trace, gos::level_enum::trace,
                                                   (gos::get_root_path() + "log/profiler/profiler.log")));
    }
    virtual ~profiler_logger() {}
};
}  // namespace gos

#endif  ///< GOS_PUBLIC_GOS_UTILITY_DETAIL_GOS_LOG_LOGGER_HPP__
