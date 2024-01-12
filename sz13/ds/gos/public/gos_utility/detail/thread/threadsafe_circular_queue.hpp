/**
 * @file            threadsafe_circular_queue.hpp
 * @brief           线程安全循环队列的实现
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2022-01-25 18:23:46
 * @copyright       Copyright (c) 2021-2022 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_THREAD_THREADSAFE_CIRCULAR_QUEUE_HPP__
#define DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_THREAD_THREADSAFE_CIRCULAR_QUEUE_HPP__

#include "gos_utility/detail/data_structure/circular_queue.hpp"

namespace gos
{
template <typename T, typename Mutex = gos::mutex>
class threadsafe_circular_queue
{
public:
    threadsafe_circular_queue(const size_t& max_count) : m_queue(max_count) {}

    void push(const T& element)
    {
        gos::lock_guard<Mutex> Guard(m_mutex);
        while (m_queue.full())
        {
            m_queue.pop_front();
        }
        m_queue.push_back(element);
    }

    bool pop(T& element)
    {
        if (m_queue.empty())
        {
            return false;
        }
        else
        {
            gos::lock_guard<Mutex> Guard(m_mutex);
            element = m_queue.front();
            m_queue.pop_front();
            return true;
        }
    }

    bool empty()
    {
        gos::lock_guard<Mutex> Guard(m_mutex);
        return m_queue.empty();
    }

    void clear()
    {
        gos::lock_guard<Mutex> Guard(m_mutex);
        m_queue.clear();
    }

private:
    Mutex m_mutex;
    circular_queue<T> m_queue;
};
}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_THREAD_THREADSAFE_CIRCULAR_QUEUE_HPP__