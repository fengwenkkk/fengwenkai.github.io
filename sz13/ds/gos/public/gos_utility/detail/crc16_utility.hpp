/**
 * @file            crc16_utility.hpp
 * @brief           CRC16 类的实现
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2022-01-05 10:04:53
 * @copyright       Copyright (c) 2022 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_CRC16_UTILITY_HPP__
#define DS_GOS_PUBLIC_GOS_UTILITY_CRC16_UTILITY_HPP__

#include "gos_typedef.h"
#include <vector>

namespace gos
{
class CRC16
{
public:
    CRC16() : m_ucCRC(0) {}

    UINT16 ProcessBuffer(const UINT8* data, UINT16 length)
    {
        while (length--)
        {
            processByte(*data++);
        }
        return m_ucCRC;
    }

    UINT16 ProcessVectorBuffer(const std::vector<UINT8>& vec)
    {
        for (UINT32 i = 0; i < vec.size(); ++i)
        {
            processByte(vec.at(i));
        }
        return m_ucCRC;
    }

private:
    UINT16 m_ucCRC;

    UINT16 processByte(UINT8 data)
    {
        m_ucCRC = m_ucCRC ^ ((UINT16)data << 8);
        for (UINT8 i = 0; i < 8; i++)
        {
            if (m_ucCRC & 0x8000)
                m_ucCRC = (m_ucCRC << 1) ^ 0x1021;
            else
                m_ucCRC <<= 1;
        }
        return m_ucCRC;
    }
};
}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_CRC16_UTILITY_HPP__