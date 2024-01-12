/**
 * @file            index_utility.hpp
 * @brief           获取索引的类对象
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-11-17 18:41:42
 * @copyright       Copyright (c) 2021 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_H__
#define DS_GOS_PUBLIC_GOS_UTILITY_H__

#include <vector>
#include <algorithm>

namespace gos
{
//// 总是返回 true 的函数
inline bool AlwaysTrue(const int& i)
{
    (void)i;
    return true;
}

/// 判断偶数
inline bool IsEven(const int& i)
{
    return i % 2 == 0;
}

/// 判断奇数
inline bool IsOdd(const int& i)
{
    return !IsEven(i);
}

/**
 * @brief           index 分配器
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-11-18 10:41:38
 * @note
 * TODO(lijiancong): 使用模板参数代替 index 类型
 *
 * @code {.cpp}
#include <numeric>
int main()
{
    std::vector<int> vec(10);
    std::iota(vec.begin(), vec.end(), 0);   ///< {0, 1, ..., 9}

    gos::index_utility generator(vec);
    /// 设置 0 这个 index 已经被使用
    generator.SetIndex(0);

    int index_odd;
    int index_even;
    if(generator.GetNextIndex(index_odd, gos::IsOdd) &&  ///< 获取奇数 index
       generator.GetNextIndex(index_even, gos::IsEven))  ///< 获取偶数 index
    {
        /// 两个 index 都获取成功后
        /// 归还偶数index
        generator.EraseIndex(index_even);
    }
    return 0;
}
 * @endcode
 *
 */
class index_utility
{
public:
    /// 使用 index 序列初始化
    index_utility(const std::vector<int>& vecIndex)
    {
        m_UnusedIndex = vecIndex;
        std::sort(m_UnusedIndex.begin(), m_UnusedIndex.end());
    }

    /// 获取第一个未使用的 index
    bool GetNextIndex(int& index)
    {
        return GetNextIndex(index, AlwaysTrue);
    }

    /**
     * @brief           获取去第一个符合要求的 index
     * @param           index           [out]
     * @param           p               [in]   函数对象类型应为 bool (const int&)
     * @return          true    找到了
     * @return          false   没找到
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-11-18 10:44:48
     * @note
     */
    template <typename UnaryPredicate>
    bool GetNextIndex(int& index, UnaryPredicate p)
    {
        for (unsigned i = 0; i < m_UnusedIndex.size(); ++i)
        {
            if (p(m_UnusedIndex.at(i)))
            {
                index = m_UnusedIndex.at(i);
                m_UnusedIndex.erase(m_UnusedIndex.begin() + i);
                m_CurrentIndex.push_back(index);
                std::sort(m_CurrentIndex.begin(), m_CurrentIndex.end());
                return true;
            }
        }
        return false;
    }

    /// 更新某个 index 已经被使用
    bool SetIndex(int index)
    {
        std::vector<int>::iterator it = std::find(m_UnusedIndex.begin(), m_UnusedIndex.end(), index);
        if (it != m_UnusedIndex.end())
        {
            m_UnusedIndex.erase(it);
            m_CurrentIndex.push_back(index);
            std::sort(m_CurrentIndex.begin(), m_CurrentIndex.end());
            return true;
        }
        else
        {
            return false;
        }
    }

    /// 更新某个 index 未被使用
    bool EraseIndex(int index)
    {
        std::vector<int>::iterator it = std::find(m_CurrentIndex.begin(), m_CurrentIndex.end(), index);
        if (it != m_CurrentIndex.end())
        {
            m_CurrentIndex.erase(it);
            m_UnusedIndex.push_back(index);
            std::sort(m_UnusedIndex.begin(), m_UnusedIndex.end());
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    std::vector<int> m_CurrentIndex;  ///< 当前已经使用的 index
    std::vector<int> m_UnusedIndex;   ///< 当前未使用的 index
};
}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_H__
