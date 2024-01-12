/**
 * @file            binary_file_tool.hpp
 * @brief           �������ļ�������
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2022-01-26 14:11:43
 * @copyright       Copyright (c) 2021-2022 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_BINARY_FILE_BINARY_FILE_TOOL_HPP__
#define DS_GOS_PUBLIC_GOS_UTILITY_BINARY_FILE_BINARY_FILE_TOOL_HPP__

#include <string>

namespace gos
{
class binary_file_tool
{
public:
    /**
     * @brief           �����·����Ϊ����·��
     * @param           strFileName    [in]
     * @return          std::string
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2022-01-25 11:12:04
     * @note
     */
    std::string ConvertFileNameToAbsolute(const std::string& strFileName)
    {
        std::string absolute_path(gos_get_root_path());
        absolute_path += "\\";

        std::string str(strFileName);
        gos::string_replace("./", absolute_path, str);
        gos::string_replace(".\\", absolute_path, str);
        return str;
    }

    /**
     * @brief           �����ļ���
     * @param           strFileName    [in]
     * @author          lijiancong(lijiancong@gbcom.com.cn)
     * @date            2022-01-26 14:09:21
     * @note
     */
    void CreateFolder(const std::string& strFileName)
    {
        size_t pos = gos::string_rfind(strFileName, "/");
        if (std::string::npos == pos)
        {
            pos = gos::string_rfind(strFileName, "\\");
            if (std::string::npos == pos)
            {
                /// �ļ��зָ���
                return;
            }
        }
        std::string strDir(strFileName.begin(), strFileName.begin() + pos);
        gos_create_redir(strDir.c_str());
    }
};
}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_BINARY_FILE_BINARY_FILE_TOOL_HPP__