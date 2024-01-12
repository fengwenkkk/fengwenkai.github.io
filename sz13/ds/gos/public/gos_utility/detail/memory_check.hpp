/**
 * @file            memory_check.hpp
 * @brief           内存检查工具
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2022-01-21 10:50:43
 * @copyright       Copyright (c) 2021-2022 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_MEMORY_CHECK_HPP__
#define DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_MEMORY_CHECK_HPP__

#include "gos_utility/detail/macro_switch.hpp"

#ifdef __GOS_USE_VS__

#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>
#include <crtdbg.h>
#include <stdint.h>
#include <string>

#include "gos_utility/detail/string_stream.hpp"
#include "gos_utility/detail/gos_log/log_stream.hpp"

namespace gos
{
/**
 * @brief           内存泄漏检查类
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-21 11:18:55
 * @note
 * 参考资料:
 * [使用CRT库查找内存泄漏]
 * (https://docs.microsoft.com/zh-cn/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2022)
 * [CRT Debug Heap Details]
 * (https://docs.microsoft.com/en-us/visualstudio/debugger/crt-debug-heap-details?view=vs-2022)
 */
class memory_check_impl
{
public:
    memory_check_impl()
    {
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    }

    /**
     * @brief           开始检测内存泄漏
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2022-01-24 08:47:23
     * @note
     */
    void start()
    {
        _CrtMemCheckpoint(&m_s1);
    }

    /**
     * @brief           结束检测泄漏函数
     * @return          true        内存没有泄漏
     * @return          false       内存泄漏
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2022-01-21 16:50:18
     * @note
     * 该函数只能调用一次，第二次调用以后都会返回 false
     */
    bool finish()
    {
        _CrtMemCheckpoint(&m_s2);
        return TRUE != _CrtMemDifference(&m_s3, &m_s1, &m_s2);
    }

    /**
     * @brief           获取泄漏信息
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2022-01-21 13:04:53
     * @note
     */
    gos::string_stream get_leak_info()
    {
        gos::string_stream strLeakInfo;
        strLeakInfo << "lHighWaterCount: " << m_s3.lHighWaterCount;
        strLeakInfo << ", lTotalCount: " << m_s3.lTotalCount;
        return strLeakInfo;
    }

private:
    _CrtMemState m_s1;  ///< 起始内存状态
    _CrtMemState m_s2;  ///< 结束内存状态
    _CrtMemState m_s3;  ///< 内存变化状态
};

class memory_check
{
public:
    memory_check(const std::string& strFileName, const std::string& strFuncName, const int32_t& iLine,
                 const std::string& strHintInfo = "")
        : m_strFunctionName(strFuncName), m_strFileName(strFileName), m_iLine(iLine), m_strHintInfo(strHintInfo)
    {
    }

    /**
     * @brief           开始检测内存
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2022-01-24 09:31:23
     * @note
     * 该函数之所以不放在构造函数中，是为了排除本对象对内存检测的干扰。
     * 如果把该函数放在构造函数中，每一次运行都会出现内存泄漏
     */
    void start()
    {
        checker.start();
    }

    ~memory_check()
    {
        if (!checker.finish())
        {
            LOG << report_leak_info();
        }
    }

private:
    memory_check_impl checker;
    std::string m_strFunctionName;  ///< 函数的名称
    std::string m_strFileName;      ///< 文件的名称
    int32_t m_iLine;                ///< 文件的行数
    std::string m_strHintInfo;      ///< 提示信息

    /// 获取泄露信息的字符串
    std::string report_leak_info()
    {
        gos::string_stream stream;

        stream << "Memory Leak. " << checker.get_leak_info();
        if (!m_strHintInfo.empty())
        {
            stream << " info: " << m_strHintInfo;
        }

        stream << gos::get_log_line_info(m_strFunctionName, m_strFileName, m_iLine, gos::get_tid());
        return stream.str();
    }
};

}  // namespace gos

#define MEMORY_CHECK()                                                     \
    gos::memory_check __MeMory_checker_(__FILE__, __FUNCTION__, __LINE__); \
    __MeMory_checker_.start()

#define MEMORY_CHECK_INFO(x)                                                  \
    gos::memory_check __MeMory_checker_(__FILE__, __FUNCTION__, __LINE__, x); \
    __MeMory_checker_.start()

#else
#define MEMORY_CHECK()  ///< Linux 系统和 C++ Builder下暂未实现
#endif                  ///< WIN32

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_MEMORY_CHECK_HPP__