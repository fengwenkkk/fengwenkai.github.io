/**
 * @file log_utility.hpp
 * @author lijiancong (lijiancong@gbcom.com.cn)
 * @brief   ����ʵ����־��صĹ��ߺ����ͺ궨��
 * @version 0.1
 * @date 2021-04-13 10:27:12
 *
 * @copyright Copyright (c) 2021 GBCOM
 *
 */

#ifndef GOS_PUBLIC_UTILITY_DETAIL_LOG_UTILITY_HPP__
#define GOS_PUBLIC_UTILITY_DETAIL_LOG_UTILITY_HPP__

#include <string>
#include <algorithm>

#include "gos_utility/detail/string_utility.hpp"
#include "gos_utility/detail/time_utility.hpp"

#ifdef _WIN32
#include <Windows.h>  ///< for GetCurrentProcessId()
#else
#include <unistd.h>  ///< for getpid()
#include <sys/syscall.h>  ///< for SYS_gettid
#endif

namespace gos
{
/**
 * @brief           ��ȡ��־��ӡʱ��
 * @return          std::string
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-22 09:14:29
 * @note
 * ��ʽ: [2021-09-14 17:39:07.870]
 */
inline std::string get_log_time()
{
    std::string str(get_current_time_ms_string());
    while (str.size() < 23)
    {
        str += "0";
    }
    str = "[" + str + "] ";
    return str;
}

/**
 * @brief           ��ȡ��ǰ�̺߳�
 * @return          long long
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-22 09:15:04
 * @note
 */
inline long long get_tid()
{
#ifdef WIN32
    return (long long)GetCurrentThreadId();
#else
    /// return (long long)getpid();
    return syscall(SYS_gettid);
#endif
}

/**
 * @brief ƴ�Ӻ������ƣ��ļ����ƺ����ں������ַ���
 *
 * @return std::string
 */
inline std::string get_log_line_info(const std::string& function_name, const std::string& file_name, const int& line,
                                     const long long& pid)
{
    std::string file(file_name);
    if(!gos::string_find_suffix(file_name, "\\", file))
    {
        gos::string_find_suffix(file_name, "/", file);
    }
    std::string sLogInfo;
    sLogInfo = sLogInfo + std::string(" <Function: ") + function_name + ", File: " + file + ":" + to_string(line) +
               ", TID: " + to_string(pid) + ">";
    return sLogInfo;
}

/**
 * @brief Ϊ�������־ģ��ʹ�ú궨������ȡ����Ϣ
 *
 */
#ifdef USE_GOS_LINE_INFO
/// lijiancong(2022-01-24): �ú궨�����á�
#define LINE_INFO "" ///< ::gos::get_log_line_info(__FUNCTION__, __FILE__, __LINE__, gos::get_tid()).c_str()
#else
#define LINE_INFO ""
#endif

}  // namespace gos

#endif  ///< GOS_PUBLIC_UTILITY_DETAIL_LOG_UTILITY_HPP__
