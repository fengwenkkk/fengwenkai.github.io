/**
 * @file            string_utility.hpp
 * @brief           �ַ�������
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
 * @brief           ���ַ���ǰ����ַ�����ȥ��
 * @param           s       [in]    Ҫ���е��ַ���
 * @param           chars   [in]    Ҫȥ��ʲô���ַ���
 * @return          std::string& ���к���ַ���
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 *
 * @code
 * std::string str("ll Hello World! ll");
 * gos::string_strip(str, "ll");
 * std::cout << str << std::endl;
 * @endcode
 *
 * > ���:
 * > Hello World!
 */
inline std::string& string_strip(std::string& s, const std::string& chars = " ")
{
    s.erase(0, s.find_first_not_of(chars.c_str()));
    s.erase(s.find_last_not_of(chars.c_str()) + 1);
    return s;
}

/**
 * @brief           ���ض�����Ϊ�ָ������з��ַ���������vector��
 * @param           s           [in]    ԭ�ַ���
 * @param           tokens      [out]   ���к�����ַ���
 * @param           delimiters  [in]    �ָ���
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
 * > ���:
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
 * @brief           �����ַ����е��Ӵ�
 * @param           sub_str    [in]     ��Ҫ�������Ӵ�
 * @param           s          [in\out] ��Ҫ�������ַ���
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 *
 * @code
 * std::string str("Hello World! ll Hi!");
 * gos::string_erase("ll", str);
 * std::cout << str;
 * @endcode
 *
 * > ��� ��
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
 * @brief           �滻ָ���ַ���Ϊ�µ��ַ���
 * @param           find_str        [in]    ���滻���ַ���
 * @param           replace_str     [in]    �滻�ַ���
 * @param           s               [out]   ��Ҫ������ԭ�ַ���
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @note
 *
 * @code
 * std::string str("Hello World!");
 * gos::string_replace("Hello", "Hi", str);
 * std::cout << str << std::endl;
 * @endcode
 *
 * > ���:
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
 * @brief           �����ַ����е��Ӵ���λ��
 * @param           src_str     [in]    �����ҵ��ַ���
 * @param           find_str    [in]    �Ӵ�
 * @return          size_t
 * @retval          ������ҵ��Ӵ����򷵻ص�һ���Ӵ�������ĸ��λ��(��0��ʼ�����λ��)
 * @retval          ���û�в��ҵĵ��Ӵ����򷵻�std::string::npos
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-10-29 13:57:29
 * @note
 *
 * @code
 * std::cout << string_find("Hello World!", "ll") << std::endl;
 * @endcode
 *
 * > ���:
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
 * @brief           ��һ���ַ����Ӻ���ǰ���ҵ�һ�����ֵ��Ӵ�������ĸ��λ��
 * @param           src_str     [in]    �����ҵ��ַ���
 * @param           find_str    [in]    �Ӵ�
 * @return          size_t
 * @retval          ������ҵ��Ӵ����򷵻����һ���Ӵ�������ĸ��λ��(��0��ʼ��������λ��)
 * @retval          ���û�в��ҵĵ��Ӵ����򷵻�std::string::npos
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-10-29 14:17:40
 * @note
 *
 * @code
 * std::cout << string_rfind("Hello World!", "ll") << std::endl;
 * @endcode
 *
 * > ���:
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
 * @brief           ����ǰ׺
 * @param           strSrc       [in]   Դ�ַ���
 * @param           delimiter    [in]   �ָ���
 * @param           strPrefix    [out]  �����ǰ׺
 * @return          true        ���ҳɹ�
 * @return          false       ����ʧ��
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
 * @brief           ���Һ�׺
 * @param           strSrc       [in]   Դ�ַ���
 * @param           delimiter    [in]   �ָ���
 * @param           strPrefix    [out]  �����ǰ׺
 * @return          true        ���ҳɹ�
 * @return          false       ����ʧ��
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
 * @brief ���޷�������ת��Ϊʮ�����Ƶ��ַ���
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
 * @brief �Ѵ����ָ������ʮ�����Ƶ���ʽ�����ַ���
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
 * @brief           �Ѳ�ͬ���͵�����ת��Ϊ std::string
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

/// ������λС��
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
