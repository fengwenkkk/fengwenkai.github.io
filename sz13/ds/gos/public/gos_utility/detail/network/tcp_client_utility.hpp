/**
 * @file            tcp_client_utility.hpp
 * @brief           tcp client 类的实现
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2022-01-05 11:07:24
 * @copyright       Copyright (c) 2022 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_NETWORK_TCP_CLIENT_UTILITY_HPP__
#define DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_NETWORK_TCP_CLIENT_UTILITY_HPP__

#include "gos_typedef.h"
#include "gos_api.h"
#include "gos_utility/detail/network/net_utility.hpp"
#include "gos_utility/detail/noncopyable.hpp"

namespace gos
{
template <typename BUFFER_TYPE = unsigned char>
class tcp_client : gos::noncopyable
{
public:
    tcp_client(const IPV4_INFO_C& stIP) : m_stTCPClientSocket(INVALID_SOCKET), m_stIPInfo(stIP)
    {
        memset(m_aucRecvBuffer, 0, sizeof(m_aucRecvBuffer));
    }

    ~tcp_client()
    {
        gos_close_socket(&m_stTCPClientSocket);
    }

    /**
     * @brief           接收 tcp server 发来的消息
     * @param           vecRecvBuffer   [out]  接收到的数据
     * @param           ulTimeout       [in]   接收超时时间, 单位是毫秒
     * @return          true    接收成功
     * @return          false   接收超时或接收错误
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2022-01-05 14:22:14
     * @note
     */
    bool Receive(std::vector<BUFFER_TYPE>& vecRecvBuffer, unsigned ulTimeout = 0)
    {
        if (!Connect())
        {
            return false;
        }

        memset(m_aucRecvBuffer, 0, sizeof(m_aucRecvBuffer));
        INT32 iReceiveLength = gos_recv_tcp_msg(m_stTCPClientSocket, static_cast<void*>(m_aucRecvBuffer),
                                                sizeof(m_aucRecvBuffer), ulTimeout);
        if (iReceiveLength >= 0)
        {
            vecRecvBuffer.clear();
            vecRecvBuffer.reserve(iReceiveLength);
            for (UINT32 i = 0; i < iReceiveLength; i++)
            {
                vecRecvBuffer.push_back(m_aucRecvBuffer[i]);
            }
            return true;
        }
        else
        {
            /// LOGE << "Tcp Client Receive failed! this: " << gos::to_string(this) << ", " << m_stIPInfo;
            gos_close_socket(&m_stTCPClientSocket);
            return false;
        }
    }

    /**
     * @brief           发送消息
     * @param           vecSendBuffer    [in]
     * @return          true
     * @return          false
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2022-01-05 15:01:44
     * @note
     */
    bool Send(const std::vector<BUFFER_TYPE>& vecSendBuffer)
    {
        if (!Connect())
        {
            return false;
        }

        gos::shared_ptr_array<BUFFER_TYPE> sp(new BUFFER_TYPE[vecSendBuffer.size()]);
        memset(sp.get(), 0, vecSendBuffer.size());
        std::copy(vecSendBuffer.begin(), vecSendBuffer.end(), sp.get());

        bool iRet = gos_tcp_send(m_stTCPClientSocket, static_cast<void*>(sp.get()), vecSendBuffer.size()) == TRUE;

        if (!iRet)
        {
            /// LOGE << "Tcp Client Send failed! this: " << gos::to_string(this) << ", " << m_stIPInfo;
            gos_close_socket(&m_stTCPClientSocket);
        }

        return iRet;
    }

private:
    SOCKET m_stTCPClientSocket;    ///< Socket
    BUFFER_TYPE m_aucRecvBuffer[60 * 1024];  ///< 接收缓冲区
    IPV4_INFO_C m_stIPInfo;        ///< IP 地址

    /**
     * @brief           连接 tcp 服务器
     * @return          true
     * @return          false
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2022-01-05 14:25:13
     * @note
     */
    bool Connect()
    {
        if (m_stTCPClientSocket == INVALID_SOCKET)
        {
            if (!m_stIPInfo.IsValid())
            {
                return false;
            }

            IP_ADDR_T stIP = m_stIPInfo.ToIPAddr();
            m_stTCPClientSocket = gos_connect_tcp_server(stIP.aucIP, stIP.usPort);
        }

        if (m_stTCPClientSocket == INVALID_SOCKET)
        {
            /// LOGE << "Tcp Client Connect failed! this: " << gos::to_string(this) << ", " << m_stIPInfo;
            return false;
        }
        else
        {
            return true;
        }
    }
};
}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_NETWORK_TCP_CLIENT_UTILITY_HPP__