/**
 * @file            macro_switch.hpp
 * @brief           本文件中存放关于宏定义开关的定义
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-07-17 14:19:25
 *
 * @copyright       Copyright (c) 2021 GBCOM
 *
 */

#ifndef GOS_UTILITY_MACRO_UTILITY_HPP__
#define GOS_UTILITY_MACRO_UTILITY_HPP__

#include <stdint.h>

/// 单元测试宏定义（根据工程需要，在工程中添加本编译宏）
#ifdef USE_GOS_UNITTEST
#include "gos_utility/thirdparty/catch.hpp"  ///< 第三方单元测试库
#endif

/// PROFILER宏定义
#ifdef _DEBUG
#define USE_GOS_PROFILER
#else
#undef USE_GOS_PROFILER
#endif

/// LINE_INFO开关
#ifdef _DEBUG
#define USE_GOS_LINE_INFO
#else
#undef USE_GOS_LINE_INFO
#endif

/// 使用日志流的开关
#ifdef _DEBUG
#define USE_LOG_STREAM_MACRO
#else
#undef USE_LOG_STREAM_MACRO
#endif

/// 使用__cplusplus宏定义来判断编译器支持的C++标准
/// 参考资料：https://stackoverflow.com/questions/52379233/is-has-cxx17-marco-usable-in-custom-project-headers-to-enable-c17-language
/// 注意: VS编译时需要添加选项 /Zc:__cplusplus ，才能显示正确的值。
#if __cplusplus >= 1
#define __GOS_HAS_PRE_CXX98__
#endif

#if __cplusplus >= 199711L
#define __GOS_HAS_CXX98__
#endif

#if __cplusplus >= 201103L
#define __GOS_HAS_CXX11__
#endif

#if __cplusplus >= 201402L
#define __GOS_HAS_CXX14__
#endif

#if __cplusplus >= 201703L
#define __GOS_HAS_CXX17__
#endif

#if __cplusplus >= 202004L
#define __GOS_HAS_CXX20__
#endif

/// 判断是否使用 Visutal Studio 编译
#ifdef _MSC_VER
#define __GOS_USE_VS__
#endif

/// 判断是否使用 GNU 编译
#ifdef __GNUC__
#define __GOS_USE_GNU__
#endif

/// 判断是否使用 GNU 编译
#ifdef __GNUG__
#define __GOS_USE_GNU__
#endif

/// 判断是否使用 clang 编译
#ifdef __clang__
#define __GOS_USE_CLANG__
#endif

/// 判断是否使用 C++ Builder 编译
#ifdef __BORLANDC__
#define __GOS_USE_BCB__
#endif

/// 定义是C++11之前还是之后
#if defined(__GOS_HAS_CXX11__) || defined(__GOS_HAS_CXX14__) || defined(__GOS_HAS_CXX17__) || defined(__GOS_HAS_CXX20__)
#define __GOS_CXX11_AFTER__
#else
#define __GOS_CXX11_BEFORE__
#endif

/// C++11 之前使用宏定义代替关键字
#if defined(__GOS_CXX11_BEFORE__)
#define nullptr NULL
#define override
#define final
#define constexpr const
#define explicit
#define noexcept throw()
#endif

/// 什么也不做的宏定义，用于填充空的代码段
#define DoNothing()

#endif  ///< GOS_UTILITY_MACRO_UTILITY_HPP__
