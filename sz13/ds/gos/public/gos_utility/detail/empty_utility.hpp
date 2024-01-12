/**
 * @file            empty_utility.hpp
 * @brief           std::empty��ʵ��
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-07-14 15:13:37
 *
 * @copyright       Copyright (c) 2021 GBCOM
 *
 */

#ifndef GOS_PUBLIC_GOS_UTILITY_DETAIL_EMPTY_HPP__
#define GOS_PUBLIC_GOS_UTILITY_DETAIL_EMPTY_HPP__

#include <vector>
#include <algorithm>

namespace gos
{
/**
 * @brief           �ж����������Ƿ�Ϊ��
 * @param           data    [in]
 * @return          true    Ϊ��
 * @return          false   ��Ϊ��
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-02 13:23:08
 * @note
 */
template <typename T>
inline bool empty(T data)
{
    return data.empty();
}

/**
 * @brief           �ж�����Ϊ�յ�ģ��������
 * @param           str    [in]
 * @return          true
 * @return          false
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-09-02 13:23:34
 * @note
 */
template <>
inline bool empty(char* str)
{
    return str != NULL && *str != '\0';
}

}  // namespace gos

#endif  ///< GOS_PUBLIC_GOS_UTILITY_DETAIL_EMPTY_HPP__
