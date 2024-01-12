/**
 * @file          ignore_unused.hpp
 * @brief         忽略编译器产生某个函数变量没有被使用的警告
 * @author        lijiancong(lijiancong@gbcom.com.cn)
 * @version       0.1
 * @date          2021-04-23 13:50:18
 *
 * @copyright     Copyright (c) 2021 GBCOM
 *
 */

#ifndef GOS_IGNORE_UNUSED_HPP__
#define GOS_IGNORE_UNUSED_HPP__

namespace gos
{
template <typename T1>
inline void ignore_unused(T1 const&)
{
}

template <typename T1, typename T2>
inline void ignore_unused(T1 const&, T2 const&)
{
}

template <typename T1, typename T2, typename T3>
inline void ignore_unused(T1 const&, T2 const&, T3 const&)
{
}

template <typename T1, typename T2, typename T3, typename T4>
inline void ignore_unused(T1 const&, T2 const&, T3 const&, T4 const&)
{
}

template <typename T1, typename T2, typename T3, typename T4, typename T5>
inline void ignore_unused(T1 const&, T2 const&, T3 const&, T4 const&, T5 const&)
{
}

template <typename T1>
inline void ignore_unused()
{
}

template <typename T1, typename T2>
inline void ignore_unused()
{
}

template <typename T1, typename T2, typename T3>
inline void ignore_unused()
{
}

template <typename T1, typename T2, typename T3, typename T4>
inline void ignore_unused()
{
}

template <typename T1, typename T2, typename T3, typename T4, typename T5>
inline void ignore_unused()
{
}
}  // namespace gos

#endif  ///< GOS_IGNORE_UNUSED_HPP__
