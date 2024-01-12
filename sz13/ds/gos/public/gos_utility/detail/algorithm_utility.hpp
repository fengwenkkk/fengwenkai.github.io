/**
 * @file          algorithm_utility.hpp
 * @brief         用于实现<algorithm>算法库中的函数
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
 * @brief         C++11之前没有这个函数，自己实现std::find_if_not
 * @param         first [in]    输入区间的开始
 * @param         last  [in]    输入区间的结束
 * @param         q     [in]    判断规则函数
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
 * @brief           全部是
 * @param           first       [in]    检查区间的开始 Input 迭代器
 * @param           last        [in]    检查区间的结束 Input 迭代器
 * @param           p           [in]    判断规则函数(谓词)
 * @return          true    区间内全部元素符合规则
 * @return          false   区间内部分或全部元素不符合规则
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
 * @brief           部分是
 * @param           first       [in]    检查区间的开始 Input 迭代器
 * @param           last        [in]    检查区间的结束 Input 迭代器
 * @param           p           [in]    判断规则函数(谓词)
 * @return          true    区间内部分或全部元素符合规则
 * @return          false   区间内没有元素符合规则
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
 * @brief           全不是
 * @param           first       [in]    检查区间的开始 Input 迭代器
 * @param           last        [in]    检查区间的结束 Input 迭代器
 * @param           p           [in]    判断规则函数(谓词)
 * @return          true    区间内所有元素都不符合规则
 * @return          false   区间内部分或全部元素符合规则
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
 * @brief           区间中任意一个元素是
 * @param           first    [in]   区间开始
 * @param           last     [in]   区间结束
 * @param           p        [in]   判断函数
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
 * @brief           容器中任意一个元素是
 * @param           c    [in]   容器
 * @param           p    [in]   判断函数
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
 * @brief           判断容器区间包含某个元素
 * @param           first    [in]   区间开始
 * @param           last     [in]   区间结束
 * @param           value    [in]   某个值
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
 * @brief           判断容器是否包含某个元素(不能用于 std::map 查找键值)
 * @param           c        [in]   容器
 * @param           value    [in]   某个值
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
 * @brief           在区间内赋值递增序列
 * @param           first       [in]    区间开始
 * @param           last        [in]    区间结束
 * @param           value       [in]    递增序列初始值
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
 * @brief           对容器赋值递增序列
 * @param           r           [in/out]    容器
 * @param           value       [in]        递增序列初始值
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
 * @brief           判断两个区间是否相同
 * @param           first1      [in]    区间一开始
 * @param           last1       [in]    区间一结束
 * @param           first2      [in]    区间二开始
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
 * @brief           拷贝第一个区间元素到第二个区间
 * @param           first1      [in]    区间一开始
 * @param           last1       [in]    区间一结束
 * @param           first2      [in]    区间二开始
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
 * @brief           判断数值是否与本对象定义的数值相近(模糊比较)
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-02-10 13:18:50
 * @note
 */
class approx
{
public:
    approx(const int64_t& anchor, const int64_t& diff) : m_anchor(anchor), m_diff(diff) {}

    /// 判断数值是否与本对象定义的数值相近, 数值在 [m_anchor - m_diff, m_anchor + m_diff] 返回真
    bool equal(const int64_t& number)
    {
        return std::abs(number - m_anchor) <= m_diff;
    }

private:
    int64_t m_anchor;  ///< 用于比较的锚点值
    int64_t m_diff;    ///< 浮动的数值
};

/// 求两数平均值
inline unsigned average(unsigned a, unsigned b)
{
    /// return (a & b) + (a ^ b) / 2;
    /// return  (a ^ b) + (a & b) * 2;
    /// return low + (high - low) / 2;
    return (a / 2) + (b / 2) + (a & b & 1);
}

}  // namespace gos

#endif  ///< DS_DS_PUBLIC_ALGORITHM_UTILITY_HPP__
