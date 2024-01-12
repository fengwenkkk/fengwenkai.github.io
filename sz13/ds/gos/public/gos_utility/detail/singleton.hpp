/**
 * @file            singleton_utility.hpp
 * @brief           用于实现单例的基类
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-12-17 14:23:38
 * @copyright       Copyright (c) 2021 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_SINGLETON_UTILITY_H__
#define DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_SINGLETON_UTILITY_H__

#include "gos_utility/detail/macro_switch.hpp"
#include "gos_utility/detail/thread/mutex_utility.hpp"
#include "gos_utility/detail/thread/lock_guard.hpp"

#ifdef __GOS_CXX11_AFTER__
#include <mutex>
#endif

namespace gos
{
/**
 * @brief           用于继承的单例实现
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-12-20 09:02:58
 * @note
 *
 * 实例代码如下:
 * @code
 * /// 公有继承
 * class A : public gos::singleton<A>
 * {
 *     /// 定义友元类
 *     friend class gos::singleton<A>;
 * private:
 *      A(){}   ///< 构造函数需为私有的
 * };
 * @endcode
 */
template <typename T>
class singleton
{
public:
#ifdef __GOS_CXX11_AFTER__
    /**
     * @brief           获取单例
     * @return          T&
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-12-17 14:55:23
     * @note
     * 该函数线程安全，但只能 C++11 以后适用
     */
    static T& GetInstance()
    {
        static std::once_flag init_flag;
        static T* pInstance = nullptr;

        std::call_once(init_flag, [&]() { pInstance = new T; });
        return *pInstance;
    }
#else
    /**
     * @brief           获取单例
     * @return          T&
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-12-17 14:48:14
     * @note
     * 1. 适用于C++11 以前
     * 2. 这里使用的是 double-check 的方法来规避线程相关问题，但这个函数并不线程安全，在极少数情况下会出现读写竞争
     * 3. 这篇论文介绍了为什么不安全
     * https://github.com/Mercy1101/my_doc/blob/master/PDF/C%2B%2B%20and%20the%20Perils%20of%20Double-Checked%20Locking.pdf
     */
    static T& GetInstance()
    {
        static gos::mutex mutex;
        static T* pInstance = NULL;

        if (pInstance == NULL)
        {
            gos::lock_guard<gos::mutex> guard(mutex);
            if (pInstance == NULL)
            {
                pInstance = new T;
            }
        }
        return *pInstance;
    }
#endif

protected:
    singleton() {}
    /// 虚析构函数，防止内存泄漏
    virtual ~singleton() {}

private:
    /// 拷贝构造函数设为私有且不实现，防止拷贝操作
    singleton(const singleton&);

    /// 拷贝赋值符设为私有且不实现，防止拷贝操作
    singleton& operator=(const singleton&);
};
}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_SINGLETON_UTILITY_H__