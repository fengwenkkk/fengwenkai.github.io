/**
 * @file            profiler_utility.hpp
 * @brief           用于实现profiler的测量工具
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-04-13 10:38:22
 * @copyright       Copyright (c) 2021 GBCOM
 */

#ifndef GOS_UTILITY_PROFILER_UTILITY_HPP__
#define GOS_UTILITY_PROFILER_UTILITY_HPP__

#include "gos_utility/detail/macro_switch.hpp"
#include "gos_utility/detail/string_utility.hpp"
#include "gos_utility/detail/gos_log/log_stream.hpp"
#include "gos_utility/detail/debug_utility/tick_count.hpp"

namespace gos
{
/**
 * @brief           测试性能工具，用于记录前后时间和内存的对象
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-02-10 09:35:52
 * @note
 */
class ProfilerImpl
{
public:
    ProfilerImpl() : m_i64Tid(0), m_iLine(0), m_bHaveLineInfo(false)
    {
        clear();
    }

    /// 清空测量变量
    void clear()
    {
        m_tick_count.clear();
        m_ulStartMemUsage = 0;
        m_ulEndMemUsage = 0;
    }

    /// 设置信息
    void SetLineInfo(const std::string &strFuncName, const std::string &strFileName, INT32 iLine, const INT64 &tid,
                     const std::string &strHintInfo = "")
    {
        clear();
        m_strFunctionName = strFuncName;
        m_strFileName = gos_get_simple_file_name(strFileName.c_str());
        m_iLine = iLine;
        m_strHintInfo = strHintInfo;
        m_i64Tid = tid;
        m_bHaveLineInfo = true;
    }

    /// 开始计时
    void Start()
    {
        if (!gos_get_proc_mem_usage(&m_ulStartMemUsage))
        {
            m_ulStartMemUsage = 0;
        }
        m_tick_count.start();
    }

    /// 停止计时
    void Finish()
    {
        m_tick_count.finish();
        if (!gos_get_proc_mem_usage(&m_ulEndMemUsage))
        {
            m_ulEndMemUsage = 0;
        }
    }

    /// 获取日志文本
    std::string GetLogString() const
    {
        std::string strLog = GetTimeString() + " " + GetMemoryString();

        if (!m_strHintInfo.empty())
        {
            strLog += " Info: " + m_strHintInfo;
        }

        strLog += GetLineInfoString();

        return strLog;
    }

    /// 打印日志
    void Report(const std::string &strLog)
    {
        LOGD << GetLogString();
    }

    /// 获取时间差(单位: 毫秒)
    uint64_t GetTimeDiffMillisecond() const
    {
        return m_tick_count.get_ms();
    }

    /// 获取时间差(单位: 微秒)
    uint64_t GetTimeDiffMicrosecond() const
    {
        return m_tick_count.get_us();
    }

    /// 获取内存差值(单位: KB)
    int32_t GetMemoryDiffKb() const
    {
        if (m_ulStartMemUsage < m_ulEndMemUsage)
        {
            return m_ulEndMemUsage - m_ulStartMemUsage;
        }
        else
        {
            return -1 * (m_ulStartMemUsage - m_ulEndMemUsage);
        }
    }

private:
    tick_count m_tick_count;        ///< 间隔时间记录对象
    UINT32 m_ulStartMemUsage;       ///< 开始内存使用量
    UINT32 m_ulEndMemUsage;         ///< 结束内存使用量
    INT64 m_i64Tid;                 ///< 函数运行在哪个 thread 上
    std::string m_strFunctionName;  ///< 函数的名称
    std::string m_strFileName;      ///< 文件的名称
    INT32 m_iLine;                  ///< 文件的行数
    bool m_bHaveLineInfo;           ///< 是否已经设置了行信息
    std::string m_strHintInfo;      ///< 提示信息

    /// 获取时间间隔文本
    std::string GetTimeString() const
    {
        std::string strTime = m_tick_count.get_time_string();
        /// 对齐
        while (strTime.size() < 8)
        {
            strTime = " " + strTime;
        }
        std::string strRet("Spend Time: ");
        strRet += strTime;

        return strRet;
    }

    /// 获取内存变化文本
    std::string GetMemoryString() const
    {
        std::string strMemoryString;
        strMemoryString += " Memory Change: " + gos::to_string(m_ulStartMemUsage / 1024) + " ~ " +
                           gos::to_string(m_ulEndMemUsage / 1024) + " KB";
        return strMemoryString;
    }

    /// 获取行信息文本
    std::string GetLineInfoString() const
    {
        /// 有行信息时添加行信息
        if (m_bHaveLineInfo)
        {
            std::string strLine;
            strLine += gos::get_log_line_info(m_strFunctionName, m_strFileName, m_iLine, m_i64Tid);
            return strLine;
        }
        else
        {
            return "";
        }
    }
};

class ProfilerObj
{
public:
    ProfilerObj(const std::string &strFuncName, const std::string &strFileName, INT32 iLine, const INT64 &tid,
                const std::string &strHintInfo = "")
    {
        Impl.SetLineInfo(strFuncName, strFileName, iLine, tid, strHintInfo);
        /// 构造函数最后一行获取当前时间
        Impl.Start();
    }

    ~ProfilerObj()
    {
        Impl.Finish();
        Impl.Report(Impl.GetLogString());
    }

private:
    ProfilerImpl Impl;
};

/**
 * @brief 获取某个函数的执行时间
 *
 * @pre 示例
 * @code
 * int f() {
 *  PROFILER(LOG);
 *  /// do something
 * }
 * @endcode
 * 函数f的执行时间会通过LOG日志函数指针打印出来
 *
 */
#ifdef USE_GOS_PROFILER
#define PROFILER() ::gos::ProfilerObj gos__profiler__impl__(__FUNCTION__, __FILE__, __LINE__, ::gos::get_tid())

#define PROFILER_INFO(hint_info) \
    ::gos::ProfilerObj gos__profiler__impl__(__FUNCTION__, __FILE__, __LINE__, ::gos::get_tid(), hint_info)
#else
#define PROFILER()
#define PROFILER_INFO(hint_info)
#endif
}  // namespace gos

#endif  ///< GOS_UTILITY_PROFILER_UTILITY_HPP__
