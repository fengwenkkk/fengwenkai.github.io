///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///
/// @file   system_utility.hpp
/// @brief  跟系统相关的工具
///
/// @author lijiancong, pipinstall@163.com
/// @date   2020-07-18 15:31:14
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////

#ifndef MY_LOG_INCLUDE_OS_H_
#define MY_LOG_INCLUDE_OS_H_

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>

#ifdef _WIN32

#include <io.h>       // _get_osfhandle and _isatty support
#include <process.h>  //  _get_pid support
#ifndef NOMINMAX
#define NOMINMAX  // prevent windows redefining min/max
#include <windows.h>
#include <stdio.h>
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef __MINGW32__
#include <share.h>
#endif

#include <direct.h>  // for _mkdir/_wmkdir

#else  // unix

#include <fcntl.h>
#include <unistd.h>

#ifdef __linux__
#include <sys/syscall.h>  //Use gettid() syscall under linux to get thread id
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iconv.h>

#elif defined(_AIX)
#include <pthread.h>  // for pthread_getthreadid_np

#elif defined(__DragonFly__) || defined(__FreeBSD__)
#include <pthread_np.h>  // for pthread_getthreadid_np

#elif defined(__NetBSD__)
#include <lwp.h>  // for _lwp_self

#elif defined(__sun)
#include <thread.h>  // for thr_self
#endif

#endif  // unix

#ifndef __has_feature       // Clang - feature checking macros.
#define __has_feature(x) 0  // Compatibility with non-clang compilers.
#endif

namespace gos
{
// folder separator
#ifdef _WIN32
static const char folder_sep = '\\';
#else
static const char folder_sep = '/';
#endif

/// @name     path_exists
/// @brief    判断一个路径或文件是否存在
///
/// @param    filename  [in]  路径名称
///
/// @return   存在则返回真
///
/// @author   Lijiancong, pipinstall@163.com
/// @date     2020-07-18 15:48:00
/// @warning  线程不安全
inline bool path_exists(const std::string &filename)
{
    return gos_file_exist(filename.c_str()) == TRUE;
}

/// @name     mkdir_
/// @brief    创建一个路径
///
/// @param    path  [in]  要创建的路径
///
/// @return   创建成功则返回真
///
/// @author   Lijiancong, pipinstall@163.com
/// @date     2020-07-18 15:45:31
/// @warning  线程不安全
static inline bool mkdir_(const std::string &path)
{
#ifdef _WIN32
    return ::_mkdir(path.c_str()) == 0;
#else
    return ::mkdir(path.c_str(), mode_t(0755)) == 0;
#endif
}

// create the given directory - and all directories leading to it
// return true on success or if the directory already exists
/// @name     create_dir
/// @brief    创建一个路径
///
/// @param    path [in] 要创建的路径
///
/// @return   创建成功或路径已经存在则返回真
///
/// @author   Lijiancong, pipinstall@163.com
/// @date     2020-07-18 15:42:57
/// @warning  线程不安全
inline bool create_dir(std::string path)
{
    return gos_create_redir((CHAR *)path.c_str()) == TRUE;
}

/// @name     dir_name
/// @brief    获取文件夹名称
/// @details  Return directory name from given path or empty string
///           "abc/file" => "abc"
///           "abc/" => "abc"
///           "abc" => ""
///           "abc///" => "abc//"
///
/// @param    path [in] 完整路径名
///
/// @return   文件夹名称
///
/// @author   Lijiancong, pipinstall@163.com
/// @date     2020-07-18 15:56:13
/// @warning  线程不安全
inline std::string dir_name(std::string path)
{
#ifdef _WIN32
    // support forward slash in windows
    std::replace(path.begin(), path.end(), '/', folder_sep);
#endif
    std::string::size_type pos = path.find_last_of(folder_sep);
    if (pos != std::string::npos)
    {
        return path.substr(0, pos);
    }
    else
    {
        return "";
    }
}

inline int remove(const std::string &filename)
{
    return gos_delete_file((CHAR*)filename.c_str());
}

inline int remove_if_exists(const std::string &filename)
{
    return path_exists(filename) ? remove(filename) : 0;
}

inline int rename(const std::string &filename1, const std::string &filename2)
{
    return gos_rename_file((CHAR*)filename1.c_str(), (CHAR*)filename2.c_str());
}

// fopen_s on non windows for writing
inline bool fopen_s(FILE **fp, const std::string &filename, const std::string &mode)
{
#ifdef _WIN32
#ifdef SPDLOG_WCHAR_FILENAMES
    *fp = ::_wfsopen((filename.c_str()), mode.c_str(), _SH_DENYNO);
#else
#ifdef _BCB_
    *fp = ::_fsopen((filename.c_str()), mode.c_str(), 0x40);
#else
    *fp = ::_fsopen((filename.c_str()), mode.c_str(), _SH_DENYNO);
#endif
#endif
#if defined(SPDLOG_PREVENT_CHILD_FD)
    if (*fp != nullptr)
    {
        intptr_t file_handle = reinterpret_cast<HANDLE>(_get_osfhandle(::_fileno(*fp)));
        if (!::SetHandleInformation(file_handle, HANDLE_FLAG_INHERIT, 0))
        {
            ::fclose(*fp);
            *fp = NULL;
        }
    }
#endif
#else  // unix
    *fp = ::fopen((filename.c_str()), mode.c_str());
#endif

    return *fp == NULL;
}

inline std::string get_root_path()
{
    char acPath[1024] = {0};

    memset(acPath, 0, sizeof(acPath));
#ifndef _OSWIN32_
    INT32 iRet = readlink("/proc/self/exe", acPath, sizeof(acPath) - 1);

    dir_name(acPath);

#else
    GetModuleFileNameA(NULL, acPath, sizeof(acPath) - 1);

    CHAR *szEnd = gos_right_strchr(acPath, '\\');

    if (szEnd)
    {
        *szEnd = '\0';
    }
#endif

    return std::string(acPath);
}

/**
 * @brief           GBK 编码字符串转 UTF8 字符串
 * @param           str_info    [in]
 * @return          std::string
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-10 11:41:35
 * @note
 */
inline std::string GbkToUtf8(const std::string &str_info)
{
#ifdef _WIN32
    const char *src_str = str_info.c_str();
    int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
    wchar_t *wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char *str = new char[len + 1];
    memset(str, 0, len + 1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    std::string strTemp = str;
    if (wstr)
        delete[] wstr;
    if (str)
        delete[] str;
    return strTemp;
#else
    char *str_str = (char *)str_info.c_str();
    size_t src_len = str_info.size();
    char *dst_str = new char[src_len + 1];
    size_t dst_len = src_len + 1;

    iconv_t cd;
    char **pin = &str_str;
    char **pout = &dst_str;

    cd = iconv_open("utf8", "gbk");
    if (cd == 0)
        return "";
    memset(dst_str, 0, dst_len);
    if (iconv(cd, pin, &src_len, pout, &dst_len) == -1)
        return "";
    iconv_close(cd);
    **pout = '\0';
    std::string strReturn(dst_str);
    delete[] dst_str;
    return strReturn;
#endif
}

/**
 * @brief           UTF8 编码的字符串转为 GBK
 * @param           str    [in]
 * @return          std::string
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-10 11:42:04
 * @note
 */
inline std::string Utf8ToGbk(const std::string &str)
{
#ifdef _WIN32
    const char *src_str = str.c_str();
    int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
    wchar_t *wszGBK = new wchar_t[len + 1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char *szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    std::string strTemp(szGBK);
    if (wszGBK)
        delete[] wszGBK;
    if (szGBK)
        delete[] szGBK;
    return strTemp;
#else
    char *src_str = (char *)str.c_str();
    size_t src_len = str.size();
    char *dst_str = new char[src_len + 1];
    size_t dst_len = src_len + 1;

    iconv_t cd;
    char **pin = &src_str;
    char **pout = &dst_str;

    cd = iconv_open("gbk", "utf8");
    if (cd == 0)
        return "";
    memset(dst_str, 0, dst_len);
    if (iconv(cd, pin, &src_len, pout, &dst_len) == -1)
        return "";
    iconv_close(cd);
    **pout = '\0';

    std::string strReturn(dst_str);
    delete[] dst_str;
    return strReturn;
#endif
}

}  // namespace gos

#endif  // end of MY_LOG_INCLUDE_OS_H_
