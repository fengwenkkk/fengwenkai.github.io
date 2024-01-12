/**
 * @file optional_utility.hpp
 * @author lijiancong (lijiancong@gbcom.com.cn)
 * @brief   ����ʵ���Լ���std::optional
 * @version 0.1
 * @date 2021-04-13 10:41:27
 *
 * @copyright Copyright (c) 2021 GBCOM
 *
 */

#ifndef GOS_OPTIONAL_UTILITY_HPP__
#define GOS_OPTIONAL_UTILITY_HPP__

#include <vector>
#include <exception>

namespace gos
{
/**
 * @brief ����ʵ��std::optional, �������������Ļ���
 *
 * @tparam T ���ڱ������ݵ�����
 *
 * @note ע����ΪC++11��ǰû��{}��ʽ�ĳ�ʼ��������T���͵�������û�г�ʼ����
 */
template <typename T>
class optional
{
public:
    optional() : has_value_(false), value_() {}
    optional(T value) : has_value_(true), value_(value) {}
    optional(const optional& other) : has_value_(other.has_value_), value_(other.value_) {}

    bool has_value()
    {
        return has_value_;
    }

    T value()
    {
        if (!has_value_)
        {
            throw std::exception("Has no value!");
        }
        else
        {
            return value_;
        }
    }

    T value_or(T default_value)
    {
        if (has_value_)
        {
            return value_;
        }
        else
        {
            return default_value;
        }
    }

private:
    bool has_value_;
    T value_;
};

}  // namespace gos
#endif  ///< GOS_OPTIONAL_UTILITY_HPP__
