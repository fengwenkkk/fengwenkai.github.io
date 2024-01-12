/**
 * @file            circular_queue.hpp
 * @brief           环形队列的实现
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2022-01-25 17:32:56
 * @copyright       Copyright (c) 2021-2022 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_CIRCULAR_QUEUE_HPP__
#define DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_CIRCULAR_QUEUE_HPP__

#include <vector>

#include "gos_utility/detail/macro_switch.hpp"
#include "gos_utility/detail/noncopyable.hpp"

namespace gos
{
/**
 * @brief           循环队列的实现
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-25 17:36:18
 * @note
 * 参考资料:
 * [循环队列实现(C++) Ring Buffer]
 * (https://www.cnblogs.com/diegodu/p/4619104.html)
 *
 * [boostorg/circular_buffer]
 * (https://github.com/boostorg/circular_buffer/blob/develop/include/boost/circular_buffer/base.hpp)
 *
 * [JustasMasiulis/circular_buffer]
 * (https://github.com/JustasMasiulis/circular_buffer/blob/master/include/circular_buffer.hpp)
 *
 * [jnk0le/Ring-Buffer]
 * (https://github.com/jnk0le/Ring-Buffer/blob/master/ringbuffer.hpp)
 *
 * [gabime/spdlog]
 * (https://github.com/gabime/spdlog/blob/v1.x/include/spdlog/details/circular_queue.h)
 *
 * [leetcode 622. 设计循环队列]
 * (https://leetcode-cn.com/problems/design-circular-queue/)
 */
template <typename T>
class circular_queue : gos::noncopyable
{
public:
    // empty ctor - create a disabled queue with no elements allocated at all
    circular_queue() : max_items_(0), head_(0), tail_(0), overrun_counter_(0) {}

    explicit circular_queue(size_t max_items)
        : max_items_(max_items + 1),  // one item is reserved as marker for full q
          head_(0),
          tail_(0),
          overrun_counter_(0),
          v_(max_items_)
    {
    }

    // push back, overrun (oldest) item if no room left
    void push_back(const T &item)
    {
        if (max_items_ > 0)
        {
            v_[tail_] = item;
            tail_ = (tail_ + 1) % max_items_;

            if (tail_ == head_)  // overrun last item if full
            {
                head_ = (head_ + 1) % max_items_;
                ++overrun_counter_;
            }
        }
    }

    // Return reference to the front item.
    // If there are no elements in the container, the behavior is undefined.
    const T &front() const
    {
        return v_[head_];
    }

    T &front()
    {
        return v_[head_];
    }

    // Return number of elements actually stored
    size_t size() const
    {
        if (tail_ >= head_)
        {
            return tail_ - head_;
        }
        else
        {
            return max_items_ - (head_ - tail_);
        }
    }

    // Return const reference to item by index.
    // If index is out of range 0…size()-1, the behavior is undefined.
    const T &at(size_t i) const
    {
        assert(i < size());
        return v_[(head_ + i) % max_items_];
    }

    // Pop item from front.
    // If there are no elements in the container, the behavior is undefined.
    void pop_front()
    {
        head_ = (head_ + 1) % max_items_;
    }

    bool empty() const
    {
        return tail_ == head_;
    }

    bool full() const
    {
        // head is ahead of the tail by 1
        if (max_items_ > 0)
        {
            return ((tail_ + 1) % max_items_) == head_;
        }
        return false;
    }

    void clear()
    {
        v_.clear();
        tail_ = 0;
        head_ = 0;
        overrun_counter_ = 0;
    }

    size_t overrun_counter() const
    {
        return overrun_counter_;
    }

private:
    size_t max_items_;
    typename std::vector<T>::size_type head_;
    typename std::vector<T>::size_type tail_;
    size_t overrun_counter_;
    std::vector<T> v_;

    /// circular_queue(const circular_queue &);
    /// circular_queue &operator=(const circular_queue &);
};
}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_CIRCULAR_QUEUE_HPP__