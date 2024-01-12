/**
 * @file          algorithm_utility.hpp
 * @brief         ����ʵ��<algorithm>�㷨���еĺ���
 * @author        lijiancong(lijiancong@gbcom.com.cn)
 * @version       0.1
 * @date          2021-04-28 15:35:44
 *
 * @copyright     Copyright (c) 2021 GBCOM
 *
 */

#ifndef DS_DS_PUBLIC_ALGORITHM_UTILITY_HPP__
#define DS_DS_PUBLIC_ALGORITHM_UTILITY_HPP__

namespace gos
{
/**
 * @brief         C++11֮ǰû������������Լ�ʵ��std::find_if_not
 * @param         first [in]    ��������Ŀ�ʼ
 * @param         last  [in]    ��������Ľ���
 * @param         q     [in]    �жϹ�����
 * @return        InputIt
 *
 * @author        lijiancong(lijiancong@gbcom.com.cn)
 */
template <class InputIt, class UnaryPredicate>
inline InputIt find_if_not(InputIt first, InputIt last, UnaryPredicate q)
{
    for (; first != last; ++first)
    {
        if (!q(*first))
        {
            return first;
        }
    }
    return last;
}

/**
 * @brief           ȫ����
 * @param           first       [in]    �������Ŀ�ʼ Input ������
 * @param           last        [in]    �������Ľ��� Input ������
 * @param           p           [in]    �жϹ�����(ν��)
 * @return          true    ������ȫ��Ԫ�ط��Ϲ���
 * @return          false   �����ڲ��ֻ�ȫ��Ԫ�ز����Ϲ���
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-01 17:32:07
 * @note
 */
template <class InputIt, class UnaryPredicate>
inline bool all_of(InputIt first, InputIt last, UnaryPredicate p)
{
    return gos::find_if_not(first, last, p) == last;
}

/**
 * @brief           ������
 * @param           first       [in]    �������Ŀ�ʼ Input ������
 * @param           last        [in]    �������Ľ��� Input ������
 * @param           p           [in]    �жϹ�����(ν��)
 * @return          true    �����ڲ��ֻ�ȫ��Ԫ�ط��Ϲ���
 * @return          false   ������û��Ԫ�ط��Ϲ���
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-01 17:14:07
 * @note
 */
template <class InputIt, class UnaryPredicate>
inline bool any_of(InputIt first, InputIt last, UnaryPredicate p)
{
    return std::find_if(first, last, p) != last;
}

/**
 * @brief           ȫ����
 * @param           first       [in]    �������Ŀ�ʼ Input ������
 * @param           last        [in]    �������Ľ��� Input ������
 * @param           p           [in]    �жϹ�����(ν��)
 * @return          true    ����������Ԫ�ض������Ϲ���
 * @return          false   �����ڲ��ֻ�ȫ��Ԫ�ط��Ϲ���
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-01 17:18:06
 * @note
 */
template <class InputIt, class UnaryPredicate>
inline bool none_of(InputIt first, InputIt last, UnaryPredicate p)
{
    return std::find_if(first, last, p) == last;
}

/**
 * @brief           ����������һ��Ԫ����
 * @param           first    [in]   ���俪ʼ
 * @param           last     [in]   �������
 * @param           p        [in]   �жϺ���
 * @return          true
 * @return          false
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-20 17:26:37
 * @note
 */
template <typename InputIterator, typename Predicate>
inline bool any_if(InputIterator first, InputIterator last, Predicate p)
{
    return std::find_if(first, last, p) != last;
}

/**
 * @brief           ����������һ��Ԫ����
 * @param           c    [in]   ����
 * @param           p    [in]   �жϺ���
 * @return          true
 * @return          false
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-20 17:27:23
 * @note
 */
template <typename Container, typename Predicate>
inline bool any_if(const Container& c, Predicate p)
{
    return any_if(c.begin(), c.end(), p);
}

/**
 * @brief           �ж������������ĳ��Ԫ��
 * @param           first    [in]   ���俪ʼ
 * @param           last     [in]   �������
 * @param           value    [in]   ĳ��ֵ
 * @return          true
 * @return          false
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-20 17:27:51
 * @note
 */
template <typename InputIterator, typename T>
inline bool container_contains(InputIterator first, InputIterator last, T value)
{
    return std::find(first, last, value) != last;
}

/**
 * @brief           �ж������Ƿ����ĳ��Ԫ��(�������� std::map ���Ҽ�ֵ)
 * @param           c        [in]   ����
 * @param           value    [in]   ĳ��ֵ
 * @return          true
 * @return          false
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-20 17:29:23
 * @note
 */
template <typename Container, typename T>
inline bool container_contains(const Container& c, const T& value)
{
    return container_contains(c.begin(), c.end(), value);
}

/**
 * @brief           �������ڸ�ֵ��������
 * @param           first       [in]    ���俪ʼ
 * @param           last        [in]    �������
 * @param           value       [in]    �������г�ʼֵ
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-25 14:43:33
 * @note
 */
template <typename ForwardIterator, typename T>
inline constexpr void iota(ForwardIterator first, ForwardIterator last, T value)
{
    for (; first != last; ++first, ++value) *first = value;
}

/**
 * @brief           ��������ֵ��������
 * @param           r           [in/out]    ����
 * @param           value       [in]        �������г�ʼֵ
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-25 14:44:27
 * @note
 */
template <typename Range, typename T>
inline constexpr void iota(Range& r, T value)
{
    iota(r.begin(), r.end(), value);
}

/**
 * @brief           �ж����������Ƿ���ͬ
 * @param           first1      [in]    ����һ��ʼ
 * @param           last1       [in]    ����һ����
 * @param           first2      [in]    �������ʼ
 * @return          true
 * @return          false
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-27 15:29:16
 * @note
 */
template <class InputIt1, class InputIt2>
inline bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2)
{
    for (; first1 != last1; ++first1, ++first2)
    {
        if (!(*first1 == *first2))
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief           ������һ������Ԫ�ص��ڶ�������
 * @param           first1      [in]    ����һ��ʼ
 * @param           last1       [in]    ����һ����
 * @param           first2      [in]    �������ʼ
 * @return          OutputIt
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-27 15:32:08
 * @note
 */
template <class InputIt, class OutputIt>
inline OutputIt copy(InputIt first, InputIt last, OutputIt d_first)
{
    while (first != last)
    {
        *d_first++ = *first++;
    }
    return d_first;
}

/**
 * @brief           �ж���ֵ�Ƿ��뱾���������ֵ���(ģ���Ƚ�)
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-02-10 13:18:50
 * @note
 */
class approx
{
public:
    approx(const int64_t& anchor, const int64_t& diff) : m_anchor(anchor), m_diff(diff) {}

    /// �ж���ֵ�Ƿ��뱾���������ֵ���, ��ֵ�� [m_anchor - m_diff, m_anchor + m_diff] ������
    bool equal(const int64_t& number)
    {
        return std::abs(number - m_anchor) <= m_diff;
    }

private:
    int64_t m_anchor;  ///< ���ڱȽϵ�ê��ֵ
    int64_t m_diff;    ///< ��������ֵ
};

/// ������ƽ��ֵ
inline unsigned average(unsigned a, unsigned b)
{
    /// return (a & b) + (a ^ b) / 2;
    /// return  (a ^ b) + (a & b) * 2;
    /// return low + (high - low) / 2;
    return (a / 2) + (b / 2) + (a & b & 1);
}

}  // namespace gos

#endif  ///< DS_DS_PUBLIC_ALGORITHM_UTILITY_HPP__
