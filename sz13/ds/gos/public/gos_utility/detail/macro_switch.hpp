/**
 * @file            macro_switch.hpp
 * @brief           ���ļ��д�Ź��ں궨�忪�صĶ���
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

/// ��Ԫ���Ժ궨�壨���ݹ�����Ҫ���ڹ�������ӱ�����꣩
#ifdef USE_GOS_UNITTEST
#include "gos_utility/thirdparty/catch.hpp"  ///< ��������Ԫ���Կ�
#endif

/// PROFILER�궨��
#ifdef _DEBUG
#define USE_GOS_PROFILER
#else
#undef USE_GOS_PROFILER
#endif

/// LINE_INFO����
#ifdef _DEBUG
#define USE_GOS_LINE_INFO
#else
#undef USE_GOS_LINE_INFO
#endif

/// ʹ����־���Ŀ���
#ifdef _DEBUG
#define USE_LOG_STREAM_MACRO
#else
#undef USE_LOG_STREAM_MACRO
#endif

/// ʹ��__cplusplus�궨�����жϱ�����֧�ֵ�C++��׼
/// �ο����ϣ�https://stackoverflow.com/questions/52379233/is-has-cxx17-marco-usable-in-custom-project-headers-to-enable-c17-language
/// ע��: VS����ʱ��Ҫ���ѡ�� /Zc:__cplusplus ��������ʾ��ȷ��ֵ��
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

/// �ж��Ƿ�ʹ�� Visutal Studio ����
#ifdef _MSC_VER
#define __GOS_USE_VS__
#endif

/// �ж��Ƿ�ʹ�� GNU ����
#ifdef __GNUC__
#define __GOS_USE_GNU__
#endif

/// �ж��Ƿ�ʹ�� GNU ����
#ifdef __GNUG__
#define __GOS_USE_GNU__
#endif

/// �ж��Ƿ�ʹ�� clang ����
#ifdef __clang__
#define __GOS_USE_CLANG__
#endif

/// �ж��Ƿ�ʹ�� C++ Builder ����
#ifdef __BORLANDC__
#define __GOS_USE_BCB__
#endif

/// ������C++11֮ǰ����֮��
#if defined(__GOS_HAS_CXX11__) || defined(__GOS_HAS_CXX14__) || defined(__GOS_HAS_CXX17__) || defined(__GOS_HAS_CXX20__)
#define __GOS_CXX11_AFTER__
#else
#define __GOS_CXX11_BEFORE__
#endif

/// C++11 ֮ǰʹ�ú궨�����ؼ���
#if defined(__GOS_CXX11_BEFORE__)
#define nullptr NULL
#define override
#define final
#define constexpr const
#define explicit
#define noexcept throw()
#endif

/// ʲôҲ�����ĺ궨�壬�������յĴ����
#define DoNothing()

#endif  ///< GOS_UTILITY_MACRO_UTILITY_HPP__
