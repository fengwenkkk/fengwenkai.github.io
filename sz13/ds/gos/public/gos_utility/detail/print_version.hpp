/**
 * @file            print_version.hpp
 * @brief           ��ӡ��ǰ�汾
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-05-31 09:45:03
 *
 * @copyright       Copyright (c) 2021 GBCOM
 *
 */

#ifndef GOS_PUBLIC_GOS_UTILITY_DETAIL_PRINT_VERSION_HPP__
#define GOS_PUBLIC_GOS_UTILITY_DETAIL_PRINT_VERSION_HPP__

#include "g_log.h"
#include <string>
#include "gos_utility/detail/macro_switch.hpp"
#include "gos_utility/detail/time_utility.hpp"
#include "gos_utility/detail/gos_log/log_stream.hpp"

namespace gos
{
/**
 * @brief           ��ȡ��ǰ���뻷����CPP�汾
 * @return          std::string CPP�汾
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
inline std::string GetCppVersion()
{
    static std::string s_strVersion = "unknow";
#ifdef __GOS_HAS_PRE_CXX98__
    s_strVersion = "Pre C++98";
#endif

#ifdef __GOS_HAS_CXX98__
    s_strVersion = "C++98";
#endif

#ifdef __GOS_HAS_CXX11__
    s_strVersion = "C++11";
#endif

#ifdef __GOS_HAS_CXX14__
    s_strVersion = "C++14";
#endif

#ifdef __GOS_HAS_CXX17__
    s_strVersion = "C++17";
#endif

#ifdef __GOS_HAS_CXX20__
    s_strVersion = "C++20";
#endif
    return s_strVersion;
}

/**
 * @brief           ��ȡ��ǰ����������
 * @return          std::string
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
inline std::string GetCompiler()
{
    static std::string s_Compiler = "unknow";
#ifdef __GOS_USE_VS__
    s_Compiler = "Visual Studio";
#endif

#ifdef __GOS_USE_GNU__
    s_Compiler = "GNU";
#endif

#ifdef __GOS_USE_CLANG__
    s_Compiler = "Clang";
#endif

#ifdef __GOS_USE_BCB__
    s_Compiler = "C++ Builder";
#endif

    return s_Compiler;
}

/**
 * @brief           ��ӡ��ǰ�汾�͹�������
 * @param           strProjectName      [in] ��������
 * @param           strVersion          [in] �汾��
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
inline void PrintCurrentVersion(const std::string& strProjectName, const std::string& strVersion)
{
    std::string strLogText;

    strLogText = strLogText + "\n*************************************************************\n" +
                 "Project:      " + strProjectName + "\n" +
                 "Version:      " + strVersion + "\n" +
#ifdef _DEBUG  //��ʽ�����汾����ӡ�汾ϸ��
                 "Compile Time: " + gos::get_time_string(gos::get_compile_time_stamp()) + "\n" +
                 "Start Time:   " + gos::get_time_string(gos::get_current_time_stamp()) + "\n" +
                 "Compiler:     " + gos::GetCompiler() + "\n" +
                 "CPP Version:  " + gos::GetCppVersion() + "\n" +
#ifdef GIT_LATEST_COMMIT_ID
                 "Commit ID:    " + GIT_LATEST_COMMIT_ID + "\n" +
#endif
#endif
                 "*************************************************************\n";
    GosLog(LOG_INFO, strLogText.c_str());
}

}  // namespace gos

#endif  ///<  GOS_PUBLIC_GOS_UTILITY_DETAIL_PRINT_VERSION_HPP__
