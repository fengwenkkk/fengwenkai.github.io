/**
 * @file            input_check_utility.hpp
 * @brief           �����ַ����ļ�鹤�ߺ���ʵ��
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-09-01 16:27:04
 *
 * @copyright       Copyright (c) 2021 GBCOM
 *
 */

#ifndef GOS_UTILITY_DETAIL_INPUT_CHECK_UTILITY_HPP__
#define GOS_UTILITY_DETAIL_INPUT_CHECK_UTILITY_HPP__

#include <string>
#include <vector>
#include <utility>
#include <cstdlib>
#include <cctype>

namespace gos
{
/// �ж��ַ��Ƿ����������
inline bool IsPunctChar(const char c)
{
    return ispunct(static_cast<int>(c)) != 0;
}

/// �ж��ַ�����ĸ�����֣�alphanumeric character��a-z A-Z 0-9 ������ true
inline bool IsAlChar(const char c)
{
    return isalnum(static_cast<int>(c)) != 0;
}

/// �ж��ַ�������
inline bool IsDigit(const char c)
{
    return isdigit(static_cast<int>(c)) != 0;
}

/// �ж��ַ��ǿո� '\r' '\n' '\t' 'space' ������ true
inline bool IsSpace(const char c)
{
    return isspace(static_cast<int>(c)) != 0;
}

/// �ж��ַ��� ��д��ĸ
inline bool IsUpper(const char c)
{
    return isupper(static_cast<int>(c)) != 0;
}

/// �ж��ַ��� Сд��ĸ
inline bool IsLower(const char c)
{
    return islower(static_cast<int>(c)) != 0;
}

/// �ж��ַ���Сд��ĸ������ a-z 0-9 ���� true
inline bool IsDigitOrLowerLetter(const char c)
{
    return IsAlChar(c) && !IsUpper(c);
}

/// �ж��ַ������ֻ� '.'
inline bool IsDigitOrDot(const char c)
{
    return isdigit(static_cast<int>(c)) != 0 || c == '.';
}

/// �Ƿ���ʮ�����Ƶ��ַ�
inline bool IsHexChar(const char c)
{
    return IsDigit(c) || ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
}

/// �ж��Ƿ����ض��ַ�
template <char C>
inline bool IsChar(const char c)
{
    return c == C;
}

/// �����ַ�������
inline bool AlwaysTrueChar(const char c)
{
    return true;
}

/// �ж��ַ��������ַ
inline bool IsMacChar(const char c)
{
    return IsHexChar(c) || c == '-' || c == ':';
}

/// ����ַ��Ƿ񸴺Ϲ�����ָ�붨��
typedef bool (*check_char_func)(const char);

/// �����ַ���ȫ��
inline bool all_of(const std::string& strInput, check_char_func p)
{
    return gos::all_of(strInput.begin(), strInput.end(), p);
}

/// �����ַ���ȫ����
inline bool none_of(const std::string& strInput, check_char_func p)
{
    return gos::none_of(strInput.begin(), strInput.end(), p);
}

/// �����ַ���������
inline bool any_of(const std::string& strInput, check_char_func p)
{
    return gos::any_of(strInput.begin(), strInput.end(), p);
}

/// �����ַ������ж��ٷ��Ϲ�����ַ�
inline bool count_equal(const std::string& str, check_char_func p, const unsigned& count)
{
    return count == (unsigned)std::count_if(str.begin(), str.end(), p);
}

/// ������
inline bool count_not_equal(const std::string& str, check_char_func p, const unsigned& count)
{
    return count != (unsigned)std::count_if(str.begin(), str.end(), p);
}

/// ����
inline bool count_great(const std::string& str, check_char_func p, const unsigned& count)
{
    return (unsigned)std::count_if(str.begin(), str.end(), p) > count;
}

/// ���ڵ���
inline bool count_great_equal(const std::string& str, check_char_func p, const unsigned& count)
{
    return (unsigned)std::count_if(str.begin(), str.end(), p) >= count;
}

/// С��
inline bool count_less(const std::string& str, check_char_func p, const unsigned& count)
{
    return (unsigned)std::count_if(str.begin(), str.end(), p) < count;
}

/// С�ڵ���
inline bool count_less_equal(const std::string& str, check_char_func p, const unsigned& count)
{
    return (unsigned)std::count_if(str.begin(), str.end(), p) <= count;
}

/// ����ַ����Ƿ񸴺Ϲ���ĺ���ָ��
typedef bool (*input_check_function)(const std::string&, check_char_func);
/// �ַ���������
typedef bool (*count_check_function)(const std::string&, check_char_func, const unsigned&);

class CountRules
{
public:
    CountRules(count_check_function count_f, check_char_func char_f, const unsigned& count)
        : m_count_f(count_f), m_check_char_f(char_f), m_count(count)
    {
    }

    CountRules& operator=(const CountRules& other)
    {
        if (&other != this)
        {
            m_count_f = other.m_count_f;
            m_check_char_f = other.m_check_char_f;
            m_count = other.m_count;
        }
        return *this;
    }

    count_check_function m_count_f;
    check_char_func m_check_char_f;
    unsigned m_count;
};

/**
 * @brief           �����ַ�������࣬����ɵ���
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-02 09:31:00
 * @note
 */
class input_check
{
public:
    /**
     * @brief           ��ӹ��˹���
     * @param           input    [in]   ����ַ����ĺ���
     * @param           p        [in]   ����ַ��ĺ���
     * @return          input_check&
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-09-02 09:31:19
     * @note
     */
    input_check& AddRules(input_check_function input, check_char_func p)
    {
        std::pair<input_check_function, check_char_func> pair(input, p);
        vecRules.push_back(pair);
        return *this;
    }

    /**
     * @brief           ��Ӽ�������
     * @param           input    [in]   ��������
     * @param           p        [in]   �ַ���������
     * @param           count    [in]   ��������
     * @return          input_check&
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-10-27 09:34:51
     * @note
     */
    input_check& AddRules(count_check_function input, check_char_func p, const unsigned& count)
    {
        CountRules temp(input, p, count);
        vecCountRules.push_back(temp);
        return *this;
    }

    /**
     * @brief           �Ƿ������ַ����Ƿ�������й���
     * @param           strInput    [in]
     * @return          true    ���ַ�������ȫ��У�����
     * @return          false   ���ַ���������ȫ��У�����
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-09-01 18:40:37
     * @note
     */
    bool IsValid(const std::string& strInput)
    {
        for (size_t i = 0; i < vecRules.size(); ++i)
        {
            if (!vecRules.at(i).first(strInput, vecRules.at(i).second))
            {
                return false;
            }
        }

        for (size_t i = 0; i < vecCountRules.size(); ++i)
        {
            if (!vecCountRules.at(i).m_count_f(strInput, vecCountRules.at(i).m_check_char_f,
                                               vecCountRules.at(i).m_count))
            {
                return false;
            }
        }

        return true;
    }

private:
    std::vector<std::pair<input_check_function, check_char_func> > vecRules;  ///< ���˹���ļ���
    std::vector<CountRules> vecCountRules;
};

/**
 * @brief           �ж�������û����Ƿ���Ч
 * @param           strInput    [in]
 * @return          true
 * @return          false
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-01 18:42:14
 * @note
 */
inline bool IsInputUserNameValid(const std::string& strInput)
{
    input_check filter;
    /// ����ĵڶ���������������ģ�д������������չʾ input_check ���÷�
    filter.AddRules(gos::all_of, gos::IsAlChar).AddRules(gos::none_of, gos::IsSpace);
    return filter.IsValid(strInput);
}

/**
 * @brief           �ж�����������Ƿ���Ч
 * @param           strInput    [in]
 * @return          true
 * @return          false
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-03 18:00:08
 * @note
 */
inline bool IsInputPasswordValid(const std::string& strInput)
{
    input_check filter;
    /// ����ĵڶ���������������ģ�д������������չʾ input_check ���÷�
    filter.AddRules(gos::none_of, gos::IsPunctChar).AddRules(gos::none_of, gos::IsSpace);

    return filter.IsValid(strInput);
}

/**
 * @brief           �ж�����������ǿ���룬�� ��Сд��ĸ����д��ĸ������
 * @param           strInput    [in]
 * @return          true
 * @return          false
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-02 10:00:29
 * @note
 */
inline bool IsInputStrongPasswordValid(const std::string& strInput)
{
    input_check filter;
    filter.AddRules(gos::any_of, gos::IsUpper).AddRules(gos::any_of, gos::IsLower).AddRules(gos::any_of, gos::IsDigit);
    return filter.IsValid(strInput);
}

/**
 * @brief           �ж������ַ����� IP �Ƿ���Ч
 * @param           strIP    [in]
 * @return          true    ����Ч�� IP �ַ���
 * @return          false   ����Ч�� IP �ַ���
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-02 09:42:26
 * @note
 */
inline bool IsInputIPV4Valid(const std::string& strIP)
{
    input_check obj;
    if (!obj.AddRules(gos::count_not_equal, gos::AlwaysTrueChar, 0)   ///< ����Ϊ��
             .AddRules(gos::all_of, gos::IsDigitOrDot)                ///< ���ж������ֻ��� '.'
             .AddRules(gos::count_equal, gos::IsChar<'.'>, 3)         ///< �ַ� '.' ֻ��������
             .AddRules(gos::count_great_equal, gos::IsDigitOrDot, 7)  ///< �ַ��������ڵ����߸�
             .AddRules(gos::count_less_equal, gos::IsDigitOrDot, 15)  ///< �ַ��������15��
             .IsValid(strIP))
    {
        return false;
    }

    /// �ж�ÿ������Ӧ��Ϊ 0 ~ 255
    std::vector<std::string> vec;
    gos::string_split(strIP, vec, ".");
    for (size_t i = 0; i < vec.size(); ++i)
    {
        int num = std::atoi(vec.at(i).c_str());
        if (num < 0 || num > 255)
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief           �ж�����������ַ�Ƿ���ȷ
 * @param           strMac      [in]    ���� mac ��ַ
 * @param           split_char  [in]    �����ַ�ķָ���
 * @return          true    ����Ч�� mac ��ַ
 * @return          false   ������Ч�� mac ��ַ
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-02 14:33:36
 * @note
 */
inline bool IsInputMacValid(const std::string& strMac, const char split_char = ':')
{
    /// �жϷָ�������Ч��
    if (split_char != ':' && split_char != '-')
    {
        return false;
    }

    input_check filter;
    filter.AddRules(gos::all_of, gos::IsMacChar).AddRules(gos::count_equal, gos::AlwaysTrueChar, 17);
    if (split_char == ':')
    {
        filter.AddRules(gos::count_equal, gos::IsChar<':'>, 5);
    }
    else if (split_char == '-')
    {
        filter.AddRules(gos::count_equal, gos::IsChar<'-'>, 5);
    }

    return filter.IsValid(strMac);
}

/**
 * @brief           �ж�ĳ���ַ�������������ȷ
 * @param           str     [in]    ��Ҫ�����ַ���
 * @param           count   [in]    ��Ҫ����
 * @return          true
 * @return          false
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-10-27 10:17:49
 * @note
 */
template <char C>
inline bool IsCharCountCorrect(const std::string& str, const unsigned& ulCount)
{
    input_check obj;
    obj.AddRules(gos::count_equal, gos::IsChar<C>, ulCount);
    return obj.IsValid(str);
}

/**
 * @brief           �ж��ַ����Ƿ�������
 * @param           str    [in]
 * @return          true
 * @return          false
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-12-07 18:38:14
 * @note
 */
inline bool IsAllNumber(const std::string& str)
{
    input_check obj;
    obj.AddRules(gos::all_of, gos::IsDigit);
    return obj.IsValid(str);
}

}  // namespace gos

#endif  ///< GOS_PUBLIC_GOS_UTILITY_DETAIL_INPUT_CHECK_UTILITY_HPP__