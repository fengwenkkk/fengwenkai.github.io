/**
 * @file            random_utility.hpp
 * @brief           ��������ĺ���ʵ��
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-12-31 16:53:20
 * @copyright       Copyright (c) 2021 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_RANDOM_UTILITY_H__
#define DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_RANDOM_UTILITY_H__

#include <algorithm>
#include <stdlib.h>
#include <time.h>

namespace gos
{
/**
 * @brief           ��ȡ [0, RAND_MAX] �����ڵ��������
 * @return          int
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-12-31 17:08:30
 * @note
 */
inline unsigned get_random_number()
{
    static bool bIsFirst = true;
    if (bIsFirst)
    {
        bIsFirst = false;
        srand(time(NULL));
    }
    return rand();
}

/**
 * @brief           ��ȡ [lower_bound, upper_bound] ������������
 * @param           lower_bound    [in]
 * @param           upper_bound    [in]
 * @return          unsigned
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-12-31 17:09:20
 * @note
 */
inline unsigned get_random_number(unsigned lower_bound, unsigned upper_bound)
{
    if (lower_bound == upper_bound)
    {
        return lower_bound;
    }

    if (lower_bound > upper_bound)
    {
        std::swap(lower_bound, upper_bound);
    }

    return get_random_number() % (upper_bound - lower_bound + 1) + lower_bound;
}

}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_RANDOM_UTILITY_H__