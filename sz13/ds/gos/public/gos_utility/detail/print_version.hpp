/**
 * @file            print_version.hpp
 * @brief           打印当前版本
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
 * @brief           获取当前编译环境的CPP版本
 * @return          std::string CPP版本
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
 * @brief           获取当前编译器名称
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
 * @brief           打印当前版本和工程名称
 * @param           strProjectName      [in] 工程名称
 * @param           strVersion          [in] 版本号
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
inline void PrintCurrentVersion(const std::string& strProjectName, const std::string& strVersion)
{
    std::string strLogText;

    strLogText = strLogText + "\n*************************************************************\n" +
                 "Project:      " + strProjectName + "\n" +
                 "Version:      " + strVersion + "\n" +
#ifdef _DEBUG  //正式发布版本不打印版本细节
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
