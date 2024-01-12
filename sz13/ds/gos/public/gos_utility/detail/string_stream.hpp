/**
 * @file            string_stream.hpp
 * @brief           实现一个返回 std::string 的流对象
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-08-05 16:33:11
 *
 * @copyright       Copyright (c) 2021 GBCOM
 *
 */

#ifndef GOS_PUBLIC_GOS_UTILITY_DETAIL_STRING_STREAM_HPP__
#define GOS_PUBLIC_GOS_UTILITY_DETAIL_STRING_STREAM_HPP__

#include <sstream>
#include <vector>
#include <string>
#include <map>

namespace gos
{
class string_stream
{
public:
    string_stream() {}
    string_stream(const string_stream& other)
    {
        m_stream.clear();
        m_stream.str("");
        m_stream << other.m_stream.str();
    }

    string_stream& operator=(const string_stream& other)
    {
        if (this != &other)
        {
            m_stream.clear();
            m_stream.str("");
            m_stream << other.m_stream.str();
        }
        return *this;
    }

    // 转换为 std::string 的函数
    std::string str() const
    {
        return m_stream.str();
    }

    /// 清空这个流
    void clear()
    {
        m_stream.clear();
        m_stream.str("");
    }

    /// 为 bool 类型重载
    string_stream& operator<<(const bool& data)
    {
        m_stream << (data ? "true" : "false");
        return *this;
    }

    /// 为原生数组重载
    template <typename T, unsigned int N>
    string_stream& operator<<(const T (&arr)[N])
    {
        m_stream << "array: ";
        for (unsigned i = 0; i < N; ++i)
        {
            *this << arr[i];
            if (i != N - 1)
            {
                m_stream << ", ";
            }
        }
        return *this;
    }

    /// 为 char 数组重载
    template <unsigned int N>
    string_stream& operator<<(const char (&arr)[N])
    {
        m_stream << arr;
        return *this;
    }

    /// 为 vector 重载
    template <typename T>
    string_stream& operator<<(const std::vector<T>& vec)
    {
        m_stream << "vector: \r\n";
        for (unsigned i = 0; i < vec.size(); i++)
        {
            *this << vec.at(i);
            if (i != vec.size() - 1)
            {
                m_stream << ", ";
            }
        }
        return *this;
    }

    /// 为 vector<unsigned char> 重载
    string_stream& operator<<(const std::vector<unsigned char>& vec)
    {
        for (unsigned i = 0; i < vec.size(); i++)
        {
            if (i != 0)
            {
                m_stream << " ";
                if (i % 8 == 0)
                {
                    m_stream << "\n";
                }
            }

            int iDec = static_cast<int>(vec.at(i));
            char acText[16] = {0};
            if (iDec < 0x10)
            {
                sprintf(acText, "0x0%X", iDec);
            }
            else
            {
                sprintf(acText, "0x%X", iDec);
            }

            m_stream << acText;
        }
        return *this;
    }

    /// 为 map 重载
    template <typename Key, typename Value>
    string_stream& operator<<(const std::map<Key, Value>& map)
    {
        typename std::map<Key, Value>::const_iterator it = map.begin();
        for (; it != map.end(); ++it)
        {
            *this << "Key: " << it->first << ", Value: " << it->second << "\r\n";
        }
        return *this;
    }

    /// 为指针类型重载
    /// template <typename T>
    /// string_stream& operator<<(const T* p)
    /// {
    ///     m_stream << gos::to_string(p);
    ///     return *this;
    /// }

    /// 为 string_stream 重载
    string_stream& operator<<(const string_stream& data)
    {
        m_stream << data.str();
        return *this;
    }

    /// 为 int 类型重载
    string_stream& operator<<(const int& data)
    {
        m_stream << gos::to_string(data);
        return *this;
    }

    /// 为 unsigned 类型重载
    string_stream& operator<<(const unsigned& data)
    {
        m_stream << gos::to_string(data);
        return *this;
    }

    /// 为 long 类型重载
    string_stream& operator<<(const long& data)
    {
        m_stream << gos::to_string(data);
        return *this;
    }

    /// 为 long long 类型重载
    string_stream& operator<<(const long long& data)
    {
        m_stream << gos::to_string(data);
        return *this;
    }

    /// 为 unsigned long 类型重载
    string_stream& operator<<(const unsigned long& data)
    {
        m_stream << gos::to_string(data);
        return *this;
    }

    /// 为 unsigned long long 类型重载
    string_stream& operator<<(const unsigned long long& data)
    {
        m_stream << gos::to_string(data);
        return *this;
    }

    /// 为 short 类型重载
    string_stream& operator<<(const short& data)
    {
        m_stream << gos::to_string(data);
        return *this;
    }

    /// 为 unsigned short 类型重载
    string_stream& operator<<(const unsigned short& data)
    {
        m_stream << gos::to_string(data);
        return *this;
    }

    /// 其余类型的重载
    template <typename T>
    string_stream& operator<<(const T& data)
    {
        m_stream << data;
        return *this;
    }

private:
    std::stringstream m_stream;
};

/**
 * @brief           返回变量名称和变量数值的宏定义
 * @param           x           [in]
 * @param           strName     [in]
 * @return          std::string
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-11-04 10:23:09
 * @note
 */
template <typename T>
inline std::string format_dbg_string(const T& x, const std::string& strName)
{
    gos::string_stream stream;
    stream << strName << "(" << x << ")";
    return stream.str();
}

/// template <typename T>
/// struct TypeName;
///
/// #define REGISTER_PARSE_TYPE(X)   template <> struct gos::TypeName<X> {static const char* name;}; const char*
/// gos::TypeName<X>::name = #X
///
/// REGISTER_PARSE_TYPE(int);
/// REGISTER_PARSE_TYPE(long);
/// REGISTER_PARSE_TYPE(long long);
/// REGISTER_PARSE_TYPE(unsigned);
/// REGISTER_PARSE_TYPE(unsigned int);
/// REGISTER_PARSE_TYPE(unsigned long);
/// REGISTER_PARSE_TYPE(unsigned long long);
/// REGISTER_PARSE_TYPE(float);
/// REGISTER_PARSE_TYPE(double);
/// REGISTER_PARSE_TYPE(std::string);
/// REGISTER_PARSE_TYPE(std::vector);
/// REGISTER_PARSE_TYPE(std::map);
}  // namespace gos

/// 用于调试查看变量名称和变量数值的宏定义
#if defined(_DEBUG) || defined(DEBUG)
#define DBG(x) gos::format_dbg_string(x, std::string(#x)).c_str()
#else
#define DBG(x) ""
#endif

#endif  ///< GOS_PUBLIC_GOS_UTILITY_DETAIL_STRING_STREAM_HPP__