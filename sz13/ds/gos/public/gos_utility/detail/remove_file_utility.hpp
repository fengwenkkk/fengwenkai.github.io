/**
 * @file            remove_file_utility.hpp
 * @brief           删除文件工具的实现
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-12-28 09:04:38
 * @copyright       Copyright (c) 2021 GBCOM
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_HPP__
#define DS_GOS_PUBLIC_GOS_UTILITY_HPP__

#include <string.h>
#include <stdlib.h>

namespace gos
{

inline int remove_file_by_prefix_compare_file(const void *t1, const void *t2)
{
    CHAR **s = (CHAR **)t1;
    CHAR **d = (CHAR **)t2;

    return strcmp(*s, *d);
}

/**
 * @brief           删除某个文件夹下所有具有名称前缀的文件
 * @param           szDir           [in] 文件夹路径
 * @param           szFilePrefix    [in] 文件名称前缀
 * @param           ulMaxFileNum    [in] 最大删除数量
 * @return          VOID
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2021-12-28 08:35:07
 * @note
 */
inline VOID remove_file_by_prefix(const CHAR *szDir, const CHAR *szFilePrefix, const UINT32 ulMaxFileNum)
{
    BOOL bIsDir;
    CHAR acFile[260] = {0};
    CHAR acFilePrefix[260] = {0};
    HANDLE hDir = gos_open_dir(szDir);

    if (!hDir)
    {
        return;
    }

    sprintf(acFilePrefix, "%s/%s", szDir, szFilePrefix);
    gos_format_path(acFilePrefix);

    std::vector<std::string> vFile;
    while (gos_get_next_file(hDir, acFile, &bIsDir))
    {
        if (!bIsDir)
        {
            gos_format_path(acFile);
            if (gos_str_like(acFile, acFilePrefix))
            {
                vFile.push_back(acFile);
            }
        }
    }

    gos_close_dir(hDir);

    if (vFile.size() <= ulMaxFileNum)
    {
        return;
    }

    std::sort(vFile.begin(), vFile.end());

    for (UINT32 i = 0; i < vFile.size(); i++)
    {
        if (i < (vFile.size() - ulMaxFileNum))
        {
            gos_delete_file(vFile.at(i).c_str());
        }
    }

    return;
}
}  // namespace gos

#endif  ///< DS_GOS_PUBLIC_GOS_UTILITY_HPP__