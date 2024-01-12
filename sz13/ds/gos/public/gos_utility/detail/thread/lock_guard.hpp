/**
 * @file            lock_guard.hpp
 * @brief           std::lock_guard 实现的对象
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-09-17 10:10:11
 *
 * @copyright       Copyright (c) 2021 GBCOM
 *
 */

#ifndef GOS_PUBLIC_GOS_UTILITY_DETAIL_LOCK_GUARD_HPP__
#define GOS_PUBLIC_GOS_UTILITY_DETAIL_LOCK_GUARD_HPP__

namespace gos
{
template <typename Mutex>
class lock_guard
{
public:
    lock_guard(Mutex& x) : mutex_(x)
    {
        mutex_.lock();
    }

    ~lock_guard()
    {
        mutex_.unlock();
    }

private:
    Mutex& mutex_;
};
}  // namespace gos
#endif  ///< GOS_PUBLIC_GOS_UTILITY_DETAIL_LOCK_GUARD_HPP__