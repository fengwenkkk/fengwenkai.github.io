/**
 * @file            check_delete.hpp
 * @brief           编译期检查指针是否指向不完全类型，如果是则编译不通过
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2022-02-09 16:47:41
 * @note
 * 摘抄于 boost/core/checked_delete.hpp
 * 具体文档见[checked_delete](http://www.boost.org/libs/core/doc/html/core/checked_delete.html)
 */

//
//  boost/checked_delete.hpp
//
//  Copyright (c) 2002, 2003 Peter Dimov
//  Copyright (c) 2003 Daniel Frey
//  Copyright (c) 2003 Howard Hinnant
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/core/doc/html/core/checked_delete.html for documentation.
//

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_CHECK_DELETE_HPP__
#define DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_CHECK_DELETE_HPP__

#include "gos_utility/detail/macro_switch.hpp"

namespace gos
{
// verify that types are complete for increased safety

template <class T>
inline void checked_delete(T* x) noexcept
{
    // intentionally complex - simplification causes regressions
    typedef char type_must_be_complete[sizeof(T) ? 1 : -1];
    (void)sizeof(type_must_be_complete);
    delete x;
}

template <class T>
inline void checked_array_delete(T* x) noexcept
{
    typedef char type_must_be_complete[sizeof(T) ? 1 : -1];
    (void)sizeof(type_must_be_complete);
    delete[] x;
}

template <class T>
struct checked_deleter
{
    typedef void result_type;
    typedef T* argument_type;

    void operator()(T* x) const noexcept
    {
        gos::checked_delete(x);
    }
};

template <class T>
struct checked_array_deleter
{
    typedef void result_type;
    typedef T* argument_type;

    void operator()(T* x) const noexcept
    {
        gos::checked_array_delete(x);
    }
};

}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_CHECK_DELETE_HPP__