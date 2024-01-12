/**
 * @file            shared_ptr.hpp
 * @brief           ����ָ��ļ�ʵ�֣�m_count��֧���̰߳�ȫ�� ��֧��malloc������ָ�룬Ҳ��֧��new����������ָ��
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-04-13 10:24:17
 * @copyright       Copyright (c) 2022 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_PUBLIC_SMART_PTR_SHARED_PTR_HPP__
#define DS_GOS_PUBLIC_GOS_PUBLIC_SMART_PTR_SHARED_PTR_HPP__

namespace gos
{
/**
 * @brief           Ĭ��������
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-25 15:19:00
 * @note
 */
template <class T>
class default_delete
{
public:
    void operator()(T* p)
    {
        delete p;
    }
};

/**
 * @brief           ����ָ���ʵ��
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-06 15:13:30
 * @note
 * �ӿ���ƶ��ǰ��� C++11 ��׼
 */
template <class T, class Deleter = default_delete<T> >
class shared_ptr
{
public:
    shared_ptr(T* p, Deleter deleter = Deleter()) : m_count(new long(1)), data(p), m_deleter(deleter) {}
    shared_ptr() : m_count(NULL), data(NULL), m_deleter(Deleter()) {}

    shared_ptr(const shared_ptr& rhs)
    {
        copy_shared_ptr(rhs);
    }

    ~shared_ptr()
    {
        reset();
    }

    shared_ptr& operator=(const shared_ptr& rhs)
    {
        if (this != &rhs)
        {
            reset();
            copy_shared_ptr(rhs);
        }
        return *this;
    }

    void reset()
    {
        if (m_count)
        {
            --(*m_count);
            if (*m_count == 0)
            {
                m_deleter(data);
                delete m_count;
            }
        }
    }

    T& operator*() const
    {
        return *data;
    }

    T* operator->() const
    {
        return data;
    }

    T* get() const
    {
        return data;
    }

    long use_count() const
    {
        return m_count ? *m_count : 0;
    }

protected:
    long* m_count;
    T* data;
    Deleter m_deleter;

    void copy_shared_ptr(const shared_ptr& rhs)
    {
        data = rhs.data;
        m_count = rhs.m_count;
        m_deleter = rhs.m_deleter;
        if (m_count)
        {
            ++(*m_count);
        }
    }
};

/**
 * @brief           ����������
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-25 15:19:10
 * @note
 */
template <class T>
class array_delete
{
public:
    void operator()(T* p)
    {
        delete[] p;
    }
};

/**
 * @brief           ������������ָ��
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-25 15:34:56
 * @note
 */
template <class T, class Deleter = gos::array_delete<T> >
class shared_ptr_array : public shared_ptr<T, Deleter>
{
public:
    shared_ptr_array(T* p, Deleter deleter = Deleter()) : shared_ptr<T, Deleter>(p, deleter) {}
    shared_ptr_array() {}

    shared_ptr_array(const shared_ptr_array& stOther)
    {
        copy_shared_ptr(stOther);
    }

    shared_ptr_array& operator=(const shared_ptr_array& stOther)
    {
        if (&stOther != this)
        {
            copy_shared_ptr(stOther);
        }

        return *this;
    }

    T& operator[](unsigned index)
    {
        return this->get()[index];
    }
};
}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_PUBLIC_SMART_PTR_SHARED_PTR_HPP__
