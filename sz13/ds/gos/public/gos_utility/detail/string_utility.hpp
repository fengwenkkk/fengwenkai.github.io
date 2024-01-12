/**
 * @file            string_utility.hpp
 * @brief           字符串工具
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-06-17 21:49:35
 *
 * @copyright       Copyright (c) 2021 GBCOM
 *
 */

#ifndef GOS_UTILITY_STRING_UTILITY_HPP__
#define GOS_UTILITY_STRING_UTILITY_HPP__

#include <stdlib.h>
#include <functional>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <ctype.h>
#include "gos_utility/detail/algorithm_utility.hpp"
#include "gos_utility/thirdparty/kmp.h"

namespace gos
{
/**
 * @brief           把字符串前后的字符串给去除
 * @param           s       [in]    要剪切的字符串
 * @param           chars   [in]    要去除什么的字符串
 * @return          std::string& 剪切后的字符串
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 *
 * @code
 * std::string str("ll Hello World! ll");
 * gos::string_strip(str, "ll");
 * std::cout << str << std::endl;
 * @endcode
 *
 * > 输出:
 * > Hello World!
 */
inline std::string& string_strip(std::string& s, const std::string& chars = " ")
{
    s.erase(0, s.find_first_not_of(chars.c_str()));
    s.erase(s.find_last_not_of(chars.c_str()) + 1);
    return s;
}

/**
 * @brief           以特定符号为分隔符，切分字符串并放入vector里
 * @param           s           [in]    原字符串
 * @param           tokens      [out]   剪切后的子字符串
 * @param           delimiters  [in]    分隔符
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 *
 * @code
 * std::string str("temp_creator_1");
 * std::vector<std::string> tokens;
 *
 * gos::string_split(str, tokens, "_");
 *
 * for(unsigned i = 0; i < tokens.size(); ++i)
 * {
 *     std::cout << tokens.at(i) << std::endl;
 * }
 * @endcode
 *
 * > 输出:
 * > temp
 * > creator
 * > 1
 */
inline unsigned string_split(const std::string& s, std::vector<std::string>& tokens,
                             const std::string& delimiters = " ")
{
    std::string::size_type lastPos = s.find_first_not_of(delimiters, 0);
    std::string::size_type pos = s.find_first_of(delimiters, lastPos);
    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        tokens.push_back(s.substr(lastPos, pos - lastPos));
        lastPos = s.find_first_not_of(delimiters, pos);
        pos = s.find_first_of(delimiters, lastPos);
    }
    return (unsigned)tokens.size();
}

/**
 * @brief           擦除字符串中的子串
 * @param           sub_str    [in]     需要擦除的子串
 * @param           s          [in\out] 需要操作的字符串
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 *
 * @code
 * std::string str("Hello World! ll Hi!");
 * gos::string_erase("ll", str);
 * std::cout << str;
 * @endcode
 *
 * > 输出 ：
 * > Heo World!  Hi!
 */
inline void string_erase(const std::string& sub_str, std::string& s)
{
    if (sub_str.empty() || s.empty())
    {
        return;
    }

    int begin_pos = gos::kmp_search(s.c_str(), sub_str.c_str());
    while (begin_pos != -1 && begin_pos <= (int)s.size())
    {
        s.erase(begin_pos, sub_str.size());
        begin_pos = gos::kmp_search(s.c_str(), sub_str.c_str(), begin_pos);
    }
}

/**
 * @brief           替换指定字符串为新的字符串
 * @param           find_str        [in]    被替换的字符串
 * @param           replace_str     [in]    替换字符串
 * @param           s               [out]   需要操作的原字符串
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 *
 * @code
 * std::string str("Hello World!");
 * gos::string_replace("Hello", "Hi", str);
 * std::cout << str << std::endl;
 * @endcode
 *
 * > 输出:
 * > Hi World!
 */
inline void string_replace(const std::string& find_str, const std::string& replace_str, std::string& s)
{
    if (find_str == replace_str || s.empty() || find_str.empty())
    {
        return;
    }

    size_t begin_pos = gos::kmp_search(s.c_str(), find_str.c_str());
    while (begin_pos != std::string::npos && begin_pos <= s.size())
    {
        s.erase(begin_pos, find_str.size());
        s.insert(begin_pos, replace_str);
        begin_pos += replace_str.size();
        begin_pos = gos::kmp_search(s.c_str(), find_str.c_str(), begin_pos);
    }
}

/**
 * @brief           查找字符串中的子串的位置
 * @param           src_str     [in]    被查找的字符串
 * @param           find_str    [in]    子串
 * @return          size_t
 * @retval          如果查找到子串，则返回第一个子串的首字母的位置(从0开始计算的位置)
 * @retval          如果没有查找的到子串，则返回std::string::npos
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-10-29 13:57:29
 * @note
 *
 * @code
 * std::cout << string_find("Hello World!", "ll") << std::endl;
 * @endcode
 *
 * > 输出:
 * > 2
 */
inline size_t string_find(const std::string& src_str, const std::string& find_str)
{
    if (src_str.size() <= 1024)
    {
        const char* pos = strstr(src_str.c_str(), find_str.c_str());
        if (pos)
        {
            return static_cast<size_t>(pos - src_str.c_str());
        }
        else
        {
            return std::string::npos;
        }
    }
    else
    {
        return gos::kmp_search(src_str.c_str(), find_str.c_str());
    }
}

/**
 * @brief           从一个字符串从后向前查找第一个出现的子串的首字母的位置
 * @param           src_str     [in]    被查找的字符串
 * @param           find_str    [in]    子串
 * @return          size_t
 * @retval          如果查找到子串，则返回最后一个子串的首字母的位置(从0开始正序计算的位置)
 * @retval          如果没有查找的到子串，则返回std::string::npos
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-10-29 14:17:40
 * @note
 *
 * @code
 * std::cout << string_rfind("Hello World!", "ll") << std::endl;
 * @endcode
 *
 * > 输出:
 * > 9
 */
inline size_t string_rfind(const std::string& src_str, const std::string& find_str)
{
    std::string src_reverse(src_str);
    std::string find_reverse(find_str);
    std::reverse(src_reverse.begin(), src_reverse.end());
    std::reverse(find_reverse.begin(), find_reverse.end());
    size_t begin_pos = kmp_search(src_reverse.c_str(), find_reverse.c_str());
    if (begin_pos != std::string::npos)
    {
        return (src_str.size() - 1) - (begin_pos - find_str.size());
    }
    return std::string::npos;
}

/**
 * @brief           查找前缀
 * @param           strSrc       [in]   源字符串
 * @param           delimiter    [in]   分隔符
 * @param           strPrefix    [out]  输出的前缀
 * @return          true        查找成功
 * @return          false       查找失败
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-03-02 10:19:55
 * @note
 */
inline bool string_find_prefix(const std::string& strSrc, const std::string& delimiter, std::string& strPrefix)
{
    size_t pos = string_find(strSrc, delimiter);
    if (pos == std::string::npos)
    {
        return false;
    }

    strPrefix = strSrc.substr(0, pos);
    return true;
}

/**
 * @brief           查找后缀
 * @param           strSrc       [in]   源字符串
 * @param           delimiter    [in]   分隔符
 * @param           strPrefix    [out]  输出的前缀
 * @return          true        查找成功
 * @return          false       查找失败
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-03-02 10:22:02
 * @note
 */
inline bool string_find_suffix(const std::string& strSrc, const std::string& delimiter, std::string& strPrefix)
{
    size_t pos = string_rfind(strSrc, delimiter);
    if (pos == std::string::npos)
    {
        return false;
    }

    strPrefix = strSrc.substr(pos, delimiter.size() - pos);
    return true;
}

/**
 * @brief 把无符号整数转换为十六进制的字符串
 *
 * @param dec
 * @return std::string
 */
inline std::string to_hex(const size_t dec)
{
    char acText[32];

    sprintf(acText, "0x%X", dec);

    return acText;
}

/**
 * @brief 把传入的指针生成十六进制的形式返回字符串
 *
 * @tparam T
 * @param pointer
 * @return std::string
 */
template <typename T>
inline std::string pointer_to_hex(T* const pointer)
{
    return gos::to_hex(reinterpret_cast<size_t>(pointer));
}

/**
 * @brief           把不同类型的数据转换为 std::string
 * @param           str
 * @return          std::string
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 */
inline std::string to_string(const std::string& str)
{
    return str;
}

inline std::string to_string(int i)
{
    char s[20] = {0};
    sprintf(s, "%d", i);
    return std::string(s);
}

inline std::string to_string(unsigned i)
{
    char s[20] = {0};
    sprintf(s, "%u", i);
    return std::string(s);
}

inline std::string to_string(unsigned long i)
{
    char s[20] = {0};
    sprintf(s, "%lu", i);
    return std::string(s);
}

inline std::string to_string(unsigned long long x)
{
    char num[64] = {0};
    sprintf(num, "%llu", x);
    return std::string(num);
}

inline std::string to_string(long i)
{
    char s[20] = {0};
    sprintf(s, "%ld", i);
    return std::string(s);
}

inline std::string to_string(long long x)
{
    char num[64] = {0};
    sprintf(num, "%lld", x);
    return std::string(num);
}

inline std::string to_string(short x)
{
    char num[16] = {0};
    sprintf(num, "%hd", x);
    return std::string(num);
}

inline std::string to_string(unsigned short x)
{
    char num[16] = {0};
    sprintf(num, "%hu", x);
    return std::string(num);
}

/// 保留两位小数
inline std::string to_string(double d)
{
    char num[16] = {0};
    sprintf(num, "%.2f", d);
    return std::string(num);
}

template <typename T>
inline std::string to_string(T* const x)
{
    return gos::pointer_to_hex(x);
}

inline std::string to_string(bool x)
{
    return x ? "true" : "false";
}

inline std::string to_string(char c)
{
    char a[2] = {0};
    sprintf(a, "%c", c);
    return std::string(a);
}

inline bool to_bool(const std::string& str)
{
    return str == "true";
}

}  // namespace gos

#endif  ///< GOS_GOS_UTILITY_DETAIL_STRING_UTILITY_HPP__
