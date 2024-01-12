/**
 * @file            threadsafe_stack.hpp
 * @brief           线程安全 栈
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2022-01-13 10:00:06
 * @copyright       Copyright (c) 2022 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_THREAD_THREADSAFE_STACK_HPP__
#define DS_GOS_PUBLIC_GOS_UTILITY_THREAD_THREADSAFE_STACK_HPP__

#include "gos_utility/detail/thread/threadsafe_queue.hpp"

namespace gos
{
template <typename T, typename ThreadSafeDeque = threadsafe_deque<T> >
class threadsafe_stack
{
public:
    void push(const gos::shared_ptr<T>& sp)
    {
        m_Dequeue.push_back(sp);
    }

    void pop(gos::shared_ptr<T>& sp)
    {
        m_Dequeue.push_front(sp);
    }

    unsigned long size()
    {
        return m_Dequeue.size();
    }

    void clear()
    {
        m_Dequeue.clear();
    }

private:
    ThreadSafeDeque m_Dequeue;
};
}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_THREAD_THREADSAFE_STACK_HPP__