/**
 * @file            binary_file_utility.hpp
 * @brief           �������ļ���ع���
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2022-01-20 09:20:53
 * @copyright       Copyright (c) 2022 GBCOM
 */

#ifndef DS_GOS_PUBLIC_BINARY_FILE_UTILITY_HPP__
#define DS_GOS_PUBLIC_BINARY_FILE_UTILITY_HPP__

#include <cstdio>
#include <string>
#include <vector>
#include <cstdint>

#include "gos_api.h"
#include "gos_utility/detail/string_utility.hpp"
#include "gos_utility/detail/binary_file/binary_file_tool.hpp"

namespace gos
{
/**
 * @brief           �������ļ�д�����
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-20 10:50:49
 * @note
 * �ο�����:
 * https://blog.csdn.net/qicheng777/article/details/75458025
 */
class binary_file_writer : protected binary_file_tool
{
public:
    binary_file_writer(const std::string &strFileName) : m_file(nullptr)
    {
        /// �����·����Ϊ����·��, ����֧���ļ���ǰ׺
        m_strFileName = ConvertFileNameToAbsolute(strFileName);
        /// ���������ڵ��ļ���
        CreateFolder(m_strFileName);
    }

    ~binary_file_writer()
    {
        if (m_file)
        {
            std::fclose(m_file);
        }
    }

    size_t write(unsigned char *buffer, const uint32_t &buffer_len)
    {
        if (!buffer || buffer_len == 0)
        {
            return 0;
        }

        if (!m_file)
        {
            m_file = fopen(m_strFileName.c_str(), "w+b");
            if (!m_file)
            {
                return 0;
            }
        }

        return std::fwrite(static_cast<void *>(buffer), sizeof(*buffer), buffer_len, m_file);  ///< д������
    }

    void save()
    {
        if (m_file)
        {
            std::fflush(m_file);
        }
    }

private:
    std::string m_strFileName;
    std::FILE *m_file;
};

/**
 * @brief           �������ļ���ȡ����
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-20 10:55:50
 * @note
 * https://blog.csdn.net/zxx910509/article/details/56682312
 */
class binary_file_reader : protected binary_file_tool
{
public:
    binary_file_reader(const std::string &strFileName) : m_file(NULL)
    {
        /// �����·����Ϊ����·��, ����֧���ļ���ǰ׺
        m_strFileName = ConvertFileNameToAbsolute(strFileName);
    }

    ~binary_file_reader()
    {
        if (m_file)
        {
            std::fclose(m_file);
        }
    }

    size_t read(unsigned char *buffer, const uint32_t &buffer_len)
    {
        if (!buffer || buffer_len == 0)
        {
            return 0;
        }

        if (!m_file)
        {
            m_file = fopen(m_strFileName.c_str(), "r+b");
            if (!m_file)
            {
                return 0;
            }
        }

        /// ��ȡ�ļ���С
        std::fseek(m_file, 0, SEEK_END);
        long file_content_size = std::ftell(m_file);
        std::fseek(m_file, 0, SEEK_SET);

        if(file_content_size <= 0)
        {
            return 0;
        }

        /// �ж��ļ��ܴ�С�뻺�����Ĵ�С
        if (buffer_len < static_cast<uint32_t>(file_content_size))
        {
            return 0;
        }
        return std::fread(buffer, sizeof(unsigned char), file_content_size, m_file);
    }

private:
    std::string m_strFileName;
    std::FILE *m_file;
};

/**
 * @brief           ��д�������ļ��Ķ���
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-01-20 14:19:05
 * @note
 */
class binary_file
{
public:
    binary_file(const std::string &strFileName) : m_reader(strFileName), m_writer(strFileName) {}

    size_t read(unsigned char *buffer, const uint32_t &buffer_len)
    {
        m_writer.save();
        return m_reader.read(buffer, buffer_len);
    }

    size_t write(unsigned char *buffer, const uint32_t &len)
    {
        return m_writer.write(buffer, len);
    }

private:
    binary_file_reader m_reader;
    binary_file_writer m_writer;
};

}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_BINARY_FILE_UTILITY_HPP__