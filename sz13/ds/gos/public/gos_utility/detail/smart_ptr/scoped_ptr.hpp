/**
 * @file            scoped_ptr.hpp
 * @brief           作用域限定的智能指针
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2022-02-09 16:33:23
 * @copyright       Copyright (c) 2021-2022 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_SMART_PTR_SCOPED_PTR_HPP__
#define DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_SMART_PTR_SCOPED_PTR_HPP__

#include <cassert>

#include "gos_utility/detail/macro_switch.hpp"
#include "gos_utility/detail/noncopyable.hpp"
#include "gos_utility/detail/check_delete.hpp"

namespace gos
{
//  scoped_ptr mimics a built-in pointer except that it guarantees deletion
//  of the object pointed to, either on destruction of the scoped_ptr or via
//  an explicit reset(). scoped_ptr is a simple solution for simple needs;
//  use shared_ptr or std::auto_ptr if your needs are more complex.

template <class T>
class scoped_ptr : gos::noncopyable
{
    typedef scoped_ptr<T> this_type;

public:
    typedef T element_type;

    explicit scoped_ptr(T* p = nullptr) : px(p)
    {
    }

    ~scoped_ptr()
    {
        gos::checked_delete(px);
    }

    void reset(T* p = nullptr)
    {
        assert(p == nullptr || p != px);  // catch self-reset errors
        this_type(p).swap(*this);
    }

    T& operator*() const
    {
        assert(px != 0);
        return *px;
    }

    T* operator->() const
    {
        assert(px != 0);
        return px;
    }

    T* get() const
    {
        return px;
    }

    void swap(scoped_ptr& b)
    {
        T* tmp = b.px;
        b.px = px;
        px = tmp;
    }

private:
    T* px;

};
}  // namespace gos
#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_SMART_PTR_SCOPED_PTR_HPP__