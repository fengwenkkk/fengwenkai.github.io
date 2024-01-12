/**
 * @file            log_factory.hpp
 * @brief           日志工厂
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-09-17 11:02:04
 *
 * @copyright       Copyright (c) 2021 GBCOM
 *
 */

#ifndef GOS_PUBLIC_GOS_UTILITY_DETAIL_GOS_LOG_LOG_FACTORY_HPP__
#define GOS_PUBLIC_GOS_UTILITY_DETAIL_GOS_LOG_LOG_FACTORY_HPP__

#include <string>
#include <vector>
#include <map>
#include "gos_utility/detail/gos_log/logger.hpp"

namespace gos
{
class LoggerFactory
{
public:
    bool AddLogger(const std::string& strLoggerName, base_logger<gos::mutex>* logger)
    {
        if (m_logger.find(strLoggerName) == m_logger.end())
        {
            m_logger[strLoggerName] = logger;
            return true;
        }
        return false;
    }

    base_logger<gos::mutex>& GetLogger(const std::string& strLoggerName)
    {
        if (m_logger.find(strLoggerName) == m_logger.end())
        {
            return empty_logger_;
        }
        return *(m_logger[strLoggerName]);
    }

    void flush_all()
    {
        std::map<std::string, base_logger<gos::mutex>*>::iterator it = m_logger.begin();
        for (; it != m_logger.end(); ++it)
        {
            it->second->flush_all();
        }
    }

public:
    static LoggerFactory& GetInstance()
    {
        static LoggerFactory* instance = NULL;
        if (instance == NULL)
        {
            instance = new LoggerFactory();
        }
        return *instance;
    }

    ~LoggerFactory()
    {
        std::map<std::string, base_logger<gos::mutex>*>::iterator it = m_logger.begin();
        for (; it != m_logger.end(); ++it)
        {
            delete it->second;
        }
    }

private:
    std::map<std::string, base_logger<gos::mutex>*> m_logger;
    empty_logger empty_logger_;

    LoggerFactory()
    {
        AddLogger("detail", new detail_logger());
        AddLogger("profiler", new profiler_logger());
    }

    LoggerFactory(const LoggerFactory&);
    LoggerFactory* operator=(const LoggerFactory&);
};
}  // namespace gos

#endif  ///< GOS_PUBLIC_GOS_UTILITY_DETAIL_GOS_LOG_LOG_FACTORY_HPP__
