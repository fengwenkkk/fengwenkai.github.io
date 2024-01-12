/**
 * @file            escape_char_utility.hpp
 * @brief           转义字符相关工具实现
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-12-14 15:21:28
 * @copyright       Copyright (c) 2021 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_ESCAPE_CHAR_UTILITY_HPP__
#define DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_ESCAPE_CHAR_UTILITY_HPP__

#include <map>
#include <string>
#include "gos_utility/detail/string_utility.hpp"

namespace gos
{
class EscapeCharUtility
{
public:
    /// 默认构造函数
    EscapeCharUtility()
    {
        m_ConverMap["\""] = "%quotes;";
        m_ConverMap["\\"] = "%backslash;";
    }

    /// 使用其他映射的构造函数
    EscapeCharUtility(const std::map<std::string, std::string>& ConverMap)
    {
        m_ConverMap = ConverMap;
    }

    /**
     * @brief           编码含有转义字符的字符串
     * @param           strData    [in]
     * @return          std::string
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-12-16 10:19:22
     * @note
     */
    std::string EncodeEscapeChar(const std::string& strData)
    {
        std::string strTmp = strData;
        std::map<std::string, std::string>::iterator it = m_ConverMap.begin();
        for (; it != m_ConverMap.end(); ++it)
        {
            string_replace(it->first, it->second, strTmp);
        }
        return strTmp;
    }

    /**
     * @brief           解码含有转义字符的字符串
     * @param           strData    [in]
     * @return          std::string
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-12-16 10:19:42
     * @note
     */
    std::string DecodeEscapeChar(const std::string& strData)
    {
        std::string strTmp = strData;
        std::map<std::string, std::string>::iterator it = m_ConverMap.begin();
        for (; it != m_ConverMap.end(); ++it)
        {
            string_replace(it->second, it->first, strTmp);
        }

        return strTmp;
    }

private:
    std::map<std::string, std::string> m_ConverMap;
};

/// 编码含有转义字符的字符串
inline std::string EncodeEscapeChar(const std::string& strData)
{
    EscapeCharUtility obj;
    return obj.EncodeEscapeChar(strData);
}

/// 解码含有转义字符的字符串
inline std::string DecodeEscapeChar(const std::string& strData)
{
    EscapeCharUtility obj;
    return obj.DecodeEscapeChar(strData);
}

}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_ESCAPE_CHAR_UTILITY_HPP__
