/**
 * @file            index_utility.hpp
 * @brief           ��ȡ�����������
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
//// ���Ƿ��� true �ĺ���
inline bool AlwaysTrue(const int& i)
{
    (void)i;
    return true;
}

/// �ж�ż��
inline bool IsEven(const int& i)
{
    return i % 2 == 0;
}

/// �ж�����
inline bool IsOdd(const int& i)
{
    return !IsEven(i);
}

/**
 * @brief           index ������
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-11-18 10:41:38
 * @note
 * TODO(lijiancong): ʹ��ģ��������� index ����
 *
 * @code {.cpp}
#include <numeric>
int main()
{
    std::vector<int> vec(10);
    std::iota(vec.begin(), vec.end(), 0);   ///< {0, 1, ..., 9}

    gos::index_utility generator(vec);
    /// ���� 0 ��� index �Ѿ���ʹ��
    generator.SetIndex(0);

    int index_odd;
    int index_even;
    if(generator.GetNextIndex(index_odd, gos::IsOdd) &&  ///< ��ȡ���� index
       generator.GetNextIndex(index_even, gos::IsEven))  ///< ��ȡż�� index
    {
        /// ���� index ����ȡ�ɹ���
        /// �黹ż��index
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
    /// ʹ�� index ���г�ʼ��
    index_utility(const std::vector<int>& vecIndex)
    {
        m_UnusedIndex = vecIndex;
        std::sort(m_UnusedIndex.begin(), m_UnusedIndex.end());
    }

    /// ��ȡ��һ��δʹ�õ� index
    bool GetNextIndex(int& index)
    {
        return GetNextIndex(index, AlwaysTrue);
    }

    /**
     * @brief           ��ȡȥ��һ������Ҫ��� index
     * @param           index           [out]
     * @param           p               [in]   ������������ӦΪ bool (const int&)
     * @return          true    �ҵ���
     * @return          false   û�ҵ�
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

    /// ����ĳ�� index �Ѿ���ʹ��
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

    /// ����ĳ�� index δ��ʹ��
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
    std::vector<int> m_CurrentIndex;  ///< ��ǰ�Ѿ�ʹ�õ� index
    std::vector<int> m_UnusedIndex;   ///< ��ǰδʹ�õ� index
};
}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_H__
