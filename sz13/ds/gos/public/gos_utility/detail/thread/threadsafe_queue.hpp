/**
 * @file            threadsafe_queue.hpp
 * @brief           线程安全队列实现
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2022-01-06 13:40:30
 * @copyright       Copyright (c) 2022 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_THREAD_THREADSAFE_QUEUE_H__
#define DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_THREAD_THREADSAFE_QUEUE_H__

#include <queue>
#include <deque>
#include "gos_utility/detail/thread/mutex_utility.hpp"
#include "gos_utility/detail/thread/lock_guard.hpp"

namespace gos
{
template <typename T, typename Mutex = gos::mutex>
class threadsafe_deque
{
public:
    threadsafe_deque() {}
    virtual ~threadsafe_deque(){}
    /// 获取大小
    unsigned long size() const
    {
        gos::lock_guard<gos::mutex> lock(m_mutex);
        return m_deque.size();
    }

    /// 在队列尾部添加元素
    void push_back(const gos::shared_ptr<T>& sp)
    {
        gos::lock_guard<gos::mutex> lock(m_mutex);
        m_deque.push_back(sp);
    }

    /// 在队列头部添加元素
    void push_front(const gos::shared_ptr<T>& sp)
    {
        gos::lock_guard<gos::mutex> lock(m_mutex);
        m_deque.push_front(sp);
    }

    /// 从队列头出列元素
    bool pop_front(gos::shared_ptr<T>& sp)
    {
        gos::lock_guard<gos::mutex> lock(m_mutex);
        if (m_deque.empty())
        {
            return false;
        }
        else
        {
            sp = m_deque.front();
            m_deque.pop_front();
            return true;
        }
    }

    /// 从队列尾出列元素
    bool pop_back(gos::shared_ptr<T>& sp)
    {
        gos::lock_guard<gos::mutex> lock(m_mutex);
        if (m_deque.empty())
        {
            return false;
        }
        else
        {
            sp = m_deque.back();
            m_deque.pop_back();
            return true;
        }
    }

    /// 清空
    void clear()
    {
        gos::lock_guard<gos::mutex> lock(m_mutex);
        m_deque.clear();
    }

private:
    std::deque<gos::shared_ptr<T> > m_deque;
    mutable Mutex m_mutex;

    bool empty() const
    {
        return m_deque.empty();
    }

    threadsafe_deque(const threadsafe_deque&);
    threadsafe_deque& operator=(const threadsafe_deque&);
};

template <typename T, typename ThreadsafeDeque = threadsafe_deque<T> >
class threadsafe_queue
{
public:
    threadsafe_queue() {}

    virtual ~threadsafe_queue() {}

    unsigned long size() const
    {
        return m_queue.size();
    }

    bool pop(gos::shared_ptr<T>& sp)
    {
        return m_queue.pop_front(sp);
    }

    void push(const gos::shared_ptr<T>& item)
    {
        return m_queue.push_back(item);
    }

    void clear()
    {
        m_queue.clear();
    }

private:
    ThreadsafeDeque m_queue;

    threadsafe_queue(const threadsafe_queue<T>&);
    threadsafe_queue& operator=(const threadsafe_queue<T>&);
};
}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_THREAD_THREADSAFE_QUEUE_H__