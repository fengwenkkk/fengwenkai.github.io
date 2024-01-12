/**
 * @file            memory_check.hpp
 * @brief           �ڴ��鹤��
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
 * @brief           �ڴ�й©�����
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-21 11:18:55
 * @note
 * �ο�����:
 * [ʹ��CRT������ڴ�й©]
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
     * @brief           ��ʼ����ڴ�й©
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2022-01-24 08:47:23
     * @note
     */
    void start()
    {
        _CrtMemCheckpoint(&m_s1);
    }

    /**
     * @brief           �������й©����
     * @return          true        �ڴ�û��й©
     * @return          false       �ڴ�й©
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2022-01-21 16:50:18
     * @note
     * �ú���ֻ�ܵ���һ�Σ��ڶ��ε����Ժ󶼻᷵�� false
     */
    bool finish()
    {
        _CrtMemCheckpoint(&m_s2);
        return TRUE != _CrtMemDifference(&m_s3, &m_s1, &m_s2);
    }

    /**
     * @brief           ��ȡй©��Ϣ
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
    _CrtMemState m_s1;  ///< ��ʼ�ڴ�״̬
    _CrtMemState m_s2;  ///< �����ڴ�״̬
    _CrtMemState m_s3;  ///< �ڴ�仯״̬
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
     * @brief           ��ʼ����ڴ�
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2022-01-24 09:31:23
     * @note
     * �ú���֮���Բ����ڹ��캯���У���Ϊ���ų���������ڴ���ĸ��š�
     * ����Ѹú������ڹ��캯���У�ÿһ�����ж�������ڴ�й©
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
    std::string m_strFunctionName;  ///< ����������
    std::string m_strFileName;      ///< �ļ�������
    int32_t m_iLine;                ///< �ļ�������
    std::string m_strHintInfo;      ///< ��ʾ��Ϣ

    /// ��ȡй¶��Ϣ���ַ���
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
#define MEMORY_CHECK()  ///< Linux ϵͳ�� C++ Builder����δʵ��
#endif                  ///< WIN32

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_MEMORY_CHECK_HPP__