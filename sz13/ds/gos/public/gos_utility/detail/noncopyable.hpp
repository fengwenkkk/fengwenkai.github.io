/**
 * @file            noncopyable.hpp
 * @brief           不可拷贝基类对象
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2022-01-20 17:43:54
 * @copyright       Copyright (c) 2022 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_NONCOPYABLE_HPP__
#define DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_NONCOPYABLE_HPP__

#include "gos_utility/detail/macro_switch.hpp"

namespace gos
{
/**
 * @brief           继承此基类会让派生类不可拷贝、不可复制
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-20 18:50:52
 * @note
 */
class noncopyable
{
protected:
#if !defined(__GOS_CXX11_BEFORE__)
    constexpr noncopyable() = default;
    ~noncopyable() = default;
#else
    noncopyable() {}
    ~noncopyable() {}
#endif
#if !defined(__GOS_CXX11_BEFORE__)
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
#else
private:  // emphasize the following members are private
    noncopyable(const noncopyable&);
    noncopyable& operator=(const noncopyable&);
#endif
};
}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_NONCOPYABLE_HPP__