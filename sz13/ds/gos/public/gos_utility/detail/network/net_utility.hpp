/**
 * @file            net_utility.hpp
 * @brief           用于写网络相关的工具函数和定义
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-08-26 15:08:28
 *
 * @copyright       Copyright (c) 2021 GBCOM
 *
 */

#ifndef GOS_PUBLIC_GOS_UTILITY_DETAIL_NET_UTILITY_HPP__
#define GOS_PUBLIC_GOS_UTILITY_DETAIL_NET_UTILITY_HPP__

#include <string>
#include "gos_typedef.h"
#include "gos_utility/detail/input_check_utility.hpp"

class IPV4_INFO_C
{
public:
    IPV4_INFO_C() : iPort(-1) {}
    IPV4_INFO_C(const std::string& ip, const int& port) : strIP(ip), iPort(port) {}

    IPV4_INFO_C(const IPV4_INFO_C& other)
    {
        strIP = other.strIP;
        iPort = other.iPort;
    }

    IPV4_INFO_C& operator=(const IPV4_INFO_C& other)
    {
        if (&other != this)
        {
            strIP = other.strIP;
            iPort = other.iPort;
        }
        return *this;
    }

    /// 判断该结构体是否有效
    bool IsValid() const
    {
        return gos::IsInputIPV4Valid(strIP) && (iPort >= 0 && iPort <= 0xffff);
    }

    VOID Set(IP_ADDR_T& stIP)
    {
        CHAR acIP[32];

        sprintf(acIP, IP_FMT, IP_ARG(stIP.aucIP));
        strIP = std::string(acIP);
        iPort = stIP.usPort;
    }

    /// 把该结构体转换为 IP_ADDR_T 的结构体返回
    IP_ADDR_T ToIPAddr()
    {
        IP_ADDR_T st;

        if (strIP.empty())
        {
            st.aucIP[0] = 0;
            st.aucIP[1] = 0;
            st.aucIP[2] = 0;
            st.aucIP[3] = 0;
        }
        else
        {
            gos_get_ip((CHAR*)strIP.c_str(), st.aucIP);
        }
        st.usPort = iPort;
        return st;
    }

    friend gos::string_stream& operator<<(gos::string_stream& out, const IPV4_INFO_C& obj)
    {
        out << " IPV4_INFO_C: ";
        out << "strIP: " << obj.strIP;
        out << ", iPort: " << obj.iPort;
        return out;
    }

public:
    std::string strIP;  ///< IP地址
    int iPort;          ///< IP端口
};

#endif  ///< GOS_PUBLIC_GOS_UTILITY_DETAIL_NET_UTILITY_HPP__