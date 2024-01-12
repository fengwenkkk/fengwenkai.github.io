/**
 * @file            final_action.hpp
 * @brief           final_action 的实现
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2022-01-25 14:04:56
 * @copyright       Copyright (c) 2021-2022 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_FINAL_ACTION_HPP__
#define DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_FINAL_ACTION_HPP__

#include <cstdio>

namespace gos
{
/**
 * @brief           final_action 的实现
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-25 14:07:35
 * @note
 * 参考资料:
 * [C.30: Define a destructor if a class needs an explicit action at object destruction]
 * (https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
 *
 * [gsl-lite: Guidelines Support Library for C++98, C++11 up]
 * (https://github.com/gsl-lite/gsl-lite/blob/b2629bdcec4f4207b8706280036e46bddac78f32/include/gsl/gsl-lite-vc6.hpp)
 *
 * [GSL: Guidelines Support Library]
 * (https://github.com/microsoft/GSL/blob/bcf008ae5578fab638e6d30b7ca000f306b14d3b/include/gsl/util)
 */
template <typename F>
class final_action
{
    final_action(F f) : m_f(f) {}
    ~final_action()
    {
        m_f();
    }

private:
    F m_f;
};

class fclose_action
{
public:
    fclose_action(FILE* fp) : m_fp(fp) {}

    void operator()()
    {
        if(m_fp)
        {
            fclose(m_fp);
        }
    }

private:
    FILE* m_fp;
};
}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_FINAL_ACTION_HPP__