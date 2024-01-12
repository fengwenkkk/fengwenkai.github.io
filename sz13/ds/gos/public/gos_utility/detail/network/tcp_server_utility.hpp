/**
 * @file            tcp_server_utility.hpp
 * @brief           tcp 服务器
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2022-01-27 11:07:55
 * @copyright       Copyright (c) 2021-2022 GBCOM
 */

#ifndef DC_GOS_PUBLIC_GOS_UTILITY_DETAIL_NETWORK_TCP_SERVER_UTILITY_HPP__
#define DC_GOS_PUBLIC_GOS_UTILITY_DETAIL_NETWORK_TCP_SERVER_UTILITY_HPP__

#include <vector>
#include "g_thread.h"
#include "gos_utility/detail/macro_switch.hpp"
#include "gos_utility/detail/smart_ptr/shared_ptr.hpp"
#include "gos_utility/detail/noncopyable.hpp"

namespace gos
{
typedef std::vector<unsigned char> BufferType;      ///< 缓冲区类型
typedef gos::shared_ptr<BufferType> CallbackParam;  ///< 回调函数入参
typedef void (*FUNC_CALLBACK)(CallbackParam);       ///< 回调函数

class tcp_server : gos::noncopyable, public GThread
{
public:
    tcp_server(const uint16_t &usTCPPort, FUNC_CALLBACK f, const TIMEVAL &stTimeout)
        : GThread(nullptr), m_usPort(usTCPPort), m_callback(f), m_stTimeout(stTimeout), m_stSocket(INVALID_SOCKET)
    {
    }

    ~tcp_server()
    {
        if (m_stSocket != INVALID_SOCKET)
        {
            CLOSE_SOCKET(m_stSocket);
        }
    }

    virtual GOS_THREAD_RET ThreadEntry(void *pPara) override final
    {
        if(IsThreadRunning())
        {
            m_stSocket = CreateSocket(m_usPort);
            if (m_stSocket == INVALID_SOCKET)
            {
                /// GosLog(LOG_ERROR, " CreateSocket failed! ");
                this->Stop();
            }

            while (IsThreadRunning())
            {
                ListenMsg(m_stSocket, m_stTimeout);
            }
        }
#ifdef _OSWIN32_
        return 1;
#else
        return NULL;
#endif
    }

    SOCKET CreateSocket(const uint16_t usTCPPort)
    {
        gos_init_socket();

        SOCKET stSocket = INVALID_SOCKET;
        stSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (stSocket == INVALID_SOCKET)
        {
            GosLog(LOG_ERROR, "Socket Error: %s", gos_get_socket_err());
            return INVALID_SOCKET;
        }

        uint32_t ulFlag = 1;
        if (setsockopt(stSocket, SOL_SOCKET, SO_REUSEADDR, (CHAR *)&ulFlag, sizeof(ulFlag)) == SOCKET_ERROR)
        {
            GosLog(LOG_ERROR, "Socket Error: %s", gos_get_socket_err());
            CLOSE_SOCKET(stSocket);

            return INVALID_SOCKET;
        }

        SOCKADDR_IN stLocalAddr = {0};
        stLocalAddr.sin_family = PF_INET;
        stLocalAddr.sin_port = htons(usTCPPort);
        memset(&stLocalAddr.sin_addr.s_addr, 0, 4);
        if (bind(stSocket, (SOCKADDR *)&stLocalAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
        {
            GosLog(LOG_ERROR, "ATSThread server_create_socket: bind socket failed, %s %s", gos_get_socket_err(),
                   LINE_INFO);
            CLOSE_SOCKET(stSocket);

            return INVALID_SOCKET;
        }

        // mpk
        if (listen(stSocket, 16) == SOCKET_ERROR)
        {
            GosLog(LOG_ERROR, "Socket Error: %s", gos_get_socket_err());
            CLOSE_SOCKET(stSocket);

            return INVALID_SOCKET;
        }

        return stSocket;
    }

    INT32 RecvMsg(SOCKET stSocket, unsigned char *pucBuffer, uint32_t ulBufferLen)
    {
        uint32_t ulByteToRecved = 0;

        /* 看看有多少字节可读 */
        /// FIONREAD 表示读取接收缓冲区的字节数
        if (ioctlsocket(stSocket, FIONREAD, (unsigned long *)&ulByteToRecved) == SOCKET_ERROR)
        {
            return -1;
        }

        /* 如果没有字节可读, 则对端已经关闭连接, 返回错误 */
        /* ulByteToRecved保存缓冲区字节数 */
        if (ulByteToRecved == 0)
        {
            return 0;
        }

        /* 处理上次RecvMsg的剩余数据 */
        return recv(stSocket, (CHAR *)pucBuffer, ulBufferLen, 0);
    }

    INT32 ListenMsg(SOCKET stSocket, TIMEVAL &stTimeout)
    {
        FDSET fdsAll;
        FD_ZERO(&fdsAll);
        FD_SET(stSocket, &fdsAll);

        while (!IsThreadWaitStop())
        {
            FDSET fds = fdsAll;
            INT32 iRet = select(FD_SETSIZE, &fds, NULL, NULL, &stTimeout);
            SOCKET stClientSocket = INVALID_SOCKET;
            if (iRet == 0 || (iRet < 0 && errno == EINTR))
            {
                continue;
            }

            if (FD_ISSET(stSocket, &fds))
            {
                while (true)
                {
                    SOCKADDR stAddr;
                    int32_t iAddrLen = sizeof(SOCKADDR);
                    stClientSocket = accept(stSocket, &stAddr, (socklen_t *)&iAddrLen);
                    if (stClientSocket == static_cast<SOCKET>(SOCKET_ERROR))
                    {
                        if (errno == EINTR)
                        {
                            continue;
                        }
                    }
                    break;
                }

                if (stClientSocket == static_cast<SOCKET>(SOCKET_ERROR))
                {
                    continue;
                }

                INT32 iNoDelay = 1;
                /* 设置NoDelay方式 */
                setsockopt(stClientSocket, IPPROTO_TCP, TCP_NODELAY, (CHAR *)&iNoDelay, sizeof(iNoDelay));
                FD_SET(stClientSocket, &fdsAll);
            }

            /* 处理 tcp 端口上的消息 */
            uint32_t ulFdCount = 0;
            uint32_t ulTotalFdCount = get_fd_num(&fds);
            for (uint32_t i = 0; i < FD_SETSIZE; i++)
            {
                if (ulFdCount >= ulTotalFdCount)
                {
                    break;
                }

                stClientSocket = dtp_get_socket_by_fds(&fds, i);
                if (stClientSocket == INVALID_SOCKET)
                {
                    continue;
                }

                if (stClientSocket == stSocket)
                {
                    continue;
                }

                ulFdCount++;

                /// 判断这个 socket 是否是可读的
                if (!FD_ISSET(stClientSocket, &fds))
                {
                    continue;
                }

                /* 有消息到, 进行处理 */
                INT32 iRecvLen = RecvMsg(stClientSocket, m_aucRecvBuf, sizeof(m_aucRecvBuf));
                if (iRecvLen <= 0)
                {
                    /// 0: 对端断开连接, < 0: 错误码
                    FD_CLR(stClientSocket, &fdsAll);
                    CLOSE_SOCKET(stClientSocket);
                }
                else
                {
                    /// 回调函数
                    CallbackParam sp(new BufferType(m_aucRecvBuf, m_aucRecvBuf + iRecvLen));
                    m_callback(sp);
                }
            }
        }
        return 0;
    }

private:
    SOCKET m_stSocket;
    uint16_t m_usPort;
    unsigned char m_aucRecvBuf[60 * 1024];
    FUNC_CALLBACK m_callback;
    TIMEVAL m_stTimeout;

    UINT32 get_fd_num(FDSET* fds)
    {
    #ifdef _OSWIN32_
        return fds->fd_count;

    #else
        UINT32  i;
        UINT32  ulCount = 0;

        for (i = 0; i < FD_SETSIZE; i++)
        {
            if (FD_ISSET(i, fds))
            {
                ulCount++;
            }
        }

        return ulCount;
    #endif
    }

    SOCKET dtp_get_socket_by_fds(FDSET *fds, UINT32 ulIndex)
    {
    #ifdef _OSWIN32_
        if (ulIndex >= fds->fd_count)
        {
            return INVALID_SOCKET;
        }

        return fds->fd_array[ulIndex];
    #else
        if (FD_ISSET(ulIndex, fds))
        {
            return ulIndex;
        }

        return INVALID_SOCKET;
    #endif
    }
};
}  // namespace gos

#endif  ///< DC_GOS_PUBLIC_GOS_UTILITY_DETAIL_NETWORK_TCP_SERVER_UTILITY_HPP__