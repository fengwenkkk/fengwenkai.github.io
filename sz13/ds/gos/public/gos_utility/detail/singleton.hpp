/**
 * @file            singleton_utility.hpp
 * @brief           ����ʵ�ֵ����Ļ���
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
 * @brief           ���ڼ̳еĵ���ʵ��
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-12-20 09:02:58
 * @note
 *
 * ʵ����������:
 * @code
 * /// ���м̳�
 * class A : public gos::singleton<A>
 * {
 *     /// ������Ԫ��
 *     friend class gos::singleton<A>;
 * private:
 *      A(){}   ///< ���캯����Ϊ˽�е�
 * };
 * @endcode
 */
template <typename T>
class singleton
{
public:
#ifdef __GOS_CXX11_AFTER__
    /**
     * @brief           ��ȡ����
     * @return          T&
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-12-17 14:55:23
     * @note
     * �ú����̰߳�ȫ����ֻ�� C++11 �Ժ�����
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
     * @brief           ��ȡ����
     * @return          T&
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2021-12-17 14:48:14
     * @note
     * 1. ������C++11 ��ǰ
     * 2. ����ʹ�õ��� double-check �ķ���������߳�������⣬��������������̰߳�ȫ���ڼ���������»���ֶ�д����
     * 3. ��ƪ���Ľ�����Ϊʲô����ȫ
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
    /// ��������������ֹ�ڴ�й©
    virtual ~singleton() {}

private:
    /// �������캯����Ϊ˽���Ҳ�ʵ�֣���ֹ��������
    singleton(const singleton&);

    /// ������ֵ����Ϊ˽���Ҳ�ʵ�֣���ֹ��������
    singleton& operator=(const singleton&);
};
}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_SINGLETON_UTILITY_H__